/**
 * @file platform.h
 * @author kubanec
 * @date 8. 4. 2014
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PLATFORM_H_
#define PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


#define STCP_PORT	GPIOB
#define STCP_PIN	8

#define MR_PORT		GPIOB
#define MR_PIN		9

#define OE_PORT 	GPIOA
#define OE_PIN		8

#define MATRIX_CLOCK_PORT	GPIOB
#define MATRIX_CLOCK_PIN	13

#define MATRIX_DATA_PORT	GPIOB
#define MATRIX_DATA_PIN		15


#define RF_CS_PORT	GPIOB
#define RF_CS_PIN	6
#define RF_CE_PORT	GPIOA
#define RF_CE_PIN	15

#define RF_MOSI_PORT	GPIOB
#define RF_MISO_PORT	GPIOB
#define RF_SCK_PORT		GPIOB
#define RF_MOSI_PIN		5
#define RF_MISO_PIN		4
#define RF_SCK_PIN		3

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_H_ */
