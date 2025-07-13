#include "kk_rtc.h" // 包含RTC驱动自定义头文件

// 定义RTC初始化标志值（存储在备份寄存器DR1中）
#define RTC_INIT_FLAG 0x2333

/**
 * @brief  进入RTC初始化模式
 * @param  hrtc  指向RTC_HandleTypeDef结构体的指针
 * @retval HAL状态值
 *
 * 功能说明：
 * 1. 等待RTC寄存器操作完成标志(RTOFF)置位
 * 2. 超时检测（防止死等）
 * 3. 禁用RTC写保护（允许配置寄存器）
 */
static HAL_StatusTypeDef RTC_EnterInitMode(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  tickstart = HAL_GetTick(); // 获取当前系统tick值
  /* 循环等待RTOFF标志置位（表示可操作寄存器） */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    /* 超时检测（RTC_TIMEOUT_VALUE需在头文件中定义） */
    if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT; // 超时返回错误
    }
  }

  /* 解除RTC寄存器写保护（使能寄存器修改） */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  return HAL_OK; // 成功进入初始化模式
}

/**
 * @brief  退出RTC初始化模式
 * @param  hrtc  RTC句柄指针
 * @retval HAL状态值
 *
 * 功能说明：
 * 1. 重新使能写保护（防止意外修改）
 * 2. 等待RTOFF标志置位（确认操作完成）
 */
static HAL_StatusTypeDef RTC_ExitInitMode(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  /* 使能RTC寄存器写保护（锁定寄存器） */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  tickstart = HAL_GetTick();
  /* 等待RTOFF标志置位（确保操作完成） */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    /* 超时检测 */
    if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/**
 * @brief  写入RTC时间计数器
 * @param  hrtc        RTC句柄指针
 * @param  TimeCounter 32位时间计数值（Unix时间戳）
 * @retval HAL状态值
 *
 * 关键流程：
 * 1. 进入初始化模式
 * 2. 拆分32位值为高低16位写入CNTH/CNTL寄存器
 * 3. 退出初始化模式
 */
static HAL_StatusTypeDef RTC_WriteTimeCounter(RTC_HandleTypeDef *hrtc, uint32_t TimeCounter)
{
  HAL_StatusTypeDef status = HAL_OK; // 默认状态为成功

  /* 尝试进入初始化模式 */
  if (RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    status = HAL_ERROR; // 进入失败返回错误
  }
  else
  {
    /* 写入时间计数器高16位 */
    WRITE_REG(hrtc->Instance->CNTH, (TimeCounter >> 16U));
    /* 写入时间计数器低16位（使用掩码确保只写入有效位） */
    WRITE_REG(hrtc->Instance->CNTL, (TimeCounter & RTC_CNTL_RTC_CNT));

    /* 尝试退出初始化模式 */
    if (RTC_ExitInitMode(hrtc) != HAL_OK)
    {
      status = HAL_ERROR; // 退出失败返回错误
    }
  }

  return status;
}

/**
 * @brief  读取RTC时间计数器
 * @param  hrtc  RTC句柄指针
 * @retval 32位时间计数值（Unix时间戳）
 *
 * 防溢出处理机制：
 * 1. 采用"读高-读低-再读高"的顺序
 * 2. 若前后两次高16位不同，说明发生进位
 * 3. 发生进位时重新读取低16位组合
 */
static uint32_t RTC_ReadTimeCounter(RTC_HandleTypeDef *hrtc)
{
  uint16_t high1 = 0U, high2 = 0U, low = 0U;
  uint32_t timecounter = 0U;

  /* 第一次读取高16位 */
  high1 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);
  /* 读取低16位 */
  low = READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT);
  /* 第二次读取高16位（检测是否发生进位） */
  high2 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);

  /* 检查高16位是否变化 */
  if (high1 != high2)
  {
    /* 发生进位：组合第二次高16位和重新读取的低16位 */
    timecounter = (((uint32_t)high2 << 16U) | READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT));
  }
  else
  {
    /* 无进位：组合第一次读取的高低16位 */
    timecounter = (((uint32_t)high1 << 16U) | low);
  }

  return timecounter;
}

/**
 * @brief 设置RTC时间（用户接口）
 * @param time 指向tm结构体的指针
 * @retval HAL状态值
 *
 * 转换流程：
 * 1. 将日历时间转换为Unix时间戳
 * 2. 调用内部函数写入计数器
 */
HAL_StatusTypeDef KK_RTC_SetTime(struct tm *time)
{
  /* 将tm结构转换为Unix时间戳 */
  uint32_t unixTime = mktime(time);
  /* 写入RTC计数器 */
  return RTC_WriteTimeCounter(&hrtc, unixTime);
}

/**
 * @brief 获取RTC时间（用户接口）
 * @retval 指向静态tm结构体的指针
 *
 * 注意：
 * - 返回的指针指向静态内存，不可长期保存
 * - 需确保系统时区配置正确
 */
struct tm *KK_RTC_GetTime(void)
{
  /* 读取计数器获取Unix时间戳 */
  time_t unixTime = RTC_ReadTimeCounter(&hrtc);
  /* 转换为日历时间（Keil MDK使用localtime()） */
  return localtime(&unixTime);
}

/**
 * @brief RTC初始化函数
 *
 * 初始化流程：
 * 1. 检查备份寄存器中的初始化标志
 * 2. 若未初始化则进行HAL层初始化
 * 3. 设置默认时间（2025-04-30 23:59:55）
 * 4. 写入初始化标志到备份寄存器
 */
void KK_RTC_Init(void)
{
  /* 读取备份寄存器DR1的初始化标志 */
  uint32_t initFlag = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);

  /* 已初始化则直接返回 */
  if (initFlag == RTC_INIT_FLAG)
    return;

  /* 初始化RTC外设（HAL层配置） */
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler(); // 错误处理（需用户实现）
  }

  /* 设置默认时间结构体 */
  struct tm time = {
      .tm_year = 2025 - 1900, // 年份偏移（1900基准）
      .tm_mon = 4 - 1,        // 月份范围0-11
      .tm_mday = 30,          // 日
      .tm_hour = 23,          // 时
      .tm_min = 59,           // 分
      .tm_sec = 55,           // 秒
  };

  /* 设置RTC时间 */
  KK_RTC_SetTime(&time);

  /* 写入初始化标志到备份寄存器DR1 */
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_INIT_FLAG);
}
