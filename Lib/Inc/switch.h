/*
 * switch.h
 *
 *  Created on: 2026年11月11日
 *      Author: HUAWEI
 */

#ifndef INC_SWITCH_H_
#define INC_SWITCH_H_

#include "tim.h"

typedef void (*switchCallback)(void);

void switch_Init();

void switch_Loop();

void switch_SetForwardCallback(switchCallback callback);

void switch_SetBackwardCallback(switchCallback callback);

void switch_SetPressedCallback(switchCallback callback);

#endif /* INC_SWITCH_H_ */
