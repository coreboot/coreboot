/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <arch/io.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include <device/pci_def.h>
#include "i82801dx.h"

#define DEBUG_SMI

/* I830M */
#define SMRAM		0x90
#define   D_OPEN	(1 << 6)
#define   D_CLS		(1 << 5)
#define   D_LCK		(1 << 4)
#define   G_SMRANE	(1 << 3)
#define   C_BASE_SEG	((0 << 2) | (1 << 1) | (0 << 0))

/* While we read PMBASE dynamically in case it changed, let's
 * initialize it with a sane value
 */
u16 pmbase = PMBASE_ADDR;

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
	printk(BIOS_SPEW, "PM1_STS: ");
	if (pm1_sts & (1 << 15)) printk(BIOS_SPEW, "WAK ");
	if (pm1_sts & (1 << 14)) printk(BIOS_SPEW, "PCIEXPWAK ");
	if (pm1_sts & (1 << 11)) printk(BIOS_SPEW, "PRBTNOR ");
	if (pm1_sts & (1 << 10)) printk(BIOS_SPEW, "RTC ");
	if (pm1_sts & (1 <<  8)) printk(BIOS_SPEW, "PWRBTN ");
	if (pm1_sts & (1 <<  5)) printk(BIOS_SPEW, "GBL ");
	if (pm1_sts & (1 <<  4)) printk(BIOS_SPEW, "BM ");
	if (pm1_sts & (1 <<  0)) printk(BIOS_SPEW, "TMROF ");
	printk(BIOS_SPEW, "\n");
	int reg16 = inw(pmbase + PM1_EN);
	printk(BIOS_SPEW, "PM1_EN: %x\n", reg16);
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
	printk(BIOS_DEBUG, "SMI_STS: ");
	if (smi_sts & (1 << 26)) printk(BIOS_DEBUG, "SPI ");
	if (smi_sts & (1 << 25)) printk(BIOS_DEBUG, "EL_SMI ");
	if (smi_sts & (1 << 21)) printk(BIOS_DEBUG, "MONITOR ");
	if (smi_sts & (1 << 20)) printk(BIOS_DEBUG, "PCI_EXP_SMI ");
	if (smi_sts & (1 << 18)) printk(BIOS_DEBUG, "INTEL_USB2 ");
	if (smi_sts & (1 << 17)) printk(BIOS_DEBUG, "LEGACY_USB2 ");
	if (smi_sts & (1 << 16)) printk(BIOS_DEBUG, "SMBUS_SMI ");
	if (smi_sts & (1 << 15)) printk(BIOS_DEBUG, "SERIRQ_SMI ");
	if (smi_sts & (1 << 14)) printk(BIOS_DEBUG, "PERIODIC ");
	if (smi_sts & (1 << 13)) printk(BIOS_DEBUG, "TCO ");
	if (smi_sts & (1 << 12)) printk(BIOS_DEBUG, "DEVMON ");
	if (smi_sts & (1 << 11)) printk(BIOS_DEBUG, "MCSMI ");
	if (smi_sts & (1 << 10)) printk(BIOS_DEBUG, "GPI ");
	if (smi_sts & (1 <<  9)) printk(BIOS_DEBUG, "GPE0 ");
	if (smi_sts & (1 <<  8)) printk(BIOS_DEBUG, "PM1 ");
	if (smi_sts & (1 <<  6)) printk(BIOS_DEBUG, "SWSMI_TMR ");
	if (smi_sts & (1 <<  5)) printk(BIOS_DEBUG, "APM ");
	if (smi_sts & (1 <<  4)) printk(BIOS_DEBUG, "SLP_SMI ");
	if (smi_sts & (1 <<  3)) printk(BIOS_DEBUG, "LEGACY_USB ");
	if (smi_sts & (1 <<  2)) printk(BIOS_DEBUG, "BIOS ");
	printk(BIOS_DEBUG, "\n");
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
	printk(BIOS_DEBUG, "GPE0_STS: ");
	for (i=31; i>= 16; i--) {
		if (gpe0_sts & (1 << i)) printk(BIOS_DEBUG, "GPIO%d ", (i-16));
	}
	if (gpe0_sts & (1 << 14)) printk(BIOS_DEBUG, "USB4 ");
	if (gpe0_sts & (1 << 13)) printk(BIOS_DEBUG, "PME_B0 ");
	if (gpe0_sts & (1 << 12)) printk(BIOS_DEBUG, "USB3 ");
	if (gpe0_sts & (1 << 11)) printk(BIOS_DEBUG, "PME ");
	if (gpe0_sts & (1 << 10)) printk(BIOS_DEBUG, "EL_SCI/BATLOW ");
	if (gpe0_sts & (1 <<  9)) printk(BIOS_DEBUG, "PCI_EXP ");
	if (gpe0_sts & (1 <<  8)) printk(BIOS_DEBUG, "RI ");
	if (gpe0_sts & (1 <<  7)) printk(BIOS_DEBUG, "SMB_WAK ");
	if (gpe0_sts & (1 <<  6)) printk(BIOS_DEBUG, "TCO_SCI ");
	if (gpe0_sts & (1 <<  5)) printk(BIOS_DEBUG, "AC97 ");
	if (gpe0_sts & (1 <<  4)) printk(BIOS_DEBUG, "USB2 ");
	if (gpe0_sts & (1 <<  3)) printk(BIOS_DEBUG, "USB1 ");
	if (gpe0_sts & (1 <<  2)) printk(BIOS_DEBUG, "HOT_PLUG ");
	if (gpe0_sts & (1 <<  0)) printk(BIOS_DEBUG, "THRM ");
	printk(BIOS_DEBUG, "\n");
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
	printk(BIOS_DEBUG, "TCO_STS: ");
	if (tco_sts & (1 << 20)) printk(BIOS_DEBUG, "SMLINK_SLV ");
	if (tco_sts & (1 << 18)) printk(BIOS_DEBUG, "BOOT ");
	if (tco_sts & (1 << 17)) printk(BIOS_DEBUG, "SECOND_TO ");
	if (tco_sts & (1 << 16)) printk(BIOS_DEBUG, "INTRD_DET ");
	if (tco_sts & (1 << 12)) printk(BIOS_DEBUG, "DMISERR ");
	if (tco_sts & (1 << 10)) printk(BIOS_DEBUG, "DMISMI ");
	if (tco_sts & (1 <<  9)) printk(BIOS_DEBUG, "DMISCI ");
	if (tco_sts & (1 <<  8)) printk(BIOS_DEBUG, "BIOSWR ");
	if (tco_sts & (1 <<  7)) printk(BIOS_DEBUG, "NEWCENTURY ");
	if (tco_sts & (1 <<  3)) printk(BIOS_DEBUG, "TIMEOUT ");
	if (tco_sts & (1 <<  2)) printk(BIOS_DEBUG, "TCO_INT ");
	if (tco_sts & (1 <<  1)) printk(BIOS_DEBUG, "SW_TCO ");
	if (tco_sts & (1 <<  0)) printk(BIOS_DEBUG, "NMI2SMI ");
	printk(BIOS_DEBUG, "\n");
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

static void busmaster_disable_on_bus(int bus)
{
	int slot, func;
	unsigned int val;
	unsigned char hdr;

	for (slot = 0; slot < 0x20; slot++) {
		for (func = 0; func < 8; func++) {
			u16 reg16;
			pci_devfn_t dev = PCI_DEV(bus, slot, func);

			val = pci_read_config32(dev, PCI_VENDOR_ID);

			if (val == 0xffffffff || val == 0x00000000 ||
			    val == 0x0000ffff || val == 0xffff0000)
				continue;

			/* Disable Bus Mastering for this one device */
			reg16 = pci_read_config16(dev, PCI_COMMAND);
			reg16 &= ~PCI_COMMAND_MASTER;
			pci_write_config16(dev, PCI_COMMAND, reg16);

			/* If this is a bridge, then follow it. */
			hdr = pci_read_config8(dev, PCI_HEADER_TYPE);
			hdr &= 0x7f;
			if (hdr == PCI_HEADER_TYPE_BRIDGE ||
			    hdr == PCI_HEADER_TYPE_CARDBUS) {
				unsigned int buses;
				buses = pci_read_config32(dev, PCI_PRIMARY_BUS);
				busmaster_disable_on_bus((buses >> 8) & 0xff);
			}
		}
	}
}

static void southbridge_smi_sleep(void)
{
	u8 reg8;
	u32 reg32;
	u8 slp_typ;
	/* FIXME: the power state on boot should be read from
	 * CMOS or even better from GNVS. Right now it's hard
	 * coded at compile time.
	 */
	u8 s5pwr = CONFIG_MAINBOARD_POWER_FAILURE_STATE;

	/* First, disable further SMIs */
	reg8 = inb(pmbase + SMI_EN);
	reg8 &= ~SLP_SMI_EN;
	outb(reg8, pmbase + SMI_EN);

	/* Figure out SLP_TYP */
	reg32 = inl(pmbase + PM1_CNT);
	printk(BIOS_SPEW, "SMI#: SLP = 0x%08x\n", reg32);
	slp_typ = acpi_sleep_from_pm1(reg32);

	/* Next, do the deed.
	 */

	switch (slp_typ) {
	case ACPI_S0: printk(BIOS_DEBUG, "SMI#: Entering S0 (On)\n"); break;
	case ACPI_S1: printk(BIOS_DEBUG, "SMI#: Entering S1 (Assert STPCLK#)\n"); break;
	case ACPI_S3:
		printk(BIOS_DEBUG, "SMI#: Entering S3 (Suspend-To-RAM)\n");
		/* Invalidate the cache before going to S3 */
		wbinvd();
		break;
	case ACPI_S4: printk(BIOS_DEBUG, "SMI#: Entering S4 (Suspend-To-Disk)\n"); break;
	case ACPI_S5:
		printk(BIOS_DEBUG, "SMI#: Entering S5 (Soft Power off)\n");

		outl(0, pmbase + GPE0_EN);

		/* Should we keep the power state after a power loss?
		 * In case the setting is "ON" or "OFF" we don't have
		 * to do anything. But if it's "KEEP" we have to switch
		 * to "OFF" before entering S5.
		 */
		if (s5pwr == MAINBOARD_POWER_KEEP) {
			reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_3);
			reg8 |= 1;
			pci_write_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_3, reg8);
		}

		/* also iterates over all bridges on bus 0 */
		busmaster_disable_on_bus(0);
		break;
	default: printk(BIOS_DEBUG, "SMI#: ERROR: SLP_TYP reserved\n"); break;
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

static void southbridge_smi_apmc(void)
{
	u32 pmctrl;
	u8 reg8;

	reg8 = apm_get_apmc();
	switch (reg8) {
	case APM_CNT_CST_CONTROL:
		/* Calling this function seems to cause
		 * some kind of race condition in Linux
		 * and causes a kernel oops
		 */
		break;
	case APM_CNT_PST_CONTROL:
		/* Calling this function seems to cause
		 * some kind of race condition in Linux
		 * and causes a kernel oops
		 */
		break;
	case APM_CNT_ACPI_DISABLE:
		pmctrl = inl(pmbase + PM1_CNT);
		pmctrl &= ~SCI_EN;
		outl(pmctrl, pmbase + PM1_CNT);
		break;
	case APM_CNT_ACPI_ENABLE:
		pmctrl = inl(pmbase + PM1_CNT);
		pmctrl |= SCI_EN;
		outl(pmctrl, pmbase + PM1_CNT);
		break;
	}
}

static void southbridge_smi_pm1(void)
{
	u16 pm1_sts;

	pm1_sts = reset_pm1_status();
	dump_pm1_status(pm1_sts);

	/* While OSPM is not active, poweroff immediately
	 * on a power button event.
	 */
	if (pm1_sts & PWRBTN_STS) {
		// power button pressed
		u32 reg32;
		reg32 = (7 << 10) | (1 << 13);
		outl(reg32, pmbase + PM1_CNT);
	}
}

static void southbridge_smi_gpe0(void)
{
	u32 gpe0_sts;

	gpe0_sts = reset_gpe0_status();
	dump_gpe0_status(gpe0_sts);
}

static void southbridge_smi_gpi(void)
{
	u16 reg16;
	reg16 = inw(pmbase + ALT_GP_SMI_STS);
	outl(reg16, pmbase + ALT_GP_SMI_STS);

	reg16 &= inw(pmbase + ALT_GP_SMI_EN);

	mainboard_smi_gpi(reg16);

	if (reg16)
		printk(BIOS_DEBUG, "GPI (mask %04x)\n",reg16);
}

static void southbridge_smi_mc(void)
{
	u32 reg32;

	reg32 = inl(pmbase + SMI_EN);

	/* Are periodic SMIs enabled? */
	if ((reg32 & MCSMI_EN) == 0)
		return;

	printk(BIOS_DEBUG, "Microcontroller SMI.\n");
}

static void southbridge_smi_tco(void)
{
	u32 tco_sts;

	tco_sts = reset_tco_status();

	/* Any TCO event? */
	if (!tco_sts)
		return;

	if (tco_sts & (1 << 8)) { // BIOSWR
		u8 bios_cntl;

		bios_cntl = pci_read_config16(PCI_DEV(0, 0x1f, 0), 0xdc);

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
			printk(BIOS_DEBUG, "Switching back to RO\n");
			pci_write_config32(PCI_DEV(0, 0x1f, 0), 0xdc, (bios_cntl & ~1));
		} /* No else for now? */
	} else if (tco_sts & (1 << 3)) { /* TIMEOUT */
		/* Handle TCO timeout */
		printk(BIOS_DEBUG, "TCO Timeout.\n");
	} else {
		dump_tco_status(tco_sts);
	}
}

static void southbridge_smi_periodic(void)
{
	u32 reg32;

	reg32 = inl(pmbase + SMI_EN);

	/* Are periodic SMIs enabled? */
	if ((reg32 & PERIODIC_EN) == 0)
		return;

	printk(BIOS_DEBUG, "Periodic SMI.\n");
}

typedef void (*smi_handler_t)(void);

smi_handler_t southbridge_smi[32] = {
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
	southbridge_smi_gpi,	  // [10] GPI_STS
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
	NULL,			  // [21] MONITOR_STS
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
 */
void southbridge_smi_handler(void)
{
	int i, dump = 0;
	u32 smi_sts;

	/* Update global variable pmbase */
	pmbase = pci_read_config16(PCI_DEV(0, 0x1f, 0), 0x40) & 0xfffc;

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
			if (southbridge_smi[i]) {
				southbridge_smi[i]();
			} else {
				printk(BIOS_DEBUG, "SMI_STS[%d] occurred, but no "
						"handler available.\n", i);
				dump = 1;
			}
		}
	}

	if (dump) {
		dump_smi_status(smi_sts);
	}
}
