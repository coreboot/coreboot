/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2015 Google, Inc.
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

static char initialized = 0;
#define LOG(x...)	printf("CBGFX: " x)

/*
 * This is the range used internally to scale bitmap images. (e.g. 128 = 50%,
 * 512 = 200%). We choose 256 so that division and multiplication become bit
 * shift operation.
 */
#define BITMAP_SCALE_BASE	256

#define ROUNDUP(x, y)	((((x) + ((y) - 1)) / (y)) * (y))
#define ABS(x)		((x) < 0 ? -(x) : (x))

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

/*
 * Transform a vector:
 * 	x' = x * a_x + offset_x
 * 	y' = y * a_y + offset_y
 */
static void transform_vector(struct vector *out,
			     const struct vector *in,
			     const struct scale *a,
			     const struct vector *offset)
{
	out->x = a->x.nume * in->x / a->x.deno + offset->x;
	out->y = a->y.nume * in->y / a->y.deno + offset->y;
}

/*
 * Returns 1 if v is exclusively within box, 0 if v is inclusively within box,
 * or -1 otherwise. Note that only the left and bottom edges are considered.
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
		return -1;

	fbaddr = phys_to_virt((uint8_t *)(uintptr_t)(fbinfo->physical_address));
	if (!fbaddr)
		return -1;

	screen.size.width = fbinfo->x_resolution;
	screen.size.height = fbinfo->y_resolution;
	screen.offset.x = 0;
	screen.offset.y = 0;

	/* Calculate canvas size & offset, assuming the screen is landscape */
	if (screen.size.height > screen.size.width) {
		LOG("Portrait screen not supported\n");
		return -1;
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

void *load_bitmap(const char *name, size_t *size)
{
	static struct cbfs_media media;
	static int cbfs_media_initialized = 0;
	if (!cbfs_media_initialized) {
		if (init_default_cbfs_media(&media)) {
			printf("Failed to initialize default media\n");
			return NULL;
		}
	}
	cbfs_media_initialized = 1;
	return cbfs_get_file_content(&media, name, CBFS_TYPE_RAW, size);
}

int draw_box(const struct rect *box, const struct rgb_color *rgb)
{
	struct vector top_left;
	struct vector size;
	struct vector p, t;
	const uint32_t color = calculate_color(rgb);
	const struct scale top_left_s = {
		.x = { .nume = box->offset.x, .deno = CANVAS_SCALE, },
		.y = { .nume = box->offset.y, .deno = CANVAS_SCALE, }
	};
	const struct scale size_s = {
		.x = { .nume = box->size.x, .deno = CANVAS_SCALE, },
		.y = { .nume = box->size.y, .deno = CANVAS_SCALE, }
	};

	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	transform_vector(&top_left, &canvas.size, &top_left_s, &canvas.offset);
	transform_vector(&size, &canvas.size, &size_s, &vzero);
	add_vectors(&t, &top_left, &size);
	if (within_box(&t, &canvas) < 0) {
		LOG("Box exceeds canvas boundary\n");
		return CBGFX_ERROR_CANVAS_BOUNDARY;
	}

	for (p.y = top_left.y; p.y < t.y; p.y++)
		for (p.x = top_left.x; p.x < t.x; p.x++)
			set_pixel(&p, color);

	return CBGFX_SUCCESS;
}

int clear_canvas(struct rgb_color *rgb)
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

int clear_screen(struct rgb_color *rgb)
{
	uint32_t color;
	struct vector p;

	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	color = calculate_color(rgb);
	for (p.y = 0; p.y < screen.size.height; p.y++)
		for (p.x = 0; p.x < screen.size.width; p.x++)
			set_pixel(&p, color);

	return CBGFX_SUCCESS;
}

/*
 * This check guarantees we will not try to read outside pixel data.
 */
static int check_bound(const struct vector *image,
		       const struct bitmap_header_v3 *header,
		       const struct scale *scale)
{
	struct vector p = {
		.x = (image->width - 1) * scale->x.deno / scale->x.nume,
		.y = (image->height -1) * scale->y.deno / scale->y.nume,
	};
	struct rect bound = {
		.offset = vzero,
		.size = { .width = header->width, .height = header->height, },
	};
	return within_box(&p, &bound) < 0;
}

static uint32_t bli(uint32_t q00, uint32_t q10, uint32_t q01, uint32_t q11,
		    struct fraction *tx, struct fraction *ty)
{
	uint32_t r0 = (tx->nume * q10 + (tx->deno - tx->nume) * q00) / tx->deno;
	uint32_t r1 = (tx->nume * q11 + (tx->deno - tx->nume) * q01) / tx->deno;
	uint32_t p = (ty->nume * r1 + (ty->deno - ty->nume) * r0) / ty->deno;
	return p;
}

static int draw_bitmap_v3(const struct vector *top_left,
			  const struct scale *scale,
			  const struct vector *image,
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
	if (scale->x.nume == 0 || scale->y.nume == 0) {
		LOG("Scaling out of range\n");
		return CBGFX_ERROR_SCALE_OUT_OF_RANGE;
	}

	if (check_bound(image, header, scale))
		return CBGFX_ERROR_SCALE_OUT_OF_RANGE;

	const int32_t y_stride = ROUNDUP(header->width * bpp / 8, 4);
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
		p.y += image->height - 1;
		dir = -1;
	}
	/*
	 * Plot pixels scaled by the bilinear interpolation. We scan
	 * over the image on canvas (using d) and find the corresponding pixel
	 * in the bitmap data (using s).
	 */
	struct vector s0, s1, d;
	struct fraction tx, ty;
	for (d.y = 0; d.y < image->height; d.y++, p.y += dir) {
		s0.y = d.y * scale->y.deno / scale->y.nume;
		s1.y = s0.y;
		if (s0.y + 1 < ABS(header->height))
			s1.y++;
		ty.deno = scale->y.nume;
		ty.nume = (d.y * scale->y.deno) % scale->y.nume;
		const uint8_t *data0 = pixel_array + s0.y * y_stride;
		const uint8_t *data1 = pixel_array + s1.y * y_stride;
		p.x = top_left->x;
		for (d.x = 0; d.x < image->width; d.x++, p.x++) {
			s0.x = d.x * scale->x.deno / scale->x.nume;
			s1.x = s0.x;
			if (s1.x + 1 < header->width)
				s1.x++;
			tx.deno = scale->x.nume;
			tx.nume = (d.x * scale->x.deno) % scale->x.nume;
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

static int parse_bitmap_header_v3(const uint8_t *bitmap,
			const struct bitmap_file_header *file_header,
			/* ^--- IN / OUT ---v */
			struct bitmap_header_v3 *header,
			const struct bitmap_palette_element_v3 **palette,
			const uint8_t **pixel_array)
{
	struct bitmap_header_v3 *h;
	size_t header_offset = sizeof(struct bitmap_file_header);
	size_t header_size = sizeof(struct bitmap_header_v3);
	size_t palette_offset = header_offset + header_size;
	size_t file_size = file_header->file_size;

	h = (struct bitmap_header_v3 *)(bitmap + header_offset);
	header->header_size = le32toh(h->header_size);
	if (header->header_size != header_size) {
		LOG("Unsupported bitmap format\n");
		return CBGFX_ERROR_BITMAP_FORMAT;
	}
	header->width = le32toh(h->width);
	header->height = le32toh(h->height);
	header->bits_per_pixel = le16toh(h->bits_per_pixel);
	header->compression = le32toh(h->compression);
	header->size = le32toh(h->size);
	header->colors_used = le32toh(h->colors_used);
	size_t palette_size = header->colors_used
			* sizeof(struct bitmap_palette_element_v3);
	size_t pixel_offset = file_header->bitmap_offset;
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
	if (pixel_size != header->height *
		ROUNDUP(header->width * header->bits_per_pixel / 8, 4)) {
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

int draw_bitmap(const struct vector *top_left_rel,
		size_t scale_rel, const void *bitmap, size_t size)
{
	struct bitmap_file_header file_header;
	struct bitmap_header_v3 header;
	const struct bitmap_palette_element_v3 *palette;
	const uint8_t *pixel_array;
	struct vector top_left, image;
	struct scale scale;
	struct vector t;
	int rv;

	if (cbgfx_init())
		return CBGFX_ERROR_INIT;

	rv = get_bitmap_file_header(bitmap, size, &file_header);
	if (rv)
		return rv;

	/* only v3 is supported now */
	rv = parse_bitmap_header_v3(bitmap, &file_header,
				    &header, &palette, &pixel_array);
	if (rv)
		return rv;

	/*
	 * Calculate absolute coordinate and self-scale (scale relative to image
	 * size). If relative scale is zero, the image is displayed at the
	 * original scale and tol_left_rel is treated as absolute coordinate.
	 */
	if (scale_rel) {
		const struct scale s = {
			.x = { .nume = top_left_rel->x, .deno = CANVAS_SCALE, },
			.y = { .nume = top_left_rel->y, .deno = CANVAS_SCALE, },
		};
		transform_vector(&top_left, &canvas.size, &s, &canvas.offset);
		scale.x.nume = scale_rel * canvas.size.width;
		scale.x.deno = header.width * CANVAS_SCALE;
	} else {
		add_vectors(&top_left, top_left_rel, &vzero);
		scale.x.nume = 1;
		scale.x.deno = 1;
	}
	scale.y.nume = scale.x.nume;
	scale.y.deno = scale.x.deno;

	/* Calculate height and width of the image on screen */
	image.width = header.width;
	image.height = ABS(header.height);
	transform_vector(&image, &image, &scale, &vzero);

	/* Check whether the right bottom corner is within screen and canvas */
	add_vectors(&t, &image, &top_left);
	if (scale_rel && within_box(&t, &canvas) < 0) {
		LOG("Bitmap image exceeds canvas boundary\n");
		return CBGFX_ERROR_CANVAS_BOUNDARY;
	}
	if (within_box(&t, &screen) < 0) {
		LOG("Bitmap image exceeds screen boundary\n");
		return CBGFX_ERROR_SCREEN_BOUNDARY;
	}

	return draw_bitmap_v3(&top_left, &scale, &image,
			      &header, palette, pixel_array);
}
