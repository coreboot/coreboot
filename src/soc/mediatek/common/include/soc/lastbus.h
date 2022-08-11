/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_LASTBUS_H
#define SOC_MEDIATEK_COMMON_LASTBUS_H

/* INFRA LASTBUS INFO */
#define BUS_INFRA_SNAPSHOT	0xd00
#define BUS_INFRA_CTRL		0xd04
#define INFRASYS_CONFIG		0xffff000c
#define INFRA_NUM		17

/* PERI LASTBUS INFO */
#define BUS_PERI_R0		0x500
#define BUS_PERI_R1		0x504
#define PERISYS_ENABLE		0xc
#define PERISYS_TIMEOUT		0xffff
#define PERI_NUM		18

void lastbus_init(void);

#endif
