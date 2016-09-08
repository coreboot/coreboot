/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <arch/io.h>
#include <console/console.h>
#include <soc/digest.h>
#include <string.h>

struct digest_ops digest_callbacks = {
	.init = (security_init_F) MVMAP2315_DIGEST_INIT,
	.sha_hash = (sha_hash_F) MVMAP2315_DIGEST_SHA_HASH,
};

u32 digest_init(void)
{
	int rc;

	rc = digest_callbacks.init(0);

	if (rc)
		printk(BIOS_DEBUG, "digest_init failed with rc=%x.\n", rc);

	return rc;
}

u32 digest_sha_hash(const u8 *msg, u32 msg_len, u8 *digest, u32 digest_len)
{
	int rc;

	rc = digest_callbacks.sha_hash(msg, msg_len, digest, digest_len);

	if (rc)
		printk(BIOS_DEBUG, "digest_sha_hash failed with rc=%x.\n", rc);

	return rc;
}

u32 digest_cmp(const u8 *msg, u8 *digest, u32 digest_len)
{
	int rc;

	rc = memcmp(msg, digest, digest_len);

	if (rc)
		printk(BIOS_DEBUG, "image hash doesn't match BDB expected");

	return rc;
}
