/*
 * new_rtc.h
 *
 *  Created on: 2026年11月10日
 *      Author: HUAWEI
 */

#ifndef INC_NEW_RTC_H_
#define INC_NEW_RTC_H_

#include "stm32f1xx_hal.h"
#include "rtc.h"
#include "time.h"

struct tm* NEW_RTC_GetTime();

HAL_StatusTypeDef NEW_RTC_SetTime(struct tm *time);

void NEW_RTC_Init();
#endif /* INC_NEW_RTC_H_ */
