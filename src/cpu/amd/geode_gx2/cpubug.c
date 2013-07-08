#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/amd/gx2def.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/cache.h>

#if 0
void bug645(void)
{
	msr_t msr;
	rdmsr(CPU_ID_CONFIG);
	msr.whatever |= ID_CONFIG_SERIAL_SET;
	wrmsr(msr);
}

void bug573(void)
{
	msr_t msr;
	msr = rdmsr(MC_GLD_MSR_PM);
	msr.eax &= 0xfff3;
	wrmsr(MC_GLD_MSR_PM);
}
#endif

/* pcideadlock
 *
 * Bugtool #465 and #609
 * PCI cache deadlock
 * There is also fix code in cache and PCI functions. This bug is very is pervasive.
 */
static void pcideadlock(void)
{
	msr_t msr;

	/* forces serialization of all load misses. Setting this bit prevents the
	 * DM pipe from backing up if a read request has to be held up waiting
	 * for PCI writes to complete.
	 */
	msr = rdmsr(CPU_DM_CONFIG0);
	msr.hi &= ~(7<<DM_CONFIG0_UPPER_WSREQ_SHIFT);
	msr.hi |= (2<<DM_CONFIG0_UPPER_WSREQ_SHIFT);
	msr.lo |= DM_CONFIG0_LOWER_MISSER_SET;
	wrmsr(CPU_DM_CONFIG0, msr);

	/* interlock instruction fetches to WS regions with data accesses.
	 * This prevents an instruction fetch from going out to PCI if the
	 * data side is about to make a request.
	 */
	msr = rdmsr(CPU_IM_CONFIG);
	msr.lo |= IM_CONFIG_LOWER_QWT_SET;
	wrmsr(CPU_IM_CONFIG, msr);

	/* write serialize memory hole to PCI. Need to unWS when something is
	 * shadowed regardless of cachablility.
	 */
	msr.lo = 0x021212121;
	msr.hi = 0x021212121;
	wrmsr( CPU_RCONF_A0_BF, msr);
	wrmsr( CPU_RCONF_C0_DF, msr);
	wrmsr( CPU_RCONF_E0_FF, msr);
}

/* CPUbug784
 *
 * Bugtool #784 + #792
 *
 * Fix CPUID instructions for < 3.0 CPUs
 */
static void bug784(void)
{
	msr_t msr;
	//static char *name = "Geode by NSC";

	/* we'll do this the stupid way, for now, but that's the string they want. NO ONE KNOWS why you
	 * would do this -- the OS can figure this type of stuff out!
	 */
	msr = rdmsr(0x3006);
	msr.hi = 0x646f6547;
	wrmsr(0x3006, msr);

	msr = rdmsr(0x3007);
	msr.hi = 0x79622065;
	msr.lo = 0x43534e20;
	wrmsr(0x3007, msr);

	msr = rdmsr(0x3002);
	wrmsr(0x3008, msr);

	/* More CPUID to match AMD better. #792*/
	msr = rdmsr(0x3009);
	msr.hi = 0x0C0C0A13D;
	msr.lo = 0x00000000;
	wrmsr(0x3009, msr);
}

/* cpubug 1398: enable MC if we KNOW we have DDR*/

/* CPUbugIAENG1398
 *
 * ClearQuest #IAENG1398
 * The MC can not be enabled with SDR memory but can for DDR. Enable for
 * DDR here if the setup token is "Default"
 * Add this back to core by default once 2.0 CPUs are not supported.
 */
static void eng1398(void)
{
	msr_t msr;

	msr = rdmsr(MSR_GLCP+0x17);
	if ((msr.lo & 0xff) <= CPU_REV_2_0) {
		msr = rdmsr(GLCP_SYS_RSTPLL);
		if (msr.lo & (1<<RSTPPL_LOWER_SDRMODE_SHIFT))
			return;
	}

	/* no CMOS/NVRAM to check, so enable MC Clock Gating */
	msr = rdmsr(MC_GLD_MSR_PM);
	msr.lo |= 3; /* enable MC clock gating.*/
	wrmsr(MC_GLD_MSR_PM, msr);
}

/* CPUbugIAENG2900
 *
 * Clear Quest IAENG00002900, VSS 118.150
 *
 * BTB issue causes blue screen in windows, but the fix is required
 * for all operating systems.
 */
static void eng2900(void)
{
	msr_t msr;

	printk(BIOS_DEBUG, "CPU_BUG:%s\n", __func__);
	/* Clear bit 43, disables the sysenter/sysexit in CPUID3 */
	msr = rdmsr(0x3003);
	msr.hi &= 0xFFFFF7FF;
	wrmsr(0x3003, msr);

	/* change this value to zero if you need to disable this BTB SWAPSiF. */
	if (1) {

		/* Disable enable_actions in DIAGCTL while setting up GLCP */
		msr.hi = 0;
		msr.lo = 0;
		wrmsr(MSR_GLCP + 0x005f, msr);

		/* Changing DBGCLKCTL register to GeodeLink */
		msr.hi = 0;
		msr.lo = 0;
		wrmsr(MSR_GLCP + 0x0016, msr);

		msr.hi = 0;
		msr.lo = 2;
		wrmsr(MSR_GLCP + 0x0016, msr);

		/* The code below sets up the CPU to stall for 4 GeodeLink
		 * clocks when CPU is snooped.  Because setting XSTATE to 0
		 * overrides any other XSTATE action, the code will always
		 * stall for 4 GeodeLink clocks after a snoop request goes
		 * away even if it occurred a clock or two later than a
		 * different snoop; the stall signal will never 'glitch high'
		 * for only one or two CPU clocks with this code.
		 */

		/* Send mb0 port 3 requests to upper GeodeLink diag bits
		   [63:32] */
		msr.hi = 0;
		msr.lo = 0x80338041;
		wrmsr(MSR_GLIU0 + 0x2005, msr);

		/* set5m watches request ready from mb0 to CPU (snoop) */
		msr.hi = 0x5ad68000;
		msr.lo = 0;
		wrmsr(MSR_GLCP + 0x0045, msr);

		/* SET4M will be high when state is idle (XSTATE=11) */
		msr.hi = 0;
		msr.lo = 0x0140;
		wrmsr(MSR_GLCP + 0x0044, msr);

		/* SET5n to watch for processor stalled state */
		msr.hi = 0x2000;
		msr.lo = 0;
		wrmsr(MSR_GLCP + 0x004D, msr);

		/* Writing action number 13: XSTATE=0 to occur when CPU is
		   snooped unless we're stalled */
		msr.hi = 0;
		msr.lo = 0x00400000;
		wrmsr(MSR_GLCP + 0x0075, msr);

		/* Writing action number 11: inc XSTATE every GeodeLink clock
		   unless we're idle */
		msr.hi = 0;
		msr.lo = 0x30000;
		wrmsr(MSR_GLCP + 0x0073, msr);

		/* Writing action number 5: STALL_CPU_PIPE when exiting idle
		   state or not in idle state */
		msr.hi = 0;
		msr.lo = 0x00430000;
		wrmsr(MSR_GLCP + 0x006D, msr);

		/* Writing DIAGCTL Register to enable the stall action and to
		   let set5m watch the upper GeodeLink diag bits. */
		msr.hi = 0;
		msr.lo = 0x80004000;
		wrmsr(MSR_GLCP + 0x005f, msr);
	}
}

static void bug118253(void)
{
	/* GLPCI PIO Post Control shouldn't be enabled */
	msr_t msr;

	msr = rdmsr(GLPCI_SPARE);
	msr.lo &= ~GLPCI_SPARE_LOWER_PPC_SET;
	wrmsr(GLPCI_SPARE, msr);
}

static void bug118339(void)
{
	/* per AMD, do this always */
	msr_t msr = {0,0};
	int msrnum;

	/* Disable enable_actions in DIAGCTL while setting up GLCP */
	wrmsr(MSR_GLCP + 0x005f, msr);

	/* SET2M fires if VG pri is odd (3, not 2) and Ystate=0 */
	msrnum =  MSR_GLCP + 0x042;
	/* 	msr.hi =  2d6b8000h */;
	msr.hi =  0x596b8000;
	msr.lo =  0x00000a00;
	wrmsr(msrnum, msr);

	/* SET3M fires if MBUS changed and VG pri is odd */
	msrnum =  MSR_GLCP + 0x043;
	msr.hi =  0x596b8040;
	msr.lo = 0;
	wrmsr(msrnum, msr);

	/* Put VG request data on lower diag bus */
	msrnum =  MSR_GLIU0 + 0x2005;
	msr.hi = 0;
	msr.lo =  0x80338041;
	wrmsr(msrnum, msr);

	/* Increment Y state if SET3M if true */
	msrnum =  MSR_GLCP + 0x074;
	msr.hi = 0;
	msr.lo =  0x0000c000;
	wrmsr(msrnum, msr);

	/* Set up MBUS action to PRI=3 read of MBIU */
	msrnum =  MSR_GLCP + 0x020;
	msr.hi =  0x0000d863;
	msr.lo =  0x20002000;
	wrmsr(msrnum, msr);

	/* Trigger MBUS action if VG=pri3 and Y=0, this blocks most PCI */
	msrnum =  MSR_GLCP + 0x071;
	msr.hi = 0;
	msr.lo =  0x00000c00;
	wrmsr(msrnum, msr);

	/* Writing DIAGCTL */
	msrnum =  MSR_GLCP + 0x005f;
	msr.hi = 0;
	msr.lo =  0x80004000;
	wrmsr(msrnum, msr);

	/* Code to enable FS2 even when BTB and VGTEAR SWAPSiFs are enabled
	 * As per Todd Roberts in PBz1094 and PBz1095
	 * Moved from CPUREG to CPUBUG per Tom Sylla
	 */
	msrnum =  0x04C000042;		/*  GLCP SETMCTL Register */
	msr = rdmsr(msrnum);
	msr.hi |= 8;			/*  Bit 35 = MCP_IN */
	wrmsr(msrnum, msr);
}



/* DisableMemoryReorder
 *
 * PBZ 3659:
 * The MC reordered transactions incorrectly and breaks coherency.
 * Disable reordering and take a potential performance hit.
 * This is safe to do here and not in MC init since there is nothing
 * to maintain coherency with and the cache is not enabled yet.
 */
static void disablememoryreadorder(void)
{
	msr_t msr;

	msr = rdmsr(MC_CF8F_DATA);
	msr.hi |=  CF8F_UPPER_REORDER_DIS_SET;
	wrmsr(MC_CF8F_DATA, msr);
}

void cpubug(void)
{
	msr_t msr;
	int rev;

	msr = rdmsr(GLCP_CHIP_REVID);

	rev = msr.lo & 0xff;
	if (rev < 0x20) {
		printk(BIOS_ERR, "%s: rev < 0x20! bailing!\n", __func__);
		return;
	}
	printk(BIOS_DEBUG, "Doing cpubug fixes for rev 0x%x\n", rev);
	switch(rev)
	{
		case 0x20:
			pcideadlock();
			eng1398();
			/* cs 5530 bug; ignore
			bug752();
			*/
			break;
		case 0x21:
			pcideadlock();
			eng1398();
			eng2900();
			bug118339();
			break;
		case 0x22:
		case 0x30:
			break;
		default:
			printk(BIOS_ERR, "unknown rev %x, bailing\n", rev);
			return;
	}
	bug784();
	bug118253();
	disablememoryreadorder();
	printk(BIOS_DEBUG, "Done cpubug fixes \n");
}
