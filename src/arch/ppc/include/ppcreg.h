/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* In the MSR, not all bits are interesting to us
   16 - EE  - External interrupts
   17 - PR  - Privilege level
   18 - FP  - Floating Point available
   19 - ME  - Machine check exception enable
   20 - FE0 - Floating exception mode 0
   23 - FE1 - Floating exception mode 1
   MSR_MASK is the bits we do not change.
   */

#define MSR_MASK 0xfff8008c
#define MSR_EE   0x00008000
#define MSR_PR   0x00004000
#define MSR_FP   0x00002000
#define MSR_ME   0x00001000
#define MSR_FE0  0x00000800
#define MSR_FE1  0x00000100

#define MSR_DEFAULT (MSR_FP | MSR_IR | MSR_DR)

/*
 * BAT defines
 */

/*
 * BL field in upper BAT register
 */
#define BAT_BL_128K	0x00000000
#define BAT_BL_256K	0x00000004
#define BAT_BL_512K	0x0000000C
#define BAT_BL_1M	0x0000001C
#define BAT_BL_2M	0x0000003C
#define BAT_BL_4M	0x0000007C
#define BAT_BL_8M	0x000000FC
#define BAT_BL_16M	0x000001FC
#define BAT_BL_32M	0x000003FC
#define BAT_BL_64M	0x000007FC
#define BAT_BL_128M	0x00000FFC
#define BAT_BL_256M	0x00001FFC

/*
 * Supervisor/user valid mode in upper BAT register
 */
#define BAT_VALID_SUPERVISOR	0x00000002
#define BAT_VALID_USER		0x00000001
#define BAT_INVALID		0x00000000

/*
 * WIMG bit setting in lower BAT register
 */
#define BAT_WRITE_THROUGH	0x00000040
#define BAT_CACHE_INHIBITED	0x00000020
#define BAT_COHERENT		0x00000010
#define BAT_GUARDED		0x00000008

/*
 * Protection bits in lower BAT register
 */
#define BAT_NO_ACCESS	0x00000000
#define BAT_READ_ONLY	0x00000001
#define BAT_READ_WRITE	0x00000002

/* Processor Version Register */

/* Processor Version Register (PVR) field extraction */

#define	PVR_VER(pvr)  (((pvr) >>  16) & 0xFFFF)	/* Version field */
#define	PVR_REV(pvr)  (((pvr) >>   0) & 0xFFFF)	/* Revison field */

/*
 * IBM has further subdivided the standard PowerPC 16-bit version and
 * revision subfields of the PVR for the PowerPC 403s into the following:
 */

#define	PVR_FAM(pvr)	(((pvr) >> 20) & 0xFFF)	/* Family field */
#define	PVR_MEM(pvr)	(((pvr) >> 16) & 0xF)	/* Member field */
#define	PVR_CORE(pvr)	(((pvr) >> 12) & 0xF)	/* Core field */
#define	PVR_CFG(pvr)	(((pvr) >>  8) & 0xF)	/* Configuration field */
#define	PVR_MAJ(pvr)	(((pvr) >>  4) & 0xF)	/* Major revision field */
#define	PVR_MIN(pvr)	(((pvr) >>  0) & 0xF)	/* Minor revision field */

/* Processor Version Numbers */

#define	PVR_403GA	0x00200000
#define	PVR_403GB	0x00200100
#define	PVR_403GC	0x00200200
#define	PVR_403GCX	0x00201400
#define	PVR_405GP	0x40110000
#define	PVR_405GP_RB	0x40110040
#define	PVR_405GP_RC	0x40110082
#define	PVR_405GP_RD	0x401100C4
#define	PVR_405GP_RE	0x40110145  /* same as pc405cr rev c */
#define	PVR_405CR_RA	0x40110041
#define	PVR_405CR_RB	0x401100C5
#define	PVR_405CR_RC	0x40110145  /* same as pc405gp rev e */
#define	PVR_405GPR_RB	0x50910951
#define	PVR_440GP_RB	0x40120440
#define	PVR_440GP_RC	0x40120481
#define	PVR_405EP_RB	0x51210950
#define	PVR_601		0x00010000
#define	PVR_602		0x00050000
#define	PVR_603		0x00030000
#define	PVR_603e	0x00060000
#define	PVR_603ev	0x00070000
#define	PVR_603r	0x00071000
#define	PVR_604		0x00040000
#define	PVR_604e	0x00090000
#define	PVR_604r	0x000A0000
#define	PVR_620		0x00140000
#define	PVR_740		0x00080000
#define	PVR_750		PVR_740
#define	PVR_740P	0x10080000
#define	PVR_750P	PVR_740P
/*
 * For the 8xx processors, all of them report the same PVR family for
 * the PowerPC core. The various versions of these processors must be
 * differentiated by the version number in the Communication Processor
 * Module (CPM).
 */
#define	PVR_821		0x00500000
#define	PVR_823		PVR_821
#define	PVR_850		PVR_821
#define	PVR_860		PVR_821
#define	PVR_7400       	0x000C0000
#define	PVR_8240	0x00810100
#define	PVR_8260	PVR_8240

/*----------------------------------------------------------------------------+
| Processor Version Register (PVR) values
+----------------------------------------------------------------------------*/
#define PVR_970                         0x0039          /* 970   any revision*/
#define PVR_970DD_1_0                   0x00391100      /* 970   DD1.0       */
#define PVR_970FX                       0x003C          /* 970FX any revision*/
#define PVR_970FX_DD_2_0                0x003C0200      /* 970FX DD2.0       */
#define PVR_970FX_DD_2_1                0x003C0201      /* 970FX DD2.1       */
#define PVR_970FX_DD_3_0                0x003C0300      /* 970FX DD3.0       */
#define PVR_RESERVED                    0x000000F0      /* reserved nibble   */

#define SPR_SRR0                        0x01a
#define SPR_SRR1                        0x01b
#define SPR_SPRG0                       0x110
#define SPR_SPRG1                       0x111
#define SPR_SPRG2                       0x112
#define SPR_SPRG3                       0x113
#define SPR_PVR                         0x11f
#define SPR_TBLR                        0x10c
#define SPR_TBUR                        0x10d

#ifdef __PPC64__
#define LOAD_64BIT_VAL(ra,value)        addis    ra,r0,value@highest;         \
                                        ori      ra,ra,value@higher;          \
                                        sldi     ra,ra,32;                    \
                                        oris     ra,ra,value@h;               \
                                        ori      ra,ra,value@l
#define TLBIEL(rb)                      .long 0x7C000000|\
                                        (rb<<11)|(274<<1)
#define HRFID()                         .long 0x4C000000|\
                                        (274<<1)
#endif

#ifndef ASM
unsigned __getmsr(void);
void __setmsr(unsigned value);
unsigned __gethid0(void);
unsigned __gethid1(void);
void __sethid0(unsigned value);
unsigned __getpvr(void);
#endif

