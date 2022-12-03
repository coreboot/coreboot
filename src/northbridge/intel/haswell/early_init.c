/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>

#include "haswell.h"

static bool peg_hidden[3];

static void haswell_setup_bars(void)
{
	printk(BIOS_DEBUG, "Setting up static northbridge registers...");
	/* Set up all hardcoded northbridge BARs */
	pci_write_config32(HOST_BRIDGE, EPBAR,  CONFIG_FIXED_EPBAR_MMIO_BASE  | 1);
	pci_write_config32(HOST_BRIDGE, EPBAR  + 4, 0);
	pci_write_config32(HOST_BRIDGE, MCHBAR, CONFIG_FIXED_MCHBAR_MMIO_BASE | 1);
	pci_write_config32(HOST_BRIDGE, MCHBAR + 4, 0);
	pci_write_config32(HOST_BRIDGE, DMIBAR, CONFIG_FIXED_DMIBAR_MMIO_BASE | 1);
	pci_write_config32(HOST_BRIDGE, DMIBAR + 4, 0);

	mchbar_write32(EDRAMBAR, EDRAM_BASE_ADDRESS | 1);
	mchbar_write32(GDXCBAR, GDXC_BASE_ADDRESS | 1);

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	pci_write_config8(HOST_BRIDGE, PAM0, 0x30);
	pci_write_config8(HOST_BRIDGE, PAM1, 0x33);
	pci_write_config8(HOST_BRIDGE, PAM2, 0x33);
	pci_write_config8(HOST_BRIDGE, PAM3, 0x33);
	pci_write_config8(HOST_BRIDGE, PAM4, 0x33);
	pci_write_config8(HOST_BRIDGE, PAM5, 0x33);
	pci_write_config8(HOST_BRIDGE, PAM6, 0x33);

	printk(BIOS_DEBUG, " done.\n");
}

static void haswell_setup_igd(void)
{
	bool igd_enabled;
	u16 ggc;

	printk(BIOS_DEBUG, "Initializing IGD...\n");

	igd_enabled = !!(pci_read_config32(HOST_BRIDGE, DEVEN) & DEVEN_D2EN);

	ggc = pci_read_config16(HOST_BRIDGE, GGC);
	ggc &= ~0x3f8;
	if (igd_enabled) {
		ggc |= GGC_GTT_2MB | GGC_IGD_MEM_IN_32MB_UNITS(1);
		ggc &= ~GGC_DISABLE_VGA_IO_DECODE;
	} else {
		ggc |= GGC_GTT_0MB | GGC_IGD_MEM_IN_32MB_UNITS(0) | GGC_DISABLE_VGA_IO_DECODE;
	}
	pci_write_config16(HOST_BRIDGE, GGC, ggc);

	if (!igd_enabled) {
		printk(BIOS_DEBUG, "IGD is disabled.\n");
		return;
	}

	/* Enable 256MB aperture */
	pci_update_config8(PCI_DEV(0, 2, 0), MSAC, ~0x06, 0x02);
}

static void start_peg2_link_training(const pci_devfn_t dev)
{
	u32 mask;

	switch (dev) {
	case PCI_DEV(0, 1, 2):
		mask = DEVEN_D1F2EN;
		break;
	case PCI_DEV(0, 1, 1):
		mask = DEVEN_D1F1EN;
		break;
	case PCI_DEV(0, 1, 0):
		mask = DEVEN_D1F0EN;
		break;
	default:
		printk(BIOS_ERR, "Link training tried on a non-PEG device!\n");
		return;
	}

	pci_update_config32(dev, 0xc24, ~(1 << 16), 1 << 5);
	printk(BIOS_DEBUG, "Started PEG1%d link training.\n", PCI_FUNC(PCI_DEV2DEVFN(dev)));

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
		pci_update_config32(HOST_BRIDGE, DEVEN, ~mask, 0);
		peg_hidden[PCI_FUNC(PCI_DEV2DEVFN(dev))] = true;
		printk(BIOS_DEBUG, "Temporarily hiding PEG1%d.\n",
		       PCI_FUNC(PCI_DEV2DEVFN(dev)));
	}
}

void haswell_unhide_peg(void)
{
	u32 deven = pci_read_config32(HOST_BRIDGE, DEVEN);

	for (u8 fn = 0; fn <= 2; fn++) {
		if (peg_hidden[fn]) {
			deven |= DEVEN_D1F0EN >> fn;
			peg_hidden[fn] = false;
			printk(BIOS_DEBUG, "Unhiding PEG1%d.\n", fn);
		}
	}

	pci_write_config32(HOST_BRIDGE, DEVEN, deven);
}

static void haswell_setup_peg(void)
{
	u32 deven = pci_read_config32(HOST_BRIDGE, DEVEN);

	if (deven & DEVEN_D1F2EN)
		start_peg2_link_training(PCI_DEV(0, 1, 2));

	if (deven & DEVEN_D1F1EN)
		start_peg2_link_training(PCI_DEV(0, 1, 1));

	if (deven & DEVEN_D1F0EN)
		start_peg2_link_training(PCI_DEV(0, 1, 0));
}

static void haswell_setup_misc(void)
{
	u32 reg32;

	/* Erratum workarounds */
	reg32 = mchbar_read32(SAPMCTL);
	reg32 |= (1 << 9) | (1 << 10);
	mchbar_write32(SAPMCTL, reg32);

	/* Enable SA Clock Gating */
	reg32 = mchbar_read32(SAPMCTL);
	mchbar_write32(SAPMCTL, reg32 | 1);

	reg32 = mchbar_read32(INTRDIRCTL);
	reg32 |= (1 << 4) | (1 << 5);
	mchbar_write32(INTRDIRCTL, reg32);
}

static void haswell_setup_iommu(void)
{
	const u32 capid0_a = pci_read_config32(HOST_BRIDGE, CAPID0_A);

	if (capid0_a & VTD_DISABLE)
		return;

	/* Setup BARs: zeroize top 32 bits; set enable bit */
	mchbar_write32(GFXVTBAR + 4, GFXVT_BASE_ADDRESS >> 32);
	mchbar_write32(GFXVTBAR + 0, GFXVT_BASE_ADDRESS | 1);
	mchbar_write32(VTVC0BAR + 4, VTVC0_BASE_ADDRESS >> 32);
	mchbar_write32(VTVC0BAR + 0, VTVC0_BASE_ADDRESS | 1);

	/* Set L3HIT2PEND_DIS, lock GFXVTBAR policy config registers */
	u32 reg32;
	reg32 = read32p(GFXVT_BASE_ADDRESS + ARCHDIS);
	write32p(GFXVT_BASE_ADDRESS + ARCHDIS, reg32 | DMAR_LCKDN | L3HIT2PEND_DIS);

	/* Clear SPCAPCTRL */
	reg32 = read32p(VTVC0_BASE_ADDRESS + ARCHDIS) & ~SPCAPCTRL;

	/* Set GLBIOTLBINV, GLBCTXTINV; lock VTVC0BAR policy config registers */
	write32p(VTVC0_BASE_ADDRESS + ARCHDIS,
			reg32 | DMAR_LCKDN | GLBIOTLBINV | GLBCTXTINV);
}

void haswell_early_initialization(void)
{
	/* Setup all BARs required for early PCIe and raminit */
	haswell_setup_bars();

	/* Setup IOMMU BARs */
	haswell_setup_iommu();

	haswell_setup_peg();
	haswell_setup_igd();

	haswell_setup_misc();
}
