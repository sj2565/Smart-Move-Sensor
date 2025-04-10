/*
 * mq135.h
 *
 *  Created on: Mar 30, 2025
 *      Author: seojoon
 */

#ifndef INC_MQ135_H_
#define INC_MQ135_H_

#include "main.h"

float MQ135_ReadPPM(void); // MQ135 읽기 함수 선언

void Update_Air_LED_RGB(float ppm); // 공기질에 따른 RGB 함수 선언
void RGB_Set(uint8_t r_percent, uint8_t g_percent, uint8_t b_percent);

#endif /* INC_MQ135_H_ */
