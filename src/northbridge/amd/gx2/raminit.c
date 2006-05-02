#include <cpu/amd/gx2def.h>

static void sdram_set_registers(const struct mem_controller *ctrl)
{
}

/* Section 6.1.3, LX processor databooks, BIOS Initialization Sequence
 * Section 4.1.4, GX/CS5535 GeodeROM Porting guide */
static void sdram_enable(int controllers, const struct mem_controller *ctrl)
{
	int i;
	msr_t msr;

	/* 2. clock gating for PMode */
	msr = rdmsr(0x20002004);
	msr.lo &= ~0x04;
	msr.lo |=  0x01;
	wrmsr(0x20002004, msr);
	/* undocmented bits in GX, in LX there are
	 * 8 bits in PM1_UP_DLY */
	msr = rdmsr(0x2000001a);
	msr.lo = 0x0101;
	wrmsr(0x2000001a, msr);
	//print_debug("sdram_enable step 2\r\n");

	/* 3. release CKE mask to enable CKE */
	msr = rdmsr(0x2000001d);
	msr.lo &= ~(0x03 << 8);
	wrmsr(0x2000201d, msr);
	//print_debug("sdram_enable step 3\r\n");

	/* 4. set and clear REF_TST 16 times, more shouldn't hurt
	 * why this is before EMRS and MRS ? */
	for (i = 0; i < 19; i++) {
		msr = rdmsr(0x20000018);
		msr.lo |=  (0x01 << 3);
		wrmsr(0x20000018, msr);
		msr.lo &= ~(0x01 << 3);
		wrmsr(0x20000018, msr);
	}
	//print_debug("sdram_enable step 4\r\n");

	/* 5. set refresh interval */
	msr = rdmsr(0x20000018);
	msr.lo &= ~(0xffff << 8);
	msr.lo |=  (0x34 << 8);
	wrmsr(0x20000018, msr);
	/* set refresh staggering to 4 SDRAM clocks */
	msr = rdmsr(0x20000018);
	msr.lo &= ~(0x03 << 6);
	msr.lo |=  (0x00 << 6);
	wrmsr(0x20000018, msr);
	//print_debug("sdram_enable step 5\r\n");

	/* 6. enable DLL, load Extended Mode Register by set and clear PROG_DRAM */
	msr = rdmsr(0x20000018);
	msr.lo |=  ((0x01 << 28) | 0x01);
	wrmsr(0x20000018, msr);
	msr.lo &= ~((0x01 << 28) | 0x01);
	wrmsr(0x20000018, msr);
	//print_debug("sdram_enable step 6\r\n");

	/* 7. Reset DLL, Bit 27 is undocumented in GX datasheet,
	 * it is documented in LX datasheet  */	
	/* load Mode Register by set and clear PROG_DRAM */
	msr = rdmsr(0x20000018);
	msr.lo |=  ((0x01 << 27) | 0x01);
	wrmsr(0x20000018, msr);
	msr.lo &= ~((0x01 << 27) | 0x01);
	wrmsr(0x20000018, msr);
	//print_debug("sdram_enable step 7\r\n");

	/* 8. load Mode Register by set and clear PROG_DRAM */
	msr = rdmsr(0x20000018);
	msr.lo |=  0x01;
	wrmsr(0x20000018, msr);
	msr.lo &= ~0x01;
	wrmsr(0x20000018, msr);
	//print_debug("sdram_enable step 8\r\n");

	/* wait 200 SDCLKs */
	for (i = 0; i < 200; i++)
		outb(0xaa, 0x80);

	/* load RDSYNC */
	msr = rdmsr(0x2000001f);
	msr.hi = 0x000ff310;
	msr.lo = 0x00000000;
	wrmsr(0x2000001f, msr);

	/* set delay control */
	msr = rdmsr(0x4c00000f);
	msr.hi = 0x830d415a;
	msr.lo = 0x8ea0ad6a;
	wrmsr(0x4c00000f, msr);

	/* DRAM working now?? */

}
