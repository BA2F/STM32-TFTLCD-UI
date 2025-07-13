#ifndef __TASK_AUTOMODE_H
#define __TASK_AUTOMODE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

// 定义阈值常量
#define DARK_THRESHOLD 3900  // 进入暗模式的ADC阈值
#define LIGHT_THRESHOLD 3500 // 进入亮模式的ADC阈值

    /**
     * @brief 初始化自动模式功能
     *
     * 该函数用于初始化自动模式模块的状态和配置。
     */
    void automode_init(void);

    /**
     * @brief 执行自动模式任务
     *
     * 该函数根据传入的ADC值判断当前环境亮度，并相应地调整显示模式。
     *
     * @param adc_val ADC读取的亮度值
     */
    void automode_task(uint16_t adc_val);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_AUTOMODE_H */
