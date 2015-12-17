#ifndef __RTC_CONTROL_H__
#define __RTC_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint32_t secs;
} rtc_time_t;


void rtc_control_GetTime(rtc_time_t * out);
void rtc_control_SetTime(const rtc_time_t * in);
void rtc_control_Init(void);

#ifdef __cplusplus
}
#endif


#endif
