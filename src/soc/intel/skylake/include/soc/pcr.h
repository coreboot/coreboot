/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corporation.
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

#ifndef _SOC_PCR_H_
#define _SOC_PCR_H_

/*
 * Primary to sideband (P2SB) for private configuration registers (PCR).
 */

/* PCH (SunRisePoint LP) */
#define PCH_PCR_BASE_ADDRESS	0xFD000000

/* DMI Control Register */
#define R_PCH_PCR_DMI_DMIC	0x2234
#define B_PCH_PCR_DMI_DMIC_SRL	(1 << 31)
#define R_PCH_PCR_DMI_LPCLGIR1	0x2730
#define R_PCH_PCR_DMI_LPCLGIR2	0x2734
#define R_PCH_PCR_DMI_LPCLGIR3	0x2738
#define R_PCH_PCR_DMI_LPCLGIR4	0x273c
#define R_PCH_PCR_DMI_LPCIOD	0x2770
#define R_PCH_PCR_DMI_LPCIOE	0x2774

/* RTC configuration */
#define R_PCH_PCR_RTC_CONF	0x3400
#define B_PCH_PCR_RTC_CONF_UCMOS_EN	0x4

/* ITSS PCRs*/
/* PIRQA Routing Control Register*/
#define R_PCH_PCR_ITSS_PIRQA_ROUT	0x3100
/* PIRQB Routing Control Register*/
#define R_PCH_PCR_ITSS_PIRQB_ROUT	0x3101
/* PIRQC Routing Control Register*/
#define R_PCH_PCR_ITSS_PIRQC_ROUT	0x3102
/* PIRQD Routing Control Register*/
#define R_PCH_PCR_ITSS_PIRQD_ROUT	0x3103
/* PIRQE Routing Control Register*/
#define R_PCH_PCR_ITSS_PIRQE_ROUT	0x3104
/* PIRQF Routing Control Register*/
#define R_PCH_PCR_ITSS_PIRQF_ROUT	0x3105
/* PIRQG Routing Control Register*/
#define R_PCH_PCR_ITSS_PIRQG_ROUT	0x3106
/* PIRQH Routing Control Register*/
#define R_PCH_PCR_ITSS_PIRQH_ROUT	0x3107

/* IO Trap PCRs */
/* Trap status Register */
#define R_PCH_PCR_PSTH_TRPST	0x1E00
/* Trapped cycle */
#define R_PCH_PCR_PSTH_TRPC		0x1E10
/* Trapped write data */
#define R_PCH_PCR_PSTH_TRPD		0x1E18

/* Serial IO UART controller legacy mode */
#define R_PCH_PCR_SERIAL_IO_GPPRVRW7	0x618
#define SIO_PCH_LEGACY_UART0		(1 << 0)
#define SIO_PCH_LEGACY_UART1		(1 << 1)
#define SIO_PCH_LEGACY_UART2		(1 << 2)

/*
 * Definition for PCR address
 * The PCR address is used to the PCR MMIO programming
 */
#define PCH_PCR_ADDRESS(pid, offset)	(void *)(\
	PCH_PCR_BASE_ADDRESS | ((u8)(pid) << 16) | (u16)(offset))

/*
 * Definition for SBI PID
 * The PCH_SBI_PID defines the PID for PCR MMIO programming and
 * PCH SBI programming as well.
 */
typedef enum {
	PID_PSTH = 0x89,
	PID_GPIOCOM3 = 0xAC,
	PID_GPIOCOM2 = 0xAD,
	PID_GPIOCOM1 = 0xAE,
	PID_GPIOCOM0 = 0xAF,
	PID_LPC = 0xC7,
	PID_ITSS = 0xC4,
	PID_RTC = 0xC3,
	PID_SERIALIO = 0xCB,
	PID_DMI = 0xEF,
} PCH_SBI_PID;

u8 pcr_read32(PCH_SBI_PID pid, u16 offset, u32 *outdata);
u8 pcr_read16(PCH_SBI_PID pid, u16 offset, u16 *outdata);
u8 pcr_read8(PCH_SBI_PID pid, u16 offset, u8 *outdata);
u8 pcr_write32(PCH_SBI_PID pid, u16 offset, u32 indata);
u8 pcr_write16(PCH_SBI_PID pid, u16 offset, u16 indata);
u8 pcr_write8(PCH_SBI_PID pid, u16 offset, u8 indata);
u8 pcr_andthenor32(PCH_SBI_PID pid, u16 offset, u32 anddata, u32 ordata);
u8 pcr_andthenor16(PCH_SBI_PID pid, u16 offset, u16 anddata, u16 ordata);
u8 pcr_andthenor8(PCH_SBI_PID pid, u16 offset, u8 anddata, u8 ordata);

#endif /* _SOC_PCR_H_ */
