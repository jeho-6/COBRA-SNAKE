#ifndef __BUZZER_H
#define __BUZZER_H

#include "main.h"
#include "tim.h"  // 包含 TIM3 的声明
#include "stm32f1xx_hal.h"

void buzzer_init(void);
void Beep_PlayTone(uint16_t freq, uint16_t duration_ms);
void Play_Startup_Melody(void);
void Play_Stop_Melody(void);
void Play_Boot_Melody(void);
void Play_Alarm_Melody(void);
void Play_Resume_Melody(void);

#endif
