#ifndef __TASK_MAIN_H
#define __TASK_MAIN_H

#include <stdio.h>
#include <string.h>

#include "kk_rtc.h"
#include "KEY.h"
#include "lcd.h"
#include "usart.h"

/**
 * @brief 日历状态枚举
 */
typedef enum
{
    CalendarState_Normal, /**< 正常模式 */
    CalendarState_Setting /**< 设置模式 */
} CalendarState;

/**
 * @brief 初始化主任务
 */
void MainTaskInit(void);

/**
 * @brief 执行主任务
 */
void MainTask(void);

#endif
