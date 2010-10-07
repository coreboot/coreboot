#include <lib.h> /* Prototypes */

static inline void print_debug_sdram_8(const char *strval, uint32_t val)
{
#if CONFIG_CACHE_AS_RAM
        printk(BIOS_DEBUG, "%s%02x\n", strval, val);
#else
        print_debug(strval); print_debug_hex8(val); print_debug("\n");
#endif
}

/* Setup SDRAM */
#if CONFIG_RAMINIT_SYSINFO
void sdram_initialize(int controllers, const struct mem_controller *ctrl, void *sysinfo)
#else
void sdram_initialize(int controllers, const struct mem_controller *ctrl)
#endif
{
	int i;
	/* Set the registers we can set once to reasonable values */
	for(i = 0; i < controllers; i++) {
		print_debug_sdram_8("Ram1.", i);

	#if CONFIG_RAMINIT_SYSINFO
		sdram_set_registers(ctrl + i, sysinfo);
	#else
		sdram_set_registers(ctrl + i);
	#endif
	}

	/* Now setup those things we can auto detect */
	for(i = 0; i < controllers; i++) {
                print_debug_sdram_8("Ram2.", i);

	#if CONFIG_RAMINIT_SYSINFO
		sdram_set_spd_registers(ctrl + i, sysinfo);
	#else
                sdram_set_spd_registers(ctrl + i);
	#endif

	}

	/* Now that everything is setup enable the SDRAM.
	 * Some chipsets do the work for us while on others
	 * we need to it by hand.
	 */
	print_debug("Ram3\n");

	#if CONFIG_RAMINIT_SYSINFO
	sdram_enable(controllers, ctrl, sysinfo);
	#else
	sdram_enable(controllers, ctrl);
	#endif

	print_debug("Ram4\n");
}
