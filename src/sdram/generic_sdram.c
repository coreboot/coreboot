#include <sdram.h>
#include <printk.h>

void sdram_no_memory(void)
{
	printk_err("No memory!!\n");
	while(1) ;
}

/* Setup SDRAM */
void sdram_initialize(void)
{
	printk_debug("Ram1\n");
	/* Set the registers we can set once to reasonable values */
	sdram_set_registers();

	printk_debug("Ram2\n");
	/* Now setup those things we can auto detect */
	sdram_set_spd_registers();

	printk_debug("Ram3\n");
	/* Now that everything is setup enable the SDRAM.
	 * Some chipsets do the work for use while on others 
	 * we need to it by hand.
	 */
	sdram_enable();

	printk_debug("Ram4\n");
	sdram_first_normal_reference();

	printk_debug("Ram5\n");
	sdram_enable_refresh();
	sdram_special_finishup();

	printk_debug("Ram6\n");
}
