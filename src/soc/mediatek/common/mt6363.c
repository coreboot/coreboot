/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/mt6363.h>
#include <soc/pmif.h>
#include <timer.h>

#define MT6363_SET_DELAY_US 200

static const struct pmic_setting key_protect_setting[] = {
	{0x39E, 0x9C, 0xFF},
	{0x39F, 0x9C, 0xFF},
	{0xFAB, 0x63, 0xFF},
	{0xFAC, 0x63, 0xFF},
	{0x142A, 0x43, 0xFF},
	{0x142B, 0x55, 0xFF},
	{0x3A7, 0x9C, 0xFF},
	{0x3A8, 0x9C, 0xFF},
	{0xA33, 0x29, 0xFF},
	{0xA34, 0x47, 0xFF},
	{0xC42, 0x06, 0xFF},
	{0xC43, 0x17, 0xFF},
};

static struct pmif *pmif_arb;
u32 mt6363_read16(u32 reg)
{
	u16 rdata = 0;

	assert(pmif_arb);
	pmif_arb->read16(pmif_arb, SPMI_SLAVE_4, reg, &rdata);
	return rdata;
}

void mt6363_write8(u32 reg, u8 data)
{
	assert(pmif_arb);
	pmif_arb->write(pmif_arb, SPMI_SLAVE_4, reg, data);
}

static void mt6363_write16(u32 reg, u16 data)
{
	assert(pmif_arb);
	pmif_arb->write16(pmif_arb, SPMI_SLAVE_4, reg, data);
}

static u32 mt6363_read_field(u32 reg, u32 mask, u32 shift)
{
	assert(pmif_arb);
	return pmif_arb->read_field(pmif_arb, SPMI_SLAVE_4, reg, mask, shift);
}

void mt6363_write_field(u32 reg, u32 val, u32 mask, u32 shift)
{
	assert(pmif_arb);
	pmif_arb->write_field(pmif_arb, SPMI_SLAVE_4, reg, val, mask, shift);
}

static void pmic_set_power_hold(void)
{
	printk(BIOS_INFO, "[%s]before POWER_HOLD = 0x%x\n", __func__,
	       mt6363_read_field(PMIC_PWRHOLD, 0x1, 0));
	mt6363_write_field(PMIC_PWRHOLD, 0x1, 0x1, 0);
	printk(BIOS_INFO, "[%s]POWER_HOLD = 0x%x\n", __func__,
	       mt6363_read_field(PMIC_PWRHOLD, 0x1, 0));
}

static void pmic_wdt_set(void)
{
	/* [5]=1, RG_WDTRSTB_DEB */
	mt6363_write_field(PMIC_TOP_RST_MISC1_SET, 0x20, 0xFFFF, 0);
	/* [1]=0, RG_WDTRSTB_MODE */
	mt6363_write_field(PMIC_TOP_RST_MISC1_CLR, 0x02, 0xFFFF, 0);
	/* [0]=1, RG_WDTRSTB_EN */
	mt6363_write_field(PMIC_TOP_RST_MISC1_SET, 0x01, 0xFFFF, 0);
	/* Enable BUCK/LDO WDT VOSEL Debug */
	mt6363_write_field(PMIC_TOP_VRCTL_DBG_CON0, 0x1, 0x1, 0);
	/* Clear WDT status */
	mt6363_write_field(PMIC_TOP_RST_MISC1_SET, 0x1, 0x1, 3);
	udelay(50);
	mt6363_write_field(PMIC_TOP_RST_MISC1_CLR, 0x1, 0x1, 3);
	printk(BIOS_INFO, "[%s]WDTRSTB[0x%x]=0x%x\n", __func__, PMIC_TOP_RST_MISC1,
	mt6363_read_field(PMIC_TOP_RST_MISC1, 0xFF, 0));
}

static void pmic_protect_key_setting(bool lock)
{
	const struct pmic_setting *entry;

	for (int i = 0; i < ARRAY_SIZE(key_protect_setting); i++) {
		entry = &key_protect_setting[i];
		mt6363_write16(entry->addr, lock ? 0 : entry->val);
	}
	printk(BIOS_INFO, "%s done\n", __func__);
}

void mt6363_buck_set_voltage(u32 buck_id, u32 buck_uv)
{
	u32 vol_offset, vol_reg, vol, vol_step;

	switch (buck_id) {
	case MT6363_VBUCK2:
		vol_offset = 0;
		vol_reg = PMIC_VRCTL_VOSEL_VBUCK2;
		vol_step = 6250;
		break;
	default:
		printk(BIOS_WARNING, "[%s] Unknown buck_id %u", __func__, buck_id);
		return;
	};

	vol = (buck_uv - vol_offset) / vol_step;
	mt6363_write_field(vol_reg, vol, 0xFF, 0);
	udelay(MT6363_SET_DELAY_US);
	printk(BIOS_INFO, "%s: %d, %d\n", __func__, buck_id, vol);
}

u32 mt6363_buck_get_voltage(u32 buck_id)
{
	u32 vol_shift, vol_offset, vol_reg, vol, vol_step;

	switch (buck_id) {
	case MT6363_VBUCK2:
		vol_shift = 0;
		vol_offset = 0;
		vol_reg = PMIC_VBUCK2_DBG0;
		vol_step = 6250;
		break;
	case MT6363_VBUCK4:
		vol_shift = 0;
		vol_offset = 0;
		vol_reg = PMIC_VBUCK4_DBG0;
		vol_step = 6250;
		break;
	default:
		printk(BIOS_WARNING, "[%s] Unknown buck_id %u", __func__, buck_id);
		return 0;
	};

	vol = mt6363_read_field(vol_reg, 0xFF, vol_shift);
	printk(BIOS_INFO, "%s: %d, %d\n", __func__, buck_id, vol);
	return vol_offset + vol * vol_step;
}

void mt6363_set_vm18_voltage(u32 vm18_uv)
{
	u8 reg_vol;

	assert(pmif_arb);
	assert(vm18_uv >= 1700000);
	assert(vm18_uv < 2000000);

	reg_vol = ((vm18_uv - 1700000) / 50000) + 12;
	mt6363_write8(PMIC_VM18_ANA_CON1, reg_vol);
	mt6363_write8(PMIC_VM18_ANA_CON0, 0x0);
	udelay(MT6363_SET_DELAY_US);
	printk(BIOS_INFO, "%s: 0x%x, %d\n", __func__, reg_vol, vm18_uv);
}

u32 mt6363_get_vm18_voltage(void)
{
	u32 reg_vol, reg_cali, voltage;

	assert(pmif_arb);

	reg_cali = mt6363_read_field(PMIC_VM18_ANA_CON0, 0xF, 0);
	reg_vol = mt6363_read_field(PMIC_VM18_ANA_CON1, 0xF, 0);
	voltage = (reg_vol - 12) * 50000 + reg_cali * 5000 + 1700000;
	printk(BIOS_INFO, "%s: reg_vol 0x%x, reg_cali 0x%x, %d\n", __func__,
	       reg_vol, reg_cali, voltage);
	return voltage;
}

void mt6363_set_vtref18_voltage(u32 vtref18_uv)
{
	u8 reg_vol, reg_cali;

	assert(pmif_arb);
	assert(vtref18_uv >= 1800000);
	assert(vtref18_uv <= 3400000);

	reg_cali = ((vtref18_uv - 1800000) % 100000) / 10000;
	reg_vol = (vtref18_uv - 1800000) / 100000;
	mt6363_write8(PMIC_VTREF18_ANA_CON0, reg_cali);
	mt6363_write8(PMIC_VTREF18_ANA_CON1, reg_vol);
	udelay(MT6363_SET_DELAY_US);
	printk(BIOS_INFO, "%s: reg_vol 0x%x, reg_cali 0x%x, %d\n", __func__, reg_vol, reg_cali,
	       vtref18_uv);
}

u32 mt6363_get_vtref18_voltage(void)
{
	u32 reg_vol, reg_cali, vtref18_uv;

	assert(pmif_arb);

	reg_cali = mt6363_read_field(PMIC_VTREF18_ANA_CON0, 0xF, 0);
	reg_vol = mt6363_read_field(PMIC_VTREF18_ANA_CON1, 0xF, 0);
	vtref18_uv = reg_cali * 10000 + reg_vol * 100000 + 1800000;
	printk(BIOS_INFO, "%s: reg_vol 0x%x, reg_cali 0x%x, %d\n", __func__, reg_vol, reg_cali,
	       vtref18_uv);
	return vtref18_uv;
}

void mt6363_enable_vtref18(bool enable)
{
	mt6363_write_field(PMIC_LDO_VTREF18_CON0, enable, 0x1, 0);
}

void mt6363_enable_buck5(bool enable)
{
	mt6363_write_field(PMIC_VBUCK5_OP_EN_2, enable, 0x1, 7);
}

void mt6363_enable_vcn15(bool enable)
{
	mt6363_write_field(PMIC_LDO_VCN15_CON0, enable, 0x1, 0);
}

void mt6363_enable_vrf09(bool enable)
{
	mt6363_write_field(PMIC_LDO_VRF09_CON0, enable, 0x1, 0);
}

void mt6363_enable_vrf12(bool enable)
{
	mt6363_write_field(PMIC_LDO_VRF12_CON0, enable, 0x1, 0);
}

void mt6363_enable_vrf13(bool enable)
{
	mt6363_write_field(PMIC_LDO_VRF13_CON0, enable, 0x1, 0);
}

void mt6363_enable_vrf18(bool enable)
{
	mt6363_write_field(PMIC_LDO_VRF18_CON0, enable, 0x1, 0);
}

void mt6363_enable_vram_digrf(bool enable)
{
	mt6363_write_field(PMIC_LDO_VSRAM_DIGRF_CON0, enable, 0x1, 0);
}

void mt6363_enable_vram_mdfe(bool enable)
{
	mt6363_write_field(PMIC_LDO_VSRAM_MDFE_CON0, enable, 0x1, 0);
}

void mt6363_init_pmif_arb(void)
{
	if (!pmif_arb) {
		pmif_arb = get_pmif_controller(PMIF_SPMI, SPMI_MASTER_1);
		assert(pmif_arb);
	}

	if (pmif_arb->is_pmif_init_done(pmif_arb))
		die("ERROR - Failed to initialize pmif spi");

	printk(BIOS_INFO, "[%s][MT6363]CHIP ID =  0x%x\n", __func__,
	       mt6363_read_field(PMIC_SWCID1, 0xFF, 0));
}

void mt6363_init(void)
{
	mt6363_init_pmif_arb();
	pmic_set_power_hold();
	pmic_wdt_set();
	pmic_protect_key_setting(false);
	mt6363_init_setting();
	pmic_protect_key_setting(true);
}
