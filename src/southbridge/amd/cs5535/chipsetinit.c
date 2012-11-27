#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include "chip.h"
#include "northbridge/amd/gx2/northbridge.h"
#include <cpu/amd/gx2def.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/cache.h>
#include "southbridge/amd/cs5535/cs5535.h"

/* the structs in this file only set msr.lo. But ... that may not always be true */

struct msrinit {
	unsigned long msrnum;
	msr_t msr;
};

/*  Master Configuration Register for Bus Masters. */
static struct msrinit SB_MASTER_CONF_TABLE[] = {
	{ USB1_SB_GLD_MSR_CONF,	{.hi=0,.lo=0x00008f000} },	/*  NOTE: Must be 1st entry in table */
	{ USB2_SB_GLD_MSR_CONF,	{.hi=0,.lo=0x00008f000} },
	{ ATA_SB_GLD_MSR_CONF,	{.hi=0,.lo=0x00048f000} },
	{ AC97_SB_GLD_MSR_CONF,	{.hi=0,.lo=0x00008f000} },
	{ MDD_SB_GLD_MSR_CONF,	{.hi=0,.lo=0x00000f000} },
/* GLPCI_SB_GLD_MSR_CONF,	0x0FFFFFFFF*/
/* GLCP_SB_GLD_MSR_CONF,	0x0FFFFFFFF*/
/* GLIU_SB_GLD_MSR_CONF,	0x0*/
	{0,{0,0}}
};

/*  5535_A3 Clock Gating*/
static struct msrinit CS5535_CLOCK_GATING_TABLE[] = {
	{ USB1_SB_GLD_MSR_PM,	{.hi=0, .lo=0x000000005} },
	{ USB2_SB_GLD_MSR_PM,	{.hi=0, .lo=0x000000005} },
	{ GLIU_SB_GLD_MSR_PM,	{.hi=0, .lo=0x000000004} },
	{ GLPCI_SB_GLD_MSR_PM,	{.hi=0, .lo=0x000000005} },
	{ GLCP_SB_GLD_MSR_PM,	{.hi=0, .lo=0x000000004} },
	{ MDD_SB_GLD_MSR_PM,	{.hi=0, .lo=0x050554111} },
	{ ATA_SB_GLD_MSR_PM,	{.hi=0, .lo=0x000000005} },
	{ AC97_SB_GLD_MSR_PM,	{.hi=0, .lo=0x000000005} },
	{ 0,			{.hi=0, .lo=0x000000000} }
};

#ifdef UNUSED_CODE
struct acpiinit {
	unsigned short ioreg;
	unsigned long regdata;
	unsigned short iolen;
};

static struct acpiinit acpi_init_table[] = {
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

/*****************************************************************************
 *
 *	pmChipsetInit
 *
 *	Program ACPI LBAR and initialize ACPI registers.
 *
 *****************************************************************************/
static void pmChipsetInit(void)
{
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
	val =  0x04601		; /*  5ms*/
	outl(val, port);

	/*  PM_SIDD*/
	port =  (PMLogic_BASE + 0x020);
	val =  0x08C02		; /*  10ms*/
	outl(val, port);

	/* GPIO24 OUT_AUX1 function is the external signal for 5535's
	 * vsb_working_aux which is de-asserted when 5535 enters Standby (S3 or
	 * S5) state.  On Hawk, GPIO24 controls all voltage rails except Vmem
	 * and Vstandby.  This means GX2 will be fully de-powered if this
	 * control de-asserts in S3/S5.
	 */

	/* GPIO24 is setup in preChipsetInit for two reasons
	 * 1. GPIO24 at reset defaults to disabled, since this signal is
	 *    vsb_work_aux on Hawk it controls the FET's for all voltage
	 *    rails except Vstanby & Vmem.  BIOS needs to enable GPIO24 as
	 *    OUT_AUX1 & OUTPUT_EN early so it is driven by 5535.
	 * 2. Non-PM builds will require GPIO24 enabled for instant-off power
	 *    button
	 */

	/* GPIO11 OUT_AUX1 function is the external signal for 5535's
	 * slp_clk_n which is asserted when 5535 enters Sleep(S1) state.
	 * On Hawk, GPIO11 is connected to control input of external clock
	 * generator for 14MHz, PCI, USB & LPC clocks.
	 * Programming of GPIO11 will be done by VSA PM code.  During VSA
	 * Init. BIOS writes PM Core Virual Register indicating if S1 Clocks
	 * should be On or Off. This is based on a Setup item.  We do not want
	 * to leave GPIO11 enabled because of a Hawk board problem.  With
	 * GPIO11 enabled in S3, something is back-driving GPIO11 causing it
	 * to float to 1.6-1.7V.
	 */
}
#endif

struct FLASH_DEVICE {
	unsigned char fType;		/* Flash type: NOR or NAND */
	unsigned char fInterface;	/* Flash interface: I/O or Memory */
	unsigned long fMask;		/* Flash size/mask */
};

static struct FLASH_DEVICE FlashInitTable[] = {
	{ FLASH_TYPE_NAND, FLASH_IF_MEM, FLASH_MEM_4K },	/* CS0, or Flash Device 0 */
	{ FLASH_TYPE_NONE, 0, 0 },	/* CS1, or Flash Device 1 */
	{ FLASH_TYPE_NONE, 0, 0 },	/* CS2, or Flash Device 2 */
	{ FLASH_TYPE_NONE, 0, 0 },	/* CS3, or Flash Device 3 */
};

#define FlashInitTableLen (ARRAY_SIZE(FlashInitTable))

static uint32_t FlashPort[] = {
	MDD_LBAR_FLSH0,
	MDD_LBAR_FLSH1,
	MDD_LBAR_FLSH2,
	MDD_LBAR_FLSH3
};

/***************************************************************************
 *
 *	ChipsetFlashSetup
 *
 *	Flash LBARs need to be setup before VSA init so the PCI BARs have
 *	correct size info.  Call this routine only if flash needs to be
 *	configured (don't call it if you want IDE).
 *
 **************************************************************************/
static void ChipsetFlashSetup(void)
{
	msr_t msr;
	int i;
	int numEnabled = 0;

	printk(BIOS_DEBUG, "ChipsetFlashSetup++\n");
	for (i = 0; i < FlashInitTableLen; i++) {
		if (FlashInitTable[i].fType != FLASH_TYPE_NONE) {
			printk(BIOS_DEBUG, "Enable CS%d\n", i);
			/* we need to configure the memory/IO mask */
			msr = rdmsr(FlashPort[i]);
			msr.hi = 0;	/* start with the "enabled" bit clear */
			if (FlashInitTable[i].fType == FLASH_TYPE_NAND)
				msr.hi |= 0x00000002;
			else
				msr.hi &= ~0x00000002;
			if (FlashInitTable[i].fInterface == FLASH_IF_MEM)
				msr.hi |= 0x00000004;
			else
				msr.hi &= ~0x00000004;
			msr.hi |= FlashInitTable[i].fMask;
			printk(BIOS_DEBUG, "WRMSR(0x%08X, %08X_%08X)\n", FlashPort[i], msr.hi, msr.lo);
			wrmsr(FlashPort[i], msr);

			/* now write-enable the device */
			msr = rdmsr(MDD_NORF_CNTRL);
			msr.lo |= (1 << i);
			printk(BIOS_DEBUG, "WRMSR(0x%08X, %08X_%08X)\n", MDD_NORF_CNTRL, msr.hi, msr.lo);
			wrmsr(MDD_NORF_CNTRL, msr);

			/* update the number enabled */
			numEnabled++;
		}
	}

	/* enable the flash */
	if (0 != numEnabled) {
		msr = rdmsr(MDD_PIN_OPT);
		msr.lo &= ~1; /* PIN_OPT_IDE */
		printk(BIOS_DEBUG, "WRMSR(0x%08X, %08X_%08X)\n", MDD_PIN_OPT, msr.hi, msr.lo);
		wrmsr(MDD_PIN_OPT, msr);
	}

	printk(BIOS_DEBUG, "ChipsetFlashSetup--\n");
}



/****************************************************************************
 *
 * 	ChipsetGeodeLinkInit
 *
 * 	Handle chipset specific GeodeLink settings here.
 * 	Called from GeodeLink init code.
 *
 ****************************************************************************/
static void
ChipsetGeodeLinkInit(void)
{
	msr_t msr;
	unsigned long msrnum;
	unsigned long totalmem;

	/*  SWASIF for A1 DMA */
	/*  Set all memory to  "just above systop" PCI so DMA will work */

	/*  check A1 */
	msrnum = MSR_SB_GLCP + 0x17;
	msr = rdmsr(msrnum);
	if ((msr.lo&0xff) == 0x11)
		return;

	totalmem = (sizeram() << 20) - 1; // highest address
	totalmem >>= 12;
	totalmem = ~totalmem;
	totalmem &= 0xfffff;
	msr.lo = totalmem;
	msr.hi = 0x20000000;		/*  Port 1 (PCI) */
	msrnum = MSR_SB_GLIU + 0x20;
	wrmsr(msrnum, msr);
}

void
chipsetinit(void)
{
	device_t dev;
	struct southbridge_amd_cs5535_config *sb;
	msr_t msr;
	struct msrinit *csi;
	int i;
	unsigned long msrnum;

	dev = dev_find_device(PCI_VENDOR_ID_AMD,
			PCI_DEVICE_ID_NS_CS5535_ISA, 0);

	if (!dev) {
		printk(BIOS_ERR, "CS5535 not found.\n");
		return;
	}

	sb = (struct southbridge_amd_cs5535_config *)dev->chip_info;

	if (!sb) {
		printk(BIOS_ERR, "CS5535 configuration not found.\n");
		return;
	}

	post_code(P80_CHIPSET_INIT);
	ChipsetGeodeLinkInit();

#ifdef UNUSED_CODE
	/* we hope NEVER to be in coreboot when S3 resumes
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

	/*  Setup USB. Need more details. #118.18 */
	msrnum = MSR_SB_USB1 + 8;
	msr.lo =  0x00012090;
	msr.hi = 0;
	wrmsr(msrnum, msr);
	msrnum = MSR_SB_USB2 + 8;
	wrmsr(msrnum, msr);

	/* set hd IRQ */
	outl	(GPIOL_2_SET, GPIOL_INPUT_ENABLE);
	outl	(GPIOL_2_SET, GPIOL_IN_AUX1_SELECT);

	/*  Allow IO read and writes during a ATA DMA operation. */
	/*   This could be done in the HD rom but do it here for easier debugging. */

	msrnum = ATA_SB_GLD_MSR_ERR;
	msr = rdmsr(msrnum);
	msr.lo &= ~0x100;
	wrmsr(msrnum, msr);

	/*  Enable Post Primary IDE. */
	msrnum = GLPCI_SB_CTRL;
	msr = rdmsr(msrnum);
	msr.lo |=  GLPCI_CRTL_PPIDE_SET;
	wrmsr(msrnum, msr);

	/*  Set up Master Configuration Register */
	/*  If 5536, use same master config settings as 5535, except for OHCI MSRs */
	i = 0;

	csi = &SB_MASTER_CONF_TABLE[i];
	for(; csi->msrnum; csi++){
		msr.lo = csi->msr.lo;
		msr.hi = csi->msr.hi;
		wrmsr(csi->msrnum, msr); // MSR - see table above
	}

	/*  Flash Setup */
	printk(BIOS_INFO, "%sDOING ChipsetFlashSetup()!\n",
			sb->setupflash ? "" : "NOT ");

	if (sb->setupflash)
		ChipsetFlashSetup();

	/*  Set up Hardware Clock Gating */

	/* if (getnvram(TOKEN_SB_CLK_GATE) != TVALUE_DISABLE) */
	{
		csi = CS5535_CLOCK_GATING_TABLE;

		for(; csi->msrnum; csi++){
			msr.lo = csi->msr.lo;
			msr.hi = csi->msr.hi;
			wrmsr(csi->msrnum, msr);	// MSR - see table above
		}
	}
}

