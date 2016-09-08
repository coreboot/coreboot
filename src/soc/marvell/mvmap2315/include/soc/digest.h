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

#ifndef __SOC_MARVELL_MVMAP2315_DIGEST_H__
#define __SOC_MARVELL_MVMAP2315_DIGEST_H__

#include <stdint.h>

#define MVMAP2315_DIGEST_INIT		0xFFE00040
#define MVMAP2315_DIGEST_SHA_HASH	0xFFE00044

typedef u32 (*security_init_F)(u32 adv_ver);
typedef u32 (*sha_hash_F)(const u8 *msg, u32 msg_len, u8 *digest,
			  u32 digest_len);

struct digest_ops {
	security_init_F init;
	sha_hash_F sha_hash;
};

u32 digest_init(void);
u32 digest_sha_hash(const u8 *msg, u32 msg_len, u8 *digest, u32 digest_len);
u32 digest_cmp(const u8 *msg, u8 *digest, u32 digest_len);

#endif /* __SOC_MARVELL_MVMAP2315_DIGEST_H__ */
