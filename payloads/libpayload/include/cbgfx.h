/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2015 Google, Inc.
 */

#include <libpayload.h>
#include <arch/types.h>
#include <stddef.h>

/*
 * API error codes
 */
#define CBGFX_SUCCESS		0
/* unknown error */
#define CBGFX_ERROR_UNKNOWN	1
/* failed to initialize cbgfx library */
#define CBGFX_ERROR_INIT	2
/* drawing beyond canvas boundary */
#define CBGFX_ERROR_BOUNDARY	3
/* bitmap error: signature mismatch */
#define CBGFX_ERROR_BITMAP_SIGNATURE	0x10
/* bitmap error: unsupported format */
#define CBGFX_ERROR_BITMAP_FORMAT	0x11
/* bitmap error: invalid data */
#define CBGFX_ERROR_BITMAP_DATA		0x12
/* bitmap error: scaling out of range */
#define CBGFX_ERROR_SCALE_OUT_OF_RANGE	0x13

struct vector {
	union {
		uint32_t x;
		uint32_t width;
	};
	union {
		uint32_t y;
		uint32_t height;
	};
};

struct rgb_color {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

/*
 * Resolution of scale parameters used to describe height, width, coordinate,
 * etc. relative to the canvas. For example, if it's 100, scales range from 0 to
 * 100%.
 */
#define CANVAS_SCALE		100

/*
 * The coordinate system is expected to have (0, 0) at top left corner with
 * y values increasing towards bottom of screen.
 */

/*
 * draw a box filled with a color on screen
 *
 * top_left_rel: coordinate of top left corner of the box, relative to canvas.
 * (0 - CANVAS_SCALE).
 * size_rel: width and height of the box, relative to canvas. (0 - CANVAS_SCALE)
 * rgb: RGB color of the box.
 *
 * return: CBGFX_* error codes
 */
int draw_box(const struct vector *top_left_rel,
	     const struct vector *size_rel,
	     const struct rgb_color *rgb);

/*
 * Clear the canvas
 */
int clear_canvas(struct rgb_color *rgb);

/*
 * Draw a bitmap image on screen.
 *
 * top_left_rel: coordinate of the top left corner of the image relative to the
 * canvas (0 - CANVAS_SCALE).
 * scale_rel: scale factor relative to the canvas width (0 - CANVAS_SCALE).
 * bitmap: pointer to the bitmap data, starting from the file header.
 * size: size of the bitmap data
 *
 * return: CBGFX_* error codes
 */
int draw_bitmap(const struct vector *top_left_rel,
		size_t scale_rel, const void *bitmap, size_t size);
