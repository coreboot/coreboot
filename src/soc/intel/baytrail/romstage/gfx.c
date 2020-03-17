/*
 * This file is part of the coreboot project.
 *
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

#include <device/pci_ops.h>
#include <soc/gfx.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>

void gfx_init(void)
{
	uint32_t ggc;
	uint8_t msac;
	const unsigned int gfx_dev = PCI_DEV(0, GFX_DEV, GFX_FUNC);

	/* The GFX device needs to set the aperture, gtt stolen size, and
	 * graphics stolen memory stolen size before running MRC. For now
	 * just hard code the defaults. Options can be added to the device
	 * tree if needed. */

	ggc = pci_read_config32(gfx_dev, GGC);
	msac = pci_read_config8(gfx_dev, MSAC);

	ggc &= ~(GGC_GTT_SIZE_MASK | GGC_GSM_SIZE_MASK);
	/* 32MB GSM is not supported with <C0 stepping. */
	ggc |= GGC_GTT_SIZE_2MB | GGC_GSM_SIZE_64MB;
	/* Enable VGA decoding as well. */
	ggc &= ~(GGC_VGA_DISABLE);

	msac &= ~(APERTURE_SIZE_MASK);
	msac |= APERTURE_SIZE_256MB;

	pci_write_config32(gfx_dev, GGC, ggc);
	pci_write_config8(gfx_dev, MSAC, msac);
}
