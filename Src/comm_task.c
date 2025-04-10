/*
 * comm_task.c
 *
 *  Created on: Mar 31, 2025
 *      Author: seojoon
 */
#include "comm_task.h"
#include "sensor_data.h"
#include "usart.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>

extern QueueHandle_t sensorDataQueue;

void StartCommTask(void const * argument)
{
    SensorData_t received;

    // 최신 센서 데이터를 저장하는 static 변수들
    static float temp = 0.0f, humid = 0.0f;
    static float ax = 0.0f, ay = 0.0f, az = 0.0f;
    static float gx = 0.0f, gy = 0.0f, gz = 0.0f;
    static int air = 0;
    static float lat = 0.0f, lon = 0.0f;
    static int fix = 0;
    static int shock = 0;

    for (;;) {

    	// 큐에서 여러 개 데이터를 빠르게 수신 처리       portMAX_DELAY : 큐에 데이터가 올 때까지 무한 대기, 0 : 즉시 수신 시도 (대기 안 함)
    	for (int i = 0; i < 10; i++) {
			if (xQueueReceive(sensorDataQueue, &received, 0) == pdTRUE) {

				// 센서별로 최신 값 저장
				switch (received.type) {
				case SENSOR_TYPE_DHT22:
					temp = received.data.dht.temperature;
					humid = received.data.dht.humidity;
					break;

				case SENSOR_TYPE_MPU6050:

					// 충격 이벤트만 들어온 경우
					if (received.shock == 1) {
						shock = 1; // 1회 감지
					} else {
						ax = received.data.mpu.ax;
						ay = received.data.mpu.ay;
						az = received.data.mpu.az;
						gx = received.data.mpu.gx;
						gy = received.data.mpu.gy;
						gz = received.data.mpu.gz;
					}
					break;

				case SENSOR_TYPE_AIR_QUALITY:
					air = received.data.air.quality_index;
					break;

				case SENSOR_TYPE_GPS:
					 lat = received.data.gps.latitude;
					 lon = received.data.gps.longitude;
					 fix = received.data.gps.fix_quality;
					break;

				default:
					break;
				}
			} else {
				break; // 큐가 비었으면 반복 중지
			}
    	}

    	char output[512];  // 출력 버퍼 넉넉하게 확보
    	memset(output, 0, sizeof(output)); // output 버퍼 초기화
    	snprintf(output, sizeof(output),
    			"{\"temp\":%.2f,\"humid\":%.2f,"
    			"\"ax\":%.5f,\"ay\":%.5f,\"az\":%.5f,"
    			"\"gx\":%.5f,\"gy\":%.5f,\"gz\":%.5f,"
    			"\"air\":%d,"
    			"\"lat\":%.6f,\"lon\":%.6f,\"fix\":%d,"
    			"\"shock\":%d}\r\n",
				temp, humid,
				ax, ay, az,
				gx, gy, gz,
				air,
				lat, lon, fix,
				shock);

    	shock = 0; // 0으로 다시 초기화

    	// 확인용
    	//printf("Test AX: %.5f\r\n", ax);

    	// print문은 UART 확인용, Trasnmit은 ESP32 수신용
    	//printf("%s", output);
    	HAL_UART_Transmit(&huart6, (uint8_t*)output, strlen(output), HAL_MAX_DELAY);

    	// 출력 간 간격 확보
    	osDelay(100);
    }
}

/** JSON이 아닌 기존 방식
void StartCommTask(void const * argument)
{
    SensorData_t received;

    // UART 통신 확인용 테스트 메시지 반복 출력
    for (int i = 0; i < 20; i++) {
    	char msg[64];
    	int val = 27;

    	snprintf(msg, sizeof(msg), "TEST FLOAT %d\r\n", val);

    	HAL_UART_Transmit(&huart6, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    	osDelay(1000);  // 1초마다 출력
    }

    for (;;) {
    	if (xQueueReceive(sensorDataQueue, &received, portMAX_DELAY) == pdTRUE) {
    		char output[512];  // 출력 버퍼 넉넉하게 확보
    		memset(output, 0, sizeof(output)); // output 버퍼 초기화

    		switch (received.type) {
    		case SENSOR_TYPE_DHT22:
    			snprintf(output, sizeof(output),
    					"[DHT22] 온도: %.1f°C, 습도: %.1f%%\r\n",
						received.data.dht.temperature,
						received.data.dht.humidity);
    			break;

            case SENSOR_TYPE_MPU6050:
            	snprintf(output, sizeof(output),
            			"[MPU6050] aX: %.2f, aY: %.2f, aZ: %.2f | "
            			"gX: %.2f, gY: %.2f, gZ: %.2f\r\n",
						received.data.mpu.ax, received.data.mpu.ay, received.data.mpu.az,
						received.data.mpu.gx, received.data.mpu.gy, received.data.mpu.gz);
            	break;

            case SENSOR_TYPE_AIR_QUALITY:
            	snprintf(output, sizeof(output),
            			"[AIR] 공기질 지수: %d\r\n",
						received.data.air.quality_index);
            	break;

            case SENSOR_TYPE_GPS:
            	snprintf(output, sizeof(output),
            			"[GPS] 위도: %.6f, 경도: %.6f, Fix: %d\r\n",
						received.data.gps.latitude,
						received.data.gps.longitude,
						received.data.gps.fix_quality);
            	break;

            default:
            	snprintf(output, sizeof(output),
            			"[CommTask] 알 수 없는 센서 타입 수신\r\n");
            	break;
    		}

    		// 한 줄로 UART 출력
    		// print문은 STM32 터미널용, Trasnmit은 ESP32 수신용
    		//printf("%s", output);
    		HAL_UART_Transmit(&huart6, (uint8_t*)output, strlen(output), HAL_MAX_DELAY);

    		// 출력 간 간격 확보
    		osDelay(100);
    	}
    }
} **/


