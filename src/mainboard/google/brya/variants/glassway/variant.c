/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <fw_config.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI_SAR_ID));
}

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	if (fw_config_probe(FW_CONFIG(DB_USB, DB_HDMI_LTE))) {
		printk(BIOS_INFO, "Enable DDI PORT 2 for HPD and DDC.\n");
		config->ddi_ports_config[DDI_PORT_2] = DDI_ENABLE_HPD | DDI_ENABLE_DDC;
	}

	if (!fw_config_probe(FW_CONFIG(WFC, WFC_PRESENT))) {
		printk(BIOS_INFO, "Disable usb2_port7 of WFC.\n");
		config->usb2_ports[6].enable = 0;
		config->usb2_ports[6].ocpin = OC_SKIP;
		config->usb2_ports[6].tx_bias = USB2_BIAS_0MV;
		config->usb2_ports[6].tx_emp_enable = USB2_EMP_OFF;
		config->usb2_ports[6].pre_emp_bias = USB2_BIAS_0MV;
		config->usb2_ports[6].pre_emp_bit = USB2_HALF_BIT_PRE_EMP;
	}
}
