/*
 *
 * Copyright (C) 2015 Google, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <libpayload.h>
#include <cbfs.h>
#include <fpmath.h>
#include <sysinfo.h>
#include "bitmap.h"

/*
 * 'canvas' is the drawing area located in the center of the screen. It's a
 * square area, stretching vertically to the edges of the screen, leaving
 * non-drawing areas on the left and right. The screen is assumed to be
 * landscape.
 */
static struct rect canvas;
static struct rect screen;

static uint8_t *gfx_buffer;

/*
 * Framebuffer is assumed to assign a higher coordinate (larger x, y) to
 * a higher address
 */
static const struct cb_framebuffer *fbinfo;

/* Shorthand for up-to-date virtual framebuffer address */
#define REAL_FB ((unsigned char *)phys_to_virt(fbinfo->physical_address))
#define FB	(gfx_buffer ? gfx_buffer : REAL_FB)

#define LOG(x...)	printf("CBGFX: " x)
#define PIVOT_H_MASK	(PIVOT_H_LEFT|PIVOT_H_CENTER|PIVOT_H_RIGHT)
#define PIVOT_V_MASK	(PIVOT_V_TOP|PIVOT_V_CENTER|PIVOT_V_BOTTOM)
#define ROUNDUP(x, y)	((((x) + ((y) - 1)) / (y)) * (y))

static char initialized = 0;

static const struct vector vzero = {
	.x = 0,
	.y = 0,
};

struct color_transformation {
	uint8_t base;
	int16_t scale;
};

struct color_mapping {
	struct color_transformation red;
	struct color_transformation green;
	struct color_transformation blue;
	int enabled;
};

static struct color_mapping color_map;

static inline void set_color_trans(struct color_transformation *trans,
				   uint8_t bg_color, uint8_t fg_color)
{
	trans->base = bg_color;
	trans->scale = fg_color - bg_color;
}

int set_color_map(const struct rgb_color *background,
		  const struct rgb_color *foreground)
{
	if (background == NULL || foreground == NULL)
		return CBGFX_ERROR_INVALID_PARAMETER;

	set_color_trans(&color_map.red, background->red, foreground->red);
	set_color_trans(&color_map.green, background->green,
			foreground->green);
	set_color_trans(&color_map.blue, background->blue, foreground->blue);
	color_map.enabled = 1;

	return CBGFX_SUCCESS;
}

void clear_color_map(void)
{
	color_map.enabled = 0;
}

struct blend_value {
	uint8_t alpha;
	struct rgb_color rgb;
};

static struct blend_value blend;

int set_blend(const struct rgb_color *rgb, uint8_t alpha)
{
	if (rgb == NULL)
		return CBGFX_ERROR_INVALID_PARAMETER;

	blend.alpha = alpha;
	blend.rgb = *rgb;

	return CBGFX_SUCCESS;
}

void clear_blend(void)
{
	blend.alpha = 0;
	blend.rgb.red = 0;
	blend.rgb.green = 0;
	blend.rgb.blue = 0;
}

static void add_vectors(struct vector *out,
			const struct vector *v1, const struct vector *v2)
{
	out->x = v1->x + v2->x;
	out->y = v1->y + v2->y;
}

static int fraction_equal(const struct fraction *f1, const struct fraction *f2)
{
	return (int64_t)f1->n * f2->d == (int64_t)f2->n * f1->d;
}

static int is_valid_fraction(const struct fraction *f)
{
	return f->d != 0;
}

static int is_valid_scale(const struct scale *s)
{
	return is_valid_fraction(&s->x) && is_valid_fraction(&s->y);
}

static void reduce_fraction(struct fraction *out, int64_t n, int64_t d)
{
	/* Simplest way to reduce the fraction until fitting in int32_t */
	int shift = log2(MAX(ABS(n), ABS(d)) >> 31) + 1;
	out->n = n >> shift;
	out->d = d >> shift;
}

/* out = f1 + f2 */
static void add_fractions(struct fraction *out,
			  const struct fraction *f1, const struct fraction *f2)
{
	reduce_fraction(out,
			(int64_t)f1->n * f2->d + (int64_t)f2->n * f1->d,
			(int64_t)f1->d * f2->d);
}

/* out = f1 - f2 */
static void subtract_fractions(struct fraction *out,
			       const struct fraction *f1,
			       const struct fraction *f2)
{
	reduce_fraction(out,
			(int64_t)f1->n * f2->d - (int64_t)f2->n * f1->d,
			(int64_t)f1->d * f2->d);
}

static void add_scales(struct scale *out,
		       const struct scale *s1, const struct scale *s2)
{
	add_fractions(&out->x, &s1->x, &s2->x);
	add_fractions(&out->y, &s1->y, &s2->y);
}

/*
 * Transform a vector:
 * 	x' = x * a_x + offset_x
 * 	y' = y * a_y + offset_y
 */
static int transform_vector(struct vector *out,
			    const struct vector *in,
			    const struct scale *a,
			    const struct vector *offset)
{
	if (!is_valid_scale(a))
		return CBGFX_ERROR_INVALID_PARAMETER;
	out->x = (int64_t)a->x.n * in->x / a->x.d + offset->x;
	out->y = (int64_t)a->y.n * in->y / a->y.d + offset->y;
	return CBGFX_SUCCESS;
}

/*
 * Returns 1 if v is exclusively within box, 0 if v is inclusively within box,
 * or -1 otherwise.
 */
static int within_box(const struct vector *v, const struct rect *bound)
{
	if (v->x > bound->offset.x &&
	    v->y > bound->offset.y &&
	    v->x < bound->offset.x + bound->size.width &&
	    v->y < bound->offset.y + bound->size.height)
		return 1;
	else if (v->x >= bound->offset.x &&
		 v->y >= bound->offset.y &&
		 v->x <= bound->offset.x + bound->size.width &&
		 v->y <= bound->offset.y + bound->size.height)
		return 0;
	else
		return -1;
}

/* Helper function that applies color_map to the color. */
static inline uint8_t apply_map(uint8_t color,
				const struct color_transformation *trans)
{
	if (!color_map.enabled)
		return color;
	return trans->base + trans->scale * color / UINT8_MAX;
}

/*
 * Helper function that applies color and opacity from blend struct
 * into the color.
 */
static inline uint8_t apply_blend(uint8_t color, uint8_t blend_color)
{
	if (blend.alpha == 0 || color == blend_color)
		return color;

	return (color * (256 - blend.alpha) +
		blend_color * blend.alpha) / 256;
}

static inline uint32_t calculate_color(const struct rgb_color *rgb,
				       uint8_t invert)
{
	uint32_t color = 0;

	color |= (apply_blend(apply_map(rgb->red, &color_map.red),
			      blend.rgb.red)
		  >> (8 - fbinfo->red_mask_size))
		 << fbinfo->red_mask_pos;
	color |= (apply_blend(apply_map(rgb->green, &color_map.green),
			      blend.rgb.green)
		  >> (8 - fbinfo->green_mask_size))
		 << fbinfo->green_mask_pos;
	color |= (apply_blend(apply_map(rgb->blue, &color_map.blue),
			      blend.rgb.blue)
		  >> (8 - fbinfo->blue_mask_size))
		 << fbinfo->blue_mask_pos;
	if (invert)
		color ^= 0xffffffff;
	return color;
}

/*
 * Plot a pixel in a framebuffer. This is called from tight loops. Keep it slim
 * and do the validation at callers' site.
 */
static inline void set_pixel(struct vector *coord, uint32_t color)
{
	const int bpp = fbinfo->bits_per_pixel;
	const int bpl = fbinfo->bytes_per_line;
	struct vector rcoord;
	int i;

	switch (fbinfo->orientation) {
	case CB_FB_ORIENTATION_NORMAL:
	default:
		rcoord.x = coord->x;
		rcoord.y = coord->y;
		break;
	case CB_FB_ORIENTATION_BOTTOM_UP:
		rcoord.x = screen.size.width - 1 - coord->x;
		rcoord.y = screen.size.height - 1 - coord->y;
		break;
	case CB_FB_ORIENTATION_LEFT_UP:
		rcoord.x = coord->y;
		rcoord.y = screen.size.width - 1 - coord->x;
		break;
	case CB_FB_ORIENTATION_RIGHT_UP:
		rcoord.x = screen.size.height - 1 - coord->y;
		rcoord.y = coord->x;
		break;
	}

	uint8_t * const pixel = FB + rcoord.y * bpl + rcoord.x * bpp / 8;
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

	fbinfo = &lib_sysinfo.framebuffer;

	if (!fbinfo->physical_address)
		return CBGFX_ERROR_FRAMEBUFFER_ADDR;

	switch (fbinfo->orientation) {
	default: /* Normal or rotated 180 degrees. */
		screen.size.width = fbinfo->x_resolution;
		screen.size.height = fbinfo->y_resolution;
		break;
	case CB_FB_ORIENTATION_LEFT_UP: /* 90 degree rotation. */
	case CB_FB_ORIENTATION_RIGHT_UP:
		screen.size.width = fbinfo->y_resolution;
		screen.size.height = fbinfo->x_resolution;
		break;
	}
	screen.offset.x = 0;
	screen.offset.y = 0;

	/* Calculate canvas size & offset. Canvas is always square. */
	if (screen.size.height > screen.size.width) {
		canvas.size.height = screen.size.width;
		canvas.size.width = canvas.size.height;
		canvas.offset.x = 0;
		canvas.offset.y = (screen.size.height - canvas.size.height) / 2;
	} else {
		canvas.size.height = screen.size.height;
		canvas.size.width = canvas.size.height;
		canvas.offset.x = (screen.size.width - canvas.size.width) / 2;
		canvas.offset.y = 0;
	}

	initialized = 1;
	LOG("cbgfx initialized: screen:width=%d, height=%d, offset=%d canvas:width=%d, height=%d, offset=%d\n",
	    screen.size.width, screen.size.height, screen.offset.x,
	    canvas.size.width, canvas.size.height, canvas.offset.x);

	return 0;
}

int draw_box(const struct rect *box, const struct rgb_color *rgb)
{
	struct vector top_left;
	struct vector p, t;

	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	const uint32_t color = calculate_color(rgb, 0);
	const struct scale top_left_s = {
		.x = { .n = box->offset.x, .d = CANVAS_SCALE, },
		.y = { .n = box->offset.y, .d = CANVAS_SCALE, }
	};
	const struct scale bottom_right_s = {
		.x = { .n = box->offset.x + box->size.x, .d = CANVAS_SCALE, },
		.y = { .n = box->offset.y + box->size.y, .d = CANVAS_SCALE, }
	};

	transform_vector(&top_left, &canvas.size, &top_left_s, &canvas.offset);
	transform_vector(&t, &canvas.size, &bottom_right_s, &canvas.offset);
	if (within_box(&t, &canvas) < 0) {
		LOG("Box exceeds canvas boundary\n");
		return CBGFX_ERROR_BOUNDARY;
	}

	for (p.y = top_left.y; p.y < t.y; p.y++)
		for (p.x = top_left.x; p.x < t.x; p.x++)
			set_pixel(&p, color);

	return CBGFX_SUCCESS;
}

int draw_rounded_box(const struct scale *pos_rel, const struct scale *dim_rel,
		     const struct rgb_color *rgb,
		     const struct fraction *thickness,
		     const struct fraction *radius)
{
	struct scale pos_end_rel;
	struct vector top_left;
	struct vector p, t;

	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	const uint32_t color = calculate_color(rgb, 0);

	if (!is_valid_scale(pos_rel) || !is_valid_scale(dim_rel))
		return CBGFX_ERROR_INVALID_PARAMETER;

	add_scales(&pos_end_rel, pos_rel, dim_rel);
	transform_vector(&top_left, &canvas.size, pos_rel, &canvas.offset);
	transform_vector(&t, &canvas.size, &pos_end_rel, &canvas.offset);
	if (within_box(&t, &canvas) < 0) {
		LOG("Box exceeds canvas boundary\n");
		return CBGFX_ERROR_BOUNDARY;
	}

	if (!is_valid_fraction(thickness) || !is_valid_fraction(radius))
		return CBGFX_ERROR_INVALID_PARAMETER;

	struct scale thickness_scale = {
		.x = { .n = thickness->n, .d = thickness->d },
		.y = { .n = thickness->n, .d = thickness->d },
	};
	struct scale radius_scale = {
		.x = { .n = radius->n, .d = radius->d },
		.y = { .n = radius->n, .d = radius->d },
	};
	struct vector d, r, s;
	transform_vector(&d, &canvas.size, &thickness_scale, &vzero);
	transform_vector(&r, &canvas.size, &radius_scale, &vzero);
	const uint8_t has_thickness = d.x > 0 && d.y > 0;
	if (thickness->n != 0 && !has_thickness)
		LOG("Thickness truncated to 0\n");
	const uint8_t has_radius = r.x > 0 && r.y > 0;
	if (radius->n != 0 && !has_radius)
		LOG("Radius truncated to 0\n");
	if (has_radius) {
		if (d.x > r.x || d.y > r.y) {
			LOG("Thickness cannot be greater than radius\n");
			return CBGFX_ERROR_INVALID_PARAMETER;
		}
		if (r.x * 2 > t.x - top_left.x || r.y * 2 > t.y - top_left.y) {
			LOG("Radius cannot be greater than half of the box\n");
			return CBGFX_ERROR_INVALID_PARAMETER;
		}
	}

	/* Step 1: Draw edges */
	int32_t x_begin, x_end;
	if (has_thickness) {
		/* top */
		for (p.y = top_left.y; p.y < top_left.y + d.y; p.y++)
			for (p.x = top_left.x + r.x; p.x < t.x - r.x; p.x++)
				set_pixel(&p, color);
		/* bottom */
		for (p.y = t.y - d.y; p.y < t.y; p.y++)
			for (p.x = top_left.x + r.x; p.x < t.x - r.x; p.x++)
				set_pixel(&p, color);
		for (p.y = top_left.y + r.y; p.y < t.y - r.y; p.y++) {
			/* left */
			for (p.x = top_left.x; p.x < top_left.x + d.x; p.x++)
				set_pixel(&p, color);
			/* right */
			for (p.x = t.x - d.x; p.x < t.x; p.x++)
				set_pixel(&p, color);
		}
	} else {
		/* Fill the regions except circular sectors */
		for (p.y = top_left.y; p.y < t.y; p.y++) {
			if (p.y >= top_left.y + r.y && p.y < t.y - r.y) {
				x_begin = top_left.x;
				x_end = t.x;
			} else {
				x_begin = top_left.x + r.x;
				x_end = t.x - r.x;
			}
			for (p.x = x_begin; p.x < x_end; p.x++)
				set_pixel(&p, color);
		}
	}

	if (!has_radius)
		return CBGFX_SUCCESS;

	/*
	 * Step 2: Draw rounded corners
	 * When has_thickness, only the border is drawn. With fixed thickness,
	 * the time complexity is linear to the size of the box.
	 */
	if (has_thickness) {
		s.x = r.x - d.x;
		s.y = r.y - d.y;
	} else {
		s.x = 0;
		s.y = 0;
	}

	/* Use 64 bits to avoid overflow */
	int32_t x, y;
	uint64_t yy;
	const uint64_t rrx = (uint64_t)r.x * r.x, rry = (uint64_t)r.y * r.y;
	const uint64_t ssx = (uint64_t)s.x * s.x, ssy = (uint64_t)s.y * s.y;
	x_begin = 0;
	x_end = 0;
	for (y = r.y - 1; y >= 0; y--) {
		/*
		 * The inequality is valid in the beginning of each iteration:
		 * y^2 + x_end^2 < r^2
		 */
		yy = (uint64_t)y * y;
		/* Check yy/ssy + xx/ssx < 1 */
		while (yy * ssx + x_begin * x_begin * ssy < ssx * ssy)
			x_begin++;
		/* The inequality must be valid now: y^2 + x_begin >= s^2 */
		x = x_begin;
		/* Check yy/rry + xx/rrx < 1 */
		while (x < x_end || yy * rrx + x * x * rry < rrx * rry) {
			/*
			 * Example sequence of (y, x) when s = (4, 4) and
			 * r = (5, 5):
			 *   [(4, 0), (4, 1), (4, 2), (3, 3), (2, 4),
			 *    (1, 4), (0, 4)].
			 * If s.x==s.y r.x==r.y, then the sequence will be
			 * symmetric, and x and y will range from 0 to (r-1).
			 */
			/* top left */
			p.y = top_left.y + r.y - 1 - y;
			p.x = top_left.x + r.x - 1 - x;
			set_pixel(&p, color);
			/* top right */
			p.y = top_left.y + r.y - 1 - y;
			p.x = t.x - r.x + x;
			set_pixel(&p, color);
			/* bottom left */
			p.y = t.y - r.y + y;
			p.x = top_left.x + r.x - 1 - x;
			set_pixel(&p, color);
			/* bottom right */
			p.y = t.y - r.y + y;
			p.x = t.x - r.x + x;
			set_pixel(&p, color);
			x++;
		}
		x_end = x;
		/* (x_begin <= x_end) must hold now */
	}

	return CBGFX_SUCCESS;
}

int draw_line(const struct scale *pos1, const struct scale *pos2,
	      const struct fraction *thickness, const struct rgb_color *rgb)
{
	struct fraction len;
	struct vector top_left;
	struct vector size;
	struct vector p, t;

	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	const uint32_t color = calculate_color(rgb, 0);

	if (!is_valid_fraction(thickness))
		return CBGFX_ERROR_INVALID_PARAMETER;

	transform_vector(&top_left, &canvas.size, pos1, &canvas.offset);
	if (fraction_equal(&pos1->y, &pos2->y)) {
		/* Horizontal line */
		subtract_fractions(&len, &pos2->x, &pos1->x);
		struct scale dim = {
			.x = { .n = len.n, .d = len.d },
			.y = { .n = thickness->n, .d = thickness->d },
		};
		transform_vector(&size, &canvas.size, &dim, &vzero);
		size.y = MAX(size.y, 1);
	} else if (fraction_equal(&pos1->x, &pos2->x)) {
		/* Vertical line */
		subtract_fractions(&len, &pos2->y, &pos1->y);
		struct scale dim = {
			.x = { .n = thickness->n, .d = thickness->d },
			.y = { .n = len.n, .d = len.d },
		};
		transform_vector(&size, &canvas.size, &dim, &vzero);
		size.x = MAX(size.x, 1);
	} else {
		LOG("Only support horizontal and vertical lines\n");
		return CBGFX_ERROR_INVALID_PARAMETER;
	}

	add_vectors(&t, &top_left, &size);
	if (within_box(&t, &canvas) < 0) {
		LOG("Line exceeds canvas boundary\n");
		return CBGFX_ERROR_BOUNDARY;
	}

	for (p.y = top_left.y; p.y < t.y; p.y++)
		for (p.x = top_left.x; p.x < t.x; p.x++)
			set_pixel(&p, color);

	return CBGFX_SUCCESS;
}

int clear_canvas(const struct rgb_color *rgb)
{
	const struct rect box = {
		vzero,
		.size = {
			.width = CANVAS_SCALE,
			.height = CANVAS_SCALE,
		},
	};

	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	return draw_box(&box, rgb);
}

int clear_screen(const struct rgb_color *rgb)
{
	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	int x, y, i;
	uint32_t color = calculate_color(rgb, 0);
	const int bpp = fbinfo->bits_per_pixel;
	const int bpl = fbinfo->bytes_per_line;
	uint8_t *line = malloc(bpl);

	if (!line) {
		LOG("Failed to allocate line buffer (%u bytes)\n", bpl);
		return CBGFX_ERROR_UNKNOWN;
	}

	/* Set line buffer pixels, then memcpy to framebuffer */
	for (x = 0; x < fbinfo->x_resolution; x++)
		for (i = 0; i < bpp / 8; i++)
			line[x * bpp / 8 + i] = (color >> (i * 8));
	for (y = 0; y < fbinfo->y_resolution; y++)
		memcpy(FB + y * bpl, line, bpl);

	free(line);
	return CBGFX_SUCCESS;
}

static int pal_to_rgb(uint8_t index, const struct bitmap_palette_element_v3 *pal,
		      size_t palcount, struct rgb_color *out)
{
	if (index >= palcount) {
		LOG("Color index %d exceeds palette boundary\n", index);
		return CBGFX_ERROR_BITMAP_DATA;
	}

	out->red = pal[index].red;
	out->green = pal[index].green;
	out->blue = pal[index].blue;
	return CBGFX_SUCCESS;
}

/*
 * We're using the Lanczos resampling algorithm to rescale images to a new size.
 * Since output size is often not cleanly divisible by input size, an output
 * pixel (ox,oy) corresponds to a point that lies in the middle between several
 * input pixels (ix,iy), meaning that if you transformed the coordinates of the
 * output pixel into the input image space, they would be fractional. To sample
 * the color of this "virtual" pixel with fractional coordinates, we gather the
 * 6x6 grid of nearest real input pixels in a sample array. Then we multiply the
 * color values for each of those pixels (separately for red, green and blue)
 * with a "weight" value that was calculated from the distance between that
 * input pixel and the fractional output pixel coordinates. This is done for
 * both X and Y dimensions separately. The combined weights for all 36 sample
 * pixels add up to 1.0, so by adding up the multiplied color values we get the
 * interpolated color for the output pixel.
 *
 * The CONFIG_LP_CBGFX_FAST_RESAMPLE option let's the user change the 'a'
 * parameter from the Lanczos weight formula from 3 to 2, which effectively
 * reduces the size of the sample array from 6x6 to 4x4. This is a bit faster
 * but doesn't look as good. Most use cases should be fine without it.
 */
#if CONFIG(LP_CBGFX_FAST_RESAMPLE)
#define LNCZ_A 2
#else
#define LNCZ_A 3
#endif

/*
 * When walking the sample array we often need to start at a pixel close to our
 * fractional output pixel (for convenience we choose the pixel on the top-left
 * which corresponds to the integer parts of the output pixel coordinates) and
 * then work our way outwards in both directions from there. Arrays in C must
 * start at 0 but we'd really prefer indexes to go from -2 to 3 (for 6x6)
 * instead, so that this "start pixel" could be 0. Since we cannot do that,
 * define a constant for the index of that "0th" pixel instead.
 */
#define S0 (LNCZ_A - 1)

/* The size of the sample array, which we need a lot. */
#define SSZ (LNCZ_A * 2)

/*
 * This is implementing the Lanczos kernel according to:
 * https://en.wikipedia.org/wiki/Lanczos_resampling
 *
 *         / 1							if x = 0
 * L(x) = <  a * sin(pi * x) * sin(pi * x / a) / (pi^2 * x^2)	if -a < x <= a
 *	   \ 0							otherwise
 */
static fpmath_t lanczos_weight(fpmath_t in, int off)
{
	/*
	 * |in| is the output pixel coordinate scaled into the input pixel
	 * space. |off| is the offset in the sample array for the pixel whose
	 * weight we're calculating. (off - S0) is the distance from that
	 * sample pixel to the S0 pixel, and the fractional part of |in|
	 * (in - floor(in)) is by definition the distance between S0 and the
	 * output pixel.
	 *
	 * So (off - S0) - (in - floor(in)) is the distance from the sample
	 * pixel to S0 minus the distance from S0 to the output pixel, aka
	 * the distance from the sample pixel to the output pixel.
	 */
	fpmath_t x = fpisub(off - S0, fpsubi(in, fpfloor(in)));

	if (fpequals(x, fp(0)))
		return fp(1);

	/* x * 2 / a can save some instructions if a == 2 */
	fpmath_t x2a = x;
	if (LNCZ_A != 2)
		x2a = fpmul(x, fpfrac(2, LNCZ_A));

	fpmath_t x_times_pi = fpmul(x, fppi());

	/*
	 * Rather than using sinr(pi*x), we leverage the "one-based" sine
	 * function (see <fpmath.h>) with sin1(2*x) so that the pi is eliminated
	 * since multiplication by an integer is a slightly faster operation.
	 */
	fpmath_t tmp = fpmuli(fpdiv(fpsin1(fpmuli(x, 2)), x_times_pi), LNCZ_A);
	return fpdiv(fpmul(tmp, fpsin1(x2a)), x_times_pi);
}

static int draw_bitmap_v3(const struct vector *top_left,
			  const struct vector *dim,
			  const struct vector *dim_org,
			  const struct bitmap_header_v3 *header,
			  const struct bitmap_palette_element_v3 *pal,
			  const uint8_t *pixel_array, uint8_t invert)
{
	const int bpp = header->bits_per_pixel;
	int32_t dir;
	struct vector p;
	int32_t ox, oy;		/* output (resampled) pixel coordinates */
	int32_t ix, iy;		/* input (source image) pixel coordinates */
	int sx, sy;	/* index into |sample| (not ringbuffer adjusted) */

	if (header->compression) {
		LOG("Compressed bitmaps are not supported\n");
		return CBGFX_ERROR_BITMAP_FORMAT;
	}
	if (bpp >= 16) {
		LOG("Non-palette bitmaps are not supported\n");
		return CBGFX_ERROR_BITMAP_FORMAT;
	}
	if (bpp != 8) {
		LOG("Unsupported bits per pixel: %d\n", bpp);
		return CBGFX_ERROR_BITMAP_FORMAT;
	}

	const int32_t y_stride = ROUNDUP(dim_org->width * bpp / 8, 4);
	/*
	 * header->height can be positive or negative.
	 *
	 * If it's negative, pixel data is stored from top to bottom. We render
	 * image from the lowest row to the highest row.
	 *
	 * If it's positive, pixel data is stored from bottom to top. We render
	 * image from the highest row to the lowest row.
	 */
	p.y = top_left->y;
	if (header->height < 0) {
		dir = 1;
	} else {
		p.y += dim->height - 1;
		dir = -1;
	}

	/* Don't waste time resampling when the scale is 1:1. */
	if (dim_org->width == dim->width && dim_org->height == dim->height) {
		for (oy = 0; oy < dim->height; oy++, p.y += dir) {
			p.x = top_left->x;
			for (ox = 0; ox < dim->width; ox++, p.x++) {
				struct rgb_color rgb;
				if (pal_to_rgb(pixel_array[oy * y_stride + ox],
					       pal, header->colors_used, &rgb))
					return CBGFX_ERROR_BITMAP_DATA;
				set_pixel(&p, calculate_color(&rgb, invert));
			}
		}
		return CBGFX_SUCCESS;
	}

	/* Precalculate the X-weights for every possible ox so that we only have
	   to multiply weights together in the end. */
	fpmath_t (*weight_x)[SSZ] = malloc(sizeof(fpmath_t) * SSZ * dim->width);
	if (!weight_x)
		return CBGFX_ERROR_UNKNOWN;
	for (ox = 0; ox < dim->width; ox++) {
		for (sx = 0; sx < SSZ; sx++) {
			fpmath_t ixfp = fpfrac(ox * dim_org->width, dim->width);
			weight_x[ox][sx] = lanczos_weight(ixfp, sx);
		}
	}

	/*
	 * For every sy in the sample array, we directly cache a pointer into
	 * the .BMP pixel array for the start of the corresponding line. On the
	 * edges of the image (where we don't have any real pixels to fill all
	 * lines in the sample array), we just reuse the last valid lines inside
	 * the image for all lines that would lie outside.
	 */
	const uint8_t *ypix[SSZ];
	for (sy = 0; sy < SSZ; sy++) {
		if (sy <= S0)
			ypix[sy] = pixel_array;
		else if (sy - S0 >= dim_org->height)
			ypix[sy] = ypix[sy - 1];
		else
			ypix[sy] = &pixel_array[y_stride * (sy - S0)];
	}

	/* iy and ix track the input pixel corresponding to sample[S0][S0]. */
	iy = 0;
	for (oy = 0; oy < dim->height; oy++, p.y += dir) {
		struct rgb_color sample[SSZ][SSZ];

		/* Like with X weights, we also cache all Y weights. */
		fpmath_t iyfp = fpfrac(oy * dim_org->height, dim->height);
		fpmath_t weight_y[SSZ];
		for (sy = 0; sy < SSZ; sy++)
			weight_y[sy] = lanczos_weight(iyfp, sy);

		/*
		 * If we have a new input pixel line between the last oy and
		 * this one, we have to adjust iy forward. When upscaling, this
		 * is not always the case for each new output line. When
		 * downscaling, we may even cross more than one line per output
		 * pixel.
		 */
		while (fpfloor(iyfp) > iy) {
			iy++;

			/* Shift ypix array up to center around next iy line. */
			for (sy = 0; sy < SSZ - 1; sy++)
				ypix[sy] = ypix[sy + 1];

			/* Calculate the last ypix that is being shifted in,
			   but beware of reaching the end of the input image. */
			if (iy + LNCZ_A < dim_org->height)
				ypix[SSZ - 1] = &pixel_array[y_stride *
							     (iy + LNCZ_A)];
		}

		/*
		 * Initialize the sample array for this line, and also
		 * the equals counter, which counts how many of the latest
		 * pixels were exactly equal.
		 */
		int equals = 0;
		uint8_t last_equal = ypix[0][0];
		for (sx = 0; sx < SSZ; sx++) {
			for (sy = 0; sy < SSZ; sy++) {
				if (sx - S0 >= dim_org->width) {
					sample[sx][sy] = sample[sx - 1][sy];
					equals++;
					continue;
				}
				/*
				 * For pixels to the left of S0 there are no
				 * corresponding input pixels so just use
				 * ypix[sy][0].
				 */
				uint8_t i = ypix[sy][MAX(0, sx - S0)];
				if (pal_to_rgb(i, pal, header->colors_used,
					       &sample[sx][sy]))
					goto bitmap_error;
				if (i == last_equal) {
					equals++;
				} else {
					last_equal = i;
					equals = 1;
				}
			}
		}

		ix = 0;
		p.x = top_left->x;
		for (ox = 0; ox < dim->width; ox++, p.x++) {
			/* Adjust ix forward, same as iy above. */
			fpmath_t ixfp = fpfrac(ox * dim_org->width, dim->width);
			while (fpfloor(ixfp) > ix) {
				ix++;

				/*
				 * We want to reuse the sample columns we
				 * already have, but we don't want to copy them
				 * all around for every new column either.
				 * Instead, treat the X dimension of the sample
				 * array like a ring buffer indexed by ix. rx is
				 * the ringbuffer-adjusted offset of the new
				 * column in sample (the rightmost one) we're
				 * trying to fill.
				 */
				int rx = (SSZ - 1 + ix) % SSZ;
				for (sy = 0; sy < SSZ; sy++) {
					if (ix + LNCZ_A >= dim_org->width) {
						sample[rx][sy] = sample[(SSZ - 2
							+ ix) % SSZ][sy];
						equals++;
						continue;
					}
					uint8_t i = ypix[sy][ix + LNCZ_A];
					if (i == last_equal) {
						if (equals++ >= (SSZ * SSZ))
							continue;
					} else {
						last_equal = i;
						equals = 1;
					}
					if (pal_to_rgb(i, pal,
						       header->colors_used,
						       &sample[rx][sy]))
						goto bitmap_error;
				}
			}

			/* If all pixels in sample are equal, fast path. */
			if (equals >= (SSZ * SSZ)) {
				set_pixel(&p, calculate_color(&sample[0][0],
							      invert));
				continue;
			}

			fpmath_t red = fp(0);
			fpmath_t green = fp(0);
			fpmath_t blue = fp(0);
			for (sy = 0; sy < SSZ; sy++) {
				for (sx = 0; sx < SSZ; sx++) {
					int rx = (sx + ix) % SSZ;
					fpmath_t weight = fpmul(weight_x[ox][sx],
								weight_y[sy]);
					red = fpadd(red, fpmuli(weight,
						sample[rx][sy].red));
					green = fpadd(green, fpmuli(weight,
						sample[rx][sy].green));
					blue = fpadd(blue, fpmuli(weight,
						sample[rx][sy].blue));
				}
			}

			/*
			 * Weights *should* sum up to 1.0 (making this not
			 * necessary) but just to hedge against rounding errors
			 * we should clamp color values to their legal limits.
			 */
			struct rgb_color rgb = {
				.red = MAX(0, MIN(UINT8_MAX, fpround(red))),
				.green = MAX(0, MIN(UINT8_MAX, fpround(green))),
				.blue = MAX(0, MIN(UINT8_MAX, fpround(blue))),
			};

			set_pixel(&p, calculate_color(&rgb, invert));
		}
	}

	free(weight_x);
	return CBGFX_SUCCESS;

bitmap_error:
	free(weight_x);
	return CBGFX_ERROR_BITMAP_DATA;
}

static int get_bitmap_file_header(const void *bitmap, size_t size,
				  struct bitmap_file_header *file_header)
{
	const struct bitmap_file_header *fh;

	if (sizeof(*file_header) > size) {
		LOG("Invalid bitmap data\n");
		return CBGFX_ERROR_BITMAP_DATA;
	}
	fh = (struct bitmap_file_header *)bitmap;
	if (fh->signature[0] != 'B' || fh->signature[1] != 'M') {
		LOG("Bitmap signature mismatch\n");
		return CBGFX_ERROR_BITMAP_SIGNATURE;
	}
	file_header->file_size = le32toh(fh->file_size);
	if (file_header->file_size != size) {
		LOG("Bitmap file size does not match cbfs file size\n");
		return CBGFX_ERROR_BITMAP_DATA;
	}
	file_header->bitmap_offset = le32toh(fh->bitmap_offset);

	return CBGFX_SUCCESS;
}

static int parse_bitmap_header_v3(
			const uint8_t *bitmap,
			size_t size,
			/* ^--- IN / OUT ---v */
			struct bitmap_header_v3 *header,
			const struct bitmap_palette_element_v3 **palette,
			const uint8_t **pixel_array,
			struct vector *dim_org)
{
	struct bitmap_file_header file_header;
	struct bitmap_header_v3 *h;
	int rv;

	rv = get_bitmap_file_header(bitmap, size, &file_header);
	if (rv)
		return rv;

	size_t header_offset = sizeof(struct bitmap_file_header);
	size_t header_size = sizeof(struct bitmap_header_v3);
	size_t palette_offset = header_offset + header_size;
	size_t file_size = file_header.file_size;

	h = (struct bitmap_header_v3 *)(bitmap + header_offset);
	header->header_size = le32toh(h->header_size);
	if (header->header_size != header_size) {
		LOG("Unsupported bitmap format\n");
		return CBGFX_ERROR_BITMAP_FORMAT;
	}

	header->width = le32toh(h->width);
	header->height = le32toh(h->height);
	if (header->width == 0 || header->height == 0) {
		LOG("Invalid image width or height\n");
		return CBGFX_ERROR_BITMAP_DATA;
	}
	dim_org->width = header->width;
	dim_org->height = ABS(header->height);

	header->bits_per_pixel = le16toh(h->bits_per_pixel);
	header->compression = le32toh(h->compression);
	header->size = le32toh(h->size);
	header->colors_used = le32toh(h->colors_used);
	size_t palette_size = header->colors_used
			* sizeof(struct bitmap_palette_element_v3);
	size_t pixel_offset = file_header.bitmap_offset;
	if (pixel_offset > file_size) {
		LOG("Bitmap pixel data exceeds buffer boundary\n");
		return CBGFX_ERROR_BITMAP_DATA;
	}
	if (palette_offset + palette_size > pixel_offset) {
		LOG("Bitmap palette data exceeds palette boundary\n");
		return CBGFX_ERROR_BITMAP_DATA;
	}
	*palette = (struct bitmap_palette_element_v3 *)(bitmap +
			palette_offset);

	size_t pixel_size = header->size;
	if (pixel_size != dim_org->height *
		ROUNDUP(dim_org->width * header->bits_per_pixel / 8, 4)) {
		LOG("Bitmap pixel array size does not match expected size\n");
		return CBGFX_ERROR_BITMAP_DATA;
	}
	if (pixel_offset + pixel_size > file_size) {
		LOG("Bitmap pixel array exceeds buffer boundary\n");
		return CBGFX_ERROR_BITMAP_DATA;
	}
	*pixel_array = bitmap + pixel_offset;

	return CBGFX_SUCCESS;
}

/*
 * This calculates the dimension of the image projected on the canvas from the
 * dimension relative to the canvas size. If either width or height is zero, it
 * is derived from the other (non-zero) value to keep the aspect ratio.
 */
static int calculate_dimension(const struct vector *dim_org,
			       const struct scale *dim_rel,
			       struct vector *dim)
{
	if (dim_rel->x.n == 0 && dim_rel->y.n == 0)
		return CBGFX_ERROR_INVALID_PARAMETER;

	if (dim_rel->x.n > dim_rel->x.d || dim_rel->y.n > dim_rel->y.d)
		return CBGFX_ERROR_INVALID_PARAMETER;

	if (dim_rel->x.n > 0) {
		if (!is_valid_fraction(&dim_rel->x))
			return CBGFX_ERROR_INVALID_PARAMETER;
		dim->width = canvas.size.width  * dim_rel->x.n / dim_rel->x.d;
	}
	if (dim_rel->y.n > 0) {
		if (!is_valid_fraction(&dim_rel->y))
			return CBGFX_ERROR_INVALID_PARAMETER;
		dim->height = canvas.size.height * dim_rel->y.n / dim_rel->y.d;
	}

	/* Derive height from width using aspect ratio */
	if (dim_rel->y.n == 0)
		dim->height = dim->width * dim_org->height / dim_org->width;
	/* Derive width from height using aspect ratio */
	if (dim_rel->x.n == 0)
		dim->width = dim->height * dim_org->width / dim_org->height;

	return CBGFX_SUCCESS;
}

static int calculate_position(const struct vector *dim,
			      const struct scale *pos_rel, uint8_t pivot,
			      struct vector *top_left)
{
	int rv;

	rv = transform_vector(top_left, &canvas.size, pos_rel, &canvas.offset);
	if (rv)
		return rv;

	switch (pivot & PIVOT_H_MASK) {
	case PIVOT_H_LEFT:
		break;
	case PIVOT_H_CENTER:
		top_left->x -= dim->width / 2;
		break;
	case PIVOT_H_RIGHT:
		top_left->x -= dim->width;
		break;
	default:
		return CBGFX_ERROR_INVALID_PARAMETER;
	}

	switch (pivot & PIVOT_V_MASK) {
	case PIVOT_V_TOP:
		break;
	case PIVOT_V_CENTER:
		top_left->y -= dim->height / 2;
		break;
	case PIVOT_V_BOTTOM:
		top_left->y -= dim->height;
		break;
	default:
		return CBGFX_ERROR_INVALID_PARAMETER;
	}

	return CBGFX_SUCCESS;
}

static int check_boundary(const struct vector *top_left,
			  const struct vector *dim,
			  const struct rect *bound)
{
	struct vector v;
	add_vectors(&v, dim, top_left);
	if (top_left->x < bound->offset.x
			|| top_left->y < bound->offset.y
			|| within_box(&v, bound) < 0)
		return CBGFX_ERROR_BOUNDARY;
	return CBGFX_SUCCESS;
}

int draw_bitmap(const void *bitmap, size_t size,
		const struct scale *pos_rel, const struct scale *dim_rel,
		uint32_t flags)
{
	struct bitmap_header_v3 header;
	const struct bitmap_palette_element_v3 *palette;
	const uint8_t *pixel_array;
	struct vector top_left, dim, dim_org;
	int rv;
	const uint8_t pivot = flags & PIVOT_MASK;
	const uint8_t invert = (flags & INVERT_COLORS) >> INVERT_SHIFT;

	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	/* only v3 is supported now */
	rv = parse_bitmap_header_v3(bitmap, size,
				    &header, &palette, &pixel_array, &dim_org);
	if (rv)
		return rv;

	/* Calculate height and width of the image */
	rv = calculate_dimension(&dim_org, dim_rel, &dim);
	if (rv)
		return rv;

	/* Calculate coordinate */
	rv = calculate_position(&dim, pos_rel, pivot, &top_left);
	if (rv)
		return rv;

	rv = check_boundary(&top_left, &dim, &canvas);
	if (rv) {
		LOG("Bitmap image exceeds canvas boundary\n");
		return rv;
	}

	return draw_bitmap_v3(&top_left, &dim, &dim_org,
			      &header, palette, pixel_array, invert);
}

int draw_bitmap_direct(const void *bitmap, size_t size,
		       const struct vector *top_left)
{
	struct bitmap_header_v3 header;
	const struct bitmap_palette_element_v3 *palette;
	const uint8_t *pixel_array;
	struct vector dim;
	int rv;

	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	/* only v3 is supported now */
	rv = parse_bitmap_header_v3(bitmap, size,
				    &header, &palette, &pixel_array, &dim);
	if (rv)
		return rv;

	rv = check_boundary(top_left, &dim, &screen);
	if (rv) {
		LOG("Bitmap image exceeds screen boundary\n");
		return rv;
	}

	return draw_bitmap_v3(top_left, &dim, &dim,
			      &header, palette, pixel_array, 0);
}

int get_bitmap_dimension(const void *bitmap, size_t sz, struct scale *dim_rel)
{
	struct bitmap_header_v3 header;
	const struct bitmap_palette_element_v3 *palette;
	const uint8_t *pixel_array;
	struct vector dim, dim_org;
	int rv;

	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	/* Only v3 is supported now */
	rv = parse_bitmap_header_v3(bitmap, sz,
				    &header, &palette, &pixel_array, &dim_org);
	if (rv)
		return rv;

	/* Calculate height and width of the image */
	rv = calculate_dimension(&dim_org, dim_rel, &dim);
	if (rv)
		return rv;

	/* Calculate size relative to the canvas */
	dim_rel->x.n = dim.width;
	dim_rel->x.d = canvas.size.width;
	dim_rel->y.n = dim.height;
	dim_rel->y.d = canvas.size.height;

	return CBGFX_SUCCESS;
}

int enable_graphics_buffer(void)
{
	if (gfx_buffer)
		return CBGFX_SUCCESS;

	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	size_t buffer_size = fbinfo->y_resolution * fbinfo->bytes_per_line;
	gfx_buffer = malloc(buffer_size);
	if (!gfx_buffer) {
		LOG("%s: Failed to create graphics buffer (%zu bytes).\n",
		    __func__, buffer_size);
		return CBGFX_ERROR_GRAPHICS_BUFFER;
	}

	return CBGFX_SUCCESS;
}

int flush_graphics_buffer(void)
{
	if (!gfx_buffer)
		return CBGFX_ERROR_GRAPHICS_BUFFER;

	memcpy(REAL_FB, gfx_buffer, fbinfo->y_resolution * fbinfo->bytes_per_line);
	return CBGFX_SUCCESS;
}

void disable_graphics_buffer(void)
{
	free(gfx_buffer);
	gfx_buffer = NULL;
}
