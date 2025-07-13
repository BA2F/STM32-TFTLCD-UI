#include "task_automode.h"
#include "lcd.h"
#include "task_main.h" // 包含主任务状态定义

// 使用头文件中定义的阈值常量
#define DARK_MODE_THRESHOLD DARK_THRESHOLD
#define LIGHT_MODE_THRESHOLD LIGHT_THRESHOLD

// 显示模式枚举
typedef enum
{
    LIGHT_MODE,
    DARK_MODE,
    UNKNOWN_MODE
} DisplayMode;

// 模块内部状态
static DisplayMode current_mode = UNKNOWN_MODE;

// 外部声明主任务状态
extern CalendarState calendarState;

/**
 * @brief 初始化自动模式模块
 *
 * 该函数初始化自动模式模块的状态，并将LCD清屏为白色，设置当前模式为LIGHT_MODE。
 */
void automode_init(void)
{
    // 初始化状态
    current_mode = UNKNOWN_MODE;

    // 初始清屏为白色
    lcd_clear(WHITE);
    current_mode = LIGHT_MODE;
}

/**
 * @brief 自动模式任务处理函数
 *
 * 该函数根据ADC值判断当前环境亮度，并相应地切换LCD显示模式。
 * 仅在正常日历状态下处理自动模式。
 *
 * @param adc_val ADC读取的亮度值
 */
void automode_task(uint16_t adc_val)
{
    // 仅在正常日历状态下处理自动模式
    if (calendarState != CalendarState_Normal)
    {
        return;
    }

    // 处理模式切换
    if (adc_val > DARK_MODE_THRESHOLD && current_mode != DARK_MODE)
    {
        // 如果ADC值大于暗模式阈值且当前模式不是暗模式，则清屏为黑色，并设置当前模式为DARK_MODE
        lcd_clear(BLACK);
        current_mode = DARK_MODE;
    }
    else if (adc_val < LIGHT_MODE_THRESHOLD && current_mode != LIGHT_MODE)
    {
        // 如果ADC值小于亮模式阈值且当前模式不是亮模式，则清屏为白色，并设置当前模式为LIGHT_MODE
        lcd_clear(WHITE);
        current_mode = LIGHT_MODE;
    }

    // 中间区域（LIGHT_THRESHOLD-DARK_THRESHOLD）保持当前模式不变
}
