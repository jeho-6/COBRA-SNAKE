//
// Created by jeho6 on June 23 2024
//

/**
  ******************************************************************************
  * @file    servo.c
  * @brief   This file provides code for the basic control
  *          of one stepmotor
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

#include "step.h"

void step4(uint8_t step){
  switch(step){
  case 0:
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);
    break;
    
  case 2:
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);
    break;
    
  case 3:
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);
    break;
    
  case 4:
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);
    break;
    
  default:break;
  }
}

void step8(uint8_t step){
  switch(step){
  case 0:
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);
    break;
    
  case 1:
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);
    break;
    
  case 2:
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);
    break;
    
  case 3:
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);
    break;
    
  case 4:
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);
    break;
    
  case 5:
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);
    break;
    
  case 6:
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);
    break;
    
  case 7:
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);
    break;
    
  default:break;
  }
}

void step_Init(){
  
}               

void stepByangle_R(uint16_t angle){
  uint16_t step = angle*TOTALSTEP/360;
  uint16_t i;
  
  for(i = 0;i<step;i++){
    step4(i%4);
    HAL_Delay(1);
  }
}

void stepByangle_L(uint16_t angle){
  uint16_t step = angle*TOTALSTEP/360;
  uint16_t i;
  
  for(i = 0;i<step;i++){
    step4(3-i%4);
    HAL_Delay(1);
  }
}
