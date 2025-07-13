#include "task_automode.h"
#include "lcd.h"
#include "task_main.h" // ����������״̬����

// ʹ��ͷ�ļ��ж������ֵ����
#define DARK_MODE_THRESHOLD DARK_THRESHOLD
#define LIGHT_MODE_THRESHOLD LIGHT_THRESHOLD

// ��ʾģʽö��
typedef enum
{
    LIGHT_MODE,
    DARK_MODE,
    UNKNOWN_MODE
} DisplayMode;

// ģ���ڲ�״̬
static DisplayMode current_mode = UNKNOWN_MODE;

// �ⲿ����������״̬
extern CalendarState calendarState;

/**
 * @brief ��ʼ���Զ�ģʽģ��
 *
 * �ú�����ʼ���Զ�ģʽģ���״̬������LCD����Ϊ��ɫ�����õ�ǰģʽΪLIGHT_MODE��
 */
void automode_init(void)
{
    // ��ʼ��״̬
    current_mode = UNKNOWN_MODE;

    // ��ʼ����Ϊ��ɫ
    lcd_clear(WHITE);
    current_mode = LIGHT_MODE;
}

/**
 * @brief �Զ�ģʽ��������
 *
 * �ú�������ADCֵ�жϵ�ǰ�������ȣ�����Ӧ���л�LCD��ʾģʽ��
 * ������������״̬�´����Զ�ģʽ��
 *
 * @param adc_val ADC��ȡ������ֵ
 */
void automode_task(uint16_t adc_val)
{
    // ������������״̬�´����Զ�ģʽ
    if (calendarState != CalendarState_Normal)
    {
        return;
    }

    // ����ģʽ�л�
    if (adc_val > DARK_MODE_THRESHOLD && current_mode != DARK_MODE)
    {
        // ���ADCֵ���ڰ�ģʽ��ֵ�ҵ�ǰģʽ���ǰ�ģʽ��������Ϊ��ɫ�������õ�ǰģʽΪDARK_MODE
        lcd_clear(BLACK);
        current_mode = DARK_MODE;
    }
    else if (adc_val < LIGHT_MODE_THRESHOLD && current_mode != LIGHT_MODE)
    {
        // ���ADCֵС����ģʽ��ֵ�ҵ�ǰģʽ������ģʽ��������Ϊ��ɫ�������õ�ǰģʽΪLIGHT_MODE
        lcd_clear(WHITE);
        current_mode = LIGHT_MODE;
    }

    // �м�����LIGHT_THRESHOLD-DARK_THRESHOLD�����ֵ�ǰģʽ����
}
