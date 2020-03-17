/*
 * This file is part of the coreboot project.
 *
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

#ifndef CONSOLE_FLASH_H
#define CONSOLE_FLASH_H 1

#include <stdint.h>

void flashconsole_init(void);
void flashconsole_tx_byte(unsigned char c);
void flashconsole_tx_flush(void);

#define __CONSOLE_FLASH_ENABLE__	CONFIG(CONSOLE_SPI_FLASH)

#if __CONSOLE_FLASH_ENABLE__
static inline void __flashconsole_init(void)	{ flashconsole_init(); }
static inline void __flashconsole_tx_byte(u8 data)
{
	flashconsole_tx_byte(data);
}
static inline void __flashconsole_tx_flush(void)
{
	flashconsole_tx_flush();
}
#else
static inline void __flashconsole_init(void)	{}
static inline void __flashconsole_tx_byte(u8 data)	{}
static inline void __flashconsole_tx_flush(void)	{}
#endif /* __CONSOLE_FLASH_ENABLE__ */


#endif /* CONSOLE_FLASH_H */
