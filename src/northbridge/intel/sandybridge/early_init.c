/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <option.h>
#include <types.h>

#include "sandybridge.h"

static void systemagent_vtd_init(void)
{
	const u32 capid0_a = pci_read_config32(HOST_BRIDGE, CAPID0_A);
	if (capid0_a & (1 << 23))
		return;

	/* Setup BARs */
	mchbar_write32(GFXVTBAR + 4, GFXVT_BASE >> 32);
	mchbar_write32(GFXVTBAR + 0, GFXVT_BASE | 1);
	mchbar_write32(VTVC0BAR + 4, VTVC0_BASE >> 32);
	mchbar_write32(VTVC0BAR + 0, VTVC0_BASE | 1);

	/* Lock policies */
	write32p(GFXVT_BASE + 0xff0, 0x80000000);

	const struct device *const azalia = pcidev_on_root(0x1b, 0);
	if (azalia && azalia->enabled) {
		write32p(VTVC0_BASE + 0xff0, 0x20000000);
		write32p(VTVC0_BASE + 0xff0, 0xa0000000);
	} else {
		write32p(VTVC0_BASE + 0xff0, 0x80000000);
	}
}

static void enable_pam_region(void)
{
	pci_write_config8(HOST_BRIDGE, PAM0, 0x30);
	pci_write_config8(HOST_BRIDGE, PAM1, 0x33);
	pci_write_config8(HOST_BRIDGE, PAM2, 0x33);
	pci_write_config8(HOST_BRIDGE, PAM3, 0x33);
	pci_write_config8(HOST_BRIDGE, PAM4, 0x33);
	pci_write_config8(HOST_BRIDGE, PAM5, 0x33);
	pci_write_config8(HOST_BRIDGE, PAM6, 0x33);
}

static void sandybridge_setup_bars(void)
{
	printk(BIOS_DEBUG, "Setting up static northbridge registers...");
	/* Set up all hardcoded northbridge BARs */
	pci_write_config32(HOST_BRIDGE, EPBAR,  CONFIG_FIXED_EPBAR_MMIO_BASE  | 1);
	pci_write_config32(HOST_BRIDGE, EPBAR  + 4, 0);
	pci_write_config32(HOST_BRIDGE, MCHBAR, CONFIG_FIXED_MCHBAR_MMIO_BASE | 1);
	pci_write_config32(HOST_BRIDGE, MCHBAR + 4, 0);
	pci_write_config32(HOST_BRIDGE, DMIBAR, CONFIG_FIXED_DMIBAR_MMIO_BASE | 1);
	pci_write_config32(HOST_BRIDGE, DMIBAR + 4, 0);

	printk(BIOS_DEBUG, " done\n");
}

static void sandybridge_setup_graphics(void)
{
	u16 reg16;
	u8 gfxsize;

	reg16 = pci_read_config16(PCI_DEV(0, 2, 0), PCI_DEVICE_ID);
	switch (reg16) {
	case 0x0102: /* GT1 Desktop */
	case 0x0106: /* GT1 Mobile */
	case 0x010a: /* GT1 Server */
	case 0x0112: /* GT2 Desktop */
	case 0x0116: /* GT2 Mobile */
	case 0x0122: /* GT2 Desktop >=1.3GHz */
	case 0x0126: /* GT2 Mobile >=1.3GHz */
	case 0x0152: /* IvyBridge */
	case 0x0156: /* IvyBridge */
	case 0x0162: /* IvyBridge */
	case 0x0166: /* IvyBridge */
	case 0x016a: /* IvyBridge */
		break;
	default:
		printk(BIOS_DEBUG, "Graphics not supported by this CPU/chipset.\n");
		return;
	}

	printk(BIOS_DEBUG, "Initializing Graphics...\n");

	/* Fall back to 32 MiB for IGD memory by setting GGC[7:3] = 1 */
	gfxsize = get_uint_option("gfx_uma_size", 0);

	reg16 = pci_read_config16(HOST_BRIDGE, GGC);
	reg16 &= ~0x00f8;
	reg16 |= (gfxsize + 1) << 3;
	/* Program GTT memory by setting GGC[9:8] = 2MB */
	reg16 &= ~0x0300;
	reg16 |= 2 << 8;
	/* Enable VGA decode */
	reg16 &= ~0x0002;
	pci_write_config16(HOST_BRIDGE, GGC, reg16);

	/* Enable 256MB aperture */
	pci_update_config8(PCI_DEV(0, 2, 0), MSAC, ~0x06, 0x02);

	/* Erratum workarounds */
	mchbar_setbits32(SAPMCTL, 1 << 9 | 1 << 10);

	/* Enable SA Clock Gating */
	mchbar_setbits32(SAPMCTL, 1 << 0);

	/* GPU RC6 workaround for sighting 366252 */
	mchbar_setbits32(SSKPD_HI, 1 << 31);

	/* VLW (Virtual Legacy Wire?) */
	mchbar_clrbits32(0x6120, 1 << 0);

	mchbar_setbits32(INTRDIRCTL, 1 << 4 | 1 << 5);
}

static void start_peg_link_training(void)
{
	u32 deven;

	const u16 base_rev = pci_read_config16(HOST_BRIDGE, PCI_DEVICE_ID) & BASE_REV_MASK;
	/*
	 * PEG on IvyBridge+ needs a special startup sequence.
	 * As the MRC has its own initialization code skip it.
	 */
	if ((base_rev != BASE_REV_IVB) || CONFIG(HAVE_MRC))
		return;

	deven = pci_read_config32(HOST_BRIDGE, DEVEN);

	/*
	 * For each PEG device, set bit 5 to use three retries for OC (Offset Calibration).
	 * We also clear DEFER_OC (bit 16) in order to start PEG training.
	 */
	if (deven & DEVEN_PEG10)
		pci_update_config32(PCI_DEV(0, 1, 0), AFE_PWRON, ~(1 << 16), 1 << 5);

	if (deven & DEVEN_PEG11)
		pci_update_config32(PCI_DEV(0, 1, 1), AFE_PWRON, ~(1 << 16), 1 << 5);

	if (deven & DEVEN_PEG12)
		pci_update_config32(PCI_DEV(0, 1, 2), AFE_PWRON, ~(1 << 16), 1 << 5);

	if (deven & DEVEN_PEG60)
		pci_update_config32(PCI_DEV(0, 6, 0), AFE_PWRON, ~(1 << 16), 1 << 5);
}

void systemagent_early_init(void)
{
	u32 capid0_a;
	u8 reg8;

	/* Device ID Override Enable should be done very early */
	capid0_a = pci_read_config32(HOST_BRIDGE, CAPID0_A);
	if (capid0_a & (1 << 10)) {
		const size_t is_mobile = get_platform_type() == PLATFORM_MOBILE;

		reg8 = pci_read_config8(HOST_BRIDGE, DIDOR);
		reg8 &= ~7; /* Clear 2:0 */

		if (is_mobile)
			reg8 |= 1; /* Set bit 0 */

		pci_write_config8(HOST_BRIDGE, DIDOR, reg8);
	}

	/* Setup all BARs required for early PCIe and raminit */
	sandybridge_setup_bars();

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	enable_pam_region();

	/* Setup IOMMU BARs */
	systemagent_vtd_init();

	/* Device Enable, don't touch PEG bits */
	pci_or_config32(HOST_BRIDGE, DEVEN, DEVEN_IGD);

	sandybridge_setup_graphics();

	/*
	 * Write magic values to start PEG link training. This should be done in PCI device
	 * enumeration, but the PCIe specification requires to wait at least 100msec after
	 * reset for devices to come up. As we don't want to increase boot time, enable it
	 * early and assume that PEG is up as soon as PCI enumeration starts.
	 *
	 * TODO: use timestamps to ensure the timings are met.
	 */
	start_peg_link_training();
}

void northbridge_romstage_finalize(void)
{
	mchbar_write16(SSKPD_HI, 0xcafe);
}
