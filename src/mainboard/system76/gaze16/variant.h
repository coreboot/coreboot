/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_H
#define VARIANT_H

#include <fsp/soc_binding.h>

void variant_memory_init_params(FSPM_UPD *mupd);
void variant_silicon_init_params(FSP_S_CONFIG *params);

#endif
