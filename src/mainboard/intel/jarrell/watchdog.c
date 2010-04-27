#include <device/pnp_def.h>

#define NSC_WD_DEV PNP_DEV(0x2e, 0xa)
#define NSC_WDBASE 0x600
#define ICH5_WDBASE 0x400
#define ICH5_GPIOBASE 0x500

static void disable_sio_watchdog(device_t dev)
{
	/* FIXME move me somewhere more appropriate */
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1);
	pnp_set_iobase(dev, PNP_IDX_IO0, NSC_WDBASE);
	/* disable the sio watchdog */
	outb(0, NSC_WDBASE + 0);
	pnp_set_enable(dev, 0);
}

static void disable_ich5_watchdog(void)
{
	/* FIXME move me somewhere more appropriate */
	device_t dev;
	unsigned long value, base;
	dev = pci_locate_device(PCI_ID(0x8086, 0x24d0), 0);
	if (dev == PCI_DEV_INVALID) {
		die("Missing ich5?");
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

static void disable_jarell_frb3(void)
{
	device_t dev;
	unsigned long value, base;
	dev = pci_locate_device(PCI_ID(0x8086, 0x24d0), 0);
	if (dev == PCI_DEV_INVALID) {
		die("Missing ich5?");
	}
	/* Enable I/O space */
	value = pci_read_config16(dev, 0x04);
	value |= (1 << 0);
	pci_write_config16(dev, 0x04, value);

	/* Set gpio base */
	pci_write_config32(dev, 0x58, ICH5_GPIOBASE | 1);
	base = ICH5_GPIOBASE;

	/* Enable GPIO Bar */
	value = pci_read_config32(dev, 0x5c);
	value |= 0x10;
	pci_write_config32(dev, 0x5c, value);

	/* Configure GPIO 48 and 40 as GPIO */
	value = inl(base + 0x30);
	value |= (1 << 16) | ( 1 << 8);
	outl(value, base + 0x30);

	/* Configure GPIO 48 as Output */
	value = inl(base + 0x34);
	value &= ~(1 << 16);
	outl(value, base + 0x34);

	/* Toggle GPIO 48 high to low */
	value = inl(base + 0x38);
	value |= (1 << 16);
	outl(value, base + 0x38);
	value &= ~(1 << 16);
	outl(value, base + 0x38);

}

static void disable_watchdogs(void)
{
	disable_sio_watchdog(NSC_WD_DEV);
	disable_ich5_watchdog();
	disable_jarell_frb3();
	print_debug("Watchdogs disabled\n");
}

static void ich5_watchdog_on(void)
{
	device_t dev;
	unsigned long value, base;
	unsigned char byte;

	/* check cmos options */
	byte = cmos_read(RTC_BOOT_BYTE-1);
	if(!(byte & 1)) return; /* no boot watchdog */
	byte = cmos_read(RTC_BOOT_BYTE);
	if(!(byte & 2)) return; /* fallback so ignore */

	dev = pci_locate_device(PCI_ID(0x8086, 0x24d0), 0);
	if (dev == PCI_DEV_INVALID) {
		die("Missing ich5?");
	}
	/* Enable I/O space */
	value = pci_read_config16(dev, 0x04);
	value |= (1 << 10);
	pci_write_config16(dev, 0x04, value);

	/* Set and enable acpibase */
	pci_write_config32(dev, 0x40, ICH5_WDBASE | 1);
	pci_write_config8(dev, 0x44, 0x10);
	base = ICH5_WDBASE + 0x60;

	/* Clear TCO timeout status */
	outw(0x0008, base + 0x04);
	outw(0x0002, base + 0x06);

	/* set the time value 1 cnt = .6 sec */
	outw(0x0010, base + 0x01);
	/* reload the timer with the value */
	outw(0x0001, base + 0x00);

	/* clear bit 11 in TCO1_CNT to start watchdog */
	value = inw(base + 0x08);
	value &= ~(1 << 11);
	outw(value, base + 0x08);

	print_debug("Watchdog ICH5 enabled\n");
}
