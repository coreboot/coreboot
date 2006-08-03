#include <cpu/amd/lxdef.h>


static void sdram_set_registers(const struct mem_controller *ctrl)
{
}

/* Section 6.1.3, LX processor databooks, BIOS Initialization Sequence
 * Section 4.1.4, GX/CS5535 GeodeROM Porting guide */
static void sdram_enable(int controllers, const struct mem_controller *ctrl)
{
	int i;
	msr_t msr;

	/* DRAM initialization sequence according to the documentation:
	 * 1)  Initialize the following GLMC registers/bits based on Serial Presence Detect (SPD) values:
	 *  — MSR 20000018h except REF_INT bits [23:8]
   	 *   — MSR 20000019h
   	 */
   	
   		// This is done by sdram_set_spd_registers()
		/*WR_MSR   MC_CF07_DATA,   DIMMCONFIG, 0x05000040

		;WR_MSR	MC_CF07_DATA,   DIMMCONFIG, 0x00000040		; MSR 20000018h except REF_INT bits [23:8]. REF_STAG value from DOCS*/
		
	/*
	 * 0x18000100 : 0x696332A3
	 * 
	 * 		63 - 32 | 31                  15                0
	 * xxxxxxx		| 0110 1001 0110 0011 0011 0010 1010 0011
	 * 
	 *  30:28 CAS latency
	 * 
	 * 010 - 2.0
	 * 110 - 2.5
	 *
	 */

	msr.hi = 0x18000100;
	msr.lo = 0x696332A3;
	wrmsr(MC_CF8F_DATA, msr);

	/* 2)  Initialize the following GLMC registers:
	 *	— MSR 2000001Ah[15:8] = C8h
	 *	— MSR 20002004h[2] = 0, [0] = 1
	 */
	msr.hi = 0x00000000;
	msr.lo = 0x130AD101;
	wrmsr(MC_CF1017_DATA, msr);

	msr.hi = 0x00000000;
	msr.lo = 0x00000001;
	wrmsr(MC_GLD_MSR_PM, msr);

	/* 3) Release MASK_CKE[1:0] (MSR 2000001Dh[9:8] = 11) */

	msr.hi = 0x00000000;
	msr.lo = 0x00001000;
	wrmsr(MC_CFCLK_DBUG, msr);
	
	//print_debug("sdram_enable step 3\r\n");

	/* 4. set and clear REF_TST 16 times, more shouldn't hurt
	 * why this is before EMRS and MRS ? */
	 
	for (i = 0; i < 19; i++) {
		msr = rdmsr(MC_CF07_DATA);
		msr.lo |=  (0x01 << 3);
		wrmsr(MC_CF07_DATA, msr);
		msr.lo &= ~(0x01 << 3);
		wrmsr(MC_CF07_DATA, msr);
	}


	/* 5) Initialize REF_INT (MSR 20000018h[23:8]) to set refresh interval.	*/
	msr.lo |= 0x2B00;
	wrmsr(MC_CF07_DATA, msr);



	/* set refresh staggering to 4 SDRAM clocks */
	msr = rdmsr(0x20000018);
	msr.lo &= ~(0x03 << 6);
	msr.lo |=  (0x00 << 6);
	wrmsr(0x20000018, msr);
	//print_debug("sdram_enable step 5\r\n");


	/* 6) Perform load-mode with MSR_BA = 01 (MSR 200000018h[29:28] = 01) 
	 * to initialize DIMM Extended Mode register. 
	 * Load-mode is performed by setting/clearing PROG_DRAM (MSR 200000018h[0]).
	 */
	msr.lo |=  ((0x01 << 28) | 0x01);
	wrmsr(MC_CF07_DATA, msr);

	msr.lo &= ~((0x01 << 28) | 0x01);
	wrmsr(MC_CF07_DATA, msr);


	/* 7. Reset DLL, Bit 27 is undocumented in GX datasheet,
	 * it is documented in LX datasheet  */	
	/* load Mode Register by set and clear PROG_DRAM */
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |=  ((0x01 << 27) | 0x01);
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= ~((0x01 << 27) | 0x01);
	wrmsr(MC_CF07_DATA, msr);
	//print_debug("sdram_enable step 7\r\n");


	/* 8. load Mode Register by set and clear PROG_DRAM */
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |=  0x01;
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= ~0x01;
	wrmsr(MC_CF07_DATA, msr);
	//print_debug("sdram_enable step 8\r\n");

	/* wait 200 SDCLKs */
	for (i = 0; i < 200; i++)
		outb(0xaa, 0x80);

	/* load RDSYNC */
	/*msr = rdmsr(0x2000001f);
	msr.hi = 0x000ff310;
	msr.lo = 0x00000000;
	wrmsr(0x2000001f, msr);*/

	/* set delay control */
	msr = rdmsr(0x4c00000f);
	msr.hi = 0x830d415a;
	msr.lo = 0x8ea0ad6a;
	wrmsr(0x4c00000f, msr);


	print_debug("DRAM controller init done.\r\n");

	/* DRAM working now?? */

}
