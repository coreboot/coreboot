/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2010 Rudolf Marek <r.marek@assembler.cz>
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
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include <device/pci_def.h>
#include "vt8237r.h"

#include "nvs.h"

/* While we read PMBASE dynamically in case it changed, let's
 * initialize it with a sane value
 */
u16 pmbase = DEFAULT_PMBASE;
u8 smm_initialized = 0;

/* GNVS needs to be updated by an 0xEA PM Trap (B2) after it has been located
 * by coreboot.
 */
global_nvs_t *gnvs = (global_nvs_t *)0x0;
void *tcg = (void *)0x0;
void *smi1 = (void *)0x0;

#if 0
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
#endif

/**
 * @brief read and clear SMI_STS
 * @return SMI_STS register
 */
static u16 reset_smi_status(void)
{
	u16 reg16;

	reg16 = inw(pmbase + SMI_STS);
	/* set status bits are cleared by writing 1 to them */
	outw(reg16, pmbase + SMI_STS);

	return reg16;
}

static void dump_smi_status(u16 smi_sts)
{
	printk(BIOS_DEBUG, "SMI_STS: ");
	if (smi_sts & (1 << 15)) printk(BIOS_DEBUG, "GPIO_RANGE_1 ");
	if (smi_sts & (1 << 14)) printk(BIOS_DEBUG, "GPIO_RANGE_0 ");
	if (smi_sts & (1 << 13)) printk(BIOS_DEBUG, "GP3_TIMEOUT ");
	if (smi_sts & (1 << 12)) printk(BIOS_DEBUG, "GP2_TIMEOUT ");
	if (smi_sts & (1 << 11)) printk(BIOS_DEBUG, "SERR_IRQ ");
	if (smi_sts & (1 << 10)) printk(BIOS_DEBUG, "PMIO_5 ");
	if (smi_sts & (1 <<  9)) printk(BIOS_DEBUG, "THRMTRIP# ");
	if (smi_sts & (1 <<  8)) printk(BIOS_DEBUG, "CLKRUN# ");
	if (smi_sts & (1 <<  7)) printk(BIOS_DEBUG, "PRIMARY_IRQ/NMI/SMI ");
	if (smi_sts & (1 <<  6)) printk(BIOS_DEBUG, "SWSMI ");
	if (smi_sts & (1 <<  5)) printk(BIOS_DEBUG, "BIOS_STATUS ");
	if (smi_sts & (1 <<  4)) printk(BIOS_DEBUG, "LEGACY_USB ");
	if (smi_sts & (1 <<  3)) printk(BIOS_DEBUG, "GP1_TIMEOUT ");
	if (smi_sts & (1 <<  2)) printk(BIOS_DEBUG, "GP0_TIMEOUT ");
	if (smi_sts & (1 <<  1)) printk(BIOS_DEBUG, "SECONDARY_EVENT_TIMEOUT ");
	if (smi_sts & (1 <<  0)) printk(BIOS_DEBUG, "PRIMARY_ACTIVITY ");
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

static void southbridge_smi_cmd(unsigned int node, smm_state_save_area_t *state_save)
{
	u16 pmctrl;
	u8 reg8;

	reg8 = inb(pmbase + 0x2f);
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
		pmctrl = inw(pmbase + PM1_CNT);
		pmctrl &= ~SCI_EN;
		outw(pmctrl, pmbase + PM1_CNT);
		printk(BIOS_DEBUG, "SMI#: ACPI disabled.\n");
		break;
	case APM_CNT_ACPI_ENABLE:
		pmctrl = inw(pmbase + PM1_CNT);
		pmctrl |= SCI_EN;
		outw(pmctrl, pmbase + PM1_CNT);
		printk(BIOS_DEBUG, "SMI#: ACPI enabled.\n");
		break;
	case APM_CNT_GNVS_UPDATE:
		if (smm_initialized) {
			printk(BIOS_DEBUG, "SMI#: SMM structures already initialized!\n");
			return;
		}
		gnvs = *(global_nvs_t **)0x500;
		tcg  = *(void **)0x504;
		smi1 = *(void **)0x508;
		smm_initialized = 1;
		printk(BIOS_DEBUG, "SMI#: Setting up structures to %p, %p, %p\n", gnvs, tcg, smi1);
		break;
	default:
		printk(BIOS_DEBUG, "SMI#: Unknown function SMI_CMD=%02x\n", reg8);
	}
}

typedef void (*smi_handler_t)(unsigned int node,
		smm_state_save_area_t *state_save);

smi_handler_t southbridge_smi[32] = {
	NULL,			  //  [0]
	NULL,			  //  [1]
	NULL,			  //  [2]
	NULL,			  //  [3]
	NULL,			  //  [4]
	NULL,			  //  [5]
	southbridge_smi_cmd,	  //  [6]
	NULL,			  //  [7]
	NULL,			  //  [8]
	NULL,			  //  [9]
	NULL,			  // [10]
	NULL,			  // [11]
	NULL,			  // [12]
	NULL,			  // [13]
	NULL,			  // [14]
	NULL,			  // [15]
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
	pmbase = pci_read_config16(PCI_DEV(0, 0x11, 0), 0x88) & 0xfffc;

	/* We need to clear the SMI status registers, or we won't see what's
	 * happening in the following calls.
	 */
	smi_sts = reset_smi_status();

	/* Filter all non-enabled SMI events */
	// FIXME Double check, this clears MONITOR
	// smi_sts &= inl(pmbase + SMI_EN);

	/* Call SMI sub handler for each of the status bits */
	for (i = 0; i < 16; i++) {
		if (smi_sts & (1 << i)) {
			if (southbridge_smi[i])
				southbridge_smi[i](node, state_save);
			else {
				printk(BIOS_DEBUG, "SMI_STS[%d] occured, but no "
						"handler available.\n", i);
				dump = 1;
			}
		}
	}

	if(dump) {
		dump_smi_status(smi_sts);
	}

}
