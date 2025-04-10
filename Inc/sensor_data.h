/*
 * sensor_data.h
 *
 *  Created on: Mar 31, 2025
 *      Author: seojoon
 */

#ifndef INC_SENSOR_DATA_H_
#define INC_SENSOR_DATA_H_

#include "stdint.h"
#include "FreeRTOS.h"

typedef enum {
    SENSOR_TYPE_DHT22,
    SENSOR_TYPE_MPU6050,
    SENSOR_TYPE_GPS,
    SENSOR_TYPE_AIR_QUALITY
} SensorType_t;

// Queue를 하나만 사용하기 위한 복합 구조체
typedef struct {
    SensorType_t type;
    TickType_t timestamp;

    union {
        struct {
            float temperature;
            float humidity;
        } dht;

        struct {
            float ax, ay, az;
            float gx, gy, gz;
        } mpu;

        struct {
            double latitude;
            double longitude;
            int fix_quality;
        } gps;

        struct {
            int quality_index;
        } air;
    } data;

    uint8_t shock; // 충격 감지 변수
} SensorData_t;


#endif /* INC_SENSOR_DATA_H_ */
