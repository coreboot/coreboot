/*
 * This file is part of the coreboot project.
 *
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

#include <soc/clk.h>
#include <soc/dmc.h>
#include <soc/gpio.h>
#include <soc/setup.h>
#include <stddef.h>

const struct mem_timings mem_timings = {
		.mem_manuf = MEM_MANUF_SAMSUNG,
		.mem_type = DDR_MODE_DDR3,
		.frequency_mhz = 800,
		.direct_cmd_msr = {
			0x00020018, 0x00030000, 0x00010046, 0x00000d70,
			0x00000c70
		},
		.timing_ref = 0x000000bb,
		.timing_row = 0x6836650f,
		.timing_data = 0x3630580b,
		.timing_power = 0x41000a26,
		.phy0_dqs = 0x08080808,
		.phy1_dqs = 0x08080808,
		.phy0_dq = 0x08080808,
		.phy1_dq = 0x08080808,
		.phy0_tFS = 0x8,
		.phy1_tFS = 0x8,
		.phy0_pulld_dqs = 0xf,
		.phy1_pulld_dqs = 0xf,

		.lpddr3_ctrl_phy_reset = 0x1,
		.ctrl_start_point = 0x10,
		.ctrl_inc = 0x10,
		.ctrl_start = 0x1,
		.ctrl_dll_on = 0x1,
		.ctrl_ref = 0x8,

		.ctrl_force = 0x1a,
		.ctrl_rdlat = 0x0b,
		.ctrl_bstlen = 0x08,

		.fp_resync = 0x8,
		.iv_size = 0x7,
		.dfi_init_start = 1,
		.aref_en = 1,

		.rd_fetch = 0x3,

		.zq_mode_dds = 0x7,
		.zq_mode_term = 0x1,
		.zq_mode_noterm = 1,

		/*
		* Dynamic Clock: Always Running
		* Memory Burst length: 8
		* Number of chips: 1
		* Memory Bus width: 32 bit
		* Memory Type: DDR3
		* Additional Latancy for PLL: 0 Cycle
		*/
		.memcontrol = DMC_MEMCONTROL_CLK_STOP_DISABLE |
			DMC_MEMCONTROL_DPWRDN_DISABLE |
			DMC_MEMCONTROL_DPWRDN_ACTIVE_PRECHARGE |
			DMC_MEMCONTROL_DSREF_DISABLE |
			DMC_MEMCONTROL_ADD_LAT_PALL_CYCLE(0) |
			DMC_MEMCONTROL_MEM_TYPE_DDR3 |
			DMC_MEMCONTROL_MEM_WIDTH_32BIT |
			DMC_MEMCONTROL_NUM_CHIP_1 |
			DMC_MEMCONTROL_BL_8 |
			DMC_MEMCONTROL_PZQ_DISABLE |
			DMC_MEMCONTROL_MRR_BYTE_7_0,
		/*
		 * For channel interleaving, the chip_base needs to be set to
		 * half the bus address. So for a base address of 0x2000_0000,
		 * the chip_base value is 0x20 without interleaving and 0x10
		 * with channel interleaving. See note in section 17.14.
		 */
		.membaseconfig0 = (0x10 << 16) | DMC_CHIP_MASK_1GB,
		.memconfig = DMC_MEMCONFIG_CHIP_MAP_SPLIT |
			DMC_MEMCONFIGx_CHIP_COL_10 |
			DMC_MEMCONFIGx_CHIP_ROW_15 |
			DMC_MEMCONFIGx_CHIP_BANK_8,
		.prechconfig_tp_cnt = 0xff,
		.dpwrdn_cyc = 0xff,
		.dsref_cyc = 0xffff,
		.concontrol = DMC_CONCONTROL_DFI_INIT_START_DISABLE |
			DMC_CONCONTROL_TIMEOUT_LEVEL0 |
			DMC_CONCONTROL_RD_FETCH_DISABLE |
			DMC_CONCONTROL_AREF_EN_DISABLE |
			DMC_CONCONTROL_IO_PD_CON_DISABLE,
		.dmc_channels = 1,
		.chips_per_channel = 1,
		.chips_to_configure = 1,
		.send_zq_init = 1,
		.gate_leveling_enable = 1,
};
