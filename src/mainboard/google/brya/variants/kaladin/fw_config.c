#include <fw_config.h>
#include <baseboard/variants.h>

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	if (fw_config_probe(FW_CONFIG(MB_USB, MB_USB_2C_1A))) {
		config->tcss_ports[1] = (struct tcss_port_config) TCSS_PORT_DEFAULT(OC_SKIP);
	}
}
