/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/intel/haswell/haswell.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <types.h>

#include "haswell.h"

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

	/* TODO: Update for BDW? Does this even work? MRC/NRI overwrites the value */
	if (cpu_is_broadwell())
		return;

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

static void haswell_setup_misc(void)
{
	u32 reg32;

	/* Erratum workarounds */
	reg32 = mchbar_read32(SAPMCTL);
	reg32 |= (1 << 9) | (1 << 10);
	mchbar_write32(SAPMCTL, reg32);

	if (cpu_is_haswell()) {
		reg32 = mchbar_read32(INTRDIRCTL);
		reg32 |= (1 << 4) | (1 << 5);
		mchbar_write32(INTRDIRCTL, reg32);
	}
}

static void northbridge_setup_iommu(void)
{
	const u32 capid0_a = pci_read_config32(HOST_BRIDGE, CAPID0_A);
	if (capid0_a & VTD_DISABLE)
		return;

	/* Setup BARs: zeroize top 32 bits; set enable bit */
	mchbar_write32(GFXVTBAR + 4, GFXVT_BASE_ADDRESS >> 32);
	mchbar_write32(GFXVTBAR + 0, GFXVT_BASE_ADDRESS | 1);
	mchbar_write32(VTVC0BAR + 4, VTVC0_BASE_ADDRESS >> 32);
	mchbar_write32(VTVC0BAR + 0, VTVC0_BASE_ADDRESS | 1);

	if (cpu_is_haswell()) {
		/*
		 * Intel Document 492662 (Haswell System Agent BIOS Spec), Rev 1.6.0
		 * Section 11.3 - DMA Remapping Engine Configuration
		 */
		const u32 gfxvt_archdis = 0x02100000 | DMAR_LCKDN;
		write32p(GFXVT_BASE_ADDRESS + ARCHDIS, gfxvt_archdis);

		clrsetbits32p(VTVC0_BASE_ADDRESS + 0xf04, 0xf << 15, 1 << 15);

		u32 vtvc0_archdis = 0x000a5003 | DMAR_LCKDN;
		if (pci_read_config16(PCI_DEV(0, 2, 0), PCI_DEVICE_ID) == 0xffff) {
			vtvc0_archdis |= SPCAPCTRL;
		}
		write32p(VTVC0_BASE_ADDRESS + ARCHDIS, vtvc0_archdis);
	}
	if (cpu_is_broadwell()) {
		/*
		 * Intel Document 535094 (Broadwell BIOS Spec), Rev 2.2.0
		 * Section 17.3 - DMA Remapping Engine Configuration
		 */

		/* TODO: For steppings <= D0 (pre-production), also clear bit 0 */
		setbits32p(GFXVT_BASE_ADDRESS + ARCHDIS, DMAR_LCKDN | PRSCAPDIS);

		write32p(GFXVT_BASE_ADDRESS + 0x100, 0x50a);

		clrsetbits32p(VTVC0_BASE_ADDRESS + 0xf04, 0xf << 15, 1 << 15);

		setbits32p(VTVC0_BASE_ADDRESS + ARCHDIS, DMAR_LCKDN);
	}
}

void haswell_early_initialization(void)
{
	/* Setup all BARs required for early PCIe and raminit */
	haswell_setup_bars();

	/* Setup IOMMU BARs */
	northbridge_setup_iommu();

	if (!CONFIG(INTEL_LYNXPOINT_LP))
		northbridge_setup_peg();

	haswell_setup_igd();

	haswell_setup_misc();
}
