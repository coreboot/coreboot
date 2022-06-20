/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BOOT_GUARD_LIB_H_
#define _BOOT_GUARD_LIB_H_

#include <types.h>

/* Boot Policy configuration for Boot Guard */
struct boot_policy {
	uint16_t pbe:		1;
	uint16_t bbi:		1;
	uint16_t vb:		1;
	uint16_t mb:		1;
	uint16_t kmid:		4;
	uint16_t dcd:		1;
	uint16_t sb_s3_opt:	1;
	uint16_t res:		6;
};

/* Boot Policy info populated by TXE */
struct boot_policy_manifest {
	struct boot_policy bpm;
	uint8_t	txe_hash[SHA256_DIGEST_SIZE];
	uint8_t	ibbl_hash[32];
	uint8_t ibb_hash[32];
};

bool fetch_pre_rbp_data(struct boot_policy_manifest *bpm_info);
void fetch_post_rbp_data(struct boot_policy_manifest *bpm_info);

#endif /* _BOOT_GUARD_LIB_H_ */
