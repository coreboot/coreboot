/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
