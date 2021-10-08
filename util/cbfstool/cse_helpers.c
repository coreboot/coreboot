/* SPDX-License-Identifier: GPL-2.0-only */
/* Helper functions for cse_serger and cse_fpt */

#include <commonlib/endian.h>

#include "cse_helpers.h"

void write_member(struct buffer *buff, void *src, size_t size)
{
	void *dst = buffer_get(buff);

	switch (size) {
	case 1:
		write_le8(dst, *(uint8_t *)src);
		break;
	case 2:
		write_le16(dst, *(uint16_t *)src);
		break;
	case 4:
		write_le32(dst, *(uint32_t *)src);
		break;
	case 8:
		write_le64(dst, *(uint64_t *)src);
		break;
	default:
		memcpy(dst, src, size);
		break;
	}

	buffer_seek(buff, size);
}

void read_member(struct buffer *buff, void *dst, size_t size)
{
	const void *src = buffer_get(buff);

	switch (size) {
	case 1:
		*(uint8_t *)dst = read_le8(src);
		break;
	case 2:
		*(uint16_t *)dst = read_le16(src);
		break;
	case 4:
		*(uint32_t *)dst = read_le32(src);
		break;
	case 8:
		*(uint64_t *)dst = read_le64(src);
		break;
	default:
		memcpy(dst, src, size);
		break;
	}

	buffer_seek(buff, size);
}
