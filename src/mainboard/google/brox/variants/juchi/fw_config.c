/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>

static const struct pad_config fp_disable_pads[] = {
	/* D2  : ISH_GP2 ==> EN_FP_PWR (NC) */
	PAD_NC(GPP_D2, NONE),
	/* GPP_D3 : IPCH_FP_BOOT0 (active high) (NC) */
	PAD_NC(GPP_D3, NONE),
	/* GPP_D15 : FPMCU_RST_J_SUB_L (active low) (NC) */
	PAD_NC(GPP_D15, NONE),
	/* GPP_D13 : FP GSPI INT (NC)  */
	PAD_NC(GPP_D13, NONE),
	/* GPP_F11 : FP GSPI CLK (NC) */
	PAD_NC(GPP_F11, NONE),
	/* GPP_F12 : FP GSPI DO (NC) */
	PAD_NC(GPP_F12, NONE),
	/* GPP_F13 : FP GSPI DI (NC) */
	PAD_NC(GPP_F13, NONE),
	/* GPP_F16 : FP GSPI CS (NC) */
	PAD_NC(GPP_F16, NONE),
};

static const struct pad_config lte_disable_pads[] = {
	/* GPP_E11 : WWAN_CFG0 */
	PAD_NC(GPP_E11, NONE),
	/* GPP_E17 : WWAN_CFG02 */
	PAD_NC(GPP_E17, NONE),
	/* GPP_D7 :WWAN_RF_DISABLE_ODL */
	PAD_NC(GPP_D7, NONE),
	/* GPP_D5 : WWAN_SAR_ODL */
	PAD_NC(GPP_D5, NONE),
	/* GPP_F21 : WWAN_FCPO_L */
	PAD_NC(GPP_F21, NONE),
	/* GPP_S4 : WWAN_WLAN_COEX1 */
	PAD_NC(GPP_S4, NONE),
	/* GPP_S5 : WWAN_WLAN_COEX2 */
	PAD_NC(GPP_S5, NONE),
	/* GPP_F6 : WWAN_WLAN_COEX3 */
	PAD_NC(GPP_F6, NONE),
	/* GPP_A12 : WWAN_PWR_EN */
	PAD_NC(GPP_A12, NONE),
	/* GPP_H23 : WWAN_RST_L */
	PAD_NC(GPP_H23, NONE),
	/* GPP_H19 : SRCCLKREQ4_L ==> SAR1_INT_L (NC) */
	PAD_NC(GPP_H19, NONE),
};

static const struct pad_config lte_sar_disable_pads[] = {
	/* GPP_D5 : WWAN_SAR_ODL */
	PAD_NC(GPP_D5, NONE),
	/* GPP_S4 : WWAN_WLAN_COEX1 */
	PAD_NC(GPP_S4, NONE),
	/* GPP_S5 : WWAN_WLAN_COEX2 */
	PAD_NC(GPP_S5, NONE),
	/* GPP_F6 : WWAN_WLAN_COEX3 */
	PAD_NC(GPP_F6, NONE),
	/* GPP_H19 : SRCCLKREQ4_L ==> SAR1_INT_L (NC) */
	PAD_NC(GPP_H19, NONE),
};

static const struct pad_config nvme_disable_pads[] = {
	/* GPP_F9 : SSD_PERST_L */
	PAD_NC(GPP_F9, NONE),
	/* GPP_D11 : EN_PP3300_SSD (NC) */
	PAD_NC(GPP_D11, NONE),
	/* GPP_D8 : SSD_CLKREQ_ODL */
	PAD_NC(GPP_D8, NONE),
};

static void fw_config_handle(void *unused)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is unprovisioned. Skip disable device's pads.\n");
		return;
	}

	if (fw_config_probe(FW_CONFIG(FPMCU, FPMCU_ABSENT))) {
		printk(BIOS_INFO, "Disable Fingerprint GPIOs by fw_config.\n");
		gpio_configure_pads(fp_disable_pads, ARRAY_SIZE(fp_disable_pads));
	}

	if (fw_config_probe(FW_CONFIG(DB_USB, DB_1A))) {
		printk(BIOS_INFO, "Disable LTE-related GPIO pins by fw_config.\n");
		gpio_configure_pads(lte_disable_pads, ARRAY_SIZE(lte_disable_pads));
	}

	if (fw_config_probe(FW_CONFIG(DB_USB, DB_1A_LTE))) {
		printk(BIOS_INFO, "Disable LTE's Sar sensor related GPIO pins by fw_config.\n");
		gpio_configure_pads(lte_sar_disable_pads, ARRAY_SIZE(lte_sar_disable_pads));
	}

	if (!fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME))) {
		printk(BIOS_INFO, "Disable NVMe GPIO pins by fw_config.\n");
		gpio_configure_pads(nvme_disable_pads, ARRAY_SIZE(nvme_disable_pads));
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
