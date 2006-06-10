

/* ***************************************************************************/
/* **/
/* *	BIST */
/* **/
/* *	GX2 BISTs need to be run before BTB or caches are enabled.*/
/* *	BIST result left in registers on failure to be checked with FS2.*/
/* **/
/* ***************************************************************************/
static void
BIST(void){
	int msrnum;
	msr_t msr;

	/* DM*/
	msrnum = CPU_DM_CONFIG0;
	msr = rdmsr(msrnum);
	msr.lo |=  DM_CONFIG0_LOWER_DCDIS_SET;
	wrmsr(msrnum, msr);
	
	msr.lo =  0x00000003F;
	msr.hi =  0x000000000;
	msrnum = CPU_DM_BIST;
	wrmsr(msrnum, msr);

	outb(POST_CPU_DM_BIST_FAILURE, 0x80);				/* 0x29*/
	msr = rdmsr(msrnum);						/* read back for pass fail*/
	msr.lo &= 0x0F3FF0000;
	if (msr.lo != 0xfeff0000)
		goto BISTFail;
 
	msrnum = CPU_DM_CONFIG0;
	msr = rdmsr(msrnum);
	msr.lo &=  ~ DM_CONFIG0_LOWER_DCDIS_SET;
	wrmsr(msrnum, msr);

	/* FPU*/
	msr.lo =  0x000000131;
	msr.hi = 0;
	msrnum = CPU_FP_UROM_BIST;
	wrmsr(msrnum, msr);

	outb(POST_CPU_FPU_BIST_FAILURE, 0x80);				/* 0x89*/
	inb(0x80);									/*  IO delay*/
	msr = rdmsr(msrnum);							/* read back for pass fail*/
	while ((msr.lo&0x884) != 0x884)
		msr = rdmsr(msrnum);					/*  Endless loop if BIST is broken*/
	if ((msr.lo&0x642) != 0x642)
		goto BISTFail;

	msr.lo = msr.hi = 0;				/*  clear FPU BIST bits*/
	msrnum = CPU_FP_UROM_BIST;
	wrmsr(msrnum, msr);


	/* BTB*/
	msr.lo =  0x000000303;
	msr.hi =  0x000000000;
	msrnum = CPU_PF_BTBRMA_BIST;
	wrmsr(msrnum, msr);

	outb(POST_CPU_BTB_BIST_FAILURE	, 0x80);				/* 0x8A*/
	msr = rdmsr(msrnum);							/* read back for pass fail*/
	if ((msr.lo & 0x3030) != 0x3030)
		goto BISTFail;

	return;

BISTFail:
	print_err("BIST failed!\n");
	while(1);
}
/* ***************************************************************************/
/* *	cpuRegInit*/
/* ***************************************************************************/
void
cpuRegInit (void){
	int msrnum;
	msr_t msr;
	/*  Turn on BTM for early debug based on setup. */
	/*if (getnvram( TOKEN_BTM_DIAG_MODE) & 3) {*/
	/*
	 * The following is only for diagnostics mode; do not use for OLPC
	 */
	if (0) {
		/*  Set Diagnostic Mode */
		msrnum = CPU_GLD_MSR_DIAG;
		msr.hi =  0;
		msr.lo = DIAG_SEL1_SET | DIAG_SET0_SET;
		wrmsr(msrnum, msr);
	
		/*  Set up GLCP to grab BTM data.*/
		msrnum = 0x04C00000C;		/*  GLCP_DBGOUT MSR*/
		msr.hi =  0x0;
		msr.lo =  0x08;			/*  reset value (SCOPE_SEL = 0) causes FIFO toshift out,*/
		wrmsr(msrnum, msr);		/*  exchange it to anything else to prevent this*/
	
		/* ;Turn off debug clock*/
		msrnum = 0x04C000016;		/* DBG_CLK_CTL*/
		msr.lo =  0x00;			/* No clock*/
		msr.hi =  0x00;
		wrmsr(msrnum, msr);
	
		/* ;Set debug clock to CPU*/
		msrnum = 0x04C000016;		/* DBG_CLK_CTL*/
		msr.lo =  0x01;			/* CPU CLOCK*/
		msr.hi =  0x00;
		wrmsr(msrnum, msr);
	
		/* ;Set fifo ctl to BTM bits wide*/
		msrnum = 0x04C00005E;		/*  FIFO_CTL*/
		msr.lo =  0x003880000;		/*  Bit [25:24] are size (11=BTM, 10 = 64 bit, 01= 32 bit, 00 = 16bit)*/
		wrmsr(msrnum, msr);	/*  Bit [23:21] are position (100 = CPU downto0)*/
							/*  Bit [19] sets it up in slow data mode.*/
	
		/* ;enable fifo loading - BTM sizing will constrain*/
		/* ; only valid BTM packets to load - this action should always be on*/
	
		msrnum = 0x04C00006F;		/*  GLCP ACTION7 - load fifo*/
		msr.lo =  0x00000F000;		/*    Any nibble all 1's will always trigger*/
		msr.hi =  0x000000000;		/* */
		wrmsr(msrnum, msr);
	
		/* ;start storing diag data in the fifo*/
		msrnum = 0x04C00005F;		/* DIAG CTL*/
		msr.lo =  0x080000000;		/*  enable actions*/
		msr.hi =  0x000000000;
		wrmsr(msrnum, msr);
	
		/*  Set up delay on data lines, so that the hold time*/
		/*  is 1 ns.*/
		msrnum = 0x04C00000D ;	/*  GLCP IO DELAY CONTROLS*/
		msr.lo =  0x082b5ad68;
		msr.hi =  0x080ad6b57;	/*  RGB delay = 0x07*/
		wrmsr(msrnum, msr);
	
		/*  Set up DF to output diag information on DF pins.*/
		msrnum = DF_GLD_MSR_MASTER_CONF;
		msr.lo =  0x0220;
		msr.hi = 0;
		wrmsr(msrnum, msr);
	
		msrnum = 0x04C00000C ;	/*  GLCP_DBGOUT MSR*/
		msr.hi =  0x0;
		msr.lo =  0x0;				/*  reset value (SCOPE_SEL = 0) causes FIFO to shift out,*/
		wrmsr(msrnum, msr);
		/* end of code for BTM */
	}

	/*  Enable Suspend on Halt*/
	msrnum = CPU_XC_CONFIG;
	msr = rdmsr(msrnum);
	msr.lo |=  XC_CONFIG_SUSP_ON_HLT;
	wrmsr(msrnum, msr);

	/*  ENable SUSP and allow TSC to run in Suspend */
	/*  to keep speed detection happy*/
	msrnum = CPU_BC_CONF_0;
	msr = rdmsr(msrnum);
	msr.lo |=  TSC_SUSP_SET | SUSP_EN_SET;
	wrmsr(msrnum, msr);

	/*  Setup throttling to proper mode if it is ever enabled.*/
	msrnum = 0x04C00001E;
	msr.hi =  0x000000000;
	msr.lo =  0x00000603C;
	wrmsr(msrnum, msr);


/*  Only do this if we are building for 5535*/
/* */
/*  FooGlue Setup*/
/* */
#if 1
	/*  Enable CIS mode B in FooGlue*/
	msrnum = MSR_FG + 0x10;
	msr = rdmsr(msrnum);
	msr.lo &= ~3;
	msr.lo |= 2;			/*  ModeB*/
	wrmsr(msrnum, msr);
#endif

/* */
/*  Disable DOT PLL. Graphics init will enable it if needed.*/
/* */
	msrnum = GLCP_DOTPLL;
	msr = rdmsr(msrnum);
	msr.lo |= DOTPPL_LOWER_PD_SET;
	wrmsr(msrnum, msr);

/* */
/*  Enable RSDC*/
/* */
	msrnum = 0x1301 ;
	msr = rdmsr(msrnum);
	msr.lo |=  0x08;
	wrmsr(msrnum, msr);


/* */
/*  BIST*/
/* */
	/*if (getnvram( TOKEN_BIST_ENABLE) & == TVALUE_DISABLE) {*/
	{
//		BIST();
	}


/* */
/*  Enable BTB*/
/* */
	/*  I hate to put this check here but it doesn't really work in cpubug.asm*/
	msrnum = MSR_GLCP+0x17;
	msr = rdmsr(msrnum);
	if (msr.lo >= CPU_REV_2_1){
		msrnum = CPU_PF_BTB_CONF;
		msr = rdmsr(msrnum);
		msr.lo |= BTB_ENABLE_SET | RETURN_STACK_ENABLE_SET;
		wrmsr(msrnum, msr);
	}

/* */
/*  FPU impercise exceptions bit*/
/* */
	/*if (getnvram( TOKEN_FPU_IE_ENABLE) != TVALUE_DISABLE) {*/
	{
		msrnum = CPU_FPU_MSR_MODE;
		msr = rdmsr(msrnum);
		msr.lo |= FPU_IE_SET;
		wrmsr(msrnum, msr);
	}

/* */
/*  Cache Overides*/
/* */
	/*  Allow NVRam to override DM Setup*/
	/*if (getnvram( TOKEN_CACHE_DM_MODE) != 1) {*/
	{

		msrnum = CPU_DM_CONFIG0;
		msr = rdmsr(msrnum);
		msr.lo |=  DM_CONFIG0_LOWER_DCDIS_SET;
		wrmsr(msrnum, msr);
	}
	/*  Allow NVRam to override IM Setup*/
	/*if (getnvram( TOKEN_CACHE_IM_MODE) ==1) {*/
	{
		msrnum = CPU_IM_CONFIG;
		msr = rdmsr(msrnum);
		msr.lo |=  IM_CONFIG_LOWER_ICD_SET;
		wrmsr(msrnum, msr);
	}
}




/* ***************************************************************************/
/* **/
/* *	MTestPinCheckBX*/
/* **/
/* *	Set MTEST pins to expected values from OPTIONS.INC/NVRAM*/
/* *  This version is called when there isn't a stack available*/
/* **/
/* ***************************************************************************/
static void
MTestPinCheckBX (void){
	int msrnum;
	msr_t msr;

	/*if (getnvram( TOKEN_MTEST_ENABLE) ==TVALUE_DISABLE ) {*/
			/* return ; */
	/* } */

	/*  Turn on MTEST*/
	msrnum = MC_CFCLK_DBUG;
	msr = rdmsr(msrnum);
	msr.hi |=  CFCLK_UPPER_MTST_B2B_DIS_SET | CFCLK_UPPER_MTEST_EN_SET;
	wrmsr(msrnum, msr);

	msrnum = GLCP_SYS_RSTPLL			/*  Get SDR/DDR mode from GLCP*/;
	msr = rdmsr(msrnum);
	msr.lo >>=  RSTPPL_LOWER_SDRMODE_SHIFT;
	if (msr.lo & 1) {
		msrnum = MC_CFCLK_DBUG;			/*  Turn on SDR MTEST stuff*/
		msr = rdmsr(msrnum);
		msr.lo |=  CFCLK_LOWER_SDCLK_SET;
		msr.hi |=  CFCLK_UPPER_MTST_DQS_EN_SET;
		wrmsr(msrnum, msr);
	}

	/*  Lock the cache down here.*/
	__asm__("wbinvd\n");

}
