/* SPDX-License-Identifier: GPL-2.0-only */

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
