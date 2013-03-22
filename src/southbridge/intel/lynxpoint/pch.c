/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright 2013 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <delay.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include "pch.h"

static device_t pch_get_lpc_device(void)
{
#ifdef __SMM__
	return PCI_DEV(0, 0x1f, 0);
#else
	return dev_find_slot(0, PCI_DEVFN(0x1f, 0));
#endif
}

int pch_silicon_revision(void)
{
	static int pch_revision_id = -1;

	if (pch_revision_id < 0)
		pch_revision_id = pci_read_config8(pch_get_lpc_device(),
						   PCI_REVISION_ID);
	return pch_revision_id;
}

int pch_silicon_type(void)
{
	static int pch_type = -1;

	if (pch_type < 0)
		pch_type = pci_read_config8(pch_get_lpc_device(),
					    PCI_DEVICE_ID + 1);
	return pch_type;
}

int pch_is_lp(void)
{
	return pch_silicon_type() == PCH_TYPE_LPT_LP;
}

u16 get_pmbase(void)
{
	static u16 pmbase;

	if (!pmbase)
		pmbase = pci_read_config16(pch_get_lpc_device(),
					   PMBASE) & 0xfffc;
	return pmbase;
}

u16 get_gpiobase(void)
{
	static u16 gpiobase;

	if (!gpiobase)
		gpiobase = pci_read_config16(pch_get_lpc_device(),
					     GPIOBASE) & 0xfffc;
	return gpiobase;
}

#ifndef __SMM__

/* Set bit in Function Disble register to hide this device */
static void pch_hide_devfn(unsigned devfn)
{
	switch (devfn) {
	case PCI_DEVFN(19, 0): /* Audio DSP */
		RCBA32_OR(FD, PCH_DISABLE_ADSPD);
		break;
	case PCI_DEVFN(20, 0): /* XHCI */
		RCBA32_OR(FD, PCH_DISABLE_XHCI);
		break;
	case PCI_DEVFN(21, 0): /* DMA */
		pch_iobp_update(SIO_IOBP_FUNCDIS0, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 1): /* I2C0 */
		pch_iobp_update(SIO_IOBP_FUNCDIS1, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 2): /* I2C1 */
		pch_iobp_update(SIO_IOBP_FUNCDIS2, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 3): /* SPI0 */
		pch_iobp_update(SIO_IOBP_FUNCDIS3, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 4): /* SPI1 */
		pch_iobp_update(SIO_IOBP_FUNCDIS4, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 5): /* UART0 */
		pch_iobp_update(SIO_IOBP_FUNCDIS5, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 6): /* UART1 */
		pch_iobp_update(SIO_IOBP_FUNCDIS6, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
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
	case PCI_DEVFN(23, 0): /* SDIO */
		pch_iobp_update(SIO_IOBP_FUNCDIS7, ~0UL, SIO_IOBP_FUNCDIS_DIS);
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
	case PCI_DEVFN(31, 0): /* LPC */
		RCBA32_OR(FD, PCH_DISABLE_LPC);
		break;
	case PCI_DEVFN(31, 2): /* SATA #1 */
		RCBA32_OR(FD, PCH_DISABLE_SATA1);
		break;
	case PCI_DEVFN(31, 3): /* SMBUS */
		RCBA32_OR(FD, PCH_DISABLE_SMBUS);
		break;
	case PCI_DEVFN(31, 5): /* SATA #2 */
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
	unsigned try;

	for (try = IOBP_RETRY; try > 0; try--) {
		u16 status = RCBA16(IOBPS);
		if ((status & IOBPS_READY) == 0)
			return 1;
		udelay(10);
	}

	printk(BIOS_ERR, "IOBP: timeout waiting for transaction to complete\n");
	return 0;
}

static u32 pch_iobp_read(u32 address)
{
	u16 status;

	if (!iobp_poll())
		return 0;

	/* Set the address */
	RCBA32(IOBPIRI) = address;

	/* READ OPCODE */
	status = RCBA16(IOBPS);
	status &= ~IOBPS_MASK;
	status |= IOBPS_READ;
	RCBA16(IOBPS) = status;

	/* Undocumented magic */
	RCBA16(IOBPU) = IOBPU_MAGIC;

	/* Set ready bit */
	status = RCBA16(IOBPS);
	status |= IOBPS_READY;
	RCBA16(IOBPS) = status;

	if (!iobp_poll())
		return 0;

	/* Check for successful transaction */
	status = RCBA16(IOBPS);
	if (status & IOBPS_TX_MASK) {
		printk(BIOS_ERR, "IOBP: read 0x%08x failed\n", address);
		return 0;
	}

	/* Read IOBP data */
	return RCBA32(IOBPD);
}

void pch_iobp_update(u32 address, u32 andvalue, u32 orvalue)
{
	u16 status;
	u32 data = pch_iobp_read(address);

	/* WRITE OPCODE */
	status = RCBA16(IOBPS);
	status &= ~IOBPS_MASK;
	status |= IOBPS_WRITE;
	RCBA16(IOBPS) = status;

	/* Update the data */
	data &= andvalue;
	data |= orvalue;
	RCBA32(IOBPD) = data;

	/* Undocumented magic */
	RCBA16(IOBPU) = IOBPU_MAGIC;

	/* Set ready bit */
	status = RCBA16(IOBPS);
	status |= IOBPS_READY;
	RCBA16(IOBPS) = status;

	if (!iobp_poll())
		return;

	/* Check for successful transaction */
	status = RCBA16(IOBPS);
	if (status & IOBPS_TX_MASK) {
		printk(BIOS_ERR, "IOBP: write 0x%08x failed\n", address);
		return;
	}

	printk(BIOS_INFO, "IOBP: set 0x%08x to 0x%08x\n", address, data);
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

/* RPFN is a write-once register so keep a copy until it is written */
static u32 new_rpfn;

/* Swap function numbers assigned to two PCIe Root Ports */
static void pch_pcie_function_swap(u8 old_fn, u8 new_fn)
{
	u32 old_rpfn = new_rpfn;

	printk(BIOS_DEBUG, "PCH: Remap PCIe function %d to %d\n",
	       old_fn, new_fn);

	new_rpfn &= ~(RPFN_FNMASK(old_fn) | RPFN_FNMASK(new_fn));

	/* Old function set to new function and disabled */
	new_rpfn |= RPFN_FNSET(old_fn, RPFN_FNGET(old_rpfn, new_fn));
	new_rpfn |= RPFN_FNSET(new_fn, RPFN_FNGET(old_rpfn, old_fn));
}

/* Update devicetree with new Root Port function number assignment */
static void pch_pcie_devicetree_update(void)
{
	device_t dev;

	/* Update the function numbers in the static devicetree */
	for (dev = all_devices; dev; dev = dev->next) {
		u8 new_devfn;

		/* Only care about PCH PCIe root ports */
		if (PCI_SLOT(dev->path.pci.devfn) !=
		    PCH_PCIE_DEV_SLOT)
			continue;

		/* Determine the new devfn for this port */
		new_devfn = PCI_DEVFN(PCH_PCIE_DEV_SLOT,
			      RPFN_FNGET(new_rpfn,
				 PCI_FUNC(dev->path.pci.devfn)));

		if (dev->path.pci.devfn != new_devfn) {
			printk(BIOS_DEBUG,
			       "PCH: PCIe map %02x.%1x -> %02x.%1x\n",
			       PCI_SLOT(dev->path.pci.devfn),
			       PCI_FUNC(dev->path.pci.devfn),
			       PCI_SLOT(new_devfn), PCI_FUNC(new_devfn));

			dev->path.pci.devfn = new_devfn;
		}
	}
}

/* Special handling for PCIe Root Port devices */
static void pch_pcie_enable(device_t dev)
{
	struct southbridge_intel_lynxpoint_config *config = dev->chip_info;
	u32 reg32;

	/*
	 * Save a copy of the Root Port Function Number map when
	 * starting to walk the list of PCIe Root Ports so it can
	 * be updated locally and written out when the last port
	 * has been processed.
	 */
	if (PCI_FUNC(dev->path.pci.devfn) == 0) {
		new_rpfn = RCBA32(RPFN);

		/*
		 * Enable Root Port coalescing if the first port is disabled
		 * or the other devices will not be enumerated by the OS.
		 */
		if (!dev->enabled)
			config->pcie_port_coalesce = 1;

		if (config->pcie_port_coalesce)
			printk(BIOS_INFO,
			       "PCH: PCIe Root Port coalescing is enabled\n");
	}

	if (!dev->enabled) {
		printk(BIOS_DEBUG, "%s: Disabling device\n",  dev_path(dev));

		/*
		 * PCIE Power Savings for PantherPoint and CougarPoint/B1+
		 *
		 * If PCIe 0-3 disabled set Function 0 0xE2[0] = 1
		 * If PCIe 4-7 disabled set Function 4 0xE2[0] = 1
		 *
		 * This check is done here instead of pcie driver
		 * because the pcie driver enable() handler is not
		 * called unless the device is enabled.
		 */
		if ((PCI_FUNC(dev->path.pci.devfn) == 0 ||
		     PCI_FUNC(dev->path.pci.devfn) == 4)) {
			/* Handle workaround for PPT and CPT/B1+ */
			if (!pch_pcie_check_set_enabled(dev)) {
				u8 reg8 = pci_read_config8(dev, 0xe2);
				reg8 |= 1;
				pci_write_config8(dev, 0xe2, reg8);
			}

			/*
			 * Enable Clock Gating for shared PCIe resources
			 * before disabling this particular port.
			 */
			pci_write_config8(dev, 0xe1, 0x3c);
		}

		/* Ensure memory, io, and bus master are all disabled */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 &= ~(PCI_COMMAND_MASTER |
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
		pci_write_config32(dev, PCI_COMMAND, reg32);

		/* Do not claim downstream transactions for PCIe ports */
		new_rpfn |= RPFN_HIDE(PCI_FUNC(dev->path.pci.devfn));

		/* Hide this device if possible */
		pch_hide_devfn(dev->path.pci.devfn);
	} else {
		int fn;

		/*
		 * Check if there is a lower disabled port to swap with this
		 * port in order to maintain linear order starting at zero.
		 */
		if (config->pcie_port_coalesce) {
			for (fn=0; fn < PCI_FUNC(dev->path.pci.devfn); fn++) {
				if (!(new_rpfn & RPFN_HIDE(fn)))
					continue;

				/* Swap places with this function */
				pch_pcie_function_swap(
					PCI_FUNC(dev->path.pci.devfn), fn);
				break;
			}
		}

		/* Enable SERR */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 |= PCI_COMMAND_SERR;
		pci_write_config32(dev, PCI_COMMAND, reg32);
	}

	/*
	 * When processing the last PCIe root port we can now
	 * update the Root Port Function Number and Hide register.
	 */
	if (PCI_FUNC(dev->path.pci.devfn) == 7) {
		printk(BIOS_SPEW, "PCH: RPFN 0x%08x -> 0x%08x\n",
		       RCBA32(RPFN), new_rpfn);
		RCBA32(RPFN) = new_rpfn;

		/* Update static devictree with new function numbers */
		if (config->pcie_port_coalesce)
			pch_pcie_devicetree_update();
	}
}

void pch_enable(device_t dev)
{
	u32 reg32;

	/* PCH PCIe Root Ports get special handling */
	if (PCI_SLOT(dev->path.pci.devfn) == PCH_PCIE_DEV_SLOT)
		return pch_pcie_enable(dev);

	if (!dev->enabled) {
		printk(BIOS_DEBUG, "%s: Disabling device\n",  dev_path(dev));

		/* Ensure memory, io, and bus master are all disabled */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 &= ~(PCI_COMMAND_MASTER |
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
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

struct chip_operations southbridge_intel_lynxpoint_ops = {
	CHIP_NAME("Intel Series 8 (Lynx Point) Southbridge")
	.enable_dev = pch_enable,
};

#endif /* __SMM__ */
