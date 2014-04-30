/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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
#include <broadwell/iobp.h>
#include <broadwell/pch.h>
#include <broadwell/pci_devs.h>
#include <broadwell/ramstage.h>
#include <broadwell/rcba.h>
#include <broadwell/serialio.h>
#include <broadwell/spi.h>

u8 pch_revision(void)
{
	return pci_read_config8(PCH_DEV_LPC, PCI_REVISION_ID);
}

u16 pch_type(void)
{
	return pci_read_config16(PCH_DEV_LPC, PCI_DEVICE_ID);
}

/* Return 1 if PCH type is WildcatPoint */
int pch_is_wpt(void)
{
	return ((pch_type() & 0xfff0) == 0x9cc0) ? 1 : 0;
}

/* Return 1 if PCH type is WildcatPoint ULX */
int pch_is_wpt_ulx(void)
{
	u16 lpcid = pch_type();

	switch (lpcid) {
	case PCH_WPT_BDW_Y_SAMPLE:
	case PCH_WPT_BDW_Y_PREMIUM:
	case PCH_WPT_BDW_Y_BASE:
		return 1;
	}

	return 0;
}

u32 pch_read_soft_strap(int id)
{
	u32 fdoc;

	fdoc = SPIBAR32(SPIBAR_FDOC);
	fdoc &= ~0x00007ffc;
	SPIBAR32(SPIBAR_FDOC) = fdoc;

	fdoc |= 0x00004000;
	fdoc |= id * 4;
	SPIBAR32(SPIBAR_FDOC) = fdoc;

	return SPIBAR32(SPIBAR_FDOD);
}

#ifndef __PRE_RAM__

/* Put device in D3Hot Power State */
static void pch_enable_d3hot(device_t dev)
{
	u32 reg32 = pci_read_config32(dev, PCH_PCS);
	reg32 |= PCH_PCS_PS_D3HOT;
	pci_write_config32(dev, PCH_PCS, reg32);
}

/* Set bit in Function Disble register to hide this device */
void pch_disable_devfn(device_t dev)
{
	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(19, 0): /* Audio DSP */
		RCBA32_OR(FD, PCH_DISABLE_ADSPD);
		break;
	case PCI_DEVFN(20, 0): /* XHCI */
		RCBA32_OR(FD, PCH_DISABLE_XHCI);
		break;
	case PCI_DEVFN(21, 0): /* DMA */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS0, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 1): /* I2C0 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS1, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 2): /* I2C1 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS2, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 3): /* SPI0 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS3, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 4): /* SPI1 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS4, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 5): /* UART0 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS5, ~0UL, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 6): /* UART1 */
		pch_enable_d3hot(dev);
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
		pch_enable_d3hot(dev);
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
		RCBA32_OR(FD, PCH_DISABLE_PCIE(PCI_FUNC(dev->path.pci.devfn)));
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

void broadwell_pch_enable_dev(device_t dev)
{
	u32 reg32;

	/* PCH PCIe Root Ports are handled in PCIe driver. */
	if (PCI_SLOT(dev->path.pci.devfn) == PCH_DEV_SLOT_PCIE)
		return;

	if (!dev->enabled) {
		printk(BIOS_DEBUG, "%s: Disabling device\n", dev_path(dev));

		/* Ensure memory, io, and bus master are all disabled */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 &= ~(PCI_COMMAND_MASTER |
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
		pci_write_config32(dev, PCI_COMMAND, reg32);

		/* Disable this device if possible */
		pch_disable_devfn(dev);
	} else {
		/* Enable SERR */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 |= PCI_COMMAND_SERR;
		pci_write_config32(dev, PCI_COMMAND, reg32);
	}
}

#endif
