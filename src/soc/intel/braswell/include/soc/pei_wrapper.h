/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_PEI_WRAPPER_H_
#define _SOC_PEI_WRAPPER_H_

#include <soc/pei_data.h>

typedef int ABI_X86(*pei_wrapper_entry_t)(struct pei_data *pei_data);

void broadwell_fill_pei_data(struct pei_data *pei_data);
void mainboard_fill_pei_data(struct pei_data *pei_data);

#endif
