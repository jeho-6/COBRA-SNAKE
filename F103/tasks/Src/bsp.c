//
// Created by jeho6 on July 04 2024
//

/**
  ******************************************************************************
  * @file    bsp.c
  * @brief   This file provides code for 
  *          extra delay functions
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
#include "bsp.h"

static u8 fac_us = 0;  
static u16 fac_ms = 0; 

void Delay_init(void)
{
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
  fac_us = SystemCoreClock / 8000000;                   
  fac_ms = (u16)fac_us * 1000;                          
}

void Delay_us(u32 nus)
{
  u32 temp;
  SysTick->LOAD = nus * fac_us;             
  SysTick->VAL = 0x00;                      
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; 
  do
  {
    temp = SysTick->CTRL;
  } while ((temp & 0x01) && !(temp & (1 << 16))); 
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;      
  SysTick->VAL = 0X00;                            
}

void Delay_ms(u16 nms)
{
  u32 temp;
  SysTick->LOAD = (u32)nms * fac_ms;        
  SysTick->VAL = 0x00;                      
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; 
  do
  {
    temp = SysTick->CTRL;
  } while ((temp & 0x01) && !(temp & (1 << 16))); 
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;      
  SysTick->VAL = 0X00;                            
}
