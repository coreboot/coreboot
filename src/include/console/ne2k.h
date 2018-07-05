/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Rudolf Marek <r.marek@assembler.cz>
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

#ifndef _NE2K_H__
#define _NE2K_H__

#include <rules.h>
#include <stdint.h>

void ne2k_append_data(unsigned char *d, int len, unsigned int base);
int ne2k_init(unsigned int eth_nic_base);
void ne2k_transmit(unsigned int eth_nic_base);

#if IS_ENABLED(CONFIG_CONSOLE_NE2K) && (ENV_ROMSTAGE || ENV_RAMSTAGE)
static inline void __ne2k_init(void)
{
	ne2k_init(CONFIG_CONSOLE_NE2K_IO_PORT);
}
static inline void __ne2k_tx_byte(u8 data)
{
	ne2k_append_data(&data, 1, CONFIG_CONSOLE_NE2K_IO_PORT);
}
static inline void __ne2k_tx_flush(void)
{
	ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
}
#else
static inline void __ne2k_init(void)		{}
static inline void __ne2k_tx_byte(u8 data)	{}
static inline void __ne2k_tx_flush(void)	{}
#endif

#endif /* _NE2K_H__ */
