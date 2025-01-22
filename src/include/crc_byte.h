/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CRC_BYTE_H
#define CRC_BYTE_H

#include <stddef.h>
#include <stdint.h>

/* This function is used to calculate crc7 byte by byte, with polynomial
 * x^7 + x^3 + 1.
 *
 * prev_crc: old crc result (0 for first)
 * data: new byte
 * return value: new crc result
 */
uint8_t crc7_byte(uint8_t prev_crc, uint8_t data);

/* This function is used to calculate crc16 byte by byte, with polynomial
 * x^16 + x^12 + x^5 + 1.
 *
 * prev_crc: old crc result (0 for first)
 * data: new byte
 * return value: new crc result
 */
uint16_t crc16_byte(uint16_t prev_crc, uint8_t data);

/* This function is used to calculate crc32 byte by byte, with polynomial
 * x^32 + x^26 +  x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 +
 * x^5 + x^4 + x^2 + x + 1
 *
 * prev_crc: old crc result (0 for first)
 * data: new byte
 * return value: new crc result
 */
uint32_t crc32_byte(uint32_t prev_crc, uint8_t data);

#define CRC(buf, size, crc_func) ({ \
	const uint8_t *_crc_local_buf = (const uint8_t *)(buf); \
	size_t _crc_local_size = size; \
	__typeof__(crc_func(0, 0)) _crc_local_result = 0; \
	while (_crc_local_size--) { \
		_crc_local_result = crc_func(_crc_local_result, *_crc_local_buf++); \
	} \
	_crc_local_result; \
})

#endif /* CRC_BYTE_H */
