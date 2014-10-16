/**
 * @file wireless.c
 * @author kubanec
 * @date 16. 10. 2014
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "RF24.h"
#include "RF24app.h"
#include "table_conf.h"
#include "packetHandling.h"
#include "matrix_driver.h"
#include "matrix_abstraction.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void new_time_cb(packetHandling * h, nrf_commands_t command, void * load,
		uint8_t size, void * userData);
static void rf_state(bool state);

/* Private variables ---------------------------------------------------------*/
static const NRF24Config cfg =
{
RF_CE_PORT, RF_CS_PORT, RF_CE_PIN, RF_CS_PIN };

RF24 rf(&SPID1, &cfg);
RF24_app app(&rf, config_table_watches.address + config_table_watches.pipe,
		config_table_watches.channel);

static const packetHandling::callback_table_t table[] =
{
{ TIME_SINCE_EPOCH, new_time_cb, NULL },
{ IDLE, NULL, NULL } };

packetHandling ph(&app, table);

/* Private functions ---------------------------------------------------------*/

void wl_init(void)
{
	animated_character_t * c = ma_get_character(C_LED);
	c->font = P_LED;
	c->x = MATRIX_COLS - 4;
	c->y = 0;
	c->animation = FADE;
	c->character = 0;

	static const SPIConfig spicfg =
	{
	NULL, NULL, 0, SPI_CR1_BR_1 };

	static const packetHandling::function_table_t t =
	{
	NULL, NULL, rf_state };

	//kruciální řádky - odpojit jtag; nechat jenom swd - sou na nem piny pro SPI1
	//premapovat SPI1 na PB3;4;5
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	AFIO->MAPR |= AFIO_MAPR_SPI1_REMAP;
	AFIO->MAPR |= 0b010 << 24;

	palSetPadMode(RF_MOSI_PORT, RF_MOSI_PIN, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(RF_MISO_PORT, RF_MISO_PIN, PAL_MODE_INPUT);
	palSetPadMode(RF_SCK_PORT, RF_SCK_PIN, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(RF_CS_PORT, RF_CS_PIN, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(RF_CE_PORT, RF_CE_PIN, PAL_MODE_OUTPUT_PUSHPULL);

	spiStart(&SPID1, &spicfg);
	app.start();

	ph.setFunctionTable(&t);

	ph.RequestData(STARTUP);
	ph.StartAutoIdle();

}

void rf_state(bool state)
{
	animated_character_t * c = ma_get_character(C_LED);
	c->character = state * L_LEFT_UPPER_CORNER;
}

void new_time_cb(packetHandling * h, nrf_commands_t command, void * load,
		uint8_t size, void * userData)
{
	if (size != 4)
		return;

	uint32_t epoch = *(uint32_t *) load;
	RTCTime t;
	t.tv_sec = epoch;
	t.tv_msec = 0;
	rtcSetTime(&RTCD1, &t);
}
