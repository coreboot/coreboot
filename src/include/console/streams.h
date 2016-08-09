/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _CONSOLE_STREAMS_H_
#define _CONSOLE_STREAMS_H_

#include <stddef.h>
#include <stdint.h>

void console_hw_init(void);
void console_tx_byte(unsigned char byte);
void console_tx_flush(void);

/*
 * Write number_of_bytes data bytes from buffer to the serial device.
 * If number_of_bytes is zero, wait until all serial data is output.
 */
void console_write_line(uint8_t *buffer, size_t number_of_bytes);

/* For remote GDB debugging. */
void gdb_hw_init(void);
void gdb_tx_byte(unsigned char byte);
void gdb_tx_flush(void);
unsigned char gdb_rx_byte(void);

#endif /* _CONSOLE_STREAMS_H_ */
