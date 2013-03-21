/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/bd82x6x/nvs.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <cpu/intel/model_206ax/model_206ax.h>
#include <elog.h>
#include <ec/compal/ene932/ec.h>
#include "ec.h"

/* Power Management PCI Configuration Registers
 * Bus 0, Device 31, Function 0, Offset 0xB8
 * 00 = No Effect
 * 01 = SMI#
 * 10 = SCI
 * 11 = NMI
 */
#define GPI_ROUT	0x8000F8B8
#define GPI_IS_SMI	0x01
#define GPI_IS_SCI	0x02

static void set_lid_gpi_mode(u32 mode)
{
	u32 reg32 = 0;
	u16 reg16 = 0;

	/* read the GPI register, clear the lid GPI's mode, write the new mode
	 * and write out the register.
	 */
	outl(GPI_ROUT, 0xcf8);
	reg32 = inl(0xcfc);
	reg32 &= ~(0x03 << (EC_LID_GPI * 2));
	reg32 |= (mode << (EC_LID_GPI * 2));
	outl(GPI_ROUT, 0xcf8);
	outl(reg32, 0xcfc);

	/* Set or Disable Lid GPE as SMI source in the ALT_GPI_SMI_EN register. */
	reg16 = inw(smm_get_pmbase() + ALT_GP_SMI_EN);
	if (mode == GPI_IS_SCI) {
		reg16 &= ~(1 << EC_LID_GPI);
	} else {
		reg16 |= (1 << EC_LID_GPI);
	}
	outw(reg16, smm_get_pmbase() + ALT_GP_SMI_EN);

	return;
}

int mainboard_io_trap_handler(int smif)
{
	printk(BIOS_DEBUG, "mainboard_io_trap_handler: %x\n", smif);
	switch (smif) {
	case 0x99:
		printk(BIOS_DEBUG, "Sample\n");
		smm_get_gnvs()->smif = 0;
		break;
	default:
		return 0;
	}

	/* On success, the IO Trap Handler returns 0
	 * On failure, the IO Trap Handler returns a value != 0
	 *
	 * For now, we force the return value to 0 and log all traps to
	 * see what's going on.
	 */
	//gnvs->smif = 0;
	return 1;
}


static u8 mainboard_smi_ec(void)
{
	u8 src;
	u32 pm1_cnt;
	static int battery_critical_logged;

	ec_kbc_write_cmd(0x56);
	src = ec_kbc_read_ob();
	printk(BIOS_DEBUG, "mainboard_smi_ec src: %x\n", src);

	switch (src) {
	case EC_BATTERY_CRITICAL:
#if CONFIG_ELOG_GSMI
		if (!battery_critical_logged)
			elog_add_event_byte(ELOG_TYPE_EC_EVENT,
					    EC_EVENT_BATTERY_CRITICAL);
#endif
		battery_critical_logged = 1;
		break;
	case EC_LID_CLOSE:
		printk(BIOS_DEBUG, "LID CLOSED, SHUTDOWN\n");

#if CONFIG_ELOG_GSMI
		elog_add_event_byte(ELOG_TYPE_EC_EVENT, EC_EVENT_LID_CLOSED);
#endif
		/* Go to S5 */
		pm1_cnt = inl(smm_get_pmbase() + PM1_CNT);
		pm1_cnt |= (0xf << 10);
		outl(pm1_cnt, smm_get_pmbase() + PM1_CNT);
		break;
	}

	return src;
}

void mainboard_smi_gpi(u16 gpi_sts)
{
	u32 pm1_cnt;

	printk(BIOS_DEBUG, "mainboard_smi_gpi: %x\n", gpi_sts);
	if (gpi_sts & (1 << EC_SMI_GPI)) {
		/* Process all pending events from EC */
		while (mainboard_smi_ec() != EC_NO_EVENT);
	}
	else if (gpi_sts & (1 << EC_LID_GPI)) {
		printk(BIOS_DEBUG, "LID CLOSED, SHUTDOWN\n");

#if CONFIG_ELOG_GSMI
		elog_add_event_byte(ELOG_TYPE_EC_EVENT, EC_EVENT_LID_CLOSED);
#endif
		/* Go to S5 */
		pm1_cnt = inl(smm_get_pmbase() + PM1_CNT);
		pm1_cnt |= (0xf << 10);
		outl(pm1_cnt, smm_get_pmbase() + PM1_CNT);
	}
}

void mainboard_smi_sleep(u8 slp_typ)
{
	printk(BIOS_DEBUG, "mainboard_smi_sleep: %x\n", slp_typ);
	/* Disable SCI and SMI events */


	/* Clear pending events that may trigger immediate wake */


	/* Enable wake events */


	/* Tell the EC to Disable USB power */
	if (smm_get_gnvs()->s3u0 == 0 && smm_get_gnvs()->s3u1 == 0) {
		ec_kbc_write_cmd(0x45);
		ec_kbc_write_ib(0xF2);
	}
}

#define APMC_FINALIZE 0xcb
#define APMC_ACPI_EN  0xe1
#define APMC_ACPI_DIS 0x1e

static int mainboard_finalized = 0;

int mainboard_smi_apmc(u8 apmc)
{
	printk(BIOS_DEBUG, "mainboard_smi_apmc: %x\n", apmc);
	switch (apmc) {
	case APMC_FINALIZE:
		printk(BIOS_DEBUG, "APMC: FINALIZE\n");
		if (mainboard_finalized) {
			printk(BIOS_DEBUG, "APMC#: Already finalized\n");
			return 0;
		}

		intel_me_finalize_smm();
		intel_pch_finalize_smm();
		intel_sandybridge_finalize_smm();
		intel_model_206ax_finalize_smm();

		mainboard_finalized = 1;
		break;
	case APMC_ACPI_EN:
		printk(BIOS_DEBUG, "APMC: ACPI_EN\n");
		/* Clear all pending events */
		/* EC cmd:59 data:E8 */
		ec_kbc_write_cmd(0x59);
		ec_kbc_write_ib(0xE8);

		/* Set LID GPI to generate SCIs */
		set_lid_gpi_mode(GPI_IS_SCI);

		break;
	case APMC_ACPI_DIS:
		printk(BIOS_DEBUG, "APMC: ACPI_DIS\n");
		/* Clear all pending events */
		/* EC cmd:59 data:e9 */
		ec_kbc_write_cmd(0x59);
		ec_kbc_write_ib(0xE9);

		/* Set LID GPI to generate SMIs */
		set_lid_gpi_mode(GPI_IS_SMI);
		break;
	}
	return 0;
}
