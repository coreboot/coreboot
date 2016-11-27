/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Damien Zammit <damien@zamaudio.com>
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

#ifndef AMDFAM10_RAMINIT_H
#define AMDFAM10_RAMINIT_H

#include <device/pci.h>
#include <northbridge/amd/amdmct/amddefs.h>
#include <northbridge/amd/amdmct/wrappers/mcti.h>

struct sys_info;
struct DCTStatStruc;
struct MCTStatStruc;

int mctRead_SPD(u32 smaddr, u32 reg);
void mctSMBhub_Init(u32 node);
void mctGet_DIMMAddr(struct DCTStatStruc *pDCTstat, u32 node);
void raminit_amdmct(struct sys_info *sysinfo);
void amdmct_cbmem_store_info(struct sys_info *sysinfo);
void fill_mem_ctrl(u32 controllers, struct mem_controller *ctrl_a, const u8 *spd_addr);
uint16_t mct_MaxLoadFreq(uint8_t count, uint8_t highest_rank_count, uint8_t registered, uint8_t voltage, uint16_t freq);
u8 mctGetProcessorPackageType(void);
void Set_NB32_DCT(uint32_t dev, uint8_t dct, uint32_t reg, uint32_t val);
uint32_t Get_NB32_DCT(uint32_t dev, uint8_t dct, uint32_t reg);
uint32_t Get_NB32_index_wait_DCT(uint32_t dev, uint8_t dct, uint32_t index_reg, uint32_t index);
void Set_NB32_index_wait_DCT(uint32_t dev, uint8_t dct, uint32_t index_reg, uint32_t index, uint32_t data);
void fam15h_switch_dct(uint32_t dev, uint8_t dct);
uint32_t Get_NB32_DCT_NBPstate(uint32_t dev, uint8_t dct, uint8_t nb_pstate, uint32_t reg);
void Set_NB32_DCT_NBPstate(uint32_t dev, uint8_t dct, uint8_t nb_pstate, uint32_t reg, uint32_t val);

#endif
