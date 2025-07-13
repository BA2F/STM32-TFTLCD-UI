/**
 * @file KEY.h
 * @brief 轻量级按键状态机头文件
 * @detail
 * - 支持GPIO按键消抖、单击事件检测
 * - 基于状态机设计，非阻塞式处理
 * - 需配合KEY.c文件使用
 * @warning 需在工程中定义KEY_DEBOUNCE_TICKS宏（消抖时间，单位：系统tick）
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

// 防抖周期
#define KEY_DEBOUNCE_TICKS 10

  /**
   * @brief 按键物理状态枚举
   * @note 对应GPIO电平状态，假设按下为低电平
   */
  typedef enum
  {
    Pressed = 0,  ///< 按键按下状态（GPIO_RESET）
    Unpressed = 1 ///< 按键释放状态（GPIO_SET）
  } KeyLiteState;

  /**
   * @brief 按键事件回调函数类型
   * @note 用户可在此函数中实现单击事件处理逻辑
   */
  typedef void (*KeyLiteCallback)(void);

  /**
   * @brief 按键状态机控制结构体
   * @note 每个物理按键对应一个独立实例
   */
  typedef struct
  {
    GPIO_TypeDef *GPIOx;      ///< GPIO端口指针（如GPIOA）
    uint16_t GPIO_Pin;        ///< GPIO引脚号（如GPIO_PIN_0）
    KeyLiteState prevState;   ///< 前次状态（用于边缘检测）
    uint32_t pressedTime;     ///< 按下时刻的时间戳（单位：系统tick）
    uint8_t isDebounced;      ///< 消抖完成标志（1=消抖有效）
    KeyLiteCallback callback; ///< 消抖完成后的回调函数指针
  } KeyLiteHandle;

  /**
   * @brief 初始化按键实例
   * @param handle 按键实例指针（需用户分配内存）
   * @param GPIOx 绑定的GPIO端口（如GPIOA）
   * @param GPIO_Pin 绑定的GPIO引脚（如GPIO_PIN_0）
   * @param cb 事件回调函数（可为NULL）
   * @warning handle指针必须有效，否则会导致内存错误
   */
  void KeyLite_Init(KeyLiteHandle *handle, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, KeyLiteCallback cb);

  /**
   * @brief 按键状态处理函数
   * @param handle 按键实例指针
   * @note
   * - 必须周期性调用（建议周期1-10ms）
   * - 执行状态检测、消抖处理和回调触发
   * - 此函数不可在中断中直接调用
   */
  void KeyLite_Process(KeyLiteHandle *handle);

#ifdef __cplusplus
}
#endif

#endif // KEY_FIR_H
