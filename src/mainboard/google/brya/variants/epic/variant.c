/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	if (!(fw_config_probe(FW_CONFIG(DB_USB, DB_C_A_LTE)) ||
		fw_config_probe(FW_CONFIG(DB_USB, DB_A_HDMI_LTE)))) {
		printk(BIOS_INFO, "Disable usb2_port5 and usb3_port3 of WWAN.\n");

		config->usb2_ports[4] = (struct usb2_port_config) USB2_PORT_EMPTY;
		config->usb3_ports[2] = (struct usb3_port_config) USB3_PORT_EMPTY;
	}

	if (!fw_config_probe(FW_CONFIG(WFC, WFC_PRESENT))) {
		printk(BIOS_INFO, "Disable usb2_port7 of WFC.\n");

		config->usb2_ports[6] = (struct usb2_port_config) USB2_PORT_EMPTY;
	}

	if (fw_config_probe(FW_CONFIG(DB_USB, DB_A)) || fw_config_probe(FW_CONFIG(DB_USB, DB_A_HDMI_LTE))) {
		printk(BIOS_INFO, "Disable typec aux_bias_pads in the SOC.\n");
		config->typec_aux_bias_pads[1].pad_auxp_dc = 0x00;
		config->typec_aux_bias_pads[1].pad_auxn_dc = 0x00;
	}
}
