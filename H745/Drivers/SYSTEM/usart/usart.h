/**
 ****************************************************************************************************
 * @file        usart.h
 * @author      ALIENTEK
 * @version     V1.0
 * @date        2022-09-6
 * @brief       串口初始化驱动(一号线驱动1)支持printf
 * @license     Copyright (c) 2020-2032, 深圳市元子科技有限公司
 ****************************************************************************************************
 * @attention
 * 该文件是ATK-MW8266D模块的UART驱动头文件，提供了串口初始化、接收和发送功能。
 *
 ****************************************************************************************************
 */

#ifndef _USART_H
#define _USART_H

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"

/*******************************************************************************************************/

#define USART_TX_GPIO_PORT              GPIOD
#define USART_TX_GPIO_PIN               GPIO_PIN_5
#define USART_TX_GPIO_AF                GPIO_AF7_USART2
#define USART_TX_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)   /* TX引脚时钟使能 */

#define USART_RX_GPIO_PORT              GPIOD
#define USART_RX_GPIO_PIN               GPIO_PIN_6
#define USART_RX_GPIO_AF                GPIO_AF7_USART2
#define USART_RX_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)   /* RX引脚时钟使能 */

#define USART_UX                        USART2
#define USART_UX_IRQn                   USART2_IRQn
#define USART_UX_IRQHandler             USART2_IRQHandler
#define USART_UX_CLK_ENABLE()           do{ __HAL_RCC_USART2_CLK_ENABLE(); }while(0)  /* USART2 时钟使能 */

/*******************************************************************************************************/

#define USART_REC_LEN   200                     /* 最大接收字节数 200 */
#define USART_EN_RX     1                       /* 使能（1）/禁止（0）接收功能 */
#define RXBUFFERSIZE    1                       /* 接收缓冲区大小 */

extern UART_HandleTypeDef g_uart1_handle;       /* UART句柄 */

extern uint8_t  g_usart_rx_buf[USART_REC_LEN];  /* 接收缓冲区，最大USART_REC_LEN字节，末尾为换行符 */
extern uint16_t g_usart_rx_sta;                 /* 接收状态标志 */
extern uint8_t g_rx_buffer[RXBUFFERSIZE];       /* HAL库USART接收Buffer */


void usart_init(uint32_t baudrate);             /* 串口初始化函数 */

#endif


