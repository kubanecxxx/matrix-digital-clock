/**
 * @file crc8.h
 * @author kubanec
 * @date 9. 1. 2014
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CRC8_H_
#define CRC8_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void crc8(uint8_t *crc, uint8_t m);
void init_crc8(void);

#ifdef __cplusplus
}
#endif

#endif /* CRC8_H_ */
