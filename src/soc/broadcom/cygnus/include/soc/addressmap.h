/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#ifndef __SOC_BROADCOM_CYGNUS_ADDRESSMAP_H__
#define __SOC_BROADCOM_CYGNUS_ADDRESSMAP_H__

#define IPROC_PERIPH_BASE		0x19020000
#define IPROC_PERIPH_GLB_TIM_REG_BASE	(IPROC_PERIPH_BASE + 0x200)

#define IPROC_QSPI_BASE			0x18047000

#define IPROC_IOMUX_OVERRIDE_BASE	0x0301D24C

#endif	/* __SOC_BROADCOM_CYGNUS_ADDRESSMAP_H__ */
