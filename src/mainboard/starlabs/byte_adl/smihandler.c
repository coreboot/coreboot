/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/io.h>
#include <cpu/x86/smm.h>
#include <intelblocks/pmclib.h>
#include <soc/gpe.h>
#include <soc/iomap.h>
#include <soc/pm.h>
#include <types.h>

void mainboard_smi_sleep_finalize(u8 slp_typ)
{
	if (slp_typ != ACPI_S5)
		return;

	/*
	 * Keep LAN_WAKE# armed in S5 for WOL.
	 * GPE0_LAN_WAK is GPE 112, which is bit 16 in the STD GPE block (127:96).
	 */
	const uint32_t lan_wake_mask = 1U << (GPE0_LAN_WAK - 96);

	/* Clear any pending LAN_WAKE event to avoid immediate wake. */
	outl(lan_wake_mask, ACPI_BASE_ADDRESS + GPE0_STS(GPE_STD));
	pmc_enable_std_gpe(lan_wake_mask);
	outl(lan_wake_mask, ACPI_BASE_ADDRESS + GPE0_STS(GPE_STD));
}
