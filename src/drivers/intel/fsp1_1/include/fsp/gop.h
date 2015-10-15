/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
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
 * Foundation, Inc.
 */

#ifndef _FSP1_1_GOP_H_
#define _FSP1_1_GOP_H_

/* GOP support */
#if IS_ENABLED(CONFIG_GOP_SUPPORT)

#include <fsp/gma.h>

const optionrom_vbt_t *fsp_get_vbt(uint32_t *vbt_len);

#endif /* CONFIG_GOP_SUPPORT */
#endif /* _FSP_GOP_H_ */
