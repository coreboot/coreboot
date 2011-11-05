/* copied and adapted from src/mainboard/dell/s1850/watchdog.c */

#define ICH5_WDBASE 0x800 /* PMBASE */
#define ICH5_GPIOBASE 0x480

static void disable_ich5_watchdog(void)
{
	/* FIXME move me somewhere more appropriate */
	device_t dev;
	unsigned long value, base;
	dev = pci_locate_device(PCI_ID(0x8086, 0x24d0), 0); /* LPC bridge */
	if (dev == PCI_DEV_INVALID) {
		die("Missing ich5?");
	}

	/* Enable I/O space */
	value = pci_read_config16(dev, 0x04); /* PCICMD */
	value |= (1 << 10); /* reserved? */
	pci_write_config16(dev, 0x04, value);

	/* Set and enable acpibase */
	pci_write_config32(dev, 0x40, ICH5_WDBASE | 1); /* PMBASE */
	pci_write_config8(dev, 0x44, 0x10); /* ACPI_CNTL = ACPI_EN */
	base = ICH5_WDBASE + 0x60; /* TCO offset, ich5 datasheet ch9.11 */

	/* Set bit 11 in TCO1_CNT */
	value = inw(base + 0x08);
	value |= 1 << 11; /* enable TCO_TMR_HLT */
	outw(value, base + 0x08);

	/* Clear TCO timeout status */
	outw(0x0008, base + 0x04); /* TCO1_STS, raise bit 3: TIMEOUT */
	outw(0x0002, base + 0x06); /* TCO2_STS, raise bit 2: SECOND_TO_STS */

	printk(BIOS_DEBUG, "Board-specific ICH5 watchdog disabled\n");
}
