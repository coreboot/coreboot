/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016-2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <chip.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <romstage_handoff.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <string.h>

void soc_init_pre_device(void *chip_info)
{
	/* Perform silicon specific init. */
	fsp_silicon_init(romstage_handoff_is_resume());
}

static void pci_domain_set_resources(device_t dev)
{
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources   = &pci_domain_read_resources,
	.set_resources    = &pci_domain_set_resources,
	.scan_bus         = &pci_domain_scan_bus,
	.ops_pci_bus      = &pci_bus_default_ops,
};

static struct device_operations cpu_bus_ops = {
	.read_resources   = DEVICE_NOOP,
	.set_resources    = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init             = DEVICE_NOOP,
};

static void soc_enable(device_t dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
}

struct chip_operations soc_intel_cannonlake_ops = {
	CHIP_NAME("Intel Cannonlake")
	.enable_dev	= &soc_enable,
	.init		= &soc_init_pre_device,
};

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	int i;
	FSP_S_CONFIG *params = &supd->FspsConfig;
	const struct device *dev = SA_DEV_ROOT;
	const config_t *config = dev->chip_info;

	/* Set USB OC pin to 0 first */
	for (i = 0; i < ARRAY_SIZE(params->Usb2OverCurrentPin); i++) {
		params->Usb2OverCurrentPin[i] = 0;
	}

	for (i = 0; i < ARRAY_SIZE(params->Usb3OverCurrentPin); i++) {
		params->Usb3OverCurrentPin[i] = 0;
	}

	mainboard_silicon_init_params(params);

	/* SATA */
	params->SataEnable = config->SataEnable;
	params->SataMode = config->SataMode;
	params->SataSalpSupport = config->SataSalpSupport;
	memcpy(params->SataPortsEnable, config->SataPortsEnable,
			sizeof(params->SataPortsEnable));
	memcpy(params->SataPortsDevSlp, config->SataPortsDevSlp,
			sizeof(params->SataPortsDevSlp));

	/* Lan */
	params->PchLanEnable = config->PchLanEnable;

	/* Audio */
	params->PchHdaDspEnable = config->PchHdaDspEnable;
	params->PchHdaAudioLinkHda = config->PchHdaAudioLinkHda;

	/* USB */
	for (i = 0; i < ARRAY_SIZE(config->usb2_ports); i++) {
		params->PortUsb20Enable[i] =
			config->usb2_ports[i].enable;
		params->Usb2OverCurrentPin[i] =
			config->usb2_ports[i].ocpin;
		params->Usb2AfePetxiset[i] =
			config->usb2_ports[i].pre_emp_bias;
		params->Usb2AfeTxiset[i] =
			config->usb2_ports[i].tx_bias;
		params->Usb2AfePredeemp[i] =
			config->usb2_ports[i].tx_emp_enable;
		params->Usb2AfePehalfbit[i] =
			config->usb2_ports[i].pre_emp_bit;
	}

	for (i = 0; i < ARRAY_SIZE(config->usb3_ports); i++) {
		params->PortUsb30Enable[i] = config->usb3_ports[i].enable;
		params->Usb3OverCurrentPin[i] = config->usb3_ports[i].ocpin;
		if (config->usb3_ports[i].tx_de_emp) {
			params->Usb3HsioTxDeEmphEnable[i] = 1;
			params->Usb3HsioTxDeEmph[i] =
				config->usb3_ports[i].tx_de_emp;
		}
		if (config->usb3_ports[i].tx_downscale_amp) {
			params->Usb3HsioTxDownscaleAmpEnable[i] = 1;
			params->Usb3HsioTxDownscaleAmp[i] =
				config->usb3_ports[i].tx_downscale_amp;
		}
	}

	params->XdciEnable = config->XdciEnable;

	/* eMMC and SD */
	params->ScsEmmcEnabled = config->ScsEmmcEnabled;
	params->ScsEmmcHs400Enabled = config->ScsEmmcHs400Enabled;
	params->ScsSdCardEnabled = config->ScsSdCardEnabled;
	params->ScsUfsEnabled = config->ScsUfsEnabled;

	params->Heci3Enabled = config->Heci3Enabled;
	params->Device4Enable = config->Device4Enable;
	params->SkipMpInit = config->FspSkipMpInit;

	/* VrConfig Settings for 5 domains
	 * 0 = System Agent, 1 = IA Core, 2 = Ring,
	 * 3 = GT unsliced,  4 = GT sliced */
	for (i = 0; i < ARRAY_SIZE(config->domain_vr_config); i++)
		fill_vr_domain_config(params, i, &config->domain_vr_config[i]);
}

/* Mainboard GPIO Configuration */
__attribute__((weak)) void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
