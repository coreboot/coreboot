/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef RAMINIT_H
#define RAMINIT_H

#include "pei_data.h"

void sdram_initialize(struct pei_data *pei_data);
int fixup_haswell_errata(void);
/* save_mrc_data() must be called after cbmem has been initialized. */
void save_mrc_data(struct pei_data *pei_data);

#endif				/* RAMINIT_H */
