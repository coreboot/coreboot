/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2015 Google, Inc.
 */

#include <libpayload.h>
#include <sysinfo.h>

/*
 * 'canvas' is the drawing area located in the center of the screen. It's a
 * square area, stretching vertically to the edges of the screen, leaving
 * non-drawing areas on the left and right. The screen is assumed to be
 * landscape.
 */
static struct vector canvas;
static uint32_t canvas_offset;		/* horizontal position of canvas */

/*
 * Framebuffer is assumed to assign a higher coordinate (larger x, y) to
 * a higher address
 */
static struct cb_framebuffer *fbinfo;
static uint8_t *fbaddr;

static char initialized = 0;
#define LOG(x...)	printf("CBGFX: " x)

static void add_vectors(struct vector *out,
			const struct vector *v1, const struct vector *v2)
{
	out->x = v1->x + v2->x;
	out->y = v1->y + v2->y;
}

static void to_canvas(const struct vector *relative, struct vector *absolute)
{
	absolute->x = canvas.width * relative->x / CANVAS_SCALE;
	absolute->y = canvas.height * relative->y / CANVAS_SCALE;
}

static int within_canvas(const struct vector *v)
{
	return v->x < canvas.width && v->y < canvas.height;
}

static inline uint32_t calculate_color(const struct rgb_color *rgb)
{
	uint32_t color = 0;
	color |= (rgb->red >> (8 - fbinfo->red_mask_size))
		<< fbinfo->red_mask_pos;
	color |= (rgb->green >> (8 - fbinfo->green_mask_size))
		<< fbinfo->green_mask_pos;
	color |= (rgb->blue >> (8 - fbinfo->blue_mask_size))
		<< fbinfo->blue_mask_pos;
	return color;
}

/*
 * Plot a pixel in a framebuffer. This is called from tight loops. Keep it slim
 * and do the validation at callers' site.
 */
static inline void set_pixel(struct vector *coord, uint32_t color)
{
	const int bpp = fbinfo->bits_per_pixel;
	int i;
	uint8_t * const pixel = fbaddr + (coord->x + canvas_offset +
			coord->y * fbinfo->x_resolution) * bpp / 8;
	for (i = 0; i < bpp / 8; i++)
		pixel[i] = (color >> (i * 8));
}

/*
 * Initializes the library. Automatically called by APIs. It sets up
 * the canvas and the framebuffer.
 */
static int cbgfx_init(void)
{
	if (initialized)
		return 0;

	fbinfo = lib_sysinfo.framebuffer;
	if (!fbinfo)
		return -1;

	fbaddr = phys_to_virt((uint8_t *)(uintptr_t)(fbinfo->physical_address));
	if (!fbaddr)
		return -1;

	/* calculate canvas size, assuming the screen is landscape */
	canvas.height = fbinfo->y_resolution;
	canvas.width = canvas.height;
	canvas_offset = (fbinfo->x_resolution - canvas.width) / 2;
	if (canvas_offset < 0) {
		LOG("Portrait screens are not supported\n");
		return -1;
	}

	initialized = 1;
	LOG("cbgfx initialized: canvas width=%d, height=%d, offset=%d\n",
	    canvas.width, canvas.height, canvas_offset);

	return 0;
}

int draw_box(const struct vector *top_left_rel,
	     const struct vector *size_rel,
	     const struct rgb_color *rgb)
{
	struct vector top_left;
	struct vector size;
	struct vector p, t;
	const uint32_t color = calculate_color(rgb);

	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	to_canvas(top_left_rel, &top_left);
	to_canvas(size_rel, &size);
	add_vectors(&t, &top_left, &size);
	if (!within_canvas(&t)) {
		LOG("Box exceeds canvas boundary\n");
		return CBGFX_ERROR_BOUNDARY;
	}

	for (p.y = top_left.y; p.y < t.y; p.y++)
		for (p.x = top_left.x; p.x < t.x; p.x++)
			set_pixel(&p, color);

	return CBGFX_SUCCESS;
}

int clear_canvas(struct rgb_color *rgb)
{
	const struct vector coord = {
		.x = 0,
		.y = 0,
	};
	const struct vector size = {
		.width = CANVAS_SCALE,
		.height = CANVAS_SCALE,
	};

	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	return draw_box(&coord, &size, rgb);
}
