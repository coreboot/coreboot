/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <bootsplash.h>
#include <console/console.h>
#include <fsp/util.h>
#include <timestamp.h>
#include <ux_locales.h>
#include <static.h>
#include <soc/soc_chip.h>

#include "ux.h"

#if CONFIG(FSP_VGA_MODE12)
void soc_set_vga_mode12_buffer(FSPM_UPD *fspm_upd, uintptr_t buffer)
{
	fspm_upd->FspmConfig.VgaGraphicsMode12ImagePtr = buffer;
}
#endif

static void setup_vga_mode12_params(FSP_M_CONFIG *m_cfg, enum ux_locale_msg id)
{
	struct soc_intel_common_config *common_config = chip_get_common_soc_structure();
	unsigned char *vga_bitmap_buffer = (unsigned char *)(uintptr_t)m_cfg->VgaGraphicsMode12ImagePtr;
	enum lb_fb_orientation current_orientation = common_config->panel_orientation;
	if (!vga_bitmap_buffer) {
		return;
	}
	int img_width = VGA12_WIDTH;
	int img_height = VGA12_HEIGHT;
	if (!m_cfg->LidStatus)
		current_orientation = LB_FB_ORIENTATION_NORMAL;
	render_text_to_bitmap_buffer(vga_bitmap_buffer,
				current_orientation,
				ux_locales_get_text(id),
				&img_width, &img_height);
	int img_size = img_width * img_height / 8; // Image is a bitmap
	// Duplicate the first plane data to all other planes
	for (int i = 1; i < CONFIG_FSP_VGA_MODE12_BPP; i++)
		memcpy(vga_bitmap_buffer + (i * img_size),
			vga_bitmap_buffer,
			img_size);

	m_cfg->LogoPixelHeight = img_height;
	m_cfg->LogoPixelWidth = img_width;
	m_cfg->LogoXPosition = (VGA12_WIDTH - img_width) / 2;
	m_cfg->LogoYPosition = (VGA12_HEIGHT - img_height) / 2;
	m_cfg->VgaInitControl |= VGA_INIT_CONTROL_MODE12;
}

static bool ux_inform_user_of_operation(const char *name, enum ux_locale_msg id,
		 FSPM_UPD *mupd)
{
	const struct soc_intel_pantherlake_config *config = config_of_soc();
	timestamp_add_now(TS_ESOL_START);

	if (!CONFIG(CHROMEOS_ENABLE_ESOL)) {
		timestamp_add_now(TS_ESOL_END);
		return false;
	}

	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
	void *vbt;
	size_t vbt_size;
	static bool ux_done = false;

	/*
	 * Prevents multiple VGA text messages from being rendered during the boot process.
	 *
	 * This function is designed to be called only once. Subsequent calls are intentionally
	 * ignored to avoid overwriting previously displayed messages. For example, if a
	 * low-battery shutdown notification is scheduled, a later call with a firmware
	 * update notification could result in the low-battery message being lost.
	 */
	if (ux_done) {
		timestamp_add_now(TS_ESOL_END);
		return true;
	}

	printk(BIOS_INFO, "Informing user on-display of %s.\n", name);

	vbt = cbfs_map("vbt.bin", &vbt_size);
	if (!vbt) {
		printk(BIOS_ERR, "Could not load vbt.bin\n");
		return false;
	}

	m_cfg->VgaInitControl = VGA_INIT_CONTROL_ENABLE;
	if (config->disable_progress_bar)
		m_cfg->VgaInitControl |= VGA_INIT_DISABLE_ANIMATION;
	m_cfg->VbtPtr = (efi_uintn_t)vbt;
	m_cfg->VbtSize = vbt_size;
	m_cfg->LidStatus = CONFIG(VBOOT_LID_SWITCH) ? get_lid_switch() : CONFIG(RUN_FSP_GOP);
	m_cfg->VgaMessage = (efi_uintn_t)ux_locales_get_text(id);

	if (CONFIG(FSP_VGA_MODE12))
		setup_vga_mode12_params(m_cfg, id);

	ux_done = true;

	timestamp_add_now(TS_ESOL_END);
	return true;
}

bool ux_inform_user_of_update_operation(const char *name, FSPM_UPD *mupd)
{
	return ux_inform_user_of_operation(name, UX_LOCALE_MSG_MEMORY_TRAINING, mupd);
}

bool ux_inform_user_of_poweroff_operation(const char *name, FSPM_UPD *mupd)
{
	return ux_inform_user_of_operation(name, UX_LOCALE_MSG_LOW_BATTERY, mupd);
}
