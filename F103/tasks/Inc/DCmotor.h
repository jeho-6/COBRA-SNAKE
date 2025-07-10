//
// Created by jeho6 on June 23 2024
//

/**
  ******************************************************************************
  * @file    DCmotor.h
  * @brief   This file contains all the function prototypes for
  *          the DCmotor.c file
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

#ifndef __DCMOTOR_H__
#define __DCMOTOR_H__

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "tim.h"
#include "gpio.h"

extern int dir;    //0: forward rotate; 1: reverse rotate 
extern int pwm;    //0-499

void DCmotor_Init();

int correct(int pwm);

void Set_motor(int dir, int pwm);
void DCmotor_DeInit();

#endif/*__ DCMOTOR_H__ */

