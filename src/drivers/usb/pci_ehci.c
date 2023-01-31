/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_ehci.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/pci.h>
#include <string.h>

#include "ehci_debug.h"
#include "ehci.h"

#if ENV_RAMSTAGE
static struct device_operations *ehci_drv_ops;
static struct device_operations ehci_dbg_ops;
#endif

int ehci_debug_hw_enable(unsigned int *base, unsigned int *dbg_offset)
{
	pci_devfn_t dev = pci_ehci_dbg_dev(CONFIG_USBDEBUG_HCD_INDEX);

	/* We only support controllers on bus 0. */
	if (PCI_DEV2SEGBUS(dev) != 0)
		return -1;

	u32 class = pci_s_read_config32(dev, PCI_CLASS_REVISION) >> 8;
	if (class != PCI_EHCI_CLASSCODE)
		return -1;

	u8 pm_cap = pci_s_find_capability(dev, PCI_CAP_ID_PM);
	if (pm_cap) {
		u16 pm_ctrl = pci_s_read_config16(dev, pm_cap + PCI_PM_CTRL);
		/* Set to D0 and disable PM events. */
		pm_ctrl &= ~PCI_PM_CTRL_PME_ENABLE;
		pm_ctrl &= ~PCI_PM_CTRL_STATE_MASK;
		pci_s_write_config16(dev, pm_cap + PCI_PM_CTRL, pm_ctrl);
	}

	u8 pos = pci_s_find_capability(dev, PCI_CAP_ID_EHCI_DEBUG);
	if (!pos)
		return -1;

	u32 cap = pci_s_read_config32(dev, pos);

	/* FIXME: We should remove static EHCI_BAR_INDEX. */
	u8 ehci_bar = 0x10 + 4 * ((cap >> 29) - 1);
	if (ehci_bar != EHCI_BAR_INDEX)
		return -1;

	pci_s_write_config32(dev, ehci_bar, CONFIG_EHCI_BAR);

	pci_s_write_config8(dev, PCI_COMMAND, PCI_COMMAND_MEMORY |
		PCI_COMMAND_MASTER);

	*base = CONFIG_EHCI_BAR;
	*dbg_offset = (cap>>16) & 0x1ffc;

	return 0;
}

void ehci_debug_select_port(unsigned int port)
{
	pci_devfn_t dbg_dev = pci_ehci_dbg_dev(CONFIG_USBDEBUG_HCD_INDEX);
	pci_ehci_dbg_set_port(dbg_dev, port);
}

#if ENV_RAMSTAGE
static void pci_ehci_set_resources(struct device *dev)
{
	struct resource *res;

	printk(BIOS_DEBUG, "%s EHCI Debug Port hook triggered\n", dev_path(dev));
	usbdebug_disable();

	if (ehci_drv_ops->set_resources)
		ehci_drv_ops->set_resources(dev);
	res = probe_resource(dev, EHCI_BAR_INDEX);
	if (!res)
		return;

	usbdebug_re_enable((u32)res->base);
	report_resource_stored(dev, res, "");
	printk(BIOS_DEBUG, "%s EHCI Debug Port relocated\n", dev_path(dev));
}

void pci_ehci_read_resources(struct device *dev)
{
	pci_devfn_t dbg_dev = pci_ehci_dbg_dev(CONFIG_USBDEBUG_HCD_INDEX);

	if (!ehci_drv_ops && pci_match_simple_dev(dev, dbg_dev)) {
		memcpy(&ehci_dbg_ops, dev->ops, sizeof(ehci_dbg_ops));
		ehci_drv_ops = dev->ops;
		ehci_dbg_ops.set_resources = pci_ehci_set_resources;
		dev->ops = &ehci_dbg_ops;
		printk(BIOS_DEBUG, "%s EHCI BAR hook registered\n", dev_path(dev));
	} else {
		printk(BIOS_DEBUG, "More than one caller of %s from %s\n", __func__, dev_path(dev));
	}

	pci_dev_read_resources(dev);
}
#endif

u8 *pci_ehci_base_regs(pci_devfn_t sdev)
{
	u32 bar = pci_s_read_config32(sdev, EHCI_BAR_INDEX) & ~0x0f;
	u8 *base = (u8 *)(uintptr_t)bar;
	return base + HC_LENGTH(read32(base));
}
