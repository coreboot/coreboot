/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Intel Corporation.
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

/*
 * This file is created based on Intel Tiger Lake Processor PCH Datasheet
 * Document number: 575857
 * Chapter number: 31-35
 */

#ifndef SOC_TIGERLAKE_PCR_H
#define SOC_TIGERLAKE_PCR_H
/*
 * Port ids
 */
#define PID_EMMC	0x52
#define PID_SDX		0x53

#define PID_GPIOCOM0	0x6e
#define PID_GPIOCOM1	0x6d
#define PID_GPIOCOM2	0x6c
#define PID_GPIOCOM4	0x6a
#define PID_GPIOCOM5	0x69

#define PID_DMI		0x88
#define PID_PSTH	0x89
#define PID_CSME0	0x90
#define PID_ISCLK	0xad
#define PID_PSF1	0xba
#define PID_PSF2	0xbb
#define PID_PSF3	0xbc
#define PID_PSF4	0xbd
#define PID_SCS		0xc0
#define PID_RTC		0xc3
#define PID_ITSS	0xc4
#define PID_ESPI	0xc7
#define PID_SERIALIO	0xcb

#endif
