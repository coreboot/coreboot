/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _CONSOLE_CBMEM_CONSOLE_H_
#define _CONSOLE_CBMEM_CONSOLE_H_

#include <stdint.h>

void cbmemc_init(void);
void cbmemc_tx_byte(unsigned char data);

#define __CBMEM_CONSOLE_ENABLE__	(CONFIG(CONSOLE_CBMEM) && \
	(ENV_RAMSTAGE || ENV_SEPARATE_VERSTAGE || ENV_POSTCAR  || \
	 ENV_ROMSTAGE || (ENV_BOOTBLOCK && CONFIG(BOOTBLOCK_CONSOLE))))

#if __CBMEM_CONSOLE_ENABLE__
static inline void __cbmemc_init(void)	{ cbmemc_init(); }
static inline void __cbmemc_tx_byte(u8 data)	{ cbmemc_tx_byte(data); }
#else
static inline void __cbmemc_init(void)	{}
static inline void __cbmemc_tx_byte(u8 data)	{}
#endif

void cbmem_dump_console(void);
#endif
