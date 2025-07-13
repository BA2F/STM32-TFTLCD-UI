#ifndef __TASK_AUTOMODE_H
#define __TASK_AUTOMODE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

// ������ֵ����
#define DARK_THRESHOLD 3900  // ���밵ģʽ��ADC��ֵ
#define LIGHT_THRESHOLD 3500 // ������ģʽ��ADC��ֵ

    /**
     * @brief ��ʼ���Զ�ģʽ����
     *
     * �ú������ڳ�ʼ���Զ�ģʽģ���״̬�����á�
     */
    void automode_init(void);

    /**
     * @brief ִ���Զ�ģʽ����
     *
     * �ú������ݴ����ADCֵ�жϵ�ǰ�������ȣ�����Ӧ�ص�����ʾģʽ��
     *
     * @param adc_val ADC��ȡ������ֵ
     */
    void automode_task(uint16_t adc_val);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_AUTOMODE_H */
