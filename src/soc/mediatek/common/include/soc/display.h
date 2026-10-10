/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_COMMON_DISPLAY_H__
#define __SOC_MEDIATEK_COMMON_DISPLAY_H__

#include <commonlib/coreboot_tables.h>
#include <edid.h>
#include <mipi/panel.h>
#include <stdbool.h>
#include <thread.h>

enum disp_path_sel {
	DISP_PATH_NONE = 0,
	DISP_PATH_EDP,
	DISP_PATH_MIPI,
	DISP_PATH_DUAL_MIPI,
};

#define PANEL_QUIRK_FORCE_MAX_SWING	BIT(0)

/*
 * Boot backlight driven by the SoC DISP_PWM, pre-set in configure_backlight()
 * so the kernel's first pwm_bl brightness write is a no-op. Values must match
 * the kernel (pwm-backlight DTS pwms period / default-brightness-level /
 * brightness-levels / scale). Leave max == 0 if the panel has no such data:
 * configure_backlight() then keeps driving the backlight through the board's
 * legacy GPIO path.
 */
struct panel_backlight {
	u32 period_ns;	/* DTS pwms period */
	u32 level;	/* brightness-levels[default-brightness-level] */
	u32 max;	/* brightness scale (last brightness-levels entry) */
};

struct panel_description {
	const char *name;
	void (*configure_backlight)(bool enable);
	void (*power_on)(void);
	void (*reset)(void);
	int (*get_edid)(struct edid *edid);
	int (*post_power_on)(const struct edid *edid);
	enum lb_fb_orientation orientation;
	enum disp_path_sel disp_path;
	struct panel_backlight backlight;
	bool pwm_ctrl_gpio;
	uint32_t quirks;
};

struct panel_description *get_active_panel(void);
void mtk_display_disable_secure_mode(void);
int mtk_display_init(struct thread_mutex *ready_mutex);
int mtk_mipi_panel_poweroff(void);

void mtk_ddp_init(void);
u32 mtk_get_vrefresh(const struct edid *edid);
void mtk_ddp_mode_set(const struct edid *edid, enum disp_path_sel path,
		      struct dsc_config *dsc_config);
void mtk_ddp_ovlsys_start(uintptr_t fb_addr, const struct edid *edid,
			  enum disp_path_sel path);

const struct panel_serializable_data *mtk_get_mipi_panel_data(void);

#endif
