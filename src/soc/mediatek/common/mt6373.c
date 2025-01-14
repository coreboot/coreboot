/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/mt6373.h>
#include <soc/pmif.h>
#include <timer.h>

static const struct mt6373_setting key_protect_setting[] = {
	{0x39E, 0x8C, 0xFF, 0},
	{0x39F, 0x9C, 0xFF, 0},
	{0xFAB, 0x73, 0xFF, 0},
	{0xFAC, 0x63, 0xFF, 0},
	{0x142A, 0x43, 0xFF, 0},
	{0x142B, 0x55, 0xFF, 0},
	{0x3A7, 0x8C, 0xFF, 0},
	{0x3A8, 0x9C, 0xFF, 0},
	{0xA1A, 0x29, 0xFF, 0},
	{0xA1B, 0x47, 0xFF, 0},
};

static struct pmif *pmif_arb;
static void mt6373_write8(u32 reg, u8 data)
{
	assert(pmif_arb);
	pmif_arb->write(pmif_arb, SPMI_SLAVE_5, reg, data);
}

static u32 mt6373_read_field(u32 reg, u32 mask, u32 shift)
{
	assert(pmif_arb);
	return pmif_arb->read_field(pmif_arb, SPMI_SLAVE_5, reg, mask, shift);
}

void mt6373_write_field(u32 reg, u32 val, u32 mask, u32 shift)
{
	assert(pmif_arb);
	pmif_arb->write_field(pmif_arb, SPMI_SLAVE_5, reg, val, mask, shift);
}

static void pmic_protect_key_setting(bool lock)
{
	for (int i = 0; i < ARRAY_SIZE(key_protect_setting); i++)
		mt6373_write8(key_protect_setting[i].addr,
			      lock ? 0 : key_protect_setting[i].val);
	printk(BIOS_INFO, "%s done\n", __func__);
}

void mt6373_set_vmc_voltage(u32 vmc_uv)
{
	u32 reg_vol, reg_cali;

	assert(pmif_arb);

	if (vmc_uv >= 1200000 && vmc_uv <= 1300000)
		reg_vol = (vmc_uv - 1200000) / 100000;
	else if (vmc_uv >= 1500000 && vmc_uv <= 1700000)
		reg_vol = (vmc_uv - 1500000) / 200000 + 2;
	else if (vmc_uv >= 1800000 && vmc_uv <= 2000000)
		reg_vol = (vmc_uv - 1800000) / 200000 + 4;
	else if (vmc_uv >= 2100000 && vmc_uv <= 2200000)
		reg_vol = (vmc_uv - 2100000) / 100000 + 6;
	else if (vmc_uv >= 2700000 && vmc_uv <= 3100000)
		reg_vol = (vmc_uv - 2700000) / 100000 + 8;
	else if (vmc_uv >= 3300000 && vmc_uv <= 3500000)
		reg_vol = (vmc_uv - 3300000) / 100000 + 13;
	else
		die("ERROR: Unknown vmc voltage %u", vmc_uv);

	reg_cali = ((vmc_uv / 1000) % 100) / 10;

	mt6373_write8(MT6373_VMC_ANA_CON1, reg_vol);
	mt6373_write8(MT6373_VMC_ANA_CON0, reg_cali);

	printk(BIOS_INFO, "%s: 0x%x, %d\n", __func__, reg_vol, vmc_uv);
}

u32 mt6373_get_vmc_voltage(void)
{
	u32 reg_vol, reg_cali, vol;

	assert(pmif_arb);

	reg_vol = mt6373_read_field(MT6373_VMC_ANA_CON1, 0xF, 0);
	reg_cali = mt6373_read_field(MT6373_VMC_ANA_CON0, 0xF, 0);

	if (reg_vol == 0 || reg_vol == 1)
		vol = (reg_vol - 0) * 100000 + 1200000;
	else if (reg_vol == 2 || reg_vol == 3)
		vol = (reg_vol - 2) * 200000 + 1500000;
	else if (reg_vol == 4 || reg_vol == 5)
		vol = (reg_vol - 4) * 200000 + 1800000;
	else if (reg_vol == 6 || reg_vol == 7)
		vol = (reg_vol - 6) * 100000 + 2100000;
	else if (reg_vol >= 8 && reg_vol <= 12)
		vol = (reg_vol - 8) * 100000 + 2700000;
	else if (reg_vol >= 13 && reg_vol <= 15)
		vol = (reg_vol - 13) * 100000 + 3300000;
	else
		die("ERROR: Unknown vsim1 reg_vol %x", reg_vol);

	printk(BIOS_INFO, "%s: reg_vol 0x%x, reg_cali 0x%x, vol %d, %d\n",
	       __func__, reg_vol, reg_cali, vol, (vol + reg_cali * 1000));

	return (vol + reg_cali * 1000);
}

void mt6373_set_vmch_voltage(u32 vmch_uv)
{
	u32 reg_vol, reg_cali;

	assert(pmif_arb);

	if (vmch_uv >= 1200000 && vmch_uv <= 1300000)
		reg_vol = (vmch_uv - 1200000) / 100000;
	else if (vmch_uv >= 1500000 && vmch_uv <= 1700000)
		reg_vol = (vmch_uv - 1500000) / 200000 + 2;
	else if (vmch_uv >= 1800000 && vmch_uv <= 2000000)
		reg_vol = (vmch_uv - 1800000) / 200000 + 4;
	else if (vmch_uv >= 2500000 && vmch_uv <= 3100000)
		reg_vol = (vmch_uv - 2500000) / 100000 + 6;
	else if (vmch_uv >= 3300000 && vmch_uv <= 3500000)
		reg_vol = (vmch_uv - 3300000) / 100000 + 13;
	else
		die("ERROR: Unknown vmc voltage %u", vmch_uv);

	reg_cali = ((vmch_uv / 1000) % 100) / 10;

	mt6373_write8(MT6373_VMCH_ANA_CON1, reg_vol);
	mt6373_write8(MT6373_VMCH_ANA_CON0, reg_cali);

	printk(BIOS_INFO, "%s: 0x%x, %d\n", __func__, reg_vol, vmch_uv);
}

u32 mt6373_get_vmch_voltage(void)
{
	u32 reg_vol, reg_cali, vol;

	assert(pmif_arb);

	reg_vol = mt6373_read_field(MT6373_VMCH_ANA_CON1, 0xF, 0);
	reg_cali = mt6373_read_field(MT6373_VMCH_ANA_CON0, 0xF, 0);

	if (reg_vol == 0 || reg_vol == 1)
		vol = (reg_vol - 0) * 100000 + 1200000;
	else if (reg_vol == 2 || reg_vol == 3)
		vol = (reg_vol - 2) * 200000 + 1500000;
	else if (reg_vol == 4 || reg_vol == 5)
		vol = (reg_vol - 4) * 200000 + 1800000;
	else if (reg_vol >= 6 && reg_vol <= 12)
		vol = (reg_vol - 6) * 100000 + 2500000;
	else if (reg_vol >= 13 && reg_vol <= 15)
		vol = (reg_vol - 13) * 100000 + 3300000;
	else
		die("ERROR: Unknown vsim1 reg_vol %x", reg_vol);

	printk(BIOS_INFO, "%s: reg_vol 0x%x, reg_cali 0x%x, vol %d, %d\n",
	       __func__, reg_vol, reg_cali, vol, (vol + reg_cali * 1000));

	return (vol + reg_cali * 1000);
}

void mt6373_set_vcn33_3_voltage(u32 vcn33_3_uv)
{
	udelay(100);
	printk(BIOS_INFO, "%s start\n", __func__);
	u32 reg_vol, reg_cali;

	assert(pmif_arb);

	if (vcn33_3_uv >= 1200000 && vcn33_3_uv <= 1300000)
		reg_vol = (vcn33_3_uv - 1200000) / 100000;
	else if (vcn33_3_uv >= 1500000 && vcn33_3_uv <= 1700000)
		reg_vol = (vcn33_3_uv - 1500000) / 200000 + 2;
	else if (vcn33_3_uv >= 1800000 && vcn33_3_uv <= 2000000)
		reg_vol = (vcn33_3_uv - 1800000) / 200000 + 4;
	else if (vcn33_3_uv >= 2500000 && vcn33_3_uv <= 3100000)
		reg_vol = (vcn33_3_uv - 2500000) / 100000 + 6;
	else if (vcn33_3_uv >= 3300000 && vcn33_3_uv <= 3500000)
		reg_vol = (vcn33_3_uv - 3300000) / 100000 + 13;
	else
		die("ERROR: Unknown vcn33_3_uv voltage %u", vcn33_3_uv);

	reg_cali = ((vcn33_3_uv / 1000) % 100) / 10;

	mt6373_write8(MT6373_VCN33_3_ANA_CON1, reg_vol);
	udelay(100);
	mt6373_write8(MT6373_VCN33_3_ANA_CON0, reg_cali);
	udelay(100);
	printk(BIOS_INFO, "%s: 0x%x, %d\n", __func__, reg_vol, vcn33_3_uv);
}

void mt6373_enable_vcn33_3(bool enable)
{
	mt6373_write_field(MT6373_LDO_VCN33_3_CON0, enable, 0x1, 0);
}

void mt6373_enable_vmc(bool enable)
{
	mt6373_write_field(MT6373_LDO_VMC_CON0, enable, 0x1, 0);
}

void mt6373_enable_vmch(bool enable)
{
	mt6373_write_field(MT6373_LDO_VMCH_CON0, enable, 0x1, 0);
}

void mt6373_enable_vant18(bool enable)
{
	mt6373_write_field(MT6373_LDO_VANT18_CON0, enable, 0x1, 0);
}

void mt6373_enable_vsim1(bool enable)
{
	mt6373_write_field(MT6373_LDO_VSIM1_CON0, enable, 0x1, 0);
}

void mt6373_enable_vsim2(bool enable)
{
	mt6373_write_field(MT6373_LDO_VSIM2_CON0, enable, 0x1, 0);
}

static void mt6373_pmic_wdt_set(void)
{
	/* [5]=1, RG_WDTRSTB_DEB */
	mt6373_write_field(0x13a, 0x20, 0xFF, 0);
	/* [1]=0, RG_WDTRSTB_MODE */
	mt6373_write_field(0x13b, 0x02, 0xFF, 0);
	/* [0]=1, RG_WDTRSTB_EN */
	mt6373_write_field(0x13a, 0x01, 0xFF, 0);
	/* Enable BUCK/LDO WDT VOSEL Debug */
	mt6373_write_field(0x231, 0x1, 0x1, 0);
	/* Clear WDT status */
	mt6373_write_field(0x13a, 0x1, 0x1, 3);
	udelay(50);
	mt6373_write_field(0x13b, 0x1, 0x1, 3);
	printk(BIOS_INFO, "[%s]WDTRSTB[0x139]=0x%x\n", __func__,
	       mt6373_read_field(0x139, 0xFF, 0));
}

void mt6373_init_pmif_arb(void)
{
	if (pmif_arb)
		return;

	pmif_arb = get_pmif_controller(PMIF_SPMI, SPMI_MASTER_1);
	assert(pmif_arb);

	if (pmif_arb->is_pmif_init_done(pmif_arb))
		die("%s: initialization failed", __func__);

	printk(BIOS_INFO, "[%s][MT6373]CHIP ID = 0x%x\n",
	       __func__, mt6373_read_field(MT6373_SWCID1, 0xFF, 0));
}

void mt6373_init(void)
{
	printk(BIOS_INFO, "%s start\n", __func__);
	mt6373_init_pmif_arb();
	mt6373_pmic_wdt_set();
	pmic_protect_key_setting(false);
	mt6373_init_setting();
	pmic_protect_key_setting(true);
}
