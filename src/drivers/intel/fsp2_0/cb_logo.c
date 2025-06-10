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
 * Calculates the destination coordinates for the logo based on both horizontal and
 * vertical alignment settings.
 *
 * horizontal_resolution: The horizontal resolution of the display panel.
 * vertical_resolution: The vertical resolution of the display panel.
 * logo_width: The width of the logo bitmap.
 * logo_height: The height of the logo bitmap.
 * halignment: The horizontal alignment setting. Use FW_SPLASH_HALIGNMENT_NONE
 *             if only vertical alignment is specified.
 * valignment: The vertical alignment setting. Use FW_SPLASH_VALIGNMENT_NONE
 *             if only horizontal alignment is specified.
 *
 * Returning `struct logo_coordinates` that contains the calculated x and y coordinates
 * for rendering the logo.
 */
static struct logo_coordinates calculate_logo_coordinates(
	uint32_t horizontal_resolution, uint32_t vertical_resolution,
	uint32_t logo_width, uint32_t logo_height,
	enum fw_splash_vertical_alignment valignment,
	enum fw_splash_horizontal_alignment halignment)
{
	struct logo_coordinates coords;

	/* Calculate X coordinate */
	switch (halignment) {
	case FW_SPLASH_HALIGNMENT_LEFT:
		coords.x = 0;
		break;
	case FW_SPLASH_HALIGNMENT_RIGHT:
		coords.x = horizontal_resolution - logo_width;
		break;
	default: /* FW_SPLASH_HALIGNMENT_CENTER (default) */
		coords.x = (horizontal_resolution - logo_width) / 2;
		break;
	}

	/* Calculate Y coordinate */
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

/*
 * Loads, converts, and renders a BMP logo to the framebuffer.
 *
 * Returns 0 on success, -1 on failure.
 */
static int render_logo_to_framebuffer(uintptr_t framebuffer_base, uint32_t bytes_per_scanline,
	uint32_t horizontal_resolution, uint32_t vertical_resolution,
	struct soc_intel_common_config *config, enum bootsplash_type logo_type)
{
	uintptr_t logo_ptr;
	size_t logo_ptr_size;
	efi_uintn_t blt_size, blt_buffer_addr;
	uint32_t logo_height, logo_width;
	struct logo_coordinates logo_coords;

	logo_ptr = (uintptr_t)bmp_load_logo_by_type(logo_type, &logo_ptr_size);

	if (!logo_ptr || logo_ptr_size < sizeof(efi_bmp_image_header)) {
		printk(BIOS_ERR, "%s: BMP image (%zu) is less than expected minimum size (%zu).\n",
				 __func__, logo_ptr_size, sizeof(efi_bmp_image_header));
		return -1;
	}

	fsp_convert_bmp_to_gop_blt(logo_ptr, logo_ptr_size, &blt_buffer_addr, &blt_size,
				   &logo_height, &logo_width, config->panel_orientation);

	enum fw_splash_horizontal_alignment halignment = FW_SPLASH_HALIGNMENT_CENTER;
	enum fw_splash_vertical_alignment valignment = FW_SPLASH_VALIGNMENT_CENTER;
	/* Determine logo coordinates based on context (main logo vs. footer) */
	if (logo_type == BOOTSPLASH_LOW_BATTERY || logo_type == BOOTSPLASH_CENTER) {
		/* Override logo alignment if the default screen orientation is not normal */
		if (config->panel_orientation != LB_FB_ORIENTATION_NORMAL)
			config->logo_valignment = FW_SPLASH_VALIGNMENT_CENTER;

		valignment = config->logo_valignment;
	} else {
		/* Calculate logo destination coordinates */
		switch (config->panel_orientation) {
		case LB_FB_ORIENTATION_RIGHT_UP:
			halignment = FW_SPLASH_HALIGNMENT_LEFT;
			break;
		case LB_FB_ORIENTATION_LEFT_UP:
			halignment = FW_SPLASH_HALIGNMENT_RIGHT;
			break;
		case LB_FB_ORIENTATION_BOTTOM_UP:
			valignment = FW_SPLASH_VALIGNMENT_TOP;
			break;
		default: /* LB_FB_ORIENTATION_NORMAL (default) */
			valignment = FW_SPLASH_VALIGNMENT_BOTTOM;
			break;
		}
	}

	logo_coords = calculate_logo_coordinates(horizontal_resolution,
		 vertical_resolution, logo_width, logo_height, valignment, halignment);

	copy_logo_to_framebuffer(framebuffer_base, bytes_per_scanline, blt_buffer_addr,
				 logo_width, logo_height, logo_coords.x, logo_coords.y);

	bmp_release_logo();

	return 0;
}

void soc_load_logo_by_coreboot(void)
{
	const struct hob_graphics_info *ginfo;
	struct soc_intel_common_config *config = chip_get_common_soc_structure();
	int temp_mtrr_index = -1;
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

	/*
	 * If the device is in low-battery mode and the low-battery splash screen is being
	 * displayed, prevent further operation and bail out early.
	 */
	if (platform_is_low_battery_shutdown_needed()) {
		if (render_logo_to_framebuffer(framebuffer_bar, bytes_per_scanline,
				 horizontal_resolution, vertical_resolution, config,
				 BOOTSPLASH_LOW_BATTERY) != 0) {
			printk(BIOS_ERR, "%s: Failed to render low-battery logo.\n", __func__);
		}
		goto cleanup;
	}

	/* Render the main logo */
	if (render_logo_to_framebuffer(framebuffer_bar, bytes_per_scanline,
				 horizontal_resolution, vertical_resolution, config,
				 BOOTSPLASH_CENTER) != 0) {
		printk(BIOS_ERR, "%s: Failed to render main splash screen logo.\n", __func__);
	} else if (CONFIG(SPLASH_SCREEN_FOOTER)) {
		/* Render the footer logo */
		if (render_logo_to_framebuffer(framebuffer_bar, bytes_per_scanline,
				 horizontal_resolution, vertical_resolution, config,
				 BOOTSPLASH_FOOTER) != 0)
			printk(BIOS_ERR, "%s: Failed to render footer logo.\n", __func__);
	}

cleanup:
	/* Clear temporary Write Combine (WC) MTRR */
	clear_var_mtrr(temp_mtrr_index);
}
