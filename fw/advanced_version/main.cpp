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

#include "menu.h"

static void blik(arg_t);
static void cb(arg_t);
static Scheduler s1(blik, NULL, MS2ST(3000));
static Scheduler s2(cb, NULL, MS2ST(10000), ONCE);
static void config_matrix();
static void start_watchdog();

systime_t sysTime;

/*
 * Application entry point.
 */

static Menu menu;

int main(void)
{
    halInit();
    chSysInit();

    config_matrix();
    //ma_init();
    ma_set_brightness(200);
    //ma_select_function(DAY_TIME);

    menu.Init();
/*
	s2.Register();
*/
    //matrix_demo();
	while (TRUE)
	{


        Scheduler::Play();
        sysTime = chVTGetSystemTime();
        chThdSleepMilliseconds(1);
        asm("nop");
	}


	return 1;
}


void cb(void * a)
{
	ma_set_brightness(0);
	//dcf_play();
    s1.Register();

}


#if 1

void blik(void * a)
{             /*
    if (dcf_ready())
    {
        //daytime
        if (dcf_getTime() > 21600 && dcf_getTime() < 79200)
        {
            ma_select_function(DAY_TIME);
            ma_set_brightness(2000);
        }
        else
        {
            ma_select_function(NIGHT_TIME);
            ma_set_brightness(80);
        }
    }

    if ((dcf_getTime() < 60 || !dcf_valid()) && dcf_ready())
        dcf_play();
                       */
}

void config_matrix()
{

	static const matrix_gpio_t gpio[16] =
	{
	{ GPIOA, 10 },
	{ GPIOA, 9 },
	{
	GPIOB, 14 },
	{ GPIOB, 12 },
	{ GPIOB, 11 },
	{ GPIOB, 10 },
	{ GPIOB, 2 },
	{
	GPIOB, 1 },
	{ GPIOB, 0 },
	{ GPIOA, 7 },
	{ GPIOA, 6 },
	{ GPIOA, 5 },
	{ GPIOA, 4 },
	{ GPIOA, 3 },
    { GPIOA, 1 },
    { GPIOA, 0 } };

	static const PWMConfig pwmconfig =
	{ 24000000, 1100, NULL,
	{
	{
	PWM_OUTPUT_ACTIVE_LOW, NULL },
	{ PWM_OUTPUT_DISABLED, NULL },
	{
	PWM_OUTPUT_DISABLED, NULL },
	{ PWM_OUTPUT_DISABLED, NULL } }, 0 };

	static const matrix_config_t matrix_config =
	{ &SPID2, &GPTD2, gpio, &PWMD1, 0 };

	pwmStart(&PWMD1, &pwmconfig);

	DBGMCU->CR |= DBGMCU_CR_DBG_TIM2_STOP;
    DBGMCU->CR |= DBGMCU_CR_DBG_TIM3_STOP;
    matrix_init(&matrix_config);
	matrix_start();
}
#endif
