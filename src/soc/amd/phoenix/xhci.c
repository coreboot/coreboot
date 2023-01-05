/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Update for Phoenix */

#include <amdblocks/gpio.h>
#include <amdblocks/smi.h>
#include <bootstate.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <drivers/usb/pci_xhci/pci_xhci.h>
#include <soc/pci_devs.h>
#include <soc/smi.h>

static const struct sci_source xhci_sci_sources[] = {
	{
		.scimap = SMITYPE_XHC0_PME,
		.gpe = GEVENT_31,
		.direction = SMI_SCI_LVL_HIGH,
		.level = SMI_SCI_EDG
	},
	{
		.scimap = SMITYPE_XHC1_PME,
		.gpe = GEVENT_31,
		.direction = SMI_SCI_LVL_HIGH,
		.level = SMI_SCI_EDG
	}
};

enum cb_err pci_xhci_get_wake_gpe(const struct device *dev, int *gpe)
{
	if (dev->bus->dev->path.type != DEVICE_PATH_PCI)
		return CB_ERR_ARG;

	if (dev->path.type != DEVICE_PATH_PCI)
		return CB_ERR_ARG;

	if (dev->bus->dev->path.pci.devfn == PCIE_ABC_A_DEVFN) {
		if (dev->path.pci.devfn == XHCI0_DEVFN) {
			*gpe = xhci_sci_sources[0].gpe;
			return CB_SUCCESS;
		} else if (dev->path.pci.devfn == XHCI1_DEVFN) {
			*gpe = xhci_sci_sources[1].gpe;
			return CB_SUCCESS;
		}
	}

	return CB_ERR_ARG;
}

static void configure_xhci_sci(void *unused)
{
	gpe_configure_sci(xhci_sci_sources, ARRAY_SIZE(xhci_sci_sources) - 1);
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_ENTRY, configure_xhci_sci, NULL);
