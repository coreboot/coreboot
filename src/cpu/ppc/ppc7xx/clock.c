#include <ppc.h>

static int PLL_multiplier[] = {
	25,	/* 0000 - 2.5x   */
	75,	/* 0001 - 7.5x   */
	70,	/* 0010 - 7x     */
	10,	/* 0011 - bypass */
	20,	/* 0100 - 2x     */
	65,	/* 0101 - 6.5x   */
	100,	/* 0110 - 10x    */
	45,	/* 0111 - 4.5x   */
	30,	/* 1000 - 3x     */
	55,	/* 1001 - 5.5x   */
	40,	/* 1010 - 4x     */
	50,	/* 1011 - 5x     */
	80,	/* 1100 - 8x     */
	60,	/* 1101 - 6x     */
	35,	/* 1110 - 3.5x   */
	0,	/* 1111 - off    */
};

unsigned long 
get_timer_freq(void)
{
	unsigned long clock = CONFIG_SYS_CLK_FREQ * 1000000;
	return clock * PLL_multiplier[ppc_gethid1() >> 28] / 10;
}
