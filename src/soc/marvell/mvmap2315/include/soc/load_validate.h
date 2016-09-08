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

#ifndef __SOC_MARVELL_MVMAP2315_LOAD_VALIDATE_H__
#define __SOC_MARVELL_MVMAP2315_LOAD_VALIDATE_H__

#include <stdint.h>

#include <soc/addressmap.h>
#include <types.h>

enum {
	PRIMARY_BDB = 0,
	ALTERNATE_BDB = 1,
	RECOVERY_BDB = 2,
};

#define MVMAP2315_BDB_TYPE		(BIT(0) | BIT(1))
struct mvmap2315_bootrom_info {
	u32 sp_ro_status;
	u32 flash_media;
};

struct mvmap2315_mcu_secconfig_regs {
	u8 _reserved0[0x04];
	u32 strap_override_sec_cfg;
	u32 sec_mcu_cfg_sec_cfg;
	u32 lcm_mcu_cfg_sec_cfg0;
	u32 lcm_mcu_cfg_sec_cfg1;
	u32 lcm_mcu_cfg_sec_cfg2;
	u8 _reserved1[0xe8];
	u32 dap_debug_disable;
	u32 boot_avs_status;
	u32 pwr_clr_in;
	u32 pwr_clr_recovery;
	u32 pwr_clr_bdb;
	u32 pwr_clr_fail_a;
	u32 pwr_clr_fail_b;
	u32 rst_clr_dev;
	u32 rst_clr_wp1;
	u32 rst_clr_wp2;
	u32 rst_clr_lp;
	u32 boot_gpio_out;
	u32 boot_gpio_in;
	u32 boot_hw_lockdown_nvm;
	u32 boot_hw_lockdown_pinmux;
	u32 boot_callback_pointer;
};

check_member(mvmap2315_mcu_secconfig_regs, boot_callback_pointer, 0x13c);
static struct mvmap2315_mcu_secconfig_regs * const mvmap2315_mcu_secconfig
					= (void *)MVMAP2315_MCU_SECCONFIG_BASE;

void load_and_validate(struct bdb_pointer *bdb_info, u32 image_type);

#endif /* __SOC_MARVELL_MVMAP2315_LOAD_VALIDATE_H__ */
