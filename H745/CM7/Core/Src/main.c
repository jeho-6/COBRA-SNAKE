/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fdcan.h"
#include "memorymap.h"
#include "gpio.h"
#include "wifi.h"
#include "./BSP/ATK_MW8266D/atk_mw8266d_uart.h"
#include "action.h"
#include "stdlib.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// 外部变量声明：电机方向和速度
extern uint8_t dir_id1, dir_id4, dir_id5, dir_id8;
extern int speed_id1, speed_id4, speed_id5, speed_id8;
extern uint8_t servo1_id1, servo1_id8;  // ID1和ID8的servo1控制变量
extern int wifi_disconnected_flag;  // WiFi断联标志位

// 外部变量声明：舵机最终位置
extern uint8_t g_servo1_id1_final;  // ID 0x01的servo1最终位置
extern uint8_t g_servo2_id1_final;  // ID 0x01的servo2最终位置
extern uint8_t g_servo1_id2_final;  // ID 0x02的servo1最终位置
extern uint8_t g_servo2_id2_final;  // ID 0x02的servo2最终位置
extern uint8_t g_servo1_id3_final;  // ID 0x03的servo1最终位置
extern uint8_t g_servo2_id3_final;  // ID 0x03的servo2最终位置
extern uint8_t g_servo1_id4_final;  // ID 0x04的servo1最终位置
extern uint8_t g_servo2_id4_final;  // ID 0x04的servo2最终位置
extern uint8_t g_servo1_id5_final;  // ID 0x05的servo1最终位置
extern uint8_t g_servo2_id5_final;  // ID 0x05的servo2最终位置
extern uint8_t g_servo1_id6_final;  // ID 0x06的servo1最终位置
extern uint8_t g_servo2_id6_final;  // ID 0x06的servo2最终位置
extern uint8_t g_servo1_id7_final;  // ID 0x07的servo1最终位置
extern uint8_t g_servo2_id7_final;  // ID 0x07的servo2最终位置
extern uint8_t g_servo1_id8_final;  // ID 0x08的servo1最终位置
extern uint8_t g_servo2_id8_final;  // ID 0x08的servo2最终位置

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t tx_data[8] = {0x00, 0x00, SERVO_MAX, SERVO_ZERO, 0x00, 0x00, 0x00, 0x00};
int busoff = 1;
int activity = 2;
char cmd[100] = {0};
int new_cmd = 0;

uint8_t tx_cmd1[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t tx_cmd2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t tx_cmd3[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t tx_cmd4[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t tx_cmd5[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t tx_cmd6[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t tx_cmd7[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t tx_cmd8[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

volatile uint8_t emergency_stop_flag = 0;  // 急停标志位
volatile uint8_t system_running = 1;       // 系统运行状态


// 急停处理函数
void emergency_stop_action(void)
{
    
    uint8_t emergency_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    
    // for (uint32_t can_id = 0x01; can_id <= 0x08; can_id++)
    // {
    //     FDCAN_Send(can_id, emergency_data, 8);
    //     HAL_Delay(1);
    // }
    FDCAN_Send(0xFF, emergency_data, 8); // 发送急停CAN消息
}

#define CHECK_EMERGENCY_STOP() \
    do { \
        if (emergency_stop_flag) { \
            static uint32_t last_emergency_send = 0; \
            uint32_t current_time = HAL_GetTick(); \
            \
            while(emergency_stop_flag) { \
                HAL_GPIO_TogglePin(GPIOB, LEDR_Pin); \
                \
                /* 每500ms重新发送一次急停CAN消息 */ \
                if (current_time - last_emergency_send >= 500) { \
                    uint8_t emergency_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF}; \
                    for (uint32_t can_id = 0x01; can_id <= 0x08; can_id++) { \
                        FDCAN_Send(can_id, emergency_data, 8); \
                    } \
                    last_emergency_send = current_time; \
                } \
                \
                HAL_Delay(100); \
                wifi_run(); /* 继续监听是否有解除急停命令 */ \
                current_time = HAL_GetTick(); \
            } \
        } \
    } while(0)


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
/* USER CODE BEGIN Boot_Mode_Sequence_0 */
  // int32_t timeout;
/* USER CODE END Boot_Mode_Sequence_0 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
  /* Wait until CPU2 boots and enters in stop mode or timeout*/
  // timeout = 0xFFFF;
  // while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
  // if ( timeout < 0 )
  // {
  // Error_Handler();
  // }
/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
/* USER CODE BEGIN Boot_Mode_Sequence_2 */
/* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
HSEM notification */
/*HW semaphore Clock enable*/
// __HAL_RCC_HSEM_CLK_ENABLE();
// /*Take HSEM */
// HAL_HSEM_FastTake(HSEM_ID_0);
// /*Release HSEM in order to notify the CPU2(CM4)*/
// HAL_HSEM_Release(HSEM_ID_0,0);
// /* wait until CPU2 wakes up from stop mode */
// timeout = 0xFFFF;
// while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
// if ( timeout < 0 )
// {
// Error_Handler();
// }
/* USER CODE END Boot_Mode_Sequence_2 */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FDCAN1_Init();
  /* USER CODE BEGIN 2 */
  HAL_FDCAN_Start(&hfdcan1);
  
  
  // for(int i = 0; i < 8; i++) {
  //   tx_data[i] = 0x01; // 初始化发送数据
  // }

  HAL_Delay(500); // 等待500毫秒，确保系统稳定


  if (wifi_init() != 0)
  {
    while (1) 
    { 
        HAL_GPIO_TogglePin(GPIOB, LEDR_Pin); // 红灯闪烁表示错误
        HAL_Delay(200); 
    }
  }


  F103GROUP_INIT(); // 初始化F103组
  RUNNING_STATUS = RUNNING_VALUE; // 设置初始运行状态为初始化模式
  FDCAN_Send(0x02, tx_data, 8); // 发送初始化数据到ID 0x02
  HAL_Delay(1);
  FDCAN_Send(0x07, tx_data, 8); // 发送初始化数据到ID 0x07
  HAL_Delay(1);
  tx_data[2] = SERVO_MIN;
  FDCAN_Send(0x03, tx_data, 8); // 发送初始化数据到ID 0x03
  HAL_Delay(1);
  FDCAN_Send(0x06, tx_data, 8); // 发送初始化数据到ID 0x04
  HAL_Delay(1);
  SNAKE_STATUS = SNAKE_LINE; // 设置蛇形状态为线条模式

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // if(wifi_disconnected_flag) {
    //     // 如果WiFi断开连接，进入急停状态
    //     emergency_stop_flag = 1; // 设置急停标志位
    //     emergency_stop_action();  // 执行急停操作
    //     HAL_GPIO_TogglePin(GPIOB, LEDR_Pin); // 红灯闪烁表示急停状态
        
    //     // 在WiFi断联期间继续运行wifi_run()以检测重连
    //     wifi_run();
    //     wifi_disconnected_flag = 0; // 重置WiFi断联标志位
    //     HAL_Delay(100); // 短延时避免过于频繁的检测
    // }

    //CHECK_EMERGENCY_STOP();
    if (system_running) 
    {
        // FDCAN_Send(0x01, tx_data, 8);
        
        // CHECK_EMERGENCY_STOP(); // 发送后再次检查
        
        // FDCAN_ProtocolStatusTypeDef status;
        // HAL_FDCAN_GetProtocolStatus(&hfdcan1, &status);
        // busoff = status.BusOff;
        // activity = status.Activity;
        if(RUNNING_STATUS == RUNNING_INIT){
          HAL_Delay(1); // 等待1秒，确保系统稳定
        }

        else{
          if(RUNNING_STATUS != RUNNING_ACTION)
          {
              if(RUNNING_STATUS == RUNNING_SLIDER){
                if(SNAKE_STATUS ==SNAKE_CAR || SNAKE_STATUS == SNAKE_LINE || SNAKE_STATUS == SNAKE_SCAN){
                  // 为tx_cmd1~8赋值，使用电机方向/速度和舵机最终位置
                  
                  // tx_cmd1 - ID 0x01
                  tx_cmd1[0] = dir_id1;                    // 电机方向
                  tx_cmd1[1] = (uint8_t)(speed_id1 & 0xFF); // 电机速度
                  tx_cmd1[2] = g_servo1_id1_final;                 // servo1由line slider控制
                  tx_cmd1[3] = g_servo2_id1_final;         // servo2最终位置
                  tx_cmd1[4] = 0x00;                       // 空
                  tx_cmd1[5] = 0x00;                       // 空
                  tx_cmd1[6] = 0x00;                       // 空
                  tx_cmd1[7] = 0x00;                       // 空
                  
                  // tx_cmd2 - ID 0x02
                  tx_cmd2[0] = 0x00;                       // 无电机控制
                  tx_cmd2[1] = 0x00;                       // 无电机控制
                  tx_cmd2[2] = g_servo1_id2_final;         // servo1最终位置
                  tx_cmd2[3] = g_servo2_id2_final;         // servo2最终位置
                  tx_cmd2[4] = 0x00;                       // 空
                  tx_cmd2[5] = 0x00;                       // 空
                  tx_cmd2[6] = 0x00;                       // 空
                  tx_cmd2[7] = 0x00;                       // 空
                  
                  // tx_cmd3 - ID 0x03
                  tx_cmd3[0] = 0x00;                       // 无电机控制
                  tx_cmd3[1] = 0x00;                       // 无电机控制
                  tx_cmd3[2] = g_servo1_id3_final;         // servo1最终位置
                  tx_cmd3[3] = g_servo2_id3_final;         // servo2最终位置
                  tx_cmd3[4] = 0x00;                       // 空
                  tx_cmd3[5] = 0x00;                       // 空
                  tx_cmd3[6] = 0x00;                       // 空
                  tx_cmd3[7] = 0x00;                       // 空
                  
                  // tx_cmd4 - ID 0x04
                  tx_cmd4[0] = dir_id4;                    // 电机方向
                  tx_cmd4[1] = (uint8_t)(speed_id4 & 0xFF); // 电机速度
                  tx_cmd4[2] = g_servo1_id4_final;         // servo1最终位置
                  tx_cmd4[3] = g_servo2_id4_final;         // servo2最终位置
                  tx_cmd4[4] = 0x00;                       // 空
                  tx_cmd4[5] = 0x00;                       // 空
                  tx_cmd4[6] = 0x00;                       // 空
                  tx_cmd4[7] = 0x00;                       // 空
                  
                  // tx_cmd5 - ID 0x05
                  tx_cmd5[0] = dir_id5;                    // 电机方向
                  tx_cmd5[1] = (uint8_t)(speed_id5 & 0xFF); // 电机速度
                  tx_cmd5[2] = g_servo1_id5_final;         // servo1最终位置
                  tx_cmd5[3] = g_servo2_id5_final;         // servo2最终位置
                  tx_cmd5[4] = 0x00;                       // 空
                  tx_cmd5[5] = 0x00;                       // 空
                  tx_cmd5[6] = 0x00;                       // 空
                  tx_cmd5[7] = 0x00;                       // 空
                  
                  // tx_cmd6 - ID 0x06
                  tx_cmd6[0] = 0x00;                       // 无电机控制
                  tx_cmd6[1] = 0x00;                       // 无电机控制
                  tx_cmd6[2] = g_servo1_id6_final;         // servo1最终位置
                  tx_cmd6[3] = g_servo2_id6_final;         // servo2最终位置
                  tx_cmd6[4] = 0x00;                       // 空
                  tx_cmd6[5] = 0x00;                       // 空
                  tx_cmd6[6] = 0x00;                       // 空
                  tx_cmd6[7] = 0x00;                       // 空
                  
                  // tx_cmd7 - ID 0x07
                  tx_cmd7[0] = 0x00;                       // 无电机控制
                  tx_cmd7[1] = 0x00;                       // 无电机控制
                  tx_cmd7[2] = g_servo1_id7_final;         // servo1最终位置
                  tx_cmd7[3] = g_servo2_id7_final;         // servo2最终位置
                  tx_cmd7[4] = 0x00;                       // 空
                  tx_cmd7[5] = 0x00;                       // 空
                  tx_cmd7[6] = 0x00;                       // 空
                  tx_cmd7[7] = 0x00;                       // 空
                  
                  // tx_cmd8 - ID 0x08
                  tx_cmd8[0] = dir_id8;                    // 电机方向
                  tx_cmd8[1] = (uint8_t)(speed_id8 & 0xFF); // 电机速度
                  tx_cmd8[2] = g_servo1_id8_final;                 // servo1由line slider控制
                  tx_cmd8[3] = g_servo2_id8_final;         // servo2最终位置
                  tx_cmd8[4] = 0x00;                       // 空
                  tx_cmd8[5] = 0x00;                       // 空
                  tx_cmd8[6] = 0x00;                       // 空
                  tx_cmd8[7] = 0x00;                       // 空
                  
                  // 发送CAN消息到各个ID
                  FDCAN_Send(0x01, tx_cmd1, 8);
                  HAL_Delay(1);
                  FDCAN_Send(0x02, tx_cmd2, 8);
                  HAL_Delay(1);
                  FDCAN_Send(0x03, tx_cmd3, 8);
                  HAL_Delay(1);
                  FDCAN_Send(0x04, tx_cmd4, 8);
                  HAL_Delay(1);
                  FDCAN_Send(0x05, tx_cmd5, 8);
                  HAL_Delay(1);
                  FDCAN_Send(0x06, tx_cmd6, 8);
                  HAL_Delay(1);
                  FDCAN_Send(0x07, tx_cmd7, 8);
                  HAL_Delay(1);
                  FDCAN_Send(0x08, tx_cmd8, 8);
                  HAL_Delay(1);

                }
              }
          }
        }

    }
    //CHECK_EMERGENCY_STOP();
    
    wifi_run();
    
    //CHECK_EMERGENCY_STOP();

    // FDCAN_Send(0x123, tx_data, 8); // 发送ID为0x123，8字节数据

    // FDCAN_ProtocolStatusTypeDef status;
    // HAL_FDCAN_GetProtocolStatus(&hfdcan1, &status);
    // busoff = status.BusOff;
    // activity = status.Activity;
    
    // if(new_cmd){
    //   HAL_Delay(2000);
    //   new_cmd = 0; // 重置新命令标志位
    // }

    // if (system_running) 
    // {
    //     HAL_GPIO_TogglePin(GPIOB, LEDG_Pin);
    //     tx_data[0]++;
    //     HAL_Delay(1000);
    // }
    // else 
    // {
    //     HAL_Delay(10); // 急停状态下减少CPU占用
    // }

    // if (status.BusOff == 0 && status.Activity != 0) {
    //     // ✅ 已进入 Bus On 状态

    // } else {
    //     // ❌ 未进入 Bus On，可能是 Bus Off、睡眠、初始化等状态
    //     while(1){
    //         HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // 用灯表示错误状态
    //         HAL_Delay(100); // 延时100ms
    //     }
    // }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x20000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.BaseAddress = 0x38000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
