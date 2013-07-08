
/* cpuRegInit */
void cpuRegInit (void)
{
	int msrnum;
	msr_t msr;
	/* The following is only for diagnostics mode; do not use for OLPC */
	if (0) {
		/* Set Diagnostic Mode */
		msrnum = CPU_GLD_MSR_DIAG;
		msr.hi =  0;
		msr.lo = DIAG_SEL1_SET | DIAG_SET0_SET;
		wrmsr(msrnum, msr);

		/* Set up GLCP to grab BTM data. */
		msrnum = GLCP_DBGOUT;		/* GLCP_DBGOUT MSR */
		msr.hi =  0x0;
		msr.lo =  0x08;			/* reset value (SCOPE_SEL = 0) causes FIFO to shift out, */
		wrmsr(msrnum, msr);		/* exchange it to anything else to prevent this */

		/* Turn off debug clock */
		msrnum = GLCP_DBGCLKCTL;	/* DBG_CLK_CTL */
		msr.lo =  0x00;			/* No clock */
		msr.hi =  0x00;
		wrmsr(msrnum, msr);

		/* Set debug clock to CPU */
		msrnum = GLCP_DBGCLKCTL;	/* DBG_CLK_CTL */
		msr.lo =  0x01;			/* CPU CLOCK */
		msr.hi =  0x00;
		wrmsr(msrnum, msr);

		/* Set fifo ctl to BTM bits wide */
		msrnum = GLCP_FIFOCTL;		/* FIFO_CTL */
		msr.lo =  0x003880000;		/* Bit [25:24] are size (11=BTM, 10 = 64 bit, 01= 32 bit, 00 = 16bit) */
		wrmsr(msrnum, msr);		/* Bit [23:21] are position (100 = CPU downto0) */
						/* Bit [19] sets it up in slow data mode. */

		/* enable fifo loading - BTM sizing will constrain */
		/* only valid BTM packets to load - this action should always be on */
		msrnum = 0x04C00006F;		/* GLCP ACTION7 - load fifo */
		msr.lo =  0x00000F000;		/* Any nibble all 1's will always trigger */
		msr.hi =  0x000000000;
		wrmsr(msrnum, msr);

		/* start storing diag data in the fifo */
		msrnum = 0x04C00005F;		/* DIAG CTL */
		msr.lo =  0x080000000;		/* enable actions */
		msr.hi =  0x000000000;
		wrmsr(msrnum, msr);

		/* Set up delay on data lines, so that the hold time */
		/* is 1 ns. */
		msrnum = GLCP_PROCSTAT;		/* GLCP IO DELAY CONTROLS */
		msr.lo =  0x082b5ad68;
		msr.hi =  0x080ad6b57;		/* RGB delay = 0x07 */
		wrmsr(msrnum, msr);

		/* Set up DF to output diag information on DF pins. */
		msrnum = DF_GLD_MSR_MASTER_CONF;
		msr.lo =  0x0220;
		msr.hi = 0;
		wrmsr(msrnum, msr);

		msrnum = GLCP_DBGOUT;		/* GLCP_DBGOUT MSR */
		msr.hi =  0x0;
		msr.lo =  0x0;			/* reset value (SCOPE_SEL = 0) causes FIFO to shift out, */
		wrmsr(msrnum, msr);
		/* end of code for BTM */
	}

	/* Enable Suspend on Halt */
	msrnum = CPU_XC_CONFIG;
	msr = rdmsr(msrnum);
	msr.lo |=  XC_CONFIG_SUSP_ON_HLT;
	wrmsr(msrnum, msr);

	/* ENable SUSP and allow TSC to run in Suspend */
	/* to keep speed detection happy */
	msrnum = CPU_BC_CONF_0;
	msr = rdmsr(msrnum);
	msr.lo |=  TSC_SUSP_SET | SUSP_EN_SET;
	wrmsr(msrnum, msr);

	/* Setup throttling to proper mode if it is ever enabled. */
	msrnum = GLCP_TH_OD;
	msr.hi =  0x000000000;
	msr.lo =  0x00000603C;
	wrmsr(msrnum, msr);

/* FooGlue Setup */
	/* Set CS5535/CS5536 mode in FooGlue */
	msrnum = FG_GIO_MSR_SEL;
	msr = rdmsr(msrnum);
	msr.lo &= ~3;
	msr.lo |= 2;		/* IIOC mode CS5535/CS5536 enable. (according to Jordan Crouse the databook is wrong bits 1:0 have to be 2 instead of 1) */
	wrmsr(msrnum, msr);

/* Disable DOT PLL. Graphics init will enable it if needed. */
	msrnum = GLCP_DOTPLL;
	msr = rdmsr(msrnum);
	msr.lo |= DOTPPL_LOWER_PD_SET;
	wrmsr(msrnum, msr);

/* Enable RSDC */
	msrnum = CPU_AC_SMM_CTL;
	msr = rdmsr(msrnum);
	msr.lo |=  0x08;
	wrmsr(msrnum, msr);

/* Enable BTB */
	/* I hate to put this check here but it doesn't really work in cpubug.asm */
	msrnum = GLCP_CHIP_REVID;
	msr = rdmsr(msrnum);
	if (msr.lo >= CPU_REV_2_1){
		msrnum = CPU_PF_BTB_CONF;
		msr = rdmsr(msrnum);
		msr.lo |= BTB_ENABLE_SET | RETURN_STACK_ENABLE_SET;
		wrmsr(msrnum, msr);
	}

/* FPU imprecise exceptions bit */
	{
		msrnum = CPU_FPU_MSR_MODE;
		msr = rdmsr(msrnum);
		msr.lo |= FPU_IE_SET;
		wrmsr(msrnum, msr);
	}
}
