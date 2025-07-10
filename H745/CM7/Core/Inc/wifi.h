/**
 ****************************************************************************************************
 * @file        demo.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MW8266Dģ��TCP͸��ʵ��
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ������ H743������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __WIFI_H
#define __WIFI_H

#define RUNNING_INIT 0
#define RUNNING_VALUE 1
#define RUNNING_SLIDER 2
#define RUNNING_ACTION 3

#define SNAKE_INIT 0
#define SNAKE_LINE 1
#define SNAKE_CAR 2
#define SNAKE_SCAN 3

#define SERVO_ZERO 0x96
#define SERVO_MAX 0xFA
#define SERVO_MIN 0x32

#include <stdint.h>
#include "main.h"

extern char g_received_data[256];
extern uint8_t g_data_ready_flag;  /* 数据就绪标志，用于触发立即处理 */
extern char cmd[100];
extern int new_cmd;
extern volatile uint8_t emergency_stop_flag;
extern volatile uint8_t system_running;
extern void emergency_stop_action(void);
extern void check_emergency_stop(const char* data);
extern int RUNNING_STATUS;  /* 系统运行状态标志 */
extern int SNAKE_STATUS;
extern uint8_t dir_id1, dir_id4, dir_id5, dir_id8;
extern int speed_id1, speed_id4, speed_id5, speed_id8;
extern uint8_t servo1_id1, servo1_id8;  // ID1和ID8的servo1控制变量
extern int pre_dir_id1, pre_dir_id4, pre_dir_id5, pre_dir_id8;  // 预存电机方向
extern int pre_speed_id1, pre_speed_id4, pre_speed_id5, pre_speed_id8;  // 预存电机速度 
extern int LINE_LR_FLAG;  // 线条滑块左右标志
extern int wifi_disconnected_flag;  // WiFi断联标志位，1表示断联，0表示连接正常
extern uint32_t last_data_time;     // 上次接收到数据的时间戳
extern uint32_t wifi_timeout_ms;    // WiFi超时时间，默认5秒
extern uint32_t servo_return_timer; // 舵机回零计时器
extern uint8_t servo_return_flag;   // 舵机回零标志位，1表示需要回零
extern int lr_speed;  // 左右速度
extern int fb_speed;  // 前后速度
extern int rotation;  // 转速

void wifi_run(void);
uint8_t wifi_init(void);
void wifi_handle_received_data(void);
void wifi_check_connection_status(void);  // WiFi连接状态检测函数
void check_emergency_stop(const char* data);
void process_car_slider_command(const char* cmd);
void process_line_slider_command(const char* cmd);
void process_motor_combination(uint8_t* dir, int* speed, uint8_t new_dir, int new_speed);
void handle_speed_overflow(uint8_t* dir_id1, int* speed_id1, uint8_t* dir_id4, int* speed_id4, 
                          uint8_t* dir_id5, int* speed_id5, uint8_t* dir_id8, int* speed_id8);


#endif
