#include <sdram.h>
#include <delay.h>
#include <printk.h>

void sdram_enable(void)
{
	printk_debug("Ram Enable 1\n");

	/* noop command */
	sdram_set_command_noop();
	udelay(200);
	sdram_assert_command();

	/* Precharge all */
	sdram_set_command_precharge();
	sdram_assert_command();

	/* wait until the all banks idle state... */
	printk_debug("Ram Enable 2\n");
	
	/* Now we need 8 AUTO REFRESH / CBR cycles to be performed */
	
	sdram_set_command_cbr();
	sdram_assert_command();
	sdram_assert_command();
	sdram_assert_command();
	sdram_assert_command();
	sdram_assert_command();
	sdram_assert_command();
	sdram_assert_command();
	sdram_assert_command();
	
	printk_debug("Ram Enable 3\n");
	
	/* mode register set */
	sdram_set_mode_register();
	/* MAx[14:0] lines,
	 * MAx[2:0 ] 010 == burst mode of 4
	 * MAx[3:3 ] 1 == interleave wrap type
	 * MAx[4:4 ] == CAS# latency bit
	 * MAx[6:5 ] == 01
	 * MAx[12:7] ==	0
	 */

	printk_debug("Ram Enable 4\n");

	/* normal operation */
	sdram_set_command_none();
	
	printk_debug("Ram Enable 5\n");
}
