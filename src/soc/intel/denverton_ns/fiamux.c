/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 - 2017 Intel Corporation
 * Copyright (C) 2017 Online SAS
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
 */

#include <compiler.h>
#include <console/console.h>
#include <soc/fiamux.h>

#define FSP_FIAMUX_HOB_GUID                                            \
	{                                                              \
		0x26ad492e, 0xf951, 0x4e43,                            \
		{                                                      \
			0xbc, 0x72, 0x22, 0x76, 0x58, 0xb1, 0xf6, 0x23 \
		}                                                      \
	}

int get_fiamux_hsio_info(uint16_t num_of_lanes, size_t num_of_entry,
				BL_HSIO_INFORMATION **config)
{
	uint8_t entry;

	if ((num_of_lanes == 0) || (num_of_entry == 0) || (*config == NULL))
		return 1;

	for (entry = 0; entry < num_of_entry; entry++) {
		if ((*config)[entry].NumLanesSupported == num_of_lanes) {
			*config = &(*config)[entry];
			return 0;
		}
	}

	return 1;
}

/*
* Display the FIA MUX HOB.
*
*   @param   Pointer of FIA MUX HOB data
*
*   @return  None
*
*/
void print_fiamux_config_hob(BL_FIA_MUX_CONFIG_HOB *fiamux_hob_data)
{
	/* Display the FIA MUX Configuration */
	printk(BIOS_DEBUG, "FIA MUX Configuration in FSP HOB is:\n");

	printk(BIOS_DEBUG, "    FiaMuxConfig.SkuNumLanesAllowed = 0x%x\n",
	       (uint32_t)(fiamux_hob_data->FiaMuxConfig.SkuNumLanesAllowed));

	printk(BIOS_DEBUG, "    FiaMuxConfig.FiaMuxConfig = 0x%llx\n",
	       (uint64_t)(fiamux_hob_data->FiaMuxConfig.FiaMuxConfig
				  .MuxConfiguration.MeFiaMuxLaneConfig));

	printk(BIOS_DEBUG,
	       "    FiaMuxConfig.FiaMuxConfig.SataLaneConfiguration = 0x%llx\n",
	       (uint64_t)(fiamux_hob_data->FiaMuxConfig.FiaMuxConfig
				  .SataLaneConfiguration.MeFiaSataLaneConfig));

	printk(BIOS_DEBUG, "    FiaMuxConfig.FiaMuxConfig."
			   "PcieRootPortsConfiguration = 0x%llx\n",
	       (uint64_t)(fiamux_hob_data->FiaMuxConfig.FiaMuxConfig
				  .PcieRootPortsConfiguration
				  .MeFiaPcieRootPortsConfig));

	printk(BIOS_DEBUG, "    FiaMuxConfig.FiaMuxConfigRequest = 0x%llx\n",
	       (uint64_t)(fiamux_hob_data->FiaMuxConfig.FiaMuxConfigRequest
				  .MuxConfiguration.MeFiaMuxLaneConfig));

	printk(BIOS_DEBUG, "    FiaMuxConfig.FiaMuxConfigRequest."
			   "SataLaneConfiguration = 0x%llx\n",
	       (uint64_t)(fiamux_hob_data->FiaMuxConfig.FiaMuxConfigRequest
				  .SataLaneConfiguration.MeFiaSataLaneConfig));

	printk(BIOS_DEBUG, "    FiaMuxConfig.FiaMuxConfigRequest."
			   "PcieRootPortsConfiguration = 0x%llx\n",
	       (uint64_t)(fiamux_hob_data->FiaMuxConfig.FiaMuxConfigRequest
				  .PcieRootPortsConfiguration
				  .MeFiaPcieRootPortsConfig));
	printk(BIOS_DEBUG,
	       "    FiaMuxConfigStatus.FiaMuxConfigGetStatus = 0x%x\n",
	       (uint32_t)(fiamux_hob_data->FiaMuxConfigStatus
				  .FiaMuxConfigGetStatus));

	printk(BIOS_DEBUG,
	       "    FiaMuxConfigStatus.FiaMuxConfigSetStatus = 0x%x\n",
	       (uint32_t)(fiamux_hob_data->FiaMuxConfigStatus
				  .FiaMuxConfigSetStatus));

	printk(BIOS_DEBUG,
	       "    FiaMuxConfigStatus.FiaMuxConfigSetRequired = 0x%x\n",
	       (uint8_t)(fiamux_hob_data->FiaMuxConfigStatus
				 .FiaMuxConfigSetRequired));
}

/*
* Get the pointer of FIA MUX HOB data
*
*   @param   Pointer of FIA MUX HOB data
*
*   @return:
*     Non-zero - FIA MUX configuration correct.
*     Zero - Either FIA MUX configuration incorrect or
*     it can not be verified.
*/
BL_FIA_MUX_CONFIG_HOB *get_fiamux_hob_data(void)
{
	u32 const *fiamux_hob = NULL;
	BL_FIA_MUX_CONFIG_HOB *fiamux_hob_data = NULL;
	const EFI_GUID fiamux_guid = FSP_FIAMUX_HOB_GUID;
	size_t size;

	/* Parse FIA MUX configuration HOB */
	fiamux_hob = fsp_find_extension_hob_by_guid(
		(uint8_t const *)&fiamux_guid, &size);

	if (fiamux_hob == NULL) {
		/* FIA MUX configuration HOB not exist */
		die("FIA MUX Configuration Data Hob does not present!\n");
	} else {
		/* Get FIA MUX configuration HOB */
		fiamux_hob_data = (BL_FIA_MUX_CONFIG_HOB *)(fiamux_hob);

		/* Display FIA MUX configuration HOB */
		print_fiamux_config_hob(fiamux_hob_data);
	}

	return fiamux_hob_data;
}

__weak size_t mainboard_get_hsio_config(BL_HSIO_INFORMATION **p_hsio_config)
{
	*p_hsio_config = NULL;
	return 0;
}
