
static void power_down_reset_check(void)
{
#if 0
	uint8_t cmos;

	/* FIXME */
	cmos=cmos_read(RTC_BOOT_BYTE)>>4 ;
	print_debug("Boot byte = ");
	print_debug_hex8(cmos);
	print_debug("\n");

	if((cmos>2)&&(cmos&1))  full_reset();
#endif
}
