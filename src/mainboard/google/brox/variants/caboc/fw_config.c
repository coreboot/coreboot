/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>

static const struct pad_config fp_disable_pads[] = {
	/* GPP_D8 : PCH_FP_BOOT0 */
	PAD_NC(GPP_D8, NONE),
	/* GPP_D9 : FP_RST_ODL */
	PAD_NC(GPP_D9, NONE),
	/* GPP_D11 : [] ==> EN_FP_PWR */
	PAD_NC(GPP_D11, NONE),
	/* GPP_E4 : FP GSPI INT */
	PAD_NC(GPP_E4, NONE),
	/* GPP_F11 : FP GSPI CLK */
	PAD_NC(GPP_F11, NONE),
	/* GPP_F12 : FP GSPI DO */
	PAD_NC(GPP_F12, NONE),
	/* GPP_F13 : FP GSPI DI */
	PAD_NC(GPP_F13, NONE),
	/* GPP_F16 : FP GSPI CS */
	PAD_NC(GPP_F16, NONE),
};

static const struct pad_config lte_disable_pads[] = {
	/* GPP_A17 :WWAN_TRANSMIT_OFF */
	PAD_NC(GPP_A17, NONE),
	/* GPP_A21 : WWAN_ASPM_EXIT */
	PAD_NC(GPP_A21, NONE),
	/* GPP_D4 : WWAN_GPS_XMIT_L */
	PAD_NC(GPP_D4, NONE),
	/* GPP_D15 : WWAN_CONFIG_3 */
	PAD_NC(GPP_D15, NONE),
	/* GPP_D16 : WWAN_CONFIG_2 */
	PAD_NC(GPP_D16, NONE),
	/* GPP_D19 : WWAN_FWUPD# */
	PAD_NC(GPP_D19, NONE),
	/* GPP_E5 : WWAN_WAKE# */
	PAD_NC(GPP_E5, NONE),
	/* GPP_F19 : PCIE_CLKREQ_WWAN */
	PAD_NC(GPP_F19, NONE),
	/* GPP_F21 : WWAN_OFF# */
	PAD_NC(GPP_F21, NONE),
};

static const struct pad_config nvme_disable_pads[] = {
	/* GPP_F9 : SSD_PERST_L */
	PAD_NC(GPP_F9, NONE),
	/* GPP_D5 : SSD_CLKREQ_ODL */
	PAD_NC(GPP_D5, NONE),
};

static void fw_config_handle(void *unused)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is unprovisioned. Skip disable device's pads.\n");
		return;
	}

	if (fw_config_probe(FW_CONFIG(FP, FP_ABSENT))) {
		printk(BIOS_INFO, "Disable Fingerprint GPIOs by fw_config.\n");
		gpio_configure_pads(fp_disable_pads, ARRAY_SIZE(fp_disable_pads));
	}

	if (fw_config_probe(FW_CONFIG(DB_USB, DB_1A))) {
		printk(BIOS_INFO, "Disable LTE-related GPIO pins by fw_config.\n");
		gpio_configure_pads(lte_disable_pads, ARRAY_SIZE(lte_disable_pads));
	}

	if (!fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME))) {
		printk(BIOS_INFO, "Disable NVMe GPIO pins by fw_config.\n");
		gpio_configure_pads(nvme_disable_pads, ARRAY_SIZE(nvme_disable_pads));
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
