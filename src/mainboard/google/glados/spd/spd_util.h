/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef SPD_UTIL_H
#define SPD_UTIL_H

#include <fsp/soc_binding.h>

void spd_memory_init_params(FSPM_UPD *mupd, int spd_index);

#endif /* SPD_UTIL_H */
