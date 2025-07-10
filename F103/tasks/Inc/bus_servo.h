//
// Created by jeho6 on July 04 2024
//

/**
  ******************************************************************************
  * @file    bus_servo.h
  * @brief   This file contains all the function prototypes for
  *          the bus_servo.c file
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

#ifndef _BUS_SERVO_H_
#define _BUS_SERVO_H_
#include "bsp.h"

void bus_servo_control(uint8_t id, uint16_t value, uint16_t time);
void bus_servo_set_id(uint8_t id);
void bus_servo_read(uint8_t id);
uint16_t bus_servo_get_value(void);
void bus_servo_uart_recv(uint8_t Rx_Temp);
uint8_t get_Rx_state(void);

#endif /* _BUS_SERVO_H_ */
