/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#ifndef _NB_CIMX_H_
#define _NB_CIMX_H_

/**
 * @brief disable GPP1 Port0,1, GPP2, GPP3a Port0,1,2,3,4,5, GPP3b
 *
 * SR5650/5670/5690 RD890 chipset, read pci config space hang at POR,
 * Disable all Pcie Bridges to work around It.
 */
void sr56x0_rd890_disable_pcie_bridge(void);

/**
 * Northbridge CIMX entries point
 */
void nb_Poweron_Init(void);
void nb_Ht_Init(void);
void nb_S3_Init(void);
void nb_Early_Post_Init(void);
void nb_Mid_Post_Init(void);
void nb_Late_Post_Init(void);
void nb_Pcie_Early_Init(void);
void nb_Pcie_Late_Init(void);

#endif//_RD890_EARLY_H_

