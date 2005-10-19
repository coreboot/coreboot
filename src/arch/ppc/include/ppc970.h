/*kernel/include/sys/as_archppc970.h, epos_code, epos_1.0 8/25/04 15:33:07*/
/*----------------------------------------------------------------------------+
|       COPYRIGHT   I B M   CORPORATION 2003
|       LICENSED MATERIAL  -  PROGRAM PROPERTY OF I B M
|	US Government Users Restricted Rights - Use, duplication or
|       disclosure restricted by GSA ADP Schedule Contract with
|	IBM Corp.
+----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
| EPOS
| Author: Maciej P. Tyrlik
| Component: Include file.
| File: sys/as_archppc970.h
| Purpose: Assembler include file for PPC970 processor.
| Changes:
| Date:		Comment:
| -----         --------
| 13-Oct-03     Created							    MPT
+----------------------------------------------------------------------------*/

#ifndef _sys_as_archppc970_h_
#define _sys_as_archppc970_h_

/*----------------------------------------------------------------------------+
| PVR value.
+----------------------------------------------------------------------------*/
#define PVR_970_DD1   		0x00391100
#define PVR_970FX_DD2   	0x003C0200
#define PVR_970FX_DD2_1 	0x003C0201
#define PVR_970FX_DD3   	0x003C0300

/*----------------------------------------------------------------------------+
| Special Purpose Registers.  Xer (64), lr (64), ctr (64), srr0 (64), srr1 (64)
| sprg0 (64), sprg1 (64), sprg2 (64), sprg3 (64), pvr (32) tblr (64), tbur (32)
| registers are defined in as_archppc.h.
+----------------------------------------------------------------------------*/
#define SPR_ACCR    	0x001D  /* 64-bit read/write                        $*/
#define SPR_ASR 	0x0118  /* 64-bit read/write, write hypervisor only  */
#define SPR_DABR	0x03F5  /* 64-bit read/write, write hypervisor only  */
#define SPR_DABRX	0x03F7  /* 64-bit read/write, write hypervisor only  */
#define SPR_DAR     	0x0013  /* 64-bit read/write	                     */
#define SPR_DEC         0x0016  /* 32-bit read/write                         */
#define SPR_DSISR   	0x0012  /* 32-bit read/write                         */
#define SPR_HDEC        0x0136	/* 64-bit read/write, write hypervisor only  */
#define SPR_HID0	0x03F0  /* 64-bit read/write, write hypervisor only  */
#define SPR_HID1    	0x03F1  /* 64-bit read/write, write hypervisor only  */
#define SPR_HID4    	0x03F4  /* 64-bit read/write, write hypervisor only  */
#define SPR_HID5        0x03F6  /* 64-bit read/write, write hypervisor only  */
#define SPR_HIOR    	0x0137  /* 64-bit read/write			     */
#define SPR_HSPRG0  	0x0130  /* 64-bit read/write, write hypervisor only  */
#define SPR_HSPRG1  	0x0131  /* 64-bit read/write, write hypervisor only  */
#define SPR_HSRR0   	0x013A  /* 64-bit read/write, write hypervisor only  */
#define SPR_HSRR1   	0x013B  /* 64-bit read/write, write hypervisor only  */
#define SPR_IMC	        0x030F  /* 64-bit read/write                         */
#define SPR_MMCR0   	0x031B  /* 64-bit read/write                         */
#define SPR_MMCR1   	0x031E  /* 64-bit read/write                         */
#define SPR_MMCRA   	0x0312  /* 64-bit read/write                         */
#define SPR_PIR 	0x03FF  /* 32-bit read                               */
#define SPR_PMC1    	0x0313  /* 32-bit read/write                         */
#define SPR_PMC2    	0x0314  /* 32-bit read/write                         */
#define SPR_PMC3    	0x0315  /* 32-bit read/write                         */
#define SPR_PMC4    	0x0316  /* 32-bit read/write                         */
#define SPR_PMC5    	0x0317  /* 32-bit read/write                         */
#define SPR_PMC6    	0x0318  /* 32-bit read/write                         */
#define SPR_PMC7    	0x0319  /* 32-bit read/write                         */
#define SPR_PMC8    	0x031A  /* 32-bit read/write                         */
#define SPR_SCOMC   	0x0114  /* 64-bit read/write, write hypervisor only  */
#define SPR_SCOMD   	0x0115  /* 64-bit read/write, write hypervisor only  */
#define SPR_SDAR        0x031D	/* 64-bit read/write                         */
#define SPR_SDR1        0x0019  /* 64-bit read/write, write hypervisor only  */
#define SPR_SIAR    	0x031C  /* 64-bit read/write                         */
#define SPR_TBL_WRITE	0x011C	/* 32-bit write				     */
#define SPR_TBU_WRITE	0x011D	/* 32-bit write                              */
#define SPR_TRACE 	0x03FE  /* 64-bit read                              $*/
#define SPR_TRIG0   	0x03D0  /* 64-bit write                              */
#define SPR_TRIG1   	0x03D1  /* 64-bit write                              */
#define SPR_TRIG2   	0x03D2  /* 64-bit write                              */
#define SPR_VRSAVE      0x0100  /* 64-bit read/write			    $*/

/*----------------------------------------------------------------------------+
| Vector status and control register is accessed using the mfvscr and mtvscr
| instructions.
+----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------+
| Machine State Register.  MSR_EE, MSR_PR, MSR_FP, MSR_ME, MSR_FE0, MSR_FE1,
| register bits are defined in as_archppc.h.  This is a 64-bit register.
+----------------------------------------------------------------------------*/
#define MSR_SF          0x8000000000000000 /* 64/32 bit mode indicator       */
#define MSR_HV		0x1000000000000000 /* hypervisor mode		     */
#define MSR_VMX		0x0000000002000000 /* vmx unit available   	     */
#define MSR_POW		0x0000000000040000 /* power management enable	     */
#define MSR_SE 		0x0000000000000400 /* single step          	     */
#define MSR_BE 		0x0000000000000200 /* branch trace      	     */
#define MSR_IS  	0x0000000000000020 /* instruction address space	     */
#define MSR_DS  	0x0000000000000010 /* data address space	     */
#define MSR_PM		0x0000000000000004 /* performance monitor	     */
#define MSR_RI		0x0000000000000002 /* recoverable interrupt          */

/*----------------------------------------------------------------------------+
| HID0 bits.
+----------------------------------------------------------------------------*/
#define HID0_ONEPPC	0x8000000000000000
#define HID0_SINGLE	0x4000000000000000
#define HID0_ISYNC_SC	0x2000000000000000
#define HID0_SERIAL_G	0x1000000000000000
#define HID0_DEEP_NAP  	0x0100000000000000
#define HID0_NAP     	0x0040000000000000
#define HID0_DPM     	0x0010000000000000
#define HID0_TR_GR   	0x0004000000000000
#define HID0_TR_DIS   	0x0002000000000000
#define HID0_NHR      	0x0001000000000000
#define HID0_INORDER    0x0000800000000000
#define HID0_ENH_TR     0x0000400000000000
#define HID0_TB_CTRL    0x0000200000000000
#define HID0_EXT_TB_EN  0x0000100000000000
#define HID0_CIABR_EN   0x0000020000000000
#define HID0_HDEC_EN    0x0000010000000000
#define HID0_EB_THERM   0x0000008000000000
#define HID0_EN_ATTN    0x0000000100000000
#define HID0_EN_MAC     0x0000000080000000

/*----------------------------------------------------------------------------+
| HID1 bits.
+----------------------------------------------------------------------------*/
#define HID1_BHT_PM	0xE000000000000000
#define HID1_BHT_STATIC 0x0000000000000000
#define HID1_BHT_GLOBAL 0x4000000000000000
#define HID1_BHT_LOCAL  0x8000000000000000
#define HID1_BHT_GL_LO  0xC000000000000000
#define HID1_BHT_GL_CO  0x6000000000000000
#define HID1_BHT_FULL   0xE000000000000000
#define HID1_EN_LS 	0x1000000000000000
#define HID1_EN_CC 	0x0800000000000000
#define HID1_EN_IC 	0x0400000000000000
#define HID1_PF_MASK	0x0180000000000000
#define HID1_PF_NSA	0x0080000000000000
#define HID1_PF_NSA_P	0x0100000000000000
#define HID1_PF_DIS  	0x0180000000000000
#define HID1_EN_ICBI 	0x0040000000000000
#define HID1_EN_IF_CACH 0x0020000000000000
#define HID1_EN_IC_REC  0x0010000000000000
#define HID1_EN_ID_REC  0x0008000000000000
#define HID1_EN_ER_REC  0x0004000000000000
#define HID1_IC_PE      0x0002000000000000
#define HID1_ICD0_PE    0x0001000000000000
#define HID1_ICD1_PE    0x0000800000000000
#define HID1_IER_PE     0x0000400000000000
#define HID1_EN_SP_ITW  0x0000200000000000
#define HID1_S_CHICKEN  0x0000100000000000

/*----------------------------------------------------------------------------+
| HID4 bits.
+----------------------------------------------------------------------------*/
#define HID4_LPES0	0x8000000000000000
#define HID4_RMLR12_MSK	0x6000000000000000
#define HID4_LPID25_MSK	0x1E00000000000000
#define HID4_RMOR_MASK 	0x01FFFE0000000000
#define HID4_RM_CI     	0x0000010000000000
#define HID4_FORCE_AI  	0x0000008000000000
#define HID4_DIS_PERF  	0x0000004000000000
#define HID4_RES_PERF  	0x0000002000000000
#define HID4_EN_SP_DTW 	0x0000001000000000
#define HID4_L1DC_FLSH 	0x0000000800000000
#define HID4_D_DERAT_P1	0x0000000400000000
#define HID4_D_DERAT_P2	0x0000000200000000
#define HID4_D_DERAT_G 	0x0000000100000000
#define HID4_D_DERAT_S1 0x0000000040000000
#define HID4_D_DERAT_S2 0x0000000080000000
#define HID4_DC_TP_S1   0x0000000020000000
#define HID4_DC_TP_S2   0x0000000010000000
#define HID4_DC_TP_GEN  0x0000000008000000
#define HID4_DC_SET1    0x0000000004000000
#define HID4_DC_SET2    0x0000000002000000
#define HID4_DC_DP_S1   0x0000000001000000
#define HID4_DC_DP_S2   0x0000000000800000
#define HID4_DC_DP_GEN  0x0000000000400000
#define HID4_R_TAG1P_CH 0x0000000000200000
#define HID4_R_TAG2P_CH 0x0000000000100000
#define HID4_TLB_PC1    0x0000000000080000
#define HID4_TLB_PC2    0x0000000000040000
#define HID4_TLB_PC3    0x0000000000020000
#define HID4_TLB_PC4    0x0000000000010000
#define HID4_TLB_P_GEN  0x0000000000008000
#define HID4_TLB_SET1   0x0000000000003800
#define HID4_TLB_SET2   0x0000000000005800
#define HID4_TLB_SET3   0x0000000000006800
#define HID4_TLB_SET4   0x0000000000007000
#define HID4_DIS_SLBPC  0x0000000000000400
#define HID4_DIS_SLBPG  0x0000000000000200
#define HID4_MCK_INJ    0x0000000000000100
#define HID4_DIS_STFWD  0x0000000000000080
#define HID4_LPES1      0x0000000000000040
#define HID4_RMLR0_MSK  0x0000000000000020
#define HID4_DIS_SPLARX 0x0000000000000008
#define HID4_LP_PG_EN   0x0000000000000004
#define HID4_LPID01_MSK 0x0000000000000003

/*----------------------------------------------------------------------------+
| HID5 bits.
+----------------------------------------------------------------------------*/
#define HID5_HRMOR_MASK 0x00000000FFFF0000
#define HID5_DC_MCK     0x0000000000002000
#define HID5_DIS_PWRSAV 0x0000000000001000
#define HID5_FORCE_G    0x0000000000000800
#define HID5_DC_REPL    0x0000000000000400
#define HID5_HWR_STMS   0x0000000000000200
#define HID5_DST_NOOP   0x0000000000000100
#define HID5_DCBZ_SIZE  0x0000000000000080
#define HID5_DCBZ32_ILL 0x0000000000000040
#define HID5_TLB_MAP    0x0000000000000020
#define HID5_IMQ_PORT   0x0000000000000010
#define HID5_LMP_SIZE0  0x0000000000000008
#define HID5_DPFLOOD    0x0000000000000004
#define HID5_TCH_NOP    0x0000000000000002
#define HID5_LMP_SIZE1  0x0000000000000001

/*----------------------------------------------------------------------------+
| Specific SRR1 bit definitions for Machine Check.
+----------------------------------------------------------------------------*/
#define SRR1_IFU_UNREC  0x0000000000200000
#define SRR1_LOAD_STORE 0x0000000000100000
#define SRR1_SLB_PARITY 0x0000000000040000
#define SRR1_TLB_PARITY 0x0000000000080000
#define SRR1_ITLB_RELOA 0x00000000000C0000
#define SRR1_RI         0x0000000000000002

#endif /* _sys_as_archppc970_h_ */
