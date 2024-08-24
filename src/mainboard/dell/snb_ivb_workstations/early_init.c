/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pci_ops.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <superio/smsc/sch5545/sch5545.h>
#include <superio/smsc/sch5545/sch5545_emi.h>

#include <baseboard/sch5545_ec.h>

void bootblock_mainboard_early_init(void)
{
	/*
	 * FIXME: the board gets stuck in reset loop in
	 * mainboard_romstage_entry. Avoid that by clearing SSKPD
	 */
	pci_write_config32(HOST_BRIDGE, MCHBAR, CONFIG_FIXED_MCHBAR_MMIO_BASE | 1);
	pci_write_config32(HOST_BRIDGE, MCHBAR + 4, 0);
	mchbar_write16(SSKPD_HI, 0);

	sch5545_early_init(0x2e);
	/* Bare EC and SIO GPIO initialization which allows to enable serial port */
	sch5545_emi_init(0x2e);
	sch5545_emi_disable_interrupts();
	sch5545_ec_early_init();

	if (CONFIG(CONSOLE_SERIAL))
		sch5545_enable_uart(0x2e, 0);
}
