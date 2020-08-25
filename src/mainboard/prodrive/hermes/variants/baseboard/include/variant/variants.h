/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/cnl_memcfg_init.h>
#include <types.h>

/* Return memory configuration structure. */
const struct cnl_mb_cfg *variant_memcfg_config(void);

void mainboard_r0x_configure_alc888(u8 *base, u32 viddid);
