/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "NbPlatform.h"
#include "rd890_cfg.h"
#include "northbridge/amd/cimx/rd890/chip.h"
#include "nbInitializer.h"
#include <string.h>
#include <arch/ioapic.h>
#include <device/device.h>

extern ROMSTAGE_CONST struct device dev_root;

/**
 * Platform dependent configuration at ramstage
 */
void nb_pcie_config(AMD_NB_CONFIG_BLOCK *pConfig)
{
	u16 i;
	struct device *pdev;
	AMD_NB_CONFIG *NbConfigPtr = &(pConfig->Northbridges[0]);
	PCIE_CONFIG *pPcieConfig = NbConfigPtr->pPcieConfig;
	struct northbridge_amd_cimx_rd890_config *rd890_info = NULL;
	DEFAULT_PLATFORM_CONFIG(platform_config);

	/* find rd890 in device list */
	for (pdev = &dev_root; pdev; pdev=pdev->next) {
		if ((pdev->path.type == DEVICE_PATH_PCI) && (pdev->path.pci.devfn == 0))
			break;
	}
	rd890_info = pdev->chip_info;

	/* update the platform depentent configuration by devicetree */
	platform_config.PortEnableMap = rd890_info->port_enable;
	if (rd890_info->gpp1_configuration == 0) {
		platform_config.Gpp1Config = GFX_CONFIG_AAAA;
	} else if (rd890_info->gpp1_configuration == 1) {
		platform_config.Gpp1Config = GFX_CONFIG_AABB;
	}
	if (rd890_info->gpp2_configuration == 0) {
		platform_config.Gpp2Config = GFX_CONFIG_AAAA;
	} else if (rd890_info->gpp2_configuration == 1) {
		platform_config.Gpp2Config = GFX_CONFIG_AABB;
	}
	platform_config.Gpp3aConfig = rd890_info->gpp3a_configuration;

	if (platform_config.Gpp1Config != 0) {
		pPcieConfig->CoreConfiguration[0] = platform_config.Gpp1Config;
	}
	if (platform_config.Gpp2Config != 0) {
		pPcieConfig->CoreConfiguration[1] = platform_config.Gpp2Config;
	}
	if (platform_config.Gpp3aConfig != 0) {
		pPcieConfig->CoreConfiguration[2] = platform_config.Gpp3aConfig;
	}

	pPcieConfig->TempMmioBaseAddress = (UINT16)(platform_config.TemporaryMmio >> 20);
	for (i = 0; i <= MAX_CORE_ID; i++) {
		NbConfigPtr->pPcieConfig->CoreSetting[i].SkipConfiguration = OFF;
		NbConfigPtr->pPcieConfig->CoreSetting[i].PerformanceMode = OFF;
	}
	for (i = MIN_PORT_ID; i <= MAX_PORT_ID; i++) {
		NbConfigPtr->pPcieConfig->PortConfiguration[i].PortLinkMode = PcieLinkModeGen2;
	}

	for (i = MIN_PORT_ID; i <= MAX_PORT_ID; i++) {
		if ((platform_config.PortEnableMap & (1 << i)) != 0) {
			pPcieConfig->PortConfiguration[i].PortPresent = ON;
			if ((platform_config.PortGen1Map & (1 << i)) != 0) {
				pPcieConfig->PortConfiguration[i].PortLinkMode = PcieLinkModeGen1;
			}
			if ((platform_config.PortHotplugMap & (1 << i)) != 0) {
				u16 j;
				pPcieConfig->PortConfiguration[j].PortHotplug = ON; /* Enable Hotplug */
				/* Set Hotplug descriptor info */
				for (j = 0; j < 8; j++) {
					u32 PortDescriptor;
					PortDescriptor = platform_config.PortHotplugDescriptors[j];
					if ((PortDescriptor & 0xF) == j) {
						pPcieConfig->ExtPortConfiguration[j].PortHotplugDevMap  = (PortDescriptor >> 4)  & 3;
						pPcieConfig->ExtPortConfiguration[j].PortHotplugByteMap = (PortDescriptor >> 6)  & 1;
						break;
					}
				}
			}
		}
	}
}

