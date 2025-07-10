//
// Created by jeho6 on June 23 2024
//

/**
  ******************************************************************************
  * @file    servo.c
  * @brief   This file provides code for the basic control
  *          of all servos
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
#include "servo.h"

Servo SERVO_1(&htim1, TIM_CHANNEL_1);
Servo SERVO_2(&htim1, TIM_CHANNEL_2);


Servo::Servo(TIM_HandleTypeDef* tim, uint32_t channel)
    : _tim(tim), _channel(channel) {
    HAL_TIM_PWM_Start(_tim, _channel);
}

void Servo::setAngle(uint32_t angle) {
    __HAL_TIM_SetCompare(_tim, _channel, angle);
}

void Servo::DeInit() {
    // 停止PWM输出
    HAL_TIM_PWM_Stop(_tim, _channel);
    
}

void Servo::Init() {
    // 启动PWM输出
    HAL_TIM_PWM_Start(_tim, _channel);
}

void servo_DeInit() {
    SERVO_1.DeInit();
    SERVO_2.DeInit();
}

void servo_Init() {
    SERVO_1.Init();
    SERVO_2.Init();
    
    // 设置初始角度
    SERVO_1.setAngle(ZERO);
    SERVO_2.setAngle(ZERO);
}

void servo_resume() {
    SERVO_1.Init();
    SERVO_2.Init();
}