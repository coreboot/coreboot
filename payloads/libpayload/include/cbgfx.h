/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2015 Google, Inc.
 */

#include <stdint.h>

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
