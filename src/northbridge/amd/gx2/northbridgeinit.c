#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include "chip.h"
#include "northbridge.h"
#include <cpu/amd/gx2def.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/cache.h>

/* put this here for now, we are not sure where it belongs */

struct gliutable
{
	unsigned long desc_name;
	unsigned short desc_type;
	unsigned long hi, lo;
};

struct gliutable gliu0table[] = {
	{.desc_name=MSR_GLIU0_BASE1,  .desc_type= BM,.hi= MSR_MC + 0x0,.lo=  0x0FFF80},			/* 0-7FFFF to MC */
	{.desc_name=MSR_GLIU0_BASE2,  .desc_type= BM,.hi= MSR_MC + 0x0,.lo=(0x80 << 20) + 0x0FFFE0},	/* 80000-9ffff to Mc */
	{.desc_name=MSR_GLIU0_SHADOW, .desc_type= SC_SHADOW,.hi=  MSR_MC + 0x0,.lo=  0x03},		/* C0000-Fffff split to MC and PCI (sub decode) A0000-Bffff handled by SoftVideo */
	{.desc_name=MSR_GLIU0_SYSMEM, .desc_type= R_SYSMEM,.hi=  MSR_MC,.lo=  0x0},			/* Catch and fix dynamicly. */
	{.desc_name=MSR_GLIU0_DMM,    .desc_type= BMO_DMM,.hi=  MSR_MC,.lo=  0x0},			/* Catch and fix dynamicly. */
	{.desc_name=MSR_GLIU0_SMM,    .desc_type= BMO_SMM,.hi=  MSR_MC,.lo=  0x0},			/* Catch and fix dynamicly. */
	{.desc_name=GLIU0_GLD_MSR_COH,.desc_type= OTHER,.hi= 0x0,.lo= GL0_CPU},
	{.desc_name=GL_END,           .desc_type= GL_END,.hi= 0x0,.lo= 0x0},
};

struct gliutable gliu1table[] = {
	{.desc_name=MSR_GLIU1_BASE1,.desc_type=  BM,.hi=  MSR_GL0 + 0x0,.lo=  0x0FFF80},		/* 0-7FFFF to MC */
	{.desc_name=MSR_GLIU1_BASE2,.desc_type=  BM,.hi=  MSR_GL0 + 0x0,.lo= (0x80 << 20) +0x0FFFE0},	/* 80000-9ffff to Mc */
	{.desc_name=MSR_GLIU1_SHADOW,.desc_type=  SC_SHADOW,.hi=  MSR_GL0 + 0x0,.lo=  0x03},		/* C0000-Fffff split to MC and PCI (sub decode) */
	{.desc_name=MSR_GLIU1_SYSMEM,.desc_type=  R_SYSMEM,.hi=  MSR_GL0,.lo=  0x0},			/* Catch and fix dynamicly. */
	{.desc_name=MSR_GLIU1_DMM,.desc_type=  BM_DMM,.hi=  MSR_GL0,.lo=  0x0},				/* Catch and fix dynamicly. */
	{.desc_name=MSR_GLIU1_SMM,.desc_type=  BM_SMM,.hi=  MSR_GL0,.lo=  0x0},				/* Catch and fix dynamicly. */
	{.desc_name=GLIU1_GLD_MSR_COH,.desc_type= OTHER,.hi= 0x0,.lo= GL1_GLIU0},
	{.desc_name=MSR_GLIU1_FPU_TRAP,.desc_type=  SCIO,.hi=  (GL1_GLCP << 29) + 0x0,.lo=  0x033000F0}, /* FooGlue FPU 0xF0 */
	{.desc_name=GL_END,.desc_type= GL_END,.hi= 0x0,.lo= 0x0},
};

struct gliutable *gliutables[]  = {gliu0table, gliu1table, 0};

struct msrinit
{
	unsigned long msrnum;
	msr_t msr;
};

struct msrinit ClockGatingDefault [] = {
	{GLIU0_GLD_MSR_PM,	{.hi=0x00,.lo=0x0005}},
	/* MC must stay off in SDR mode. It is turned on in CPUBug??? lotus #77.142 */
	{MC_GLD_MSR_PM,		{.hi=0x00,.lo=0x0000}},
	{GLIU1_GLD_MSR_PM,	{.hi=0x00,.lo=0x0005}},
	{VG_GLD_MSR_PM,		{.hi=0x00,.lo=0x0000}},	/* lotus #77.163 */
	{GP_GLD_MSR_PM,		{.hi=0x00,.lo=0x0001}},
	{DF_GLD_MSR_PM,		{.hi=0x00,.lo=0x0155}},
	{GLCP_GLD_MSR_PM,	{.hi=0x00,.lo=0x0015}},
	{GLPCI_GLD_MSR_PM,	{.hi=0x00,.lo=0x0015}},
	{FG_GLD_MSR_PM,		{.hi=0x00,.lo=0x0000}},	/* Always on */
	{0xffffffff, 				{0xffffffff, 0xffffffff}},
};

/* All On */
struct msrinit ClockGatingAllOn[] = {
	{GLIU0_GLD_MSR_PM,	{.hi=0x00,.lo=0x0FFFFFFFF}},
	{MC_GLD_MSR_PM,		{.hi=0x00,.lo=0x0FFFFFFFF}},
	{GLIU1_GLD_MSR_PM,	{.hi=0x00,.lo=0x0FFFFFFFF}},
	{VG_GLD_MSR_PM,		{.hi=0x00, .lo=0x00}},
	{GP_GLD_MSR_PM,		{.hi=0x00,.lo=0x000000001}},
	{DF_GLD_MSR_PM,		{.hi=0x00,.lo=0x0FFFFFFFF}},
	{GLCP_GLD_MSR_PM,	{.hi=0x00,.lo=0x0FFFFFFFF}},
	{GLPCI_GLD_MSR_PM,	{.hi=0x00,.lo=0x0FFFFFFFF}},
	{FG_GLD_MSR_PM,		{.hi=0x00,.lo=0x0000}},
 	{0xffffffff, 				{0xffffffff, 0xffffffff}},
};

/* Performance */
struct msrinit ClockGatingPerformance[] = {
	{VG_GLD_MSR_PM,		{.hi=0x00,.lo=0x0000}},	/* lotus #77.163 */
	{GP_GLD_MSR_PM,		{.hi=0x00,.lo=0x0001}},
	{DF_GLD_MSR_PM,		{.hi=0x00,.lo=0x0155}},
	{GLCP_GLD_MSR_PM,	{.hi=0x00,.lo=0x0015}},
	{0xffffffff, 				{0xffffffff, 0xffffffff}},
};

/* SET GeodeLink PRIORITY */
struct msrinit GeodeLinkPriorityTable [] = {
	{CPU_GLD_MSR_CONFIG,		{.hi=0x00,.lo=0x0220}},		/* CPU Priority. */
	{DF_GLD_MSR_MASTER_CONF,	{.hi=0x00,.lo=0x0000}},		/* DF Priority. */
	{VG_GLD_MSR_CONFIG,		{.hi=0x00,.lo=0x0720}},		/* VG Primary and Secondary Priority. */
	{GP_GLD_MSR_CONFIG,		{.hi=0x00,.lo=0x0010}},		/* Graphics Priority. */
	{GLPCI_GLD_MSR_CONFIG,		{.hi=0x00,.lo=0x0027}},		/* GLPCI Priority + PID */
	{GLCP_GLD_MSR_CONF,		{.hi=0x00,.lo=0x0001}},		/* GLCP Priority + PID */
	{VIP_GLD_MSR_CONFIG,		{.hi=0x00,.lo=0x0622}},		/* VIP PID */
	{AES_GLD_MSR_CONFIG,		{.hi=0x00,.lo=0x0013}},		/* AES PID */
	{0x0FFFFFFFF, 			{0x0FFFFFFFF, 0x0FFFFFFFF}},	/* END */
};

/* do we have dmi or not? assume NO per AMD */
int havedmi = 0;

static void writeglmsr(struct gliutable *gl)
{
	msr_t msr;

	msr.lo = gl->lo;
	msr.hi = gl->hi;
	wrmsr(gl->desc_name, msr);	/* MSR - see table above */
	printk(BIOS_DEBUG, "%s: write msr 0x%08lx, val 0x%08x:0x%08x\n", __func__, gl->desc_name, msr.hi, msr.lo);
	/* they do this, so we do this */
	msr = rdmsr(gl->desc_name);
	printk(BIOS_DEBUG, "%s: AFTER write msr 0x%08lx, val 0x%08x:0x%08x\n", __func__, gl->desc_name, msr.hi, msr.lo);
}

static void ShadowInit(struct gliutable *gl)
{
	msr_t msr;

	msr = rdmsr(gl->desc_name);

	if (msr.lo == 0) {
		writeglmsr(gl);
	}
}

/* NOTE: transcribed from assembly code. There is the usual redundant assembly nonsense in here.
 * CLEAN ME UP
 */
/* yes, this duplicates later code, but it seems that is how they want it done. */
static void SysmemInit(struct gliutable *gl)
{
	msr_t msr;
	int sizembytes, sizebytes;

	/* Figure out how much RAM is in the machine and alocate all to the
	 * system. We will adjust for SMM and DMM now and Frame Buffer later.
	 */
	sizembytes = sizeram();
	printk(BIOS_DEBUG, "%s: enable for %dm bytes\n", __func__, sizembytes);
	sizebytes = sizembytes << 20;

	sizebytes -= SMM_SIZE*1024 +1;

	if (havedmi)
		sizebytes -= DMM_SIZE * 1024 + 1;

	sizebytes -= 1;
	msr.hi = (gl->hi & 0xFFFFFF00) | (sizebytes >> 24);
	/* set up sizebytes to fit into msr.lo */
	sizebytes <<= 8; /* what? well, we want bits 23:12 in bits 31:20. */
	sizebytes &= 0xfff00000;
	sizebytes |= 0x100;
	msr.lo = sizebytes;
	wrmsr(gl->desc_name, msr);	/* MSR - see table above */
	msr = rdmsr(gl->desc_name);
	printk(BIOS_DEBUG, "%s: AFTER write msr 0x%08lx, val 0x%08x:0x%08x\n", __func__,
				gl->desc_name, msr.hi, msr.lo);
}

static void DMMGL0Init(struct gliutable *gl)
{
	msr_t msr;
	int sizebytes = sizeram()<<20;
	long offset;

	if (! havedmi)
		return;

	printk(BIOS_DEBUG, "%s: %d bytes\n", __func__, sizebytes);

	sizebytes -= DMM_SIZE*1024;
	offset = sizebytes - DMM_OFFSET;
	printk(BIOS_DEBUG, "%s: offset is 0x%08lx\n", __func__, offset);
	offset >>= 12;
	msr.hi = (gl->hi) | (offset << 8);
	/* I don't think this is needed */
	msr.hi &= 0xffffff00;
	msr.hi |= (DMM_OFFSET >> 24);
	msr.lo = DMM_OFFSET << 8;
	msr.lo |= ((~(DMM_SIZE*1024)+1)>>12)&0xfffff;

	wrmsr(gl->desc_name, msr);	/* MSR - See table above */
	msr = rdmsr(gl->desc_name);
	printk(BIOS_DEBUG, "%s: AFTER write msr 0x%08lx, val 0x%08x:0x%08x\n", __func__, gl->desc_name, msr.hi, msr.lo);
}

static void DMMGL1Init(struct gliutable *gl)
{
	msr_t msr;

	if (! havedmi)
		return;

	printk(BIOS_DEBUG, "%s:\n", __func__ );

	msr.hi = gl->hi;
	/* I don't think this is needed */
	msr.hi &= 0xffffff00;
	msr.hi |= (DMM_OFFSET >> 24);
	msr.lo = DMM_OFFSET << 8;
	/* hmm. AMD source has SMM here ... SMM, not DMM? We think DMM */
	printk(BIOS_ERR, "%s: warning, using DMM_SIZE even though AMD used SMM_SIZE\n", __func__);
	msr.lo |= ((~(DMM_SIZE*1024)+1)>>12)&0xfffff;

	wrmsr(gl->desc_name, msr);	/* MSR - See table above */
	msr = rdmsr(gl->desc_name);
	printk(BIOS_DEBUG, "%s: AFTER write msr 0x%08lx, val 0x%08x:0x%08x\n", __func__, gl->desc_name, msr.hi, msr.lo);
}

static void SMMGL0Init(struct gliutable *gl)
{
	msr_t msr;
	int sizebytes = sizeram()<<20;
	long offset;

	sizebytes -= SMM_SIZE*1024;

	if (havedmi)
		sizebytes -= DMM_SIZE * 1024;

	printk(BIOS_DEBUG, "%s: %d bytes\n", __func__, sizebytes);

	offset = sizebytes - SMM_OFFSET;
	printk(BIOS_DEBUG, "%s: offset is 0x%08lx\n", __func__, offset);
	offset >>= 12;

	msr.hi = offset << 8;
	msr.hi |= SMM_OFFSET>>24;

	msr.lo = SMM_OFFSET << 8;
	msr.lo |= ((~(SMM_SIZE*1024)+1)>>12)&0xfffff;

	wrmsr(gl->desc_name, msr);	/* MSR - See table above */
	msr = rdmsr(gl->desc_name);
	printk(BIOS_DEBUG, "%s: AFTER write msr 0x%08lx, val 0x%08x:0x%08x\n", __func__, gl->desc_name, msr.hi, msr.lo);
}

static void SMMGL1Init(struct gliutable *gl)
{
	msr_t msr;
	printk(BIOS_DEBUG, "%s:\n", __func__ );

	msr.hi = gl->hi;
	/* I don't think this is needed */
	msr.hi &= 0xffffff00;
	msr.hi |= (SMM_OFFSET >> 24);
	msr.lo = SMM_OFFSET << 8;
	msr.lo |= ((~(SMM_SIZE*1024)+1)>>12)&0xfffff;

	wrmsr(gl->desc_name, msr);	/* MSR - See table above */
	msr = rdmsr(gl->desc_name);
	printk(BIOS_DEBUG, "%s: AFTER write msr 0x%08lx, val 0x%08x:0x%08x\n", __func__, gl->desc_name, msr.hi, msr.lo);
}

static void GLIUInit(struct gliutable *gl)
{
	while (gl->desc_type != GL_END){
		switch(gl->desc_type){
		default:
			/* For Unknown types: Write then read MSR */
			writeglmsr(gl);
		case SC_SHADOW: /* Check for a Shadow entry */
			ShadowInit(gl);
			break;

		case R_SYSMEM: /* check for a SYSMEM entry */
			SysmemInit(gl);
			break;

		case BMO_DMM: /* check for a DMM entry */
			DMMGL0Init(gl);
			break;

		case BM_DMM	: /* check for a DMM entry */
			DMMGL1Init(gl);
			break;

		case BMO_SMM	: /* check for a SMM entry */
			SMMGL0Init(gl);
			break;

		case BM_SMM	: /* check for a SMM entry */
			SMMGL1Init(gl);
			break;
		}
		gl++;
	}
}

/* Set up GLPCI settings for reads/write into memory.
 *
 * R0:  0-640KB,
 * R1:  1MB - Top of System Memory
 * R2: SMM Memory
 * R3: Framebuffer? - not set up yet
 * R4: ??
 */
static void GLPCIInit(void)
{
	struct gliutable *gl = 0;
	int i;
	msr_t msr;
	int msrnum;

	/* R0 - GLPCI settings for Conventional Memory space. */
	msr.hi =  (0x09F000 >> 12) << GLPCI_RC_UPPER_TOP_SHIFT;	/* 640 */
	msr.lo =  0;						/* 0 */
	msr.lo |= GLPCI_RC_LOWER_EN_SET+ GLPCI_RC_LOWER_PF_SET + GLPCI_RC_LOWER_WC_SET;
	msrnum = GLPCI_RC0;
	wrmsr(msrnum, msr);

	/* R1 - GLPCI settings for SysMem space. */
	/* Get systop from GLIU0 SYSTOP Descriptor */
	for(i = 0; gliu0table[i].desc_name != GL_END; i++) {
		if (gliu0table[i].desc_type == R_SYSMEM) {
			gl = &gliu0table[i];
			break;
		}
	}
	if (gl) {
		unsigned long pah, pal;
		msrnum = gl->desc_name;
		msr = rdmsr(msrnum);
		/* example R_SYSMEM value: 20:00:00:0f:fb:f0:01:00
                 * translates to a base of 0x00100000 and top of 0xffbf0000
                 * base of 1M and top of around 256M
		 */
		/* we have to create a page-aligned (4KB page) address for base and top
		 * so we need a high page aligned addresss (pah) and low page aligned address (pal)
		 * pah is from msr.hi << 12 | msr.low >> 20. pal is msr.lo << 12
		 */
		printk(BIOS_DEBUG, "GLPCI r1: system msr.lo 0x%08x msr.hi 0x%08x\n", msr.lo, msr.hi);
		pah = ((msr.hi &0xff) << 12) | ((msr.lo >> 20) & 0xfff);
		/* we have the page address. Now make it a page-aligned address */
		pah <<= 12;

		pal = msr.lo << 12;
		msr.hi =  pah;
		msr.lo =  pal;
		msr.lo |= GLPCI_RC_LOWER_EN_SET | GLPCI_RC_LOWER_PF_SET | GLPCI_RC_LOWER_WC_SET;
		printk(BIOS_DEBUG, "GLPCI r1: system msr.lo 0x%08x msr.hi 0x%08x\n", msr.lo, msr.hi);
		msrnum = GLPCI_RC1;
		wrmsr(msrnum, msr);
	}

	/* R2 - GLPCI settings for SMM space. */
	msr.hi =  ((SMM_OFFSET+(SMM_SIZE*1024-1)) >> 12) << GLPCI_RC_UPPER_TOP_SHIFT;
	msr.lo =  (SMM_OFFSET >> 12) << GLPCI_RC_LOWER_BASE_SHIFT;
	msr.lo |= GLPCI_RC_LOWER_EN_SET | GLPCI_RC_LOWER_PF_SET;
	msrnum = GLPCI_RC2;
	wrmsr(msrnum, msr);

	/* this is done elsewhere already, but it does no harm to do it more than once */
	/* write serialize memory hole to PCI. Need to to unWS when something is shadowed regardless of cachablility. */
	msr.lo =  0x021212121;		/* cache disabled and write serialized */
	msr.hi =  0x021212121;		/* cache disabled and write serialized */

	msrnum = CPU_RCONF_A0_BF;
	wrmsr(msrnum, msr);

	msrnum = CPU_RCONF_C0_DF;
	wrmsr(msrnum, msr);

	msrnum = CPU_RCONF_E0_FF;
	wrmsr(msrnum, msr);

	/* Set Non-Cacheable Read Only for NorthBound Transactions to Memory. The Enable bit is handled in the Shadow setup. */
	msrnum = GLPCI_A0_BF;
	msr.hi =  0x35353535;
	msr.lo =  0x35353535;
	wrmsr(msrnum, msr);

	msrnum = GLPCI_C0_DF;
	msr.hi =  0x35353535;
	msr.lo =  0x35353535;
	wrmsr(msrnum, msr);

	msrnum = GLPCI_E0_FF;
	msr.hi =  0x35353535;
	msr.lo =  0x35353535;
	wrmsr(msrnum, msr);

	/* Set WSREQ */
	msrnum = CPU_DM_CONFIG0;
	msr = rdmsr(msrnum);
	msr.hi &= ~ (7 << DM_CONFIG0_UPPER_WSREQ_SHIFT);
	msr.hi |= 2 << DM_CONFIG0_UPPER_WSREQ_SHIFT	;	/* reduce to 1 for safe mode. */
	wrmsr(msrnum, msr);

	/* we are ignoring the 5530 case for now, and perhaps forever. */

	/* 5535 NB Init */
	msrnum = GLPCI_ARB;
	msr = rdmsr(msrnum);
	msr.hi |=  GLPCI_ARB_UPPER_PRE0_SET | GLPCI_ARB_UPPER_PRE1_SET;
	msr.lo |=  GLPCI_ARB_LOWER_IIE_SET;
	wrmsr(msrnum, msr);

	msrnum = GLPCI_CTRL;
	msr = rdmsr(msrnum);

	msr.lo |=  GLPCI_CTRL_LOWER_ME_SET | GLPCI_CTRL_LOWER_OWC_SET | GLPCI_CTRL_LOWER_PCD_SET; 	/* (Out will be disabled in CPUBUG649 for < 2.0 parts .) */
	msr.lo |=  GLPCI_CTRL_LOWER_LDE_SET;

	msr.lo &=  ~ (0x03 << GLPCI_CTRL_LOWER_IRFC_SHIFT);
	msr.lo |=  0x02 << GLPCI_CTRL_LOWER_IRFC_SHIFT;

	msr.lo &=  ~ (0x07 << GLPCI_CTRL_LOWER_IRFT_SHIFT);
	msr.lo |=  0x06 << GLPCI_CTRL_LOWER_IRFT_SHIFT;

	msr.hi &=  ~ (0x0f << GLPCI_CTRL_UPPER_FTH_SHIFT);
	msr.hi |=  0x0F << GLPCI_CTRL_UPPER_FTH_SHIFT;

	msr.hi &=  ~ (0x0f << GLPCI_CTRL_UPPER_RTH_SHIFT);
	msr.hi |=  0x0F << GLPCI_CTRL_UPPER_RTH_SHIFT;

	msr.hi &=  ~ (0x0f << GLPCI_CTRL_UPPER_SBRTH_SHIFT);
	msr.hi |=  0x0F << GLPCI_CTRL_UPPER_SBRTH_SHIFT;

	msr.hi &=  ~ (0x03 << GLPCI_CTRL_UPPER_WTO_SHIFT);
	msr.hi |=  0x06 << GLPCI_CTRL_UPPER_WTO_SHIFT;

	msr.hi &=  ~ (0x03 << GLPCI_CTRL_UPPER_ILTO_SHIFT);
	msr.hi |=  0x00 << GLPCI_CTRL_UPPER_ILTO_SHIFT;
	wrmsr(msrnum, msr);

	/* Set GLPCI Latency Timer. */
	msrnum = GLPCI_CTRL;
	msr = rdmsr(msrnum);
	msr.hi |=  0x1F << GLPCI_CTRL_UPPER_LAT_SHIFT; 	/* Change once 1.x is gone. */
	wrmsr(msrnum, msr);

	/* GLPCI_SPARE */
	msrnum = GLPCI_SPARE;
	msr = rdmsr(msrnum);
	msr.lo &=  ~ 0x7;
	msr.lo |=  GLPCI_SPARE_LOWER_AILTO_SET | GLPCI_SPARE_LOWER_PPD_SET | GLPCI_SPARE_LOWER_PPC_SET | GLPCI_SPARE_LOWER_MPC_SET | GLPCI_SPARE_LOWER_NSE_SET | GLPCI_SPARE_LOWER_SUPO_SET;
	wrmsr(msrnum, msr);
}

/* Enable Clock Gating. */
static void ClockGatingInit (void)
{
	msr_t msr;
	struct msrinit *gating = ClockGatingDefault;
	int i;

#if 0
	mov	cx, TOKEN_CLK_GATE
	NOSTACK	bx, GetNVRAMValueBX
	cmp	al, TVALUE_CG_OFF
	je	gatingdone

	cmp	al, TVALUE_CG_DEFAULT
	jb	allon
	ja	performance
	lea	si, ClockGatingDefault
	jmp	nextdevice

allon:
	lea	si, ClockGatingAllOn
	jmp	nextdevice

performance:
	lea	si, ClockGatingPerformance
#endif

	for(i = 0; gating->msrnum != 0xffffffff; i++) {
		msr = rdmsr(gating->msrnum);
		printk(BIOS_DEBUG, "%s: MSR 0x%08lx is 0x%08x:0x%08x\n", __func__, gating->msrnum, msr.hi, msr.lo);
		msr.hi |= gating->msr.hi;
		msr.lo |= gating->msr.lo;
		printk(BIOS_DEBUG, "%s: MSR 0x%08lx will be set to  0x%08x:0x%08x\n", __func__,
			gating->msrnum, msr.hi, msr.lo);
		wrmsr(gating->msrnum, msr);	/* MSR - See the table above */
		gating +=1;
	}
}

static void GeodeLinkPriority(void)
{
	msr_t msr;
	struct msrinit *prio = GeodeLinkPriorityTable;
	int i;

	for(i = 0; prio->msrnum != 0xffffffff; i++) {
		msr = rdmsr(prio->msrnum);
		printk(BIOS_DEBUG, "%s: MSR 0x%08lx is 0x%08x:0x%08x\n", __func__, prio->msrnum, msr.hi, msr.lo);
		msr.hi |= prio->msr.hi;
		msr.lo &= ~0xfff;
		msr.lo |= prio->msr.lo;
		printk(BIOS_DEBUG, "%s: MSR 0x%08lx will be set to 0x%08x:0x%08x\n", __func__,
			prio->msrnum, msr.hi, msr.lo);
		wrmsr(prio->msrnum, msr);	/* MSR - See the table above */
		prio +=1;
	}
}

/* Get the GLIU0 shadow register settings.
 *
 * If the setShadow function is used then all shadow descriptors
 * will stay sync'ed.
 */
static uint64_t getShadow(void)
{
	msr_t msr;
	msr = rdmsr(MSR_GLIU0_SHADOW);
	return ( ( (uint64_t) msr.hi ) << 32 ) | msr.lo;
}

/* Set the cache RConf registers for the memory hole.
 *
 * Keeps all cache shadow descriptors sync'ed.
 * This is part of the PCI lockup solution.
 *
 * Entry: EDX:EAX is the shadow settings.
 */
static void setShadowRCONF(uint32_t shadowHi, uint32_t shadowLo)
{
	/* ok this is whacky bit translation time. */
	int bit;
	uint8_t shadowByte;
	msr_t msr;
	shadowByte = (uint8_t) (shadowLo >> 16);

	/* load up D000 settings in edx. */
	for (bit = 8; (bit > 4); bit--) {
		msr.hi <<= 8;
		msr.hi |= 1;			/* cache disable PCI/Shadow memory */
		if (shadowByte && (1 << bit))
			msr.hi |= 0x20;		/* write serialize PCI memory */
	}

	/* load up C000 settings in eax. */
	for ( ; bit; bit--) {
		msr.lo <<= 8;
		msr.lo |= 1;			/* cache disable PCI/Shadow memory */
		if (shadowByte && (1 << bit))
			msr.lo |= 0x20;		/* write serialize PCI memory */
	}

	wrmsr(CPU_RCONF_C0_DF, msr);

	shadowByte = (uint8_t) (shadowLo >> 24);

	/* load up F000 settings in edx. */
	for (bit = 8; (bit > 4); bit--) {
		msr.hi <<= 8;
		msr.hi |= 1;			/* cache disable PCI/Shadow memory */
		if (shadowByte && (1 << bit))
			msr.hi |= 0x20;		/* write serialize PCI memory */
	}

	/* load up E000 settings in eax. */
	for ( ; bit; bit--) {
		msr.lo <<= 8;
		msr.lo |= 1;			/* cache disable PCI/Shadow memory */
		if (shadowByte && (1 << bit))
			msr.lo |= 0x20;		/* write serialize PCI memory */
	}

	wrmsr(CPU_RCONF_E0_FF, msr);
}

/* Set the GLPCI registers for the memory hole.
 * Keeps all cache shadow descriptors sync'ed.
 * Entry: EDX:EAX is the shadow settings
 */
static void setShadowGLPCI(uint32_t shadowHi, uint32_t shadowLo)
{
	msr_t msr;

	/* Set the Enable Register. */
	msr = rdmsr(GLPCI_REN);
	msr.lo &= 0xFFFF00FF;
	msr.lo |= ( (shadowLo & 0xFFFF0000) >> 8);
	wrmsr(GLPCI_REN, msr);
}

/* Set the GLIU SC register settings. Scans descriptor tables for SC_SHADOW.
 * Keeps all shadow descriptors sync'ed.
 * Entry: EDX:EAX is the shadow settings
 */
static void setShadow(uint64_t shadowSettings)
{
	int i;
	msr_t msr;
	struct gliutable* pTable;
	uint32_t shadowLo, shadowHi;

	shadowLo = (uint32_t) shadowSettings;
	shadowHi = (uint32_t) (shadowSettings >> 32);

	setShadowRCONF(shadowHi, shadowLo);
	setShadowGLPCI(shadowHi, shadowLo);

	for(i = 0; gliutables[i]; i++) {
		for (pTable = gliutables[i]; pTable->desc_type != GL_END; pTable++) {
			if (pTable->desc_type == SC_SHADOW) {

				msr = rdmsr(pTable->desc_name);
				msr.lo = (uint32_t) shadowSettings;
				msr.hi &= 0xFFFF0000;		/* maintain PDID in upper EDX */
				msr.hi |= ((uint32_t) (shadowSettings >> 32)) & 0x0000FFFF;
				wrmsr(pTable->desc_name, msr);	/* MSR - See the table above */

			}
		}
	}
}

/* Set up a stack for ease of further testing. */
static void shadowRom(void)
{
	uint64_t shadowSettings = getShadow();
	shadowSettings &= (uint64_t) 0xFFFF00000000FFFFULL;	/* Disable read & writes */
	shadowSettings |= (uint64_t) 0x0000FFFFFFFF0000ULL;	/* Enable reads for C0000-FFFFF */
	setShadow(shadowSettings);
}

/* Set up RCONF_DEFAULT and any other RCONF registers needed.
 *
 * DEVRC_RCONF_DEFAULT:
 * ROMRC(63:56) = 04h	 ; write protect ROMBASE
 * ROMBASE(36:55) = 0FFFC0h ; Top of PCI/bottom of rom chipselect area
 * DEVRC(35:28) =  39h	 ; cache disabled in PCI memory + WS bit on + Write Combine + write burst.
 * SYSTOP(27:8) = top of system memory
 * SYSRC(7:0) = 00h 		 ; writeback, can set to 08h to make writethrough
 */
#define SYSMEM_RCONF_WRITETHROUGH 8
#define DEVRC_RCONF_DEFAULT 0x21
#define ROMBASE_RCONF_DEFAULT 0xFFFC0000
#define ROMRC_RCONF_DEFAULT 0x25

static void RCONFInit(void)
{
	struct gliutable *gl = 0;
	int i;
	msr_t msr;
	uint8_t SysMemCacheProp;

	/* Locate SYSMEM entry in GLIU0table */
	for(i = 0; gliu0table[i].desc_name != GL_END; i++) {
		if (gliu0table[i].desc_type == R_SYSMEM) {
			gl = &gliu0table[i];
			break;
		}
	}
	if (gl == 0) {
		post_code(0xCE);	/* POST_RCONFInitError */
		while (1);
	}

/* sysdescfound: */
	/* found the descriptor... get its contents */
	msr = rdmsr(gl->desc_name);

	/* 20 bit address -  The bottom 12 bits go into bits 20-31 in eax, the
	 * top 8 bits go into 0-7 of edx.
	 */
	msr.lo = (msr.lo & 0xFFFFFF00) | (msr.hi & 0xFF);
	msr.lo = ((msr.lo << 12) | (msr.lo >> 20)) & 0x000FFFFF;
	msr.lo <<= RCONF_DEFAULT_LOWER_SYSTOP_SHIFT;	/* 8 */

	/* Set Default SYSMEM region properties */
	msr.lo &= ~SYSMEM_RCONF_WRITETHROUGH;	/* 8 (or ~8) */

	/* Set PCI space cache properties */
	msr.hi = (DEVRC_RCONF_DEFAULT >> 4);	/* only need the bottom bits and lets clean the rest of edx */
	msr.lo |= (DEVRC_RCONF_DEFAULT << 28);

	/* Set the ROMBASE. This is usually FFFC0000h */
	msr.hi |= (ROMBASE_RCONF_DEFAULT >> 12) << RCONF_DEFAULT_UPPER_ROMBASE_SHIFT;

	/* Set ROMBASE cache properties. */
	msr.hi |= ((ROMRC_RCONF_DEFAULT >> 8) | (ROMRC_RCONF_DEFAULT << 24));

	/* now program RCONF_DEFAULT */
	wrmsr(CPU_RCONF_DEFAULT, msr);

	/* RCONF_BYPASS: Cache tablewalk properties and SMM/DMM header access properties. */
	/* Set to match system memory cache properties. */
	msr = rdmsr(CPU_RCONF_DEFAULT);
	SysMemCacheProp = (uint8_t) (msr.lo & 0xFF);
	msr = rdmsr(CPU_RCONF_BYPASS);
	msr.lo = (msr.lo & 0xFFFF0000) | (SysMemCacheProp << 8) | SysMemCacheProp;
	wrmsr(CPU_RCONF_BYPASS, msr);
}

/* Core Logic initialization: Host bridge. */
void northbridgeinit(void)
{
	msr_t msr;
	int i;
	printk(BIOS_DEBUG, "Enter %s\n", __func__);

	for(i = 0; gliutables[i]; i++)
		GLIUInit(gliutables[i]);

	GeodeLinkPriority();

	shadowRom();

	RCONFInit();

	/* The cacheInit function in GeodeROM tests cache and, among other things,
	 * makes sure all INVD instructions are treated as WBINVD.  We do this
	 * because we've found some programs which require this behavior.
	 * That subset of cacheInit() is implemented here:
	 */
	msr = rdmsr(CPU_DM_CONFIG0);
	msr.lo |= DM_CONFIG0_LOWER_WBINVD_SET;
	wrmsr(CPU_DM_CONFIG0, msr);

	/* Now that the descriptor to memory is set up. */
	/* The memory controller needs one read to synch its lines before it can be used. */
	i = *(int *) 0;

	GLPCIInit();
	ClockGatingInit();
	__asm__("FINIT\n");
	/* CPUBugsFix -- called elsewhere */
	printk(BIOS_DEBUG, "Exit %s\n", __func__);
}

