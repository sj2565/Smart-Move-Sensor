/*
 * gps_task.c
 *
 *  Created on: Mar 31, 2025
 *      Author: seojoon
 */
#include "gps_task.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern UART_HandleTypeDef huart1;  // USART1 사용

// GPS 데이터 전역 변수
GPS_Data_t gps_data;

// 내부 GPS 수신 버퍼
static char gps_buffer[128];
static uint8_t gps_index = 0;
static uint8_t rx_byte;  // UART 인터럽트 수신 바이트

// 내부 함수 프로토타입
static float ConvertToDecimal(char* nmea);
static void Parse_GNGGA(char* sentence, GPS_Data_t* data);

// UART 인터럽트 콜백
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        if (rx_byte == '\n')
        {
            gps_buffer[gps_index] = '\0';  // 문자열 종료
            gps_index = 0;

            if (strstr(gps_buffer, "$GNGGA") != NULL || strstr(gps_buffer, "$GPGGA") != NULL)
            {
                printf("GPS RAW: %s\r\n", gps_buffer);
                strcpy(gps_data.raw, gps_buffer);
                Parse_GNGGA(gps_buffer, &gps_data);
            }
        }
        else
        {
            if (gps_index < sizeof(gps_buffer) - 1)
                gps_buffer[gps_index++] = rx_byte;
        }

        // 다음 바이트 인터럽트 수신
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
}

// FreeRTOS GPS Task
void StartGpsTask(void const * argument)
{
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1); // UART 인터럽트 수신 시작
    printf("GPS Task Started.\r\n");

    for (;;)
    {
        // 인터럽트 기반으로 GPS 처리되므로 여기서는 대기만
        osDelay(100);
    }
}

// NMEA 위도/경도 → 10진수 변환
static float ConvertToDecimal(char* nmea)
{
    if (nmea == NULL || strlen(nmea) < 3) return 0.0f;
    float val = atof(nmea);
    int deg = (int)(val / 100);
    float min = val - (deg * 100);
    return deg + (min / 60.0f);
}

// GNGGA 파싱
static void Parse_GNGGA(char* sentence, GPS_Data_t* data)
{
    char* token;
    int field = 0;

    token = strtok(sentence, ",");

    while (token != NULL)
    {
        field++;

        if (field == 3) // 위도
            data->latitude = ConvertToDecimal(token);
        else if (field == 5) // 경도
            data->longitude = ConvertToDecimal(token);
        else if (field == 7) // Fix 여부
            data->fix = atoi(token);

        token = strtok(NULL, ",");
    }

    printf("[LAT] %.6f\r\n", data->latitude);
    printf("[LON] %.6f\r\n", data->longitude);
    printf("[FIX] %d\r\n", data->fix);
}
