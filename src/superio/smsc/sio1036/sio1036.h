/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#ifndef SUPERIO_SMSC_SIO1306_H
#define SUPERIO_SMSC_SIO1306_H

#define SIO1036_SP1		0 /* Com1 */

#define UART_POWER_DOWN		(1 << 7)
#define LPT_POWER_DOWN		(1 << 2)
#define IR_OUTPUT_MUX		(1 << 6)

#include <arch/io.h>
#include <stdint.h>

void sio1036_enable_serial(pnp_devfn_t dev, u16 iobase);

#endif /* SUPERIO_SMSC_SIO1306_H */
