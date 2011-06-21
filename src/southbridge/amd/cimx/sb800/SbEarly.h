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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#ifndef _CIMX_SB_EARLY_H_
#define _CIMX_SB_EARLY_H_

/**
 * @brief Get SouthBridge device number, called by finalize_node_setup()
 * @param[in] bus target bus number
 * @return southbridge device number
 */
u32 get_sbdn(u32 bus);

/**
 * South Bridge CIMx romstage entry, sbPowerOnInit entry point wrapper.
 */
void sb_poweron_init(void);
//void sb_before_pci_init(void);

#endif
