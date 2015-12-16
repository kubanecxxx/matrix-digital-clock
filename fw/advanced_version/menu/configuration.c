#include "ch.h"
#include "configuration.h"
#include <string.h>
#include "crc8.h"

//select last flash page as an EEPROM
static const configuration_t * global_configuration_flash = (configuration_t *) 0x08000000;

//working configuration
configuration_t global_configuration;

//factory setting configuration
static const configuration_t factory_configuration =
{
    .ssid = "nic",
    .password = "nic",
    .source = SOURCE_MANUAL,
    .switch_type = SWITCH_TYPE_TIME,
    .toDay = {6,0},
    .toNight = {22,0},
    .maxLuminance = 100,
    .minLuminance = 20,
    .photoDay = 200,
    .photoNight = 100,
    .crc = 243
};

static uint16_t checksum(const configuration_t * c);

void config_retrieve(void)
{
    //if checksum is not valid, load factory setting configuration
    uint8_t i;
    uint8_t crc;
    //uint8_t crc2;
    //uint8_t sze = sizeof(configuration_t);
    for (i = 0 ;i < sizeof(configuration_t) ; i++)
    {
        crc8(&crc, *((uint8_t *)global_configuration_flash + i));
        //crc8(&crc2, *((uint8_t *)&factory_configuration + i));
    }

    if (!crc)
    {
        memcpy(&global_configuration, global_configuration_flash, sizeof(configuration_t));
    }
    else
    {
        memcpy(&global_configuration, &factory_configuration, sizeof(configuration_t));
    }


}

//save whole struct into flash page
void config_save(void)
{
    //compute crc and save
    global_configuration.crc = checksum(&global_configuration);

    //save to flash
}

static uint16_t checksum(const configuration_t * c)
{
    uint16_t crc;
    uint8_t i;
    for (i = 0 ;i < sizeof(configuration_t)  - 2 ; i++)
    {
        crc8(&crc, *((uint8_t *)c + i));
    }
    return crc;
}
