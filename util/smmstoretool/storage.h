/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SMMSTORETOOL__STORAGE_H__
#define SMMSTORETOOL__STORAGE_H__

#include "vs.h"
#include "utils.h"

struct storage_t {
	bool rw;
	struct mem_range_t file;
	struct mem_range_t region;
	struct mem_range_t store_area;
	struct var_store_t vs;
};

bool storage_open(const char store_file[], struct storage_t *storage, bool rw);

bool storage_write_back(struct storage_t *storage);

void storage_drop(struct storage_t *storage);

#endif // SMMSTORETOOL__STORAGE_H__
