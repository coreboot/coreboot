/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __SOC_IMGTEC_DANUBE_CPU_H__
#define __SOC_IMGTEC_DANUBE_CPU_H__

#include <arch/io.h>

#define IMG_SPIM0_BASE_ADDRESS	0xB8100F00
#define IMG_SPIM1_BASE_ADDRESS	0xB8101000

/*
 * This register holds the FPGA image version
 * If we're not working on the FPGA this will be 0
 */
#define PRIMARY_FPGA_VERSION		0xB8149060
#define IMG_PLATFORM_ID()		read32(PRIMARY_FPGA_VERSION)
#define IMG_PLATFORM_ID_FPGA		0xD1400003 /* Last FPGA image */
#define IMG_PLATFORM_ID_SILICON		0

#endif
