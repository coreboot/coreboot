/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <device/pci.h>
#include "iobp.h"
#include "pch.h"

#ifdef __SIMPLE_DEVICE__
static pci_devfn_t pch_get_lpc_device(void)
{
	return PCI_DEV(0, 0x1f, 0);
}
#else
static struct device *pch_get_lpc_device(void)
{
	return pcidev_on_root(0x1f, 0);
}
#endif

int pch_silicon_revision(void)
{
	static int pch_revision_id = -1;

	if (pch_revision_id < 0)
		pch_revision_id = pci_read_config8(pch_get_lpc_device(),
						   PCI_REVISION_ID);
	return pch_revision_id;
}

int pch_silicon_id(void)
{
	static int pch_id = -1;

	if (pch_id < 0)
		pch_id = pci_read_config16(pch_get_lpc_device(), PCI_DEVICE_ID);

	return pch_id;
}

enum pch_platform_type get_pch_platform_type(void)
{
	const u16 did = pci_read_config16(pch_get_lpc_device(), PCI_DEVICE_ID);

	/* Check if this is a LPT-LP or WPT-LP device ID */
	if ((did & 0xff00) == 0x9c00)
		return PCH_TYPE_ULT;

	/* Non-LP laptop SKUs have an odd device ID (least significant bit is one) */
	if (did & 1)
		return PCH_TYPE_MOBILE;

	/* Desktop and Server SKUs have an even device ID */
	return PCH_TYPE_DESKTOP;
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

#ifndef __SIMPLE_DEVICE__

/* Put device in D3Hot Power State */
static void pch_enable_d3hot(struct device *dev)
{
	pci_or_config32(dev, PCH_PCS, PCH_PCS_PS_D3HOT);
}

/* Set bit in function disable register to hide this device */
void pch_disable_devfn(struct device *dev)
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
		pch_iobp_update(SIO_IOBP_FUNCDIS0, ~0, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 1): /* I2C0 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS1, ~0, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 2): /* I2C1 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS2, ~0, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 3): /* SPI0 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS3, ~0, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 4): /* SPI1 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS4, ~0, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 5): /* UART0 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS5, ~0, SIO_IOBP_FUNCDIS_DIS);
		break;
	case PCI_DEVFN(21, 6): /* UART1 */
		pch_enable_d3hot(dev);
		pch_iobp_update(SIO_IOBP_FUNCDIS6, ~0, SIO_IOBP_FUNCDIS_DIS);
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
		pch_iobp_update(SIO_IOBP_FUNCDIS7, ~0, SIO_IOBP_FUNCDIS_DIS);
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

void pch_enable(struct device *dev)
{
	/* PCH PCIe Root Ports are handled in PCIe driver. */
	if (PCI_SLOT(dev->path.pci.devfn) == PCH_PCIE_DEV_SLOT)
		return;

	if (!dev->enabled) {
		printk(BIOS_DEBUG, "%s: Disabling device\n", dev_path(dev));

		/* Ensure memory, io, and bus master are all disabled */
		pci_and_config16(dev, PCI_COMMAND,
				~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO));

		/* Disable this device if possible */
		pch_disable_devfn(dev);
	} else {
		/* Enable SERR */
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_SERR);
	}
}

struct chip_operations southbridge_intel_lynxpoint_ops = {
	CHIP_NAME("Intel Series 8 (Lynx Point) Southbridge")
	.enable_dev = pch_enable,
};

#endif /* __SIMPLE_DEVICE__ */
