/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#ifndef NORTHBRIDGE_AMD_GEODE_GEODELINK_H
#define NORTHBRIDGE_AMD_GEODE_GEODELINK_H

struct gliutable {
	unsigned long desc_name;
	unsigned short desc_type;
	unsigned long hi, lo;
};

static const struct gliutable gliu0table[] = {
	/* 0-7FFFF to MC */
	{.desc_name = MSR_GLIU0_BASE1,.desc_type = BM,.hi = MSR_MC + 0x0,
	 .lo = 0x0FFF80},
	/* 80000-9FFFF to MC */
	{.desc_name = MSR_GLIU0_BASE2,.desc_type = BM,.hi = MSR_MC + 0x0,
	 .lo = (0x80 << 20) + 0x0FFFE0},
	/* C0000-FFFFF split to MC and PCI (sub decode) A0000-BFFFF
	 * handled by SoftVideo.
	 */
	{.desc_name = MSR_GLIU0_SHADOW,.desc_type = SC_SHADOW,
	 .hi = MSR_MC + 0x0,.lo = 0x03},
	/* Catch and fix dynamically. */
	{.desc_name = MSR_GLIU0_SYSMEM,.desc_type = R_SYSMEM,
	 .hi = MSR_MC,.lo = 0x0},
	/* Catch and fix dynamically. */
	{.desc_name = MSR_GLIU0_SMM,.desc_type = BMO_SMM,
	 .hi = MSR_MC,.lo = 0x0},
	{.desc_name = GLIU0_GLD_MSR_COH,.desc_type = OTHER,
	 .hi = 0x0,.lo = GL0_CPU},
	{.desc_name = GL_END,.desc_type = GL_END,.hi = 0x0,.lo = 0x0},
};

static const struct gliutable gliu1table[] = {
	/* 0-7FFFF to MC */
	{.desc_name = MSR_GLIU1_BASE1,.desc_type = BM,.hi = MSR_GL0 + 0x0,
	 .lo = 0x0FFF80},
	/* 80000-9FFFF to MC */
	{.desc_name = MSR_GLIU1_BASE2,.desc_type = BM,.hi = MSR_GL0 + 0x0,
	 .lo = (0x80 << 20) + 0x0FFFE0},
	/* C0000-Fffff split to MC and PCI (sub decode) */
	{.desc_name = MSR_GLIU1_SHADOW,.desc_type = SC_SHADOW,
	 .hi = MSR_GL0 + 0x0,.lo = 0x03},
	/* Catch and fix dynamically. */
	{.desc_name = MSR_GLIU1_SYSMEM,.desc_type = R_SYSMEM,
	 .hi = MSR_GL0,.lo = 0x0},
	/* Catch and fix dynamically. */
	{.desc_name = MSR_GLIU1_SMM,.desc_type = BM_SMM,
	 .hi = MSR_GL0,.lo = 0x0},
	{.desc_name = GLIU1_GLD_MSR_COH,.desc_type = OTHER,
	 .hi = 0x0,.lo = GL1_GLIU0},
	/* FooGlue FPU 0xF0 */
	{.desc_name = MSR_GLIU1_FPU_TRAP,.desc_type = SCIO,
	 .hi = (GL1_GLCP << 29) + 0x0,.lo = 0x033000F0},
	{.desc_name = GL_END,.desc_type = GL_END,.hi = 0x0,.lo = 0x0},
};

static const struct gliutable *gliutables[] = { gliu0table, gliu1table, 0 };

#endif /* NORTHBRIDGE_AMD_GEODE_GEODELINK_H */
