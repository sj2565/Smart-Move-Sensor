/*
 * button_task.c
 *
 *  Created on: Mar 30, 2025
 *      Author: seojoon
 */
#include "main.h"
#include "cmsis_os.h"
#include "button_task.h"
#include "mq135.h" // RGB_Set() 사용
#include <stdio.h>

// 외부 세마포어 핸들 (CubeMX에서 생성되어 있어야 함)
extern osSemaphoreId buttonSemHandle;

// 외부에서 접근할 수 있게 전역 변수로 선언
volatile uint8_t sensor_enable = 1;

void StartButtonTask(void const * argument)
{
	// (세마포어 핸들, 타임 아웃)
	/**
	   STM32 리셋 시, EXTI 인터럽트가 가짜로 한 번 발생
       osSemaphoreRelease()가 실행돼서 세마포어가 하나 쌓임
       버튼 Task가 실행되자마자 → 바로 깨어나서 sensor_enable ^= 1; → RGB OFF 발생 **/
	osSemaphoreWait(buttonSemHandle, 0); // 리셋버튼 눌렀을 시 발동되는(RGB OFF) 세마포어 제거
	osDelay(1000);  // 시스템 안정화 대기

	for (;;)
	{
		// 버튼 눌릴 때까지 대기
		if (osSemaphoreWait(buttonSemHandle, osWaitForever) == osOK)
		{
			// 토글 방식: 눌릴 때마다 1 ↔ 0 전환
			sensor_enable ^= 1;

			if (sensor_enable == 0)
			{
				// 센서 끄기: RGB 꺼짐
				RGB_Set(0, 0, 0);
				printf("버튼 누름: RGB OFF\r\n");
			}
			else {
				printf("버튼 누름: RGB ON\r\n");
			}
		}
	}
}

