/* SPDX-License-Identifier: GPL-2.0-or-later */

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
