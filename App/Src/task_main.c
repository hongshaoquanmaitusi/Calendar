#include "task_main.h"

#define CURSON_FLASH_INTERVAL 500

char weeks[7][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

typedef enum {CalendarState_Normal, CalendarState_Setting} CalendarState;

typedef enum {Year, Month, Day, Hour, Minute, Second} SettingState;

typedef struct {uint8_t x1; uint8_t y1; uint8_t x2; uint8_t y2;} CursonPosition;

CursonPosition cursorPositions[6] = {
	{24 + 0 * 8, 17, 24 + 4 * 8, 17},  //year
	{24 + 5 * 8, 17, 24 + 7 * 8, 17},
	{24 + 8 * 8, 17, 24 + 10 * 8, 17},
	{16 + 0 * 12, 45, 16 + 2 * 12, 45},
	{16 + 3 * 12, 45, 16 + 5 * 12, 45},
	{16 + 6 * 12, 45, 16 + 8 * 12, 45}, //second
};

CalendarState calendarState = CalendarState_Normal;

SettingState settingState = Year;

struct tm settingTime;

void onswitchForward(){
	if(calendarState == CalendarState_Setting){
		switch (settingState){
		case Year:
			settingTime.tm_year++;
			break;
		case Month:
			settingTime.tm_mon++;
			if(settingTime.tm_mon > 11){
				settingTime.tm_mon = 0;
			}
			break;
		case Day:
					settingTime.tm_mday++;
					if(settingTime.tm_mday > 31){
						settingTime.tm_mday = 1;
					}
					break;
		case Hour:
			settingTime.tm_hour++;
			if (settingTime.tm_hour > 23){
				settingTime.tm_hour = 0;
			}
			break;
		case Minute:
					settingTime.tm_min++;
					if(settingTime.tm_min > 59){
						settingTime.tm_min = 0;
					}
					break;

		case Second:
					settingTime.tm_sec++;
					if(settingTime.tm_sec > 59){
						settingTime.tm_sec = 0;
					}
					break;
		}
	}
}

void onswitchBackward(){
	if(calendarState == CalendarState_Setting){
		switch (settingState) {
		case Year:
			settingTime.tm_year--;
			if(settingTime.tm_year < 70){
				settingTime.tm_year = 70;
			}
			break;
		case Month:
			settingTime.tm_mon--;
			if (settingTime.tm_mon < 0) {
				settingTime.tm_mon = 11;
			}
			break;
		case Day:
			settingTime.tm_mday--;
			if (settingTime.tm_mday < 1) {
				settingTime.tm_mday = 31;
			}
			break;
		case Hour:
			settingTime.tm_hour--;
			if (settingTime.tm_hour < 0) {
				settingTime.tm_hour = 23;
			}
			break;
		case Minute:
			settingTime.tm_min--;
			if (settingTime.tm_min < 0) {
				settingTime.tm_min = 59;
			}
			break;

		case Second:
			settingTime.tm_sec--;
			if (settingTime.tm_sec < 0) {
				settingTime.tm_sec = 59;
			}
			break;
		}
	}
}

void onswitchPressed(){
	if(calendarState == CalendarState_Normal){
		settingTime = *NEW_RTC_GetTime();
		settingState = Year;
		calendarState = CalendarState_Setting;
	}else{
		if(settingState == Second){
			NEW_RTC_SetTime(&settingTime);
			calendarState = CalendarState_Normal;
		}else{
			settingState++;
		}
	}
}

void showTime(struct tm* time){
	char str[30];
		sprintf(str, "%d-%02d-%02d", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday);
		OLED_PrintASCIIString(24, 0, str, &afont16x8, OLED_COLOR_NORMAL);

		sprintf(str, "%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec);
		OLED_PrintASCIIString(16, 20, str, &afont24x12, OLED_COLOR_NORMAL);

		char* week = weeks[time->tm_wday];
		uint8_t x_week = (128 - (strlen(week) * 8)) / 2;
		OLED_PrintASCIIString(x_week, 48, week, &afont16x8, OLED_COLOR_NORMAL);

}

void showCursor(){
	static uint32_t startTime = 0;
	uint32_t difftime = HAL_GetTick() - startTime;
	if(difftime > 2 * CURSON_FLASH_INTERVAL){
		startTime = HAL_GetTick();
	}else if(difftime > CURSON_FLASH_INTERVAL){
		CursonPosition position = cursorPositions[settingState];
		OLED_DrawLine(position.x1, position.y1, position.x2, position.y2, OLED_COLOR_NORMAL);
	}
}

void MainTaskInit(){
	HAL_Delay(20);
	OLED_Init();
	NEW_RTC_Init();
	switch_Init();
	switch_SetForwardCallback(onswitchForward);
	switch_SetBackwardCallback(onswitchBackward);
	switch_SetPressedCallback(onswitchPressed);
}

void MainTask(){
	switch_Loop();
	OLED_NewFrame();

	if(calendarState == CalendarState_Normal){
		struct tm *now = NEW_RTC_GetTime();
		showTime(now);
	}else{
		showTime(&settingTime);
		showCursor();
	}

	OLED_ShowFrame();
}
