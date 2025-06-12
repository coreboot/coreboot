/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_APL_PCR_H
#define SOC_INTEL_APL_PCR_H

/*
 * Port ids.
 */
#define PID_CNVI	0x73
#if CONFIG(SOC_INTEL_GEMINILAKE)
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

#define PID_AUNIT	0x4D
#define PID_BUNIT	0x4C
#define PID_TUNIT	0x52

#define PID_PSF3	0xC6
#define PID_DMI	0x00 /* Reserved */
#define PID_CSME0	0x9A /* Reserved */

#endif	/* SOC_INTEL_APL_PCR_H */
