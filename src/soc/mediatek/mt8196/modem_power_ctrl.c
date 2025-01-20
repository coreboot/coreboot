/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/modem_power_ctrl.h>
#include <soc/mt6363.h>
#include <soc/mt6373.h>
#include <soc/pmif.h>
#include <soc/spm.h>
#include <soc/spmi.h>

#define SPMI_SLAVE_4_750MV	0x78
#define MAX_RETRY_COUNT		4000
#define VSRAM_VOSEL		0x24d
#define VMODEM_VOSEL		0x24e
#define MD1_PWR_STA_MASK	BIT(0)
#define MD1_PROT_STEP1_0_MASK	BIT(0)
#define PWR_ON			BIT(2)
#define MD1_PROT_STEP0_0_MASK	BIT(29)

struct mtk_apifrbus_regs {
	u32 sleep_protect_en_0;
	u32 sleep_protect_en_0_set;
	u32 sleep_protect_en_0_clr;
	u32 sleep_protect_rdy_sta_0;
	u32 reserved0[4];
	u32 sleep_protect_en_1;
	u32 sleep_protect_en_1_set;
	u32 sleep_protect_en_1_clr;
	u32 sleep_protect_rdy_sta_1;
};

static struct mtk_apifrbus_regs *const mtk_apifrbus_reg =
		(void *)IFRBUS_AO_REG_BUS_BASE;

static void md_pmic_lowpower_set(void)
{
	mt6363_init_pmif_arb();
	/* Vmodem_vosel = 0x24e, write Vmodem 0.75V */
	mt6363_write8(VMODEM_VOSEL, SPMI_SLAVE_4_750MV);
	printk(BIOS_INFO, "Vmodem value: 0x%x\n", mt6363_read8(VMODEM_VOSEL));
}

static void disable_pmic_ldo(void)
{
	mt6363_enable_buck5(false);
	mt6363_enable_vcn15(false);
	mt6363_enable_vrf09(false);
	mt6363_enable_vrf12(false);
	mt6363_enable_vrf13(false);
	mt6363_enable_vrf18(false);
	mt6363_enable_vram_digrf(false);
	mt6363_enable_vram_mdfe(false);
	mt6373_enable_vant18(false);
	mt6373_enable_vsim1(false);
	mt6373_enable_vsim2(false);
}

static void spm_mtcmos_ctrl_md1_disable(void)
{
	write32(&mtk_apifrbus_reg->sleep_protect_en_0_set, MD1_PROT_STEP0_0_MASK);
	if (!retry(MAX_RETRY_COUNT,
		   (read32(&mtk_apifrbus_reg->sleep_protect_rdy_sta_1) &
		    MD1_PROT_STEP0_0_MASK) == MD1_PROT_STEP0_0_MASK)) {
		printk(BIOS_ERR, "Check MD1_PROT_STEP0 failed\n");
	}

	write32(&mtk_apifrbus_reg->sleep_protect_en_1_set, MD1_PROT_STEP1_0_MASK);
	if (!retry(MAX_RETRY_COUNT,
		   (read32(&mtk_apifrbus_reg->sleep_protect_rdy_sta_1) &
		    MD1_PROT_STEP1_0_MASK) == MD1_PROT_STEP1_0_MASK)) {
		printk(BIOS_ERR, "Check MD1_PROT_STEP1 failed\n");
	}

	clrbits32(&mtk_spm->md1_pwr_con, PWR_ON);
	if (!retry(MAX_RETRY_COUNT,
		   (read32(&mtk_spm->pwr_status) & MD1_PWR_STA_MASK) == 0)) {
		printk(BIOS_ERR, "Check MD1_PWR_STA failed\n");
	}

	setbits32(&mtk_spm->md_buck_iso_con, 0x3);
}

static void md_cd_topclkgen_off(void)
{
	setbits32p(CKSYS_BASE, BIT(8) | BIT(9));
}

void modem_power_down(void)
{
	/* step 1. drop Vmodem voltage same as Vsram voltage */
	md_pmic_lowpower_set();

	/* step 2. disable pmic ldo */
	disable_pmic_ldo();

	/* step 3. power off md mtcmos */
	spm_mtcmos_ctrl_md1_disable();

	/* step 4. disable MD input 26M&32K */
	md_cd_topclkgen_off();
}
