/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/mt6316.h>
#include <soc/pmif.h>
#include <soc/pmif_spmi.h>

static struct pmif *pmif_arb;
static const u32 mt6316_slave_id[] = {
	SPMI_SLAVE_6, SPMI_SLAVE_7, SPMI_SLAVE_8, SPMI_SLAVE_15
};

static u8 mt6316_read8(enum spmi_slave slvid, u32 reg)
{
	u32 rdata = 0;

	assert(pmif_arb);
	pmif_arb->read(pmif_arb, slvid, reg, &rdata);

	return (u8)rdata;
}

static void mt6316_write8(enum spmi_slave slvid, u32 reg, u8 data)
{
	assert(pmif_arb);
	pmif_arb->write(pmif_arb, slvid, reg, data);
}

void mt6316_write_field(enum spmi_slave slvid, u32 reg, u32 val, u32 mask, u32 shift)
{
	assert(pmif_arb);
	return pmif_arb->write_field(pmif_arb, slvid, reg, val, mask, shift);
}

static u32 mt6316_read_field(enum spmi_slave slvid, u32 reg, u32 mask, u32 shift)
{
	assert(pmif_arb);
	return pmif_arb->read_field(pmif_arb, slvid, reg, mask, shift);
}

static void mt6316_wdt_enable(enum spmi_slave slvid)
{
	u8 reg_val;

	/* read the current wdt setting */
	reg_val = mt6316_read8(slvid, MT6316_PMIC_TOP_RST_MISC);
	printk(BIOS_INFO, "[%s]S%u TOP_RST_MISC=%#x\n", __func__, slvid, reg_val);
	mt6316_write8(slvid, MT6316_PMIC_PLT_DIG_WPK, 0xE9);
	mt6316_write8(slvid, MT6316_PMIC_PLT_DIG_WPK_H, 0xE6);
	mt6316_write8(slvid, MT6316_PMIC_TOP_RST_MISC_SET, 0x2);
	mt6316_write8(slvid, MT6316_PMIC_TOP_RST_MISC_SET, 0x1);
	mt6316_write8(slvid, MT6316_PMIC_TOP_RST_MISC_SET, 0x8);
	udelay(50);
	mt6316_write8(slvid, MT6316_PMIC_TOP_RST_MISC_CLR, 0x8);
	mt6316_write8(slvid, MT6316_PMIC_PLT_DIG_WPK, 0);
	mt6316_write8(slvid, MT6316_PMIC_PLT_DIG_WPK_H, 0);
	reg_val = mt6316_read8(slvid, MT6316_PMIC_TOP_RST_MISC);
	printk(BIOS_INFO, "[%s]S%u TOP_RST_MISC=%#x\n", __func__, slvid, reg_val);
}

void mt6316_buck_set_voltage(enum spmi_slave slvid, u32 buck_id, u32 buck_uv)
{
	u32 vol_reg, vol_reg_l, vol_val;

	assert(pmif_arb);

	switch (buck_id) {
	case MT6316_BUCK_1:
		vol_reg = MT6316_BUCK_TOP_ELR0;
		vol_reg_l = MT6316_BUCK_TOP_ELR1;
		break;
	case MT6316_BUCK_3:
		vol_reg = MT6316_BUCK_TOP_ELR4;
		vol_reg_l = MT6316_BUCK_TOP_ELR5;
		break;
	default:
		printk(BIOS_ERR, "%s: Unknown buck_id %u\n", __func__, buck_id);
		return;
	};

	vol_val = buck_uv / 2500;
	mt6316_write8(slvid, vol_reg_l, vol_val & 0x1);
	mt6316_write8(slvid, vol_reg, vol_val >> 1);
	udelay(200);
	printk(BIOS_INFO, "%s: %u, %#x, %u\n", __func__, buck_id, vol_val, buck_uv);
}

u32 mt6316_buck_get_voltage(enum spmi_slave slvid, u32 buck_id)
{
	u8 vol;
	u32 vol_reg, vol_reg_l, vol_l, vol_val;

	assert(pmif_arb);

	switch (buck_id) {
	case MT6316_BUCK_1:
		vol_reg = MT6316_BUCK_VBUCK1_DBG0;
		vol_reg_l = MT6316_BUCK_VBUCK1_DBG1;
		break;
	case MT6316_BUCK_3:
		vol_reg = MT6316_BUCK_VBUCK3_DBG0;
		vol_reg_l = MT6316_BUCK_VBUCK3_DBG1;
		break;
	default:
		printk(BIOS_ERR, "%s: Unknown buck_id %u\n", __func__, buck_id);
		return 0;
	};

	vol = mt6316_read8(slvid, vol_reg);
	vol_l = mt6316_read_field(slvid, vol_reg_l, 0x1, 0);
	vol_val = vol * 5000 + vol_l * 2500;
	printk(BIOS_INFO, "%s: %u, %#x, %#x, %u\n", __func__, buck_id, vol, vol_l,
	       vol_val);
	return vol_val;
}

void mt6316_buck_enable(enum spmi_slave slvid, u32 buck_id, bool enable)
{
	u32 mod_shift;

	assert(pmif_arb);

	switch (buck_id) {
	case MT6316_BUCK_1:
		mod_shift = 0;
		break;
	case MT6316_BUCK_3:
		mod_shift = 2;
		break;
	default:
		printk(BIOS_ERR, "%s: Unknown buck_id %u\n", __func__, buck_id);
		return;
	};

	mt6316_write_field(slvid, MT6316_BUCK_TOP_4PHASE_TOP_ANA_CON0,
			   enable, 0x1, mod_shift);
	udelay(100);
	printk(BIOS_INFO, "%s: %u, %u\n", __func__, buck_id, enable);
}

bool mt6316_buck_is_enabled(enum spmi_slave slvid, u32 buck_id)
{
	u32 mod_shift, mod;

	assert(pmif_arb);

	switch (buck_id) {
	case MT6316_BUCK_1:
		mod_shift = 0;
		break;
	case MT6316_BUCK_3:
		mod_shift = 2;
		break;
	default:
		printk(BIOS_ERR, "%s: Unknown buck_id %u\n", __func__, buck_id);
		return false;
	};

	mod = mt6316_read_field(slvid, MT6316_BUCK_TOP_4PHASE_TOP_ANA_CON0, 0x1, mod_shift);
	printk(BIOS_INFO, "%s: %u, %u\n", __func__, buck_id, mod);
	return !!mod;
}

static void mt6316_set_all_test_con9(void)
{
	for (int i = 0; i < ARRAY_SIZE(mt6316_slave_id); i++)
		mt6316_write8(mt6316_slave_id[i], MT6316_PMIC_TEST_CON9, 0x20);
}

static void init_pmif_arb(void)
{
	if (!pmif_arb) {
		pmif_arb = get_pmif_controller(PMIF_SPMI, SPMI_MASTER_0);
		assert(pmif_arb);
	}

	if (pmif_arb->is_pmif_init_done(pmif_arb))
		die("ERROR - Failed to initialize pmif spmi");

	for (int i = 0; i < ARRAY_SIZE(mt6316_slave_id); i++)
		printk(BIOS_INFO, "%s: MT6316_%u: CHIP ID = %#x\n", __func__,
		       mt6316_slave_id[i],
		       mt6316_read_field(mt6316_slave_id[i],
					 MT6316_PMIC_SWCID_H_ADDR, 0xFF, 0x0));
}

void mt6316_init(void)
{
	init_pmif_arb();

	for (int i = 0; i < ARRAY_SIZE(mt6316_slave_id); i++)
		mt6316_wdt_enable(mt6316_slave_id[i]);

	mt6316_init_setting();
	mt6316_set_all_test_con9();
}
