/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SUPERIO_INTEL_I8900_I8900_H
#define SUPERIO_INTEL_I8900_I8900_H

/*
 * The SIW ("Serial I/O and Watchdog Timer") integrated into the i8900 is
 * very similar to a Super I/O, both in functionality and config mechanism.
 *
 * The SIW contains:
 *  - UART(s)
 *  - Serial interrupt controller
 *  - Watchdog timer (WDT)
 *  - LPC interface
 */

/* Logical device numbers (LDNs). */
#define I8900_SP1 0x04 /* Com1 */
#define I8900_SP2 0x05 /* Com2 */
#define I8900_WDT 0x06 /* Watchdog timer */

/* Registers and bit definitions: */

#define I8900_SIW_CONFIGURATION		0x29

/*
 * SIW_CONFIGURATION[3:2] = UART_CLK predivide
 * 00: divide by 1
 * 01: divide by 8
 * 10: divide by 26
 * 11: reserved
 */
#define I8900_UART_CLK_PREDIVIDE_1	0x00
#define I8900_UART_CLK_PREDIVIDE_8	0x01
#define I8900_UART_CLK_PREDIVIDE_26	0x02
#define I8900_ENABLE_SIRQ		0x01

void i8900_configure_uart_clk(pnp_devfn_t dev, u8 predivide);
void i8900_enable_serial(pnp_devfn_t dev, u16 iobase);
void i8900_enable_wdt(pnp_devfn_t dev, u16 iobase);

#endif
