//
// Created by jeho6 on June 23 2024
//

/**
  ******************************************************************************
  * @file    DCmotor.c
  * @brief   This file provides code for the basic control
  *          of all DCmotors
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "DCmotor.h"


void DCmotor_Init(){
  
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
  Set_motor(0, 0);
}

void DCmotor_DeInit(){
    // 停止PWM输出
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
    
    // 将PWM占空比设为0，确保电机停止
    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_1, 0);
    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_2, 0);
    
    // 清除使能引脚，关闭电机驱动器
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
}

int correct(int pwm){
  if(pwm<0){pwm = 0;}
  
  if(pwm>499){pwm = 499;}
  
  return pwm;
}

void Set_motor(int dir, int pwm){
  
  int vel = correct(pwm);
  
  if(dir == 0){
      __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,vel);  
      __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_2,0);   
  }
  else{
      __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_2,vel);   
      __HAL_TIM_SetCompare(&htim4,TIM_CHANNEL_1,0); 
  }

}