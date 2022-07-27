/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CONSOLE_SYSTEM76_EC_H
#define CONSOLE_SYSTEM76_EC_H 1

#include <stddef.h>
#include <stdint.h>

void system76_ec_init(void);
void system76_ec_flush(void);
void system76_ec_print(uint8_t byte);

#define __CONSOLE_SYSTEM76_EC_ENABLE__	(CONFIG(CONSOLE_SYSTEM76_EC) && \
	(ENV_BOOTBLOCK || ENV_ROMSTAGE || ENV_RAMSTAGE \
	 || ENV_SEPARATE_VERSTAGE || ENV_POSTCAR \
	 || (ENV_SMM && CONFIG(DEBUG_SMI))))

#if __CONSOLE_SYSTEM76_EC_ENABLE__
static inline void __system76_ec_init(void)
{
	system76_ec_init();
}
static inline void __system76_ec_tx_flush(void)
{
	system76_ec_flush();
}
static inline void __system76_ec_tx_byte(unsigned char byte)
{
	system76_ec_print(byte);
}
#else
static inline void __system76_ec_init(void) {}
static inline void __system76_ec_tx_flush(void) {}
static inline void __system76_ec_tx_byte(unsigned char byte) {}
#endif

#endif
