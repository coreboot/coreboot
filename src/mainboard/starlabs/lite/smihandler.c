/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <cpu/x86/smm.h>
#include <intelblocks/xhci.h>

void mainboard_smi_sleep(u8 slp_typ)
{
	/*
	 * Workaround: Reset the XHCI controller prior to S5 to avoid
	 * XHCI preventing shutdown. Linux needs to put the XHCI into D3
	 * before shutting down but the powerstate commands do not
	 * perform a reset.
	 */
	if (slp_typ == ACPI_S5)
		xhci_host_reset();
}
