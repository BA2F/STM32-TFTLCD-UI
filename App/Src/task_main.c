/**
 * @file task_main.c
 * @brief 主任务文件，实现日历显示和时间设置功能
 */

#include "task_main.h"

/**
 * @brief 光标闪烁间隔时间(毫秒)
 */
#define CURSOR_FLASH_INTERVAL 500

/**
 * @brief 星期几的字符串数组
 */
char weeks[7][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

/**
 * @brief  声明一个外部变量 calendarState，其类型为 CalendarState，用于在不同文件间共享日历状态信息。
 */
extern CalendarState calendarState;

/**
 * @brief 设置状态枚举，表示当前正在设置的时间项
 */
typedef enum
{
	Year = 0,
	Month,
	Day,
	Hour,
	Minute,
	Second
} SettingState;

/**
 * @brief 光标位置结构体
 */
typedef struct
{
	uint16_t x1;
	uint16_t x2;
	uint16_t y1;
	uint16_t y2;
} CursorPosition;

/**
 * @brief 各时间项的光标位置数组
 */
CursorPosition cursorPosition[6] = {
	{117 + 0 * 8, 115, 117 + 4 * 8, 115},	// Year
	{117 + 5 * 8, 115, 117 + 7 * 8, 115},	// Month
	{116 + 8 * 8, 115, 116 + 10 * 8, 115},	// Day
	{95 + 0 * 12, 150, 95 + 2 * 12, 150},	// Hour
	{110 + 3 * 12, 150, 110 + 5 * 12, 150}, // Minute
	{120 + 6 * 12, 150, 120 + 8 * 12, 150}, // Second
};

/**
 * @brief 当前日历状态
 */
CalendarState calendarState = CalendarState_Normal;

/**
 * @brief 当前设置状态
 */
SettingState settingState = Year;

/**
 * @brief 设置时间结构体
 */
struct tm settingTime;

/**
 * @brief 设置按键
 */
KeyLiteHandle key0, key1, key2;

/**
 * @brief KEY0 按键回调函数
 * @details 该函数在 KEY0 按键被触发时执行。主要功能是在日历处于设置模式时，
 *          减少当前选中时间项的值。同时，它还会切换一个 GPIO 引脚的状态，
 *          并通过串口发送一个标识消息。
 */
void Key0KeyLiteTask()
{
	// 切换 GPIOB 的第 5 引脚状态（例如用于指示灯或调试）
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);

	// 定义一个用于串口传输的字符串消息
	char message[] = "KEY0";
	// 通过 UART1 发送该消息，用于调试或状态监控
	HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);

	// 检查当前日历状态是否为设置模式
	if (calendarState == CalendarState_Setting)
	{
		// 根据当前设置状态（选中的时间项）来减少相应的值
		switch (settingState)
		{
		case Year:
			// 减少年份值
			settingTime.tm_year--;
			// 如果年份小于最小值（这里假设70代表1970年），则设置为最大值
			if (settingTime.tm_year < 70)
			{
				settingTime.tm_year = 70;
			}
			break;
		case Month:
			// 减少月份值
			settingTime.tm_mon--;
			// 如果月份小于1，则设置为12（循环到上一年）
			if (settingTime.tm_mon < 1)
			{
				settingTime.tm_mon = 11; // 注意：tm_mon 是 0-11
			}
			break;
		case Day:
			// 减少日期值
			settingTime.tm_mday--;
			// 如果日期小于等于0，则设置为31（循环到上个月）
			if (settingTime.tm_mday <= 0)
			{
				settingTime.tm_mday = 31; // 注意：这里没有处理不同月份天数不同的情况
			}
			break;
		case Hour:
			// 减少小时值
			settingTime.tm_hour--;
			// 如果小时小于0，则设置为23（循环到前一天）
			if (settingTime.tm_hour < 0)
			{
				settingTime.tm_hour = 23;
			}
			break;
		case Minute:
			// 减少分钟值
			settingTime.tm_min--;
			// 如果分钟小于0，则设置为59（循环到上一小时）
			if (settingTime.tm_min < 0)
			{
				settingTime.tm_min = 59;
			}
			break;
		case Second:
			// 减少秒值
			settingTime.tm_sec--;
			// 如果秒小于0，则设置为59（循环到上一分钟）
			if (settingTime.tm_sec < 0)
			{
				settingTime.tm_sec = 59;
			}
			break;
		}
	}
}

/**
 * @brief KEY1 按键回调函数
 * @details 该函数在 KEY1 按键被触发时执行。主要功能是在日历处于设置模式时，
 *          增加当前选中时间项的值。同时，它还会切换一个 GPIO 引脚的状态，
 *          并通过串口发送一个标识消息。
 */
void Key1KeyLiteTask()
{
	// 切换 GPIOE 的第 5 引脚状态（例如用于指示灯或调试）
	HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);

	// 定义一个用于串口传输的字符串消息
	char message[] = "KEY1";
	// 通过 UART1 发送该消息，用于调试或状态监控
	HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);

	// 检查当前日历状态是否为设置模式
	if (calendarState == CalendarState_Setting)
	{
		// 根据当前设置状态（选中的时间项）来增加相应的值
		switch (settingState)
		{
		case Year:
			// 增加年份值
			settingTime.tm_year++;
			// 注意：这里没有设置年份最大值，根据实际情况可能需要添加
			break;
		case Month:
			// 增加月份值
			settingTime.tm_mon++;
			// 如果月份大于11，则设置为0（循环到下一年）
			if (settingTime.tm_mon > 11)
			{
				settingTime.tm_mon = 0;
			}
			break;
		case Day:
			// 增加日期值
			settingTime.tm_mday++;
			// 如果日期大于31，则设置为1（循环到下个月）
			if (settingTime.tm_mday > 31) // 注意：这里没有处理不同月份天数不同的情况
			{
				settingTime.tm_mday = 1;
			}
			break;
		case Hour:
			// 增加小时值
			settingTime.tm_hour++;
			// 如果小时大于23，则设置为0（循环到下一天）
			if (settingTime.tm_hour > 23)
			{
				settingTime.tm_hour = 0;
			}
			break;
		case Minute:
			// 增加分钟值
			settingTime.tm_min++;
			// 如果分钟大于59，则设置为0（循环到下一小时）
			if (settingTime.tm_min > 59)
			{
				settingTime.tm_min = 0;
			}
			break;
		case Second:
			// 增加秒值
			settingTime.tm_sec++;
			// 如果秒大于59，则设置为0（循环到下一分钟）
			if (settingTime.tm_sec > 59)
			{
				settingTime.tm_sec = 0;
			}
			break;
		}
	}
}

/**
 * @brief KEY2 (WK_UP) 按键回调函数
 * @details 该函数在 KEY2 (WK_UP) 按键被触发时执行。功能取决于当前日历状态：
 *          - 在普通模式下：进入设置模式，初始化设置时间和状态。
 *          - 在设置模式下：如果当前选中项是秒，则保存设置并返回普通模式；
 *                         否则，切换到下一个设置项。
 *          同时，它还会切换两个 GPIO 引脚的状态，并通过串口发送一个标识消息。
 */
void Key2KeyLiteTask()
{
	// 切换 GPIOB 的第 5 引脚状态（例如用于指示灯或调试）
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);
	// 切换 GPIOE 的第 5 引脚状态（例如用于指示灯或调试）
	HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);

	// 定义一个用于串口传输的字符串消息
	char message[] = "WK_UP";
	// 通过 UART1 发送该消息，用于调试或状态监控
	HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);

	// 检查当前日历状态是否为普通模式
	if (calendarState == CalendarState_Normal)
	{
		// 获取当前 RTC 时间，并复制到设置时间结构体中
		settingTime = *KK_RTC_GetTime();
		// 设置初始选中项为年份
		settingState = Year;
		// 切换日历状态为设置模式
		calendarState = CalendarState_Setting;
	}
	else // 当前处于设置模式
	{
		// 检查当前选中项是否为秒
		if (settingState == Second)
		{
			// 将设置好的时间写入 RTC
			KK_RTC_SetTime(&settingTime);
			// 切换回普通模式
			calendarState = CalendarState_Normal;
		}
		else
		{
			// 如果不是秒，则切换到下一个设置项
			settingState++;
		}
	}
}

/**
 * @brief 串口调试时间显示函数
 * @details HAL_Delay()堵塞，慎用
 */
void usartTime(struct tm *time)
{
	char message[64] = "";
	sprintf(message, "%d-%d-%d %02d:%02d:%02d", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
			time->tm_hour, time->tm_min, time->tm_sec);
	HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
	HAL_Delay(1000);
}

/**
 * @brief 显示时间函数
 * @param time 要显示的时间结构体指针
 * @details 在TFTLCD上显示年月日、时分秒和星期几
 */
void showTime(struct tm *time)
{
	char str[32];
	// 清除旧的时间数字区域
	lcd_fill(40, 100, 400, 80, WHITE);

	// 显示时间
	sprintf(str, "%d-%02d-%02d", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday);
	uint8_t x_date = (395 - (strlen(str) * 16)) / 2; // 计算水平居中位置
	lcd_show_string(x_date, 100, 320, 16, 16, str, BLACK);

	sprintf(str, "%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec);
	uint8_t x_time = (440 - (strlen(str) * 32)) / 2; // 32像素字体需要更大的居中计算
	lcd_show_string(x_time, 120, 320, 32, 32, str, BLACK);

	char *week = weeks[time->tm_wday];
	uint8_t x_weeks = (395 - (strlen(week) * 16)) / 2;
	lcd_show_string(x_weeks, 160, 320, 16, 16, week, BLACK);
}

/**
 * @brief 显示或闪烁光标（Cursor）的函数。
 *
 * 该函数通过记录时间差来控制光标的闪烁频率。当达到设定的时间间隔时，
 * 会在LCD上绘制光标线条，实现闪烁效果。
 */
void showCursor()
{
	// 静态变量，用于记录上次绘制光标的时间（单位为ms）
	static uint32_t startTime = 0;

	// 计算当前时间与上次开始时间之间的差值（单位为毫秒）
	uint32_t difftime = HAL_GetTick() - startTime;

	// 如果时间差大于两倍的光标闪烁间隔，则重置开始时间为当前时间
	if (difftime > 2 * CURSOR_FLASH_INTERVAL)
	{
		startTime = HAL_GetTick(); // 重置起始时间
	}
	// 如果时间差在第一段闪烁区间内（即介于1倍和2倍间隔之间），则绘制光标
	else if (difftime > CURSOR_FLASH_INTERVAL)
	{
		// 获取当前设置状态下光标的位置信息
		CursorPosition position = cursorPosition[settingState];

		// 在LCD上根据位置信息绘制光标线段
		lcd_draw_line(position.x1, position.x2, position.y1, position.y2, BLACK);
		// 参数说明：
		// x1, y1: 线段起点坐标
		// x2, y2: 线段终点坐标
		// BLACK: 绘制颜色为黑色
	}

	// 注意：未处理“时间差小于等于CURSOR_FLASH_INTERVAL”的情况，
	// 此时光标不绘制，表现为熄灭状态，从而形成闪烁效果。
}

/**
 * @brief 主任务初始化函数
 * @details 初始化TFTLCD、RTC和按键
 */
void MainTaskInit(void)
{
	HAL_Delay(20);
	KK_RTC_Init();
	HAL_Delay(20);
	lcd_init();
	HAL_Delay(100);

	// 初始化状态变量
	/**
   *	@note 原本为	&key0 --> Key0KeyLiteTask	减少
									&key1 --> Key1KeyLiteTask 增加
									&key2 --> Key2KeyLiteTask 设置
						因为 Key2 (WK_UP) 键电路上为 高电平触发 ，导致复位或开机后自动进入设置状态，TODO:待解决
						所以先将按键-功能映射修改为
									&key0 --> Key0KeyLiteTask	减少
									&key1 --> Key2KeyLiteTask 设置
									&key2 --> Key1KeyLiteTask 增加
   */
	KeyLite_Init(&key0, GPIOE, GPIO_PIN_4, Key0KeyLiteTask);
	KeyLite_Init(&key1, GPIOE, GPIO_PIN_3, Key2KeyLiteTask);
	KeyLite_Init(&key2, WK_UP_GPIO_Port, WK_UP_Pin, Key1KeyLiteTask);
}

/**
 * @brief 主任务函数，用于处理按键输入、时间显示以及光标控制。
 *
 * 该函数通常在主循环中被周期性调用，负责以下主要功能：
 * 1. 按键事件的轮询处理；
 * 2. 根据当前日历状态决定是显示实时时间还是设置时间；
 * 3. 在设置状态下显示光标以提示用户正在编辑的位置。
 */
void MainTask(void)
{
	// 处理三个按键对象的状态更新与事件检测
	KeyLite_Process(&key0);
	KeyLite_Process(&key1);
	KeyLite_Process(&key2);

	// 判断当前是否处于正常模式（非设置模式）
	if (calendarState == CalendarState_Normal)
	{
		// 获取当前实时时间（来自RTC模块）
		struct tm *now = KK_RTC_GetTime();

		// 显示当前时间到LCD屏幕上
		showTime(now);

		/* 串口输出时间信息（调试用途）注意：此函数内可能包含 Hal_Delay()，
		 * 可能会阻塞主任务执行。如需使用，请确保不影响系统实时性。
		 *
		 * usartTime(now);
		 */
	}
	else
	{
		// 如果处于设置模式，则显示正在设置的时间
		showTime(&settingTime);

		// 在设置界面显示闪烁光标，提示当前编辑位置
		showCursor();
	}
}
