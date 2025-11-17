/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8196_BOOKER_H
#define SOC_MEDIATEK_MT8196_BOOKER_H

#include <stdint.h>

#define MTE_TAG_ADDR 0x460E80000

void booker_init(void);
void booker_mte_init(uint64_t mte_tag_addr);

#endif
