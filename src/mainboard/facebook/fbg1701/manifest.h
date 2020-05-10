/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MANIFEST_H__
#define __MANIFEST_H__

/**
 * Make sure the index matches the actual order in the manifest generated
 * using the HashCb.cmd file
 */
#define HASH_IDX_ROM_STAGE	0
#define HASH_IDX_RAM_STAGE	1
#define HASH_IDX_PAYLOAD	2
#define HASH_IDX_OPROM		3
#define HASH_IDX_FSP		4
#define HASH_IDX_MICROCODE	5
#define HASH_IDX_SPD0		6
#define HASH_IDX_LOGO		7
#define HASH_IDX_DSDT		8
#define HASH_IDX_POSTCAR_STAGE	9
#define HASH_IDX_PUBLICKEY	10
#define HASH_IDX_BOOTBLOCK	11 /* Should always be the last one */
#endif
