#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

typedef struct
{
    uint8_t hours;
    uint8_t minutes;
} config_time_t;

#define SOURCE_MANUAL 0
#define SOURCE_RTC 1
#define SOURCE_WIFI 2

#define SWITCH_TYPE_TIME 0
#define SWITCH_TYPE_PHOTO 1

typedef struct {
    //wifi essid
    //wifi password
    //rtc source manual/dcf/wifi
    //day/night switch time
    //day/night switch type - photores/time
    //day/night max/min luminance
    //photo voltage day
    //photo voltage night
    char ssid[32];
    char password[32];
    uint8_t source;
    uint8_t switch_type;
    config_time_t toDay;
    config_time_t toNight;
    uint16_t maxLuminance;
    uint16_t minLuminance;
    uint16_t photoDay;
    uint16_t photoNight;
    uint32_t crc;
} configuration_t;


void config_save(void);
void config_retrieve(void);



#endif
