/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
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

#ifndef SUPERIO_SMSC_KBC1100_H
#define SUPERIO_SMSC_KBC1100_H

#include <arch/io.h>
#include <stdint.h>

#define KBC1100_PM1              1            /* PM1 */
#define SMSCSUPERIO_SP1          4            /* Com1 */
#define SMSCSUPERIO_SP2          5            /* Com2 */
#define KBC1100_KBC              7            /* Keyboard */
#define KBC1100_EC0              8            /* EC Channel 0 */
#define KBC1100_MAILBOX          9            /* Mail Box */
#define KBC1100_GPIO             0x0A         /* GPIO */
#define KBC1100_SPI              0x0B         /* Share flash interface */

#define KBC1100_EC1              0x0D         /* EC Channel 1 */
#define KBC1100_EC2              0x0E         /* EC Channel 2 */

void kbc1100_early_serial(pnp_devfn_t dev, u16 iobase);
void kbc1100_early_init(u16 port);

#endif /* SUPERIO_SMSC_KBC1100_H */
