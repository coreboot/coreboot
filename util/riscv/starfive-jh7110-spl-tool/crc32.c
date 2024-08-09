// SPDX-License-Identifier: GPL-2.0+
#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>

static uint32_t crc32_reverse(uint32_t x)
{
	x = ((x & 0x55555555) << 1) | ((x >> 1) & 0x55555555);
	x = ((x & 0x33333333) << 2) | ((x >> 2) & 0x33333333);
	x = ((x & 0x0F0F0F0F) << 4) | ((x >> 4) & 0x0F0F0F0F);
	x = (x << 24) | ((x & 0xFF00) << 8) | ((x >> 8) & 0xFF00) | (x >> 24);
	return x;
}

uint32_t crc32(uint32_t iv, uint32_t sv, const void *data, size_t n)
{
	const unsigned char *ptr;
	unsigned x;
	uint32_t byte, crc;

	crc = iv;
	ptr = data;
	while (n--) {
		byte = *ptr++;
		byte = crc32_reverse(byte);
		for (x = 0; x < 8; x++, byte <<= 1) crc = ((crc ^ byte) & 0x80000000U) ? (crc << 1) ^ sv : (crc << 1);
	}

	return crc;
}

uint32_t crc32_final(uint32_t iv)
{
	return crc32_reverse(iv ^ ~0U);
}
