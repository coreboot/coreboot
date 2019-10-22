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

#ifndef __SOC_QUALCOMM_SC7180_EFUSE_ADDRESS_MAP_H__
#define __SOC_QUALCOMM_SC7180_EFUSE_ADDRESS_MAP_H__

/**
 *  USB EFUSE registers
 */
struct qfprom_corr {
	u8 rsvd[0x4258 - 0x0];
	u32 qusb_hstx_trim_lsb;
};
check_member(qfprom_corr, qusb_hstx_trim_lsb, 0x4258);

#endif /* __SOC_QUALCOMM_SC7180_EFUSE_ADDRESS_MAP_H__ */
