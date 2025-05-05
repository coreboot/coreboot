/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef FSP_GOP_BLT_H
#define FSP_GOP_BLT_H

#include <boot/coreboot_tables.h>
#include <efi/efi_datatype.h>
#include <types.h>

/* Convert a *.BMP graphics image to a GOP blt buffer */
void fsp_convert_bmp_to_gop_blt(efi_uintn_t *logo, uint32_t *logo_size,
	efi_uintn_t *blt_ptr, efi_uintn_t *blt_size, uint32_t *pixel_height, uint32_t *pixel_width,
	enum lb_fb_orientation orientation);

#endif	/* FSP_GOP_BLT_H */
