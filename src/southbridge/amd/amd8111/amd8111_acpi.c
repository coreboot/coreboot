#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <bitops.h>
#include <arch/io.h>
#include "amd8111.h"

#define PREVIOUS_POWER_STATE 0x43
#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON 1
#define SLOW_CPU_OFF 0
#define SLOW_CPU__ON 1

#ifndef MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif


static void acpi_init(struct device *dev)
{
	uint8_t byte;
	uint16_t word;
	uint16_t pm10_bar;
	uint32_t dword;
	int on;

#if 0
	printk_debug("ACPI: disabling NMI watchdog.. ");
	pci_read_config_byte(dev, 0x49, &byte);
	pci_write_config_byte(dev, 0x49, byte | (1<<2));


	pci_read_config_byte(dev, 0x41, &byte);
	pci_write_config_byte(dev, 0x41, byte | (1<<6)|(1<<2));

	/* added from sourceforge */
	pci_read_config_byte(dev, 0x48, &byte);
	pci_write_config_byte(dev, 0x48, byte | (1<<3));

	printk_debug("done.\n");


	printk_debug("ACPI: Routing IRQ 12 to PS2 port.. ");
	pci_read_config_word(dev, 0x46, &word);
	pci_write_config_word(dev, 0x46, word | (1<<9));
	printk_debug("done.\n");

	
	printk_debug("ACPI: setting PM class code.. ");
	pci_write_config_dword(dev, 0x60, 0x06800000);
	printk_debug("done.\n");
#endif
	on = MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
	get_option(&on, "power_on_after_fail");
	byte = pci_read_config8(dev, PREVIOUS_POWER_STATE);
	byte &= ~0x40;
	if (!on) {
		byte |= 0x40;
	}
	pci_write_config8(dev, PREVIOUS_POWER_STATE, byte);
	printk_info("set power %s after power fail\n", on?"on":"off");

	/* Throttle the CPU speed down for testing */
	on = SLOW_CPU_OFF;
	get_option(&on, "slow_cpu");
	if(on) {
		pm10_bar = (pci_read_config16(dev, 0x58)&0xff00);
		outl(((on<<1)+0x10)  ,(pm10_bar + 0x10));
		dword = inl(pm10_bar + 0x10);
		on = 8-on;
		printk_debug("Throttling CPU %2d.%1.1d percent.\n",
				(on*12)+(on>>1),(on&1)*5);
	}
}

static void acpi_read_resources(device_t dev)
{
	/* Handle the generic bars */
	pci_dev_read_resources(dev);

	if ((dev->resources + 1) < MAX_RESOURCES) {
		struct resource *resource = &dev->resource[dev->resources];
		dev->resources++;
		resource->base  = 0;
		resource->size  = 256;
		resource->align = log2(256);
		resource->gran  = log2(256);
		resource->limit = 65536;
		resource->flags = IORESOURCE_IO;
		resource->index = 0x58;
	}
	else {
		printk_err("%s Unexpected resource shortage\n",
			dev_path(dev));
	}
}

static struct device_operations acpi_ops  = {
	.read_resources   = acpi_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = acpi_init,
	.scan_bus         = 0,
//	.enable           = amd8111_enable,
};

static struct pci_driver acpi_driver __pci_driver = {
	.ops    = &acpi_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_8111_ACPI,
};

