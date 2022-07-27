/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SPKMODEM_H
#define SPKMODEM_H 1

#include <stdint.h>

void spkmodem_init(void);
void spkmodem_tx_byte(unsigned char c);

#if CONFIG(SPKMODEM) && (ENV_ROMSTAGE || ENV_RAMSTAGE)
static inline void __spkmodem_init(void)		{ spkmodem_init(); }
static inline void __spkmodem_tx_byte(u8 data)	{ spkmodem_tx_byte(data); }
#else
static inline void __spkmodem_init(void)		{}
static inline void __spkmodem_tx_byte(u8 data)	{}
#endif

#endif
