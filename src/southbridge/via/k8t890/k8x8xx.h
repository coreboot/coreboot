/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
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

#ifndef SOUTHBRIDGE_VIA_K8T890_K8X8XX_H
#define SOUTHBRIDGE_VIA_K8T890_K8X8XX_H

#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#ifndef __PRE_RAM__
#include <device/device.h>
#endif
#include "k8t890.h"

#ifndef __PRE_RAM__
struct k8x8xx_vt8237_mirrored_regs {
	u16 low_top_address;
	u8 rom_shadow_ctrl_pg_c,
		rom_shadow_ctrl_pg_d,
		rom_shadow_ctrl_pg_e_memhole_smi_decoding,
		rom_shadow_ctrl_pg_f_memhole,
		smm_apic_decoding,
		shadow_mem_ctrl;
};

void k8x8xx_vt8237_mirrored_regs_fill(struct k8x8xx_vt8237_mirrored_regs *regs);
void k8x8xx_vt8237r_cfg(struct device *, struct device *);
#endif

#endif /* SOUTHBRIDGE_VIA_K8T890_K8X8XX_H */
