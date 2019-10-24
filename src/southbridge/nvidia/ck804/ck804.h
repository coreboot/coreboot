/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
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

#ifndef SOUTHBRIDGE_NVIDIA_CK804_CK804_H
#define SOUTHBRIDGE_NVIDIA_CK804_CK804_H

#if CONFIG_HT_CHAIN_END_UNITID_BASE < CONFIG_HT_CHAIN_UNITID_BASE
#define CK804_DEVN_BASE CONFIG_HT_CHAIN_END_UNITID_BASE
#else
#define CK804_DEVN_BASE CONFIG_HT_CHAIN_UNITID_BASE
#endif

#define CK804B_BUSN 0x80
#define CK804B_DEVN_BASE (!CONFIG(SB_HT_CHAIN_UNITID_OFFSET_ONLY) ? CK804_DEVN_BASE : 1)

void enable_fid_change_on_sb(unsigned int sbbusn, unsigned int sbdn);

#endif
