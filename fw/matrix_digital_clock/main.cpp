/*
 ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "ch.h"
#include "hal.h"
#include "scheduler.hpp"
#include "matrix_driver.h"
#include "matrix_abstraction.h"

/*
 static SPIConfig config =
 { NULL, CS_PORT, CS_PIN, SPI_BR };
 */

//IRQ PC5
//CE PA2
//CS PA4
/*
 static NRF24Config c =
 { CE_PORT, CS_PORT, CE_PIN, CS_PIN };


 //no fear konstruktor se zavolá
 RF24 rf(&SPID1, &c);

 */
static const uint64_t pipe = 0x7878787878LL;

void blik(arg_t a)
{

	//pwmEnableChannel(&PWMD2, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD2, ja));

	ma_demo();

}

/*
 * PWM configuration structure.
 * Cyclic callback enabled, channels 3 and 4 enabled without callbacks,
 * the active state is a logic one.
 */
static PWMConfig pwmcfg =
{ 1000000, /* 10kHz PWM clock frequency.   */
1000, /* PWM period 1S (in ticks).    */
NULL,
{
{ PWM_OUTPUT_DISABLED, NULL },
{ PWM_OUTPUT_DISABLED, NULL },
{ PWM_OUTPUT_ACTIVE_HIGH, NULL },
{ PWM_OUTPUT_DISABLED, NULL } },
/* HW dependent part.*/
0, 0,
#if STM32_PWM_USE_ADVANCED
		0
#endif
	};

/*
 * Application entry point.
 */
systime_t sysTime;

static Scheduler s1(blik, NULL, MS2ST(500));
//static serial ser(&SD1, &rf);

static void config_matrix();

int main(void)
{

	halInit();
	chSysInit();
	rtcInit();

	RTCTime t;
	t.tv_sec = 1412876280;
	//rtcSetTime(&RTCD1, &t);

	//kruciální řádky - odpojit jtag; nechat jenom swd - sou na nem piny pro SPI1
	//premapovat SPI1 na PB3;4;5
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	AFIO->MAPR |= AFIO_MAPR_SPI1_REMAP;
	AFIO->MAPR |= 0b010 << 24;

#if 0
	//kruciální řádky - odpojit jtag; nechat jenom swd - sou na nem piny pro SPI1
	//premapovat SPI1 na PB3;4;5
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	AFIO->MAPR |= AFIO_MAPR_SPI1_REMAP;
	AFIO->MAPR |= 0b010 << 24;

	spiStart(&SPID1, &config);
	palSetPadMode(config.ssport, config.sspad, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(SPI_SCK_PORT, SPI_SCK_PIN, SPI_SCK_MODE);
	palSetPadMode(SPI_MISO_PORT, SPI_MISO_PIN, SPI_MISO_MODE);
	palSetPadMode(SPI_MOSI_PORT, SPI_MOSI_PIN, SPI_MOSI_MODE);
#endif

	//setup watchdog
	/*
	 DBGMCU->CR |= DBGMCU_CR_DBG_IWDG_STOP;
	 IWDG->KR = 0x5555;
	 IWDG->PR = 6;
	 IWDG->RLR = 0xFFF;
	 IWDG->KR = 0xCCCC;
	 */

	config_matrix();
	ma_init();

#if 1

#endif
	while (TRUE)
	{
		Scheduler::Play();
		sysTime = chTimeNow();
	}

	return 1;
}

void config_matrix()
{
	static const matrix_gpio_t gpio[16] =
	{
	{ GPIOA, 10 },
	{ GPIOA, 9 },
	{ GPIOB, 14 },
	{ GPIOB, 12 },
	{ GPIOB, 11 },
	{ GPIOB, 10 },
	{ GPIOB, 2 },
	{ GPIOB, 1 },
	{ GPIOB, 0 },
	{ GPIOA, 7 },
	{ GPIOA, 6 },
	{ GPIOA, 5 },
	{ GPIOA, 4 },
	{ GPIOA, 3 },
	{ GPIOA, 2 },
	{ GPIOA, 1 } };

	static const matrix_config_t matrix_config =
	{ &SPID2, &GPTD1, gpio };

	DBGMCU->CR |= DBGMCU_CR_DBG_TIM1_STOP;
	matrix_init(&matrix_config);
	matrix_start();
}
