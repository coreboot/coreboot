/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#include <assert.h>
#include <arch/io.h>
#include <arch/cache.h>
#include <arch/exception.h>
#include <arch/hlt.h>
#include <bootblock_common.h>
#include <cbfs.h>
#include <console/console.h>
#include <delay.h>
#include <arch/stages.h>
#include <symbols.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <soc/common.h>

#define CLOCK_BIT_SATA23	BIT30
#define CLOCK_BIT_PNC		BIT29
#define CLOCK_BIT_TDM		BIT25
#define CLOCK_BIT_CRYPTO0_GATE	BIT23
#define CLOCK_BIT_CRYPTO1_GATE	BIT21
#define CLOCK_BIT_CRYPTO1_Z	BIT16
#define CLOCK_BIT_SATA01	BIT15
#define CLOCK_BIT_CRYPTO0_Z	BIT14
#define CLOCK_BIT_BM		BIT13
#define CLOCK_BIT_PCIE2		BIT6
#define CLOCK_BIT_PCIE1		BIT5
#define CLOCK_BIT_GBE0		BIT4
#define CLOCK_BIT_GBE1		BIT3
#define CLOCK_BIT_GBE2		BIT2
#define CLOCK_BIT_AUDIO		BIT0

#define A38x_MPP0_7_OFFSET 0x18000
#define A38x_MPP8_15_OFFSET 0x18004
#define A38x_MPP16_23_OFFSET 0x18008
#define A38x_MPP24_31_OFFSET 0x1800c
#define A38x_MPP32_39_OFFSET 0x18010
#define A38x_MPP40_47_OFFSET 0x18014
#define A38x_MPP48_55_OFFSET 0x18018
#define A38x_MPP56_63_OFFSET 0x1801c

#define A38X_GPP_OUT_ENA_OFFSET_LOW 0x18104
#define A38X_GPP_OUT_ENA_OFFSET_MID 0x18144

#define A38X_GPP_OUT_VALUE_OFFSET_LOW	0x18100
#define A38X_GPP_OUT_VALUE_OFFSET_MID	0x18140

#define A38x_CUBE_BOARD_MPP0_7 0x00001111
#define A38x_CUBE_BOARD_MPP8_15 0x46200000
#define A38x_CUBE_BOARD_MPP16_23 0x00400444
#define A38x_CUBE_BOARD_MPP24_31 0x00043300
#define A38x_CUBE_BOARD_MPP32_39 0x44400000
#define A38x_CUBE_BOARD_MPP40_47 0x00000004
#define A38x_CUBE_BOARD_MPP48_55 0x00444444
#define A38x_CUBE_BOARD_MPP56_63 0x00004444

/* Set bit x to enable GPIO output mode for MPP x */
#define A38x_CUBE_BOARD_0_GPP_OUT_ENA_LOW ~(BIT4 | BIT6)
/* MID group is for MPP32 ~ MPP63 e.g BIT3 corresponds to MPP35 */
#define A38x_CUBE_BOARD_0_GPP_OUT_ENA_MID ~(BIT3)

#define A38x_CUSTOMER_BOARD_0_GPP_OUT_VAL_LOW (BIT4)
/* MID group is for MPP32 ~ MPP63 e.g BIT3 corresponds to MPP35 */
#define A38x_CUSTOMER_BOARD_0_GPP_OUT_VAL_MID (BIT3)

#define A38X_POWER_MANAGEMENT_CLOCK_GATING_CONTROL 0x18220

#define A38x_SOC_IO_ERR_CTRL_OFFSET 0x20200
#define A38x_SOC_WIN_CTRL_OFFSET 0x20250
#define A38x_SOC_WIN_BASE_OFFSET 0x20254

#define A38x_CUBE_BOARD_SOC_IO_ERR_CTRL 0x00000000
#define A38x_CUBE_BOARD_SOC_WIN_CTRL 0x1ff00001
#define A38x_CUBE_BOARD_SOC_BASE_CTRL 0xe0000000

#define DRAM_START ((uintptr_t)_dram / MiB)
#define DRAM_SIZE (CONFIG_DRAM_SIZE_MB)
/* DMA memory for drivers */
#define DMA_START ((uintptr_t)_dma_coherent / MiB)
#define DMA_SIZE (_dma_coherent_size / MiB)

static void setup_pinmux(void)
{
	/* Hard coded pin mux configuration */
	mrvl_reg_write(A38x_MPP0_7_OFFSET, A38x_CUBE_BOARD_MPP0_7);
	mrvl_reg_write(A38x_MPP8_15_OFFSET, A38x_CUBE_BOARD_MPP8_15);
	mrvl_reg_write(A38x_MPP16_23_OFFSET, A38x_CUBE_BOARD_MPP16_23);
	mrvl_reg_write(A38x_MPP24_31_OFFSET, A38x_CUBE_BOARD_MPP24_31);
	mrvl_reg_write(A38x_MPP32_39_OFFSET, A38x_CUBE_BOARD_MPP32_39);
	mrvl_reg_write(A38x_MPP40_47_OFFSET, A38x_CUBE_BOARD_MPP40_47);
	mrvl_reg_write(A38x_MPP48_55_OFFSET, A38x_CUBE_BOARD_MPP48_55);
	mrvl_reg_write(A38x_MPP56_63_OFFSET, A38x_CUBE_BOARD_MPP56_63);
}

static void setup_gpp_out_value(void)
{
	mrvl_reg_write(
		A38X_GPP_OUT_VALUE_OFFSET_LOW,
			A38x_CUSTOMER_BOARD_0_GPP_OUT_VAL_LOW);
	mrvl_reg_write(
		A38X_GPP_OUT_VALUE_OFFSET_MID,
			A38x_CUSTOMER_BOARD_0_GPP_OUT_VAL_MID);
}

static void setup_gpp_out_enable(void)
{
	mrvl_reg_write(
		A38X_GPP_OUT_ENA_OFFSET_LOW,
			A38x_CUBE_BOARD_0_GPP_OUT_ENA_LOW);
	mrvl_reg_write(
		A38X_GPP_OUT_ENA_OFFSET_MID,
			A38x_CUBE_BOARD_0_GPP_OUT_ENA_MID);
}

/* This function disable unused periperal clocks */
static void setup_peripherals_clocks(void)
{
	mrvl_reg_bit_reset(
		A38X_POWER_MANAGEMENT_CLOCK_GATING_CONTROL, (
			CLOCK_BIT_SATA23 | CLOCK_BIT_PNC | CLOCK_BIT_TDM |
			CLOCK_BIT_CRYPTO0_GATE | CLOCK_BIT_CRYPTO1_GATE |
			CLOCK_BIT_CRYPTO1_Z | CLOCK_BIT_SATA01 |
			CLOCK_BIT_CRYPTO0_Z | CLOCK_BIT_BM |
			CLOCK_BIT_PCIE2 | CLOCK_BIT_PCIE1 |
			CLOCK_BIT_GBE0 | CLOCK_BIT_GBE1 |
			CLOCK_BIT_GBE2 | CLOCK_BIT_AUDIO
		)
	);
}

static void setup_win_regs(void)
{
	mrvl_reg_write(A38x_SOC_IO_ERR_CTRL_OFFSET,
			A38x_CUBE_BOARD_SOC_IO_ERR_CTRL);
	mrvl_reg_write(A38x_SOC_WIN_CTRL_OFFSET, A38x_CUBE_BOARD_SOC_WIN_CTRL);
	mrvl_reg_write(A38x_SOC_WIN_BASE_OFFSET, A38x_CUBE_BOARD_SOC_BASE_CTRL);
}

void main(void)
{
	if (CONFIG_BOOTBLOCK_CONSOLE) {
		console_init();
		exception_init();
	}

	init_timer();

	/* enable mmu */
	mmu_init();
	mmu_config_range(0, 4096, DCACHE_OFF);
	mmu_config_range(DRAM_START, DRAM_SIZE, DCACHE_WRITEBACK);
	mmu_config_range(DMA_START, DMA_SIZE, DCACHE_OFF);
	dcache_mmu_enable();

	bootblock_mainboard_init();

	setup_pinmux();
	setup_gpp_out_value();
	setup_gpp_out_enable();
	setup_win_regs();
	setup_peripherals_clocks();
	run_romstage();
}
