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

#ifndef EC_KONTRON_KEMPLD_H
#define EC_KONTRON_KEMPLD_H

#include <stdint.h>

void kempld_write8(uint8_t addr, uint8_t data);
uint8_t kempld_read8(uint8_t addr);

int kempld_get_mutex(int timeout_ms);
void kempld_release_mutex(void);

void kempld_enable_uart_for_console(void);

#endif /* EC_KONTRON_KEMPLD_H */
