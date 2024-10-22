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

static const struct pad_config wifi_pcie_enable_pad[] = {
	/* H20 : IMGCLKOUT1 ==> WLAN_PERST_L */
	PAD_CFG_GPO(GPP_H20, 1, DEEP),
	/* B11 : NC ==> EN_PP3300_WLAN_X*/
	PAD_CFG_GPO(GPP_B11, 1, DEEP),
};

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	if (!fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_WIFI6E)) || fw_config_is_provisioned()) {
		printk(BIOS_INFO, "CNVi bluetooth disabled by fw_config\n");
		config->cnvi_bt_core = false;
		printk(BIOS_INFO, "CNVi bluetooth audio offload disabled by fw_config\n");
		config->cnvi_bt_audio_offload = false;
	}
}

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_WIFI6E)) || !fw_config_is_provisioned()) {
		printk(BIOS_INFO, "Enable PCie based Wifi GPIO pins.\n");
		gpio_padbased_override(padbased_table, wifi_pcie_enable_pad,
				       ARRAY_SIZE(wifi_pcie_enable_pad));
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
