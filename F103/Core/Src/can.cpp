//
// Created by jeho6 on June 26 2024
//

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
#include "stdio.h"
/* USER CODE BEGIN 0 */



CAN_TxHeaderTypeDef txHeader;
uint32_t mailbox;
/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  // CAN filter configuration
    CAN_FilterTypeDef Mycan_Filter;

    Mycan_Filter.FilterIdHigh         = 0x34;                 
    Mycan_Filter.FilterIdLow          = 0x00;                 
    Mycan_Filter.FilterMaskIdHigh     = 0x00;                
    Mycan_Filter.FilterMaskIdLow      = 0x00;                 
    Mycan_Filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;     
    Mycan_Filter.FilterBank           = 0;                    
    Mycan_Filter.FilterMode           = CAN_FILTERMODE_IDMASK; 
    Mycan_Filter.FilterScale          = CAN_FILTERSCALE_16BIT; 
    Mycan_Filter.FilterActivation     = CAN_FILTER_ENABLE;    
    Mycan_Filter.SlaveStartFilterBank = 14;   
  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 4;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_2TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_6TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {

        Error_Handler();
    }
  if (HAL_CAN_ConfigFilter(&hcan, &Mycan_Filter) != HAL_OK)
    {
        printf("DWB --- can filter configuration failed\n");
        Error_Handler();
    }

  /* USER CODE BEGIN CAN_Init 2 */

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
/* CAN send function */
void CAN_Send(uint32_t id, uint8_t* data, uint8_t length) {
  
    txHeader.StdId = id; 
    txHeader.ExtId = 0; 
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.IDE = CAN_ID_STD; 
    txHeader.DLC = length; 

    
    if (HAL_CAN_AddTxMessage(&hcan, &txHeader, data, &mailbox) != HAL_OK) {
        //printf("Error: Failed to add CAN message to mailbox\n");
          //ErrorDetect();
        Error_Handler(); 
    }

    // Optional: Wait for transmission to complete
    /*while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) != 3) {
      
        // Waiting for all mailboxes to be free
    }*/
    //ErrorDetect();
    //printf("CAN message sent successfully\n");
}


/* CAN receive function */
void CAN_Receive(void) {
    CAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8]; 

    if (HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &rxHeader, rxData) != HAL_OK) {
        Error_Handler(); 
    }

}


void USB_LP_CAN1_RX0_IRQHandler(void) {
    HAL_CAN_IRQHandler(&hcan);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK) {
        // Process the received message (e.g., print or store the data)
        printf("Received CAN message: ID = 0x%X, Data = ", rxHeader.StdId);
        for (int i = 0; i < rxHeader.DLC; i++) {
            printf("0x%X ", rxData[i]);
        }
        printf("\n");
    } else {
        // Handle reception error
        Error_Handler();
    }
}

void ErrorDetect(void){
  while(1){
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
      
      HAL_Delay(100);
      
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);

      
      HAL_Delay(100);
  }
  
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
