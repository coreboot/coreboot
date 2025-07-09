/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boot/coreboot_tables.h>
#include <bootsplash.h>
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/fsp_gop_blt.h>
#include <stdlib.h>

/* Convert a *.BMP graphics image to a GOP blt buffer */
void fsp_load_and_convert_bmp_to_gop_blt(efi_uintn_t *logo, uint32_t *logo_size,
	efi_uintn_t *blt_ptr, efi_uintn_t *blt_size, uint32_t *pixel_height, uint32_t *pixel_width,
	enum lb_fb_orientation orientation)
{
	load_and_convert_bmp_to_blt((uintptr_t *)logo, (size_t *)logo_size, (uintptr_t *)blt_ptr,
				(size_t *)blt_size, pixel_height, pixel_width, orientation);
}

/* Convert a *.BMP graphics image (as per input `logo_ptr`) to a GOP blt buffer */
void fsp_convert_bmp_to_gop_blt(uintptr_t logo_ptr, size_t logo_ptr_size,
	efi_uintn_t *blt_ptr, efi_uintn_t *blt_size, uint32_t *pixel_height, uint32_t *pixel_width,
	enum lb_fb_orientation orientation)
{
	convert_bmp_to_blt(logo_ptr, logo_ptr_size, (uintptr_t *)blt_ptr, (size_t *)blt_size,
				pixel_height, pixel_width, orientation);
}
