/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_ops.h>
#include <northbridge/intel/haswell/haswell.h>
#include <types.h>

static bool peg_hidden[MAX_PEG_FUNC];

static u32 deven_for_peg(const u8 func)
{
	switch (func) {
		case 0:  return DEVEN_D1F0EN;
		case 1:  return DEVEN_D1F1EN;
		case 2:  return DEVEN_D1F2EN;
		default: return 0;
	}
}

static void start_peg2_link_training(const u8 func)
{
	const pci_devfn_t dev = PEG_DEV(func);

	pci_update_config32(dev, 0xc24, ~(1 << 16), 1 << 5);
	printk(BIOS_DEBUG, "Started PEG1%u link training.\n", func);

	/*
	 * The MRC will perform PCI enumeration, and if it detects a VGA
	 * device in a PEG slot, it will disable the IGD and not reserve
	 * any memory for it. Since the memory map is locked by the time
	 * MRC finishes, the IGD can't be enabled afterwards. Wonderful.
	 *
	 * If one really wants to enable the Intel iGPU as primary, hide
	 * all PEG devices during MRC execution. This will trick the MRC
	 * into thinking there aren't any, and will enable the IGD. Note
	 * that PEG AFE settings will not be programmed, which may cause
	 * stability problems at higher PCIe link speeds. The most ideal
	 * way to fix this problem for good is to implement native init.
	 */
	if (CONFIG(HASWELL_HIDE_PEG_FROM_MRC)) {
		pci_and_config32(HOST_BRIDGE, DEVEN, ~deven_for_peg(func));
		peg_hidden[func] = true;
		printk(BIOS_DEBUG, "Temporarily hiding PEG1%u.\n", func);
	}
}

void northbridge_setup_peg(void)
{
	const u32 deven = pci_read_config32(HOST_BRIDGE, DEVEN);

	for (u8 func = 0; func < MAX_PEG_FUNC; func++) {
		if (deven & deven_for_peg(func)) {
			start_peg2_link_training(func);
		}
	}
}

void northbridge_unhide_peg(void)
{
	u32 deven = pci_read_config32(HOST_BRIDGE, DEVEN);

	for (u8 func = 0; func < MAX_PEG_FUNC; func++) {
		if (peg_hidden[func]) {
			deven |= deven_for_peg(func);
			peg_hidden[func] = false;
			printk(BIOS_DEBUG, "Unhiding PEG1%u.\n", func);
		}
	}

	pci_write_config32(HOST_BRIDGE, DEVEN, deven);
}
