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


#ifndef _CIMX_H_
#define _CIMX_H_

/**
 * AMD South Bridge CIMx entry point wrapper
 */
void sb_Poweron_Init(void);
void sb_Before_Pci_Init(void);
void sb_After_Pci_Init(void);
void sb_Mid_Post_Init(void);
void sb_Late_Post(void);
void sb_Before_Pci_Restore_Init(void);
void sb_After_Pci_Restore_Init(void);

int acpi_is_wakeup_early(void);

/**
 * CIMX not set the clock to 48Mhz until sbBeforePciInit,
 * coreboot may need to set this even more earlier
 */
void sb800_clk_output_48Mhz(void);

#if CONFIG_RAMINIT_SYSINFO
/**
 * @brief Get SouthBridge device number, called by finalize_node_setup()
 * @param[in] bus target bus number
 * @return southbridge device number
 */
u32 get_sbdn(u32 bus);
#endif
#endif
