/* SPDX-License-Identifier: GPL-2.0-only */

#include "stubs/timestamp.h"

static uint64_t timestamp_value = 0;
static int timestamp_tick_freq_mhz_value = 1;

/* Provides way to control timestamp value */
void dummy_timestamp_set(uint64_t v)
{
	timestamp_value = v;
}

/* Provides way to control timestamp tick frequency MHz value */
void dummy_timestamp_tick_freq_mhz_set(int v)
{
	timestamp_tick_freq_mhz_value = v;
}

/* Reimplementation of timestamp getter to control behaviour */
uint64_t timestamp_get(void)
{
	return timestamp_value;
}

int timestamp_tick_freq_mhz(void)
{
	return timestamp_tick_freq_mhz_value;
}
