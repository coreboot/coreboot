#ifndef	__DC21064_LOADED
#define	__DC21064_LOADED	1
/*
*****************************************************************************
**                                                                          *
**  Copyright © 1993, 1994						    *
**  by Digital Equipment Corporation, Maynard, Massachusetts.		    *
**                                                                          *
**  All Rights Reserved							    *
**                                                                          *
**  Permission  is  hereby  granted  to  use, copy, modify and distribute   *
**  this  software  and  its  documentation,  in  both  source  code  and   *
**  object  code  form,  and without fee, for the purpose of distribution   *
**  of this software  or  modifications  of this software within products   *
**  incorporating  an  integrated   circuit  implementing  Digital's  AXP   *
**  architecture,  regardless  of the  source of such integrated circuit,   *
**  provided that the  above copyright  notice and this permission notice   *
**  appear  in  all copies,  and  that  the  name  of  Digital  Equipment   *
**  Corporation  not  be  used  in advertising or publicity pertaining to   *
**  distribution of the  document  or  software without specific, written   *
**  prior permission.							    *
**                                                                          *
**  Digital  Equipment  Corporation   disclaims  all   warranties  and/or   *
**  guarantees  with  regard  to  this  software,  including  all implied   *
**  warranties of fitness for  a  particular purpose and merchantability,   *
**  and makes  no  representations  regarding  the use of, or the results   *
**  of the use of, the software and documentation in terms of correctness,  *
**  accuracy,  reliability,  currentness  or  otherwise;  and you rely on   *
**  the software, documentation and results solely at your own risk.	    *
**                                                                          *
**  AXP is a trademark of Digital Equipment Corporation.		    *
**                                                                          *
*****************************************************************************
**
**  FACILITY:  
**
**	DECchip 21064/21066 OSF/1 PALcode
**
**  MODULE:
**
**	dc21064.h
**
**  MODULE DESCRIPTION:
**
**      DECchip 21064/21066 specific definitions
**
**  AUTHOR: ER
**
**  CREATION DATE:  29-Oct-1992
**
**  $Id$
**
**  MODIFICATION HISTORY:
**
**  $Log$
**  Revision 1.1  2001/03/22 21:26:32  rminnich
**  testing I HATE CVS
**
**  Revision 1.1  2000/03/21 03:56:31  stepan
**  Check in current version which is a nearly a 2.2-16
**
**  Revision 2.2  1994/06/16  14:47:31  samberg
**  For ANSI, changed $ to _, except for pvc and reg def
**
**  Revision 2.1  1994/04/01  21:55:51  ericr
**  1-APR-1994 V2 OSF/1 PALcode
**
**  Revision 1.6  1994/03/29  20:53:23  ericr
**  Fixed comments for ESR mask definitions NOT_CEE and ERR_NOT_CEE
**
**  Revision 1.5  1994/03/29  19:32:22  ericr
**  Fixed IOC_STAT0<ERR> bit definition and mask.
**
**  Revision 1.4  1994/03/14  20:45:58  ericr
**  Added FPR definitions
**
**  Revision 1.3  1994/03/14  16:38:44  samberg
**  Use LOAD_REGION_BASE macro instead of individual load_x_csr macros
**
**  Revision 1.2  1994/03/08  20:27:52  ericr
**  Replaced DEBUG_MONITOR conditional with KDEBUG
**
**  Revision 1.1  1994/02/28  18:23:46  ericr
**  Initial revision
**
**
*/

/*======================================================================*/
/*                INTERNAL PROCESSOR REGISTER DEFINITIONS		*/
/*======================================================================*/

#define IPR_V_PAL	7
#define IPR_M_PAL	(1<<IPR_V_PAL)
#define IPR_V_ABX	6
#define IPR_M_ABX	(1<<IPR_V_ABX)
#define IPR_V_IBX	5
#define IPR_M_IBX	(1<<IPR_V_IBX)
#define IPR_V_INDEX	0
#define IPR_M_INDEX	(0x1F<<IPR_V_INDEX)

/*
**  Ibox IPR Definitions
*/

#define tbTag		IPR_M_IBX + 0x0
#define itbPte		IPR_M_IBX + 0x1
#define iccsr		IPR_M_IBX + 0x2
#define itbPteTemp	IPR_M_IBX + 0x3
#define excAddr		IPR_M_IBX + 0x4
#define slRcv		IPR_M_IBX + 0x5
#define itbZap		IPR_M_IBX + 0x6
#define itbAsm		IPR_M_IBX + 0x7
#define itbIs		IPR_M_IBX + 0x8
#define ps		IPR_M_IBX + 0x9
#define excSum		IPR_M_IBX + 0xA
#define palBase		IPR_M_IBX + 0xB
#define hirr		IPR_M_IBX + 0xC
#define sirr		IPR_M_IBX + 0xD
#define astrr		IPR_M_IBX + 0xE
#define hier		IPR_M_IBX + 0x10
#define sier		IPR_M_IBX + 0x11
#define aster		IPR_M_IBX + 0x12
#define slClr		IPR_M_IBX + 0x13
#define slXmit		IPR_M_IBX + 0x16

/*
**  Instruction Cache Control and Status Register (ICCSR) Bit Summary
**
**	  Loc	Size	Name	Function
**	 -----	----	----	---------------------------------
**	   <45>	  1	PME1	Performance Monitor Enable 1
**	   <44>	  1	PME0	Performance Monitor Enable 0
**	   <42>	  1	FPE	Floating Point Enable
**	   <41>	  1	MAP	I-stream superpage mapping enable
**	   <40>	  1	HWE	Allow PALRES to be issued in kernel mode
**	   <39>	  1	DI	Dual Issue enable
**	   <38>	  1	BHE	Branch History Enable
**	   <37>	  1	JSE	JSR Stack Enable
**	   <36>	  1	BPE	Branch Prediction Enable
**	   <35>	  1	PIPE	Pipeline enable
**	<34:32>	  3	MUX1	Performance Counter 1 Select
**	<11:08>	  3	MUX0	Performance Counter 0 Select
**	    <3>   1	PC0	Performance Counter 0 Interrupt Enable
**	    <0>	  1	PC1	Performance Counter 1 Interrupt Enable
*/

#define ICCSR_V_PME1	    45
#define ICCSR_M_PME1	    (1<<(ICCSR_V_PME1-32))
#define ICCSR_V_PME0	    44
#define ICCSR_M_PME0	    (1<<(ICCSR_V_PME0-32))
#define ICCSR_V_FPE	    42
#define ICCSR_M_FPE	    (1<<(ICCSR_V_FPE-32))
#define ICCSR_V_MAP	    41
#define ICCSR_M_MAP	    (1<<(ICCSR_V_MAP-32))
#define ICCSR_V_HWE	    40
#define ICCSR_M_HWE	    (1<<(ICCSR_V_HWE-32))
#define ICCSR_V_DI	    39
#define ICCSR_M_DI	    (1<<(ICCSR_V_DI-32))
#define ICCSR_V_BHE	    38
#define ICCSR_M_BHE	    (1<<(ICCSR_V_BHE-32))
#define ICCSR_V_JSE	    37
#define ICCSR_M_JSE	    (1<<(ICCSR_V_JSE-32))
#define ICCSR_V_BPE	    36
#define ICCSR_M_BPE	    (1<<(ICCSR_V_BPE-32))
#define ICCSR_V_PIPE	    35
#define ICCSR_M_PIPE	    (1<<(ICCSR_V_PIPE-32))
#define ICCSR_V_MUX1	    32
#define ICCSR_M_MUX1	    (7<<(ICCSR_V_MUX1-32))
#define ICCSR_V_MUX0	    8
#define ICCSR_M_MUX0	    (0xF<<ICCSR_V_MUX0)
#define ICCSR_V_PC0	    3
#define ICCSR_M_PC0	    (1<<ICCSR_V_PC0)
#define ICCSR_V_PC1	    0
#define ICCSR_M_PC1	    (1<<ICCSR_V_PC1)

/*
**  Exception Summary Register (EXC_SUM) Bit Summary
**
**	 Loc	Size	Name	Function
**	-----	----	----	------------------------------------
**	 <33>	 1	MSK	Exception Register Write Mask window
**	  <8>	 1	IOV	Integer overflow
**	  <7>	 1	INE	Inexact result
**	  <6>	 1	UNF	Underflow
**	  <5>	 1	OVF	Overflow
**	  <4>	 1	DZE	Division by zero
**	  <3>	 1	INV	Invalid operation
**	  <2>	 1	SWC	Software completion
*/

#define EXC_V_MSK	33
#define EXC_M_MSK	(1<<(EXC_V_MSK-32))
#define EXC_V_IOV	8
#define EXC_M_IOV	(1<<EXC_V_IOV)
#define EXC_V_INE	7
#define EXC_M_INE	(1<<EXC_V_INE)
#define EXC_V_UNF	6
#define EXC_M_UNF	(1<<EXC_V_UNF)
#define EXC_V_OVF	5
#define EXC_M_OVF	(1<<EXC_V_OVF)
#define EXC_V_DZE	4
#define	EXC_M_DZE	(1<<EXC_V_DZE)
#define EXC_V_INV	3
#define EXC_M_INV	(1<<EXC_V_INV)
#define	EXC_V_SWC	2
#define EXC_M_SWC	(1<<EXC_V_SWC)

#ifdef DC21064
/*
**  Hardware Interrupt Request Register (HIRR) Bit Summary
**
**	  Loc	Size	Name	Function
**	 -----	----	----	---------------------------------
**	  <13>	  1	SLR	Serial Line interrupt
**     <12:10>    3     IRQ     Corresponds to pins Irq_h[2:0]
**	   <9>	  1	PC0	Performance Counter 0 interrupt
**	   <8>	  1	PC1	Performance Counter 1 interrupt
**         <7>    1     IRQ5    Corresponds to pin Irq_h[5]
**         <6>    1     IRQ4    Corresponds to pin Irq_h[4]
**         <5>    1     IRQ3    Corresponds to pin Irq_h[3]
**	   <4>	  1	CRR	Correctable read data interrupt
**	   <3>	  1	ATR	AST interrupt
**	   <2>	  1	SWR	Software interrupt
**	   <1>	  1	HWR	Hardware interrupt
*/
#endif /* DC21064 */

#ifdef DC21066
/*
**  Hardware Interrupt Request Register (HIRR) Bit Summary
**
**	  Loc	Size	Name	Function
**	 -----	----	----	---------------------------------
**	  <13>	  1	SLR	Serial Line interrupt
**     <12:10>    3     IRQ     Corresponds to pins Irq_h[2:0]
**	   <9>	  1	PC0	Performance Counter 0 interrupt
**	   <8>	  1	PC1	Performance Counter 1 interrupt
**         <6>    1     MERR    Memory error interrupt
**         <5>	  1	IERR    IOC error interrupt
**	   <4>	  1	CRR	Correctable read data interrupt
**	   <3>	  1	ATR	AST interrupt
**	   <2>	  1	SWR	Software interrupt
**	   <1>	  1	HWR	Hardware interrupt
*/
#endif /* DC21066 */

#define HIRR_V_SLR	13
#define HIRR_M_SLR	(1<<HIRR_V_SLR)

#define HIRR_V_IRQ2	12
#define HIRR_M_IRQ2	(1<<HIRR_V_IRQ2)
#define HIRR_V_IRQ1	11
#define HIRR_M_IRQ1	(1<<HIRR_V_IRQ1)
#define HIRR_V_IRQ0	10
#define HIRR_M_IRQ0	(1<<HIRR_V_IRQ0)

#define HIRR_V_PC0	9
#define HIRR_M_PC0	(1<<HIRR_V_PC0)
#define HIRR_V_PC1	8
#define HIRR_M_PC1	(1<<HIRR_V_PC1)

#ifdef DC21064
#define HIRR_V_IRQ5	7
#define HIRR_M_IRQ5	(1<<HIRR_V_IRQ5)
#define HIRR_V_IRQ4	6
#define HIRR_M_IRQ4	(1<<HIRR_V_IRQ4)
#define HIRR_V_IRQ3	5
#define HIRR_M_IRQ3	(1<<HIRR_V_IRQ3)
#endif  /* DC21064 */

#ifdef DC21066
#define HIRR_V_MERR	6
#define HIRR_M_MERR	(1<<HIRR_V_MERR)
#define HIRR_V_IERR	5
#define HIRR_M_IERR	(1<<HIRR_V_IERR)
#endif  /* DC21066 */

#define HIRR_V_CRR	4
#define HIRR_M_CRR	(1<<HIRR_V_CRR)
#define HIRR_V_ATR	3
#define HIRR_M_ATR	(1<<HIRR_V_ATR)
#define HIRR_V_SWR	2
#define HIRR_M_SWR	(1<<HIRR_V_SWR)
#define HIRR_V_HWR	1
#define HIRR_M_HWR	(1<<HIRR_V_HWR)

/*
**  Hardware Interrupt Enable Register (HIER) Write Bit Summary
**
**	  Loc	Size	Name	Function
**	 -----	----	----	---------------------------------
**	  <32>	  1	SLE	Serial Line interrupt enable
**	  <15>	  1	PC1	Performance Counter 1 interrupt enable
**	<14:9>	  6	HIER	Interrupt enables for Irq_h<5:0>
**	   <8>	  1	PC0	Performance Counter 0 interrupt enable
**	   <2>	  1	CRE	Correctable read data interrupt enable
*/

#define HIERW_V_SLE	32
#define HIERW_V_PC1	15
#define HIERW_V_PC0	8
#define HIERW_V_CRE	2
#define HIERW_M_CRE     (1<<HIERW_V_CRE)

#define HIERR_V_CRE	4
#define HIERR_M_CRE     (1<<HIERR_V_CRE)

/*
**  Clear Serial Line Interrupt Register (SL_CLR) Bit Summary
**
**	  Loc	Size	Name	    Function
**	 -----	----	----	    ---------------------------------
**	   <32>	  1	SLC	    W0C -- Clear serial line int request
**	   <15>	  1	PC1	    W0C -- Clear PC1 interrupt request
**	    <8>	  1	PC0	    W0C -- Clear PC0 interrupt request
**	    <2>	  1	CRD	    W0C -- Clear CRD interrupt request
*/

#define SL_CLR_V_SLC	    32
#define SL_CLR_V_PC1	    15
#define SL_CLR_V_PC0	    8
#define SL_CLR_V_CRD	    2

/*
**  Abox IPR Definitions
*/

#define dtbCtl		IPR_M_ABX + 0x0
#define tbCtl		IPR_M_ABX + 0x0
#define dtbPte		IPR_M_ABX + 0x2
#define dtbPteTemp	IPR_M_ABX + 0x3
#define mmcsr		IPR_M_ABX + 0x4
#define va		IPR_M_ABX + 0x5
#define dtbZap		IPR_M_ABX + 0x6
#define dtbAsm		IPR_M_ABX + 0x7
#define dtbIs		IPR_M_ABX + 0x8

#ifdef DC21064
#define biuAddr		IPR_M_ABX + 0x9
#define biuStat		IPR_M_ABX + 0xA
#endif /* DC21064 */

#define dcAddr		IPR_M_ABX + 0xB
#define dcStat		IPR_M_ABX + 0xC

#ifdef DC21064
#define fillAddr	IPR_M_ABX + 0xD
#endif  /* DC21064 */

#define aboxCtl		IPR_M_ABX + 0xE
#define altMode		IPR_M_ABX + 0xF
#define cc		IPR_M_ABX + 0x10
#define ccCtl		IPR_M_ABX + 0x11

#ifdef DC21064
#define biuCtl		IPR_M_ABX + 0x12
#define fillSyndrome	IPR_M_ABX + 0x13
#define bcTag		IPR_M_ABX + 0x14
#endif  /* DC21064 */

#define flushIc		IPR_M_ABX + 0x15
#define flushIcAsm	IPR_M_ABX + 0x17
#define xtbZap		IPR_M_ABX + IPR_M_IBX + 0x6
#define xtbAsm		IPR_M_ABX + IPR_M_IBX + 0x7

/*
**  Memory Management Control and Status Register (MM_CSR) Bit Summary
**
**	  Loc	Size	Name	Function
**	 -----	----	----	---------------------------------
**	<14:9>	  6	OPC	Opcode of faulting instruction
**	 <8:4>	  5	RA	Ra field of faulting instruction
**	   <3>	  1	FOW	Fault on write
**	   <2>	  1	FOR	Fault on read
**	   <1>	  1	ACV	Access violation
**	   <0>	  1	WR	Faulting reference is a write
*/

#define	MMCSR_V_OPC	9
#define MMCSR_M_OPC	(0x7E<<MMCSR_V_OPC)
#define MMCSR_V_RA	4
#define MMCSR_M_RA	(0x1F<<MMCSR_V_RA)
#define MMCSR_V_FOW	3
#define MMCSR_M_FOW	(1<<MMCSR_V_FOW)
#define MMCSR_V_FOR	2
#define MMCSR_M_FOR	(1<<MMCSR_V_FOR)
#define MMCSR_V_ACV	1
#define MMCSR_M_ACV	(1<<MMCSR_V_ACV)
#define MMCSR_V_WR	0
#define MMCSR_M_WR	(1<<MMCSR_V_WR)

#define MMCSR_M_FAULT	0x000E

/*
** Abox Control Register (ABOX_CTL) Bit Summary
**
**	  Loc	Size	Name	    Function
**	 -----	----	----	    ---------------------------------
**	  <14>	  1	NOCHK_PAR   EV45 - Set to disable checking of 
**				    primary cache parity
**	  <13>    1	F_TAG_ERR   EV45 - Set to generate bad primary 
**				    cache tag parity
**	  <12>    1	DC_16K	    EV45 - Set to select 16KB cache
**	  <11>	  1	DC_FHIT	    Dcache Force Hit
**	  <10>	  1	DC_ENA	    Dcache Enable
**	   <6>	  1	EMD_EN	    Limited big endian support enable
**	   <5>	  1	SPE_2	    D-stream superpage 1 enable
**	   <4>	  1	SPE_1	    D-stream superpage 2 enable
**	   <3>	  1	IC_SBUF_EN  Icache Stream Buffer Enable
**	   <2>	  1	CRD_EN	    Corrected Read Data Enable
**	   <1>	  1	MCHK_EN	    Machine Check Enable
**	   <0>	  1	WB_DIS	    Write Buffer unload Disable
*/

#define ABOX_V_NOCHK_PAR    14
#define ABOX_M_NOCHK_PAR    (1<<ABOX_V_NOCHK_PAR)
#define ABOX_V_F_TAG_ERR    13
#define ABOX_M_F_TAG_ERR    (1<<ABOX_V_F_TAG_ERR)
#define ABOX_V_DC_16K	    12
#define ABOX_M_DC_16K	    (1<<ABOX_V_DC_16K)
#define ABOX_V_DC_FHIT	    11
#define ABOX_M_DC_FHIT	    (1<<ABOX_V_DC_FHIT)
#define ABOX_V_DC_ENA	    10
#define	ABOX_M_DC_ENA	    (1<<ABOX_V_DC_ENA)
#define ABOX_V_EMD_EN	    6
#define ABOX_M_EMD_EN	    (1<<ABOX_V_EMD_EN)
#define ABOX_V_SPE_2	    5
#define ABOX_M_SPE_2	    (1<<ABOX_V_SPE_2)
#define ABOX_V_SPE_1	    4
#define ABOX_M_SPE_1	    (1<<ABOX_V_SPE_1)
#define ABOX_V_IC_SBUF_EN   3
#define ABOX_M_IC_SBUF_EN   (1<<ABOX_V_IC_SBUF_EN)
#define ABOX_V_CRD_EN	    2
#define ABOX_M_CRD_EN	    (1<<ABOX_V_CRD_EN)
#define ABOX_V_MCHK_EN	    1
#define ABOX_M_MCHK_EN	    (1<<ABOX_V_MCHK_EN)
#define ABOX_V_WB_DIS	    0
#define	ABOX_M_WB_DIS	    (1<<ABOX_V_WB_DIS)


#ifdef DC21064
/*
**  Bus Interface Unit Control Register (BIU_CTL) Bit Summary
**
**	  Loc	Size	Name	    Function
**	 -----	----	----	    ---------------------------------
**	   <39>   1	IMAP_EN	    Allow dMapWe<1:0> to assert on 
**				    I-stream backup cache reads
**	   <37>   1	BYTE_PARITY External byte parity (ignore if ECC set)
**	   <36>	  1	BAD_DP	    Force bad data parity/ECC check bits
**	<35:32>	  4	BC_PA_DIS   Don't cache PA quadrant specified
**	   <31>	  1	BAD_TCP	    Force bad tag parity
**	<30:28>	  3	BC_SIZE	    External cache size
**	<27:13>	 16	BC_WE_CTL   External cache write enable control
**	 <11:8>	  4	BC_WR_SPD   External cache write speed
**	  <7:4>	  4	BC_RD_SPD   External cache read speed
**	    <3>	  1	BC_FHIT	    External cache force hit
**	    <2>	  1	OE	    Output enable
**	    <1>	  1	ECC	    Enable ECC
**	    <0>	  1	BC_ENA	    External cache enable
*/

#define BIU_V_IMAP_EN	    39
#define BIU_M_IMAP_EN	    (1<<(BIU_V_IMAP_EN-32))
#define BIU_V_BYTE_PARITY   37
#define BIU_M_BYTE_PARITY   (1<<(BIU_V_BYTE_PARITY-32))
#define	BIU_V_BAD_DP	    36
#define BIU_M_BAD_DP	    (1<<(BIU_V_BAD_DP-32))
#define BIU_V_BC_PA_DIS	    32
#define BIU_M_BC_PA_DIS	    (0xF<<(BIU_V_BC_PA_DIS-32))
#define BIU_V_BAD_TCP	    31
#define BIU_M_BAD_TCP	    (1<<(BIU_V_BAD_TCP-16))
#define BIU_V_BC_SIZE	    28
#define BIU_M_BC_SIZE	    (7<<(BIU_V_BC_SIZE-16))
#define BIU_V_BC_WE_CTL	    13
#define BIU_M_BC_WE_CTL	    (0x7FFF<<BIU_V_BC_WE_CTL)
#define BIU_V_BC_WR_SPD	    8
#define BIU_M_BC_WR_SPD	    (0xF<<BIU_V_BC_WR_SPD)
#define BIU_V_BC_RD_SPD	    4
#define BIU_M_BC_RD_SPD	    (0xF<<BIU_V_BC_RD_SPD)
#define BIU_V_BC_FHIT	    3
#define BIU_M_BC_FHIT	    (1<<BIU_V_BC_FHIT)
#define BIU_V_OE	    2
#define BIU_M_OE	    (1<<BIU_V_OE)
#define BIU_V_ECC	    1
#define BIU_M_ECC	    (1<<BIU_V_ECC)
#define BIU_V_BC_ENA	    0
#define BIU_M_BC_ENA	    (1<<BIU_V_BC_ENA)

/*
**  Bus Interface Unit Status Register (BIU_STAT) Bit Summary
**
**	  Loc	Size	Name	    Function
**	 -----	----	----	    ---------------------------------
**	   <14>	  1	FILL_SEO    Second error while FILL_ECC or FILL_DPERR
**	<13:12>	  2	FILL_QW	    Used with FILL_ADDR for physical address
**	   <11>	  1	FILL_IRD    Icache fill when FILL_ECC or FILL_DPERR
**	   <10>	  1	FILL_DPERR  Fill parity error or double bit ECC
**	    <9>	  1	FILL_CRD    Corrected read data
**	    <8>	  1	FILL_ECC    ECC error
**	    <7>	  1	BIU_SEO	    Second error while BIU or BC error
**	  <6:4>	  3	BIU_CMD	    Cycle type
**	    <3>	  1	BC_TCPERR   Tag control parity error on external cache
**	    <2>	  1	BC_TPERR    Tag address parity error on external cache
**	    <1>	  1	BIU_SERR    cAck_h pins indicate SOFT_ERROR
**	    <0>	  1	BIU_HERR    cAck_h pins indicate HARD_ERROR
*/

#define BIU_STAT_V_FILL_SEO	14
#define BIU_STAT_M_FILL_SEO	(1<<BIU_STAT_V_FILL_SEO)
#define BIU_STAT_V_FILL_CRD	9
#define BIU_STAT_M_FILL_CRD	(1<<BIU_STAT_V_FILL_CRD)
#define	BIU_STAT_V_FILL_ECC	8
#define BIU_STAT_M_FILL_ECC	(1<<BIU_STAT_V_FILL_ECC)
#define BIU_STAT_V_BC_TCPERR	3
#define BIU_STAT_M_BC_TCPERR	(1<<BIU_STAT_V_BC_TCPERR)
#define BIU_STAT_V_BC_TPERR	2
#define BIU_STAT_M_BC_TPERR	(1<<BIU_STAT_V_BC_TPERR)
#define BIU_STAT_V_BIU_SERR	1
#define BIU_STAT_M_BIU_SERR	(1<<BIU_STAT_V_BIU_SERR)
#define BIU_STAT_V_BIU_HERR	0
#define BIU_STAT_M_BIU_HERR	(1<<BIU_STAT_V_BIU_HERR)

/*
** Bus Interface Unit Status Register (BIU_STAT) definitions:
**
** These definitions specify masks for identifying hard errors and all
** potential BIU errors, derived from BIU_STAT definitions.
*/

#define BIU_STAT_M_HARD (BIU_STAT_M_BC_TPERR | BIU_STAT_M_BC_TCPERR | \
                         BIU_STAT_M_BIU_HERR)

#define BIU_STAT_M_ALL  (BIU_STAT_M_BIU_HERR | BIU_STAT_M_BIU_SERR  | \
                         BIU_STAT_M_BC_TPERR | BIU_STAT_M_BC_TCPERR | \
                         BIU_STAT_M_FILL_SEO)

/*
** Cache Status Register (C_STAT) Bit Summary
**
**	  Loc	Size	Name	    Function
**	 -----	----	----	    ---------------------------------
**	   <5>	  1	IC_ERR	    EV45 - ICache parity error
**	   <4>	  1	DC_ERR	    EV45 - DCache parity error
**	   <3>	  1	DC_HIT	    Dcache hit
**	  <2:0>   3		    EV4  - 111 binary (pass3, i.e. with ECC)
**				    EV45 - 101 binary
*/

#define C_STAT_V_IC_ERR	    5
#define C_STAT_M_IC_ERR	    (1<<C_STAT_V_IC_ERR)
#define C_STAT_V_DC_ERR	    4
#define C_STAT_M_DC_ERR	    (1<<C_STAT_V_DC_ERR)
#define C_STAT_V_DC_HIT	    3
#define C_STAT_M_DC_HIT     (1<<C_STAT_V_DC_HIT)

#endif /* DC21064 */

#ifdef DC21066

/*======================================================================*/
/*                DC21066 MEMORY MAPPED CSR DEFINITIONS			*/
/*======================================================================*/

/*
**  Macro to create the base of a region of physical address space
**  The inputs are:
**      reg     the register to which the physical base address is written
**      base    base<5:0> to be shifted left 28 bits into <33:28>
*/ 

#define LOAD_REGION_BASE(reg,base) \
  lda   reg, base(r31); \
  sll   reg, 28, reg

/*
**  Memory Controller (MEM_CTL) Register Definitions
*/

#define MEM_CSR_BASE 0x12       /* Bits <33:28> of physical address base */

/* 
** Offsets to Memory Controller CSRs
*/

#define	bcr0	0x0	/* Bank Configuration 0 */
#define	bcr1	0x8	/* Bank Configuration 1 */
#define	bcr2	0x10	/* Bank Configuration 2 */
#define	bcr3	0x18	/* Bank Configuration 3 */
#define	bmr0	0x20	/* Bank Mask 0 */
#define	bmr1	0x28	/* Bank Mask 1 */
#define	bmr2	0x30	/* Bank Mask 2 */
#define	bmr3	0x38	/* Bank Mask 3 */
#define	btr0	0x40	/* Bank Timing 0 */
#define	btr1	0x48	/* Bank Timing 1 */
#define	btr2	0x50	/* Bank Timing 2 */
#define	btr3	0x58	/* Bank Timing 3 */
#define gtr	0x60	/* Global Timing */
#define esr	0x68	/* Error Status */
#define ear	0x70	/* Error Address */
#define car	0x78	/* Backup Cache Control */
#define vgr	0x80	/* Video and Graphics Control */
#define plm	0x88	/* Plane Mask */
#define for	0x90	/* Foreground */

/* 
** Bank Configuration Register (BCR) Bit Summary
**
**	  Loc	Size	Name    Function
**	 -----	----	----	---------------------------------
**     <28:20>    9	BASE	Bank Base Address
**	  <14>	  1	BAV	Base Address Valid
**	  <13>	  1	SBE	Split (dual) Bank Enable
**	  <12>	  1	BWE	Byte Write Enable
**	  <11>	  1	WRM	Write Mode
**        <10>    1     ERM	Error Mode
**	 <9:6>	  4	RAS	Row Address Select
*/

#define bcr_v_ras   	6
#define bcr_m_ras	(0xF<<bcr_v_ras)
#define bcr_v_erm   	10
#define bcr_m_erm	(1<<bcr_v_erm)
#define bcr_v_wrm   	11
#define bcr_m_wrm	(1<<bcr_v_wrm)
#define bcr_v_bwe   	12
#define bcr_m_bwe	(1<<bcr_v_bwe)
#define bcr_v_sbe   	13
#define bcr_m_sbe	(1<<bcr_v_sbe)
#define bcr_v_bav   	14
#define bcr_m_bav	(1<<bcr_v_bav)
#define bcr_v_base  	20

/*
** Bank Address Mask Register (BMR) Bit Summary
**
**	  Loc	Size	Name    Function
**	 -----	----	----	---------------------------------
**     <28:20>    9	MASK	Bank Address Mask
*/

#define bmr_v_mask  20

/* 
** Cache Register (CAR) Bit Summary
**
**	  Loc	Size	Name    Function
**	 -----	----	----	---------------------------------
**	  <31>	  1	HIT	Backup Cache Hit
**	  <15>	  1	PWR	Power Saving
**        <14>    1	WHD	Write Hold Time
**     <13:11>	  3	WRS	Backup Cache Write Speed
**     <10:08>	  3	RDS	Backup Cache Read Speed
**     <07:05>    3	SIZE	Backup Cache Size
**	   <4>    1	ECE	Backup Cache ECC
**	   <3>	  1	WWP	Write Wrong Tag Parity
**	   <2>	  1	ETP	Enable Tag Parity
**	   <0>	  1	BCE	Backup Cache Enable               
*/

#define CAR_V_BCE	0
#define CAR_M_BCE	(1<<CAR_V_BCE)
#define CAR_V_ETP   	2
#define CAR_M_ETP	(1<<CAR_V_ETP)
#define CAR_V_WWP   	3
#define CAR_M_WWP	(1<<CAR_V_WWP)
#define CAR_V_ECE   	4
#define CAR_M_ECE	(1<<CAR_V_ECE)
#define CAR_V_SIZE  	5
#define CAR_M_SIZE	(7<<CAR_V_SIZE)
#define CAR_V_RDS   	8
#define CAR_M_RDS	(7<<CAR_V_RDS)
#define CAR_V_WRS  	11
#define CAR_M_WRS	(7<<CAR_V_WRS)
#define CAR_V_WHD  	14
#define CAR_M_WHD	(1<<CAR_V_WHD)
#define CAR_V_PWR	15
#define CAR_M_PWR	(1<<CAR_V_PWR)
#define CAR_V_HIT	31
#define CAR_M_HIT	(1<<CAR_V_HIT)

/*
**  Error Status Register (ESR) Bit Summary
**
**	  Loc	Size	Name	    Function
**	 -----	----	----	    ---------------------------------
**	   <12>	  1	NXM   	    Non-existant memory address
**	   <11>	  1	ICE         Ignore corrected errors
**	   <10>	  1	MHE    	    Multiple hard errors
**	    <9>   1	MSE         Multiple soft errors
**	    <7>	  1	CTE         Cache tag parity error
**	    <4>	  1	SOR         Error source (0=cache, 1=DRAM)
**	    <3>	  1	WRE         Error access type (0=read, 1=write)
**	    <2>	  1	UEE	    Uncorrectable ECC error
**	    <1>	  1	CEE	    Correctable ECC error
**	    <0>	  1	EAV	    Error address valid
*/

#define	ESR_V_NXM	    12
#define ESR_M_NXM	    (1<<ESR_V_NXM)
#define ESR_V_ICE      	    11
#define ESR_M_ICE      	    (1<<ESR_V_ICE)
#define ESR_V_MHE    	    10
#define ESR_M_MHE      	    (1<<ESR_V_MHE)
#define ESR_V_MSE      	    9 
#define ESR_M_MSE     	    (1<<ESR_V_MSE)
#define ESR_V_CTE      	    7 
#define ESR_M_CTE           (1<<ESR_V_CTE)
#define ESR_V_SOR           4
#define ESR_M_SOR      	    (1<<ESR_V_SOR)
#define ESR_V_WRE      	    3
#define ESR_M_WRE      	    (1<<ESR_V_WRE)
#define ESR_V_UEE    	    2
#define ESR_M_UEE    	    (1<<ESR_V_UEE)
#define ESR_V_CEE	    1
#define ESR_M_CEE	    (1<<ESR_V_CEE)
#define ESR_V_EAV	    0
#define ESR_M_EAV	    (1<<ESR_V_EAV)

/* 
** Mask to set all the write-1-to-clear bits
*/

#define ESR_M_INIT	    ESR_M_CEE | \
                            ESR_M_UEE | \
                            ESR_M_CTE | \
                            ESR_M_NXM | \
                            ESR_M_MSE | \
                            ESR_M_MHE | \
                            ESR_M_ICE
/* 
** Mask to set all the write-1-to-clear bits except cee and mse. 
*/

#define ESR_M_ERR_NOT_CEE   ESR_M_UEE | \
                            ESR_M_CTE | \
                            ESR_M_NXM | \
                            ESR_M_MHE
/* 
** Mask to set all the write-1-to-clear bits except cee and mse, 
** plus other bits which could be non-zero.
*/

#define ESR_M_NOT_CEE       ESR_M_UEE | \
                            ESR_M_CTE | \
                            ESR_M_NXM | \
                            ESR_M_MHE | \
                            ESR_M_EAV | \
                            ESR_M_WRE | \
                            ESR_M_SOR | \
                            ESR_M_ICE
/*
**  Error Address Register (EAR)
*/

#define ear_max_bit       29


/*
** I/O Controller (IOC) Register Definitions
*/

#define IOC_CSR_BASE	0x18    /* Bits <33:28> of physical address base */

/* 
** Offsets to I/O controller CSRs 
*/

#define	ioc_hae		0x0	/* Host address extension */
#define	ioc_conf	0x20	/* Configuration cycle type */
#define	ioc_stat0	0x40	/* Error status */
#define	ioc_stat1	0x60	/* Error address */
#define	ioc_tbia	0x80	/* Scatter gather TB invalidate */
#define	ioc_tben	0xa0	/* Scatter gather TB enable */
#define	ioc_pci_rst	0xc0	/* PCI reset */
#define	ioc_w_base0	0x100	/* Window Base 0*/
#define	ioc_w_base1	0x120	/* Window Base 1*/
#define	ioc_w_mask0	0x140	/* Window Mask 0*/
#define	ioc_w_mask1	0x160	/* Window Mask 1*/
#define	ioc_t_base0	0x180	/* Translated Base 0*/
#define	ioc_t_base1	0x1a0	/* Translated Base 1*/

/*
**  Error Status Register (ESR) Bit Summary
**
**	  Loc	Size	Name	    Function
**	 -----	----	----	    ---------------------------------
**	<31:13>	 19 	NBR   	    Error address
**	 <10:8>	  3	CODE	    Error type
**	    <7>	  1	TREF	    Target window reference indicator
**	    <6>	  1	THIT	    TB hit indicator
**	    <5>	  1	LOST	    Lost error
**	    <4>	  1	ERR         Error status valid
**	  <3:0>	  4	CMD	    PCI command field of error cycle
*/

#define	IOC_V_NBR	    13
#define IOC_V_CODE          8
#define IOC_M_CODE          (7<<IOC_V_CODE)
#define IOC_V_TREF    	    7
#define IOC_M_TREF	    (1<<IOC_V_TREF)
#define IOC_V_THIT	    6
#define IOC_M_THIT     	    (1<<IOC_V_THIT)
#define IOC_V_LOST	    5 
#define IOC_M_LOST          (1<<IOC_V_LOST)
#define IOC_V_ERR      	    4 
#define IOC_M_ERR           (1<<IOC_V_ERR)
#define IOC_V_CMD           0
#define IOC_M_CMD      	    (0xF<<IOC_V_CMD)
               
/* 
** Mask to set all the write-1-to-clear bits 
*/

#define IOC_M_INIT	    (IOC_M_LOST | IOC_M_ERR)

#define IOC_V_HAE	    27

/*
**  Window Base Register Bit Summary
**
**	  Loc	Size	Name	    Function
**	 -----	----	----	    ---------------------------------
**	   <33>	  1	WEN	    Window Enable
**	   <32>	  1	SG	    Scatter Gather Enable 
**	<31:20>	 19 	WBASE	    Window Base
*/

#define	IOC_V_WEN	    33

/*
**  PCI Reset Bit Summary
**
**	  Loc	Size	Name	    Function
**	 -----	----	----	    ---------------------------------
**	    <6>	  1	PCI_RST	    PCI Reset Bit
*/

#define	IOC_V_PCI_RST	    6
#define IOC_M_PCI_RST       0x4

#endif /* DC21066 */

/*======================================================================*/
/*                 GENERAL PURPOSE REGISTER DEFINITIONS			*/
/*======================================================================*/

#define	r0		$0
#define r1		$1
#define r2		$2
#define r3		$3
#define r4		$4
#define r5		$5
#define r6		$6
#define r7		$7
#define r8		$8
#define r9		$9
#define r10		$10
#define r11		$11
#define r12		$12
#define r13		$13
#define r14		$14
#define	r15		$15
#define	r16		$16
#define	r17		$17
#define	r18		$18
#define	r19		$19
#define	r20		$20
#define	r21		$21
#define r22		$22
#define r23		$23
#define r24		$24
#define r25		$25
#define r26		$26
#define r27		$27
#define r28		$28
#define r29		$29
#define r30		$30
#define r31		$31

/*======================================================================*/
/*                 FLOATING POINT REGISTER DEFINITIONS			*/
/*======================================================================*/

#define	f0		$f0
#define f1		$f1
#define f2		$f2
#define f3		$f3
#define f4		$f4
#define f5		$f5
#define f6		$f6
#define f7		$f7
#define f8		$f8
#define f9		$f9
#define f10		$f10
#define f11		$f11
#define f12		$f12
#define f13		$f13
#define f14		$f14
#define	f15		$f15
#define	f16		$f16
#define	f17		$f17
#define	f18		$f18
#define	f19		$f19
#define	f20		$f20
#define	f21		$f21
#define f22		$f22
#define f23		$f23
#define f24		$f24
#define f25		$f25
#define f26		$f26
#define f27		$f27
#define f28		$f28
#define f29		$f29
#define f30		$f30
#define f31		$f31

/*======================================================================*/
/*                  PAL TEMPORARY REGISTER DEFINITIONS			*/
/*======================================================================*/

#define	pt0		IPR_M_PAL + 0x0 
#define	pt1		IPR_M_PAL + 0x1
#define	pt2		IPR_M_PAL + 0x2
#define	pt3		IPR_M_PAL + 0x3
#define	pt4		IPR_M_PAL + 0x4
#define	pt5		IPR_M_PAL + 0x5
#define	pt6		IPR_M_PAL + 0x6
#define	pt7		IPR_M_PAL + 0x7
#define	pt8		IPR_M_PAL + 0x8
#define	pt9		IPR_M_PAL + 0x9
#define	pt10		IPR_M_PAL + 0xA
#define	pt11		IPR_M_PAL + 0xB
#define	pt12		IPR_M_PAL + 0xC
#define	pt13		IPR_M_PAL + 0xD
#define	pt14		IPR_M_PAL + 0XE
#define	pt15		IPR_M_PAL + 0xF
#define	pt16		IPR_M_PAL + 0x10
#define	pt17		IPR_M_PAL + 0x11
#define	pt18		IPR_M_PAL + 0x12
#define	pt19		IPR_M_PAL + 0x13
#define	pt20		IPR_M_PAL + 0x14
#define	pt21		IPR_M_PAL + 0x15
#define	pt22		IPR_M_PAL + 0x16
#define	pt23		IPR_M_PAL + 0x17
#define	pt24		IPR_M_PAL + 0x18
#define	pt25		IPR_M_PAL + 0x19
#define	pt26		IPR_M_PAL + 0x1A
#define	pt27		IPR_M_PAL + 0x1B
#define	pt28		IPR_M_PAL + 0x1C
#define	pt29		IPR_M_PAL + 0x1D
#define	pt30		IPR_M_PAL + 0x1E
#define	pt31		IPR_M_PAL + 0x1F

/*======================================================================*/
/*   DECchip 21064/21066 Privileged Architecture Library Entry Points	*/
/*======================================================================*/

/*
**	Entry Name	    Offset (Hex)	Length (Instructions)
**
**	RESET			0000		    8
**	MCHK			0020		   16
**	ARITH			0060		   32
**	INTERRUPT		00E0		   64
**	D_FAULT			01E0		  128
**	ITB_MISS		03E0		  256
**	ITB_ACV			07E0		   64
**	DTB_MISS (Native)	08E0		   64
**	DTB_MISS (PAL)		09E0		  512
**	UNALIGN			11E0		  128
**	OPCDEC			13E0		  256
**	FEN			17E0		  520
**	CALL_PAL (Privileged)	2000
**	CALL_PAL (Unprivileged) 3000
*/

#define PAL_RESET_ENTRY		    0x0000
#define PAL_MCHK_ENTRY		    0x0020
#define PAL_ARITH_ENTRY		    0x0060
#define PAL_INTERRUPT_ENTRY	    0x00E0
#define PAL_D_FAULT_ENTRY	    0x01E0
#define PAL_ITB_MISS_ENTRY	    0x03E0
#define PAL_ITB_ACV_ENTRY	    0x07E0
#define PAL_NDTB_MISS_ENTRY	    0x08E0
#define PAL_PDTB_MISS_ENTRY	    0x09E0
#define PAL_UNALIGN_ENTRY	    0x11E0
#define PAL_OPCDEC_ENTRY	    0x13E0
#define PAL_FEN_ENTRY		    0x17E0
#define PAL_CALL_PAL_PRIV_ENTRY	    0x2000
#define PAL_CALL_PAL_UNPRIV_ENTRY   0x3000

/*
** Architecturally Reserved Opcode Definitions
*/

#define	mtpr	    hw_mtpr
#define	mfpr	    hw_mfpr

#define	ldl_a	    hw_ldl/a
#define ldq_a	    hw_ldq/a
#define stq_a	    hw_stq/a
#define stl_a	    hw_stl/a

#define ldl_p	    hw_ldl/p
#define ldq_p	    hw_ldq/p
#define stl_p	    hw_stl/p
#define stq_p	    hw_stq/p

/*
** Physical mode load-lock and store-conditional variants of
** HW_LD and HW_ST.
*/

#define ldl_pa	    hw_ldl/pa
#define ldq_pa	    hw_ldq/pa
#define stl_pa	    hw_stl/pa
#define stq_pa	    hw_stq/pa

/*
**  This table is an accounting of the DECchip 21064/21066 storage
**  used to implement the SRM defined state for OSF/1.
*/

#define pt2_iccsr	IPR_M_PAL + IPR_M_IBX + 0x2 /* ICCSR shadow register*/

#define pt9_ps		IPR_M_PAL + IPR_M_IBX + 0x9 /* PS shadow register   */

#define ptEntInt	pt10	/* Entry point to HW interrupt dispatch	    */

#if defined(KDEBUG)

#define ptEntDbg	pt11	/* Entry point to kernel debugger           */

#endif /* KDEBUG */

#define	ptEntArith	pt12	/* Entry point to arithmetic trap dispatch  */
#define ptEntMM		pt13	/* Entry point to MM fault dispatch	    */
#define ptEntUna	pt14	/* Entry point to unaligned access dispatch */
#define ptEntSys	pt15	/* Entry point to syscall dispatch	    */
#define ptEntIF		pt16	/* Entry point to instruction fault dispatch*/
#define ptImpure	pt17	/* Pointer to common impure area	    */
#define ptUsp		pt18	/* User stack pointer			    */
#define ptKsp		pt19	/* Kernel stack pointer			    */
#define ptKgp		pt20	/* Kernel global pointer		    */

#define ptIntMask	pt22	/* Interrupt enable masks for IRQ_L<7:0>    */

#define ptSysVal	pt24	/* Per-processor system value		    */
#define ptMces		pt25	/* Machine check error status		    */
#define ptWhami		pt27	/* Who-Am-I ... and why am I here! ;^)	    */
#define ptPtbr		pt28	/* Page table base register		    */
#define ptVptPtr	pt29	/* Virtual page table pointer		    */

#define ptPrevPal	pt30	/* Previous PAL base			    */
#define ptPrcb		pt31	/* Pointer to process control block	    */

#endif /* __DC21064_LOADED */
