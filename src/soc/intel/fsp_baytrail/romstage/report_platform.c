/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <console/console.h>
#include <arch/io.h>
#include <baytrail/iosf.h>
#include <baytrail/romstage.h>
#include <cpu/x86/msr.h>
#include <baytrail/msr.h>
#include <cpu/x86/name.h>

static void print_dram_info(void)
{
	const int mrc_ver_reg = 0xf0;
	const uint32_t soc_dev = PCI_DEV(0, SOC_DEV, SOC_FUNC);
	uint32_t reg;
	int num_channels;
	int speed;
	uint32_t ch0;
	uint32_t ch1;

	reg = pci_read_config32(soc_dev, mrc_ver_reg);

	printk(BIOS_INFO, "MRC v%d.%02d\n", (reg >> 8) & 0xff, reg & 0xff);

	/* Number of channels enabled and DDR3 type. Determine number of
	 * channels by the keying of the rank enable bits [3:0]. * */
	ch0 = iosf_dunit_ch0_read(DRP);
	ch1 = iosf_dunit_ch1_read(DRP);
	num_channels = 0;
	if (ch0 & DRP_RANK_MASK)
		num_channels++;
	if (ch1 & DRP_RANK_MASK)
		num_channels++;

	printk(BIOS_INFO, "%d channels of %sDDR3 @ ", num_channels,
	       (reg & (1 << 22)) ? "LP" : "");

	/* DRAM frequency -- all channels run at same frequency. */
	reg = iosf_dunit_read(DTR0);
	switch (reg & 0x3) {
	case 0:
		speed = 800; break;
	case 1:
		speed = 1066; break;
	case 2:
		speed = 1333; break;
	case 3:
		speed = 1600; break;
	}
	printk(BIOS_INFO, "%dMHz\n", speed);
}

#define VARIANT_ID_BYTE	18
#define VARIANT_ID_MASK 7
void report_platform_info(void)
{
	const char *baytrail_variants[4] = {
		"Bay Trail-I (ISG/embedded)",
		"Bay Trail-T (Tablet)",
		"Bay Trail-D (Desktop)",
		"Bay Trail-M (Mobile)",
	};
	msr_t platform_id = rdmsr(MSR_IA32_PLATFORM_ID);
	uint8_t variant = (platform_id.hi >> VARIANT_ID_BYTE) & VARIANT_ID_MASK;

	printk(BIOS_INFO, "Baytrail Chip Variant: %s\n", variant < 4 ?
	       baytrail_variants[variant] : "Unknown");
	print_dram_info();

}
