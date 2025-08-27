/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/spm.h>
#include <timer.h>

#define MTCMOS_TIMEOUT_US	500
#define MTCMOS_ETIMEDOUT	25

enum {
	SRAM_ISOINT_B	= 1U << 6,
	SRAM_CKISO	= 1U << 5,
	PWR_CLK_DIS	= 1U << 4,
	PWR_ON_2ND	= 1U << 3,
	PWR_ON		= 1U << 2,
	PWR_ISO		= 1U << 1,
	PWR_RST_B	= 1U << 0
};

__weak void mtcmos_set_scpd_ext_buck_iso(const struct power_domain_data *pd)
{
	/* do nothing */
}

static int mtcmos_wait_for_state(u32 *reg, u32 mask, bool is_set)
{
	u32 expect = is_set ? mask : 0;

	if (!wait_us(MTCMOS_TIMEOUT_US, ((read32(reg) & mask) == expect))) {
		printk(BIOS_ERR,
		       "%s (0x%p, %#x, %d) timeout after %d us, reg_val=%#x\n",
		       __func__, reg, mask, is_set, MTCMOS_TIMEOUT_US, read32(reg));
		return -MTCMOS_ETIMEDOUT;
	}

	return 0;
}

static void release_bus_protection(const struct power_domain_data *pd)
{
	int i;

	for (i = 0; i < pd->bp_steps; i++)
		write32(pd->bp_table[i].clr_addr, pd->bp_table[i].mask);
}

static void set_bus_protection(const struct power_domain_data *pd)
{
	int i;

	for (i = pd->bp_steps - 1; i >= 0; i--) {
		const struct bus_protect *bp = &pd->bp_table[i];
		assert(bp->set_addr && bp->rdy_addr);
		write32(bp->set_addr, bp->mask);
		mtcmos_wait_for_state(bp->rdy_addr, bp->mask, true);
	}
}


void mtcmos_power_on(const struct power_domain_data *pd)
{
	u32 *pwr_status;
	u32 *pwr_status_2nd;

	write32(&mtk_spm->poweron_config_set,
		(SPM_PROJECT_CODE << 16) | (1U << 0));

	if (pd->caps & SCPD_EXT_BUCK_ISO)
		mtcmos_set_scpd_ext_buck_iso(pd);

	setbits32(pd->pwr_con, PWR_ON);
	setbits32(pd->pwr_con, PWR_ON_2ND);

	if ((pd->pwr_status != NULL) && (pd->pwr_status_2nd != NULL)) {
		pwr_status = pd->pwr_status;
		pwr_status_2nd = pd->pwr_status_2nd;
	} else {
		pwr_status = &mtk_spm->pwr_status;
		pwr_status_2nd = &mtk_spm->pwr_status_2nd;
	}

	while (!(read32(pwr_status) & pd->pwr_sta_mask) ||
	       !(read32(pwr_status_2nd) & pd->pwr_sta_mask))
		continue;

	clrbits32(pd->pwr_con, PWR_CLK_DIS);
	clrbits32(pd->pwr_con, PWR_ISO);
	setbits32(pd->pwr_con, PWR_RST_B);
	clrbits32(pd->pwr_con, pd->sram_pdn_mask);

	while (read32(pd->pwr_con) & pd->sram_ack_mask)
		continue;

	if (pd->caps & SCPD_SRAM_ISO) {
		setbits32(pd->pwr_con, SRAM_ISOINT_B);
		udelay(1);
		clrbits32(pd->pwr_con, SRAM_CKISO);
	}

	release_bus_protection(pd);
}

void mtcmos_power_off(const struct power_domain_data *pd)
{
	u32 *pwr_status;
	u32 *pwr_status_2nd;

	write32(&mtk_spm->poweron_config_set,
		(SPM_PROJECT_CODE << 16) | (1U << 0));

	set_bus_protection(pd);

	if (pd->sram_pdn_mask) {
		if (pd->caps & SCPD_SRAM_ISO) {
			setbits32(pd->pwr_con, SRAM_CKISO);
			clrbits32(pd->pwr_con, SRAM_ISOINT_B);
			udelay(1);
		}
		setbits32(pd->pwr_con, pd->sram_pdn_mask);
		mtcmos_wait_for_state(pd->pwr_con, pd->sram_ack_mask, true);
	}

	setbits32(pd->pwr_con, PWR_ISO);
	setbits32(pd->pwr_con, PWR_CLK_DIS);
	clrbits32(pd->pwr_con, PWR_RST_B);

	if (pd->pwr_status && pd->pwr_status_2nd) {
		pwr_status = pd->pwr_status;
		pwr_status_2nd = pd->pwr_status_2nd;
	} else {
		pwr_status = &mtk_spm->pwr_status;
		pwr_status_2nd = &mtk_spm->pwr_status_2nd;
	}

	clrbits32(pd->pwr_con, PWR_ON);
	mtcmos_wait_for_state(pwr_status, pd->pwr_sta_mask, false);
	clrbits32(pd->pwr_con, PWR_ON_2ND);
	mtcmos_wait_for_state(pwr_status_2nd, pd->pwr_sta_mask, false);
}

void mtcmos_display_power_on(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(disp); i++)
		mtcmos_power_on(&disp[i]);
}

void mtcmos_audio_power_on(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(audio); i++)
		mtcmos_power_on(&audio[i]);
}
