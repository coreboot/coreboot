/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <types.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include "i82801gx.h"
#include "i82801gx_power.h"

#define DEBUG_SMI

#define APM_CNT		0xb2
#define   CST_CONTROL	0x85 // 0x85 crashes the box
#define   PST_CONTROL	0x80 // 0x80 crashes the box
#define   ACPI_DISABLE	0x1e
#define   ACPI_ENABLE	0xe1
#define   GNVS_UPDATE   0xea
#define APM_STS		0xb3

/* I945 */
#define SMRAM		0x9d
#define   D_OPEN	(1 << 6)
#define   D_CLS		(1 << 5)
#define   D_LCK		(1 << 4)
#define   G_SMRANE	(1 << 3)
#define   C_BASE_SEG	((0 << 2) | (1 << 1) | (0 << 0))

/* ICH7 */
#define PM1_STS		0x00
#define PM1_EN		0x02
#define PM1_CNT		0x04
#define   SLP_EN	(1 << 13)
#define   SLP_TYP	(7 << 10)
#define   GBL_RLS	(1 << 2)
#define   BM_RLD	(1 << 1)
#define   SCI_EN	(1 << 0)
#define PM1_TMR		0x08
#define PROC_CNT	0x10
#define LV2		0x14
#define LV3		0x15
#define LV4		0x16
#define PM2_CNT		0x20 // mobile only
#define GPE0_STS	0x28
#define GPE0_EN		0x2c
#define   PME_B0_EN	(1 << 13)
#define SMI_EN		0x30
#define   EL_SMI_EN	 (1 << 25) // Intel Quick Resume Technology
#define   INTEL_USB2_EN	 (1 << 18) // Intel-Specific USB2 SMI logic
#define   LEGACY_USB2_EN (1 << 17) // Legacy USB2 SMI logic
#define   PERIODIC_EN	 (1 << 14) // SMI on PERIODIC_STS in SMI_STS
#define   TCO_EN	 (1 << 13) // Enable TCO Logic (BIOSWE et al)
#define   MCSMI_EN	 (1 << 11) // Trap microcontroller range access
#define   BIOS_RLS	 (1 <<  7) // asserts SCI on bit set
#define   SWSMI_TMR_EN	 (1 <<  6) // start software smi timer on bit set
#define   APMC_EN	 (1 <<  5) // Writes to APM_CNT cause SMI#
#define   SLP_SMI_EN	 (1 <<  4) // Write to SLP_EN in PM1_CNT asserts SMI#
#define   LEGACY_USB_EN  (1 <<  3) // Legacy USB circuit SMI logic
#define   BIOS_EN	 (1 <<  2) // Assert SMI# on setting GBL_RLS bit
#define   EOS		 (1 <<  1) // End of SMI (deassert SMI#)
#define   GBL_SMI_EN	 (1 <<  0) // SMI# generation at all?
#define SMI_STS		0x34
#define ALT_GP_SMI_EN	0x38
#define ALT_GP_SMI_STS	0x3a
#define GPE_CNTL	0x42
#define DEVACT_STS	0x44
#define SS_CNT		0x50
#define C3_RES		0x54

#include "i82801gx_nvs.h"

/* While we read PMBASE dynamically in case it changed, let's
 * initialize it with a sane value
 */
u16 pmbase = DEFAULT_PMBASE;

/* GNVS needs to be updated by an 0xEA PM Trap (B2) after it has been located
 * by coreboot.
 */
global_nvs_t *gnvs = (global_nvs_t *)0x0;
void *tcg = (void *)0x0;
void *smi1 = (void *)0x0;

/**
 * @brief read and clear PM1_STS 
 * @return PM1_STS register
 */
static u16 reset_pm1_status(void)
{
	u16 reg16;
	
	reg16 = inw(pmbase + PM1_STS);
	/* set status bits are cleared by writing 1 to them */
	outw(reg16, pmbase + PM1_STS);
	
	return reg16;
}

static void dump_pm1_status(u16 pm1_sts)
{
	printk_debug("PM1_STS: ");
	if (pm1_sts & (1 << 15)) printk_debug("WAK ");
	if (pm1_sts & (1 << 14)) printk_debug("PCIEXPWAK ");
	if (pm1_sts & (1 << 11)) printk_debug("PRBTNOR ");
	if (pm1_sts & (1 << 10)) printk_debug("RTC ");
	if (pm1_sts & (1 <<  8)) printk_debug("PWRBTN ");
	if (pm1_sts & (1 <<  5)) printk_debug("GBL ");
	if (pm1_sts & (1 <<  4)) printk_debug("BM ");
	if (pm1_sts & (1 <<  0)) printk_debug("TMROF ");
	printk_debug("\n");
}

/**
 * @brief read and clear SMI_STS 
 * @return SMI_STS register
 */
static u32 reset_smi_status(void)
{
	u32 reg32;
	
	reg32 = inl(pmbase + SMI_STS);
	/* set status bits are cleared by writing 1 to them */
	outl(reg32, pmbase + SMI_STS);
	
	return reg32;
}

static void dump_smi_status(u32 smi_sts)
{
	printk_debug("SMI_STS: ");
	if (smi_sts & (1 << 26)) printk_debug("SPI ");
	if (smi_sts & (1 << 25)) printk_debug("EL_SMI ");
	if (smi_sts & (1 << 21)) printk_debug("MONITOR ");
	if (smi_sts & (1 << 20)) printk_debug("PCI_EXP_SMI ");
	if (smi_sts & (1 << 18)) printk_debug("INTEL_USB2 ");
	if (smi_sts & (1 << 17)) printk_debug("LEGACY_USB2 ");
	if (smi_sts & (1 << 16)) printk_debug("SMBUS_SMI ");
	if (smi_sts & (1 << 15)) printk_debug("SERIRQ_SMI ");
	if (smi_sts & (1 << 14)) printk_debug("PERIODIC ");
	if (smi_sts & (1 << 13)) printk_debug("TCO ");
	if (smi_sts & (1 << 12)) printk_debug("DEVMON ");
	if (smi_sts & (1 << 11)) printk_debug("MCSMI ");
	if (smi_sts & (1 << 10)) printk_debug("GPI ");
	if (smi_sts & (1 <<  9)) printk_debug("GPE0 ");
	if (smi_sts & (1 <<  8)) printk_debug("PM1 ");
	if (smi_sts & (1 <<  6)) printk_debug("SWSMI_TMR ");
	if (smi_sts & (1 <<  5)) printk_debug("APM ");
	if (smi_sts & (1 <<  4)) printk_debug("SLP_SMI ");
	if (smi_sts & (1 <<  3)) printk_debug("LEGACY_USB ");
	if (smi_sts & (1 <<  2)) printk_debug("BIOS ");
	printk_debug("\n");
}


/**
 * @brief read and clear GPE0_STS
 * @return GPE0_STS register
 */
static u32 reset_gpe0_status(void)
{
	u32 reg32;
	
	reg32 = inl(pmbase + GPE0_STS);
	/* set status bits are cleared by writing 1 to them */
	outl(reg32, pmbase + GPE0_STS);
	
	return reg32;
}

static void dump_gpe0_status(u32 gpe0_sts)
{
	int i;
	printk_debug("GPE0_STS: ");
	for (i=31; i<= 16; i--) {
		if (gpe0_sts & (1 << i)) printk_debug("GPIO%d ", (i-16));
	}
	if (gpe0_sts & (1 << 14)) printk_debug("USB4 ");
	if (gpe0_sts & (1 << 13)) printk_debug("PME_B0 ");
	if (gpe0_sts & (1 << 12)) printk_debug("USB3 ");
	if (gpe0_sts & (1 << 11)) printk_debug("PME ");
	if (gpe0_sts & (1 << 10)) printk_debug("EL_SCI/BATLOW ");
	if (gpe0_sts & (1 <<  9)) printk_debug("PCI_EXP ");
	if (gpe0_sts & (1 <<  8)) printk_debug("RI ");
	if (gpe0_sts & (1 <<  7)) printk_debug("SMB_WAK ");
	if (gpe0_sts & (1 <<  6)) printk_debug("TCO_SCI ");
	if (gpe0_sts & (1 <<  5)) printk_debug("AC97 ");
	if (gpe0_sts & (1 <<  4)) printk_debug("USB2 ");
	if (gpe0_sts & (1 <<  3)) printk_debug("USB1 ");
	if (gpe0_sts & (1 <<  2)) printk_debug("HOT_PLUG ");
	if (gpe0_sts & (1 <<  0)) printk_debug("THRM ");
	printk_debug("\n");
}


/**
 * @brief read and clear TCOx_STS 
 * @return TCOx_STS registers
 */
static u32 reset_tco_status(void)
{
	u32 tcobase = pmbase + 0x60;
	u32 reg32;
	
	reg32 = inl(tcobase + 0x04);
	/* set status bits are cleared by writing 1 to them */
	outl(reg32 & ~(1<<18), tcobase + 0x04); //  Don't clear BOOT_STS before SECOND_TO_STS
	if (reg32 & (1 << 18))
		outl(reg32 & (1<<18), tcobase + 0x04); // clear BOOT_STS
	
	return reg32;
}


static void dump_tco_status(u32 tco_sts)
{
	printk_debug("TCO_STS: ");
	if (tco_sts & (1 << 20)) printk_debug("SMLINK_SLV ");
	if (tco_sts & (1 << 18)) printk_debug("BOOT ");
	if (tco_sts & (1 << 17)) printk_debug("SECOND_TO ");
	if (tco_sts & (1 << 16)) printk_debug("INTRD_DET ");
	if (tco_sts & (1 << 12)) printk_debug("DMISERR ");
	if (tco_sts & (1 << 10)) printk_debug("DMISMI ");
	if (tco_sts & (1 <<  9)) printk_debug("DMISCI ");
	if (tco_sts & (1 <<  8)) printk_debug("BIOSWR ");
	if (tco_sts & (1 <<  7)) printk_debug("NEWCENTURY ");
	if (tco_sts & (1 <<  3)) printk_debug("TIMEOUT ");
	if (tco_sts & (1 <<  2)) printk_debug("TCO_INT ");
	if (tco_sts & (1 <<  1)) printk_debug("SW_TCO ");
	if (tco_sts & (1 <<  0)) printk_debug("NMI2SMI ");
	printk_debug("\n");
}

/* We are using PCIe accesses for now
 *  1. the chipset can do it
 *  2. we don't need to worry about how we leave 0xcf8/0xcfc behind
 */
#include "../../../northbridge/intel/i945/pcie_config.c"

int southbridge_io_trap_handler(int smif)
{
	switch (smif) {
	case 0x32:
		printk_debug("OS Init\n");
		gnvs->smif = 0;
		break;
	default:
		/* Not handled */
		return 0;
	}

	/* On success, the IO Trap Handler returns 0
	 * On failure, the IO Trap Handler returns a value != 0
	 *
	 * For now, we force the return value to 0 and log all traps to
	 * see what's going on.
	 */
	//gnvs->smif = 0;
	return 1; /* IO trap handled */
}

/**
 * @brief Set the EOS bit
 */
void southbridge_smi_set_eos(void)
{
	u8 reg8;

	reg8 = inb(pmbase + SMI_EN);
	reg8 |= EOS;
	outb(reg8, pmbase + SMI_EN);
}


static void southbridge_smi_sleep(unsigned int node, smm_state_save_area_t *state_save)
{
	u8 reg8;
	u32 reg32;
	u8 slp_typ;
	/* FIXME: the power state on boot should be read from 
	 * CMOS or even better from GNVS. Right now it's hard
	 * coded at compile time.
	 */
	u8 s5pwr = CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;

	/* First, disable further SMIs */
	reg8 = inb(pmbase + SMI_EN);
	reg8 &= ~SLP_SMI_EN;
	outb(reg8, pmbase + SMI_EN);

	/* Figure out SLP_TYP */
	reg32 = inl(pmbase + PM1_CNT);
	printk_spew("SMI#: SLP = 0x%08x\n", reg32);
	slp_typ = (reg32 >> 10) & 7;

	/* Next, do the deed.
	 */

	switch (slp_typ) {
	case 0: printk_debug("SMI#: Entering S0 (On)\n"); break;
	case 1: printk_debug("SMI#: Entering S1 (Assert STPCLK#)\n"); break;
	case 5:
		printk_debug("SMI#: Entering S3 (Suspend-To-RAM)\n");
		/* Invalidate the cache before going to S3 */
		wbinvd();
		break;
	case 6: printk_debug("SMI#: Entering S4 (Suspend-To-Disk)\n"); break;
	case 7:
		printk_debug("SMI#: Entering S5 (Soft Power off)\n");
#if 0
		/* Set PME_B0_EN before going to S5 */
		reg32 = inl(pmbase + GPE0_EN);
		reg32 |= PME_B0_EN;
		outl(reg32, pmbase + GPE0_EN);
#endif
		/* Should we keep the power state after a power loss?
		 * In case the setting is "ON" or "OFF" we don't have
		 * to do anything. But if it's "KEEP" we have to switch
		 * to "OFF" before entering S5.
		 */
		if (s5pwr == MAINBOARD_POWER_KEEP) {
			reg8 = pcie_read_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_3);
			reg8 |= 1;
			pcie_write_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_3, reg8);
		}
		break;
	default: printk_debug("SMI#: ERROR: SLP_TYP reserved\n"); break;
	}

	/* Write back to the SLP register to cause the originally intended
	 * event again. We need to set BIT13 (SLP_EN) though to make the 
	 * sleep happen.
	 */
	outl(reg32 | SLP_EN, pmbase + PM1_CNT);

	/* In most sleep states, the code flow of this function ends at
	 * the line above. However, if we entered sleep state S1 and wake
	 * up again, we will continue to execute code in this function.
	 */
	reg32 = inl(pmbase + PM1_CNT);
	if (reg32 & SCI_EN) {
		/* The OS is not an ACPI OS, so we set the state to S0 */
		reg32 &= ~(SLP_EN | SLP_TYP);
		outl(reg32, pmbase + PM1_CNT);
	}
}

static void southbridge_smi_apmc(unsigned int node, smm_state_save_area_t *state_save)
{
	u32 pmctrl;
	u8 reg8;

	/* Emulate B2 register as the FADT / Linux expects it */

	reg8 = inb(APM_CNT);
	switch (reg8) {
	case CST_CONTROL:
		/* Calling this function seems to cause
		 * some kind of race condition in Linux
		 * and causes a kernel oops
		 */
		printk_debug("C-state control\n");
		break;
	case PST_CONTROL:
		/* Calling this function seems to cause
		 * some kind of race condition in Linux
		 * and causes a kernel oops
		 */
		printk_debug("P-state control\n");
		break;
	case ACPI_DISABLE:
		pmctrl = inl(pmbase + PM1_CNT);
		pmctrl &= ~SCI_EN;
		outl(pmctrl, pmbase + PM1_CNT);
		printk_debug("SMI#: ACPI disabled.\n");
		break;
	case ACPI_ENABLE:
		pmctrl = inl(pmbase + PM1_CNT);
		pmctrl |= SCI_EN;
		outl(pmctrl, pmbase + PM1_CNT);
		printk_debug("SMI#: ACPI enabled.\n");
		break;
	case GNVS_UPDATE:
		gnvs = *(global_nvs_t **)0x500;
		tcg  = *(void **)0x504;
		smi1 = *(void **)0x508;
		printk_debug("SMI#: Setting up structures to %p, %p, %p\n", gnvs, tcg, smi1);
		break;
	default:
		printk_debug("SMI#: Unknown function APM_CNT=%02x\n", reg8);
	}
}

static void southbridge_smi_pm1(unsigned int node, smm_state_save_area_t *state_save)
{
	u16 pm1_sts;

	pm1_sts = reset_pm1_status();
	dump_pm1_status(pm1_sts);
}

static void southbridge_smi_gpe0(unsigned int node, smm_state_save_area_t *state_save)
{
	u32 gpe0_sts;

	gpe0_sts = reset_gpe0_status();
	dump_gpe0_status(gpe0_sts);
}

static void southbridge_smi_mc(unsigned int node, smm_state_save_area_t *state_save)
{
	u32 reg32;

	reg32 = inl(pmbase + SMI_EN);

	/* Are periodic SMIs enabled? */
	if ((reg32 & MCSMI_EN) == 0)
		return;

	printk_debug("Microcontroller SMI.\n");
}



static void southbridge_smi_tco(unsigned int node, smm_state_save_area_t *state_save)
{
	u32 tco_sts;

	tco_sts = reset_tco_status();

	/* Any TCO event? */
	if (!tco_sts)
		return;

	if (tco_sts & (1 << 8)) { // BIOSWR
		u8 bios_cntl;

		bios_cntl = pcie_read_config16(PCI_DEV(0, 0x1f, 0), 0xdc);

		if (bios_cntl & 1) {
			/* BWE is RW, so the SMI was caused by a
			 * write to BWE, not by a write to the BIOS
			 */

			/* This is the place where we notice someone
			 * is trying to tinker with the BIOS. We are
			 * trying to be nice and just ignore it. A more
			 * resolute answer would be to power down the
			 * box.
			 */
			printk_debug("Switching back to RO\n");
			pcie_write_config32(PCI_DEV(0, 0x1f, 0), 0xdc, (bios_cntl & ~1));
		} /* No else for now? */
	} else if (tco_sts & (1 << 3)) { /* TIMEOUT */
		/* Handle TCO timeout */
		printk_debug("TCO Timeout.\n");
	} else if (!tco_sts) {
		dump_tco_status(tco_sts);
	}
}

static void southbridge_smi_periodic(unsigned int node, smm_state_save_area_t *state_save)
{
	u32 reg32;

	reg32 = inl(pmbase + SMI_EN);

	/* Are periodic SMIs enabled? */
	if ((reg32 & PERIODIC_EN) == 0)
		return;

	printk_debug("Periodic SMI.\n");
}

static void southbridge_smi_monitor(unsigned int node, smm_state_save_area_t *state_save)
{
#define IOTRAP(x) (trap_sts & (1 << x))
	u32 trap_sts, trap_cycle;
	u32 data, mask = 0;
	int i;

	trap_sts = RCBA32(0x1e00); // TRSR - Trap Status Register
	RCBA32(0x1e00) = trap_sts; // Clear trap(s) in TRSR

	trap_cycle = RCBA32(0x1e10);
	for (i=16; i<20; i++) {
		if (trap_cycle & (1 << i))
			mask |= (0xff << ((i - 16) << 2));
	}


	/* IOTRAP(3) SMI function call */
	if (IOTRAP(3)) {
		if (gnvs && gnvs->smif)
			io_trap_handler(gnvs->smif); // call function smif
		return;
	}

	/* IOTRAP(2) currently unused
	 * IOTRAP(1) currently unused */

	/* IOTRAP(0) SMIC */
	if (IOTRAP(0)) {
		if (!(trap_cycle & (1 << 24))) { // It's a write
			printk_debug("SMI1 command\n");
			data = RCBA32(0x1e18);
			data &= mask;
			// if (smi1)
			// 	southbridge_smi_command(data);
			// return;
		}
		// Fall through to debug
	}

	printk_debug("  trapped io address = 0x%x\n", trap_cycle & 0xfffc);
	for (i=0; i < 4; i++) if(IOTRAP(i)) printk_debug("  TRAP = %d\n", i);
	printk_debug("  AHBE = %x\n", (trap_cycle >> 16) & 0xf);
	printk_debug("  MASK = 0x%08x\n", mask);
	printk_debug("  read/write: %s\n", (trap_cycle & (1 << 24)) ? "read" : "write");

	if (!(trap_cycle & (1 << 24))) {
		/* Write Cycle */
		data = RCBA32(0x1e18);
		printk_debug("  iotrap written data = 0x%08x\n", data);
	}
#undef IOTRAP
}

typedef void (*smi_handler)(unsigned int node, 
		smm_state_save_area_t *state_save);

smi_handler southbridge_smi[32] = {
	NULL,			  //  [0] reserved
	NULL,			  //  [1] reserved
	NULL,			  //  [2] BIOS_STS
	NULL,			  //  [3] LEGACY_USB_STS
	southbridge_smi_sleep,	  //  [4] SLP_SMI_STS
	southbridge_smi_apmc,	  //  [5] APM_STS
	NULL,			  //  [6] SWSMI_TMR_STS
	NULL,			  //  [7] reserved
	southbridge_smi_pm1,	  //  [8] PM1_STS
	southbridge_smi_gpe0,	  //  [9] GPE0_STS
	NULL,			  // [10] GPI_STS
	southbridge_smi_mc,	  // [11] MCSMI_STS
	NULL,			  // [12] DEVMON_STS
	southbridge_smi_tco,	  // [13] TCO_STS
	southbridge_smi_periodic, // [14] PERIODIC_STS
	NULL,			  // [15] SERIRQ_SMI_STS
	NULL,			  // [16] SMBUS_SMI_STS
	NULL,			  // [17] LEGACY_USB2_STS
	NULL,			  // [18] INTEL_USB2_STS
	NULL,			  // [19] reserved
	NULL,			  // [20] PCI_EXP_SMI_STS
	southbridge_smi_monitor,  // [21] MONITOR_STS
	NULL,			  // [22] reserved
	NULL,			  // [23] reserved
	NULL,			  // [24] reserved
	NULL,			  // [25] EL_SMI_STS
	NULL,			  // [26] SPI_STS
	NULL,			  // [27] reserved
	NULL,			  // [28] reserved
	NULL,			  // [29] reserved
	NULL,			  // [30] reserved
	NULL			  // [31] reserved 
};

/**
 * @brief Interrupt handler for SMI#
 *
 * @param smm_revision revision of the smm state save map
 */

void southbridge_smi_handler(unsigned int node, smm_state_save_area_t *state_save)
{
	int i, dump = 0;
	u32 smi_sts;

	/* Update global variable pmbase */
	pmbase = pcie_read_config16(PCI_DEV(0, 0x1f, 0), 0x40) & 0xfffc;

	/* We need to clear the SMI status registers, or we won't see what's
	 * happening in the following calls.
	 */
	smi_sts = reset_smi_status();
	
	/* Filter all non-enabled SMI events */
	// FIXME Double check, this clears MONITOR
	// smi_sts &= inl(pmbase + SMI_EN);

	/* Call SMI sub handler for each of the status bits */
	for (i = 0; i < 31; i++) {
		if (smi_sts & (1 << i)) { 
			if (southbridge_smi[i])
				southbridge_smi[i](node, state_save);
			else {
				printk_debug("SMI_STS[%d] occured, but no "
						"handler available.\n", i);
				dump = 1;
			}
		}
	}

	if(dump) {
		dump_smi_status(smi_sts);
	}

}
