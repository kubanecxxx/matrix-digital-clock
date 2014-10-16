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
#include "packetHandling.h"
#include "wireless.h"


static void blik(arg_t);
static Scheduler s1(blik, NULL, MS2ST(3000));
static void config_matrix();
static void start_watchdog();

systime_t sysTime;

/*
 * Application entry point.
 */

int main(void)
{
	halInit();
	chSysInit();

	//start_watchdog();
	config_matrix();
	ma_init();
	wl_init();
	//s1.Register();

	while (TRUE)
	{
		Scheduler::Play();
		sysTime = chTimeNow();
		chThdSleepMilliseconds(1);
		ph.HandlePacketLoop();
	}

	return 1;
}

void blik(arg_t )
{
	//pwmEnableChannel(&PWMD2, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD2, ja));
	static int i ;

	animated_character_t * c = ma_get_character(C_LED);
	c->character =  (i&1) * L_LEFT_UPPER_CORNER;
	i++;
	return;

	if (i &1 )
		ma_select_function(DAY_TIME);
	else
		ma_select_function(NIGHT_TIME);

	i++;
}

void start_watchdog()
{
	 DBGMCU->CR |= DBGMCU_CR_DBG_IWDG_STOP;
	 IWDG->KR = 0x5555;
	 IWDG->PR = 6;
	 IWDG->RLR = 0xFFF;
	 IWDG->KR = 0xCCCC;
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

	static const PWMConfig pwmconfig =
	{ 24000000, 2400, NULL,
	{
	{ PWM_OUTPUT_ACTIVE_LOW, NULL },
	{ PWM_OUTPUT_DISABLED, NULL },
	{ PWM_OUTPUT_DISABLED, NULL },
	{ PWM_OUTPUT_DISABLED, NULL } }, 0 };

	static const matrix_config_t matrix_config =
	{ &SPID2, &GPTD2, gpio, &PWMD1, 0 };

	pwmStart(&PWMD1, &pwmconfig);

	DBGMCU->CR |= DBGMCU_CR_DBG_TIM2_STOP;
	matrix_init(&matrix_config);
	matrix_start();
}
