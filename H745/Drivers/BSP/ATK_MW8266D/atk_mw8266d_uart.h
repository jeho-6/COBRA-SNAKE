/**
 ****************************************************************************************************
 * @file        atk_mw8266d_uart.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MW8266Dģ�� UART �ӿ�����ͷ�ļ�
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ����ƽ̨: ����ԭ�� STM32H7 ϵ�п�����
 * ����:     www.alientek.com
 * ��̳:     www.openedv.com
 * �Ա�:     openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __ATK_MW8266D_UART_H
#define __ATK_MW8266D_UART_H

#include "./SYSTEM/sys/sys.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_usart.h"
#include "stm32h7xx_hal_uart.h"


/* UART4引脚宏定义（H745 UART4，TX:PB9, RX:PB8） */
#define ATK_MW8266D_UART_TX_GPIO_PORT           GPIOB
#define ATK_MW8266D_UART_TX_GPIO_PIN            GPIO_PIN_9
#define ATK_MW8266D_UART_TX_GPIO_AF             GPIO_AF8_UART4
#define ATK_MW8266D_UART_TX_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define ATK_MW8266D_UART_RX_GPIO_PORT           GPIOB
#define ATK_MW8266D_UART_RX_GPIO_PIN            GPIO_PIN_8
#define ATK_MW8266D_UART_RX_GPIO_AF             GPIO_AF8_UART4
#define ATK_MW8266D_UART_RX_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define ATK_MW8266D_UART_INTERFACE              UART4
#define ATK_MW8266D_UART_IRQn                   UART4_IRQn
#define ATK_MW8266D_UART_IRQHandler             UART4_IRQHandler
#define ATK_MW8266D_UART_CLK_ENABLE()           do{ __HAL_RCC_UART4_CLK_ENABLE(); }while(0)    /* UART4 时钟使能 */

/* UART��������С */
#define ATK_MW8266D_UART_RX_BUF_SIZE            128
#define ATK_MW8266D_UART_TX_BUF_SIZE            64

/* �������� */
void atk_mw8266d_uart_printf(char *fmt, ...);       /* ATK-MW8266D UART printf */
void atk_mw8266d_uart_rx_restart(void);             /* ���¿�ʼ����һ֡���� */
uint8_t *atk_mw8266d_uart_rx_get_frame(void);       /* ��ȡ���յ���һ֡���� */
uint16_t atk_mw8266d_uart_rx_get_frame_len(void);   /* ��ȡ���յ���һ֡���ݳ��� */
void atk_mw8266d_uart_init(uint32_t baudrate);      /* ATK-MW8266D UART��ʼ�� */

typedef enum {
    ATK_UART_WAIT_READY = 0,
    ATK_UART_WORKING
} atk_uart_state_t;

extern atk_uart_state_t g_atk_uart_state;

#endif
