/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

#ifndef __SOC_MEDIATEK_MT8183_INCLUDE_SOC_ADDRESSMAP_H__
#define __SOC_MEDIATEK_MT8183_INCLUDE_SOC_ADDRESSMAP_H__

enum {
	IO_PHYS		= 0x10000000,
	DDR_BASE	= 0x40000000
};

enum {
	GPT_BASE	= IO_PHYS + 0x00008000,
	UART0_BASE	= IO_PHYS + 0x01002000,
};

#endif
