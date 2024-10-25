/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <fw_config.h>
#include <sar.h>
#include <soc/gpio_soc_defs.h>
#include <intelblocks/graphics.h>
#include <drivers/intel/gma/opregion.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>

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
	if (!fw_config_probe(FW_CONFIG(WIFI, WIFI_6E))) {
		printk(BIOS_INFO, "CNVi bluetooth disabled by fw_config\n");
		config->cnvi_bt_core = false;
		printk(BIOS_INFO, "CNVi bluetooth audio offload disabled by fw_config\n");
		config->cnvi_bt_audio_offload = false;
	}
}

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (fw_config_probe(FW_CONFIG(WIFI, WIFI_6_7921))
		|| fw_config_probe(FW_CONFIG(WIFI, WIFI_6_8852))) {
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

static int get_ssfc(uint32_t *val)
{
	static uint32_t known_value;
	static enum {
		SSFC_NOT_READ,
		SSFC_AVAILABLE,
	} ssfc_state = SSFC_NOT_READ;

	if (ssfc_state == SSFC_AVAILABLE) {
		*val = known_value;
		return 0;
	}

	/*
	 * If SSFC field is not in the CBI then the value of SSFC will be 0 for
	 * further processing later since 0 of each bits group means default
	 * component in a variant. For more detail, please refer to cbi_ssfc.h.
	 */
	if (google_chromeec_cbi_get_ssfc(&known_value) != 0) {
		printk(BIOS_DEBUG, "SSFC not set in CBI\n");
		return -1;
	}

	ssfc_state = SSFC_AVAILABLE;
	*val = known_value;
	printk(BIOS_INFO, "SSFC 0x%x.\n", known_value);
	return 0;
}

const char *mainboard_vbt_filename(void)
{
	uint32_t ssfc;
	if (get_ssfc(&ssfc)) {
		printk(BIOS_INFO, "Failed to read SSFC, using default vbt-teliks.bin\n");
		return "vbt-teliks.bin";
	}

	/*
	 * Determine if the panel is 11 inches based on the SSFC register.
	 *
	 * Bit 6 of the SSFC register indicates the panel size:
	 *   0: 12.2 inch panel
	 *   1: 11.6 inch panel
	 */
	bool is_panel_11_inch = (ssfc >> 6) & 0x1;

	if (is_panel_11_inch) {
		printk(BIOS_INFO, "Bit 6 of SSFC is 1, use vbt-teliks_panel_11_inch.bin\n");
		return "vbt-teliks_panel_11_inch.bin";
	}

	printk(BIOS_INFO, "Bit 6 of SSFC is 0, use vbt-teliks.bin\n");
	return "vbt-teliks.bin";
}
