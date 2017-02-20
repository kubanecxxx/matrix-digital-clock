/**
 * @file dcf.c
 * @author kubanec
 * @date 8. 1. 2015
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "dcf.h"
#include "matrix_driver.h"
#include "scheduler.h"
#include "matrix_abstraction.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static void gpt_cb(GPTDriver * gpt);
static void gpt_secs_cb(GPTDriver * gpt);
static uint8_t get_59(uint8_t in);

static const GPTConfig gpt =
{ 10000, gpt_cb, 0 };
static GPTDriver * timer = &GPTD3;
static GPTDriver * secs = &GPTD4;

static const GPTConfig sec_gpt  =
{
		10000, gpt_secs_cb, 0
};

static delay_t del;
static uint8_t decode_finished = 0;
static uint8_t dcf_data[60];

static uint32_t time = 0;
static uint8_t rdy = 1;
static uint8_t time_valid  = 0;

static void dcf_task(arg_t c);
static void dcf_decode(void);

/* Private functions ---------------------------------------------------------*/
void dcf_init(void)
{
	palSetPadMode(DCF_PORT, DCF_PIN, PAL_MODE_INPUT);
	DBGMCU->CR |= DBGMCU_CR_DBG_TIM3_STOP;

	shFillStruct(&del, dcf_task, NULL, MS2ST(100), PERIODIC);
	gptStart(timer, &gpt);
	gptStart(secs , &sec_gpt);

	gptStartContinuous(secs,10000);
}

void dcf_task(arg_t c)
{
	(void) c;
	static uint8_t machine = 0;

	// machine start
	if (machine == 0)
	{
		ma_disable(1);
		gptStartContinuous(timer, 100);
		machine++;
	}
	else if (machine == 1)
	{
		if (decode_finished)
		{
			decode_finished = 0;
			machine = 0;
			shUnregisterStruct(&del);
			gptStopTimer(timer);
			dcf_decode();
			ma_disable(0);
			rdy = 1;
		}
	}
}

uint8_t dcf_ready(void)
{
	return rdy;
}

uint8_t dcf_valid(void)
{
	return time_valid;
}

void dcf_play(void)
{
	rdy = 0;
	time_valid = 0;
	ma_set_brightness(40);

	shRegisterStruct(&del);
}

uint32_t dcf_getTime(void)
{
	return time % 86400;
}

static const uint8_t bcd_table[] =
{ 1, 2, 4, 8, 10, 20, 40, 80 };

uint16_t bcd2num(uint8_t * data, uint8_t size)
{
	uint16_t t = 0;
	int i;
	for (i = 0; i < size; i++)
	{
		t += data[i] * bcd_table[i];
	}
	return t;
}

uint8_t parity(uint8_t * data, uint8_t size)
{
	int i;
	uint8_t t = 0;
	for (i = 0; i < size; i++)
	{
		t += *data;
		data++;
	}

	return t & 1;
}



void dcf_decode(void)
{
	//minutes

	//start bit dcf_data[20]
	//RTCTime rtc;
	uint8_t minutes = bcd2num(&dcf_data[21], 7);
	uint8_t hours = bcd2num(&dcf_data[29], 6);
	uint8_t day = bcd2num(&dcf_data[36], 6);
	uint8_t month = bcd2num(&dcf_data[45], 5);
	uint32_t year = bcd2num(&dcf_data[50], 8);

	uint8_t p1 = parity(&dcf_data[21], 7);
	uint8_t p2 = parity(&dcf_data[29], 6);
	uint8_t p3 = parity(&dcf_data[36], 22);

	uint8_t p11 = dcf_data[28];
	uint8_t p22 = dcf_data[35];
	uint8_t p33 = dcf_data[58];

	if (p1 == p11 && p2 == p22 && p3 == p33)
	{
		time = minutes * 60;
		time += hours * 3600;
		time_valid = 1;

		//rtc.tv_sec += day * 86400;
		//rtc.tv_sec += month * 2629743;
		//rtc.tv_sec += year * 31556926;

		//rtcSetTime(&RTCD1, &rtc);
	}
}

uint16_t input_counter;
uint16_t input_secs;

//every 10ms
void gpt_cb(GPTDriver * gpt)
{
	(void) gpt;
	static uint8_t in;
	static uint32_t free;
	free++;

	in = palReadPad(DCF_PORT, DCF_PIN);
	ma_dcf_led(in);

	static int32_t seconds = -1;
	static uint8_t ones;
	static uint8_t buffer[60];
	static uint8_t * pointer = buffer;
	static uint8_t secs;


	uint8_t last = get_59(in);
	uint8_t modulo;
	if (last || seconds != -1)
	{
		if (last)
		{
			//restart machine
			seconds = 0;
			pointer = buffer;
			memset(pointer, 0, 60);
			ones = 0;
			secs = 0;
			input_counter = 0;
		}

		//100 means one second
		seconds++;
		if (in)
		{
			ones++;
		}

		modulo = seconds % 100;
		if (!modulo)
		{
			input_counter++;
			seconds += modulo;
			secs++;
			ma_dcf_progress(secs);
			input_secs = ones;
			//zero is automatic
			if (ones > 75 && ones < 85)
			{
				//one
				*pointer = 1;
			}
			else if (ones >= 85 && ones < 95)
			{
				//zero
			}
			else
			{
				//error
			}
			ones = 0;
			pointer++;
		}
		if (secs > 58)
		{
			//end
			decode_finished = 1;
			seconds = -1;
			memcpy(dcf_data, buffer, 60);
		}

	}
}

uint8_t get_59(uint8_t in)
{
	static uint8_t prev;
	static uint8_t counter59;
	static uint8_t zeros;

//detect 59th second
	if (counter59 > 170 && !in && prev)
	{
		//now definitely 59th second passed
		//falling edge of 1st second
		//tidy up
		zeros = 0;
		counter59 = 0;
		prev = 0;

		return 1;
	}

	if (in)
	{
		counter59++;
		if (zeros)
			zeros--;
	}
	else
	{
		zeros++;
		//shit could happen
		if (zeros > 5)
		{
			counter59 = 0;
			zeros = 0;
		}
	}

	prev = in;
	return 0;
}

void gpt_secs_cb(GPTDriver * gpt)
{
	time++;
}
