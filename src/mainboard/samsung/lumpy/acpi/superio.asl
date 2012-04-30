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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Values should match those defined in devicetree.cb */

#define SIO_ENABLE_SPM1          // pnp 2e.1: Enable ACPI PM1 Block
#define SIO_SPM1_IO0      0xb00  // pnp 2e.1: io 0x60

#undef SIO_ENABLE_SEC1           // pnp 2e.2: Disable EC 1

#undef SIO_ENABLE_SEC2           // pnp 2e.3: Disable EC 2

#undef SIO_ENABLE_SSP1           // pnp 2e.4: Disable UART

#define SIO_ENABLE_SKBC          // pnp 2e.7: Enable Keyboard

#undef SIO_ENABLE_SEC0           // pnp 2e.8: Already exported as EC

#define SIO_ENABLE_SMBX          // pnp 2e.9: Enable Mailbox
#define SIO_SMBX_IO0      0xa00  // pnp 2e.9: io 0xa00

#include "superio/smsc/mec1308/acpi/superio.asl"
