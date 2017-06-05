/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2017 Intel Corporation.
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

#ifndef _SOC_CANNONLAKE_PCH_H_
#define _SOC_CANNONLAKE_PCH_H_

#include <stdint.h>

#define PCH_H				1
#define PCH_LP				2
#define PCH_UNKNOWN_SERIES		0xFF

u8 pch_revision(void);
u16 pch_type(void);
void pch_log_state(void);
void pch_uart_init(void);

#endif
