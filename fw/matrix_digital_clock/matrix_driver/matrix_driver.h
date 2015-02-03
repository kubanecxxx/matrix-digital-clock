/**
 * @file matrix_driver.h
 * @author kubanec
 * @date 8. 10. 2014
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MATRIX_DRIVER_H_
#define MATRIX_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hal.h"
/* Exported types ------------------------------------------------------------*/
typedef struct
{
	GPIO_TypeDef * port;
	uint8_t pin;
} matrix_gpio_t;

typedef struct matrix_config_t matrix_config_t;

struct matrix_config_t
{
	SPIDriver * spi;
	GPTDriver * timer;
	const matrix_gpio_t * gpio;
	PWMDriver * pwm;
	uint8_t pwmChannel;
};

/* Exported constants --------------------------------------------------------*/
#define MATRIX_ROWS 16
#define MATRIX_COLS 96
#define MATRIX_ARRAY_COLS (MATRIX_COLS / 16)
#define MATRIX_BRIGHT 3

extern const uint8_t matrix_table[7];


/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void matrix_init(const matrix_config_t * config);
void matrix_start(void);
void matrix_stop(void);
void matrix_demo(void);

void matrix_clear_screen(void);
//void matrix_put_pixel(uint8_t on, uint16_t x, uint16_t y, uint8_t bright);
void matrix_put_bitmap(const uint16_t * data);
//void matrix_put_line(const uint16_t * data, uint8_t row,uint8_t bright);
void matrix_pwm_set_period(uint16_t percent);


#ifdef __cplusplus
}
#endif

#endif /* MATRIX_DRIVER_H_ */
