/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 secunet Security Networks AG
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

#ifndef SUPERIO_ITE_IT8783EF_H
#define SUPERIO_ITE_IT8783EF_H

#define IT8783EF_FDC	0x00 /* Floppy disk controller */
#define IT8783EF_SP1	0x01 /* COM1 */
#define IT8783EF_SP2	0x02 /* COM2 */
#define IT8783EF_PP	0x03 /* Printer port */
#define IT8783EF_EC	0x04 /* Environment controller */
#define IT8783EF_KBCK	0x05 /* Keyboard */
#define IT8783EF_KBCM	0x06 /* Mouse */
#define IT8783EF_GPIO	0x07 /* GPIO */
#define IT8783EF_SP3	0x08 /* COM3 */
#define IT8783EF_SP4	0x09 /* COM4 */
#define IT8783EF_SP5	0x0a /* COM5 */
#define IT8783EF_SP6	0x0b /* COM6 */
#define IT8783EF_CIR	0x0c /* Consumer IR */

#include <arch/io.h>
#include <stdint.h>

#endif /* SUPERIO_ITE_IT8783EF_H */
