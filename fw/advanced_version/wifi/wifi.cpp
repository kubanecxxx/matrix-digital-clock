#include "ch.h"
#include "hal.h"
#include "wifi.h"
#include "esp8266.h"
#include "matrix_abstraction.h"
#include "scheduler.hpp"
#include "configuration.h"
#include <string.h>

static char buffer[700];
static void watchdog_reset(void);
static void setup_watchdog(void);

static uint8_t flash = 0;
static uint8_t ok;
static uint8_t time_ready;

static rtc_time_t internetTime;

static esp_config_t cfg =
{
    &SD1, watchdog_reset, "wifi", "asdfghjkl", 300
};

static void parseTime(const char * text);
static void send_ntp_request(uint8_t connection_id);

static const SerialConfig ser_cfg =
{
    19200,0,0,0
};

uint16_t failed ;

static THD_WORKING_AREA(waThread1, 512);
static THD_FUNCTION(Thread1, arg)
{
    chRegSetThreadName("wifi");
    (void )arg;
    uint8_t len;
    uint8_t id;
    uint8_t k;
    uint8_t listen = 0;
    systime_t old=0xfffff,now;
    systime_t lis;

    esp_run_command("AT+CIOBAUD=19200",200,NULL,0);
    chThdSleepMilliseconds(100);
    sdStop(&SD1);
    sdStart(&SD1, &ser_cfg);

    while(1)
    {
        watchdog_reset();
        ok = esp_keep_connected_loop(8081,0);

        if (ok)
        {


            now = chVTGetSystemTime();
            if (listen && now - lis > S2ST(5))
            {
                old = S2ST(121)+now;
                lis = chVTGetSystemTime();
                failed++;
            }

            //synchronize time from internet
            if (((now - old > S2ST(120)) && ok && now > S2ST(10)))
            {
                old = now;
                listen = 1;
                k = esp_run_command("AT+CIPCLOSE=4", 1000,buffer,sizeof(buffer));
                k = esp_run_command("AT+CIPSTART=4,\"UDP\",\"tak.cesnet.cz\",123",1000,buffer,sizeof(buffer));
                if (k)
                {
                    send_ntp_request(4);
                    //wait for data
                    chThdSleepMilliseconds(200);
                }
            }
            len = esp_decode_ipd(buffer,&id);
            if (len)
            {
                if (id == 4 && len ==48)
                {
                    k = esp_run_command("AT+CIPCLOSE=4", 1000,buffer,sizeof(buffer));
                    parseTime(buffer);
                    if (time_ready)
                        listen = 0;
                }
                else
                {
                   esp_basic_commands(buffer,len,id);
                }
            }
        }
    }
}

void send_ntp_request(uint8_t connection_id)
{
    char req[48];
    memset(req,0,sizeof(req));
    req[0] = 0b11100011;
    req[1] = 0;
    req[2] = 6;
    req[3] = 0xec;

    req[12] = 49;
    req[13] = 0x4E;
    req[14] = 49;
    req[15] = 52;

    esp_write_udp(req,sizeof(req),connection_id);
}

void parseTime(const char *data)
{
    uint32_t tme;
    tme = data[40] << 24;
    tme |= data[41] << 16;
    tme |= data[42] << 8;
    tme |= data[43] ;

    uint16_t h,m,s;
    uint16_t mon, year, day;

    //since 1.1.1900 to 1.1.1970
    tme = tme - 2208988800;
    h = (tme % 86400) / 3600 ;
    h = h + 1;
    m = (tme % 3600) / 60;
    s = tme % 60;

    mon = (tme % 31556926) / 2629743 + 1;
    year = tme / 31556926 + 1970 ;
    day = ((tme % 31556926) %  2629743) / 86400 + 1;


    uint16_t start = 31 - ((4 +5 * year / 4) % 7);
    uint16_t stop = 31 - ((1 + 5 * year / 4 ) % 7);

    if ((mon == 3 && start >= day ) || ( mon == 10 && day < stop ) || (mon > 3 && mon < 10))
    {
        h++;
    }


    internetTime.hours = h;
    internetTime.minutes = m;
    internetTime.seconds = s;
    time_ready = 1;
}




void wifi_Init(void)
{
    //setup_watchdog();
    //enable uart
    AFIO->MAPR |= AFIO_MAPR_USART1_REMAP;
    sdStart(&SD1,NULL);
    palSetPadMode(GPIOB, 7, PAL_MODE_INPUT);
    palSetPadMode(GPIOB, 6, PAL_MODE_STM32_ALTERNATE_PUSHPULL);

    //setup ssid and passwd
    cfg.essid  = global_configuration.ssid;
    cfg.password = global_configuration.password;

    //setup esp
    esp_set_sd(&cfg);

    //start dedicated thread
    chThdCreateStatic(waThread1,sizeof(waThread1), LOWPRIO, Thread1,NULL);
}

uint8_t wifi_internetReady()
{
    return ok;
}

uint8_t wifi_getTime(rtc_time_t *time)
{
    //create request
    //send it by GET
    //wait for it in background task
    //when ready master will read it and ready=0

    if (!time_ready)
        return 0;

    memcpy(time, &internetTime, sizeof(rtc_time_t));
    time_ready = 0;
    return 1;

}

void setup_watchdog(void)
{
    IWDG->KR = 0x5555;

    //watchdog timeout is about 2 seconds
    IWDG->PR = 2;
    IWDG->RLR = 0xfff;
#ifdef STM32F4xx_MCUCONF
    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_IWDG_STOP;
#elif defined STM32F100_MCUCONF
    DBGMCU->CR |= DBGMCU_CR_DBG_IWDG_STOP;
#endif


    IWDG->KR = 0xCCCC;
}

void watchdog_reset(void)
{
    IWDG->KR = 0xAAAA;
}
