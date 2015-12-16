#ifndef __WIFI_H__
#define __WIFI_H__

#include "rtc_control.h"

void wifi_Init(void);
uint8_t wifi_getTime(rtc_time_t * time);
uint8_t wifi_internetReady(void);

#endif
