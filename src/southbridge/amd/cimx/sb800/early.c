/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
#include <device/pci_ids.h>
#include <arch/io.h>		/* inl, outl */
#include <arch/acpi.h>
#include "SBPLATFORM.h"
#include "sb_cimx.h"
#include "cfg.h"		/*sb800_cimx_config*/
#include "cbmem.h"

#if CONFIG_RAMINIT_SYSINFO
/**
 * @brief Get SouthBridge device number
 * @param[in] bus target bus number
 * @return southbridge device number
 */
u32 get_sbdn(u32 bus)
{
	device_t dev;

	printk(BIOS_DEBUG, "SB800 - %s - %s - Start.\n", __FILE__, __func__);
	//dev = PCI_DEV(bus, 0x14, 0);
	dev = pci_locate_device_on_bus(
			PCI_ID(PCI_VENDOR_ID_ATI, PCI_DEVICE_ID_ATI_SB800_SM),
			bus);

	printk(BIOS_DEBUG, "SB800 - %s - %s - End.\n", __FILE__, __func__);
	return (dev >> 15) & 0x1f;
}
#endif


/**
 * @brief South Bridge CIMx romstage entry,
 *        wrapper of sbPowerOnInit entry point.
 */
void sb_Poweron_Init(void)
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

/**
 * CIMX not set the clock to 48Mhz until sbBeforePciInit,
 * coreboot may need to set this even more earlier
 */
void sb800_clk_output_48Mhz(void)
{
	/* AcpiMMioDecodeEn */
	RWPMIO(SB_PMIOA_REG24, AccWidthUint8, ~(BIT0 + BIT1), BIT0);

        *(volatile u32 *)(ACPI_MMIO_BASE + MISC_BASE + 0x40) &= ~((1 << 0) | (1 << 2)); /* 48Mhz */
        *(volatile u32 *)(ACPI_MMIO_BASE + MISC_BASE + 0x40) |= 1 << 1; /* 48Mhz */
}

#if CONFIG_HAVE_ACPI_RESUME
int acpi_is_wakeup_early(void)
{
	return (acpi_get_sleep_type() == 3);
}
#endif
