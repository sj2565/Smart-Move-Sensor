/*
 * mpu6050.c
 *
 *  Created on: Mar 28, 2025
 *      Author: seojoon
 */

#include "mpu6050.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define FILTER_SIZE 5 // 필터 크기 (최근 N개의 값 평균)

// 가속도 및 자이로 정규화 계수 (스케일 설정에 따라 변경 가능)
#define ACCEL_SCALE_4G 16384.0f// ±4g → 1g당 8192, 2g이니 16384
#define GYRO_SCALE_500DPS 65.5f // ±500 dps → 1dps당 65.5

volatile uint8_t mpu6050_interrupt_flag = 0;

// 이동 평균 버퍼 (static → 전역 상태 유지)
static int16_t ax_buf[FILTER_SIZE] = {0}, ay_buf[FILTER_SIZE] = {0}, az_buf[FILTER_SIZE] = {0};
static int16_t gx_buf[FILTER_SIZE] = {0}, gy_buf[FILTER_SIZE] = {0}, gz_buf[FILTER_SIZE] = {0};
static uint8_t buf_index = 0;

// 센서 초기화 함수
void MPU6050_Init(I2C_HandleTypeDef* hi2c)
{
    uint8_t check, data;

    HAL_I2C_Mem_Read(hi2c, MPU6050_ADDR, 0x75, 1, &check, 1, HAL_MAX_DELAY);
    if (check != 0x68) {
        printf("MPU6050 연결 실패: ID=0x%X\r\n", check);
        return;
    }

    // 슬립모드 해제 (PWR_MGMT_1 레지스터 0x00으로 설정)
    data = 0x00;
    HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x6B, 1, &data, 1, HAL_MAX_DELAY);

    // 가속도 센서 범위 ±4g (0x08), LSB 값 : 1g = 8192, ±2g : 0x00, ±8g : 0x10, ±16g : 0x18
    data = 0x08;
    HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x1C, 1, &data, 1, HAL_MAX_DELAY);

    // 자이로 센서 범위 ±500dps (0x08), LSB 값 : 65.5 = 1dps
    data = 0x08;
    HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x1B, 1, &data, 1, HAL_MAX_DELAY);

    // 밑에 하드웨어 인터럽트 활성화 (충격 감지)
    MPU6050_Set_Interrupt(hi2c);

    printf("MPU6050 초기화 완료\r\n");
}

// 하드웨어 인터럽트
void MPU6050_Set_Interrupt(I2C_HandleTypeDef* hi2c)
{
    uint8_t data;

    // 인터럽트 핀 설정: 데이터 레디 시 INT 발생
    data = 0x01; // 2g
    HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x38, 1, &data, 1, HAL_MAX_DELAY);

    // 1. 모션 감지 임계값 (낮을수록 민감) -> MOT_THR
    data = 10; // 약 0.625g
    HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x1F, 1, &data, 1, HAL_MAX_DELAY);

    // 2. 모션 지속시간 (ms 단위) → MOT_DUR
    data = 1;
    HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x20, 1, &data, 1, HAL_MAX_DELAY);

    // 3. 저역통과필터 설정
    data = 0x03; // 44Hz
    HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x1D, 1, &data, 1, HAL_MAX_DELAY);

    // 4. 모션 인터럽트 활성화
    data = 0x40; // Motion interrupt enable
    HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x38, 1, &data, 1, HAL_MAX_DELAY);

    // (선택) INT 핀 설정: 활성화 조건, 레벨 등 → INT_PIN_CFG(0x37)
    //data = 0x10; // Active high, push-pull
    //HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x37, 1, &data, 1, HAL_MAX_DELAY);
}

// 폴링 전용 센서 읽기 함수 (원본, 머신러닝 학습용)
void MPU6050_ReadAll(I2C_HandleTypeDef* hi2c, MPU6050_t* data)
{
    uint8_t buffer[14];

    // 0x3B부터 14바이트 연속 읽기
    HAL_I2C_Mem_Read(hi2c, MPU6050_ADDR, MPU6050_REG_ACCEL_XOUT_H, 1, buffer, 14, HAL_MAX_DELAY);

    // 가속도 raw 데이터 추출
    int16_t raw_ax = (int16_t)(buffer[0] << 8 | buffer[1]);
    int16_t raw_ay = (int16_t)(buffer[2] << 8 | buffer[3]);
    int16_t raw_az = (int16_t)(buffer[4] << 8 | buffer[5]);

    // 자이로 raw 데이터 추출
    int16_t raw_gx = (int16_t)(buffer[8] << 8 | buffer[9]);
    int16_t raw_gy = (int16_t)(buffer[10] << 8 | buffer[11]);
    int16_t raw_gz = (int16_t)(buffer[12] << 8 | buffer[13]);

    // 이동 평균 버퍼에 추가
    ax_buf[buf_index] = raw_ax;
    ay_buf[buf_index] = raw_ay;
    az_buf[buf_index] = raw_az;

    gx_buf[buf_index] = raw_gx;
    gy_buf[buf_index] = raw_gy;
    gz_buf[buf_index] = raw_gz;

    // 다음 위치로 인덱스 이동 (순환)
    buf_index = (buf_index + 1) % FILTER_SIZE;

    // 평균 계산
    int32_t sum_ax = 0, sum_ay = 0, sum_az = 0;
    int32_t sum_gx = 0, sum_gy = 0, sum_gz = 0;

    for (int i = 0; i < FILTER_SIZE; i++) {
    	sum_ax += ax_buf[i];
        sum_ay += ay_buf[i];
        sum_az += az_buf[i];

        sum_gx += gx_buf[i];
        sum_gy += gy_buf[i];
        sum_gz += gz_buf[i];
    }

    // 평균 계산 후 float 단위 변환
    float avg_ax = (float)sum_ax / FILTER_SIZE;
    float avg_ay = (float)sum_ay / FILTER_SIZE;
    float avg_az = (float)sum_az / FILTER_SIZE;

    float avg_gx = (float)sum_gx / FILTER_SIZE;
    float avg_gy = (float)sum_gy / FILTER_SIZE;
    float avg_gz = (float)sum_gz / FILTER_SIZE;

    // 실제 단위로 변환
    data->aX = avg_ax / ACCEL_SCALE_4G;
    data->aY = avg_ay / ACCEL_SCALE_4G;
    data->aZ = avg_az / ACCEL_SCALE_4G;

    data->gX = avg_gx / GYRO_SCALE_500DPS;
    data->gY = avg_gy / GYRO_SCALE_500DPS;
    data->gZ = avg_gz / GYRO_SCALE_500DPS;

    // 확인용
    //printf("aX(raw): %d, aX(g): %.4f\r\n", (int)avg_ax, avg_ax / ACCEL_SCALE_4G);
}

// 인터럽트 이벤트 발생 함수
void MPU6050_ProcessEvent(I2C_HandleTypeDef* hi2c, MPU6050_t* data) // hi2c 인자 추가
{
	// 1) INT_STATUS 확인, 임계값 기반 신호 이벤트 검출
	uint8_t status;
	HAL_I2C_Mem_Read(hi2c, MPU6050_ADDR, 0x3A, 1, &status, 1, HAL_MAX_DELAY);

	// 충격인지 확인
	if (status & 0x40) // 6번 비트 0x40 = 64 = 0b1000000
	{
		printf("EVENT : 충격 감지!\r\n");
	}
}

