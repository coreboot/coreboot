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

struct logo_coordinates {
	uint32_t x;
	uint32_t y;
};

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

/*
 * Calculates the destination coordinates for the logo based on alignment settings.
 *
 * horizontal_resolution: The horizontal resolution of the display panel.
 * vertical_resolution: The vertical resolution of the display panel.
 * logo_width: The width of the logo bitmap.
 * logo_height: The height of the logo bitmap.
 * valignment: The vertical alignment setting.
 *
 * Returning `struct logo_coordinates` that contains the calculated x and y coordinates
 * for rendering the logo.
 */
static struct logo_coordinates calculate_logo_coordinates(
	uint32_t horizontal_resolution, uint32_t vertical_resolution,
	uint32_t logo_width, uint32_t logo_height, enum fw_splash_vertical_alignment valignment)
{
	struct logo_coordinates coords;
	/* Always horizontally centered */
	coords.x = (horizontal_resolution - logo_width) / 2;

	switch (valignment) {
	case FW_SPLASH_VALIGNMENT_MIDDLE:
		coords.y = vertical_resolution / 2;
		break;
	case FW_SPLASH_VALIGNMENT_TOP:
		coords.y = 0;
		break;
	case FW_SPLASH_VALIGNMENT_BOTTOM:
		coords.y = vertical_resolution - logo_height;
		break;
	default: /* FW_SPLASH_VALIGNMENT_CENTER (default) */
		coords.y = (vertical_resolution - logo_height) / 2;
		break;
	}

	return coords;
}

/*
 * Copies the logo to the framebuffer.
 *
 * framebuffer_base: The base address of the framebuffer.
 * bytes_per_scanline: The number of bytes per scanline in the framebuffer.
 * logo_buffer_addr: The address of the logo data in BLT format.
 * logo_width: The width of the logo bitmap.
 * logo_height: The height of the logo bitmap.
 * dest_x: The destination x-coordinate in the framebuffer for rendering the logo.
 * dest_y: The destination y-coordinate in the framebuffer for rendering the logo.
 */
static void copy_logo_to_framebuffer(
	uintptr_t framebuffer_base, uint32_t bytes_per_scanline,
	efi_uintn_t logo_buffer_addr, uint32_t logo_width, uint32_t logo_height,
	efi_uintn_t dest_x, efi_uintn_t dest_y)
{
	size_t pixel_size = sizeof(efi_graphics_output_blt_pixel);
	size_t logo_line_bytes = logo_width * pixel_size;
	efi_uintn_t framebuffer_offset = framebuffer_base + dest_y * bytes_per_scanline
					 + dest_x * pixel_size;
	uint8_t *dst_row_address = (uint8_t *)framebuffer_offset;
	uint8_t *src_row_address = (uint8_t *)(uintptr_t)logo_buffer_addr;
	for (uint32_t i = 0; i < logo_height; i++) {
		memcpy(dst_row_address, src_row_address, logo_line_bytes);
		dst_row_address += bytes_per_scanline;
		src_row_address += logo_line_bytes;
	}
}

void soc_load_logo_by_coreboot(void)
{
	const struct hob_graphics_info *ginfo;
	struct soc_intel_common_config *config = chip_get_common_soc_structure();
	uintptr_t logo_ptr;
	size_t size, logo_ptr_size;
	efi_uintn_t blt_size, blt_buffer_addr;
	uint32_t logo_height, logo_width;
	int temp_mtrr_index = -1;

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

	uintptr_t framebuffer_bar = ginfo->framebuffer_base;
	uint32_t horizontal_resolution = ginfo->horizontal_resolution;
	uint32_t vertical_resolution = ginfo->vertical_resolution;
	uint32_t bytes_per_scanline = ginfo->pixels_per_scanline *
				 sizeof(efi_graphics_output_blt_pixel);

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

	logo_ptr = (uintptr_t)bmp_load_logo(&logo_ptr_size);

	if (!logo_ptr || logo_ptr_size < sizeof(efi_bmp_image_header)) {
		printk(BIOS_ERR, "%s: BMP image (%zu) is less than expected minimum size (%zu).\n",
				 __func__, logo_ptr_size, sizeof(efi_bmp_image_header));
		return;
	}

	/* Convert BMP logo to GOP BLT format */
	fsp_convert_bmp_to_gop_blt(logo_ptr, logo_ptr_size, &blt_buffer_addr, &blt_size,
			   &logo_height, &logo_width, config->panel_orientation);

	/* Override logo alignment if the default screen orientation is not normal */
	if (config->panel_orientation != LB_FB_ORIENTATION_NORMAL)
		config->logo_valignment = FW_SPLASH_VALIGNMENT_CENTER;

	/* Calculate logo destination coordinates */
	struct logo_coordinates logo_coords = calculate_logo_coordinates(horizontal_resolution,
			 vertical_resolution, logo_width, logo_height, config->logo_valignment);

	/* Copy the logo to the framebuffer */
	copy_logo_to_framebuffer(framebuffer_bar, bytes_per_scanline, blt_buffer_addr, logo_width,
			 logo_height, logo_coords.x, logo_coords.y);

	if (CONFIG(SPLASH_SCREEN_FOOTER)) {
		bmp_release_logo();
		logo_ptr = (uintptr_t)bmp_load_logo_by_type(BOOTSPLASH_FOOTER, &logo_ptr_size);
		if (!logo_ptr || logo_ptr_size < sizeof(efi_bmp_image_header)) {
			printk(BIOS_ERR, "%s: BMP image (%zu) is less than expected minimum size (%zu).\n",
				 __func__, logo_ptr_size, sizeof(efi_bmp_image_header));
			return;
		}

		/* Convert BMP logo to GOP BLT format */
		fsp_convert_bmp_to_gop_blt(logo_ptr, logo_ptr_size, &blt_buffer_addr, &blt_size,
				   &logo_height, &logo_width, config->panel_orientation);

		logo_coords = calculate_logo_coordinates(horizontal_resolution,
				 vertical_resolution, logo_width, logo_height, FW_SPLASH_VALIGNMENT_BOTTOM);

		/* Copy the logo to the framebuffer */
		copy_logo_to_framebuffer(framebuffer_bar, bytes_per_scanline, blt_buffer_addr,
				 logo_width, logo_height, logo_coords.x, logo_coords.y);
	}

	/* Clear temporary Write Combine (WC) MTRR */
	clear_var_mtrr(temp_mtrr_index);
}
