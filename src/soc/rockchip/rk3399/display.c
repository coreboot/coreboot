/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmu.h>
#include <device/mmio.h>
#include <console/console.h>
#include <device/device.h>
#include <delay.h>
#include <edid.h>
#include <gpio.h>
#include <stdint.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/display.h>
#include <soc/edp.h>
#include <soc/grf.h>
#include <soc/mmu_operations.h>
#include <soc/mipi.h>
#include <soc/soc.h>
#include <soc/vop.h>
#include <framebuffer_info.h>

#include "chip.h"

static void reset_edp(void)
{
	/* rst edp */
	write32(&cru_ptr->softrst_con[17],
		RK_SETBITS(1 << 12 | 1 << 13));
		udelay(1);
	write32(&cru_ptr->softrst_con[17],
		RK_CLRBITS(1 << 12 | 1 << 13));
	printk(BIOS_WARNING, "Retrying EDP initialization.\n");
}

void rk_display_init(struct device *dev)
{
	struct edid edid;
	struct soc_rockchip_rk3399_config *conf = dev->chip_info;
	enum vop_modes detected_mode = VOP_MODE_UNKNOWN;
	const struct mipi_panel_data *panel_data = NULL;
	int retry_count_init = 0;
	int retry_count_edp_prepare = 0;

	/* let's use vop0 in rk3399 */
	uint32_t vop_id = 0;

	switch (conf->vop_mode) {
	case VOP_MODE_NONE:
		return;
	case VOP_MODE_EDP:
		printk(BIOS_DEBUG, "Attempting to set up EDP display.\n");
		rkclk_configure_vop_aclk(vop_id, 200 * MHz);
		rkclk_configure_edp(25 * MHz);

		/* select edp signal from vop0 */
		write32(&rk3399_grf->soc_con20, RK_CLRBITS(1 << 5));

		/* select edp clk from SoC internal 24M crystal, otherwise,
		 * it will source from edp's 24M clock (that depends on
		 * edp vendor, could be unstable)
		 */
		write32(&rk3399_grf->soc_con25, RK_SETBITS(1 << 11));

retry_edp:
		/* Reset in case code jumped here. */
		retry_count_init = 0;
		while (retry_count_init++ < 3) {
			rk_edp_init();
			if (rk_edp_get_edid(&edid) == 0) {
				detected_mode = VOP_MODE_EDP;
				break;
			}
			if (retry_count_init == 3) {
				printk(BIOS_WARNING, "EDP initialization failed.\n");
				return;
			} else {
				reset_edp();
			}
		}
		break;
	case VOP_MODE_MIPI:
		printk(BIOS_DEBUG, "Attempting to setup MIPI display.\n");

		rkclk_configure_mipi();
		rkclk_configure_vop_aclk(vop_id, 200 * MHz);

		/*
		 * disable tx0 turnrequest, turndisable,
		 * forcetxstop, forcerxmode
		 */
		write32(&rk3399_grf->soc_con22, RK_CLRBITS(0xffff));

		/* disable tx1 turndisable, forcetxstop, forcerxmode */
		write32(&rk3399_grf->soc_con23, RK_CLRBITS(0xfff0));

		/*
		 * enable dphy_tx1rx1_masterslavez,
		 * clear dphy_tx1rx1_enableclk,
		 * clear dphy_tx1rx1_basedir,
		 * disable tx1 turnrequest
		 */
		write32(&rk3399_grf->soc_con24,
			RK_CLRSETBITS(1 << 7 | 1 << 6 | 1 << 5 | 0xf,
				      1 << 7 | 0 << 6 | 0 << 5 | 0 << 0));

		/* dphy_tx1rx1_enable */
		write32(&rk3399_grf->soc_con23, RK_SETBITS(0xf));

		/* select mipi-dsi0 and mipi-dsi1 signal from vop0 */
		write32(&rk3399_grf->soc_con20,
			RK_CLRBITS((1 << 0) | (1 << 4)));

		panel_data = mainboard_get_mipi_mode(&edid.mode);
		if (panel_data) {
			if (panel_data->mipi_num > 1)
				detected_mode = VOP_MODE_DUAL_MIPI;
			else
				detected_mode = VOP_MODE_MIPI;
		} else {
			printk(BIOS_WARNING, "Can not get mipi panel data\n");
			return;
		}
		break;
	default:
		printk(BIOS_WARNING, "Unsupported vop_mode, aborting.\n");
		return;
	}

	if (rkclk_configure_vop_dclk(vop_id,
				     edid.mode.pixel_clock * KHz)) {
		printk(BIOS_WARNING, "config vop err\n");
		return;
	}

	edid_set_framebuffer_bits_per_pixel(&edid,
		conf->framebuffer_bits_per_pixel, 0);
	rkvop_mode_set(vop_id, &edid, detected_mode);

	rkvop_prepare(vop_id, &edid);

	switch (detected_mode) {
	case VOP_MODE_MIPI:
	case VOP_MODE_DUAL_MIPI:
		rk_mipi_prepare(&edid, panel_data);
		break;
	case VOP_MODE_EDP:
		/* will enable edp in depthcharge */
		if (rk_edp_prepare()) {
			if (retry_count_edp_prepare++ < 3) {
				reset_edp();
				/* Rerun entire init sequence */
				goto retry_edp;
			}
			printk(BIOS_ERR, "EDP preparation failed.");
			return;
		}
		break;
	default:
		break;
	}
	mainboard_power_on_backlight();
	fb_new_framebuffer_info_from_edid(&edid, (uintptr_t)0);
}
