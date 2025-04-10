/*
 * sensor_task.c
 *
 *  Created on: Mar 28, 2025
 *      Author: seojoon
 */

#include "sensor_task.h" // mpu6050.h 구조체 -> sensor_task.h
#include "button_task.h"
#include "gps_task.h"
#include "sensor_data.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>
#include "tim.h"

extern I2C_HandleTypeDef hi2c1;
extern volatile uint8_t mpu6050_interrupt_flag;
extern float temperature, humidity; // 온습도 센서 변수 선언
extern volatile uint8_t sensor_enable; // 버튼 HIGH/LOW 제어 변수
extern QueueHandle_t sensorDataQueue;  // sensorTask -> commTask 큐 선언

static uint8_t first_boot = 1;

MPU6050_t mpu_data; // mpu6050.h
uint32_t lastPrintTick; // 출력 타이밍 기준점

void StartSensorTask(void const * argument)
{
	DHT22_Init();

    MPU6050_Init(&hi2c1);
    MPU6050_Set_Interrupt(&hi2c1);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // R (PA5)
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // G (PA6)
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2); // B (PA7)

    printf("센서 데이터 출력 시작! \r\n");

    for (;;) {
        // 충격 발생 시 즉시 이벤트 출력 (인터럽트)
        if (mpu6050_interrupt_flag) { // 인터럽트가 발생했을 때만 실행
            mpu6050_interrupt_flag = 0; // 플래그 초기화

            // 리셋버튼 눌렀을 때 가짜 충격 감지 제거
            if (first_boot) {
            	first_boot = 0;
            } else {
            	MPU6050_ProcessEvent(&hi2c1, &mpu_data);

            	// shock 이벤트 큐 전송 추가
            	SensorData_t shock_event;
            	memset(&shock_event, 0, sizeof(SensorData_t));
            	shock_event.timestamp = xTaskGetTickCount();
            	shock_event.type = SENSOR_TYPE_MPU6050;
            	shock_event.shock = 1; // 인터럽트 감지

            	xQueueSend(sensorDataQueue, &shock_event, 0);
            }
        }

        // 주기적으로 센서 데이터 출력 (폴링)
        if ((HAL_GetTick() - lastPrintTick) >= 2000) {
        	lastPrintTick = HAL_GetTick();

        	SensorData_t data;
        	memset(&data, 0, sizeof(SensorData_t));  // 초기화
        	data.timestamp = xTaskGetTickCount();

        	// 온습도 센서 관련
        	if (DHT22_Read(&temperature, &humidity) == 0) {
        		data.type = SENSOR_TYPE_DHT22;
        	    data.data.dht.temperature = temperature;
        	    data.data.dht.humidity = humidity;
        	    xQueueSend(sensorDataQueue, &data, 0);
        	}
        	/**
        	if (DHT22_Read(&temperature, &humidity) == 0) {
        		printf("온도: %.1f°C, 습도: %.1f%%\r\n", temperature, humidity);
        	} else {
        		printf("DHT22 실패\r\n");
        	}**/

        	// 각속도, 기울기 관련
        	MPU6050_ReadAll(&hi2c1, &mpu_data);
        	data.type = SENSOR_TYPE_MPU6050;
        	data.data.mpu.ax = mpu_data.aX;
        	data.data.mpu.ay = mpu_data.aY;
        	data.data.mpu.az = mpu_data.aZ;
        	data.data.mpu.gx = mpu_data.gX;
        	data.data.mpu.gy = mpu_data.gY;
        	data.data.mpu.gz = mpu_data.gZ;
        	xQueueSend(sensorDataQueue, &data, 0);
        	/**
            MPU6050_ReadAll(&hi2c1, &mpu_data);
            printf("aX: %d, aY: %d, aZ: %d | gX: %d, gY: %d, gZ: %d\r\n",
                   mpu_data.aX, mpu_data.aY, mpu_data.aZ,
                   mpu_data.gX, mpu_data.gY, mpu_data.gZ);
                   **/

        	// 공기질 관련
        	float ppm = MQ135_ReadPPM();
        	data.type = SENSOR_TYPE_AIR_QUALITY;
        	data.data.air.quality_index = (int)ppm;  // 정수형으로 변환
        	xQueueSend(sensorDataQueue, &data, 0);
        	/**
            float ppm = MQ135_ReadPPM();
            printf("공기질(PPM): %.1f\r\n", ppm);
            **/

            // GPS위치 관련
        	if (gps_data.fix) {
        		data.type = SENSOR_TYPE_GPS;
        	    data.data.gps.latitude = gps_data.latitude;
        	    data.data.gps.longitude = gps_data.longitude;
        	    data.data.gps.fix_quality = gps_data.fix;
        	    xQueueSend(sensorDataQueue, &data, 0);
        	}
        	/**
            if (gps_data.fix) {
                printf("위치: 위도 %.6f, 경도 %.6f\n", gps_data.latitude, gps_data.longitude);
            } else {
                printf("GPS 수신 불안정 (위성 고정 안됨)\n");
            }**/

            // 스위치에 따른 RGB 출력
            if (sensor_enable) {
            	Update_Air_LED_RGB(ppm);
            }
        }
        osDelay(10);  // CPU 점유율 방지 + 이벤트 반응 빠르게 유지
    }
}

