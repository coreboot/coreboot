/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include <arch/io.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/x86/smm.h>

#include "pch.h"

void smm_southbridge_clear_state(void)
{
	u32 smi_en;

	/* Log events from chipset before clearing */
	if (CONFIG(ELOG))
		pch_log_state();

	smi_en = inl(get_pmbase() + SMI_EN);
	if (smi_en & APMC_EN) {
		printk(BIOS_INFO, "SMI# handler already enabled?\n");
		return;
	}

	/* Dump and clear status registers */
	clear_smi_status();
	clear_pm1_status();
	clear_tco_status();
	clear_gpe_status();
}

static void smm_southbridge_enable(uint16_t pm1_events)
{
	printk(BIOS_DEBUG, "Enabling SMIs.\n");
	/* Configure events */
	enable_pm1(pm1_events);
	disable_gpe(PME_B0_EN);

	/* Enable SMI generation:
	 *  - on APMC writes (io 0xb2)
	 *  - on writes to SLP_EN (sleep states)
	 *  - on writes to GBL_RLS (bios commands)
	 * No SMIs:
	 *  - on microcontroller writes (io 0x62/0x66)
	 *  - on TCO events
	 */
	enable_smi(APMC_EN | SLP_SMI_EN | GBL_SMI_EN | EOS);
}

void global_smi_enable(void)
{
	smm_southbridge_enable(PWRBTN_EN | GBL_EN);
}
