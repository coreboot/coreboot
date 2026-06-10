/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _AMD_PSP_COMBO_DIRECTORY_H_
#define  _AMD_PSP_COMBO_DIRECTORY_H_

#include <commonlib/bsd/compiler.h>
#include <stdint.h>

/*
 * AMD PSP directory structures.
 * Document #55758.
 */
struct psp_combo_header {
	uint32_t cookie;
	uint32_t checksum;
	uint32_t num_entries;
	uint32_t lookup;
	uint64_t reserved[2];
} __packed __aligned(16);

struct psp_combo_entry {
	uint32_t id_sel;
	uint32_t id;
	uint64_t lvl2_addr;
} __packed;

struct psp_combo_directory {
	struct psp_combo_header header;
	struct psp_combo_entry entries[];
} __packed __aligned(16);

#define PSP2_COOKIE 0x50535032		/* 'PSP2' */
#define BHD2_COOKIE 0x44484232		/* 'DHB2' */

#endif  /* _AMD_PSP_COMBO_DIRECTORY_H_ */
