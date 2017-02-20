/**
 * @file matrix_abstraction.h
 * @author kubanec
 * @date 9. 10. 2014
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MATRIX_ABSTRACTION_H_
#define MATRIX_ABSTRACTION_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef enum
{
	NONE, SLIDE_DOWN, FADE
} animation_t;

typedef enum
{
	P_8, P_16, P_WEATHER, P_LED, P_LAST
} fonts_t;

typedef struct
{
//user setup
	animation_t animation;
	char character;
	uint8_t x;
	uint8_t y;
	fonts_t font;
	uint8_t brightness;

//internal data
	uint8_t changed;
	char old_character;
	char old_character_changed;
	int8_t counter;

} animated_character_t;

typedef enum
{
	DAY_TIME, NIGHT_TIME, WEATHER, HOME_AUTOMATION
} function_t;

typedef enum
{
	L_NONE,L_RIGHT_UPPER_CORNER,L_LEFT_UPPER_CORNER
} led_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define ANIMATED_CHARS_COUNT 8

#define C_SECONDS_ONES 	0
#define C_SECONDS_TENS 	1
#define C_MINUTES_ONES 	2
#define C_MINUTES_TENS 	3
#define C_HOURS_ONES	4
#define C_HOURS_TENS	5
#define C_DOUBLE_DOT1	6
#define C_DOUBLE_DOT2	7
#define C_LED			8

/* Exported functions --------------------------------------------------------*/

void ma_init(void);
void ma_select_function(function_t f);
void ma_write_ambient_lumination(uint16_t luminance);
void ma_put_pixel(uint8_t on, uint16_t x, uint16_t y, uint8_t bright);
animated_character_t * ma_get_character(uint8_t index);
void ma_set_brightness(uint16_t percent);
uint16_t ma_brightness(void);


void ma_dcf_led(uint8_t state);
void ma_dcf_progress(uint8_t percent);
void ma_disable(uint8_t disable);

#ifdef __cplusplus
}
#endif

#endif /* MATRIX_ABSTRACTION_H_ */
