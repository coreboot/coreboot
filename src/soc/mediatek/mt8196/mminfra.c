/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/mminfra.h>
#include <soc/spm_mtcmos.h>
#include <stddef.h>
#include <string.h>
#include <timer.h>
#include <types.h>

#define TIMEOUT_US 20

static void wait_for_write_done(u32 write_reg, u32 status_reg, u32 val)
{
	write32p(write_reg, val);

	if (!wait_us(TIMEOUT_US, read32p(status_reg) == val))
		die("Wait write done timeout\n");
}

static void mm_gce_lock_prot_en(void)
{
	setbits32p(MMINFRA_GCE_PROT_EN,
		   GCE_D_SLEEPPORT_RX_EN | GCE_D_HAND_SLEEPPORT_RX_EN |
		   GCE_D_HAND_SLEEPPORT_TX_EN | GCE_M_SLEEPPORT_RX_EN |
		   GCE_M_HAND_SLEEPPORT_RX_EN | GCE_M_HAND_SLEEPPORT_TX_EN);
	write32p(MMINFRA_GCE_DDREN_SEL, 0);
}

static void mm_gce_release_prot_en(void)
{
	write32p(MMINFRA_GCE_PROT_EN, 0);
	write32p(MMINFRA_GCE_DDREN_SEL, 0x1);
}

static void mm_infra0_lock_prot_en(void)
{
	wait_for_write_done(MMINFRA_MM0_GALS_PROT_TX_EN,
			    MMINFRA_MM0_GALS_PROT_TX_RDY, 0xff);
	wait_for_write_done(MMINFRA_MM0_GALS_PROT_RX_EN,
			    MMINFRA_MM0_GALS_PROT_RX_RDY, 0xfffff);
}

static void mm_infra0_release_prot_en(void)
{
	write32p(MMINFRA_MM0_GALS_PROT_RX_EN, 0);
	write32p(MMINFRA_MM0_GALS_PROT_TX_EN, 0);
}

static void mm_infra1_lock_prot_en(void)
{
	mm_gce_lock_prot_en();
	wait_for_write_done(MMINFRA_MM1_GALS_PROT_TX_EN,
			    MMINFRA_MM1_GALS_PROT_TX_RDY, 0x3f);
	wait_for_write_done(MMINFRA_MM1_GALS_PROT_RX_EN,
			    MMINFRA_MM1_GALS_PROT_RX_RDY, 0xf);
}

static void mm_infra1_release_prot_en(void)
{
	write32p(MMINFRA_MM1_GALS_PROT_RX_EN, 0);
	write32p(MMINFRA_MM1_GALS_PROT_TX_EN, 0);
	mm_gce_release_prot_en();
}

static int pd_mm_infra0_pre_on(void)
{
	setbits32p(MMPC_PM_BOOT_UP_PWR_CON, MM_INFRA0_PM_BOOT_UP | MM_INFRA1_PM_BOOT_UP);
	return 0;
}

static int pd_mm_infra0_post_on(void)
{
	mm_infra0_release_prot_en();
	return 0;
}

static int pd_mm_infra0_pre_off(void)
{
	mm_infra0_lock_prot_en();
	return 0;
}

static int pd_mm_infra1_post_on(void)
{
	mm_infra1_release_prot_en();
	write32p(VLP_AO_RSVD6, 0x1);
	return 0;
}

static int pd_mm_infra1_pre_off(void)
{
	mm_infra1_lock_prot_en();
	return 0;
}

static struct mtcmos_cb mm_infra0_pb_cb = {
	.pre_on = pd_mm_infra0_pre_on,
	.post_on = pd_mm_infra0_post_on,
	.pre_off = pd_mm_infra0_pre_off,
};

static struct mtcmos_cb mm_infra1_pb_cb = {
	.post_on = pd_mm_infra1_post_on,
	.pre_off = pd_mm_infra1_pre_off,
};

void mminfra_post_init(void)
{
	if (mtcmos_cb_register(MTCMOS_ID_MM_INFRA0, &mm_infra0_pb_cb))
		return;

	mtcmos_cb_register(MTCMOS_ID_MM_INFRA1, &mm_infra1_pb_cb);
}
