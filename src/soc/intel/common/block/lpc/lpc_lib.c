/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016-2018 Intel Corp.
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

#include <assert.h>
#include <console/console.h>
#include <device/pci.h>
#include <intelblocks/lpc_lib.h>
#include <lib.h>
#include "lpc_def.h"
#include <soc/pci_devs.h>

uint16_t lpc_enable_fixed_io_ranges(uint16_t io_enables)
{
	uint16_t reg_io_enables;

	reg_io_enables = pci_read_config16(PCH_DEV_LPC, LPC_IO_ENABLES);
	io_enables |= reg_io_enables;
	pci_write_config16(PCH_DEV_LPC, LPC_IO_ENABLES, io_enables);

	return io_enables;
}

/*
 * Find the first unused IO window.
 * Returns -1 if not found, 0 for reg 0x84, 1 for reg 0x88 ...
 */
static int find_unused_pmio_window(void)
{
	int i;
	uint32_t lgir;

	for (i = 0; i < LPC_NUM_GENERIC_IO_RANGES; i++) {
		lgir = pci_read_config32(PCH_DEV_LPC, LPC_GENERIC_IO_RANGE(i));

		if (!(lgir & LPC_LGIR_EN))
			return i;
	}

	return -1;
}

void lpc_close_pmio_windows(void)
{
	size_t i;

	for (i = 0; i < LPC_NUM_GENERIC_IO_RANGES; i++)
		pci_write_config32(PCH_DEV_LPC, LPC_GENERIC_IO_RANGE(i), 0);
}

void lpc_open_pmio_window(uint16_t base, uint16_t size)
{
	int i, lgir_reg_num;
	uint32_t lgir_reg_offset, lgir, window_size, alignment;
	resource_t bridged_size, bridge_base;

	printk(BIOS_SPEW, "LPC: Trying to open IO window from %x size %x\n",
				base, size);

	bridged_size = 0;
	bridge_base = base;

	while (bridged_size < size) {
		/* Each IO range register can only open a 256-byte window. */
		window_size = MIN(size, LPC_LGIR_MAX_WINDOW_SIZE);

		/* Window size must be a power of two for the AMASK to work. */
		alignment = 1UL << (log2_ceil(window_size));
		window_size = ALIGN_UP(window_size, alignment);

		/* Address[15:2] in LGIR[15:12] and Mask[7:2] in LGIR[23:18]. */
		lgir = (bridge_base & LPC_LGIR_ADDR_MASK) | LPC_LGIR_EN;
		lgir |= ((window_size - 1) << 16) & LPC_LGIR_AMASK_MASK;

		/* Skip programming if same range already programmed. */
		for (i = 0; i < LPC_NUM_GENERIC_IO_RANGES; i++) {
			if (lgir == pci_read_config32(PCH_DEV_LPC,
						LPC_GENERIC_IO_RANGE(i)))
				return;
		}

		lgir_reg_num = find_unused_pmio_window();
		if (lgir_reg_num < 0) {
			printk(BIOS_ERR,
				"LPC: Cannot open IO window: %llx size %llx\n",
				bridge_base, size - bridged_size);
			printk(BIOS_ERR, "No more IO windows\n");
			return;
		}
		lgir_reg_offset = LPC_GENERIC_IO_RANGE(lgir_reg_num);

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

	lgmr = pci_read_config32(PCH_DEV_LPC, LPC_GENERIC_MEM_RANGE);

	if (lgmr & LPC_LGMR_EN) {
		printk(BIOS_ERR,
		       "LPC: Cannot open window to resource %lx size %zx\n",
		       base, size);
		printk(BIOS_ERR, "LPC: MMIO window already in use\n");
		return;
	}

	if (size > LPC_LGMR_WINDOW_SIZE) {
		printk(BIOS_WARNING,
		       "LPC:  Resource %lx size %zx larger than window(%x)\n",
		       base, size, LPC_LGMR_WINDOW_SIZE);
	}

	lgmr = (base & LPC_LGMR_ADDR_MASK) | LPC_LGMR_EN;

	pci_write_config32(PCH_DEV_LPC, LPC_GENERIC_MEM_RANGE, lgmr);
}

bool lpc_fits_fixed_mmio_window(uintptr_t base, size_t size)
{
	resource_t res_end, range_end;
	const struct lpc_mmio_range *range;
	const struct lpc_mmio_range *lpc_fixed_mmio_ranges =
		soc_get_fixed_mmio_ranges();

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

/*
 * Set FAST_SPIBAR BIOS Control register based on input bit field.
 */
static void lpc_set_bios_control_reg(uint8_t bios_cntl_bit)
{
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev = PCH_DEV_LPC;
#else
	struct device *dev = PCH_DEV_LPC;
#endif
	uint8_t bc_cntl;

	assert(IS_POWER_OF_2(bios_cntl_bit));
	bc_cntl = pci_read_config8(dev, LPC_BIOS_CNTL);
	bc_cntl |= bios_cntl_bit;
	pci_write_config8(dev, LPC_BIOS_CNTL, bc_cntl);

	/*
	* Ensure an additional read back after performing lock down
	*/
	pci_read_config8(PCH_DEV_LPC, LPC_BIOS_CNTL);
}

/*
* Set LPC BIOS Control BILD bit.
*/
void lpc_set_bios_interface_lock_down(void)
{
	lpc_set_bios_control_reg(LPC_BC_BILD);
}

/*
* Set LPC BIOS Control LE bit.
*/
void lpc_set_lock_enable(void)
{
	lpc_set_bios_control_reg(LPC_BC_LE);
}

/*
* Set LPC BIOS Control EISS bit.
*/
void lpc_set_eiss(void)
{
	lpc_set_bios_control_reg(LPC_BC_EISS);
}

/*
* Set LPC Serial IRQ mode.
*/
void lpc_set_serirq_mode(enum serirq_mode mode)
{
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev = PCH_DEV_LPC;
#else
	struct device *dev = PCH_DEV_LPC;
#endif
	uint8_t scnt;

	scnt = pci_read_config8(dev, LPC_SERIRQ_CTL);
	scnt &= ~(LPC_SCNT_EN | LPC_SCNT_MODE);

	switch (mode) {
	case SERIRQ_QUIET:
		scnt |= LPC_SCNT_EN;
		break;
	case SERIRQ_CONTINUOUS:
		scnt |= LPC_SCNT_EN | LPC_SCNT_MODE;
		break;
	case SERIRQ_OFF:
	default:
		break;
	}

	pci_write_config8(dev, LPC_SERIRQ_CTL, scnt);
}


void lpc_io_setup_comm_a_b(void)
{
	/* ComA Range 3F8h-3FFh [2:0] */
	uint16_t com_ranges = LPC_IOD_COMA_RANGE;
	uint16_t com_enable = LPC_IOE_COMA_EN;

	/* ComB Range 2F8h-2FFh [6:4] */
	if (IS_ENABLED(CONFIG_SOC_INTEL_COMMON_BLOCK_LPC_COMB_ENABLE)) {
		com_ranges |= LPC_IOD_COMB_RANGE;
		com_enable |= LPC_IOE_COMB_EN;
	}

	/* Setup I/O Decode Range Register for LPC */
	pci_write_config16(PCH_DEV_LPC, LPC_IO_DECODE, com_ranges);
	/* Enable ComA and ComB Port */
	lpc_enable_fixed_io_ranges(com_enable);
}

static void lpc_set_gen_decode_range(
	uint32_t gen_io_dec[LPC_NUM_GENERIC_IO_RANGES])
{
	size_t i;

	/* Set in PCI generic decode range registers */
	for (i = 0; i < LPC_NUM_GENERIC_IO_RANGES; i++)
		pci_write_config32(PCH_DEV_LPC, LPC_GENERIC_IO_RANGE(i),
			gen_io_dec[i]);
}

static void pch_lpc_interrupt_init(void)
{
	const struct device *dev;

	dev = dev_find_slot(0, PCI_DEVFN(PCH_DEV_SLOT_LPC, 0));
	if (!dev || !dev->chip_info)
		return;

	soc_pch_pirq_init(dev);
}

void pch_enable_lpc(void)
{
	/* Lookup device tree in romstage */
	const struct device *dev;
	uint32_t gen_io_dec[LPC_NUM_GENERIC_IO_RANGES];

	dev = dev_find_slot(0, PCI_DEVFN(PCH_DEV_SLOT_LPC, 0));
	if (!dev || !dev->chip_info)
		return;

	soc_get_gen_io_dec_range(dev, gen_io_dec);
	lpc_set_gen_decode_range(gen_io_dec);
	soc_setup_dmi_pcr_io_dec(gen_io_dec);
	if (ENV_RAMSTAGE)
		pch_lpc_interrupt_init();
}

void lpc_enable_pci_clk_cntl(void)
{
	pci_write_config8(PCH_DEV_LPC, LPC_PCCTL, LPC_PCCTL_CLKRUN_EN);
}
