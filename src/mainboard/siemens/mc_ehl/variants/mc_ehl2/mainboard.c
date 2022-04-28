/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <bootstate.h>
#include <device/pci_ids.h>
#include <gpio.h>
#include <intelblocks/pcr.h>
#include <soc/gpio.h>
#include <soc/pcr_ids.h>

void variant_mainboard_final(void)
{
	struct device *dev;

	/* PIR8 register mapping for PCIe root ports
	   INTA#->PIRQC#, INTB#->PIRQD#, INTC#->PIRQA#, INTD#-> PIRQB# */
	pcr_write16(PID_ITSS, 0x3150, 0x1032);

	/* Disable clock outputs 1-5 (CLKOUT) for XIO2001 PCIe to PCI Bridge. */
	dev = dev_find_device(PCI_VID_TI, PCI_DID_TI_XIO2001, 0);
	if (dev)
		pci_write_config8(dev, 0xd8, 0x3e);
}

static void finalize_boot(void *unused)
{
	/* Set coreboot ready LED. */
	gpio_output(GPP_F20, 1);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, finalize_boot, NULL);
