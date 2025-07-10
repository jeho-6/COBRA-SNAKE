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

#ifndef __STEP_H__
#define __STEP_H__

#ifdef __cplusplus
extern "C" {
#endif
  
/* Includes ------------------------------------------------------------------*/
  
#include "main.h"
#include "tim.h"
#include "gpio.h"

#define TOTALSTEP 400
void step_Init();

#ifdef __cplusplus
}
#endif

void step4(uint8_t step);

void step8(uint8_t step);

void stepByangle_R(uint16_t angle);

void stepByangle_L(uint16_t angle);

#endif /*__ STEP_H__ */
