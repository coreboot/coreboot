void sdram_no_memory(void)
{
	print_err("No memory!!\r\n");
	while(1) { 
		hlt(); 
	}
}

/* Setup SDRAM */
void sdram_initialize(const struct mem_controller *ctrl)
{
	print_debug("Ram1\r\n");
	/* Set the registers we can set once to reasonable values */
	sdram_set_registers(ctrl);

	print_debug("Ram2\r\n");
	/* Now setup those things we can auto detect */
	sdram_set_spd_registers(ctrl);

	print_debug("Ram3\r\n");
	/* Now that everything is setup enable the SDRAM.
	 * Some chipsets do the work for use while on others 
	 * we need to it by hand.
	 */
	sdram_enable(ctrl);

	print_debug("Ram4\r\n");
}
