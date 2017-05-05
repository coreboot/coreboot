/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define __SIMPLE_DEVICE__

#include <console/console.h>
#include <device/pci.h>
#include <lib.h>
#include <soc/gpio.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>

/*
 * These are MMIO ranges that the silicon designers decided are always going to
 * be decoded to LPC.
 */
static const struct lpc_mmio_range {
	uintptr_t base;
	size_t size;
} lpc_fixed_mmio_ranges[] = {
	{ 0xfed40000, 0x8000 },
	{ 0xfedc0000, 0x4000 },
	{ 0xfed20800, 16 },
	{ 0xfed20880, 8 },
	{ 0xfed208e0, 16 },
	{ 0xfed208f0, 8 },
	{ 0xfed30800, 16 },
	{ 0xfed30880, 8 },
	{ 0xfed308e0, 16 },
	{ 0xfed308f0, 8 },
	{ 0, 0 }
};

static const struct pad_config lpc_gpios[] = {
#if IS_ENABLED(CONFIG_SOC_INTEL_GLK)
	PAD_CFG_NF(GPIO_147, UP_20K, DEEP, NF1), /* LPC_ILB_SERIRQ */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_148, UP_20K, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPC_CLKOUT0 */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_149, UP_20K, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPC_CLKOUT1 */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_150, UP_20K, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPC_AD0 */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_151, UP_20K, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPC_AD1 */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_152, UP_20K, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPC_AD2 */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_153, UP_20K, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPC_AD3 */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_154, UP_20K, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPC_CLKRUNB */
	PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_155, UP_20K, DEEP, NF1, HIZCRx1,
		DISPUPD), /* LPC_FRAMEB*/
#else
	PAD_CFG_NF(LPC_ILB_SERIRQ, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_CLKRUNB, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_AD0, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_AD1, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_AD2, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_AD3, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_FRAMEB, NATIVE, DEEP, NF1),
	PAD_CFG_NF(LPC_CLKOUT0, UP_20K, DEEP, NF1),
	PAD_CFG_NF(LPC_CLKOUT1, UP_20K, DEEP, NF1)
#endif
};

void lpc_configure_pads(void)
{
	gpio_configure_pads(lpc_gpios, ARRAY_SIZE(lpc_gpios));
}

void lpc_enable_fixed_io_ranges(uint16_t io_enables)
{
	uint16_t reg_io_enables;

	reg_io_enables = pci_read_config16(PCH_DEV_LPC, REG_IO_ENABLES);
	io_enables |= reg_io_enables;
	pci_write_config16(PCH_DEV_LPC, REG_IO_ENABLES, io_enables);
}

/*
 * Find the first unused IO window.
 * Returns -1 if not found, 0 for reg 0x84, 1 for reg 0x88 ...
 */
static int find_unused_pmio_window(void)
{
	int i;
	uint32_t lgir;

	for (i = 0; i < NUM_GENERIC_IO_RANGES; i++) {
		lgir = pci_read_config32(PCH_DEV_LPC, REG_GENERIC_IO_RANGE(i));

		if (!(lgir & LGIR_EN))
			return i;
	}

	return -1;
}

void lpc_close_pmio_windows(void)
{
	size_t i;

	for (i = 0; i < NUM_GENERIC_IO_RANGES; i++)
		pci_write_config32(PCH_DEV_LPC, REG_GENERIC_IO_RANGE(i), 0);
}

void lpc_open_pmio_window(uint16_t base, uint16_t size)
{
	int lgir_reg_num;
	uint32_t lgir_reg_offset, lgir, window_size, alignment;
	resource_t bridged_size, bridge_base;

	printk(BIOS_SPEW, "LPC: Trying to open IO window from %x size %x\n",
				base, size);

	bridged_size = 0;
	bridge_base = base;

	while (bridged_size < size) {
		lgir_reg_num = find_unused_pmio_window();
		if (lgir_reg_num < 0) {
			printk(BIOS_ERR,
				"LPC: Cannot open IO window: %llx size %llx\n",
				bridge_base, size - bridged_size);
			printk(BIOS_ERR, "No more IO windows\n");
			return;
		}
		lgir_reg_offset = REG_GENERIC_IO_RANGE(lgir_reg_num);

		/* Each IO range register can only open a 256-byte window. */
		window_size = MIN(size, LGIR_MAX_WINDOW_SIZE);

		/* Window size must be a power of two for the AMASK to work. */
		alignment = 1 << (log2_ceil(window_size));
		window_size = ALIGN_UP(window_size, alignment);

		/* Address[15:2] in LGIR[15:12] and Mask[7:2] in LGIR[23:18]. */
		lgir = (bridge_base & LGIR_ADDR_MASK) | LGIR_EN;
		lgir |= ((window_size - 1) << 16) & LGIR_AMASK_MASK;

		pci_write_config32(PCH_DEV_LPC, lgir_reg_offset, lgir);

		printk(BIOS_DEBUG,
		       "LPC: Opened IO window LGIR%d: base %llx size %x\n",
		       lgir_reg_num, bridge_base, window_size);

		bridged_size += window_size;
		bridge_base += window_size;
	}
}

void lpc_open_mmio_window(uintptr_t base, size_t size)
{
	uint32_t lgmr;

	lgmr = pci_read_config32(PCH_DEV_LPC, REG_GENERIC_MEM_RANGE);

	if (lgmr & LGMR_EN) {
		printk(BIOS_ERR,
		       "LPC: Cannot open window to resource %lx size %zx\n",
		       base, size);
		printk(BIOS_ERR, "LPC: MMIO window already in use\n");
		return;
	}

	if (size > LGMR_WINDOW_SIZE) {
		printk(BIOS_WARNING,
		       "LPC:  Resource %lx size %zx larger than window(%x)\n",
		       base, size, LGMR_WINDOW_SIZE);
	}

	lgmr = (base & LGMR_ADDR_MASK) | LGMR_EN;

	pci_write_config32(PCH_DEV_LPC, REG_GENERIC_MEM_RANGE, lgmr);
}

bool lpc_fits_fixed_mmio_window(uintptr_t base, size_t size)
{
	resource_t res_end, range_end;
	const struct lpc_mmio_range *range;

	for (range = lpc_fixed_mmio_ranges; range->size; range++) {
		range_end = range->base + range->size;
		res_end = base + size;

		if ((base >= range->base) && (res_end <= range_end)) {
			printk(BIOS_DEBUG,
			       "Resource %lx size %zx fits in fixed window"
			       " %lx size %zx\n",
			       base, size, range->base, range->size);
			return true;
		}
	}
	return false;
}
