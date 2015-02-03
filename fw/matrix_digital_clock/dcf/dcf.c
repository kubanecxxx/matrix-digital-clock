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
static uint8_t get_59(uint8_t in);

static const GPTConfig gpt =
{ 10000, gpt_cb, 0 };
static GPTDriver * timer = &GPTD3;

static delay_t del;
static uint16_t backup;
static uint8_t decode_finished = 0;
static uint8_t dcf_data[60];

static void dcf_task(arg_t c);
static void dcf_decode(void);

/* Private functions ---------------------------------------------------------*/
void dcf_init(void)
{
	palSetPadMode(DCF_PORT, DCF_PIN, PAL_MODE_INPUT);

	shFillStruct(&del,dcf_task,NULL,MS2ST(100),PERIODIC);
	gptStart(timer, &gpt);
}

void dcf_task(arg_t c)
{
	(void) c;
	static uint8_t machine = 0;

	// machine start
	if (machine == 0)
	{
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
			ma_set_brightness(4000);
		}
	}
}

void dcf_play(void)
{
	backup = ma_brightness();
	ma_set_brightness(0);
	shRegisterStruct(&del);
}

static const uint8_t bcd_table[] = {1,2,4,8,10,20,40,80};

uint16_t bcd2num(uint8_t * data, uint8_t size)
{
	uint16_t t = 0;
	int i ;
	for (i = 0; i < size; i++)
	{
		t |= data[i] * bcd_table[i];
	}
	return t;
}

uint8_t parity(uint8_t * data, uint8_t size)
{
	int i;
	uint8_t t = 0;
	for (i = 0; i < size ; i++)
	{
		t += *data;
		data++;
	}

	return t & 1;
}

#if 0
22:39
dcf_data	uint8_t [60]	0x20000c9c <dcf_data>
	dcf_data[0]	uint8_t	0 '\0'
	dcf_data[1]	uint8_t	1 '\001'
	dcf_data[2]	uint8_t	1 '\001'
	dcf_data[3]	uint8_t	1 '\001'
	dcf_data[4]	uint8_t	1 '\001'
	dcf_data[5]	uint8_t	1 '\001'
	dcf_data[6]	uint8_t	1 '\001'
	dcf_data[7]	uint8_t	0 '\0'
	dcf_data[8]	uint8_t	1 '\001'
	dcf_data[9]	uint8_t	1 '\001'
	dcf_data[10]	uint8_t	0 '\0'
	dcf_data[11]	uint8_t	1 '\001'
	dcf_data[12]	uint8_t	1 '\001'
	dcf_data[13]	uint8_t	1 '\001'
	dcf_data[14]	uint8_t	1 '\001'
	dcf_data[15]	uint8_t	0 '\0'
	dcf_data[16]	uint8_t	0 '\0'
	dcf_data[17]	uint8_t	0 '\0'
	dcf_data[18]	uint8_t	1 '\001'
	dcf_data[19]	uint8_t	0 '\0'
	s
	dcf_data[20]	uint8_t	1 '\001'
	9
	dcf_data[21]	uint8_t	1 '\001'
	dcf_data[22]	uint8_t	0 '\0'
	dcf_data[23]	uint8_t	0 '\0'
	dcf_data[24]	uint8_t	1 '\001'
	30
	dcf_data[25]	uint8_t	1 '\001'
	dcf_data[26]	uint8_t	1 '\001'
	dcf_data[27]	uint8_t	0 '\0'

	dcf_data[28]	uint8_t	0 '\0'
	dcf_data[29]	uint8_t	1 '\001'
	dcf_data[30]	uint8_t	1 '\001'
	dcf_data[31]	uint8_t	0 '\0'
	dcf_data[32]	uint8_t	0 '\0'
	dcf_data[33]	uint8_t	1 '\001'
	dcf_data[34]	uint8_t	0 '\0'
	dcf_data[35]	uint8_t	0 '\0'
	dcf_data[36]	uint8_t	0 '\0'
	dcf_data[37]	uint8_t	1 '\001'
	dcf_data[38]	uint8_t	0 '\0'
	dcf_data[39]	uint8_t	1 '\001'
	dcf_data[40]	uint8_t	0 '\0'
	dcf_data[41]	uint8_t	1 '\001'
	dcf_data[42]	uint8_t	1 '\001'
	dcf_data[43]	uint8_t	0 '\0'
	dcf_data[44]	uint8_t	1 '\001'
	dcf_data[45]	uint8_t	0 '\0'
	dcf_data[46]	uint8_t	0 '\0'
	dcf_data[47]	uint8_t	0 '\0'
	dcf_data[48]	uint8_t	0 '\0'
	dcf_data[49]	uint8_t	1 '\001'
	dcf_data[50]	uint8_t	0 '\0'
	dcf_data[51]	uint8_t	1 '\001'
	dcf_data[52]	uint8_t	0 '\0'
	dcf_data[53]	uint8_t	1 '\001'
	dcf_data[54]	uint8_t	0 '\0'
	dcf_data[55]	uint8_t	0 '\0'
	dcf_data[56]	uint8_t	0 '\0'
	dcf_data[57]	uint8_t	0 '\0'
	dcf_data[58]	uint8_t	0 '\0'
	dcf_data[59]	uint8_t	0 '\0'

	22:59
	dcf_data[0]	uint8_t	0 '\0'
	dcf_data[1]	uint8_t	1 '\001'
	dcf_data[2]	uint8_t	0 '\0'
	dcf_data[3]	uint8_t	0 '\0'
	dcf_data[4]	uint8_t	0 '\0'
	dcf_data[5]	uint8_t	1 '\001'
	dcf_data[6]	uint8_t	1 '\001'
	dcf_data[7]	uint8_t	0 '\0'
	dcf_data[8]	uint8_t	0 '\0'
	dcf_data[9]	uint8_t	0 '\0'
	dcf_data[10]	uint8_t	0 '\0'
	dcf_data[11]	uint8_t	1 '\001'
	dcf_data[12]	uint8_t	1 '\001'
	dcf_data[13]	uint8_t	0 '\0'
	dcf_data[14]	uint8_t	1 '\001'
	dcf_data[15]	uint8_t	0 '\0'
	dcf_data[16]	uint8_t	0 '\0'
	dcf_data[17]	uint8_t	0 '\0'
	dcf_data[18]	uint8_t	1 '\001'
	dcf_data[19]	uint8_t	0 '\0'
	dcf_data[20]	uint8_t	1 '\001'
	dcf_data[21]	uint8_t	1 '\001'
	dcf_data[22]	uint8_t	0 '\0'
	dcf_data[23]	uint8_t	0 '\0'
	dcf_data[24]	uint8_t	1 '\001'
	dcf_data[25]	uint8_t	1 '\001'
	dcf_data[26]	uint8_t	0 '\0'
	dcf_data[27]	uint8_t	1 '\001'
	dcf_data[28]	uint8_t	0 '\0'
	dcf_data[29]	uint8_t	1 '\001'
	dcf_data[30]	uint8_t	1 '\001'
	dcf_data[31]	uint8_t	0 '\0'
	dcf_data[32]	uint8_t	0 '\0'
	dcf_data[33]	uint8_t	1 '\001'
	dcf_data[34]	uint8_t	0 '\0'
	dcf_data[35]	uint8_t	0 '\0'
	dcf_data[36]	uint8_t	0 '\0'
	dcf_data[37]	uint8_t	1 '\001'
	dcf_data[38]	uint8_t	0 '\0'
	dcf_data[39]	uint8_t	1 '\001'
	dcf_data[40]	uint8_t	0 '\0'
	dcf_data[41]	uint8_t	0 '\0'
	dcf_data[42]	uint8_t	1 '\001'
	dcf_data[43]	uint8_t	0 '\0'
	dcf_data[44]	uint8_t	1 '\001'
	dcf_data[45]	uint8_t	0 '\0'
	dcf_data[46]	uint8_t	0 '\0'
	dcf_data[47]	uint8_t	0 '\0'
	dcf_data[48]	uint8_t	0 '\0'
	dcf_data[49]	uint8_t	1 '\001'
	dcf_data[50]	uint8_t	0 '\0'
	dcf_data[51]	uint8_t	1 '\001'
	dcf_data[52]	uint8_t	0 '\0'
	dcf_data[53]	uint8_t	1 '\001'
	dcf_data[54]	uint8_t	0 '\0'
	dcf_data[55]	uint8_t	0 '\0'
	dcf_data[56]	uint8_t	0 '\0'
	dcf_data[57]	uint8_t	0 '\0'
	dcf_data[58]	uint8_t	0 '\0'
	dcf_data[59]	uint8_t	0 '\0'


#endif

	static uint8_t temp[100];
void dcf_decode(void)
{
	//minutes

	//start bit dcf_data[20]
	RTCTime rtc;
	uint8_t minutes = bcd2num(&dcf_data[21],7);
	uint8_t hours = bcd2num(&dcf_data[29],6);
	uint8_t day = bcd2num(&dcf_data[36],6);
	uint8_t month = bcd2num(&dcf_data[45],5);
	uint32_t year = 30 + bcd2num(&dcf_data[50],8);

	uint8_t p1 = parity(&dcf_data[21],7);
	uint8_t p2 = parity(&dcf_data[29],6);
	uint8_t p3 = parity(&dcf_data[36],22);

	rtc.tv_sec = minutes * 60;
	rtc.tv_sec += hours * 3600;
	rtc.tv_sec += day * 86400;
	rtc.tv_sec += month * 2629743;
	rtc.tv_sec += year *  31556926;

	rtcSetTime(&RTCD1,&rtc);
}

#if 0
	23:03
	dcf_data[0]	uint8_t	0 '\0'
	dcf_data[1]	uint8_t	1 '\001'
	dcf_data[2]	uint8_t	1 '\001'
	dcf_data[3]	uint8_t	1 '\001'
	dcf_data[4]	uint8_t	0 '\0'
	dcf_data[5]	uint8_t	1 '\001'
	dcf_data[6]	uint8_t	0 '\0'
	dcf_data[7]	uint8_t	1 '\001'
	dcf_data[8]	uint8_t	1 '\001'
	dcf_data[9]	uint8_t	0 '\0'
	dcf_data[10]	uint8_t	1 '\001'
	dcf_data[11]	uint8_t	1 '\001'
	dcf_data[12]	uint8_t	0 '\0'
	dcf_data[13]	uint8_t	0 '\0'
	dcf_data[14]	uint8_t	1 '\001'
	dcf_data[15]	uint8_t	0 '\0'
	dcf_data[16]	uint8_t	0 '\0'
	dcf_data[17]	uint8_t	0 '\0'
	dcf_data[18]	uint8_t	1 '\001'
	dcf_data[19]	uint8_t	0 '\0'
	s
	dcf_data[20]	uint8_t	1 '\001'

	dcf_data[21]	uint8_t	1 '\001'
	dcf_data[22]	uint8_t	1 '\001'
	dcf_data[23]	uint8_t	0 '\0'
	dcf_data[24]	uint8_t	0 '\0'

	dcf_data[25]	uint8_t	0 '\0'
	dcf_data[26]	uint8_t	0 '\0'
	dcf_data[27]	uint8_t	0 '\0'
	p
	dcf_data[28]	uint8_t	1 '\001'

	dcf_data[29]	uint8_t	1 '\001'
	dcf_data[30]	uint8_t	0 '\0'
	dcf_data[31]	uint8_t	0 '\0'
	dcf_data[32]	uint8_t	0 '\0'

	dcf_data[33]	uint8_t	1 '\001'
	dcf_data[34]	uint8_t	1 '\001'

	dcf_data[35]	uint8_t	1 '\001'

	dcf_data[36]	uint8_t	0 '\0'
	dcf_data[37]	uint8_t	1 '\001'
	dcf_data[38]	uint8_t	0 '\0'
	dcf_data[39]	uint8_t	1 '\001'
	dcf_data[40]	uint8_t	0 '\0'
	dcf_data[41]	uint8_t	1 '\001'
	dcf_data[42]	uint8_t	1 '\001'
	dcf_data[43]	uint8_t	0 '\0'
	dcf_data[44]	uint8_t	1 '\001'
	dcf_data[45]	uint8_t	0 '\0'
	dcf_data[46]	uint8_t	0 '\0'
	dcf_data[47]	uint8_t	0 '\0'
	dcf_data[48]	uint8_t	0 '\0'
	dcf_data[49]	uint8_t	1 '\001'
	dcf_data[50]	uint8_t	0 '\0'
	dcf_data[51]	uint8_t	1 '\001'
	dcf_data[52]	uint8_t	0 '\0'
	dcf_data[53]	uint8_t	1 '\001'
	dcf_data[54]	uint8_t	0 '\0'
	dcf_data[55]	uint8_t	0 '\0'
	dcf_data[56]	uint8_t	0 '\0'
	dcf_data[57]	uint8_t	0 '\0'
	dcf_data[58]	uint8_t	0 '\0'
	dcf_data[59]	uint8_t	0 '\0'

#endif

//every 10ms
void gpt_cb(GPTDriver * gpt)
{
	(void) gpt;
	uint8_t in;

	in = palReadPad(DCF_PORT, DCF_PIN);


	static int32_t seconds = -1;
	static uint8_t ones;
	static uint8_t buffer[60];
	static uint8_t * pointer = buffer;
	static uint8_t prev;
	static uint8_t secs;

	static uint8_t *  t = temp;

	uint8_t last = get_59(in);
	uint8_t modulo;
	if (last || seconds != -1)
	{
		if (last)
		{
			//restart machine
			seconds = 0;
			pointer = buffer;
			memset(pointer,0,60);
			ones = 0;
			secs = 0;
		}

		//100 means one second
		seconds++;
		if (in)
		{
			ones++;
			(*t)++;
		}

		modulo = seconds % 100;
		if (!in && prev && (modulo > 95 || modulo < 10))
		{
			seconds += modulo;
			secs++;
			t++;
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
		if (secs > 59)
		{
			//end
			decode_finished = 1;
			seconds = -1;
			memcpy(dcf_data,buffer,60);
		}
		prev = in;
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
