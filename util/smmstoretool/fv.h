/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SMMSTORETOOL__FV_H__
#define SMMSTORETOOL__FV_H__

#include <stdbool.h>

#include "utils.h"

// Firmware volume is what's stored in SMMSTORE region of CBFS.  It wraps
// variable store.

bool fv_init(struct mem_range_t fv);

bool fv_parse(struct mem_range_t fv,
	      struct mem_range_t *var_store,
	      bool *is_auth_var_store);

#endif // SMMSTORETOOL__FV_H__
