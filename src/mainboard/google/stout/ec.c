/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The Chromium OS Authors. All rights reserved.
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

#include <arch/acpi.h>
#include <arch/io.h>
#include <bootmode.h>
#include <types.h>
#include <console/console.h>
#include <ec/quanta/it8518/ec.h>
#include <device/device.h>
#include <device/pci.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/pmbase.h>
#include <elog.h>
#include "ec.h"

#ifdef __SMM__
#include <cpu/x86/smm.h>
#endif

#ifndef __SMM__
void stout_ec_init(void)
{

	printk(BIOS_DEBUG,"%s: EC FW version %x%x\n", __func__,
			ec_read(EC_FW_VER), ec_read(EC_FW_VER + 1));

	/*
	 *  Important: get_recovery_mode_switch() must be called in EC init.
	 */
	get_recovery_mode_switch();

	/* Unmute */
	ec_kbc_write_cmd(EC_KBD_CMD_UNMUTE);

	/*
	 * Set USB Power off in S3 (enabled in S3 path if requested in gnvs)
	 * Bit0 of 0x0D/Bit0 of 0x26
	 * 0/0 All USB port off
	 * 1/0 USB on, all USB port didn't support wake up
	 * 0/1 USB on, yellow port support wake up charge, but may not support
	 *             charge smart phone.
	 * 1/1 USB on, yellow port in AUTO mode and didn't support wake up system.
	 */
	ec_write(EC_PERIPH_CNTL_3, ec_read(EC_PERIPH_CNTL_3) & 0xE);
	ec_write(EC_USB_S3_EN, ec_read(EC_USB_S3_EN) & 0xE);

	// TODO: Power Limit Setting
}

#else // SMM

void stout_ec_finalize_smm(void)
{
	u8 ec_reg, critical_shutdown = 0;

	/*
	 * Check EC for error conditions.
	 */

	/* Fan Error : Peripheral Status 3 (0x35) bit 4 */
	ec_reg = ec_read(EC_PERIPH_STAT_3);

	if (ec_reg & 0x8) {
		printk(BIOS_ERR, "  EC Fan Error\n");
		critical_shutdown = 1;
#if IS_ENABLED(CONFIG_ELOG_GSMI)
		elog_add_event_word(EC_EVENT_BATTERY_CRITICAL, EC_EVENT_FAN_ERROR);
#endif
	}


	/* Thermal Device Error : Peripheral Status 3 (0x35) bit 8 */
	if (ec_reg & 0x80) {
		printk(BIOS_ERR, "  EC Thermal Device Error\n");
		critical_shutdown = 1;
#if IS_ENABLED(CONFIG_ELOG_GSMI)
		elog_add_event_word(EC_EVENT_BATTERY_CRITICAL, EC_EVENT_THERMAL);
#endif
	}


	/* Critical Battery Error */
	ec_reg = ec_read(EC_MBAT_STATUS);

	if ((ec_reg & 0xCF) == 0xC0) {
		printk(BIOS_ERR, "  EC Critical Battery Error\n");
		critical_shutdown = 1;
#if IS_ENABLED(CONFIG_ELOG_GSMI)
		elog_add_event_word(ELOG_TYPE_EC_EVENT, EC_EVENT_BATTERY_CRITICAL);
#endif
	}

	if ((ec_reg & 0x8F) == 0x8F) {
		printk(BIOS_ERR, "  EC Read Battery Error\n");
#if IS_ENABLED(CONFIG_ELOG_GSMI)
		elog_add_event_word(ELOG_TYPE_EC_EVENT, EC_EVENT_BATTERY);
#endif
	}


	if (critical_shutdown) {
		printk(BIOS_ERR, "EC critical_shutdown");

		/* Go to S5 */
		write_pmbase32(PM1_CNT, read_pmbase32(PM1_CNT) | (0xf << 10));
	}
}
#endif //__SMM__
