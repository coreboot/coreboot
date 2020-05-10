/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_GOOGLE_COMMON_MEC_H
#define EC_GOOGLE_COMMON_MEC_H

#include <stddef.h>
#include <stdint.h>

/* Indicate read or write from MEC IO region */
enum mec_io_type {
	MEC_IO_READ,
	MEC_IO_WRITE
};

/*
 * mec_io_bytes - Read / write bytes to MEC EMI port
 *
 * @type:    Indicate read or write operation
 * @base:    Base address for MEC EMI region
 * @offset:  Base read / write address
 * @buffer:  Destination / source buffer
 * @size:    Number of bytes to read / write
 *
 * @returns 8-bit checksum of all bytes read or written
 */

uint8_t mec_io_bytes(enum mec_io_type type, uint16_t base,
		     uint16_t offset, void *buffer, size_t size);

#endif /* EC_GOOGLE_COMMON_MEC_H */
