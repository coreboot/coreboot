/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <boardid.h>
#include <device/device.h>
#include <drivers/i2c/hid/chip.h>
#include <fw_config.h>
#include <gpio.h>

static void devtree_update_emmc_rtd3(uint32_t board_ver)
{
	struct device *emmc_rtd3 = DEV_PTR(emmc_rtd3);
	if (board_ver > 1)
		return;

	emmc_rtd3->enabled = 0;
}

static void devtree_update_audio_codec(void)
{
	struct device *audio_codec = DEV_PTR(audio_codec);
	struct drivers_i2c_generic_config *config = audio_codec->chip_info;

	if (fw_config_probe(FW_CONFIG(AUDIO, MAX98360_ALC5682I_VS_I2S)))
		config->hid = "RTL5682";
}

static const struct pad_config nvme_disable_pads[] = {
	PAD_NC(GPP_B2, NONE),	/* B2  : VRALERT# ==> M2_SSD_PLA_L */
	PAD_NC(GPP_B4, NONE),	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_NC(GPP_D3, NONE),	/* D3  : ISH_GP3 ==> M2_SSD_PLN_L */
	PAD_NC(GPP_D5, NONE),	/* D5  : SRCCLKREQ0# ==> SSD_CLKREQ_ODL */
	PAD_NC(GPP_D11, NONE),	/* D11 : ISH_SPI_MISO ==> EN_PP3300_SSD */
};

static const struct pad_config emmc_disable_pads[] = {
	PAD_NC(GPP_B3, NONE),	/* B3  : PROC_GP2 ==> eMMC_PERST_L */
	PAD_NC(GPP_E20, NONE),	/* E20 : USB_C1_LSX_SOC_TX ==> EN_PP3300_eMMC */
	PAD_NC(GPP_F19, NONE),	/* F19 : SRCCLKREQ6# ==> EMMC_CLKREQ_ODL */
};

static void disable_unused_gpios(void)
{
	int emmc_detected = gpio_get(GPP_T2);

	if (emmc_detected == 1)
		gpio_configure_pads(nvme_disable_pads, ARRAY_SIZE(nvme_disable_pads));
	else
		gpio_configure_pads(emmc_disable_pads, ARRAY_SIZE(emmc_disable_pads));
}

void variant_devtree_update(void)
{
	uint32_t board_ver = board_id();
	disable_unused_gpios();
	devtree_update_emmc_rtd3(board_ver);
	devtree_update_audio_codec();
}
