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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */
#ifndef _CONSOLE_CBMEM_CONSOLE_H_
#define _CONSOLE_CBMEM_CONSOLE_H_

#include <console/streams.h>

void cbmemc_init(void);
void cbmemc_tx_byte(unsigned char data);

#if CONFIG_CONSOLE_CBMEM
void cbmemc_reinit(void);
#else
#define cbmemc_reinit()
#endif

#define __CBMEM_CONSOLE_ENABLE__	CONFIG_CONSOLE_CBMEM && \
	(ENV_ROMSTAGE && CONFIG_EARLY_CBMEM_INIT || ENV_RAMSTAGE)

#if __CBMEM_CONSOLE_ENABLE__
#define __cbmemc_init()		cbmemc_init()
#define __cbmemc_tx_byte(x)	cbmemc_tx_byte(x)
#else
#define __cbmemc_init()
#define __cbmemc_tx_byte(x)
#endif

#endif
