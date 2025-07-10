//
// Created by jeho6 on July 04 2024
//

/**
  ******************************************************************************
  * @file    bsp.h
  * @brief   This file contains all the function prototypes for
  *          the bsp.c file
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

#ifndef _BSP_H_
#define _BSP_H_

#define STM32F10X_MD
#include "stm32f10x.h"
#include "usart.h"
#include "stdio.h"
#include "i2c_ctl.h"
#include "bus_servo.h"
#include "timer_delay.h"



#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n) //??
#define PAin(n) BIT_ADDR(GPIOA_IDR_Addr, n)  //??

#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n) //??
#define PBin(n) BIT_ADDR(GPIOB_IDR_Addr, n)  //??

#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n) //??
#define PCin(n) BIT_ADDR(GPIOC_IDR_Addr, n)  //??

#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n) //??
#define PDin(n) BIT_ADDR(GPIOD_IDR_Addr, n)  //??

#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n) //??
#define PEin(n) BIT_ADDR(GPIOE_IDR_Addr, n)  //??

#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr, n) //??
#define PFin(n) BIT_ADDR(GPIOF_IDR_Addr, n)  //??

#define PGout(n) BIT_ADDR(GPIOG_ODR_Addr, n) //??
#define PGin(n) BIT_ADDR(GPIOG_IDR_Addr, n)  //??


void Delay_init(void);
void Delay_ms(u16 nms);
void Delay_us(u32 nus);

#endif /* _BSP_H_ */
