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


#define DCF_PORT GPIOB
#define DCF_PIN	6
#define DCF_TIMER  GPTD3

#define BUTTON_ENTER_PORT GPIOB
#define BUTTON_ENTER_PIN 4
#define BUTTON_LEFT_PORT GPIOB
#define BUTTON_LEFT_PIN 5
#define BUTTON_RIGHT_PORT GPIOA
#define BUTTON_RIGHT_PIN 15

#define WIFI_UART SD1;
#define WIFI_UART_BAUDRATE 115200

#define WIFI_RX_PORT GPIOB
#define WIFI_RX_PIN  1
#define WIFI_TX_PORT GPIOB
#define WIFI_TX_PIN 2


#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_H_ */
