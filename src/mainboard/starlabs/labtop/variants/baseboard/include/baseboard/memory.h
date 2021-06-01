/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MEMORY_H
#define MEMORY_H

u8 get_memory_config_straps(void);
const struct cnl_mb_cfg *get_memory_cfg(struct cnl_mb_cfg *mem_cfg);

#endif
