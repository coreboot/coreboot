/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/fsp_bd82x6x/nvs.h>
#include <southbridge/intel/fsp_bd82x6x/pch.h>
#include <southbridge/intel/fsp_bd82x6x/me.h>
#include <northbridge/intel/fsp_sandybridge/sandybridge.h>
#include <cpu/intel/fsp_model_206ax/model_206ax.h>

/*
 * Change LED_POWER# (SIO GPIO 45) state based on sleep type.
 * The IO address is hardcoded as we don't have device path in SMM.
 */
#define SIO_GPIO_BASE_SET4	(0x730 + 3)
#define SIO_GPIO_BLINK_GPIO45	0x25
void mainboard_smi_sleep(u8 slp_typ)
{
	u8 reg8;

	switch (slp_typ) {
	case SLP_TYP_S3:
	case SLP_TYP_S4:
		break;

	case SLP_TYP_S5:
		/* Turn off LED */
		reg8 = inb(SIO_GPIO_BASE_SET4);
		reg8 |= (1 << 5);
		outb(reg8, SIO_GPIO_BASE_SET4);
		break;
	}
}


static int mainboard_finalized = 0;

int mainboard_smi_apmc(u8 apmc)
{
	switch (apmc) {
	case APM_CNT_FINALIZE:
		if (mainboard_finalized) {
			printk(BIOS_DEBUG, "SMI#: Already finalized\n");
			return 0;
		}

		intel_me_finalize_smm();
		intel_pch_finalize_smm();
		intel_sandybridge_finalize_smm();
		intel_model_206ax_finalize_smm();

		mainboard_finalized = 1;
		break;
	}
	return 0;
}
