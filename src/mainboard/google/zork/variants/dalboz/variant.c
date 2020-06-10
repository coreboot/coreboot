/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <console/console.h>
#include <device/device.h>
#include <drivers/i2c/generic/chip.h>
#include <soc/pci_devs.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/i2c_tunnel/chip.h>
#include <string.h>

#define EC_PNP_ID 0x0c09

/* Look for an EC device of type PNP with id 0x0c09 */
static bool match_ec_dev(DEVTREE_CONST struct device *dev)
{
	if (dev->path.type != DEVICE_PATH_PNP)
		return false;

	if (dev->path.pnp.port != EC_PNP_ID)
		return false;

	return true;
}

extern struct chip_operations drivers_i2c_generic_ops;

/* Look for an I2C device with HID "10EC5682" */
static bool match_audio_dev(DEVTREE_CONST struct device *dev)
{
	struct drivers_i2c_generic_config *cfg;

	if (dev->chip_ops != &drivers_i2c_generic_ops)
		return false;

	cfg = dev->chip_info;

	return !strcmp(cfg->hid, "10EC5682");
}

extern struct chip_operations ec_google_chromeec_i2c_tunnel_ops;

/* Look for Cros EC tunnel device which has audio device under it. */
static bool match_audio_tunnel(DEVTREE_CONST struct device *dev)
{
	const struct device *audio_dev;

	if (dev->chip_ops != &ec_google_chromeec_i2c_tunnel_ops)
		return false;

	audio_dev = dev_find_matching_device_on_bus(dev->link_list, match_audio_dev);

	if (!audio_dev)
		return false;

	return true;
}

/*
 * This is to allow support for audio on older board versions (< 2). [b/153458561]. This
 * should be removed once these boards are phased out.
 */
static void update_audio_configuration(void)
{
	uint32_t board_version;
	const struct device *lpc_controller;
	const struct device *ec_dev;
	const struct device *i2c_tunnel_dev;
	struct ec_google_chromeec_i2c_tunnel_config *cfg;

	/* If CBI board version cannot be read, assume this is an older revision of hardware. */
	if (google_chromeec_cbi_get_board_version(&board_version) != 0)
		board_version = 1;

	if (board_version >= 2)
		return;

	lpc_controller = SOC_LPC_DEV;
	if (lpc_controller == NULL) {
		printk(BIOS_ERR, "%s: LPC controller device not found!\n", __func__);
		return;
	}

	ec_dev = dev_find_matching_device_on_bus(lpc_controller->link_list, match_ec_dev);

	if (ec_dev == NULL) {
		printk(BIOS_ERR, "%s: EC device not found!\n", __func__);
		return;
	}

	i2c_tunnel_dev = dev_find_matching_device_on_bus(ec_dev->link_list, match_audio_tunnel);

	if (i2c_tunnel_dev == NULL) {
		printk(BIOS_ERR, "%s: I2C tunnel device not found!\n", __func__);
		return;
	}

	cfg = i2c_tunnel_dev->chip_info;
	if (cfg == NULL) {
		printk(BIOS_ERR, "%s: I2C tunnel device config not found!\n", __func__);
		return;
	}

	cfg->remote_bus = 5;
}

static int sku_has_emmc(void)
{
	uint32_t board_sku = sku_id();

	/* Factory flow requires all OS boot media to be enabled. */
	if (boot_is_factory_unprovisioned())
		return 1;

	/* FIXME: This needs to be fw_config controlled. */
	/* Enable emmc0 for unknown skus. Only sku3/0xC really has it. */
	if (board_sku == 0x5A80000C || board_sku == 0x5A800003 || board_sku == CROS_SKU_UNKNOWN)
		return 1;

	return 0;
}

void variant_devtree_update(void)
{
	struct soc_amd_picasso_config *cfg;

	cfg = config_of_soc();

	if (sku_has_emmc()) {
		if (sku_id() == 0x5A800003)
			/* rev0 boards have issues with HS400 */
			cfg->sd_emmc_config = SD_EMMC_EMMC_HS200;
	} else {
		cfg->sd_emmc_config = SD_EMMC_DISABLE;
	}

	update_audio_configuration();
}

/* FIXME: Comments seem to suggest these are not entirely correct. */
static const fsp_ddi_descriptor non_hdmi_ddi_descriptors[] = {
	{
		// DDI0, DP0, eDP
		.connector_type = EDP,
		.aux_index = AUX1,
		.hdp_index = HDP1
	},
	{
		// DDI1, DP1, DB OPT2 USB-C1 / DB OPT3 MST hub
		.connector_type = DP,
		.aux_index = AUX2,
		.hdp_index = HDP2
	},
	{
		// DP2 pins not connected on Dali
		// DDI2, DP3, USB-C0
		.connector_type = DP,
		.aux_index = AUX4,
		.hdp_index = HDP4,
	}
};

static const fsp_ddi_descriptor hdmi_ddi_descriptors[] = {
	{ // DDI0, DP0, eDP
		.connector_type = EDP,
		.aux_index = AUX1,
		.hdp_index = HDP1
	},
	{ // DDI1, DP1, DB OPT2 USB-C1 / DB OPT3 MST hub
		.connector_type = HDMI,
		.aux_index = AUX2,
		.hdp_index = HDP2
	},
	// DP2 pins not connected on Dali
	{ // DDI2, DP3, USB-C0
		.connector_type = DP,
		.aux_index = AUX4,
		.hdp_index = HDP4,
	}
};

void variant_get_pcie_ddi_descriptors(const fsp_pcie_descriptor **pcie_descs,
				      size_t *pcie_num,
				      const fsp_ddi_descriptor **ddi_descs,
				      size_t *ddi_num)
{
	uint32_t board_sku = sku_id();

	*pcie_descs = baseboard_get_pcie_descriptors(pcie_num);

	/* SKU 1, A, and D DB have HDMI, as well as unknown */
	/* FIXME: this needs to be fw_config controlled. */
	if ((board_sku == 0x5A80000A) || (board_sku == 0x5A80000D) || (board_sku == 0x5A800001)
	    || (board_sku == CROS_SKU_UNKNOWN)) {
		*ddi_descs = &hdmi_ddi_descriptors[0];
		*ddi_num = ARRAY_SIZE(hdmi_ddi_descriptors);
	} else {
		*ddi_descs = &non_hdmi_ddi_descriptors[0];
		*ddi_num = ARRAY_SIZE(non_hdmi_ddi_descriptors);
	}
}
