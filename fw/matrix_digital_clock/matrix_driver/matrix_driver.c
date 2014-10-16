/**
 * @file matrix_driver.c
 * @author kubanec
 * @date 8. 10. 2014
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "hal.h"
#include "ch.h"
#include "matrix_driver.h"

static void gpt_cb(GPTDriver * gpt);

static const matrix_config_t * c;

static const GPTConfig gpt =
{ 1000000, gpt_cb, 0 };

static const SPIConfig spi =
{

NULL, NULL, 0, SPI_CR1_DFF };

static uint16_t display_data[MATRIX_ARRAY_COLS][MATRIX_ROWS][MATRIX_BRIGHT];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void matrix_demo(void)
{
	static uint8_t a = 48;
	a++;
	int i, j;
	uint16_t pattern;
	uint16_t p1, p2;

	for (i = 0; i < MATRIX_ROWS; i += 1)
	{
		if (a % 2 == 0)
		{
			p1 = 0xaaaa;
			p2 = 0x5555;
		}
		else
		{
			p1 = 0x5555;
			p2 = 0xaaaa;
		}

		if (i % 2 == 0)
			pattern = p1;
		else
			pattern = p2;

		for (j = 0; j < MATRIX_ARRAY_COLS; j++)
			display_data[j][i][0] = pattern;
	}
}

const uint8_t matrix_table[7] =
{ 0, 0, 0, 0, 1, 1, 2 };

static uint8_t inter = 0;
void gpt_cb(GPTDriver * gpt)
{
	inter = 1;
	(void) gpt;
	int i;
	static uint16_t current_row = 0;
	static uint8_t brigh_count = 0;
	uint16_t old_row = current_row;
	uint16_t r;
	r = matrix_table[brigh_count];
	current_row++;

	if (current_row > 15)
	{
		current_row = 0;
		brigh_count++;
		if (brigh_count >= 8)
		{
			brigh_count = 0;

		}

	}

	palClearPad(c->gpio[old_row].port, c->gpio[old_row].pin);
	palClearPad(STCP_PORT, STCP_PIN);

	for (i = 0; i < MATRIX_ARRAY_COLS; i++)
	{
		spiPolledExchange(c->spi, display_data[i][current_row][r]);
	}

	palSetPad(STCP_PORT, STCP_PIN);
	palSetPad(c->gpio[current_row].port, c->gpio[current_row].pin);
	inter = 0;
}

void matrix_init(const matrix_config_t * cfg)
{
	c = cfg;
	matrix_demo();
}

void matrix_pwm_set_period(uint16_t percent)
{
	pwmEnableChannel(c->pwm, c->pwmChannel,
			PWM_PERCENTAGE_TO_WIDTH(c->pwm, percent));
}

void matrix_start()
{
	//config gpio
	int i;
	for (i = 0; i < 16; i++)
	{
		palSetPadMode(c->gpio[i].port, c->gpio[i].pin,
				PAL_MODE_OUTPUT_PUSHPULL);
		palClearPad(c->gpio[i].port, c->gpio[i].pin);
	}

	palSetPadMode(STCP_PORT, STCP_PIN, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(MR_PORT, MR_PIN, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(OE_PORT, OE_PIN, PAL_MODE_STM32_ALTERNATE_PUSHPULL);

	//palClearPad(OE_PORT, OE_PIN);
	palClearPad(MR_PORT, MR_PIN);
	chThdSleepMilliseconds(1);

	palSetPad(MR_PORT, MR_PIN);

	//config timer
	gptStart(c->timer, &gpt);
	gptStartContinuous(c->timer, 100);

	//config spi
	palSetPadMode(MATRIX_CLOCK_PORT, MATRIX_CLOCK_PIN,
			PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	palSetPadMode(MATRIX_DATA_PORT, MATRIX_DATA_PIN,
			PAL_MODE_STM32_ALTERNATE_PUSHPULL);
	spiStart(c->spi, &spi);
}

void matrix_clear_screen()
{
	int i, j, k;
	for (i = 0; i < MATRIX_ARRAY_COLS; i++)
	{
		for (j = 0; j < MATRIX_ROWS; j++)
		{
			for (k = 0; k < MATRIX_BRIGHT; k++)
				display_data[i][j][k] = 0;
		}
	}
}
#if 0
void matrix_put_pixel(uint8_t on, uint16_t x, uint16_t y, uint8_t bright)
{
	chDbgAssert(x < MATRIX_COLS, "out of scope", 0);
	chDbgAssert(y < MATRIX_ROWS, "out of scope", 0);
	chDbgAssert(bright< MATRIX_BRIGHT, "out of scope", 0);
	uint8_t slowo = x / 16;
	uint8_t biit = 15 - (x % 16);
	if (on)
		display_data[slowo][y][bright] |= 1 << biit;
	else
		display_data[slowo][y][bright] &= (~1 << biit);
}
#endif

// 16 columns in one word,
void matrix_put_bitmap(const uint16_t * data)
{
	//memcpy(&display_data[0][0][brig],data,MATRIX_ARRAY_COLS*MATRIX_ROWS);
	int i, j, k;
	for (i = 0; i < MATRIX_ARRAY_COLS; i++)
	{
		for (j = 0; j < MATRIX_ROWS; j++)
		{
			for (k = 0; k < MATRIX_BRIGHT; k++)
				display_data[i][j][k] = *data++;
		}
	}
}

#if 0
void matrix_put_line(const uint16_t * data, uint8_t row, uint8_t bright)
{
	int i;
	for (i = 0; i < MATRIX_ARRAY_COLS; i++)
	{
		display_data[i][row][bright] = *data++;
	}
}
#endif

