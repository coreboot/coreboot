/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

/* In the MSR, not all bits are interesting to us
   13 - POW - Power management
   14 - TGPR - temporary registers for page table routines
   15 - ILE - Exception little endian
   16 - EE  - External interrupts
   17 - PR  - Privilege level
   18 - FP  - Floating Point available
   19 - ME  - Machine check exception enable
   20 - FE0 - Floating exception mode 0
   21 - SE  - Single step trace mode
   22 - BE  - Branch trace enable
   23 - FE1 - Floating exception mode 1
   25 - IP  - Exception prefix
   26 - IR  - Instruction address translation
   27 - DR  - Data address translation
   30 - RI  - Recoverable exception
   31 - LE  - Little endian mode
   MSR_MASK is the bits we do not change.
   */

#define MSR_MASK 0xfff8008c
#define MSR_POW  0x00040000
#define MSR_TGPR 0x00020000
#define MSR_ILE  0x00010000
#define MSR_EE   0x00008000
#define MSR_PR   0x00004000
#define MSR_FP   0x00002000
#define MSR_ME   0x00001000
#define MSR_FE0  0x00000800
#define MSR_SE   0x00000400
#define MSR_BE   0x00000200
#define MSR_FE1  0x00000100
#define MSR_IP   0x00000040
#define MSR_IR   0x00000020
#define MSR_DR   0x00000010
#define MSR_RI   0x00000002
#define MSR_LE   0x00000001

#define MSR_DEFAULT (MSR_FP | MSR_IR | MSR_DR)

/* We are interested in the following hid0 bits:
   6  - ECLK  - Enable external test clock (603 only)
   11 - DPM   - Turn on dynamic power management (603 only)
   15 - NHR   - Not hard reset
   16 - ICE   - Instruction cache enable
   17 - DCE   - Data cache enable
   18 - ILOCK - Instruction cache lock
   19 - DLOCK - Data cache lock
   20 - ICFI  - Instruction cache invalidate
   21 - DCFI  - Data cache invalidate
   24 - NOSER - Serial execution disable (604 only - turbo mode)
   24 - SGE   - Store gathering enable (7410 only)
   29 - BHT   - Branch history table (604 only)
   
   I made up the tags for the 604 specific bits, as they aren't
   named in the 604 book.  The 603 book calls the invalidate bits
   ICFI and DCI, and I have no idea why it isn't DCFI. Maybe IBM named
   one, and Motorola named the other. */

#define HID0_ECLK   0x02000000
#define HID0_DPM    0x00100000
#define HID0_NHR    0x00010000
#define HID0_ICE    0x00008000
#define HID0_DCE    0x00004000
#define HID0_ILOCK  0x00002000
#define HID0_DLOCK  0x00001000
#define HID0_ICFI   0x00000800
#define HID0_DCFI   0x00000400
#define HID0_NOSER  0x00000080
#define HID0_SGE    0x00000080
#define HID0_BTIC   0x00000020
#define HID0_BHT    0x00000004

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

#ifndef ASM
unsigned __getmsr(void);
void __setmsr(unsigned value);
unsigned __gethid0(void);
unsigned __gethid1(void);
void __sethid0(unsigned value);
unsigned __getpvr(void);
#endif

