/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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


//#include <config.h>
#include <stdint.h>
#include <device/pci_ids.h>
#include <arch/io.h>		/* inl, outl */
#include <arch/romcc_io.h>	/* device_t */
#include "SBPLATFORM.h"
#include "SbEarly.h"
#include "sb800_cfg.h"		/*sb800_cimx_config*/


/**
 * @brief Get SouthBridge device number
 * @param[in] bus target bus number
 * @return southbridge device number
 */
u32 get_sbdn(u32 bus)
{
	device_t dev;

	//dev = PCI_DEV(bus, 0x14, 0);
	dev = pci_locate_device_on_bus(
			PCI_ID(PCI_VENDOR_ID_ATI, PCI_DEVICE_ID_ATI_SB800_SM),
			bus);

	return (dev >> 15) & 0x1f;
}


/**
 * @brief South Bridge CIMx romstage entry,
 *        wrapper of sbPowerOnInit entry point.
 */
void sb_poweron_init(void)
{
	AMDSBCFG sb_early_cfg;

	sb800_cimx_config(&sb_early_cfg);
	//sb_early_cfg.StdHeader.Func = SB_POWERON_INIT;
	//AmdSbDispatcher(&sb_early_cfg);
	//TODO
	//AMD_IMAGE_HEADER was missing, when using AmdSbDispatcher,
	// VerifyImage() will fail, LocateImage() take minitues to find the image.
	sbPowerOnInit(&sb_early_cfg);
}
