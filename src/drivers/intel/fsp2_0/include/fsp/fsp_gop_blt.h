/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef FSP_GOP_BLT_H
#define FSP_GOP_BLT_H

#include <efi/efi_datatype.h>
#include <types.h>

/* Convert a *.BMP graphics image to a GOP blt buffer */
void fsp_convert_bmp_to_gop_blt(uint32_t *logo, uint32_t *logo_size,
	uint32_t *blt_ptr, uint32_t *blt_size,
	uint32_t *pixel_height, uint32_t *pixel_width);

#endif	/* FSP_GOP_BLT_H */
