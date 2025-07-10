#include "main.h"
#include "fdcan.h"
#include "memorymap.h"
#include "gpio.h"
#include "wifi.h"
#include "./BSP/ATK_MW8266D/atk_mw8266d_uart.h"
#include "action.h"
#include <string.h>


// 全局变量：保存LINE_TO_CAR执行完毕后各ID舵机的最终位置
uint8_t g_servo1_id1_final = SERVO_ZERO;  // ID 0x01的servo1最终位置
uint8_t g_servo2_id1_final = SERVO_ZERO;  // ID 0x01的servo2最终位置
uint8_t g_servo1_id2_final = SERVO_ZERO;  // ID 0x02的servo1最终位置
uint8_t g_servo2_id2_final = SERVO_ZERO;  // ID 0x02的servo2最终位置
uint8_t g_servo1_id3_final = SERVO_ZERO;  // ID 0x03的servo1最终位置
uint8_t g_servo2_id3_final = SERVO_ZERO;  // ID 0x03的servo2最终位置
uint8_t g_servo1_id4_final = SERVO_ZERO;   // ID 0x04的servo1最终位置
uint8_t g_servo2_id4_final = SERVO_ZERO;  // ID 0x04的servo2最终位置
uint8_t g_servo1_id5_final = SERVO_ZERO;   // ID 0x05的servo1最终位置
uint8_t g_servo2_id5_final = SERVO_ZERO;  // ID 0x05的servo2最终位置
uint8_t g_servo1_id6_final = SERVO_ZERO;  // ID 0x06的servo1最终位置
uint8_t g_servo2_id6_final = SERVO_ZERO;  // ID 0x06的servo2最终位置
uint8_t g_servo1_id7_final = SERVO_ZERO;  // ID 0x07的servo1最终位置
uint8_t g_servo2_id7_final = SERVO_ZERO;  // ID 0x07的servo2最终位置
uint8_t g_servo1_id8_final = SERVO_ZERO;   // ID 0x08的servo1最终位置
uint8_t g_servo2_id8_final = SERVO_ZERO;  // ID 0x08的servo2最终位置

void F103GROUP_INIT(void)
{
    uint8_t tx_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    /* Enable the MPU */
    FDCAN_Send(0xAA, tx_data, 8); // 发送一个空数据帧作为初始化
    HAL_Delay(5000);
}

void LINE_TO_CAR(void)
{
    uint8_t tx_data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    // 初始化舵机位置
    int servo1_id4 = SERVO_ZERO;
    int servo2_id4 = SERVO_ZERO;
    int servo1_id5 = SERVO_ZERO;
    int servo2_id5 = SERVO_ZERO;
    int servo1_id6 = SERVO_ZERO;  // 新增ID 0x06的servo1
    int servo2_id6 = SERVO_ZERO;  // 新增ID 0x06的servo2
    int servo1_id3 = SERVO_ZERO;  // 新增ID 0x03的servo1
    int servo2_id3 = SERVO_ZERO;  // 新增ID 0x03的servo2
    int servo1_id8 = SERVO_ZERO;  // 新增ID 0x08的servo1
    int servo2_id8 = SERVO_ZERO;  // 新增ID 0x08的servo2
    int servo1_id1 = SERVO_ZERO;  // 新增ID 0x01的servo1
    int servo2_id1 = SERVO_ZERO;  // 新增ID 0x01的servo2
    
    // 初始化舵机变量后，立即向每个ID发送舵机归零指令，电机速度为0
    uint8_t init_data[8] = {0x00, 0x00, SERVO_ZERO, SERVO_ZERO, 0x00, 0x00, 0x00, 0x00};
    
    // 向ID 0x01发送舵机归零指令
    FDCAN_Send(0x01, init_data, 8);
    HAL_Delay(1);
    
    // 向ID 0x02发送舵机归零指令
    FDCAN_Send(0x02, init_data, 8);
    HAL_Delay(1);
    
    // 向ID 0x03发送舵机归零指令
    FDCAN_Send(0x03, init_data, 8);
    HAL_Delay(1);
    
    // 向ID 0x04发送舵机归零指令
    FDCAN_Send(0x04, init_data, 8);
    HAL_Delay(1);
    
    // 向ID 0x05发送舵机归零指令
    FDCAN_Send(0x05, init_data, 8);
    HAL_Delay(1);
    
    // 向ID 0x06发送舵机归零指令
    FDCAN_Send(0x06, init_data, 8);
    HAL_Delay(1);
    
    // 向ID 0x07发送舵机归零指令
    FDCAN_Send(0x07, init_data, 8);
    HAL_Delay(1);
    
    // 向ID 0x08发送舵机归零指令
    FDCAN_Send(0x08, init_data, 8);
    HAL_Delay(200);
    
    // 设置步数
    int N = 20;
    int K = 15;  // ID 0x06和ID 0x03的步数，可以根据需要调整
    int P = 10;  // ID 0x08和ID 0x01的步数，可以根据需要调整
    int Q = 20;  // ID 0x03和ID 0x06的最后一步步数，可以根据需要调整
    
    // 计算每步的增量
    int step_id4 = (SERVO_MIN - SERVO_ZERO) / N;   // ID 0x04的步长
    int step_id5 = (SERVO_MAX - SERVO_ZERO) / N;   // ID 0x05的步长
    int step_id6 = (0x6E - SERVO_ZERO) / K;        // ID 0x06的步长
    int step_id3 = (0xBE - SERVO_ZERO) / K;        // ID 0x03的步长
    int step_id8 = (SERVO_MIN - SERVO_ZERO) / P;   // ID 0x08的步长
    int step_id1 = (SERVO_MAX - SERVO_ZERO) / P;   // ID 0x01的步长
    
    // 新增：计算ID 0x03和ID 0x06的servo2从当前位置到SERVO_ZERO的步长
    int step_id3_final = (SERVO_ZERO - 0xBE) / Q;  // ID 0x03的servo2从0xBE到SERVO_ZERO
    int step_id6_final = (SERVO_ZERO - 0x6E) / Q;  // ID 0x06的servo2从0x6E到SERVO_ZERO
    
    // 在舵机运动前发送启动信号到0x01
    uint8_t start_data[8] = {0x01, 0x90, SERVO_ZERO, SERVO_ZERO, 0x00, 0x00, 0x00, 0x00};
    FDCAN_Send(0x01, start_data, 8);
    HAL_Delay(1);  // 短暂延时，确保信号发送完成
    FDCAN_Send(0x08, start_data, 8);
    
    // 分N步执行，ID 0x04和ID 0x05同步运动
    for(int i = 0; i < N; i++) {
        servo1_id4 += step_id4;  // ID 0x04从SERVO_ZERO到SERVO_MIN
        servo1_id5 += step_id5;  // ID 0x05从SERVO_ZERO到SERVO_MAX
        
        // 发送ID 0x04的控制数据
        tx_data[2] = servo1_id4;
        tx_data[3] = servo2_id4;
        FDCAN_Send(0x04, tx_data, 8);
        HAL_Delay(1);  // 短暂延时
        
        // 发送ID 0x05的控制数据
        tx_data[2] = servo1_id5;
        tx_data[3] = servo2_id5;
        FDCAN_Send(0x05, tx_data, 8);
        HAL_Delay(99);  // 调整延时保持总延时100ms
    }
    
    // 确保最终到达目标位置
    tx_data[2] = SERVO_MIN;
    tx_data[3] = servo2_id4;
    FDCAN_Send(0x04, tx_data, 8);
    HAL_Delay(1);
    
    tx_data[2] = SERVO_MAX;
    tx_data[3] = servo2_id5;
    FDCAN_Send(0x05, tx_data, 8);
    HAL_Delay(1);
    
    // 在舵机到达目标位置后发送停止信号到0x01
    uint8_t stop_data[8] = {0x01, 0x00, SERVO_ZERO, SERVO_ZERO, 0x00, 0x00, 0x00, 0x00};
    FDCAN_Send(0x01, stop_data, 8);
    FDCAN_Send(0x08, stop_data, 8);

    HAL_Delay(1000);  // 延迟1秒

    // 新增部分：ID 0x06和ID 0x03的同步控制
    // 向ID 0x08发送特定数据（开始时）
    uint8_t id8_start[8] = {0x00, 0x90, SERVO_ZERO, SERVO_ZERO, 0x00, 0x00, 0x00, 0x00};
    FDCAN_Send(0x08, id8_start, 8);
    FDCAN_Send(0x01, id8_start, 8); // ID 0x06也发送开始信号
    
    // 分K步执行，ID 0x06和ID 0x03同步运动
    for(int i = 0; i < K; i++) {
        servo2_id6 += step_id6;  // ID 0x06的servo2从SERVO_ZERO到0x6E
        servo2_id3 += step_id3;  // ID 0x03的servo2从SERVO_ZERO到0xBE
        
        // 发送ID 0x06的控制数据
        tx_data[2] = servo1_id6;  // servo1保持在SERVO_ZERO
        tx_data[3] = servo2_id6;  // servo2逐步增加到0x6E
        FDCAN_Send(0x06, tx_data, 8);
        HAL_Delay(1);  // 短暂延时
        
        // 发送ID 0x03的控制数据
        tx_data[2] = servo1_id3;  // servo1保持在SERVO_ZERO
        tx_data[3] = servo2_id3;  // servo2逐步增加到0xBE
        FDCAN_Send(0x03, tx_data, 8);
        HAL_Delay(99);  // 调整延时保持总延时100ms
    }
    
    // 确保ID 0x06和ID 0x03最终到达目标位置
    tx_data[2] = SERVO_ZERO;  // servo1保持在SERVO_ZERO
    tx_data[3] = 0x6E;        // servo2到达0x6E
    FDCAN_Send(0x06, tx_data, 8);
    HAL_Delay(1);
    
    tx_data[2] = SERVO_ZERO;  // servo1保持在SERVO_ZERO
    tx_data[3] = 0xBE;        // servo2到达0xBE
    FDCAN_Send(0x03, tx_data, 8);
    HAL_Delay(1);
    
    // ID 0x06和ID 0x03到达目标位置后向0x08发送停止信号
    uint8_t id8_stop[8] = {0x00, 0x00, SERVO_ZERO, SERVO_ZERO, 0x00, 0x00, 0x00, 0x00};
    FDCAN_Send(0x08, id8_stop, 8);
    HAL_Delay(1);  // 短暂延时，确保信号发送完成
    FDCAN_Send(0x01, id8_stop, 8); // ID 0x06也发送停止信号
    
    HAL_Delay(1000);  // 延迟1秒

    // 第三步：ID 0x08和ID 0x01的同步控制
    // 分P步执行，ID 0x08的servo1从SERVO_ZERO到SERVO_MIN，ID 0x01的servo1从SERVO_ZERO到SERVO_MAX
    for(int i = 0; i < P; i++) {
        servo1_id8 += step_id8;  // ID 0x08的servo1从SERVO_ZERO到SERVO_MIN
        servo1_id1 += step_id1;  // ID 0x01的servo1从SERVO_ZERO到SERVO_MAX
        
        // 发送ID 0x08的控制数据
        tx_data[2] = servo1_id8;  // servo1逐步增加到SERVO_MIN
        tx_data[3] = servo2_id8;  // servo2保持在SERVO_ZERO
        FDCAN_Send(0x08, tx_data, 8);
        HAL_Delay(1);  // 短暂延时
        
        // 发送ID 0x01的控制数据
        tx_data[2] = servo1_id1;  // servo1逐步增加到SERVO_MAX
        tx_data[3] = servo2_id1;  // servo2保持在SERVO_ZERO
        FDCAN_Send(0x01, tx_data, 8);
        HAL_Delay(99);  // 调整延时保持总延时100ms
    }
    
    // 确保ID 0x08和ID 0x01最终到达目标位置
    tx_data[2] = SERVO_MIN;   // servo1到达SERVO_MIN
    tx_data[3] = SERVO_ZERO;  // servo2保持在SERVO_ZERO
    FDCAN_Send(0x08, tx_data, 8);
    HAL_Delay(1);
    
    tx_data[2] = SERVO_MAX;   // servo1到达SERVO_MAX
    tx_data[3] = SERVO_ZERO;  // servo2保持在SERVO_ZERO
    FDCAN_Send(0x01, tx_data, 8);
    HAL_Delay(1);
    
    HAL_Delay(1000);  // 最终延时

    tx_data[2] = SERVO_ZERO;
    tx_data[3] = 0x6E;
    FDCAN_Send(0x02, tx_data, 8); // 确保ID 0x06的servo1保持在SERVO_ZERO
    HAL_Delay(1);
    tx_data[2] = SERVO_ZERO;
    tx_data[3] = 0xBE;
    FDCAN_Send(0x07, tx_data, 8); // 确保ID 0x03的servo1保持在SERVO_ZERO
    HAL_Delay(1000);

    // 重新设置servo2的当前位置
    servo2_id3 = 0xBE;  // ID 0x03的servo2当前在0xBE
    servo2_id6 = 0x6E;  // ID 0x06的servo2当前在0x6E

    start_data[1] = 0xEE; // 启动信号
    start_data[2] = SERVO_MAX;  // servo1保持在SERVO_MAX（当前位置）
    start_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO（当前位置）
    FDCAN_Send(0x01, start_data, 8); // 向ID 0x01发送开始信号
    start_data[2] = SERVO_MIN;  // servo1保持在SERVO_MIN（当前位置）
    start_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO（当前位置）
    FDCAN_Send(0x08, start_data, 8); // 向ID 0x08发送开始信号
    
    // 分Q步执行，ID 0x03和ID 0x06的servo2从当前位置到SERVO_ZERO
    for(int i = 0; i < Q; i++) {
        servo2_id3 += step_id3_final;  // ID 0x03的servo2从0xBE逐步减少到SERVO_ZERO
        servo2_id6 += step_id6_final;  // ID 0x06的servo2从0x6E逐步减少到SERVO_ZERO
        
        // 发送ID 0x03的控制数据
        tx_data[2] = SERVO_ZERO;  // servo1保持在SERVO_ZERO
        tx_data[3] = servo2_id3;  // servo2逐步减少到SERVO_ZERO
        FDCAN_Send(0x03, tx_data, 8);
        HAL_Delay(1);  // 短暂延时
        
        // 发送ID 0x06的控制数据
        tx_data[2] = SERVO_ZERO;  // servo1保持在SERVO_ZERO
        tx_data[3] = servo2_id6;  // servo2逐步减少到SERVO_ZERO
        FDCAN_Send(0x06, tx_data, 8);
        HAL_Delay(99);  // 调整延时保持总延时100ms
    }
    
    // 确保ID 0x03和ID 0x06最终到达目标位置
    tx_data[2] = SERVO_ZERO;  // servo1保持在SERVO_ZERO
    tx_data[3] = SERVO_ZERO;  // servo2到达SERVO_ZERO
    FDCAN_Send(0x03, tx_data, 8);
    HAL_Delay(1);
    
    tx_data[2] = SERVO_ZERO;  // servo1保持在SERVO_ZERO
    tx_data[3] = SERVO_ZERO;  // servo2到达SERVO_ZERO
    FDCAN_Send(0x06, tx_data, 8);
    HAL_Delay(1);

    stop_data[2] = SERVO_MAX;  // servo1保持在SERVO_MAX（当前位置）
    stop_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO（当前位置）
    FDCAN_Send(0x01, stop_data, 8); // 向ID 0x01发送停止信号
    stop_data[2] = SERVO_MIN;  // servo1保持在SERVO_MIN（当前位置）
    stop_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO（当前位置）
    FDCAN_Send(0x08, stop_data, 8); // 向ID 0x08发送停止信号

    HAL_Delay(1000);

    // 最后一步：ID 0x02和ID 0x07的servo2同时分步到达SERVO_ZERO
    // 初始化ID 0x02和ID 0x07的servo2当前位置
    int servo2_id2 = 0x6E;  // ID 0x02的servo2当前在0x6E
    int servo2_id7 = 0xBE;  // ID 0x07的servo2当前在0xBE
    
    // 设置最后一步的步数
    int R = 25;  // ID 0x02和ID 0x07的总步数，可以根据需要调整
    int F = 15;  // ID 0x02先走的步数，可以根据需要调整
    
    // 计算每步的增量
    int step_id2_final = (SERVO_ZERO - 0x6E) / R;  // ID 0x02的servo2从0x6E到SERVO_ZERO
    int step_id7_final = (SERVO_ZERO - 0xBE) / R;  // ID 0x07的servo2从0xBE到SERVO_ZERO

    // 第一阶段：只有ID 0x07运动，启动ID 0x08对应的电机
    start_data[1] = 0xF9; // 启动信号
    start_data[2] = SERVO_MIN;  // ID 0x08对应的servo1保持在SERVO_MIN（当前位置）
    start_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO（当前位置）
    FDCAN_Send(0x08, start_data, 8); // 向ID 0x08发送开始信号（启动对应电机）

    // ID 0x07先走F步
    for(int i = 0; i < F; i++) {
        servo2_id7 += step_id7_final;  // ID 0x07的servo2从0xBE逐步减少到SERVO_ZERO
        
        // 发送ID 0x07的控制数据
        tx_data[2] = SERVO_ZERO;  // servo1保持在SERVO_ZERO
        tx_data[3] = servo2_id7;  // servo2逐步减少到SERVO_ZERO
        FDCAN_Send(0x07, tx_data, 8);
        HAL_Delay(100);  // 延时
    }

    // 第二阶段：ID 0x07走F步后停止ID 0x08电机，启动ID 0x01电机，ID 0x02开始运动
    // 停止ID 0x08对应的电机
    stop_data[1] = 0x00; // 停止信号
    stop_data[2] = SERVO_MIN;  // servo1保持在SERVO_MIN
    stop_data[3] = SERVO_ZERO; // servo2保持在当前位置
    FDCAN_Send(0x08, stop_data, 8); // 向ID 0x08发送停止信号（停止对应电机）
    
    // 启动ID 0x01对应的电机
    start_data[1] = 0xF9; // 启动信号
    start_data[2] = SERVO_MAX;  // ID 0x01对应的servo1保持在SERVO_MAX（当前位置）
    start_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO（当前位置）
    FDCAN_Send(0x01, start_data, 8); // 向ID 0x01发送开始信号（启动对应电机）

    // ID 0x02走剩下的(R-F)步，ID 0x07走剩下的(R-F)步到达最终目标
    for(int i = F; i < R; i++) {
        servo2_id2 += step_id2_final;  // ID 0x02的servo2从0x6E逐步减少到SERVO_ZERO
        servo2_id7 += step_id7_final;  // ID 0x07继续到SERVO_ZERO
        
        // 发送ID 0x02的控制数据
        tx_data[2] = SERVO_ZERO;  // servo1保持在SERVO_ZERO
        tx_data[3] = servo2_id2;  // servo2逐步减少到SERVO_ZERO
        FDCAN_Send(0x02, tx_data, 8);
        HAL_Delay(1);  // 短暂延时
        
        // 发送ID 0x07的控制数据（虽然对应电机已停，但位置需要更新）
        tx_data[2] = SERVO_ZERO;  // servo1保持在SERVO_ZERO
        tx_data[3] = servo2_id7;  // servo2逐步减少到SERVO_ZERO
        FDCAN_Send(0x07, tx_data, 8);
        HAL_Delay(99);  // 调整延时保持总延时100ms
    }
    
    // 确保ID 0x02和ID 0x07最终到达目标位置
    tx_data[2] = SERVO_ZERO;  // servo1保持在SERVO_ZERO
    tx_data[3] = SERVO_ZERO;  // servo2到达SERVO_ZERO
    FDCAN_Send(0x02, tx_data, 8);
    HAL_Delay(1);
    
    tx_data[2] = SERVO_ZERO;  // servo1保持在SERVO_ZERO
    tx_data[3] = SERVO_ZERO;  // servo2到达SERVO_ZERO
    FDCAN_Send(0x07, tx_data, 8);
    HAL_Delay(1);

    // 停止ID 0x01对应的电机
    stop_data[1] = 0x00; // 停止信号
    stop_data[2] = SERVO_MAX;  // servo1保持在SERVO_MAX
    stop_data[3] = SERVO_ZERO; // servo2保持在最终位置
    FDCAN_Send(0x01, stop_data, 8); // 向ID 0x01发送停止信号（停止对应电机）

    HAL_Delay(1000);  // 最终延时

    // 最后：设置ID 0x04和ID 0x05的servo2到指定位置（不需要分步）
    tx_data[2] = SERVO_MIN;  // servo1保持在SERVO_MIN（当前位置）
    tx_data[3] = 0x82;       // servo2转到0x82
    FDCAN_Send(0x04, tx_data, 8);
    HAL_Delay(1);
    
    tx_data[2] = SERVO_MAX;  // servo1保持在SERVO_MAX（当前位置）
    tx_data[3] = 0xAA;       // servo2转到0xAA
    FDCAN_Send(0x05, tx_data, 8);
    HAL_Delay(1);

    // // 继续设置ID 0x02和ID 0x07的servo2到指定位置（不需要分步）
    // tx_data[2] = SERVO_ZERO; // servo1保持在SERVO_ZERO（当前位置）
    // tx_data[3] = 0x82;       // servo2转到0x82
    // FDCAN_Send(0x02, tx_data, 8);
    // HAL_Delay(1);
    // FDCAN_Send(0x03, tx_data, 8); // ID 0x03也发送相同数据
    // HAL_Delay(1);
    
    // tx_data[2] = SERVO_ZERO; // servo1保持在SERVO_ZERO（当前位置）
    // tx_data[3] = 0xAA;       // servo2转到0xAA
    // FDCAN_Send(0x07, tx_data, 8);
    // HAL_Delay(1);
    // FDCAN_Send(0x06, tx_data, 8); // ID 0x06也发送相同数据
    // HAL_Delay(1);

    // HAL_Delay(1000);  // 延时

    // 最后：启动ID 0x01和ID 0x08电机像第一步中那样旋转，维持1秒
    start_data[1] = 0xF9; // 启动信号（和第一步相同）
    start_data[2] = SERVO_MAX;  // servo1保持在SERVO_MAX（当前位置）
    start_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO（当前位置）
    FDCAN_Send(0x01, start_data, 8); // 向ID 0x01发送开始信号
    
    start_data[2] = SERVO_MIN;  // servo1保持在SERVO_MIN（当前位置）
    start_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO（当前位置）
    FDCAN_Send(0x08, start_data, 8); // 向ID 0x08发送开始信号
    
    HAL_Delay(1000);  // 维持1秒
    
    // 停止ID 0x01和ID 0x08电机
    stop_data[1] = 0x00; // 停止信号
    stop_data[2] = SERVO_MAX;  // servo1保持在SERVO_MAX
    stop_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x01, stop_data, 8); // 向ID 0x01发送停止信号
    
    stop_data[2] = SERVO_MIN;  // servo1保持在SERVO_MIN
    stop_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x08, stop_data, 8); // 向ID 0x08发送停止信号

    HAL_Delay(200);  // 最终延时

    // 最后：让ID 0x04和ID 0x05的servo2回到SERVO_ZERO
    tx_data[2] = SERVO_MIN;  // servo1保持在SERVO_MIN（当前位置）
    tx_data[3] = SERVO_ZERO; // servo2回到SERVO_ZERO
    FDCAN_Send(0x04, tx_data, 8);
    HAL_Delay(1);
    
    tx_data[2] = SERVO_MAX;  // servo1保持在SERVO_MAX（当前位置）
    tx_data[3] = SERVO_ZERO; // servo2回到SERVO_ZERO
    FDCAN_Send(0x05, tx_data, 8);
    HAL_Delay(1);

    HAL_Delay(1000);  // 最终延时

    // 记录LINE_TO_CAR执行完毕后各ID舵机的最终位置到全局变量
    g_servo1_id1_final = SERVO_MAX;   // ID 0x01的servo1最终位置
    g_servo2_id1_final = SERVO_ZERO;  // ID 0x01的servo2最终位置
    g_servo1_id2_final = SERVO_ZERO;  // ID 0x02的servo1最终位置
    g_servo2_id2_final = SERVO_ZERO;  // ID 0x02的servo2最终位置
    g_servo1_id3_final = SERVO_ZERO;  // ID 0x03的servo1最终位置
    g_servo2_id3_final = SERVO_ZERO;  // ID 0x03的servo2最终位置
    g_servo1_id4_final = SERVO_MIN;   // ID 0x04的servo1最终位置
    g_servo2_id4_final = SERVO_ZERO;  // ID 0x04的servo2最终位置
    g_servo1_id5_final = SERVO_MAX;   // ID 0x05的servo1最终位置
    g_servo2_id5_final = SERVO_ZERO;  // ID 0x05的servo2最终位置
    g_servo1_id6_final = SERVO_ZERO;  // ID 0x06的servo1最终位置
    g_servo2_id6_final = SERVO_ZERO;  // ID 0x06的servo2最终位置
    g_servo1_id7_final = SERVO_ZERO;  // ID 0x07的servo1最终位置
    g_servo2_id7_final = SERVO_ZERO;  // ID 0x07的servo2最终位置
    g_servo1_id8_final = SERVO_MIN;   // ID 0x08的servo1最终位置
    g_servo2_id8_final = SERVO_ZERO;  // ID 0x08的servo2最终位置

}

void CAR_TO_LINE(void){
    uint8_t tx_data[8] = {0};
    uint8_t start_data[8] = {0};
    uint8_t stop_data[8] = {0};

    // 启动ID 0x01和ID 0x08电机
    start_data[0] = 0x00; // 保留字节
    start_data[1] = 0xF9; // 启动信号
    start_data[2] = 0xE6;  // servo1保持在SERVO_MAX
    start_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x01, start_data, 8);

    start_data[2] = 0x46;  // servo1保持在SERVO_MIN
    start_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x08, start_data, 8);

    HAL_Delay(500);  // 维持1秒

    tx_data[2] = SERVO_ZERO; // servo1保持在SERVO_ZERO
    tx_data[3] = 0x64; // servo2保持在0x64
    FDCAN_Send(0x02, tx_data, 8); // 向ID 0x02发送数据
    FDCAN_Send(0x06, tx_data, 8); // 向ID 0x06发送数据
    HAL_Delay(1);
    tx_data[3] = 0xC8; // servo2保持在0xC8
    FDCAN_Send(0x03, tx_data, 8); // 向ID 0x07发送数据
    FDCAN_Send(0x07, tx_data, 8); // 向ID 0x07发送数据
    HAL_Delay(500);  // 维持500毫秒

    // 停止ID 0x01和ID 0x08电机
    stop_data[1] = 0x00; // 停止信号
    stop_data[2] = SERVO_MAX;  // servo1保持在SERVO_MAX
    stop_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x01, stop_data, 8);

    stop_data[2] = SERVO_MIN;  // servo1保持在SERVO_MIN
    stop_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x08, stop_data, 8);


    HAL_Delay(200);  // 最终延时

    tx_data[0] = 0x00; // 保留字节
    tx_data[1] = 0x00; // 保留字节
    tx_data[2] = SERVO_ZERO; // servo1保持在SERVO_ZERO
    tx_data[3] = SERVO_ZERO; // servo2保持在0x6E
    FDCAN_Send(0x01, tx_data, 8); // 向ID 0x04发送数据
    HAL_Delay(1);
    FDCAN_Send(0x08, tx_data, 8); // 向ID 0x05发送数据
    HAL_Delay(1);

    tx_data[2] = SERVO_ZERO; // servo1保持在SERVO_ZERO
    tx_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x02, tx_data, 8); // 向ID 0x02发送数据
    HAL_Delay(1);
    FDCAN_Send(0x07, tx_data, 8); // 向ID 0x07发送数据
    HAL_Delay(100);  // 延时100毫秒

    start_data[0] = 0x00; // 保留字节
    start_data[1] = 0xF9; // 启动信号
    start_data[2] = SERVO_ZERO;  // servo1保持在SERVO_ZERO
    start_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x01, start_data, 8);

    FDCAN_Send(0x08, start_data, 8); // 向ID 0x08发送开始信号
    
    // 让ID 0x04和ID 0x05的servo1分步回到SERVO_ZERO
    int servo1_id4_current = SERVO_MIN;  // ID 0x04的servo1当前位置
    int servo1_id5_current = SERVO_MAX;  // ID 0x05的servo1当前位置
    
    // 设置步数
    int N_back = 20;  // 回零的步数
    
    // 计算每步的增量
    int step_id4_back = (SERVO_ZERO - SERVO_MIN) / N_back;   // ID 0x04从SERVO_MIN到SERVO_ZERO的步长
    int step_id5_back = (SERVO_ZERO - SERVO_MAX) / N_back;   // ID 0x05从SERVO_MAX到SERVO_ZERO的步长
    
    // 分N_back步执行，ID 0x04和ID 0x05的servo1同步回到SERVO_ZERO
    for(int i = 0; i < N_back; i++) {
        tx_data[0] = 0x00; // 保留字节
        tx_data[1] = 0x00; // 保留字节
        servo1_id4_current += step_id4_back;  // ID 0x04的servo1逐步回到SERVO_ZERO
        servo1_id5_current += step_id5_back;  // ID 0x05的servo1逐步回到SERVO_ZERO
        
        // 发送ID 0x04的控制数据
        tx_data[2] = servo1_id4_current;  // servo1逐步回到SERVO_ZERO
        tx_data[3] = SERVO_ZERO;          // servo2保持在SERVO_ZERO
        FDCAN_Send(0x04, tx_data, 8);
        HAL_Delay(1);  // 短暂延时
        
        // 发送ID 0x05的控制数据
        tx_data[2] = servo1_id5_current;  // servo1逐步回到SERVO_ZERO
        tx_data[3] = SERVO_ZERO;          // servo2保持在SERVO_ZERO
        FDCAN_Send(0x05, tx_data, 8);
        HAL_Delay(99);  // 调整延时保持总延时100ms
    }
    
    // 确保ID 0x04和ID 0x05最终到达SERVO_ZERO
    tx_data[2] = SERVO_ZERO;  // servo1到达SERVO_ZERO
    tx_data[3] = SERVO_ZERO;  // servo2保持在SERVO_ZERO
    FDCAN_Send(0x04, tx_data, 8);
    HAL_Delay(1);
    
    tx_data[2] = SERVO_ZERO;  // servo1到达SERVO_ZERO
    tx_data[3] = SERVO_ZERO;  // servo2保持在SERVO_ZERO
    FDCAN_Send(0x05, tx_data, 8);
    HAL_Delay(1);
    
    // 停止ID 0x01和ID 0x08电机
    stop_data[1] = 0x00; // 停止信号
    stop_data[2] = SERVO_ZERO;  // servo1保持在SERVO_ZERO
    stop_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x01, stop_data, 8);
    FDCAN_Send(0x08, stop_data, 8);
    
    HAL_Delay(100);  // 延时确保停止信号处理完成
    
    // 最后确保每个ID的两个舵机都是SERVO_ZERO
    uint8_t final_data[8] = {0x00, 0x00, SERVO_ZERO, SERVO_ZERO, 0x00, 0x00, 0x00, 0x00};
    
    // 依次向每个ID发送舵机归零指令
    FDCAN_Send(0x01, final_data, 8);  // ID 0x01的两个舵机都归零
    FDCAN_Send(0x02, final_data, 8);  // ID 0x02的两个舵机都归零
    FDCAN_Send(0x03, final_data, 8);  // ID 0x03的两个舵机都归零
    FDCAN_Send(0x04, final_data, 8);  // ID 0x04的两个舵机都归零
    FDCAN_Send(0x05, final_data, 8);  // ID 0x05的两个舵机都归零
    FDCAN_Send(0x06, final_data, 8);  // ID 0x06的两个舵机都归零
    FDCAN_Send(0x07, final_data, 8);  // ID 0x07的两个舵机都归零
    FDCAN_Send(0x08, final_data, 8);  // ID 0x08的两个舵机都归零

    g_servo1_id1_final = SERVO_ZERO;   // ID 0x01的servo1最终位置
    g_servo2_id1_final = SERVO_ZERO;  // ID 0x01的servo2最终位置
    g_servo1_id2_final = SERVO_MAX;  // ID 0x02的servo1最终位置
    g_servo2_id2_final = SERVO_ZERO;  // ID 0x02的servo2最终位置
    g_servo1_id3_final = SERVO_MIN;  // ID 0x03的servo1最终位置
    g_servo2_id3_final = SERVO_ZERO;  // ID 0x03的servo2最终位置
    g_servo1_id4_final = SERVO_ZERO;   // ID 0x04的servo1最终位置
    g_servo2_id4_final = SERVO_ZERO;  // ID 0x04的servo2最终位置
    g_servo1_id5_final = SERVO_ZERO;   // ID 0x05的servo1最终位置
    g_servo2_id5_final = SERVO_ZERO;  // ID 0x05的servo2最终位置
    g_servo1_id6_final = SERVO_MIN;  // ID 0x06的servo1最终位置
    g_servo2_id6_final = SERVO_ZERO;  // ID 0x06的servo2最终位置
    g_servo1_id7_final = SERVO_MAX;  // ID 0x07的servo1最终位置
    g_servo2_id7_final = SERVO_ZERO;  // ID 0x07的servo2最终位置
    g_servo1_id8_final = SERVO_ZERO;   // ID 0x08的servo1最终位置
    g_servo2_id8_final = SERVO_ZERO;  // ID 0x08的servo2最终位置

}


void CAR_TO_SCAN(void){
    uint8_t tx_data[8] = {0};
    uint8_t start_data[8] = {0};
    uint8_t stop_data[8] = {0};

    uint8_t servo_scan_id2id7 = 0xC8; // ID 0x02和ID 0x07的servo2初始位置
    uint8_t servo_scan_id3id6 = 0x64; // ID 0x03和ID 0x06的servo2初始位置

    tx_data[0] = 0x00; // 保留字节
    tx_data[1] = 0x00; // 保留字节
    tx_data[2] = SERVO_MAX; // servo1保持在SERVO_MAX
    tx_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x02, tx_data, 8); // 向ID 0x02发送数据
    FDCAN_Send(0x07, tx_data, 8); // 向ID 0x07发送数据
    HAL_Delay(1);
    tx_data[2] = SERVO_MIN; // servo1保持在SERVO_MIN
    tx_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x03, tx_data, 8); // 向ID 0x03发送数据
    FDCAN_Send(0x06, tx_data, 8); // 向ID 0x06发送数据
    HAL_Delay(500);  // 维持500毫秒

    start_data[0] = 0x01; // 保留字节
    start_data[1] = 0xF9; // 启动信号
    start_data[2] = SERVO_MAX;  // servo1保持在SERVO_MAX
    start_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x01, start_data, 8); // 向ID 0x01发送开始信号
    start_data[2] = SERVO_MIN;  // servo1保持在SERVO_MIN
    start_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x08, start_data, 8); // 向ID 0x08发送开始信号

    start_data[0] = 0x00; // 保留字节
    FDCAN_Send(0x04, start_data, 8); // 向ID 0x04发送开始信号
    start_data[2] = SERVO_MAX;  // servo1保持在SERVO_MAX
    FDCAN_Send(0x05, start_data, 8); // 向ID 0x05发送开始信号
    HAL_Delay(100);  // 维持100毫秒

    tx_data[0] = 0x00; // 保留字节
    tx_data[1] = 0x00; // 保留字节
    tx_data[2] = SERVO_MAX; // servo1保持在SERVO_ZERO
    tx_data[3] = servo_scan_id2id7; // servo2保持在0xC8
    FDCAN_Send(0x02, tx_data, 8); // 向ID 0x02发送数据
    FDCAN_Send(0x07, tx_data, 8); // 向ID 0x07发送数据
    HAL_Delay(1);
    tx_data[2] = SERVO_MIN; // servo1保持在SERVO_ZERO
    tx_data[3] = servo_scan_id3id6; // servo2保持在0xA8
    FDCAN_Send(0x03, tx_data, 8); // 向 ID 0x03发送数据
    FDCAN_Send(0x06, tx_data, 8); // 向 ID 0x06发送数据
    HAL_Delay(1);  // 维持500毫秒

    stop_data[0] = 0x00; // 保留字节
    stop_data[1] = 0x00; // 停止信号
    stop_data[2] = SERVO_MAX;  // servo1保持在SERVO_MAX
    stop_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x01, stop_data, 8); // 向ID 0x01发送停止信号
    FDCAN_Send(0x05, stop_data, 8); // 向ID 0x05发送停止信号
    stop_data[2] = SERVO_MIN;  // servo1保持在SERVO_MIN
    stop_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x08, stop_data, 8); // 向ID 0x08发送停止信号
    FDCAN_Send(0x04, stop_data, 8); // 向ID 0x04发送停止信号
    HAL_Delay(200);  // 最终延时


    g_servo1_id1_final = SERVO_MAX;   // ID 0x01的servo1最终位置
    g_servo2_id1_final = SERVO_ZERO;  // ID 0x01的servo2最终位置
    g_servo1_id2_final = SERVO_MAX;  // ID 0x02的servo1最终位置
    g_servo2_id2_final = 0xC8;  // ID 0x02的servo2最终位置
    g_servo1_id3_final = SERVO_MIN;  // ID 0x03的servo1最终位置
    g_servo2_id3_final = 0x64;  // ID 0x03的servo2最终位置
    g_servo1_id4_final = SERVO_MIN;   // ID 0x04的servo1最终位置
    g_servo2_id4_final = SERVO_ZERO;  // ID 0x04的servo2最终位置
    g_servo1_id5_final = SERVO_MAX;   // ID 0x05的servo1最终位置
    g_servo2_id5_final = SERVO_ZERO;  // ID 0x05的servo2最终位置
    g_servo1_id6_final = SERVO_MIN;  // ID 0x06的servo1最终位置
    g_servo2_id6_final = 0x64;  // ID 0x06的servo2最终位置
    g_servo1_id7_final = SERVO_MAX;  // ID 0x07的servo1最终位置
    g_servo2_id7_final = 0xC8;  // ID 0x07的servo2最终位置
    g_servo1_id8_final = SERVO_MIN;   // ID 0x08的servo1最终位置
    g_servo2_id8_final = SERVO_ZERO;  // ID 0x08的servo2最终位置


}

void SCAN_TO_CAR(void){
    uint8_t tx_data[8] = {0};
    tx_data[0] = 0x00; // 保留字节
    tx_data[1] = 0x00; // 保留字节
    tx_data[2] = SERVO_MAX; // servo1保持在SERVO_MAX
    tx_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x02, tx_data, 8); // 向ID 0x02发送数据
    FDCAN_Send(0x07, tx_data, 8); // 向ID 0x07发送数据
    HAL_Delay(1);
    tx_data[2] = SERVO_MIN; // servo1保持在SERVO_MIN
    tx_data[3] = SERVO_ZERO; // servo2保持在SERVO_ZERO
    FDCAN_Send(0x03, tx_data, 8); // 向ID 0x03发送数据
    FDCAN_Send(0x06, tx_data, 8); // 向ID 0x06发送数据
    HAL_Delay(500);  // 维持500毫秒

    tx_data[2] = SERVO_ZERO; // servo1保持在SERVO_ZERO
    FDCAN_Send(0x02, tx_data, 8); // 向ID 0x02发送数据
    FDCAN_Send(0x07, tx_data, 8); // 向ID 0x07发送数据
    FDCAN_Send(0x03, tx_data, 8); // 向ID 0x03发送数据
    FDCAN_Send(0x06, tx_data, 8); // 向ID 0x06发送数据
    HAL_Delay(500);  // 短暂延时

    // 记录LINE_TO_CAR执行完毕后各ID舵机的最终位置到全局变量
    g_servo1_id1_final = SERVO_MAX;   // ID 0x01的servo1最终位置
    g_servo2_id1_final = SERVO_ZERO;  // ID 0x01的servo2最终位置
    g_servo1_id2_final = SERVO_ZERO;  // ID 0x02的servo1最终位置
    g_servo2_id2_final = SERVO_ZERO;  // ID 0x02的servo2最终位置
    g_servo1_id3_final = SERVO_ZERO;  // ID 0x03的servo1最终位置
    g_servo2_id3_final = SERVO_ZERO;  // ID 0x03的servo2最终位置
    g_servo1_id4_final = SERVO_MIN;   // ID 0x04的servo1最终位置
    g_servo2_id4_final = SERVO_ZERO;  // ID 0x04的servo2最终位置
    g_servo1_id5_final = SERVO_MAX;   // ID 0x05的servo1最终位置
    g_servo2_id5_final = SERVO_ZERO;  // ID 0x05的servo2最终位置
    g_servo1_id6_final = SERVO_ZERO;  // ID 0x06的servo1最终位置
    g_servo2_id6_final = SERVO_ZERO;  // ID 0x06的servo2最终位置
    g_servo1_id7_final = SERVO_ZERO;  // ID 0x07的servo1最终位置
    g_servo2_id7_final = SERVO_ZERO;  // ID 0x07的servo2最终位置
    g_servo1_id8_final = SERVO_MIN;   // ID 0x08的servo1最终位置
    g_servo2_id8_final = SERVO_ZERO;  // ID 0x08的servo2最终位置

}

void LINE_TO_SCAN(void){
    LINE_TO_CAR(); // 先执行LINE_TO_CAR函数
    HAL_Delay(500); // 等待1秒，确保LINE_TO_CAR完成
    SCAN_TO_CAR(); // 然后执行SCAN_TO_CAR函数

    g_servo1_id1_final = SERVO_MAX;   // ID 0x01的servo1最终位置
    g_servo2_id1_final = SERVO_ZERO;  // ID 0x01的servo2最终位置
    g_servo1_id2_final = SERVO_MAX;  // ID 0x02的servo1最终位置
    g_servo2_id2_final = 0xC8;  // ID 0x02的servo2最终位置
    g_servo1_id3_final = SERVO_MIN;  // ID 0x03的servo1最终位置
    g_servo2_id3_final = 0x64;  // ID 0x03的servo2最终位置
    g_servo1_id4_final = SERVO_MIN;   // ID 0x04的servo1最终位置
    g_servo2_id4_final = SERVO_ZERO;  // ID 0x04的servo2最终位置
    g_servo1_id5_final = SERVO_MAX;   // ID 0x05的servo1最终位置
    g_servo2_id5_final = SERVO_ZERO;  // ID 0x05的servo2最终位置
    g_servo1_id6_final = SERVO_MIN;  // ID 0x06的servo1最终位置
    g_servo2_id6_final = 0x64;  // ID 0x06的servo2最终位置
    g_servo1_id7_final = SERVO_MAX;  // ID 0x07的servo1最终位置
    g_servo2_id7_final = 0xC8;  // ID 0x07的servo2最终位置
    g_servo1_id8_final = SERVO_MIN;   // ID 0x08的servo1最终位置
    g_servo2_id8_final = SERVO_ZERO;  // ID 0x08的servo2最终位置

}

void SCAN_TO_LINE(void){
    SCAN_TO_CAR(); // 先执行SCAN_TO_CAR函数
    HAL_Delay(500); // 等待1秒，确保SCAN_TO_CAR完成
    CAR_TO_LINE(); // 然后执行CAR_TO_LINE函数

    g_servo1_id1_final = SERVO_ZERO;   // ID 0x01的servo1最终位置
    g_servo2_id1_final = SERVO_ZERO;  // ID 0x01的servo2最终位置
    g_servo1_id2_final = SERVO_ZERO;  // ID 0x02的servo1最终位置
    g_servo2_id2_final = SERVO_ZERO;  // ID 0x02的servo2最终位置
    g_servo1_id3_final = SERVO_ZERO;  // ID 0x03的servo1最终位置
    g_servo2_id3_final = SERVO_ZERO;  // ID 0x03的servo2最终位置
    g_servo1_id4_final = SERVO_ZERO;   // ID 0x04的servo1最终位置
    g_servo2_id4_final = SERVO_ZERO;  // ID 0x04的servo2最终位置
    g_servo1_id5_final = SERVO_ZERO;   // ID 0x05的servo1最终位置
    g_servo2_id5_final = SERVO_ZERO;  // ID 0x05的servo2最终位置
    g_servo1_id6_final = SERVO_ZERO;  // ID 0x06的servo1最终位置
    g_servo2_id6_final = SERVO_ZERO;  // ID 0x06的servo2最终位置
    g_servo1_id7_final = SERVO_ZERO;  // ID 0x07的servo1最终位置
    g_servo2_id7_final = SERVO_ZERO;  // ID 0x07的servo2最终位置
    g_servo1_id8_final = SERVO_ZERO;   // ID 0x08的servo1最终位置
    g_servo2_id8_final = SERVO_ZERO;  // ID 0x08的servo2最终位置

}
