/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <timestamp.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <cbmem.h>
#include <console/console.h>
#include <halt.h>
#include <program_loading.h>
#include <reset.h>
#include <superio/smsc/sio1007/chip.h>
#include <fsp_util.h>
#include <northbridge/intel/fsp_sandybridge/northbridge.h>
#include <northbridge/intel/fsp_sandybridge/raminit.h>
#include <southbridge/intel/fsp_bd82x6x/pch.h>
#include <southbridge/intel/fsp_bd82x6x/gpio.h>
#include <southbridge/intel/fsp_bd82x6x/me.h>
#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include "gpio.h"

#define SIO_PORT 0x164e

static inline void reset_system(void)
{
	hard_reset();
	halt();
}

static void pch_enable_lpc(void)
{
	pci_devfn_t dev = PCH_LPC_DEV;

	/* Set COM1/COM2 decode range */
	pci_write_config16(dev, LPC_IO_DEC, 0x0010);

	/* Enable SuperIO + PS/2 Keyboard/Mouse */
	u16 lpc_config = CNF1_LPC_EN | CNF2_LPC_EN | KBC_LPC_EN;
	pci_write_config16(dev, LPC_EN, lpc_config);

	/* Map 256 bytes at 0x1600 to the LPC bus. */
	pci_write_config32(dev, LPC_GEN1_DEC, 0xfc1601);

	/* Map a range for the runtime registers to the LPC bus. */
	pci_write_config32(dev, LPC_GEN2_DEC, 0xc0181);

	if (sio1007_enable_uart_at(SIO_PORT)) {
		pci_write_config16(dev, LPC_EN,
				   lpc_config | COMA_LPC_EN);
	}
}

static void setup_sio_gpios(void)
{
	const u16 port = SIO_PORT;
	const u16 runtime_port = 0x180;

	/* Turn on configuration mode. */
	outb(0x55, port);

	/* Set the GPIO direction, polarity, and type. */
	sio1007_setreg(port, 0x31, 1 << 0, 1 << 0);
	sio1007_setreg(port, 0x32, 0 << 0, 1 << 0);
	sio1007_setreg(port, 0x33, 0 << 0, 1 << 0);

	/* Set the base address for the runtime register block. */
	sio1007_setreg(port, 0x30, runtime_port >> 4, 0xff);
	sio1007_setreg(port, 0x21, runtime_port >> 12, 0xff);

	/* Turn on address decoding for it. */
	sio1007_setreg(port, 0x3a, 1 << 1, 1 << 1);

	/*
	 * Enable the RS232 transiver.
	 * Set the value of GPIO 10 by changing GP1, bit 0.
	 */
	u8 byte;
	byte = inb(runtime_port + 0xc);
	byte |= (1 << 0);
	outb(byte, runtime_port + 0xc);

	/* Turn off address decoding for it. */
	sio1007_setreg(port, 0x3a, 0 << 1, 1 << 1);

	/* Turn off configuration mode. */
	outb(0xaa, port);
}

static void rcba_config(void)
{
	u32 reg32;

	/*
	 *             GFX    INTA -> PIRQA (MSI)
	 * D31IP_SIP   SATA   INTB -> PIRQD
	 * D31IP_SMIP  SMBUS  INTC -> PIRQC
	 * D31IP_SIP   SATA2  INTB -> PIRQD
	 * D31IP_TTIP  THRT   INTC -> PIRQC
	 * D29IP_E1P   EHCI1  INTA -> PIRQD
	 * D28IP_P1IP         INTA -> PIRQD
	 * D28IP_P2IP         INTB -> PIRQC
	 * D28IP_P3IP         INTC -> PIRQB
	 * D28IP_P4IP         INTD -> PIRQA
	 * D28IP_P5IP         INTA -> PIRQD
	 * D28IP_P6IP         INTB -> PIRQC
	 * D28IP_P7IP         INTC -> PIRQB
	 * D28IP_P8IP         INTD -> PIRQA
	 * D27IP_ZIP   HDA    INTA -> PIRQD
	 * D26IP_E2P   EHCI2  INTA -> PIRQD
	 * D20IP_XHCI  XHCI   INTA -> PIRQD (MSI)
	 */

	/* Device interrupt pin register (board specific) */
	RCBA32(D31IP) = (INTC << D31IP_TTIP) | (INTB << D31IP_SIP2) |
		(INTC << D31IP_SMIP) | (INTB << D31IP_SIP);
	RCBA32(D29IP) = (INTA << D29IP_E1P);
	RCBA32(D28IP) = (INTD << D28IP_P8IP) | (INTC << D28IP_P7IP) |
		(INTB << D28IP_P6IP) | (INTA << D28IP_P5IP) |
		(INTD << D28IP_P4IP) | (INTC << D28IP_P3IP) |
		(INTB << D28IP_P2IP) | (INTA << D28IP_P1IP);
	RCBA32(D27IP) = (INTA << D27IP_ZIP);
	RCBA32(D26IP) = (INTA << D26IP_E2P);
	RCBA32(D25IP) = (INTA << D25IP_LIP);
	RCBA32(D22IP) = (INTB << D22IP_KTIP) | (INTC << D22IP_IDERIP) |
		(INTB << D22IP_MEI2IP) | (INTA << D22IP_MEI1IP);
	RCBA32(D20IP) = (INTA << D20IP_XHCIIP);

	/* Device interrupt route registers */
	DIR_ROUTE(D31IR, PIRQA, PIRQD, PIRQC, PIRQA);
	DIR_ROUTE(D29IR, PIRQH, PIRQD, PIRQA, PIRQC);
	DIR_ROUTE(D28IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D27IR, PIRQG, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D26IR, PIRQF, PIRQA, PIRQC, PIRQD);
	DIR_ROUTE(D25IR, PIRQE, PIRQF, PIRQG, PIRQH);
	DIR_ROUTE(D22IR, PIRQA, PIRQD, PIRQC, PIRQB);
	DIR_ROUTE(D20IR, PIRQD, PIRQE, PIRQF, PIRQG);

	/* Enable IOAPIC (generic) */
	RCBA16(OIC) = 0x0100;
	/* PCH BWG says to read back the IOAPIC enable register */
	(void) RCBA16(OIC);

	/* Disable unused devices (board specific) */
	reg32 = RCBA32(FD);
	reg32 |= PCH_DISABLE_ALWAYS;
	RCBA32(FD) = reg32;
}

void main(FSP_INFO_HEADER *fsp_info_header)
{
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
	int boot_mode = 0;
#endif
	u32 pm1_cnt;
	u16 pm1_sts;

	post_code(0x40);

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	pch_enable_lpc();

	/* Enable GPIOs */
	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE|1);
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);
	setup_pch_gpios(&gpio_map);
	setup_sio_gpios();

	console_init();
	post_code(0x41);

	post_code(0x42);
	sandybridge_sb_early_initialization();

	post_code(0x43);
	sandybridge_early_initialization(SANDYBRIDGE_MOBILE);
	printk(BIOS_DEBUG, "Back from sandybridge_early_initialization()\n");

	post_code(0x44);
	/* Wait for ME to be ready */
	intel_early_me_status();
	intel_early_me_init();
	intel_early_me_uma_size();

	post_code(0x45);
	/* Check PM1_STS[15] to see if we are waking from Sx */
	pm1_sts = inw(DEFAULT_PMBASE + PM1_STS);

	/* Read PM1_CNT[12:10] to determine which Sx state */
	pm1_cnt = inl(DEFAULT_PMBASE + PM1_CNT);
	post_code(0x46);
	if ((pm1_sts & WAK_STS) && ((pm1_cnt >> 10) & 7) == 5) {
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
		printk(BIOS_DEBUG, "Resume from S3 detected.\n");
		boot_mode = 2;
		/* Clear SLP_TYPE. This will break stage2 but
		 * we care for that when we get there.
		 */
		outl(pm1_cnt & ~(7 << 10), DEFAULT_PMBASE + PM1_CNT);
#else
		printk(BIOS_DEBUG, "Resume from S3 detected, but disabled.\n");
#endif
	}

	post_code(0x48);

	timestamp_add_now(TS_BEFORE_INITRAM);

  /*
   * Call early init to initialize memory and chipset. This function returns
   * to the romstage_main_continue function with a pointer to the HOB
   * structure.
   */
	printk(BIOS_DEBUG, "Starting the Intel FSP (early_init)\n");
	fsp_early_init(fsp_info_header);
	die("Uh Oh! fsp_early_init should not return here.\n");
}

/*******************************************************************************
 * The FSP early_init function returns to this function.
 * Memory is setup and the stack is set by the FSP.
 ******************************************************************************/
void romstage_main_continue(EFI_STATUS status, VOID *HobListPtr) {
	int cbmem_was_initted;
	u32 reg32;
	void *cbmem_hob_ptr;

	timestamp_add_now(TS_AFTER_INITRAM);

	/*
	 * HD AUDIO is not used on this system, so we're using some registers
	 * in there as temporary registers to save TSC values.  This is complete
	 * now, so disable the audio block.
	 */
	reg32 = RCBA32(FD);
	reg32 |= PCH_DISABLE_HD_AUDIO;
	RCBA32(FD) = reg32;

	post_code(0x49);

#if IS_ENABLED(CONFIG_USBDEBUG)
	/* FSP reconfigures USB, so reinit it to have debug */
	early_usbdebug_init();
#endif

	/* For reference print FSP version */
	u32 version = MCHBAR32(0x5034);
	printk(BIOS_DEBUG, "FSP Version %d.%d.%d Build %d\n",
		version >> 24 , (version >> 16) & 0xff,
		(version >> 8) & 0xff, version & 0xff);
	printk(BIOS_DEBUG, "FSP Status: 0x%0x\n", (u32)status);

	intel_early_me_init_done(ME_INIT_STATUS_SUCCESS);

	printk(BIOS_SPEW, "FD & FD2 Settings:\n");
	display_fd_settings();

	report_memory_config();

	post_code(0x4b);

	early_pch_init();
	post_code(0x4c);

	rcba_config();
	post_code(0x4d);

	quick_ram_check();
	post_code(0x4e);

	cbmem_was_initted = !cbmem_recovery(0);

	if(cbmem_was_initted) {
		reset_system();
	}

	/* Save the HOB pointer in CBMEM to be used in ramstage. */
	cbmem_hob_ptr = cbmem_add(CBMEM_ID_HOB_POINTER, sizeof(*HobListPtr));
	if (cbmem_hob_ptr == NULL)
		die("Could not allocate cbmem for HOB pointer");
	*(u32*)cbmem_hob_ptr = (u32)HobListPtr;
	post_code(0x4f);

	/* Load the ramstage. */
	run_ramstage();
	while (1);
}

void romstage_fsp_rt_buffer_callback(FSP_INIT_RT_BUFFER *FspRtBuffer)
{
	/* No overrides needed */
	return;
}

uint64_t get_initial_timestamp(void)
{
	return (uint64_t) pci_read_config32(PCI_DEV(0, 0x1f, 2), 0xd0) << 4;
}
