#include <cpu/amd/gx2def.h>

static void sdram_set_registers(const struct mem_controller *ctrl)
{
}

/* here is programming for the various MSRs.*/
#define IM_QWAIT 0x100000

#define DMCF_WRITE_SERIALIZE_REQUEST (2<<12) /* 2 outstanding */ /* in high */
#define DMCF_SERIAL_LOAD_MISSES  (2) /* enabled */

/* these are the 8-bit attributes for controlling RCONF registers */
#define CACHE_DISABLE (1<<0)
#define WRITE_ALLOCATE (1<<1)
#define WRITE_PROTECT (1<<2)
#define WRITE_THROUGH (1<<3)
#define WRITE_COMBINE (1<<4)
#define WRITE_SERIALIZE (1<<5)

/* ram has none of this stuff */
#define RAM_PROPERTIES (0)
#define DEVICE_PROPERTIES (WRITE_SERIALIZE|CACHE_DISABLE)
#define ROM_PROPERTIES (WRITE_SERIALIZE|WRITE_THROUGH|CACHE_DISABLE)
#define MSR_WS_CD_DEFAULT (0x21212121)

/* 1810-1817 give you 8 registers with which to program protection regions */
/* the are region configuration range registers, or RRCF */
/* in msr terms, the are a straight base, top address assign, since they are 4k aligned. */
/* so no left-shift needed for top or base */
#define RRCF_LOW(base,properties) (base|(1<<8)|properties)
#define RRCF_LOW_CD(base)	RRCF_LOW(base, CACHE_DISABLE)

/* build initializer for P2D MSR */
#define P2D_BM(msr, pdid1, bizarro, pbase, pmask) {msr, .hi=(pdid1<<29)|(bizarro<<28)|(pbase>>24), .lo=(pbase<<8)|pmask}
#define P2D_BMO(msr, pdid1, bizarro, poffset, pbase, pmask) {msr, .hi=(pdid1<<29)|(bizarro<<28)|(poffset<<8)|(pbase>>24), .lo=(pbase<<8)|pmask}
#define P2D_R(msr, pdid1, bizarro, pmax, pmin) {msr, .hi=(pdid1<<29)|(bizarro<<28)|(pmax>>12), .lo=(pmax<<20)|pmin}
#define P2D_RO(msr, pdid1, bizarro, poffset, pmax, pmin) {msr, .hi=(pdid1<<29)|(bizarro<<28)|(poffset<<8)|(pmax>>12), .lo=(pmax<<20)|pmin}
#define P2D_SC(msr, pdid1, bizarro, wen, ren,pscbase) {msr, .hi=(pdid1<<29)|(bizarro<<28)|(wen), .lo=(ren<<16)|(pscbase>>18)}
#define IOD_BM(msr, pdid1, bizarro, ibase, imask) {msr, .hi=(pdid1<<29)|(bizarro<<28)|(ibase>>12), .lo=(ibase<<20)|imask}
#define IOD_SC(msr, pdid1, bizarro, en, wen, ren, ibase) {msr, .hi=(pdid1<<29)|(bizarro<<28), .lo=(en<<24)|(wen<<21)|(ren<<20)|(ibase<<3)}



struct msr_defaults {
	int msr_no;
	unsigned long hi, lo;
};
const struct msr_defaults  msr_defaults [] = {
	{0x1700, .hi = 0, .lo = IM_QWAIT},
	{0x1800, .hi = DMCF_WRITE_SERIALIZE_REQUEST, .lo = DMCF_SERIAL_LOAD_MISSES},
	/* 1808 will be done down below, so we have to do 180a->1817 (well, 1813 really) */
	/* for 180a, for now, we assume VSM will configure it */
	/* 180b is left at reset value,a0000-bffff is non-cacheable */
	/* 180c, c0000-dffff is set to write serialize and non-cachable */
	/* oops, 180c will be set by cpu bug handling in cpubug.c */
	//{0x180c, .hi = MSR_WS_CD_DEFAULT, .lo = MSR_WS_CD_DEFAULT},
	/* 180d is left at default, e0000-fffff is non-cached */

	/* we will assume 180e, the ssm region configuration, is left at default or set by VSM */
	/* we will not set 0x180f, the DMM,yet */
	//{0x1810, .hi=0xee7ff000, .lo=RRCF_LOW(0xee000000, WRITE_COMBINE|CACHE_DISABLE)},
	//{0x1811, .hi = 0xefffb000, .lo = RRCF_LOW_CD(0xefff8000)},
	//{0x1812, .hi = 0xefff7000, .lo = RRCF_LOW_CD(0xefff4000)},
	//{0x1813, .hi = 0xefff3000, .lo = RRCF_LOW_CD(0xefff0000)},
	/* now for GLPCI routing */
	/* GLIU0 */
	P2D_BM(0x10000020, 0x1, 0x0, 0x0, 0xfff80),
	P2D_BM(0x10000021, 0x1, 0x0, 0x80000, 0xfffe0),
	P2D_SC(0x1000002c, 0x1, 0x0, 0x0,  0xff03, 0x3),
	/* GLIU1 */
	P2D_BM(0x40000020, 0x1, 0x0, 0x0, 0xfff80),
	P2D_BM(0x40000021, 0x1, 0x0, 0x80000, 0xfffe0),
	P2D_SC(0x4000002d, 0x1, 0x0, 0x0,  0xff03, 0x3),
	{0}
};

#define SMM_OFFSET 0x40400000
#define SMM_SIZE   256

/* we have to do this here. We have not found a nicer way to do it */
void
setup_gx2(void)
{
	int i;
	unsigned long tmp, tmp2, tmp3;
	msr_t msr;
	unsigned long sizem, membytes;
#if 0
	sizem = setup_gx2_cache();

	membytes = sizem * 1048576;

	/* we need to set 0x10000028 and 0x40000029 */
	//	print_debug("sizem 0x%x, membytes 0x%x\n", sizem, membytes);
	msr.hi = 0x20000000 | membytes>>24;
	msr.lo = 0x100 | ( ((membytes >>12) & 0xfff) << 20);
	wrmsr(0x10000028, msr);
	msr.hi = 0x20000000 | membytes>>24;
	msr.lo = 0x100 | ( ((membytes >>12) & 0xfff) << 20);
	wrmsr(0x40000029, msr);
	msr = rdmsr(0x10000028);
	//	print_debug("MSR 0x%x is now 0x%x:0x%x\n", 0x10000028, msr.hi,msr.lo);
	msr = rdmsr(0x40000029);
	//	print_debug("MSR 0x%x is now 0x%x:0x%x\n", 0x40000029, msr.hi,msr.lo);


	/* fixme: SMM MSR 0x10000026 and 0x400000023 */
	/* calculate the OFFSET field */
	tmp = membytes - SMM_OFFSET;
	tmp >>= 12;
	tmp <<= 8;
	tmp |= 0x20000000;
	tmp |= (SMM_OFFSET >> 24);

	/* calculate the PBASE and PMASK fields */
	tmp2 = (SMM_OFFSET << 8) & 0xFFF00000; /* shift right 12 then left 20  == left 8 */
	tmp2 |= (((~(SMM_SIZE * 1024) + 1) >> 12) & 0xfffff);
	//	print_debug("MSR 0x%x is now 0x%x:0x%x\n", 0x10000026, tmp, tmp2);
	msr.hi = tmp;
	msr.lo = tmp2;
	wrmsr(0x10000026, msr);
#else
	msr.hi = 0x2000000f;
	msr.lo = 0xfbf00100;
	wrmsr(0x10000028, msr);
	msr = rdmsr(0x10000028);
	//	print_debug("MSR 0x%x is now 0x%x:0x%x\n", 0x10000028, msr.hi, msr.lo);
	wrmsr(0x40000029, msr);
	msr = rdmsr(0x40000029);
	//	print_debug("MSR 0x%x is now 0x%x:0x%x\n", 0x40000029, msr.hi, msr.lo);

	msr.hi = 0x2cfbc040;
	msr.lo = 0x400fffc0;
	wrmsr(0x10000026, msr);
	msr = rdmsr(0x10000026);
	//	print_debug("MSR 0x%x is now 0x%x:0x%x\n", 0x10000026, msr.hi, msr.lo);

	msr.hi = 0x22fffc02;
	msr.lo = 0x10ffbf00;
	wrmsr(0x1808, msr);
	msr = rdmsr(0x1808);
	//	print_debug("MSR 0x%x is now 0x%x:0x%x\n", 0x1808, msr.hi, msr.lo);
#endif
	/* now do the default MSR values */

	wrmsr(msr_defaults[0].msr_no, msr);
	for(i = 0; msr_defaults[i].msr_no; i++) {
		//		msr_t msr;
		msr.lo = msr_defaults[i].lo;
		msr.hi = msr_defaults[i].hi;
		wrmsr(msr_defaults[i].msr_no, msr);
		//msr = rdmsr(msr_defaults[i].msr_no);
		//		print_debug("MSR 0x%x is now 0x%x:0x%x\n", msr_defaults[i].msr_no, msr.hi,msr.lo);
	}
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
	setup_gx2();
}
