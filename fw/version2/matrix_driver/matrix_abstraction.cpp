/**
 * @file matrix_abstraction.c
 * @author kubanec
 * @date 9. 10. 2014
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
//#include "fonts.h"
#include "matrix_driver.h"
#include "matrix_abstraction.h"
#include "scheduler.h"
#include <string.h>
#include "pfont.h"
#include "rtc_control.h"
#include "wifi.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define DISPLAY_HEIGHT 16

//x coordination offset for nighttime
#define NIGHT_OFFSET 6

#define ma_animated_character_animation(c,_animation) {if (c->animation != _animation) \
    {c->animation = _animation; c->counter = 16;}}
#define ma_setup_animated_character(character_number,_animation,_character,_x,_y,_size,_brightness) \
        {time_c[character_number].animation = _animation; time_c[character_number].character = _character; \
        time_c[character_number].x = _x; time_c[character_number].y = _y; time_c[character_number].font = _size; \
        time_c[character_number].brightness = _brightness;}
#define ma_putchar_animated_number(character_number) {ma_putchar_animated(&time_c[character_number]);}

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint16_t buffer[MATRIX_ARRAY_COLS][MATRIX_ROWS][MATRIX_BRIGHT];
static animated_character_t time_c[ANIMATED_CHARS_COUNT];
static function_t func = DAY_TIME;

static const uint8_t font_sizes[] =
{ 8, 16, 16, 4 };

static uint16_t brightness = 4000;

extern uint8_t terminus16_bold[];
static const DECL_FONT(terminus16_bold_props,terminus16_bold,11,16,2);
piris::PFont terminus_bold(terminus16_bold_props);

/* Private function prototypes -----------------------------------------------*/

static void ma_putchar_animated(animated_character_t * data);
static void ma_putchar_struct(const animated_character_t * c);


/* Private functions ---------------------------------------------------------*/

static uint16_t b2;
void ma_set_brightness(uint16_t b)
{
    matrix_pwm_set_period(b);
}

inline uint16_t ma_brightness(void)
{
	return brightness;
}

inline void ma_dcf_led(uint8_t state)
{
	matrix_dcf_led(state);
}

inline void ma_dcf_progress(uint8_t percent)
{
	matrix_progress(percent);
}

void ma_disable(uint8_t disable)
{
	if (disable)
		matrix_stop();
	else
		matrix_start();
}
static uint16_t bdak;

void ma_time_loop(uint8_t day)
{
    ma_clear_screen();
    rtc_time_t t;
    rtc_control_GetTime(&t);
    uint32_t sec, min, hour;

	uint8_t temp;

    sec = t.seconds;
    min = t.minutes;
    hour = t.hours;

    animated_character_t * c = time_c;
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
	if (hour < 10)
		c->character = ' ';
	else
		c->character = hour / 10 + 48;


    c = time_c;
	int i;
	if (day)
	{
		for (i = 0; i < 6; i++)
		{
            c->brightness = 7;
			if (i == C_SECONDS_ONES)
			{
                ma_animated_character_animation(c, FADE);
                c->font = &piris::PFont::terminus12;
                c->x = 0;
                c->y = 4;
			}
            else if (i == C_SECONDS_TENS)
			{
                c->font = &piris::PFont::terminus12;
                ma_animated_character_animation(c, SLIDE_DOWN);
                c->x = 6;
                c->y = 4;
			}
            else
            {
                c->font = &terminus_bold;
                c->y = 0;

                uint8_t pos;
                if (i == C_MINUTES_ONES || i == C_MINUTES_TENS)
                    pos = 14 + ((i - 2) & 1) * c->font->width();
                else if (i == C_HOURS_ONES || i == C_HOURS_TENS)
                    pos = 18 + ((i & 1) + 2) * c->font->width();

                c->x = pos;
                ma_animated_character_animation(c, SLIDE_DOWN);
            }
            ma_putchar_animated(c++);
		}
		temp = ':';
        ma_setup_animated_character(C_DOUBLE_DOT1, FADE, temp, 30 + 2, 0, &piris::PFont::terminus16,
				7);
        ma_putchar_animated_number(C_DOUBLE_DOT1);
	}
	else
	{
        c = time_c + C_MINUTES_ONES;
		temp = 0;
		for (i = 0; i < 4; i++)
		{
			//c->animation = FADE;
			ma_animated_character_animation(c, FADE);
            c->font = &terminus_bold;
            c->y = 0;
			if (i > 1)
				temp = 8;
            c->x = temp + (c->font->width() * i) + NIGHT_OFFSET;

			c->brightness = 7;
			ma_putchar_animated(c++);

		}

		if (sec & 1)
			temp = ':';
		else
			temp = ' ';

        ma_setup_animated_character(C_DOUBLE_DOT1, FADE, temp, NIGHT_OFFSET+18,
                0, &terminus_bold, 7);
		ma_putchar_animated_number(C_DOUBLE_DOT1);
	}
    ma_buffer_flush();
}

static uint8_t mode_change_sequence = 0;

void ma_select_function(function_t f)
{
	if (f != func)
		mode_change_sequence = 0;

	func = f;

}

void ma_buffer_flush()
{
    //connected to the internet
    ma_put_pixel(wifi_internetReady(), 0,15,0);
    ma_put_pixel(wifi_internetReady(), 0,15,1);
    ma_put_pixel(wifi_internetReady(), 0,15,2);

    matrix_put_bitmap((uint16_t *) buffer);
}

void ma_init(void)
{
    animated_character_t * c = time_c;
	int i;
	for (i = 0; i < ANIMATED_CHARS_COUNT; i++)
	{
        //c->animation = SLIDE_DOWN;
        ma_animated_character_animation(c,SLIDE_DOWN);

        c->font = &piris::PFont::terminus16;
		c->y = 1;
		//c->x = 14 * i + (!(i & 1))* 2 - 3;
		c->x = 10 * i;

		c->character = '0';
		c->brightness = 7;
		c++;
	}
    ma_animated_character_animation(time_c,FADE);


//matrix_pwm_set_period(brightness);
}

void ma_clear_screen(void)
{
	memset(buffer, 0, MATRIX_ARRAY_COLS * MATRIX_ROWS * 2 * MATRIX_BRIGHT);
}

void ma_putchar_animated(animated_character_t * data)
{
    uint8_t sze = data->font->height();
	int8_t b, c;
	if (data->animation == FADE)
	{
        if (data->counter < DISPLAY_HEIGHT)
		{

			ma_putchar(data->character, data->x, data->y, data->font,
                    data->counter / 2);
			ma_putchar(data->old_character, data->x, data->y, data->font,
					7 - data->counter / 2);
			data->counter++;
		}

        if (data->character != data->old_character_changed)
		{
			ma_putchar(data->old_character_changed, data->x, data->y,
					data->font, data->brightness);
            data->old_character = data->old_character_changed;
			data->old_character_changed = data->character;
			data->counter = 0;
		}
		if (data->counter >= DISPLAY_HEIGHT)
		{
			ma_putchar_struct(data);
		}
	}
	else if (data->animation == SLIDE_DOWN)
	{
		if (data->counter < DISPLAY_HEIGHT)
		{
			c = MIN(data->counter, sze);
			ma_putchar(data->character, data->x, data->y + c - sze, data->font,
					data->brightness);
			if (data->counter < sze)
				ma_putchar(data->old_character, data->x, data->y + c,
						data->font, data->brightness);
			data->counter++;
		}

		if (data->character != data->old_character_changed)
		{
			ma_putchar(data->old_character_changed, data->x, data->y,
					data->font, data->brightness);
			data->old_character = data->old_character_changed;
			data->old_character_changed = data->character;
			data->counter = 0;
		}

		if (data->counter >= DISPLAY_HEIGHT)
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
	ma_putchar(c->character, c->x, c->y, c->font, c->brightness);
}

static const uint8_t led_table[4][4] =
{
{ 0, 0, 0, 0 },
{ 0x1, 0x3, 0x7, 0xf },
{ 0xf, 0x7, 0x3, 0x1 }, };

void ma_putchar(char c, uint16_t x, uint16_t y, const piris::PFont * f,
		uint8_t brightness)
{
    chDbgAssert(brightness < 8, "brightness out of scope");

    //uint8_t width = ma_font_size(font);
    //uint8_t height = width;

    uint8_t width = f->width();
    uint8_t height = f->height();

    uint16_t znak[16];
	int j, i, z;
    uint32_t line;

    if (c != 31)
    {
        for (i = 0; i < height; i++)
        {
            line = f->line(c, i);
            znak[i] = line ;
        }
    }
    else
    {
        //null character
        for (int i = 0 ; i < 16; i+=2)
        {
            znak[i] = 0x5555;
            znak[i] = 0xaaaa;
        }
    }

	uint8_t slowo;
	uint8_t biit;
	uint8_t xx, yy;

    if (brightness < 7)
    {
        asm("nop");
    }

	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			xx = i + x;
			yy = y + j;
            if ((znak[j] & (1<<i))&& yy < MATRIX_ROWS && xx < MATRIX_COLS)
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

void ma_put_pixel(uint8_t on, uint16_t x, uint16_t y, uint8_t bright)
{
    chDbgAssert(x < MATRIX_COLS, "out of scope");
    chDbgAssert(y < MATRIX_ROWS, "out of scope");
    chDbgAssert(bright< MATRIX_BRIGHT, "out of scope");
	uint8_t slowo = x / 16;
	uint8_t biit = 15 - (x % 16);
	if (on)
		buffer[slowo][y][bright] |= 1 << biit;
	else
		buffer[slowo][y][bright] &= (~1 << biit);
}

inline animated_character_t * ma_get_character(uint8_t index)
{
    chDbgAssert(index < ANIMATED_CHARS_COUNT, "index of array out of scope");
    return time_c + index;
}

const uint8_t matrix_table[7] =
{ 0, 0, 0, 0, 1, 1, 2 };

static uint16_t current_row = 0;

