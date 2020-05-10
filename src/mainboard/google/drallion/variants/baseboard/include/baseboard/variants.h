/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef BASEBOARD_VARIANTS_H
#define BASEBOARD_VARIANTS_H

#include <fsp/api.h>
#include <soc/cnl_memcfg_init.h>

void variant_mainboard_post_init_params(FSPM_UPD *mupd);

/* Return board specific memory configuration */
const struct cnl_mb_cfg *get_variant_memory_cfg(struct cnl_mb_cfg *mem_cfg);

#endif /* BASEBOARD_VARIANTS_H */
