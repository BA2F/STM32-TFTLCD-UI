/**
 * @file KEY.c
 * @brief 按键状态机：消抖，单击
 * @detail
 */
#include "KEY.h"

/**
 * @brief 初始化按键实例
 * @param handle 按键实例句柄指针
 * @param GPIOx GPIO端口（如GPIOA）
 * @param GPIO_Pin GPIO引脚（如GPIO_PIN_0）
 * @param cb 按键事件回调函数指针（可为NULL）
 * @note
 * - 初始化所有内部状态为默认值
 * - 必须在使用按键实例前调用
 * - 回调函数在消抖完成后触发
 */
void KeyLite_Init(KeyLiteHandle *handle, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, KeyLiteCallback cb)
{
  handle->GPIOx = GPIOx;         // 绑定GPIO端口
  handle->GPIO_Pin = GPIO_Pin;   // 绑定GPIO引脚
  handle->prevState = Unpressed; // 初始化状态为未按下
  handle->pressedTime = 0;       // 清零按下时间戳
  handle->isDebounced = 0;       // 重置消抖完成标志
  handle->callback = cb;         // 设置事件回调函数
}

/**
 * @brief 按键状态处理函数
 * @param handle 按键实例句柄指针
 * @note
 * - 需在main循环中定期调用（建议每1-10ms调用一次）
 * - 执行流程：
 *   1. 读取当前GPIO状态
 *   2. 检测状态变化（前沿/后沿）
 *   3. 按下时记录时间戳
 *   4. 经过消抖时间后触发回调
 * - 消抖策略：检测到状态变化后，需保持新状态超过KEY_DEBOUNCE_TICKS时间才视为有效
 */
void KeyLite_Process(KeyLiteHandle *handle)
{
  // 读取当前物理状态（GPIO_RESET表示按下）
  KeyLiteState currentState = (HAL_GPIO_ReadPin(handle->GPIOx, handle->GPIO_Pin) == GPIO_PIN_RESET)
                                  ? Pressed
                                  : Unpressed;

  /* 状态变化检测（前沿/后沿检测）*/
  if (currentState != handle->prevState)
  {
    handle->prevState = currentState; // 更新历史状态
    handle->isDebounced = 0;          // 重置消抖标志
    handle->pressedTime = 0;          // 清除按下时间戳
  }

  /* 按下状态处理 */
  if (currentState == Pressed)
  {
    // 初次检测到按下时记录时间戳
    if (handle->pressedTime == 0)
    {
      handle->pressedTime = HAL_GetTick();
    }
    // 消抖时间检查（防抖周期：KEY_DEBOUNCE_TICKS，通常10-20ms）
    else if (!handle->isDebounced && (HAL_GetTick() - handle->pressedTime) > KEY_DEBOUNCE_TICKS)
    {
      handle->isDebounced = 1; // 标记消抖完成
      // 触发回调函数（如果已注册）
      if (handle->callback)
      {
        handle->callback(); // 执行用户定义的操作
      }
    }
  }
}
