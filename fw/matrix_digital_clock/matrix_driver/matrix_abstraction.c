/**
 * @file matrix_abstraction.c
 * @author kubanec
 * @date 9. 10. 2014
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "fonts.h"
#include "matrix_driver.h"
#include "matrix_abstraction.h"
#include "scheduler.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint16_t buffer[MATRIX_ARRAY_COLS][MATRIX_ROWS][MATRIX_BRIGHT];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

animated_character_t time[6];

void ma_dayTime_loop(void)
{
	RTCTime t;
	rtcGetTime(&RTCD1, &t);
	uint32_t sec, min, hour, temp;

	sec = t.tv_sec % 60;
	temp = t.tv_sec / 60;

	min = temp % 60;
	temp = temp / 60;

	hour = temp % 24;
	hour += 2;

	animated_character_t * c = time;
	c->character = sec % 10 + 48;
	c++;
	c->character = sec / 10 + 48;
	c++;

	c->character = min % 10 + 48;
	c++;
	c->character = min / 10 + 48;
	c++;

	c->character = hour % 10 + 48;
	c++;
	c->character = hour / 10 + 48;

	c = time;
	int i;
	for (i = 0; i < 6; i++)
	{
		ma_putchar_animated(c++);
	}
}

void ma_cb(arg_t b)
{
	ma_clear_screen();
	ma_dayTime_loop();

	matrix_put_bitmap(buffer);

}

void ma_init(void)
{
	static delay_t d;
	shFillStruct(&d, ma_cb, NULL, MS2ST(20), PERIODIC);
	shRegisterStruct(&d);

	animated_character_t * c = time;
	int i;
	for (i = 0; i < 6; i++)
	{
		c->animation = SLIDE_DOWN;
		c->size = 16;
		c->y = 1;
		//c->x = 14 * i + (!(i & 1))* 2 - 3;
		c->x = 10 * i;

		c->character = '0';
		c->brightness = 7;
		c++;
	}
	time[0].animation = FADE;
}

void ma_clear_screen(void)
{
	memset(buffer, 0, MATRIX_ARRAY_COLS * MATRIX_ROWS * 2 * MATRIX_BRIGHT);
}

void ma_demo(void)
{
}

void ma_time()
{

}

void ma_putchar_animated(animated_character_t * data)
{
	if (data->animation == FADE)
	{
		if (data->counter < 16)
		{

			ma_putchar(data->character, data->x, data->y, data->size,
					data->counter/2);
			ma_putchar(data->old_character, data->x, data->y, data->size,
					7 - data->counter/2);
			data->counter++;
		}

		if (data->character != data->old_character_changed)
		{
			ma_putchar(data->old_character_changed, data->x, data->y,
					data->size, data->brightness);
			data->old_character = data->old_character_changed;
			data->old_character_changed = data->character;
			data->counter = 0;
		}
		if (data->counter >= 16)
		{
			ma_putchar_struct(data);
		}
	}
	else if (data->animation == SLIDE_DOWN)
	{
		if (data->counter < 1)
		{

			ma_putchar(data->character, data->x, data->y + data->counter,
					data->size, data->brightness);
			ma_putchar(data->old_character, data->x,
					data->y + data->size + data->counter, data->size,
					data->brightness);
			data->counter++;
		}

		if (data->character != data->old_character_changed)
		{
			ma_putchar(data->old_character_changed, data->x, data->y,
					data->size, data->brightness);
			data->old_character = data->old_character_changed;
			data->old_character_changed = data->character;
			data->counter = -16;
		}

		if (data->counter >= 0)
		{
			ma_putchar_struct(data);
		}
	}
	else if (data->animation == NONE)
	{
		ma_putchar_struct(data);
	}
}

void ma_putchar_struct(const animated_character_t * c)
{
	ma_putchar(c->character, c->x, c->y, c->size, c->brightness);
}

void ma_putchar(char c, uint16_t x, uint16_t y, uint8_t size,
		uint8_t brightness)
{
	chDbgAssert(brightness < 8, "brightness out of scope", 0);
	const uint8_t * d = Fonts_GetChar(c, 16);

	uint8_t znak[16][16];
	int j, i, z;
	for (i = 0; i < 16; i++)
	{
		z = 0;

		for (j = 0; j < 8; j++)
		{
			znak[i][j] = (*d >> z) & 1;
			z++;
		}
		d++;
	}

	for (i = 0; i < 16; i++)
	{
		z = 0;

		for (j = 8; j < 16; j++)
		{

			znak[i][j] = (*d >> z) & 1;
			z++;
		}
		d++;
	}

	uint8_t slowo;
	uint8_t biit;
	uint8_t xx, yy;
	for (j = 0; j < 16; j++)
	{
		for (i = 0; i < 16; i++)
		{
			xx = i + x;
			yy = y + j;
			if (znak[15 - i][j] && yy < MATRIX_ROWS && xx < MATRIX_COLS)
			{
				slowo = (xx) / 16;
				biit = 15 - ((xx) % 16);
				if (brightness & 1)
					buffer[slowo][yy][2] |= 1 << biit;
				if (brightness & 2)
					buffer[slowo][yy][1] |= 1 << biit;
				if (brightness & 4)
					buffer[slowo][yy][0] |= 1 << biit;

			}
		}
	}
}
