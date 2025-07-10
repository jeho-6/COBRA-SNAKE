//
// Created by jeho6 on June 23 2024
//

/**
  ******************************************************************************
  * @file    servo.h
  * @brief   This file contains all the function prototypes for
  *          the servo.c file
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

#ifndef __SERVO_H__
#define __SERVO_H__

#ifdef __cplusplus
extern "C" {
#endif
  
/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "stm32f1xx_hal.h"

#define SERVO_MIN    50    // -90度 (1.0ms)
#define SERVO_ZERO   150    // 0度 (1.5ms) 
#define SERVO_MAX    250    // +90度 (2.0ms)

#define ZERO 150
    
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

class Servo{
  
public:
  Servo(TIM_HandleTypeDef* tim, uint32_t channel);
  
  void setAngle(uint32_t angle);

  void DeInit();

  void Init();
  
private:
  TIM_HandleTypeDef* _tim; 
  uint32_t _channel;       
};

extern Servo SERVO_1;
extern Servo SERVO_2;

void servo_Init();
void servo_DeInit();
void servo_resume();

#endif // __cplusplus

#endif /*__ SERVO_H__ */