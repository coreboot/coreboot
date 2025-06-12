/* SPDX-License-Identifier: GPL-2.0-only */

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
#define PID_GPIOCOM0	0x6e
#define PID_GPIOCOM1	0x6d
#define PID_GPIOCOM2	0x6c
#define PID_GPIOCOM3	0x6b
#define PID_GPIOCOM4	0x6a
#define PID_GPIOCOM5	0x69

#define PID_CNVI	0x73
#define PID_DMI		0x88
#define PID_PSTH	0x89
#define PID_CSME0	0x90
#define PID_ISCLK	0xad
#define PID_PSF1	0xba
#define PID_PSF2	0xbb
#define PID_PSF3	0xbc
#define PID_PSF4	0xbd
#define PID_SCS		0xc0
#define PID_IOM		0xc1
#define PID_RTC		0xc3
#define PID_ITSS	0xc4
#define PID_ESPI	0xc7
#define PID_SERIALIO	0xcb

/* CPU Port IDs */
#define PID_CPU_GPIOCOM0	0xb7
#define PID_CPU_GPIOCOM1	0xb8
#define PID_CPU_GPIOCOM4	0xb9
#define PID_CPU_GPIOCOM5	0xba

#endif
