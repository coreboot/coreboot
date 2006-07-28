#define POST_CODE(x) outb(0x80, x)

static void pll_reset(void)
{
	msr_t msrGlcpSysRstpll;

        msrGlcpSysRstpll = rdmsr(GLCP_SYS_RSTPLL);
	
        print_debug("MSR GLCP_SYS_RSTPLL (");
		print_debug_hex32(GLCP_SYS_RSTPLL);
		print_debug(") value is: ");
		print_debug_hex32(msrGlcpSysRstpll.hi);
		print_debug(":");
		print_debug_hex32(msrGlcpSysRstpll.lo);
		print_debug("\n");
	
        msrGlcpSysRstpll.lo &= 0x80000000;

	// If the "we've already been here" flag is set, don't reconfigure the pll
        if ( !(msrGlcpSysRstpll.lo) )
	{ // we haven't configured the PLL; do it now
                POST_CODE(0x77);
	
               /*
                *    64 - 32 	    |  31-0 
                * 
                *      (03FB)
                * 0000 0011 1111 1011 | 1000 0000 1101 1110 0000 0000 1000 0001
                * 
                *      (039C)
                * 0000 0011 1001 1100 | 1000 0000 1101 1110 0000 0000 1000 0001
                * 
                *       (029C)
                * 0000 0010 1001 1100 | 1000 0000 1101 1110 0000 0000 1000 0001
                * 
                *       (02CB)
                * 0000 0010 1100 1011 | 1000 0000 1101 1110 0000 0000 1000 0001
                * 
                * 00101		1			00101 		1 		| 100000	0		0		11011110 	0000 0000 1000 0001
                * GLIUMULT	GLIUDIV		COREMULT	COREDIV	| SWFLAGS	(RO)	(RO)	HOLD_COUNT	
                */

                /* ### 02CB  ###
                 * GLIUMULT = 6
                 * GLIUDIV = 2
                 * COREMULT = 6
                 * COREDIV = 2
                 * 
                 * ### 03FB  ###
                 * GLIUMULT = 8
                 * GLIUDIV = 2
                 * COREMULT = 30
                 * COREDIV = 2
                 * 
                 * ### 039C  ###  bad... why?
                 * GLIUMULT = 8
                 * GLIUDIV = 0
                 * COREMULT = 15
                 * COREDIV = 0
                 * 
                 * ### 029C  ###  good...
                 * GLIUMULT = 6
                 * GLIUDIV = 0
                 * COREMULT = 15
                 * COREDIV = 0
                 * 
                 *  CLOCK = 33 MHz
                 *
                 */

			/* CPU and GLIU mult/div (GLMC_CLK = GLIU_CLK / 2)  */
			msrGlcpSysRstpll.hi = 0x0000029C;

			/* Hold Count - how long we will sit in reset */
			msrGlcpSysRstpll.lo = 0x00DE0000;

			/* Use SWFLAGS to remember: "we've already been here"  */
			msrGlcpSysRstpll.lo |= 0x80000000;

			/* "reset the chip" value */
			msrGlcpSysRstpll.lo |= 0x00000001;

		wrmsr(GLCP_SYS_RSTPLL, msrGlcpSysRstpll);
	}
}
