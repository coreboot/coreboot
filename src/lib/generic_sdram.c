#include <lib.h> /* Prototypes */

/* Setup SDRAM */
#if CONFIG_RAMINIT_SYSINFO
void sdram_initialize(int controllers, const struct mem_controller *ctrl, void *sysinfo)
#else
void sdram_initialize(int controllers, const struct mem_controller *ctrl)
#endif
{
	int i;
	/* Set the registers we can set once to reasonable values */
	for (i = 0; i < controllers; i++) {
		printk(BIOS_DEBUG, "Ram1.%02x\n", i);

	#if CONFIG_RAMINIT_SYSINFO
		sdram_set_registers(ctrl + i, sysinfo);
	#else
		sdram_set_registers(ctrl + i);
	#endif
	}

	/* Now setup those things we can auto detect */
	for (i = 0; i < controllers; i++) {
		printk(BIOS_DEBUG, "Ram2.%02x\n", i);

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
	printk(BIOS_DEBUG, "Ram3\n");

	#if CONFIG_RAMINIT_SYSINFO
	sdram_enable(controllers, ctrl, sysinfo);
	#else
	sdram_enable(controllers, ctrl);
	#endif

	printk(BIOS_DEBUG, "Ram4\n");
}
