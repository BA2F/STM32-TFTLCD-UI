/**
 * @file KEY.c
 * @brief ����״̬��������������
 * @detail
 */
#include "KEY.h"

/**
 * @brief ��ʼ������ʵ��
 * @param handle ����ʵ�����ָ��
 * @param GPIOx GPIO�˿ڣ���GPIOA��
 * @param GPIO_Pin GPIO���ţ���GPIO_PIN_0��
 * @param cb �����¼��ص�����ָ�루��ΪNULL��
 * @note
 * - ��ʼ�������ڲ�״̬ΪĬ��ֵ
 * - ������ʹ�ð���ʵ��ǰ����
 * - �ص�������������ɺ󴥷�
 */
void KeyLite_Init(KeyLiteHandle *handle, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, KeyLiteCallback cb)
{
  handle->GPIOx = GPIOx;         // ��GPIO�˿�
  handle->GPIO_Pin = GPIO_Pin;   // ��GPIO����
  handle->prevState = Unpressed; // ��ʼ��״̬Ϊδ����
  handle->pressedTime = 0;       // ���㰴��ʱ���
  handle->isDebounced = 0;       // ����������ɱ�־
  handle->callback = cb;         // �����¼��ص�����
}

/**
 * @brief ����״̬������
 * @param handle ����ʵ�����ָ��
 * @note
 * - ����mainѭ���ж��ڵ��ã�����ÿ1-10ms����һ�Σ�
 * - ִ�����̣�
 *   1. ��ȡ��ǰGPIO״̬
 *   2. ���״̬�仯��ǰ��/���أ�
 *   3. ����ʱ��¼ʱ���
 *   4. ��������ʱ��󴥷��ص�
 * - �������ԣ���⵽״̬�仯���豣����״̬����KEY_DEBOUNCE_TICKSʱ�����Ϊ��Ч
 */
void KeyLite_Process(KeyLiteHandle *handle)
{
  // ��ȡ��ǰ����״̬��GPIO_RESET��ʾ���£�
  KeyLiteState currentState = (HAL_GPIO_ReadPin(handle->GPIOx, handle->GPIO_Pin) == GPIO_PIN_RESET)
                                  ? Pressed
                                  : Unpressed;

  /* ״̬�仯��⣨ǰ��/���ؼ�⣩*/
  if (currentState != handle->prevState)
  {
    handle->prevState = currentState; // ������ʷ״̬
    handle->isDebounced = 0;          // ����������־
    handle->pressedTime = 0;          // �������ʱ���
  }

  /* ����״̬���� */
  if (currentState == Pressed)
  {
    // ���μ�⵽����ʱ��¼ʱ���
    if (handle->pressedTime == 0)
    {
      handle->pressedTime = HAL_GetTick();
    }
    // ����ʱ���飨�������ڣ�KEY_DEBOUNCE_TICKS��ͨ��10-20ms��
    else if (!handle->isDebounced && (HAL_GetTick() - handle->pressedTime) > KEY_DEBOUNCE_TICKS)
    {
      handle->isDebounced = 1; // ����������
      // �����ص������������ע�ᣩ
      if (handle->callback)
      {
        handle->callback(); // ִ���û�����Ĳ���
      }
    }
  }
}
