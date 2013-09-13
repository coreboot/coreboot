/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Samsung Electronics
 * Copyright 2013 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stddef.h>
#include <stdlib.h>
#include <console/console.h>

#include <cpu/samsung/exynos5250/gpio.h>
#include <cpu/samsung/exynos5250/setup.h>
#include <cpu/samsung/exynos5250/dmc.h>
#include <cpu/samsung/exynos5250/clk.h>

const struct mem_timings mem_timings[] = {
	{
		.mem_manuf = MEM_MANUF_ELPIDA,
		.mem_type = DDR_MODE_DDR3,
		.frequency_mhz = 800,
		.mpll_mdiv = 0x64,
		.mpll_pdiv = 0x3,
		.mpll_sdiv = 0x0,
		.cpll_mdiv = 0xde,
		.cpll_pdiv = 0x4,
		.cpll_sdiv = 0x2,
		.gpll_mdiv = 0x215,
		.gpll_pdiv = 0xc,
		.gpll_sdiv = 0x1,
		.epll_mdiv = 0x60,
		.epll_pdiv = 0x3,
		.epll_sdiv = 0x3,
		.vpll_mdiv = 0x96,
		.vpll_pdiv = 0x3,
		.vpll_sdiv = 0x2,

		.bpll_mdiv = 0x64,
		.bpll_pdiv = 0x3,
		.bpll_sdiv = 0x0,
		.use_bpll = 0,
		.pclk_cdrex_ratio = 0x5,
		.direct_cmd_msr = {
			0x00020018, 0x00030000, 0x00010042, 0x00000d70
		},
		.timing_ref = 0x000000bb,
		.timing_row = 0x8c36660f,
		.timing_data = 0x3630580b,
		.timing_power = 0x41000a44,
		.phy0_dqs = 0x08080808,
		.phy1_dqs = 0x08080808,
		.phy0_dq = 0x08080808,
		.phy1_dq = 0x08080808,
		.phy0_tFS = 0x4,
		.phy1_tFS = 0x4,
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
		.zq_mode_noterm = 0,

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
			DMC_MEMCONTROL_TP_DISABLE |
			DMC_MEMCONTROL_DSREF_ENABLE |
			DMC_MEMCONTROL_ADD_LAT_PALL_CYCLE(0) |
			DMC_MEMCONTROL_MEM_TYPE_DDR3 |
			DMC_MEMCONTROL_MEM_WIDTH_32BIT |
			DMC_MEMCONTROL_NUM_CHIP_1 |
			DMC_MEMCONTROL_BL_8 |
			DMC_MEMCONTROL_PZQ_DISABLE |
			DMC_MEMCONTROL_MRR_BYTE_7_0,
		.memconfig = DMC_MEMCONFIGx_CHIP_MAP_INTERLEAVED |
			DMC_MEMCONFIGx_CHIP_COL_10 |
			DMC_MEMCONFIGx_CHIP_ROW_15 |
			DMC_MEMCONFIGx_CHIP_BANK_8,
		.membaseconfig0 = DMC_MEMBASECONFIG_VAL(0x40),
		.membaseconfig1 = DMC_MEMBASECONFIG_VAL(0x80),
		.prechconfig_tp_cnt = 0xff,
		.dpwrdn_cyc = 0xff,
		.dsref_cyc = 0xffff,
		.concontrol = DMC_CONCONTROL_DFI_INIT_START_DISABLE |
			DMC_CONCONTROL_TIMEOUT_LEVEL0 |
			DMC_CONCONTROL_RD_FETCH_DISABLE |
			DMC_CONCONTROL_EMPTY_DISABLE |
			DMC_CONCONTROL_AREF_EN_DISABLE |
			DMC_CONCONTROL_IO_PD_CON_DISABLE,
		.dmc_channels = 2,
		.chips_per_channel = 2,
		.chips_to_configure = 1,
		.send_zq_init = 1,
		.impedance = IMP_OUTPUT_DRV_30_OHM,
		.gate_leveling_enable = 0,
	}, {
		.mem_manuf = MEM_MANUF_SAMSUNG,
		.mem_type = DDR_MODE_DDR3,
		.frequency_mhz = 800,
		.mpll_mdiv = 0x64,
		.mpll_pdiv = 0x3,
		.mpll_sdiv = 0x0,
		.cpll_mdiv = 0xde,
		.cpll_pdiv = 0x4,
		.cpll_sdiv = 0x2,
		.gpll_mdiv = 0x215,
		.gpll_pdiv = 0xc,
		.gpll_sdiv = 0x1,
		.epll_mdiv = 0x60,
		.epll_pdiv = 0x3,
		.epll_sdiv = 0x3,
		.vpll_mdiv = 0x96,
		.vpll_pdiv = 0x3,
		.vpll_sdiv = 0x2,

		.bpll_mdiv = 0x64,
		.bpll_pdiv = 0x3,
		.bpll_sdiv = 0x0,
		.use_bpll = 0,
		.pclk_cdrex_ratio = 0x5,
		.direct_cmd_msr = {
			0x00020018, 0x00030000, 0x00010000, 0x00000d70
		},
		.timing_ref = 0x000000bb,
		.timing_row = 0x8c36660f,
		.timing_data = 0x3630580b,
		.timing_power = 0x41000a44,
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

		.zq_mode_dds = 0x5,
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
			DMC_MEMCONTROL_TP_DISABLE |
			DMC_MEMCONTROL_DSREF_ENABLE |
			DMC_MEMCONTROL_ADD_LAT_PALL_CYCLE(0) |
			DMC_MEMCONTROL_MEM_TYPE_DDR3 |
			DMC_MEMCONTROL_MEM_WIDTH_32BIT |
			DMC_MEMCONTROL_NUM_CHIP_1 |
			DMC_MEMCONTROL_BL_8 |
			DMC_MEMCONTROL_PZQ_DISABLE |
			DMC_MEMCONTROL_MRR_BYTE_7_0,
		.memconfig = DMC_MEMCONFIGx_CHIP_MAP_INTERLEAVED |
			DMC_MEMCONFIGx_CHIP_COL_10 |
			DMC_MEMCONFIGx_CHIP_ROW_15 |
			DMC_MEMCONFIGx_CHIP_BANK_8,
		.membaseconfig0 = DMC_MEMBASECONFIG_VAL(0x40),
		.membaseconfig1 = DMC_MEMBASECONFIG_VAL(0x80),
		.prechconfig_tp_cnt = 0xff,
		.dpwrdn_cyc = 0xff,
		.dsref_cyc = 0xffff,
		.concontrol = DMC_CONCONTROL_DFI_INIT_START_DISABLE |
			DMC_CONCONTROL_TIMEOUT_LEVEL0 |
			DMC_CONCONTROL_RD_FETCH_DISABLE |
			DMC_CONCONTROL_EMPTY_DISABLE |
			DMC_CONCONTROL_AREF_EN_DISABLE |
			DMC_CONCONTROL_IO_PD_CON_DISABLE,
		.dmc_channels = 2,
		.chips_per_channel = 2,
		.chips_to_configure = 1,
		.send_zq_init = 1,
		.impedance = IMP_OUTPUT_DRV_40_OHM,
		.gate_leveling_enable = 1,
	},
	{
		.mem_manuf = MEM_MANUF_ELPIDA,
		.mem_type = DDR_MODE_DDR3,
		.frequency_mhz = 780,
		.mpll_mdiv = 0x64,
		.mpll_pdiv = 0x3,
		.mpll_sdiv = 0x0,
		.cpll_mdiv = 0xde,
		.cpll_pdiv = 0x4,
		.cpll_sdiv = 0x2,
		.gpll_mdiv = 0x215,
		.gpll_pdiv = 0xc,
		.gpll_sdiv = 0x1,
		.epll_mdiv = 0x60,
		.epll_pdiv = 0x3,
		.epll_sdiv = 0x3,
		.vpll_mdiv = 0x96,
		.vpll_pdiv = 0x3,
		.vpll_sdiv = 0x2,

		.bpll_mdiv = 0x82,
		.bpll_pdiv = 0x4,
		.bpll_sdiv = 0x0,
		.use_bpll = 1,
		.pclk_cdrex_ratio = 0x5,
		.direct_cmd_msr = {
			0x00020018, 0x00030000, 0x00010042, 0x00000d70
		},
		.timing_ref = 0x000000bb,
		.timing_row = 0x8c36660f,
		.timing_data = 0x3630580b,
		.timing_power = 0x41000a44,
		.phy0_dqs = 0x08080808,
		.phy1_dqs = 0x08080808,
		.phy0_dq = 0x08080808,
		.phy1_dq = 0x08080808,
		.phy0_tFS = 0x4,
		.phy1_tFS = 0x4,
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
		.zq_mode_noterm = 0,

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
			DMC_MEMCONTROL_TP_DISABLE |
			DMC_MEMCONTROL_DSREF_ENABLE |
			DMC_MEMCONTROL_ADD_LAT_PALL_CYCLE(0) |
			DMC_MEMCONTROL_MEM_TYPE_DDR3 |
			DMC_MEMCONTROL_MEM_WIDTH_32BIT |
			DMC_MEMCONTROL_NUM_CHIP_1 |
			DMC_MEMCONTROL_BL_8 |
			DMC_MEMCONTROL_PZQ_DISABLE |
			DMC_MEMCONTROL_MRR_BYTE_7_0,
		.memconfig = DMC_MEMCONFIGx_CHIP_MAP_INTERLEAVED |
			DMC_MEMCONFIGx_CHIP_COL_10 |
			DMC_MEMCONFIGx_CHIP_ROW_15 |
			DMC_MEMCONFIGx_CHIP_BANK_8,
		.membaseconfig0 = DMC_MEMBASECONFIG_VAL(0x40),
		.membaseconfig1 = DMC_MEMBASECONFIG_VAL(0x80),
		.prechconfig_tp_cnt = 0xff,
		.dpwrdn_cyc = 0xff,
		.dsref_cyc = 0xffff,
		.concontrol = DMC_CONCONTROL_DFI_INIT_START_DISABLE |
			DMC_CONCONTROL_TIMEOUT_LEVEL0 |
			DMC_CONCONTROL_RD_FETCH_DISABLE |
			DMC_CONCONTROL_EMPTY_DISABLE |
			DMC_CONCONTROL_AREF_EN_DISABLE |
			DMC_CONCONTROL_IO_PD_CON_DISABLE,
		.dmc_channels = 2,
		.chips_per_channel = 2,
		.chips_to_configure = 1,
		.send_zq_init = 1,
		.impedance = IMP_OUTPUT_DRV_30_OHM,
		.gate_leveling_enable = 0,
	}, {
		.mem_manuf = MEM_MANUF_SAMSUNG,
		.mem_type = DDR_MODE_DDR3,
		.frequency_mhz = 780,
		.mpll_mdiv = 0x64,
		.mpll_pdiv = 0x3,
		.mpll_sdiv = 0x0,
		.cpll_mdiv = 0xde,
		.cpll_pdiv = 0x4,
		.cpll_sdiv = 0x2,
		.gpll_mdiv = 0x215,
		.gpll_pdiv = 0xc,
		.gpll_sdiv = 0x1,
		.epll_mdiv = 0x60,
		.epll_pdiv = 0x3,
		.epll_sdiv = 0x3,
		.vpll_mdiv = 0x96,
		.vpll_pdiv = 0x3,
		.vpll_sdiv = 0x2,

		.bpll_mdiv = 0x82,
		.bpll_pdiv = 0x4,
		.bpll_sdiv = 0x0,
		.use_bpll = 1,
		.pclk_cdrex_ratio = 0x5,
		.direct_cmd_msr = {
			0x00020018, 0x00030000, 0x00010000, 0x00000d70
		},
		.timing_ref = 0x000000bb,
		.timing_row = 0x8c36660f,
		.timing_data = 0x3630580b,
		.timing_power = 0x41000a44,
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

		.zq_mode_dds = 0x5,
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
			DMC_MEMCONTROL_TP_DISABLE |
			DMC_MEMCONTROL_DSREF_ENABLE |
			DMC_MEMCONTROL_ADD_LAT_PALL_CYCLE(0) |
			DMC_MEMCONTROL_MEM_TYPE_DDR3 |
			DMC_MEMCONTROL_MEM_WIDTH_32BIT |
			DMC_MEMCONTROL_NUM_CHIP_1 |
			DMC_MEMCONTROL_BL_8 |
			DMC_MEMCONTROL_PZQ_DISABLE |
			DMC_MEMCONTROL_MRR_BYTE_7_0,
		.memconfig = DMC_MEMCONFIGx_CHIP_MAP_INTERLEAVED |
			DMC_MEMCONFIGx_CHIP_COL_10 |
			DMC_MEMCONFIGx_CHIP_ROW_15 |
			DMC_MEMCONFIGx_CHIP_BANK_8,
		.membaseconfig0 = DMC_MEMBASECONFIG_VAL(0x40),
		.membaseconfig1 = DMC_MEMBASECONFIG_VAL(0x80),
		.prechconfig_tp_cnt = 0xff,
		.dpwrdn_cyc = 0xff,
		.dsref_cyc = 0xffff,
		.concontrol = DMC_CONCONTROL_DFI_INIT_START_DISABLE |
			DMC_CONCONTROL_TIMEOUT_LEVEL0 |
			DMC_CONCONTROL_RD_FETCH_DISABLE |
			DMC_CONCONTROL_EMPTY_DISABLE |
			DMC_CONCONTROL_AREF_EN_DISABLE |
			DMC_CONCONTROL_IO_PD_CON_DISABLE,
		.dmc_channels = 2,
		.chips_per_channel = 2,
		.chips_to_configure = 1,
		.send_zq_init = 1,
		.impedance = IMP_OUTPUT_DRV_40_OHM,
		.gate_leveling_enable = 1,
	}
};

#define BOARD_ID0_GPIO	88	/* GPD0, pin 0 */
#define BOARD_ID1_GPIO	89	/* GPD0, pin 1 */

enum board_config {
	SNOW_CONFIG_UNKNOWN = -1,
	SNOW_CONFIG_SAMSUNG_EVT,
	SNOW_CONFIG_ELPIDA_EVT,
	SNOW_CONFIG_SAMSUNG_DVT,
	SNOW_CONFIG_ELPIDA_DVT,
	SNOW_CONFIG_SAMSUNG_PVT,
	SNOW_CONFIG_ELPIDA_PVT,
	SNOW_CONFIG_SAMSUNG_MP,
	SNOW_CONFIG_ELPIDA_MP,
	SNOW_CONFIG_RSVD,
};

struct {
	enum mvl3 id0, id1;
	enum board_config config;
} id_map[] = {
	/*  ID0      ID1         config */
	{ LOGIC_0, LOGIC_0, SNOW_CONFIG_SAMSUNG_MP },
	{ LOGIC_0, LOGIC_1, SNOW_CONFIG_ELPIDA_MP },
	{ LOGIC_1, LOGIC_0, SNOW_CONFIG_SAMSUNG_DVT },
	{ LOGIC_1, LOGIC_1, SNOW_CONFIG_ELPIDA_DVT },
	{ LOGIC_0, LOGIC_Z, SNOW_CONFIG_SAMSUNG_PVT },
	{ LOGIC_1, LOGIC_Z, SNOW_CONFIG_ELPIDA_PVT },
	{ LOGIC_Z, LOGIC_0, SNOW_CONFIG_SAMSUNG_MP },
	{ LOGIC_Z, LOGIC_Z, SNOW_CONFIG_ELPIDA_MP },
	{ LOGIC_Z, LOGIC_1, SNOW_CONFIG_RSVD },
};

static int board_get_config(void)
{
	int i;
	int id0, id1;
	enum board_config config = SNOW_CONFIG_UNKNOWN;

	id0 = gpio_read_mvl3(BOARD_ID0_GPIO);
	id1 = gpio_read_mvl3(BOARD_ID1_GPIO);
	if (id0 < 0 || id1 < 0)
		return -1;

	for (i = 0; i < ARRAY_SIZE(id_map); i++) {
		if (id0 == id_map[i].id0 && id1 == id_map[i].id1) {
			config = id_map[i].config;
			break;
		}
	}

	return config;
}

struct mem_timings *get_mem_timings(void)
{
	int i;
	enum board_config config;
	enum ddr_mode mem_type;
	unsigned int frequency_mhz;
	enum mem_manuf mem_manuf;
	const struct mem_timings *mem;

	config = board_get_config();
	switch (config) {
	case SNOW_CONFIG_ELPIDA_EVT:
	case SNOW_CONFIG_ELPIDA_DVT:
	case SNOW_CONFIG_ELPIDA_PVT:
	case SNOW_CONFIG_ELPIDA_MP:
		mem_manuf = MEM_MANUF_ELPIDA;
		mem_type = DDR_MODE_DDR3;
		frequency_mhz = 800;
		break;
	case SNOW_CONFIG_SAMSUNG_EVT:
	case SNOW_CONFIG_SAMSUNG_DVT:
	case SNOW_CONFIG_SAMSUNG_PVT:
	case SNOW_CONFIG_SAMSUNG_MP:
		mem_manuf = MEM_MANUF_SAMSUNG;
		mem_type = DDR_MODE_DDR3;
		frequency_mhz = 800;
		break;
	default:
		printk(BIOS_CRIT, "Unknown board configuration.\n");
		return NULL;
	}

	for (i = 0, mem = mem_timings; i < ARRAY_SIZE(mem_timings);
			i++, mem++) {
		if (mem->mem_type == mem_type &&
			mem->frequency_mhz == frequency_mhz &&
			mem->mem_manuf == mem_manuf)
			return (struct mem_timings *)mem;
	}

	return NULL;
}
