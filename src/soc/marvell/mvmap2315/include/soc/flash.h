/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#ifndef __SOC_MARVELL_MVMAP2315_FLASH_H__
#define __SOC_MARVELL_MVMAP2315_FLASH_H__

#include <stdint.h>

#define MVMAP2315_FLASH_INIT		0xFFE00084
#define MVMAP2315_FLASH_READ		0xFFE00088
#define MVMAP2315_FLASH_WRITE		0xFFE0008C
#define MVMAP2315_FLASH_SHUTDOWN	0xFFE00098
#define MVMAP2315_FLASH_SET_PARTITION	0xFFE00094

#define MVMAP2315_MMC_CLK_MHZ		0x00000019

enum {
	MVMAP2315_SD = 7,
	MVMAP2315_EMMC = 8,
	MVMAP2315_EEPROM = 27,
};

struct flash_params {
	u32 offset;
	u32 buff;
	u32 size;
	u32 id;
	u32 partition;
};

struct flash_ops {
	u32 (*init)(u32 media, u32 context, u32 clock_input_mhz);
	u32 (*read)(u32 media, u32 context, struct flash_params *);
	u32 (*write)(u32 media, u32 context, struct flash_params *);
	u32 (*shutdown)(u32 media, u32 context, struct flash_params *);
	u32 (*set_partition)(u32 media, u32 context, struct flash_params *);
};

u32 flash_init(u32 media, u32 clock_input_mhz);
u32 flash_read(u32 media, struct flash_params *flash_image_info);
u32 flash_write(u32 media, struct flash_params *flash_image_info);
u32 flash_shutdown(u32 media);
u32 flash_partition(u32 media, struct flash_params *flash_image_info);

#endif /* __SOC_MARVELL_MVMAP2315_FLASH_H__ */
