#include "switch.h"

typedef enum {Pressed, Unpressed} SwitchState;

#define COUNTER_INIT_VALUE 65535 / 2
#define SWITCH_DEBOUNCE_TICKS 10  //消抖时间

void setCounter(int value){
	__HAL_TIM_SET_COUNTER(&htim1, value);
}

uint32_t getCounter(){
	return __HAL_TIM_GET_COUNTER(&htim1);
}

SwitchState getSwitchState(){
	return HAL_GPIO_ReadPin(switch_GPIO_Port, switch_Pin) == GPIO_PIN_RESET ? Pressed : Unpressed;
}

uint32_t getTick(){
	return HAL_GetTick();
}

switchCallback onForwardCallback = NULL;
switchCallback onBackwardCallback = NULL;
switchCallback onPressedCallback = NULL;

void switch_SetForwardCallback(switchCallback callback){
	onForwardCallback = callback;
}

void switch_SetBackwardCallback(switchCallback callback){
	onBackwardCallback = callback;
}

void switch_SetPressedCallback(switchCallback callback){
	onPressedCallback = callback;
}

void switch_Init(){
	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
	setCounter(COUNTER_INIT_VALUE);
}

void switch_Loop(){
	uint32_t counter = getCounter();
	if(counter > COUNTER_INIT_VALUE){
		//正转
		if(onForwardCallback != NULL){
			onForwardCallback();
		}
	}else if(counter < COUNTER_INIT_VALUE){
		//反转
		if(onBackwardCallback != NULL){
			onBackwardCallback();
		}
	}
	setCounter(COUNTER_INIT_VALUE);

	SwitchState switchState = getSwitchState();
	static uint8_t callbackState = 0; //0：未调用  1：已调用
	static uint32_t pressedTime = 0;

	if(switchState == Pressed){
		if(pressedTime == 0){
			pressedTime = getTick();
			//HAL_GetTick()是无符号32位，只能用后-前避免溢出无法计算
		}else if(callbackState == 0 && getTick() - pressedTime > SWITCH_DEBOUNCE_TICKS){
			//按键消抖完成，且未执行回调函数
			if(onPressedCallback != NULL){
				onPressedCallback();
			}
			callbackState = 1;
		}
	}else{
		//按键松开
		pressedTime = 0;
		callbackState = 0;
	}
}

