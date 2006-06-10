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


/* the structs in this file only set msr.lo. But ... that may not always be true */

struct msrinit {
	unsigned long msrnum;
	msr_t msr;
};

/*  Master Configuration Register for Bus Masters.*/
struct msrinit SB_MASTER_CONF_TABLE[] = {
	{USB1_SB_GLD_MSR_CONF,	{.hi=0,.lo=0x00008f000}},	/*  NOTE: Must be 1st entry in table*/
	{USB2_SB_GLD_MSR_CONF,	{.hi=0,.lo=0x00008f000}},
	{ATA_SB_GLD_MSR_CONF,	{.hi=0,.lo=0x00048f000}},
	{AC97_SB_GLD_MSR_CONF,	{.hi=0,.lo=0x00008f000}},
	{MDD_SB_GLD_MSR_CONF,	{.hi=0,.lo=0x00000f000}},
/* GLPCI_SB_GLD_MSR_CONF,	0x0FFFFFFFF*/
/* GLCP_SB_GLD_MSR_CONF,	0x0FFFFFFFF*/
/* GLIU_SB_GLD_MSR_CONF,	0x0*/
	{0,{0,0}}
};

/*  5535_A3 Clock Gating*/
struct msrinit CS5535_CLOCK_GATING_TABLE[] = {
	{      USB1_SB_GLD_MSR_PM,	{.hi=0,.lo=0x000000005}},
	{      USB2_SB_GLD_MSR_PM,	{.hi=0,.lo=0x000000005}},
	{      GLIU_SB_GLD_MSR_PM,	{.hi=0,.lo=0x000000004}},
	{      GLPCI_SB_GLD_MSR_PM,	{.hi=0,.lo=0x000000005}},
	{      GLCP_SB_GLD_MSR_PM,	{.hi=0,.lo=0x000000004}},
	{      MDD_SB_GLD_MSR_PM,	{.hi=0,.lo=0x050554111}},
	{      ATA_SB_GLD_MSR_PM,	{.hi=0,.lo=0x000000005}},
	{	AC97_SB_GLD_MSR_PM,	{.hi=0,.lo=0x000000005}},
	{0,{0,0}}
};

/*  5536 Clock Gating*/
struct msrinit CS5536_CLOCK_GATING_TABLE[] = {
/* MSR		  Setting*/
	{      GLIU_SB_GLD_MSR_PM,	{.hi=0,.lo=0x000000004}},
	{      GLPCI_SB_GLD_MSR_PM,	{.hi=0,.lo=0x000000005}},
	{      GLCP_SB_GLD_MSR_PM,	{.hi=0,.lo=0x000000004}},
	{      MDD_SB_GLD_MSR_PM,	{.hi=0,.lo=0x050554111}}, /*  SMBus clock gating errata (PBZ 2226 & SiBZ 3977)*/
	{      ATA_SB_GLD_MSR_PM,	{.hi=0,.lo=0x000000005}},
	{      AC97_SB_GLD_MSR_PM,	{.hi=0,.lo=0x000000005}},
	{0,{0,0}}
};

struct acpiinit {
	unsigned short ioreg; 
	unsigned long regdata;
	unsigned short iolen;
};

struct acpiinit acpi_init_table[] = {
	{ACPI_BASE+0x00, 0x01000000, 4},
	{ACPI_BASE+0x08, 0, 4},
	{ACPI_BASE+0x0C, 0, 4},
	{ACPI_BASE+0x1C, 0, 4},
	{ACPI_BASE+0x18, 0x0FFFFFFFF, 4},
	{ACPI_BASE+0x00, 0x0000FFFF, 4},

	{PM_SCLK, 0x000000E00, 4},
	{PM_SED,  0x000004601, 4},
	{PM_SIDD, 0x000008C02, 4},
	{PM_WKD,  0x0000000A0, 4},
	{PM_WKXD, 0x0000000A0, 4},
	{0,0,0}
};

/* return 1 if we are a 5536-based system */
static int is_5536(void){
	msr_t msr;
	msr = rdmsr(GLIU_SB_GLD_MSR_CAP);
	msr.lo >>= 20;
	printk_debug("is_5536: msr.lo is 0x%x(==5 means 5536)\n", msr.lo&0xf);
	return ((msr.lo&0xf) == 5);
}
/* ***************************************************************************/
/* **/
/* *	pmChipsetInit*/
/* **/
/* *	Program ACPI LBAR and initialize ACPI registers.*/
/* *  */
/* **/
/* *	Entry:*/
/* *		None*/
/* **/
/* *	Exit:*/
/* *		None*/
/* **/
/* *	Destroys:*/
/* *		None*/
/* **/
/* ***************************************************************************/
static void
pmChipsetInit(void) {
	unsigned long val = 0;
	unsigned short port;

	port =  (PMLogic_BASE + 0x010);
	val =  0x0E00		; /*  1ms*/
	outl(val, port);

	/*  PM_WKXD*/
	/*  Make sure bits[3:0]=0000b to clear the*/
	/*  saved Sx state*/
	port =  (PMLogic_BASE + 0x034);
	val =  0x0A0		; /*  5ms*/
	outl(val, port);
	
	/*  PM_WKD*/
	port =  (PMLogic_BASE + 0x030);
	outl(val, port);
		
	/*  PM_SED*/
	port =  (PMLogic_BASE + 0x014);
/* 	mov		eax, 0x057642	; 100ms, works*/
	val =  0x04601		; /*  5ms*/
	outl(val, port);
	
	/*  PM_SIDD*/
	port =  (PMLogic_BASE + 0x020);
/* 	mov		eax, 0x0AEC84	; 200ms, works*/
	val =  0x08C02		; /*  10ms*/
	outl(val, port);
	
	/*  GPIO24 OUT_AUX1 function is the external signal for 5535's vsb_working_aux*/
	/*  which is de-asserted when 5535 enters Standby(S3 or S5) state.*/
	/*  On Hawk, GPIO24 controls all voltage rails except Vmem and Vstandby.  This means*/
	/*  GX2 will be fully de-powered if this control de-asserts in S3/S5.*/
	/* */
	/*  GPIO24 is setup in preChipsetInit for two reasons*/
	/*  1. GPIO24 at reset defaults to disabled, since this signal is vsb_work_aux on*/
	/*     Hawk it controls the FET's for all voltage rails except Vstanby & Vmem.*/
	/*     BIOS needs to enable GPIO24 as OUT_AUX1 & OUTPUT_EN early so it is driven*/
	/*     by 5535.*/
	/*  2. Non-PM builds will require GPIO24 enabled for instant-off power button*/
	/* */

	/*  GPIO11 OUT_AUX1 function is the external signal for 5535's slp_clk_n which is asserted*/
	/*  when 5535 enters Sleep(S1) state.*/
	/*  On Hawk, GPIO11 is connected to control input of external clock generator*/
	/*  for 14MHz, PCI, USB & LPC clocks.*/
	/*  Programming of GPIO11 will be done by VSA PM code.  During VSA Init. BIOS writes*/
	/*  PM Core Virual Register indicating if S1 Clocks should be On or Off. This is based*/
	/*  on a Setup item.  We do not want to leave GPIO11 enabled because of a Hawk board*/
	/*  problem.  With GPIO11 enabled in S3, something is back-driving GPIO11 causing it to*/
	/*  float to 1.6-1.7V.*/

}


/* ***************************************************************************/
/* **/
/* *	ChipsetGeodeLinkInit*/
/* *	Handle chipset specific GeodeLink settings here. */
/* *	Called from GeodeLink init code.*/
/* **/
/* *	Entry:*/
/* *	Exit:*/
/* *	Destroys: GS*/
/* **/
/* ***************************************************************************/
static void 
ChipsetGeodeLinkInit(void){
	msr_t msr;
	unsigned long msrnum;
	unsigned long totalmem;

	if (is_5536())
		return;
	/*  SWASIF for A1 DMA */
	/*  Set all memory to  "just above systop" PCI so DMA will work*/
	/*  check A1*/
	msrnum = MSR_SB_GLCP + 0x17;
	msr = rdmsr(msrnum);
	if ((msr.lo&0xff) == 0x11)
		return;

	totalmem = sizeram() << 20 - 1;
	totalmem >>= 12; 
	totalmem = ~totalmem;
	totalmem &= 0xfffff;
	msr.lo = totalmem;
	msr.hi = 0x20000000;				/*  Port 1 (PCI)*/
	msrnum = MSR_SB_GLIU + 0x20;		/*  */;
	wrmsr(msrnum, msr);
}

void
chipsetinit (void){
	msr_t msr;
	struct msrinit *csi;
	int i;
	unsigned long msrnum;

	outb( P80_CHIPSET_INIT, 0x80);
	ChipsetGeodeLinkInit();
#if 0
	/* we hope NEVER to be in linuxbios when S3 resumes 
	if (! IsS3Resume()) */
	{
		struct acpiinit *aci = acpi_init_table;
		while (aci->ioreg){
			if (aci->iolen == 2) {
				outw(aci->regdata, aci->ioreg);
				inw(aci->ioreg);
			} else {
				outl(aci->regdata, aci->ioreg);
				inl(aci->ioreg);
			}
		}

		pmChipsetInit();
	}
#endif


	if (!is_5536()) {
		/*  Setup USB. Need more details. #118.18*/
		msrnum = MSR_SB_USB1 + 8;
		msr.lo =  0x00012090;
		msr.hi = 0;
		wrmsr(msrnum, msr);
		msrnum = MSR_SB_USB2 + 8;
		wrmsr(msrnum, msr);
	}
	
	/* set hd IRQ */
	outl	(GPIOL_2_SET, GPIOL_INPUT_ENABLE);
	outl	(GPIOL_2_SET, GPIOL_IN_AUX1_SELECT);

	/*  Allow IO read and writes during a ATA DMA operation.*/
	/*   This could be done in the HD rom but do it here for easier debugging.*/
	
	msrnum = ATA_SB_GLD_MSR_ERR;
	msr = rdmsr(msrnum);
	msr.lo &= ~0x100;
	wrmsr(msrnum, msr);

	/*  Enable Post Primary IDE.*/
	msrnum = GLPCI_SB_CTRL;
	msr = rdmsr(msrnum);
	msr.lo |=  GLPCI_CRTL_PPIDE_SET;
	wrmsr(msrnum, msr);


	/*  Set up Master Configuration Register*/
	/*  If 5536, use same master config settings as 5535, except for OHCI MSRs*/
	if (is_5536()) 
		i = 2;
	else
		i = 0;

	csi = &SB_MASTER_CONF_TABLE[i];
	for(; csi->msrnum; csi++){
		msr.lo = csi->msr.lo;
		msr.hi = csi->msr.hi;
		wrmsr(csi->msrnum, msr); // MSR - see table above
	}


	/*  Flash Setup*/
	printk_err("NOT DOING ChipsetFlashSetup()!!!!!!!!!!!!!!!!!!\n");
//	ChipsetFlashSetup();



	/* */
	/*  Set up Hardware Clock Gating*/
	/* */
	/* if (getnvram(TOKEN_SB_CLK_GATE) != TVALUE_DISABLE) */
	{
		if (is_5536())
			csi = CS5536_CLOCK_GATING_TABLE;
		else
			csi = CS5535_CLOCK_GATING_TABLE;

		for(; csi->msrnum; csi++){
			msr.lo = csi->msr.lo;
			msr.hi = csi->msr.hi;
			wrmsr(csi->msrnum, msr);	// MSR - see table above
		}
	}

}
