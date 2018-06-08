/*
 * (C) 2003 Linux Networx, SuSE Linux AG
 *  2006.1 yhlu add dest apicid for IRQ0
 */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <cpu/x86/lapic.h>
#include <arch/ioapic.h>
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <cpu/amd/powernow.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "amd8111.h"

#define NMI_OFF 0

static void enable_hpet(struct device *dev)
{
	unsigned long hpet_address;

	pci_write_config32(dev, 0xa0, CONFIG_HPET_ADDRESS|1);
	hpet_address = pci_read_config32(dev,0xa0)& 0xfffffffe;
	printk(BIOS_DEBUG, "enabling HPET @0x%lx\n", hpet_address);

}

static void lpc_init(struct device *dev)
{
	uint8_t byte;
	int nmi_option;

	/* IO APIC initialization */
	byte = pci_read_config8(dev, 0x4B);
	byte |= 1;
	pci_write_config8(dev, 0x4B, byte);
	/* Don't rename IO APIC */
	setup_ioapic(VIO_APIC_VADDR, 0);

	/* posted memory write enable */
	byte = pci_read_config8(dev, 0x46);
	pci_write_config8(dev, 0x46, byte | (1<<0));

	/* Enable 5Mib Rom window */
	byte = pci_read_config8(dev, 0x43);
	byte |= 0xc0;
	pci_write_config8(dev, 0x43, byte);

	/* Enable Port 92 fast reset */
	byte = pci_read_config8(dev, 0x41);
	byte |= (1 << 5);
	pci_write_config8(dev, 0x41, byte);

	/* Enable Error reporting */
	/* Set up sync flood detected */
	byte = pci_read_config8(dev, 0x47);
	byte |= (1 << 1);
	pci_write_config8(dev, 0x47, byte);

	/* Set up NMI on errors */
	byte = pci_read_config8(dev, 0x40);
	byte |= (1 << 1); /* clear PW2LPC error */
	byte |= (1 << 6); /* clear LPCERR */
	pci_write_config8(dev, 0x40, byte);
	nmi_option = NMI_OFF;
	get_option(&nmi_option, "nmi");
	if (nmi_option) {
		byte |= (1 << 7); /* set NMI */
		pci_write_config8(dev, 0x40, byte);
	}

	/* Initialize the real time clock */
	cmos_init(0);

	/* Initialize isa dma */
	isa_dma_init();

	/* Initialize the High Precision Event Timers */
	enable_hpet(dev);
}

static void amd8111_lpc_read_resources(struct device *dev)
{
	struct resource *res;

	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->base = 0xff800000;
	res->size = 0x00800000; /* 8 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void lpci_set_subsystem(struct device *dev, unsigned vendor,
			       unsigned device)
{
	pci_write_config32(dev, 0x70,
			   ((device & 0xffff) << 16) | (vendor & 0xffff));
}

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)

extern u16 pm_base;

unsigned long acpi_fill_mcfg(unsigned long current)
{
	/* Just a dummy */
	return current;
}

static void southbridge_acpi_fill_ssdt_generator(struct device *device) {
#if IS_ENABLED(CONFIG_SET_FIDVID)
	amd_generate_powernow(pm_base + 0x10, 6, 1);
	acpigen_write_mainboard_resources("\\_SB.PCI0.MBRS", "_CRS");
#endif
}

#endif


static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

static struct device_operations lpc_ops  = {
	.read_resources   = amd8111_lpc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = lpc_init,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.write_acpi_tables      = acpi_write_hpet,
	.acpi_fill_ssdt_generator = southbridge_acpi_fill_ssdt_generator,
#endif
	.scan_bus         = scan_lpc_bus,
	.enable           = amd8111_enable,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops    = &lpc_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_8111_ISA,
};
