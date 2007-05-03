/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2006 Indrek Kruusa <indrek.kruusa@artecdesign.ee>
 * Copyright (C) 2006 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

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
	
	//GX3 suspend: what is desired?

	/*  Enable Suspend on Halt*/
	/*msrnum = CPU_XC_CONFIG;
	msr = rdmsr(msrnum);
	msr.lo |=  XC_CONFIG_SUSP_ON_HLT;
	wrmsr(msrnum, msr);*/

	/*  ENable SUSP and allow TSC to run in Suspend */
	/*  to keep speed detection happy*/
	/*msrnum = CPU_BC_CONF_0;
	msr = rdmsr(msrnum);
	msr.lo |=  TSC_SUSP_SET | SUSP_EN_SET;
	wrmsr(msrnum, msr);*/

	/*  Setup throttling to proper mode if it is ever enabled.*/
	msrnum = 0x04C00001E;
	msr.hi =  0x000000000;
	msr.lo =  0x00000603C;
	wrmsr(msrnum, msr);		// GX3 OK +/-


/*  Only do this if we are building for 5535*/
/* */
/*  FooGlue Setup*/
/* */
#if 0
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

// GX3: Disable DOT PLL? No. Lets tick.

/*	msrnum = GLCP_DOTPLL;
	msr = rdmsr(msrnum);
	msr.lo |= DOTPPL_LOWER_PD_SET;
	wrmsr(msrnum, msr); */

/* */
/*  Enable RSDC*/
/* */
	msrnum = 0x1301 ;
	msr = rdmsr(msrnum);
	msr.lo |=  0x08;
	wrmsr(msrnum, msr);		//GX3 OK


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

//GX3: BTB is enabled by default

/*	msrnum = MSR_GLCP+0x17;
	msr = rdmsr(msrnum);
	if (msr.lo >= CPU_REV_2_1){
		msrnum = CPU_PF_BTB_CONF;
		msr = rdmsr(msrnum);
		msr.lo |= BTB_ENABLE_SET | RETURN_STACK_ENABLE_SET;
		wrmsr(msrnum, msr);
	}

	*/

/* */
/*  FPU impercise exceptions bit*/
/* */
	/*if (getnvram( TOKEN_FPU_IE_ENABLE) != TVALUE_DISABLE) {*/



// GX3: FPU impercise exceptions bit - what's that?
/*	{
		msrnum = CPU_FPU_MSR_MODE;
		msr = rdmsr(msrnum);
		msr.lo |= FPU_IE_SET;
		wrmsr(msrnum, msr);
	}

	*/

#if 0
	/* */
	/*  Cache Overides*/
	/* */
	/* This code disables the data cache.  Don't execute this
	 * unless you're testing something.
	 */ 
	/*  Allow NVRam to override DM Setup*/
	/*if (getnvram( TOKEN_CACHE_DM_MODE) != 1) {*/
	{

		msrnum = CPU_DM_CONFIG0;
		msr = rdmsr(msrnum);
		msr.lo |=  DM_CONFIG0_LOWER_DCDIS_SET;
		wrmsr(msrnum, msr);
	}
	/* This code disables the instruction cache.  Don't execute
	 * this unless you're testing something.
	*/ 
	/*  Allow NVRam to override IM Setup*/
	/*if (getnvram( TOKEN_CACHE_IM_MODE) ==1) {*/
	{
		msrnum = CPU_IM_CONFIG;
		msr = rdmsr(msrnum);
		msr.lo |=  IM_CONFIG_LOWER_ICD_SET;
		wrmsr(msrnum, msr);
	}
#endif
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
