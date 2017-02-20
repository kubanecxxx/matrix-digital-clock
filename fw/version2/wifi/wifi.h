#ifndef __WIFI_H__
#define __WIFI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "rtc_control.h"



void wifi_Init(void);
uint8_t wifi_getTime(rtc_time_t * time);
uint8_t wifi_internetReady(void);

#ifdef __cplusplus
}
#endif

#endif
