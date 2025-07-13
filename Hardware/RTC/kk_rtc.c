#include "kk_rtc.h" // ����RTC�����Զ���ͷ�ļ�

// ����RTC��ʼ����־ֵ���洢�ڱ��ݼĴ���DR1�У�
#define RTC_INIT_FLAG 0x2333

/**
 * @brief  ����RTC��ʼ��ģʽ
 * @param  hrtc  ָ��RTC_HandleTypeDef�ṹ���ָ��
 * @retval HAL״ֵ̬
 *
 * ����˵����
 * 1. �ȴ�RTC�Ĵ���������ɱ�־(RTOFF)��λ
 * 2. ��ʱ��⣨��ֹ���ȣ�
 * 3. ����RTCд�������������üĴ�����
 */
static HAL_StatusTypeDef RTC_EnterInitMode(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  tickstart = HAL_GetTick(); // ��ȡ��ǰϵͳtickֵ
  /* ѭ���ȴ�RTOFF��־��λ����ʾ�ɲ����Ĵ����� */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    /* ��ʱ��⣨RTC_TIMEOUT_VALUE����ͷ�ļ��ж��壩 */
    if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT; // ��ʱ���ش���
    }
  }

  /* ���RTC�Ĵ���д������ʹ�ܼĴ����޸ģ� */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  return HAL_OK; // �ɹ������ʼ��ģʽ
}

/**
 * @brief  �˳�RTC��ʼ��ģʽ
 * @param  hrtc  RTC���ָ��
 * @retval HAL״ֵ̬
 *
 * ����˵����
 * 1. ����ʹ��д��������ֹ�����޸ģ�
 * 2. �ȴ�RTOFF��־��λ��ȷ�ϲ�����ɣ�
 */
static HAL_StatusTypeDef RTC_ExitInitMode(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  /* ʹ��RTC�Ĵ���д�����������Ĵ����� */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  tickstart = HAL_GetTick();
  /* �ȴ�RTOFF��־��λ��ȷ��������ɣ� */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    /* ��ʱ��� */
    if ((HAL_GetTick() - tickstart) > RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/**
 * @brief  д��RTCʱ�������
 * @param  hrtc        RTC���ָ��
 * @param  TimeCounter 32λʱ�����ֵ��Unixʱ�����
 * @retval HAL״ֵ̬
 *
 * �ؼ����̣�
 * 1. �����ʼ��ģʽ
 * 2. ���32λֵΪ�ߵ�16λд��CNTH/CNTL�Ĵ���
 * 3. �˳���ʼ��ģʽ
 */
static HAL_StatusTypeDef RTC_WriteTimeCounter(RTC_HandleTypeDef *hrtc, uint32_t TimeCounter)
{
  HAL_StatusTypeDef status = HAL_OK; // Ĭ��״̬Ϊ�ɹ�

  /* ���Խ����ʼ��ģʽ */
  if (RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    status = HAL_ERROR; // ����ʧ�ܷ��ش���
  }
  else
  {
    /* д��ʱ���������16λ */
    WRITE_REG(hrtc->Instance->CNTH, (TimeCounter >> 16U));
    /* д��ʱ���������16λ��ʹ������ȷ��ֻд����Чλ�� */
    WRITE_REG(hrtc->Instance->CNTL, (TimeCounter & RTC_CNTL_RTC_CNT));

    /* �����˳���ʼ��ģʽ */
    if (RTC_ExitInitMode(hrtc) != HAL_OK)
    {
      status = HAL_ERROR; // �˳�ʧ�ܷ��ش���
    }
  }

  return status;
}

/**
 * @brief  ��ȡRTCʱ�������
 * @param  hrtc  RTC���ָ��
 * @retval 32λʱ�����ֵ��Unixʱ�����
 *
 * �����������ƣ�
 * 1. ����"����-����-�ٶ���"��˳��
 * 2. ��ǰ�����θ�16λ��ͬ��˵��������λ
 * 3. ������λʱ���¶�ȡ��16λ���
 */
static uint32_t RTC_ReadTimeCounter(RTC_HandleTypeDef *hrtc)
{
  uint16_t high1 = 0U, high2 = 0U, low = 0U;
  uint32_t timecounter = 0U;

  /* ��һ�ζ�ȡ��16λ */
  high1 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);
  /* ��ȡ��16λ */
  low = READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT);
  /* �ڶ��ζ�ȡ��16λ������Ƿ�����λ�� */
  high2 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);

  /* ����16λ�Ƿ�仯 */
  if (high1 != high2)
  {
    /* ������λ����ϵڶ��θ�16λ�����¶�ȡ�ĵ�16λ */
    timecounter = (((uint32_t)high2 << 16U) | READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT));
  }
  else
  {
    /* �޽�λ����ϵ�һ�ζ�ȡ�ĸߵ�16λ */
    timecounter = (((uint32_t)high1 << 16U) | low);
  }

  return timecounter;
}

/**
 * @brief ����RTCʱ�䣨�û��ӿڣ�
 * @param time ָ��tm�ṹ���ָ��
 * @retval HAL״ֵ̬
 *
 * ת�����̣�
 * 1. ������ʱ��ת��ΪUnixʱ���
 * 2. �����ڲ�����д�������
 */
HAL_StatusTypeDef KK_RTC_SetTime(struct tm *time)
{
  /* ��tm�ṹת��ΪUnixʱ��� */
  uint32_t unixTime = mktime(time);
  /* д��RTC������ */
  return RTC_WriteTimeCounter(&hrtc, unixTime);
}

/**
 * @brief ��ȡRTCʱ�䣨�û��ӿڣ�
 * @retval ָ��̬tm�ṹ���ָ��
 *
 * ע�⣺
 * - ���ص�ָ��ָ��̬�ڴ棬���ɳ��ڱ���
 * - ��ȷ��ϵͳʱ��������ȷ
 */
struct tm *KK_RTC_GetTime(void)
{
  /* ��ȡ��������ȡUnixʱ��� */
  time_t unixTime = RTC_ReadTimeCounter(&hrtc);
  /* ת��Ϊ����ʱ�䣨Keil MDKʹ��localtime()�� */
  return localtime(&unixTime);
}

/**
 * @brief RTC��ʼ������
 *
 * ��ʼ�����̣�
 * 1. ��鱸�ݼĴ����еĳ�ʼ����־
 * 2. ��δ��ʼ�������HAL���ʼ��
 * 3. ����Ĭ��ʱ�䣨2025-04-30 23:59:55��
 * 4. д���ʼ����־�����ݼĴ���
 */
void KK_RTC_Init(void)
{
  /* ��ȡ���ݼĴ���DR1�ĳ�ʼ����־ */
  uint32_t initFlag = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);

  /* �ѳ�ʼ����ֱ�ӷ��� */
  if (initFlag == RTC_INIT_FLAG)
    return;

  /* ��ʼ��RTC���裨HAL�����ã� */
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler(); // ���������û�ʵ�֣�
  }

  /* ����Ĭ��ʱ��ṹ�� */
  struct tm time = {
      .tm_year = 2025 - 1900, // ���ƫ�ƣ�1900��׼��
      .tm_mon = 4 - 1,        // �·ݷ�Χ0-11
      .tm_mday = 30,          // ��
      .tm_hour = 23,          // ʱ
      .tm_min = 59,           // ��
      .tm_sec = 55,           // ��
  };

  /* ����RTCʱ�� */
  KK_RTC_SetTime(&time);

  /* д���ʼ����־�����ݼĴ���DR1 */
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_INIT_FLAG);
}
