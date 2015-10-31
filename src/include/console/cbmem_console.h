/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
#ifndef _CONSOLE_CBMEM_CONSOLE_H_
#define _CONSOLE_CBMEM_CONSOLE_H_

#include <rules.h>
#include <stdint.h>

void cbmemc_init(void);
void cbmemc_tx_byte(unsigned char data);

#define __CBMEM_CONSOLE_ENABLE__	CONFIG_CONSOLE_CBMEM && \
	(ENV_RAMSTAGE || ENV_VERSTAGE || \
		(IS_ENABLED(CONFIG_EARLY_CBMEM_INIT) && \
		 (ENV_ROMSTAGE || (ENV_BOOTBLOCK && CONFIG_BOOTBLOCK_CONSOLE)))\
	)

#if __CBMEM_CONSOLE_ENABLE__
static inline void __cbmemc_init(void)	{ cbmemc_init(); }
static inline void __cbmemc_tx_byte(u8 data)	{ cbmemc_tx_byte(data); }
#else
static inline void __cbmemc_init(void)	{}
static inline void __cbmemc_tx_byte(u8 data)	{}
#endif

void cbmem_dump_console(void);
#endif
