/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/device.h>
#include <device/pci_ehci.h>
#include <console/usb.h>

extern void *ehci_bar;

pci_devfn_t pci_ehci_dbg_dev(unsigned hcd_idx)
{
	u32 class;
	pci_devfn_t dev;

#if CONFIG_HAVE_USBDEBUG_OPTIONS
	if (hcd_idx==2)
		dev = PCI_DEV(0, 0x1a, 0);
	else
		dev = PCI_DEV(0, 0x1d, 0);
#else
	dev = PCI_DEV(0, 0x1d, 7);
#endif

	class = pci_read_config32(dev, PCI_CLASS_REVISION) >> 8;
#if CONFIG_HAVE_USBDEBUG_OPTIONS
	if (class != PCI_EHCI_CLASSCODE) {
		/* If we enter here before RCBA programming, EHCI function may
		 * appear with the highest function number instead.
		 */
		dev |= PCI_DEV(0, 0, 7);
		class = pci_read_config32(dev, PCI_CLASS_REVISION) >> 8;
	}
#endif
	if (class != PCI_EHCI_CLASSCODE)
		return 0;

	return dev;
}

void pci_ehci_dbg_set_port(pci_devfn_t dev, unsigned int port)
{
	/* claim USB debug port */
	const unsigned long dbgctl_addr =
		((unsigned long)ehci_bar) + CONFIG_EHCI_DEBUG_OFFSET;
	write32(dbgctl_addr, read32(dbgctl_addr) | (1 << 30));
}

void pci_ehci_dbg_enable(pci_devfn_t dev, unsigned long base)
{
}
