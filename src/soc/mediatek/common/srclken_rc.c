/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/clkbuf_ctl.h>
#include <soc/pmif.h>
#include <soc/pmif_common.h>
#include <soc/spmi.h>
#include <soc/srclken_rc.h>
#include <soc/srclken_rc_common.h>
#include <timer.h>

/* RC initial flow and relative setting */
static void rc_ctrl_mode_switch(enum chn_id id, enum rc_ctrl_m mode)
{
	assert(id < ARRAY_SIZE(rc_regs->rc_mxx_srclken_cfg));
	int cfg = (mode == SW_MODE);

	SET32_BITFIELDS(&rc_regs->rc_mxx_srclken_cfg[id], SW_SRCLKEN_RC, cfg);

	printk(BIOS_INFO, "M0%d: 0x%x\n", id, read32(&rc_regs->rc_mxx_srclken_cfg[id]));
}

/* RC subsys FPM control */
static void rc_ctrl_fpm_switch(enum chn_id id, u32 mode)
{
	assert(id < ARRAY_SIZE(rc_regs->rc_mxx_srclken_cfg));
	assert(mode == SW_FPM_HIGH || mode == SW_FPM_LOW);

	int fpm = (mode == SW_FPM_HIGH) ? 1 : 0;
	SET32_BITFIELDS(&rc_regs->rc_mxx_srclken_cfg[id], SW_SRCLKEN_FPM, fpm);

	printk(BIOS_INFO, "M0%d FPM SWITCH: %#x\n", id,
	       read32(&rc_regs->rc_mxx_srclken_cfg[id]));
}

void rc_init_subsys_hw_mode(void)
{
	int chn_n;

	for (chn_n = 0; chn_n < MAX_CHN_NUM; chn_n++) {
		if (rc_config[chn_n].hw_mode)
			rc_ctrl_mode_switch(chn_n, HW_MODE);
	}
}

void rc_init_subsys_lpm(void)
{
	int chn_n;

	for (chn_n = 0; chn_n < MAX_CHN_NUM; chn_n++) {
		if (rc_config[chn_n].lpm)
			rc_ctrl_fpm_switch(chn_n, SW_FPM_LOW);
	}
}
