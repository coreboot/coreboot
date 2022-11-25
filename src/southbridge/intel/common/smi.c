/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <cpu/intel/smm_reloc.h>
#include <southbridge/intel/common/pmbase.h>
#include <southbridge/intel/common/pmutil.h>

#include "pmutil.h"

u16 get_pmbase(void)
{
	return lpc_get_pmbase();
}

static int smi_enabled(void)
{
	u32 smi_en;

	/* Log events from chipset before clearing */
	if (CONFIG(ELOG))
		pch_log_state();

	smi_en = read_pmbase32(SMI_EN);
	if (smi_en & APMC_EN) {
		printk(BIOS_INFO, "SMI# handler already enabled?\n");
		return 1;
	}

	return 0;
}

static void smm_southbridge_enable(uint16_t pm1_events)
{
	u32 smi_en;
	u32 gpe0_en;

	/* Disable GPE0 PME_B0 */
	gpe0_en = read_pmbase32(GPE0_EN);
	gpe0_en &= ~PME_B0_EN;
	write_pmbase32(GPE0_EN, gpe0_en);

	write_pmbase16(PM1_EN, pm1_events);

	/* Enable SMI generation:
	 *  - on TCO events
	 *  - on APMC writes (io 0xb2)
	 *  - on writes to SLP_EN (sleep states)
	 * No SMIs:
	 *  - on microcontroller writes (io 0x62/0x66)
	 */

	smi_en = 0; /* reset SMI enables */
	smi_en |= TCO_EN;
	smi_en |= APMC_EN;
	if (CONFIG(DEBUG_PERIODIC_SMI))
		smi_en |= PERIODIC_EN;
	smi_en |= SLP_SMI_EN;

	/* The following need to be on for SMIs to happen */
	smi_en |= EOS | GBL_SMI_EN;

	write_pmbase32(SMI_EN, smi_en);
}

void global_smi_enable(void)
{
	if (smi_enabled())
		return;

	dump_all_status();
	smm_southbridge_enable(PWRBTN_EN | GBL_EN);
}

void smm_southbridge_clear_state(void)
{
	if (smi_enabled())
		return;

	/* Dump and clear status registers */
	reset_smi_status();
	reset_pm1_status();
	reset_tco_status();
	reset_gpe0_status();
}
