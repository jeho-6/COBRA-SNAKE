/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
#include "can.h"
#include "main.h"
/* USER CODE BEGIN 0 */
#include "stdio.h"
#include "main.h"
#include "DCmotor.h"
#include "servo.h"
#include "buzzer.h"

CAN_RxHeaderTypeDef g_canx_rxheader;    
CAN_TxHeaderTypeDef txHeader;
uint8_t rxbuf[8];
uint32_t txMailbox;
uint32_t freeMailboxes;
uint8_t flag = 0;

extern int motor_dir;
extern int motor_vel;
extern int servo1;
extern int servo2;
extern int state;

/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 9;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_5TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = ENABLE;
  hcan.Init.AutoWakeUp = ENABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */
    CAN_FilterTypeDef sFilterConfig;
  
    sFilterConfig.FilterActivation = ENABLE;    
    sFilterConfig.FilterBank = 0;                             
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;       

    sFilterConfig.FilterIdHigh = 0x0000;                     
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;                  
    sFilterConfig.FilterMaskIdLow = 0x0000;
    //sFilterConfig.SlaveStartFilterBank = 14;
    
    
    if ( HAL_CAN_ConfigFilter( &hcan, &sFilterConfig ) != HAL_OK )
    {
        Error_Handler();
    }
    
    if ( HAL_CAN_Start( &hcan ) != HAL_OK )
    {
        Error_Handler();
    }
    if(HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)//??????0????
     {
      Error_Handler();
     }
     HAL_NVIC_EnableIRQ( CAN1_RX0_IRQn );                      
     HAL_NVIC_SetPriority( CAN1_RX0_IRQn, 7, 0 ); 
  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void CAN1_RX0_IRQHandler( void )
{
    HAL_CAN_IRQHandler( &hcan );  
}

void  HAL_CAN_RxFifo0MsgPendingCallback( CAN_HandleTypeDef* hcan )
{
    
  HAL_CAN_GetRxMessage( hcan, CAN_RX_FIFO0, &g_canx_rxheader, rxbuf );

    
    if(g_canx_rxheader.StdId == BOARD){
        if(state ==RUNNING){
          motor_dir = rxbuf[0];
          motor_vel = rxbuf[1]*2;
          servo1 = rxbuf[2];
          servo2 = rxbuf[3];
        }
    }
    else if(g_canx_rxheader.StdId == 0x10){
      if(state == RUNNING){
        motor_dir = rxbuf[0];
        motor_vel = rxbuf[1]*2;
        servo1 = rxbuf[2];
        servo2 = rxbuf[3];
      }
    }
    else if(g_canx_rxheader.StdId == 0xAA){
      //初始化  
      motor_dir = 0;
      motor_vel = 0;
      servo1 = ZERO;
      servo2 = ZERO;
      DCmotor_Init();
      servo_Init();
      buzzer_init();
      HAL_Delay(INIT_DELAY_TIME);
      state = RUNNING;
      Play_Startup_Melody();
    }
    else if(g_canx_rxheader.StdId == 0xFF){
      //急停
      Set_motor(motor_dir, 0);
      DCmotor_DeInit();
      servo_DeInit();
      HAL_Delay(DEINIT_DELAY_TIME);
      state = ESTOP;
      Play_Stop_Melody();
    }
    else if(g_canx_rxheader.StdId == 0xCC){
      //恢复
      motor_dir = 0;
      motor_vel = 0;

      DCmotor_Init();
      servo_resume();
      buzzer_init();
      HAL_Delay(INIT_DELAY_TIME);
      state = RUNNING;
      Play_Resume_Melody();
    }
}


void CAN_Send(uint32_t id, uint8_t* data, uint8_t length){
    
    txHeader.StdId = id;             
    txHeader.ExtId = 0x00;           
    txHeader.IDE = CAN_ID_STD;       
    txHeader.RTR = CAN_RTR_DATA;     
    txHeader.DLC = length;        
    txHeader.TransmitGlobalTime = DISABLE;
    freeMailboxes = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);
    if (HAL_CAN_AddTxMessage(&hcan, &txHeader, data, &txMailbox) != HAL_OK) {
        Error_Handler(); 
    }
    
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
