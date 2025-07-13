#ifndef __TASK_MAIN_H
#define __TASK_MAIN_H

#include <stdio.h>
#include <string.h>

#include "kk_rtc.h"
#include "KEY.h"
#include "lcd.h"
#include "usart.h"

/**
 * @brief ����״̬ö��
 */
typedef enum
{
    CalendarState_Normal, /**< ����ģʽ */
    CalendarState_Setting /**< ����ģʽ */
} CalendarState;

/**
 * @brief ��ʼ��������
 */
void MainTaskInit(void);

/**
 * @brief ִ��������
 */
void MainTask(void);

#endif
