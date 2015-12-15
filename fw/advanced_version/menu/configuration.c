#include "ch.h"
#include "configuration.h"
#include <string.h>

//select last flash page as an EEPROM
const configuration_t * global_configuration_flash = (configuration_t *) 0x08000000;

//working configuration
configuration_t global_configuration;

//factory setting configuration
static const configuration_t factory_configuration;

static uint32_t checksum(const configuration_t * c);

void config_retreive(void)
{
    //if checksum is not valid, load factory setting configuration
    memcpy(&global_configuration, global_configuration_flash, sizeof(configuration_t));
}

//save whole struct into flash page
void config_save(void)
{

}

static uint32_t checksum(const configuration_t * c)
{

    return 0;
}
