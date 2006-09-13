#define POST_CODE(x) outb(0x80, x)

static void pll_reset(void)
{
	msr_t msrGlcpSysRstpll;

        msrGlcpSysRstpll = rdmsr(GLCP_SYS_RSTPLL);
	
	print_debug("_MSR GLCP_SYS_RSTPLL (");
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
	    print_debug("CONFIGURING PLL");
	
                POST_CODE(0x77);
	
		// HARDCODED VALUES MOVED BACK TO auto.c AS THEY HAVE TO BE BOARD-SPECIFIC
		// (this file is included from there)

			/* CPU and GLIU mult/div (GLMC_CLK = GLIU_CLK / 2)  */
		msrGlcpSysRstpll.hi = PLLMSRhi;

			/* Hold Count - how long we will sit in reset */
		msrGlcpSysRstpll.lo = PLLMSRlo;

			/* Use SWFLAGS to remember: "we've already been here"  */
			msrGlcpSysRstpll.lo |= 0x80000000;

			/* "reset the chip" value */
			msrGlcpSysRstpll.lo |= 0x00000001;

		wrmsr(GLCP_SYS_RSTPLL, msrGlcpSysRstpll);
	}
}
