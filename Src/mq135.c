/*
 * mq135.c
 *
 *  Created on: Mar 30, 2025
 *      Author: seojoon
 */
#include "main.h"
#include "mq135.h"
#include <stdio.h>
#include "tim.h"

extern ADC_HandleTypeDef hadc1; // ADC 핸들러 외부 참조

float MQ135_ReadPPM(void)
{
    HAL_ADC_Start(&hadc1); // ADC 시작
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY); // 변환 완료 대기
    uint32_t adc_value = HAL_ADC_GetValue(&hadc1); // ADC 값 읽기 (0 ~ 4095)
    HAL_ADC_Stop(&hadc1); // ADC 종료

    float voltage = (adc_value / 4095.0f) * 5.0f; // Vref 기준 전압 계산
    float ppm = voltage * 100.0f; // 단순 비례식 (보정 전)

    // 이동 평균 필터 적용
    static float ppm_buffer[5] = {0};
    static uint8_t index = 0;

    ppm_buffer[index++] = ppm;
    if (index >= 5) index = 0;

    float sum = 0;
    for (int i = 0; i < 5; ++i) {
    	sum += ppm_buffer[i];
    }
    return sum / 5.0f;  // 필터링된 평균값 반환
}

// 공기질에 따른 RGB변화
void RGB_Set(uint8_t r_percent, uint8_t g_percent, uint8_t b_percent)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, r_percent * 10); // PA5
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, g_percent * 10); // PA6
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, b_percent * 10); // PA7
}

void Update_Air_LED_RGB(float ppm)
{
    if (ppm <= 50) {
        RGB_Set(0, 100, 0); // 초록
    }
    else if (ppm <= 100) {
    	RGB_Set(100, 100, 0); // 노랑 (R+G)
    }
    else {
    	RGB_Set(100, 0, 0); // 빨강
    }
}
