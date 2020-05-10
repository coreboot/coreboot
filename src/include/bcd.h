/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BCD_H_
#define _BCD_H_

#include <stdint.h>

static inline uint8_t bcd2bin(uint8_t val)
{
	return ((val >> 4) & 0xf) * 10 + (val & 0xf);
}

static inline uint8_t bin2bcd(uint8_t val)
{
	return ((val / 10) << 4) | (val % 10);
}

#endif /* _BCD_H_ */
