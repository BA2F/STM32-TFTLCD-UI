/**
 * @file KEY.h
 * @brief ����������״̬��ͷ�ļ�
 * @detail
 * - ֧��GPIO���������������¼����
 * - ����״̬����ƣ�������ʽ����
 * - �����KEY.c�ļ�ʹ��
 * @warning ���ڹ����ж���KEY_DEBOUNCE_TICKS�꣨����ʱ�䣬��λ��ϵͳtick��
 */
#ifndef ___KEY_H__
#define ___KEY_H__

#include "stm32f1xx.h"
#include "main.h"
// #include "stdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

// ��������
#define KEY_DEBOUNCE_TICKS 10

  /**
   * @brief ��������״̬ö��
   * @note ��ӦGPIO��ƽ״̬�����谴��Ϊ�͵�ƽ
   */
  typedef enum
  {
    Pressed = 0,  ///< ��������״̬��GPIO_RESET��
    Unpressed = 1 ///< �����ͷ�״̬��GPIO_SET��
  } KeyLiteState;

  /**
   * @brief �����¼��ص���������
   * @note �û����ڴ˺�����ʵ�ֵ����¼������߼�
   */
  typedef void (*KeyLiteCallback)(void);

  /**
   * @brief ����״̬�����ƽṹ��
   * @note ÿ����������Ӧһ������ʵ��
   */
  typedef struct
  {
    GPIO_TypeDef *GPIOx;      ///< GPIO�˿�ָ�루��GPIOA��
    uint16_t GPIO_Pin;        ///< GPIO���źţ���GPIO_PIN_0��
    KeyLiteState prevState;   ///< ǰ��״̬�����ڱ�Ե��⣩
    uint32_t pressedTime;     ///< ����ʱ�̵�ʱ�������λ��ϵͳtick��
    uint8_t isDebounced;      ///< ������ɱ�־��1=������Ч��
    KeyLiteCallback callback; ///< ������ɺ�Ļص�����ָ��
  } KeyLiteHandle;

  /**
   * @brief ��ʼ������ʵ��
   * @param handle ����ʵ��ָ�루���û������ڴ棩
   * @param GPIOx �󶨵�GPIO�˿ڣ���GPIOA��
   * @param GPIO_Pin �󶨵�GPIO���ţ���GPIO_PIN_0��
   * @param cb �¼��ص���������ΪNULL��
   * @warning handleָ�������Ч������ᵼ���ڴ����
   */
  void KeyLite_Init(KeyLiteHandle *handle, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, KeyLiteCallback cb);

  /**
   * @brief ����״̬������
   * @param handle ����ʵ��ָ��
   * @note
   * - ���������Ե��ã���������1-10ms��
   * - ִ��״̬��⡢��������ͻص�����
   * - �˺����������ж���ֱ�ӵ���
   */
  void KeyLite_Process(KeyLiteHandle *handle);

#ifdef __cplusplus
}
#endif

#endif // KEY_FIR_H
