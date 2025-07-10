#include "buzzer.h"
#include "tim.h"
#include "main.h"
#include "can.h"

void buzzer_init(void)
{
    // 初始化 TIM3 的 PWM 模式
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;  // 初始占空比为0
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3);
    
    // 启动 PWM 输出
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
}
void Beep_PlayTone(uint16_t freq, uint16_t duration_ms)
{
    uint16_t prescaler = 71;
    uint32_t period = 1000000 / freq - 1;

    __HAL_TIM_SET_PRESCALER(&htim3, prescaler);
    __HAL_TIM_SET_AUTORELOAD(&htim3, period);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, period / 2);

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    HAL_Delay(duration_ms);
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
}

void Play_Startup_Melody(void)
{
    Beep_PlayTone(423, 400);
    Beep_PlayTone(659, 400);
    Beep_PlayTone(1000, 600);

    HAL_Delay(1000);

    // Beep_PlayTone(1100, 100);
    // HAL_Delay(100);
    // Beep_PlayTone(1100, 100);
    for(int i = 0; i<BOARD; i++)
    {
        Beep_PlayTone(1100, 100);
        HAL_Delay(100);
    }
}

void Play_Resume_Melody(void)
{
    Beep_PlayTone(423, 400);
    Beep_PlayTone(659, 400);
    Beep_PlayTone(1000, 600);

    HAL_Delay(1000);

}

void Play_Stop_Melody(void)
{
    Beep_PlayTone(523, 150);
    Beep_PlayTone(440, 150);
    Beep_PlayTone(392, 150);
    Beep_PlayTone(261, 200);
}

void Play_Boot_Melody(void)
{
    
    // 第一小节：E-E-E-C-E-G-G(低音)
    Beep_PlayTone(659, 150);    // E5
    HAL_Delay(50);              // 稍微增加间隔
    Beep_PlayTone(659, 150);    // E5
    HAL_Delay(150);
    Beep_PlayTone(659, 150);    // E5
    HAL_Delay(150);
    Beep_PlayTone(523, 150);    // C5
    Beep_PlayTone(659, 150);    // E5
    HAL_Delay(150);
    Beep_PlayTone(784, 300);    // G5 (高音强调)
    HAL_Delay(300);
    Beep_PlayTone(392, 300);    // G4 (低音)
    HAL_Delay(300);
    
}


void Play_Alarm_Melody(void)
{
    Beep_PlayTone(600, 100);
    HAL_Delay(100);
    Beep_PlayTone(600, 100);
    HAL_Delay(100);
    HAL_Delay(1000);
}

