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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include "NbPlatform.h"
#include "rd890_cfg.h"
#include "nb_cimx.h"


/**
 * @brief disable GPP1 Port0,1, GPP2, GPP3a Port0,1,2,3,4,5, GPP3b
 *
 * SR5650/5670/5690 RD890 chipset, read pci config space hang at POR,
 * Disable all Pcie Bridges to work around It.
 */
void sr56x0_rd890_disable_pcie_bridge(void)
{
	u32			nb_dev;
	u32			mask;
	u32			val;
	AMD_NB_CONFIG_BLOCK	cfg_block;
	AMD_NB_CONFIG_BLOCK	*cfg_ptr = &cfg_block;
	AMD_NB_CONFIG		*nb_cfg  = &(cfg_block.Northbridges[0]);

        nb_cfg->ConfigPtr = &cfg_ptr;
	nb_dev = MAKE_SBDFO(0, 0x0, 0x0, 0x0, 0x0);
	val = (1 << 2) | (1 << 3); /*GPP1*/
	val |= (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 16) | (1 << 17); /*GPP3a*/
	val |= (1 << 18) | (1 << 19); /*GPP2*/
	val |= (1 << 20); /*GPP3b*/
	mask = ~val;
	LibNbPciIndexRMW(nb_dev | NB_MISC_INDEX, NB_MISC_REG0C,
			AccessS3SaveWidth32,
			mask,
			val,
			nb_cfg);
}


/**
 * @brief South Bridge CIMx romstage entry,
 *  wrapper of AmdPowerOnResetInit entry point.
 */
void nb_Poweron_Init(void)
{
	NB_CONFIG nb_cfg[MAX_NB_COUNT];
	HT_CONFIG ht_cfg[MAX_NB_COUNT];
	PCIE_CONFIG pcie_cfg[MAX_NB_COUNT];
	AMD_NB_CONFIG_BLOCK gConfig;
	AMD_NB_CONFIG_BLOCK *ConfigPtr = &gConfig;
	AGESA_STATUS status;

	printk(BIOS_DEBUG, "cimx/rd890 early.c %s() Start\n", __func__);
	CIMX_INIT_TRACE();
	CIMX_TRACE((BIOS_DEBUG, "NbPowerOnResetInit entry\n"));
	rd890_cimx_config(&gConfig, &nb_cfg[0], &ht_cfg[0], &pcie_cfg[0]);

	if (ConfigPtr->StandardHeader.CalloutPtr != NULL) {
		ConfigPtr->StandardHeader.CalloutPtr(CB_AmdSetNbPorConfig, 0, &gConfig);
	}

	status = AmdPowerOnResetInit(&gConfig);
	printk(BIOS_DEBUG, "cimx/rd890 early.c %s() End. return status=%x\n", __func__, status);
}

/**
 * @brief South Bridge CIMx romstage entry,
 *  wrapper of AmdHtInit entry point.
 */
void nb_Ht_Init(void)
{
	AGESA_STATUS status;
	NB_CONFIG nb_cfg[MAX_NB_COUNT];
	HT_CONFIG ht_cfg[MAX_NB_COUNT];
	PCIE_CONFIG pcie_cfg[MAX_NB_COUNT];
	AMD_NB_CONFIG_BLOCK gConfig;
	AMD_NB_CONFIG_BLOCK *ConfigPtr = &gConfig;
	u32 i;

	rd890_cimx_config(&gConfig, &nb_cfg[0], &ht_cfg[0], &pcie_cfg[0]);

	//Initialize HT structure
	LibSystemApiCall(AmdHtInitializer, &gConfig);
	for (i = 0; i < MAX_NB_COUNT; i ++) {
		if (ConfigPtr->StandardHeader.CalloutPtr != NULL) {
			ConfigPtr->StandardHeader.CalloutPtr(CB_AmdSetHtConfig, 0, (VOID*)&(gConfig.Northbridges[i]));
		}
	}

	status = LibSystemApiCall(AmdHtInit, &gConfig);
	printk(BIOS_DEBUG, "AmdHtInit status: %x\n", status);
}

void nb_S3_Init(void)
{
	//TODO
}
