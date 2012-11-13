void full_reset(void)
{
	/* Enable power on after power fail... */
	unsigned byte;
	byte = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa4);
	byte &= 0xfe;
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa4, byte);

	outb(0x0e, 0xcf9);
}

static void power_down_reset_check(void)
{
	uint8_t cmos;

	cmos=cmos_read(RTC_BOOT_BYTE)>>4 ;
	print_debug("Boot byte = ");
	print_debug_hex8(cmos);
	print_debug("\n");

	if((cmos>2)&&(cmos&1))  full_reset();
}
