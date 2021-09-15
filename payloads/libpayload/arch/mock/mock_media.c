/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>

int libpayload_init_default_cbfs_media(struct cbfs_media *media);

__attribute__((weak)) int libpayload_init_default_cbfs_media(struct cbfs_media *media)
{
	return -1;
}
