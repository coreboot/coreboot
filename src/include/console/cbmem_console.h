/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _CONSOLE_CBMEM_CONSOLE_H_
#define _CONSOLE_CBMEM_CONSOLE_H_

#include <stdint.h>
#include <stddef.h>

void cbmemc_init(void);
void cbmemc_tx_byte(unsigned char data);

#define __CBMEM_CONSOLE_ENABLE__	(CONFIG(CONSOLE_CBMEM) && \
	(ENV_RAMSTAGE || ENV_SEPARATE_VERSTAGE || ENV_POSTCAR  || \
	 ENV_ROMSTAGE || (ENV_BOOTBLOCK && CONFIG(BOOTBLOCK_CONSOLE)) || \
	 (ENV_SMM && CONFIG(DEBUG_SMI))))

#if __CBMEM_CONSOLE_ENABLE__
static inline void __cbmemc_init(void)	{ cbmemc_init(); }
static inline void __cbmemc_tx_byte(u8 data)	{ cbmemc_tx_byte(data); }
#else
static inline void __cbmemc_init(void)	{}
static inline void __cbmemc_tx_byte(u8 data)	{}
#endif

/*
 * Copy an external cbmem_console into the active cbmem_console.
 */
void cbmemc_copy_in(void *buffer, size_t size);

void cbmem_dump_console_to_uart(void);
void cbmem_dump_console(void);
#endif

/* Retrieves the location of the CBMEM Console buffer in SMM mode */
void smm_get_cbmemc_buffer(void **buffer_out, size_t *size_out);
