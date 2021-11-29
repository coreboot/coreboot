/* SPDX-License-Identifier: BSD-3-Clause */

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <libflashrom.h>

#include "uflashrom.h"

static int flashrom_print_cb(enum flashrom_log_level level, const char *fmt, va_list ap)
{
	int ret = 0;
	FILE *output_type = stderr;

	if (level > FLASHROM_MSG_INFO)
		return ret;

	ret = vfprintf(output_type, fmt, ap);
	/* msg_*spew often happens inside chip accessors
	 * in possibly time-critical operations.
	 * If increasing verbosity, don't slow them down by flushing.
	 */
	fflush(output_type);

	return ret;
}

static size_t resize_buf_to_offset(uint8_t **buf, unsigned int start, unsigned int len)
{
	uint8_t *old = *buf; // make a copy to free the old heap.

	*buf = calloc(1, len);
	memcpy(*buf, &old[start], len);
	free(old);

	return len;
}

static uint8_t *resize_buf_from_offset(const uint8_t *buf, size_t len, unsigned int rstart,
				       unsigned int rlen)
{
	size_t nlen = rstart + rlen;
	if (nlen > len)
		return NULL;

	uint8_t *nbuf = calloc(1, len); /* NOTE: full len buf required for writes. */
	memcpy(nbuf + rstart, buf, rlen);

	return nbuf;
}

/**
 * @brief Reads from flash into a buffer with an optional region.
 *
 * @param image, containing the programmer to use, unallocated buffer and size.
 * @param region, (optional) the string of the region to read from.
 * @return 0 on success
 */
int flashrom_read(struct firmware_programmer *image, const char *region)
{
	int r = 0;
	size_t len = 0;

	struct flashrom_programmer *prog = NULL;
	struct flashrom_flashctx *flashctx = NULL;
	struct flashrom_layout *layout = NULL;

	flashrom_set_log_callback((flashrom_log_callback *)&flashrom_print_cb);

	r |= flashrom_init(1);
	r |= flashrom_programmer_init(&prog, image->programmer, NULL);
	r |= flashrom_flash_probe(&flashctx, prog, NULL);
	if (r) {
		r = -1;
		goto err_cleanup;
	}

	len = flashrom_flash_getsize(flashctx);
	if (region) {
		r = flashrom_layout_read_fmap_from_rom(&layout, flashctx, 0, len);
		if (r > 0) {
			fprintf(stderr, "could not read fmap from rom, r=%d\n", r);
			r = -1;
			goto err_cleanup;
		}
		/* empty region causes seg fault in API. */
		r |= flashrom_layout_include_region(layout, region);
		if (r > 0) {
			fprintf(stderr, "could not include region = '%s'\n", region);
			r = -1;
			goto err_cleanup;
		}
		flashrom_layout_set(flashctx, layout);
	}
	/* Due to how the libflashrom API works we first need a buffer sized
	 * to the entire flash and after the read has finished, find the
	 * the precise region size then resize the buffer accordingly.
	 */
	image->data = calloc(1, len);
	image->size = len;

	r |= flashrom_image_read(flashctx, image->data, len);

	/* Here we resize the buffer from being the entire flash down to the specific
	 * region size read and that we were interested in. Note that we only include
	 * a singular region.
	 */
	if (region) {
		unsigned int r_start, r_len;
		flashrom_layout_get_region_range(layout, region, &r_start, &r_len);
		image->size = resize_buf_to_offset(&image->data, r_start, r_len);
	}

err_cleanup:
	flashrom_programmer_shutdown(prog);
	if (layout)
		flashrom_layout_release(layout);
	if (flashctx)
		flashrom_flash_release(flashctx);

	return r;
}

/**
 * @brief Writes flash from a buffer with an optional region.
 *
 * @param image, containing the programmer to use, allocated buffer and its size.
 * @param region, (optional) the string of the region to write to.
 * @return 0 on success
 */
int flashrom_write(struct firmware_programmer *image, const char *region)
{
	int r = 0;
	size_t len = 0;
	uint8_t *buf = image->data;

	struct flashrom_programmer *prog = NULL;
	struct flashrom_flashctx *flashctx = NULL;
	struct flashrom_layout *layout = NULL;

	flashrom_set_log_callback((flashrom_log_callback *)&flashrom_print_cb);

	r |= flashrom_init(1);
	r |= flashrom_programmer_init(&prog, image->programmer, NULL);
	r |= flashrom_flash_probe(&flashctx, prog, NULL);
	if (r) {
		r = -1;
		goto err_cleanup;
	}

	len = flashrom_flash_getsize(flashctx);
	if (len == 0) {
		fprintf(stderr, "zero sized flash detected\n");
		r = -1;
		goto err_cleanup;
	}
	if (region) {
		r = flashrom_layout_read_fmap_from_buffer(
			&layout, flashctx, (const uint8_t *)image->data, image->size);
		if (r > 0) {
			r = flashrom_layout_read_fmap_from_rom(&layout, flashctx, 0, len);
			if (r > 0) {
				fprintf(stderr, "could not read fmap from image or rom, r=%d\n",
					r);
				r = -1;
				goto err_cleanup;
			}
		}
		/* empty region causes seg fault in API. */
		r |= flashrom_layout_include_region(layout, region);
		if (r > 0) {
			fprintf(stderr, "could not include region = '%s'\n", region);
			r = -1;
			goto err_cleanup;
		}
		flashrom_layout_set(flashctx, layout);

		unsigned int r_start, r_len;
		flashrom_layout_get_region_range(layout, region, &r_start, &r_len);
		assert(r_len == image->size);
		buf = resize_buf_from_offset(image->data, len, r_start, r_len);
		if (!buf) {
			r = -1;
			goto err_cleanup_free;
		}
	} else if (image->size != len) {
		r = -1;
		goto err_cleanup;
	}

	flashrom_flag_set(flashctx, FLASHROM_FLAG_VERIFY_WHOLE_CHIP, false);
	flashrom_flag_set(flashctx, FLASHROM_FLAG_VERIFY_AFTER_WRITE, true);

	r |= flashrom_image_write(flashctx, buf, len, NULL);

err_cleanup_free:
	if (region)
		free(buf);
err_cleanup:
	flashrom_programmer_shutdown(prog);
	if (layout)
		flashrom_layout_release(layout);
	if (flashctx)
		flashrom_flash_release(flashctx);

	return r;
}
