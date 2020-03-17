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

#ifndef EC_GOOGLE_WILCO_BOOTBLOCK_H
#define EC_GOOGLE_WILCO_BOOTBLOCK_H

/**
 * wilco_ec_early_init
 *
 * This function performs early initialization of the EC:
 *
 * - Enable EC UART passthru for COM1 if serial console support
 *   is enabled with CONFIG_DRIVERS_UART_8250IO.
 */
void wilco_ec_early_init(void);

#endif /* EC_GOOGLE_WILCO_BOOTBLOCK_H */
