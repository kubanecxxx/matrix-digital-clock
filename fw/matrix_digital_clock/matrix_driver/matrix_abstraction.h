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

typedef struct
{
	//user setup
	animation_t animation;
	char character;
	uint8_t x;
	uint8_t y;
	uint8_t size;
	uint8_t brightness;

	//internal data
	uint8_t changed;
	char old_character;
	char old_character_changed;
	int8_t counter;

} animated_character_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

//void ma_putchar(char c, uint16_t x, uint16_t y);
void ma_demo(void);
void ma_time(void);
void ma_init(void);

void ma_putchar_struct(const animated_character_t * c);
void ma_putchar(char c, uint16_t x, uint16_t y, uint8_t size, uint8_t brightness);

#ifdef __cplusplus
}
#endif

#endif /* MATRIX_ABSTRACTION_H_ */
