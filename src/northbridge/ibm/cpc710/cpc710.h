/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _CPC710_H_
#define _CPC710_H_

/* Revision */
#define CPC710_TYPE_100		0x80
#define CPC710_TYPE_100P	0x90

/* System control area */
#define CPC710_PHYS_SCA		0xff000000

#define CPC710_SCA_CPC0		0x000000
#define CPC710_SCA_SDRAM0	0x000000
#define CPC710_SCA_DMA0		0x1C0000

#define CPC710_PHYS_CPC0		(CPC710_PHYS_SCA + CPC710_SCA_CPC0)
#define CPC710_PHYS_SDRAM0		(CPC710_PHYS_SCA + CPC710_SCA_SDRAM0)

#define CPC710_CPC0_PIDR		0x0008
#define CPC710_CPC0_PCICNFR		0x000c
#define CPC710_CPC0_RSTR		0x0010
#define CPC710_CPC0_SPOR		0x00e8
#define CPC710_CPC0_UCTL		0x1000
#define CPC710_CPC0_MPSR		0x1010
#define CPC710_CPC0_SIOC0		0x1020
#define CPC710_CPC0_ABCNTL		0x1030
#define CPC710_CPC0_SRST		0x1040
#define CPC710_CPC0_ERRC		0x1050
#define CPC710_CPC0_SESR		0x1060
#define CPC710_CPC0_SEAR		0x1070
#define CPC710_CPC0_PGCHP		0x1100
#define CPC710_CPC0_RGBAN0		0x1110
#define CPC710_CPC0_RGBAN1		0x1120

#define CPC710_CPC0_GPDIR		0x1130
#define CPC710_CPC0_GPIN		0x1140
#define CPC710_CPC0_GPOUT		0x1150

#define CPC710_CPC0_ATAS		0x1160
#define CPC710_CPC0_AVDG		0x1170

#define CPC710_CPC0_PCIBAR		0x200018
#define CPC710_CPC0_PCIENB		0x201000

#define CPC710_SDRAM0_MCCR		0x1200
#define CPC710_SDRAM0_MWPR		0x1210
#define CPC710_SDRAM0_MESR		0x1220
#define CPC710_SDRAM0_MEAR		0x1230

#define CPC710_SDRAM0_MCER0		0x1300
#define CPC710_SDRAM0_MCER1		0x1310
#define CPC710_SDRAM0_MCER2		0x1320
#define CPC710_SDRAM0_MCER3		0x1330
#define CPC710_SDRAM0_MCER4		0x1340
#define CPC710_SDRAM0_MCER5		0x1350
#define CPC710_SDRAM0_MCER6		0x1360
#define CPC710_SDRAM0_MCER7		0x1370

#define CPC710_SDRAM0_SIOR0		0x1400
#define CPC710_SDRAM0_SIOR1		0x1420

#define CPC710_BRIDGE_PSEA		0xf6110
#define CPC710_BRIDGE_PCIDG		0xf6120
#define CPC710_BRIDGE_INTACK		0xf7700
#define CPC710_BRIDGE_PIBAR		0xf7800
#define CPC710_BRIDGE_PMBAR		0xf7810
#define CPC710_BRIDGE_CRR		0xf7ef0
#define CPC710_BRIDGE_PR		0xf7f20
#define CPC710_BRIDGE_ACR		0xf7f30
#define CPC710_BRIDGE_MSIZE		0xf7f40
#define CPC710_BRIDGE_IOSIZE		0xf7f60
#define CPC710_BRIDGE_SMBAR		0xf7f80
#define CPC710_BRIDGE_SIBAR		0xf7fc0
#define CPC710_BRIDGE_CTLRW		0xf7fd0
#define CPC710_BRIDGE_CFGADDR		0xf8000
#define CPC710_BRIDGE_CFGDATA		0xf8010
#define CPC710_BRIDGE_PSSIZE		0xf8100
#define CPC710_BRIDGE_BARPS		0xf8120
#define CPC710_BRIDGE_PSBAR		0xf8140
#define CPC710_BRIDGE_BPMDLK		0xf8200
#define CPC710_BRIDGE_TPMDLK		0xf8210
#define CPC710_BRIDGE_BIODLK		0xf8220
#define CPC710_BRIDGE_TIODLK		0xf8230
#define CPC710_BRIDGE_INTSET		0xf8310

/* Configuration space registers */
#define CPC710_BUS_NUMBER	0x40
#define CPC710_SUB_BUS_NUMBER	0x41

/* MCCR register bits */
#define CPC710_MCCR_DIAG_MODE		0x40000000
#define CPC710_MCCR_ECC_DISABLE		0x08000000
#define CPC710_MCCR_REFRESH_7CY		0x02000000
#define CPC710_MCCR_DATA_MASK		0x00100000
#define CPC710_MCCR_FIXED_BITS		0x00008000

extern void setCPC710(uint32_t, uint32_t);
extern uint32_t getCPC710(uint32_t);

#endif
