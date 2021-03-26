/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <northbridge/intel/pineview/pineview.h>
#include <northbridge/intel/pineview/chip.h>
#include <option.h>
#include <types.h>

#define LPC_DEV		PCI_DEV(0, 0x1f, 0)

#define CRCLK_PINEVIEW	0x02
#define CDCLK_PINEVIEW	0x10

static void early_graphics_setup(void)
{
	u8 reg8;
	u16 reg16;
	u32 reg32;

	const struct device *d0f0 = pcidev_on_root(0, 0);
	const struct northbridge_intel_pineview_config *config = d0f0->chip_info;

	pci_write_config8(HOST_BRIDGE, DEVEN, BOARD_DEVEN);

	/* Fetch VRAM size from CMOS option */
	if (get_option(&reg8, "gfx_uma_size") != CB_SUCCESS)
		reg8 = 0;	/* 0 for 8MB */

	/* Ensure the setting is valid */
	if (reg8 > 6)
		reg8 = 0;

	/* Select 1M GTT */
	pci_write_config16(HOST_BRIDGE, GGC, (1 << 8) | ((reg8 + 3) << 4));

	printk(BIOS_SPEW, "Set GFX clocks...");
	reg16 = MCHBAR16(MCH_GCFGC);
	MCHBAR16(MCH_GCFGC) = reg16 | (1 << 9);
	reg16 &= ~0x7f;
	reg16 |= CDCLK_PINEVIEW | CRCLK_PINEVIEW;
	reg16 &= ~(1 << 9);
	MCHBAR16(MCH_GCFGC) = reg16;

	/* Graphics core */
	reg8 = MCHBAR8(HPLLVCO);
	reg8 &= 0x7;

	reg16 = pci_read_config16(GMCH_IGD, 0xcc) & ~0x1ff;

	if (reg8 == 0x4) {
		/* 2666MHz */
		reg16 |= 0xad;
	} else if (reg8 == 0) {
		/* 3200MHz */
		reg16 |= 0xa0;
	} else if (reg8 == 1) {
		/* 4000MHz */
		reg16 |= 0xad;
	}

	pci_write_config16(GMCH_IGD, 0xcc, reg16);

	pci_and_config8(GMCH_IGD, 0x62, ~0x3);
	pci_or_config8(GMCH_IGD,  0x62, 2);

	if (config->use_crt) {
		/* Enable VGA */
		MCHBAR32_OR(DACGIOCTRL1, 1 << 15);
	} else {
		/* Disable VGA */
		MCHBAR32_AND(DACGIOCTRL1, ~(1 << 15));
	}

	if (config->use_lvds) {
		/* Enable LVDS */
		reg32 = MCHBAR32(LVDSICR2);
		reg32 &= ~0xf1000000;
		reg32 |=  0x90000000;
		MCHBAR32(LVDSICR2) = reg32;
		MCHBAR32_OR(IOCKTRR1, 1 << 9);
	} else {
		/* Disable LVDS */
		MCHBAR32_OR(DACGIOCTRL1, 3 << 25);
	}

	MCHBAR32(CICTRL) = 0xc6db8b5f;
	MCHBAR16(CISDCTRL) = 0x024f;

	MCHBAR32_AND(DACGIOCTRL1, 0xffffff00);
	MCHBAR32_OR(DACGIOCTRL1,  1 << 5);

	/* Legacy backlight control */
	pci_write_config8(GMCH_IGD, 0xf4, 0x4c);
}

static void early_misc_setup(void)
{
	MCHBAR32(HIT0);
	MCHBAR32(HIT0) = 0x00021800;
	DMIBAR32(0x2c) = 0x86000040;
	pci_write_config32(PCI_DEV(0, 0x1e, 0), 0x18, 0x00020200);
	pci_write_config32(PCI_DEV(0, 0x1e, 0), 0x18, 0x00000000);

	early_graphics_setup();

	MCHBAR32(HIT4);
	MCHBAR32(HIT4) = 0;
	MCHBAR32(HIT4);
	MCHBAR32(HIT4) = 8;

	pci_write_config8(LPC_DEV, 0x08, 0x1d);
	pci_write_config8(LPC_DEV, 0x08, 0x00);
	RCBA32(0x3410) = 0x00020465;

	pci_write_config32(PCI_DEV(0, 0x1d, 0), 0xca, 0x1);
	pci_write_config32(PCI_DEV(0, 0x1d, 1), 0xca, 0x1);
	pci_write_config32(PCI_DEV(0, 0x1d, 2), 0xca, 0x1);
	pci_write_config32(PCI_DEV(0, 0x1d, 3), 0xca, 0x1);

	RCBA32(0x3100) = 0x00042210;
	RCBA32(0x3108) = 0x10004321;
	RCBA32(0x310c) = 0x00214321;
	RCBA32(0x3110) = 1;
	RCBA32(0x3140) = 0x01460132;
	RCBA32(0x3142) = 0x02370146;
	RCBA32(0x3144) = 0x32010237;
	RCBA32(0x3146) = 0x01463201;
	RCBA32(0x3148) = 0x00000146;
}

static void pineview_setup_bars(void)
{
	printk(BIOS_DEBUG, "Setting up static northbridge registers...");
	pci_write_config8(HOST_BRIDGE, 0x08, 0x69);

	/* Set up all hardcoded northbridge BARs */
	pci_write_config32(HOST_BRIDGE, EPBAR,  CONFIG_FIXED_EPBAR_MMIO_BASE  | 1);
	pci_write_config32(HOST_BRIDGE, MCHBAR, CONFIG_FIXED_MCHBAR_MMIO_BASE | 1);
	pci_write_config32(HOST_BRIDGE, DMIBAR, CONFIG_FIXED_DMIBAR_MMIO_BASE | 1);
	pci_write_config32(HOST_BRIDGE, PMIOBAR, DEFAULT_PMIOBAR | 1);

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

void pineview_early_init(void)
{
	/* Print some chipset specific information */
	printk(BIOS_DEBUG, "Intel Pineview northbridge\n");

	/* Setup all BARs required for early PCIe and raminit */
	pineview_setup_bars();

	/* Miscellaneous setup */
	early_misc_setup();

	/* Route port80 to LPC */
	RCBA32(GCS) &= (~0x04);
	RCBA32(0x2010) |= (1 << 10);
}
