/*
 * sensor_task.h
 *
 *  Created on: Mar 28, 2025
 *      Author: seojoon
 */

#ifndef INC_SENSOR_TASK_H_
#define INC_SENSOR_TASK_H_

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "mpu6050.h"
#include "dht22.h"
#include "mq135.h"

// Sensor Task 함수 (freertos.c에서 Task 생성 시 사용)
void StartSensorTask(void const * argument);

// 충격 감지 인터럽트 플래그 (EXTI 콜백과 공유)
extern volatile uint8_t mpu6050_interrupt_flag;

#endif /* INC_SENSOR_TASK_H_ */
