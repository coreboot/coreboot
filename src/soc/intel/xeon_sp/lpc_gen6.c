/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <intelblocks/itss.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>

void lpc_soc_init(struct device *dev)
{
	printk(BIOS_SPEW, "pch: lpc_init\n");

	/* Program irq pin/line for PCI devices by PCH convention */
	pch_pirq_init();

	/* Explicitly set polarity low for PIRQA to PIRQH */
	for (int i = 0; i < PIRQ_COUNT; i++) {
		itss_set_irq_polarity(pcr_read8(PID_ITSS, PCR_ITSS_PIRQA_ROUT + i), 1);
	}
}
