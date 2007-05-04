/*
*
* Copyright (C) 2007 Advanced Micro Devices
*
*/

static void pll_reset(char manualconf)
{
	msr_t msrGlcpSysRstpll;

        msrGlcpSysRstpll = rdmsr(GLCP_SYS_RSTPLL);
	
	print_debug("_MSR GLCP_SYS_RSTPLL (");
		print_debug_hex32(GLCP_SYS_RSTPLL);
		print_debug(") value is: ");
		print_debug_hex32(msrGlcpSysRstpll.hi);
		print_debug(":");
		print_debug_hex32(msrGlcpSysRstpll.lo);
		print_debug("\r\n");
	POST_CODE(POST_PLL_INIT);
	
	if (!(msrGlcpSysRstpll.lo & (1 << RSTPLL_LOWER_SWFLAGS_SHIFT))){
		print_debug("Configuring PLL\n");
		if(manualconf){
			POST_CODE(POST_PLL_MANUAL);
			/* CPU and GLIU mult/div (GLMC_CLK = GLIU_CLK / 2)  */
		msrGlcpSysRstpll.hi = PLLMSRhi;

			/* Hold Count - how long we will sit in reset */
		msrGlcpSysRstpll.lo = PLLMSRlo;
		}
		else{
			/*automatic configuration (straps)*/
			POST_CODE(POST_PLL_STRAP);
			msrGlcpSysRstpll.lo &= ~(0xFF << RSTPPL_LOWER_HOLD_COUNT_SHIFT);
			msrGlcpSysRstpll.lo |= (0xDE << RSTPPL_LOWER_HOLD_COUNT_SHIFT);
			msrGlcpSysRstpll.lo &= ~(RSTPPL_LOWER_COREBYPASS_SET | RSTPPL_LOWER_MBBYPASS_SET);
			msrGlcpSysRstpll.lo |= RSTPPL_LOWER_COREPD_SET | RSTPPL_LOWER_CLPD_SET;
		}
			/* Use SWFLAGS to remember: "we've already been here"  */
		msrGlcpSysRstpll.lo |= (1 << RSTPLL_LOWER_SWFLAGS_SHIFT);

			/* "reset the chip" value */
		msrGlcpSysRstpll.lo |= RSTPPL_LOWER_CHIP_RESET_SET;
		wrmsr(GLCP_SYS_RSTPLL, msrGlcpSysRstpll);

		/*	You should never get here..... The chip has reset.*/
		print_debug("CONFIGURING PLL FAILURE\n");
		POST_CODE(POST_PLL_RESET_FAIL);
		__asm__ __volatile__("hlt\n");

	}
	print_debug("Done cpuRegInit\n");
	return;
}

static unsigned int CPUSpeed(void){
	unsigned int speed;
	msr_t msr;

	msr = rdmsr(GLCP_SYS_RSTPLL);
	speed = ((((msr.hi >> RSTPLL_UPPER_CPUMULT_SHIFT) & 0x1F)+1)*333)/10;
	if((((((msr.hi >> RSTPLL_UPPER_CPUMULT_SHIFT) & 0x1F)+1)*333)%10) > 5){
		++speed;
	}
	return(speed);
}
static unsigned int GeodeLinkSpeed(void){
	unsigned int speed;
	msr_t msr;

	msr = rdmsr(GLCP_SYS_RSTPLL);
	speed = ((((msr.hi >> RSTPLL_UPPER_GLMULT_SHIFT) & 0x1F)+1)*333)/10;
	if((((((msr.hi >> RSTPLL_UPPER_GLMULT_SHIFT) & 0x1F)+1)*333)%10) > 5){
		++speed;
	}
	return(speed);
}
static unsigned int PCISpeed(void){
	msr_t msr;

	msr = rdmsr(GLCP_SYS_RSTPLL);
	if (msr.hi & (1 << RSTPPL_LOWER_PCISPEED_SHIFT)){
		return(66);
	}
	else{
		return(33);
	}
}

