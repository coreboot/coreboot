/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>
#include <inttypes.h>

/* t: base table; o: override table */
#define GPIO_PADBASED_OVERRIDE(t, o) gpio_padbased_override(t, o, ARRAY_SIZE(o))
/* t: table */
#define GPIO_CONFIGURE_PADS(t) gpio_configure_pads(t, ARRAY_SIZE(t))

static const struct pad_config pre_mem_gen4_ssd_pwr_pads[] = {
	/* GPP_H18:     GEN4_SSD_PWREN */
	PAD_CFG_GPO(GPP_H18, 0, PLTRST),
};

static const struct pad_config gen4_ssd_pads[] = {
	/* GPP_H18:     GEN4_SSD_PWREN */
	PAD_CFG_GPO(GPP_H18, 1, PLTRST),
	/* GPP_A08:     M2_GEN4_SSD_RESET_N */
	PAD_CFG_GPO(GPP_A08, 1, PLTRST),
};

static const struct pad_config ufs_enable_pads[] = {
	/* GPP_D21:     GPP_D21_UFS_REFCLK */
	PAD_CFG_NF(GPP_D21, NONE, DEEP, NF1),
};

static const struct pad_config cnvi_enable_pads[] = {
	/* GPP_B09:     BT_RF_KILL_N */
	PAD_CFG_GPO(GPP_B09, 1, DEEP),
	/* GPP_C10:     WIFI_RF_KILL_N */
	PAD_CFG_GPO(GPP_C10, 1, DEEP),
	/* GPP_F00:     M.2_CNV_BRI_DT_BT_UART2_RTS_N */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F00, NONE, DEEP, NF1),
	/* GPP_F01:     M.2_CNV_BRI_RSP_BT_UART2_RXD */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F01, NONE, DEEP, NF1),
	/* GPP_F02:     M.2_CNV_RGI_DT_BT_UART2_TXD */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F02, NONE, DEEP, NF1),
	/* GPP_F03:     M.2_CNV_RGI_RSP_BT_UART2_CTS_N */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F03, NONE, DEEP, NF1),
	/* GPP_F04:     CNV_RF_RESET_R_N */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F04, NONE, DEEP, NF1),
	/* GPP_F05:     CRF_CLKREQ_R */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F05, NONE, DEEP, NF3),
};

static const struct pad_config cnvi_disable_pads[] = {
	/* GPP_B09:     BT_RF_KILL_N */
	PAD_NC(GPP_B09, NONE),
	/* GPP_C10:     WIFI_RF_KILL_N */
	PAD_NC(GPP_C10, NONE),
	/* GPP_F00:     M.2_CNV_BRI_DT_BT_UART2_RTS_N */
	PAD_NC(GPP_F00, NONE),
	/* GPP_F01:     M.2_CNV_BRI_RSP_BT_UART2_RXD */
	PAD_NC(GPP_F01, NONE),
	/* GPP_F02:     M.2_CNV_RGI_DT_BT_UART2_TXD */
	PAD_NC(GPP_F02, NONE),
	/* GPP_F03:     M.2_CNV_RGI_RSP_BT_UART2_CTS_N */
	PAD_NC(GPP_F03, NONE),
	/* GPP_F04:     CNV_RF_RESET_R_N */
	PAD_NC(GPP_F04, NONE),
	/* GPP_F05:     CRF_CLKREQ_R */
	PAD_NC(GPP_F05, NONE),
};

void fw_config_configure_pre_mem_gpio(void)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}

	if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME_GEN4))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pwr_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UNKNOWN))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pwr_pads);
	}
}

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	//const struct soc_intel_pantherlake_config *config = config_of_soc();

	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}

	if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME_GEN4))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, gen4_ssd_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UFS))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, ufs_enable_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UNKNOWN))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, gen4_ssd_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, ufs_enable_pads);
	}

	if (fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_6)) ||
		fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_7))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, cnvi_enable_pads);
	} else if (fw_config_probe(FW_CONFIG(WIFI, WIFI_NONE)))  {
		GPIO_PADBASED_OVERRIDE(padbased_table, cnvi_disable_pads);
	}
}
