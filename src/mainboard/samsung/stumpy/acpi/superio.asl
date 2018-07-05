/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

/* Values should match those defined in devicetree.cb */

#undef SIO_ENABLE_FDC0           // pnp 2e.0: Disable Floppy Controller
#undef SIO_ENABLE_INFR           // pnp 2e.a: Disable Consumer IR

#define SIO_ENABLE_PS2K          // pnp 2e.5: Enable PS/2 Keyboard
#define SIO_ENABLE_PS2M          // pnp 2e.6: Enable PS/2 Mouse
#define SIO_ENABLE_COM1          // pnp 2e.1: Enable Serial Port 1
#define SIO_ENABLE_ENVC          // pnp 2e.4: Enable Environmental Controller
#define SIO_ENVC_IO0      0x700  // pnp 2e.4: io 0x60
#define SIO_ENVC_IO1      0x710  // pnp 2e.4: io 0x62
#define SIO_ENABLE_GPIO		 // pnp 2e.7: Enable GPIO
#define SIO_GPIO_IO0      0x720  // pnp 2e.7: io 0x60
#define SIO_GPIO_IO1      0x730  // pnp 2e.7: io 0x60

#include "superio/ite/it8772f/acpi/superio.asl"
