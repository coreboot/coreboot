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
 */

#include <types.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <device/pci_def.h>
#include <cpu/x86/smm.h>
#include <elog.h>
#include <halt.h>
#include <pc80/mc146818rtc.h>
#include "pch.h"

#include "nvs.h"

/* We are using PCIe accesses for now
 *  1. the chipset can do it
 *  2. we don't need to worry about how we leave 0xcf8/0xcfc behind
 */
#include <northbridge/intel/fsp_sandybridge/northbridge.h>

/* While we read PMBASE dynamically in case it changed, let's
 * initialize it with a sane value
 */
static u16 pmbase = DEFAULT_PMBASE;
u16 smm_get_pmbase(void)
{
	return pmbase;
}

static u8 smm_initialized = 0;

/* GNVS needs to be updated by an 0xEA PM Trap (B2) after it has been located
 * by coreboot.
 */
static global_nvs_t *gnvs;
global_nvs_t *smm_get_gnvs(void)
{
	return gnvs;
}

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
	for (i=31; i >= 16; i--) {
		if (gpe0_sts & (1 << i)) printk(BIOS_DEBUG, "GPIO%d ", (i-16));
	}
	if (gpe0_sts & (1 << 14)) printk(BIOS_DEBUG, "USB4 ");
	if (gpe0_sts & (1 << 13)) printk(BIOS_DEBUG, "PME_B0 ");
	if (gpe0_sts & (1 << 12)) printk(BIOS_DEBUG, "USB3 ");
	if (gpe0_sts & (1 << 11)) printk(BIOS_DEBUG, "PME ");
	if (gpe0_sts & (1 << 10)) printk(BIOS_DEBUG, "BATLOW ");
	if (gpe0_sts & (1 <<  9)) printk(BIOS_DEBUG, "PCI_EXP ");
	if (gpe0_sts & (1 <<  8)) printk(BIOS_DEBUG, "RI ");
	if (gpe0_sts & (1 <<  7)) printk(BIOS_DEBUG, "SMB_WAK ");
	if (gpe0_sts & (1 <<  6)) printk(BIOS_DEBUG, "TCO_SCI ");
	if (gpe0_sts & (1 <<  5)) printk(BIOS_DEBUG, "AC97 ");
	if (gpe0_sts & (1 <<  4)) printk(BIOS_DEBUG, "USB2 ");
	if (gpe0_sts & (1 <<  3)) printk(BIOS_DEBUG, "USB1 ");
	if (gpe0_sts & (1 <<  2)) printk(BIOS_DEBUG, "SWGPE ");
	if (gpe0_sts & (1 <<  1)) printk(BIOS_DEBUG, "HOTPLUG ");
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

int southbridge_io_trap_handler(int smif)
{
	switch (smif) {
	case 0x32:
		printk(BIOS_DEBUG, "OS Init\n");
		/* gnvs->smif:
		 *  On success, the IO Trap Handler returns 0
		 *  On failure, the IO Trap Handler returns a value != 0
		 */
		gnvs->smif = 0;
		return 1; /* IO trap handled */
	}

	/* Not handled */
	return 0;
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
			u32 reg32;
			pci_devfn_t dev = PCI_DEV(bus, slot, func);

			val = pci_read_config32(dev, PCI_VENDOR_ID);

			if (val == 0xffffffff || val == 0x00000000 ||
			    val == 0x0000ffff || val == 0xffff0000)
				continue;

			/* Disable Bus Mastering for this one device */
			reg32 = pci_read_config32(dev, PCI_COMMAND);
			reg32 &= ~PCI_COMMAND_MASTER;
			pci_write_config32(dev, PCI_COMMAND, reg32);

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

/*
 * Drive GPIO 60 low to gate memory reset in S3.
 *
 * Intel reference designs all use GPIO 60 but it is
 * not a requirement and boards could use a different pin.
 */
static void southbridge_gate_memory_reset(void)
{
	u32 reg32;
	u16 gpiobase;

	gpiobase = pci_read_config16(PCI_DEV(0, 0x1f, 0), GPIOBASE) & 0xfffc;
	if (!gpiobase)
		return;

	/* Make sure it is set as GPIO */
	reg32 = inl(gpiobase + GPIO_USE_SEL2);
	if (!(reg32 & (1 << 28))) {
		reg32 |= (1 << 28);
		outl(reg32, gpiobase + GPIO_USE_SEL2);
	}

	/* Make sure it is set as output */
	reg32 = inl(gpiobase + GP_IO_SEL2);
	if (reg32 & (1 << 28)) {
		reg32 &= ~(1 << 28);
		outl(reg32, gpiobase + GP_IO_SEL2);
	}

	/* Drive the output low */
	reg32 = inl(gpiobase + GP_LVL2);
	reg32 &= ~(1 << 28);
	outl(reg32, gpiobase + GP_LVL2);
}

static void southbridge_smi_sleep(void)
{
	u8 reg8;
	u32 reg32;
	u8 slp_typ;
	u8 s5pwr = CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;

	// save and recover RTC port values
	u8 tmp70, tmp72;
	tmp70 = inb(0x70);
	tmp72 = inb(0x72);
	get_option(&s5pwr, "power_on_after_fail");
	outb(tmp70, 0x70);
	outb(tmp72, 0x72);

	/* First, disable further SMIs */
	reg8 = inb(pmbase + SMI_EN);
	reg8 &= ~SLP_SMI_EN;
	outb(reg8, pmbase + SMI_EN);

	/* Figure out SLP_TYP */
	reg32 = inl(pmbase + PM1_CNT);
	printk(BIOS_SPEW, "SMI#: SLP = 0x%08x\n", reg32);
	slp_typ = acpi_sleep_from_pm1(reg32);

	/* Do any mainboard sleep handling */
	mainboard_smi_sleep(slp_typ);

#if IS_ENABLED(CONFIG_ELOG_GSMI)
	/* Log S3, S4, and S5 entry */
	if (slp_typ >= ACPI_S3)
		elog_add_event_byte(ELOG_TYPE_ACPI_ENTER, slp_typ);
#endif

	/* Next, do the deed.
	 */

	switch (slp_typ) {
	case ACPI_S0: printk(BIOS_DEBUG, "SMI#: Entering S0 (On)\n"); break;
	case ACPI_S1: printk(BIOS_DEBUG, "SMI#: Entering S1 (Assert STPCLK#)\n"); break;
	case ACPI_S3:
		printk(BIOS_DEBUG, "SMI#: Entering S3 (Suspend-To-RAM)\n");

		/* Gate memory reset */
		southbridge_gate_memory_reset();

		/* Invalidate the cache before going to S3 */
		wbinvd();
		break;
	case ACPI_S4: printk(BIOS_DEBUG, "SMI#: Entering S4 (Suspend-To-Disk)\n"); break;
	case ACPI_S5:
		printk(BIOS_DEBUG, "SMI#: Entering S5 (Soft Power off)\n");

		outl(0, pmbase + GPE0_EN);

		/* Always set the flag in case CMOS was changed on runtime. For
		 * "KEEP", switch to "OFF" - KEEP is software emulated
		 */
		reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_3);
		if (s5pwr == MAINBOARD_POWER_ON) {
			reg8 &= ~1;
		} else {
			reg8 |= 1;
		}
		pci_write_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_3, reg8);

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

	/* Make sure to stop executing code here for S3/S4/S5 */
	if (slp_typ >= ACPI_S3)
		halt();

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

/*
 * Look for Synchronous IO SMI and use save state from that
 * core in case we are not running on the same core that
 * initiated the IO transaction.
 */
static em64t101_smm_state_save_area_t *smi_apmc_find_state_save(u8 cmd)
{
	em64t101_smm_state_save_area_t *state;
	int node;

	/* Check all nodes looking for the one that issued the IO */
	for (node = 0; node < CONFIG_MAX_CPUS; node++) {
		state = smm_get_save_state(node);

		/* Check for Synchronous IO (bit0 == 1) */
		if (!(state->io_misc_info & (1 << 0)))
			continue;

		/* Make sure it was a write (bit4 == 0) */
		if (state->io_misc_info & (1 << 4))
			continue;

		/* Check for APMC IO port */
		if (((state->io_misc_info >> 16) & 0xff) != APM_CNT)
			continue;

		/* Check AX against the requested command */
		if ((state->rax & 0xff) != cmd)
			continue;

		return state;
	}

	return NULL;
}

#if IS_ENABLED(CONFIG_ELOG_GSMI)
static void southbridge_smi_gsmi(void)
{
	u32 *ret, *param;
	u8 sub_command;
	em64t101_smm_state_save_area_t *io_smi =
		smi_apmc_find_state_save(ELOG_GSMI_APM_CNT);

	if (!io_smi)
		return;

	/* Command and return value in EAX */
	ret = (u32*)&io_smi->rax;
	sub_command = (u8)(*ret >> 8);

	/* Parameter buffer in EBX */
	param = (u32*)&io_smi->rbx;

	/* drivers/elog/gsmi.c */
	*ret = gsmi_exec(sub_command, param);
}
#endif

static void southbridge_smi_apmc(void)
{
	u32 pmctrl;
	u8 reg8;
	em64t101_smm_state_save_area_t *state;

	/* Emulate B2 register as the FADT / Linux expects it */

	reg8 = inb(APM_CNT);
	switch (reg8) {
	case APM_CNT_CST_CONTROL:
		/* Calling this function seems to cause
		 * some kind of race condition in Linux
		 * and causes a kernel oops
		 */
		printk(BIOS_DEBUG, "C-state control\n");
		break;
	case APM_CNT_PST_CONTROL:
		/* Calling this function seems to cause
		 * some kind of race condition in Linux
		 * and causes a kernel oops
		 */
		printk(BIOS_DEBUG, "P-state control\n");
		break;
	case APM_CNT_ACPI_DISABLE:
		pmctrl = inl(pmbase + PM1_CNT);
		pmctrl &= ~SCI_EN;
		outl(pmctrl, pmbase + PM1_CNT);
		printk(BIOS_DEBUG, "SMI#: ACPI disabled.\n");
		break;
	case APM_CNT_ACPI_ENABLE:
		pmctrl = inl(pmbase + PM1_CNT);
		pmctrl |= SCI_EN;
		outl(pmctrl, pmbase + PM1_CNT);
		printk(BIOS_DEBUG, "SMI#: ACPI enabled.\n");
		break;
	case APM_CNT_GNVS_UPDATE:
		if (smm_initialized) {
			printk(BIOS_DEBUG, "SMI#: SMM structures already initialized!\n");
			return;
		}
		state = smi_apmc_find_state_save(reg8);
		if (state) {
			/* EBX in the state save contains the GNVS pointer */
			gnvs = (global_nvs_t *)((u32)state->rbx);
			smm_initialized = 1;
			printk(BIOS_DEBUG, "SMI#: Setting GNVS to %p\n", gnvs);
		}
		break;
#if IS_ENABLED(CONFIG_ELOG_GSMI)
	case ELOG_GSMI_APM_CNT:
		southbridge_smi_gsmi();
		break;
#endif
	}

	mainboard_smi_apmc(reg8);
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
#if IS_ENABLED(CONFIG_ELOG_GSMI)
		elog_add_event(ELOG_TYPE_POWER_BUTTON);
#endif
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
	outw(reg16, pmbase + ALT_GP_SMI_STS);

	reg16 &= inw(pmbase + ALT_GP_SMI_EN);

	mainboard_smi_gpi(reg16);

	if (reg16)
		printk(BIOS_DEBUG, "GPI (mask %04x)\n",reg16);

	outw(reg16, pmbase + ALT_GP_SMI_STS);
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
	} else if (!tco_sts) {
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

static void southbridge_smi_monitor(void)
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
			printk(BIOS_DEBUG, "SMI1 command\n");
			data = RCBA32(0x1e18);
			data &= mask;
			// if (smi1)
			//	southbridge_smi_command(data);
			// return;
		}
		// Fall through to debug
	}

	printk(BIOS_DEBUG, "  trapped io address = 0x%x\n", trap_cycle & 0xfffc);
	for (i=0; i < 4; i++) if (IOTRAP(i)) printk(BIOS_DEBUG, "  TRAP = %d\n", i);
	printk(BIOS_DEBUG, "  AHBE = %x\n", (trap_cycle >> 16) & 0xf);
	printk(BIOS_DEBUG, "  MASK = 0x%08x\n", mask);
	printk(BIOS_DEBUG, "  read/write: %s\n", (trap_cycle & (1 << 24)) ? "read" : "write");

	if (!(trap_cycle & (1 << 24))) {
		/* Write Cycle */
		data = RCBA32(0x1e18);
		printk(BIOS_DEBUG, "  iotrap written data = 0x%08x\n", data);
	}
#undef IOTRAP
}

typedef void (*smi_handler_t)(void);

static smi_handler_t southbridge_smi[32] = {
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
 * @param node
 * @param state_save
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
