/*
 * This file is part of the libpayload project.
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

/*
 * Framebuffer is assumed to assign a higher coordinate (larger x, y) to
 * a higher address
 */
static struct cb_framebuffer *fbinfo;
static uint8_t *fbaddr;

#define LOG(x...)	printf("CBGFX: " x)
#define PIVOT_H_MASK	(PIVOT_H_LEFT|PIVOT_H_CENTER|PIVOT_H_RIGHT)
#define PIVOT_V_MASK	(PIVOT_V_TOP|PIVOT_V_CENTER|PIVOT_V_BOTTOM)
#define ROUNDUP(x, y)	((((x) + ((y) - 1)) / (y)) * (y))
#define ABS(x)		((x) < 0 ? -(x) : (x))

static char initialized = 0;

static const struct vector vzero = {
	.x = 0,
	.y = 0,
};

static void add_vectors(struct vector *out,
			const struct vector *v1, const struct vector *v2)
{
	out->x = v1->x + v2->x;
	out->y = v1->y + v2->y;
}

static int is_valid_fraction(const struct fraction *f)
{
	return f->d != 0;
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
	if (!is_valid_fraction(&a->x) || !is_valid_fraction(&a->y))
		return CBGFX_ERROR_INVALID_PARAMETER;
	out->x = a->x.n * in->x / a->x.d + offset->x;
	out->y = a->y.n * in->y / a->y.d + offset->y;
	return CBGFX_SUCCESS;
}

/*
 * Returns 1 if v is exclusively within box, 0 if v is inclusively within box,
 * or -1 otherwise. Note that only the right and bottom edges are examined.
 */
static int within_box(const struct vector *v, const struct rect *bound)
{
	if (v->x < bound->offset.x + bound->size.width &&
			v->y < bound->offset.y + bound->size.height)
		return 1;
	else if (v->x <= bound->offset.x + bound->size.width &&
			v->y <= bound->offset.y + bound->size.height)
		return 0;
	else
		return -1;
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
	uint8_t * const pixel = fbaddr + (coord->x +
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
		return CBGFX_ERROR_FRAMEBUFFER_INFO;

	fbaddr = phys_to_virt((uint8_t *)(uintptr_t)(fbinfo->physical_address));
	if (!fbaddr)
		return CBGFX_ERROR_FRAMEBUFFER_ADDR;

	screen.size.width = fbinfo->x_resolution;
	screen.size.height = fbinfo->y_resolution;
	screen.offset.x = 0;
	screen.offset.y = 0;

	/* Calculate canvas size & offset, assuming the screen is landscape */
	if (screen.size.height > screen.size.width) {
		LOG("Portrait screen not supported\n");
		return CBGFX_ERROR_PORTRAIT_SCREEN;
	}
	canvas.size.height = screen.size.height;
	canvas.size.width = canvas.size.height;
	canvas.offset.x = (screen.size.width - canvas.size.width) / 2;
	canvas.offset.y = 0;

	initialized = 1;
	LOG("cbgfx initialized: screen:width=%d, height=%d, offset=%d canvas:width=%d, height=%d, offset=%d\n",
	    screen.size.width, screen.size.height, screen.offset.x,
	    canvas.size.width, canvas.size.height, canvas.offset.x);

	return 0;
}

int draw_box(const struct rect *box, const struct rgb_color *rgb)
{
	struct vector top_left;
	struct vector size;
	struct vector p, t;
	const uint32_t color = calculate_color(rgb);
	const struct scale top_left_s = {
		.x = { .n = box->offset.x, .d = CANVAS_SCALE, },
		.y = { .n = box->offset.y, .d = CANVAS_SCALE, }
	};
	const struct scale size_s = {
		.x = { .n = box->size.x, .d = CANVAS_SCALE, },
		.y = { .n = box->size.y, .d = CANVAS_SCALE, }
	};

	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	transform_vector(&top_left, &canvas.size, &top_left_s, &canvas.offset);
	transform_vector(&size, &canvas.size, &size_s, &vzero);
	add_vectors(&t, &top_left, &size);
	if (within_box(&t, &canvas) < 0) {
		LOG("Box exceeds canvas boundary\n");
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

	struct vector p;
	uint32_t color = calculate_color(rgb);
	const int bpp = fbinfo->bits_per_pixel;
	const int bpl = fbinfo->bytes_per_line;

	/* If all significant bytes in color are equal, fastpath through memset.
	 * We assume that for 32bpp the high byte gets ignored anyway. */
	if ((((color >> 8) & 0xff) == (color & 0xff)) && (bpp == 16 ||
	    (((color >> 16) & 0xff) == (color & 0xff)))) {
		memset(fbaddr, color & 0xff, screen.size.height * bpl);
	} else {
		for (p.y = 0; p.y < screen.size.height; p.y++)
			for (p.x = 0; p.x < screen.size.width; p.x++)
				set_pixel(&p, color);
	}

	return CBGFX_SUCCESS;
}

/*
 * Bi-linear Interpolation
 *
 * It estimates the value of a middle point (tx, ty) using the values from four
 * adjacent points (q00, q01, q10, q11).
 */
static uint32_t bli(uint32_t q00, uint32_t q10, uint32_t q01, uint32_t q11,
		    struct fraction *tx, struct fraction *ty)
{
	uint32_t r0 = (tx->n * q10 + (tx->d - tx->n) * q00) / tx->d;
	uint32_t r1 = (tx->n * q11 + (tx->d - tx->n) * q01) / tx->d;
	uint32_t p = (ty->n * r1 + (ty->d - ty->n) * r0) / ty->d;
	return p;
}

static int draw_bitmap_v3(const struct vector *top_left,
			  const struct scale *scale,
			  const struct vector *dim,
			  const struct vector *dim_org,
			  const struct bitmap_header_v3 *header,
			  const struct bitmap_palette_element_v3 *pal,
			  const uint8_t *pixel_array)
{
	const int bpp = header->bits_per_pixel;
	int32_t dir;
	struct vector p;

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
	if (scale->x.n == 0 || scale->y.n == 0) {
		LOG("Scaling out of range\n");
		return CBGFX_ERROR_SCALE_OUT_OF_RANGE;
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
	/*
	 * Plot pixels scaled by the bilinear interpolation. We scan over the
	 * image on canvas (using d) and find the corresponding pixel in the
	 * bitmap data (using s0, s1).
	 *
	 * When d hits the right bottom corner, s0 also hits the right bottom
	 * corner of the pixel array because that's how scale->x and scale->y
	 * have been set. Since the pixel array size is already validated in
	 * parse_bitmap_header_v3, s0 is guranteed not to exceed pixel array
	 * boundary.
	 */
	struct vector s0, s1, d;
	struct fraction tx, ty;
	for (d.y = 0; d.y < dim->height; d.y++, p.y += dir) {
		s0.y = d.y * scale->y.d / scale->y.n;
		s1.y = s0.y;
		if (s0.y + 1 < dim_org->height)
			s1.y++;
		ty.d = scale->y.n;
		ty.n = (d.y * scale->y.d) % scale->y.n;
		const uint8_t *data0 = pixel_array + s0.y * y_stride;
		const uint8_t *data1 = pixel_array + s1.y * y_stride;
		p.x = top_left->x;
		for (d.x = 0; d.x < dim->width; d.x++, p.x++) {
			s0.x = d.x * scale->x.d / scale->x.n;
			s1.x = s0.x;
			if (s1.x + 1 < dim_org->width)
				s1.x++;
			tx.d = scale->x.n;
			tx.n = (d.x * scale->x.d) % scale->x.n;
			uint8_t c00 = data0[s0.x];
			uint8_t c10 = data0[s1.x];
			uint8_t c01 = data1[s0.x];
			uint8_t c11 = data1[s1.x];
			if (c00 >= header->colors_used
					|| c10 >= header->colors_used
					|| c01 >= header->colors_used
					|| c11 >= header->colors_used) {
				LOG("Color index exceeds palette boundary\n");
				return CBGFX_ERROR_BITMAP_DATA;
			}
			const struct rgb_color rgb = {
				.red = bli(pal[c00].red, pal[c10].red,
					   pal[c01].red, pal[c11].red,
					   &tx, &ty),
				.green = bli(pal[c00].green, pal[c10].green,
					     pal[c01].green, pal[c11].green,
					     &tx, &ty),
				.blue = bli(pal[c00].blue, pal[c10].blue,
					    pal[c01].blue, pal[c11].blue,
					    &tx, &ty),
			};
			set_pixel(&p, calculate_color(&rgb));
		}
	}

	return CBGFX_SUCCESS;
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
		const struct scale *pos_rel, uint8_t pivot,
		const struct scale *dim_rel)
{
	struct bitmap_header_v3 header;
	const struct bitmap_palette_element_v3 *palette;
	const uint8_t *pixel_array;
	struct vector top_left, dim, dim_org;
	struct scale scale;
	int rv;

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

	/* Calculate self scale */
	scale.x.n = dim.width;
	scale.x.d = dim_org.width;
	scale.y.n = dim.height;
	scale.y.d = dim_org.height;

	/* Calculate coordinate */
	rv = calculate_position(&dim, pos_rel, pivot, &top_left);
	if (rv)
		return rv;

	rv = check_boundary(&top_left, &dim, &canvas);
	if (rv) {
		LOG("Bitmap image exceeds canvas boundary\n");
		return rv;
	}

	return draw_bitmap_v3(&top_left, &scale, &dim, &dim_org,
			      &header, palette, pixel_array);
}

int draw_bitmap_direct(const void *bitmap, size_t size,
		       const struct vector *top_left)
{
	struct bitmap_header_v3 header;
	const struct bitmap_palette_element_v3 *palette;
	const uint8_t *pixel_array;
	struct vector dim;
	struct scale scale;
	int rv;

	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	/* only v3 is supported now */
	rv = parse_bitmap_header_v3(bitmap, size,
				    &header, &palette, &pixel_array, &dim);
	if (rv)
		return rv;

	/* Calculate self scale */
	scale.x.n = 1;
	scale.x.d = 1;
	scale.y.n = 1;
	scale.y.d = 1;

	rv = check_boundary(top_left, &dim, &screen);
	if (rv) {
		LOG("Bitmap image exceeds screen boundary\n");
		return rv;
	}

	return draw_bitmap_v3(top_left, &scale, &dim, &dim,
			      &header, palette, pixel_array);
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
