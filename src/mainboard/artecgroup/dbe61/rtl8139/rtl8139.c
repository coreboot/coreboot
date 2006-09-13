#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#undef __KERNEL__
#include <arch/io.h>
#include <string.h>
#include "chip.h"

void pci_level_irq(unsigned char intNum);

// initialize Realtek NIC

static void nic_initialize(device_t dev)
{
	struct drivers_pci_rtl8139_config *cfg = (struct drivers_pci_rtl8139_config*)dev->chip_info;
	uint16_t pciCmd = 0;

	printk_debug("RTL8139 initialize\n");

	dev->on_mainboard=1;
	pci_dev_init(dev);

	//  Set PCI IRQ descriptors. 
	//  This configures nothing (no magic done in VSA, too), only descriptors are set, 
	//  that are later read by operating system to find out which irq is used by dev.
	//  The real GPIO to IRQ mapping configuration takes place in cs5536.c
	//  and is configurable in Config.lb. 

	printk_debug("Setting NIC IRQ to %d\n", cfg->nic_irq);
	pci_write_config8(dev, PCI_INTERRUPT_LINE, cfg->nic_irq);
	pci_level_irq(cfg->nic_irq);
		
	// RTL8139 must have bus mastering for some data transfers
	pciCmd = pci_read_config16(dev, PCI_COMMAND);
	pciCmd |= (PCI_COMMAND_MASTER | PCI_COMMAND_IO);
	pci_write_config16(dev, PCI_COMMAND, pciCmd);
}

// device operations : on PCI device init, call nic_initialize

static struct device_operations drivers_pci_rtl8139_dev_ops = 
{
        .init             = nic_initialize,
        .read_resources   = pci_dev_read_resources,
        .set_resources    = pci_dev_set_resources,
		.enable_resources = pci_dev_enable_resources,        
		.scan_bus         = 0,
};


/* FIRST LOOP : traversing static.c list and enabling all chips found in there
 * set given device operations descriptor to call nic_initialize 
 * */

void nic_configure_pci(device_t dev)
{
	dev->ops = &drivers_pci_rtl8139_dev_ops;
}

// PCI NIC operations
struct chip_operations drivers_pci_rtl8139_ops = {
	.enable_dev	= nic_configure_pci,
};
