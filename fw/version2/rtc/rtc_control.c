#include "ch.h"
#include "hal.h"
#include "rtc_control.h"
#include "configuration.h"
#include "scheduler.h"
#include "wifi.h"

static delay_t d;
static void rtc_control_task(arg_t  a);

void rtc_control_GetTime(rtc_time_t * in)
{
    uint32_t tv_sec, tv_msec;
    rtcSTM32GetSecMsec(&RTCD1, &tv_sec, &tv_msec);

    uint32_t secs = tv_sec % (3600 * 24);
    in->secs = secs;
    in->hours = secs / 3600 ;
    secs = secs % 3600;
    in->minutes = secs / 60;
    in->seconds = secs % 60;

}

void rtc_control_SetTime(const rtc_time_t * out)
{
    uint32_t secs = out->seconds + out->hours*3600 + out->minutes*60;
    rtcSTM32SetSec(&RTCD1, secs);
}

void rtc_control_Init(void)
{
    //start selected module during startup
    if (global_configuration.source == SOURCE_MANUAL)
    {
        //do nothing
    }
    else if (global_configuration.source == SOURCE_WIFI)
    {
        //start wifi module
        wifi_Init();

    }
    else if (global_configuration.source == SOURCE_DCF)
    {
        //start dcf module
    }

    shFillStruct(&d, rtc_control_task,(arg_t)(uint32_t)global_configuration.source,MS2ST(200), PERIODIC);
    shRegisterStruct(&d);
}


void rtc_control_task(arg_t a)
{
    uint32_t source = (uint32_t ) a;
    rtc_time_t time;
    uint8_t ok;

    //auto refresh time from DCF/Manual/wifi

    if (source == SOURCE_MANUAL)
    {
        //do nothing - time will be set from Menu during save
    }
    else if(source == SOURCE_DCF)
    {
        //disable matrix_driver
        //start progresbar driver
        //synchronize every 2am and right after startup
        //set rtc time after dcf time is ready
        //when ready enable matrix_driver again

    }
    else if (source == SOURCE_WIFI)
    {
        //synchronize every 2am and right after startup
        //set rtc time when wifi is ready
        ok = wifi_getTime(&time);
        if (ok)
            rtc_control_SetTime(&time);
    }


}
