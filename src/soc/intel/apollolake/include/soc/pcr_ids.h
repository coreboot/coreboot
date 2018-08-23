/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Intel Corporation.
 * Copyright (C) 2018 Siemens AG
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

#ifndef SOC_INTEL_APL_PCR_H
#define SOC_INTEL_APL_PCR_H

/*
 * Port ids.
 */
#if IS_ENABLED(CONFIG_SOC_INTEL_GLK)
#define PID_GPIO_AUDIO	0xC9
#define PID_GPIO_SCC	0xC8
#else
#define PID_GPIO_SW	0xC0
#define PID_GPIO_S	0xC2
#define PID_GPIO_W	0xC7
#endif
#define PID_GPIO_NW	0xC4
#define PID_GPIO_N	0xC5
#define PID_ITSS	0xD0
#define PID_RTC		0xD1
#define PID_LPC		0xD2
#define PID_MODPHY	0xA5

#define PID_AUNIT	0x4d
#define PID_BUNIT	0x4c
#define PID_TUNIT	0x52

#endif	/* SOC_INTEL_APL_PCR_H */
