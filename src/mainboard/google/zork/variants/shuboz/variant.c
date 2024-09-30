/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/amd/picasso/chip.h>
#include <soc/pci_devs.h>
#include <static.h>

uint32_t usb_oc_map_override[USB_PORT_COUNT] = {
	USB_OC_PIN_0,
	USB_OC_PIN_0,
	USB_OC_NONE, //for camera
	USB_OC_PIN_1,
	USB_OC_NONE,
	USB_OC_NONE
};

static const fsp_ddi_descriptor hdmi_ddi_descriptors[] = {
	{ // DDI0, DP0, eDP
		.connector_type = EDP,
		.aux_index = AUX1,
		.hdp_index = HDP1
	},
	{ // DDI1, DP1, DB OPT2 USB-C1 / DB OPT3 MST hub
		.connector_type = DP,
		.aux_index = AUX2,
		.hdp_index = HDP2
	},
	{ // DDI2, DP3, USB-C0
		.connector_type = DP,
		.aux_index = AUX4,
		.hdp_index = HDP4,
	}
};

void variant_get_dxio_ddi_descriptors(const fsp_dxio_descriptor **dxio_descs,
				      size_t *dxio_num,
				      const fsp_ddi_descriptor **ddi_descs,
				      size_t *ddi_num)
{
	*dxio_descs = baseboard_get_dxio_descriptors(dxio_num);

	*ddi_descs = &hdmi_ddi_descriptors[0];
	*ddi_num = ARRAY_SIZE(hdmi_ddi_descriptors);
}

void variant_devtree_update(void)
{
	struct soc_amd_picasso_config *soc_cfg;
	soc_cfg = config_of_soc();
	uint8_t i;

	/*
	  For convertible SKU, it will support additional camera on USB1,
	  so we need the different OC map to support.
	*/
	if (variant_is_convertible()) {
		for (i = 0; i < USB_PORT_COUNT; i++)
			soc_cfg->usb_port_overcurrent_pin[i] = usb_oc_map_override[i];
	}
}
