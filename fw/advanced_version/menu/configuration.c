#include "ch.h"
#include "configuration.h"
#include <string.h>
#include "crc8.h"
#include "stm32f10x_flash.h"

//select last flash page as an EEPROM
#define LAST_PAGE (FLASH_BASE + 1024 * 63)
static const configuration_t * global_configuration_flash = (configuration_t * ) LAST_PAGE;

//working configuration
configuration_t global_configuration;

static void Erase(uint32_t startAddress);
static void Write(uint32_t where, const void * datas,  uint32_t size);

//factory setting configuration
static const configuration_t factory_configuration =
{
    .ssid = "nic",
    .password = "nic",
    .source = SOURCE_MANUAL,
    .switch_type = SWITCH_TYPE_TIME,
    .toDay = {6,0},
    .toNight = {23,0},
    .maxLuminance = 40,
    .minLuminance = 10,
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
    Erase(LAST_PAGE);
    Write(LAST_PAGE, &global_configuration, sizeof(configuration_t));

    //save to flash

}

static uint16_t checksum(const configuration_t * c)
{
    uint8_t crc;
    uint8_t i;
    for (i = 0 ;i < sizeof(configuration_t)  - 2 ; i++)
    {
        crc8(&crc, *((uint8_t *)c + i));
    }
    return crc;
}

void Erase(uint32_t startAddress)
{
    FLASH_Unlock();

    if (FLASH_ErasePage(startAddress) != FLASH_COMPLETE)
    {
        asm("bkpt");
    }

    FLASH_Lock();
}

void Write(uint32_t where, const void * datas,  uint32_t size)
{
    FLASH_Unlock();
    uint16_t * data = (uint16_t *)datas;

    size = (size + 1) / 2;
    uint32_t i;

    for (i = 0 ; i < size ; i++)
    {
        if (FLASH_ProgramHalfWord(where,*data) != FLASH_COMPLETE)
        {
            asm("bkpt");
        }
        data++;
        where += 2;
    }

    FLASH_Lock();
}
