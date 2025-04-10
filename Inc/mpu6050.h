/*
 * mpu6050.h
 *
 *  Created on: Mar 28, 2025
 *      Author: seojoon
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#include "main.h"
#include <stdint.h>

#define MPU6050_ADDR 0xD0  // I2C 주소 (AD0 GND 연결 기준)
#define MPU6050_REG_ACCEL_XOUT_H 0x3B

typedef struct {
    float aX;
    float aY;
    float aZ;
    float gX;
    float gY;
    float gZ;
} MPU6050_t;

extern volatile uint8_t mpu6050_interrupt_flag;

void MPU6050_Init(I2C_HandleTypeDef* hi2c);
void MPU6050_ReadAll(I2C_HandleTypeDef* hi2c, MPU6050_t* data);
void MPU6050_Set_Interrupt(I2C_HandleTypeDef* hi2c);
void MPU6050_ProcessEvent(I2C_HandleTypeDef* hi2c, MPU6050_t* data); // I2C_HandleTypeDef* hi2c 추가

#endif /* INC_MPU6050_H_ */
