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

		config->usb2_ports[4].enable = 0;
		config->usb2_ports[4].ocpin = OC_SKIP;
		config->usb2_ports[4].tx_bias = USB2_BIAS_0MV;
		config->usb2_ports[4].tx_emp_enable = USB2_EMP_OFF;
		config->usb2_ports[4].pre_emp_bias = USB2_BIAS_0MV;
		config->usb2_ports[4].pre_emp_bit = USB2_HALF_BIT_PRE_EMP;

		config->usb3_ports[2].enable = 0;
		config->usb3_ports[2].ocpin = OC_SKIP;
		config->usb3_ports[2].tx_de_emp = 0x00;
		config->usb3_ports[2].tx_downscale_amp = 0x00;
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

	if (fw_config_probe(FW_CONFIG(DB_USB, DB_A_HDMI_LTE))) {
		printk(BIOS_INFO, "Disable typec aux_bias_pads in the SOC.\n");
		config->typec_aux_bias_pads[1].pad_auxp_dc = 0x00;
		config->typec_aux_bias_pads[1].pad_auxn_dc = 0x00;
	}
}
