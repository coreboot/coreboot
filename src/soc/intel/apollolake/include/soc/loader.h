/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_LOADER_H_
#define _SOC_LOADER_H_

/* HECI 1 offsets MMIO */
#define HOST2CSE		0x70
#define CSE2HOST		0x60

#define CSE_RBP_LIMIT		0x9be2
#define FIRST_CHUNK		0x8000

bool load_ibb(uint32_t ibb_dst, uint32_t ibb_size);

#endif
