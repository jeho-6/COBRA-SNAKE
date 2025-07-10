//
// Created by jeho6 on July 04 2024
//

/**
  ******************************************************************************
  * @file    bus_servo.c
  * @brief   This file provides code for the basic control
  *          of three uart bus servos
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

#include "bus_servo.h"

#define RX_MAX_BUF 8

uint8_t Rx_Data[8] = {0};
uint8_t Rx_index = 0;
uint8_t Rx_Flag = 0;
uint8_t RecvFlag = 0;


void bus_servo_control(uint8_t id, uint16_t value, uint16_t time)
{
    if (value >= 96 && value <= 4000)
    {
        const uint8_t s_id = id & 0xff;
        const uint8_t len = 0x07;
        const uint8_t cmd = 0x03;
        const uint8_t addr = 0x2a;

        const uint8_t pos_H = (value >> 8) & 0xff;
        const uint8_t pos_L = value & 0xff;

        const uint8_t time_H = (time >> 8) & 0xff;
        const uint8_t time_L = time & 0xff;
        
        const uint8_t checknum = (~(s_id + len + cmd + addr + pos_H + pos_L + time_H + time_L)) & 0xff;
        // unsigned char data[] = {0xff, 0xff, s_id, len, cmd, addr, pos_H, pos_L, time_H, time_L, checknum};
        unsigned char data[11] = {0};
        data[0] = 0xff;
        data[1] = 0xff;
        data[2] = s_id;
        data[3] = len;
        data[4] = cmd;
        data[5] = addr;
        data[6] = pos_H;
        data[7] = pos_L;
        data[8] = time_H;
        data[9] = time_L;
        data[10] = checknum;

        Usart_SendStr_length(data, 11);
    }
}

void bus_servo_set_id(uint8_t id)
{
    if ((id >= 1) && (id <= 250))
    {
        const uint8_t s_id = 0xfe; 
        const uint8_t len = 0x04;
        const uint8_t cmd = 0x03;
        const uint8_t addr = 0x05;
        const uint8_t set_id = id; 

        const uint8_t checknum = (~(s_id + len + cmd + addr + set_id)) & 0xff;
        // const uint8_t data[] = {0xff, 0xff, s_id, len, cmd, addr, set_id, checknum};
        unsigned char data[8] = {0};
        data[0] = 0xff;
        data[1] = 0xff;
        data[2] = s_id;
        data[3] = len;
        data[4] = cmd;
        data[5] = addr;
        data[6] = set_id;
        data[7] = checknum;
        Usart_SendStr_length(data, 8);
    }
}

void bus_servo_read(uint8_t id)
{
    if (id > 0 && id <= 250)
    {
        const uint8_t s_id = id & 0xff;
        const uint8_t len = 0x04;
        const uint8_t cmd = 0x02;
        const uint8_t param_H = 0x38;
        const uint8_t param_L = 0x02;

        const uint8_t checknum = (~(s_id + len + cmd + param_H + param_L)) & 0xff;
        // const uint8_t data[] = {0xff, 0xff, s_id, len, cmd, param_H, param_L, checknum};

        unsigned char data[8] = {0};
        data[0] = 0xff;
        data[1] = 0xff;
        data[2] = s_id;
        data[3] = len;
        data[4] = cmd;
        data[5] = param_H;
        data[6] = param_L;
        data[7] = checknum;
        Usart_SendStr_length(data, 8);
    }
}

uint16_t bus_servo_get_value(void)
{
    uint8_t checknum = (~(Rx_Data[2] + Rx_Data[3] + Rx_Data[4] + Rx_Data[5] + Rx_Data[6])) & 0xff;
    if(checknum == Rx_Data[7])
    {
        // uint8_t s_id = Rx_Data[2];
        uint16_t value_H = 0;
        uint16_t value_L = 0;
        uint16_t value = 0;

        value_H = Rx_Data[5];
        value_L = Rx_Data[6];
        value = (value_H << 8) + value_L;
        return value;
    }
    return 0;
}

uint8_t get_Rx_state(void)
{
    return RecvFlag;
}

void bus_servo_uart_recv(uint8_t Rx_Temp)
{
    switch(Rx_Flag)
    {
        case 0:
            if(Rx_Temp == 0xff)
            {
                Rx_Data[0] = 0xff;
                Rx_Flag = 1;
            }
            else if (Rx_Temp == 0xf5)
            {
                Rx_Data[0] = 0xff;
                Rx_Data[1] = 0xf5;
                Rx_Flag = 2;
                Rx_index = 2;
            }
            break;

        case 1:
            if(Rx_Temp == 0xf5)
            {
                Rx_Data[1] = 0xf5;
                Rx_Flag = 2;
                Rx_index = 2;
            } else
            {
                Rx_Flag = 0;
                Rx_Data[0] = 0x0;
            }
            break;

        case 2:
            Rx_Data[Rx_index] = Rx_Temp;
            Rx_index++;

            if(Rx_index >= RX_MAX_BUF)
            {
                Rx_Flag = 0;
                RecvFlag = 1;
            }
            break;

        default:
            break;
    }
}
