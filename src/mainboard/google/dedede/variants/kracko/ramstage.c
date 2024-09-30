/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <fw_config.h>
#include <soc/soc_chip.h>
#include <static.h>

static void ext_vr_update(void)
{
	struct soc_intel_jasperlake_config *cfg = config_of_soc();
	if (fw_config_probe(FW_CONFIG(EXT_VR, EXT_VR_ABSENT)))
		cfg->disable_external_bypass_vr = 1;
}

static void usb_port_update(void)
{
	struct soc_intel_jasperlake_config *cfg = config_of_soc();

	if (fw_config_is_provisioned() &&
	   fw_config_probe(FW_CONFIG(DB_PORTS, DB_PORTS_NONE))) {
		/* Disable USB C1 port */
		cfg->usb2_ports[1].enable = 0;
		cfg->usb3_ports[1].enable = 0;
		/* Disable USB A1 port */
		cfg->usb2_ports[3].enable = 0;
		cfg->usb3_ports[3].enable = 0;
	}
	if (fw_config_is_provisioned() &&
	   fw_config_probe(FW_CONFIG(DB_PORTS, DB_PORTS_LTE))) {
		/* Disable USB C1 port */
		cfg->usb2_ports[1].enable = 0;
		cfg->usb3_ports[1].enable = 0;
	}
}

void variant_devtree_update(void)
{
	ext_vr_update();
	usb_port_update();
}
