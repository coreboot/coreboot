void sdram_no_memory(void)
{
	print_err("No memory!!\r\n");
	while(1) { 
		hlt(); 
	}
}

/* Setup SDRAM */
void sdram_initialize(void)
{
	print_debug("Ram1\r\n");
	/* Set the registers we can set once to reasonable values */
	sdram_set_registers();

	print_debug("Ram2\r\n");
	/* Now setup those things we can auto detect */
	sdram_set_spd_registers();

	print_debug("Ram3\r\n");
	/* Now that everything is setup enable the SDRAM.
	 * Some chipsets do the work for use while on others 
	 * we need to it by hand.
	 */
	sdram_enable();

	print_debug("Ram4\r\n");
	sdram_first_normal_reference();

	print_debug("Ram5\r\n");
	sdram_enable_refresh();
	sdram_special_finishup();

	print_debug("Ram6\r\n");
}
