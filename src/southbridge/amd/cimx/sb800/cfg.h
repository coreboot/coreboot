/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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


#ifndef _SB800_CFG_H_
#define _SB800_CFG_H_

#include <stdint.h>

/**
 * @brief South Bridge CIMx configuration
 *
 */
void sb800_cimx_config(AMDSBCFG *sb_cfg);

/**
 * @brief Entry point of Southbridge CIMx callout
 *
 * prototype UINT32 (*SBCIM_HOOK_ENTRY)(UINT32 Param1, UINT32 Param2, void* pConfig)
 *
 * @param[in] func    Southbridge CIMx Function ID.
 * @param[in] data    Southbridge Input Data.
 * @param[in] sb_cfg  Southbridge configuration structure pointer.
 *
 */
u32 sb800_callout_entry(u32 func, u32 data, void* sb_cfg);

#endif
