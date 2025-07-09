/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __RENDER_BMP_H__
#define __RENDER_BMP_H__

#ifndef OFFSET_OF
#if (defined (__GNUC__) && __GNUC__ >= 4) || defined (__clang__)
#define OFFSET_OF(TYPE, Field)  ((size_t) __builtin_offsetof(TYPE, Field))
#endif
#endif

#ifndef OFFSET_OF
#define OFFSET_OF(TYPE, Field)  ((size_t) &(((TYPE *)0)->Field))
#endif

#pragma pack(1)

struct bmp_color_map {
	uint8_t Blue;
	uint8_t Green;
	uint8_t Red;
	uint8_t Reserved;
};

struct bmp_image_header {
	char     CharB;				// 'B'
	char     CharM;				// 'M'
	uint32_t Size;
	uint16_t Reserved[2];
	uint32_t ImageOffset;
	uint32_t HeaderSize;
	uint32_t PixelWidth;
	uint32_t PixelHeight;
	uint16_t Planes;
	uint16_t BitPerPixel;			// 1, 4, 8, 24 or 32
	uint32_t CompressionType;
	uint32_t ImageSize;			// Compressed image size in bytes
	uint32_t XPixelsPerMeter;
	uint32_t YPixelsPerMeter;
	uint32_t NumberOfColors;
	uint32_t ImportantColors;
};

struct blt_pixel {
	uint8_t Blue;
	uint8_t Green;
	uint8_t Red;
	uint8_t Reserved;
};

struct logo_coordinates {
	uint32_t x;
	uint32_t y;
};

#pragma pack()

#endif // __RENDER_BMP_H__
