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

#define IMG_SPIM0_BASE_ADDRESS	0xB8100F00
#define IMG_SPIM1_BASE_ADDRESS	0xB8101000

/*
 * Reading at this address allows to identify the platform the code is running
 * on.
 */
#define IMG_PLATFORM_ID()	(*((unsigned *)0xB8149060))
#define IMG_PLATFORM_ID_SILICON 0xF00D0006

#endif
