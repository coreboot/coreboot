/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
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

// __PRE_RAM__ means: use "unsigned" for device, not a struct.

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <arch/acpi.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/tsc.h>
#include <cpu/intel/romstage.h>
#include <cbmem.h>
#include <lib.h>
#include <romstage_handoff.h>
#include <console/console.h>
#include <southbridge/intel/i82801ix/i82801ix.h>
#include <northbridge/intel/gm45/gm45.h>
#include <drivers/lenovo/hybrid_graphics/hybrid_graphics.h>
#include <timestamp.h>
#include "dock.h"
#include "gpio.h"

#define LPC_DEV PCI_DEV(0, 0x1f, 0)
#define MCH_DEV PCI_DEV(0, 0, 0)

static void hybrid_graphics_init(sysinfo_t *sysinfo)
{
	bool peg, igd;

	early_hybrid_graphics(&igd, &peg);

	sysinfo->enable_igd = igd;
	sysinfo->enable_peg = peg;
}

static void early_lpc_setup(void)
{
	/* Set up SuperIO LPC forwards */

	/* Configure serial IRQs.*/
	pci_write_config8(LPC_DEV, D31F0_SERIRQ_CNTL, 0xd0);
	/* Map COMa on 0x3f8, COMb on 0x2f8. */
	pci_write_config16(LPC_DEV, D31F0_LPC_IODEC, 0x0010);
	pci_write_config16(LPC_DEV, D31F0_LPC_EN, 0x3f0f);
	pci_write_config32(LPC_DEV, D31F0_GEN1_DEC, 0x7c1601);
	pci_write_config32(LPC_DEV, D31F0_GEN2_DEC, 0xc15e1);
	pci_write_config32(LPC_DEV, D31F0_GEN3_DEC, 0x1c1681);
}

void mainboard_romstage_entry(unsigned long bist)
{
	sysinfo_t sysinfo;
	int s3resume = 0;
	int cbmem_initted;
	u16 reg16;

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	/* basic northbridge setup, including MMCONF BAR */
	gm45_early_init();

	if (bist == 0)
		enable_lapic();

	/* First, run everything needed for console output. */
	i82801ix_early_init();
	early_lpc_setup();

	dock_connect();

	console_init();
	printk(BIOS_DEBUG, "running main(bist = %lu)\n", bist);

	reg16 = pci_read_config16(LPC_DEV, D31F0_GEN_PMCON_3);
	pci_write_config16(LPC_DEV, D31F0_GEN_PMCON_3, reg16);
	if ((MCHBAR16(SSKPD_MCHBAR) == 0xCAFE) && !(reg16 & (1 << 9))) {
		printk(BIOS_DEBUG, "soft reset detected, rebooting properly\n");
		gm45_early_reset();
	}

	setup_pch_gpios(&t400_gpio_map);

	/* ASPM related setting, set early by original BIOS. */
	DMIBAR16(0x204) &= ~(3 << 10);

	/* Check for S3 resume. */
	const u32 pm1_cnt = inl(DEFAULT_PMBASE + 0x04);
	if (((pm1_cnt >> 10) & 7) == 5) {
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
		printk(BIOS_DEBUG, "Resume from S3 detected.\n");
		s3resume = 1;
		/* Clear SLP_TYPE. This will break stage2 but
		 * we care for that when we get there.
		 */
		outl(pm1_cnt & ~(7 << 10), DEFAULT_PMBASE + 0x04);
#else
		printk(BIOS_DEBUG, "Resume from S3 detected, but disabled.\n");
#endif
	}

	/* RAM initialization */
	enter_raminit_or_reset();
	memset(&sysinfo, 0, sizeof(sysinfo));
	sysinfo.spd_map[0] = 0x50;
	sysinfo.spd_map[2] = 0x51;
	get_gmch_info(&sysinfo);

	/* Configure graphic GPIOs.
	* Make sure there's a little delay between
	* setup_pch_gpios() and this call ! */
	hybrid_graphics_init(&sysinfo);

	raminit(&sysinfo, s3resume);

	const u32 deven = pci_read_config32(MCH_DEV, D0F0_DEVEN);
	/* Disable D4F0 (unknown signal controller). */
	pci_write_config32(MCH_DEV, D0F0_DEVEN, deven & ~0x4000);

	init_pm(&sysinfo, 0);

	i82801ix_dmi_setup();
	gm45_late_init(sysinfo.stepping);
	i82801ix_dmi_poll_vc1();

	MCHBAR16(SSKPD_MCHBAR) = 0xCAFE;
	/* Enable ethernet.  */
	RCBA32(0x3414) &= ~0x20;

	RCBA32(0x0238) = 0x00543210;
	RCBA32(0x0240) = 0x009c0b02;
	RCBA32(0x0244) = 0x00a20b1a;
	RCBA32(0x0248) = 0x005402cb;
	RCBA32(0x0254) = 0x00470966;
	RCBA32(0x0258) = 0x00470473;
	RCBA32(0x0260) = 0x00e90825;
	RCBA32(0x0278) = 0x00bc0efb;
	RCBA32(0x027c) = 0x00c00f0b;
	RCBA32(0x0280) = 0x00670000;
	RCBA32(0x0284) = 0x006d0000;
	RCBA32(0x0288) = 0x00600b4e;
	RCBA32(0x1e10) = 0x00020800;
	RCBA32(0x1e18) = 0x36ea00a0;
	RCBA32(0x1e80) = 0x000c0801;
	RCBA32(0x1e84) = 0x000200f0;
	RCBA32(0x2028) = 0x04c8f95e;
	RCBA32(0x202c) = 0x055c095e;
	RCBA32(0x204c) = 0x001ffc00;
	RCBA32(0x2050) = 0x00100fff;
	RCBA32(0x2090) = 0x37000000;
	RCBA32(0x20b0) = 0x0c000000;
	RCBA32(0x20d0) = 0x09000000;
	RCBA32(0x20f0) = 0x05000000;
	RCBA32(0x3400) = 0x0000001c;
	RCBA32(0x3410) = 0x00100461;
	RCBA32(0x3414) = 0x00000000;
	RCBA32(0x341c) = 0xbf4f001f;
	RCBA32(0x3420) = 0x00000000;
	RCBA32(0x3430) = 0x00000001;

	init_iommu();

	/* FIXME: make a proper SMBUS mux support.  */
	outl(inl(DEFAULT_GPIOBASE + 0x38) & ~0x400, DEFAULT_GPIOBASE + 0x38);

	cbmem_initted = !cbmem_recovery(s3resume);

	romstage_handoff_init(cbmem_initted && s3resume);

	printk(BIOS_SPEW, "exit main()\n");
}
