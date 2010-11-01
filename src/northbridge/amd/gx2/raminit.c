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
	msr = rdmsr(MC_GLD_MSR_PM);
	msr.lo &= ~0x04;
	msr.lo |=  0x01;
	wrmsr(MC_GLD_MSR_PM, msr);
	/* undocmented bits in GX, in LX there are
	 * 8 bits in PM1_UP_DLY */
	msr = rdmsr(MC_CF1017_DATA);
	msr.lo = 0x0101;
	wrmsr(MC_CF1017_DATA, msr);
	//print_debug("sdram_enable step 2\n");

	/* 3. release CKE mask to enable CKE */
	msr = rdmsr(MC_CFCLK_DBUG);
	msr.lo &= ~(0x03 << 8);
	wrmsr(MC_CFCLK_DBUG, msr);
	//print_debug("sdram_enable step 3\n");

	/* 4. set and clear REF_TST 16 times, more shouldn't hurt
	 * why this is before EMRS and MRS ? */
	for (i = 0; i < 19; i++) {
		msr = rdmsr(MC_CF07_DATA);
		msr.lo |=  (0x01 << 3);
		wrmsr(MC_CF07_DATA, msr);
		msr.lo &= ~(0x01 << 3);
		wrmsr(MC_CF07_DATA, msr);
	}
	//print_debug("sdram_enable step 4\n");

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
	//print_debug("sdram_enable step 5\n");

	/* 6. enable DLL, load Extended Mode Register by set and clear PROG_DRAM */
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |=  ((0x01 << 28) | 0x01);
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= ~((0x01 << 28) | 0x01);
	wrmsr(MC_CF07_DATA, msr);
	//print_debug("sdram_enable step 6\n");

	/* 7. Reset DLL, Bit 27 is undocumented in GX datasheet,
	 * it is documented in LX datasheet  */
	/* load Mode Register by set and clear PROG_DRAM */
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |=  ((0x01 << 27) | 0x01);
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= ~((0x01 << 27) | 0x01);
	wrmsr(MC_CF07_DATA, msr);
	//print_debug("sdram_enable step 7\n");

	/* 8. load Mode Register by set and clear PROG_DRAM */
	msr = rdmsr(MC_CF07_DATA);
	msr.lo |=  0x01;
	wrmsr(MC_CF07_DATA, msr);
	msr.lo &= ~0x01;
	wrmsr(MC_CF07_DATA, msr);
	//print_debug("sdram_enable step 8\n");

	/* wait 200 SDCLKs */
	for (i = 0; i < 200; i++)
		outb(0xaa, 0x80);

	/* load RDSYNC */
	msr = rdmsr(MC_CF_RDSYNC);
	msr.hi = 0x000ff310;
	/* the above setting is supposed to be good for "slow" ram. We have found that for
	 * some dram, at some clock rates, e.g. hynix at 366/244, this will actually
	 * cause errors. The fix is to just set it to 0x310. Tested on 3 boards
	 * with 3 different type of dram -- Hynix, PSC, infineon.
	 * I am leaving this comment here so that at some future time nobody is tempted
	 * to mess with this setting -- RGM, 9/2006
	 */
	msr.hi = 0x00000310;
	msr.lo = 0x00000000;
	wrmsr(MC_CF_RDSYNC, msr);

	/* set delay control */
	msr = rdmsr(GLCP_DELAY_CONTROLS);
	msr.hi = 0x830d415a;
	msr.lo = 0x8ea0ad6a;
	wrmsr(GLCP_DELAY_CONTROLS, msr);

	/* The RAM dll needs a write to lock on so generate a few dummy writes */
	/* Note: The descriptor needs to be enabled to point at memory */
	volatile unsigned long *ptr;
	for (i = 0; i < 5; i++) {
		ptr = (void *)i;
		*ptr = (unsigned long)i;
	}

	print_info("RAM DLL lock\n");

}
