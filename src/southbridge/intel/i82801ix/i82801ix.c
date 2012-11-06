/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *               2012 secunet Security Networks AG
 * (Written by Nico Huber <nico.huber@secunet.com> for secunet)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdlib.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include "i82801ix.h"

typedef struct southbridge_intel_i82801ix_config config_t;

static void i82801ix_enable_device(device_t dev)
{
	u32 reg32;

	/* Enable SERR */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_SERR;
	pci_write_config32(dev, PCI_COMMAND, reg32);
}

static void i82801ix_early_settings(const config_t *const info)
{
	/* Program FERR# as processor break event indicator. */
	RCBA32(0x3410) |= (1 << 6);
	/* BIOS must program... */
	RCBA32(0x3430) = (RCBA32(0x3430) & ~(0x3 <<  0)) | (0x2 <<  0);
	RCBA32(0x3418) |= (1 << 0);
	RCBA32(0x350c) = (RCBA32(0x350c) & ~(0x3 << 26)) | (0x2 << 26);
	RCBA32(0x2034) = (RCBA32(0x2034) & ~(0xf << 16)) | (0x5 << 16);
	RCBA32(0x0f20) = (RCBA32(0x0f20) & ~(0xf << 16)) | (0x5 << 16);
	RCBA32(0x1d40) |= (1 << 0);
	RCBA32(0x352c) |= (3 << 16);
}

static void i82801ix_pcie_init(const config_t *const info)
{
	device_t pciePort[6];
	int i, slot_number = 1; /* Reserve slot number 0 for nb's PEG. */
	u32 reg32;

	/* PCIe - BIOS must program... */
	for (i = 0; i < 6; ++i) {
		pciePort[i] = dev_find_slot(0, PCI_DEVFN(0x1c, i));
		if (!pciePort[i]) {
			printk(BIOS_EMERG, "PCIe port 00:1c.%x", i);
			die(" is not listed in devicetree.\n");
		}
#if CONFIG_MMCONF_SUPPORT
		reg32 = pci_mmio_read_config32(pciePort[i], 0x300);
		pci_mmio_write_config32(pciePort[i], 0x300, reg32 | (1 << 21));
		pci_mmio_write_config8(pciePort[i], 0x324, 0x40);
#else
#error "MMIO needed for ICH9 PCIe"
#endif
	}

	if (LPC_IS_MOBILE(dev_find_slot(0, PCI_DEVFN(0x1f, 0)))) {
		for (i = 0; i < 6; ++i) {
			if (pciePort[i]->enabled) {
				reg32 = pci_read_config32(pciePort[i], 0xe8);
				reg32 |= 1;
				pci_write_config32(pciePort[i], 0xe8, reg32);
			}
		}
	}

	for (i = 5; (i >= 0) && !pciePort[i]->enabled; --i) {
		/* Only for the top disabled ports. */
#if CONFIG_MMCONF_SUPPORT
		reg32 = pci_mmio_read_config32(pciePort[i], 0x300);
		reg32 |= 0x3 << 16;
		pci_mmio_write_config32(pciePort[i], 0x300, reg32);
#else
#error "MMIO needed for ICH9 PCIe"
#endif
	}

	/* Set slot implemented, slot number and slot power limits. */
	for (i = 0; i < 6; ++i) {
		const device_t dev = pciePort[i];
		u32 xcap = pci_read_config32(dev, D28Fx_XCAP);
		if (info->pcie_slot_implemented & (1 << i))
			xcap |=  PCI_EXP_FLAGS_SLOT;
		else
			xcap &= ~PCI_EXP_FLAGS_SLOT;
		pci_write_config32(dev, D28Fx_XCAP, xcap);

		if (info->pcie_slot_implemented & (1 << i)) {
			u32 slcap = pci_read_config32(dev, D28Fx_SLCAP);
			slcap &= ~(0x1fff << 19);
			slcap |=  (slot_number++ << 19);
			slcap &= ~(0x0003 << 16);
			slcap |=  (info->pcie_power_limits[i].scale << 16);
			slcap &= ~(0x00ff <<  7);
			slcap |=  (info->pcie_power_limits[i].value <<  7);
			pci_write_config32(dev, D28Fx_SLCAP, slcap);
		}
	}

	/* Lock R/WO ASPM support bits. */
	for (i = 0; i < 6; ++i) {
		reg32 = pci_read_config32(pciePort[i], 0x4c);
		pci_write_config32(pciePort[i], 0x4c, reg32);
	}
}

static void i82801ix_ehci_init(void)
{
	const device_t pciEHCI1 = dev_find_slot(0, PCI_DEVFN(0x1d, 7));
	if (!pciEHCI1)
		die("EHCI controller (00:1d.7) not listed in devicetree.\n");
	const device_t pciEHCI2 = dev_find_slot(0, PCI_DEVFN(0x1a, 7));
	if (!pciEHCI2)
		die("EHCI controller (00:1a.7) not listed in devicetree.\n");

	u32 reg32;

	/* TODO: Maybe we have to save and
		 restore these settings across S3. */
	reg32 = pci_read_config32(pciEHCI1, 0xfc);
	pci_write_config32(pciEHCI1, 0xfc, (reg32 & ~(3 << 2)) |
					   (1 << 29) | (1 << 17) | (2 << 2));
	reg32 = pci_read_config32(pciEHCI2, 0xfc);
	pci_write_config32(pciEHCI2, 0xfc, (reg32 & ~(3 << 2)) |
					   (1 << 29) | (1 << 17) | (2 << 2));
}

static int i82801ix_function_disabled(const unsigned devfn)
{
	const struct device *const dev = dev_find_slot(0, devfn);
	if (!dev) {
		printk(BIOS_EMERG,
		       "PCI device 00:%x.%x",
		       PCI_SLOT(devfn), PCI_FUNC(devfn));
		die(" is not listed in devicetree.\n");
	}
	return !dev->enabled;
}

static void i82801ix_hide_functions(void)
{
	int i;
	u32 reg32;

	/* FIXME: This works pretty good if the devicetree is consistent. But
	          some functions have to be disabled in right order and/or have
		  other constraints. */

	if (i82801ix_function_disabled(PCI_DEVFN(0x19, 0)))
		RCBA32(RCBA_BUC) |= BUC_LAND;

	reg32 = RCBA32(RCBA_FD);
	struct {
		int devfn;
		u32 mask;
	} functions[] = {
		{ PCI_DEVFN(0x1a, 0), FD_U4D },		/* UHCI #4 */
		{ PCI_DEVFN(0x1a, 1), FD_U5D },		/* UHCI #5 */
		{ PCI_DEVFN(0x1a, 2), FD_U6D },		/* UHCI #6 */
		{ PCI_DEVFN(0x1a, 7), FD_EHCI2D },	/* EHCI #2 */
		{ PCI_DEVFN(0x1b, 0), FD_HDAD },	/* HD Audio */
		{ PCI_DEVFN(0x1c, 0), FD_PE1D },	/* PCIe #1 */
		{ PCI_DEVFN(0x1c, 1), FD_PE2D },	/* PCIe #2 */
		{ PCI_DEVFN(0x1c, 2), FD_PE3D },	/* PCIe #3 */
		{ PCI_DEVFN(0x1c, 3), FD_PE4D },	/* PCIe #4 */
		{ PCI_DEVFN(0x1c, 4), FD_PE5D },	/* PCIe #5 */
		{ PCI_DEVFN(0x1c, 5), FD_PE6D },	/* PCIe #6 */
		{ PCI_DEVFN(0x1d, 0), FD_U1D },		/* UHCI #1 */
		{ PCI_DEVFN(0x1d, 1), FD_U2D },		/* UHCI #2 */
		{ PCI_DEVFN(0x1d, 2), FD_U3D },		/* UHCI #3 */
		{ PCI_DEVFN(0x1d, 7), FD_EHCI1D },	/* EHCI #1 */
		{ PCI_DEVFN(0x1f, 0), FD_LBD },		/* LPC */
		{ PCI_DEVFN(0x1f, 2), FD_SAD1 },	/* SATA #1 */
		{ PCI_DEVFN(0x1f, 3), FD_SD },		/* SMBus */
		{ PCI_DEVFN(0x1f, 5), FD_SAD2 },	/* SATA #2 */
		{ PCI_DEVFN(0x1f, 6), FD_TTD },		/* Thermal Throttle */
	};
	for (i = 0; i < ARRAY_SIZE(functions); ++i) {
		if (i82801ix_function_disabled(functions[i].devfn))
			reg32 |= functions[i].mask;
	}
	RCBA32(RCBA_FD) = reg32;
	RCBA32(RCBA_FD) |= (1 << 0); /* BIOS must write this... */
	RCBA32(RCBA_FDSW) |= (1 << 7); /* Lock function-disable? */

	/* Hide PCIe root port PCI functions. RPFN is partially R/WO. */
	reg32 = RCBA32(RCBA_RPFN);
	for (i = 0; i < 6; ++i) {
		if (i82801ix_function_disabled(PCI_DEVFN(0x1c, i)))
			reg32 |= (1 << ((i * 4) + 3));
	}
	RCBA32(RCBA_RPFN) = reg32;

	/* Lock R/WO UHCI controller #6 remapping. */
	RCBA32(RCBA_MAP) = RCBA32(RCBA_MAP);
}

static void i82801ix_init(void *chip_info)
{
	const config_t *const info = (config_t *)chip_info;

	printk(BIOS_DEBUG, "Initializing i82801ix southbridge...\n");

	i82801ix_early_settings(info);

	/* PCI Express setup. */
	i82801ix_pcie_init(info);

	/* EHCI configuration. */
	i82801ix_ehci_init();

	/* Now hide internal functions. We can't access them after this. */
	i82801ix_hide_functions();

	/* Reset watchdog timer. */
#if !CONFIG_HAVE_SMI_HANDLER
	outw(0x0008, DEFAULT_TCOBASE + 0x12); /* Set higher timer value. */
#endif
	outw(0x0000, DEFAULT_TCOBASE + 0x00); /* Update timer. */

#if CONFIG_HAVE_ACPI_RESUME
	switch (pci_read_config32(dev_find_slot(0, PCI_DEVFN(0, 0)), /*D0F0_SKPD*/0xdc)) {
	case SKPAD_NORMAL_BOOT_MAGIC:
		printk(BIOS_DEBUG, "Normal boot.\n");
		acpi_slp_type=0;
		break;
	case SKPAD_ACPI_S3_MAGIC:
		printk(BIOS_DEBUG, "S3 Resume.\n");
		acpi_slp_type=3;
		break;
	default:
		printk(BIOS_DEBUG, "Unknown boot method, assuming normal.\n");
		acpi_slp_type=0;
		break;
	}
#endif
}

struct chip_operations southbridge_intel_i82801ix_ops = {
	CHIP_NAME("Intel ICH9/ICH9-M (82801Ix) Series Southbridge")
	.enable_dev	= i82801ix_enable_device,
	.init		= i82801ix_init,
};
