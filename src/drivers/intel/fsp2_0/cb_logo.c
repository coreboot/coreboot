/* SPDX-License-Identifier: GPL-2.0-or-later */

#define __SIMPLE_DEVICE__

#include <bootmode.h>
#include <bootsplash.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <device/pci_ops.h>
#include <fsp/api.h>
#include <fsp/fsp_gop_blt.h>
#include <fsp/graphics.h>
#include <fsp/util.h>
#include <intelblocks/graphics.h>
#include <soc/iomap.h>
#include <soc/soc_chip.h>
#include <stdlib.h>
#include <string.h>

/*
 * Programs the Local Memory BAR (LMEMBAR) for the IGD.
 *
 * This function disables PCI command bits related to I/O, memory, and bus mastering
 * for the IGD, programs the LMEMBAR with the provided base address, and then
 * re-enables the PCI command bits.
 */
static void program_igd_lmembar(uint32_t base)
{
	const uint16_t disable_mask = ~(PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);
	const uint16_t enable_mask = (PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

	/* Disable response in IO, MMIO space and Bus Master. */
	pci_and_config16(SA_DEV_IGD, PCI_COMMAND, disable_mask);

	/* Program IGD Base Address Register 2 aka LMEMBAR */
	pci_write_config32(SA_DEV_IGD, PCI_BASE_ADDRESS_2, base);

	/* Enable response in IO, MMIO space and Bus Master. */
	pci_or_config16(SA_DEV_IGD, PCI_COMMAND, enable_mask);
}

void soc_load_logo_by_coreboot(void)
{
	const struct hob_graphics_info *ginfo;
	struct soc_intel_common_config *config = chip_get_common_soc_structure();
	int temp_mtrr_index = -1;
	struct logo_config logo_cfg;
	size_t size;

	/* Find the graphics information HOB */
	ginfo = fsp_find_extension_hob_by_guid(fsp_graphics_info_guid, &size);
	if (!ginfo || ginfo->framebuffer_base == 0) {
		printk(BIOS_ERR, "Graphics information HOB not found or invalid framebuffer base.\n");
		return;
	}

	/* Program the IGD LMEMBAR */
	program_igd_lmembar(GMADR_BASE);

	/* Set up a temporary Write Combine (WC) MTRR for the GMADR range */
	temp_mtrr_index = acquire_and_configure_mtrr(GMADR_BASE, GMADR_SIZE, MTRR_TYPE_WRCOMB);
	if (temp_mtrr_index < 0) {
		printk(BIOS_ERR, "Failed to configure WC MTRR for GMADR.\n");
		return;
	}

	/*
	 * Adjusts panel orientation for external display when the lid is closed.
	 *
	 * When the lid is closed, indicating the onboard display is inactive,
	 * below logic forces the panel orientation to normal. This ensures proper display
	 * on an external monitor, as rotated orientations are typically not suitable in
	 * such state.
	 */
	if (CONFIG(VBOOT_LID_SWITCH) ? !get_lid_switch() : !CONFIG(RUN_FSP_GOP))
		config->panel_orientation = LB_FB_ORIENTATION_NORMAL;


	memset((void *)&logo_cfg, 0, sizeof(logo_cfg));
	logo_cfg.framebuffer_base = ginfo->framebuffer_base;
	logo_cfg.horizontal_resolution = ginfo->horizontal_resolution;
	logo_cfg.vertical_resolution = ginfo->vertical_resolution;
	logo_cfg.bytes_per_scanline = ginfo->pixels_per_scanline *
				 sizeof(efi_graphics_output_blt_pixel);
	logo_cfg.panel_orientation = config->panel_orientation;
	logo_cfg.halignment = FW_SPLASH_HALIGNMENT_CENTER;
	logo_cfg.valignment = config->logo_valignment;
	logo_cfg.logo_bottom_margin = config->logo_bottom_margin;

	render_logo_to_framebuffer(&logo_cfg);

	/* Clear temporary Write Combine (WC) MTRR */
	clear_var_mtrr(temp_mtrr_index);
}
