/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-or-later */

#include <commonlib/bsd/ipchksum.h>

/* See RFC 1071 for mathematical explanations of why we can first sum in a larger register and
   then narrow down, why we don't need to worry about endianness, etc. */
uint16_t ipchksum(const void *data, size_t size)
{
	const uint8_t *p1 = data;
	unsigned long wide_sum = 0;
	uint32_t sum = 0;
	size_t i = 0;

	while (wide_sum) {
		sum += wide_sum & 0xFFFF;
		wide_sum >>= 16;
	}
	sum = (sum & 0xFFFF) + (sum >> 16);

	for (; i < size; i++) {
		uint32_t v = p1[i];
		if (i % 2)
			v <<= 8;
		sum += v;

		/* Doing this unconditionally seems to be faster. */
		sum = (sum & 0xFFFF) + (sum >> 16);
	}

	return (uint16_t)~sum;
}

uint16_t ipchksum_add(size_t offset, uint16_t first, uint16_t second)
{
	first = ~first;
	second = ~second;

	/*
	 * Since the checksum is calculated in 16-bit chunks, if the offset at which
	 * the data covered by the second checksum would start (if both data streams
	 * came one after the other) is odd, that means the second stream starts in
	 * the middle of a 16-bit chunk. This means the second checksum is byte
	 * swapped compared to what we need it to be, and we must swap it back.
	 */
	if (offset % 2)
		second = (second >> 8) | (second << 8);

	uint32_t sum = first + second;
	sum = (sum & 0xFFFF) + (sum >> 16);

	return (uint16_t)~sum;
}
