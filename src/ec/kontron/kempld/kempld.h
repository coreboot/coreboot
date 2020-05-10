/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_KONTRON_KEMPLD_H
#define EC_KONTRON_KEMPLD_H

#include <stdint.h>

void kempld_write8(uint8_t addr, uint8_t data);
uint8_t kempld_read8(uint8_t addr);

int kempld_get_mutex(int timeout_ms);
void kempld_release_mutex(void);

void kempld_enable_uart_for_console(void);

#endif /* EC_KONTRON_KEMPLD_H */
