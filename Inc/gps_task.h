/*
 * gps_task.h
 *
 *  Created on: Mar 31, 2025
 *      Author: seojoon
 */

#ifndef INC_GPS_TASK_H_
#define INC_GPS_TASK_H_

#include "main.h"

typedef struct {
    char raw[128]; // GNGGA 원문 저장 (디버그용)
    float latitude; // 위도 (소수점 변환됨)
    float longitude; // 경도
    uint8_t fix; // 위성 수신 여부 (0: 없음, 1 이상: 위치 고정 성공)
} GPS_Data_t;

extern GPS_Data_t gps_data;

void StartGpsTask(void const * argument);

#endif /* INC_GPS_TASK_H_ */
