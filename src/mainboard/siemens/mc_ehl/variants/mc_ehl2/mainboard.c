/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>

void variant_mainboard_final(void)
{
	/* PIR8 register mapping for PCIe root ports
	   INTA#->PIRQC#, INTB#->PIRQD#, INTC#->PIRQA#, INTD#-> PIRQB# */
	pcr_write16(PID_ITSS, 0x3150, 0x1032);
}
