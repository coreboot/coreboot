/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootsplash.h>
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/fsp_gop_blt.h>
#include <stdlib.h>

static bool is_bmp_image_valid(efi_bmp_image_header *header)
{
	if (header == NULL)
		return false;

	/* Check if the BMP Header Signature is valid */
	if (header->CharB != 'B' || header->CharM != 'M')
		return false;

	/* Check if the BMP Image Header Length is valid */
	if (!header->PixelHeight || !header->PixelWidth)
		return false;

	if (header->Size < header->ImageOffset)
		return false;

	if (header->ImageOffset < sizeof(efi_bmp_image_header))
		return false;

	return true;
}

static bool is_bmp_image_compressed(efi_bmp_image_header *header)
{
	if (header == NULL)
		return false;

	if (header->CompressionType != 0)
		return true;

	return false;
}

static bool is_bitmap_format_supported(efi_bmp_image_header *header)
{
	if (header == NULL)
		return false;

	/*
	 * Check BITMAP format is supported
	 * BMP_IMAGE_HEADER = BITMAP_FILE_HEADER + BITMAP_INFO_HEADER
	 */
	if (header->HeaderSize != sizeof(efi_bmp_image_header) -
			 OFFSET_OF(efi_bmp_image_header, HeaderSize))
		return false;

	return true;
}

static bool do_bmp_image_authentication(efi_bmp_image_header *header)
{
	if (header == NULL)
		return false;

	if (!is_bmp_image_valid(header)) {
		printk(BIOS_ERR, "%s: BMP Image Header is invalid.\n", __func__);
		return false;
	}

	/*
	 * BMP image compression is unsupported by FSP implementation,
	 * hence, exit if the BMP image is compressed.
	 */
	if (is_bmp_image_compressed(header)) {
		printk(BIOS_ERR, "%s: BMP Image Compression is unsupported.\n", __func__);
		return false;
	}

	if (!is_bitmap_format_supported(header)) {
		printk(BIOS_ERR, "%s: BmpHeader Header Size (0x%x) is not as expected.\n",
			 __func__, header->HeaderSize);
		return false;
	}

	return true;
}

static uint32_t calculate_blt_buffer_size(efi_bmp_image_header *header)
{
	uint32_t blt_buffer_size;

	if (header == NULL)
		return 0;

	/* Calculate the size required for BLT buffer */
	blt_buffer_size = header->PixelWidth * header->PixelHeight *
			 sizeof(efi_graphics_output_blt_pixel);
	if (!blt_buffer_size)
		return 0;

	return blt_buffer_size;
}

static uint32_t get_color_map_num(efi_bmp_image_header *header)
{
	uint32_t col_map_number = 0;

	if (header == NULL)
		return 0;

	switch (header->BitPerPixel) {
	case 1:
		col_map_number = 2;
		break;
	case 4:
		col_map_number = 16;
		break;
	case 8:
		col_map_number = 256;
		break;
	default:
		break;
	}

	/*
	 * At times BMP file may have padding data between its header section and the
	 * data section.
	 */
	if (header->ImageOffset - sizeof(efi_bmp_image_header) <
			 sizeof(efi_bmp_color_map) * col_map_number)
		return 0;

	return col_map_number;
}

/* Fill BMP image into BLT buffer format */
static void *fill_blt_buffer(efi_bmp_image_header *header,
	uintptr_t logo_ptr, size_t blt_buffer_size)
{
	efi_graphics_output_blt_pixel *gop_blt_buffer;
	efi_graphics_output_blt_pixel *gop_blt_ptr;
	efi_graphics_output_blt_pixel *gop_blt;
	uint8_t *bmp_image;
	uint8_t *bmp_image_header;
	efi_bmp_color_map *bmp_color_map;
	size_t image_index;

	if (header == NULL)
		return NULL;

	gop_blt_ptr = malloc(sizeof(blt_buffer_size));
	if (!gop_blt_ptr)
		die("%s: out of memory. Consider increasing the `CONFIG_HEAP_SIZE`\n",
			 __func__);

	bmp_image = ((uint8_t *)logo_ptr) + header->ImageOffset;
	bmp_image_header = bmp_image;
	gop_blt_buffer = gop_blt_ptr;
	bmp_color_map = (efi_bmp_color_map *)(logo_ptr + sizeof(efi_bmp_image_header));

	for (size_t height = 0; height < header->PixelHeight; height++) {
		gop_blt = &gop_blt_buffer[(header->PixelHeight - height - 1) *
				 header->PixelWidth];
		for (size_t width = 0; width < header->PixelWidth; width++, bmp_image++,
			 gop_blt++) {
			size_t index = 0;
			switch (header->BitPerPixel) {
			/* Translate 1-bit (2 colors) BMP to 24-bit color */
			case 1:
				for (index = 0; index < 8 && width < header->PixelWidth; index++) {
					gop_blt->Red = bmp_color_map[((*bmp_image) >> (7 - index))
									 & 0x1].Red;
					gop_blt->Green = bmp_color_map[((*bmp_image) >> (7 - index))
									 & 0x1].Green;
					gop_blt->Blue = bmp_color_map[((*bmp_image) >> (7 - index))
									 & 0x1].Blue;
					gop_blt++;
					width++;
				}
				gop_blt--;
				width--;
				break;

			/* Translate 4-bit (16 colors) BMP Palette to 24-bit color */
			case 4:
				index = (*bmp_image) >> 4;
				gop_blt->Red = bmp_color_map[index].Red;
				gop_blt->Green = bmp_color_map[index].Green;
				gop_blt->Blue = bmp_color_map[index].Blue;
				if (width < (header->PixelWidth - 1)) {
					gop_blt++;
					width++;
					index = (*bmp_image) & 0x0f;
					gop_blt->Red = bmp_color_map[index].Red;
					gop_blt->Green = bmp_color_map[index].Green;
					gop_blt->Blue = bmp_color_map[index].Blue;
				}
				break;

			/* Translate 8-bit (256 colors) BMP Palette to 24-bit color */
			case 8:
				gop_blt->Red = bmp_color_map[*bmp_image].Red;
				gop_blt->Green = bmp_color_map[*bmp_image].Green;
				gop_blt->Blue = bmp_color_map[*bmp_image].Blue;
				break;

			/* For 24-bit BMP */
			case 24:
				gop_blt->Blue = *bmp_image++;
				gop_blt->Green = *bmp_image++;
				gop_blt->Red = *bmp_image;
				break;

			/* Convert 32 bit to 24bit bmp - just ignore the final byte of each pixel */
			case 32:
				gop_blt->Blue = *bmp_image++;
				gop_blt->Green = *bmp_image++;
				gop_blt->Red = *bmp_image++;
				break;

			/* Other bit format of BMP is not supported. */
			default:
				free(gop_blt_ptr);
				gop_blt_ptr = NULL;

				printk(BIOS_ERR, "%s, BMP Bit format not supported. 0x%X\n", __func__,
					 header->BitPerPixel);
				return NULL;
			}
		}
		image_index = (uintptr_t)bmp_image - (uintptr_t)bmp_image_header;
		/* Each row in BMP Image should be 4-byte align */
		if ((image_index % 4) != 0)
			bmp_image = bmp_image + (4 - (image_index % 4));
	}

	return gop_blt_ptr;
}

/* Convert a *.BMP graphics image to a GOP blt buffer */
void fsp_convert_bmp_to_gop_blt(efi_uintn_t *logo, uint32_t *logo_size,
	efi_uintn_t *blt_ptr, efi_uintn_t *blt_size,
	uint32_t *pixel_height, uint32_t *pixel_width)
{
	uintptr_t logo_ptr;
	size_t logo_ptr_size, blt_buffer_size;
	efi_bmp_image_header *bmp_header;

	if (!logo || !logo_size || !blt_ptr || !blt_size || !pixel_height || !pixel_width)
		return;

	logo_ptr = (uintptr_t)bmp_load_logo(&logo_ptr_size);

	if (!logo_ptr || logo_ptr_size < sizeof(efi_bmp_image_header)) {
		printk(BIOS_ERR, "%s: BMP Image size is too small.\n", __func__);
		return;
	}

	bmp_header = (efi_bmp_image_header *)logo_ptr;
	if (!do_bmp_image_authentication(bmp_header) || (bmp_header->Size != logo_ptr_size))
		return;

	blt_buffer_size = calculate_blt_buffer_size(bmp_header);
	if (!blt_buffer_size)
		return;

	if (!get_color_map_num(bmp_header))
		return;

	*logo = logo_ptr;
	*logo_size = logo_ptr_size;
	*blt_size = blt_buffer_size;
	*pixel_height = bmp_header->PixelHeight;
	*pixel_width = bmp_header->PixelWidth;
	*blt_ptr = (uintptr_t)fill_blt_buffer(bmp_header, logo_ptr, blt_buffer_size);
}
