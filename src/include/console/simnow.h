/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CONSOLE_SIMNOW_H
#define CONSOLE_SIMNOW_H

#include <stdint.h>

void simnow_console_init(void);
void simnow_console_tx_byte(unsigned char data);

#define __SIMNOW_CONSOLE_ENABLE__	CONFIG(CONSOLE_AMD_SIMNOW)

#if __SIMNOW_CONSOLE_ENABLE__
static inline void __simnow_console_init(void)
{
	simnow_console_init();
}

static inline void __simnow_console_tx_byte(u8 data)
{
	simnow_console_tx_byte(data);
}
#else
static inline void __simnow_console_init(void)	{}
static inline void __simnow_console_tx_byte(u8 data)	{}
#endif

#endif /* CONSOLE_SIMNOW_H */
