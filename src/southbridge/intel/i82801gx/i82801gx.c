/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include "i82801gx.h"
#include "sata.h"

static void ich_hide_devfn(unsigned int devfn)
{
	switch (devfn) {
	case PCI_DEVFN(27, 0): /* HD Audio Controller */
		RCBA32_OR(FD, FD_HDAUD);
		break;
	case PCI_DEVFN(28, 0): /* PCI Express Root Port 1 */
	case PCI_DEVFN(28, 1): /* PCI Express Root Port 2 */
	case PCI_DEVFN(28, 2): /* PCI Express Root Port 3 */
	case PCI_DEVFN(28, 3): /* PCI Express Root Port 4 */
	case PCI_DEVFN(28, 4): /* PCI Express Root Port 5 */
	case PCI_DEVFN(28, 5): /* PCI Express Root Port 6 */
		RCBA32_OR(FD, ICH_DISABLE_PCIE(PCI_FUNC(devfn)));
		break;
	case PCI_DEVFN(29, 0): /* UHCI #1 */
	case PCI_DEVFN(29, 1): /* UHCI #2 */
	case PCI_DEVFN(29, 2): /* UHCI #3 */
	case PCI_DEVFN(29, 3): /* UHCI #4 */
		RCBA32_OR(FD, ICH_DISABLE_UHCI(PCI_FUNC(devfn)));
		break;
	case PCI_DEVFN(29, 7): /* EHCI #1 */
		RCBA32_OR(FD, FD_EHCI);
		break;
	case PCI_DEVFN(30, 2): /* AC Audio */
		RCBA32_OR(FD, FD_ACAUD);
		break;
	case PCI_DEVFN(30, 3): /* AC Modem */
		RCBA32_OR(FD, FD_ACMOD);
		break;
	case PCI_DEVFN(31, 0): /* LPC */
		RCBA32_OR(FD, FD_LPCB);
		break;
	case PCI_DEVFN(31, 1): /* PATA #1 */
		RCBA32_OR(FD, FD_PATA);
		break;
	case PCI_DEVFN(31, 2): /* SATA #1 */
		RCBA32_OR(FD, FD_SATA);
		break;
	case PCI_DEVFN(31, 3): /* SMBUS */
		RCBA32_OR(FD, FD_SMBUS);
		break;
	}
}

void i82801gx_enable(struct device *dev)
{
	u16 reg16;

	if (!dev->enabled) {
		printk(BIOS_DEBUG, "%s: Disabling device\n", dev_path(dev));

		/* Ensure memory, io, and bus master are all disabled */
		reg16 = pci_read_config16(dev, PCI_COMMAND);
		reg16 &= ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
		pci_write_config16(dev, PCI_COMMAND, reg16);

		/* Hide this device if possible */
		ich_hide_devfn(dev->path.pci.devfn);
	} else {
		/* Enable SERR */
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_SERR);

		if (dev->path.pci.devfn == PCI_DEVFN(31, 2)) {
			printk(BIOS_DEBUG, "Set SATA mode early\n");
			sata_enable(dev);
		}
	}
}

static void i82801gx_init(void *chip_info)
{
	/* Disable performance counter */
	RCBA32_OR(FD, 1);
}

struct chip_operations southbridge_intel_i82801gx_ops = {
	CHIP_NAME("Intel ICH7/ICH7-M (82801Gx) Series Southbridge")
	.enable_dev =	i82801gx_enable,
	.init =		i82801gx_init,
};
