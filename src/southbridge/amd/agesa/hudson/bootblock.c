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
 */

#include <stdint.h>
#include <device/pci_ops.h>

/*
 * Enable 4MB (LPC) ROM access at 0xFFC00000 - 0xFFFFFFFF.
 *
 * Hardware should enable LPC ROM by pin straps. This function does not
 * handle the theoretically possible PCI ROM, FWH, or SPI ROM configurations.
 *
 * The HUDSON power-on default is to map 512K ROM space.
 *
 */
static void hudson_enable_rom(void)
{
	u8 reg8;
	pci_devfn_t dev;

	dev = PCI_DEV(0, 0x14, 3);

	/* Decode variable LPC ROM address ranges 1 and 2. */
	reg8 = pci_io_read_config8(dev, 0x48);
	reg8 |= (1 << 3) | (1 << 4);
	pci_io_write_config8(dev, 0x48, reg8);

	/* LPC ROM address range 1: */
	/* Enable LPC ROM range mirroring start at 0x000e(0000). */
	pci_io_write_config16(dev, 0x68, 0x000e);
	/* Enable LPC ROM range mirroring end at 0x000f(ffff). */
	pci_io_write_config16(dev, 0x6a, 0x000f);

	/* LPC ROM address range 2: */
	/*
	 * Enable LPC ROM range start at:
	 * 0xfff8(0000): 512KB
	 * 0xfff0(0000): 1MB
	 * 0xffe0(0000): 2MB
	 * 0xffc0(0000): 4MB
	 */
	pci_io_write_config16(dev, 0x6c, 0x10000 - (CONFIG_COREBOOT_ROMSIZE_KB >> 6));
	/* Enable LPC ROM range end at 0xffff(ffff). */
	pci_io_write_config16(dev, 0x6e, 0xffff);
}

static void bootblock_southbridge_init(void)
{
	hudson_enable_rom();
}


#if !CONFIG(ROMCC_BOOTBLOCK)

#include <bootblock_common.h>
#include <amdblocks/acpimmio.h>
#include <southbridge/amd/agesa/hudson/hudson.h>

void bootblock_soc_early_init(void)
{
	pci_devfn_t dev;
	u32 data;

	bootblock_southbridge_init();
	hudson_lpc_decode();
	enable_acpimmio_decode_pm24();

	dev = PCI_DEV(0, 0x14, 3);
	data = pci_read_config32(dev, LPC_IO_OR_MEM_DECODE_ENABLE);
	/* enable 0x2e/0x4e IO decoding for SuperIO */
	pci_write_config32(dev, LPC_IO_OR_MEM_DECODE_ENABLE, data | 3);

	/*
	 * Enable FCH to decode TPM associated Memory and IO regions for vboot
	 *
	 * Enable decoding of TPM cycles defined in TPM 1.2 spec
	 * Enable decoding of legacy TPM addresses: IO addresses 0x7f-
	 * 0x7e and 0xef-0xee.
	 */

	data = pci_read_config32(dev, LPC_TRUSTED_PLATFORM_MODULE);
	data |= TPM_12_EN | TPM_LEGACY_EN;
	pci_write_config32(dev, LPC_TRUSTED_PLATFORM_MODULE, data);

	/*
	 *  In Hudson RRG, PMIOxD2[5:4] is "Drive strength control for
	 *  LpcClk[1:0]".  This following register setting has been
	 *  replicated in every reference design since Parmer, so it is
	 *  believed to be required even though it is not documented in
	 *  the SoC BKDGs.  Without this setting, there is no serial
	 *  output.
	 */
	pm_write8(0xd2, 0);
}
#endif
