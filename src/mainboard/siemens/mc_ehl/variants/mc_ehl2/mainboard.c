/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <bootstate.h>
#include <console/console.h>
#include <gpio.h>
#include <intelblocks/pcr.h>
#include <soc/gpio.h>
#include <soc/pcr_ids.h>

void variant_mainboard_final(void)
{
	/* PIR8 register mapping for PCIe root ports
	   INTA#->PIRQC#, INTB#->PIRQD#, INTC#->PIRQA#, INTD#-> PIRQB# */
	pcr_write16(PID_ITSS, 0x3150, 0x1032);
}

static void finalize_boot(void *unused)
{
	/* Set coreboot ready LED. */
	gpio_output(GPP_F20, 1);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, finalize_boot, NULL);
