/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 3.7
 */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/mt6366.h>
#include <soc/pmic_wrap.h>
#include <soc/pmif.h>
#include <soc/regulator.h>
#include <timer.h>

static struct pmic_setting init_setting[] = {
	{0x1E, 0xA, 0xA, 0},
	{0x22, 0x1F00, 0x1F00, 0},
	{0x2E, 0x1, 0x1, 0},
	{0x30, 0x1, 0x1, 0},
	{0x36, 0x8888, 0xFFFF, 0},
	{0x3A, 0x8888, 0xFFFF, 0},
	{0x3C, 0x8888, 0xFFFF, 0},
	{0x3E, 0x888, 0xFFF, 0},
	{0x94, 0x0, 0xFFFF, 0},
	{0x10C, 0x18, 0x18, 0},
	{0x112, 0x4, 0x4, 0},
	{0x118, 0x8, 0x8, 0},
	{0x12A, 0x100, 0x180, 0},
	{0x134, 0x80, 0x2890, 0},
	{0x14C, 0x20, 0x20, 0},
	{0x198, 0x0, 0x1FF, 0},
	{0x790, 0x280, 0x780, 0},
	{0x7AC, 0x0, 0x2000, 0},
	{0x98A, 0x1840, 0x1E40, 0},
	{0xA08, 0x1, 0x1, 0},
	{0xA24, 0x1E00, 0x1F00, 0},
	{0xA38, 0x0, 0x100, 0},
	{0xA3C, 0x81E0, 0x81E0, 0},
	{0xA44, 0xFFFF, 0xFFFF, 0},
	{0xA46, 0xFC00, 0xFC00, 0},
	{0xC8A, 0x4, 0xC, 0},
	{0xF8C, 0xAAA, 0xAAA, 0},
	{0x1188, 0x0, 0x8000, 0},
	{0x119E, 0x6000, 0x7000, 0},
	{0x11A2, 0x0, 0x3000, 0},
	{0x11B0, 0x4000, 0x4000, 0},
	{0x11B4, 0x0, 0x100, 0},
	{0x123A, 0x8040, 0x83FF, 0},
	{0x123E, 0x4, 0x4, 0},
	{0x1242, 0x1, 0x1, 0},
	{0x1260, 0x0, 0x154, 0},
	{0x1312, 0x8, 0x8, 0},
	{0x1334, 0x0, 0x100, 0},
	{0x138A, 0x10, 0x7F, 0},
	{0x138C, 0x15, 0x7F, 0},
	{0x138E, 0x1030, 0x3030, 0},
	{0x140A, 0x10, 0x7F, 0},
	{0x140C, 0x15, 0x7F, 0},
	{0x140E, 0x1030, 0x3030, 0},
	{0x148A, 0x10, 0x7F, 0},
	{0x148E, 0x1030, 0x3030, 0},
	{0x14A2, 0x20, 0x20, 0},
	{0x150A, 0x10, 0x7F, 0},
	{0x150E, 0x1030, 0x3030, 0},
	{0x158A, 0x8, 0x7F, 0},
	{0x158C, 0x90C, 0x7F7F, 0},
	{0x158E, 0x1030, 0x3030, 0},
	{0x159C, 0x8, 0xC, 0},
	{0x15A2, 0x20, 0x20, 0},
	{0x168A, 0x50, 0x7F, 0},
	{0x168C, 0x1964, 0x7F7F, 0},
	{0x168E, 0x2020, 0x3030, 0},
	{0x16A2, 0x20, 0x20, 0},
	{0x16AA, 0x50, 0x7F, 0},
	{0x170C, 0x1964, 0x7F7F, 0},
	{0x170E, 0x2020, 0x3030, 0},
	{0x172A, 0x44, 0x7F, 0},
	{0x178C, 0x202, 0x7F7F, 0},
	{0x178E, 0x70, 0x73, 0},
	{0x1790, 0xC, 0xC, 0},
	{0x1798, 0x2810, 0x3F3F, 0},
	{0x179A, 0x800, 0x3F00, 0},
	{0x179E, 0x1, 0x1, 0},
	{0x1808, 0x2000, 0x3000, 0},
	{0x180C, 0x60, 0x60, 0},
	{0x1814, 0x3FF0, 0x7FFF, 0},
	{0x1816, 0x3, 0x7, 0},
	{0x181A, 0x6081, 0xFFBF, 0},
	{0x181C, 0x503, 0x787, 0},
	{0x181E, 0xA462, 0xFFFF, 0},
	{0x1820, 0xA662, 0xFFFF, 0},
	{0x1824, 0xDB6, 0xFFF, 0},
	{0x1828, 0x160, 0x160, 0},
	{0x1830, 0x3FF0, 0x7FFF, 0},
	{0x1832, 0x3, 0x7, 0},
	{0x1836, 0x6081, 0xFFBF, 0},
	{0x1838, 0x503, 0x787, 0},
	{0x183A, 0xA262, 0xFFFF, 0},
	{0x183C, 0xA262, 0xFFFF, 0},
	{0x1840, 0xDB6, 0xFFF, 0},
	{0x1888, 0x420, 0xE7C, 0},
	{0x188A, 0x801, 0x3C07, 0},
	{0x188C, 0x1F, 0x3F, 0},
	{0x188E, 0x129A, 0xFFFF, 0},
	{0x1894, 0x58, 0x1F8, 0},
	{0x1896, 0x1C, 0x7C, 0},
	{0x1898, 0x1805, 0x3C07, 0},
	{0x189A, 0xF, 0xF, 0},
	{0x189C, 0x221A, 0xFFFF, 0},
	{0x18A0, 0x2E, 0x3F, 0},
	{0x18A2, 0x0, 0x40, 0},
	{0x18A4, 0x2C06, 0x3C07, 0},
	{0x18A6, 0xF, 0xF, 0},
	{0x18A8, 0x221A, 0xFFFF, 0},
	{0x18AC, 0x2E, 0x3F, 0},
	{0x18AE, 0x0, 0x40, 0},
	{0x18B0, 0x1805, 0x3C07, 0},
	{0x18B2, 0xF, 0xF, 0},
	{0x18B4, 0x221A, 0xFFFF, 0},
	{0x18B8, 0x2E, 0x3F, 0},
	{0x18BC, 0x50, 0x4F0, 0},
	{0x18BE, 0x3C, 0xFC, 0},
	{0x18C0, 0x0, 0x300, 0},
	{0x18C2, 0x8886, 0xFFFF, 0},
	{0x1A0E, 0x3, 0x3, 0},
	{0x1A10, 0x1, 0x1, 0},
	{0x1A12, 0x0, 0x1, 0},
	{0x1A14, 0x0, 0x1, 0},
	{0x1A16, 0x0, 0x1, 0},
	{0x1A18, 0x0, 0x1, 0},
	{0x1A1A, 0x0, 0x1, 0},
	{0x1A1C, 0x0, 0x1, 0},
	{0x1A1E, 0x0, 0x1, 0},
	{0x1A20, 0x0, 0x1, 0},
	{0x1A22, 0x0, 0x1, 0},
	{0x1A24, 0x0, 0x1, 0},
	{0x1A26, 0x0, 0x1, 0},
	{0x1A2C, 0x0, 0x1, 0},
	{0x1A2E, 0x0, 0x1, 0},
	{0x1A30, 0x0, 0x1, 0},
	{0x1A32, 0x0, 0x1, 0},
	{0x1A34, 0x0, 0x1, 0},
	{0x1A36, 0x0, 0x1, 0},
	{0x1A38, 0x0, 0x1, 0},
	{0x1A3A, 0x0, 0x1, 0},
	{0x1A3C, 0x0, 0x1, 0},
	{0x1A3E, 0x0, 0x1, 0},
	{0x1A40, 0x0, 0x1, 0},
	{0x1A42, 0x0, 0x1, 0},
	{0x1A44, 0x0, 0x1, 0},
	{0x1A46, 0x0, 0x1, 0},
	{0x1A48, 0x0, 0x1, 0},
	{0x1A4A, 0x0, 0x1, 0},
	{0x1A4C, 0x0, 0x1, 0},
	{0x1A4E, 0x0, 0x1, 0},
	{0x1A50, 0xE7FF, 0xE7FF, 0},
	{0x1A56, 0x7FFF, 0x7FFF, 0},
	{0x1B48, 0x10, 0x7F, 0},
	{0x1B4A, 0xF15, 0x7F7F, 0},
	{0x1B8A, 0x10, 0x7F, 0},
	{0x1B8C, 0xF15, 0x7F7F, 0},
	{0x1BA8, 0x10, 0x7F, 0},
	{0x1BAA, 0xF15, 0x7F7F, 0},
	{0x1BAC, 0x0, 0x3, 0},
	{0x1BCA, 0x10, 0x7F, 0},
	{0x1BCC, 0x70F, 0x7F7F, 0},
	{0x1C9E, 0x38, 0x7F, 0},
	{0x1CA0, 0x70F, 0x7F7F, 0},
	/* VSRAM_CORE: set SW mode */
	{0x1CA4, 0x1, 0xFFFF, 0},
	/* VSRAM_CORE: SW set OFF */
	{0x1C9C, 0x0, 0xFFFF, 0},
	{0x1EA2, 0x1B, 0x1F, 0},
	{0x1EA4, 0xC00, 0x1C00, 0},
	{0x1EA6, 0xC00, 0x1C00, 0},
	{0x1EA8, 0xC00, 0x1C00, 0},
};

static struct pmic_setting lp_setting[] = {
	/* Suspend */
	/* [0:0]: RG_BUCK_VPROC11_SW_OP_EN */
	{0x1390, 0x1, 0x1, 0},
	/* [0:0]: RG_BUCK_VCORE_SW_OP_EN */
	{0x1490, 0x1, 0x1, 0},
	/* [0:0]: RG_BUCK_VGPU_SW_OP_EN */
	{0x1510, 0x1, 0x1, 0},
	/* [0:0]: RG_BUCK_VMODEM_SW_OP_EN */
	{0x1590, 0x1, 0x1, 0},
	/* [0:0]: RG_BUCK_VS1_SW_OP_EN */
	{0x1690, 0x1, 0x1, 0},
	/* [1:1]: RG_BUCK_VS2_HW0_OP_EN */
	{0x1710, 0x1, 0x1, 1},
	/* [1:1]: RG_BUCK_VS2_HW0_OP_CFG */
	{0x1716, 0x1, 0x1, 1},
	/* [1:1]: RG_BUCK_VDRAM1_HW0_OP_EN */
	{0x1610, 0x1, 0x1, 1},
	/* [1:1]: RG_BUCK_VDRAM1_HW0_OP_CFG */
	{0x1616, 0x1, 0x1, 1},
	/* [0:0]: RG_BUCK_VPROC12_SW_OP_EN */
	{0x1410, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VSRAM_GPU_SW_OP_EN */
	{0x1BD0, 0x1, 0x1, 0},
	/* [1:1]: RG_LDO_VSRAM_OTHERS_HW0_OP_EN */
	{0x1BAE, 0x1, 0x1, 1},
	/* [1:1]: RG_LDO_VSRAM_OTHERS_HW0_OP_CFG */
	{0x1BB4, 0x1, 0x1, 1},
	/* [0:0]: RG_LDO_VSRAM_PROC11_SW_OP_EN */
	{0x1B4E, 0x1, 0x1, 0},
	/* [1:1]: RG_LDO_VXO22_HW0_OP_EN */
	{0x1A8A, 0x1, 0x1, 1},
	/* [1:1]: RG_LDO_VXO22_HW0_OP_CFG */
	{0x1A90, 0x1, 0x1, 1},
	/* [2:2]: RG_LDO_VRF18_HW1_OP_EN */
	{0x1C1E, 0x1, 0x1, 2},
	/* [2:2]: RG_LDO_VRF18_HW1_OP_CFG */
	{0x1C24, 0x0, 0x1, 2},
	/* [0:0]: RG_LDO_VEFUSE_SW_OP_EN */
	{0x1C46, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCN33_SW_OP_EN */
	{0x1D1E, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCN33_SW_OP_EN */
	{0x1D1E, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCN28_SW_OP_EN */
	{0x1D8A, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCN18_SW_OP_EN */
	{0x1C5A, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCAMA1_SW_OP_EN */
	{0x1C6E, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCAMD_SW_OP_EN */
	{0x1C9E, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCAMA2_SW_OP_EN */
	{0x1C8A, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VSRAM_PROC12_SW_OP_EN */
	{0x1B90, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCAMIO_SW_OP_EN */
	{0x1CB2, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VLDO28_SW_OP_EN */
	{0x1D34, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VLDO28_SW_OP_EN */
	{0x1D34, 0x1, 0x1, 0},
	/* [1:1]: RG_LDO_VA12_HW0_OP_EN */
	{0x1A9E, 0x1, 0x1, 1},
	/* [1:1]: RG_LDO_VA12_HW0_OP_CFG */
	{0x1AA4, 0x1, 0x1, 1},
	/* [1:1]: RG_LDO_VAUX18_HW0_OP_EN */
	{0x1AB2, 0x1, 0x1, 1},
	/* [1:1]: RG_LDO_VAUX18_HW0_OP_CFG */
	{0x1AB8, 0x1, 0x1, 1},
	/* [1:1]: RG_LDO_VAUD28_HW0_OP_EN */
	{0x1AC6, 0x1, 0x1, 1},
	/* [1:1]: RG_LDO_VAUD28_HW0_OP_CFG */
	{0x1ACC, 0x1, 0x1, 1},
	/* [0:0]: RG_LDO_VIO28_SW_OP_EN */
	{0x1ADA, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VIO18_SW_OP_EN */
	{0x1AEE, 0x1, 0x1, 0},
	/* [2:2]: RG_LDO_VFE28_HW1_OP_EN */
	{0x1C0A, 0x1, 0x1, 2},
	/* [2:2]: RG_LDO_VFE28_HW1_OP_CFG */
	{0x1C10, 0x0, 0x1, 2},
	/* [1:1]: RG_LDO_VDRAM2_HW0_OP_EN */
	{0x1B0A, 0x1, 0x1, 1},
	/* [1:1]: RG_LDO_VDRAM2_HW0_OP_CFG */
	{0x1B10, 0x1, 0x1, 1},
	/* [0:0]: RG_LDO_VMC_SW_OP_EN */
	{0x1CC6, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VMCH_SW_OP_EN */
	{0x1CDA, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VEMC_SW_OP_EN */
	{0x1B1E, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VSIM1_SW_OP_EN */
	{0x1D4A, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VSIM2_SW_OP_EN */
	{0x1D5E, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VIBR_SW_OP_EN */
	{0x1D0A, 0x1, 0x1, 0},
	/* [1:1]: RG_LDO_VUSB_HW0_OP_EN */
	{0x1B32, 0x1, 0x1, 1},
	/* [1:1]: RG_LDO_VUSB_HW0_OP_CFG */
	{0x1B38, 0x1, 0x1, 1},
	/* [1:1]: RG_LDO_VUSB_HW0_OP_EN */
	{0x1B32, 0x1, 0x1, 1},
	/* [1:1]: RG_LDO_VUSB_HW0_OP_CFG */
	{0x1B38, 0x1, 0x1, 1},
	/* [1:1]: RG_LDO_VBIF28_HW0_OP_EN */
	{0x1DA0, 0x1, 0x1, 1},
	/* [1:1]: RG_LDO_VBIF28_HW0_OP_CFG */
	{0x1DA6, 0x0, 0x1, 1},

	/* Deep idle setting */
	/* [0:0]: RG_BUCK_VPROC11_SW_OP_EN */
	{0x1390, 0x1, 0x1, 0},
	/* [0:0]: RG_BUCK_VCORE_SW_OP_EN */
	{0x1490, 0x1, 0x1, 0},
	/* [0:0]: RG_BUCK_VGPU_SW_OP_EN */
	{0x1510, 0x1, 0x1, 0},
	/* [0:0]: RG_BUCK_VMODEM_SW_OP_EN */
	{0x1590, 0x1, 0x1, 0},
	/* [0:0]: RG_BUCK_VS1_SW_OP_EN */
	{0x1690, 0x1, 0x1, 0},
	/* [3:3]: RG_BUCK_VS2_HW2_OP_EN */
	{0x1710, 0x1, 0x1, 3},
	/* [3:3]: RG_BUCK_VS2_HW2_OP_CFG */
	{0x1716, 0x1, 0x1, 3},
	/* [3:3]: RG_BUCK_VDRAM1_HW2_OP_EN */
	{0x1610, 0x1, 0x1, 3},
	/* [3:3]: RG_BUCK_VDRAM1_HW2_OP_CFG */
	{0x1616, 0x1, 0x1, 3},
	/* [0:0]: RG_BUCK_VPROC12_SW_OP_EN */
	{0x1410, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VSRAM_GPU_SW_OP_EN */
	{0x1BD0, 0x1, 0x1, 0},
	/* [3:3]: RG_LDO_VSRAM_OTHERS_HW2_OP_EN */
	{0x1BAE, 0x1, 0x1, 3},
	/* [3:3]: RG_LDO_VSRAM_OTHERS_HW2_OP_CFG */
	{0x1BB4, 0x1, 0x1, 3},
	/* [0:0]: RG_LDO_VSRAM_PROC11_SW_OP_EN */
	{0x1B4E, 0x1, 0x1, 0},
	/* [3:3]: RG_LDO_VXO22_HW2_OP_EN */
	{0x1A8A, 0x1, 0x1, 3},
	/* [3:3]: RG_LDO_VXO22_HW2_OP_CFG */
	{0x1A90, 0x1, 0x1, 3},
	/* [2:2]: RG_LDO_VRF18_HW1_OP_EN */
	{0x1C1E, 0x1, 0x1, 2},
	/* [2:2]: RG_LDO_VRF18_HW1_OP_CFG */
	{0x1C24, 0x0, 0x1, 2},
	/* [0:0]: RG_LDO_VEFUSE_SW_OP_EN */
	{0x1C46, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCN33_SW_OP_EN */
	{0x1D1E, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCN33_SW_OP_EN */
	{0x1D1E, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCN28_SW_OP_EN */
	{0x1D8A, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCN18_SW_OP_EN */
	{0x1C5A, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCAMA1_SW_OP_EN */
	{0x1C6E, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCAMD_SW_OP_EN */
	{0x1C9E, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCAMA2_SW_OP_EN */
	{0x1C8A, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VSRAM_PROC12_SW_OP_EN */
	{0x1B90, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VCAMIO_SW_OP_EN */
	{0x1CB2, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VLDO28_SW_OP_EN */
	{0x1D34, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VLDO28_SW_OP_EN */
	{0x1D34, 0x1, 0x1, 0},
	/* [3:3]: RG_LDO_VA12_HW2_OP_EN */
	{0x1A9E, 0x1, 0x1, 3},
	/* [3:3]: RG_LDO_VA12_HW2_OP_CFG */
	{0x1AA4, 0x1, 0x1, 3},
	/* [3:3]: RG_LDO_VAUX18_HW2_OP_EN */
	{0x1AB2, 0x1, 0x1, 3},
	/* [3:3]: RG_LDO_VAUX18_HW2_OP_CFG */
	{0x1AB8, 0x1, 0x1, 3},
	/* [0:0]: RG_LDO_VAUD28_SW_OP_EN */
	{0x1AC6, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VIO28_SW_OP_EN */
	{0x1ADA, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VIO18_SW_OP_EN */
	{0x1AEE, 0x1, 0x1, 0},
	/* [2:2]: RG_LDO_VFE28_HW1_OP_EN */
	{0x1C0A, 0x1, 0x1, 2},
	/* [2:2]: RG_LDO_VFE28_HW1_OP_CFG */
	{0x1C10, 0x0, 0x1, 2},
	/* [3:3]: RG_LDO_VDRAM2_HW2_OP_EN */
	{0x1B0A, 0x1, 0x1, 3},
	/* [3:3]: RG_LDO_VDRAM2_HW2_OP_CFG */
	{0x1B10, 0x1, 0x1, 3},
	/* [0:0]: RG_LDO_VMC_SW_OP_EN */
	{0x1CC6, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VMCH_SW_OP_EN */
	{0x1CDA, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VEMC_SW_OP_EN */
	{0x1B1E, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VSIM1_SW_OP_EN */
	{0x1D4A, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VSIM2_SW_OP_EN */
	{0x1D5E, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VIBR_SW_OP_EN */
	{0x1D0A, 0x1, 0x1, 0},
	/* [3:3]: RG_LDO_VUSB_HW2_OP_EN */
	{0x1B32, 0x1, 0x1, 3},
	/* [3:3]: RG_LDO_VUSB_HW2_OP_CFG */
	{0x1B38, 0x1, 0x1, 3},
	/* [3:3]: RG_LDO_VUSB_HW2_OP_EN */
	{0x1B32, 0x1, 0x1, 3},
	/* [3:3]: RG_LDO_VUSB_HW2_OP_CFG */
	{0x1B38, 0x1, 0x1, 3},
	/* [3:3]: RG_LDO_VBIF28_HW2_OP_EN */
	{0x1DA0, 0x1, 0x1, 3},
	/* [3:3]: RG_LDO_VBIF28_HW2_OP_CFG */
	{0x1DA6, 0x0, 0x1, 3},
};

static struct pmic_setting scp_setting[] = {
	/* scp voltage initialization */
	/* [6:0]: RG_BUCK_VCORE_SSHUB_VOSEL */
	{0x14A6, 0x20, 0x7F, 0},
	/* [14:8]: RG_BUCK_VCORE_SSHUB_VOSEL_SLEEP */
	{0x14A6, 0x20, 0x7F, 8},
	/* [0:0]: RG_BUCK_VCORE_SSHUB_EN */
	{0x14A4, 0x1, 0x1, 0},
	/* [1:1]: RG_BUCK_VCORE_SSHUB_SLEEP_VOSEL_EN */
	{0x14A4, 0x0, 0x1, 1},
	/* [6:0]: RG_LDO_VSRAM_OTHERS_SSHUB_VOSEL */
	{0x1BC6, 0x40, 0x7F, 0},
	/* [14:8]: RG_LDO_VSRAM_OTHERS_SSHUB_VOSEL_SLEEP */
	{0x1BC6, 0x40, 0x7F, 8},
	/* [0:0]: RG_LDO_VSRAM_OTHERS_SSHUB_EN */
	{0x1BC4, 0x1, 0x1, 0},
	/* [1:1]: RG_LDO_VSRAM_OTHERS_SSHUB_SLEEP_VOSEL_EN */
	{0x1BC4, 0x0, 0x1, 1},
	/* [4:4]: RG_SRCVOLTEN_LP_EN */
	{0x134, 0x1, 0x1, 4},
};

static const int vddq_votrim[] = {
	0, -10000, -20000, -30000, -40000, -50000, -60000, -70000,
	80000, 70000, 60000, 50000, 40000, 30000, 20000, 10000,
};

static void mt6366_protect_control(bool en_protect)
{
	/* Write a magic number 0x9CA7 to disable protection */
	pwrap_write_field(PMIC_TOP_TMA_KEY, en_protect ? 0 : 0x9CA7, 0xFFFF, 0);
}

static u32 pmic_read_efuse(int i)
{
	u32 efuse_data = 0;

	/* 1. Enable efuse ctrl engine clock */
	pwrap_write_field(PMIC_TOP_CKHWEN_CON0_CLR, 0x1, 0x1, 2);
	pwrap_write_field(PMIC_TOP_CKPDN_CON0_CLR, 0x1, 0x1, 4);

	/* 2. */
	pwrap_write_field(PMIC_OTP_CON11, 0x1, 0x1, 0);

	/* 3. Set row to read */
	pwrap_write_field(PMIC_OTP_CON0, i * 2, 0xFF, 0);

	/* 4. Toggle RG_OTP_RD_TRIG */
	if (pwrap_read_field(PMIC_OTP_CON8, 0x1, 0) == 0)
		pwrap_write_field(PMIC_OTP_CON8, 0x1, 0x1, 0);
	else
		pwrap_write_field(PMIC_OTP_CON8, 0, 0x1, 0);

	/* 5. Polling RG_OTP_RD_BUSY = 0 */
	udelay(300);
	while (pwrap_read_field(PMIC_OTP_CON13, 0x1, 0) == 1)
		;

	/* 6. Read RG_OTP_DOUT_SW */
	udelay(100);
	efuse_data = pwrap_read_field(PMIC_OTP_CON12, 0xFFFF, 0);

	/* 7. Disable efuse ctrl engine clock */
	pwrap_write_field(PMIC_TOP_CKHWEN_CON0_SET, 0x1, 0x1, 2);
	pwrap_write_field(PMIC_TOP_CKPDN_CON0_SET, 0x1, 0x1, 4);

	return efuse_data;
}

static int pmic_get_efuse_votrim(void)
{
	const u32 cali_efuse = pmic_read_efuse(106) & 0xF;
	assert(cali_efuse < ARRAY_SIZE(vddq_votrim));
	return vddq_votrim[cali_efuse];
}

static u32 pmic_get_vcore_vol(void)
{
	u16 vol_reg;

	vol_reg = pwrap_read_field(PMIC_VCORE_DBG0, 0x7F, 0);
	return 500000 + vol_reg * 6250;
}

static void pmic_set_vcore_vol(u32 vcore_uv)
{
	u16 vol_reg;

	assert(vcore_uv >= 500000);
	assert(vcore_uv <= 1100000);

	vol_reg = (vcore_uv - 500000) / 6250;

	pwrap_write_field(PMIC_VCORE_OP_EN, 1, 0x7F, 0);
	pwrap_write_field(PMIC_VCORE_VOSEL, vol_reg, 0x7F, 0);
	udelay(1);
}

static u32 pmic_get_vproc12_vol(void)
{
	u16 vol_reg;

	vol_reg = pwrap_read_field(PMIC_VPROC12_DBG0, 0x7F, 0);
	return 500000 + vol_reg * 6250;
}

static void pmic_set_vproc12_vol(u32 v_uv)
{
	u16 vol_reg;

	assert(v_uv >= 500000);
	assert(v_uv <= 1293750);

	vol_reg = (v_uv - 500000) / 6250;

	pwrap_write_field(PMIC_VPROC12_OP_EN, 1, 0x7F, 0);
	pwrap_write_field(PMIC_VPROC12_VOSEL, vol_reg, 0x7F, 0);
	udelay(1);
}

static u32 pmic_get_vsram_proc12_vol(void)
{
	u16 vol_reg;

	vol_reg = pwrap_read_field(PMIC_VSRAM_PROC12_DBG0, 0x7F, 0);
	return 500000 + vol_reg * 6250;
}

static void pmic_set_vsram_proc12_vol(u32 v_uv)
{
	u16 vol_reg;

	assert(v_uv >= 500000);
	assert(v_uv <= 1293750);

	vol_reg = (v_uv - 500000) / 6250;

	pwrap_write_field(PMIC_VSRAM_PROC12_OP_EN, 1, 0x7F, 0);
	pwrap_write_field(PMIC_VSRAM_PROC12_VOSEL, vol_reg, 0x7F, 0);
	udelay(1);
}

static u32 pmic_get_vdram1_vol(void)
{
	u16 vol_reg;

	vol_reg = pwrap_read_field(PMIC_VDRAM1_DBG0, 0x7F, 0);
	return 500000 + vol_reg * 12500;
}

static void pmic_set_vdram1_vol(u32 vdram_uv)
{
	u16 vol_reg;

	assert(vdram_uv >= 500000);
	assert(vdram_uv <= 1300000);

	vol_reg = (vdram_uv - 500000) / 12500;

	pwrap_write_field(PMIC_VDRAM1_OP_EN, 1, 0x7F, 0);
	pwrap_write_field(PMIC_VDRAM1_VOSEL, vol_reg, 0x7F, 0);
	udelay(1);
}

static u32 pmic_get_vddq_vol(void)
{
	int efuse_votrim;
	u16 cali_trim;

	if (!pwrap_read_field(PMIC_VDDQ_OP_EN, 0x1, 15))
		return 0;

	efuse_votrim = pmic_get_efuse_votrim();
	cali_trim = pwrap_read_field(PMIC_VDDQ_ELR_0, 0xF, 0);
	assert(cali_trim < ARRAY_SIZE(vddq_votrim));
	return 600 * 1000 - efuse_votrim + vddq_votrim[cali_trim];
}

static void pmic_set_vddq_vol(u32 vddq_uv)
{
	int target_mv, dram2_ori_mv, cali_offset_uv;
	u16 cali_trim;

	assert(vddq_uv >= 530000);
	assert(vddq_uv <= 680000);

	/* Round down to multiple of 10 */
	target_mv = (vddq_uv / 1000) / 10 * 10;

	dram2_ori_mv = 600 - pmic_get_efuse_votrim() / 1000;
	cali_offset_uv = 1000 * (target_mv - dram2_ori_mv);

	if (cali_offset_uv >= 80000)
		cali_trim = 8;
	else if (cali_offset_uv <= -70000)
		cali_trim = 7;
	else {
		cali_trim = 0;
		while (cali_trim < ARRAY_SIZE(vddq_votrim) &&
		       vddq_votrim[cali_trim] != cali_offset_uv)
			++cali_trim;
		assert(cali_trim < ARRAY_SIZE(vddq_votrim));
	}

	mt6366_protect_control(false);
	pwrap_write_field(PMIC_VDDQ_ELR_0, cali_trim, 0xF, 0);
	mt6366_protect_control(true);
	udelay(1);
}

static u32 pmic_get_vmch_vol(void)
{
	u32 ret;
	u16 vol_reg;

	vol_reg = pwrap_read_field(PMIC_VMCH_ANA_CON0, 0x7, 8);

	switch (vol_reg) {
	case 2:
		ret = 2900000;
		break;
	case 3:
		ret = 3000000;
		break;
	case 5:
		ret = 3300000;
		break;
	default:
		printk(BIOS_ERR, "ERROR[%s] VMCH read fail: %d\n", __func__, vol_reg);
		ret = 0;
		break;
	}
	return ret;
}

static void pmic_set_vmch_vol(u32 vmch_uv)
{
	u16 val = 0;

	switch (vmch_uv) {
	case 2900000:
		val = 2;
		break;
	case 3000000:
		val = 3;
		break;
	case 3300000:
		val = 5;
		break;
	default:
		die("ERROR[%s]: VMCH voltage %u is not support.\n", __func__, vmch_uv);
		return;
	}

	pwrap_write_field(PMIC_VMCH_ANA_CON0, val, 0x7, 8);

	/* Force SW to turn on */
	pwrap_write_field(PMIC_LDO_VMCH_OP_EN, 1, 0xFF, 0);
	pwrap_write_field(PMIC_LDO_VMCH_CON0, 1, 0xFF, 0);
}

static u32 pmic_get_vmc_vol(void)
{
	u32 ret;
	u16 vol_reg;

	vol_reg = pwrap_read_field(PMIC_VMC_ANA_CON0, 0xF, 8);

	switch (vol_reg) {
	case 0x4:
		ret = 1800000;
		break;
	case 0xA:
		ret = 2900000;
		break;
	case 0xB:
		ret = 3000000;
		break;
	case 0xD:
		ret = 3300000;
		break;
	default:
		printk(BIOS_ERR, "ERROR[%s] VMC read fail: %d\n", __func__, vol_reg);
		ret = 0;
		break;
	}
	return ret;
}

static void pmic_set_vmc_vol(u32 vmc_uv)
{
	u16 val = 0;

	switch (vmc_uv) {
	case 1800000:
		val = 0x4;
		break;
	case 2900000:
		val = 0xA;
		break;
	case 3000000:
		val = 0xB;
		break;
	case 3300000:
		val = 0xD;
		break;
	default:
		die("ERROR[%s]: VMC voltage %u is not support.\n", __func__, vmc_uv);
		return;
	}

	pwrap_write_field(PMIC_VMC_ANA_CON0, val, 0xF, 8);

	/* Force SW to turn on */
	pwrap_write_field(PMIC_LDO_VMC_OP_EN, 1, 0xFF, 0);
	pwrap_write_field(PMIC_LDO_VMC_CON0, 1, 0xFF, 0);
}

static u32 pmic_get_vrf12_vol(void)
{
	return (pwrap_read_field(PMIC_LDO_VRF12_CON0, 0x3, 0) &
		pwrap_read_field(PMIC_LDO_VRF12_OP_EN, 0x3, 0)) ? 1200000 : 0;
}

static void pmic_enable_vrf12(void)
{
	pwrap_write_field(PMIC_LDO_VRF12_CON0, 1, 0x3, 0);
	pwrap_write_field(PMIC_LDO_VRF12_OP_EN, 1, 0x3, 0);
}

static u32 pmic_get_vcn33_vol(void)
{
	u32 ret;
	u16 vol_reg;

	vol_reg = pwrap_read_field(PMIC_VCN33_ANA_CON0, 0x3, 8);

	switch (vol_reg) {
	case 0x1:
		ret = 3300000;
		break;
	case 0x2:
		ret = 3400000;
		break;
	case 0x3:
		ret = 3500000;
		break;
	default:
		printk(BIOS_ERR, "ERROR[%s] VCN33 read fail: %d\n", __func__, vol_reg);
		ret = 0;
		break;
	}
	return ret;
}

static void pmic_set_vcn33_vol(u32 vcn33_uv)
{
	u16 val = 0;

	switch (vcn33_uv) {
	case 3300000:
		val = 0x1;
		break;
	case 3400000:
		val = 0x2;
		break;
	case 3500000:
		val = 0x3;
		break;
	default:
		die("ERROR[%s]: VCN33 voltage %u is not support.\n", __func__, vcn33_uv);
		return;
	}

	pwrap_write_field(PMIC_VCN33_ANA_CON0, val, 0x3, 8);

	/* Force SW to turn on */
	pwrap_write_field(PMIC_LDO_VCN33_CON0_0, 1, 0x1, 0);
}

static void pmic_wdt_set(void)
{
	/* [5]=1, RG_WDTRSTB_DEB */
	pwrap_write_field(PMIC_TOP_RST_MISC_SET, 0x0020, 0xFFFF, 0);
	/* [1]=0, RG_WDTRSTB_MODE */
	pwrap_write_field(PMIC_TOP_RST_MISC_CLR, 0x0002, 0xFFFF, 0);
	/* [0]=1, RG_WDTRSTB_EN */
	pwrap_write_field(PMIC_TOP_RST_MISC_SET, 0x0001, 0xFFFF, 0);
}

static void mt6366_init_setting(void)
{
	mt6366_protect_control(false);
	for (size_t i = 0; i < ARRAY_SIZE(init_setting); i++)
		pwrap_write_field(
			init_setting[i].addr, init_setting[i].val,
			init_setting[i].mask, init_setting[i].shift);
	mt6366_protect_control(true);
}

static void wk_sleep_voltage_by_ddr(void)
{
	if (pwrap_read_field(PMIC_VM_MODE, 0x3, 0) == 0x2)
		pwrap_write_field(PMIC_VDRAM1_VOSEL_SLEEP, 0x3A, 0x7F, 0);
}

static void wk_power_down_seq(void)
{
	mt6366_protect_control(false);
	/* Set VPROC12 power-down time slot to 0xF to avoid 20ms delay */
	pwrap_write_field(PMIC_CPSDSA4, 0xF, 0x1F, 0);
	mt6366_protect_control(true);
}

static void mt6366_lp_setting(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(lp_setting); i++)
		pwrap_write_field(
			lp_setting[i].addr, lp_setting[i].val,
			lp_setting[i].mask, lp_setting[i].shift);
}

static void pmic_check_hwcid(void)
{
	printk(BIOS_INFO, "%s: ID = %#x\n", __func__,
	       pwrap_read_field(0x8, 0xFFFF, 0));
}

void mt6366_set_power_hold(bool enable)
{
	pwrap_write_field(PMIC_PWRHOLD, (enable) ? 1 : 0, 0x1, 0);
}

void mt6366_init_scp_voltage(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(scp_setting); i++)
		pwrap_write_field(
			scp_setting[i].addr, scp_setting[i].val,
			scp_setting[i].mask, scp_setting[i].shift);
}

void mt6366_set_vsim2_cali_mv(u32 vsim2_mv)
{
	u16 vsim2_reg, cali_mv;

	cali_mv = vsim2_mv % 100;
	assert(cali_mv % 10 == 0);

	switch (vsim2_mv - cali_mv) {
	case 1700:
		vsim2_reg = 0x3;
		break;
	case 1800:
		vsim2_reg = 0x4;
		break;
	case 2700:
		vsim2_reg = 0x8;
		break;
	case 3000:
		vsim2_reg = 0xb;
		break;
	case 3100:
		vsim2_reg = 0xc;
		break;
	default:
		printk(BIOS_ERR, "%s: voltage %d is not supported\n", __func__, vsim2_mv);
		return;
	};

	/* [11:8]=0x8, RG_VSIM2_VOSEL */
	pwrap_write_field(PMIC_VSIM2_ANA_CON0, vsim2_reg, 0xF, 8);

	/* [3:0], RG_VSIM2_VOCAL */
	pwrap_write_field(PMIC_VSIM2_ANA_CON0, cali_mv / 10, 0xF, 0);
}

void mt6366_set_voltage(enum mt6366_regulator_id id, u32 voltage_uv)
{
	switch (id) {
	case MT6366_VCORE:
		pmic_set_vcore_vol(voltage_uv);
		break;
	case MT6366_VDRAM1:
		pmic_set_vdram1_vol(voltage_uv);
		break;
	case MT6366_VDDQ:
		pmic_set_vddq_vol(voltage_uv);
		break;
	case MT6366_VMCH:
		pmic_set_vmch_vol(voltage_uv);
		break;
	case MT6366_VMC:
		pmic_set_vmc_vol(voltage_uv);
		break;
	case MT6366_VPROC12:
		pmic_set_vproc12_vol(voltage_uv);
		break;
	case MT6366_VSRAM_PROC12:
		pmic_set_vsram_proc12_vol(voltage_uv);
		break;
	case MT6366_VRF12:
		/* VRF12 only provides 1.2V, so we just need to enable it */
		pmic_enable_vrf12();
		break;
	case MT6366_VCN33:
		pmic_set_vcn33_vol(voltage_uv);
		break;
	default:
		printk(BIOS_ERR, "%s: PMIC %d is not supported\n", __func__, id);
		break;
	}
}

u32 mt6366_get_voltage(enum mt6366_regulator_id id)
{
	switch (id) {
	case MT6366_VCORE:
		return pmic_get_vcore_vol();
	case MT6366_VDRAM1:
		return pmic_get_vdram1_vol();
	case MT6366_VDDQ:
		return pmic_get_vddq_vol();
	case MT6366_VMCH:
		return pmic_get_vmch_vol();
	case MT6366_VMC:
		return pmic_get_vmc_vol();
	case MT6366_VPROC12:
		return pmic_get_vproc12_vol();
	case MT6366_VSRAM_PROC12:
		return pmic_get_vsram_proc12_vol();
	case MT6366_VRF12:
		return pmic_get_vrf12_vol();
	case MT6366_VCN33:
		return pmic_get_vcn33_vol();
	default:
		printk(BIOS_ERR, "%s: PMIC %d is not supported\n", __func__, id);
		break;
	}
	return 0;
}

void mt6366_init(void)
{
	struct stopwatch voltage_settled;

	if (pwrap_init())
		die("ERROR - Failed to initialize pmic wrap!");

	pmic_check_hwcid();
	mt6366_set_power_hold(true);
	pmic_wdt_set();
	mt6366_init_setting();
	stopwatch_init_usecs_expire(&voltage_settled, 200);
	wk_sleep_voltage_by_ddr();
	wk_power_down_seq();
	mt6366_lp_setting();
	pmif_spmi_set_lp_mode();

	while (!stopwatch_expired(&voltage_settled))
		/* wait for voltages to settle */;
}
