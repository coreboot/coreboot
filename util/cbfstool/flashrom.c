/* Copyright 2020 The ChromiumOS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdbool.h>

#include "common.h" /* from cbfstool for buffer API. */
#include <flashrom.h>

int flashrom_host_read(struct buffer *buffer, const char *region)
{
	struct firmware_image image = {
		.programmer = FLASHROM_PROGRAMMER_INTERNAL_AP,
	};

	if (flashrom_read_region(&image, region, 0) != VB2_SUCCESS)
		return -1;

	buffer_init(buffer, image.file_name, image.data, image.size);
	return 0;
}

int flashrom_host_write(struct buffer *buffer, const char *region)
{
	struct firmware_image image = {
		.programmer = FLASHROM_PROGRAMMER_INTERNAL_AP,
		.data = buffer_get(buffer),
		.size = buffer_size(buffer),
	};

	if (flashrom_write_region(&image, region, true, 0) != VB2_SUCCESS)
		return -1;

	return 0;
}
