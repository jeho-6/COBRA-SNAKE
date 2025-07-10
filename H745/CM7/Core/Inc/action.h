/**
 * @file action.h
 * @author Jeho
 * @date 2025-07-04
 * @brief Action module header file
 */

#ifndef __ACTION_H__
#define __ACTION_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>
#include "fdcan.h"
#include "wifi.h"
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/


/* Exported variables --------------------------------------------------------*/
// 外部声明全局变量：记录LINE_TO_CAR执行完毕后各ID舵机的最终位置
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

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief Initialize F103 group
 * @param None
 * @retval None
 */
void F103GROUP_INIT(void);
void LINE_TO_CAR(void);
void CAR_TO_LINE(void);
void CAR_TO_SCAN(void);
void SCAN_TO_CAR(void);
void LINE_TO_SCAN(void);
void SCAN_TO_LINE(void);


#ifdef __cplusplus
}
#endif

#endif /* __ACTION_H__ */

