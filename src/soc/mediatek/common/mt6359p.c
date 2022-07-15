/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/mt6359p.h>
#include <soc/pmif.h>
#include <timer.h>

static const struct pmic_setting key_protect_setting[] = {
	{0x3A8, 0x9CA6, 0xFFFF, 0},
	{0x44A, 0xBADE, 0xFFFF, 0},
	{0xA3A, 0x4729, 0xFFFF, 0},
	{0xC58, 0x1605, 0xFFFF, 0},
	{0xC5A, 0x1706, 0xFFFF, 0},
	{0xC5C, 0x1807, 0xFFFF, 0},
	{0xFB4, 0x6359, 0xFFFF, 0},
	{0x1432, 0x5543, 0xFFFF, 0},
};

static const struct pmic_efuse efuse_setting[] = {
	{79, 0xa0e, 0x1, 0xf},
	{886, 0x198c, 0xf, 0x8},
	{890, 0x198e, 0xf, 0x0},
	{902, 0x1998, 0xf, 0x8},
	{906, 0x1998, 0xf, 0xc},
	{918, 0x19a2, 0xf, 0x8},
	{922, 0x19a2, 0xf, 0xc},
	{1014, 0x19ae, 0xf, 0x7},
	{1018, 0x19ae, 0xf, 0xb},
	{1158, 0x1a0a, 0xf, 0x7},
	{1162, 0x1a0a, 0xf, 0xb},
	{1206, 0x1a16, 0xf, 0x7},
	{1210, 0x1a16, 0xf, 0xb},
	{1254, 0x1a22, 0xf, 0x7},
	{1258, 0x1a22, 0xf, 0xb},
	{1304, 0x1a2c, 0x7, 0x4},
	{1307, 0x1a32, 0x7, 0x8},
	{1336, 0x1a34, 0x7, 0x4},
	{1339, 0x1a3a, 0x7, 0x8},
	{1683, 0x79c, 0xf, 0x4},
	{1688, 0xc8a, 0x1, 0x3},
	{1689, 0xc88, 0x1, 0x3},
	{1690, 0xc88, 0x7, 0x0},
};

static struct pmif *pmif_arb = NULL;
static void mt6359p_write(u32 reg, u32 data)
{
	pmif_arb->write(pmif_arb, 0, reg, data);
}

static u32 mt6359p_read_field(u32 reg, u32 mask, u32 shift)
{
	return pmif_arb->read_field(pmif_arb, 0, reg, mask, shift);
}

void mt6359p_write_field(u32 reg, u32 val, u32 mask, u32 shift)
{
	pmif_arb->write_field(pmif_arb, 0, reg, val, mask, shift);
}

static void pmic_set_power_hold(void)
{
	mt6359p_write_field(PMIC_PWRHOLD, 0x1, 0x1, 0);
}

static void pmic_wdt_set(void)
{
	/* [5]=1, RG_WDTRSTB_DEB */
	mt6359p_write_field(PMIC_TOP_RST_MISC_SET, 0x20, 0xFFFF, 0);
	/* [1]=0, RG_WDTRSTB_MODE */
	mt6359p_write_field(PMIC_TOP_RST_MISC_CLR, 0x02, 0xFFFF, 0);
	/* [0]=1, RG_WDTRSTB_EN */
	mt6359p_write_field(PMIC_TOP_RST_MISC_SET, 0x01, 0xFFFF, 0);
}

static void pmic_protect_key_setting(bool lock)
{
	for (int i = 0; i < ARRAY_SIZE(key_protect_setting); i++)
		mt6359p_write(key_protect_setting[i].addr,
			      lock ? 0 : key_protect_setting[i].val);
}

static int check_idle(u32 timeout, u32 addr, u32 mask)
{
	if (!wait_us(timeout, !mt6359p_read_field(addr, mask, 0)))
		return -1;

	return 0;
}

static u32 pmic_read_efuse(u32 efuse_bit, u32 mask)
{
	u32 efuse_data;
	int index, shift;

	index = efuse_bit / 16;
	shift = efuse_bit % 16;
	mt6359p_write_field(PMIC_TOP_CKHWEN_CON0, 0, 0x1, 2);
	mt6359p_write_field(PMIC_TOP_CKPDN_CON0, 0, 0x1, 4);
	mt6359p_write_field(PMIC_OTP_CON11, 1, 0x1, 0);
	mt6359p_write_field(PMIC_OTP_CON0, index * 2, 0xFF, 0);
	if (mt6359p_read_field(PMIC_OTP_CON8, 1, 0))
		mt6359p_write_field(PMIC_OTP_CON8, 0, 1, 0);
	else
		mt6359p_write_field(PMIC_OTP_CON8, 1, 1, 0);

	udelay(300);
	if (check_idle(EFUSE_WAIT_US, PMIC_OTP_CON13, EFUSE_BUSY))
		die("[%s] timeout after %d usecs\n", __func__, EFUSE_WAIT_US);

	udelay(100);

	efuse_data = mt6359p_read_field(PMIC_OTP_CON12, 0xFFFF, 0);
	efuse_data = (efuse_data >> shift) & mask;

	mt6359p_write_field(PMIC_TOP_CKHWEN_CON0, 1, 0x1, 2);
	mt6359p_write_field(PMIC_TOP_CKPDN_CON0, 1, 0x1, 4);

	return efuse_data;
}

static void pmic_efuse_setting(void)
{
	u32 efuse_data;
	struct stopwatch sw;

	stopwatch_init(&sw);

	for (int i = 0; i < ARRAY_SIZE(efuse_setting); i++) {
		efuse_data = pmic_read_efuse(efuse_setting[i].efuse_bit, efuse_setting[i].mask);
		mt6359p_write_field(efuse_setting[i].addr, efuse_data,
			efuse_setting[i].mask, efuse_setting[i].shift);
	}

	efuse_data = pmic_read_efuse(EFUSE_RG_VPA_OC_FT, 0x1);
	if (efuse_data) {
		/* restore VPA_DLC initial setting */
		mt6359p_write(PMIC_BUCK_VPA_DLC_CON0, 0x2810);
		mt6359p_write(PMIC_BUCK_VPA_DLC_CON1, 0x800);
	}

	printk(BIOS_DEBUG, "%s: Set efuses in %ld msecs\n",
	       __func__, stopwatch_duration_msecs(&sw));
}

static void pmic_wk_vs2_voter_setting(void)
{
	/*
	 *  1. Set VS2_VOTER_VOSEL = 1.35V
	 *  2. Clear VS2_VOTER
	 *  3. Set VS2_VOSEL = 1.4V
	 */
	mt6359p_write_field(PMIC_VS2_VOTER_CFG, 0x2C, 0x7F, 0);
	mt6359p_write_field(PMIC_VS2_VOTER, 0, 0xFFF, 0);
	mt6359p_write_field(PMIC_VS2_ELR0, 0x30, 0x7F, 0);
}

void mt6359p_buck_set_voltage(u32 buck_id, u32 buck_uv)
{
	u32 vol_offset, vol_reg, vol, vol_step;

	if (!pmif_arb)
		die("ERROR: pmif_arb not initialized");

	switch (buck_id) {
	case MT6359P_GPU11:
		vol_offset = 400000;
		vol_reg = PMIC_VGPU11_ELR0;
		vol_step = 6250;
		break;
	case MT6359P_SRAM_PROC1:
		vol_offset = 500000;
		vol_reg = PMIC_VSRAM_PROC1_ELR;
		vol_step = 6250;
		break;
	case MT6359P_SRAM_PROC2:
		vol_offset = 500000;
		vol_reg = PMIC_VSRAM_PROC2_ELR;
		vol_step = 6250;
		break;
	case MT6359P_CORE:
		vol_offset = 506250;
		vol_reg = PMIC_VCORE_ELR0;
		vol_step = 6250;
		break;
	case MT6359P_PA:
		vol_offset = 500000;
		vol_reg = PMIC_VPA_CON1;
		vol_step = 50000;
		break;
	default:
		die("ERROR: Unknown buck_id %u", buck_id);
		return;
	};

	vol = (buck_uv - vol_offset) / vol_step;
	mt6359p_write_field(vol_reg, vol, 0x7F, 0);
}

u32 mt6359p_buck_get_voltage(u32 buck_id)
{
	u32 vol_shift, vol_offset, vol_reg, vol, vol_step;

	if (!pmif_arb)
		die("ERROR: pmif_arb not initialized");

	switch (buck_id) {
	case MT6359P_GPU11:
		vol_shift = 0;
		vol_offset = 400000;
		vol_reg = PMIC_VGPU11_DBG0;
		vol_step = 6250;
		break;
	case MT6359P_SRAM_PROC1:
		vol_shift = 8;
		vol_offset = 500000;
		vol_reg = PMIC_VSRAM_PROC1_VOSEL1;
		vol_step = 6250;
		break;
	case MT6359P_SRAM_PROC2:
		vol_shift = 8;
		vol_offset = 500000;
		vol_reg = PMIC_VSRAM_PROC2_VOSEL1;
		vol_step = 6250;
		break;
	case MT6359P_CORE:
		vol_shift = 0;
		vol_offset = 506250;
		vol_reg = PMIC_VCORE_DBG0;
		vol_step = 6250;
		break;
	case MT6359P_PA:
		vol_shift = 0;
		vol_offset = 500000;
		vol_reg = PMIC_VPA_DBG0;
		vol_step = 50000;
		break;
	default:
		die("ERROR: Unknown buck_id %u", buck_id);
		return 0;
	};

	vol = mt6359p_read_field(vol_reg, 0x7F, vol_shift);
	return vol_offset + vol * vol_step;
}

void mt6359p_set_vm18_voltage(u32 vm18_uv)
{
	u32 reg_vol, reg_cali;

	if (!pmif_arb)
		die("ERROR: pmif_arb not initialized");

	assert(vm18_uv >= 1700000);
	assert(vm18_uv < 2000000);

	reg_vol = (vm18_uv / 1000 - VM18_VOL_OFFSET) / 100;
	reg_cali = ((vm18_uv / 1000) % 100) / 10;
	mt6359p_write(PMIC_VM18_ANA_CON0, (reg_vol << VM18_VOL_REG_SHIFT) | reg_cali);
}

u32 mt6359p_get_vm18_voltage(void)
{
	u32 reg_vol, reg_cali;

	if (!pmif_arb)
		die("ERROR: pmif_arb not initialized");

	reg_vol = 100 * mt6359p_read_field(PMIC_VM18_ANA_CON0, 0xF, VM18_VOL_REG_SHIFT);
	reg_cali = 10 * mt6359p_read_field(PMIC_VM18_ANA_CON0, 0xF, 0);
	return 1000 * (VM18_VOL_OFFSET + reg_vol + reg_cali);
}

void mt6359p_set_vsim1_voltage(u32 vsim1_uv)
{
	u32 reg_vol, reg_cali;

	if (!pmif_arb)
		die("ERROR: pmif_arb not initialized");

	if ((vsim1_uv >= 1700000) && (vsim1_uv <= 1900000))
		reg_vol = (vsim1_uv / 1000 - VSIM1_VOL_OFFSET_1) / 100;
	else if ((vsim1_uv >= 2700000) && (vsim1_uv <= 2800000))
		reg_vol = (vsim1_uv / 1000 - VSIM1_VOL_OFFSET_2) / 100;
	else if ((vsim1_uv >= 3000000) && (vsim1_uv <= 3200000))
		reg_vol = (vsim1_uv / 1000 - VSIM1_VOL_OFFSET_2) / 100;
	else
		die("ERROR: Unknown vsim1 voltage %u", vsim1_uv);

	reg_cali = ((vsim1_uv / 1000) % 100) / 10;
	mt6359p_write(PMIC_VSIM1_ANA_CON0, (reg_vol << VSIM1_VOL_REG_SHIFT) | reg_cali);
}

u32 mt6359p_get_vsim1_voltage(void)
{
	u32 reg_vol, reg_cali, reg_offset;

	if (!pmif_arb)
		die("ERROR: pmif_arb not initialized");

	reg_vol = 100 * mt6359p_read_field(PMIC_VSIM1_ANA_CON0, 0xF,
					   VSIM1_VOL_REG_SHIFT);
	reg_cali = 10 * mt6359p_read_field(PMIC_VSIM1_ANA_CON0, 0xF, 0);

	if ((reg_vol == 300) || (reg_vol == 400))
		reg_offset = VSIM1_VOL_OFFSET_1;
	else if ((reg_vol == 800) || (reg_vol == 1100) || (reg_vol == 1200))
		reg_offset = VSIM1_VOL_OFFSET_2;
	else
		die("ERROR: Unknown vsim1 reg_vol %x", reg_vol);

	return 1000 * (reg_offset + reg_vol + reg_cali);
}

void mt6359p_enable_vpa(bool enable)
{
	mt6359p_write_field(PMIC_VPA_CON0, !!enable, 0x1, 0);
}

void mt6359p_enable_vsim1(bool enable)
{
	mt6359p_write_field(PMIC_VSIM1_CON0, !!enable, 0x1, 0);
}

static void init_pmif_arb(void)
{
	if (!pmif_arb) {
		pmif_arb = get_pmif_controller(PMIF_SPI, 0);
		if (!pmif_arb)
			die("ERROR: No spi device");
	}

	if (pmif_arb->is_pmif_init_done(pmif_arb))
		die("ERROR - Failed to initialize pmif spi");
}

void mt6359p_init(void)
{
	init_pmif_arb();
	pmic_set_power_hold();
	pmic_wdt_set();
	pmic_protect_key_setting(false);
	pmic_init_setting();
	pmic_lp_setting();
	pmic_efuse_setting();
	pmic_protect_key_setting(true);
	pmic_wk_vs2_voter_setting();
}
