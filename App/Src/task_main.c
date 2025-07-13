/**
 * @file task_main.c
 * @brief �������ļ���ʵ��������ʾ��ʱ�����ù���
 */

#include "task_main.h"

/**
 * @brief �����˸���ʱ��(����)
 */
#define CURSOR_FLASH_INTERVAL 500

/**
 * @brief ���ڼ����ַ�������
 */
char weeks[7][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

/**
 * @brief  ����һ���ⲿ���� calendarState��������Ϊ CalendarState�������ڲ�ͬ�ļ��乲������״̬��Ϣ��
 */
extern CalendarState calendarState;

/**
 * @brief ����״̬ö�٣���ʾ��ǰ�������õ�ʱ����
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
 * @brief ���λ�ýṹ��
 */
typedef struct
{
	uint16_t x1;
	uint16_t x2;
	uint16_t y1;
	uint16_t y2;
} CursorPosition;

/**
 * @brief ��ʱ����Ĺ��λ������
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
 * @brief ��ǰ����״̬
 */
CalendarState calendarState = CalendarState_Normal;

/**
 * @brief ��ǰ����״̬
 */
SettingState settingState = Year;

/**
 * @brief ����ʱ��ṹ��
 */
struct tm settingTime;

/**
 * @brief ���ð���
 */
KeyLiteHandle key0, key1, key2;

/**
 * @brief KEY0 �����ص�����
 * @details �ú����� KEY0 ����������ʱִ�С���Ҫ��������������������ģʽʱ��
 *          ���ٵ�ǰѡ��ʱ�����ֵ��ͬʱ���������л�һ�� GPIO ���ŵ�״̬��
 *          ��ͨ�����ڷ���һ����ʶ��Ϣ��
 */
void Key0KeyLiteTask()
{
	// �л� GPIOB �ĵ� 5 ����״̬����������ָʾ�ƻ���ԣ�
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);

	// ����һ�����ڴ��ڴ�����ַ�����Ϣ
	char message[] = "KEY0";
	// ͨ�� UART1 ���͸���Ϣ�����ڵ��Ի�״̬���
	HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);

	// ��鵱ǰ����״̬�Ƿ�Ϊ����ģʽ
	if (calendarState == CalendarState_Setting)
	{
		// ���ݵ�ǰ����״̬��ѡ�е�ʱ�����������Ӧ��ֵ
		switch (settingState)
		{
		case Year:
			// �������ֵ
			settingTime.tm_year--;
			// ������С����Сֵ���������70����1970�꣩��������Ϊ���ֵ
			if (settingTime.tm_year < 70)
			{
				settingTime.tm_year = 70;
			}
			break;
		case Month:
			// �����·�ֵ
			settingTime.tm_mon--;
			// ����·�С��1��������Ϊ12��ѭ������һ�꣩
			if (settingTime.tm_mon < 1)
			{
				settingTime.tm_mon = 11; // ע�⣺tm_mon �� 0-11
			}
			break;
		case Day:
			// ��������ֵ
			settingTime.tm_mday--;
			// �������С�ڵ���0��������Ϊ31��ѭ�����ϸ��£�
			if (settingTime.tm_mday <= 0)
			{
				settingTime.tm_mday = 31; // ע�⣺����û�д���ͬ�·�������ͬ�����
			}
			break;
		case Hour:
			// ����Сʱֵ
			settingTime.tm_hour--;
			// ���СʱС��0��������Ϊ23��ѭ����ǰһ�죩
			if (settingTime.tm_hour < 0)
			{
				settingTime.tm_hour = 23;
			}
			break;
		case Minute:
			// ���ٷ���ֵ
			settingTime.tm_min--;
			// �������С��0��������Ϊ59��ѭ������һСʱ��
			if (settingTime.tm_min < 0)
			{
				settingTime.tm_min = 59;
			}
			break;
		case Second:
			// ������ֵ
			settingTime.tm_sec--;
			// �����С��0��������Ϊ59��ѭ������һ���ӣ�
			if (settingTime.tm_sec < 0)
			{
				settingTime.tm_sec = 59;
			}
			break;
		}
	}
}

/**
 * @brief KEY1 �����ص�����
 * @details �ú����� KEY1 ����������ʱִ�С���Ҫ��������������������ģʽʱ��
 *          ���ӵ�ǰѡ��ʱ�����ֵ��ͬʱ���������л�һ�� GPIO ���ŵ�״̬��
 *          ��ͨ�����ڷ���һ����ʶ��Ϣ��
 */
void Key1KeyLiteTask()
{
	// �л� GPIOE �ĵ� 5 ����״̬����������ָʾ�ƻ���ԣ�
	HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);

	// ����һ�����ڴ��ڴ�����ַ�����Ϣ
	char message[] = "KEY1";
	// ͨ�� UART1 ���͸���Ϣ�����ڵ��Ի�״̬���
	HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);

	// ��鵱ǰ����״̬�Ƿ�Ϊ����ģʽ
	if (calendarState == CalendarState_Setting)
	{
		// ���ݵ�ǰ����״̬��ѡ�е�ʱ�����������Ӧ��ֵ
		switch (settingState)
		{
		case Year:
			// �������ֵ
			settingTime.tm_year++;
			// ע�⣺����û������������ֵ������ʵ�����������Ҫ���
			break;
		case Month:
			// �����·�ֵ
			settingTime.tm_mon++;
			// ����·ݴ���11��������Ϊ0��ѭ������һ�꣩
			if (settingTime.tm_mon > 11)
			{
				settingTime.tm_mon = 0;
			}
			break;
		case Day:
			// ��������ֵ
			settingTime.tm_mday++;
			// ������ڴ���31��������Ϊ1��ѭ�����¸��£�
			if (settingTime.tm_mday > 31) // ע�⣺����û�д���ͬ�·�������ͬ�����
			{
				settingTime.tm_mday = 1;
			}
			break;
		case Hour:
			// ����Сʱֵ
			settingTime.tm_hour++;
			// ���Сʱ����23��������Ϊ0��ѭ������һ�죩
			if (settingTime.tm_hour > 23)
			{
				settingTime.tm_hour = 0;
			}
			break;
		case Minute:
			// ���ӷ���ֵ
			settingTime.tm_min++;
			// ������Ӵ���59��������Ϊ0��ѭ������һСʱ��
			if (settingTime.tm_min > 59)
			{
				settingTime.tm_min = 0;
			}
			break;
		case Second:
			// ������ֵ
			settingTime.tm_sec++;
			// ��������59��������Ϊ0��ѭ������һ���ӣ�
			if (settingTime.tm_sec > 59)
			{
				settingTime.tm_sec = 0;
			}
			break;
		}
	}
}

/**
 * @brief KEY2 (WK_UP) �����ص�����
 * @details �ú����� KEY2 (WK_UP) ����������ʱִ�С�����ȡ���ڵ�ǰ����״̬��
 *          - ����ͨģʽ�£���������ģʽ����ʼ������ʱ���״̬��
 *          - ������ģʽ�£������ǰѡ�������룬�򱣴����ò�������ͨģʽ��
 *                         �����л�����һ�������
 *          ͬʱ���������л����� GPIO ���ŵ�״̬����ͨ�����ڷ���һ����ʶ��Ϣ��
 */
void Key2KeyLiteTask()
{
	// �л� GPIOB �ĵ� 5 ����״̬����������ָʾ�ƻ���ԣ�
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);
	// �л� GPIOE �ĵ� 5 ����״̬����������ָʾ�ƻ���ԣ�
	HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);

	// ����һ�����ڴ��ڴ�����ַ�����Ϣ
	char message[] = "WK_UP";
	// ͨ�� UART1 ���͸���Ϣ�����ڵ��Ի�״̬���
	HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);

	// ��鵱ǰ����״̬�Ƿ�Ϊ��ͨģʽ
	if (calendarState == CalendarState_Normal)
	{
		// ��ȡ��ǰ RTC ʱ�䣬�����Ƶ�����ʱ��ṹ����
		settingTime = *KK_RTC_GetTime();
		// ���ó�ʼѡ����Ϊ���
		settingState = Year;
		// �л�����״̬Ϊ����ģʽ
		calendarState = CalendarState_Setting;
	}
	else // ��ǰ��������ģʽ
	{
		// ��鵱ǰѡ�����Ƿ�Ϊ��
		if (settingState == Second)
		{
			// �����úõ�ʱ��д�� RTC
			KK_RTC_SetTime(&settingTime);
			// �л�����ͨģʽ
			calendarState = CalendarState_Normal;
		}
		else
		{
			// ��������룬���л�����һ��������
			settingState++;
		}
	}
}

/**
 * @brief ���ڵ���ʱ����ʾ����
 * @details HAL_Delay()����������
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
 * @brief ��ʾʱ�亯��
 * @param time Ҫ��ʾ��ʱ��ṹ��ָ��
 * @details ��TFTLCD����ʾ�����ա�ʱ��������ڼ�
 */
void showTime(struct tm *time)
{
	char str[32];
	// ����ɵ�ʱ����������
	lcd_fill(40, 100, 400, 80, WHITE);

	// ��ʾʱ��
	sprintf(str, "%d-%02d-%02d", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday);
	uint8_t x_date = (395 - (strlen(str) * 16)) / 2; // ����ˮƽ����λ��
	lcd_show_string(x_date, 100, 320, 16, 16, str, BLACK);

	sprintf(str, "%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec);
	uint8_t x_time = (440 - (strlen(str) * 32)) / 2; // 32����������Ҫ����ľ��м���
	lcd_show_string(x_time, 120, 320, 32, 32, str, BLACK);

	char *week = weeks[time->tm_wday];
	uint8_t x_weeks = (395 - (strlen(week) * 16)) / 2;
	lcd_show_string(x_weeks, 160, 320, 16, 16, week, BLACK);
}

/**
 * @brief ��ʾ����˸��꣨Cursor���ĺ�����
 *
 * �ú���ͨ����¼ʱ��������ƹ�����˸Ƶ�ʡ����ﵽ�趨��ʱ����ʱ��
 * ����LCD�ϻ��ƹ��������ʵ����˸Ч����
 */
void showCursor()
{
	// ��̬���������ڼ�¼�ϴλ��ƹ���ʱ�䣨��λΪms��
	static uint32_t startTime = 0;

	// ���㵱ǰʱ�����ϴο�ʼʱ��֮��Ĳ�ֵ����λΪ���룩
	uint32_t difftime = HAL_GetTick() - startTime;

	// ���ʱ�����������Ĺ����˸����������ÿ�ʼʱ��Ϊ��ǰʱ��
	if (difftime > 2 * CURSOR_FLASH_INTERVAL)
	{
		startTime = HAL_GetTick(); // ������ʼʱ��
	}
	// ���ʱ����ڵ�һ����˸�����ڣ�������1����2�����֮�䣩������ƹ��
	else if (difftime > CURSOR_FLASH_INTERVAL)
	{
		// ��ȡ��ǰ����״̬�¹���λ����Ϣ
		CursorPosition position = cursorPosition[settingState];

		// ��LCD�ϸ���λ����Ϣ���ƹ���߶�
		lcd_draw_line(position.x1, position.x2, position.y1, position.y2, BLACK);
		// ����˵����
		// x1, y1: �߶��������
		// x2, y2: �߶��յ�����
		// BLACK: ������ɫΪ��ɫ
	}

	// ע�⣺δ����ʱ���С�ڵ���CURSOR_FLASH_INTERVAL���������
	// ��ʱ��겻���ƣ�����ΪϨ��״̬���Ӷ��γ���˸Ч����
}

/**
 * @brief �������ʼ������
 * @details ��ʼ��TFTLCD��RTC�Ͱ���
 */
void MainTaskInit(void)
{
	HAL_Delay(20);
	KK_RTC_Init();
	HAL_Delay(20);
	lcd_init();
	HAL_Delay(100);

	// ��ʼ��״̬����
	/**
   *	@note ԭ��Ϊ	&key0 --> Key0KeyLiteTask	����
									&key1 --> Key1KeyLiteTask ����
									&key2 --> Key2KeyLiteTask ����
						��Ϊ Key2 (WK_UP) ����·��Ϊ �ߵ�ƽ���� �����¸�λ�򿪻����Զ���������״̬��TODO:�����
						�����Ƚ�����-����ӳ���޸�Ϊ
									&key0 --> Key0KeyLiteTask	����
									&key1 --> Key2KeyLiteTask ����
									&key2 --> Key1KeyLiteTask ����
   */
	KeyLite_Init(&key0, GPIOE, GPIO_PIN_4, Key0KeyLiteTask);
	KeyLite_Init(&key1, GPIOE, GPIO_PIN_3, Key2KeyLiteTask);
	KeyLite_Init(&key2, WK_UP_GPIO_Port, WK_UP_Pin, Key1KeyLiteTask);
}

/**
 * @brief �������������ڴ��������롢ʱ����ʾ�Լ������ơ�
 *
 * �ú���ͨ������ѭ���б������Ե��ã�����������Ҫ���ܣ�
 * 1. �����¼�����ѯ����
 * 2. ���ݵ�ǰ����״̬��������ʾʵʱʱ�仹������ʱ�䣻
 * 3. ������״̬����ʾ�������ʾ�û����ڱ༭��λ�á�
 */
void MainTask(void)
{
	// �����������������״̬�������¼����
	KeyLite_Process(&key0);
	KeyLite_Process(&key1);
	KeyLite_Process(&key2);

	// �жϵ�ǰ�Ƿ�������ģʽ��������ģʽ��
	if (calendarState == CalendarState_Normal)
	{
		// ��ȡ��ǰʵʱʱ�䣨����RTCģ�飩
		struct tm *now = KK_RTC_GetTime();

		// ��ʾ��ǰʱ�䵽LCD��Ļ��
		showTime(now);

		/* �������ʱ����Ϣ��������;��ע�⣺�˺����ڿ��ܰ��� Hal_Delay()��
		 * ���ܻ�����������ִ�С�����ʹ�ã���ȷ����Ӱ��ϵͳʵʱ�ԡ�
		 *
		 * usartTime(now);
		 */
	}
	else
	{
		// �����������ģʽ������ʾ�������õ�ʱ��
		showTime(&settingTime);

		// �����ý�����ʾ��˸��꣬��ʾ��ǰ�༭λ��
		showCursor();
	}
}
