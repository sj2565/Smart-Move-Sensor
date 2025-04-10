/*
 * dht22.c
 *
 *  Created on: Mar 23, 2025
 *      Author: seojoon
 */
#include <stdio.h>
#include "dht22.h"

#define DHT22_PORT GPIOA
#define DHT22_PIN GPIO_PIN_1

float temperature = 0.0f;
float humidity = 0.0f;

// 딜레이용 DWT
void DWT_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}
__STATIC_INLINE void DWT_Delay_us(volatile uint32_t microseconds) {
    uint32_t clk_cycle_start = DWT->CYCCNT;
    microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);
    while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
}

// 핀 제어 함수
void DHT22_Set_Pin_Output(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT22_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT22_PORT, &GPIO_InitStruct);
}

void DHT22_Set_Pin_Input(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT22_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DHT22_PORT, &GPIO_InitStruct);
}

// 센서 초기화
void DHT22_Init(void) {
    DWT_Init();
    DHT22_Set_Pin_Output();
    HAL_GPIO_WritePin(DHT22_PORT, DHT22_PIN, GPIO_PIN_SET);
    printf("DHT22 초기화 완료\r\n");
}

// 센서 데이터 읽기
uint8_t DHT22_Read(float *temperature, float *humidity) {
    uint8_t data[5] = {0};
    uint32_t timeout = 0;

    // 1. MCU -> Start signal
    DHT22_Set_Pin_Output();
    HAL_GPIO_WritePin(DHT22_PORT, DHT22_PIN, GPIO_PIN_RESET);
    HAL_Delay(1);  // 1ms
    HAL_GPIO_WritePin(DHT22_PORT, DHT22_PIN, GPIO_PIN_SET);
    DWT_Delay_us(30);

    // 2. DHT22 -> 응답
    DHT22_Set_Pin_Input();
    timeout = 0;
    while (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN) == GPIO_PIN_SET) {
        if (++timeout > 10000) return 1;
    }
    timeout = 0;
    while (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN) == GPIO_PIN_RESET) {
        if (++timeout > 10000) return 2;
    }
    timeout = 0;
    while (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN) == GPIO_PIN_SET) {
        if (++timeout > 10000) return 3;
    }

    // 3. 40bit 데이터 수신
    for (int i = 0; i < 40; i++) {
        // LOW 기다림
        while (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN) == GPIO_PIN_RESET);

        DWT_Delay_us(40);  // 40us 후 HIGH 유지 여부로 판단
        if (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN) == GPIO_PIN_SET) {
            data[i/8] |= (1 << (7 - (i%8)));
        }

        // HIGH 떨어질 때까지 대기
        timeout = 0;
        while (HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN) == GPIO_PIN_SET) {
            if (++timeout > 10000) break;
        }
    }

    // DHT22에서 수신한 5바이트 확인 디버깅 (data[0], [1] : 습도, data[2], [3] : 온도, data[4] : 체크섬
    //printf("D0=%d D1=%d D2=%d D3=%d D4=%d\r\n", data[0], data[1], data[2], data[3], data[4]);

    // 4. 체크섬 확인
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4]) return 4;
    //printf("CheckSum = %d, Calc = %d\r\n", data[4], checksum);;

    // 5. 온습도 계산
    *temperature = ((data[2] & 0x7F) << 8 | data[3]) * 0.1f;
    *humidity = ((data[0] << 8) | data[1]) * 0.1f;
    if (data[2] & 0x80) *temperature *= -1;

    return 0; // 성공
}
