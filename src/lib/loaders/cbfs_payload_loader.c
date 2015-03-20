/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <cbfs.h>
#include <program_loading.h>

static int cbfs_locate_payload(struct prog *payload)
{
	void *buffer;
	size_t size;
	const int type = CBFS_TYPE_PAYLOAD;

	buffer = cbfs_get_file_content(CBFS_DEFAULT_MEDIA, payload->name,
					type, &size);

	if (buffer == NULL)
		return -1;

	prog_set_area(payload, buffer, size);

	return 0;
}

const struct prog_loader_ops cbfs_payload_loader = {
	.name = "CBFS",
	.prepare = cbfs_locate_payload,
};
