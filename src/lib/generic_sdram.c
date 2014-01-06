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
void sdram_initialize(int controllers, const struct mem_controller *ctrl)
{
	int i;
	/* Set the registers we can set once to reasonable values */
	for(i = 0; i < controllers; i++) {
		print_debug_sdram_8("Ram1.", i);
		sdram_set_registers(ctrl + i);
	}

	/* Now setup those things we can auto detect */
	for(i = 0; i < controllers; i++) {
                print_debug_sdram_8("Ram2.", i);
                sdram_set_spd_registers(ctrl + i);
	}

	/* Now that everything is setup enable the SDRAM.
	 * Some chipsets do the work for us while on others
	 * we need to it by hand.
	 */
	print_debug("Ram3\n");
	sdram_enable(controllers, ctrl);
	print_debug("Ram4\n");
}
