/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2019 Qualcomm Technologies
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_QUALCOMM_SC7180_ADDRESS_MAP_H_
#define _SOC_QUALCOMM_SC7180_ADDRESS_MAP_H_

#include <stdint.h>

#define AOSS_CC_BASE			0x0C2A0000
#define GCC_BASE			0x00100000
#define QSPI_BASE			0x088DC000
#define TLMM_NORTH_TILE_BASE		0x03900000
#define TLMM_SOUTH_TILE_BASE		0x03D00000
#define TLMM_WEST_TILE_BASE		0x03500000

/*
 * USB BASE ADDRESSES
 */
#define QFPROM_BASE			0x00780000
#define QUSB_PRIM_PHY_BASE		0x088e3000
#define QUSB_PRIM_PHY_DIG_BASE		0x088e3200
#define QMP_PHY_QSERDES_COM_REG_BASE	0x088e9000
#define QMP_PHY_QSERDES_TX_REG_BASE	0x088e9200
#define QMP_PHY_QSERDES_RX_REG_BASE	0x088e9400
#define QMP_PHY_PCS_REG_BASE		0x088e9c00
#define USB_HOST_DWC3_BASE		0x0a60c100

#endif /*  __SOC_QUALCOMM_SC7180_ADDRESS_MAP_H__ */
