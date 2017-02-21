/**
 * @file dcf.h
 * @author kubanec
 * @date 8. 1. 2015
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DCF_DCF_H_
#define DCF_DCF_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void dcf_init(void);
void dcf_play(void);
uint32_t dcf_getTime(void);
uint8_t dcf_ready(void);
uint8_t dcf_valid(void);

#ifdef __cplusplus
}
#endif

#endif /* DCF_DCF_H_ */
