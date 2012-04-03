/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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

#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include "pch.h"

static int pch_revision_id = -1;

int pch_silicon_revision(void)
{
	if (pch_revision_id < 0)
		pch_revision_id = pci_read_config8(
			dev_find_slot(0, PCI_DEVFN(0x1f, 0)),
			PCI_REVISION_ID);
	return pch_revision_id;
}

/* Set bit in Function Disble register to hide this device */
static void pch_hide_devfn(unsigned devfn)
{
	switch (devfn) {
	case PCI_DEVFN(22, 0): /* MEI #1 */
		RCBA32_OR(FD2, PCH_DISABLE_MEI1);
		break;
	case PCI_DEVFN(22, 1): /* MEI #2 */
		RCBA32_OR(FD2, PCH_DISABLE_MEI2);
		break;
	case PCI_DEVFN(22, 2): /* IDE-R */
		RCBA32_OR(FD2, PCH_DISABLE_IDER);
		break;
	case PCI_DEVFN(22, 3): /* KT */
		RCBA32_OR(FD2, PCH_DISABLE_KT);
		break;
	case PCI_DEVFN(25, 0): /* Gigabit Ethernet */
		RCBA32_OR(BUC, PCH_DISABLE_GBE);
		break;
	case PCI_DEVFN(26, 0): /* EHCI #2 */
		RCBA32_OR(FD, PCH_DISABLE_EHCI2);
		break;
	case PCI_DEVFN(27, 0): /* HD Audio Controller */
		RCBA32_OR(FD, PCH_DISABLE_HD_AUDIO);
		break;
	case PCI_DEVFN(28, 0): /* PCI Express Root Port 1 */
	case PCI_DEVFN(28, 1): /* PCI Express Root Port 2 */
	case PCI_DEVFN(28, 2): /* PCI Express Root Port 3 */
	case PCI_DEVFN(28, 3): /* PCI Express Root Port 4 */
	case PCI_DEVFN(28, 4): /* PCI Express Root Port 5 */
	case PCI_DEVFN(28, 5): /* PCI Express Root Port 6 */
	case PCI_DEVFN(28, 6): /* PCI Express Root Port 7 */
	case PCI_DEVFN(28, 7): /* PCI Express Root Port 8 */
		RCBA32_OR(FD, PCH_DISABLE_PCIE(PCI_FUNC(devfn)));
		break;
	case PCI_DEVFN(29, 0): /* EHCI #1 */
		RCBA32_OR(FD, PCH_DISABLE_EHCI1);
		break;
	case PCI_DEVFN(30, 0): /* PCI-to-PCI Bridge */
		RCBA32_OR(FD, PCH_DISABLE_P2P);
		break;
	case PCI_DEVFN(31, 0): /* LPC */
		RCBA32_OR(FD, PCH_DISABLE_LPC);
		break;
	case PCI_DEVFN(31, 2): /* SATA #1 */
		RCBA32_OR(FD, PCH_DISABLE_SATA1);
		break;
	case PCI_DEVFN(31, 3): /* SMBUS */
		RCBA32_OR(FD, PCH_DISABLE_SMBUS);
		break;
	case PCI_DEVFN(31, 5): /* SATA #22 */
		RCBA32_OR(FD, PCH_DISABLE_SATA2);
		break;
	case PCI_DEVFN(31, 6): /* Thermal Subsystem */
		RCBA32_OR(FD, PCH_DISABLE_THERMAL);
		break;
	}
}

#define IOBP_RETRY 1000
static inline int iobp_poll(void)
{
	unsigned try = IOBP_RETRY;
	u32 data;

	while (try--) {
		data = RCBA32(IOBPS);
		if ((data & 1) == 0)
			return 1;
		udelay(10);
	}

	printk(BIOS_ERR, "IOBP timeout\n");
	return 0;
}

void pch_iobp_update(u32 address, u32 andvalue, u32 orvalue)
{
	u32 data;

	/* Set the address */
	RCBA32(IOBPIRI) = address;

	/* READ OPCODE */
	if (pch_silicon_revision() >= PCH_STEP_B0)
		RCBA32(IOBPS) = IOBPS_RW_BX;
	else
		RCBA32(IOBPS) = IOBPS_READ_AX;
	if (!iobp_poll())
		return;

	/* Read IOBP data */
	data = RCBA32(IOBPD);
	if (!iobp_poll())
		return;

	/* Check for successful transaction */
	if ((RCBA32(IOBPS) & 0x6) != 0) {
		printk(BIOS_ERR, "IOBP read 0x%08x failed\n", address);
		return;
	}

	/* Update the data */
	data &= andvalue;
	data |= orvalue;

	/* WRITE OPCODE */
	if (pch_silicon_revision() >= PCH_STEP_B0)
		RCBA32(IOBPS) = IOBPS_RW_BX;
	else
		RCBA32(IOBPS) = IOBPS_WRITE_AX;
	if (!iobp_poll())
		return;

	/* Write IOBP data */
	RCBA32(IOBPD) = data;
	if (!iobp_poll())
		return;
}

/* Check if any port in set X to X+3 is enabled */
static int pch_pcie_check_set_enabled(device_t dev)
{
	device_t port;
	int port_func;
	int dev_func = PCI_FUNC(dev->path.pci.devfn);

	printk(BIOS_DEBUG, "%s: check set enabled\n", dev_path(dev));

	/* Go through static device tree list of devices
	 * because enumeration is still in progress */
	for (port = all_devices; port; port = port->next) {
		/* Only care about PCIe root ports */
		if (PCI_SLOT(port->path.pci.devfn) !=
		    PCI_SLOT(dev->path.pci.devfn))
			continue;

		/* Check if port is in range and enabled */
		port_func = PCI_FUNC(port->path.pci.devfn);
		if (port_func >= dev_func &&
		    port_func < (dev_func + 4) &&
		    port->enabled)
			return 1;
	}

	/* None of the ports in this set are enabled */
	return 0;
}

void pch_enable(device_t dev)
{
	u32 reg32;

	if (!dev->enabled) {
		printk(BIOS_DEBUG, "%s: Disabling device\n",  dev_path(dev));

		/*
		 * PCIE Power Savings for stepping B1+:
		 *
		 * If PCIe 0-3 disabled set Function 0 0xE2[0] = 1
		 * If PCIe 4-7 disabled set Function 4 0xE2[0] = 1
		 *
		 * This check is done here instead of pcie driver
		 * because the pcie driver enable() handler is not
		 * called unless the device is enabled.
		 */
		if (pch_silicon_revision() >= PCH_STEP_B1 &&
		    PCI_SLOT(dev->path.pci.devfn) == PCH_PCIE_DEV_SLOT &&
		    (PCI_FUNC(dev->path.pci.devfn) == 0 ||
		     PCI_FUNC(dev->path.pci.devfn) == 4)) {
			if (!pch_pcie_check_set_enabled(dev)) {
				u8 reg8 = pci_read_config8(dev, 0xe2);
				reg8 |= 1;
				pci_write_config8(dev, 0xe2, reg8);
			}
		}

		/* Ensure memory, io, and bus master are all disabled */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 &= ~(PCI_COMMAND_MASTER |
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO	);
		pci_write_config32(dev, PCI_COMMAND, reg32);

		/* Hide this device if possible */
		pch_hide_devfn(dev->path.pci.devfn);
	} else {
		/* Enable SERR */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 |= PCI_COMMAND_SERR;
		pci_write_config32(dev, PCI_COMMAND, reg32);
	}
}

struct chip_operations southbridge_intel_bd82x6x_ops = {
	CHIP_NAME("Intel Series 6 (" CONFIG_PCH_CHIP_NAME ") Southbridge")
	.enable_dev = pch_enable,
};
