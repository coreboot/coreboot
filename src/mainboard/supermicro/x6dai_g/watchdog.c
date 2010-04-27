#include <device/pnp_def.h>

#define NSC_WD_DEV PNP_DEV(0x2e, 0xa)
#define NSC_WDBASE 0x600
#define ICH5_WDBASE 0x400
#define ICH5_GPIOBASE 0x500

static void disable_esb6300_watchdog(void)
{
	/* FIXME move me somewhere more appropriate */
	device_t dev;
	unsigned long value, base;
	dev = pci_locate_device(PCI_ID(0x8086, 0x25a1), 0);
	if (dev == PCI_DEV_INVALID) {
		die("Missing 6300ESB?");
	}
	/* Enable I/O space */
	value = pci_read_config16(dev, 0x04);
	value |= (1 << 10);
	pci_write_config16(dev, 0x04, value);

	/* Set and enable acpibase */
	pci_write_config32(dev, 0x40, ICH5_WDBASE | 1);
	pci_write_config8(dev, 0x44, 0x10);
	base = ICH5_WDBASE + 0x60;

	/* Set bit 11 in TCO1_CNT */
	value = inw(base + 0x08);
	value |= 1 << 11;
	outw(value, base + 0x08);

	/* Clear TCO timeout status */
	outw(0x0008, base + 0x04);
	outw(0x0002, base + 0x06);
}

static void disable_watchdogs(void)
{
	disable_esb6300_watchdog();
	print_debug("Watchdogs disabled\n");
}

