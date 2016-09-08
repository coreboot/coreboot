/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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
#ifndef __SOC_MARVELL_MVMAP2315_NVM_H__
#define __SOC_MARVELL_MVMAP2315_NVM_H__

#include <stdint.h>

#define MVMAP2315_NVM_LOCKDOWN_FLAG	BIT(0)

u32 nvm_init(void);
u32 nvm_read(u32 offset, u32 *buffer, u32 size);
u32 nvm_write(u32 offset, u32 *buffer, u32 size);
void nvm_lockdown(void);

#endif /* __SOC_MARVELL_MVMAP2315_NVM_H__ */
