/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef X4X_IOMAP_H
#define X4X_IOMAP_H

#define DEFAULT_MCHBAR		0xfed14000	/* 16 KB */
#define DEFAULT_DMIBAR		0xfed18000	/* 4 KB */
#define DEFAULT_EPBAR		0xfed19000	/* 4 KB */
#define DEFAULT_HECIBAR		0xfed10000

#define TPMBASE 0xfed40000
#define TPM32(x) (*((volatile u32 *)(TPMBASE + (x))))

#endif /* X4X_IOMAP_H */
