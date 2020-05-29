/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_USB_PCI_XHCI__
#define __DRIVERS_USB_PCI_XHCI__

#include <commonlib/bsd/cb_err.h>
#include <device/device.h>

/*
 * Returns the wake GPE for the Extensible Host Controller.
 * Set gpe to -1 if there is no GPE is available.
 */
enum cb_err pci_xhci_get_wake_gpe(const struct device *dev, int *gpe);

#endif /* __DRIVERS_USB_PCI_XHCI__ */
