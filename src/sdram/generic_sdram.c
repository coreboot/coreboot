
#ifndef RAMINIT_SYSINFO
        #define RAMINIT_SYSINFO 0
#endif

static inline void print_debug_sdram_8(const char *strval, uint32_t val)
{
#if CONFIG_USE_INIT
        printk_debug("%s%02x\r\n", strval, val);
#else
        print_debug(strval); print_debug_hex8(val); print_debug("\r\n");
#endif
}

void sdram_no_memory(void)
{
	print_err("No memory!!\r\n");
	while(1) { 
		hlt(); 
	}
}

/* Setup SDRAM */
#if RAMINIT_SYSINFO == 1
void sdram_initialize(int controllers, const struct mem_controller *ctrl, void *sysinfo)
#else
void sdram_initialize(int controllers, const struct mem_controller *ctrl)
#endif
{
	int i;
	/* Set the registers we can set once to reasonable values */
	for(i = 0; i < controllers; i++) {
		print_debug_sdram_8("Ram1.",i);

	#if RAMINIT_SYSINFO == 1
		sdram_set_registers(ctrl + i , sysinfo);
	#else
		sdram_set_registers(ctrl + i);
	#endif
	}

	/* Now setup those things we can auto detect */
	for(i = 0; i < controllers; i++) {
                print_debug_sdram_8("Ram2.",i);

	#if RAMINIT_SYSINFO == 1
		sdram_set_spd_registers(ctrl + i , sysinfo);
	#else
                sdram_set_spd_registers(ctrl + i);
	#endif

	}

	/* Now that everything is setup enable the SDRAM.
	 * Some chipsets do the work for use while on others 
	 * we need to it by hand.
	 */
	print_debug("Ram3\r\n");

	#if RAMINIT_SYSINFO == 1
	sdram_enable(controllers, ctrl, sysinfo);
	#else
	sdram_enable(controllers, ctrl);
	#endif

	print_debug("Ram4\r\n");
}
