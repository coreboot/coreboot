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

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/pmic_wrap.h>
#include <soc/mt6358.h>
#include <timer.h>

static struct pmic_setting init_setting[] = {
	/* [15:0]: TMA_KEY */
	{0x3A8, 0x9CA7, 0xFFFF, 0},
	/* [1:1]: RG_SRCLKEN_IN0_HW_MODE */
	/* [3:3]: RG_SRCLKEN_IN1_HW_MODE */
	{0x1E, 0xA, 0xA, 0},
	/* [12:8]: RG_MON_GRP_SEL */
	{0x22, 0x1F00, 0x1F00, 0},
	/* [0:0]: RG_SMT_WDTRSTB_IN */
	{0x2E, 0x1, 0x1, 0},
	/* [0:0]: RG_SMT_SPI_CLK */
	{0x30, 0x1, 0x1, 0},
	/* [3:0]: RG_OCTL_SRCLKEN_IN0 */
	/* [7:4]: RG_OCTL_SRCLKEN_IN1 */
	/* [11:8]: RG_OCTL_RTC_32K1V8_0 */
	/* [15:12]: RG_OCTL_RTC_32K1V8_1 */
	{0x36, 0x8888, 0xFFFF, 0},
	/* [3:0]: RG_OCTL_AUD_CLK_MOSI */
	/* [7:4]: RG_OCTL_AUD_DAT_MOSI0 */
	/* [11:8]: RG_OCTL_AUD_DAT_MOSI1 */
	/* [15:12]: RG_OCTL_AUD_SYNC_MOSI */
	{0x3A, 0x8888, 0xFFFF, 0},
	/* [3:0]: RG_OCTL_AUD_CLK_MISO */
	/* [7:4]: RG_OCTL_AUD_DAT_MISO0 */
	/* [11:8]: RG_OCTL_AUD_DAT_MISO1 */
	/* [15:12]: RG_OCTL_AUD_SYNC_MISO */
	{0x3C, 0x8888, 0xFFFF, 0},
	/* [3:0]: RG_OCTL_HOMEKEY */
	/* [7:4]: RG_OCTL_SCP_VREQ_VAO */
	/* [11:8]: RG_OCTL_SD_CARD_DET_N */
	{0x3E, 0x888, 0xFFF, 0},
	/* [15:0]: GPIO_PULLEN0 */
	{0x94, 0x0, 0xFFFF, 0},
	/* [3:3]: RG_INTRP_PRE_OC_CK_PDN */
	/* [4:4]: RG_EFUSE_CK_PDN */
	{0x10C, 0x18, 0x18, 0},
	/* [2:2]: RG_TRIM_128K_CK_PDN */
	{0x112, 0x4, 0x4, 0},
	/* [3:3]: RG_RTC_32K1V8_SEL */
	{0x118, 0x8, 0x8, 0},
	/* [7:7]: RG_PMU_VXO22_ON */
	/* [8:8]: RG_PMU_VXO22_ON_SW_EN */
	{0x12A, 0x100, 0x180, 0},
	/* [4:4]: RG_SRCVOLTEN_LP_EN */
	/* [7:7]: RG_SRCLKEN2_LP_EN */
	/* [11:11]: RG_BUCK_PFM_FLAG_SW_EN */
	/* [13:13]: RG_DCXO26M_RDY_SW_EN */
	{0x134, 0x80, 0x2890, 0},
	/* [5:5]: RG_WDTRSTB_DEB */
	{0x14C, 0x20, 0x20, 0},
	/* [0:0]: RG_INT_MASK_BUCK_TOP */
	/* [1:1]: RG_INT_MASK_LDO_TOP */
	/* [2:2]: RG_INT_MASK_PSC_TOP */
	/* [3:3]: RG_INT_MASK_SCK_TOP */
	/* [4:4]: RG_INT_MASK_BM_TOP */
	/* [5:5]: RG_INT_MASK_HK_TOP */
	/* [6:6]: RG_INT_MASK_XPP_TOP */
	/* [7:7]: RG_INT_MASK_AUD_TOP */
	/* [8:8]: RG_INT_MASK_MISC_TOP */
	{0x198, 0x0, 0x1FF, 0},
	/* [8:7]: XO_AAC_MODE_LPM */
	/* [10:9]: XO_AAC_MODE_FPM */
	{0x790, 0x280, 0x780, 0},
	/* [13:13]: XO_AUDIO_EN_M */
	{0x7AC, 0x0, 0x2000, 0},
	/* [6:6]: RG_RST_DRVSEL */
	{0x98A, 0x40, 0x40, 0},
	/* [0:0]: RG_PWRHOLD */
	{0xA08, 0x1, 0x1, 0},
	/* [8:8]: RG_UVLO_DEC_EN */
	{0xA38, 0x0, 0x100, 0},
	/* [5:5]: RG_STRUP_LONG_PRESS_EXT_CHR_CTRL */
	/* [6:6]: RG_STRUP_LONG_PRESS_EXT_PWRKEY_CTRL */
	/* [7:7]: RG_STRUP_LONG_PRESS_EXT_SPAR_CTRL */
	/* [8:8]: RG_STRUP_LONG_PRESS_EXT_RTCA_CTRL */
	/* [15:15]: RG_STRUP_ENVTEM_CTRL */
	{0xA3C, 0x81E0, 0x81E0, 0},
	/* [0:0]: RG_STRUP_VDRAM2_PG_H2L_EN */
	/* [1:1]: RG_STRUP_VEMC_PG_H2L_EN */
	/* [2:2]: RG_STRUP_VSRAM_PROC12_PG_H2L_EN */
	/* [3:3]: RG_STRUP_VSRAM_PROC11_PG_H2L_EN */
	/* [4:4]: RG_STRUP_VA12_PG_H2L_EN */
	/* [5:5]: RG_STRUP_VSRAM_GPU_PG_H2L_EN */
	/* [6:6]: RG_STRUP_VSRAM_OTHERS_PG_H2L_EN */
	/* [7:7]: RG_STRUP_VAUX18_PG_H2L_EN */
	/* [8:8]: RG_STRUP_VDRAM1_PG_H2L_EN */
	/* [9:9]: RG_STRUP_VPROC12_PG_H2L_EN */
	/* [10:10]: RG_STRUP_VPROC11_PG_H2L_EN */
	/* [11:11]: RG_STRUP_VS1_PG_H2L_EN */
	/* [12:12]: RG_STRUP_VGPU_PG_H2L_EN */
	/* [13:13]: RG_STRUP_VMODEM_PG_H2L_EN */
	/* [14:14]: RG_STRUP_VCORE_PG_H2L_EN */
	/* [15:15]: RG_STRUP_VS2_PG_H2L_EN */
	{0xA44, 0xFFFF, 0xFFFF, 0},
	/* [13:13]: RG_STRUP_RSV_PG_H2L_EN */
	/* [14:14]: RG_STRUP_VAUD28_PG_H2L_EN */
	/* [15:15]: RG_STRUP_VUSB_PG_H2L_EN */
	{0xA46, 0xE000, 0xE000, 0},
	/* [10:10]: RG_SDN_DLY_ENB */
	{0xA62, 0x400, 0x400, 0},
	/* [2:2]: FG_RNG_EN_MODE */
	/* [3:3]: FG_RNG_EN_SW */
	{0xC8A, 0x4, 0xC, 0},
	/* [1:1]: RG_AUXADC_1M_CK_PDN_HWEN */
	/* [3:3]: RG_AUXADC_CK_PDN_HWEN */
	/* [5:5]: RG_AUXADC_RNG_CK_PDN_HWEN */
	/* [7:7]: RG_AUXADC_32K_CK_PDN_HWEN */
	/* [9:9]: RG_AUXADC_1K_CK_PDN_HWEN */
	/* [11:11]: RG_HK_INTRP_CK_PDN_HWEN */
	{0xF8C, 0xAAA, 0xAAA, 0},
	/* [15:15]: AUXADC_CK_AON */
	{0x1188, 0x0, 0x8000, 0},
	/* [14:12]: AUXADC_AVG_NUM_CH0 */
	{0x119E, 0x6000, 0x7000, 0},
	/* [13:12]: AUXADC_TRIM_CH6_SEL */
	{0x11A2, 0x0, 0x3000, 0},
	/* [14:14]: AUXADC_START_SHADE_EN */
	{0x11B0, 0x4000, 0x4000, 0},
	/* [8:8]: AUXADC_DATA_REUSE_EN */
	{0x11B4, 0x0, 0x100, 0},
	/* [9:0]: AUXADC_MDRT_DET_PRD */
	/* [15:15]: AUXADC_MDRT_DET_EN */
	{0x123A, 0x8040, 0x83FF, 0},
	/* [2:2]: AUXADC_MDRT_DET_WKUP_EN */
	{0x123E, 0x4, 0x4, 0},
	/* [0:0]: AUXADC_MDRT_DET_START_SEL */
	{0x1242, 0x1, 0x1, 0},
	/* [2:2]: AUXADC_LBAT_CK_SW_MODE */
	/* [4:4]: AUXADC_BAT_TEMP_CK_SW_MODE */
	/* [6:6]: AUXADC_LBAT2_CK_SW_MODE */
	/* [8:8]: AUXADC_NAG_CK_SW_MODE */
	{0x1260, 0x0, 0x154, 0},
	/* [3:3]: RG_BUCK_DCM_MODE */
	{0x1312, 0x8, 0x8, 0},
	/* [8:8]: RG_BUCK_K_CK_EN */
	{0x1334, 0x0, 0x100, 0},
	/* [8:8]: RG_BUCK_VPA_OC_SDN_EN */
	{0x1346, 0x100, 0x100, 0},
	/* [6:0]: RG_BUCK_VPROC11_VOSEL_SLEEP */
	{0x138A, 0x10, 0x7F, 0},
	/* [6:0]: RG_BUCK_VPROC11_SFCHG_FRATE */
	{0x138C, 0x15, 0x7F, 0},
	/* [5:4]: RG_BUCK_VPROC11_DVS_EN_CTRL */
	/* [13:12]: RG_BUCK_VPROC11_DVS_DOWN_CTRL */
	{0x138E, 0x1030, 0x3030, 0},
	/* [6:0]: RG_BUCK_VPROC12_VOSEL_SLEEP */
	{0x140A, 0x10, 0x7F, 0},
	/* [6:0]: RG_BUCK_VPROC12_SFCHG_FRATE */
	{0x140C, 0x15, 0x7F, 0},
	/* [5:4]: RG_BUCK_VPROC12_DVS_EN_CTRL */
	/* [13:12]: RG_BUCK_VPROC12_DVS_DOWN_CTRL */
	{0x140E, 0x1030, 0x3030, 0},
	/* [6:0]: RG_BUCK_VCORE_VOSEL_SLEEP */
	{0x148A, 0x10, 0x7F, 0},
	/* [5:4]: RG_BUCK_VCORE_DVS_EN_CTRL */
	/* [13:12]: RG_BUCK_VCORE_DVS_DOWN_CTRL */
	{0x148E, 0x1030, 0x3030, 0},
	/* [5:5]: RG_BUCK_VCORE_OSC_SEL_DIS */
	{0x14A2, 0x20, 0x20, 0},
	/* [6:0]: RG_BUCK_VGPU_VOSEL_SLEEP */
	{0x150A, 0x10, 0x7F, 0},
	/* [5:4]: RG_BUCK_VGPU_DVS_EN_CTRL */
	/* [13:12]: RG_BUCK_VGPU_DVS_DOWN_CTRL */
	{0x150E, 0x1030, 0x3030, 0},
	/* [6:0]: RG_BUCK_VMODEM_VOSEL_SLEEP */
	{0x158A, 0x8, 0x7F, 0},
	/* [6:0]: RG_BUCK_VMODEM_SFCHG_FRATE */
	/* [14:8]: RG_BUCK_VMODEM_SFCHG_RRATE */
	{0x158C, 0x90C, 0x7F7F, 0},
	/* [5:4]: RG_BUCK_VMODEM_DVS_EN_CTRL */
	/* [13:12]: RG_BUCK_VMODEM_DVS_DOWN_CTRL */
	{0x158E, 0x1030, 0x3030, 0},
	/* [3:2]: RG_BUCK_VMODEM_OC_WND */
	{0x159C, 0x8, 0xC, 0},
	/* [5:5]: RG_BUCK_VMODEM_OSC_SEL_DIS */
	{0x15A2, 0x20, 0x20, 0},
	/* [6:0]: RG_BUCK_VS1_VOSEL_SLEEP */
	{0x168A, 0x50, 0x7F, 0},
	/* [6:0]: RG_BUCK_VS1_SFCHG_FRATE */
	/* [14:8]: RG_BUCK_VS1_SFCHG_RRATE */
	{0x168C, 0x1964, 0x7F7F, 0},
	/* [5:4]: RG_BUCK_VS1_DVS_EN_CTRL */
	/* [13:12]: RG_BUCK_VS1_DVS_DOWN_CTRL */
	{0x168E, 0x2020, 0x3030, 0},
	/* [5:5]: RG_BUCK_VS1_OSC_SEL_DIS */
	{0x16A2, 0x20, 0x20, 0},
	/* [6:0]: RG_BUCK_VS1_VOTER_VOSEL */
	{0x16AA, 0x48, 0x7F, 0},
	/* [6:0]: RG_BUCK_VS2_SFCHG_FRATE */
	/* [14:8]: RG_BUCK_VS2_SFCHG_RRATE */
	{0x170C, 0x1964, 0x7F7F, 0},
	/* [5:4]: RG_BUCK_VS2_DVS_EN_CTRL */
	/* [13:12]: RG_BUCK_VS2_DVS_DOWN_CTRL */
	{0x170E, 0x2020, 0x3030, 0},
	/* [6:0]: RG_BUCK_VS2_VOTER_VOSEL */
	{0x172A, 0x3C, 0x7F, 0},
	/* [6:0]: RG_BUCK_VPA_SFCHG_FRATE */
	/* [14:8]: RG_BUCK_VPA_SFCHG_RRATE */
	{0x178C, 0x202, 0x7F7F, 0},
	/* [1:0]: RG_BUCK_VPA_DVS_TRANST_TD */
	/* [5:4]: RG_BUCK_VPA_DVS_TRANST_CTRL */
	/* [6:6]: RG_BUCK_VPA_DVS_TRANST_ONCE */
	{0x178E, 0x70, 0x73, 0},
	/* [3:2]: RG_BUCK_VPA_OC_WND */
	{0x1790, 0xC, 0xC, 0},
	/* [5:0]: RG_BUCK_VPA_VOSEL_DLC011 */
	/* [13:8]: RG_BUCK_VPA_VOSEL_DLC111 */
	{0x1798, 0x2810, 0x3F3F, 0},
	/* [13:8]: RG_BUCK_VPA_VOSEL_DLC001 */
	{0x179A, 0x800, 0x3F00, 0},
	/* [0:0]: RG_BUCK_VPA_MSFG_EN */
	{0x179E, 0x1, 0x1, 0},
	/* [13:12]: RG_VPA_BURSTH */
	{0x1808, 0x2000, 0x3000, 0},
	/* [5:5]: RG_VPROC11_FCOT */
	/* [6:6]: RG_VPROC12_FCOT */
	{0x180C, 0x60, 0x60, 0},
	/* [1:0]: RG_VPROC11_TB_WIDTH */
	/* [3:2]: RG_VPROC12_TB_WIDTH */
	/* [5:4]: RG_VPROC11_UG_SR */
	/* [7:6]: RG_VPROC11_LG_SR */
	/* [9:8]: RG_VPROC12_UG_SR */
	/* [11:10]: RG_VPROC12_LG_SR */
	/* [14:12]: RG_VPROC11_PFM_TON */
	{0x1814, 0x3FF0, 0x7FFF, 0},
	/* [2:0]: RG_VPROC12_PFM_TON */
	{0x1816, 0x3, 0x7, 0},
	/* [5:0]: RG_VPROC11_TRAN_BST */
	/* [12:7]: RG_VPROC12_TRAN_BST */
	/* [15:13]: RG_VPROC11_COTRAMP_SLP */
	{0x181A, 0x6081, 0xFFBF, 0},
	/* [2:0]: RG_VPROC12_COTRAMP_SLP */
	/* [8:7]: RG_VPROC11_VREFTB */
	/* [10:9]: RG_VPROC12_VREFTB */
	{0x181C, 0x503, 0x787, 0},
	/* [15:0]: RG_VPROC11_RSV */
	{0x181E, 0xA662, 0xFFFF, 0},
	/* [15:0]: RG_VPROC12_RSV */
	{0x1820, 0xA662, 0xFFFF, 0},
	/* [2:0]: RG_VPROC11_CSP */
	/* [5:3]: RG_VPROC11_CSN */
	/* [8:6]: RG_VPROC12_CSP */
	/* [11:9]: RG_VPROC12_CSN */
	{0x1824, 0xDB6, 0xFFF, 0},
	/* [5:5]: RG_VCORE_FCOT */
	/* [6:6]: RG_VGPU_FCOT */
	/* [8:8]: RG_VCORE_TBDIS */
	{0x1828, 0x160, 0x160, 0},
	/* [1:0]: RG_VCORE_TB_WIDTH */
	/* [3:2]: RG_VGPU_TB_WIDTH */
	/* [5:4]: RG_VCORE_UG_SR */
	/* [7:6]: RG_VCORE_LG_SR */
	/* [9:8]: RG_VGPU_UG_SR */
	/* [11:10]: RG_VGPU_LG_SR */
	/* [14:12]: RG_VCORE_PFM_TON */
	{0x1830, 0x3FF0, 0x7FFF, 0},
	/* [2:0]: RG_VGPU_PFM_TON */
	{0x1832, 0x3, 0x7, 0},
	/* [5:0]: RG_VCORE_TRAN_BST */
	/* [12:7]: RG_VGPU_TRAN_BST */
	/* [15:13]: RG_VCORE_COTRAMP_SLP */
	{0x1836, 0x6081, 0xFFBF, 0},
	/* [2:0]: RG_VGPU_COTRAMP_SLP */
	/* [8:7]: RG_VCORE_VREFTB */
	/* [10:9]: RG_VGPU_VREFTB */
	{0x1838, 0x503, 0x787, 0},
	/* [15:0]: RG_VCORE_RSV */
	{0x183A, 0xA262, 0xFFFF, 0},
	/* [15:0]: RG_VGPU_RSV */
	{0x183C, 0xA262, 0xFFFF, 0},
	/* [11:0]: RG_VCORE_CSP */
	/* [5:3]: RG_VCORE_CSN */
	/* [8:6]: RG_VGPU_CSP */
	/* [11:9]: RG_VGPU_CSN */
	{0x1840, 0xDB6, 0xFFF, 0},
	/* [2:0]: RG_VPROC11_RPSI1_TRIM */
	{0x1854, 0x0, 0x7, 0},
	/* [12:10]: RG_VPROC12_RPSI1_TRIM */
	{0x1856, 0x0, 0x1C00, 0},
	/* [2:0]: RG_VCORE_RPSI1_TRIM */
	{0x185C, 0x0, 0x7, 0},
	/* [12:10]: RG_VGPU_RPSI1_TRIM */
	{0x185E, 0x0, 0x1C00, 0},
	/* [5:2]: RG_VMODEM_RCOMP */
	/* [6:6]: RG_VMODEM_TB_DIS */
	/* [11:9]: RG_VMODEM_PFM_TON */
	{0x1888, 0x420, 0xE7C, 0},
	/* [2:0]: RG_VMODEM_COTRAMP_SLP */
	/* [11:10]: RG_VMODEM_VREFUP */
	/* [13:12]: RG_VMODEM_TB_WIDTH */
	{0x188A, 0x801, 0x3C07, 0},
	/* [1:0]: RG_VMODEM_UG_SR */
	/* [3:2]: RG_VMODEM_LG_SR */
	/* [5:4]: RG_VMODEM_CCOMP */
	{0x188C, 0x1F, 0x3F, 0},
	/* [15:0]: RG_VMODEM_RSV */
	{0x188E, 0x129A, 0xFFFF, 0},
	/* [5:3]: RG_VMODEM_CSN */
	/* [8:6]: RG_VMODEM_SONIC_PFM_TON */
	{0x1894, 0x58, 0x1F8, 0},
	/* [5:2]: RG_VDRAM1_RCOMP */
	/* [6:6]: RG_VDRAM1_TB_DIS */
	{0x1896, 0x1C, 0x7C, 0},
	/* [2:0]: RG_VDRAM1_COTRAMP_SLP */
	/* [11:10]: RG_VDRAM1_VREFUP */
	/* [13:12]: RG_VDRAM1_TB_WIDTH */
	{0x1898, 0x1805, 0x3C07, 0},
	/* [3:0]: RG_VDRAM1_UG_SR */
	{0x189A, 0xF, 0xF, 0},
	/* [15:0]: RG_VDRAM1_RSV */
	{0x189C, 0x221A, 0xFFFF, 0},
	/* [2:0]: RG_VDRAM1_CSP */
	/* [5:3]: RG_VDRAM1_CSN */
	{0x18A0, 0x2E, 0x3F, 0},
	/* [6:6]: RG_VS1_TB_DIS */
	{0x18A2, 0x0, 0x40, 0},
	/* [2:0]: RG_VS1_COTRAMP_SLP */
	/* [11:10]: RG_VS1_VREFUP */
	/* [13:12]: RG_VS1_TB_WIDTH */
	{0x18A4, 0x2C06, 0x3C07, 0},
	/* [1:0]: RG_VS1_UG_SR */
	/* [3:2]: RG_VS1_LG_SR */
	{0x18A6, 0xF, 0xF, 0},
	/* [15:0]: RG_VS1_RSV */
	{0x18A8, 0x221A, 0xFFFF, 0},
	/* [2:0]: RG_VS1_CSP */
	/* [5:3]: RG_VS1_CSN */
	{0x18AC, 0x2E, 0x3F, 0},
	/* [6:6]: RG_VS2_TB_DIS */
	{0x18AE, 0x0, 0x40, 0},
	/* [2:0]: RG_VS2_COTRAMP_SLP */
	/* [11:10]: RG_VS2_VREFUP */
	/* [13:12]: RG_VS2_TB_WIDTH */
	{0x18B0, 0x1805, 0x3C07, 0},
	/* [1:0]: RG_VS2_UG_SR */
	/* [3:2]: RG_VS2_LG_SR */
	{0x18B2, 0xF, 0xF, 0},
	/* [15:0]: RG_VS2_RSV */
	{0x18B4, 0x221A, 0xFFFF, 0},
	/* [2:0]: RG_VS2_CSP */
	/* [5:3]: RG_VS2_CSN */
	{0x18B8, 0x2E, 0x3F, 0},
	/* [5:4]: RG_VPA_CSMIR */
	/* [7:6]: RG_VPA_CSL */
	/* [10:10]: RG_VPA_AZC_EN */
	{0x18BC, 0x50, 0x4F0, 0},
	/* [3:2]: RG_VPA_SLEW */
	/* [5:4]: RG_VPA_SLEW_NMOS */
	/* [7:6]: RG_VPA_MIN_ON */
	{0x18BE, 0x3C, 0xFC, 0},
	/* [9:8]: RG_VPA_MIN_PK */
	{0x18C0, 0x0, 0x300, 0},
	/* [7:0]: RG_VPA_RSV1 */
	/* [15:8]: RG_VPA_RSV2 */
	{0x18C2, 0x8886, 0xFFFF, 0},
	/* [11:8]: RG_VPA_NLIM_SEL */
	{0x18D8, 0x700, 0xF00, 0},
	/* [0:0]: RG_LDO_32K_CK_PDN_HWEN */
	/* [1:1]: RG_LDO_INTRP_CK_PDN_HWEN */
	{0x1A0E, 0x3, 0x3, 0},
	/* [0:0]: RG_LDO_DCM_MODE */
	{0x1A10, 0x1, 0x1, 0},
	/* [0:0]: RG_LDO_VFE28_CK_SW_MODE */
	{0x1A12, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VXO22_CK_SW_MODE */
	{0x1A14, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VRF18_CK_SW_MODE */
	{0x1A16, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VRF12_CK_SW_MODE */
	{0x1A18, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VEFUSE_CK_SW_MODE */
	{0x1A1A, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VCN33_CK_SW_MODE */
	{0x1A1C, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VCN28_CK_SW_MODE */
	{0x1A1E, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VCN18_CK_SW_MODE */
	{0x1A20, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VCAMA1_CK_SW_MODE */
	{0x1A22, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VCAMA2_CK_SW_MODE */
	{0x1A24, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VCAMD_CK_SW_MODE */
	{0x1A26, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VCAMIO_CK_SW_MODE */
	{0x1A28, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VLDO28_CK_SW_MODE */
	{0x1A2A, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VA12_CK_SW_MODE */
	{0x1A2C, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VAUX18_CK_SW_MODE */
	{0x1A2E, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VAUD28_CK_SW_MODE */
	{0x1A30, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VIO28_CK_SW_MODE */
	{0x1A32, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VIO18_CK_SW_MODE */
	{0x1A34, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VSRAM_PROC11_CK_SW_MODE */
	{0x1A36, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VSRAM_PROC12_CK_SW_MODE */
	{0x1A38, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VSRAM_OTHERS_CK_SW_MODE */
	{0x1A3A, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VSRAM_GPU_CK_SW_MODE */
	{0x1A3C, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VDRAM2_CK_SW_MODE */
	{0x1A3E, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VMC_CK_SW_MODE */
	{0x1A40, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VMCH_CK_SW_MODE */
	{0x1A42, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VEMC_CK_SW_MODE */
	{0x1A44, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VSIM1_CK_SW_MODE */
	{0x1A46, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VSIM2_CK_SW_MODE */
	{0x1A48, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VIBR_CK_SW_MODE */
	{0x1A4A, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VUSB_CK_SW_MODE */
	{0x1A4C, 0x0, 0x1, 0},
	/* [0:0]: RG_LDO_VBIF28_CK_SW_MODE */
	{0x1A4E, 0x0, 0x1, 0},
	/* [6:0]: RG_LDO_VSRAM_PROC11_VOSEL_SLEEP */
	{0x1B48, 0x10, 0x7F, 0},
	/* [6:0]: RG_LDO_VSRAM_PROC11_SFCHG_FRATE */
	/* [14:8]: RG_LDO_VSRAM_PROC11_SFCHG_RRATE */
	{0x1B4A, 0xF15, 0x7F7F, 0},
	/* [6:0]: RG_LDO_VSRAM_PROC12_VOSEL_SLEEP */
	{0x1B8A, 0x10, 0x7F, 0},
	/* [6:0]: RG_LDO_VSRAM_PROC12_SFCHG_FRATE */
	/* [14:8]: RG_LDO_VSRAM_PROC12_SFCHG_RRATE */
	{0x1B8C, 0xF15, 0x7F7F, 0},
	/* [6:0]: RG_LDO_VSRAM_OTHERS_VOSEL_SLEEP */
	{0x1BA8, 0x38, 0x7F, 0},
	/* [6:0]: RG_LDO_VSRAM_OTHERS_SFCHG_FRATE */
	/* [14:8]: RG_LDO_VSRAM_OTHERS_SFCHG_RRATE */
	{0x1BAA, 0x70F, 0x7F7F, 0},
	/* [1:0]: RG_LDO_VSRAM_OTHERS_DVS_TRANS_TD */
	{0x1BAC, 0x0, 0x3, 0},
	/* [6:0]: RG_LDO_VSRAM_GPU_VOSEL_SLEEP */
	{0x1BCA, 0x10, 0x7F, 0},
	/* [6:0]: RG_LDO_VSRAM_GPU_SFCHG_FRATE */
	/* [14:8]: RG_LDO_VSRAM_GPU_SFCHG_RRATE */
	{0x1BCC, 0x70F, 0x7F7F, 0},
	/* [4:0]: RG_VSRAM_PROC11_RSV_H */
	{0x1EA2, 0x1B, 0x1F, 0},
	/* [12:10]: RG_VSRAM_PROC12_RSV_H */
	{0x1EA4, 0xC00, 0x1C00, 0},
	/* [12:10]: RG_VSRAM_OTHERS_RSV_H */
	{0x1EA6, 0xC00, 0x1C00, 0},
	/* [12:10]: RG_VSRAM_GPU_RSV_H */
	{0x1EA8, 0xC00, 0x1C00, 0},
	/* [15:0]: TMA_KEY */
	{0x3A8, 0x0, 0xFFFF, 0},

	/* MT6358 HW tracking init */
	/* [6:0]: RG_LDO_VSRAM_PROC11_VOSEL_DELTA */
	/* [14:8]: RG_LDO_VSRAM_PROC11_VOSEL_OFFSET */
	{0x1B66, 0x1000, 0x7F7F, 0},
	/* [6:0]: RG_LDO_VSRAM_PROC11_VOSEL_ON_LB */
	/* [14:8]: RG_LDO_VSRAM_PROC11_VOSEL_ON_HB */
	{0x1B68, 0x6340, 0x7F7F, 0},
	/* [1:1]: RG_LDO_VSRAM_PROC11_TRACK_ON_CTRL */
	/* [2:2]: RG_LDO_VSRAM_PROC11_TRACK_VPROC11_ON_CTRL */
	{0x1B64, 0x6, 0x6, 0},
	/* [6:0]: RG_LDO_VSRAM_PROC12_VOSEL_DELTA */
	/* [14:8]: RG_LDO_VSRAM_PROC12_VOSEL_OFFSET */
	{0x1B6E, 0x1000, 0x7F7F, 0},
	/* [6:0]: RG_LDO_VSRAM_PROC12_VOSEL_ON_LB */
	/* [14:8]: RG_LDO_VSRAM_PROC12_VOSEL_ON_HB */
	{0x1B70, 0x6340, 0x7F7F, 0},
	/* [2:1]: RG_LDO_VSRAM_PROC12_TRACK_ON_CTRL */
	/* [2:2]: RG_LDO_VSRAM_PROC12_TRACK_VPROC12_ON_CTRL */
	{0x1B6C, 0x6, 0x6, 0},

	/* Vproc11/Vproc12 to 1.05V */
	/* [6:0]: RG_BUCK_VPROC11_VOSEL */
	{0x13a6, 0x58, 0x7F, 0},
	/* [6:0]: RG_BUCK_VPROC12_VOSEL */
	{0x1426, 0x58, 0x7F, 0},
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
	/* [2:2]: RG_LDO_VRF12_HW1_OP_EN */
	{0x1C32, 0x1, 0x1, 2},
	/* [2:2]: RG_LDO_VRF12_HW1_OP_CFG */
	{0x1C38, 0x0, 0x1, 2},
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
	/* [2:2]: RG_LDO_VRF12_HW1_OP_EN */
	{0x1C32, 0x1, 0x1, 2},
	/* [2:2]: RG_LDO_VRF12_HW1_OP_CFG */
	{0x1C38, 0x0, 0x1, 2},
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

static unsigned int pmic_read_efuse(int i)
{
	unsigned int efuse_data = 0;

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
	const unsigned int cali_efuse = pmic_read_efuse(104) & 0xF;
	assert(cali_efuse < ARRAY_SIZE(vddq_votrim));
	return vddq_votrim[cali_efuse];
}

void pmic_set_power_hold(bool enable)
{
	pwrap_write_field(PMIC_PWRHOLD, (enable) ? 1 : 0, 0x1, 0);
}

void pmic_init_scp_voltage(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(scp_setting); i++)
		pwrap_write_field(
			scp_setting[i].addr, scp_setting[i].val,
			scp_setting[i].mask, scp_setting[i].shift);
}

void pmic_set_vsim2_cali(unsigned int vsim2_mv)
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
		assert(0);
		return;
	};

	/* [11:8]=0x8, RG_VSIM2_VOSEL */
	pwrap_write_field(PMIC_VSIM2_ANA_CON0, vsim2_reg, 0xF, 8);

	/* [3:0], RG_VSIM2_VOCAL */
	pwrap_write_field(PMIC_VSIM2_ANA_CON0, cali_mv / 10, 0xF, 0);
}

unsigned int pmic_get_vcore_vol(void)
{
	unsigned int vol_reg;

	vol_reg = pwrap_read_field(PMIC_VCORE_DBG0, 0x7F, 0);
	return 500000 + vol_reg * 6250;
}

void pmic_set_vcore_vol(unsigned int vcore_uv)
{
	unsigned int vol_reg;

	assert(vcore_uv >= 500000);
	assert(vcore_uv <= 1100000);

	vol_reg = (vcore_uv - 500000) / 6250;

	pwrap_write_field(PMIC_VCORE_OP_EN, 1, 0x7F, 0);
	pwrap_write_field(PMIC_VCORE_VOSEL, vol_reg, 0x7F, 0);
	udelay(1);
}

unsigned int pmic_get_vdram1_vol(void)
{
	unsigned int vol_reg;

	vol_reg = pwrap_read_field(PMIC_VDRAM1_DBG0, 0x7F, 0);
	return 500000 + vol_reg * 12500;
}

void pmic_set_vdram1_vol(unsigned int vdram_uv)
{
	unsigned int vol_reg;

	assert(vdram_uv >= 500000);
	assert(vdram_uv <= 1300000);

	vol_reg = (vdram_uv - 500000) / 12500;

	pwrap_write_field(PMIC_VDRAM1_OP_EN, 1, 0x7F, 0);
	pwrap_write_field(PMIC_VDRAM1_VOSEL, vol_reg, 0x7F, 0);
	udelay(1);
}

unsigned int pmic_get_vddq_vol(void)
{
	int efuse_votrim;
	unsigned int cali_trim;

	if (!pwrap_read_field(PMIC_VDDQ_OP_EN, 0x1, 15))
		return 0;

	efuse_votrim = pmic_get_efuse_votrim();
	cali_trim = pwrap_read_field(PMIC_VDDQ_ELR_0, 0xF, 0);
	assert(cali_trim < ARRAY_SIZE(vddq_votrim));
	return 600 * 1000 - efuse_votrim + vddq_votrim[cali_trim];
}

void pmic_set_vddq_vol(unsigned int vddq_uv)
{
	int target_mv, dram2_ori_mv, cali_offset_uv, cali_trim;

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

	pwrap_write_field(PMIC_TOP_TMA_KEY, 0x9CA7, 0xFFFF, 0);
	pwrap_write_field(PMIC_VDDQ_ELR_0, cali_trim, 0xF, 0);
	pwrap_write_field(PMIC_TOP_TMA_KEY, 0, 0xFFFF, 0);
	udelay(1);
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

static void mt6358_init_setting(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(init_setting); i++)
		pwrap_write_field(
			init_setting[i].addr, init_setting[i].val,
			init_setting[i].mask, init_setting[i].shift);
}

static void wk_sleep_voltage_by_ddr(void)
{
	if (pwrap_read_field(PMIC_VM_MODE, 0x3, 0) == 0x2)
		pwrap_write_field(PMIC_VDRAM1_VOSEL_SLEEP, 0x3A, 0x7F, 0);
}

static void wk_power_down_seq(void)
{
	/* Write TMA KEY with magic number */
	pwrap_write_field(PMIC_TOP_TMA_KEY, 0x9CA7, 0xFFFF, 0);
	/* Set VPROC12 sequence to VA12 */
	pwrap_write_field(PMIC_CPSDSA4, 0xA, 0x1F, 0);
	pwrap_write_field(PMIC_TOP_TMA_KEY, 0x0, 0xFFFF, 0);
}

static void mt6358_lp_setting(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(lp_setting); i++)
		pwrap_write_field(
			lp_setting[i].addr, lp_setting[i].val,
			lp_setting[i].mask, lp_setting[i].shift);
}

void mt6358_init(void)
{
	struct stopwatch voltage_settled;

	if (pwrap_init())
		die("ERROR - Failed to initialize pmic wrap!");

	pmic_set_power_hold(true);
	pmic_wdt_set();
	mt6358_init_setting();
	stopwatch_init_usecs_expire(&voltage_settled, 200);
	wk_sleep_voltage_by_ddr();
	wk_power_down_seq();
	mt6358_lp_setting();
	while (!stopwatch_expired(&voltage_settled))
		/* wait for voltages to settle */;
}
