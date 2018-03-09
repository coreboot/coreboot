#ifndef __BDK_CSRS_L2C_H__
#define __BDK_CSRS_L2C_H__
/* This file is auto-generated. Do not edit */

/***********************license start***************
 * Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
 * reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.

 *   * Neither the name of Cavium Inc. nor the names of
 *     its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written
 *     permission.

 * This Software, including technical data, may be subject to U.S. export  control
 * laws, including the U.S. Export Administration Act and its  associated
 * regulations, and may be subject to export or import  regulations in other
 * countries.

 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND CAVIUM  NETWORKS MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY REPRESENTATION OR
 * DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT DEFECTS, AND CAVIUM
 * SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES OF TITLE,
 * MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF
 * VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. THE ENTIRE  RISK ARISING OUT OF USE OR
 * PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
 ***********************license end**************************************/


/**
 * @file
 *
 * Configuration and status register (CSR) address and type definitions for
 * Cavium L2C.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration inv_cmd_e
 *
 * INTERNAL: INV Command Enumeration
 *
 * Enumerates the different INV command encodings.
 */
#define BDK_INV_CMD_E_ALLEX (0xc)
#define BDK_INV_CMD_E_ASIDE1 (0xd)
#define BDK_INV_CMD_E_GBLSYNC (0xf)
#define BDK_INV_CMD_E_IALLU (9)
#define BDK_INV_CMD_E_INV (8)
#define BDK_INV_CMD_E_IPAS2E1 (7)
#define BDK_INV_CMD_E_IVAU (4)
#define BDK_INV_CMD_E_NOP (0)
#define BDK_INV_CMD_E_SEV (0xe)
#define BDK_INV_CMD_E_VAAE1 (6)
#define BDK_INV_CMD_E_VAEX (5)
#define BDK_INV_CMD_E_VMALLE1 (0xa)
#define BDK_INV_CMD_E_VMALLS12 (0xb)

/**
 * Enumeration ioc_cmd_e
 *
 * INTERNAL: IOC Command Enumeration
 *
 * Enumerates the different IOC command encodings.
 */
#define BDK_IOC_CMD_E_ADDR (6)
#define BDK_IOC_CMD_E_IAADD (0xc)
#define BDK_IOC_CMD_E_IACAS (0xa)
#define BDK_IOC_CMD_E_IACLR (0xd)
#define BDK_IOC_CMD_E_IASET (0xe)
#define BDK_IOC_CMD_E_IASWP (0xb)
#define BDK_IOC_CMD_E_IDLE (0)
#define BDK_IOC_CMD_E_LMTST (3)
#define BDK_IOC_CMD_E_LOAD (2)
#define BDK_IOC_CMD_E_SLILD (8)
#define BDK_IOC_CMD_E_SLIST (7)
#define BDK_IOC_CMD_E_STORE (1)
#define BDK_IOC_CMD_E_STOREP (9)

/**
 * Enumeration ior_cmd_e
 *
 * INTERNAL: IOR Command Enumeration
 *
 * Enumerates the different IOR command encodings.
 */
#define BDK_IOR_CMD_E_DATA (1)
#define BDK_IOR_CMD_E_IDLE (0)
#define BDK_IOR_CMD_E_SLIRSP (3)

/**
 * Enumeration l2c_bar_e
 *
 * L2C Base Address Register Enumeration
 * Enumerates the base address registers.
 */
#define BDK_L2C_BAR_E_L2C_PF_BAR0 (0x87e080800000ll)
#define BDK_L2C_BAR_E_L2C_PF_BAR0_SIZE 0x800000ull

/**
 * Enumeration l2c_dat_errprio_e
 *
 * L2C Quad Error Priority Enumeration
 * Enumerates the different quad error priorities.
 */
#define BDK_L2C_DAT_ERRPRIO_E_FBFDBE (4)
#define BDK_L2C_DAT_ERRPRIO_E_FBFSBE (1)
#define BDK_L2C_DAT_ERRPRIO_E_L2DDBE (6)
#define BDK_L2C_DAT_ERRPRIO_E_L2DSBE (3)
#define BDK_L2C_DAT_ERRPRIO_E_NBE (0)
#define BDK_L2C_DAT_ERRPRIO_E_SBFDBE (5)
#define BDK_L2C_DAT_ERRPRIO_E_SBFSBE (2)

/**
 * Enumeration l2c_tad_prf_sel_e
 *
 * L2C TAD Performance Counter Select Enumeration
 * Enumerates the different TAD performance counter selects.
 */
#define BDK_L2C_TAD_PRF_SEL_E_L2T_HIT (1)
#define BDK_L2C_TAD_PRF_SEL_E_L2T_MISS (2)
#define BDK_L2C_TAD_PRF_SEL_E_L2T_NOALLOC (3)
#define BDK_L2C_TAD_PRF_SEL_E_L2_OPEN_OCI (0x48)
#define BDK_L2C_TAD_PRF_SEL_E_L2_RTG_VIC (0x44)
#define BDK_L2C_TAD_PRF_SEL_E_L2_VIC (4)
#define BDK_L2C_TAD_PRF_SEL_E_LFB_OCC (7)
#define BDK_L2C_TAD_PRF_SEL_E_LMC_WR (0x4e)
#define BDK_L2C_TAD_PRF_SEL_E_LMC_WR_SBLKDTY (0x4f)
#define BDK_L2C_TAD_PRF_SEL_E_LOOKUP (0x40)
#define BDK_L2C_TAD_PRF_SEL_E_LOOKUP_ALL (0x44)
#define BDK_L2C_TAD_PRF_SEL_E_LOOKUP_MIB (0x43)
#define BDK_L2C_TAD_PRF_SEL_E_LOOKUP_XMC_LCL (0x41)
#define BDK_L2C_TAD_PRF_SEL_E_LOOKUP_XMC_RMT (0x42)
#define BDK_L2C_TAD_PRF_SEL_E_NONE (0)
#define BDK_L2C_TAD_PRF_SEL_E_OCI_FLDX_TAG_E_DAT (0x6d)
#define BDK_L2C_TAD_PRF_SEL_E_OCI_FLDX_TAG_E_NODAT (0x6c)
#define BDK_L2C_TAD_PRF_SEL_E_OCI_FWD_CYC_HIT (0x69)
#define BDK_L2C_TAD_PRF_SEL_E_OCI_FWD_RACE (0x6a)
#define BDK_L2C_TAD_PRF_SEL_E_OCI_HAKS (0x6b)
#define BDK_L2C_TAD_PRF_SEL_E_OCI_RLDD (0x6e)
#define BDK_L2C_TAD_PRF_SEL_E_OCI_RLDD_PEMD (0x6f)
#define BDK_L2C_TAD_PRF_SEL_E_OCI_RRQ_DAT_CNT (0x70)
#define BDK_L2C_TAD_PRF_SEL_E_OCI_RRQ_DAT_DMASK (0x71)
#define BDK_L2C_TAD_PRF_SEL_E_OCI_RSP_DAT_CNT (0x72)
#define BDK_L2C_TAD_PRF_SEL_E_OCI_RSP_DAT_DMASK (0x73)
#define BDK_L2C_TAD_PRF_SEL_E_OCI_RSP_DAT_VICD_CNT (0x74)
#define BDK_L2C_TAD_PRF_SEL_E_OCI_RSP_DAT_VICD_DMASK (0x75)
#define BDK_L2C_TAD_PRF_SEL_E_OCI_RTG_WAIT (0x68)
#define BDK_L2C_TAD_PRF_SEL_E_OPEN_CCPI (0xa)
#define BDK_L2C_TAD_PRF_SEL_E_QDX_BNKS(a) (0x82 + 0x10 * (a))
#define BDK_L2C_TAD_PRF_SEL_E_QDX_IDX(a) (0x80 + 0x10 * (a))
#define BDK_L2C_TAD_PRF_SEL_E_QDX_RDAT(a) (0x81 + 0x10 * (a))
#define BDK_L2C_TAD_PRF_SEL_E_QDX_WDAT(a) (0x83 + 0x10 * (a))
#define BDK_L2C_TAD_PRF_SEL_E_RTG_ALC (0x5d)
#define BDK_L2C_TAD_PRF_SEL_E_RTG_ALC_EVICT (0x76)
#define BDK_L2C_TAD_PRF_SEL_E_RTG_ALC_HIT (0x5e)
#define BDK_L2C_TAD_PRF_SEL_E_RTG_ALC_HITWB (0x5f)
#define BDK_L2C_TAD_PRF_SEL_E_RTG_ALC_VIC (0x77)
#define BDK_L2C_TAD_PRF_SEL_E_RTG_HIT (0x41)
#define BDK_L2C_TAD_PRF_SEL_E_RTG_MISS (0x42)
#define BDK_L2C_TAD_PRF_SEL_E_SC_FAIL (5)
#define BDK_L2C_TAD_PRF_SEL_E_SC_PASS (6)
#define BDK_L2C_TAD_PRF_SEL_E_STC_LCL (0x64)
#define BDK_L2C_TAD_PRF_SEL_E_STC_LCL_FAIL (0x65)
#define BDK_L2C_TAD_PRF_SEL_E_STC_RMT (0x62)
#define BDK_L2C_TAD_PRF_SEL_E_STC_RMT_FAIL (0x63)
#define BDK_L2C_TAD_PRF_SEL_E_STC_TOTAL (0x60)
#define BDK_L2C_TAD_PRF_SEL_E_STC_TOTAL_FAIL (0x61)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_ALC_HIT (0x48)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_ALC_LCL_CLNVIC (0x59)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_ALC_LCL_DTYVIC (0x5a)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_ALC_LCL_EVICT (0x58)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_ALC_MISS (0x49)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_ALC_NALC (0x4a)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_ALC_RMT_EVICT (0x5b)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_ALC_RMT_VIC (0x5c)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_ALC_RTG_HIT (0x50)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_ALC_RTG_HITE (0x51)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_ALC_RTG_HITS (0x52)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_ALC_RTG_MISS (0x53)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_NALC_HIT (0x4b)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_NALC_MISS (0x4c)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_NALC_RTG_HIT (0x54)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_NALC_RTG_HITE (0x56)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_NALC_RTG_HITS (0x57)
#define BDK_L2C_TAD_PRF_SEL_E_TAG_NALC_RTG_MISS (0x55)
#define BDK_L2C_TAD_PRF_SEL_E_WAIT_LFB (8)
#define BDK_L2C_TAD_PRF_SEL_E_WAIT_VAB (9)

/**
 * Enumeration l2c_tag_errprio_e
 *
 * L2C Tag Error Priority Enumeration
 * Enumerates the different TAG error priorities.
 */
#define BDK_L2C_TAG_ERRPRIO_E_DBE (3)
#define BDK_L2C_TAG_ERRPRIO_E_NBE (0)
#define BDK_L2C_TAG_ERRPRIO_E_NOWAY (1)
#define BDK_L2C_TAG_ERRPRIO_E_SBE (2)

/**
 * Enumeration oci_ireq_cmd_e
 *
 * INTERNAL: OCI IREQ Command Enumeration
 */
#define BDK_OCI_IREQ_CMD_E_IAADD (0x10)
#define BDK_OCI_IREQ_CMD_E_IACAS (0x15)
#define BDK_OCI_IREQ_CMD_E_IACLR (0x12)
#define BDK_OCI_IREQ_CMD_E_IASET (0x13)
#define BDK_OCI_IREQ_CMD_E_IASWP (0x14)
#define BDK_OCI_IREQ_CMD_E_IDLE (0x1f)
#define BDK_OCI_IREQ_CMD_E_IOBADDR (6)
#define BDK_OCI_IREQ_CMD_E_IOBADDRA (7)
#define BDK_OCI_IREQ_CMD_E_IOBLD (0)
#define BDK_OCI_IREQ_CMD_E_IOBST (2)
#define BDK_OCI_IREQ_CMD_E_IOBSTA (3)
#define BDK_OCI_IREQ_CMD_E_IOBSTP (4)
#define BDK_OCI_IREQ_CMD_E_IOBSTPA (5)
#define BDK_OCI_IREQ_CMD_E_LMTST (8)
#define BDK_OCI_IREQ_CMD_E_LMTSTA (9)
#define BDK_OCI_IREQ_CMD_E_SLILD (0x1c)
#define BDK_OCI_IREQ_CMD_E_SLIST (0x1d)

/**
 * Enumeration oci_irsp_cmd_e
 *
 * INTERNAL: OCI IRSP Command Enumeration
 */
#define BDK_OCI_IRSP_CMD_E_IDLE (0x1f)
#define BDK_OCI_IRSP_CMD_E_IOBACK (1)
#define BDK_OCI_IRSP_CMD_E_IOBRSP (0)
#define BDK_OCI_IRSP_CMD_E_SLIRSP (2)

/**
 * Enumeration oci_mfwd_cmd_e
 *
 * INTERNAL: OCI MFWD Command Enumeration
 */
#define BDK_OCI_MFWD_CMD_E_FEVX_EH (0xb)
#define BDK_OCI_MFWD_CMD_E_FEVX_OH (0xc)
#define BDK_OCI_MFWD_CMD_E_FLDRO_E (0)
#define BDK_OCI_MFWD_CMD_E_FLDRO_O (1)
#define BDK_OCI_MFWD_CMD_E_FLDRS_E (2)
#define BDK_OCI_MFWD_CMD_E_FLDRS_EH (4)
#define BDK_OCI_MFWD_CMD_E_FLDRS_O (3)
#define BDK_OCI_MFWD_CMD_E_FLDRS_OH (5)
#define BDK_OCI_MFWD_CMD_E_FLDT_E (6)
#define BDK_OCI_MFWD_CMD_E_FLDX_E (7)
#define BDK_OCI_MFWD_CMD_E_FLDX_EH (9)
#define BDK_OCI_MFWD_CMD_E_FLDX_O (8)
#define BDK_OCI_MFWD_CMD_E_FLDX_OH (0xa)
#define BDK_OCI_MFWD_CMD_E_IDLE (0x1f)
#define BDK_OCI_MFWD_CMD_E_SINV (0xd)
#define BDK_OCI_MFWD_CMD_E_SINV_H (0xe)

/**
 * Enumeration oci_mreq_cmd_e
 *
 * INTERNAL: OCI MREQ Command Enumeration
 */
#define BDK_OCI_MREQ_CMD_E_GINV (0x14)
#define BDK_OCI_MREQ_CMD_E_GSYNC (0x18)
#define BDK_OCI_MREQ_CMD_E_IDLE (0x1f)
#define BDK_OCI_MREQ_CMD_E_RADD (0xd)
#define BDK_OCI_MREQ_CMD_E_RC2D_O (6)
#define BDK_OCI_MREQ_CMD_E_RC2D_S (7)
#define BDK_OCI_MREQ_CMD_E_RCAS (0x13)
#define BDK_OCI_MREQ_CMD_E_RCAS_O (0x15)
#define BDK_OCI_MREQ_CMD_E_RCAS_S (0x16)
#define BDK_OCI_MREQ_CMD_E_RCLR (0x12)
#define BDK_OCI_MREQ_CMD_E_RDEC (0xf)
#define BDK_OCI_MREQ_CMD_E_REOR (0xb)
#define BDK_OCI_MREQ_CMD_E_RINC (0xe)
#define BDK_OCI_MREQ_CMD_E_RLDD (0)
#define BDK_OCI_MREQ_CMD_E_RLDI (1)
#define BDK_OCI_MREQ_CMD_E_RLDT (2)
#define BDK_OCI_MREQ_CMD_E_RLDWB (4)
#define BDK_OCI_MREQ_CMD_E_RLDX (5)
#define BDK_OCI_MREQ_CMD_E_RLDY (3)
#define BDK_OCI_MREQ_CMD_E_RSET (0x11)
#define BDK_OCI_MREQ_CMD_E_RSMAX (0x1b)
#define BDK_OCI_MREQ_CMD_E_RSMIN (0x1c)
#define BDK_OCI_MREQ_CMD_E_RSTC (0x17)
#define BDK_OCI_MREQ_CMD_E_RSTC_O (0x19)
#define BDK_OCI_MREQ_CMD_E_RSTC_S (0x1a)
#define BDK_OCI_MREQ_CMD_E_RSTP (0xa)
#define BDK_OCI_MREQ_CMD_E_RSTT (8)
#define BDK_OCI_MREQ_CMD_E_RSTY (9)
#define BDK_OCI_MREQ_CMD_E_RSWP (0x10)
#define BDK_OCI_MREQ_CMD_E_RUMAX (0x1d)
#define BDK_OCI_MREQ_CMD_E_RUMIN (0x1e)

/**
 * Enumeration oci_mrsp_cmd_e
 *
 * INTERNAL: OCI MRSP Command Enumeration
 */
#define BDK_OCI_MRSP_CMD_E_GSDN (0x18)
#define BDK_OCI_MRSP_CMD_E_HAKD (4)
#define BDK_OCI_MRSP_CMD_E_HAKI (6)
#define BDK_OCI_MRSP_CMD_E_HAKN_S (5)
#define BDK_OCI_MRSP_CMD_E_HAKS (7)
#define BDK_OCI_MRSP_CMD_E_HAKV (8)
#define BDK_OCI_MRSP_CMD_E_IDLE (0x1f)
#define BDK_OCI_MRSP_CMD_E_P2DF (0xd)
#define BDK_OCI_MRSP_CMD_E_PACK (0xc)
#define BDK_OCI_MRSP_CMD_E_PATM (0xb)
#define BDK_OCI_MRSP_CMD_E_PEMD (0xa)
#define BDK_OCI_MRSP_CMD_E_PSHA (9)
#define BDK_OCI_MRSP_CMD_E_VICC (1)
#define BDK_OCI_MRSP_CMD_E_VICD (0)
#define BDK_OCI_MRSP_CMD_E_VICDHI (3)
#define BDK_OCI_MRSP_CMD_E_VICS (2)

/**
 * Enumeration rsc_cmd_e
 *
 * INTERNAL: RSC Command Enumeration
 *
 * Enumerates the different RSC command encodings.
 */
#define BDK_RSC_CMD_E_FLDN (3)
#define BDK_RSC_CMD_E_GSDN (2)
#define BDK_RSC_CMD_E_IACK (5)
#define BDK_RSC_CMD_E_IFDN (1)
#define BDK_RSC_CMD_E_NOP (0)
#define BDK_RSC_CMD_E_SCDN (6)
#define BDK_RSC_CMD_E_SCFL (7)
#define BDK_RSC_CMD_E_STDN (4)

/**
 * Enumeration xmc_cmd_e
 *
 * INTERNAL: XMC Command Enumeration
 *
 * Enumerates the different XMC command encodings.
 */
#define BDK_XMC_CMD_E_AADD (0x28)
#define BDK_XMC_CMD_E_ACAS (0x26)
#define BDK_XMC_CMD_E_ACLR (0x29)
#define BDK_XMC_CMD_E_ADEC (0x25)
#define BDK_XMC_CMD_E_AEOR (0x2a)
#define BDK_XMC_CMD_E_AINC (0x24)
#define BDK_XMC_CMD_E_ALLEX (0x3c)
#define BDK_XMC_CMD_E_ASET (0x2b)
#define BDK_XMC_CMD_E_ASIDE1 (0x3d)
#define BDK_XMC_CMD_E_ASMAX (0x2c)
#define BDK_XMC_CMD_E_ASMIN (0x2d)
#define BDK_XMC_CMD_E_ASWP (0x27)
#define BDK_XMC_CMD_E_AUMAX (0x2e)
#define BDK_XMC_CMD_E_AUMIN (0x2f)
#define BDK_XMC_CMD_E_DWB (5)
#define BDK_XMC_CMD_E_GBLSYNC (0x3f)
#define BDK_XMC_CMD_E_IAADD (0x68)
#define BDK_XMC_CMD_E_IACAS (0x66)
#define BDK_XMC_CMD_E_IACLR (0x69)
#define BDK_XMC_CMD_E_IALLU (0x39)
#define BDK_XMC_CMD_E_IASET (0x6b)
#define BDK_XMC_CMD_E_IASWP (0x67)
#define BDK_XMC_CMD_E_INVL2 (0x1c)
#define BDK_XMC_CMD_E_IOBADDR (0x43)
#define BDK_XMC_CMD_E_IOBADDRA (0x53)
#define BDK_XMC_CMD_E_IOBLD (0x40)
#define BDK_XMC_CMD_E_IOBST (0x41)
#define BDK_XMC_CMD_E_IOBSTA (0x51)
#define BDK_XMC_CMD_E_IOBSTP (0x42)
#define BDK_XMC_CMD_E_IOBSTPA (0x52)
#define BDK_XMC_CMD_E_IPAS2E1 (0x37)
#define BDK_XMC_CMD_E_IVAU (0x34)
#define BDK_XMC_CMD_E_LCKL2 (0x1f)
#define BDK_XMC_CMD_E_LDD (8)
#define BDK_XMC_CMD_E_LDDT (0xc)
#define BDK_XMC_CMD_E_LDE (0xb)
#define BDK_XMC_CMD_E_LDI (2)
#define BDK_XMC_CMD_E_LDP (7)
#define BDK_XMC_CMD_E_LDT (1)
#define BDK_XMC_CMD_E_LDWB (0xd)
#define BDK_XMC_CMD_E_LDY (6)
#define BDK_XMC_CMD_E_LMTST (0x45)
#define BDK_XMC_CMD_E_LMTSTA (0x55)
#define BDK_XMC_CMD_E_LTGL2I (0x19)
#define BDK_XMC_CMD_E_NOP (0)
#define BDK_XMC_CMD_E_PL2 (3)
#define BDK_XMC_CMD_E_PL2T (0x16)
#define BDK_XMC_CMD_E_PS2 (0xa)
#define BDK_XMC_CMD_E_PS2T (0x17)
#define BDK_XMC_CMD_E_PSL1 (9)
#define BDK_XMC_CMD_E_RPL2 (4)
#define BDK_XMC_CMD_E_RSTP (0xf)
#define BDK_XMC_CMD_E_SEV (0x3e)
#define BDK_XMC_CMD_E_STC (0x13)
#define BDK_XMC_CMD_E_STF (0x10)
#define BDK_XMC_CMD_E_STFIL1 (0x14)
#define BDK_XMC_CMD_E_STGL2I (0x1a)
#define BDK_XMC_CMD_E_STP (0x12)
#define BDK_XMC_CMD_E_STT (0x11)
#define BDK_XMC_CMD_E_STTIL1 (0x15)
#define BDK_XMC_CMD_E_STY (0xe)
#define BDK_XMC_CMD_E_VAAE1 (0x36)
#define BDK_XMC_CMD_E_VAEX (0x35)
#define BDK_XMC_CMD_E_VMALLE1 (0x3a)
#define BDK_XMC_CMD_E_VMALLS12 (0x3b)
#define BDK_XMC_CMD_E_WBIL2 (0x1d)
#define BDK_XMC_CMD_E_WBIL2I (0x18)
#define BDK_XMC_CMD_E_WBL2 (0x1e)
#define BDK_XMC_CMD_E_WBL2I (0x1b)

/**
 * Register (RSL) l2c_asc_region#_attr
 *
 * L2C Address Space Control Region Attributes Registers
 */
union bdk_l2c_asc_regionx_attr
{
    uint64_t u;
    struct bdk_l2c_asc_regionx_attr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t s_en                  : 1;  /**< [  1:  1](R/W) Enables secure access to region.
                                                                 Undefined if both [S_EN] and [NS_EN] are set for the same region. */
        uint64_t ns_en                 : 1;  /**< [  0:  0](R/W) Enables nonsecure access to region.
                                                                 Undefined if both [S_EN] and [NS_EN] are set for the same region.
                                                                 See also DFA_ASC_REGION()_ATTR[NS_EN]. */
#else /* Word 0 - Little Endian */
        uint64_t ns_en                 : 1;  /**< [  0:  0](R/W) Enables nonsecure access to region.
                                                                 Undefined if both [S_EN] and [NS_EN] are set for the same region.
                                                                 See also DFA_ASC_REGION()_ATTR[NS_EN]. */
        uint64_t s_en                  : 1;  /**< [  1:  1](R/W) Enables secure access to region.
                                                                 Undefined if both [S_EN] and [NS_EN] are set for the same region. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    struct bdk_l2c_asc_regionx_attr_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t s_en                  : 1;  /**< [  1:  1](R/W) Enables secure access to region.
                                                                 Undefined if both [S_EN] and [NS_EN] are set for the same region. */
        uint64_t ns_en                 : 1;  /**< [  0:  0](R/W) Enables nonsecure access to region.
                                                                 Undefined if both [S_EN] and [NS_EN] are set for the same region.

                                                                 Internal:
                                                                 See also DFA_ASC_REGION()_ATTR[NS_EN]. */
#else /* Word 0 - Little Endian */
        uint64_t ns_en                 : 1;  /**< [  0:  0](R/W) Enables nonsecure access to region.
                                                                 Undefined if both [S_EN] and [NS_EN] are set for the same region.

                                                                 Internal:
                                                                 See also DFA_ASC_REGION()_ATTR[NS_EN]. */
        uint64_t s_en                  : 1;  /**< [  1:  1](R/W) Enables secure access to region.
                                                                 Undefined if both [S_EN] and [NS_EN] are set for the same region. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_l2c_asc_regionx_attr_s cn88xx; */
    /* struct bdk_l2c_asc_regionx_attr_cn81xx cn83xx; */
};
typedef union bdk_l2c_asc_regionx_attr bdk_l2c_asc_regionx_attr_t;

static inline uint64_t BDK_L2C_ASC_REGIONX_ATTR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_ASC_REGIONX_ATTR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX) && (a<=3))
        return 0x87e080801010ll + 0x40ll * ((a) & 0x3);
    __bdk_csr_fatal("L2C_ASC_REGIONX_ATTR", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_ASC_REGIONX_ATTR(a) bdk_l2c_asc_regionx_attr_t
#define bustype_BDK_L2C_ASC_REGIONX_ATTR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_ASC_REGIONX_ATTR(a) "L2C_ASC_REGIONX_ATTR"
#define device_bar_BDK_L2C_ASC_REGIONX_ATTR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_ASC_REGIONX_ATTR(a) (a)
#define arguments_BDK_L2C_ASC_REGIONX_ATTR(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_asc_region#_end
 *
 * L2C Address Space Control Region End Address Registers
 */
union bdk_l2c_asc_regionx_end
{
    uint64_t u;
    struct bdk_l2c_asc_regionx_end_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_40_63        : 24;
        uint64_t addr                  : 20; /**< [ 39: 20](R/W) Node-local physical address \<39:20\> marking the inclusive end of the corresponding ASC
                                                                 region.
                                                                 Note that the region includes this address.
                                                                 Software must ensure that regions do not overlap.
                                                                 To specify an empty region, clear both the [S_EN] and [NS_EN] fields of
                                                                 the corresponding L2C_ASC_REGION()_ATTR register. */
        uint64_t reserved_0_19         : 20;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_19         : 20;
        uint64_t addr                  : 20; /**< [ 39: 20](R/W) Node-local physical address \<39:20\> marking the inclusive end of the corresponding ASC
                                                                 region.
                                                                 Note that the region includes this address.
                                                                 Software must ensure that regions do not overlap.
                                                                 To specify an empty region, clear both the [S_EN] and [NS_EN] fields of
                                                                 the corresponding L2C_ASC_REGION()_ATTR register. */
        uint64_t reserved_40_63        : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_asc_regionx_end_s cn; */
};
typedef union bdk_l2c_asc_regionx_end bdk_l2c_asc_regionx_end_t;

static inline uint64_t BDK_L2C_ASC_REGIONX_END(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_ASC_REGIONX_END(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX) && (a<=3))
        return 0x87e080801008ll + 0x40ll * ((a) & 0x3);
    __bdk_csr_fatal("L2C_ASC_REGIONX_END", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_ASC_REGIONX_END(a) bdk_l2c_asc_regionx_end_t
#define bustype_BDK_L2C_ASC_REGIONX_END(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_ASC_REGIONX_END(a) "L2C_ASC_REGIONX_END"
#define device_bar_BDK_L2C_ASC_REGIONX_END(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_ASC_REGIONX_END(a) (a)
#define arguments_BDK_L2C_ASC_REGIONX_END(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_asc_region#_start
 *
 * L2C Address Space Control Region Start Address Registers
 */
union bdk_l2c_asc_regionx_start
{
    uint64_t u;
    struct bdk_l2c_asc_regionx_start_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_40_63        : 24;
        uint64_t addr                  : 20; /**< [ 39: 20](R/W) Node-local physical address \<39:20\> marking the start of the corresponding ASC region.
                                                                 Software must ensure that regions do not overlap. */
        uint64_t reserved_0_19         : 20;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_19         : 20;
        uint64_t addr                  : 20; /**< [ 39: 20](R/W) Node-local physical address \<39:20\> marking the start of the corresponding ASC region.
                                                                 Software must ensure that regions do not overlap. */
        uint64_t reserved_40_63        : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_asc_regionx_start_s cn; */
};
typedef union bdk_l2c_asc_regionx_start bdk_l2c_asc_regionx_start_t;

static inline uint64_t BDK_L2C_ASC_REGIONX_START(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_ASC_REGIONX_START(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX) && (a<=3))
        return 0x87e080801000ll + 0x40ll * ((a) & 0x3);
    __bdk_csr_fatal("L2C_ASC_REGIONX_START", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_ASC_REGIONX_START(a) bdk_l2c_asc_regionx_start_t
#define bustype_BDK_L2C_ASC_REGIONX_START(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_ASC_REGIONX_START(a) "L2C_ASC_REGIONX_START"
#define device_bar_BDK_L2C_ASC_REGIONX_START(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_ASC_REGIONX_START(a) (a)
#define arguments_BDK_L2C_ASC_REGIONX_START(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_cbc#_bist_status
 *
 * L2C CBC BIST Status Registers
 */
union bdk_l2c_cbcx_bist_status
{
    uint64_t u;
    struct bdk_l2c_cbcx_bist_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_37_63        : 27;
        uint64_t mibfl                 : 5;  /**< [ 36: 32](RO/H) BIST failure status for various MIB memories. ({XMD, IPM, IRM, MXD, MXN}) */
        uint64_t rsdfl                 : 32; /**< [ 31:  0](RO/H) BIST failure status for RSDQW0-31. */
#else /* Word 0 - Little Endian */
        uint64_t rsdfl                 : 32; /**< [ 31:  0](RO/H) BIST failure status for RSDQW0-31. */
        uint64_t mibfl                 : 5;  /**< [ 36: 32](RO/H) BIST failure status for various MIB memories. ({XMD, IPM, IRM, MXD, MXN}) */
        uint64_t reserved_37_63        : 27;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_cbcx_bist_status_s cn; */
};
typedef union bdk_l2c_cbcx_bist_status bdk_l2c_cbcx_bist_status_t;

static inline uint64_t BDK_L2C_CBCX_BIST_STATUS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_CBCX_BIST_STATUS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0580a0000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0580a0000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e0580a0000ll + 0x1000000ll * ((a) & 0x3);
    __bdk_csr_fatal("L2C_CBCX_BIST_STATUS", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_CBCX_BIST_STATUS(a) bdk_l2c_cbcx_bist_status_t
#define bustype_BDK_L2C_CBCX_BIST_STATUS(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_CBCX_BIST_STATUS(a) "L2C_CBCX_BIST_STATUS"
#define device_bar_BDK_L2C_CBCX_BIST_STATUS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_CBCX_BIST_STATUS(a) (a)
#define arguments_BDK_L2C_CBCX_BIST_STATUS(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_cbc#_dll
 *
 * L2C CBC DLL Observability Register
 * Register for DLL observability.
 */
union bdk_l2c_cbcx_dll
{
    uint64_t u;
    struct bdk_l2c_cbcx_dll_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_60_63        : 4;
        uint64_t max_dll_setting       : 12; /**< [ 59: 48](RO/H) Max reported DLL setting. */
        uint64_t min_dll_setting       : 12; /**< [ 47: 36](RO/H) Min reported DLL setting. */
        uint64_t pd_pos_rclk_refclk    : 1;  /**< [ 35: 35](RO/H) Phase detector output. */
        uint64_t pdl_rclk_refclk       : 1;  /**< [ 34: 34](RO/H) Phase detector output. */
        uint64_t pdr_rclk_refclk       : 1;  /**< [ 33: 33](RO/H) Phase detector output. */
        uint64_t reserved_32           : 1;
        uint64_t dly_elem_enable       : 16; /**< [ 31: 16](RO/H) Delay element enable. */
        uint64_t dll_setting           : 12; /**< [ 15:  4](RO/H) DLL setting. */
        uint64_t reserved_1_3          : 3;
        uint64_t dll_lock              : 1;  /**< [  0:  0](RO/H) DLL locked. */
#else /* Word 0 - Little Endian */
        uint64_t dll_lock              : 1;  /**< [  0:  0](RO/H) DLL locked. */
        uint64_t reserved_1_3          : 3;
        uint64_t dll_setting           : 12; /**< [ 15:  4](RO/H) DLL setting. */
        uint64_t dly_elem_enable       : 16; /**< [ 31: 16](RO/H) Delay element enable. */
        uint64_t reserved_32           : 1;
        uint64_t pdr_rclk_refclk       : 1;  /**< [ 33: 33](RO/H) Phase detector output. */
        uint64_t pdl_rclk_refclk       : 1;  /**< [ 34: 34](RO/H) Phase detector output. */
        uint64_t pd_pos_rclk_refclk    : 1;  /**< [ 35: 35](RO/H) Phase detector output. */
        uint64_t min_dll_setting       : 12; /**< [ 47: 36](RO/H) Min reported DLL setting. */
        uint64_t max_dll_setting       : 12; /**< [ 59: 48](RO/H) Max reported DLL setting. */
        uint64_t reserved_60_63        : 4;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_cbcx_dll_s cn; */
};
typedef union bdk_l2c_cbcx_dll bdk_l2c_cbcx_dll_t;

static inline uint64_t BDK_L2C_CBCX_DLL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_CBCX_DLL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e058040000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e058040000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e058040000ll + 0x1000000ll * ((a) & 0x3);
    __bdk_csr_fatal("L2C_CBCX_DLL", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_CBCX_DLL(a) bdk_l2c_cbcx_dll_t
#define bustype_BDK_L2C_CBCX_DLL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_CBCX_DLL(a) "L2C_CBCX_DLL"
#define device_bar_BDK_L2C_CBCX_DLL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_CBCX_DLL(a) (a)
#define arguments_BDK_L2C_CBCX_DLL(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_cbc#_iocerr
 *
 * L2C CBC Error Information Registers
 * Reserved.
 */
union bdk_l2c_cbcx_iocerr
{
    uint64_t u;
    struct bdk_l2c_cbcx_iocerr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_0_63         : 64;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_63         : 64;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_cbcx_iocerr_s cn; */
};
typedef union bdk_l2c_cbcx_iocerr bdk_l2c_cbcx_iocerr_t;

static inline uint64_t BDK_L2C_CBCX_IOCERR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_CBCX_IOCERR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e058080010ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e058080010ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e058080010ll + 0x1000000ll * ((a) & 0x3);
    __bdk_csr_fatal("L2C_CBCX_IOCERR", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_CBCX_IOCERR(a) bdk_l2c_cbcx_iocerr_t
#define bustype_BDK_L2C_CBCX_IOCERR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_CBCX_IOCERR(a) "L2C_CBCX_IOCERR"
#define device_bar_BDK_L2C_CBCX_IOCERR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_CBCX_IOCERR(a) (a)
#define arguments_BDK_L2C_CBCX_IOCERR(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_cbc#_iodisocierr
 *
 * L2C CBC IODISOCI Error Information Registers
 * This register records error information associated with IORDDISOCI/IOWRDISOCI interrupts.
 * IOWRDISOCI events take priority over previously captured IORDDISOCI events. Of the available
 * I/O transactions, some commands will either set [IORDDISOCI], set [IOWRDISOCI], or set both
 * [IORDDISOCI] and [IOWRDISOCI]. See L2C_CBC()_INT_W1C for information about which I/O
 * transactions
 * may result in IORDDISOCI/IOWRDISOCI interrupts.
 */
union bdk_l2c_cbcx_iodisocierr
{
    uint64_t u;
    struct bdk_l2c_cbcx_iodisocierr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t iorddisoci            : 1;  /**< [ 63: 63](RO/H) Logged information is for a IORDDISOCI error. */
        uint64_t iowrdisoci            : 1;  /**< [ 62: 62](RO/H) Logged information is for a IOWRDISOCI error. */
        uint64_t reserved_59_61        : 3;
        uint64_t cmd                   : 7;  /**< [ 58: 52](RO/H) Encoding of XMC command.
                                                                 Internal:
                                                                 Enumerated by XMC_CMD_E. */
        uint64_t ppvid                 : 6;  /**< [ 51: 46](RO/H) CMB source PPVID. */
        uint64_t node                  : 2;  /**< [ 45: 44](RO/H) Destination node ID. */
        uint64_t did                   : 8;  /**< [ 43: 36](RO/H) Destination device ID. */
        uint64_t addr                  : 36; /**< [ 35:  0](RO/H) I/O address. */
#else /* Word 0 - Little Endian */
        uint64_t addr                  : 36; /**< [ 35:  0](RO/H) I/O address. */
        uint64_t did                   : 8;  /**< [ 43: 36](RO/H) Destination device ID. */
        uint64_t node                  : 2;  /**< [ 45: 44](RO/H) Destination node ID. */
        uint64_t ppvid                 : 6;  /**< [ 51: 46](RO/H) CMB source PPVID. */
        uint64_t cmd                   : 7;  /**< [ 58: 52](RO/H) Encoding of XMC command.
                                                                 Internal:
                                                                 Enumerated by XMC_CMD_E. */
        uint64_t reserved_59_61        : 3;
        uint64_t iowrdisoci            : 1;  /**< [ 62: 62](RO/H) Logged information is for a IOWRDISOCI error. */
        uint64_t iorddisoci            : 1;  /**< [ 63: 63](RO/H) Logged information is for a IORDDISOCI error. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_cbcx_iodisocierr_s cn; */
};
typedef union bdk_l2c_cbcx_iodisocierr bdk_l2c_cbcx_iodisocierr_t;

static inline uint64_t BDK_L2C_CBCX_IODISOCIERR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_CBCX_IODISOCIERR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e058080008ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e058080008ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e058080008ll + 0x1000000ll * ((a) & 0x3);
    __bdk_csr_fatal("L2C_CBCX_IODISOCIERR", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_CBCX_IODISOCIERR(a) bdk_l2c_cbcx_iodisocierr_t
#define bustype_BDK_L2C_CBCX_IODISOCIERR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_CBCX_IODISOCIERR(a) "L2C_CBCX_IODISOCIERR"
#define device_bar_BDK_L2C_CBCX_IODISOCIERR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_CBCX_IODISOCIERR(a) (a)
#define arguments_BDK_L2C_CBCX_IODISOCIERR(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_cbc#_miberr
 *
 * L2C CBC MIB Error Information Registers
 * This register records error information for all CBC MIB errors. An error locks the INDEX and
 * [SYN] fields and set the bit corresponding to the error received. MIBDBE errors take priority
 * and overwrite an earlier logged MIBSBE error. Only one of [MIBSBE]/[MIBDBE] is set at any
 * given
 * time and serves to document which error the INDEX/[SYN] is associated with. The syndrome is
 * recorded for DBE errors, though the utility of the value is not clear.
 */
union bdk_l2c_cbcx_miberr
{
    uint64_t u;
    struct bdk_l2c_cbcx_miberr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t mibdbe                : 1;  /**< [ 63: 63](RO/H) INDEX/SYN corresponds to a double-bit MIB ECC error. */
        uint64_t mibsbe                : 1;  /**< [ 62: 62](RO/H) INDEX/SYN corresponds to a single-bit MIB ECC error. */
        uint64_t reserved_40_61        : 22;
        uint64_t syn                   : 8;  /**< [ 39: 32](RO/H) Error syndrome. */
        uint64_t reserved_3_31         : 29;
        uint64_t memid                 : 2;  /**< [  2:  1](RO/H) Indicates the memory that had the error.
                                                                 0x0 = Error from MXB_VC_MRN, MXB_VC_MFN, MXB_VC_MPN VCs.
                                                                 0x1 = Error from MXB_VC_MRD, MXB_VC_MPD VCs.
                                                                 0x2 = Error from MXB_VC_IRM VC.
                                                                 0x3 = Error from MXB_VC_IPM VC. */
        uint64_t mibnum                : 1;  /**< [  0:  0](RO/H) Indicates the MIB bus that had the error. */
#else /* Word 0 - Little Endian */
        uint64_t mibnum                : 1;  /**< [  0:  0](RO/H) Indicates the MIB bus that had the error. */
        uint64_t memid                 : 2;  /**< [  2:  1](RO/H) Indicates the memory that had the error.
                                                                 0x0 = Error from MXB_VC_MRN, MXB_VC_MFN, MXB_VC_MPN VCs.
                                                                 0x1 = Error from MXB_VC_MRD, MXB_VC_MPD VCs.
                                                                 0x2 = Error from MXB_VC_IRM VC.
                                                                 0x3 = Error from MXB_VC_IPM VC. */
        uint64_t reserved_3_31         : 29;
        uint64_t syn                   : 8;  /**< [ 39: 32](RO/H) Error syndrome. */
        uint64_t reserved_40_61        : 22;
        uint64_t mibsbe                : 1;  /**< [ 62: 62](RO/H) INDEX/SYN corresponds to a single-bit MIB ECC error. */
        uint64_t mibdbe                : 1;  /**< [ 63: 63](RO/H) INDEX/SYN corresponds to a double-bit MIB ECC error. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_cbcx_miberr_s cn; */
};
typedef union bdk_l2c_cbcx_miberr bdk_l2c_cbcx_miberr_t;

static inline uint64_t BDK_L2C_CBCX_MIBERR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_CBCX_MIBERR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e058080020ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e058080020ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e058080020ll + 0x1000000ll * ((a) & 0x3);
    __bdk_csr_fatal("L2C_CBCX_MIBERR", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_CBCX_MIBERR(a) bdk_l2c_cbcx_miberr_t
#define bustype_BDK_L2C_CBCX_MIBERR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_CBCX_MIBERR(a) "L2C_CBCX_MIBERR"
#define device_bar_BDK_L2C_CBCX_MIBERR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_CBCX_MIBERR(a) (a)
#define arguments_BDK_L2C_CBCX_MIBERR(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_cbc#_rsderr
 *
 * L2C CBC RSD Error Information Registers
 * This register records error information for all CBC RSD errors.
 * An error locks the INDEX and [SYN] fields and set the bit corresponding to the error received.
 * RSDDBE errors take priority and overwrite an earlier logged RSDSBE error. Only one of
 * [RSDSBE]/[RSDDBE] is set at any given time and serves to document which error the INDEX/[SYN]
 * is
 * associated with.
 * The syndrome is recorded for DBE errors, though the utility of the value is not clear.
 */
union bdk_l2c_cbcx_rsderr
{
    uint64_t u;
    struct bdk_l2c_cbcx_rsderr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t rsddbe                : 1;  /**< [ 63: 63](RO/H) INDEX/SYN corresponds to a double-bit RSD ECC error. */
        uint64_t rsdsbe                : 1;  /**< [ 62: 62](RO/H) INDEX/SYN corresponds to a single-bit RSD ECC error. */
        uint64_t reserved_40_61        : 22;
        uint64_t syn                   : 8;  /**< [ 39: 32](RO/H) Error syndrome. */
        uint64_t reserved_9_31         : 23;
        uint64_t tadnum                : 3;  /**< [  8:  6](RO/H) Indicates the TAD FIFO containing the error. */
        uint64_t qwnum                 : 2;  /**< [  5:  4](RO/H) Indicates the QW containing the error. */
        uint64_t rsdnum                : 4;  /**< [  3:  0](RO/H) Indicates the RSD that had the error. */
#else /* Word 0 - Little Endian */
        uint64_t rsdnum                : 4;  /**< [  3:  0](RO/H) Indicates the RSD that had the error. */
        uint64_t qwnum                 : 2;  /**< [  5:  4](RO/H) Indicates the QW containing the error. */
        uint64_t tadnum                : 3;  /**< [  8:  6](RO/H) Indicates the TAD FIFO containing the error. */
        uint64_t reserved_9_31         : 23;
        uint64_t syn                   : 8;  /**< [ 39: 32](RO/H) Error syndrome. */
        uint64_t reserved_40_61        : 22;
        uint64_t rsdsbe                : 1;  /**< [ 62: 62](RO/H) INDEX/SYN corresponds to a single-bit RSD ECC error. */
        uint64_t rsddbe                : 1;  /**< [ 63: 63](RO/H) INDEX/SYN corresponds to a double-bit RSD ECC error. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_cbcx_rsderr_s cn; */
};
typedef union bdk_l2c_cbcx_rsderr bdk_l2c_cbcx_rsderr_t;

static inline uint64_t BDK_L2C_CBCX_RSDERR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_CBCX_RSDERR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e058080018ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e058080018ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e058080018ll + 0x1000000ll * ((a) & 0x3);
    __bdk_csr_fatal("L2C_CBCX_RSDERR", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_CBCX_RSDERR(a) bdk_l2c_cbcx_rsderr_t
#define bustype_BDK_L2C_CBCX_RSDERR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_CBCX_RSDERR(a) "L2C_CBCX_RSDERR"
#define device_bar_BDK_L2C_CBCX_RSDERR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_CBCX_RSDERR(a) (a)
#define arguments_BDK_L2C_CBCX_RSDERR(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_ctl
 *
 * L2C Control Register
 */
union bdk_l2c_ctl
{
    uint64_t u;
    struct bdk_l2c_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t ocla_qos              : 3;  /**< [ 31: 29](R/W) QOS level for the transactions from OCLA to L2C. */
        uint64_t reserved_28           : 1;
        uint64_t disstgl2i             : 1;  /**< [ 27: 27](R/W) Disable STGL2Is from changing the tags. */
        uint64_t reserved_25_26        : 2;
        uint64_t discclk               : 1;  /**< [ 24: 24](R/W) Disable conditional clocking in L2C PNR blocks. */
        uint64_t reserved_16_23        : 8;
        uint64_t rsp_arb_mode          : 1;  /**< [ 15: 15](R/W) Arbitration mode for RSC/RSD bus. 0 = round-robin; 1 = static priority.
                                                                 1. IOR data.
                                                                 2. STIN/FILLs.
                                                                 3. STDN/SCDN/SCFL. */
        uint64_t xmc_arb_mode          : 1;  /**< [ 14: 14](R/W) Arbitration mode for ADD bus QOS queues. 0 = fully determined through QOS, 1 = QOS0
                                                                 highest priority; QOS 1-7 use normal mode. */
        uint64_t rdf_cnt               : 8;  /**< [ 13:  6](R/W) Defines the sample point of the LMC response data in the DDR-clock/core-clock crossing.
                                                                 For optimal performance set to
                                                                 10 * (DDR-clock period/core-clock period) - 1.
                                                                 To disable set to 0. All other values are reserved. */
        uint64_t disdwb                : 1;  /**< [  5:  5](R/W) Suppresses DWB and INVL2 commands, effectively turning them into NOPs.
                                                                 Internal:
                                                                 The real behavior is DWB and INVL2 commands are forced to look like STGL2I commands with
                                                                 DISSTGL2I set. */
        uint64_t disgsyncto            : 1;  /**< [  4:  4](R/W) Disable global sync timeout. */
        uint64_t disldwb               : 1;  /**< [  3:  3](R/W) Suppresses the DWB functionality of any received LDWB, effectively turning them into LDTs. */
        uint64_t dissblkdty            : 1;  /**< [  2:  2](R/W) Disable bandwidth optimization between L2 and LMC and MOB which only transfers modified
                                                                 sub-blocks when possible. In an CCPI system all nodes must use the same setting of
                                                                 DISSBLKDTY or operation is undefined. */
        uint64_t disecc                : 1;  /**< [  1:  1](R/W) Tag and data ECC disable. */
        uint64_t disidxalias           : 1;  /**< [  0:  0](R/W) Index alias disable. */
#else /* Word 0 - Little Endian */
        uint64_t disidxalias           : 1;  /**< [  0:  0](R/W) Index alias disable. */
        uint64_t disecc                : 1;  /**< [  1:  1](R/W) Tag and data ECC disable. */
        uint64_t dissblkdty            : 1;  /**< [  2:  2](R/W) Disable bandwidth optimization between L2 and LMC and MOB which only transfers modified
                                                                 sub-blocks when possible. In an CCPI system all nodes must use the same setting of
                                                                 DISSBLKDTY or operation is undefined. */
        uint64_t disldwb               : 1;  /**< [  3:  3](R/W) Suppresses the DWB functionality of any received LDWB, effectively turning them into LDTs. */
        uint64_t disgsyncto            : 1;  /**< [  4:  4](R/W) Disable global sync timeout. */
        uint64_t disdwb                : 1;  /**< [  5:  5](R/W) Suppresses DWB and INVL2 commands, effectively turning them into NOPs.
                                                                 Internal:
                                                                 The real behavior is DWB and INVL2 commands are forced to look like STGL2I commands with
                                                                 DISSTGL2I set. */
        uint64_t rdf_cnt               : 8;  /**< [ 13:  6](R/W) Defines the sample point of the LMC response data in the DDR-clock/core-clock crossing.
                                                                 For optimal performance set to
                                                                 10 * (DDR-clock period/core-clock period) - 1.
                                                                 To disable set to 0. All other values are reserved. */
        uint64_t xmc_arb_mode          : 1;  /**< [ 14: 14](R/W) Arbitration mode for ADD bus QOS queues. 0 = fully determined through QOS, 1 = QOS0
                                                                 highest priority; QOS 1-7 use normal mode. */
        uint64_t rsp_arb_mode          : 1;  /**< [ 15: 15](R/W) Arbitration mode for RSC/RSD bus. 0 = round-robin; 1 = static priority.
                                                                 1. IOR data.
                                                                 2. STIN/FILLs.
                                                                 3. STDN/SCDN/SCFL. */
        uint64_t reserved_16_23        : 8;
        uint64_t discclk               : 1;  /**< [ 24: 24](R/W) Disable conditional clocking in L2C PNR blocks. */
        uint64_t reserved_25_26        : 2;
        uint64_t disstgl2i             : 1;  /**< [ 27: 27](R/W) Disable STGL2Is from changing the tags. */
        uint64_t reserved_28           : 1;
        uint64_t ocla_qos              : 3;  /**< [ 31: 29](R/W) QOS level for the transactions from OCLA to L2C. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    struct bdk_l2c_ctl_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t ocla_qos              : 3;  /**< [ 31: 29](R/W) QOS level for the transactions from OCLA to L2C. */
        uint64_t reserved_28           : 1;
        uint64_t disstgl2i             : 1;  /**< [ 27: 27](R/W) Disable STGL2Is from changing the tags. */
        uint64_t reserved_25_26        : 2;
        uint64_t discclk               : 1;  /**< [ 24: 24](R/W) Disable conditional clocking in L2C PNR blocks. */
        uint64_t reserved_16_23        : 8;
        uint64_t rsp_arb_mode          : 1;  /**< [ 15: 15](R/W) Arbitration mode for RSC/RSD bus. 0 = round-robin; 1 = static priority.
                                                                 1. IOR data.
                                                                 2. STIN/FILLs.
                                                                 3. STDN/SCDN/SCFL. */
        uint64_t xmc_arb_mode          : 1;  /**< [ 14: 14](R/W) Arbitration mode for ADD bus QOS queues. 0 = fully determined through QOS, 1 = QOS0
                                                                 highest priority; QOS 1-7 use normal mode. */
        uint64_t rdf_cnt               : 8;  /**< [ 13:  6](R/W) Defines the sample point of the LMC response data in the DDR-clock/core-clock crossing.
                                                                 For optimal performance set to
                                                                 10 * (DDR-clock period/core-clock period) - 1.
                                                                 To disable set to 0. All other values are reserved. */
        uint64_t reserved_5            : 1;
        uint64_t reserved_4            : 1;
        uint64_t disldwb               : 1;  /**< [  3:  3](R/W) Suppresses the DWB functionality of any received LDWB, effectively turning them into LDTs. */
        uint64_t dissblkdty            : 1;  /**< [  2:  2](R/W) Disable bandwidth optimization between L2 and LMC and MOB which only transfers modified
                                                                 sub-blocks when possible. In an CCPI system all nodes must use the same setting of
                                                                 DISSBLKDTY or operation is undefined. */
        uint64_t disecc                : 1;  /**< [  1:  1](R/W) Tag and data ECC disable. */
        uint64_t disidxalias           : 1;  /**< [  0:  0](R/W) Index alias disable. */
#else /* Word 0 - Little Endian */
        uint64_t disidxalias           : 1;  /**< [  0:  0](R/W) Index alias disable. */
        uint64_t disecc                : 1;  /**< [  1:  1](R/W) Tag and data ECC disable. */
        uint64_t dissblkdty            : 1;  /**< [  2:  2](R/W) Disable bandwidth optimization between L2 and LMC and MOB which only transfers modified
                                                                 sub-blocks when possible. In an CCPI system all nodes must use the same setting of
                                                                 DISSBLKDTY or operation is undefined. */
        uint64_t disldwb               : 1;  /**< [  3:  3](R/W) Suppresses the DWB functionality of any received LDWB, effectively turning them into LDTs. */
        uint64_t reserved_4            : 1;
        uint64_t reserved_5            : 1;
        uint64_t rdf_cnt               : 8;  /**< [ 13:  6](R/W) Defines the sample point of the LMC response data in the DDR-clock/core-clock crossing.
                                                                 For optimal performance set to
                                                                 10 * (DDR-clock period/core-clock period) - 1.
                                                                 To disable set to 0. All other values are reserved. */
        uint64_t xmc_arb_mode          : 1;  /**< [ 14: 14](R/W) Arbitration mode for ADD bus QOS queues. 0 = fully determined through QOS, 1 = QOS0
                                                                 highest priority; QOS 1-7 use normal mode. */
        uint64_t rsp_arb_mode          : 1;  /**< [ 15: 15](R/W) Arbitration mode for RSC/RSD bus. 0 = round-robin; 1 = static priority.
                                                                 1. IOR data.
                                                                 2. STIN/FILLs.
                                                                 3. STDN/SCDN/SCFL. */
        uint64_t reserved_16_23        : 8;
        uint64_t discclk               : 1;  /**< [ 24: 24](R/W) Disable conditional clocking in L2C PNR blocks. */
        uint64_t reserved_25_26        : 2;
        uint64_t disstgl2i             : 1;  /**< [ 27: 27](R/W) Disable STGL2Is from changing the tags. */
        uint64_t reserved_28           : 1;
        uint64_t ocla_qos              : 3;  /**< [ 31: 29](R/W) QOS level for the transactions from OCLA to L2C. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } cn88xxp1;
    /* struct bdk_l2c_ctl_s cn81xx; */
    /* struct bdk_l2c_ctl_s cn83xx; */
    struct bdk_l2c_ctl_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t ocla_qos              : 3;  /**< [ 31: 29](R/W) QOS level for the transactions from OCLA to L2C. */
        uint64_t reserved_28           : 1;
        uint64_t disstgl2i             : 1;  /**< [ 27: 27](R/W) Disable STGL2Is from changing the tags. */
        uint64_t reserved_25_26        : 2;
        uint64_t discclk               : 1;  /**< [ 24: 24](R/W) Disable conditional clocking in L2C PNR blocks. */
        uint64_t reserved_16_23        : 8;
        uint64_t rsp_arb_mode          : 1;  /**< [ 15: 15](R/W) Arbitration mode for RSC/RSD bus. 0 = round-robin; 1 = static priority.
                                                                 1. IOR data.
                                                                 2. STIN/FILLs.
                                                                 3. STDN/SCDN/SCFL. */
        uint64_t xmc_arb_mode          : 1;  /**< [ 14: 14](R/W) Arbitration mode for ADD bus QOS queues. 0 = fully determined through QOS, 1 = QOS0
                                                                 highest priority; QOS 1-7 use normal mode. */
        uint64_t rdf_cnt               : 8;  /**< [ 13:  6](R/W) Defines the sample point of the LMC response data in the DDR-clock/core-clock crossing.
                                                                 For optimal performance set to
                                                                 10 * (DDR-clock period/core-clock period) - 1.
                                                                 To disable set to 0. All other values are reserved. */
        uint64_t reserved_5            : 1;
        uint64_t disgsyncto            : 1;  /**< [  4:  4](R/W) Disable global sync timeout. */
        uint64_t disldwb               : 1;  /**< [  3:  3](R/W) Suppresses the DWB functionality of any received LDWB, effectively turning them into LDTs. */
        uint64_t dissblkdty            : 1;  /**< [  2:  2](R/W) Disable bandwidth optimization between L2 and LMC and MOB which only transfers modified
                                                                 sub-blocks when possible. In an CCPI system all nodes must use the same setting of
                                                                 DISSBLKDTY or operation is undefined. */
        uint64_t disecc                : 1;  /**< [  1:  1](R/W) Tag and data ECC disable. */
        uint64_t disidxalias           : 1;  /**< [  0:  0](R/W) Index alias disable. */
#else /* Word 0 - Little Endian */
        uint64_t disidxalias           : 1;  /**< [  0:  0](R/W) Index alias disable. */
        uint64_t disecc                : 1;  /**< [  1:  1](R/W) Tag and data ECC disable. */
        uint64_t dissblkdty            : 1;  /**< [  2:  2](R/W) Disable bandwidth optimization between L2 and LMC and MOB which only transfers modified
                                                                 sub-blocks when possible. In an CCPI system all nodes must use the same setting of
                                                                 DISSBLKDTY or operation is undefined. */
        uint64_t disldwb               : 1;  /**< [  3:  3](R/W) Suppresses the DWB functionality of any received LDWB, effectively turning them into LDTs. */
        uint64_t disgsyncto            : 1;  /**< [  4:  4](R/W) Disable global sync timeout. */
        uint64_t reserved_5            : 1;
        uint64_t rdf_cnt               : 8;  /**< [ 13:  6](R/W) Defines the sample point of the LMC response data in the DDR-clock/core-clock crossing.
                                                                 For optimal performance set to
                                                                 10 * (DDR-clock period/core-clock period) - 1.
                                                                 To disable set to 0. All other values are reserved. */
        uint64_t xmc_arb_mode          : 1;  /**< [ 14: 14](R/W) Arbitration mode for ADD bus QOS queues. 0 = fully determined through QOS, 1 = QOS0
                                                                 highest priority; QOS 1-7 use normal mode. */
        uint64_t rsp_arb_mode          : 1;  /**< [ 15: 15](R/W) Arbitration mode for RSC/RSD bus. 0 = round-robin; 1 = static priority.
                                                                 1. IOR data.
                                                                 2. STIN/FILLs.
                                                                 3. STDN/SCDN/SCFL. */
        uint64_t reserved_16_23        : 8;
        uint64_t discclk               : 1;  /**< [ 24: 24](R/W) Disable conditional clocking in L2C PNR blocks. */
        uint64_t reserved_25_26        : 2;
        uint64_t disstgl2i             : 1;  /**< [ 27: 27](R/W) Disable STGL2Is from changing the tags. */
        uint64_t reserved_28           : 1;
        uint64_t ocla_qos              : 3;  /**< [ 31: 29](R/W) QOS level for the transactions from OCLA to L2C. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_l2c_ctl bdk_l2c_ctl_t;

#define BDK_L2C_CTL BDK_L2C_CTL_FUNC()
static inline uint64_t BDK_L2C_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e080800000ll;
    __bdk_csr_fatal("L2C_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_L2C_CTL bdk_l2c_ctl_t
#define bustype_BDK_L2C_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_CTL "L2C_CTL"
#define device_bar_BDK_L2C_CTL 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_CTL 0
#define arguments_BDK_L2C_CTL -1,-1,-1,-1

/**
 * Register (RSL) l2c_ecc_ctl
 *
 * L2C ECC Control Register
 * Flip ECC bits to generate single-bit or double-bit ECC errors in all instances of a given
 * memory type. Encodings are as follows.
 * 0x0 = No error.
 * 0x1 = Single-bit error on ECC\<0\>.
 * 0x2 = Single-bit error on ECC\<1\>.
 * 0x3 = Double-bit error on ECC\<1:0\>.
 *
 * L2DFLIP allows software to generate L2DSBE, L2DDBE, VBFSBE, and VBFDBE errors for the purposes
 * of testing error handling code. When one (or both) of these bits are set, a PL2 that misses in
 * the L2 will fill with the appropriate error in the first two OWs of the fill. Software can
 * determine which OW pair gets the error by choosing the desired fill order (address\<6:5\>). A
 * PL2 that hits in the L2 will not inject any errors. Therefore sending a WBIL2 prior to the PL2
 * is recommended to make a miss likely. (If multiple processors are involved, software must be
 * sure that no other processor or I/O device can bring the block into the L2).
 *
 * To generate a VBFSBE or VBFDBE, software must first get the cache block into the cache with an
 * error using a PL2 that misses the L2. Then a store partial to a portion of the cache block
 * without the error must change the block to dirty. Then, a subsequent WBL2/WBIL2/victim will
 * trigger the VBFSBE/VBFDBE error.
 */
union bdk_l2c_ecc_ctl
{
    uint64_t u;
    struct bdk_l2c_ecc_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t mibflip               : 2;  /**< [ 11: 10](R/W) Generate an ECC error in the MIB. See note above. */
        uint64_t l2dflip               : 2;  /**< [  9:  8](R/W) Generate an ECC error in the L2D. See note above. */
        uint64_t l2tflip               : 2;  /**< [  7:  6](R/W) Generate an ECC error in the L2T. */
        uint64_t rdfflip               : 2;  /**< [  5:  4](R/W) Generate an ECC error in RDF memory. */
        uint64_t xmdflip               : 2;  /**< [  3:  2](R/W) Generate an ECC error in all corresponding CBC XMD memories. */
        uint64_t reserved_0_1          : 2;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_1          : 2;
        uint64_t xmdflip               : 2;  /**< [  3:  2](R/W) Generate an ECC error in all corresponding CBC XMD memories. */
        uint64_t rdfflip               : 2;  /**< [  5:  4](R/W) Generate an ECC error in RDF memory. */
        uint64_t l2tflip               : 2;  /**< [  7:  6](R/W) Generate an ECC error in the L2T. */
        uint64_t l2dflip               : 2;  /**< [  9:  8](R/W) Generate an ECC error in the L2D. See note above. */
        uint64_t mibflip               : 2;  /**< [ 11: 10](R/W) Generate an ECC error in the MIB. See note above. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_ecc_ctl_s cn; */
};
typedef union bdk_l2c_ecc_ctl bdk_l2c_ecc_ctl_t;

#define BDK_L2C_ECC_CTL BDK_L2C_ECC_CTL_FUNC()
static inline uint64_t BDK_L2C_ECC_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_ECC_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e080800010ll;
    __bdk_csr_fatal("L2C_ECC_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_L2C_ECC_CTL bdk_l2c_ecc_ctl_t
#define bustype_BDK_L2C_ECC_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_ECC_CTL "L2C_ECC_CTL"
#define device_bar_BDK_L2C_ECC_CTL 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_ECC_CTL 0
#define arguments_BDK_L2C_ECC_CTL -1,-1,-1,-1

/**
 * Register (RSL) l2c_mci#_bist_status
 *
 * Level 2 MCI BIST Status (DCLK) Registers
 * If clear BIST is desired, [CLEAR_BIST] must be written to 1 before [START_BIST] is
 * written to 1 using a separate CSR write operation. [CLEAR_BIST] must not be changed
 * after writing [START_BIST] to 1 until the BIST operation completes (indicated by
 * [START_BIST] returning to 0) or operation is undefined.
 */
union bdk_l2c_mcix_bist_status
{
    uint64_t u;
    struct bdk_l2c_mcix_bist_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t start_bist            : 1;  /**< [ 63: 63](R/W/H) When written to 1, starts BIST. Remains 1 until BIST is complete. */
        uint64_t clear_bist            : 1;  /**< [ 62: 62](R/W) When BIST is triggered, run clear BIST. */
        uint64_t reserved_2_61         : 60;
        uint64_t vbffl                 : 2;  /**< [  1:  0](RO/H) BIST failure status for VBF0-1. */
#else /* Word 0 - Little Endian */
        uint64_t vbffl                 : 2;  /**< [  1:  0](RO/H) BIST failure status for VBF0-1. */
        uint64_t reserved_2_61         : 60;
        uint64_t clear_bist            : 1;  /**< [ 62: 62](R/W) When BIST is triggered, run clear BIST. */
        uint64_t start_bist            : 1;  /**< [ 63: 63](R/W/H) When written to 1, starts BIST. Remains 1 until BIST is complete. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_mcix_bist_status_s cn; */
};
typedef union bdk_l2c_mcix_bist_status bdk_l2c_mcix_bist_status_t;

static inline uint64_t BDK_L2C_MCIX_BIST_STATUS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_MCIX_BIST_STATUS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e05c020000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e05c020000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e05c020000ll + 0x1000000ll * ((a) & 0x3);
    __bdk_csr_fatal("L2C_MCIX_BIST_STATUS", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_MCIX_BIST_STATUS(a) bdk_l2c_mcix_bist_status_t
#define bustype_BDK_L2C_MCIX_BIST_STATUS(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_MCIX_BIST_STATUS(a) "L2C_MCIX_BIST_STATUS"
#define device_bar_BDK_L2C_MCIX_BIST_STATUS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_MCIX_BIST_STATUS(a) (a)
#define arguments_BDK_L2C_MCIX_BIST_STATUS(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_mci#_err
 *
 * L2C MCI Error Information Registers
 * This register records error information for all MCI errors.
 * An error locks [VBF4], [INDEX], [SYN0] and [SYN1] and sets the bit corresponding to the error
 * received. VBFDBE errors take priority and will overwrite an earlier logged VBFSBE error. The
 * information from exactly one VBF read is present at any given time and serves to document
 * which error(s) were present in the read with the highest priority error.
 * The syndrome is recorded for DBE errors.
 */
union bdk_l2c_mcix_err
{
    uint64_t u;
    struct bdk_l2c_mcix_err_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vbfdbe1               : 1;  /**< [ 63: 63](RO/H) INDEX/SYN1 corresponds to a double-bit VBF ECC error. */
        uint64_t vbfdbe0               : 1;  /**< [ 62: 62](RO/H) INDEX/SYN0 corresponds to a double-bit VBF ECC error. */
        uint64_t vbfsbe1               : 1;  /**< [ 61: 61](RO/H) INDEX/SYN1 corresponds to a single-bit VBF ECC error. */
        uint64_t vbfsbe0               : 1;  /**< [ 60: 60](RO/H) INDEX/SYN0 corresponds to a single-bit VBF ECC error. */
        uint64_t reserved_48_59        : 12;
        uint64_t syn1                  : 8;  /**< [ 47: 40](RO/H) Error syndrome for QW1 ([127:64]).
                                                                 Records only on single bit errors.

                                                                 Internal:
                                                                 See bug26334. */
        uint64_t syn0                  : 8;  /**< [ 39: 32](RO/H) Error syndrome for QW0 ([63:0]).
                                                                 Records only on single bit errors.

                                                                 Internal:
                                                                 See bug26334. */
        uint64_t reserved_12_31        : 20;
        uint64_t vbf4                  : 1;  /**< [ 11: 11](RO/H) When 1, errors were from VBF (4+a), when 0, from VBF (0+a). */
        uint64_t index                 : 7;  /**< [ 10:  4](RO/H) VBF index which was read and had the error(s). */
        uint64_t reserved_0_3          : 4;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_3          : 4;
        uint64_t index                 : 7;  /**< [ 10:  4](RO/H) VBF index which was read and had the error(s). */
        uint64_t vbf4                  : 1;  /**< [ 11: 11](RO/H) When 1, errors were from VBF (4+a), when 0, from VBF (0+a). */
        uint64_t reserved_12_31        : 20;
        uint64_t syn0                  : 8;  /**< [ 39: 32](RO/H) Error syndrome for QW0 ([63:0]).
                                                                 Records only on single bit errors.

                                                                 Internal:
                                                                 See bug26334. */
        uint64_t syn1                  : 8;  /**< [ 47: 40](RO/H) Error syndrome for QW1 ([127:64]).
                                                                 Records only on single bit errors.

                                                                 Internal:
                                                                 See bug26334. */
        uint64_t reserved_48_59        : 12;
        uint64_t vbfsbe0               : 1;  /**< [ 60: 60](RO/H) INDEX/SYN0 corresponds to a single-bit VBF ECC error. */
        uint64_t vbfsbe1               : 1;  /**< [ 61: 61](RO/H) INDEX/SYN1 corresponds to a single-bit VBF ECC error. */
        uint64_t vbfdbe0               : 1;  /**< [ 62: 62](RO/H) INDEX/SYN0 corresponds to a double-bit VBF ECC error. */
        uint64_t vbfdbe1               : 1;  /**< [ 63: 63](RO/H) INDEX/SYN1 corresponds to a double-bit VBF ECC error. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_mcix_err_s cn; */
};
typedef union bdk_l2c_mcix_err bdk_l2c_mcix_err_t;

static inline uint64_t BDK_L2C_MCIX_ERR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_MCIX_ERR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e05c010000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e05c010000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e05c010000ll + 0x1000000ll * ((a) & 0x3);
    __bdk_csr_fatal("L2C_MCIX_ERR", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_MCIX_ERR(a) bdk_l2c_mcix_err_t
#define bustype_BDK_L2C_MCIX_ERR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_MCIX_ERR(a) "L2C_MCIX_ERR"
#define device_bar_BDK_L2C_MCIX_ERR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_MCIX_ERR(a) (a)
#define arguments_BDK_L2C_MCIX_ERR(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_oci_ctl
 *
 * L2C CCPI Control Register
 */
union bdk_l2c_oci_ctl
{
    uint64_t u;
    struct bdk_l2c_oci_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_31_63        : 33;
        uint64_t ncpend                : 1;  /**< [ 30: 30](RO/H) An indication that a node change is pending.  Hardware sets this bit when
                                                                 OCX_COM_NODE[ID] is changed and clears the bit when the node change has taken
                                                                 effect. */
        uint64_t lock_local_cas        : 1;  /**< [ 29: 29](RO) Reserved. */
        uint64_t lock_local_stc        : 1;  /**< [ 28: 28](RO) Reserved. */
        uint64_t lock_local_pp         : 1;  /**< [ 27: 27](RO) Reserved. */
        uint64_t lngtolen              : 5;  /**< [ 26: 22](R/W) Reserved.
                                                                 Internal:
                                                                 This only controls the GSYNC timeout in the L2C_CBCs in non-OCI chips. */
        uint64_t shtolen               : 5;  /**< [ 21: 17](RO) Reserved. */
        uint64_t shtoioen              : 1;  /**< [ 16: 16](RO) Reserved. */
        uint64_t shtoen                : 3;  /**< [ 15: 13](RO) Reserved. */
        uint64_t shto                  : 1;  /**< [ 12: 12](RO) Reserved. */
        uint64_t inv_mode              : 2;  /**< [ 11: 10](RO) Reserved. */
        uint64_t cas_fdx               : 1;  /**< [  9:  9](RO) Reserved. */
        uint64_t rldd_psha             : 1;  /**< [  8:  8](RO) Reserved. */
        uint64_t lock_local_iob        : 1;  /**< [  7:  7](RO) Reserved. */
        uint64_t iofrcl                : 1;  /**< [  6:  6](RO) Reserved. */
        uint64_t reserved_4_5          : 2;
        uint64_t enaoci                : 4;  /**< [  3:  0](RO) CCPI is not present. Any attempt to enable it will be ignored. */
#else /* Word 0 - Little Endian */
        uint64_t enaoci                : 4;  /**< [  3:  0](RO) CCPI is not present. Any attempt to enable it will be ignored. */
        uint64_t reserved_4_5          : 2;
        uint64_t iofrcl                : 1;  /**< [  6:  6](RO) Reserved. */
        uint64_t lock_local_iob        : 1;  /**< [  7:  7](RO) Reserved. */
        uint64_t rldd_psha             : 1;  /**< [  8:  8](RO) Reserved. */
        uint64_t cas_fdx               : 1;  /**< [  9:  9](RO) Reserved. */
        uint64_t inv_mode              : 2;  /**< [ 11: 10](RO) Reserved. */
        uint64_t shto                  : 1;  /**< [ 12: 12](RO) Reserved. */
        uint64_t shtoen                : 3;  /**< [ 15: 13](RO) Reserved. */
        uint64_t shtoioen              : 1;  /**< [ 16: 16](RO) Reserved. */
        uint64_t shtolen               : 5;  /**< [ 21: 17](RO) Reserved. */
        uint64_t lngtolen              : 5;  /**< [ 26: 22](R/W) Reserved.
                                                                 Internal:
                                                                 This only controls the GSYNC timeout in the L2C_CBCs in non-OCI chips. */
        uint64_t lock_local_pp         : 1;  /**< [ 27: 27](RO) Reserved. */
        uint64_t lock_local_stc        : 1;  /**< [ 28: 28](RO) Reserved. */
        uint64_t lock_local_cas        : 1;  /**< [ 29: 29](RO) Reserved. */
        uint64_t ncpend                : 1;  /**< [ 30: 30](RO/H) An indication that a node change is pending.  Hardware sets this bit when
                                                                 OCX_COM_NODE[ID] is changed and clears the bit when the node change has taken
                                                                 effect. */
        uint64_t reserved_31_63        : 33;
#endif /* Word 0 - End */
    } s;
    struct bdk_l2c_oci_ctl_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_31_63        : 33;
        uint64_t reserved_30           : 1;
        uint64_t lock_local_cas        : 1;  /**< [ 29: 29](R/W) When set, L2 CAS operations to remote addresses which miss at the requester will be
                                                                 performed locally (if possible) on the requesting node. Default operation will instead
                                                                 send the CAS request to be performed on the home node. For STC ops [LOCK_LOCAL_STC]. */
        uint64_t lock_local_stc        : 1;  /**< [ 28: 28](R/W) When set, L2 STC operations to remote addresses which miss at the requester will be
                                                                 performed locally (if possible) on the requesting node. Default operation will instead
                                                                 send the STC request to be performed on the home node. For CAS ops [LOCK_LOCAL_CAS]. */
        uint64_t lock_local_pp         : 1;  /**< [ 27: 27](R/W) When clear, L2 atomic operations (excluding CAS/STC) core initiated requests to remote
                                                                 addresses which miss at the requester will send the atomic request to be performed on the
                                                                 home node. Default operation will instead  be performed locally on the requesting node.
                                                                 For request initiated by IOB & for STC & CAS ops, see
                                                                 [LOCK_LOCAL_IOB]/[LOCK_LOCAL_STC]/[LOCK_LOCAL_CAS]. */
        uint64_t lngtolen              : 5;  /**< [ 26: 22](R/W) Selects the bit in the counter for the long timeout value (timeout used when [SHTO] is
                                                                 clear). Values supported are between 11 and 29 (for a timeout values between 2^11 and
                                                                 2^29). Actual timeout is between 1x and 2x this interval. For example if [LNGTOLEN] = 28
                                                                 (the reset value), the timeout is between 256M and 512M core clocks. Note: a value of 0
                                                                 disables this timer. */
        uint64_t shtolen               : 5;  /**< [ 21: 17](R/W) Selects the bit in the counter for the short timeout value (timeout used when [SHTO] is
                                                                 set). Values supported are between 9 and 29 (for a timeout values between 2^9 and 2^29).
                                                                 Actual timeout is between 1x and 2x this interval. For example if [SHTOLEN] = 14 (the
                                                                 reset
                                                                 value), the timeout is between 16K and 32K core clocks. Note: a value of 0 disables this
                                                                 timer. */
        uint64_t shtoioen              : 1;  /**< [ 16: 16](R/W) When set, any core issues any of an IO load, atomic, acking store, acking IOBADDR, or
                                                                 acking LMTST to a node that doesn't exist (existence defined by the ENAOCI bits), then the
                                                                 hardware sets [SHTO]. */
        uint64_t shtoen                : 3;  /**< [ 15: 13](R/W) When set, if the corresponding CCPI link is down, the hardware sets [SHTO].
                                                                 See OCX_COM_LINK(0..2)_CTL for a description of what events can contribute to the
                                                                 link_down condition. */
        uint64_t shto                  : 1;  /**< [ 12: 12](R/W/H) Use short timeout intervals. When set, core uses SDIDTTO for both DID and commit counter
                                                                 timeouts, rather than DIDTTO/DIDTTO2. Similarly, L2C will use short instead of long
                                                                 timeout. */
        uint64_t inv_mode              : 2;  /**< [ 11: 10](R/W) Describes how aggressive to be when waiting for local invalidates before sending CCPI
                                                                 responses which act like commits at the remote.
                                                                 0x0 = Conservative mode, waits until all local invalidates have been sent by their
                                                                 respective CBCs to the cores.
                                                                 0x1 = Moderate mode, waits until all local invalidates have been sent to their respective
                                                                 CBCs, but not necessarily actually sent to the cores themselves.
                                                                 0x2 = Aggressive mode, does not wait for local invalidates to begin their processing. */
        uint64_t cas_fdx               : 1;  /**< [  9:  9](R/W) When set, L2 STC/CAS operations performed at the home will immediately bring the block
                                                                 exclusive into the home. Default operation is to first request the block shared and only
                                                                 invalidate the remote if the compare succeeds. */
        uint64_t rldd_psha             : 1;  /**< [  8:  8](R/W) When set, RLDD is assumed to return a shared response (PSHA). Default operation assumes an
                                                                 exclusive response (PEMD). Note that an incorrect assumption only causes an extra tag
                                                                 write to be done upon receiving the response. */
        uint64_t lock_local_iob        : 1;  /**< [  7:  7](R/W) When set, L2 atomic operations (excluding CAS/STC) initiated by IOB to remote addresses
                                                                 which miss at the requester are performed locally on the requesting node. When clear the
                                                                 operation instead sends the atomic request to be performed on the home node. For request
                                                                 initiated by core for STC and CAS ops; see
                                                                 [LOCK_LOCAL_PP]/[LOCK_LOCAL_STC]/[LOCK_LOCAL_CAS].
                                                                 Default is set to 1 (local locks). */
        uint64_t iofrcl                : 1;  /**< [  6:  6](R/W) When set, L2C services all I/O read and write operations on the local node, regardless of
                                                                 the value of the node ID bits in the physical address. During normal operation this bit is
                                                                 expected to be 0. Will only transition from 1 to 0, never from 0 to 1. */
        uint64_t reserved_4_5          : 2;
        uint64_t enaoci                : 4;  /**< [  3:  0](R/W) Enable CCPI processing (one bit per node_id). When set, perform CCPI
                                                                 processing. When clear, CCPI memory writes are blocked and CCPI memory reads
                                                                 return unpredictable data. When clear,
                                                                 CCPI I/O requests and MOC references are processed and sent to OCX where they are
                                                                 ultimately discarded. RDDISOCI/WRDISOCI/IORDDISOCI/IOWRDISOCI interrupts occur if and only
                                                                 if the corresponding ENAOCI\<node\> bit is clear. References to the local node (configured
                                                                 via OCX_COM_NODE[ID]) ignore the value of ENAOCI\<node\> because no CCPI processing is
                                                                 required. Similarly, all I/O references ignore the value of ENAOCI when
                                                                 L2C_OCI_CTL[IOFRCL] is set. */
#else /* Word 0 - Little Endian */
        uint64_t enaoci                : 4;  /**< [  3:  0](R/W) Enable CCPI processing (one bit per node_id). When set, perform CCPI
                                                                 processing. When clear, CCPI memory writes are blocked and CCPI memory reads
                                                                 return unpredictable data. When clear,
                                                                 CCPI I/O requests and MOC references are processed and sent to OCX where they are
                                                                 ultimately discarded. RDDISOCI/WRDISOCI/IORDDISOCI/IOWRDISOCI interrupts occur if and only
                                                                 if the corresponding ENAOCI\<node\> bit is clear. References to the local node (configured
                                                                 via OCX_COM_NODE[ID]) ignore the value of ENAOCI\<node\> because no CCPI processing is
                                                                 required. Similarly, all I/O references ignore the value of ENAOCI when
                                                                 L2C_OCI_CTL[IOFRCL] is set. */
        uint64_t reserved_4_5          : 2;
        uint64_t iofrcl                : 1;  /**< [  6:  6](R/W) When set, L2C services all I/O read and write operations on the local node, regardless of
                                                                 the value of the node ID bits in the physical address. During normal operation this bit is
                                                                 expected to be 0. Will only transition from 1 to 0, never from 0 to 1. */
        uint64_t lock_local_iob        : 1;  /**< [  7:  7](R/W) When set, L2 atomic operations (excluding CAS/STC) initiated by IOB to remote addresses
                                                                 which miss at the requester are performed locally on the requesting node. When clear the
                                                                 operation instead sends the atomic request to be performed on the home node. For request
                                                                 initiated by core for STC and CAS ops; see
                                                                 [LOCK_LOCAL_PP]/[LOCK_LOCAL_STC]/[LOCK_LOCAL_CAS].
                                                                 Default is set to 1 (local locks). */
        uint64_t rldd_psha             : 1;  /**< [  8:  8](R/W) When set, RLDD is assumed to return a shared response (PSHA). Default operation assumes an
                                                                 exclusive response (PEMD). Note that an incorrect assumption only causes an extra tag
                                                                 write to be done upon receiving the response. */
        uint64_t cas_fdx               : 1;  /**< [  9:  9](R/W) When set, L2 STC/CAS operations performed at the home will immediately bring the block
                                                                 exclusive into the home. Default operation is to first request the block shared and only
                                                                 invalidate the remote if the compare succeeds. */
        uint64_t inv_mode              : 2;  /**< [ 11: 10](R/W) Describes how aggressive to be when waiting for local invalidates before sending CCPI
                                                                 responses which act like commits at the remote.
                                                                 0x0 = Conservative mode, waits until all local invalidates have been sent by their
                                                                 respective CBCs to the cores.
                                                                 0x1 = Moderate mode, waits until all local invalidates have been sent to their respective
                                                                 CBCs, but not necessarily actually sent to the cores themselves.
                                                                 0x2 = Aggressive mode, does not wait for local invalidates to begin their processing. */
        uint64_t shto                  : 1;  /**< [ 12: 12](R/W/H) Use short timeout intervals. When set, core uses SDIDTTO for both DID and commit counter
                                                                 timeouts, rather than DIDTTO/DIDTTO2. Similarly, L2C will use short instead of long
                                                                 timeout. */
        uint64_t shtoen                : 3;  /**< [ 15: 13](R/W) When set, if the corresponding CCPI link is down, the hardware sets [SHTO].
                                                                 See OCX_COM_LINK(0..2)_CTL for a description of what events can contribute to the
                                                                 link_down condition. */
        uint64_t shtoioen              : 1;  /**< [ 16: 16](R/W) When set, any core issues any of an IO load, atomic, acking store, acking IOBADDR, or
                                                                 acking LMTST to a node that doesn't exist (existence defined by the ENAOCI bits), then the
                                                                 hardware sets [SHTO]. */
        uint64_t shtolen               : 5;  /**< [ 21: 17](R/W) Selects the bit in the counter for the short timeout value (timeout used when [SHTO] is
                                                                 set). Values supported are between 9 and 29 (for a timeout values between 2^9 and 2^29).
                                                                 Actual timeout is between 1x and 2x this interval. For example if [SHTOLEN] = 14 (the
                                                                 reset
                                                                 value), the timeout is between 16K and 32K core clocks. Note: a value of 0 disables this
                                                                 timer. */
        uint64_t lngtolen              : 5;  /**< [ 26: 22](R/W) Selects the bit in the counter for the long timeout value (timeout used when [SHTO] is
                                                                 clear). Values supported are between 11 and 29 (for a timeout values between 2^11 and
                                                                 2^29). Actual timeout is between 1x and 2x this interval. For example if [LNGTOLEN] = 28
                                                                 (the reset value), the timeout is between 256M and 512M core clocks. Note: a value of 0
                                                                 disables this timer. */
        uint64_t lock_local_pp         : 1;  /**< [ 27: 27](R/W) When clear, L2 atomic operations (excluding CAS/STC) core initiated requests to remote
                                                                 addresses which miss at the requester will send the atomic request to be performed on the
                                                                 home node. Default operation will instead  be performed locally on the requesting node.
                                                                 For request initiated by IOB & for STC & CAS ops, see
                                                                 [LOCK_LOCAL_IOB]/[LOCK_LOCAL_STC]/[LOCK_LOCAL_CAS]. */
        uint64_t lock_local_stc        : 1;  /**< [ 28: 28](R/W) When set, L2 STC operations to remote addresses which miss at the requester will be
                                                                 performed locally (if possible) on the requesting node. Default operation will instead
                                                                 send the STC request to be performed on the home node. For CAS ops [LOCK_LOCAL_CAS]. */
        uint64_t lock_local_cas        : 1;  /**< [ 29: 29](R/W) When set, L2 CAS operations to remote addresses which miss at the requester will be
                                                                 performed locally (if possible) on the requesting node. Default operation will instead
                                                                 send the CAS request to be performed on the home node. For STC ops [LOCK_LOCAL_STC]. */
        uint64_t reserved_30           : 1;
        uint64_t reserved_31_63        : 33;
#endif /* Word 0 - End */
    } cn88xxp1;
    /* struct bdk_l2c_oci_ctl_s cn81xx; */
    /* struct bdk_l2c_oci_ctl_s cn83xx; */
    struct bdk_l2c_oci_ctl_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_31_63        : 33;
        uint64_t ncpend                : 1;  /**< [ 30: 30](RO/H) An indication that a node change is pending.  Hardware sets this bit when
                                                                 OCX_COM_NODE[ID] is changed and clears the bit when the node change has taken
                                                                 effect. */
        uint64_t lock_local_cas        : 1;  /**< [ 29: 29](R/W) When set, L2 CAS operations to remote addresses which miss at the requester will be
                                                                 performed locally (if possible) on the requesting node. Default operation will instead
                                                                 send the CAS request to be performed on the home node. For STC ops [LOCK_LOCAL_STC]. */
        uint64_t lock_local_stc        : 1;  /**< [ 28: 28](R/W) When set, L2 STC operations to remote addresses which miss at the requester will be
                                                                 performed locally (if possible) on the requesting node. Default operation will instead
                                                                 send the STC request to be performed on the home node. For CAS ops [LOCK_LOCAL_CAS]. */
        uint64_t lock_local_pp         : 1;  /**< [ 27: 27](R/W) When clear, L2 atomic operations (excluding CAS/STC) core initiated requests to remote
                                                                 addresses which miss at the requester will send the atomic request to be performed on the
                                                                 home node. Default operation will instead  be performed locally on the requesting node.
                                                                 For request initiated by IOB & for STC & CAS ops, see
                                                                 [LOCK_LOCAL_IOB]/[LOCK_LOCAL_STC]/[LOCK_LOCAL_CAS]. */
        uint64_t lngtolen              : 5;  /**< [ 26: 22](R/W) Selects the bit in the counter for the long timeout value (timeout used when [SHTO] is
                                                                 clear). Values supported are between 11 and 29 (for a timeout values between 2^11 and
                                                                 2^29). Actual timeout is between 1x and 2x this interval. For example if [LNGTOLEN] = 28
                                                                 (the reset value), the timeout is between 256M and 512M core clocks. Note: a value of 0
                                                                 disables this timer. */
        uint64_t shtolen               : 5;  /**< [ 21: 17](R/W) Selects the bit in the counter for the short timeout value (timeout used when [SHTO] is
                                                                 set). Values supported are between 9 and 29 (for a timeout values between 2^9 and 2^29).
                                                                 Actual timeout is between 1x and 2x this interval. For example if [SHTOLEN] = 14 (the
                                                                 reset
                                                                 value), the timeout is between 16K and 32K core clocks. Note: a value of 0 disables this
                                                                 timer. */
        uint64_t shtoioen              : 1;  /**< [ 16: 16](R/W) When set, any core issues any of an IO load, atomic, acking store, acking IOBADDR, or
                                                                 acking LMTST to a node that doesn't exist (existence defined by the ENAOCI bits), then the
                                                                 hardware sets [SHTO]. */
        uint64_t shtoen                : 3;  /**< [ 15: 13](R/W) When set, if the corresponding CCPI link is down, the hardware sets [SHTO].
                                                                 See OCX_COM_LINK(0..2)_CTL for a description of what events can contribute to the
                                                                 link_down condition. */
        uint64_t shto                  : 1;  /**< [ 12: 12](R/W/H) Use short timeout intervals. When set, core uses SDIDTTO for both DID and commit counter
                                                                 timeouts, rather than DIDTTO/DIDTTO2. Similarly, L2C will use short instead of long
                                                                 timeout. */
        uint64_t inv_mode              : 2;  /**< [ 11: 10](R/W) Describes how aggressive to be when waiting for local invalidates before sending CCPI
                                                                 responses which act like commits at the remote.
                                                                 0x0 = Conservative mode, waits until all local invalidates have been sent by their
                                                                 respective CBCs to the cores.
                                                                 0x1 = Moderate mode, waits until all local invalidates have been sent to their respective
                                                                 CBCs, but not necessarily actually sent to the cores themselves.
                                                                 0x2 = Aggressive mode, does not wait for local invalidates to begin their processing. */
        uint64_t cas_fdx               : 1;  /**< [  9:  9](R/W) When set, L2 STC/CAS operations performed at the home will immediately bring the block
                                                                 exclusive into the home. Default operation is to first request the block shared and only
                                                                 invalidate the remote if the compare succeeds. */
        uint64_t rldd_psha             : 1;  /**< [  8:  8](R/W) When set, RLDD is assumed to return a shared response (PSHA). Default operation assumes an
                                                                 exclusive response (PEMD). Note that an incorrect assumption only causes an extra tag
                                                                 write to be done upon receiving the response. */
        uint64_t lock_local_iob        : 1;  /**< [  7:  7](R/W) When set, L2 atomic operations (excluding CAS/STC) initiated by IOB to remote addresses
                                                                 which miss at the requester are performed locally on the requesting node. When clear the
                                                                 operation instead sends the atomic request to be performed on the home node. For request
                                                                 initiated by core for STC and CAS ops; see
                                                                 [LOCK_LOCAL_PP]/[LOCK_LOCAL_STC]/[LOCK_LOCAL_CAS].
                                                                 Default is set to 1 (local locks). */
        uint64_t iofrcl                : 1;  /**< [  6:  6](R/W) When set, L2C services all I/O read and write operations on the local node, regardless of
                                                                 the value of the node ID bits in the physical address. During normal operation this bit is
                                                                 expected to be 0. Will only transition from 1 to 0, never from 0 to 1. */
        uint64_t reserved_4_5          : 2;
        uint64_t enaoci                : 4;  /**< [  3:  0](R/W) Enable CCPI processing (one bit per node_id). When set, perform CCPI
                                                                 processing. When clear, CCPI memory writes are blocked and CCPI memory reads
                                                                 return unpredictable data. When clear,
                                                                 CCPI I/O requests and MOC references are processed and sent to OCX where they are
                                                                 ultimately discarded. RDDISOCI/WRDISOCI/IORDDISOCI/IOWRDISOCI interrupts occur if and only
                                                                 if the corresponding ENAOCI\<node\> bit is clear. References to the local node (configured
                                                                 via OCX_COM_NODE[ID]) ignore the value of ENAOCI\<node\> because no CCPI processing is
                                                                 required. Similarly, all I/O references ignore the value of ENAOCI when
                                                                 L2C_OCI_CTL[IOFRCL] is set. */
#else /* Word 0 - Little Endian */
        uint64_t enaoci                : 4;  /**< [  3:  0](R/W) Enable CCPI processing (one bit per node_id). When set, perform CCPI
                                                                 processing. When clear, CCPI memory writes are blocked and CCPI memory reads
                                                                 return unpredictable data. When clear,
                                                                 CCPI I/O requests and MOC references are processed and sent to OCX where they are
                                                                 ultimately discarded. RDDISOCI/WRDISOCI/IORDDISOCI/IOWRDISOCI interrupts occur if and only
                                                                 if the corresponding ENAOCI\<node\> bit is clear. References to the local node (configured
                                                                 via OCX_COM_NODE[ID]) ignore the value of ENAOCI\<node\> because no CCPI processing is
                                                                 required. Similarly, all I/O references ignore the value of ENAOCI when
                                                                 L2C_OCI_CTL[IOFRCL] is set. */
        uint64_t reserved_4_5          : 2;
        uint64_t iofrcl                : 1;  /**< [  6:  6](R/W) When set, L2C services all I/O read and write operations on the local node, regardless of
                                                                 the value of the node ID bits in the physical address. During normal operation this bit is
                                                                 expected to be 0. Will only transition from 1 to 0, never from 0 to 1. */
        uint64_t lock_local_iob        : 1;  /**< [  7:  7](R/W) When set, L2 atomic operations (excluding CAS/STC) initiated by IOB to remote addresses
                                                                 which miss at the requester are performed locally on the requesting node. When clear the
                                                                 operation instead sends the atomic request to be performed on the home node. For request
                                                                 initiated by core for STC and CAS ops; see
                                                                 [LOCK_LOCAL_PP]/[LOCK_LOCAL_STC]/[LOCK_LOCAL_CAS].
                                                                 Default is set to 1 (local locks). */
        uint64_t rldd_psha             : 1;  /**< [  8:  8](R/W) When set, RLDD is assumed to return a shared response (PSHA). Default operation assumes an
                                                                 exclusive response (PEMD). Note that an incorrect assumption only causes an extra tag
                                                                 write to be done upon receiving the response. */
        uint64_t cas_fdx               : 1;  /**< [  9:  9](R/W) When set, L2 STC/CAS operations performed at the home will immediately bring the block
                                                                 exclusive into the home. Default operation is to first request the block shared and only
                                                                 invalidate the remote if the compare succeeds. */
        uint64_t inv_mode              : 2;  /**< [ 11: 10](R/W) Describes how aggressive to be when waiting for local invalidates before sending CCPI
                                                                 responses which act like commits at the remote.
                                                                 0x0 = Conservative mode, waits until all local invalidates have been sent by their
                                                                 respective CBCs to the cores.
                                                                 0x1 = Moderate mode, waits until all local invalidates have been sent to their respective
                                                                 CBCs, but not necessarily actually sent to the cores themselves.
                                                                 0x2 = Aggressive mode, does not wait for local invalidates to begin their processing. */
        uint64_t shto                  : 1;  /**< [ 12: 12](R/W/H) Use short timeout intervals. When set, core uses SDIDTTO for both DID and commit counter
                                                                 timeouts, rather than DIDTTO/DIDTTO2. Similarly, L2C will use short instead of long
                                                                 timeout. */
        uint64_t shtoen                : 3;  /**< [ 15: 13](R/W) When set, if the corresponding CCPI link is down, the hardware sets [SHTO].
                                                                 See OCX_COM_LINK(0..2)_CTL for a description of what events can contribute to the
                                                                 link_down condition. */
        uint64_t shtoioen              : 1;  /**< [ 16: 16](R/W) When set, any core issues any of an IO load, atomic, acking store, acking IOBADDR, or
                                                                 acking LMTST to a node that doesn't exist (existence defined by the ENAOCI bits), then the
                                                                 hardware sets [SHTO]. */
        uint64_t shtolen               : 5;  /**< [ 21: 17](R/W) Selects the bit in the counter for the short timeout value (timeout used when [SHTO] is
                                                                 set). Values supported are between 9 and 29 (for a timeout values between 2^9 and 2^29).
                                                                 Actual timeout is between 1x and 2x this interval. For example if [SHTOLEN] = 14 (the
                                                                 reset
                                                                 value), the timeout is between 16K and 32K core clocks. Note: a value of 0 disables this
                                                                 timer. */
        uint64_t lngtolen              : 5;  /**< [ 26: 22](R/W) Selects the bit in the counter for the long timeout value (timeout used when [SHTO] is
                                                                 clear). Values supported are between 11 and 29 (for a timeout values between 2^11 and
                                                                 2^29). Actual timeout is between 1x and 2x this interval. For example if [LNGTOLEN] = 28
                                                                 (the reset value), the timeout is between 256M and 512M core clocks. Note: a value of 0
                                                                 disables this timer. */
        uint64_t lock_local_pp         : 1;  /**< [ 27: 27](R/W) When clear, L2 atomic operations (excluding CAS/STC) core initiated requests to remote
                                                                 addresses which miss at the requester will send the atomic request to be performed on the
                                                                 home node. Default operation will instead  be performed locally on the requesting node.
                                                                 For request initiated by IOB & for STC & CAS ops, see
                                                                 [LOCK_LOCAL_IOB]/[LOCK_LOCAL_STC]/[LOCK_LOCAL_CAS]. */
        uint64_t lock_local_stc        : 1;  /**< [ 28: 28](R/W) When set, L2 STC operations to remote addresses which miss at the requester will be
                                                                 performed locally (if possible) on the requesting node. Default operation will instead
                                                                 send the STC request to be performed on the home node. For CAS ops [LOCK_LOCAL_CAS]. */
        uint64_t lock_local_cas        : 1;  /**< [ 29: 29](R/W) When set, L2 CAS operations to remote addresses which miss at the requester will be
                                                                 performed locally (if possible) on the requesting node. Default operation will instead
                                                                 send the CAS request to be performed on the home node. For STC ops [LOCK_LOCAL_STC]. */
        uint64_t ncpend                : 1;  /**< [ 30: 30](RO/H) An indication that a node change is pending.  Hardware sets this bit when
                                                                 OCX_COM_NODE[ID] is changed and clears the bit when the node change has taken
                                                                 effect. */
        uint64_t reserved_31_63        : 33;
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_l2c_oci_ctl bdk_l2c_oci_ctl_t;

#define BDK_L2C_OCI_CTL BDK_L2C_OCI_CTL_FUNC()
static inline uint64_t BDK_L2C_OCI_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_OCI_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e080800020ll;
    __bdk_csr_fatal("L2C_OCI_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_L2C_OCI_CTL bdk_l2c_oci_ctl_t
#define bustype_BDK_L2C_OCI_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_OCI_CTL "L2C_OCI_CTL"
#define device_bar_BDK_L2C_OCI_CTL 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_OCI_CTL 0
#define arguments_BDK_L2C_OCI_CTL -1,-1,-1,-1

/**
 * Register (RSL) l2c_qos_pp#
 *
 * L2C Core QOS Level Registers
 */
union bdk_l2c_qos_ppx
{
    uint64_t u;
    struct bdk_l2c_qos_ppx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_3_63         : 61;
        uint64_t lvl                   : 3;  /**< [  2:  0](R/W) QOS level to use for this core. */
#else /* Word 0 - Little Endian */
        uint64_t lvl                   : 3;  /**< [  2:  0](R/W) QOS level to use for this core. */
        uint64_t reserved_3_63         : 61;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_qos_ppx_s cn; */
};
typedef union bdk_l2c_qos_ppx bdk_l2c_qos_ppx_t;

static inline uint64_t BDK_L2C_QOS_PPX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_QOS_PPX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e080880000ll + 8ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x87e080880000ll + 8ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x87e080880000ll + 8ll * ((a) & 0x3f);
    __bdk_csr_fatal("L2C_QOS_PPX", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_QOS_PPX(a) bdk_l2c_qos_ppx_t
#define bustype_BDK_L2C_QOS_PPX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_QOS_PPX(a) "L2C_QOS_PPX"
#define device_bar_BDK_L2C_QOS_PPX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_QOS_PPX(a) (a)
#define arguments_BDK_L2C_QOS_PPX(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_qos_wgt
 *
 * L2C QOS Weight Register
 */
union bdk_l2c_qos_wgt
{
    uint64_t u;
    struct bdk_l2c_qos_wgt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t wgt7                  : 8;  /**< [ 63: 56](R/W) Weight for QOS level 7. */
        uint64_t wgt6                  : 8;  /**< [ 55: 48](R/W) Weight for QOS level 6. */
        uint64_t wgt5                  : 8;  /**< [ 47: 40](R/W) Weight for QOS level 5. */
        uint64_t wgt4                  : 8;  /**< [ 39: 32](R/W) Weight for QOS level 4. */
        uint64_t wgt3                  : 8;  /**< [ 31: 24](R/W) Weight for QOS level 3. */
        uint64_t wgt2                  : 8;  /**< [ 23: 16](R/W) Weight for QOS level 2. */
        uint64_t wgt1                  : 8;  /**< [ 15:  8](R/W) Weight for QOS level 1. */
        uint64_t wgt0                  : 8;  /**< [  7:  0](R/W) Weight for QOS level 0. */
#else /* Word 0 - Little Endian */
        uint64_t wgt0                  : 8;  /**< [  7:  0](R/W) Weight for QOS level 0. */
        uint64_t wgt1                  : 8;  /**< [ 15:  8](R/W) Weight for QOS level 1. */
        uint64_t wgt2                  : 8;  /**< [ 23: 16](R/W) Weight for QOS level 2. */
        uint64_t wgt3                  : 8;  /**< [ 31: 24](R/W) Weight for QOS level 3. */
        uint64_t wgt4                  : 8;  /**< [ 39: 32](R/W) Weight for QOS level 4. */
        uint64_t wgt5                  : 8;  /**< [ 47: 40](R/W) Weight for QOS level 5. */
        uint64_t wgt6                  : 8;  /**< [ 55: 48](R/W) Weight for QOS level 6. */
        uint64_t wgt7                  : 8;  /**< [ 63: 56](R/W) Weight for QOS level 7. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_qos_wgt_s cn; */
};
typedef union bdk_l2c_qos_wgt bdk_l2c_qos_wgt_t;

#define BDK_L2C_QOS_WGT BDK_L2C_QOS_WGT_FUNC()
static inline uint64_t BDK_L2C_QOS_WGT_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_QOS_WGT_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e080800008ll;
    __bdk_csr_fatal("L2C_QOS_WGT", 0, 0, 0, 0, 0);
}

#define typedef_BDK_L2C_QOS_WGT bdk_l2c_qos_wgt_t
#define bustype_BDK_L2C_QOS_WGT BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_QOS_WGT "L2C_QOS_WGT"
#define device_bar_BDK_L2C_QOS_WGT 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_QOS_WGT 0
#define arguments_BDK_L2C_QOS_WGT -1,-1,-1,-1

/**
 * Register (RSL) l2c_tad#_dll
 *
 * L2C TAD DLL Observability Register
 * This register provides the parameters for DLL observability.
 */
union bdk_l2c_tadx_dll
{
    uint64_t u;
    struct bdk_l2c_tadx_dll_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_60_63        : 4;
        uint64_t max_dll_setting       : 12; /**< [ 59: 48](RO/H) Max reported DLL setting. */
        uint64_t min_dll_setting       : 12; /**< [ 47: 36](RO/H) Min reported DLL setting. */
        uint64_t pd_pos_rclk_refclk    : 1;  /**< [ 35: 35](RO/H) Phase detector output. */
        uint64_t pdl_rclk_refclk       : 1;  /**< [ 34: 34](RO/H) Phase detector output. */
        uint64_t pdr_rclk_refclk       : 1;  /**< [ 33: 33](RO/H) Phase detector output. */
        uint64_t reserved_32           : 1;
        uint64_t dly_elem_enable       : 16; /**< [ 31: 16](RO/H) Delay element enable. */
        uint64_t dll_setting           : 12; /**< [ 15:  4](RO/H) DLL setting. */
        uint64_t reserved_1_3          : 3;
        uint64_t dll_lock              : 1;  /**< [  0:  0](RO/H) DLL lock: 1 = locked, 0 = unlocked. */
#else /* Word 0 - Little Endian */
        uint64_t dll_lock              : 1;  /**< [  0:  0](RO/H) DLL lock: 1 = locked, 0 = unlocked. */
        uint64_t reserved_1_3          : 3;
        uint64_t dll_setting           : 12; /**< [ 15:  4](RO/H) DLL setting. */
        uint64_t dly_elem_enable       : 16; /**< [ 31: 16](RO/H) Delay element enable. */
        uint64_t reserved_32           : 1;
        uint64_t pdr_rclk_refclk       : 1;  /**< [ 33: 33](RO/H) Phase detector output. */
        uint64_t pdl_rclk_refclk       : 1;  /**< [ 34: 34](RO/H) Phase detector output. */
        uint64_t pd_pos_rclk_refclk    : 1;  /**< [ 35: 35](RO/H) Phase detector output. */
        uint64_t min_dll_setting       : 12; /**< [ 47: 36](RO/H) Min reported DLL setting. */
        uint64_t max_dll_setting       : 12; /**< [ 59: 48](RO/H) Max reported DLL setting. */
        uint64_t reserved_60_63        : 4;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_tadx_dll_s cn; */
};
typedef union bdk_l2c_tadx_dll bdk_l2c_tadx_dll_t;

static inline uint64_t BDK_L2C_TADX_DLL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_DLL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050030000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050030000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050030000ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_DLL", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_DLL(a) bdk_l2c_tadx_dll_t
#define bustype_BDK_L2C_TADX_DLL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_DLL(a) "L2C_TADX_DLL"
#define device_bar_BDK_L2C_TADX_DLL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_DLL(a) (a)
#define arguments_BDK_L2C_TADX_DLL(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad#_err
 *
 * L2C TAD Request Error Info Registers
 * This register records error information for *DISOCI and *NXM
 * interrupts. The NXM logic only applies to local addresses. A command for
 * a remote address does not cause a [RDNXM]/[WRNXM] on the requesting node, but
 * may on the remote node. Similarly, [RDDISOCI]/[WRDISOCI] is always for a remote
 * address. The first [WRDISOCI]/[WRNXM] error will lock the register until the
 * logged error type is cleared; [RDDISOCI]/[RDNXM] never locks the register.
 */
union bdk_l2c_tadx_err
{
    uint64_t u;
    struct bdk_l2c_tadx_err_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t rdnxm                 : 1;  /**< [ 63: 63](RO/H) Logged information is for a L2C_TAD()_INT_W1C[RDNXM] error. */
        uint64_t wrnxm                 : 1;  /**< [ 62: 62](RO/H) Logged information is for a L2C_TAD()_INT_W1C[WRNXM] error. */
        uint64_t rddisoci              : 1;  /**< [ 61: 61](RO/H) Logged information is for a L2C_TAD()_INT_W1C[RDDISOCI] error. */
        uint64_t wrdisoci              : 1;  /**< [ 60: 60](RO/H) Logged information is for a L2C_TAD()_INT_W1C[WRDISOCI] error. */
        uint64_t nonsec                : 1;  /**< [ 59: 59](RO/H) Nonsecure (NS) bit of request causing error. */
        uint64_t cmd                   : 8;  /**< [ 58: 51](RO/H) Encoding of XMC or CCPI command causing error.
                                                                 Internal:
                                                                 If CMD\<7\>==1, use XMC_CMD_E to
                                                                 decode CMD\<6:0\>. If CMD\<7:5\>==0, use OCI_MREQ_CMD_E to decode CMD\<4:0\>. If CMD\<7:5\>==1,
                                                                 use OCI_MFWD_CMD_E to decode CMD\<4:0\>. If CMD\<7:5\>==2, use OCI_MRSP_CMD_E to decode
                                                                 CMD\<4:0\>. */
        uint64_t source                : 7;  /**< [ 50: 44](RO/H) XMC source of request causing error. If [SOURCE]\<6\>==0, then [SOURCE]\<5:0\> is
                                                                 PPID, else [SOURCE]\<3:0\> is BUSID of the IOB which made the request. If
                                                                 [CMD]\<7\>==0, this field is unpredictable. */
        uint64_t reserved_42_43        : 2;
        uint64_t node                  : 2;  /**< [ 41: 40](RO/H) CCPI node of XMC request causing error. For *NXM errors [NODE] is always the node that
                                                                 generated request causing the error (*NXM errors are logged at the home node). For *DISOCI
                                                                 errors, is the NODE the request is directed to (DISOCI request is always the current
                                                                 node). */
        uint64_t addr                  : 40; /**< [ 39:  0](RO/H) XMC address causing the error. [ADDR]\<6:0\> is unpredictable for *DISOCI and *NXM
                                                                 errors. This field is the physical address after index aliasing (if enabled). */
#else /* Word 0 - Little Endian */
        uint64_t addr                  : 40; /**< [ 39:  0](RO/H) XMC address causing the error. [ADDR]\<6:0\> is unpredictable for *DISOCI and *NXM
                                                                 errors. This field is the physical address after index aliasing (if enabled). */
        uint64_t node                  : 2;  /**< [ 41: 40](RO/H) CCPI node of XMC request causing error. For *NXM errors [NODE] is always the node that
                                                                 generated request causing the error (*NXM errors are logged at the home node). For *DISOCI
                                                                 errors, is the NODE the request is directed to (DISOCI request is always the current
                                                                 node). */
        uint64_t reserved_42_43        : 2;
        uint64_t source                : 7;  /**< [ 50: 44](RO/H) XMC source of request causing error. If [SOURCE]\<6\>==0, then [SOURCE]\<5:0\> is
                                                                 PPID, else [SOURCE]\<3:0\> is BUSID of the IOB which made the request. If
                                                                 [CMD]\<7\>==0, this field is unpredictable. */
        uint64_t cmd                   : 8;  /**< [ 58: 51](RO/H) Encoding of XMC or CCPI command causing error.
                                                                 Internal:
                                                                 If CMD\<7\>==1, use XMC_CMD_E to
                                                                 decode CMD\<6:0\>. If CMD\<7:5\>==0, use OCI_MREQ_CMD_E to decode CMD\<4:0\>. If CMD\<7:5\>==1,
                                                                 use OCI_MFWD_CMD_E to decode CMD\<4:0\>. If CMD\<7:5\>==2, use OCI_MRSP_CMD_E to decode
                                                                 CMD\<4:0\>. */
        uint64_t nonsec                : 1;  /**< [ 59: 59](RO/H) Nonsecure (NS) bit of request causing error. */
        uint64_t wrdisoci              : 1;  /**< [ 60: 60](RO/H) Logged information is for a L2C_TAD()_INT_W1C[WRDISOCI] error. */
        uint64_t rddisoci              : 1;  /**< [ 61: 61](RO/H) Logged information is for a L2C_TAD()_INT_W1C[RDDISOCI] error. */
        uint64_t wrnxm                 : 1;  /**< [ 62: 62](RO/H) Logged information is for a L2C_TAD()_INT_W1C[WRNXM] error. */
        uint64_t rdnxm                 : 1;  /**< [ 63: 63](RO/H) Logged information is for a L2C_TAD()_INT_W1C[RDNXM] error. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_tadx_err_s cn; */
};
typedef union bdk_l2c_tadx_err bdk_l2c_tadx_err_t;

static inline uint64_t BDK_L2C_TADX_ERR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_ERR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050060000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050060000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050060000ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_ERR", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_ERR(a) bdk_l2c_tadx_err_t
#define bustype_BDK_L2C_TADX_ERR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_ERR(a) "L2C_TADX_ERR"
#define device_bar_BDK_L2C_TADX_ERR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_ERR(a) (a)
#define arguments_BDK_L2C_TADX_ERR(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad#_pfc#
 *
 * L2C TAD Performance Counter Registers
 */
union bdk_l2c_tadx_pfcx
{
    uint64_t u;
    struct bdk_l2c_tadx_pfcx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Current counter value. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Current counter value. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_tadx_pfcx_s cn; */
};
typedef union bdk_l2c_tadx_pfcx bdk_l2c_tadx_pfcx_t;

static inline uint64_t BDK_L2C_TADX_PFCX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_PFCX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=3)))
        return 0x87e050010100ll + 0x1000000ll * ((a) & 0x0) + 8ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b<=3)))
        return 0x87e050010100ll + 0x1000000ll * ((a) & 0x3) + 8ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=7) && (b<=3)))
        return 0x87e050010100ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x3);
    __bdk_csr_fatal("L2C_TADX_PFCX", 2, a, b, 0, 0);
}

#define typedef_BDK_L2C_TADX_PFCX(a,b) bdk_l2c_tadx_pfcx_t
#define bustype_BDK_L2C_TADX_PFCX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_PFCX(a,b) "L2C_TADX_PFCX"
#define device_bar_BDK_L2C_TADX_PFCX(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_PFCX(a,b) (a)
#define arguments_BDK_L2C_TADX_PFCX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) l2c_tad#_prf
 *
 * L2C TAD Performance Counter Control Registers
 * All four counters are equivalent and can use any of the defined selects.
 */
union bdk_l2c_tadx_prf
{
    uint64_t u;
    struct bdk_l2c_tadx_prf_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t cnt3sel               : 8;  /**< [ 31: 24](R/W) Selects event to count for L2C_TAD(0)_PFC(3). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt2sel               : 8;  /**< [ 23: 16](R/W) Selects event to count for L2C_TAD(0)_PFC(2). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt1sel               : 8;  /**< [ 15:  8](R/W) Selects event to count for L2C_TAD(0)_PFC(1). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt0sel               : 8;  /**< [  7:  0](R/W) Selects event to count for L2C_TAD(0)_PFC(0). Enumerated by L2C_TAD_PRF_SEL_E. */
#else /* Word 0 - Little Endian */
        uint64_t cnt0sel               : 8;  /**< [  7:  0](R/W) Selects event to count for L2C_TAD(0)_PFC(0). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt1sel               : 8;  /**< [ 15:  8](R/W) Selects event to count for L2C_TAD(0)_PFC(1). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt2sel               : 8;  /**< [ 23: 16](R/W) Selects event to count for L2C_TAD(0)_PFC(2). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt3sel               : 8;  /**< [ 31: 24](R/W) Selects event to count for L2C_TAD(0)_PFC(3). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_tadx_prf_s cn81xx; */
    struct bdk_l2c_tadx_prf_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t cnt3sel               : 8;  /**< [ 31: 24](R/W) Selects event to count for L2C_TAD(0..7)_PFC(3). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt2sel               : 8;  /**< [ 23: 16](R/W) Selects event to count for L2C_TAD(0..7)_PFC(2). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt1sel               : 8;  /**< [ 15:  8](R/W) Selects event to count for L2C_TAD(0..7)_PFC(1). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt0sel               : 8;  /**< [  7:  0](R/W) Selects event to count for L2C_TAD(0..7)_PFC(0). Enumerated by L2C_TAD_PRF_SEL_E. */
#else /* Word 0 - Little Endian */
        uint64_t cnt0sel               : 8;  /**< [  7:  0](R/W) Selects event to count for L2C_TAD(0..7)_PFC(0). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt1sel               : 8;  /**< [ 15:  8](R/W) Selects event to count for L2C_TAD(0..7)_PFC(1). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt2sel               : 8;  /**< [ 23: 16](R/W) Selects event to count for L2C_TAD(0..7)_PFC(2). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt3sel               : 8;  /**< [ 31: 24](R/W) Selects event to count for L2C_TAD(0..7)_PFC(3). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_l2c_tadx_prf_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t cnt3sel               : 8;  /**< [ 31: 24](R/W) Selects event to count for L2C_TAD(0..3)_PFC(3). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt2sel               : 8;  /**< [ 23: 16](R/W) Selects event to count for L2C_TAD(0..3)_PFC(2). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt1sel               : 8;  /**< [ 15:  8](R/W) Selects event to count for L2C_TAD(0..3)_PFC(1). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt0sel               : 8;  /**< [  7:  0](R/W) Selects event to count for L2C_TAD(0..3)_PFC(0). Enumerated by L2C_TAD_PRF_SEL_E. */
#else /* Word 0 - Little Endian */
        uint64_t cnt0sel               : 8;  /**< [  7:  0](R/W) Selects event to count for L2C_TAD(0..3)_PFC(0). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt1sel               : 8;  /**< [ 15:  8](R/W) Selects event to count for L2C_TAD(0..3)_PFC(1). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt2sel               : 8;  /**< [ 23: 16](R/W) Selects event to count for L2C_TAD(0..3)_PFC(2). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t cnt3sel               : 8;  /**< [ 31: 24](R/W) Selects event to count for L2C_TAD(0..3)_PFC(3). Enumerated by L2C_TAD_PRF_SEL_E. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_l2c_tadx_prf bdk_l2c_tadx_prf_t;

static inline uint64_t BDK_L2C_TADX_PRF(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_PRF(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050010000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050010000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050010000ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_PRF", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_PRF(a) bdk_l2c_tadx_prf_t
#define bustype_BDK_L2C_TADX_PRF(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_PRF(a) "L2C_TADX_PRF"
#define device_bar_BDK_L2C_TADX_PRF(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_PRF(a) (a)
#define arguments_BDK_L2C_TADX_PRF(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad#_stat
 *
 * L2C TAD Status Registers
 * This register holds information about the instantaneous state of the TAD.
 */
union bdk_l2c_tadx_stat
{
    uint64_t u;
    struct bdk_l2c_tadx_stat_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t lfb_valid_cnt         : 6;  /**< [ 13:  8](RO/H) The number of LFBs in use. */
        uint64_t reserved_5_7          : 3;
        uint64_t vbf_inuse_cnt         : 5;  /**< [  4:  0](RO/H) The number of MCI VBFs in use. */
#else /* Word 0 - Little Endian */
        uint64_t vbf_inuse_cnt         : 5;  /**< [  4:  0](RO/H) The number of MCI VBFs in use. */
        uint64_t reserved_5_7          : 3;
        uint64_t lfb_valid_cnt         : 6;  /**< [ 13:  8](RO/H) The number of LFBs in use. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_tadx_stat_s cn; */
};
typedef union bdk_l2c_tadx_stat bdk_l2c_tadx_stat_t;

static inline uint64_t BDK_L2C_TADX_STAT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_STAT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050020008ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050020008ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050020008ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_STAT", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_STAT(a) bdk_l2c_tadx_stat_t
#define bustype_BDK_L2C_TADX_STAT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_STAT(a) "L2C_TADX_STAT"
#define device_bar_BDK_L2C_TADX_STAT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_STAT(a) (a)
#define arguments_BDK_L2C_TADX_STAT(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad#_tag
 *
 * L2C TAD Tag Data Registers
 * This register holds the tag information for LTGL2I and STGL2I commands.
 */
union bdk_l2c_tadx_tag
{
    uint64_t u;
    struct bdk_l2c_tadx_tag_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t sblkdty               : 4;  /**< [ 63: 60](R/W/H) Sub-block dirty bits. Ignored/loaded with 0 for RTG accesses. If [TS] is invalid
                                                                 (0), [SBLKDTY] must be 0 or operation is undefined. */
        uint64_t reserved_59           : 1;
        uint64_t nonsec                : 1;  /**< [ 58: 58](R/W/H) Nonsecure bit. */
        uint64_t businfo               : 9;  /**< [ 57: 49](R/W/H) The bus information bits. Ignored/loaded with 0 for RTG accesses. */
        uint64_t ecc                   : 7;  /**< [ 48: 42](R/W/H) The tag ECC. This field is undefined if L2C_CTL[DISECC] is not 1 when the LTGL2I reads the tags. */
        uint64_t reserved_6_41         : 36;
        uint64_t node                  : 2;  /**< [  5:  4](RAZ) Reserved. */
        uint64_t ts                    : 2;  /**< [  3:  2](R/W/H) The tag state.
                                                                 0x0 = Invalid.
                                                                 0x1 = Shared.
                                                                 0x2 = Exclusive.

                                                                 Note that a local address will never have the value of exclusive as that state
                                                                 is encoded as shared in the tag and invalid in the RTG. */
        uint64_t used                  : 1;  /**< [  1:  1](R/W/H) The LRU use bit. If setting the [LOCK] bit, the USE bit should also be set or
                                                                 the operation is undefined. Ignored/loaded with 0 for RTG accesses. */
        uint64_t lock                  : 1;  /**< [  0:  0](R/W/H) The lock bit. If setting the [LOCK] bit, the USE bit should also be set or the
                                                                 operation is undefined. Ignored/loaded with 0 for RTG accesses. */
#else /* Word 0 - Little Endian */
        uint64_t lock                  : 1;  /**< [  0:  0](R/W/H) The lock bit. If setting the [LOCK] bit, the USE bit should also be set or the
                                                                 operation is undefined. Ignored/loaded with 0 for RTG accesses. */
        uint64_t used                  : 1;  /**< [  1:  1](R/W/H) The LRU use bit. If setting the [LOCK] bit, the USE bit should also be set or
                                                                 the operation is undefined. Ignored/loaded with 0 for RTG accesses. */
        uint64_t ts                    : 2;  /**< [  3:  2](R/W/H) The tag state.
                                                                 0x0 = Invalid.
                                                                 0x1 = Shared.
                                                                 0x2 = Exclusive.

                                                                 Note that a local address will never have the value of exclusive as that state
                                                                 is encoded as shared in the tag and invalid in the RTG. */
        uint64_t node                  : 2;  /**< [  5:  4](RAZ) Reserved. */
        uint64_t reserved_6_41         : 36;
        uint64_t ecc                   : 7;  /**< [ 48: 42](R/W/H) The tag ECC. This field is undefined if L2C_CTL[DISECC] is not 1 when the LTGL2I reads the tags. */
        uint64_t businfo               : 9;  /**< [ 57: 49](R/W/H) The bus information bits. Ignored/loaded with 0 for RTG accesses. */
        uint64_t nonsec                : 1;  /**< [ 58: 58](R/W/H) Nonsecure bit. */
        uint64_t reserved_59           : 1;
        uint64_t sblkdty               : 4;  /**< [ 63: 60](R/W/H) Sub-block dirty bits. Ignored/loaded with 0 for RTG accesses. If [TS] is invalid
                                                                 (0), [SBLKDTY] must be 0 or operation is undefined. */
#endif /* Word 0 - End */
    } s;
    struct bdk_l2c_tadx_tag_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t sblkdty               : 4;  /**< [ 63: 60](R/W/H) Sub-block dirty bits. Ignored/loaded with 0 for RTG accesses. If [TS] is invalid
                                                                 (0), [SBLKDTY] must be 0 or operation is undefined. */
        uint64_t reserved_59           : 1;
        uint64_t nonsec                : 1;  /**< [ 58: 58](R/W/H) Nonsecure bit. */
        uint64_t reserved_57           : 1;
        uint64_t businfo               : 8;  /**< [ 56: 49](R/W/H) The bus information bits. Ignored/loaded with 0 for RTG accesses. */
        uint64_t ecc                   : 7;  /**< [ 48: 42](R/W/H) The tag ECC. This field is undefined if L2C_CTL[DISECC] is not 1 when the LTGL2I reads the tags. */
        uint64_t reserved_40_41        : 2;
        uint64_t tag                   : 23; /**< [ 39: 17](R/W/H) The tag. TAG\<39:17\> is the corresponding bits from the L2C+LMC internal L2/DRAM byte
                                                                 address. */
        uint64_t reserved_6_16         : 11;
        uint64_t node                  : 2;  /**< [  5:  4](RAZ) Reserved. */
        uint64_t ts                    : 2;  /**< [  3:  2](R/W/H) The tag state.
                                                                 0x0 = Invalid.
                                                                 0x1 = Shared.
                                                                 0x2 = Exclusive.

                                                                 Note that a local address will never have the value of exclusive as that state
                                                                 is encoded as shared in the tag and invalid in the RTG. */
        uint64_t used                  : 1;  /**< [  1:  1](R/W/H) The LRU use bit. If setting the [LOCK] bit, the USE bit should also be set or
                                                                 the operation is undefined. Ignored/loaded with 0 for RTG accesses. */
        uint64_t lock                  : 1;  /**< [  0:  0](R/W/H) The lock bit. If setting the [LOCK] bit, the USE bit should also be set or the
                                                                 operation is undefined. Ignored/loaded with 0 for RTG accesses. */
#else /* Word 0 - Little Endian */
        uint64_t lock                  : 1;  /**< [  0:  0](R/W/H) The lock bit. If setting the [LOCK] bit, the USE bit should also be set or the
                                                                 operation is undefined. Ignored/loaded with 0 for RTG accesses. */
        uint64_t used                  : 1;  /**< [  1:  1](R/W/H) The LRU use bit. If setting the [LOCK] bit, the USE bit should also be set or
                                                                 the operation is undefined. Ignored/loaded with 0 for RTG accesses. */
        uint64_t ts                    : 2;  /**< [  3:  2](R/W/H) The tag state.
                                                                 0x0 = Invalid.
                                                                 0x1 = Shared.
                                                                 0x2 = Exclusive.

                                                                 Note that a local address will never have the value of exclusive as that state
                                                                 is encoded as shared in the tag and invalid in the RTG. */
        uint64_t node                  : 2;  /**< [  5:  4](RAZ) Reserved. */
        uint64_t reserved_6_16         : 11;
        uint64_t tag                   : 23; /**< [ 39: 17](R/W/H) The tag. TAG\<39:17\> is the corresponding bits from the L2C+LMC internal L2/DRAM byte
                                                                 address. */
        uint64_t reserved_40_41        : 2;
        uint64_t ecc                   : 7;  /**< [ 48: 42](R/W/H) The tag ECC. This field is undefined if L2C_CTL[DISECC] is not 1 when the LTGL2I reads the tags. */
        uint64_t businfo               : 8;  /**< [ 56: 49](R/W/H) The bus information bits. Ignored/loaded with 0 for RTG accesses. */
        uint64_t reserved_57           : 1;
        uint64_t nonsec                : 1;  /**< [ 58: 58](R/W/H) Nonsecure bit. */
        uint64_t reserved_59           : 1;
        uint64_t sblkdty               : 4;  /**< [ 63: 60](R/W/H) Sub-block dirty bits. Ignored/loaded with 0 for RTG accesses. If [TS] is invalid
                                                                 (0), [SBLKDTY] must be 0 or operation is undefined. */
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_l2c_tadx_tag_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t sblkdty               : 4;  /**< [ 63: 60](R/W/H) Sub-block dirty bits. Ignored/loaded with 0 for RTG accesses. If [TS] is invalid
                                                                 (0), [SBLKDTY] must be 0 or operation is undefined. */
        uint64_t reserved_59           : 1;
        uint64_t nonsec                : 1;  /**< [ 58: 58](R/W/H) Nonsecure bit. */
        uint64_t businfo               : 9;  /**< [ 57: 49](R/W/H) The bus information bits. Ignored/loaded with 0 for RTG accesses. */
        uint64_t ecc                   : 7;  /**< [ 48: 42](R/W/H) The tag ECC. This field is undefined if L2C_CTL[DISECC] is not 1 when the LTGL2I reads the tags. */
        uint64_t tag                   : 22; /**< [ 41: 20](R/W/H) The tag. TAG\<39:20\> is the corresponding bits from the L2C+LMC internal L2/DRAM byte
                                                                 address. TAG\<41:40\> is the CCPI node of the address. The RTG must always have the
                                                                 TAG\<41:40\> equal to the current node or operation is undefined. */
        uint64_t reserved_6_19         : 14;
        uint64_t node                  : 2;  /**< [  5:  4](R/W/H) The node ID for the remote node which holds this block. Ignored/loaded with 0 for TAG accesses. */
        uint64_t ts                    : 2;  /**< [  3:  2](R/W/H) The tag state.
                                                                 0x0 = Invalid.
                                                                 0x1 = Shared.
                                                                 0x2 = Exclusive.

                                                                 Note that a local address will never have the value of exclusive as that state
                                                                 is encoded as shared in the tag and invalid in the RTG. */
        uint64_t used                  : 1;  /**< [  1:  1](R/W/H) The LRU use bit. If setting the [LOCK] bit, the USE bit should also be set or
                                                                 the operation is undefined. Ignored/loaded with 0 for RTG accesses. */
        uint64_t lock                  : 1;  /**< [  0:  0](R/W/H) The lock bit. If setting the [LOCK] bit, the USE bit should also be set or the
                                                                 operation is undefined. Ignored/loaded with 0 for RTG accesses. */
#else /* Word 0 - Little Endian */
        uint64_t lock                  : 1;  /**< [  0:  0](R/W/H) The lock bit. If setting the [LOCK] bit, the USE bit should also be set or the
                                                                 operation is undefined. Ignored/loaded with 0 for RTG accesses. */
        uint64_t used                  : 1;  /**< [  1:  1](R/W/H) The LRU use bit. If setting the [LOCK] bit, the USE bit should also be set or
                                                                 the operation is undefined. Ignored/loaded with 0 for RTG accesses. */
        uint64_t ts                    : 2;  /**< [  3:  2](R/W/H) The tag state.
                                                                 0x0 = Invalid.
                                                                 0x1 = Shared.
                                                                 0x2 = Exclusive.

                                                                 Note that a local address will never have the value of exclusive as that state
                                                                 is encoded as shared in the tag and invalid in the RTG. */
        uint64_t node                  : 2;  /**< [  5:  4](R/W/H) The node ID for the remote node which holds this block. Ignored/loaded with 0 for TAG accesses. */
        uint64_t reserved_6_19         : 14;
        uint64_t tag                   : 22; /**< [ 41: 20](R/W/H) The tag. TAG\<39:20\> is the corresponding bits from the L2C+LMC internal L2/DRAM byte
                                                                 address. TAG\<41:40\> is the CCPI node of the address. The RTG must always have the
                                                                 TAG\<41:40\> equal to the current node or operation is undefined. */
        uint64_t ecc                   : 7;  /**< [ 48: 42](R/W/H) The tag ECC. This field is undefined if L2C_CTL[DISECC] is not 1 when the LTGL2I reads the tags. */
        uint64_t businfo               : 9;  /**< [ 57: 49](R/W/H) The bus information bits. Ignored/loaded with 0 for RTG accesses. */
        uint64_t nonsec                : 1;  /**< [ 58: 58](R/W/H) Nonsecure bit. */
        uint64_t reserved_59           : 1;
        uint64_t sblkdty               : 4;  /**< [ 63: 60](R/W/H) Sub-block dirty bits. Ignored/loaded with 0 for RTG accesses. If [TS] is invalid
                                                                 (0), [SBLKDTY] must be 0 or operation is undefined. */
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_l2c_tadx_tag_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t sblkdty               : 4;  /**< [ 63: 60](R/W/H) Sub-block dirty bits. Ignored/loaded with 0 for RTG accesses. If [TS] is invalid
                                                                 (0), [SBLKDTY] must be 0 or operation is undefined. */
        uint64_t reserved_59           : 1;
        uint64_t nonsec                : 1;  /**< [ 58: 58](R/W/H) Nonsecure bit. */
        uint64_t businfo               : 9;  /**< [ 57: 49](R/W/H) The bus information bits. Ignored/loaded with 0 for RTG accesses. */
        uint64_t ecc                   : 7;  /**< [ 48: 42](R/W/H) The tag ECC. This field is undefined if L2C_CTL[DISECC] is not 1 when the LTGL2I reads the tags. */
        uint64_t reserved_40_41        : 2;
        uint64_t tag                   : 22; /**< [ 39: 18](R/W/H) The tag. TAG\<39:18\> is the corresponding bits from the L2C+LMC internal L2/DRAM byte
                                                                 address. */
        uint64_t reserved_6_17         : 12;
        uint64_t node                  : 2;  /**< [  5:  4](RAZ) Reserved. */
        uint64_t ts                    : 2;  /**< [  3:  2](R/W/H) The tag state.
                                                                 0x0 = Invalid.
                                                                 0x1 = Shared.
                                                                 0x2 = Exclusive.

                                                                 Note that a local address will never have the value of exclusive as that state
                                                                 is encoded as shared in the tag and invalid in the RTG. */
        uint64_t used                  : 1;  /**< [  1:  1](R/W/H) The LRU use bit. If setting the [LOCK] bit, the USE bit should also be set or
                                                                 the operation is undefined. Ignored/loaded with 0 for RTG accesses. */
        uint64_t lock                  : 1;  /**< [  0:  0](R/W/H) The lock bit. If setting the [LOCK] bit, the USE bit should also be set or the
                                                                 operation is undefined. Ignored/loaded with 0 for RTG accesses. */
#else /* Word 0 - Little Endian */
        uint64_t lock                  : 1;  /**< [  0:  0](R/W/H) The lock bit. If setting the [LOCK] bit, the USE bit should also be set or the
                                                                 operation is undefined. Ignored/loaded with 0 for RTG accesses. */
        uint64_t used                  : 1;  /**< [  1:  1](R/W/H) The LRU use bit. If setting the [LOCK] bit, the USE bit should also be set or
                                                                 the operation is undefined. Ignored/loaded with 0 for RTG accesses. */
        uint64_t ts                    : 2;  /**< [  3:  2](R/W/H) The tag state.
                                                                 0x0 = Invalid.
                                                                 0x1 = Shared.
                                                                 0x2 = Exclusive.

                                                                 Note that a local address will never have the value of exclusive as that state
                                                                 is encoded as shared in the tag and invalid in the RTG. */
        uint64_t node                  : 2;  /**< [  5:  4](RAZ) Reserved. */
        uint64_t reserved_6_17         : 12;
        uint64_t tag                   : 22; /**< [ 39: 18](R/W/H) The tag. TAG\<39:18\> is the corresponding bits from the L2C+LMC internal L2/DRAM byte
                                                                 address. */
        uint64_t reserved_40_41        : 2;
        uint64_t ecc                   : 7;  /**< [ 48: 42](R/W/H) The tag ECC. This field is undefined if L2C_CTL[DISECC] is not 1 when the LTGL2I reads the tags. */
        uint64_t businfo               : 9;  /**< [ 57: 49](R/W/H) The bus information bits. Ignored/loaded with 0 for RTG accesses. */
        uint64_t nonsec                : 1;  /**< [ 58: 58](R/W/H) Nonsecure bit. */
        uint64_t reserved_59           : 1;
        uint64_t sblkdty               : 4;  /**< [ 63: 60](R/W/H) Sub-block dirty bits. Ignored/loaded with 0 for RTG accesses. If [TS] is invalid
                                                                 (0), [SBLKDTY] must be 0 or operation is undefined. */
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_l2c_tadx_tag bdk_l2c_tadx_tag_t;

static inline uint64_t BDK_L2C_TADX_TAG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_TAG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050020000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050020000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050020000ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_TAG", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_TAG(a) bdk_l2c_tadx_tag_t
#define bustype_BDK_L2C_TADX_TAG(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_TAG(a) "L2C_TADX_TAG"
#define device_bar_BDK_L2C_TADX_TAG(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_TAG(a) (a)
#define arguments_BDK_L2C_TADX_TAG(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad#_timeout
 *
 * L2C TAD LFB Timeout Info Registers
 * This register records error information for an LFBTO (LFB TimeOut). The first LFBTO error will
 * lock the register until the logged error type s cleared. If multiple LFBs timed out
 * simultaneously, then this will contain the information from the lowest LFB number that has
 * timed-out. The address can be for the original transaction address or the replacement address
 * (if both could have timed out, then the transaction address will be here).
 */
union bdk_l2c_tadx_timeout
{
    uint64_t u;
    struct bdk_l2c_tadx_timeout_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t infolfb               : 1;  /**< [ 63: 63](RO/H) Logged address information is for the LFB original transation. */
        uint64_t infovab               : 1;  /**< [ 62: 62](RO/H) Logged address information is for the VAB (replacement). If both this and
                                                                 [INFOLFB] is set, then both could have timed out, but info captured is from the
                                                                 original LFB. */
        uint64_t reserved_57_61        : 5;
        uint64_t lfbnum                : 5;  /**< [ 56: 52](RO/H) The LFB number of the entry that timed out, and have its info captures in this register. */
        uint64_t cmd                   : 8;  /**< [ 51: 44](RO/H) Encoding of XMC or CCPI command causing error.
                                                                 Internal:
                                                                 If CMD\<7\>==1, use XMC_CMD_E to decode CMD\<6:0\>. If CMD\<7:5\>==0, use
                                                                 OCI_MREQ_CMD_E to
                                                                 decode CMD\<4:0\>. If CMD\<7:5\>==1, use OCI_MFWD_CMD_E to decode CMD\<4:0\>. If CMD\<7:5\>==2,
                                                                 use OCI_MRSP_CMD_E to decode CMD\<4:0\>. */
        uint64_t reserved_42_43        : 2;
        uint64_t node                  : 2;  /**< [ 41: 40](RO/H) Home node of the address causing the error. Similar to [ADDR] below, this can be the
                                                                 request address (if [INFOLFB] is set), else it is the replacement address (if [INFOLFB] is
                                                                 clear & [INFOVAB] is set). */
        uint64_t addr                  : 33; /**< [ 39:  7](RO/H) Cache line address causing the error. This can be either the request address or
                                                                 the replacement (if [INFOLFB] is set), else it is the replacement address (if
                                                                 [INFOLFB] is clear & [INFOVAB] is set). This address is a physical address. L2C
                                                                 performs index aliasing (if enabled) on the written address and uses that for
                                                                 the command. This index-aliased address is what is returned on a read of
                                                                 L2C_XMC_CMD. */
        uint64_t reserved_0_6          : 7;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_6          : 7;
        uint64_t addr                  : 33; /**< [ 39:  7](RO/H) Cache line address causing the error. This can be either the request address or
                                                                 the replacement (if [INFOLFB] is set), else it is the replacement address (if
                                                                 [INFOLFB] is clear & [INFOVAB] is set). This address is a physical address. L2C
                                                                 performs index aliasing (if enabled) on the written address and uses that for
                                                                 the command. This index-aliased address is what is returned on a read of
                                                                 L2C_XMC_CMD. */
        uint64_t node                  : 2;  /**< [ 41: 40](RO/H) Home node of the address causing the error. Similar to [ADDR] below, this can be the
                                                                 request address (if [INFOLFB] is set), else it is the replacement address (if [INFOLFB] is
                                                                 clear & [INFOVAB] is set). */
        uint64_t reserved_42_43        : 2;
        uint64_t cmd                   : 8;  /**< [ 51: 44](RO/H) Encoding of XMC or CCPI command causing error.
                                                                 Internal:
                                                                 If CMD\<7\>==1, use XMC_CMD_E to decode CMD\<6:0\>. If CMD\<7:5\>==0, use
                                                                 OCI_MREQ_CMD_E to
                                                                 decode CMD\<4:0\>. If CMD\<7:5\>==1, use OCI_MFWD_CMD_E to decode CMD\<4:0\>. If CMD\<7:5\>==2,
                                                                 use OCI_MRSP_CMD_E to decode CMD\<4:0\>. */
        uint64_t lfbnum                : 5;  /**< [ 56: 52](RO/H) The LFB number of the entry that timed out, and have its info captures in this register. */
        uint64_t reserved_57_61        : 5;
        uint64_t infovab               : 1;  /**< [ 62: 62](RO/H) Logged address information is for the VAB (replacement). If both this and
                                                                 [INFOLFB] is set, then both could have timed out, but info captured is from the
                                                                 original LFB. */
        uint64_t infolfb               : 1;  /**< [ 63: 63](RO/H) Logged address information is for the LFB original transation. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_tadx_timeout_s cn; */
};
typedef union bdk_l2c_tadx_timeout bdk_l2c_tadx_timeout_t;

static inline uint64_t BDK_L2C_TADX_TIMEOUT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_TIMEOUT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050050100ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050050100ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050050100ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_TIMEOUT", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_TIMEOUT(a) bdk_l2c_tadx_timeout_t
#define bustype_BDK_L2C_TADX_TIMEOUT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_TIMEOUT(a) "L2C_TADX_TIMEOUT"
#define device_bar_BDK_L2C_TADX_TIMEOUT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_TIMEOUT(a) (a)
#define arguments_BDK_L2C_TADX_TIMEOUT(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad#_timetwo
 *
 * L2C TAD LFB Timeout Count Registers
 * This register records the number of LFB entries that have timed out.
 */
union bdk_l2c_tadx_timetwo
{
    uint64_t u;
    struct bdk_l2c_tadx_timetwo_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_33_63        : 31;
        uint64_t sid                   : 4;  /**< [ 32: 29](RO/H) Source id of the original request, that is 'source' of request. This is only valid if the
                                                                 request is a local request (valid if L2C_TAD()_TIMEOUT[CMD] is an XMC request and not
                                                                 relevant if it is an CCPI request). */
        uint64_t busid                 : 4;  /**< [ 28: 25](RO/H) Busid of the original request, that is 'source' of request. */
        uint64_t vabst                 : 3;  /**< [ 24: 22](RO/H) This is the LFB internal state if INFOLFB is set, else will contain VAB internal state if
                                                                 INFOVAB is set. */
        uint64_t lfbst                 : 14; /**< [ 21:  8](RO/H) This is the LFB internal state if INFOLFB is set, else will contain VAB internal state if
                                                                 INFOVAB is set. */
        uint64_t tocnt                 : 8;  /**< [  7:  0](RO/H) This is a running count of the LFB that has timed out ... the count will saturate at 0xFF.
                                                                 Will clear when the LFBTO interrupt is cleared. */
#else /* Word 0 - Little Endian */
        uint64_t tocnt                 : 8;  /**< [  7:  0](RO/H) This is a running count of the LFB that has timed out ... the count will saturate at 0xFF.
                                                                 Will clear when the LFBTO interrupt is cleared. */
        uint64_t lfbst                 : 14; /**< [ 21:  8](RO/H) This is the LFB internal state if INFOLFB is set, else will contain VAB internal state if
                                                                 INFOVAB is set. */
        uint64_t vabst                 : 3;  /**< [ 24: 22](RO/H) This is the LFB internal state if INFOLFB is set, else will contain VAB internal state if
                                                                 INFOVAB is set. */
        uint64_t busid                 : 4;  /**< [ 28: 25](RO/H) Busid of the original request, that is 'source' of request. */
        uint64_t sid                   : 4;  /**< [ 32: 29](RO/H) Source id of the original request, that is 'source' of request. This is only valid if the
                                                                 request is a local request (valid if L2C_TAD()_TIMEOUT[CMD] is an XMC request and not
                                                                 relevant if it is an CCPI request). */
        uint64_t reserved_33_63        : 31;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_tadx_timetwo_s cn; */
};
typedef union bdk_l2c_tadx_timetwo bdk_l2c_tadx_timetwo_t;

static inline uint64_t BDK_L2C_TADX_TIMETWO(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_TIMETWO(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050050000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050050000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050050000ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_TIMETWO", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_TIMETWO(a) bdk_l2c_tadx_timetwo_t
#define bustype_BDK_L2C_TADX_TIMETWO(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_TIMETWO(a) "L2C_TADX_TIMETWO"
#define device_bar_BDK_L2C_TADX_TIMETWO(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_TIMETWO(a) (a)
#define arguments_BDK_L2C_TADX_TIMETWO(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad_ctl
 *
 * L2C TAD Control Register
 * In CNXXXX, [MAXLFB], EXLRQ, EXRRQ, EXFWD, EXVIC refer to half-TAD
 * LFBs/VABs. Therefore, even though there are 32 LFBs/VABs in a full TAD, the number
 * applies to both halves.
 * * If [MAXLFB] is != 0, [VBF_THRESH] should be less than [MAXLFB].
 * * If [MAXVBF] is != 0, [VBF_THRESH] should be less than [MAXVBF].
 * * If [MAXLFB] != 0, EXLRQ + EXRRQ + EXFWD + EXVIC must be less than or equal to [MAXLFB] - 4.
 * * If [MAXLFB] == 0, EXLRQ + EXRRQ + EXFWD + EXVIC must be less than or equal to 12.
 */
union bdk_l2c_tad_ctl
{
    uint64_t u;
    struct bdk_l2c_tad_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_33_63        : 31;
        uint64_t frcnalc               : 1;  /**< [ 32: 32](R/W) When set, all cache accesses are forced to not allocate in the local L2. */
        uint64_t disrstp               : 1;  /**< [ 31: 31](RO) Reserved. */
        uint64_t wtlmcwrdn             : 1;  /**< [ 30: 30](R/W) Be more conservative with LFB done relative to LMC writes. */
        uint64_t wtinvdn               : 1;  /**< [ 29: 29](R/W) Be more conservative with LFB done relative to invalidates. */
        uint64_t wtfilldn              : 1;  /**< [ 28: 28](R/W) Be more conservative with LFB done relative to fills. */
        uint64_t exlrq                 : 4;  /**< [ 27: 24](RO) Reserved. */
        uint64_t exrrq                 : 4;  /**< [ 23: 20](RO) Reserved. */
        uint64_t exfwd                 : 4;  /**< [ 19: 16](RO) Reserved. */
        uint64_t exvic                 : 4;  /**< [ 15: 12](RO) Reserved. */
        uint64_t vbf_thresh            : 4;  /**< [ 11:  8](R/W) VBF threshold. When the number of in-use VBFs exceeds this number the L2C TAD increases
                                                                 the priority of all its write operations in the LMC. */
        uint64_t maxvbf                : 4;  /**< [  7:  4](R/W) Maximum VBFs in use at once (0 means 16, 1-15 as expected). */
        uint64_t maxlfb                : 4;  /**< [  3:  0](R/W) Maximum VABs/LFBs in use at once (0 means 16, 1-15 as expected). */
#else /* Word 0 - Little Endian */
        uint64_t maxlfb                : 4;  /**< [  3:  0](R/W) Maximum VABs/LFBs in use at once (0 means 16, 1-15 as expected). */
        uint64_t maxvbf                : 4;  /**< [  7:  4](R/W) Maximum VBFs in use at once (0 means 16, 1-15 as expected). */
        uint64_t vbf_thresh            : 4;  /**< [ 11:  8](R/W) VBF threshold. When the number of in-use VBFs exceeds this number the L2C TAD increases
                                                                 the priority of all its write operations in the LMC. */
        uint64_t exvic                 : 4;  /**< [ 15: 12](RO) Reserved. */
        uint64_t exfwd                 : 4;  /**< [ 19: 16](RO) Reserved. */
        uint64_t exrrq                 : 4;  /**< [ 23: 20](RO) Reserved. */
        uint64_t exlrq                 : 4;  /**< [ 27: 24](RO) Reserved. */
        uint64_t wtfilldn              : 1;  /**< [ 28: 28](R/W) Be more conservative with LFB done relative to fills. */
        uint64_t wtinvdn               : 1;  /**< [ 29: 29](R/W) Be more conservative with LFB done relative to invalidates. */
        uint64_t wtlmcwrdn             : 1;  /**< [ 30: 30](R/W) Be more conservative with LFB done relative to LMC writes. */
        uint64_t disrstp               : 1;  /**< [ 31: 31](RO) Reserved. */
        uint64_t frcnalc               : 1;  /**< [ 32: 32](R/W) When set, all cache accesses are forced to not allocate in the local L2. */
        uint64_t reserved_33_63        : 31;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_tad_ctl_s cn81xx; */
    struct bdk_l2c_tad_ctl_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_33_63        : 31;
        uint64_t frcnalc               : 1;  /**< [ 32: 32](R/W) When set, all cache accesses are forced to not allocate in the local L2. */
        uint64_t disrstp               : 1;  /**< [ 31: 31](R/W) When set, if the L2 receives an RSTP XMC command, it treats it as a STP. */
        uint64_t wtlmcwrdn             : 1;  /**< [ 30: 30](R/W) Be more conservative with LFB done relative to LMC writes. */
        uint64_t wtinvdn               : 1;  /**< [ 29: 29](R/W) Be more conservative with LFB done relative to invalidates. */
        uint64_t wtfilldn              : 1;  /**< [ 28: 28](R/W) Be more conservative with LFB done relative to fills. */
        uint64_t exlrq                 : 4;  /**< [ 27: 24](R/W) Extra LFBs to reserve for locally generated XMC commands. None are reserved for functional
                                                                 correctness. Ignored if L2C_OCI_CTL[ENAOCI] is 0. */
        uint64_t exrrq                 : 4;  /**< [ 23: 20](R/W) Extra LFBs to reserve for Rxxx CCPI commands beyond the 1 required for CCPI protocol
                                                                 functional correctness. Ignored if L2C_OCI_CTL[ENAOCI] is 0. */
        uint64_t exfwd                 : 4;  /**< [ 19: 16](R/W) Extra LFBs to reserve for Fxxx/SINV CCPI commands beyond the 1 required for CCPI protocol
                                                                 functional correctness. Ignored if L2C_OCI_CTL[ENAOCI] is 0. */
        uint64_t exvic                 : 4;  /**< [ 15: 12](R/W) Extra LFBs to reserve for VICx CCPI commands beyond the 1 required for CCPI protocol
                                                                 functional correctness. Ignored if L2C_OCI_CTL[ENAOCI] is 0. */
        uint64_t vbf_thresh            : 4;  /**< [ 11:  8](R/W) VBF threshold. When the number of in-use VBFs exceeds this number the L2C TAD increases
                                                                 the priority of all its write operations in the LMC. */
        uint64_t maxvbf                : 4;  /**< [  7:  4](R/W) Maximum VBFs in use at once (0 means 16, 1-15 as expected). */
        uint64_t maxlfb                : 4;  /**< [  3:  0](R/W) Maximum VABs/LFBs in use at once (0 means 16, 1-15 as expected). */
#else /* Word 0 - Little Endian */
        uint64_t maxlfb                : 4;  /**< [  3:  0](R/W) Maximum VABs/LFBs in use at once (0 means 16, 1-15 as expected). */
        uint64_t maxvbf                : 4;  /**< [  7:  4](R/W) Maximum VBFs in use at once (0 means 16, 1-15 as expected). */
        uint64_t vbf_thresh            : 4;  /**< [ 11:  8](R/W) VBF threshold. When the number of in-use VBFs exceeds this number the L2C TAD increases
                                                                 the priority of all its write operations in the LMC. */
        uint64_t exvic                 : 4;  /**< [ 15: 12](R/W) Extra LFBs to reserve for VICx CCPI commands beyond the 1 required for CCPI protocol
                                                                 functional correctness. Ignored if L2C_OCI_CTL[ENAOCI] is 0. */
        uint64_t exfwd                 : 4;  /**< [ 19: 16](R/W) Extra LFBs to reserve for Fxxx/SINV CCPI commands beyond the 1 required for CCPI protocol
                                                                 functional correctness. Ignored if L2C_OCI_CTL[ENAOCI] is 0. */
        uint64_t exrrq                 : 4;  /**< [ 23: 20](R/W) Extra LFBs to reserve for Rxxx CCPI commands beyond the 1 required for CCPI protocol
                                                                 functional correctness. Ignored if L2C_OCI_CTL[ENAOCI] is 0. */
        uint64_t exlrq                 : 4;  /**< [ 27: 24](R/W) Extra LFBs to reserve for locally generated XMC commands. None are reserved for functional
                                                                 correctness. Ignored if L2C_OCI_CTL[ENAOCI] is 0. */
        uint64_t wtfilldn              : 1;  /**< [ 28: 28](R/W) Be more conservative with LFB done relative to fills. */
        uint64_t wtinvdn               : 1;  /**< [ 29: 29](R/W) Be more conservative with LFB done relative to invalidates. */
        uint64_t wtlmcwrdn             : 1;  /**< [ 30: 30](R/W) Be more conservative with LFB done relative to LMC writes. */
        uint64_t disrstp               : 1;  /**< [ 31: 31](R/W) When set, if the L2 receives an RSTP XMC command, it treats it as a STP. */
        uint64_t frcnalc               : 1;  /**< [ 32: 32](R/W) When set, all cache accesses are forced to not allocate in the local L2. */
        uint64_t reserved_33_63        : 31;
#endif /* Word 0 - End */
    } cn88xx;
    /* struct bdk_l2c_tad_ctl_s cn83xx; */
};
typedef union bdk_l2c_tad_ctl bdk_l2c_tad_ctl_t;

#define BDK_L2C_TAD_CTL BDK_L2C_TAD_CTL_FUNC()
static inline uint64_t BDK_L2C_TAD_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TAD_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e080800018ll;
    __bdk_csr_fatal("L2C_TAD_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_L2C_TAD_CTL bdk_l2c_tad_ctl_t
#define bustype_BDK_L2C_TAD_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TAD_CTL "L2C_TAD_CTL"
#define device_bar_BDK_L2C_TAD_CTL 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TAD_CTL 0
#define arguments_BDK_L2C_TAD_CTL -1,-1,-1,-1

/**
 * Register (RSL) l2c_wpar_iob#
 *
 * L2C IOB Way Partitioning Registers
 */
union bdk_l2c_wpar_iobx
{
    uint64_t u;
    struct bdk_l2c_wpar_iobx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t mask                  : 16; /**< [ 15:  0](R/W/H) Way partitioning mask (1 means do not use). The read value of [MASK] includes bits set
                                                                 because of the L2C cripple fuses. */
#else /* Word 0 - Little Endian */
        uint64_t mask                  : 16; /**< [ 15:  0](R/W/H) Way partitioning mask (1 means do not use). The read value of [MASK] includes bits set
                                                                 because of the L2C cripple fuses. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_wpar_iobx_s cn; */
};
typedef union bdk_l2c_wpar_iobx bdk_l2c_wpar_iobx_t;

static inline uint64_t BDK_L2C_WPAR_IOBX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_WPAR_IOBX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX) && (a<=15))
        return 0x87e080840200ll + 8ll * ((a) & 0xf);
    __bdk_csr_fatal("L2C_WPAR_IOBX", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_WPAR_IOBX(a) bdk_l2c_wpar_iobx_t
#define bustype_BDK_L2C_WPAR_IOBX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_WPAR_IOBX(a) "L2C_WPAR_IOBX"
#define device_bar_BDK_L2C_WPAR_IOBX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_WPAR_IOBX(a) (a)
#define arguments_BDK_L2C_WPAR_IOBX(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_wpar_pp#
 *
 * L2C Core Way Partitioning Registers
 */
union bdk_l2c_wpar_ppx
{
    uint64_t u;
    struct bdk_l2c_wpar_ppx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t mask                  : 16; /**< [ 15:  0](R/W/H) Way partitioning mask (1 means do not use). The read value of [MASK] includes bits set
                                                                 because of the L2C cripple fuses. */
#else /* Word 0 - Little Endian */
        uint64_t mask                  : 16; /**< [ 15:  0](R/W/H) Way partitioning mask (1 means do not use). The read value of [MASK] includes bits set
                                                                 because of the L2C cripple fuses. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_wpar_ppx_s cn; */
};
typedef union bdk_l2c_wpar_ppx bdk_l2c_wpar_ppx_t;

static inline uint64_t BDK_L2C_WPAR_PPX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_WPAR_PPX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e080840000ll + 8ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=23))
        return 0x87e080840000ll + 8ll * ((a) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=47))
        return 0x87e080840000ll + 8ll * ((a) & 0x3f);
    __bdk_csr_fatal("L2C_WPAR_PPX", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_WPAR_PPX(a) bdk_l2c_wpar_ppx_t
#define bustype_BDK_L2C_WPAR_PPX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_WPAR_PPX(a) "L2C_WPAR_PPX"
#define device_bar_BDK_L2C_WPAR_PPX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_WPAR_PPX(a) (a)
#define arguments_BDK_L2C_WPAR_PPX(a) (a),-1,-1,-1

#endif /* __BDK_CSRS_L2C_H__ */
