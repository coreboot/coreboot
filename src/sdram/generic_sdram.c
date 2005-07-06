void sdram_no_memory(void)
{
	print_err("No memory!!\r\n");
	while(1) { 
		hlt(); 
	}
}

/* Setup SDRAM */
void sdram_initialize(int controllers, const struct mem_controller *ctrl)
{
	int i;
	/* Set the registers we can set once to reasonable values */
	for(i = 0; i < controllers; i++) {
#if CONFIG_USE_INIT
		printk_debug("Ram1.%02x\r\n",i);
#else
		print_debug("Ram1.");
		print_debug_hex8(i);
		print_debug("\r\n");
#endif
		sdram_set_registers(ctrl + i);
	}

	/* Now setup those things we can auto detect */
	for(i = 0; i < controllers; i++) {
#if CONFIG_USE_INIT
                printk_debug("Ram2.%02x\r\n",i);
#else
		print_debug("Ram2.");
		print_debug_hex8(i);
		print_debug("\r\n");
#endif
		sdram_set_spd_registers(ctrl + i);
	}

	/* Now that everything is setup enable the SDRAM.
	 * Some chipsets do the work for use while on others 
	 * we need to it by hand.
	 */
	print_debug("Ram3\r\n");
	sdram_enable(controllers, ctrl);

	print_debug("Ram4\r\n");
}
