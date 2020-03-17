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

#ifndef I82801GX_SATA_H
#define I82801GX_SATA_H

#define SATA_MAP	0x90
#define SATA_PCS	0x92
#define SATA_IR		0x94
#define  SIF1		0x180
#define  SIF2		(1 << 23)
#define  SIF3(ports)	((~(ports) & 0xf) << 24)
#define  SCRE		(1 << 28)
#define  SCRD		(1 << 30)

void sata_enable(struct device *dev);

#endif
