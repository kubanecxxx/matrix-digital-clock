/**
 * @file crc8.c
 * @author kubanec
 * @date 9. 1. 2014
 *
 */

#include "ch.h"

/**
 * @ingroup remote
 * @defgroup remote_crc
 * @title CRC
 * @brief výpočty crc pro remote
 * @{
 */

/// @brief generační polynom crc8
#define GP  0x107   /* x^8 + x^2 + x + 1 */
#define DI  0x07

/// @brief lookup tabulka crc8
static uint8_t crc8_table[256]; /* 8-bit table */
static uint8_t made_table = 0;

/**
 * @brief vytvoří lookup table pro crc8
 */
void init_crc8(void)
/*
 * Should be called before any other crc function.
 */
{
	int i, j;
	uint8_t crc;

	if (!made_table)
	{
		for (i = 0; i < 256; i++)
		{
			crc = i;
			for (j = 0; j < 8; j++)
				crc = (crc << 1) ^ ((crc & 0x80) ? DI : 0);
			crc8_table[i] = crc & 0xFF;
		}
		made_table = 1;
	}
}

/**
 * @brief For a byte array whose accumulated crc value is stored in *crc, computes
 * resultant crc obtained by appending m to the byte array
 */
void crc8(uint8_t *crc, uint8_t m)
{
    init_crc8();
	*crc = crc8_table[(*crc) ^ m];
	*crc &= 0xFF;
}

/**
 * @}
 */
