/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <fw_config.h>
#include <sar.h>
#include <soc/gpio_soc_defs.h>
#include <intelblocks/graphics.h>

/* Per-pipe DDI Function Control 2 */
#define TRANS_DDI_FUNC_CTL2_A					0x60404
#define TRANS_DDI_AUDIO_MUTE_OVERRIDE_BITS_FIELDS		(3 << 6)

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI));
}

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	if (!fw_config_is_provisioned() || fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_WIFI6E))) {
		printk(BIOS_INFO, "CNVi bluetooth enabled by fw_config\n");
		config->cnvi_bt_core = true;
		config->cnvi_bt_audio_offload = true;
	}
}

void variant_finalize(void)
{
	/*
	 * Panel CSOT MNB601LS1-3 will flicker once during enter Chrome login screen,
	 * it is because it inserts 12 blank frames if it receives the unmute in VB-ID.
	 *
	 * Always override the mute in VB-ID to avoid Tcon EC detected the
	 * audiomute_flag change.
	 */
	graphics_gtt_rmw(TRANS_DDI_FUNC_CTL2_A, ~TRANS_DDI_AUDIO_MUTE_OVERRIDE_BITS_FIELDS,
		TRANS_DDI_AUDIO_MUTE_OVERRIDE_BITS_FIELDS);
}
