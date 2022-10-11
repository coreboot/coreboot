/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Alder Lake Processor PCH Datasheet
 * Document number: 621483
 * Chapter number: 31-35
 */

#ifndef SOC_ALDERLAKE_PCR_H
#define SOC_ALDERLAKE_PCR_H

/*
 * Port ids
 */
#define PID_GPIOCOM0	0x6e
#define PID_GPIOCOM1	0x6d
#define PID_GPIOCOM2	0x6c
#define PID_GPIOCOM3	0x6b
#define PID_GPIOCOM4	0x6a
#define PID_GPIOCOM5	0x69

#define PID_ESPI	0x72
#define PID_DMI		0x88
#define PID_PSTH	0x89
#define PID_CSME0	0x90
#define PID_ISCLK	0xad
#define PID_PSF1	0xba
#define PID_PSF2	0xbb
#define PID_PSF3	0xbc
#define PID_PSF4	0xbd
#define PID_IOM		0xc1
#define PID_RTC		0xc3
#define PID_ITSS	0xc4
#define PID_SERIALIO	0xcb

/* eMMC Port ID for Alder Lake N */
#define PID_EMMC	0xa1

/* CPU Port IDs */
#define PID_CPU_GPIOCOM0	0xb7
#define PID_CPU_GPIOCOM1	0xb8
#define PID_CPU_GPIOCOM3	0xbb
#define PID_CPU_GPIOCOM4	0xb9
#define PID_CPU_GPIOCOM5	0xba

#define PID_UFSX2	0x50

#endif
