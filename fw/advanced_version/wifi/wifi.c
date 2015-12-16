#include "ch.h"
#include "hal.h"
#include "wifi.h"

void wifi_Init(void)
{
    //enable uart
    //connect to wifi
    //test internet
}

uint8_t wifi_getTime(rtc_time_t *time)
{
    //create request
    //send it by GET
    //wait for it in background task
    //when ready master will read it and ready=0

}
