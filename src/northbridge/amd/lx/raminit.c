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
   	
   	// This is done by sdram_set_spd_registers() that is called by sdram/generic_sdram.c just before this
   	// sdram_set_spd_registers is responsible for reading ram settings from spd rom and configuring sdram conrtoller
   	// Here follows generic sdram initialization procedure.

	/* 2)  Initialize the following GLMC registers:
	 *	— MSR 2000001Ah[15:8] = C8h
	 *	— MSR 20002004h[2] = 0, [0] = 1
	 */
	msr.hi = 0x00000000;
	msr.lo = 0x130AD101;
	wrmsr(MC_CF1017_DATA, msr);

	//ok
	msr.hi = 0x00000000;
	msr.lo = 0x00000001;
	wrmsr(MC_GLD_MSR_PM, msr);

	/* 3) Release MASK_CKE[1:0] (MSR 2000001Dh[9:8] = 11) */

	msr.hi = 0x00000000;
	msr.lo = 0x00000000;
	wrmsr(MC_CFCLK_DBUG, msr);
	
	// reset memory controller
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |=  0x00000002;
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= 0xFFFFFFFD;
	wrmsr(MC_CF07_DATA, msr);

	/* 4. set and clear REF_TST 16 times, more shouldn't hurt
	 * why this is before EMRS and MRS ? */
	 
	for (i = 0; i < 19; i++) {
		msr = rdmsr(MC_CF07_DATA);
		msr.lo |=  0x00000008;
		wrmsr(MC_CF07_DATA, msr);
		msr.lo &= 0xFFFFFFF7;
		wrmsr(MC_CF07_DATA, msr);
	}


	/* 5) Initialize REF_INT (MSR 20000018h[23:8]) to set refresh interval.	*/
	msr.lo |= 0x3A00;
	wrmsr(MC_CF07_DATA, msr);

	/* 6) Perform load-mode with MSR_BA = 01 (MSR 200000018h[29:28] = 01) 
	 * to initialize DIMM Extended Mode register. 
	 * Load-mode is performed by setting/clearing PROG_DRAM (MSR 200000018h[0]).
	 */
// eeldus et bit29 = 0, mida ta praegu ka on
	msr.lo |=  ((0x01 << 28) | 0x01);
	wrmsr(MC_CF07_DATA, msr);

	msr.lo &= ~((0x01 << 28) | 0x01);
	wrmsr(MC_CF07_DATA, msr);


	/* 7. Reset DLL, Bit 27 is undocumented in GX datasheet,
	 * it is documented in LX datasheet  */	
	/* load Mode Register by set and clear PROG_DRAM */
// eeldus et bit27:28=00, mida nad ka on
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |=  ((0x01 << 27) | 0x01);
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= ~((0x01 << 27) | 0x01);
	wrmsr(MC_CF07_DATA, msr);

	//Delay
	i=inb(0x61);
	while (i==inb(0x61));
	i=inb(0x61);
	while (i==inb(0x61));
	i=inb(0x61);
	while (i==inb(0x61));

	/* 8. load Mode Register by set and clear PROG_DRAM */
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |=  0x01;
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= ~0x01;
	wrmsr(MC_CF07_DATA, msr);

	/* wait 200 SDCLKs */
	for (i = 0; i < 200; i++)
		outb(0xaa, 0x80);

	print_debug("DRAM controller init done.\r\n");

	/* Fixes from Jordan Crouse of AMD. */

	/* make sure there is nothing stale in the cache */
	__asm__("wbinvd\n");

	print_debug("RAM DLL lock\r\n");
	/* The RAM dll needs a write to lock on so generate a few dummy writes */
	volatile unsigned long *ptr;
	for (i=0;i<5;i++) {
		ptr = (void *)i;
		*ptr = (unsigned long)i;
	}

}
