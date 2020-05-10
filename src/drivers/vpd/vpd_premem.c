/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>

#include "vpd.h"

void vpd_get_buffers(struct vpd_blob *blob)
{
	const struct vpd_blob *b;

	b = vpd_load_blob();
	memcpy(blob, b, sizeof(*b));
}
