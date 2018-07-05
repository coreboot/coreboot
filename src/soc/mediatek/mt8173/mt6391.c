/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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
#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/mt6391.h>
#include <soc/pmic_wrap.h>
#include <types.h>

#if IS_ENABLED(CONFIG_DEBUG_PMIC)
#define DEBUG_PMIC(level, x...)		printk(level, x)
#else
#define DEBUG_PMIC(level, x...)
#endif

u32 mt6391_read(u16 reg, u32 mask, u32 shift)
{
	u16 rdata;

	pwrap_wacs2(0, reg, 0, &rdata, 1);
	rdata &= (mask << shift);
	rdata = (rdata >> shift);

	DEBUG_PMIC(BIOS_INFO, "[%s] Reg[%#x]=%#x\n",
		   __func__, reg, rdata);
	return rdata;
}

void mt6391_write(u16 reg, u16 val, u32 mask, u32 shift)
{
	u16 rdata;
	u16 old_rdata, new_rdata;

	pwrap_wacs2(0, reg, 0, &rdata, 1);
	old_rdata = rdata;

	rdata &= ~(mask << shift);
	rdata |= (val << shift);
	new_rdata = rdata;

	pwrap_wacs2(1, reg, rdata, &rdata, 1);

	DEBUG_PMIC(BIOS_INFO, "[%s] write Reg[%#x]=%#x -> %#x\n",
		    __func__, reg, old_rdata, new_rdata);
	return;
}

int mt6391_configure_ca53_voltage(int uv)
{
	/* target voltage = 700mv + 6.25mv * buck_val */
	u16 buck_val = (uv - 700000) / 6250;
	u16 current_val = mt6391_read(PMIC_RG_VCA15_CON12, 0x7f, 0x0);

	assert(buck_val < (1 << 8));
	mt6391_write(PMIC_RG_VCA15_CON9, buck_val, 0x7f, 0x0);
	mt6391_write(PMIC_RG_VCA15_CON10, buck_val, 0x7f, 0x0);

	/* For buck delay, default slew rate is 6.25mv/0.5us */
	if (buck_val > current_val)
		return ((buck_val - current_val) / 2) ;
	else
		return 0;
}

static void mt6391_configure_vcama(enum ldo_voltage vsel)
{
	/* 2'b00: 1.5V
	 * 2'b01: 1.8V
	 * 2'b10: 2.5V
	 * 2'b11: 2.8V
	 */
	mt6391_write(PMIC_RG_ANALDO_CON6, vsel - 2, PMIC_RG_VCAMA_VOSEL_MASK,
		     PMIC_RG_VCAMA_VOSEL_SHIFT);
	mt6391_write(PMIC_RG_ANALDO_CON2, 1, PMIC_RG_VCAMA_EN_MASK,
                     PMIC_RG_VCAMA_EN_SHIFT);
}

void mt6391_configure_ldo(enum ldo_power ldo, enum ldo_voltage vsel)
{
	u16 addr;
	switch (ldo) {
	case LDO_VCAMD:
		assert(vsel != 0);
		if (vsel == LDO_1P22)
			vsel = 0;
		break;
	case LDO_VGP2:
		assert(vsel != 1);
		if (vsel == LDO_1P0)
			vsel = 1;
		break;
	case LDO_VGP5:
		assert(vsel != 7);
		if (vsel == LDO_2P0)
			vsel = 7;
		break;
	case LDO_VCAMA:
		assert(vsel > LDO_1P3 && vsel < LDO_3P0);
		mt6391_configure_vcama(vsel);
		return;
	default:
		break;
	}
	assert(vsel < LDO_NUM_VOLTAGES);

	if (ldo == LDO_VGP6)
		addr = PMIC_RG_DIGLDO_CON33;
	else
		addr = PMIC_RG_DIGLDO_CON19 + ldo * 2;

	mt6391_write(addr, vsel, 0x7, 5);
	mt6391_write(PMIC_RG_DIGLDO_CON5 + ldo * 2, 1, 1, 15);

}

void mt6391_enable_reset_when_ap_resets(void)
{
	/* Enable AP watchdog reset */
	mt6391_write(PMIC_RG_TOP_RST_MISC, 0x0, 0x1, 0);
}

static void mt6391_init_setting(void)
{
	/* Enable PMIC RST function (depends on main chip RST function) */
	/*
	 * state1: RG_SYSRSTB_EN = 1, RG_STRUP_MAN_RST_EN=1, RG_RST_PART_SEL=1
	 * state2: RG_SYSRSTB_EN = 1, RG_STRUP_MAN_RST_EN=0, RG_RST_PART_SEL=1
	 * state3: RG_SYSRSTB_EN = 1, RG_STRUP_MAN_RST_EN=x, RG_RST_PART_SEL=0
	 */
	mt6391_write(PMIC_RG_TOP_RST_MISC, 0x1, 0x1, 1);
	mt6391_write(PMIC_RG_TOP_RST_MISC, 0x0, 0x1, 2);
	mt6391_write(PMIC_RG_TOP_RST_MISC, 0x1, 0x1, 4);

	/* Disable AP watchdog reset */
	mt6391_write(PMIC_RG_TOP_RST_MISC, 0x1, 0x1, 0);

	/* Enable CA15 by default for different PMIC behavior */
	mt6391_write(PMIC_RG_VCA15_CON7, 0x1, 0x1, 0);
	mt6391_write(PMIC_RG_VSRMCA15_CON7, 0x1, 0x1, 0);
	mt6391_write(PMIC_RG_VPCA7_CON7, 0x1, 0x1, 0);
	udelay(200);  /* delay for Buck ready */

	/* [3:3]: RG_PWMOC_CK_PDN; For OC protection */
	mt6391_write(PMIC_RG_TOP_CKPDN, 0x0, 0x1, 3);
	/* [9:9]: RG_SRCVOLT_HW_AUTO_EN; */
	mt6391_write(PMIC_RG_TOP_CKCON1, 0x1, 0x1, 9);
	/* [8:8]: RG_OSC_SEL_AUTO; */
	mt6391_write(PMIC_RG_TOP_CKCON1, 0x1, 0x1, 8);
	/* [6:6]: RG_SMPS_DIV2_SRC_AUTOFF_DIS; */
	mt6391_write(PMIC_RG_TOP_CKCON1, 0x1, 0x1, 6);
	/* [5:5]: RG_SMPS_AUTOFF_DIS; */
	mt6391_write(PMIC_RG_TOP_CKCON1, 0x1, 0x1, 5);
	/* [7:7]: VDRM_DEG_EN; */
	mt6391_write(PMIC_RG_OC_DEG_EN, 0x1, 0x1, 7);
	/* [6:6]: VSRMCA7_DEG_EN; */
	mt6391_write(PMIC_RG_OC_DEG_EN, 0x1, 0x1, 6);
	/* [5:5]: VPCA7_DEG_EN; */
	mt6391_write(PMIC_RG_OC_DEG_EN, 0x1, 0x1, 5);
	/* [4:4]: VIO18_DEG_EN; */
	mt6391_write(PMIC_RG_OC_DEG_EN, 0x1, 0x1, 4);
	/* [3:3]: VGPU_DEG_EN; For OC protection */
	mt6391_write(PMIC_RG_OC_DEG_EN, 0x1, 0x1, 3);
	/* [2:2]: VCORE_DEG_EN; */
	mt6391_write(PMIC_RG_OC_DEG_EN, 0x1, 0x1, 2);
	/* [1:1]: VSRMCA15_DEG_EN; */
	mt6391_write(PMIC_RG_OC_DEG_EN, 0x1, 0x1, 1);
	/* [0:0]: VCA15_DEG_EN; */
	mt6391_write(PMIC_RG_OC_DEG_EN, 0x1, 0x1, 0);
	/* [11:11]: RG_INT_EN_THR_H; */
	mt6391_write(PMIC_RG_INT_CON0, 0x1, 0x1, 11);
	/* [10:10]: RG_INT_EN_THR_L; */
	mt6391_write(PMIC_RG_INT_CON0, 0x1, 0x1, 10);
	/* [4:4]: RG_INT_EN_BAT_L; */
	mt6391_write(PMIC_RG_INT_CON0, 0x1, 0x1, 4);
	/* [11:11]: RG_INT_EN_VGPU; OC protection */
	mt6391_write(PMIC_RG_INT_CON1, 0x1, 0x1, 11);
	/* [8:8]: RG_INT_EN_VCA15; OC protection */
	mt6391_write(PMIC_RG_INT_CON1, 0x1, 0x1, 8);
	/* [12:0]: BUCK_RSV; for OC protection */
	mt6391_write(PMIC_RG_BUCK_CON3, 0x600, 0x0FFF, 0);
	/* [11:10]: QI_VCORE_VSLEEP; sleep mode only (0.7V) */
	mt6391_write(PMIC_RG_BUCK_CON8, 0x0, 0x3, 10);
	/* [7:6]: QI_VSRMCA7_VSLEEP; sleep mode only (0.85V) */
	mt6391_write(PMIC_RG_BUCK_CON8, 0x0, 0x3, 6);
	/* [5:4]: QI_VSRMCA15_VSLEEP; sleep mode only (0.7V) */
	mt6391_write(PMIC_RG_BUCK_CON8, 0x1, 0x3, 4);
	/* [3:2]: QI_VPCA7_VSLEEP; sleep mode only (0.85V) */
	mt6391_write(PMIC_RG_BUCK_CON8, 0x0, 0x3, 2);
	/* [1:0]: QI_VCA15_VSLEEP; sleep mode only (0.7V) */
	mt6391_write(PMIC_RG_BUCK_CON8, 0x1, 0x3, 0);
	/* [13:12]: RG_VCA15_CSL2; for OC protection */
	mt6391_write(PMIC_RG_VCA15_CON1, 0x0, 0x3, 12);
	/* [11:10]: RG_VCA15_CSL1; for OC protection */
	mt6391_write(PMIC_RG_VCA15_CON1, 0x0, 0x3, 10);
	/* [15:15]: VCA15_SFCHG_REN; soft change rising enable */
	mt6391_write(PMIC_RG_VCA15_CON8, 0x1, 0x1, 15);
	/* [14:8]: VCA15_SFCHG_RRATE; soft change rising step=0.5 */
	mt6391_write(PMIC_RG_VCA15_CON8, 0x5, 0x7F, 8);
	/* [7:7]: VCA15_SFCHG_FEN; soft change falling enable */
	mt6391_write(PMIC_RG_VCA15_CON8, 0x1, 0x1, 7);
	/* [6:0]: VCA15_SFCHG_FRATE; soft change falling step=2us */
	mt6391_write(PMIC_RG_VCA15_CON8, 0x17, 0x7F, 0);
	/* [6:0]: VCA15_VOSEL_SLEEP; sleep mode only (0.7V) */
	mt6391_write(PMIC_RG_VCA15_CON11, 0x0, 0x7F, 0);
	/* [8:8]: VCA15_VSLEEP_EN; set sleep mode reference volt */
	mt6391_write(PMIC_RG_VCA15_CON18, 0x1, 0x1, 8);
	/* [5:4]: VCA15_VOSEL_TRANS_EN; rising & falling enable */
	mt6391_write(PMIC_RG_VCA15_CON18, 0x3, 0x3, 4);
	/* [5:5]: VSRMCA15_TRACK_SLEEP_CTRL; */
	mt6391_write(PMIC_RG_VSRMCA15_CON5, 0x1, 0x1, 5);
	/* [5:4]: VSRMCA15_VOSEL_SEL; */
	mt6391_write(PMIC_RG_VSRMCA15_CON6, 0x0, 0x3, 4);
	/* [15:15]: VSRMCA15_SFCHG_REN; */
	mt6391_write(PMIC_RG_VSRMCA15_CON8, 0x1, 0x1, 15);
	/* [14:8]: VSRMCA15_SFCHG_RRATE; */
	mt6391_write(PMIC_RG_VSRMCA15_CON8, 0x5, 0x7F, 8);
	/* [7:7]: VSRMCA15_SFCHG_FEN; */
	mt6391_write(PMIC_RG_VSRMCA15_CON8, 0x1, 0x1, 7);
	/* [6:0]: VSRMCA15_SFCHG_FRATE; */
	mt6391_write(PMIC_RG_VSRMCA15_CON8, 0x17, 0x7F, 0);
	/* [6:0]: VSRMCA15_VOSEL_SLEEP; Sleep mode setting on */
	mt6391_write(PMIC_RG_VSRMCA15_CON11, 0x00, 0x7F, 0);
	/* [8:8]: VSRMCA15_VSLEEP_EN; set sleep mode referenc */
	mt6391_write(PMIC_RG_VSRMCA15_CON18, 0x1, 0x1, 8);
	/* [5:4]: VSRMCA15_VOSEL_TRANS_EN; rising & falling e */
	mt6391_write(PMIC_RG_VSRMCA15_CON18, 0x3, 0x3, 4);
	/* [1:1]: VCORE_VOSEL_CTRL; sleep mode voltage control fo */
	mt6391_write(PMIC_RG_VCORE_CON5, 0x1, 0x1, 1);
	/* [5:4]: VCORE_VOSEL_SEL; */
	mt6391_write(PMIC_RG_VCORE_CON6, 0x0, 0x3, 4);
	/* [15:15]: VCORE_SFCHG_REN; */
	mt6391_write(PMIC_RG_VCORE_CON8, 0x1, 0x1, 15);
	/* [14:8]: VCORE_SFCHG_RRATE; */
	mt6391_write(PMIC_RG_VCORE_CON8, 0x5, 0x7F, 8);
	/* [6:0]: VCORE_SFCHG_FRATE; */
	mt6391_write(PMIC_RG_VCORE_CON8, 0x17, 0x7F, 0);
	/* [6:0]: VCORE_VOSEL_SLEEP; Sleep mode setting only (0. */
	mt6391_write(PMIC_RG_VCORE_CON11, 0x0, 0x7F, 0);
	/* [8:8]: VCORE_VSLEEP_EN; Sleep mode HW control  R2R to */
	mt6391_write(PMIC_RG_VCORE_CON18, 0x1, 0x1, 8);
	/* [5:4]: VCORE_VOSEL_TRANS_EN; Follows MT6320 VCORE set */
	mt6391_write(PMIC_RG_VCORE_CON18, 0x0, 0x3, 4);
	/* [1:0]: VCORE_TRANSTD; */
	mt6391_write(PMIC_RG_VCORE_CON18, 0x3, 0x3, 0);
	/* [9:8]: RG_VGPU_CSL; for OC protection */
	mt6391_write(PMIC_RG_VGPU_CON1, 0x1, 0x3, 8);
	/* [15:15]: VGPU_SFCHG_REN; */
	mt6391_write(PMIC_RG_VGPU_CON8, 0x1, 0x1, 15);
	/* [14:8]: VGPU_SFCHG_RRATE; */
	mt6391_write(PMIC_RG_VGPU_CON8, 0x5, 0x7F, 8);
	/* [6:0]: VGPU_SFCHG_FRATE; */
	mt6391_write(PMIC_RG_VGPU_CON8, 0x17, 0x7F, 0);
	/* [5:4]: VGPU_VOSEL_TRANS_EN; */
	mt6391_write(PMIC_RG_VGPU_CON18, 0x0, 0x3, 4);
	/* [1:0]: VGPU_TRANSTD; */
	mt6391_write(PMIC_RG_VGPU_CON18, 0x3, 0x3, 0);
	/* [5:4]: VPCA7_VOSEL_SEL; */
	mt6391_write(PMIC_RG_VPCA7_CON6, 0x0, 0x3, 4);
	/* [15:15]: VPCA7_SFCHG_REN; */
	mt6391_write(PMIC_RG_VPCA7_CON8, 0x1, 0x1, 15);
	/* [14:8]: VPCA7_SFCHG_RRATE; */
	mt6391_write(PMIC_RG_VPCA7_CON8, 0x5, 0x7F, 8);
	/* [7:7]: VPCA7_SFCHG_FEN; */
	mt6391_write(PMIC_RG_VPCA7_CON8, 0x1, 0x1, 7);
	/* [6:0]: VPCA7_SFCHG_FRATE; */
	mt6391_write(PMIC_RG_VPCA7_CON8, 0x17, 0x7F, 0);
	/* [6:0]: VPCA7_VOSEL_SLEEP; */
	mt6391_write(PMIC_RG_VPCA7_CON11, 0x18, 0x7F, 0);
	/* [8:8]: VPCA7_VSLEEP_EN; */
	mt6391_write(PMIC_RG_VPCA7_CON18, 0x0, 0x1, 8);
	/* [5:4]: VPCA7_VOSEL_TRANS_EN; */
	mt6391_write(PMIC_RG_VPCA7_CON18, 0x3, 0x3, 4);
	/* [5:5]: VSRMCA7_TRACK_SLEEP_CTRL; */
	mt6391_write(PMIC_RG_VSRMCA7_CON5, 0x0, 0x1, 5);
	/* [5:4]: VSRMCA7_VOSEL_SEL; */
	mt6391_write(PMIC_RG_VSRMCA7_CON6, 0x0, 0x3, 4);
	/* [15:15]: VSRMCA7_SFCHG_REN; */
	mt6391_write(PMIC_RG_VSRMCA7_CON8, 0x1, 0x1, 15);
	/* [14:8]: VSRMCA7_SFCHG_RRATE; */
	mt6391_write(PMIC_RG_VSRMCA7_CON8, 0x5, 0x7F, 8);
	/* [7:7]: VSRMCA7_SFCHG_FEN; */
	mt6391_write(PMIC_RG_VSRMCA7_CON8, 0x1, 0x1, 7);
	/* [6:0]: VSRMCA7_SFCHG_FRATE; */
	mt6391_write(PMIC_RG_VSRMCA7_CON8, 0x17, 0x7F, 0);
	/* [6:0]: VSRMCA7_VOSEL_SLEEP; */
	mt6391_write(PMIC_RG_VSRMCA7_CON11, 0x18, 0x7F, 0);
	/* [8:8]: VSRMCA7_VSLEEP_EN; */
	mt6391_write(PMIC_RG_VSRMCA7_CON18, 0x0, 0x1, 8);
	/* [5:4]: VSRMCA7_VOSEL_TRANS_EN; */
	mt6391_write(PMIC_RG_VSRMCA7_CON18, 0x3, 0x3, 4);
	/* [8:8]: VDRM_VSLEEP_EN; */
	mt6391_write(PMIC_RG_VDRM_CON18, 0x1, 0x1, 8);
	/* [2:2]: VIBR_THER_SHEN_EN; */
	mt6391_write(PMIC_RG_DIGLDO_CON24, 0x1, 0x1, 2);
	/* [5:5]: THR_HWPDN_EN; */
	mt6391_write(PMIC_RG_STRUP_CON0, 0x1, 0x1, 5);
	/* [3:3]: RG_RST_DRVSEL; */
	mt6391_write(PMIC_RG_STRUP_CON2, 0x1, 0x1, 3);
	/* [2:2]: RG_EN_DRVSEL; */
	mt6391_write(PMIC_RG_STRUP_CON2, 0x1, 0x1, 2);
	/* [1:1]: PWRBB_DEB_EN; */
	mt6391_write(PMIC_RG_STRUP_CON5, 0x1, 0x1, 1);
	/* [12:12]: VSRMCA15_PG_H2L_EN; */
	mt6391_write(PMIC_RG_STRUP_CON7, 0x1, 0x1, 12);
	/* [11:11]: VPCA15_PG_H2L_EN; */
	mt6391_write(PMIC_RG_STRUP_CON7, 0x1, 0x1, 11);
	/* [10:10]: VCORE_PG_H2L_EN; */
	mt6391_write(PMIC_RG_STRUP_CON7, 0x1, 0x1, 10);
	/* [9:9]: VSRMCA7_PG_H2L_EN; */
	mt6391_write(PMIC_RG_STRUP_CON7, 0x1, 0x1, 9);
	/* [8:8]: VPCA7_PG_H2L_EN; */
	mt6391_write(PMIC_RG_STRUP_CON7, 0x1, 0x1, 8);
	/* [1:1]: STRUP_PWROFF_PREOFF_EN; */
	mt6391_write(PMIC_RG_STRUP_CON10, 0x1, 0x1, 1);
	/* [0:0]: STRUP_PWROFF_SEQ_EN; */
	mt6391_write(PMIC_RG_STRUP_CON10, 0x1, 0x1, 0);
	/* [15:8]: RG_ADC_TRIM_CH_SEL; */
	mt6391_write(PMIC_RG_AUXADC_CON14, 0xFC, 0xFF, 8);
	/* [1:1]: FLASH_THER_SHDN_EN; */
	mt6391_write(PMIC_RG_FLASH_CON0, 0x1, 0x1, 1);
	/* [1:1]: KPLED_THER_SHDN_EN; */
	mt6391_write(PMIC_RG_KPLED_CON0, 0x1, 0x1, 1);
	/* [14:8]: VSRMCA15_VOSEL_OFFSET; set offset=100mV */
	mt6391_write(PMIC_RG_VSRMCA15_CON19, 0x10, 0x7F, 8);
	/* [6:0]: VSRMCA15_VOSEL_DELTA; set delta=0mV */
	mt6391_write(PMIC_RG_VSRMCA15_CON19, 0x0, 0x7F, 0);
	/* [14:8]: VSRMCA15_VOSEL_ON_HB; set HB=1.15V */
	mt6391_write(PMIC_RG_VSRMCA15_CON20, 0x48, 0x7F, 8);
	/* [6:0]: VSRMCA15_VOSEL_ON_LB; set LB=0.7V */
	mt6391_write(PMIC_RG_VSRMCA15_CON20, 0x0, 0x7F, 0);
	/* [6:0]: VSRMCA15_VOSEL_SLEEP_LB; set sleep LB=0.7V */
	mt6391_write(PMIC_RG_VSRMCA15_CON21, 0x0, 0x7F, 0);
	/* [14:8]: VSRMCA7_VOSEL_OFFSET; set offset=25mV */
	mt6391_write(PMIC_RG_VSRMCA7_CON19, 0x4, 0x7F, 8);
	/* [6:0]: VSRMCA7_VOSEL_DELTA; set delta=0mV */
	mt6391_write(PMIC_RG_VSRMCA7_CON19, 0x0, 0x7F, 0);
	/* [14:8]: VSRMCA7_VOSEL_ON_HB; set HB=1.275V */
	mt6391_write(PMIC_RG_VSRMCA7_CON20, 0x5C, 0x7F, 8);
	/* [6:0]: VSRMCA7_VOSEL_ON_LB; set LB=1.05000V */
	mt6391_write(PMIC_RG_VSRMCA7_CON20, 0x38, 0x7F, 0);
	/* [6:0]: VSRMCA7_VOSEL_SLEEP_LB; set sleep LB=0.85000 */
	mt6391_write(PMIC_RG_VSRMCA7_CON21, 0x18, 0x7F, 0);
	/* [1:1]: VCA15_VOSEL_CTRL, VCA15_EN_CTRL; DVS HW control */
	mt6391_write(PMIC_RG_VCA15_CON5, 0x3, 0x3, 0);
	/* [1:1]: VSRMCA15_VOSEL_CTRL, VSRAM15_EN_CTRL; */
	mt6391_write(PMIC_RG_VSRMCA15_CON5, 0x3, 0x3, 0);
	/* [1:1]: VPCA7_VOSEL_CTRL; */
	mt6391_write(PMIC_RG_VPCA7_CON5, 0x0, 0x1, 1);
	/* [1:1]: VSRMCA7_VOSEL_CTRL; */
	mt6391_write(PMIC_RG_VSRMCA7_CON5, 0x0, 0x1, 1);
	/* [0:0]: VSRMCA7_EN_CTRL; */
	mt6391_write(PMIC_RG_VSRMCA7_CON5, 0x1, 0x1, 0);
	/* [4:4]: VCA15_TRACK_ON_CTRL; DVFS tracking enable */
	mt6391_write(PMIC_RG_VCA15_CON5, 0x1, 0x1, 4);
	/* [4:4]: VSRMCA15_TRACK_ON_CTRL; */
	mt6391_write(PMIC_RG_VSRMCA15_CON5, 0x1, 0x1, 4);
	/* [4:4]: VPCA7_TRACK_ON_CTRL; */
	mt6391_write(PMIC_RG_VPCA7_CON5, 0x0, 0x1, 4);
	/* [4:4]: VSRMCA7_TRACK_ON_CTRL; */
	mt6391_write(PMIC_RG_VSRMCA7_CON5, 0x0, 0x1, 4);
	/* [15:14]: VGPU OC; */
	mt6391_write(PMIC_RG_OC_CTL1, 0x3, 0x3, 14);
	/* [3:2]: VCA15 OC; */
	mt6391_write(PMIC_RG_OC_CTL1, 0x3, 0x3, 2);

	/* Set VPCA7 to 1.2V */
	mt6391_write(PMIC_RG_VPCA7_CON9, 0x50, 0x7f, 0x0);
	mt6391_write(PMIC_RG_VPCA7_CON10, 0x50, 0x7f, 0x0);
	/* Set VSRMCA7 to 1.1V */
	mt6391_write(PMIC_RG_VSRMCA7_CON9, 0x40, 0x7f, 0x0);
	mt6391_write(PMIC_RG_VSRMCA7_CON10, 0x40, 0x7f, 0x0);

	/* Enable VGP6 and set to 3.3V*/
	mt6391_write(PMIC_RG_DIGLDO_CON10, 0x1, 0x1, 15);
	mt6391_write(PMIC_RG_DIGLDO_CON33, 0x07, 0x07, 5);

	/* Set VDRM to 1.21875V */
	mt6391_write(PMIC_RG_VDRM_CON9, 0x43, 0x7F, 0);
	mt6391_write(PMIC_RG_VDRM_CON10, 0x43, 0x7F, 0);

	/* 26M clock amplitute adjust */
	mt6391_write(PMIC_RG_DCXO_ANALOG_CON1, 0x0, 0x3, 2);
	mt6391_write(PMIC_RG_DCXO_ANALOG_CON1, 0x1, 0x3, 11);

	/* For low power, set VTCXO switch by SRCVOLTEN */
	mt6391_write(PMIC_RG_DIGLDO_CON27, 0x0100, 0x0100, 0);
	/* [6:5]=0(VTCXO_SRCLK_MODE_SEL) */
	mt6391_write(PMIC_RG_ANALDO_CON0, 0, 0x3, 13);
	/* [11]=0(VTCXO_ON_CTRL), */
	mt6391_write(PMIC_RG_ANALDO_CON0, 1, 0x1, 11);
	/* [10]=1(RG_VTCXO_EN), */
	mt6391_write(PMIC_RG_ANALDO_CON0, 1, 0x1, 10);
	/* [4:3]=1(RG_VTCXOTD_SEL) */
	mt6391_write(PMIC_RG_ANALDO_CON0, 0x3, 0x3, 3);
	/* For low power, VIO18 set sleep_en to HW mode */
	mt6391_write(PMIC_RG_VIO18_CON18, 0x1, 0x1, 8);

}

static void mt6391_default_buck_voltage(void)
{
	u16 reg_val = 0;
	u16 buck_val = 0;
	/*
	 * There are two kinds of PMIC used for MT8173 : MT6397s/MT6391.
	 *
	 * MT6397s: the default voltage of register was not suitable for
	 * MT8173, needs to apply the setting of eFuse.
	 * VPCA15/VSRMCA15/: 1.15V
	 * VCORE: 1.05V
	 *
	 * MT6391: the default voltage of register was matched for MT8173.
	 * VPAC15/VCORE/VGPU: 1.0V
	 * VSRMCA15: 1.0125V
	 */
	reg_val = mt6391_read(PMIC_RG_EFUSE_DOUT_288_303, 0xFFFF, 0);

	if ((reg_val & 0x01) == 0x01) {
		/* VCORE */
		reg_val = mt6391_read(PMIC_RG_EFUSE_DOUT_256_271, 0xF, 12);
		buck_val = mt6391_read(PMIC_RG_VCORE_CON9, 0x7f, 0x0);

		/* VCORE_VOSEL[3:6] => eFuse bit 268-271 */
		buck_val = (buck_val & 0x07) | (reg_val << 3);
		mt6391_write(PMIC_RG_VCORE_CON9, buck_val, 0x7f, 0x0);
		mt6391_write(PMIC_RG_VCORE_CON10, buck_val, 0x7f, 0x0);

		reg_val = mt6391_read(PMIC_RG_EFUSE_DOUT_272_287, 0xFFFF, 0);
		/* VCA15 */
		buck_val = 0;
		buck_val = mt6391_read(PMIC_RG_VCA15_CON9, 0x7f, 0x0);
		buck_val = (buck_val & 0x07) | ((reg_val & 0x0F) << 3);
		mt6391_write(PMIC_RG_VCA15_CON9, buck_val, 0x7f, 0x0);
		mt6391_write(PMIC_RG_VCA15_CON10, buck_val, 0x7f, 0x0);

		/* VSAMRCA15 */
		buck_val = 0;
		buck_val = mt6391_read(PMIC_RG_VSRMCA15_CON9, 0x7f, 0x0);
		buck_val = (buck_val & 0x07) | ((reg_val & 0xF0) >> 1);
		mt6391_write(PMIC_RG_VSRMCA15_CON9, buck_val, 0x7f, 0x0);
		mt6391_write(PMIC_RG_VSRMCA15_CON10, buck_val, 0x7f, 0x0);

		/* set the power control by register(use original) */
		mt6391_write(PMIC_RG_BUCK_CON3, 0x1, 0x1, 12);
	}
}

void mt6391_init(void)
{
	if (pwrap_init())
		die("ERROR - Failed to initialize pmic wrap!");
	/* pmic initial setting */
	mt6391_init_setting();

	/* Adjust default BUCK voltage from eFuse */
	mt6391_default_buck_voltage();
}

/* API of GPIO in PMIC MT6391 */
enum {
	MAX_GPIO_REG_BITS = 16,
	MAX_GPIO_MODE_PER_REG = 5,
	GPIO_MODE_BITS = 3,
	GPIO_PORT_OFFSET = 3,
	GPIO_SET_OFFSET = 2,
	GPIO_RST_OFFSET = 4,
	MAX_MT6391_GPIO = 40
};

enum {
	MT6391_GPIO_DIRECTION_IN = 0,
	MT6391_GPIO_DIRECTION_OUT = 1,
};

enum {
	MT6391_GPIO_MODE = 0,
};

static void pos_bit_calc(u32 pin, u16 *pos, u16 *bit)
{
	*pos = (pin / MAX_GPIO_REG_BITS) << GPIO_PORT_OFFSET;
	*bit = pin % MAX_GPIO_REG_BITS;
}

static void pos_bit_calc_mode(u32 pin, u16 *pos, u16 *bit)
{
	*pos = (pin / MAX_GPIO_MODE_PER_REG) << GPIO_PORT_OFFSET;
	*bit = (pin % MAX_GPIO_MODE_PER_REG) * GPIO_MODE_BITS;
}

static s32 mt6391_gpio_set_dir(u32 pin, u32 dir)
{
	u16 pos;
	u16 bit;
	u16 reg;

	assert(pin <= MAX_MT6391_GPIO);

	pos_bit_calc(pin, &pos, &bit);

	if (dir == MT6391_GPIO_DIRECTION_IN)
		reg = MT6391_GPIO_DIR_BASE + pos + GPIO_RST_OFFSET;
	else
		reg = MT6391_GPIO_DIR_BASE + pos + GPIO_SET_OFFSET;

	if (pwrap_write(reg, 1L << bit) != 0)
		return -1;

	return 0;
}

void mt6391_gpio_set_pull(u32 pin, enum mt6391_pull_enable enable,
			  enum mt6391_pull_select select)
{
	u16 pos;
	u16 bit;
	u16 en_reg, sel_reg;

	assert(pin <= MAX_MT6391_GPIO);

	pos_bit_calc(pin, &pos, &bit);

	if (enable == MT6391_GPIO_PULL_DISABLE) {
		en_reg = MT6391_GPIO_PULLEN_BASE + pos + GPIO_RST_OFFSET;
	} else {
		en_reg = MT6391_GPIO_PULLEN_BASE + pos + GPIO_SET_OFFSET;
		sel_reg = (select == MT6391_GPIO_PULL_DOWN) ?
			  (MT6391_GPIO_PULLSEL_BASE + pos + GPIO_RST_OFFSET) :
			  (MT6391_GPIO_PULLSEL_BASE + pos + GPIO_SET_OFFSET);
		pwrap_write(sel_reg, 1L << bit);
	}
	pwrap_write(en_reg, 1L << bit);
}

int mt6391_gpio_get(u32 pin)
{
	u16 pos;
	u16 bit;
	u16 reg;
	u16 data;

	assert(pin <= MAX_MT6391_GPIO);

	pos_bit_calc(pin, &pos, &bit);

	reg = MT6391_GPIO_DIN_BASE + pos;
	pwrap_read(reg, &data);

	return (data & (1L << bit)) ? 1 : 0;
}

void mt6391_gpio_set(u32 pin, int output)
{
	u16 pos;
	u16 bit;
	u16 reg;

	assert(pin <= MAX_MT6391_GPIO);

	pos_bit_calc(pin, &pos, &bit);

	if (output == 0)
		reg = MT6391_GPIO_DOUT_BASE + pos + GPIO_RST_OFFSET;
	else
		reg = MT6391_GPIO_DOUT_BASE + pos + GPIO_SET_OFFSET;

	pwrap_write(reg, 1L << bit);
}

void mt6391_gpio_set_mode(u32 pin, int mode)
{
	u16 pos;
	u16 bit;
	u16 mask = (1L << GPIO_MODE_BITS) - 1;

	assert(pin <= MAX_MT6391_GPIO);

	pos_bit_calc_mode(pin, &pos, &bit);
	mt6391_write(MT6391_GPIO_MODE_BASE + pos, mode, mask, bit);
}

void mt6391_gpio_input_pulldown(u32 gpio)
{
	mt6391_gpio_set_pull(gpio, MT6391_GPIO_PULL_ENABLE,
			     MT6391_GPIO_PULL_DOWN);
	mt6391_gpio_set_dir(gpio, MT6391_GPIO_DIRECTION_IN);
	mt6391_gpio_set_mode(gpio, MT6391_GPIO_MODE);
}

void mt6391_gpio_input_pullup(u32 gpio)
{
	mt6391_gpio_set_pull(gpio, MT6391_GPIO_PULL_ENABLE,
			     MT6391_GPIO_PULL_UP);
	mt6391_gpio_set_dir(gpio, MT6391_GPIO_DIRECTION_IN);
	mt6391_gpio_set_mode(gpio, MT6391_GPIO_MODE);
}

void mt6391_gpio_input(u32 gpio)
{
	mt6391_gpio_set_pull(gpio, MT6391_GPIO_PULL_DISABLE,
			     MT6391_GPIO_PULL_DOWN);
	mt6391_gpio_set_dir(gpio, MT6391_GPIO_DIRECTION_IN);
	mt6391_gpio_set_mode(gpio, MT6391_GPIO_MODE);
}

void mt6391_gpio_output(u32 gpio, int value)
{
	mt6391_gpio_set_pull(gpio, MT6391_GPIO_PULL_DISABLE,
			     MT6391_GPIO_PULL_DOWN);
	mt6391_gpio_set(gpio, value);
	mt6391_gpio_set_dir(gpio, MT6391_GPIO_DIRECTION_OUT);
	mt6391_gpio_set_mode(gpio, MT6391_GPIO_MODE);
}
