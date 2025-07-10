/**
 ****************************************************************************************************
 * @file        wifi.c
 * @author      原子教程团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MW8266D模块TCP透传实验 - 核心板版本
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 ****************************************************************************************************
 */

#include "wifi.h"
#include "./BSP/ATK_MW8266D/atk_mw8266d.h"
#include "./SYSTEM/usart/usart.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "fdcan.h"
#include "action.h"

#define DEMO_WIFI_SSID          "sic-guest"
#define DEMO_WIFI_PWD           "sicguest"
// #define DEMO_TCP_SERVER_IP      "172.18.20.10"
#define DEMO_TCP_SERVER_IP      "172.18.29.118"

#define DEMO_TCP_SERVER_PORT    "8080"

int error_flag = 0;  /* 错误标志位 */
int ret_num = 0;

uint8_t dir_id1 = 0, dir_id4 = 0, dir_id5 = 0, dir_id8 = 0;
int speed_id1 = 0, speed_id4 = 0, speed_id5 = 0, speed_id8 = 0;
uint8_t servo1_id1 = SERVO_ZERO, servo1_id8 = SERVO_ZERO;  // ID1和ID8的servo1控制变量

char g_received_data[256] = {0};
uint8_t g_data_ready_flag = 0;  /* 数据就绪标志，用于触发立即处理 */

int RUNNING_STATUS = RUNNING_INIT;
int SNAKE_STATUS = SNAKE_INIT;

int LINE_LR_FLAG = 0;
int pre_dir_id1 = 0, pre_dir_id4 = 0, pre_dir_id5 = 0, pre_dir_id8 = 0;  // 预存电机方向
int pre_speed_id1 = 0, pre_speed_id4 = 0, pre_speed_id5 = 0, pre_speed_id8 = 0;  // 预存电机速度
int pre_lr_speed = 0;

int wifi_disconnected_flag = 0;  // WiFi断联标志位，1表示断联，0表示连接正常
uint32_t last_data_time = 0;     // 上次接收到数据的时间戳
uint32_t wifi_timeout_ms = 5000;  // WiFi超时时间，5秒无数据则认为断联

    int lr_speed = 0;  // 左右速度
    int fb_speed = 0;  // 前后速度
    int rotation = 0;  // 转速

uint32_t servo_return_timer = 0;  // 舵机回零计时器
uint8_t servo_return_flag = 0;    // 舵机回零标志位，1表示需要回零

static void demo_show_ip(char *buf)
{
    printf("IP: %s\r\n", buf);
}

/**
 * @brief       WiFi初始化, 该函数只需要调用一次
 * @param       无
 * @retval      0: 成功
 *              1: 失败
 */
uint8_t wifi_init(void)
{
    uint8_t ret;
    char ip_buf[16];

    printf("Starting WiFi initialization...\r\n");
    
    // 初始化ATK-MW8266D模块
    ret = atk_mw8266d_init(115200);
    ret_num = ret;  /* 保存返回值 */
    if (ret != 0)
    {
        printf("ATK-MW8266D init failed!\r\n");
        error_flag = 1;  /* 设置错误标志位 */
        return 1;
    }

    printf("Joining to AP...\r\n");
    ret  = atk_mw8266d_restore();                               /* 恢复出厂设置 */
    ret += atk_mw8266d_at_test();                               /* AT测试 */
    ret += atk_mw8266d_set_mode(1);                             /* Station模式 */
    ret += atk_mw8266d_sw_reset();                              /* 软件复位 */
    ret += atk_mw8266d_ate_config(0);                           /* 关闭回显功能 */
    ret += atk_mw8266d_join_ap(DEMO_WIFI_SSID, DEMO_WIFI_PWD);  /* 连接WIFI */
    ret += atk_mw8266d_get_ip(ip_buf);                          /* 获取IP地址 */
    if (ret != 0)
    {
        printf("Error to join ap!\r\n");
        error_flag = 2;
        return 1;
    }
    demo_show_ip(ip_buf);

    // 连接TCP服务器
    printf("Connecting to TCP server...\r\n");
    ret = atk_mw8266d_connect_tcp_server(DEMO_TCP_SERVER_IP, DEMO_TCP_SERVER_PORT);
    if (ret != 0)
    {
        printf("Error to connect tcp server!\r\n");
        error_flag = 3;
        return 1;
    }
    HAL_Delay(1000);  /* 等待1秒钟，确保连接成功 */
    
    // 进入透传模式
    printf("Entering unvarnished mode...\r\n");
    ret = atk_mw8266d_enter_unvarnished();
    if (ret != 0)
    {
        printf("Enter unvarnished failed!\r\n");
        error_flag = 4;  /* 设置错误标志位 */
        return 1;
    }
    
    printf("WiFi initialization completed successfully!\r\n");
    printf("Now in TCP transparent transmission mode.\r\n");
    
    /* 重新开始接收新的一帧数据 */
    atk_mw8266d_uart_rx_restart();
    
    return 0;
}

/**
 * @brief       处理接收到的TCP数据并串口打印
 * @param       无
 * @retval      无
 */
void wifi_process_data(void)
{
    uint8_t *buf;
    
    /* 等待接收ATK-MW8266D UART的一帧数据 */
    buf = atk_mw8266d_uart_rx_get_frame();
    if (buf != NULL)
    {
        strncpy(g_received_data, (char*)buf, sizeof(g_received_data) - 1);
        g_received_data[sizeof(g_received_data) - 1] = '\0';  /* 确保字符串结束 */

        // 更新最后接收数据的时间戳
        last_data_time = HAL_GetTick();
        
        // 如果之前断联，现在收到数据了，重置断联标志
        if (wifi_disconnected_flag == 1) {
            wifi_disconnected_flag = 0;
            printf("WiFi connection restored! Flag reset to 0\r\n");
        }

        check_emergency_stop(g_received_data);
        
        /* 设置数据就绪标志，触发系统立即处理 */
        g_data_ready_flag = 1;

        // printf("%s", buf);
        /* 重新开始等待ATK-MW8266D UART数据 */
        atk_mw8266d_uart_rx_restart();
    }

}

/**
 * @brief       主循环处理函数
 * @param       无
 * @retval      无
 */
void wifi_run(void)
{
    /* 循环接收来自服务器的数据，并打印 */
    wifi_process_data();
    wifi_handle_received_data();
    
    /* 检测WiFi连接状态 */
    wifi_check_connection_status();
}

/**
 * @brief       立即处理接收到的数据
 * @param       无
 * @retval      无
 */
void wifi_handle_received_data(void)
{
    if (g_data_ready_flag)
    {
        new_cmd = 1;  /* 设置新命令标志位 */
        /* 在这里添加您的数据处理逻辑 */
        printf("Processing data: %s\r\n", g_received_data);
        
        strncpy(cmd, g_received_data, sizeof(cmd) - 1);
        cmd[sizeof(cmd) - 1] = '\0';  /* 确保字符串结束 */

        if(RUNNING_STATUS == RUNNING_VALUE){
            /* 初始化状态下，处理初始化指令 */
            if (strstr(cmd, "value to slider") != NULL) {
                RUNNING_STATUS = RUNNING_SLIDER;  /* 设置运行状态为滑块模式 */
            }
        }
        /* 检查指令并执行相应动作 */
        if(RUNNING_STATUS !=RUNNING_ACTION && SNAKE_STATUS == SNAKE_LINE){
            if (strstr(cmd, "line to car") != NULL)
            {
                RUNNING_STATUS = RUNNING_ACTION;  /* 设置运行状态为动作执行 */
                LINE_TO_CAR();
                SNAKE_STATUS = SNAKE_CAR;  /* 设置蛇形状态为车辆模式 */
                RUNNING_STATUS = RUNNING_SLIDER;  /* 恢复为滑块模式 */
            }
        }

        if(RUNNING_STATUS !=RUNNING_ACTION && SNAKE_STATUS == SNAKE_CAR){
            if (strstr(cmd, "car to line") != NULL)
            {
                RUNNING_STATUS = RUNNING_ACTION;  /* 设置运行状态为动作执行 */
                CAR_TO_LINE();
                SNAKE_STATUS = SNAKE_LINE;  /* 设置蛇形状态为车辆模式 */
                RUNNING_STATUS = RUNNING_SLIDER;  /* 恢复为滑块模式 */
            }
        }

        if(RUNNING_STATUS !=RUNNING_ACTION && SNAKE_STATUS == SNAKE_CAR){
            if (strstr(cmd, "car to scan") != NULL)
            {
                RUNNING_STATUS = RUNNING_ACTION;  /* 设置运行状态为动作执行 */
                CAR_TO_SCAN();
                SNAKE_STATUS = SNAKE_SCAN;  /* 设置蛇形状态为扫描模式 */
                RUNNING_STATUS = RUNNING_SLIDER;  /* 恢复为滑块模式 */
            }
        }

        if(RUNNING_STATUS !=RUNNING_ACTION && SNAKE_STATUS == SNAKE_SCAN){
            if (strstr(cmd, "scan to car") != NULL)
            {
                RUNNING_STATUS = RUNNING_ACTION;  /* 设置运行状态为动作执行 */
                SCAN_TO_CAR();
                SNAKE_STATUS = SNAKE_CAR;  /* 设置蛇形状态为车辆模式 */
                RUNNING_STATUS = RUNNING_SLIDER;  /* 恢复为滑块模式 */
            }
        }




        if(RUNNING_STATUS == RUNNING_SLIDER){
            if(SNAKE_STATUS == SNAKE_CAR){
                /* 处理CAR模式下的slider控制指令 */
                if (strstr(cmd, "car slider") != NULL) {
                    process_car_slider_command(cmd);
                }
            }
            else if(SNAKE_STATUS == SNAKE_LINE){
                /* 处理LINE模式下的slider控制指令 */
                if (strstr(cmd, "line slider") != NULL) {
                    process_line_slider_command(cmd);
                }
            }
            else if(SNAKE_STATUS == SNAKE_SCAN){
                /* 处理SCAN模式下的slider控制指令 */
                if (strstr(cmd, "car slider") != NULL) {
                    process_car_slider_command(cmd);
                }
            }
        }

        /* 处理完成后清除标志位 */
        g_data_ready_flag = 0;
        
        /* 清空接收缓冲区 */
        memset(g_received_data, 0, sizeof(g_received_data));
    }
}


void check_emergency_stop(const char* data)
{
    // 检查急停命令
    if (strstr(data, "ESTOP") != NULL)
    {
        emergency_stop_flag = 1;
        system_running = 0;
        printf("EMERGENCY STOP ACTIVATED!\r\n");
        emergency_stop_action();
    }
    // 检查急停解除命令
    else if (strstr(data, "RESUME") != NULL)
    {
        uint8_t resume_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        if (emergency_stop_flag) 
        {
            emergency_stop_flag = 0;
            system_running = 1;
            FDCAN_Send(0xCC, resume_data, 8); // 发送解除急停的CAN消息
            printf("EMERGENCY STOP RELEASED - SYSTEM RESUMED\r\n");
                        
            HAL_GPIO_WritePin(GPIOB, LEDR_Pin, GPIO_PIN_SET);
        }
    }
}

/**
 * @brief       处理CAR模式下的slider控制指令
 * @param       cmd: 指令字符串
 * @retval      无
 */
void process_car_slider_command(const char* cmd)
{
    int lr_speed = 0;  // 左右速度
    int fb_speed = 0;  // 前后速度
    int rotation = 0;  // 转速
    
    // 解析指令，格式："car slider 左右速度 前后速度 转速"
    int parsed = sscanf(cmd, "car slider %d %d %d", &lr_speed, &fb_speed, &rotation);
    
    if (parsed != 3) {
        printf("Invalid car slider command format\r\n");
        return;
    }
        
    // 重置四个电机的转向和转速变量
    dir_id1 = 0; dir_id4 = 0; dir_id5 = 0; dir_id8 = 0;
    speed_id1 = 0; speed_id4 = 0; speed_id5 = 0; speed_id8 = 0;
    
    // 处理左右速度
    if (lr_speed != 0) {
        int lr_mapped_speed = (abs(lr_speed) * 0xF9) / 100;  // 映射到0x00~0xF9
        
        if (lr_speed < 0) {
            // 左右速度为负：id4和id8转向0x01, id1和id5转向0x00
            dir_id4 = 0x01; dir_id8 = 0x01; dir_id1 = 0x00; dir_id5 = 0x00;
            speed_id1 += lr_mapped_speed; speed_id4 += lr_mapped_speed;
            speed_id5 += lr_mapped_speed; speed_id8 += lr_mapped_speed;
        } else {
            // 左右速度为正：id4和id8转向0x00, id1和id5转向0x01
            dir_id4 = 0x00; dir_id8 = 0x00; dir_id1 = 0x01; dir_id5 = 0x01;
            speed_id1 += lr_mapped_speed; speed_id4 += lr_mapped_speed;
            speed_id5 += lr_mapped_speed; speed_id8 += lr_mapped_speed;
        }
    }
    
    // 处理前后速度
    if (fb_speed != 0) {
        int fb_mapped_speed = (abs(fb_speed) * 0xF9) / 100;  // 映射到0x00~0xF9
        
        if (fb_speed > 0) {
            // 前后速度为正：四个id转向都为0x01
            process_motor_combination(&dir_id1, &speed_id1, 0x01, fb_mapped_speed);
            process_motor_combination(&dir_id4, &speed_id4, 0x01, fb_mapped_speed);
            process_motor_combination(&dir_id5, &speed_id5, 0x01, fb_mapped_speed);
            process_motor_combination(&dir_id8, &speed_id8, 0x01, fb_mapped_speed);
        } else {
            // 前后速度为负：四个id转向都为0x00
            process_motor_combination(&dir_id1, &speed_id1, 0x00, fb_mapped_speed);
            process_motor_combination(&dir_id4, &speed_id4, 0x00, fb_mapped_speed);
            process_motor_combination(&dir_id5, &speed_id5, 0x00, fb_mapped_speed);
            process_motor_combination(&dir_id8, &speed_id8, 0x00, fb_mapped_speed);
        }
    }
    
    // 处理转速
    if (rotation != 0) {
        int rot_mapped_speed = (abs(rotation) * 0xF9) / 100;  // 映射到0x00~0xF9
        
        if (rotation > 0) {
            // 转速为正：id1和id4转向0x01, id8和id5转向0x00
            process_motor_combination(&dir_id1, &speed_id1, 0x01, rot_mapped_speed);
            process_motor_combination(&dir_id4, &speed_id4, 0x01, rot_mapped_speed);
            process_motor_combination(&dir_id8, &speed_id8, 0x00, rot_mapped_speed);
            process_motor_combination(&dir_id5, &speed_id5, 0x00, rot_mapped_speed);
        } else {
            // 转速为负：id1和id4转向0x00, id8和id5转向0x01
            process_motor_combination(&dir_id1, &speed_id1, 0x00, rot_mapped_speed);
            process_motor_combination(&dir_id4, &speed_id4, 0x00, rot_mapped_speed);
            process_motor_combination(&dir_id8, &speed_id8, 0x01, rot_mapped_speed);
            process_motor_combination(&dir_id5, &speed_id5, 0x01, rot_mapped_speed);
        }
    }
    
    // 处理溢出
    handle_speed_overflow(&dir_id1, &speed_id1, &dir_id4, &speed_id4, &dir_id5, &speed_id5, &dir_id8, &speed_id8);
    

}

/**
 * @brief       处理LINE模式下的slider控制指令
 * @param       cmd: 指令字符串
 * @retval      无
 */
void process_line_slider_command(const char* cmd)
{


    pre_dir_id1 = dir_id1;  // 预存电机ID1的方向
    pre_dir_id4 = dir_id4;  // 预存电机ID4的方向
    pre_dir_id5 = dir_id5;  // 预存电机ID
    pre_dir_id8 = dir_id8;  // 预存电机ID

    pre_speed_id1 = speed_id1;  // 预存电机ID1的速度
    pre_speed_id4 = speed_id4;  // 预存电机ID4的速度
    pre_speed_id5 = speed_id5;  // 预存电机ID5的速度
    pre_speed_id8 = speed_id8;  // 预存电机ID8的速度

    pre_lr_speed = lr_speed;  // 预存左右速度

    uint8_t preset_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // 解析指令，格式："line slider 左右速度 前后速度 转速"
    int parsed = sscanf(cmd, "line slider %d %d %d", &lr_speed, &fb_speed, &rotation);
    
    if (parsed != 3) {
        printf("Invalid line slider command format\r\n");
        return;
    }
    
    
    // 重置四个电机的转向和转速变量（共用同一套变量）
    dir_id1 = 0; dir_id4 = 0; dir_id5 = 0; dir_id8 = 0;
    speed_id1 = 0; speed_id4 = 0; speed_id5 = 0; speed_id8 = 0;

    // 处理左右速度
    if (lr_speed != 0) {
        // 先处理左右移动时的舵机控制逻辑

        if (lr_speed < 0) {
            // 左右移动为负且不为零
                servo1_id1 = SERVO_ZERO + 0x1F;
                servo1_id8 = SERVO_ZERO - 0x1F;
        } else if (lr_speed > 0) {
            // 左右移动为正且不为零
                servo1_id1 = SERVO_ZERO - 0x1F;
                servo1_id8 = SERVO_ZERO + 0x1F;
        }
        if(g_servo1_id1_final != servo1_id1 || g_servo1_id8_final != servo1_id8) {
            LINE_LR_FLAG = 0;  // 设置标志位，表示需要执行左右移动
        }
        if(LINE_LR_FLAG == 0) {
            // 如果是第一次执行左右移动，设置舵机位置
            preset_data[0] = 0x00;
            preset_data[1] = 0x00;
            preset_data[2] = servo1_id1;  // 设置ID1的舵机位置
            preset_data[3] = SERVO_ZERO;  // ID1的舵机2位置保持不变
            FDCAN_Send(0x01, preset_data, 8);  // 发送ID1的舵机位置
            HAL_Delay(1);
            preset_data[2] = servo1_id8;  // 设置ID8的舵机位置
            FDCAN_Send(0x08, preset_data, 8);  // 发送ID8的舵机位置
            HAL_Delay(100);
            LINE_LR_FLAG = 1;  // 设置标志位，表示已经执行过左右移动
        }

        
        // 舵机设置完成后，再执行左右移动的电机控制
        int lr_mapped_speed = (abs(lr_speed) * 0xF9) / 100;  // 映射到0x00~0xF9
        
        if (lr_speed > 0) {
            // 左右速度为正：id1和id8转向0x00, id5和id4转向0x01
            process_motor_combination(&dir_id1, &speed_id1, 0x00, lr_mapped_speed);
            process_motor_combination(&dir_id8, &speed_id8, 0x00, lr_mapped_speed);
            process_motor_combination(&dir_id5, &speed_id5, 0x01, lr_mapped_speed);
            process_motor_combination(&dir_id4, &speed_id4, 0x01, lr_mapped_speed);
        } else {
            // 左右速度为负：id1和id8转向0x01, id5和id4转向0x00
            process_motor_combination(&dir_id1, &speed_id1, 0x01, lr_mapped_speed);
            process_motor_combination(&dir_id8, &speed_id8, 0x01, lr_mapped_speed);
            process_motor_combination(&dir_id5, &speed_id5, 0x00, lr_mapped_speed);
            process_motor_combination(&dir_id4, &speed_id4, 0x00, lr_mapped_speed);
        }
    }

    else{
        if(pre_lr_speed != 0){
            uint8_t motor_set_zero[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            // 当前左右速度为0，但之前有左右速度
            // 立即停止电机，启动舵机回零计时器
            dir_id1 = 0; dir_id4 = 0; dir_id5 = 0; dir_id8 = 0;
            speed_id1 = 0; speed_id4 = 0; speed_id5 = 0; speed_id8 = 0;
            motor_set_zero[2] = servo1_id1;  // 设置ID1的舵机位置
            motor_set_zero[3] = SERVO_ZERO;  // ID1的舵机2位置保持不变
            FDCAN_Send(0x01, motor_set_zero, 8);  // 发送ID1的舵机位置
            HAL_Delay(1);
            motor_set_zero[2] = servo1_id8;  // 设置ID8的舵机位置
            FDCAN_Send(0x08, motor_set_zero, 8);  // 发送ID8的舵机位置
            HAL_Delay(1);
            motor_set_zero[2] = SERVO_ZERO;  // ID1的舵机1位置回到零点
            FDCAN_Send(0x04, motor_set_zero, 8);  // 发送ID4的舵机位置
            HAL_Delay(1);
            FDCAN_Send(0x05, motor_set_zero, 8);  // 发送ID5的舵机位置
            HAL_Delay(1);
            // // 启动舵机回零计时器
            // if (servo_return_flag == 0) {
            //     servo_return_timer = HAL_GetTick();  // 记录当前时间
            //     servo_return_flag = 1;               // 设置回零标志
            //     printf("Motors stopped, servo return timer started\r\n");
            // }
            
            // // 检查是否到了舵机回零时间（1秒后）
            // if (servo_return_flag == 1 && (HAL_GetTick() - servo_return_timer) >= 2000) {
            //     // 1秒后，舵机回到零点
            //     servo1_id1 = SERVO_ZERO;
            //     servo1_id8 = SERVO_ZERO;
            //     servo_return_flag = 0;  // 清除回零标志
            //     pre_lr_speed = 0;       // 清除预存速度
            //     LINE_LR_FLAG = 0;       // 重置标志位
            //     printf("Servos returned to zero position\r\n");

            HAL_Delay(1000);  // 等待1秒钟，确保舵机回零完成

                servo1_id1 = SERVO_ZERO;
                servo1_id8 = SERVO_ZERO;
                servo_return_flag = 0;  // 清除回零标志
                pre_lr_speed = 0;       // 清除预存速度
                LINE_LR_FLAG = 0;       // 重置标志位

                // 立即发送舵机回零指令
                uint8_t servo_zero_data[8] = {0x00, 0x00, SERVO_ZERO, SERVO_ZERO, 0x00, 0x00, 0x00, 0x00};
                FDCAN_Send(0x01, servo_zero_data, 8);  // 发送ID1的舵机回零
                HAL_Delay(1);
                FDCAN_Send(0x08, servo_zero_data, 8);  // 发送ID8的舵机回零
            //}
        }
        else{
    // 处理前后速度
        if (fb_speed != 0) {
            int fb_mapped_speed = (abs(fb_speed) * 0xF9) / 100;  // 映射到0x00~0xF9
            
            if (fb_speed > 0) {
                // 前后速度为正：id8和id5转向0x00, id1和id4转向0x01
                process_motor_combination(&dir_id8, &speed_id8, 0x00, fb_mapped_speed);
                process_motor_combination(&dir_id5, &speed_id5, 0x00, fb_mapped_speed);
                process_motor_combination(&dir_id1, &speed_id1, 0x01, fb_mapped_speed);
                process_motor_combination(&dir_id4, &speed_id4, 0x01, fb_mapped_speed);
            } else {
                // 前后速度为负：id8和id5转向0x01, id1和id4转向0x00
                process_motor_combination(&dir_id8, &speed_id8, 0x01, fb_mapped_speed);
                process_motor_combination(&dir_id5, &speed_id5, 0x01, fb_mapped_speed);
                process_motor_combination(&dir_id1, &speed_id1, 0x00, fb_mapped_speed);
                process_motor_combination(&dir_id4, &speed_id4, 0x00, fb_mapped_speed);
            }

            speed_id1/=2;  // 前后速度为正时，id1和id4的速度减半
            speed_id8/=2;  // 前后速度为正时，id8和id5的速度减半
        }

        // 处理旋转速度 - 改回电机旋转叠加
        if (rotation != 0) {
            // 将旋转速度从-100~100映射到0x00~0xF9
            int rot_mapped_speed = (abs(rotation) * 0xF9) / 100;
            
            if (rotation > 0) {
                // 旋转速度为正：左侧电机正转，右侧电机反转
                process_motor_combination(&dir_id1, &speed_id1, 0x00, rot_mapped_speed);
                process_motor_combination(&dir_id8, &speed_id8, 0x01, rot_mapped_speed);
            } else {
                // 旋转速度为负：左侧电机反转，右侧电机正转
                process_motor_combination(&dir_id1, &speed_id1, 0x01, rot_mapped_speed);
                process_motor_combination(&dir_id8, &speed_id8, 0x00, rot_mapped_speed);
            }
        }
        // 舵机在LINE模式下只受左右移动控制，不受旋转控制
        // 舵机位置保持由左右移动逻辑控制
    }

    }
    
    
    // 处理溢出（共用同一套溢出处理函数）
    handle_speed_overflow(&dir_id1, &speed_id1, &dir_id4, &speed_id4, &dir_id5, &speed_id5, &dir_id8, &speed_id8);

    g_servo1_id1_final = servo1_id1;  // 更新ID1的舵机位置
    g_servo2_id1_final = SERVO_ZERO;  // ID1的舵机2位置保持不变
    g_servo1_id2_final = SERVO_MIN;  // ID2的舵机1位置保持不变
    g_servo2_id2_final = SERVO_ZERO;  // ID2的舵机2位置保持不变
    g_servo1_id3_final = SERVO_MAX;  // ID3的舵机1位置保持不变
    g_servo2_id3_final = SERVO_ZERO;  // ID3的舵机2位置保持不变
    g_servo1_id4_final = SERVO_ZERO;  // ID4的舵机1位置保持不变
    g_servo2_id4_final = SERVO_ZERO;  // ID4的舵机2位置保持不变
    g_servo1_id5_final = SERVO_ZERO;  // ID5的舵机1位置保持不变
    g_servo2_id5_final = SERVO_ZERO;  // ID5的舵机2位置保持不变
    g_servo1_id6_final = SERVO_MAX;  // ID6的舵机1位置保持不变
    g_servo2_id6_final = SERVO_ZERO;  // ID6的舵机2位置保持不变
    g_servo1_id7_final = SERVO_MIN;  // ID7的舵机1位置保持不变
    g_servo2_id7_final = SERVO_ZERO;  // ID7的舵机2位置保持不变
    g_servo1_id8_final = servo1_id8;  // 更新ID8的舵机位置
    g_servo2_id8_final = SERVO_ZERO;  // ID8的舵机2位置保持不变

}

/**
 * @brief       检测WiFi连接状态
 * @param       无
 * @retval      无
 */
void wifi_check_connection_status(void)
{
    uint32_t current_time = HAL_GetTick();
    
    // 检查是否超时（超过设定时间没有接收到数据）
    if (last_data_time != 0 && (current_time - last_data_time) > wifi_timeout_ms) {
        if (wifi_disconnected_flag == 0) {
            wifi_disconnected_flag = 1;  // 设置断联标志位
            printf("WiFi connection lost! Flag set to 1\r\n");
            
            // 可以在这里添加断联后的处理逻辑
            // 比如：停止所有电机、设置安全状态等
            dir_id1 = 0; dir_id4 = 0; dir_id5 = 0; dir_id8 = 0;
            speed_id1 = 0; speed_id4 = 0; speed_id5 = 0; speed_id8 = 0;
        }
    }
    
    // 如果之前断联，但现在又有数据了，则恢复连接状态
    if (wifi_disconnected_flag == 1 && last_data_time != 0 && 
        (current_time - last_data_time) <= wifi_timeout_ms) {
        wifi_disconnected_flag = 0;
        printf("WiFi connection restored! Flag set to 0\r\n");
    }
}

/**
 * @brief       处理电机组合逻辑（同向相加，反向相减）
 * @param       dir: 当前方向指针
 * @param       speed: 当前速度指针
 * @param       new_dir: 新方向
 * @param       new_speed: 新速度
 * @retval      无
 */
void process_motor_combination(uint8_t* dir, int* speed, uint8_t new_dir, int new_speed)
{
    if (*speed == 0) {
        // 如果当前速度为0，直接设置新的方向和速度
        *dir = new_dir;
        *speed = new_speed;
    } else {
        // 如果当前已有速度，需要合并
        if (*dir == new_dir) {
            // 同方向：速度相加
            *speed += new_speed;
        } else {
            // 反方向：速度相减，取较大的方向
            if (*speed >= new_speed) {
                *speed -= new_speed;
                // 方向保持不变
            } else {
                *speed = new_speed - *speed;
                *dir = new_dir;  // 更新为新方向
            }
        }
    }
}

/**
 * @brief       处理速度溢出
 * @param       所有电机的方向和速度指针
 * @retval      无
 */
void handle_speed_overflow(uint8_t* dir_id1, int* speed_id1, uint8_t* dir_id4, int* speed_id4, 
                          uint8_t* dir_id5, int* speed_id5, uint8_t* dir_id8, int* speed_id8)
{
    // 处理溢出的数组
    int* speeds[] = {speed_id1, speed_id4, speed_id5, speed_id8};
    uint8_t* dirs[] = {dir_id1, dir_id4, dir_id5, dir_id8};
    
    // 计算总的溢出量
    int total_overflow = 0;
    int overflow_count = 0;
    int non_overflow_count = 0;
    
    for (int i = 0; i < 4; i++) {
        if (*speeds[i] > 0xF9) {
            total_overflow += (*speeds[i] - 0xF9);
            *speeds[i] = 0xF9;  // 限制到最大值
            overflow_count++;
        } else if (*speeds[i] > 0) {
            non_overflow_count++;
        }
    }
    
    // 如果有溢出且有其他未溢出的电机，尝试分配溢出量
    if (total_overflow > 0 && non_overflow_count > 0) {
        int overflow_per_motor = total_overflow / non_overflow_count;
        
        for (int i = 0; i < 4; i++) {
            if (*speeds[i] < 0xF9 && *speeds[i] > 0) {
                // 反向减去溢出量（相当于给反方向加速）
                if (*speeds[i] >= overflow_per_motor) {
                    *speeds[i] -= overflow_per_motor;
                } else {
                    // 如果当前速度小于溢出量，则反向
                    *speeds[i] = overflow_per_motor - *speeds[i];
                    *dirs[i] = (*dirs[i] == 0x01) ? 0x00 : 0x01;  // 反向
                }
            }
        }
    }
    
    // 确保所有速度都在有效范围内
    for (int i = 0; i < 4; i++) {
        if (*speeds[i] > 0xF9) {
            *speeds[i] = 0xF9;
        } else if (*speeds[i] < 0) {
            *speeds[i] = 0;
        }
    }
}
