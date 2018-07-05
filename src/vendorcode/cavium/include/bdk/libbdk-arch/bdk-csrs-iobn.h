#ifndef __BDK_CSRS_IOBN_H__
#define __BDK_CSRS_IOBN_H__
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
 * Cavium IOBN.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration iobn_bar_e
 *
 * IOBN Base Address Register Enumeration
 * Enumerates the base address registers.
 */
#define BDK_IOBN_BAR_E_IOBNX_PF_BAR0_CN8(a) (0x87e0f0000000ll + 0x1000000ll * (a))
#define BDK_IOBN_BAR_E_IOBNX_PF_BAR0_CN8_SIZE 0x800000ull
#define BDK_IOBN_BAR_E_IOBNX_PF_BAR0_CN9(a) (0x87e0f0000000ll + 0x1000000ll * (a))
#define BDK_IOBN_BAR_E_IOBNX_PF_BAR0_CN9_SIZE 0x100000ull
#define BDK_IOBN_BAR_E_IOBNX_PF_BAR4(a) (0x87e0f0f00000ll + 0x1000000ll * (a))
#define BDK_IOBN_BAR_E_IOBNX_PF_BAR4_SIZE 0x100000ull

/**
 * Enumeration iobn_int_vec_e
 *
 * IOBN MSI-X Vector Enumeration
 * Enumerates the MSI-X interrupt vectors.
 */
#define BDK_IOBN_INT_VEC_E_INTS (0)
#define BDK_IOBN_INT_VEC_E_INTS1 (1)

/**
 * Enumeration iobn_ncbi_ro_mod_e
 *
 * IOBN NCBI Relax Order Modification Enumeration
 * Enumerates the controls for when CR's are allowed to pass PRs, see
 * IOBN()_ARBID()_CTL[CRPPR_ENA].
 */
#define BDK_IOBN_NCBI_RO_MOD_E_BUS_CTL (0)
#define BDK_IOBN_NCBI_RO_MOD_E_OFF (2)
#define BDK_IOBN_NCBI_RO_MOD_E_ON (3)
#define BDK_IOBN_NCBI_RO_MOD_E_RSVD (1)

/**
 * Enumeration iobn_psb_acc_e
 *
 * IOBN Power Serial Bus Accumulator Enumeration
 * Enumerates the IOB accumulators for IOB slaves, which correspond to index {b} of
 * PSBS_SYS()_ACCUM().
 */
#define BDK_IOBN_PSB_ACC_E_NCBI_RD_CMD_ACTIVE (0)
#define BDK_IOBN_PSB_ACC_E_NCBI_WR_CMD_ACTIVE (1)
#define BDK_IOBN_PSB_ACC_E_NCBO_CMD_ACTIVE (2)

/**
 * Enumeration iobn_psb_event_e
 *
 * IOBN Power Serial Bus Event Enumeration
 * Enumerates the event numbers for IOB slaves, which correspond to index {b} of
 * PSBS_SYS()_EVENT()_CFG.
 */
#define BDK_IOBN_PSB_EVENT_E_NCBI_CMD_ACTIVE_BUS0 (8)
#define BDK_IOBN_PSB_EVENT_E_NCBI_CMD_ACTIVE_BUS1 (9)
#define BDK_IOBN_PSB_EVENT_E_NCBI_CMD_ACTIVE_BUS2 (0xa)
#define BDK_IOBN_PSB_EVENT_E_NCBI_CMD_ACTIVE_BUS_RSV0 (0xb)
#define BDK_IOBN_PSB_EVENT_E_NCBI_DATA_ACTIVE_BUS0 (0xc)
#define BDK_IOBN_PSB_EVENT_E_NCBI_DATA_ACTIVE_BUS1 (0xd)
#define BDK_IOBN_PSB_EVENT_E_NCBI_DATA_ACTIVE_BUS2 (0xe)
#define BDK_IOBN_PSB_EVENT_E_NCBI_DATA_ACTIVE_BUS_RSV0 (0xf)
#define BDK_IOBN_PSB_EVENT_E_NCBI_RD_CMD_ACTIVE_BUS0 (0)
#define BDK_IOBN_PSB_EVENT_E_NCBI_RD_CMD_ACTIVE_BUS1 (1)
#define BDK_IOBN_PSB_EVENT_E_NCBI_RD_CMD_ACTIVE_BUS2 (2)
#define BDK_IOBN_PSB_EVENT_E_NCBI_RD_CMD_ACTIVE_BUS_RSV0 (3)
#define BDK_IOBN_PSB_EVENT_E_NCBI_WR_CMD_ACTIVE_BUS0 (4)
#define BDK_IOBN_PSB_EVENT_E_NCBI_WR_CMD_ACTIVE_BUS1 (5)
#define BDK_IOBN_PSB_EVENT_E_NCBI_WR_CMD_ACTIVE_BUS2 (6)
#define BDK_IOBN_PSB_EVENT_E_NCBI_WR_CMD_ACTIVE_BUS_RSV0 (7)
#define BDK_IOBN_PSB_EVENT_E_NCBO_CMD_ACTIVE_BUS0 (0x10)
#define BDK_IOBN_PSB_EVENT_E_NCBO_CMD_ACTIVE_BUS1 (0x11)
#define BDK_IOBN_PSB_EVENT_E_NCBO_CMD_ACTIVE_BUS2 (0x12)
#define BDK_IOBN_PSB_EVENT_E_NCBO_CMD_ACTIVE_BUS_RSV0 (0x13)
#define BDK_IOBN_PSB_EVENT_E_NCBO_DATA_ACTIVE_BUS0 (0x14)
#define BDK_IOBN_PSB_EVENT_E_NCBO_DATA_ACTIVE_BUS1 (0x15)
#define BDK_IOBN_PSB_EVENT_E_NCBO_DATA_ACTIVE_BUS2 (0x16)
#define BDK_IOBN_PSB_EVENT_E_NCBO_DATA_ACTIVE_BUS_RSV0 (0x17)

/**
 * Register (RSL) iobn#_arbid#_ctl
 *
 * IOBN NCB Constant Registers
 * This register set properties for each of the flat ARBIDs.
 */
union bdk_iobnx_arbidx_ctl
{
    uint64_t u;
    struct bdk_iobnx_arbidx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t fast_ord              : 1;  /**< [  9:  9](R/W) When set IOB will send commits for PR's to IOW as soon as PR is sent to mesh,
                                                                 rather than waiting for ordering. This bit should only be set for non-PEM
                                                                 devices */
        uint64_t sow_dis               : 1;  /**< [  8:  8](R/W) Disables the PCIe store widget for memory store performance. Does not affect
                                                                 observable ordering. No impact on IO stores.  For diagnostic use only.
                                                                 0 = Performance optimization on. Issue prefetches on stores to improve
                                                                 store-store ordering.
                                                                 1 = Performance optimization off. No prefetches.

                                                                 Internal:
                                                                 The SOW is only available on the NCB2/256b devices which include PEMs, CPT,
                                                                 DPI. The expectation is that CPT and DPI use the RelaxOrder bit so they will
                                                                 only use the widget when the VA address CAM detects and promotes two
                                                                 transactions to the same memory cacheline. */
        uint64_t crppr_ena             : 2;  /**< [  7:  6](R/W) Controls when a CR is allowed to pass a PR for NCBO and NCBI. Enumerated by IOBN_NCBI_RO_MOD_E. */
        uint64_t prefetch_dis          : 1;  /**< [  5:  5](R/W) Disables mesh prefetches. For diagnostic use only.
                                                                 0 = Store-store ordered transactions will issue prefetches before the second
                                                                 store to improve performance.
                                                                 1 = No prefetches. */
        uint64_t pr_iova_dis           : 1;  /**< [  4:  4](R/W) PR queue IOVA comparison disable. For diagnostic use only.
                                                                 0 = PR will not pass a younger PR with the same IOVA.
                                                                 1 = PR may pass a younger PR with the same IOVA, if the relaxed ordering request
                                                                 and [RO_DIS] bit allow it. */
        uint64_t ro_dis                : 1;  /**< [  3:  3](R/W) Disable relaxed ordering. For diagnostic use only.
                                                                 0 = Relaxed ordering is performed if the NCB device requests it.
                                                                 1 = IOB ignores the relaxed ordering request bit and treats all requests as
                                                                 strictly ordered. */
        uint64_t st_ld_ord             : 1;  /**< [  2:  2](R/W) If enabled, NP queue loaded in order it arrives from NCBI. [ST_ST_ORD] should also be
                                                                 set when this bit is set. NP queue will not pass PS queue.

                                                                 Internal:
                                                                 FIXME check meaning */
        uint64_t st_st_ord             : 1;  /**< [  1:  1](R/W) If enabled, PS queue used (ignore RO bit). Placed in order store arrives.
                                                                 Internal:
                                                                 FIXME check meaning */
        uint64_t ld_ld_ord             : 1;  /**< [  0:  0](R/W) Load-load ordering. For diagnostic use only.
                                                                 0 = NPR may pass NPR under some cases. The ordering is based on SMMU completion
                                                                 ordering.
                                                                 1 = NPR never passes NPR; the NPR ordering is based strictly on NCB arrival order.
                                                                 This may harm performance. */
#else /* Word 0 - Little Endian */
        uint64_t ld_ld_ord             : 1;  /**< [  0:  0](R/W) Load-load ordering. For diagnostic use only.
                                                                 0 = NPR may pass NPR under some cases. The ordering is based on SMMU completion
                                                                 ordering.
                                                                 1 = NPR never passes NPR; the NPR ordering is based strictly on NCB arrival order.
                                                                 This may harm performance. */
        uint64_t st_st_ord             : 1;  /**< [  1:  1](R/W) If enabled, PS queue used (ignore RO bit). Placed in order store arrives.
                                                                 Internal:
                                                                 FIXME check meaning */
        uint64_t st_ld_ord             : 1;  /**< [  2:  2](R/W) If enabled, NP queue loaded in order it arrives from NCBI. [ST_ST_ORD] should also be
                                                                 set when this bit is set. NP queue will not pass PS queue.

                                                                 Internal:
                                                                 FIXME check meaning */
        uint64_t ro_dis                : 1;  /**< [  3:  3](R/W) Disable relaxed ordering. For diagnostic use only.
                                                                 0 = Relaxed ordering is performed if the NCB device requests it.
                                                                 1 = IOB ignores the relaxed ordering request bit and treats all requests as
                                                                 strictly ordered. */
        uint64_t pr_iova_dis           : 1;  /**< [  4:  4](R/W) PR queue IOVA comparison disable. For diagnostic use only.
                                                                 0 = PR will not pass a younger PR with the same IOVA.
                                                                 1 = PR may pass a younger PR with the same IOVA, if the relaxed ordering request
                                                                 and [RO_DIS] bit allow it. */
        uint64_t prefetch_dis          : 1;  /**< [  5:  5](R/W) Disables mesh prefetches. For diagnostic use only.
                                                                 0 = Store-store ordered transactions will issue prefetches before the second
                                                                 store to improve performance.
                                                                 1 = No prefetches. */
        uint64_t crppr_ena             : 2;  /**< [  7:  6](R/W) Controls when a CR is allowed to pass a PR for NCBO and NCBI. Enumerated by IOBN_NCBI_RO_MOD_E. */
        uint64_t sow_dis               : 1;  /**< [  8:  8](R/W) Disables the PCIe store widget for memory store performance. Does not affect
                                                                 observable ordering. No impact on IO stores.  For diagnostic use only.
                                                                 0 = Performance optimization on. Issue prefetches on stores to improve
                                                                 store-store ordering.
                                                                 1 = Performance optimization off. No prefetches.

                                                                 Internal:
                                                                 The SOW is only available on the NCB2/256b devices which include PEMs, CPT,
                                                                 DPI. The expectation is that CPT and DPI use the RelaxOrder bit so they will
                                                                 only use the widget when the VA address CAM detects and promotes two
                                                                 transactions to the same memory cacheline. */
        uint64_t fast_ord              : 1;  /**< [  9:  9](R/W) When set IOB will send commits for PR's to IOW as soon as PR is sent to mesh,
                                                                 rather than waiting for ordering. This bit should only be set for non-PEM
                                                                 devices */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_arbidx_ctl_s cn; */
};
typedef union bdk_iobnx_arbidx_ctl bdk_iobnx_arbidx_ctl_t;

static inline uint64_t BDK_IOBNX_ARBIDX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_ARBIDX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=19)))
        return 0x87e0f0002100ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x1f);
    __bdk_csr_fatal("IOBNX_ARBIDX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_ARBIDX_CTL(a,b) bdk_iobnx_arbidx_ctl_t
#define bustype_BDK_IOBNX_ARBIDX_CTL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_ARBIDX_CTL(a,b) "IOBNX_ARBIDX_CTL"
#define device_bar_BDK_IOBNX_ARBIDX_CTL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_ARBIDX_CTL(a,b) (a)
#define arguments_BDK_IOBNX_ARBIDX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_bistr_reg
 *
 * IOBN BIST Status Register
 * This register contains the result of the BIST run on the IOB rclk memories.
 */
union bdk_iobnx_bistr_reg
{
    uint64_t u;
    struct bdk_iobnx_bistr_reg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_22_63        : 42;
        uint64_t status                : 22; /**< [ 21:  0](RO/H) Memory BIST status.
                                                                 Internal:
                                                                 \<18\> = gmr_ixofifo_bstatus_rclk.
                                                                   \<17\> = sli_preq_2_ffifo_bstatus_rclk.
                                                                   \<16\> = sli_req_2_ffifo_bstatus_rclk.
                                                                   \<15\> = sli_preq_1_ffifo_bstatus_rclk.
                                                                   \<14\> = sli_req_1_ffifo_bstatus_rclk.
                                                                   \<13\> = sli_preq_0_ffifo_bstatus_rclk.
                                                                   \<12\> = sli_req_0_ffifo_bstatus_rclk.
                                                                   \<11\> = iop_ffifo_bstatus_rclk.
                                                                   \<10\> = ixo_icc_fifo0_bstatus_rclk.
                                                                   \<9\> = ixo_icc_fifo1_bstatus_rclk.
                                                                   \<8\>  = ixo_ics_mem_bstatus_rclk.
                                                                   \<7\>  = iob_mem_data_xmd0_bstatus_rclk.
                                                                   \<6\>  = iob_mem_data_xmd1_bstatus_rclk.
                                                                   \<5\>  = ics_cmd_fifo_bstatus_rclk.
                                                                   \<4\>  = ixo_xmd_mem0_bstatus_rclk.
                                                                   \<3\>  = ixo_xmd_mem1_bstatus_rclk.
                                                                   \<2\>  = iobn_iorn_ffifo0_bstatus_rclk.
                                                                   \<1\>  = iobn_iorn_ffifo1_bstatus_rclk.
                                                                   \<0\>  = ixo_smmu_mem0_bstatus_rclk. */
#else /* Word 0 - Little Endian */
        uint64_t status                : 22; /**< [ 21:  0](RO/H) Memory BIST status.
                                                                 Internal:
                                                                 \<18\> = gmr_ixofifo_bstatus_rclk.
                                                                   \<17\> = sli_preq_2_ffifo_bstatus_rclk.
                                                                   \<16\> = sli_req_2_ffifo_bstatus_rclk.
                                                                   \<15\> = sli_preq_1_ffifo_bstatus_rclk.
                                                                   \<14\> = sli_req_1_ffifo_bstatus_rclk.
                                                                   \<13\> = sli_preq_0_ffifo_bstatus_rclk.
                                                                   \<12\> = sli_req_0_ffifo_bstatus_rclk.
                                                                   \<11\> = iop_ffifo_bstatus_rclk.
                                                                   \<10\> = ixo_icc_fifo0_bstatus_rclk.
                                                                   \<9\> = ixo_icc_fifo1_bstatus_rclk.
                                                                   \<8\>  = ixo_ics_mem_bstatus_rclk.
                                                                   \<7\>  = iob_mem_data_xmd0_bstatus_rclk.
                                                                   \<6\>  = iob_mem_data_xmd1_bstatus_rclk.
                                                                   \<5\>  = ics_cmd_fifo_bstatus_rclk.
                                                                   \<4\>  = ixo_xmd_mem0_bstatus_rclk.
                                                                   \<3\>  = ixo_xmd_mem1_bstatus_rclk.
                                                                   \<2\>  = iobn_iorn_ffifo0_bstatus_rclk.
                                                                   \<1\>  = iobn_iorn_ffifo1_bstatus_rclk.
                                                                   \<0\>  = ixo_smmu_mem0_bstatus_rclk. */
        uint64_t reserved_22_63        : 42;
#endif /* Word 0 - End */
    } s;
    struct bdk_iobnx_bistr_reg_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_19_63        : 45;
        uint64_t status                : 19; /**< [ 18:  0](RO/H) Memory BIST status.
                                                                 Internal:
                                                                 \<18\> = gmr_ixofifo_bstatus_rclk.
                                                                   \<17\> = sli_preq_2_ffifo_bstatus_rclk.
                                                                   \<16\> = sli_req_2_ffifo_bstatus_rclk.
                                                                   \<15\> = sli_preq_1_ffifo_bstatus_rclk.
                                                                   \<14\> = sli_req_1_ffifo_bstatus_rclk.
                                                                   \<13\> = sli_preq_0_ffifo_bstatus_rclk.
                                                                   \<12\> = sli_req_0_ffifo_bstatus_rclk.
                                                                   \<11\> = iop_ffifo_bstatus_rclk.
                                                                   \<10\> = ixo_icc_fifo0_bstatus_rclk.
                                                                   \<9\> = ixo_icc_fifo1_bstatus_rclk.
                                                                   \<8\>  = ixo_ics_mem_bstatus_rclk.
                                                                   \<7\>  = iob_mem_data_xmd0_bstatus_rclk.
                                                                   \<6\>  = 0 unused.
                                                                   \<5\>  = ics_cmd_fifo_bstatus_rclk.
                                                                   \<4\>  = ixo_xmd_mem0_bstatus_rclk.
                                                                   \<3\>  = ixo_xmd_mem1_bstatus_rclk.
                                                                   \<2\>  = iobn_iorn_ffifo0_bstatus_rclk.
                                                                   \<1\>  = iobn_iorn_ffifo1_bstatus_rclk.
                                                                   \<0\>  = ixo_smmu_mem0_bstatus_rclk. */
#else /* Word 0 - Little Endian */
        uint64_t status                : 19; /**< [ 18:  0](RO/H) Memory BIST status.
                                                                 Internal:
                                                                 \<18\> = gmr_ixofifo_bstatus_rclk.
                                                                   \<17\> = sli_preq_2_ffifo_bstatus_rclk.
                                                                   \<16\> = sli_req_2_ffifo_bstatus_rclk.
                                                                   \<15\> = sli_preq_1_ffifo_bstatus_rclk.
                                                                   \<14\> = sli_req_1_ffifo_bstatus_rclk.
                                                                   \<13\> = sli_preq_0_ffifo_bstatus_rclk.
                                                                   \<12\> = sli_req_0_ffifo_bstatus_rclk.
                                                                   \<11\> = iop_ffifo_bstatus_rclk.
                                                                   \<10\> = ixo_icc_fifo0_bstatus_rclk.
                                                                   \<9\> = ixo_icc_fifo1_bstatus_rclk.
                                                                   \<8\>  = ixo_ics_mem_bstatus_rclk.
                                                                   \<7\>  = iob_mem_data_xmd0_bstatus_rclk.
                                                                   \<6\>  = 0 unused.
                                                                   \<5\>  = ics_cmd_fifo_bstatus_rclk.
                                                                   \<4\>  = ixo_xmd_mem0_bstatus_rclk.
                                                                   \<3\>  = ixo_xmd_mem1_bstatus_rclk.
                                                                   \<2\>  = iobn_iorn_ffifo0_bstatus_rclk.
                                                                   \<1\>  = iobn_iorn_ffifo1_bstatus_rclk.
                                                                   \<0\>  = ixo_smmu_mem0_bstatus_rclk. */
        uint64_t reserved_19_63        : 45;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_iobnx_bistr_reg_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_19_63        : 45;
        uint64_t status                : 19; /**< [ 18:  0](RO/H) Memory BIST status.
                                                                 Internal:
                                                                 \<18\> = gmr_ixofifo_bstatus_rclk.
                                                                   \<17\> = sli_preq_2_ffifo_bstatus_rclk.
                                                                   \<16\> = sli_req_2_ffifo_bstatus_rclk.
                                                                   \<15\> = sli_preq_1_ffifo_bstatus_rclk.
                                                                   \<14\> = sli_req_1_ffifo_bstatus_rclk.
                                                                   \<13\> = sli_preq_0_ffifo_bstatus_rclk.
                                                                   \<12\> = sli_req_0_ffifo_bstatus_rclk.
                                                                   \<11\> = iop_ffifo_bstatus_rclk.
                                                                   \<10\> = ixo_icc_fifo0_bstatus_rclk.
                                                                   \<9\> = ixo_icc_fifo1_bstatus_rclk.
                                                                   \<8\>  = ixo_ics_mem_bstatus_rclk.
                                                                   \<7\>  = iob_mem_data_xmd0_bstatus_rclk.
                                                                   \<6\>  = iob_mem_data_xmd1_bstatus_rclk.
                                                                   \<5\>  = ics_cmd_fifo_bstatus_rclk.
                                                                   \<4\>  = ixo_xmd_mem0_bstatus_rclk.
                                                                   \<3\>  = ixo_xmd_mem1_bstatus_rclk.
                                                                   \<2\>  = iobn_iorn_ffifo0_bstatus_rclk.
                                                                   \<1\>  = iobn_iorn_ffifo1_bstatus_rclk.
                                                                   \<0\>  = ixo_smmu_mem0_bstatus_rclk. */
#else /* Word 0 - Little Endian */
        uint64_t status                : 19; /**< [ 18:  0](RO/H) Memory BIST status.
                                                                 Internal:
                                                                 \<18\> = gmr_ixofifo_bstatus_rclk.
                                                                   \<17\> = sli_preq_2_ffifo_bstatus_rclk.
                                                                   \<16\> = sli_req_2_ffifo_bstatus_rclk.
                                                                   \<15\> = sli_preq_1_ffifo_bstatus_rclk.
                                                                   \<14\> = sli_req_1_ffifo_bstatus_rclk.
                                                                   \<13\> = sli_preq_0_ffifo_bstatus_rclk.
                                                                   \<12\> = sli_req_0_ffifo_bstatus_rclk.
                                                                   \<11\> = iop_ffifo_bstatus_rclk.
                                                                   \<10\> = ixo_icc_fifo0_bstatus_rclk.
                                                                   \<9\> = ixo_icc_fifo1_bstatus_rclk.
                                                                   \<8\>  = ixo_ics_mem_bstatus_rclk.
                                                                   \<7\>  = iob_mem_data_xmd0_bstatus_rclk.
                                                                   \<6\>  = iob_mem_data_xmd1_bstatus_rclk.
                                                                   \<5\>  = ics_cmd_fifo_bstatus_rclk.
                                                                   \<4\>  = ixo_xmd_mem0_bstatus_rclk.
                                                                   \<3\>  = ixo_xmd_mem1_bstatus_rclk.
                                                                   \<2\>  = iobn_iorn_ffifo0_bstatus_rclk.
                                                                   \<1\>  = iobn_iorn_ffifo1_bstatus_rclk.
                                                                   \<0\>  = ixo_smmu_mem0_bstatus_rclk. */
        uint64_t reserved_19_63        : 45;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_iobnx_bistr_reg_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_22_63        : 42;
        uint64_t status                : 22; /**< [ 21:  0](RO/H) Memory BIST status.
                                                                 Internal:
                                                                 \<21\> = gmr_sli_ixofifo_bstatus_rclk.
                                                                 \<20\> = sli_preq_2_ffifo_bstatus_rclk.
                                                                 \<19\> = sli_req_2_ffifo_bstatus_rclk.
                                                                 \<18\> = gmr_ixofifo_bstatus_rclk.
                                                                   \<17\> = sli_preq_2_ffifo_bstatus_rclk.
                                                                   \<16\> = sli_req_2_ffifo_bstatus_rclk.
                                                                   \<15\> = sli_preq_1_ffifo_bstatus_rclk.
                                                                   \<14\> = sli_req_1_ffifo_bstatus_rclk.
                                                                   \<13\> = sli_preq_0_ffifo_bstatus_rclk.
                                                                   \<12\> = sli_req_0_ffifo_bstatus_rclk.
                                                                   \<11\> = iop_ffifo_bstatus_rclk.
                                                                   \<10\> = ixo_icc_fifo0_bstatus_rclk.
                                                                   \<9\> = ixo_icc_fifo1_bstatus_rclk.
                                                                   \<8\>  = ixo_ics_mem_bstatus_rclk.
                                                                   \<7\>  = iob_mem_data_xmd0_bstatus_rclk.
                                                                   \<6\>  = iob_mem_data_xmd1_bstatus_rclk.
                                                                   \<5\>  = ics_cmd_fifo_bstatus_rclk.
                                                                   \<4\>  = ixo_xmd_mem0_bstatus_rclk.
                                                                   \<3\>  = ixo_xmd_mem1_bstatus_rclk.
                                                                   \<2\>  = iobn_iorn_ffifo0_bstatus_rclk.
                                                                   \<1\>  = iobn_iorn_ffifo1_bstatus_rclk.
                                                                   \<0\>  = ixo_smmu_mem0_bstatus_rclk. */
#else /* Word 0 - Little Endian */
        uint64_t status                : 22; /**< [ 21:  0](RO/H) Memory BIST status.
                                                                 Internal:
                                                                 \<21\> = gmr_sli_ixofifo_bstatus_rclk.
                                                                 \<20\> = sli_preq_2_ffifo_bstatus_rclk.
                                                                 \<19\> = sli_req_2_ffifo_bstatus_rclk.
                                                                 \<18\> = gmr_ixofifo_bstatus_rclk.
                                                                   \<17\> = sli_preq_2_ffifo_bstatus_rclk.
                                                                   \<16\> = sli_req_2_ffifo_bstatus_rclk.
                                                                   \<15\> = sli_preq_1_ffifo_bstatus_rclk.
                                                                   \<14\> = sli_req_1_ffifo_bstatus_rclk.
                                                                   \<13\> = sli_preq_0_ffifo_bstatus_rclk.
                                                                   \<12\> = sli_req_0_ffifo_bstatus_rclk.
                                                                   \<11\> = iop_ffifo_bstatus_rclk.
                                                                   \<10\> = ixo_icc_fifo0_bstatus_rclk.
                                                                   \<9\> = ixo_icc_fifo1_bstatus_rclk.
                                                                   \<8\>  = ixo_ics_mem_bstatus_rclk.
                                                                   \<7\>  = iob_mem_data_xmd0_bstatus_rclk.
                                                                   \<6\>  = iob_mem_data_xmd1_bstatus_rclk.
                                                                   \<5\>  = ics_cmd_fifo_bstatus_rclk.
                                                                   \<4\>  = ixo_xmd_mem0_bstatus_rclk.
                                                                   \<3\>  = ixo_xmd_mem1_bstatus_rclk.
                                                                   \<2\>  = iobn_iorn_ffifo0_bstatus_rclk.
                                                                   \<1\>  = iobn_iorn_ffifo1_bstatus_rclk.
                                                                   \<0\>  = ixo_smmu_mem0_bstatus_rclk. */
        uint64_t reserved_22_63        : 42;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_iobnx_bistr_reg bdk_iobnx_bistr_reg_t;

static inline uint64_t BDK_IOBNX_BISTR_REG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_BISTR_REG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f0005080ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f0005080ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f0005080ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_BISTR_REG", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_BISTR_REG(a) bdk_iobnx_bistr_reg_t
#define bustype_BDK_IOBNX_BISTR_REG(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_BISTR_REG(a) "IOBNX_BISTR_REG"
#define device_bar_BDK_IOBNX_BISTR_REG(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_BISTR_REG(a) (a)
#define arguments_BDK_IOBNX_BISTR_REG(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_bists_reg
 *
 * IOBN BIST Status Register
 * This register contains the result of the BIST run on the IOB sclk memories.
 */
union bdk_iobnx_bists_reg
{
    uint64_t u;
    struct bdk_iobnx_bists_reg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_11_63        : 53;
        uint64_t status                : 11; /**< [ 10:  0](RO/H) Memory BIST status.
                                                                 Internal:
                                                                 \<10\> = irp0_flid_mem_status.
                                                                   \<9\>  = irp1_flid_mem_status.
                                                                   \<8\>  = icc0_xmc_fifo_ecc_bstatus.
                                                                   \<7\>  = icc1_xmc_fifo_ecc_bstatus.
                                                                   \<6\>  = icc_xmc_fifo_ecc_bstatus.
                                                                   \<5\>  = rsd_mem0_bstatus.
                                                                   \<4\>  = rsd_mem1_bstatus.
                                                                   \<3\>  = iop_breq_fifo0_bstatus.
                                                                   \<2\>  = iop_breq_fifo1_bstatus.
                                                                   \<1\>  = iop_breq_fifo2_bstatus.
                                                                   \<0\>  = iop_breq_fifo3_bstatus. */
#else /* Word 0 - Little Endian */
        uint64_t status                : 11; /**< [ 10:  0](RO/H) Memory BIST status.
                                                                 Internal:
                                                                 \<10\> = irp0_flid_mem_status.
                                                                   \<9\>  = irp1_flid_mem_status.
                                                                   \<8\>  = icc0_xmc_fifo_ecc_bstatus.
                                                                   \<7\>  = icc1_xmc_fifo_ecc_bstatus.
                                                                   \<6\>  = icc_xmc_fifo_ecc_bstatus.
                                                                   \<5\>  = rsd_mem0_bstatus.
                                                                   \<4\>  = rsd_mem1_bstatus.
                                                                   \<3\>  = iop_breq_fifo0_bstatus.
                                                                   \<2\>  = iop_breq_fifo1_bstatus.
                                                                   \<1\>  = iop_breq_fifo2_bstatus.
                                                                   \<0\>  = iop_breq_fifo3_bstatus. */
        uint64_t reserved_11_63        : 53;
#endif /* Word 0 - End */
    } s;
    struct bdk_iobnx_bists_reg_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_11_63        : 53;
        uint64_t status                : 11; /**< [ 10:  0](RO/H) Memory BIST status.
                                                                 Internal:
                                                                 \<10\> = irp0_flid_mem_status.
                                                                   \<9\>  = 0.
                                                                   \<8\>  = icc0_xmc_fifo_ecc_bstatus.
                                                                   \<7\>  = 0 unused.
                                                                   \<6\>  = icc_xmc_fifo_ecc_bstatus.
                                                                   \<5\>  = rsd_mem0_bstatus.
                                                                   \<4\>  = 0 un used
                                                                   \<3\>  = iop_breq_fifo0_bstatus.
                                                                   \<2\>  = 0 Unused
                                                                   \<1\>  = iop_breq_fifo2_bstatus.
                                                                   \<0\>  = iop_breq_fifo3_bstatus. */
#else /* Word 0 - Little Endian */
        uint64_t status                : 11; /**< [ 10:  0](RO/H) Memory BIST status.
                                                                 Internal:
                                                                 \<10\> = irp0_flid_mem_status.
                                                                   \<9\>  = 0.
                                                                   \<8\>  = icc0_xmc_fifo_ecc_bstatus.
                                                                   \<7\>  = 0 unused.
                                                                   \<6\>  = icc_xmc_fifo_ecc_bstatus.
                                                                   \<5\>  = rsd_mem0_bstatus.
                                                                   \<4\>  = 0 un used
                                                                   \<3\>  = iop_breq_fifo0_bstatus.
                                                                   \<2\>  = 0 Unused
                                                                   \<1\>  = iop_breq_fifo2_bstatus.
                                                                   \<0\>  = iop_breq_fifo3_bstatus. */
        uint64_t reserved_11_63        : 53;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_iobnx_bists_reg_s cn88xx; */
    /* struct bdk_iobnx_bists_reg_s cn83xx; */
};
typedef union bdk_iobnx_bists_reg bdk_iobnx_bists_reg_t;

static inline uint64_t BDK_IOBNX_BISTS_REG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_BISTS_REG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f0005000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f0005000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f0005000ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_BISTS_REG", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_BISTS_REG(a) bdk_iobnx_bists_reg_t
#define bustype_BDK_IOBNX_BISTS_REG(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_BISTS_REG(a) "IOBNX_BISTS_REG"
#define device_bar_BDK_IOBNX_BISTS_REG(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_BISTS_REG(a) (a)
#define arguments_BDK_IOBNX_BISTS_REG(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_bp_test#
 *
 * INTERNAL: IOBN Backpressure Test Registers
 */
union bdk_iobnx_bp_testx
{
    uint64_t u;
    struct bdk_iobnx_bp_testx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t enable                : 8;  /**< [ 63: 56](R/W) Enable test mode. For diagnostic use only.
                                                                 Internal:
                                                                 Once a bit is set, random backpressure is generated
                                                                 at the corresponding point to allow for more frequent backpressure.

                                                                 IOBN()_BP_TEST(0) - INRF: Defined by iobn_defs::inrf_bp_test_t
                                                                 \<63\> = TBD.
                                                                 \<62\> = TBD.
                                                                 \<61\> = TBD.
                                                                 \<60\> = TBD.
                                                                 \<59\> = TBD.
                                                                 \<58\> = TBD.
                                                                 \<57\> = TBD.
                                                                 \<56\> = TBD.

                                                                 IOBN()_BP_TEST(1) - INRM: Defined by iobn_defs::inrm_bp_test_t
                                                                 \<63\> = Stall CMT processing for outbound LBK transactions
                                                                 \<62\> = Stall CMT processing for outbound MSH transactions
                                                                 \<61\> = omp_vcc_ret.
                                                                 \<60\> = imi_dat_fif - Backpressure VCC return counters(OMP)
                                                                 \<59\> = TBD.
                                                                 \<58\> = TBD.
                                                                 \<57\> = TBD.
                                                                 \<56\> = TBD.

                                                                 IOBN()_BP_TEST(2) - INRF: Defined by iobn_defs::inrf_bp_test_t
                                                                 \<63\> = TBD.
                                                                 \<62\> = TBD.
                                                                 \<61\> = TBD.
                                                                 \<60\> = TBD.
                                                                 \<59\> = TBD.
                                                                 \<58\> = TBD.
                                                                 \<57\> = TBD.
                                                                 \<56\> = TBD.

                                                                 IOBN()_BP_TEST(3) - INRF: Defined by iobn_defs::inrm_bp_test_t
                                                                 \<63\> = VCC - Victim DAT.
                                                                 \<62\> = VCC - Victim REQ (CMD).
                                                                 \<61\> = VCC - DAT (REQ/REQH).
                                                                 \<60\> = VCC - CMD (REQ/RQH).
                                                                 \<59\> = SLC - VCC.
                                                                 \<58\> = SLC - ACK.
                                                                 \<57\> = SLC - DAT.
                                                                 \<56\> = SLC - CMD. */
        uint64_t reserved_32_55        : 24;
        uint64_t bp_cfg                : 16; /**< [ 31: 16](R/W) Backpressure weight. For diagnostic use only.
                                                                 Internal:
                                                                 There are 2 backpressure configuration bits per enable, with the two bits
                                                                 defined as 0x0=100% of the time, 0x1=75% of the time, 0x2=50% of the time,
                                                                 0x3=25% of the time.
                                                                   \<31:30\> = Config 7.
                                                                   \<29:28\> = Config 6.
                                                                   \<27:26\> = Config 5.
                                                                   \<25:24\> = Config 4.
                                                                   \<23:22\> = Config 3.
                                                                   \<21:20\> = Config 2.
                                                                   \<19:18\> = Config 1.
                                                                   \<17:16\> = Config 0. */
        uint64_t reserved_12_15        : 4;
        uint64_t lfsr_freq             : 12; /**< [ 11:  0](R/W) Test LFSR update frequency in coprocessor-clocks minus one. */
#else /* Word 0 - Little Endian */
        uint64_t lfsr_freq             : 12; /**< [ 11:  0](R/W) Test LFSR update frequency in coprocessor-clocks minus one. */
        uint64_t reserved_12_15        : 4;
        uint64_t bp_cfg                : 16; /**< [ 31: 16](R/W) Backpressure weight. For diagnostic use only.
                                                                 Internal:
                                                                 There are 2 backpressure configuration bits per enable, with the two bits
                                                                 defined as 0x0=100% of the time, 0x1=75% of the time, 0x2=50% of the time,
                                                                 0x3=25% of the time.
                                                                   \<31:30\> = Config 7.
                                                                   \<29:28\> = Config 6.
                                                                   \<27:26\> = Config 5.
                                                                   \<25:24\> = Config 4.
                                                                   \<23:22\> = Config 3.
                                                                   \<21:20\> = Config 2.
                                                                   \<19:18\> = Config 1.
                                                                   \<17:16\> = Config 0. */
        uint64_t reserved_32_55        : 24;
        uint64_t enable                : 8;  /**< [ 63: 56](R/W) Enable test mode. For diagnostic use only.
                                                                 Internal:
                                                                 Once a bit is set, random backpressure is generated
                                                                 at the corresponding point to allow for more frequent backpressure.

                                                                 IOBN()_BP_TEST(0) - INRF: Defined by iobn_defs::inrf_bp_test_t
                                                                 \<63\> = TBD.
                                                                 \<62\> = TBD.
                                                                 \<61\> = TBD.
                                                                 \<60\> = TBD.
                                                                 \<59\> = TBD.
                                                                 \<58\> = TBD.
                                                                 \<57\> = TBD.
                                                                 \<56\> = TBD.

                                                                 IOBN()_BP_TEST(1) - INRM: Defined by iobn_defs::inrm_bp_test_t
                                                                 \<63\> = Stall CMT processing for outbound LBK transactions
                                                                 \<62\> = Stall CMT processing for outbound MSH transactions
                                                                 \<61\> = omp_vcc_ret.
                                                                 \<60\> = imi_dat_fif - Backpressure VCC return counters(OMP)
                                                                 \<59\> = TBD.
                                                                 \<58\> = TBD.
                                                                 \<57\> = TBD.
                                                                 \<56\> = TBD.

                                                                 IOBN()_BP_TEST(2) - INRF: Defined by iobn_defs::inrf_bp_test_t
                                                                 \<63\> = TBD.
                                                                 \<62\> = TBD.
                                                                 \<61\> = TBD.
                                                                 \<60\> = TBD.
                                                                 \<59\> = TBD.
                                                                 \<58\> = TBD.
                                                                 \<57\> = TBD.
                                                                 \<56\> = TBD.

                                                                 IOBN()_BP_TEST(3) - INRF: Defined by iobn_defs::inrm_bp_test_t
                                                                 \<63\> = VCC - Victim DAT.
                                                                 \<62\> = VCC - Victim REQ (CMD).
                                                                 \<61\> = VCC - DAT (REQ/REQH).
                                                                 \<60\> = VCC - CMD (REQ/RQH).
                                                                 \<59\> = SLC - VCC.
                                                                 \<58\> = SLC - ACK.
                                                                 \<57\> = SLC - DAT.
                                                                 \<56\> = SLC - CMD. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_bp_testx_s cn; */
};
typedef union bdk_iobnx_bp_testx bdk_iobnx_bp_testx_t;

static inline uint64_t BDK_IOBNX_BP_TESTX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_BP_TESTX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=3)))
        return 0x87e0f0003800ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x3);
    __bdk_csr_fatal("IOBNX_BP_TESTX", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_BP_TESTX(a,b) bdk_iobnx_bp_testx_t
#define bustype_BDK_IOBNX_BP_TESTX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_BP_TESTX(a,b) "IOBNX_BP_TESTX"
#define device_bar_BDK_IOBNX_BP_TESTX(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_BP_TESTX(a,b) (a)
#define arguments_BDK_IOBNX_BP_TESTX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_cfg0
 *
 * IOBN General Configuration 0 Register
 */
union bdk_iobnx_cfg0
{
    uint64_t u;
    struct bdk_iobnx_cfg0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t force_gibm_ncbi_clk_en : 1; /**< [  4:  4](R/W) Force on GIBM NCBI clocks. For diagnostic use only. */
        uint64_t force_immx_sclk_cond_clk_en : 1;/**< [  3:  3](R/W) Force on IMMX clocks. For diagnostic use only. */
        uint64_t force_inrm_sclk_cond_clk_en : 1;/**< [  2:  2](R/W) Force on INRM clocks. For diagnostic use only. */
        uint64_t force_inrf_sclk_cond_clk_en : 1;/**< [  1:  1](R/W) Force on INRF clocks. For diagnostic use only. */
        uint64_t force_ins_sclk_cond_clk_en : 1;/**< [  0:  0](R/W) Force on INS clocks. For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t force_ins_sclk_cond_clk_en : 1;/**< [  0:  0](R/W) Force on INS clocks. For diagnostic use only. */
        uint64_t force_inrf_sclk_cond_clk_en : 1;/**< [  1:  1](R/W) Force on INRF clocks. For diagnostic use only. */
        uint64_t force_inrm_sclk_cond_clk_en : 1;/**< [  2:  2](R/W) Force on INRM clocks. For diagnostic use only. */
        uint64_t force_immx_sclk_cond_clk_en : 1;/**< [  3:  3](R/W) Force on IMMX clocks. For diagnostic use only. */
        uint64_t force_gibm_ncbi_clk_en : 1; /**< [  4:  4](R/W) Force on GIBM NCBI clocks. For diagnostic use only. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_cfg0_s cn; */
};
typedef union bdk_iobnx_cfg0 bdk_iobnx_cfg0_t;

static inline uint64_t BDK_IOBNX_CFG0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_CFG0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f0002000ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_CFG0", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_CFG0(a) bdk_iobnx_cfg0_t
#define bustype_BDK_IOBNX_CFG0(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_CFG0(a) "IOBNX_CFG0"
#define device_bar_BDK_IOBNX_CFG0(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_CFG0(a) (a)
#define arguments_BDK_IOBNX_CFG0(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_cfg1
 *
 * IOBN General Configuration 1 Register
 */
union bdk_iobnx_cfg1
{
    uint64_t u;
    struct bdk_iobnx_cfg1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_3_63         : 61;
        uint64_t force_immx_rclk_cond_clk_en : 1;/**< [  2:  2](R/W) For debug only. Must be zero during normal operation.
                                                                 Internal:
                                                                 Force the conditional clock active */
        uint64_t force_inrm_rclk_cond_clk_en : 1;/**< [  1:  1](R/W) For debug only. Must be zero during normal operation.
                                                                 Internal:
                                                                 Force the conditional clock active */
        uint64_t force_inrf_rclk_cond_clk_en : 1;/**< [  0:  0](R/W) For debug only. Must be zero during normal operation.
                                                                 Internal:
                                                                 Force the conditional clock active */
#else /* Word 0 - Little Endian */
        uint64_t force_inrf_rclk_cond_clk_en : 1;/**< [  0:  0](R/W) For debug only. Must be zero during normal operation.
                                                                 Internal:
                                                                 Force the conditional clock active */
        uint64_t force_inrm_rclk_cond_clk_en : 1;/**< [  1:  1](R/W) For debug only. Must be zero during normal operation.
                                                                 Internal:
                                                                 Force the conditional clock active */
        uint64_t force_immx_rclk_cond_clk_en : 1;/**< [  2:  2](R/W) For debug only. Must be zero during normal operation.
                                                                 Internal:
                                                                 Force the conditional clock active */
        uint64_t reserved_3_63         : 61;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_cfg1_s cn; */
};
typedef union bdk_iobnx_cfg1 bdk_iobnx_cfg1_t;

static inline uint64_t BDK_IOBNX_CFG1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_CFG1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f0002010ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_CFG1", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_CFG1(a) bdk_iobnx_cfg1_t
#define bustype_BDK_IOBNX_CFG1(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_CFG1(a) "IOBNX_CFG1"
#define device_bar_BDK_IOBNX_CFG1(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_CFG1(a) (a)
#define arguments_BDK_IOBNX_CFG1(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_chip_cur_pwr
 *
 * INTERNAL: IOBN Chip Current Power Register
 *
 * For diagnostic use only.
 * This register contains the current power setting.
 * Only index zero (IOB(0)) is used.
 */
union bdk_iobnx_chip_cur_pwr
{
    uint64_t u;
    struct bdk_iobnx_chip_cur_pwr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t current_power_setting : 8;  /**< [  7:  0](RO/H) Global throttling value currently being used. Throttling can force units (CPU cores, in
                                                                 particular) idle for a portion of time, which will reduce power consumption. When
                                                                 [CURRENT_POWER_SETTING] is equal to zero, the unit is idle most of the time and consumes
                                                                 minimum power. When [CURRENT_POWER_SETTING] is equal to 0xFF, units are never idled to
                                                                 reduce power. The hardware generally uses a [CURRENT_POWER_SETTING] value that is as large
                                                                 as possible (in order to maximize performance) subject to the following constraints (in
                                                                 priority order):
                                                                 * PWR_MIN \<= [CURRENT_POWER_SETTING] \<= PWR_MAX.
                                                                 * Power limits from the PWR_SETTING feedback control system.

                                                                 In the case of the CPU cores, [CURRENT_POWER_SETTING] effectively limits the CP0
                                                                 PowThrottle[POWLIM] value: effective POWLIM = MINIMUM([CURRENT_POWER_SETTING],
                                                                 PowThrottle[POWLIM]) */
#else /* Word 0 - Little Endian */
        uint64_t current_power_setting : 8;  /**< [  7:  0](RO/H) Global throttling value currently being used. Throttling can force units (CPU cores, in
                                                                 particular) idle for a portion of time, which will reduce power consumption. When
                                                                 [CURRENT_POWER_SETTING] is equal to zero, the unit is idle most of the time and consumes
                                                                 minimum power. When [CURRENT_POWER_SETTING] is equal to 0xFF, units are never idled to
                                                                 reduce power. The hardware generally uses a [CURRENT_POWER_SETTING] value that is as large
                                                                 as possible (in order to maximize performance) subject to the following constraints (in
                                                                 priority order):
                                                                 * PWR_MIN \<= [CURRENT_POWER_SETTING] \<= PWR_MAX.
                                                                 * Power limits from the PWR_SETTING feedback control system.

                                                                 In the case of the CPU cores, [CURRENT_POWER_SETTING] effectively limits the CP0
                                                                 PowThrottle[POWLIM] value: effective POWLIM = MINIMUM([CURRENT_POWER_SETTING],
                                                                 PowThrottle[POWLIM]) */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_chip_cur_pwr_s cn; */
};
typedef union bdk_iobnx_chip_cur_pwr bdk_iobnx_chip_cur_pwr_t;

static inline uint64_t BDK_IOBNX_CHIP_CUR_PWR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_CHIP_CUR_PWR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f000a110ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f000a110ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f000a110ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_CHIP_CUR_PWR", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_CHIP_CUR_PWR(a) bdk_iobnx_chip_cur_pwr_t
#define bustype_BDK_IOBNX_CHIP_CUR_PWR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_CHIP_CUR_PWR(a) "IOBNX_CHIP_CUR_PWR"
#define device_bar_BDK_IOBNX_CHIP_CUR_PWR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_CHIP_CUR_PWR(a) (a)
#define arguments_BDK_IOBNX_CHIP_CUR_PWR(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_chip_glb_pwr_throttle
 *
 * INTERNAL: IOBN Chip Global Power Throttle Register
 *
 * For diagnostic use only.
 * This register controls the min/max power settings.
 * Only index zero (IOB(0)) is used.
 */
union bdk_iobnx_chip_glb_pwr_throttle
{
    uint64_t u;
    struct bdk_iobnx_chip_glb_pwr_throttle_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_34_63        : 30;
        uint64_t pwr_bw                : 2;  /**< [ 33: 32](R/W) Configures the reaction time of the closed-loop feedback control system for the
                                                                 AVG_CHIP_POWER power approximation. Higher numbers decrease bandwidth, reducing response
                                                                 time, which could lead to greater tracking error, but reduce ringing. */
        uint64_t pwr_max               : 8;  /**< [ 31: 24](R/W) Reserved. */
        uint64_t pwr_min               : 8;  /**< [ 23: 16](R/W) Reserved. */
        uint64_t pwr_setting           : 16; /**< [ 15:  0](R/W) A power limiter for the chip. A limiter of the power consumption of the
                                                                 chip. This power limiting is implemented by a closed-loop feedback control
                                                                 system for the AVG_CHIP_POWER power approximation. The direct output of the
                                                                 [PWR_SETTING] feedback control system is the CURRENT_POWER_SETTING value. The
                                                                 power consumed by the chip (estimated currently by the AVG_CHIP_POWER value) is
                                                                 an indirect output of the PWR_SETTING feedback control system. [PWR_SETTING] is
                                                                 not used by the hardware when [PWR_MIN] equals [PWR_MAX]. [PWR_MIN] and
                                                                 [PWR_MAX] threshold requirements always supersede [PWR_SETTING] limits. (For
                                                                 maximum [PWR_SETTING] feedback control freedom, set [PWR_MIN]=0 and
                                                                 [PWR_MAX]=0xff.)

                                                                 [PWR_SETTING] equal to 0 forces the chip to consume near minimum
                                                                 power. Increasing [PWR_SETTING] value from 0 to 0xFFFF increases the power that
                                                                 the chip is allowed to consume linearly (roughly) from minimum to maximum. */
#else /* Word 0 - Little Endian */
        uint64_t pwr_setting           : 16; /**< [ 15:  0](R/W) A power limiter for the chip. A limiter of the power consumption of the
                                                                 chip. This power limiting is implemented by a closed-loop feedback control
                                                                 system for the AVG_CHIP_POWER power approximation. The direct output of the
                                                                 [PWR_SETTING] feedback control system is the CURRENT_POWER_SETTING value. The
                                                                 power consumed by the chip (estimated currently by the AVG_CHIP_POWER value) is
                                                                 an indirect output of the PWR_SETTING feedback control system. [PWR_SETTING] is
                                                                 not used by the hardware when [PWR_MIN] equals [PWR_MAX]. [PWR_MIN] and
                                                                 [PWR_MAX] threshold requirements always supersede [PWR_SETTING] limits. (For
                                                                 maximum [PWR_SETTING] feedback control freedom, set [PWR_MIN]=0 and
                                                                 [PWR_MAX]=0xff.)

                                                                 [PWR_SETTING] equal to 0 forces the chip to consume near minimum
                                                                 power. Increasing [PWR_SETTING] value from 0 to 0xFFFF increases the power that
                                                                 the chip is allowed to consume linearly (roughly) from minimum to maximum. */
        uint64_t pwr_min               : 8;  /**< [ 23: 16](R/W) Reserved. */
        uint64_t pwr_max               : 8;  /**< [ 31: 24](R/W) Reserved. */
        uint64_t pwr_bw                : 2;  /**< [ 33: 32](R/W) Configures the reaction time of the closed-loop feedback control system for the
                                                                 AVG_CHIP_POWER power approximation. Higher numbers decrease bandwidth, reducing response
                                                                 time, which could lead to greater tracking error, but reduce ringing. */
        uint64_t reserved_34_63        : 30;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_chip_glb_pwr_throttle_s cn; */
};
typedef union bdk_iobnx_chip_glb_pwr_throttle bdk_iobnx_chip_glb_pwr_throttle_t;

static inline uint64_t BDK_IOBNX_CHIP_GLB_PWR_THROTTLE(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_CHIP_GLB_PWR_THROTTLE(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f000a100ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f000a100ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f000a100ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_CHIP_GLB_PWR_THROTTLE", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_CHIP_GLB_PWR_THROTTLE(a) bdk_iobnx_chip_glb_pwr_throttle_t
#define bustype_BDK_IOBNX_CHIP_GLB_PWR_THROTTLE(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_CHIP_GLB_PWR_THROTTLE(a) "IOBNX_CHIP_GLB_PWR_THROTTLE"
#define device_bar_BDK_IOBNX_CHIP_GLB_PWR_THROTTLE(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_CHIP_GLB_PWR_THROTTLE(a) (a)
#define arguments_BDK_IOBNX_CHIP_GLB_PWR_THROTTLE(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_chip_pwr_out
 *
 * IOBN Chip Power Out Register
 * This register contains power numbers from the various partitions on the chip.
 * Only index zero (IOB(0)) is used.
 */
union bdk_iobnx_chip_pwr_out
{
    uint64_t u;
    struct bdk_iobnx_chip_pwr_out_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t cpu_pwr               : 16; /**< [ 63: 48](RO/H) An estimate of the current CPU core complex power consumption, including a sum
                                                                 of all processor's AP_CVM_POWER_EL1[AVERAGE_POWER]. The CPU core complex
                                                                 includes the caches and DRAM controller(s), as well as all CPU cores. Linearly
                                                                 larger values indicate linearly higher power consumption. */
        uint64_t chip_power            : 16; /**< [ 47: 32](RO/H) An estimate of the current total power consumption by the chip. Linearly larger values
                                                                 indicate linearly higher power consumption. [CHIP_POWER] is the sum of [CPU_PWR] and
                                                                 [COPROC_POWER]. */
        uint64_t coproc_power          : 16; /**< [ 31: 16](RO/H) An estimate of the current coprocessor power consumption. Linearly larger values indicate
                                                                 linearly higher power consumption. This estimate is energy per core clock, and will
                                                                 generally decrease as the ratio of core to coprocessor clock speed increases. */
        uint64_t avg_chip_power        : 16; /**< [ 15:  0](RO/H) Average chip power.
                                                                 An average of [CHIP_POWER], calculated using an IIR filter with an average
                                                                 weight of 16K core clocks, in mA/GHz.

                                                                 Accurate power numbers should be calculated using a platform-specific method which
                                                                 e.g. reads the current consumption of the VRM.

                                                                 Otherwise an approximation of this chip's power is calculated with:

                                                                   _ power = chip_const + core_const * cores_powered_on + [AVG_CHIP_POWER] * voltage

                                                                 Where:

                                                                 _ power is in mW.

                                                                 _ chip_const is in mW and represents the I/O power and chip excluding core_const.
                                                                 This may vary as I/O and coprocessor loads vary, therefore only
                                                                 platform methods can be used for accurate estimates.

                                                                 _ core_const is a per-core constant leakage from the HRM power application note, and is in
                                                                 mA.

                                                                 _ cores_powered_on is a population count of all bits set in RST_PP_POWER.

                                                                 _ voltage is determined by the platform, perhaps by reading a VRM setting. */
#else /* Word 0 - Little Endian */
        uint64_t avg_chip_power        : 16; /**< [ 15:  0](RO/H) Average chip power.
                                                                 An average of [CHIP_POWER], calculated using an IIR filter with an average
                                                                 weight of 16K core clocks, in mA/GHz.

                                                                 Accurate power numbers should be calculated using a platform-specific method which
                                                                 e.g. reads the current consumption of the VRM.

                                                                 Otherwise an approximation of this chip's power is calculated with:

                                                                   _ power = chip_const + core_const * cores_powered_on + [AVG_CHIP_POWER] * voltage

                                                                 Where:

                                                                 _ power is in mW.

                                                                 _ chip_const is in mW and represents the I/O power and chip excluding core_const.
                                                                 This may vary as I/O and coprocessor loads vary, therefore only
                                                                 platform methods can be used for accurate estimates.

                                                                 _ core_const is a per-core constant leakage from the HRM power application note, and is in
                                                                 mA.

                                                                 _ cores_powered_on is a population count of all bits set in RST_PP_POWER.

                                                                 _ voltage is determined by the platform, perhaps by reading a VRM setting. */
        uint64_t coproc_power          : 16; /**< [ 31: 16](RO/H) An estimate of the current coprocessor power consumption. Linearly larger values indicate
                                                                 linearly higher power consumption. This estimate is energy per core clock, and will
                                                                 generally decrease as the ratio of core to coprocessor clock speed increases. */
        uint64_t chip_power            : 16; /**< [ 47: 32](RO/H) An estimate of the current total power consumption by the chip. Linearly larger values
                                                                 indicate linearly higher power consumption. [CHIP_POWER] is the sum of [CPU_PWR] and
                                                                 [COPROC_POWER]. */
        uint64_t cpu_pwr               : 16; /**< [ 63: 48](RO/H) An estimate of the current CPU core complex power consumption, including a sum
                                                                 of all processor's AP_CVM_POWER_EL1[AVERAGE_POWER]. The CPU core complex
                                                                 includes the caches and DRAM controller(s), as well as all CPU cores. Linearly
                                                                 larger values indicate linearly higher power consumption. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_chip_pwr_out_s cn; */
};
typedef union bdk_iobnx_chip_pwr_out bdk_iobnx_chip_pwr_out_t;

static inline uint64_t BDK_IOBNX_CHIP_PWR_OUT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_CHIP_PWR_OUT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f000a108ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f000a108ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f000a108ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_CHIP_PWR_OUT", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_CHIP_PWR_OUT(a) bdk_iobnx_chip_pwr_out_t
#define bustype_BDK_IOBNX_CHIP_PWR_OUT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_CHIP_PWR_OUT(a) "IOBNX_CHIP_PWR_OUT"
#define device_bar_BDK_IOBNX_CHIP_PWR_OUT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_CHIP_PWR_OUT(a) (a)
#define arguments_BDK_IOBNX_CHIP_PWR_OUT(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_cond_clk_cap#
 *
 * INTERNAL: IOBN Conditional Clock Capacitance Register
 *
 * This register is for diagnostic use only.
 * Internal:
 * Each index corresponds to a different net as follows:
 *   0 = bgx0___bgx___bgx_clk___csclk_drv.
 *   1 = bgx0___bgx___bgx_clk___ssclk_drv.
 *   2 = bgx0___bgx___bgx_clk___gsclk_drv.
 *   3 = bgx1___bgx___bgx_clk___csclk_drv.
 *   4 = bgx1___bgx___bgx_clk___ssclk_drv.
 *   5 = bgx1___bgx___bgx_clk___gsclk_drv.
 *   6 = bgx2___bgx___bgx_clk___csclk_drv.
 *   7 = bgx2___bgx___bgx_clk___ssclk_drv.
 *   8 = bgx2___bgx___bgx_clk___gsclk_drv.
 *   9 = bgx3___bgx___bgx_clk___csclk_drv.
 *   10 = bgx3___bgx___bgx_clk___ssclk_drv.
 *   11 = bgx3___bgx___bgx_clk___gsclk_drv.
 *   12 = dpi___dpi___csclk_drv.
 *   13 = fpa___fpa___gbl___csclk_drv.
 *   14 = lbk___lbk___lbk_core_p0x0___csclk_drv.
 *   15 = lbk___lbk___lbk_core_p0x1___csclk_drv.
 *   16 = lbk___lbk___lbk_core_p1x0___csclk_drv.
 *   17 = lbk___lbk___lbk_core_p1x1___csclk_drv.
 *   18 = mio___mio___uaa0___u_csclk_drv.
 *   19 = mio___mio___uaa1___u_csclk_drv.
 *   20 = mio___mio___uaa2___u_csclk_drv.
 *   21 = mio___mio___uaa3___u_csclk_drv.
 *   22 = nic___nic___nic_l___nic_l1___nic_clk___csclk_drv.
 *   23 = nic___nic___nic_l___nic_l2___nic_clk___csclk_drv.
 *   24 = nic___nic___nic_u___nic_u1___nic_clk___csclk_drv.
 *   25 = pem0___pem___pem_clks___csclk_drv.
 *   26 = pem0___pem___pem_clks___sync_pwr_thr_pclk.
 *   27 = pem1___pem___pem_clks___csclk_drv.
 *   28 = pem1___pem___pem_clks___sync_pwr_thr_pclk.
 *   29 = pem2___pem___pem_clks___csclk_drv.
 *   30 = pem2___pem___pem_clks___sync_pwr_thr_pclk.
 *   31 = pem3___pem___pem_clks___csclk_drv.
 *   32 = pem3___pem___pem_clks___sync_pwr_thr_pclk.
 *   33 = pki___pki___pdp___pfe___csclk_drv.
 *   34 = pki___pki___pdp___pbe___csclk_drv.
 *   35 = pki___pki___pix___ipec0___csclk_drv.
 *   36 = pki___pki___pix___ipec1___csclk_drv.
 *   37 = pki___pki___pix___mech___csclk_drv.
 *   38 = roc_ocla___roc_ocla___core___clks___csclk_drv.
 *   39 = rst___rst___mio_clk_ctl___csclk_drv.
 *   40 = sata0___sata___u_csclk_drv.
 *   41 = sata0___sata___u_csclk_drv.
 *   42 = sata0___sata___u_csclk_drv.
 *   43 = sata0___sata___u_csclk_drv.
 *   44 = sata0___sata___u_csclk_drv.
 *   45 = sata0___sata___u_csclk_drv.
 *   46 = smmu___smmu___wcsr___gbl___crclk_drv.
 *   47 = smmu___smmu___wcsr___gbl___u_c2rclk_drv.
 *   48 = smmu___smmu___wcsr___gbl___u_c2rclk_drv_n.
 *   49 = smmu___smmu___xl___ctl___crclk_drv.
 *   50 = sso___sso___sso_pnr___sso_aw___clk___csclk_drv.
 *   51 = sso___sso___sso_pnr___sso_gw___clk___csclk_drv.
 *   52 = sso___sso___sso_pnr___sso_ws___clk___csclk_drv.
 *   53 = usbdrd0___usbdrd_i___u_csclk_drv.
 *   54 = usbdrd0___usbdrd_i___u_csclk_drv.
 *   55 = zipc0___zipc___zipc_clk___zip_hash_csclk_drv.
 *   56 = zipc0___zipc___zipc_clk___zip_history_csclk_drv.
 *   57 = zipc0___zipc___zipc_clk___zip_state_csclk_drv.
 *   58 = zipc0___zipc___zipc_clk___zip_sha_csclk_drv.
 *   59 = zipc1___zipc___zipc_clk___zip_hash_csclk_drv.
 *   60 = zipc1___zipc___zipc_clk___zip_history_csclk_drv.
 *   61 = zipc1___zipc___zipc_clk___zip_state_csclk_drv.
 *   62 = zipc1___zipc___zipc_clk___zip_sha_csclk_drv.
 *   63 = zipc2___zipc___zipc_clk___zip_hash_csclk_drv.
 *   64 = zipc2___zipc___zipc_clk___zip_history_csclk_drv.
 *   65 = zipc2___zipc___zipc_clk___zip_state_csclk_drv.
 *   66 = zipc2___zipc___zipc_clk___zip_sha_csclk_drv.
 *   67 = zipd3___zipd___zipd_clk___zip_history_csclk_drv.
 *   68 = zipd3___zipd___zipd_clk___zip_state_csclk_drv.
 *   69 = zipd3___zipd___zipd_clk___zip_sha_csclk_drv.
 *   70 = zipd4___zipd___zipd_clk___zip_history_csclk_drv.
 *   71 = zipd4___zipd___zipd_clk___zip_state_csclk_drv.
 *   72 = zipd4___zipd___zipd_clk___zip_sha_csclk_drv.
 *   73 = zipd5___zipd___zipd_clk___zip_history_csclk_drv.
 *   74 = zipd5___zipd___zipd_clk___zip_state_csclk_drv.
 *   75 = zipd5___zipd___zipd_clk___zip_sha_csclk_drv.
 */
union bdk_iobnx_cond_clk_capx
{
    uint64_t u;
    struct bdk_iobnx_cond_clk_capx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t cap                   : 16; /**< [ 15:  0](R/W) Conditional clock capacitance for drivers. (cap value * 0.9/128.)
                                                                 For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t cap                   : 16; /**< [ 15:  0](R/W) Conditional clock capacitance for drivers. (cap value * 0.9/128.)
                                                                 For diagnostic use only. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_cond_clk_capx_s cn; */
};
typedef union bdk_iobnx_cond_clk_capx bdk_iobnx_cond_clk_capx_t;

static inline uint64_t BDK_IOBNX_COND_CLK_CAPX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_COND_CLK_CAPX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=75)))
        return 0x87e0f000f000ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x7f);
    __bdk_csr_fatal("IOBNX_COND_CLK_CAPX", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_COND_CLK_CAPX(a,b) bdk_iobnx_cond_clk_capx_t
#define bustype_BDK_IOBNX_COND_CLK_CAPX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_COND_CLK_CAPX(a,b) "IOBNX_COND_CLK_CAPX"
#define device_bar_BDK_IOBNX_COND_CLK_CAPX(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_COND_CLK_CAPX(a,b) (a)
#define arguments_BDK_IOBNX_COND_CLK_CAPX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_const
 *
 * IOBN Constant Registers
 * This register returns discovery information.
 */
union bdk_iobnx_const
{
    uint64_t u;
    struct bdk_iobnx_const_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t arbs                  : 8;  /**< [ 23: 16](RO) Maximum number of grants on any NCB bus attached to this IOB. */
        uint64_t ncbs                  : 8;  /**< [ 15:  8](RO) Number of physical NCB busses attached to this IOB. */
        uint64_t iobs                  : 8;  /**< [  7:  0](RO) Number of IOBs. */
#else /* Word 0 - Little Endian */
        uint64_t iobs                  : 8;  /**< [  7:  0](RO) Number of IOBs. */
        uint64_t ncbs                  : 8;  /**< [ 15:  8](RO) Number of physical NCB busses attached to this IOB. */
        uint64_t arbs                  : 8;  /**< [ 23: 16](RO) Maximum number of grants on any NCB bus attached to this IOB. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_const_s cn; */
};
typedef union bdk_iobnx_const bdk_iobnx_const_t;

static inline uint64_t BDK_IOBNX_CONST(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_CONST(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f0000000ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_CONST", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_CONST(a) bdk_iobnx_const_t
#define bustype_BDK_IOBNX_CONST(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_CONST(a) "IOBNX_CONST"
#define device_bar_BDK_IOBNX_CONST(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_CONST(a) (a)
#define arguments_BDK_IOBNX_CONST(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_core_bist_status
 *
 * IOBN Cores BIST Status Register
 * This register contains the result of the BIST run on the cores.
 */
union bdk_iobnx_core_bist_status
{
    uint64_t u;
    struct bdk_iobnx_core_bist_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t core_bstat            : 24; /**< [ 23:  0](RO/H) BIST status of the cores. IOBN0 contains the BIST status for the even numbered cores and
                                                                 IOBN1 contains the BIST status for the odd numbered cores.

                                                                 \<pre\>
                                                                    BIT    IOBN0     IOBN0 MASK BIT     IOBN1     IOBN1 MASK BIT
                                                                    [0]    Core 0        \<0\>            Core 1        \<1\>
                                                                    [1]    Core 2        \<2\>            Core 3        \<3\>
                                                                    ...
                                                                    [23]   Core 46      \<46\>            Core 47      \<47\>
                                                                 \</pre\>

                                                                 Software must logically AND CORE_BSTAT bits with appropriate bits from RST_PP_AVAILABLE
                                                                 before using them. The "IOBN0 MASK BIT" column in the table above shows the
                                                                 RST_PP_AVAILABLE bits to use with IOBN0. The "IOBN1 MASK BIT" column in the
                                                                 table above shows the RST_PP_AVAILABLE bits to use with IOBN1. */
#else /* Word 0 - Little Endian */
        uint64_t core_bstat            : 24; /**< [ 23:  0](RO/H) BIST status of the cores. IOBN0 contains the BIST status for the even numbered cores and
                                                                 IOBN1 contains the BIST status for the odd numbered cores.

                                                                 \<pre\>
                                                                    BIT    IOBN0     IOBN0 MASK BIT     IOBN1     IOBN1 MASK BIT
                                                                    [0]    Core 0        \<0\>            Core 1        \<1\>
                                                                    [1]    Core 2        \<2\>            Core 3        \<3\>
                                                                    ...
                                                                    [23]   Core 46      \<46\>            Core 47      \<47\>
                                                                 \</pre\>

                                                                 Software must logically AND CORE_BSTAT bits with appropriate bits from RST_PP_AVAILABLE
                                                                 before using them. The "IOBN0 MASK BIT" column in the table above shows the
                                                                 RST_PP_AVAILABLE bits to use with IOBN0. The "IOBN1 MASK BIT" column in the
                                                                 table above shows the RST_PP_AVAILABLE bits to use with IOBN1. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    struct bdk_iobnx_core_bist_status_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t core_bstat            : 4;  /**< [  3:  0](RO/H) BIST status of the cores. IOBN0 contains the BIST status for the even numbered cores and
                                                                 IOBN1 contains the BIST status for the odd numbered cores.

                                                                 \<pre\>
                                                                    BIT    IOBN
                                                                    [0]    Core 0
                                                                    [1]    Core 1
                                                                    [2]    Core 2
                                                                    [3]    Core 3
                                                                 \</pre\>

                                                                 Software must bit-wise logical AND CORE_BSTAT with RST_PP_AVAILABLE before using it. */
#else /* Word 0 - Little Endian */
        uint64_t core_bstat            : 4;  /**< [  3:  0](RO/H) BIST status of the cores. IOBN0 contains the BIST status for the even numbered cores and
                                                                 IOBN1 contains the BIST status for the odd numbered cores.

                                                                 \<pre\>
                                                                    BIT    IOBN
                                                                    [0]    Core 0
                                                                    [1]    Core 1
                                                                    [2]    Core 2
                                                                    [3]    Core 3
                                                                 \</pre\>

                                                                 Software must bit-wise logical AND CORE_BSTAT with RST_PP_AVAILABLE before using it. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_iobnx_core_bist_status_s cn88xx; */
    struct bdk_iobnx_core_bist_status_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t core_bstat            : 24; /**< [ 23:  0](RO/H) BIST status of the cores. IOBN0 contains the BIST status for all cores. IOBN1
                                                                 always returns 0x0.

                                                                 Software must bit-wise logical and CORE_BSTAT with RST_PP_AVAILABLE before using
                                                                 it. */
#else /* Word 0 - Little Endian */
        uint64_t core_bstat            : 24; /**< [ 23:  0](RO/H) BIST status of the cores. IOBN0 contains the BIST status for all cores. IOBN1
                                                                 always returns 0x0.

                                                                 Software must bit-wise logical and CORE_BSTAT with RST_PP_AVAILABLE before using
                                                                 it. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_iobnx_core_bist_status bdk_iobnx_core_bist_status_t;

static inline uint64_t BDK_IOBNX_CORE_BIST_STATUS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_CORE_BIST_STATUS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f0005008ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f0005008ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f0005008ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_CORE_BIST_STATUS", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_CORE_BIST_STATUS(a) bdk_iobnx_core_bist_status_t
#define bustype_BDK_IOBNX_CORE_BIST_STATUS(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_CORE_BIST_STATUS(a) "IOBNX_CORE_BIST_STATUS"
#define device_bar_BDK_IOBNX_CORE_BIST_STATUS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_CORE_BIST_STATUS(a) (a)
#define arguments_BDK_IOBNX_CORE_BIST_STATUS(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_dis_ncbi_io
 *
 * IOBN Disable NCBI IO Register
 * IOBN control.
 */
union bdk_iobnx_dis_ncbi_io
{
    uint64_t u;
    struct bdk_iobnx_dis_ncbi_io_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t sli_key_mem           : 1;  /**< [  5:  5](R/W) SLI KEY memory isolation.
                                                                 0 = SLI initiated requests are allowed.
                                                                 1 = SLI initiated read and write requests are allowed to
                                                                     KEY_MEM(0..2047) (e.g. 16KB, not all of KEY_MEM) only.
                                                                     SLI requests to any other address (non-KEY_MEM(0..2047))
                                                                     will be redirected to ECAM0_NOP_ZF. */
        uint64_t sli_off               : 1;  /**< [  4:  4](R/W) SLI isolation.
                                                                 Resets to one in trusted mode, else zero.
                                                                 0 = Operation of NCBI transactions are not impacted.
                                                                 1 = NCBI transactions initiating at the SLI are disabled and
                                                                     turn into accesses to ECAM0_NOP_ZF. When set, this bit
                                                                     overrides [SLI_KEY_MEM]. */
        uint64_t tlb_sync_dis          : 1;  /**< [  3:  3](R/W) When set the IOBN will return SYNC-RDY to the SMMU without waiting for
                                                                 outstanding request to receive responses. */
        uint64_t oci_key_only          : 1;  /**< [  2:  2](RO) Restrict CCPI-sourced I/O write requests.

                                                                 0 = CCPI-sourced I/O read and write requests are allowed to any device through
                                                                 IOB, including allowing read/writes to all of KEY_MEM().

                                                                 1 = CCPI-sourced I/O write requests allowed to KEY_MEM(0..2047) (e.g. 16KB, not
                                                                 all of KEY_MEM) only. CCPI-sourced writes to __any__ other address
                                                                 (non-KEY_MEM(0..2047)), or any CCPI-source read will be redirected to
                                                                 ECAM0_NOP_ZF (for non-ECAM) or ECAM0_NOP_ONNF (for-ECAM).

                                                                 This setting does not affect local-node originated traffic.

                                                                 In pass 1, read-only. */
        uint64_t all_gic               : 1;  /**< [  1:  1](R/W) All-to-GIC. For diagnostic use only.
                                                                 Internal:
                                                                 0 = Normal operation. NCBI traffic to GIC interrupt delivery registers will be ordered
                                                                 with other interrupt delivery traffic and over the RIB bus.  NCBI traffic to normal non-
                                                                 interrupt-delivery GIC registers will go via RSL.
                                                                   1 = All NCBI traffic to the GIC DID will be assumed to be interrupt delivery traffic.
                                                                 This will break NCBI write transactions to non-interrupt-delivery GIC registers, but may
                                                                 work around bugs whereby interrupt-delivery CSRs are mis-catagorized inside IOB. */
        uint64_t ncbi_off              : 1;  /**< [  0:  0](R/W) When set NCBI translation to I/O space (with exception of GIC traffic) will be disabled.
                                                                 Disabled traffic will turn into access to ECAM0_NOP_ZF. */
#else /* Word 0 - Little Endian */
        uint64_t ncbi_off              : 1;  /**< [  0:  0](R/W) When set NCBI translation to I/O space (with exception of GIC traffic) will be disabled.
                                                                 Disabled traffic will turn into access to ECAM0_NOP_ZF. */
        uint64_t all_gic               : 1;  /**< [  1:  1](R/W) All-to-GIC. For diagnostic use only.
                                                                 Internal:
                                                                 0 = Normal operation. NCBI traffic to GIC interrupt delivery registers will be ordered
                                                                 with other interrupt delivery traffic and over the RIB bus.  NCBI traffic to normal non-
                                                                 interrupt-delivery GIC registers will go via RSL.
                                                                   1 = All NCBI traffic to the GIC DID will be assumed to be interrupt delivery traffic.
                                                                 This will break NCBI write transactions to non-interrupt-delivery GIC registers, but may
                                                                 work around bugs whereby interrupt-delivery CSRs are mis-catagorized inside IOB. */
        uint64_t oci_key_only          : 1;  /**< [  2:  2](RO) Restrict CCPI-sourced I/O write requests.

                                                                 0 = CCPI-sourced I/O read and write requests are allowed to any device through
                                                                 IOB, including allowing read/writes to all of KEY_MEM().

                                                                 1 = CCPI-sourced I/O write requests allowed to KEY_MEM(0..2047) (e.g. 16KB, not
                                                                 all of KEY_MEM) only. CCPI-sourced writes to __any__ other address
                                                                 (non-KEY_MEM(0..2047)), or any CCPI-source read will be redirected to
                                                                 ECAM0_NOP_ZF (for non-ECAM) or ECAM0_NOP_ONNF (for-ECAM).

                                                                 This setting does not affect local-node originated traffic.

                                                                 In pass 1, read-only. */
        uint64_t tlb_sync_dis          : 1;  /**< [  3:  3](R/W) When set the IOBN will return SYNC-RDY to the SMMU without waiting for
                                                                 outstanding request to receive responses. */
        uint64_t sli_off               : 1;  /**< [  4:  4](R/W) SLI isolation.
                                                                 Resets to one in trusted mode, else zero.
                                                                 0 = Operation of NCBI transactions are not impacted.
                                                                 1 = NCBI transactions initiating at the SLI are disabled and
                                                                     turn into accesses to ECAM0_NOP_ZF. When set, this bit
                                                                     overrides [SLI_KEY_MEM]. */
        uint64_t sli_key_mem           : 1;  /**< [  5:  5](R/W) SLI KEY memory isolation.
                                                                 0 = SLI initiated requests are allowed.
                                                                 1 = SLI initiated read and write requests are allowed to
                                                                     KEY_MEM(0..2047) (e.g. 16KB, not all of KEY_MEM) only.
                                                                     SLI requests to any other address (non-KEY_MEM(0..2047))
                                                                     will be redirected to ECAM0_NOP_ZF. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } s;
    struct bdk_iobnx_dis_ncbi_io_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t tlb_sync_dis          : 1;  /**< [  3:  3](R/W) When set the IOBN will return SYNC-RDY to the SMMU without waiting for
                                                                 outstanding request to receive responses. */
        uint64_t oci_key_only          : 1;  /**< [  2:  2](RO) Restrict CCPI-sourced I/O write requests.

                                                                 0 = CCPI-sourced I/O read and write requests are allowed to any device through
                                                                 IOB, including allowing read/writes to all of KEY_MEM().

                                                                 1 = CCPI-sourced I/O write requests allowed to KEY_MEM(0..2047) (e.g. 16KB, not
                                                                 all of KEY_MEM) only. CCPI-sourced writes to __any__ other address
                                                                 (non-KEY_MEM(0..2047)), or any CCPI-source read will be redirected to
                                                                 ECAM0_NOP_ZF (for non-ECAM) or ECAM0_NOP_ONNF (for-ECAM).

                                                                 This setting does not affect local-node originated traffic.

                                                                 In pass 1, read-only. */
        uint64_t all_gic               : 1;  /**< [  1:  1](R/W) All-to-GIC. For diagnostic use only.
                                                                 Internal:
                                                                 0 = Normal operation. NCBI traffic to GIC interrupt delivery registers will be ordered
                                                                 with other interrupt delivery traffic and over the RIB bus.  NCBI traffic to normal non-
                                                                 interrupt-delivery GIC registers will go via RSL.
                                                                   1 = All NCBI traffic to the GIC DID will be assumed to be interrupt delivery traffic.
                                                                 This will break NCBI write transactions to non-interrupt-delivery GIC registers, but may
                                                                 work around bugs whereby interrupt-delivery CSRs are mis-catagorized inside IOB. */
        uint64_t ncbi_off              : 1;  /**< [  0:  0](R/W) When set NCBI translation to I/O space (with exception of GIC traffic) will be disabled.
                                                                 Disabled traffic will turn into access to ECAM0_NOP_ZF. */
#else /* Word 0 - Little Endian */
        uint64_t ncbi_off              : 1;  /**< [  0:  0](R/W) When set NCBI translation to I/O space (with exception of GIC traffic) will be disabled.
                                                                 Disabled traffic will turn into access to ECAM0_NOP_ZF. */
        uint64_t all_gic               : 1;  /**< [  1:  1](R/W) All-to-GIC. For diagnostic use only.
                                                                 Internal:
                                                                 0 = Normal operation. NCBI traffic to GIC interrupt delivery registers will be ordered
                                                                 with other interrupt delivery traffic and over the RIB bus.  NCBI traffic to normal non-
                                                                 interrupt-delivery GIC registers will go via RSL.
                                                                   1 = All NCBI traffic to the GIC DID will be assumed to be interrupt delivery traffic.
                                                                 This will break NCBI write transactions to non-interrupt-delivery GIC registers, but may
                                                                 work around bugs whereby interrupt-delivery CSRs are mis-catagorized inside IOB. */
        uint64_t oci_key_only          : 1;  /**< [  2:  2](RO) Restrict CCPI-sourced I/O write requests.

                                                                 0 = CCPI-sourced I/O read and write requests are allowed to any device through
                                                                 IOB, including allowing read/writes to all of KEY_MEM().

                                                                 1 = CCPI-sourced I/O write requests allowed to KEY_MEM(0..2047) (e.g. 16KB, not
                                                                 all of KEY_MEM) only. CCPI-sourced writes to __any__ other address
                                                                 (non-KEY_MEM(0..2047)), or any CCPI-source read will be redirected to
                                                                 ECAM0_NOP_ZF (for non-ECAM) or ECAM0_NOP_ONNF (for-ECAM).

                                                                 This setting does not affect local-node originated traffic.

                                                                 In pass 1, read-only. */
        uint64_t tlb_sync_dis          : 1;  /**< [  3:  3](R/W) When set the IOBN will return SYNC-RDY to the SMMU without waiting for
                                                                 outstanding request to receive responses. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_iobnx_dis_ncbi_io_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t tlb_sync_dis          : 1;  /**< [  3:  3](R/W) When set the IOBN will return SYNC-RDY to the SMMU without waiting for
                                                                 outstanding request to receive responses. */
        uint64_t oci_key_only          : 1;  /**< [  2:  2](R/W) Restrict CCPI-sourced I/O write requests.

                                                                 0 = CCPI-sourced I/O read and write requests are allowed to any device through
                                                                 IOB, including allowing read/writes to all of KEY_MEM().

                                                                 1 = CCPI-sourced I/O write requests allowed to KEY_MEM(0..2047) (e.g. 16KB, not
                                                                 all of KEY_MEM) only. CCPI-sourced writes to __any__ other address
                                                                 (non-KEY_MEM(0..2047)), or any CCPI-source read will be redirected to
                                                                 ECAM0_NOP_ZF (for non-ECAM) or ECAM0_NOP_ONNF (for-ECAM).

                                                                 This setting does not affect local-node originated traffic.

                                                                 In pass 1, read-only. */
        uint64_t all_gic               : 1;  /**< [  1:  1](R/W) All-to-GIC. For diagnostic use only.
                                                                 Internal:
                                                                 0 = Normal operation. NCBI traffic to GIC interrupt delivery registers will be ordered
                                                                 with other interrupt delivery traffic and over the RIB bus.  NCBI traffic to normal non-
                                                                 interrupt-delivery GIC registers will go via RSL.
                                                                   1 = All NCBI traffic to the GIC DID will be assumed to be interrupt delivery traffic.
                                                                 This will break NCBI write transactions to non-interrupt-delivery GIC registers, but may
                                                                 work around bugs whereby interrupt-delivery CSRs are mis-catagorized inside IOB. */
        uint64_t ncbi_off              : 1;  /**< [  0:  0](R/W) When set NCBI translation to I/O space (with exception of GIC traffic) will be disabled.
                                                                 Disabled traffic will turn into access to ECAM0_NOP_ZF. */
#else /* Word 0 - Little Endian */
        uint64_t ncbi_off              : 1;  /**< [  0:  0](R/W) When set NCBI translation to I/O space (with exception of GIC traffic) will be disabled.
                                                                 Disabled traffic will turn into access to ECAM0_NOP_ZF. */
        uint64_t all_gic               : 1;  /**< [  1:  1](R/W) All-to-GIC. For diagnostic use only.
                                                                 Internal:
                                                                 0 = Normal operation. NCBI traffic to GIC interrupt delivery registers will be ordered
                                                                 with other interrupt delivery traffic and over the RIB bus.  NCBI traffic to normal non-
                                                                 interrupt-delivery GIC registers will go via RSL.
                                                                   1 = All NCBI traffic to the GIC DID will be assumed to be interrupt delivery traffic.
                                                                 This will break NCBI write transactions to non-interrupt-delivery GIC registers, but may
                                                                 work around bugs whereby interrupt-delivery CSRs are mis-catagorized inside IOB. */
        uint64_t oci_key_only          : 1;  /**< [  2:  2](R/W) Restrict CCPI-sourced I/O write requests.

                                                                 0 = CCPI-sourced I/O read and write requests are allowed to any device through
                                                                 IOB, including allowing read/writes to all of KEY_MEM().

                                                                 1 = CCPI-sourced I/O write requests allowed to KEY_MEM(0..2047) (e.g. 16KB, not
                                                                 all of KEY_MEM) only. CCPI-sourced writes to __any__ other address
                                                                 (non-KEY_MEM(0..2047)), or any CCPI-source read will be redirected to
                                                                 ECAM0_NOP_ZF (for non-ECAM) or ECAM0_NOP_ONNF (for-ECAM).

                                                                 This setting does not affect local-node originated traffic.

                                                                 In pass 1, read-only. */
        uint64_t tlb_sync_dis          : 1;  /**< [  3:  3](R/W) When set the IOBN will return SYNC-RDY to the SMMU without waiting for
                                                                 outstanding request to receive responses. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_iobnx_dis_ncbi_io_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t sli_key_mem           : 1;  /**< [  5:  5](R/W) SLI KEY memory isolation.
                                                                 0 = SLI initiated requests are allowed.
                                                                 1 = SLI initiated read and write requests are allowed to
                                                                     KEY_MEM(0..2047) (e.g. 16KB, not all of KEY_MEM) only.
                                                                     SLI requests to any other address (non-KEY_MEM(0..2047))
                                                                     will be redirected to ECAM0_NOP_ZF. */
        uint64_t sli_off               : 1;  /**< [  4:  4](R/W) SLI isolation.
                                                                 Resets to one in trusted mode, else zero.
                                                                 0 = Operation of NCBI transactions are not impacted.
                                                                 1 = NCBI transactions initiating at the SLI are disabled and
                                                                     turn into accesses to ECAM0_NOP_ZF. When set, this bit
                                                                     overrides [SLI_KEY_MEM]. */
        uint64_t tlb_sync_dis          : 1;  /**< [  3:  3](R/W) When set the IOBN will return SYNC-RDY to the SMMU without waiting for
                                                                 outstanding request to receive responses. */
        uint64_t oci_key_only          : 1;  /**< [  2:  2](R/W) Restrict CCPI-sourced I/O write requests.

                                                                 0 = CCPI-sourced I/O read and write requests are allowed to any device through
                                                                 IOB, including allowing read/writes to all of KEY_MEM().

                                                                 1 = CCPI-sourced I/O write requests allowed to KEY_MEM(0..2047) (e.g. 16KB, not
                                                                 all of KEY_MEM) only. CCPI-sourced writes to __any__ other address
                                                                 (non-KEY_MEM(0..2047)), or any CCPI-source read will be redirected to
                                                                 ECAM0_NOP_ZF (for non-ECAM) or ECAM0_NOP_ONNF (for-ECAM).

                                                                 This setting does not affect local-node originated traffic.

                                                                 In pass 1, read-only. */
        uint64_t all_gic               : 1;  /**< [  1:  1](R/W) All-to-GIC. For diagnostic use only.
                                                                 Internal:
                                                                 0 = Normal operation. NCBI traffic to GIC interrupt delivery registers will be ordered
                                                                 with other interrupt delivery traffic and over the RIB bus.  NCBI traffic to normal non-
                                                                 interrupt-delivery GIC registers will go via RSL.
                                                                   1 = All NCBI traffic to the GIC DID will be assumed to be interrupt delivery traffic.
                                                                 This will break NCBI write transactions to non-interrupt-delivery GIC registers, but may
                                                                 work around bugs whereby interrupt-delivery CSRs are mis-catagorized inside IOB. */
        uint64_t ncbi_off              : 1;  /**< [  0:  0](R/W) When set NCBI translation to I/O space (with exception of GIC traffic) will be disabled.
                                                                 Disabled traffic will turn into access to ECAM0_NOP_ZF. */
#else /* Word 0 - Little Endian */
        uint64_t ncbi_off              : 1;  /**< [  0:  0](R/W) When set NCBI translation to I/O space (with exception of GIC traffic) will be disabled.
                                                                 Disabled traffic will turn into access to ECAM0_NOP_ZF. */
        uint64_t all_gic               : 1;  /**< [  1:  1](R/W) All-to-GIC. For diagnostic use only.
                                                                 Internal:
                                                                 0 = Normal operation. NCBI traffic to GIC interrupt delivery registers will be ordered
                                                                 with other interrupt delivery traffic and over the RIB bus.  NCBI traffic to normal non-
                                                                 interrupt-delivery GIC registers will go via RSL.
                                                                   1 = All NCBI traffic to the GIC DID will be assumed to be interrupt delivery traffic.
                                                                 This will break NCBI write transactions to non-interrupt-delivery GIC registers, but may
                                                                 work around bugs whereby interrupt-delivery CSRs are mis-catagorized inside IOB. */
        uint64_t oci_key_only          : 1;  /**< [  2:  2](R/W) Restrict CCPI-sourced I/O write requests.

                                                                 0 = CCPI-sourced I/O read and write requests are allowed to any device through
                                                                 IOB, including allowing read/writes to all of KEY_MEM().

                                                                 1 = CCPI-sourced I/O write requests allowed to KEY_MEM(0..2047) (e.g. 16KB, not
                                                                 all of KEY_MEM) only. CCPI-sourced writes to __any__ other address
                                                                 (non-KEY_MEM(0..2047)), or any CCPI-source read will be redirected to
                                                                 ECAM0_NOP_ZF (for non-ECAM) or ECAM0_NOP_ONNF (for-ECAM).

                                                                 This setting does not affect local-node originated traffic.

                                                                 In pass 1, read-only. */
        uint64_t tlb_sync_dis          : 1;  /**< [  3:  3](R/W) When set the IOBN will return SYNC-RDY to the SMMU without waiting for
                                                                 outstanding request to receive responses. */
        uint64_t sli_off               : 1;  /**< [  4:  4](R/W) SLI isolation.
                                                                 Resets to one in trusted mode, else zero.
                                                                 0 = Operation of NCBI transactions are not impacted.
                                                                 1 = NCBI transactions initiating at the SLI are disabled and
                                                                     turn into accesses to ECAM0_NOP_ZF. When set, this bit
                                                                     overrides [SLI_KEY_MEM]. */
        uint64_t sli_key_mem           : 1;  /**< [  5:  5](R/W) SLI KEY memory isolation.
                                                                 0 = SLI initiated requests are allowed.
                                                                 1 = SLI initiated read and write requests are allowed to
                                                                     KEY_MEM(0..2047) (e.g. 16KB, not all of KEY_MEM) only.
                                                                     SLI requests to any other address (non-KEY_MEM(0..2047))
                                                                     will be redirected to ECAM0_NOP_ZF. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } cn83xx;
    /* struct bdk_iobnx_dis_ncbi_io_cn81xx cn88xxp2; */
};
typedef union bdk_iobnx_dis_ncbi_io bdk_iobnx_dis_ncbi_io_t;

static inline uint64_t BDK_IOBNX_DIS_NCBI_IO(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_DIS_NCBI_IO(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f0003000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f0003000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f0003000ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_DIS_NCBI_IO", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_DIS_NCBI_IO(a) bdk_iobnx_dis_ncbi_io_t
#define bustype_BDK_IOBNX_DIS_NCBI_IO(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_DIS_NCBI_IO(a) "IOBNX_DIS_NCBI_IO"
#define device_bar_BDK_IOBNX_DIS_NCBI_IO(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_DIS_NCBI_IO(a) (a)
#define arguments_BDK_IOBNX_DIS_NCBI_IO(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_dll
 *
 * INTERNAL: IOBN Core-Clock DLL Status Register
 *
 * Status of the CCU core-clock DLL. For diagnostic use only.
 */
union bdk_iobnx_dll
{
    uint64_t u;
    struct bdk_iobnx_dll_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_35_63        : 29;
        uint64_t dbg_window            : 3;  /**< [ 34: 32](R/W/H) Defines a debug window, during which the DLL settings and the phase detector
                                                                 outputs will be monitored. The min and the max DLL setting during that window is
                                                                 going to be reported as well as any illegal phase detector outputs. Every write
                                                                 to the [DBG_WINDOW] resets [ILLEGAL_PD_REVERSED], [ILLEGAL_PD_LATE],
                                                                 [ILLEGAL_PD_EARLY], [MAX_DLL_SETTING] and [MIN_DLL_SETTING]. The debug window
                                                                 will correspond to the following number of rclk cycles based on the [DBG_WINDOW]
                                                                 value.
                                                                 0x0 = Indefinetly.
                                                                 0x1 = 2 ^ 8  core clock cycles.
                                                                 0x2 = 2 ^ 12 core clock cycles.
                                                                 0x3 = 2 ^ 16 core clock cycles.
                                                                 0x4 = 2 ^ 20 core clock cycles.
                                                                 0x5 = 2 ^ 24 core clock cycles.
                                                                 0x6 = 2 ^ 28 core clock cycles.
                                                                 0x7 = 2 ^ 32 core clock cycles. */
        uint64_t dbg_window_done       : 1;  /**< [ 31: 31](RO/H) Indicates if the debug window set by [DBG_WINDOW] is completed. */
        uint64_t illegal_pd_reversed   : 1;  /**< [ 30: 30](RO/H) clk_fast_rgt and clk_fast_lft outputs of the phase detector had concurrently an
                                                                 illegal reading during the last debug window set by [DBG_WINDOW]. */
        uint64_t illegal_pd_late       : 1;  /**< [ 29: 29](RO/H) clk_fast_rgt output of the phase detector had an illegal reading (1) during the
                                                                 last debug window set by [DBG_WINDOW]. */
        uint64_t illegal_pd_early      : 1;  /**< [ 28: 28](RO/H) clk_fast_lft output of the phase detector had an illegal reading (0) during the
                                                                 last debug window set by [DBG_WINDOW]. */
        uint64_t reserved_27           : 1;
        uint64_t max_dll_setting       : 7;  /**< [ 26: 20](RO/H) Max reported DLL setting during the last debug window set by [DBG_WINDOW]. */
        uint64_t reserved_19           : 1;
        uint64_t min_dll_setting       : 7;  /**< [ 18: 12](RO/H) Min reported DLL setting during the last debug window set by [DBG_WINDOW]. */
        uint64_t pd_out                : 3;  /**< [ 11:  9](RO/H) Synchronized output from CCU phase detector:
                                                                 \<11\> = clk_fast_mid.
                                                                 \<10\> = clk_fast_lft.
                                                                 \<9\> = clk_fast_rgt. */
        uint64_t dll_lock              : 1;  /**< [  8:  8](RO/H) The dll_lock signal from ROC core-clock DLL, from the positive edge of refclk. */
        uint64_t reserved_7            : 1;
        uint64_t dll_setting           : 7;  /**< [  6:  0](RO/H) The ROC core-clock DLL setting, from the negative edge of refclk. */
#else /* Word 0 - Little Endian */
        uint64_t dll_setting           : 7;  /**< [  6:  0](RO/H) The ROC core-clock DLL setting, from the negative edge of refclk. */
        uint64_t reserved_7            : 1;
        uint64_t dll_lock              : 1;  /**< [  8:  8](RO/H) The dll_lock signal from ROC core-clock DLL, from the positive edge of refclk. */
        uint64_t pd_out                : 3;  /**< [ 11:  9](RO/H) Synchronized output from CCU phase detector:
                                                                 \<11\> = clk_fast_mid.
                                                                 \<10\> = clk_fast_lft.
                                                                 \<9\> = clk_fast_rgt. */
        uint64_t min_dll_setting       : 7;  /**< [ 18: 12](RO/H) Min reported DLL setting during the last debug window set by [DBG_WINDOW]. */
        uint64_t reserved_19           : 1;
        uint64_t max_dll_setting       : 7;  /**< [ 26: 20](RO/H) Max reported DLL setting during the last debug window set by [DBG_WINDOW]. */
        uint64_t reserved_27           : 1;
        uint64_t illegal_pd_early      : 1;  /**< [ 28: 28](RO/H) clk_fast_lft output of the phase detector had an illegal reading (0) during the
                                                                 last debug window set by [DBG_WINDOW]. */
        uint64_t illegal_pd_late       : 1;  /**< [ 29: 29](RO/H) clk_fast_rgt output of the phase detector had an illegal reading (1) during the
                                                                 last debug window set by [DBG_WINDOW]. */
        uint64_t illegal_pd_reversed   : 1;  /**< [ 30: 30](RO/H) clk_fast_rgt and clk_fast_lft outputs of the phase detector had concurrently an
                                                                 illegal reading during the last debug window set by [DBG_WINDOW]. */
        uint64_t dbg_window_done       : 1;  /**< [ 31: 31](RO/H) Indicates if the debug window set by [DBG_WINDOW] is completed. */
        uint64_t dbg_window            : 3;  /**< [ 34: 32](R/W/H) Defines a debug window, during which the DLL settings and the phase detector
                                                                 outputs will be monitored. The min and the max DLL setting during that window is
                                                                 going to be reported as well as any illegal phase detector outputs. Every write
                                                                 to the [DBG_WINDOW] resets [ILLEGAL_PD_REVERSED], [ILLEGAL_PD_LATE],
                                                                 [ILLEGAL_PD_EARLY], [MAX_DLL_SETTING] and [MIN_DLL_SETTING]. The debug window
                                                                 will correspond to the following number of rclk cycles based on the [DBG_WINDOW]
                                                                 value.
                                                                 0x0 = Indefinetly.
                                                                 0x1 = 2 ^ 8  core clock cycles.
                                                                 0x2 = 2 ^ 12 core clock cycles.
                                                                 0x3 = 2 ^ 16 core clock cycles.
                                                                 0x4 = 2 ^ 20 core clock cycles.
                                                                 0x5 = 2 ^ 24 core clock cycles.
                                                                 0x6 = 2 ^ 28 core clock cycles.
                                                                 0x7 = 2 ^ 32 core clock cycles. */
        uint64_t reserved_35_63        : 29;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_dll_s cn; */
};
typedef union bdk_iobnx_dll bdk_iobnx_dll_t;

static inline uint64_t BDK_IOBNX_DLL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_DLL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f0003040ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_DLL", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_DLL(a) bdk_iobnx_dll_t
#define bustype_BDK_IOBNX_DLL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_DLL(a) "IOBNX_DLL"
#define device_bar_BDK_IOBNX_DLL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_DLL(a) (a)
#define arguments_BDK_IOBNX_DLL(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_dom#_bus#_streams
 *
 * IOBN Domain Bus Permit Registers
 * This register sets the permissions for a NCBI transaction (which are DMA
 * transactions or MSI-X writes), for requests for NCB device virtual-functions
 * and bridges.
 *
 * Index {b} corresponds to the stream's domain (stream_id\<21:16\>).
 *
 * Index {c} corresponds to the stream's bus number (stream_id\<15:8\>).
 *
 * For each combination of index {b} and {c}, each index {a} (the IOB number) must be
 * programmed to the same value.
 *
 * Streams which hit index {c}=0x0 are also affected by IOBN()_DOM()_DEV()_STREAMS.
 * Streams which hit index {b}=PCC_DEV_CON_E::MRML\<21:16\>,
 * {c}=PCC_DEV_CON_E::MRML\<15:8\> are also affected by IOBN()_RSL()_STREAMS.
 * Both of those alternative registers provide better granularity, so those indices
 * into this register should be left permissive (value of 0x0).
 */
union bdk_iobnx_domx_busx_streams
{
    uint64_t u;
    struct bdk_iobnx_domx_busx_streams_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t strm_nsec             : 1;  /**< [  1:  1](SR/W) Stream nonsecure.

                                                                   0 = The device's stream ID is marked secure headed into the SMMU.  If the
                                                                   device is making a non-physical request, the SMMU will use secure world
                                                                   lookup. The SMMU may, if properly configured, generate an outgoing physical
                                                                   address that is secure.

                                                                   1 = The device's stream ID is marked nonsecure headed into the SMMU.  If the
                                                                   device is making a non-physical request, the SMMU will use nonsecure world
                                                                   lookup. The SMMU outgoing physical address will be nonsecure.

                                                                   [STRM_NSEC] is ignored if the device is making a physical request (as these
                                                                   transactions bypass the SMMU translation process).

                                                                   Typically firmware sets [PHYS_NSEC] and [STRM_NSEC] the same. */
        uint64_t phys_nsec             : 1;  /**< [  0:  0](SR/W) Physical nonsecure.
                                                                   0 = When the device makes a physical request, IOB will use the device's
                                                                   requested secure bit to determine if the request to DRAM/LLC is secure or not.
                                                                   1 = When the device makes a physical request, IOB will squash the
                                                                   device's secure request and issue the request to DRAM/LLC as nonsecure.

                                                                   Ignored if a device makes a non-physical request.  (As non-physical requests
                                                                   cause the SMMU to generate the SMMU-outgoing secure bit based on the SMMU
                                                                   translation process, including [STRM_NSEC].)

                                                                   Typically firmware sets [PHYS_NSEC] and [STRM_NSEC] the same. */
#else /* Word 0 - Little Endian */
        uint64_t phys_nsec             : 1;  /**< [  0:  0](SR/W) Physical nonsecure.
                                                                   0 = When the device makes a physical request, IOB will use the device's
                                                                   requested secure bit to determine if the request to DRAM/LLC is secure or not.
                                                                   1 = When the device makes a physical request, IOB will squash the
                                                                   device's secure request and issue the request to DRAM/LLC as nonsecure.

                                                                   Ignored if a device makes a non-physical request.  (As non-physical requests
                                                                   cause the SMMU to generate the SMMU-outgoing secure bit based on the SMMU
                                                                   translation process, including [STRM_NSEC].)

                                                                   Typically firmware sets [PHYS_NSEC] and [STRM_NSEC] the same. */
        uint64_t strm_nsec             : 1;  /**< [  1:  1](SR/W) Stream nonsecure.

                                                                   0 = The device's stream ID is marked secure headed into the SMMU.  If the
                                                                   device is making a non-physical request, the SMMU will use secure world
                                                                   lookup. The SMMU may, if properly configured, generate an outgoing physical
                                                                   address that is secure.

                                                                   1 = The device's stream ID is marked nonsecure headed into the SMMU.  If the
                                                                   device is making a non-physical request, the SMMU will use nonsecure world
                                                                   lookup. The SMMU outgoing physical address will be nonsecure.

                                                                   [STRM_NSEC] is ignored if the device is making a physical request (as these
                                                                   transactions bypass the SMMU translation process).

                                                                   Typically firmware sets [PHYS_NSEC] and [STRM_NSEC] the same. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_domx_busx_streams_s cn; */
};
typedef union bdk_iobnx_domx_busx_streams bdk_iobnx_domx_busx_streams_t;

static inline uint64_t BDK_IOBNX_DOMX_BUSX_STREAMS(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_DOMX_BUSX_STREAMS(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=6) && (c<=255)))
        return 0x87e0f0040000ll + 0x1000000ll * ((a) & 0x1) + 0x800ll * ((b) & 0x7) + 8ll * ((c) & 0xff);
    __bdk_csr_fatal("IOBNX_DOMX_BUSX_STREAMS", 3, a, b, c, 0);
}

#define typedef_BDK_IOBNX_DOMX_BUSX_STREAMS(a,b,c) bdk_iobnx_domx_busx_streams_t
#define bustype_BDK_IOBNX_DOMX_BUSX_STREAMS(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_DOMX_BUSX_STREAMS(a,b,c) "IOBNX_DOMX_BUSX_STREAMS"
#define device_bar_BDK_IOBNX_DOMX_BUSX_STREAMS(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_DOMX_BUSX_STREAMS(a,b,c) (a)
#define arguments_BDK_IOBNX_DOMX_BUSX_STREAMS(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) iobn#_dom#_dev#_streams
 *
 * IOBN Device Bus Permit Registers
 * This register sets the permissions for a NCBI transaction (which are DMA
 * transactions or MSI-X writes), for requests for NCB device physicical-functions,
 * i.e. those where:
 *
 *   _ stream_id\<15:8\> = 0x0.
 *
 * Index {a} corresponds to the stream's domain number (stream_id\<21:16\>).
 *
 * Index {b} corresponds to the non-ARI ECAM device number (stream_id\<7:3\>).
 *
 * For each combination of index {b} and {c}, each index {a} (the IOB number) must be
 * programmed to the same value.
 */
union bdk_iobnx_domx_devx_streams
{
    uint64_t u;
    struct bdk_iobnx_domx_devx_streams_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t strm_nsec             : 1;  /**< [  1:  1](SR/W) Stream nonsecure.

                                                                   0 = The device's stream ID is marked secure headed into the SMMU.  If the
                                                                   device is making a non-physical request, the SMMU will use secure world
                                                                   lookup. The SMMU may, if properly configured, generate an outgoing physical
                                                                   address that is secure.

                                                                   1 = The device's stream ID is marked nonsecure headed into the SMMU.  If the
                                                                   device is making a non-physical request, the SMMU will use nonsecure world
                                                                   lookup. The SMMU outgoing physical address will be nonsecure.

                                                                   [STRM_NSEC] is ignored if the device is making a physical request (as these
                                                                   transactions bypass the SMMU translation process).

                                                                   Typically firmware sets [PHYS_NSEC] and [STRM_NSEC] the same. */
        uint64_t phys_nsec             : 1;  /**< [  0:  0](SR/W) Physical nonsecure.
                                                                   0 = When the device makes a physical request, IOB will use the device's
                                                                   requested secure bit to determine if the request to DRAM/LLC is secure or not.
                                                                   1 = When the device makes a physical request, IOB will squash the
                                                                   device's secure request and issue the request to DRAM/LLC as nonsecure.

                                                                   Ignored if a device makes a non-physical request.  (As non-physical requests
                                                                   cause the SMMU to generate the SMMU-outgoing secure bit based on the SMMU
                                                                   translation process, including [STRM_NSEC].)

                                                                   Typically firmware sets [PHYS_NSEC] and [STRM_NSEC] the same. */
#else /* Word 0 - Little Endian */
        uint64_t phys_nsec             : 1;  /**< [  0:  0](SR/W) Physical nonsecure.
                                                                   0 = When the device makes a physical request, IOB will use the device's
                                                                   requested secure bit to determine if the request to DRAM/LLC is secure or not.
                                                                   1 = When the device makes a physical request, IOB will squash the
                                                                   device's secure request and issue the request to DRAM/LLC as nonsecure.

                                                                   Ignored if a device makes a non-physical request.  (As non-physical requests
                                                                   cause the SMMU to generate the SMMU-outgoing secure bit based on the SMMU
                                                                   translation process, including [STRM_NSEC].)

                                                                   Typically firmware sets [PHYS_NSEC] and [STRM_NSEC] the same. */
        uint64_t strm_nsec             : 1;  /**< [  1:  1](SR/W) Stream nonsecure.

                                                                   0 = The device's stream ID is marked secure headed into the SMMU.  If the
                                                                   device is making a non-physical request, the SMMU will use secure world
                                                                   lookup. The SMMU may, if properly configured, generate an outgoing physical
                                                                   address that is secure.

                                                                   1 = The device's stream ID is marked nonsecure headed into the SMMU.  If the
                                                                   device is making a non-physical request, the SMMU will use nonsecure world
                                                                   lookup. The SMMU outgoing physical address will be nonsecure.

                                                                   [STRM_NSEC] is ignored if the device is making a physical request (as these
                                                                   transactions bypass the SMMU translation process).

                                                                   Typically firmware sets [PHYS_NSEC] and [STRM_NSEC] the same. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_domx_devx_streams_s cn; */
};
typedef union bdk_iobnx_domx_devx_streams bdk_iobnx_domx_devx_streams_t;

static inline uint64_t BDK_IOBNX_DOMX_DEVX_STREAMS(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_DOMX_DEVX_STREAMS(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=6) && (c<=31)))
        return 0x87e0f0010000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x7) + 8ll * ((c) & 0x1f);
    __bdk_csr_fatal("IOBNX_DOMX_DEVX_STREAMS", 3, a, b, c, 0);
}

#define typedef_BDK_IOBNX_DOMX_DEVX_STREAMS(a,b,c) bdk_iobnx_domx_devx_streams_t
#define bustype_BDK_IOBNX_DOMX_DEVX_STREAMS(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_DOMX_DEVX_STREAMS(a,b,c) "IOBNX_DOMX_DEVX_STREAMS"
#define device_bar_BDK_IOBNX_DOMX_DEVX_STREAMS(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_DOMX_DEVX_STREAMS(a,b,c) (a)
#define arguments_BDK_IOBNX_DOMX_DEVX_STREAMS(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) iobn#_gbl_dll
 *
 * INTERNAL: IOBN Global Core-Clock DLL Status Register
 *
 * Status of the global core-clock DLL.
 */
union bdk_iobnx_gbl_dll
{
    uint64_t u;
    struct bdk_iobnx_gbl_dll_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_20_63        : 44;
        uint64_t pdr_rclk_refclk       : 1;  /**< [ 19: 19](RO/H) Synchronized pdr_rclk_refclk from global core-clock DLL cmb0 phase detectors. */
        uint64_t pdl_rclk_refclk       : 1;  /**< [ 18: 18](RO/H) Synchronized pdl_rclk_refclk from global core-clock DLL cmb0 phase detectors. */
        uint64_t pd_pos_rclk_refclk    : 1;  /**< [ 17: 17](RO/H) Synchronized pd_pos_rclk_refclk from global core-clock DLL cmb0 phase detectors. */
        uint64_t dll_fsm_state_a       : 3;  /**< [ 16: 14](RO/H) State for the global core-clock DLL, from the positive edge of refclk.
                                                                 0x0 = TMD_IDLE.
                                                                 0x1 = TMD_STATE1.
                                                                 0x2 = TMD_STATE2.
                                                                 0x3 = TMD_STATE3.
                                                                 0x4 = TMD_STATE4.
                                                                 0x5 = TMD_LOCKED. */
        uint64_t dll_lock              : 1;  /**< [ 13: 13](RO/H) The dll_lock signal from global core-clock DLL, from the positive edge of refclk. */
        uint64_t dll_clk_invert_out    : 1;  /**< [ 12: 12](RO/H) The clk_invert setting from the global core-clock DLL, from the negative edge of refclk. */
        uint64_t dll_setting           : 12; /**< [ 11:  0](RO/H) The global core-clock DLL setting, from the negative edge of refclk. */
#else /* Word 0 - Little Endian */
        uint64_t dll_setting           : 12; /**< [ 11:  0](RO/H) The global core-clock DLL setting, from the negative edge of refclk. */
        uint64_t dll_clk_invert_out    : 1;  /**< [ 12: 12](RO/H) The clk_invert setting from the global core-clock DLL, from the negative edge of refclk. */
        uint64_t dll_lock              : 1;  /**< [ 13: 13](RO/H) The dll_lock signal from global core-clock DLL, from the positive edge of refclk. */
        uint64_t dll_fsm_state_a       : 3;  /**< [ 16: 14](RO/H) State for the global core-clock DLL, from the positive edge of refclk.
                                                                 0x0 = TMD_IDLE.
                                                                 0x1 = TMD_STATE1.
                                                                 0x2 = TMD_STATE2.
                                                                 0x3 = TMD_STATE3.
                                                                 0x4 = TMD_STATE4.
                                                                 0x5 = TMD_LOCKED. */
        uint64_t pd_pos_rclk_refclk    : 1;  /**< [ 17: 17](RO/H) Synchronized pd_pos_rclk_refclk from global core-clock DLL cmb0 phase detectors. */
        uint64_t pdl_rclk_refclk       : 1;  /**< [ 18: 18](RO/H) Synchronized pdl_rclk_refclk from global core-clock DLL cmb0 phase detectors. */
        uint64_t pdr_rclk_refclk       : 1;  /**< [ 19: 19](RO/H) Synchronized pdr_rclk_refclk from global core-clock DLL cmb0 phase detectors. */
        uint64_t reserved_20_63        : 44;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_gbl_dll_s cn; */
};
typedef union bdk_iobnx_gbl_dll bdk_iobnx_gbl_dll_t;

static inline uint64_t BDK_IOBNX_GBL_DLL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_GBL_DLL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f000a000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f000a000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f000a000ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_GBL_DLL", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_GBL_DLL(a) bdk_iobnx_gbl_dll_t
#define bustype_BDK_IOBNX_GBL_DLL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_GBL_DLL(a) "IOBNX_GBL_DLL"
#define device_bar_BDK_IOBNX_GBL_DLL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_GBL_DLL(a) (a)
#define arguments_BDK_IOBNX_GBL_DLL(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_int1
 *
 * IOBN Interrupt Summary Register
 * This register contains the different interrupt-summary bits of the IOBN.
 */
union bdk_iobnx_int1
{
    uint64_t u;
    struct bdk_iobnx_int1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t gibm_f                : 1;  /**< [  0:  0](R/W1C/H) GIBM on NCB0 received a STDN with fault. */
#else /* Word 0 - Little Endian */
        uint64_t gibm_f                : 1;  /**< [  0:  0](R/W1C/H) GIBM on NCB0 received a STDN with fault. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_int1_s cn; */
};
typedef union bdk_iobnx_int1 bdk_iobnx_int1_t;

static inline uint64_t BDK_IOBNX_INT1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_INT1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f000a000ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_INT1", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_INT1(a) bdk_iobnx_int1_t
#define bustype_BDK_IOBNX_INT1(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_INT1(a) "IOBNX_INT1"
#define device_bar_BDK_IOBNX_INT1(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_INT1(a) (a)
#define arguments_BDK_IOBNX_INT1(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_int1_ena_w1c
 *
 * IOBN Interrupt Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_iobnx_int1_ena_w1c
{
    uint64_t u;
    struct bdk_iobnx_int1_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t gibm_f                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT1[GIBM_F]. */
#else /* Word 0 - Little Endian */
        uint64_t gibm_f                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT1[GIBM_F]. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_int1_ena_w1c_s cn; */
};
typedef union bdk_iobnx_int1_ena_w1c bdk_iobnx_int1_ena_w1c_t;

static inline uint64_t BDK_IOBNX_INT1_ENA_W1C(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_INT1_ENA_W1C(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f000a010ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_INT1_ENA_W1C", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_INT1_ENA_W1C(a) bdk_iobnx_int1_ena_w1c_t
#define bustype_BDK_IOBNX_INT1_ENA_W1C(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_INT1_ENA_W1C(a) "IOBNX_INT1_ENA_W1C"
#define device_bar_BDK_IOBNX_INT1_ENA_W1C(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_INT1_ENA_W1C(a) (a)
#define arguments_BDK_IOBNX_INT1_ENA_W1C(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_int1_ena_w1s
 *
 * IOBN Interrupt Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_iobnx_int1_ena_w1s
{
    uint64_t u;
    struct bdk_iobnx_int1_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t gibm_f                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT1[GIBM_F]. */
#else /* Word 0 - Little Endian */
        uint64_t gibm_f                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT1[GIBM_F]. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_int1_ena_w1s_s cn; */
};
typedef union bdk_iobnx_int1_ena_w1s bdk_iobnx_int1_ena_w1s_t;

static inline uint64_t BDK_IOBNX_INT1_ENA_W1S(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_INT1_ENA_W1S(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f000a018ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_INT1_ENA_W1S", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_INT1_ENA_W1S(a) bdk_iobnx_int1_ena_w1s_t
#define bustype_BDK_IOBNX_INT1_ENA_W1S(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_INT1_ENA_W1S(a) "IOBNX_INT1_ENA_W1S"
#define device_bar_BDK_IOBNX_INT1_ENA_W1S(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_INT1_ENA_W1S(a) (a)
#define arguments_BDK_IOBNX_INT1_ENA_W1S(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_int1_w1s
 *
 * IOBN Interrupt Set Register
 * This register sets interrupt bits.
 */
union bdk_iobnx_int1_w1s
{
    uint64_t u;
    struct bdk_iobnx_int1_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t gibm_f                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets IOBN(0..1)_INT1[GIBM_F]. */
#else /* Word 0 - Little Endian */
        uint64_t gibm_f                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets IOBN(0..1)_INT1[GIBM_F]. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_int1_w1s_s cn; */
};
typedef union bdk_iobnx_int1_w1s bdk_iobnx_int1_w1s_t;

static inline uint64_t BDK_IOBNX_INT1_W1S(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_INT1_W1S(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f000a008ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_INT1_W1S", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_INT1_W1S(a) bdk_iobnx_int1_w1s_t
#define bustype_BDK_IOBNX_INT1_W1S(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_INT1_W1S(a) "IOBNX_INT1_W1S"
#define device_bar_BDK_IOBNX_INT1_W1S(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_INT1_W1S(a) (a)
#define arguments_BDK_IOBNX_INT1_W1S(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_int_ena_w1c
 *
 * IOBN Interrupt Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_iobnx_int_ena_w1c
{
    uint64_t u;
    struct bdk_iobnx_int_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[PEM_SIE]. */
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_31         : 32;
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[PEM_SIE]. */
#endif /* Word 0 - End */
    } s;
    struct bdk_iobnx_int_ena_w1c_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_60_63        : 4;
        uint64_t ied0_dbe              : 28; /**< [ 59: 32](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_28_31        : 4;
        uint64_t ied0_sbe              : 28; /**< [ 27:  0](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 28; /**< [ 27:  0](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_28_31        : 4;
        uint64_t ied0_dbe              : 28; /**< [ 59: 32](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_60_63        : 4;
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_iobnx_int_ena_w1c_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t ncbo_ncb2_psn         : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[NCBO_NCB2_PSN]. */
        uint64_t ncbo_ncb1_psn         : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[NCBO_NCB1_PSN]. */
        uint64_t ncbo_ncb0_psn         : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[NCBO_NCB0_PSN]. */
        uint64_t ncbo_to               : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[NCBO_TO]. */
#else /* Word 0 - Little Endian */
        uint64_t ncbo_to               : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[NCBO_TO]. */
        uint64_t ncbo_ncb0_psn         : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[NCBO_NCB0_PSN]. */
        uint64_t ncbo_ncb1_psn         : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[NCBO_NCB1_PSN]. */
        uint64_t ncbo_ncb2_psn         : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[NCBO_NCB2_PSN]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn9;
    struct bdk_iobnx_int_ena_w1c_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) Reads or clears enable for IOBN(0)_INT_SUM[PEM_SIE]. */
        uint64_t reserved_61_62        : 2;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1C/H) Reads or clears enable for IOBN(0)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1C/H) Reads or clears enable for IOBN(0)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1C/H) Reads or clears enable for IOBN(0)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1C/H) Reads or clears enable for IOBN(0)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_61_62        : 2;
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) Reads or clears enable for IOBN(0)_INT_SUM[PEM_SIE]. */
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_iobnx_int_ena_w1c_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[PEM_SIE]. */
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      sli_preq_3_dbe_sclk,
                                                                      sli_req_3_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_31           : 1;
        uint64_t ied0_sbe              : 31; /**< [ 30:  0](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       sli_preq_3_sbe_sclk,
                                                                       sli_req_3_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 31; /**< [ 30:  0](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       sli_preq_3_sbe_sclk,
                                                                       sli_req_3_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_31           : 1;
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      sli_preq_3_dbe_sclk,
                                                                      sli_req_3_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[PEM_SIE]. */
#endif /* Word 0 - End */
    } cn83xx;
    struct bdk_iobnx_int_ena_w1c_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[PEM_SIE]. */
        uint64_t reserved_61_62        : 2;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_61_62        : 2;
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) Reads or clears enable for IOBN(0..1)_INT_SUM[PEM_SIE]. */
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_iobnx_int_ena_w1c bdk_iobnx_int_ena_w1c_t;

static inline uint64_t BDK_IOBNX_INT_ENA_W1C(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_INT_ENA_W1C(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f0008000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f0008000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f0008000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f0008000ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_INT_ENA_W1C", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_INT_ENA_W1C(a) bdk_iobnx_int_ena_w1c_t
#define bustype_BDK_IOBNX_INT_ENA_W1C(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_INT_ENA_W1C(a) "IOBNX_INT_ENA_W1C"
#define device_bar_BDK_IOBNX_INT_ENA_W1C(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_INT_ENA_W1C(a) (a)
#define arguments_BDK_IOBNX_INT_ENA_W1C(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_int_ena_w1s
 *
 * IOBN Interrupt Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_iobnx_int_ena_w1s
{
    uint64_t u;
    struct bdk_iobnx_int_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[PEM_SIE]. */
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_31         : 32;
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[PEM_SIE]. */
#endif /* Word 0 - End */
    } s;
    struct bdk_iobnx_int_ena_w1s_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_60_63        : 4;
        uint64_t ied0_dbe              : 28; /**< [ 59: 32](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_28_31        : 4;
        uint64_t ied0_sbe              : 28; /**< [ 27:  0](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 28; /**< [ 27:  0](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_28_31        : 4;
        uint64_t ied0_dbe              : 28; /**< [ 59: 32](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_60_63        : 4;
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_iobnx_int_ena_w1s_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t ncbo_ncb2_psn         : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[NCBO_NCB2_PSN]. */
        uint64_t ncbo_ncb1_psn         : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[NCBO_NCB1_PSN]. */
        uint64_t ncbo_ncb0_psn         : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[NCBO_NCB0_PSN]. */
        uint64_t ncbo_to               : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[NCBO_TO]. */
#else /* Word 0 - Little Endian */
        uint64_t ncbo_to               : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[NCBO_TO]. */
        uint64_t ncbo_ncb0_psn         : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[NCBO_NCB0_PSN]. */
        uint64_t ncbo_ncb1_psn         : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[NCBO_NCB1_PSN]. */
        uint64_t ncbo_ncb2_psn         : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[NCBO_NCB2_PSN]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn9;
    struct bdk_iobnx_int_ena_w1s_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets enable for IOBN(0)_INT_SUM[PEM_SIE]. */
        uint64_t reserved_61_62        : 2;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1S/H) Reads or sets enable for IOBN(0)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1S/H) Reads or sets enable for IOBN(0)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1S/H) Reads or sets enable for IOBN(0)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1S/H) Reads or sets enable for IOBN(0)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_61_62        : 2;
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets enable for IOBN(0)_INT_SUM[PEM_SIE]. */
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_iobnx_int_ena_w1s_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[PEM_SIE]. */
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      sli_preq_3_dbe_sclk,
                                                                      sli_req_3_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_31           : 1;
        uint64_t ied0_sbe              : 31; /**< [ 30:  0](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       sli_preq_3_sbe_sclk,
                                                                       sli_req_3_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 31; /**< [ 30:  0](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       sli_preq_3_sbe_sclk,
                                                                       sli_req_3_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_31           : 1;
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      sli_preq_3_dbe_sclk,
                                                                      sli_req_3_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[PEM_SIE]. */
#endif /* Word 0 - End */
    } cn83xx;
    struct bdk_iobnx_int_ena_w1s_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[PEM_SIE]. */
        uint64_t reserved_61_62        : 2;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_61_62        : 2;
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets enable for IOBN(0..1)_INT_SUM[PEM_SIE]. */
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_iobnx_int_ena_w1s bdk_iobnx_int_ena_w1s_t;

static inline uint64_t BDK_IOBNX_INT_ENA_W1S(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_INT_ENA_W1S(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f0009000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f0009000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f0009000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f0009000ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_INT_ENA_W1S", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_INT_ENA_W1S(a) bdk_iobnx_int_ena_w1s_t
#define bustype_BDK_IOBNX_INT_ENA_W1S(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_INT_ENA_W1S(a) "IOBNX_INT_ENA_W1S"
#define device_bar_BDK_IOBNX_INT_ENA_W1S(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_INT_ENA_W1S(a) (a)
#define arguments_BDK_IOBNX_INT_ENA_W1S(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_int_sum
 *
 * IOBN Interrupt Summary Register
 * This register contains the different interrupt-summary bits of the IOBN.
 */
union bdk_iobnx_int_sum
{
    uint64_t u;
    struct bdk_iobnx_int_sum_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) PEM sent in an invalid stream ID, the transaction was returned with fault. Advisory
                                                                 notification only. */
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1C/H) IED0 double-bit error. When set, an IED0 double-bit error has occurred.
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_31         : 32;
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1C/H) IED0 double-bit error. When set, an IED0 double-bit error has occurred.
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) PEM sent in an invalid stream ID, the transaction was returned with fault. Advisory
                                                                 notification only. */
#endif /* Word 0 - End */
    } s;
    struct bdk_iobnx_int_sum_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_60_63        : 4;
        uint64_t ied0_dbe              : 28; /**< [ 59: 32](R/W1C/H) IED0 double-bit error. When set, an IED0 double-bit error has occurred.
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_28_31        : 4;
        uint64_t ied0_sbe              : 28; /**< [ 27:  0](R/W1C/H) IED0 single-bit error. When set, an IED0 single-bit error has occurred.
                                                                 Internal:
                                                                 gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 28; /**< [ 27:  0](R/W1C/H) IED0 single-bit error. When set, an IED0 single-bit error has occurred.
                                                                 Internal:
                                                                 gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_28_31        : 4;
        uint64_t ied0_dbe              : 28; /**< [ 59: 32](R/W1C/H) IED0 double-bit error. When set, an IED0 double-bit error has occurred.
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_60_63        : 4;
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_iobnx_int_sum_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t ncbo_ncb2_psn         : 1;  /**< [  3:  3](R/W1C/H) NCBO bus2 store data with poison. IOBN()_NCBO()_PSN_STATUS saves the first error information. */
        uint64_t ncbo_ncb1_psn         : 1;  /**< [  2:  2](R/W1C/H) NCB1 bus0 store data with poison. IOBN()_NCBO()_PSN_STATUS saves the first error information. */
        uint64_t ncbo_ncb0_psn         : 1;  /**< [  1:  1](R/W1C/H) NCBO bus0 store data with poison. IOBN()_NCBO()_PSN_STATUS saves the first error information. */
        uint64_t ncbo_to               : 1;  /**< [  0:  0](R/W1C/H) NPR to a NCB-DEVICE has timed out. See IOBN()_NCBO_TO[SUB_TIME]. */
#else /* Word 0 - Little Endian */
        uint64_t ncbo_to               : 1;  /**< [  0:  0](R/W1C/H) NPR to a NCB-DEVICE has timed out. See IOBN()_NCBO_TO[SUB_TIME]. */
        uint64_t ncbo_ncb0_psn         : 1;  /**< [  1:  1](R/W1C/H) NCBO bus0 store data with poison. IOBN()_NCBO()_PSN_STATUS saves the first error information. */
        uint64_t ncbo_ncb1_psn         : 1;  /**< [  2:  2](R/W1C/H) NCB1 bus0 store data with poison. IOBN()_NCBO()_PSN_STATUS saves the first error information. */
        uint64_t ncbo_ncb2_psn         : 1;  /**< [  3:  3](R/W1C/H) NCBO bus2 store data with poison. IOBN()_NCBO()_PSN_STATUS saves the first error information. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn9;
    struct bdk_iobnx_int_sum_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) PEM sent in an invalid stream ID, the transaction was returned with fault. Advisory
                                                                 notification only. */
        uint64_t reserved_61_62        : 2;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1C/H) IED0 double-bit error. When set, an IED0 double-bit error has occurred.
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1C/H) IED0 single-bit error. When set, an IED0 single-bit error has occurred.
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1C/H) IED0 single-bit error. When set, an IED0 single-bit error has occurred.
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1C/H) IED0 double-bit error. When set, an IED0 double-bit error has occurred.
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_61_62        : 2;
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) PEM sent in an invalid stream ID, the transaction was returned with fault. Advisory
                                                                 notification only. */
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_iobnx_int_sum_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) PEM sent in an invalid stream ID, the transaction was returned with fault. Advisory
                                                                 notification only. */
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1C/H) IED0 double-bit error. When set, an IED0 double-bit error has occurred.
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      sli_preq_3_dbe_sclk,
                                                                      sli_req_3_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_31           : 1;
        uint64_t ied0_sbe              : 31; /**< [ 30:  0](R/W1C/H) IED0 single-bit error. When set, an IED0 single-bit error has occurred.
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       sli_preq_3_sbe_sclk,
                                                                       sli_req_3_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 31; /**< [ 30:  0](R/W1C/H) IED0 single-bit error. When set, an IED0 single-bit error has occurred.
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       sli_preq_3_sbe_sclk,
                                                                       sli_req_3_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_31           : 1;
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1C/H) IED0 double-bit error. When set, an IED0 double-bit error has occurred.
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      sli_preq_3_dbe_sclk,
                                                                      sli_req_3_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) PEM sent in an invalid stream ID, the transaction was returned with fault. Advisory
                                                                 notification only. */
#endif /* Word 0 - End */
    } cn83xx;
    struct bdk_iobnx_int_sum_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) PEM sent in an invalid stream ID. */
        uint64_t reserved_61_62        : 2;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1C/H) IED0 double-bit error. When set, an IED0 double-bit error has occurred.
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1C/H) IED0 single-bit error. When set, an IED0 single-bit error has occurred.
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1C/H) IED0 single-bit error. When set, an IED0 single-bit error has occurred.
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1C/H) IED0 double-bit error. When set, an IED0 double-bit error has occurred.
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_61_62        : 2;
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1C/H) PEM sent in an invalid stream ID. */
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_iobnx_int_sum bdk_iobnx_int_sum_t;

static inline uint64_t BDK_IOBNX_INT_SUM(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_INT_SUM(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f0006000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f0006000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f0006000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f0006000ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_INT_SUM", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_INT_SUM(a) bdk_iobnx_int_sum_t
#define bustype_BDK_IOBNX_INT_SUM(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_INT_SUM(a) "IOBNX_INT_SUM"
#define device_bar_BDK_IOBNX_INT_SUM(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_INT_SUM(a) (a)
#define arguments_BDK_IOBNX_INT_SUM(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_int_sum_w1s
 *
 * IOBN Interrupt Set Register
 * This register sets interrupt bits.
 */
union bdk_iobnx_int_sum_w1s
{
    uint64_t u;
    struct bdk_iobnx_int_sum_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[PEM_SIE]. */
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_31         : 32;
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[PEM_SIE]. */
#endif /* Word 0 - End */
    } s;
    struct bdk_iobnx_int_sum_w1s_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_60_63        : 4;
        uint64_t ied0_dbe              : 28; /**< [ 59: 32](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_28_31        : 4;
        uint64_t ied0_sbe              : 28; /**< [ 27:  0](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 28; /**< [ 27:  0](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_28_31        : 4;
        uint64_t ied0_dbe              : 28; /**< [ 59: 32](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_60_63        : 4;
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_iobnx_int_sum_w1s_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t ncbo_ncb2_psn         : 1;  /**< [  3:  3](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[NCBO_NCB2_PSN]. */
        uint64_t ncbo_ncb1_psn         : 1;  /**< [  2:  2](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[NCBO_NCB1_PSN]. */
        uint64_t ncbo_ncb0_psn         : 1;  /**< [  1:  1](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[NCBO_NCB0_PSN]. */
        uint64_t ncbo_to               : 1;  /**< [  0:  0](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[NCBO_TO]. */
#else /* Word 0 - Little Endian */
        uint64_t ncbo_to               : 1;  /**< [  0:  0](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[NCBO_TO]. */
        uint64_t ncbo_ncb0_psn         : 1;  /**< [  1:  1](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[NCBO_NCB0_PSN]. */
        uint64_t ncbo_ncb1_psn         : 1;  /**< [  2:  2](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[NCBO_NCB1_PSN]. */
        uint64_t ncbo_ncb2_psn         : 1;  /**< [  3:  3](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[NCBO_NCB2_PSN]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn9;
    struct bdk_iobnx_int_sum_w1s_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets IOBN(0)_INT_SUM[PEM_SIE]. */
        uint64_t reserved_61_62        : 2;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1S/H) Reads or sets IOBN(0)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1S/H) Reads or sets IOBN(0)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1S/H) Reads or sets IOBN(0)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1S/H) Reads or sets IOBN(0)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_61_62        : 2;
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets IOBN(0)_INT_SUM[PEM_SIE]. */
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_iobnx_int_sum_w1s_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[PEM_SIE]. */
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      sli_preq_3_dbe_sclk,
                                                                      sli_req_3_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_31           : 1;
        uint64_t ied0_sbe              : 31; /**< [ 30:  0](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       sli_preq_3_sbe_sclk,
                                                                       sli_req_3_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 31; /**< [ 30:  0](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       sli_preq_3_sbe_sclk,
                                                                       sli_req_3_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_31           : 1;
        uint64_t ied0_dbe              : 31; /**< [ 62: 32](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      sli_preq_3_dbe_sclk,
                                                                      sli_req_3_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[PEM_SIE]. */
#endif /* Word 0 - End */
    } cn83xx;
    struct bdk_iobnx_int_sum_w1s_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[PEM_SIE]. */
        uint64_t reserved_61_62        : 2;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
#else /* Word 0 - Little Endian */
        uint64_t ied0_sbe              : 29; /**< [ 28:  0](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[IED0_SBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                       gmr_ixofifo_sbe_sclk,
                                                                       icc0_xmc_fif_sbe,
                                                                       icc1_xmc_fif_sbe,
                                                                       icc_xmc_fifo_ecc_sbe,
                                                                       sli_preq_0_sbe_sclk,
                                                                       sli_req_0_sbe_sclk,
                                                                       sli_preq_1_sbe_sclk,
                                                                       sli_req_1_sbe_sclk,
                                                                       sli_preq_2_sbe_sclk,
                                                                       sli_req_2_sbe_sclk,
                                                                       ixo_smmu_mem0_sbe_sclk,
                                                                       iop_breq_fifo0_sbe,
                                                                       iop_breq_fifo1_sbe ,
                                                                       iop_breq_fifo2_sbe,
                                                                       iop_breq_fifo3_sbe ,
                                                                       iop_ffifo_sbe_sclk,
                                                                       rsd_mem0_sbe,
                                                                       rsd_mem1_sbe,
                                                                       ics_cmd_fifo_sbe_sclk,
                                                                       ixo_xmd_mem1_sbe_sclk,
                                                                       ixo_xmd_mem0_sbe_sclk,
                                                                       iobn_iorn_ffifo0__sbe_sclk,
                                                                       iobn_iorn_ffifo1__sbe_sclk,
                                                                       irp1_flid_mem_sbe,
                                                                       irp0_flid_mem_sbe,
                                                                       ixo_icc_fifo0_sbe_in_sclk,
                                                                       ixo_icc_fifo1_sbe_in_sclk,
                                                                       ixo_ics_mem_sbe_in_sclk. */
        uint64_t reserved_29_31        : 3;
        uint64_t ied0_dbe              : 29; /**< [ 60: 32](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[IED0_DBE].
                                                                 Internal:
                                                                 iob_mem_data_xmd_sbe_sclk,
                                                                      gmr_ixofifo_dbe_sclk,
                                                                      icc0_xmc_fif_dbe,
                                                                      icc1_xmc_fif_dbe,
                                                                      icc_xmc_fifo_ecc_dbe,
                                                                      sli_preq_0_dbe_sclk,
                                                                      sli_req_0_dbe_sclk,
                                                                      sli_preq_1_dbe_sclk,
                                                                      sli_req_1_dbe_sclk,
                                                                      sli_preq_2_dbe_sclk,
                                                                      sli_req_2_dbe_sclk,
                                                                      ixo_smmu_mem0_dbe_sclk,
                                                                      iop_breq_fifo0_dbe,
                                                                      iop_breq_fifo1_dbe ,
                                                                      iop_breq_fifo2_dbe,
                                                                      iop_breq_fifo3_dbe ,
                                                                      iop_ffifo_dbe_sclk,
                                                                      rsd_mem0_dbe,
                                                                      rsd_mem1_dbe,
                                                                      ics_cmd_fifo_dbe_sclk,
                                                                      ixo_xmd_mem1_dbe_sclk,
                                                                      ixo_xmd_mem0_dbe_sclk,
                                                                      iobn_iorn_ffifo0__dbe_sclk,
                                                                      iobn_iorn_ffifo1__dbe_sclk,
                                                                      irp1_flid_mem_dbe,
                                                                      irp0_flid_mem_dbe,
                                                                      ixo_icc_fifo0_dbe_in_sclk,
                                                                      ixo_icc_fifo1_dbe_in_sclk,
                                                                      ixo_ics_mem_dbe_in_sclk. */
        uint64_t reserved_61_62        : 2;
        uint64_t pem_sie               : 1;  /**< [ 63: 63](R/W1S/H) Reads or sets IOBN(0..1)_INT_SUM[PEM_SIE]. */
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_iobnx_int_sum_w1s bdk_iobnx_int_sum_w1s_t;

static inline uint64_t BDK_IOBNX_INT_SUM_W1S(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_INT_SUM_W1S(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f0007000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f0007000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f0007000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f0007000ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_INT_SUM_W1S", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_INT_SUM_W1S(a) bdk_iobnx_int_sum_w1s_t
#define bustype_BDK_IOBNX_INT_SUM_W1S(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_INT_SUM_W1S(a) "IOBNX_INT_SUM_W1S"
#define device_bar_BDK_IOBNX_INT_SUM_W1S(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_INT_SUM_W1S(a) (a)
#define arguments_BDK_IOBNX_INT_SUM_W1S(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_mctlr_reg
 *
 * IOBN Memory SControl Register
 * Contains the sclk memory control for memories.
 */
union bdk_iobnx_mctlr_reg
{
    uint64_t u;
    struct bdk_iobnx_mctlr_reg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t dis                   : 17; /**< [ 48: 32](R/W) Memory ECC disable.
                                                                 Internal:
                                                                 \<47:32\>  = iobn_gmr_ixofifo_csr_cor_dis,
                                                                              sli_req_2_cor_dis and sli_preq_2_cor_dis,
                                                                              sli_req_1_cor_dis and sli_preq_1_cor_dis,
                                                                              sli_req_0_cor_dis and sli_preq_0_cor_dis,
                                                                              iob__iob_xmd_csr_cor_dis_rclk,
                                                                              ixo_smmu_mem0_csr_cor_dis,
                                                                              ixo_smmu_mem1_csr_cor_dis,
                                                                              ixo_ics_mem_csr_cor_dis,
                                                                              ixo_icc_fifo0_csr_cor_dis,
                                                                              ixo_icc_fifo1_csr_cor_dis,
                                                                              ixo_xmd_mem0_csr_cor_dis,
                                                                              ixo_xmd_mem1_csr_cor_dis,
                                                                              iobn_iorn_ffifo0__csr_cor_dis,
                                                                              iobn_iorn_ffifo1__csr_cor_dis,
                                                                              iop_ffifo_csr_cor_dis,
                                                                              ics_cmd_fifo_csr_cor_dis */
        uint64_t reserved_30_31        : 2;
        uint64_t flip                  : 30; /**< [ 29:  0](R/W) Memory ECC flip.
                                                                 Internal:
                                                                 \<27:0\>  = iobn_gmr_ixofifo_csr_flip_synd,
                                                                             sli_req_2_flip_synd and sli_preq_2_flip_synd,
                                                                             sli_req_1_flip_synd and sli_preq_1_flip_synd,
                                                                             sli_req_0_flip_synd and sli_preq_0_flip_synd,
                                                                             iobn_rsd_mem0_csr_flip_synd_rclk,
                                                                             iobn_rsd_mem1_csr_flip_synd_rclk,
                                                                             ixo_smmu_mem0_csr_flip_synd,
                                                                             ixo_smmu_mem1_csr_flip_synd,
                                                                             ixo_ics_mem_csr_flip_synd,
                                                                             iop_ffifo_csr_flip_synd,
                                                                             iop_breq_fifo0_csr_flip_synd,
                                                                             iop_breq_fifo1_csr_flip_synd,
                                                                             iop_breq_fifo2_csr_flip_synd,
                                                                             iop_breq_fifo3_csr_flip_synd */
#else /* Word 0 - Little Endian */
        uint64_t flip                  : 30; /**< [ 29:  0](R/W) Memory ECC flip.
                                                                 Internal:
                                                                 \<27:0\>  = iobn_gmr_ixofifo_csr_flip_synd,
                                                                             sli_req_2_flip_synd and sli_preq_2_flip_synd,
                                                                             sli_req_1_flip_synd and sli_preq_1_flip_synd,
                                                                             sli_req_0_flip_synd and sli_preq_0_flip_synd,
                                                                             iobn_rsd_mem0_csr_flip_synd_rclk,
                                                                             iobn_rsd_mem1_csr_flip_synd_rclk,
                                                                             ixo_smmu_mem0_csr_flip_synd,
                                                                             ixo_smmu_mem1_csr_flip_synd,
                                                                             ixo_ics_mem_csr_flip_synd,
                                                                             iop_ffifo_csr_flip_synd,
                                                                             iop_breq_fifo0_csr_flip_synd,
                                                                             iop_breq_fifo1_csr_flip_synd,
                                                                             iop_breq_fifo2_csr_flip_synd,
                                                                             iop_breq_fifo3_csr_flip_synd */
        uint64_t reserved_30_31        : 2;
        uint64_t dis                   : 17; /**< [ 48: 32](R/W) Memory ECC disable.
                                                                 Internal:
                                                                 \<47:32\>  = iobn_gmr_ixofifo_csr_cor_dis,
                                                                              sli_req_2_cor_dis and sli_preq_2_cor_dis,
                                                                              sli_req_1_cor_dis and sli_preq_1_cor_dis,
                                                                              sli_req_0_cor_dis and sli_preq_0_cor_dis,
                                                                              iob__iob_xmd_csr_cor_dis_rclk,
                                                                              ixo_smmu_mem0_csr_cor_dis,
                                                                              ixo_smmu_mem1_csr_cor_dis,
                                                                              ixo_ics_mem_csr_cor_dis,
                                                                              ixo_icc_fifo0_csr_cor_dis,
                                                                              ixo_icc_fifo1_csr_cor_dis,
                                                                              ixo_xmd_mem0_csr_cor_dis,
                                                                              ixo_xmd_mem1_csr_cor_dis,
                                                                              iobn_iorn_ffifo0__csr_cor_dis,
                                                                              iobn_iorn_ffifo1__csr_cor_dis,
                                                                              iop_ffifo_csr_cor_dis,
                                                                              ics_cmd_fifo_csr_cor_dis */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } s;
    struct bdk_iobnx_mctlr_reg_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_48_63        : 16;
        uint64_t dis                   : 16; /**< [ 47: 32](R/W) Memory ECC disable.
                                                                 Internal:
                                                                 \<47:32\>  = iobn_gmr_ixofifo_csr_cor_dis,
                                                                              sli_req_2_cor_dis and sli_preq_2_cor_dis,
                                                                              sli_req_1_cor_dis and sli_preq_1_cor_dis,
                                                                              sli_req_0_cor_dis and sli_preq_0_cor_dis,
                                                                              iob__iob_xmd_csr_cor_dis_rclk,
                                                                              ixo_smmu_mem0_csr_cor_dis,
                                                                              ixo_smmu_mem1_csr_cor_dis,
                                                                              ixo_ics_mem_csr_cor_dis,
                                                                              ixo_icc_fifo0_csr_cor_dis,
                                                                              ixo_icc_fifo1_csr_cor_dis,
                                                                              ixo_xmd_mem0_csr_cor_dis,
                                                                              ixo_xmd_mem1_csr_cor_dis,
                                                                              iobn_iorn_ffifo0__csr_cor_dis,
                                                                              iobn_iorn_ffifo1__csr_cor_dis,
                                                                              iop_ffifo_csr_cor_dis,
                                                                              ics_cmd_fifo_csr_cor_dis */
        uint64_t reserved_28_31        : 4;
        uint64_t flip                  : 28; /**< [ 27:  0](R/W) Memory ECC flip.
                                                                 Internal:
                                                                 \<27:0\>  = iobn_gmr_ixofifo_csr_flip_synd,
                                                                             sli_req_2_flip_synd and sli_preq_2_flip_synd,
                                                                             sli_req_1_flip_synd and sli_preq_1_flip_synd,
                                                                             sli_req_0_flip_synd and sli_preq_0_flip_synd,
                                                                             iobn_rsd_mem0_csr_flip_synd_rclk,
                                                                             iobn_rsd_mem1_csr_flip_synd_rclk,
                                                                             ixo_smmu_mem0_csr_flip_synd,
                                                                             ixo_smmu_mem1_csr_flip_synd,
                                                                             ixo_ics_mem_csr_flip_synd,
                                                                             iop_ffifo_csr_flip_synd,
                                                                             iop_breq_fifo0_csr_flip_synd,
                                                                             iop_breq_fifo1_csr_flip_synd,
                                                                             iop_breq_fifo2_csr_flip_synd,
                                                                             iop_breq_fifo3_csr_flip_synd */
#else /* Word 0 - Little Endian */
        uint64_t flip                  : 28; /**< [ 27:  0](R/W) Memory ECC flip.
                                                                 Internal:
                                                                 \<27:0\>  = iobn_gmr_ixofifo_csr_flip_synd,
                                                                             sli_req_2_flip_synd and sli_preq_2_flip_synd,
                                                                             sli_req_1_flip_synd and sli_preq_1_flip_synd,
                                                                             sli_req_0_flip_synd and sli_preq_0_flip_synd,
                                                                             iobn_rsd_mem0_csr_flip_synd_rclk,
                                                                             iobn_rsd_mem1_csr_flip_synd_rclk,
                                                                             ixo_smmu_mem0_csr_flip_synd,
                                                                             ixo_smmu_mem1_csr_flip_synd,
                                                                             ixo_ics_mem_csr_flip_synd,
                                                                             iop_ffifo_csr_flip_synd,
                                                                             iop_breq_fifo0_csr_flip_synd,
                                                                             iop_breq_fifo1_csr_flip_synd,
                                                                             iop_breq_fifo2_csr_flip_synd,
                                                                             iop_breq_fifo3_csr_flip_synd */
        uint64_t reserved_28_31        : 4;
        uint64_t dis                   : 16; /**< [ 47: 32](R/W) Memory ECC disable.
                                                                 Internal:
                                                                 \<47:32\>  = iobn_gmr_ixofifo_csr_cor_dis,
                                                                              sli_req_2_cor_dis and sli_preq_2_cor_dis,
                                                                              sli_req_1_cor_dis and sli_preq_1_cor_dis,
                                                                              sli_req_0_cor_dis and sli_preq_0_cor_dis,
                                                                              iob__iob_xmd_csr_cor_dis_rclk,
                                                                              ixo_smmu_mem0_csr_cor_dis,
                                                                              ixo_smmu_mem1_csr_cor_dis,
                                                                              ixo_ics_mem_csr_cor_dis,
                                                                              ixo_icc_fifo0_csr_cor_dis,
                                                                              ixo_icc_fifo1_csr_cor_dis,
                                                                              ixo_xmd_mem0_csr_cor_dis,
                                                                              ixo_xmd_mem1_csr_cor_dis,
                                                                              iobn_iorn_ffifo0__csr_cor_dis,
                                                                              iobn_iorn_ffifo1__csr_cor_dis,
                                                                              iop_ffifo_csr_cor_dis,
                                                                              ics_cmd_fifo_csr_cor_dis */
        uint64_t reserved_48_63        : 16;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_iobnx_mctlr_reg_cn81xx cn88xx; */
    struct bdk_iobnx_mctlr_reg_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t dis                   : 17; /**< [ 48: 32](R/W) Memory ECC disable.
                                                                 Internal:
                                                                 \<48:32\>  = iobn_gmr_ixofifo_csr_cor_dis,
                                                                              sli_req_3_cor_dis and sli_preq_3_cor_dis,
                                                                              sli_req_2_cor_dis and sli_preq_2_cor_dis,
                                                                              sli_req_1_cor_dis and sli_preq_1_cor_dis,
                                                                              sli_req_0_cor_dis and sli_preq_0_cor_dis,
                                                                              iob__iob_xmd_csr_cor_dis_rclk,
                                                                              ixo_smmu_mem0_csr_cor_dis,
                                                                              ixo_smmu_mem1_csr_cor_dis,
                                                                              ixo_ics_mem_csr_cor_dis,
                                                                              ixo_icc_fifo0_csr_cor_dis,
                                                                              ixo_icc_fifo1_csr_cor_dis,
                                                                              ixo_xmd_mem0_csr_cor_dis,
                                                                              ixo_xmd_mem1_csr_cor_dis,
                                                                              iobn_iorn_ffifo0__csr_cor_dis,
                                                                              iobn_iorn_ffifo1__csr_cor_dis,
                                                                              iop_ffifo_csr_cor_dis,
                                                                              ics_cmd_fifo_csr_cor_dis */
        uint64_t reserved_30_31        : 2;
        uint64_t flip                  : 30; /**< [ 29:  0](R/W) Memory ECC flip.
                                                                 Internal:
                                                                 \<29:0\>  = iobn_gmr_ixofifo_csr_flip_synd,
                                                                             sli_req_3_flip_synd and sli_preq_3_flip_synd,
                                                                             sli_req_2_flip_synd and sli_preq_2_flip_synd,
                                                                             sli_req_1_flip_synd and sli_preq_1_flip_synd,
                                                                             sli_req_0_flip_synd and sli_preq_0_flip_synd,
                                                                             iobn_rsd_mem0_csr_flip_synd_rclk,
                                                                             iobn_rsd_mem1_csr_flip_synd_rclk,
                                                                             ixo_smmu_mem0_csr_flip_synd,
                                                                             ixo_smmu_mem1_csr_flip_synd,
                                                                             ixo_ics_mem_csr_flip_synd,
                                                                             iop_ffifo_csr_flip_synd,
                                                                             iop_breq_fifo0_csr_flip_synd,
                                                                             iop_breq_fifo1_csr_flip_synd,
                                                                             iop_breq_fifo2_csr_flip_synd,
                                                                             iop_breq_fifo3_csr_flip_synd */
#else /* Word 0 - Little Endian */
        uint64_t flip                  : 30; /**< [ 29:  0](R/W) Memory ECC flip.
                                                                 Internal:
                                                                 \<29:0\>  = iobn_gmr_ixofifo_csr_flip_synd,
                                                                             sli_req_3_flip_synd and sli_preq_3_flip_synd,
                                                                             sli_req_2_flip_synd and sli_preq_2_flip_synd,
                                                                             sli_req_1_flip_synd and sli_preq_1_flip_synd,
                                                                             sli_req_0_flip_synd and sli_preq_0_flip_synd,
                                                                             iobn_rsd_mem0_csr_flip_synd_rclk,
                                                                             iobn_rsd_mem1_csr_flip_synd_rclk,
                                                                             ixo_smmu_mem0_csr_flip_synd,
                                                                             ixo_smmu_mem1_csr_flip_synd,
                                                                             ixo_ics_mem_csr_flip_synd,
                                                                             iop_ffifo_csr_flip_synd,
                                                                             iop_breq_fifo0_csr_flip_synd,
                                                                             iop_breq_fifo1_csr_flip_synd,
                                                                             iop_breq_fifo2_csr_flip_synd,
                                                                             iop_breq_fifo3_csr_flip_synd */
        uint64_t reserved_30_31        : 2;
        uint64_t dis                   : 17; /**< [ 48: 32](R/W) Memory ECC disable.
                                                                 Internal:
                                                                 \<48:32\>  = iobn_gmr_ixofifo_csr_cor_dis,
                                                                              sli_req_3_cor_dis and sli_preq_3_cor_dis,
                                                                              sli_req_2_cor_dis and sli_preq_2_cor_dis,
                                                                              sli_req_1_cor_dis and sli_preq_1_cor_dis,
                                                                              sli_req_0_cor_dis and sli_preq_0_cor_dis,
                                                                              iob__iob_xmd_csr_cor_dis_rclk,
                                                                              ixo_smmu_mem0_csr_cor_dis,
                                                                              ixo_smmu_mem1_csr_cor_dis,
                                                                              ixo_ics_mem_csr_cor_dis,
                                                                              ixo_icc_fifo0_csr_cor_dis,
                                                                              ixo_icc_fifo1_csr_cor_dis,
                                                                              ixo_xmd_mem0_csr_cor_dis,
                                                                              ixo_xmd_mem1_csr_cor_dis,
                                                                              iobn_iorn_ffifo0__csr_cor_dis,
                                                                              iobn_iorn_ffifo1__csr_cor_dis,
                                                                              iop_ffifo_csr_cor_dis,
                                                                              ics_cmd_fifo_csr_cor_dis */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_iobnx_mctlr_reg bdk_iobnx_mctlr_reg_t;

static inline uint64_t BDK_IOBNX_MCTLR_REG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_MCTLR_REG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f0005108ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f0005108ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f0005108ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_MCTLR_REG", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_MCTLR_REG(a) bdk_iobnx_mctlr_reg_t
#define bustype_BDK_IOBNX_MCTLR_REG(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_MCTLR_REG(a) "IOBNX_MCTLR_REG"
#define device_bar_BDK_IOBNX_MCTLR_REG(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_MCTLR_REG(a) (a)
#define arguments_BDK_IOBNX_MCTLR_REG(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_mctls_reg
 *
 * IOBN Memory SControl Register
 * Contains the sclk memory control for memories.
 */
union bdk_iobnx_mctls_reg
{
    uint64_t u;
    struct bdk_iobnx_mctls_reg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_43_63        : 21;
        uint64_t dis                   : 11; /**< [ 42: 32](R/W) Memory ECC disable.
                                                                 Internal:
                                                                 \<42:32\>  = iobn_rsd_mem0_csr_cor_dis,
                                                                              iobn_rsd_mem1_csr_cor_dis,
                                                                              irp0_flid_mem_csr_cor_dis,
                                                                              irp1_flid_mem_csr_cor_dis,
                                                                              iop_breq_fifo0_csr_cor_dis,
                                                                              iop_breq_fifo1_csr_cor_dis,
                                                                              iop_breq_fifo2_csr_cor_dis,
                                                                              iop_breq_fifo3_csr_cor_dis,
                                                                              icc_xmc_fifo_ecc_csr_cor_dis,
                                                                              icc0_xmc_fifo_csr_cor_dis,
                                                                              icc1_xmc_fifo_csr_cor_dis */
        uint64_t reserved_26_31        : 6;
        uint64_t flip                  : 26; /**< [ 25:  0](R/W) Memory ECC flip.
                                                                 Internal:
                                                                 \<25:0\>  = iob__iob_xmd_csr_flip_synd_sclk,
                                                                             ixo_icc_fifo0_csr_flip_synd,
                                                                             ixo_icc_fifo1_csr_flip_synd,
                                                                             ixo_xmd_mem0_csr_flip_synd,
                                                                             ixo_xmd_mem1_csr_flip_synd,
                                                                             irp0_flid_mem_csr_flip_synd,
                                                                             irp1_flid_mem_csr_flip_synd,
                                                                             iobn_iorn_ffifo0__csr_flip_synd,
                                                                             iobn_iorn_ffifo1__csr_flip_synd,
                                                                             icc_xmc_fifo_ecc_csr_flip_synd,
                                                                             ics_cmd_fifo_csr_flip_synd,
                                                                             icc0_xmc_fifo_csr_flip_synd,
                                                                             icc1_xmc_fifo_csr_flip_synd */
#else /* Word 0 - Little Endian */
        uint64_t flip                  : 26; /**< [ 25:  0](R/W) Memory ECC flip.
                                                                 Internal:
                                                                 \<25:0\>  = iob__iob_xmd_csr_flip_synd_sclk,
                                                                             ixo_icc_fifo0_csr_flip_synd,
                                                                             ixo_icc_fifo1_csr_flip_synd,
                                                                             ixo_xmd_mem0_csr_flip_synd,
                                                                             ixo_xmd_mem1_csr_flip_synd,
                                                                             irp0_flid_mem_csr_flip_synd,
                                                                             irp1_flid_mem_csr_flip_synd,
                                                                             iobn_iorn_ffifo0__csr_flip_synd,
                                                                             iobn_iorn_ffifo1__csr_flip_synd,
                                                                             icc_xmc_fifo_ecc_csr_flip_synd,
                                                                             ics_cmd_fifo_csr_flip_synd,
                                                                             icc0_xmc_fifo_csr_flip_synd,
                                                                             icc1_xmc_fifo_csr_flip_synd */
        uint64_t reserved_26_31        : 6;
        uint64_t dis                   : 11; /**< [ 42: 32](R/W) Memory ECC disable.
                                                                 Internal:
                                                                 \<42:32\>  = iobn_rsd_mem0_csr_cor_dis,
                                                                              iobn_rsd_mem1_csr_cor_dis,
                                                                              irp0_flid_mem_csr_cor_dis,
                                                                              irp1_flid_mem_csr_cor_dis,
                                                                              iop_breq_fifo0_csr_cor_dis,
                                                                              iop_breq_fifo1_csr_cor_dis,
                                                                              iop_breq_fifo2_csr_cor_dis,
                                                                              iop_breq_fifo3_csr_cor_dis,
                                                                              icc_xmc_fifo_ecc_csr_cor_dis,
                                                                              icc0_xmc_fifo_csr_cor_dis,
                                                                              icc1_xmc_fifo_csr_cor_dis */
        uint64_t reserved_43_63        : 21;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_mctls_reg_s cn; */
};
typedef union bdk_iobnx_mctls_reg bdk_iobnx_mctls_reg_t;

static inline uint64_t BDK_IOBNX_MCTLS_REG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_MCTLS_REG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f0005100ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f0005100ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f0005100ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_MCTLS_REG", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_MCTLS_REG(a) bdk_iobnx_mctls_reg_t
#define bustype_BDK_IOBNX_MCTLS_REG(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_MCTLS_REG(a) "IOBNX_MCTLS_REG"
#define device_bar_BDK_IOBNX_MCTLS_REG(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_MCTLS_REG(a) (a)
#define arguments_BDK_IOBNX_MCTLS_REG(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_msix_pba#
 *
 * IOBN MSI-X Pending Bit Array Registers
 * This register is the MSI-X PBA table; the bit number is indexed by the IOBN_INT_VEC_E enumeration.
 */
union bdk_iobnx_msix_pbax
{
    uint64_t u;
    struct bdk_iobnx_msix_pbax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO) Pending message for the associated IOBN()_MSIX_VEC()_CTL, enumerated by IOBN_INT_VEC_E.
                                                                 Bits that have no associated IOBN_INT_VEC_E are zero. */
#else /* Word 0 - Little Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO) Pending message for the associated IOBN()_MSIX_VEC()_CTL, enumerated by IOBN_INT_VEC_E.
                                                                 Bits that have no associated IOBN_INT_VEC_E are zero. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_msix_pbax_s cn; */
};
typedef union bdk_iobnx_msix_pbax bdk_iobnx_msix_pbax_t;

static inline uint64_t BDK_IOBNX_MSIX_PBAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_MSIX_PBAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b==0)))
        return 0x87e0f0ff0000ll + 0x1000000ll * ((a) & 0x0) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b==0)))
        return 0x87e0f0ff0000ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b==0)))
        return 0x87e0f0ff0000ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b==0)))
        return 0x87e0f0ff0000ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x0);
    __bdk_csr_fatal("IOBNX_MSIX_PBAX", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_MSIX_PBAX(a,b) bdk_iobnx_msix_pbax_t
#define bustype_BDK_IOBNX_MSIX_PBAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_MSIX_PBAX(a,b) "IOBNX_MSIX_PBAX"
#define device_bar_BDK_IOBNX_MSIX_PBAX(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_IOBNX_MSIX_PBAX(a,b) (a)
#define arguments_BDK_IOBNX_MSIX_PBAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_msix_vec#_addr
 *
 * IOBN MSI-X Vector-Table Address Register
 * This register is the MSI-X vector table, indexed by the IOBN_INT_VEC_E enumeration.
 */
union bdk_iobnx_msix_vecx_addr
{
    uint64_t u;
    struct bdk_iobnx_msix_vecx_addr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_53_63        : 11;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's IOBN_MSIX_VEC()_ADDR, IOBN_MSIX_VEC()_CTL, and corresponding
                                                                 bit of IOBN_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_IOBN_VSEC_SCTL[MSIX_SEC] (for documentation, see PCCPF_XXX_VSEC_SCTL[MSIX_SEC])
                                                                 is set, all vectors are secure and function as if [SECVEC] was set. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's IOBN_MSIX_VEC()_ADDR, IOBN_MSIX_VEC()_CTL, and corresponding
                                                                 bit of IOBN_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_IOBN_VSEC_SCTL[MSIX_SEC] (for documentation, see PCCPF_XXX_VSEC_SCTL[MSIX_SEC])
                                                                 is set, all vectors are secure and function as if [SECVEC] was set. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_53_63        : 11;
#endif /* Word 0 - End */
    } s;
    struct bdk_iobnx_msix_vecx_addr_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's IOBN_MSIX_VEC()_ADDR, IOBN_MSIX_VEC()_CTL, and corresponding
                                                                 bit of IOBN_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_IOBN_VSEC_SCTL[MSIX_SEC] (for documentation, see PCCPF_XXX_VSEC_SCTL[MSIX_SEC])
                                                                 is set, all vectors are secure and function as if [SECVEC] was set. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's IOBN_MSIX_VEC()_ADDR, IOBN_MSIX_VEC()_CTL, and corresponding
                                                                 bit of IOBN_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_IOBN_VSEC_SCTL[MSIX_SEC] (for documentation, see PCCPF_XXX_VSEC_SCTL[MSIX_SEC])
                                                                 is set, all vectors are secure and function as if [SECVEC] was set. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } cn8;
    struct bdk_iobnx_msix_vecx_addr_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_53_63        : 11;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's IOBN()_MSIX_VEC()_ADDR, IOBN()_MSIX_VEC()_CTL, and
                                                                 corresponding bit of IOBN()_MSIX_PBA() are RAZ/WI and does not cause a fault
                                                                 when accessed by the nonsecure world.

                                                                 If PCCPF_IOBN_VSEC_SCTL[MSIX_SEC] (for documentation, see PCCPF_XXX_VSEC_SCTL[MSIX_SEC])
                                                                 is set, all vectors are secure and function as if [SECVEC] was set. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's IOBN()_MSIX_VEC()_ADDR, IOBN()_MSIX_VEC()_CTL, and
                                                                 corresponding bit of IOBN()_MSIX_PBA() are RAZ/WI and does not cause a fault
                                                                 when accessed by the nonsecure world.

                                                                 If PCCPF_IOBN_VSEC_SCTL[MSIX_SEC] (for documentation, see PCCPF_XXX_VSEC_SCTL[MSIX_SEC])
                                                                 is set, all vectors are secure and function as if [SECVEC] was set. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_53_63        : 11;
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_iobnx_msix_vecx_addr bdk_iobnx_msix_vecx_addr_t;

static inline uint64_t BDK_IOBNX_MSIX_VECX_ADDR(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_MSIX_VECX_ADDR(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b==0)))
        return 0x87e0f0f00000ll + 0x1000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b==0)))
        return 0x87e0f0f00000ll + 0x1000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b==0)))
        return 0x87e0f0f00000ll + 0x1000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0f0f00000ll + 0x1000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("IOBNX_MSIX_VECX_ADDR", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_MSIX_VECX_ADDR(a,b) bdk_iobnx_msix_vecx_addr_t
#define bustype_BDK_IOBNX_MSIX_VECX_ADDR(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_MSIX_VECX_ADDR(a,b) "IOBNX_MSIX_VECX_ADDR"
#define device_bar_BDK_IOBNX_MSIX_VECX_ADDR(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_IOBNX_MSIX_VECX_ADDR(a,b) (a)
#define arguments_BDK_IOBNX_MSIX_VECX_ADDR(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_msix_vec#_ctl
 *
 * IOBN MSI-X Vector-Table Control and Data Register
 * This register is the MSI-X vector table, indexed by the IOBN_INT_VEC_E enumeration.
 */
union bdk_iobnx_msix_vecx_ctl
{
    uint64_t u;
    struct bdk_iobnx_msix_vecx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_33_63        : 31;
        uint64_t mask                  : 1;  /**< [ 32: 32](R/W) When set, no MSI-X interrupts are sent to this vector. */
        uint64_t data                  : 32; /**< [ 31:  0](R/W) Data to use for MSI-X delivery of this vector. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 32; /**< [ 31:  0](R/W) Data to use for MSI-X delivery of this vector. */
        uint64_t mask                  : 1;  /**< [ 32: 32](R/W) When set, no MSI-X interrupts are sent to this vector. */
        uint64_t reserved_33_63        : 31;
#endif /* Word 0 - End */
    } s;
    struct bdk_iobnx_msix_vecx_ctl_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_33_63        : 31;
        uint64_t mask                  : 1;  /**< [ 32: 32](R/W) When set, no MSI-X interrupts are sent to this vector. */
        uint64_t reserved_20_31        : 12;
        uint64_t data                  : 20; /**< [ 19:  0](R/W) Data to use for MSI-X delivery of this vector. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 20; /**< [ 19:  0](R/W) Data to use for MSI-X delivery of this vector. */
        uint64_t reserved_20_31        : 12;
        uint64_t mask                  : 1;  /**< [ 32: 32](R/W) When set, no MSI-X interrupts are sent to this vector. */
        uint64_t reserved_33_63        : 31;
#endif /* Word 0 - End */
    } cn8;
    /* struct bdk_iobnx_msix_vecx_ctl_s cn9; */
};
typedef union bdk_iobnx_msix_vecx_ctl bdk_iobnx_msix_vecx_ctl_t;

static inline uint64_t BDK_IOBNX_MSIX_VECX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_MSIX_VECX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b==0)))
        return 0x87e0f0f00008ll + 0x1000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b==0)))
        return 0x87e0f0f00008ll + 0x1000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b==0)))
        return 0x87e0f0f00008ll + 0x1000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0f0f00008ll + 0x1000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("IOBNX_MSIX_VECX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_MSIX_VECX_CTL(a,b) bdk_iobnx_msix_vecx_ctl_t
#define bustype_BDK_IOBNX_MSIX_VECX_CTL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_MSIX_VECX_CTL(a,b) "IOBNX_MSIX_VECX_CTL"
#define device_bar_BDK_IOBNX_MSIX_VECX_CTL(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_IOBNX_MSIX_VECX_CTL(a,b) (a)
#define arguments_BDK_IOBNX_MSIX_VECX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_ncb#_acc
 *
 * IOBN NCB Access Registers
 * This register sets attributes of NCBDIDs address bits \<43:36\>.
 */
union bdk_iobnx_ncbx_acc
{
    uint64_t u;
    struct bdk_iobnx_ncbx_acc_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t all_cmds              : 1;  /**< [  0:  0](R/W) Device supports all commends.
                                                                 0 = Only naturally aligned loads and stores that are 64-bit or smaller are
                                                                 permitted to the NCB device. This setting is used for non-PEM devices.
                                                                 1 = Allow all size accesses, plus atomics and LMTSTs. This setting is used for
                                                                 PEM.

                                                                 Reset value of this field varies for different devices.
                                                                 Using non-reset values is for diagnostic use only.

                                                                 Internal:
                                                                 FIXME resets to be added. */
#else /* Word 0 - Little Endian */
        uint64_t all_cmds              : 1;  /**< [  0:  0](R/W) Device supports all commends.
                                                                 0 = Only naturally aligned loads and stores that are 64-bit or smaller are
                                                                 permitted to the NCB device. This setting is used for non-PEM devices.
                                                                 1 = Allow all size accesses, plus atomics and LMTSTs. This setting is used for
                                                                 PEM.

                                                                 Reset value of this field varies for different devices.
                                                                 Using non-reset values is for diagnostic use only.

                                                                 Internal:
                                                                 FIXME resets to be added. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncbx_acc_s cn; */
};
typedef union bdk_iobnx_ncbx_acc bdk_iobnx_ncbx_acc_t;

static inline uint64_t BDK_IOBNX_NCBX_ACC(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBX_ACC(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=255)))
        return 0x87e0f0080000ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0xff);
    __bdk_csr_fatal("IOBNX_NCBX_ACC", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_NCBX_ACC(a,b) bdk_iobnx_ncbx_acc_t
#define bustype_BDK_IOBNX_NCBX_ACC(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBX_ACC(a,b) "IOBNX_NCBX_ACC"
#define device_bar_BDK_IOBNX_NCBX_ACC(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBX_ACC(a,b) (a)
#define arguments_BDK_IOBNX_NCBX_ACC(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_ncb#_arb#_bp_test
 *
 * IOBN Back Pressure Register
 */
union bdk_iobnx_ncbx_arbx_bp_test
{
    uint64_t u;
    struct bdk_iobnx_ncbx_arbx_bp_test_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t bp_cfg                : 64; /**< [ 63:  0](R/W) Backpressure weight. For diagnostic use only.
                                                                 Internal:
                                                                 There are 2 backpressure configuration bits per enable, with the two bits
                                                                 defined as 0x0=100% of the time, 0x1=75% of the time, 0x2=50% of the time,
                                                                 0x3=25% of the time.
                                                                   \<etc\> up to max number of supported ports per arbiter
                                                                   \<7:6\>  = Config 3.
                                                                   \<25:4\> = Config 2.
                                                                   \<3:2\>  = Config 1.
                                                                   \<1:0\>  = Config 0. */
#else /* Word 0 - Little Endian */
        uint64_t bp_cfg                : 64; /**< [ 63:  0](R/W) Backpressure weight. For diagnostic use only.
                                                                 Internal:
                                                                 There are 2 backpressure configuration bits per enable, with the two bits
                                                                 defined as 0x0=100% of the time, 0x1=75% of the time, 0x2=50% of the time,
                                                                 0x3=25% of the time.
                                                                   \<etc\> up to max number of supported ports per arbiter
                                                                   \<7:6\>  = Config 3.
                                                                   \<25:4\> = Config 2.
                                                                   \<3:2\>  = Config 1.
                                                                   \<1:0\>  = Config 0. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncbx_arbx_bp_test_s cn; */
};
typedef union bdk_iobnx_ncbx_arbx_bp_test bdk_iobnx_ncbx_arbx_bp_test_t;

static inline uint64_t BDK_IOBNX_NCBX_ARBX_BP_TEST(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBX_ARBX_BP_TEST(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=2) && (c<=31)))
        return 0x87e0f00f8000ll + 0x1000000ll * ((a) & 0x1) + 0x400ll * ((b) & 0x3) + 8ll * ((c) & 0x1f);
    __bdk_csr_fatal("IOBNX_NCBX_ARBX_BP_TEST", 3, a, b, c, 0);
}

#define typedef_BDK_IOBNX_NCBX_ARBX_BP_TEST(a,b,c) bdk_iobnx_ncbx_arbx_bp_test_t
#define bustype_BDK_IOBNX_NCBX_ARBX_BP_TEST(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBX_ARBX_BP_TEST(a,b,c) "IOBNX_NCBX_ARBX_BP_TEST"
#define device_bar_BDK_IOBNX_NCBX_ARBX_BP_TEST(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBX_ARBX_BP_TEST(a,b,c) (a)
#define arguments_BDK_IOBNX_NCBX_ARBX_BP_TEST(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) iobn#_ncb#_arb#_crds
 *
 * IOBN NREQ Priority Register
 */
union bdk_iobnx_ncbx_arbx_crds
{
    uint64_t u;
    struct bdk_iobnx_ncbx_arbx_crds_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t max                   : 8;  /**< [ 15:  8](R/W) Maximum number of FLIDs available to the requestor.
                                                                 Decreasing this number will limit the maximum burst performance of this device. */
        uint64_t min                   : 8;  /**< [  7:  0](R/W) Minimum number of FLIDs available to the requestor. From the total available
                                                                 credits this many will be set aside for this NREQID to use.
                                                                 Increasing this number will insure this device has dedicated bandwidth over
                                                                 other devices. Must be 0x1 or larger for GIC. Recommend 0x1 or larger for
                                                                 all devices that are used. */
#else /* Word 0 - Little Endian */
        uint64_t min                   : 8;  /**< [  7:  0](R/W) Minimum number of FLIDs available to the requestor. From the total available
                                                                 credits this many will be set aside for this NREQID to use.
                                                                 Increasing this number will insure this device has dedicated bandwidth over
                                                                 other devices. Must be 0x1 or larger for GIC. Recommend 0x1 or larger for
                                                                 all devices that are used. */
        uint64_t max                   : 8;  /**< [ 15:  8](R/W) Maximum number of FLIDs available to the requestor.
                                                                 Decreasing this number will limit the maximum burst performance of this device. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncbx_arbx_crds_s cn; */
};
typedef union bdk_iobnx_ncbx_arbx_crds bdk_iobnx_ncbx_arbx_crds_t;

static inline uint64_t BDK_IOBNX_NCBX_ARBX_CRDS(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBX_ARBX_CRDS(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=2) && (c<=31)))
        return 0x87e0f00f0000ll + 0x1000000ll * ((a) & 0x1) + 0x400ll * ((b) & 0x3) + 8ll * ((c) & 0x1f);
    __bdk_csr_fatal("IOBNX_NCBX_ARBX_CRDS", 3, a, b, c, 0);
}

#define typedef_BDK_IOBNX_NCBX_ARBX_CRDS(a,b,c) bdk_iobnx_ncbx_arbx_crds_t
#define bustype_BDK_IOBNX_NCBX_ARBX_CRDS(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBX_ARBX_CRDS(a,b,c) "IOBNX_NCBX_ARBX_CRDS"
#define device_bar_BDK_IOBNX_NCBX_ARBX_CRDS(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBX_ARBX_CRDS(a,b,c) (a)
#define arguments_BDK_IOBNX_NCBX_ARBX_CRDS(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) iobn#_ncb#_arb#_rw#_lat_pc
 *
 * IOBN NCB Latency Performance Counter Registers
 */
union bdk_iobnx_ncbx_arbx_rwx_lat_pc
{
    uint64_t u;
    struct bdk_iobnx_ncbx_arbx_rwx_lat_pc_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Latency performance counter.

                                                                 _ RW(0) increments every cycle by the number of read transactions that have been
                                                                 granted from the given NCB, but have not had there credit returned to the NGNT.

                                                                 _ RW(0) increments every cycle by the number of write transactions that have been
                                                                 granted from the given NCB, but have not had there credit returned to the NGNT.

                                                                 This counter should be divided by IOBN()_NCB()_ARB()_RW()_REQ_PC to determine each NCB
                                                                 bus's average read and write latency. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Latency performance counter.

                                                                 _ RW(0) increments every cycle by the number of read transactions that have been
                                                                 granted from the given NCB, but have not had there credit returned to the NGNT.

                                                                 _ RW(0) increments every cycle by the number of write transactions that have been
                                                                 granted from the given NCB, but have not had there credit returned to the NGNT.

                                                                 This counter should be divided by IOBN()_NCB()_ARB()_RW()_REQ_PC to determine each NCB
                                                                 bus's average read and write latency. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncbx_arbx_rwx_lat_pc_s cn; */
};
typedef union bdk_iobnx_ncbx_arbx_rwx_lat_pc bdk_iobnx_ncbx_arbx_rwx_lat_pc_t;

static inline uint64_t BDK_IOBNX_NCBX_ARBX_RWX_LAT_PC(unsigned long a, unsigned long b, unsigned long c, unsigned long d) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBX_ARBX_RWX_LAT_PC(unsigned long a, unsigned long b, unsigned long c, unsigned long d)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=2) && (c<=31) && (d<=1)))
        return 0x87e0f00f4000ll + 0x1000000ll * ((a) & 0x1) + 0x400ll * ((b) & 0x3) + 0x20ll * ((c) & 0x1f) + 8ll * ((d) & 0x1);
    __bdk_csr_fatal("IOBNX_NCBX_ARBX_RWX_LAT_PC", 4, a, b, c, d);
}

#define typedef_BDK_IOBNX_NCBX_ARBX_RWX_LAT_PC(a,b,c,d) bdk_iobnx_ncbx_arbx_rwx_lat_pc_t
#define bustype_BDK_IOBNX_NCBX_ARBX_RWX_LAT_PC(a,b,c,d) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBX_ARBX_RWX_LAT_PC(a,b,c,d) "IOBNX_NCBX_ARBX_RWX_LAT_PC"
#define device_bar_BDK_IOBNX_NCBX_ARBX_RWX_LAT_PC(a,b,c,d) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBX_ARBX_RWX_LAT_PC(a,b,c,d) (a)
#define arguments_BDK_IOBNX_NCBX_ARBX_RWX_LAT_PC(a,b,c,d) (a),(b),(c),(d)

/**
 * Register (RSL) iobn#_ncb#_arb#_rw#_req_pc
 *
 * IOBN NCB Request Performance Counter Registers
 */
union bdk_iobnx_ncbx_arbx_rwx_req_pc
{
    uint64_t u;
    struct bdk_iobnx_ncbx_arbx_rwx_req_pc_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Request performance counter.

                                                                 _ RW(0) increments on read  transaction being granted by NGNT.

                                                                 _ RW(1) increments on write transaction being granted by NGNT. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Request performance counter.

                                                                 _ RW(0) increments on read  transaction being granted by NGNT.

                                                                 _ RW(1) increments on write transaction being granted by NGNT. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncbx_arbx_rwx_req_pc_s cn; */
};
typedef union bdk_iobnx_ncbx_arbx_rwx_req_pc bdk_iobnx_ncbx_arbx_rwx_req_pc_t;

static inline uint64_t BDK_IOBNX_NCBX_ARBX_RWX_REQ_PC(unsigned long a, unsigned long b, unsigned long c, unsigned long d) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBX_ARBX_RWX_REQ_PC(unsigned long a, unsigned long b, unsigned long c, unsigned long d)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=2) && (c<=31) && (d<=1)))
        return 0x87e0f00f2000ll + 0x1000000ll * ((a) & 0x1) + 0x400ll * ((b) & 0x3) + 0x20ll * ((c) & 0x1f) + 8ll * ((d) & 0x1);
    __bdk_csr_fatal("IOBNX_NCBX_ARBX_RWX_REQ_PC", 4, a, b, c, d);
}

#define typedef_BDK_IOBNX_NCBX_ARBX_RWX_REQ_PC(a,b,c,d) bdk_iobnx_ncbx_arbx_rwx_req_pc_t
#define bustype_BDK_IOBNX_NCBX_ARBX_RWX_REQ_PC(a,b,c,d) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBX_ARBX_RWX_REQ_PC(a,b,c,d) "IOBNX_NCBX_ARBX_RWX_REQ_PC"
#define device_bar_BDK_IOBNX_NCBX_ARBX_RWX_REQ_PC(a,b,c,d) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBX_ARBX_RWX_REQ_PC(a,b,c,d) (a)
#define arguments_BDK_IOBNX_NCBX_ARBX_RWX_REQ_PC(a,b,c,d) (a),(b),(c),(d)

/**
 * Register (RSL) iobn#_ncb#_const
 *
 * IOBN NCB Constant Registers
 * This register returns discovery information indexed by each NCB ID (physical address
 * bits \<43:36\>). Each index {a} (IOB) returns identical information for a given index {b}.
 */
union bdk_iobnx_ncbx_const
{
    uint64_t u;
    struct bdk_iobnx_ncbx_const_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t farbid                : 8;  /**< [ 23: 16](RO) Flat ARBID for the given NCB ID. */
        uint64_t arbid                 : 8;  /**< [ 15:  8](RO) NCB bus ARBID for the given NCB ID. */
        uint64_t ncb                   : 4;  /**< [  7:  4](RO) Physical bus number for the given NCB ID. */
        uint64_t iob                   : 3;  /**< [  3:  1](RO) IOB number for the given NCB ID. */
        uint64_t valid                 : 1;  /**< [  0:  0](RO) Set if this NCB ID is a valid ID. */
#else /* Word 0 - Little Endian */
        uint64_t valid                 : 1;  /**< [  0:  0](RO) Set if this NCB ID is a valid ID. */
        uint64_t iob                   : 3;  /**< [  3:  1](RO) IOB number for the given NCB ID. */
        uint64_t ncb                   : 4;  /**< [  7:  4](RO) Physical bus number for the given NCB ID. */
        uint64_t arbid                 : 8;  /**< [ 15:  8](RO) NCB bus ARBID for the given NCB ID. */
        uint64_t farbid                : 8;  /**< [ 23: 16](RO) Flat ARBID for the given NCB ID. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncbx_const_s cn; */
};
typedef union bdk_iobnx_ncbx_const bdk_iobnx_ncbx_const_t;

static inline uint64_t BDK_IOBNX_NCBX_CONST(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBX_CONST(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=255)))
        return 0x87e0f0001000ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0xff);
    __bdk_csr_fatal("IOBNX_NCBX_CONST", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_NCBX_CONST(a,b) bdk_iobnx_ncbx_const_t
#define bustype_BDK_IOBNX_NCBX_CONST(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBX_CONST(a,b) "IOBNX_NCBX_CONST"
#define device_bar_BDK_IOBNX_NCBX_CONST(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBX_CONST(a,b) (a)
#define arguments_BDK_IOBNX_NCBX_CONST(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_ncb#_credits
 *
 * IOBN NCB Credits Register
 * This register controls the number of loads and stores each NCB can have to the L2.
 */
union bdk_iobnx_ncbx_credits
{
    uint64_t u;
    struct bdk_iobnx_ncbx_credits_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_23_63        : 41;
        uint64_t ncb_wr_buf_crd        : 7;  /**< [ 22: 16](R/W) NCB write buffer credit. Each NCB can have 64 write buffers in flight to the L2; this is
                                                                 the number by which to decrease the 64. */
        uint64_t reserved_15           : 1;
        uint64_t ncb0_wr_crd           : 7;  /**< [ 14:  8](R/W) NCB write credit. Each NCB can have 64 writes in flight to the L2; this is the number by
                                                                 which to
                                                                 decrease the 64. */
        uint64_t reserved_7            : 1;
        uint64_t ncb0_rd_crd           : 7;  /**< [  6:  0](R/W) NCB read credit. Each NCB can have 64 reads in flight to the L2; this is the number to
                                                                 decrease the 64 by. */
#else /* Word 0 - Little Endian */
        uint64_t ncb0_rd_crd           : 7;  /**< [  6:  0](R/W) NCB read credit. Each NCB can have 64 reads in flight to the L2; this is the number to
                                                                 decrease the 64 by. */
        uint64_t reserved_7            : 1;
        uint64_t ncb0_wr_crd           : 7;  /**< [ 14:  8](R/W) NCB write credit. Each NCB can have 64 writes in flight to the L2; this is the number by
                                                                 which to
                                                                 decrease the 64. */
        uint64_t reserved_15           : 1;
        uint64_t ncb_wr_buf_crd        : 7;  /**< [ 22: 16](R/W) NCB write buffer credit. Each NCB can have 64 write buffers in flight to the L2; this is
                                                                 the number by which to decrease the 64. */
        uint64_t reserved_23_63        : 41;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncbx_credits_s cn; */
};
typedef union bdk_iobnx_ncbx_credits bdk_iobnx_ncbx_credits_t;

static inline uint64_t BDK_IOBNX_NCBX_CREDITS(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBX_CREDITS(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b==0)))
        return 0x87e0f0001000ll + 0x1000000ll * ((a) & 0x0) + 0x100ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0f0001000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0f0001000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x1);
    __bdk_csr_fatal("IOBNX_NCBX_CREDITS", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_NCBX_CREDITS(a,b) bdk_iobnx_ncbx_credits_t
#define bustype_BDK_IOBNX_NCBX_CREDITS(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBX_CREDITS(a,b) "IOBNX_NCBX_CREDITS"
#define device_bar_BDK_IOBNX_NCBX_CREDITS(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBX_CREDITS(a,b) (a)
#define arguments_BDK_IOBNX_NCBX_CREDITS(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_ncb#_ctl
 *
 * IOBN NCB Control Registers
 */
union bdk_iobnx_ncbx_ctl
{
    uint64_t u;
    struct bdk_iobnx_ncbx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t stp                   : 2;  /**< [  9:  8](R/W) When a complete cache block is written a STP will be converted to:
                                                                    0 = STF.
                                                                    1 = STY.
                                                                    2 = STT.
                                                                    3 = Reserved. */
        uint64_t reserved_0_7          : 8;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_7          : 8;
        uint64_t stp                   : 2;  /**< [  9:  8](R/W) When a complete cache block is written a STP will be converted to:
                                                                    0 = STF.
                                                                    1 = STY.
                                                                    2 = STT.
                                                                    3 = Reserved. */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } s;
    struct bdk_iobnx_ncbx_ctl_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t stp                   : 2;  /**< [  9:  8](R/W) When a complete cache block is written a STP will be converted to:
                                                                    0 = STF.
                                                                    1 = STY.
                                                                    2 = STT.
                                                                    3 = Reserved. */
        uint64_t reserved_2_7          : 6;
        uint64_t rstp                  : 2;  /**< [  1:  0](R/W) When a complete cache block is written a RSTP will be converted to:
                                                                    0 = STY.
                                                                    1 = STT.
                                                                    2 = STF.
                                                                    3 = Reserved. */
#else /* Word 0 - Little Endian */
        uint64_t rstp                  : 2;  /**< [  1:  0](R/W) When a complete cache block is written a RSTP will be converted to:
                                                                    0 = STY.
                                                                    1 = STT.
                                                                    2 = STF.
                                                                    3 = Reserved. */
        uint64_t reserved_2_7          : 6;
        uint64_t stp                   : 2;  /**< [  9:  8](R/W) When a complete cache block is written a STP will be converted to:
                                                                    0 = STF.
                                                                    1 = STY.
                                                                    2 = STT.
                                                                    3 = Reserved. */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } cn8;
    struct bdk_iobnx_ncbx_ctl_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t dis                   : 1;  /**< [  1:  1](R/W/H) Disable the opportunistic low latency mode for all ports. For diagnostic use only. */
        uint64_t cal                   : 1;  /**< [  0:  0](R/W/H) Calibration active. Write one to field to start calibration. Cleared when
                                                                 calibration is complete. For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t cal                   : 1;  /**< [  0:  0](R/W/H) Calibration active. Write one to field to start calibration. Cleared when
                                                                 calibration is complete. For diagnostic use only. */
        uint64_t dis                   : 1;  /**< [  1:  1](R/W/H) Disable the opportunistic low latency mode for all ports. For diagnostic use only. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_iobnx_ncbx_ctl bdk_iobnx_ncbx_ctl_t;

static inline uint64_t BDK_IOBNX_NCBX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b==0)))
        return 0x87e0f0004000ll + 0x1000000ll * ((a) & 0x0) + 0x100ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0f0004000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0f0004000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=2)))
        return 0x87e0f00f6000ll + 0x1000000ll * ((a) & 0x1) + 0x400ll * ((b) & 0x3);
    __bdk_csr_fatal("IOBNX_NCBX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_NCBX_CTL(a,b) bdk_iobnx_ncbx_ctl_t
#define bustype_BDK_IOBNX_NCBX_CTL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBX_CTL(a,b) "IOBNX_NCBX_CTL"
#define device_bar_BDK_IOBNX_NCBX_CTL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBX_CTL(a,b) (a)
#define arguments_BDK_IOBNX_NCBX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_ncb#_mrml_permit_shadow
 *
 * INTERNAL: IOBN NCB Access Registers
 *
 * This register provides a way to read back IOB's IOB captures writes to MRML's
 * MRML_NCB()_PERMIT. For diagnostic use only.
 */
union bdk_iobnx_ncbx_mrml_permit_shadow
{
    uint64_t u;
    struct bdk_iobnx_ncbx_mrml_permit_shadow_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t kill                  : 1;  /**< [  7:  7](SRO/H) Kill the device. Once written with one, stays
                                                                 set until warm chip reset.  If set, no access
                                                                 allowed by any initiator. */
        uint64_t reserved_2_6          : 5;
        uint64_t nsec_dis              : 1;  /**< [  1:  1](SRO/H) Nonsecure disable. As with [SEC_DIS], but for accesses initiated by non-secure devices */
        uint64_t sec_dis               : 1;  /**< [  0:  0](SRO/H) Secure disable. */
#else /* Word 0 - Little Endian */
        uint64_t sec_dis               : 1;  /**< [  0:  0](SRO/H) Secure disable. */
        uint64_t nsec_dis              : 1;  /**< [  1:  1](SRO/H) Nonsecure disable. As with [SEC_DIS], but for accesses initiated by non-secure devices */
        uint64_t reserved_2_6          : 5;
        uint64_t kill                  : 1;  /**< [  7:  7](SRO/H) Kill the device. Once written with one, stays
                                                                 set until warm chip reset.  If set, no access
                                                                 allowed by any initiator. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncbx_mrml_permit_shadow_s cn; */
};
typedef union bdk_iobnx_ncbx_mrml_permit_shadow bdk_iobnx_ncbx_mrml_permit_shadow_t;

static inline uint64_t BDK_IOBNX_NCBX_MRML_PERMIT_SHADOW(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBX_MRML_PERMIT_SHADOW(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=255)))
        return 0x87e0f0090000ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0xff);
    __bdk_csr_fatal("IOBNX_NCBX_MRML_PERMIT_SHADOW", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_NCBX_MRML_PERMIT_SHADOW(a,b) bdk_iobnx_ncbx_mrml_permit_shadow_t
#define bustype_BDK_IOBNX_NCBX_MRML_PERMIT_SHADOW(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBX_MRML_PERMIT_SHADOW(a,b) "IOBNX_NCBX_MRML_PERMIT_SHADOW"
#define device_bar_BDK_IOBNX_NCBX_MRML_PERMIT_SHADOW(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBX_MRML_PERMIT_SHADOW(a,b) (a)
#define arguments_BDK_IOBNX_NCBX_MRML_PERMIT_SHADOW(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_ncb#_rw#_lat_pc
 *
 * IOBN NCB Latency Performance Counter Registers
 */
union bdk_iobnx_ncbx_rwx_lat_pc
{
    uint64_t u;
    struct bdk_iobnx_ncbx_rwx_lat_pc_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Latency performance counter.

                                                                 _ RW(0) increments every cycle by the number of read transactions that have
                                                                 entered IOB from the given NCB, but have not returned read data to the device.

                                                                 _ RW(1) increments every cycle by the number of write transactions that have
                                                                 entered IOB from the given NCB, but have not returned write commits to the
                                                                 device.

                                                                 This counter should be divided by IOBN()_NCB()_RW()_REQ_PC to determine each NCB
                                                                 bus's average read and write latency. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Latency performance counter.

                                                                 _ RW(0) increments every cycle by the number of read transactions that have
                                                                 entered IOB from the given NCB, but have not returned read data to the device.

                                                                 _ RW(1) increments every cycle by the number of write transactions that have
                                                                 entered IOB from the given NCB, but have not returned write commits to the
                                                                 device.

                                                                 This counter should be divided by IOBN()_NCB()_RW()_REQ_PC to determine each NCB
                                                                 bus's average read and write latency. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncbx_rwx_lat_pc_s cn; */
};
typedef union bdk_iobnx_ncbx_rwx_lat_pc bdk_iobnx_ncbx_rwx_lat_pc_t;

static inline uint64_t BDK_IOBNX_NCBX_RWX_LAT_PC(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBX_RWX_LAT_PC(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b==0) && (c<=1)))
        return 0x87e0f000d000ll + 0x1000000ll * ((a) & 0x0) + 0x100ll * ((b) & 0x0) + 0x10ll * ((c) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1) && (c<=1)))
        return 0x87e0f000d000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x1) + 0x10ll * ((c) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=2) && (c<=1)))
        return 0x87e0f000d000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x3) + 0x10ll * ((c) & 0x1);
    __bdk_csr_fatal("IOBNX_NCBX_RWX_LAT_PC", 3, a, b, c, 0);
}

#define typedef_BDK_IOBNX_NCBX_RWX_LAT_PC(a,b,c) bdk_iobnx_ncbx_rwx_lat_pc_t
#define bustype_BDK_IOBNX_NCBX_RWX_LAT_PC(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBX_RWX_LAT_PC(a,b,c) "IOBNX_NCBX_RWX_LAT_PC"
#define device_bar_BDK_IOBNX_NCBX_RWX_LAT_PC(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBX_RWX_LAT_PC(a,b,c) (a)
#define arguments_BDK_IOBNX_NCBX_RWX_LAT_PC(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) iobn#_ncb#_rw#_req_pc
 *
 * IOBN NCB Request Performance Counter Registers
 */
union bdk_iobnx_ncbx_rwx_req_pc
{
    uint64_t u;
    struct bdk_iobnx_ncbx_rwx_req_pc_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Request performance counter.

                                                                 _ RW(0) increments on read  transaction entering IOB on given NCB bus.

                                                                 _ RW(1) increments on write transaction entering IOB on given NCB bus. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Request performance counter.

                                                                 _ RW(0) increments on read  transaction entering IOB on given NCB bus.

                                                                 _ RW(1) increments on write transaction entering IOB on given NCB bus. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncbx_rwx_req_pc_s cn; */
};
typedef union bdk_iobnx_ncbx_rwx_req_pc bdk_iobnx_ncbx_rwx_req_pc_t;

static inline uint64_t BDK_IOBNX_NCBX_RWX_REQ_PC(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBX_RWX_REQ_PC(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b==0) && (c<=1)))
        return 0x87e0f000c000ll + 0x1000000ll * ((a) & 0x0) + 0x100ll * ((b) & 0x0) + 0x10ll * ((c) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1) && (c<=1)))
        return 0x87e0f000c000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x1) + 0x10ll * ((c) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS2_X) && ((a<=1) && (b<=1) && (c<=1)))
        return 0x87e0f000c000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x1) + 0x10ll * ((c) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=2) && (c<=1)))
        return 0x87e0f000c000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x3) + 0x10ll * ((c) & 0x1);
    __bdk_csr_fatal("IOBNX_NCBX_RWX_REQ_PC", 3, a, b, c, 0);
}

#define typedef_BDK_IOBNX_NCBX_RWX_REQ_PC(a,b,c) bdk_iobnx_ncbx_rwx_req_pc_t
#define bustype_BDK_IOBNX_NCBX_RWX_REQ_PC(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBX_RWX_REQ_PC(a,b,c) "IOBNX_NCBX_RWX_REQ_PC"
#define device_bar_BDK_IOBNX_NCBX_RWX_REQ_PC(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBX_RWX_REQ_PC(a,b,c) (a)
#define arguments_BDK_IOBNX_NCBX_RWX_REQ_PC(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) iobn#_ncb#_rw#_smmu_lat_pc
 *
 * IOBN NCB SMMU Latency Performance Counter Registers
 */
union bdk_iobnx_ncbx_rwx_smmu_lat_pc
{
    uint64_t u;
    struct bdk_iobnx_ncbx_rwx_smmu_lat_pc_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) SMMU latency performance counter.

                                                                 _ RW(0) increments every cycle by the number of read transactions that have
                                                                 entered IOB from the given NCB, but have not been address translated by the
                                                                 SMMU.

                                                                 _ RW(1) increments by the number of write transactions that have entered IOB
                                                                 from the given NCB, but have not been address translated by the SMMU.

                                                                 This counter should be divided by IOBN()_NCB()_RW()_REQ_PC to determine each NCB
                                                                 bus's average read and write SMMU plus IOB front-end latency. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) SMMU latency performance counter.

                                                                 _ RW(0) increments every cycle by the number of read transactions that have
                                                                 entered IOB from the given NCB, but have not been address translated by the
                                                                 SMMU.

                                                                 _ RW(1) increments by the number of write transactions that have entered IOB
                                                                 from the given NCB, but have not been address translated by the SMMU.

                                                                 This counter should be divided by IOBN()_NCB()_RW()_REQ_PC to determine each NCB
                                                                 bus's average read and write SMMU plus IOB front-end latency. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncbx_rwx_smmu_lat_pc_s cn; */
};
typedef union bdk_iobnx_ncbx_rwx_smmu_lat_pc bdk_iobnx_ncbx_rwx_smmu_lat_pc_t;

static inline uint64_t BDK_IOBNX_NCBX_RWX_SMMU_LAT_PC(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBX_RWX_SMMU_LAT_PC(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b==0) && (c<=1)))
        return 0x87e0f000e000ll + 0x1000000ll * ((a) & 0x0) + 0x100ll * ((b) & 0x0) + 0x10ll * ((c) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1) && (c<=1)))
        return 0x87e0f000e000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x1) + 0x10ll * ((c) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=2) && (c<=1)))
        return 0x87e0f000e000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x3) + 0x10ll * ((c) & 0x1);
    __bdk_csr_fatal("IOBNX_NCBX_RWX_SMMU_LAT_PC", 3, a, b, c, 0);
}

#define typedef_BDK_IOBNX_NCBX_RWX_SMMU_LAT_PC(a,b,c) bdk_iobnx_ncbx_rwx_smmu_lat_pc_t
#define bustype_BDK_IOBNX_NCBX_RWX_SMMU_LAT_PC(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBX_RWX_SMMU_LAT_PC(a,b,c) "IOBNX_NCBX_RWX_SMMU_LAT_PC"
#define device_bar_BDK_IOBNX_NCBX_RWX_SMMU_LAT_PC(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBX_RWX_SMMU_LAT_PC(a,b,c) (a)
#define arguments_BDK_IOBNX_NCBX_RWX_SMMU_LAT_PC(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) iobn#_ncb0_hp
 *
 * IOBN NCBI0 High Performance Register
 */
union bdk_iobnx_ncb0_hp
{
    uint64_t u;
    struct bdk_iobnx_ncb0_hp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t lp                    : 1;  /**< [  3:  3](R/W) For IOBN0 the reset value for this is 0x1. For IOBN1 the reset value is 0x0.
                                                                 When set, NCBI 0 ARB 0 request port 3 will use the low performance path through ARB 0. */
        uint64_t hp                    : 3;  /**< [  2:  0](R/W) When set, NCBI 0 ARB 0 will use the high performance path through the IOBN.
                                                                 Software typically must have IOB(0)_NCB0_HP[HP] set, and IOB(1)_NCB0_HP[HP] clear. */
#else /* Word 0 - Little Endian */
        uint64_t hp                    : 3;  /**< [  2:  0](R/W) When set, NCBI 0 ARB 0 will use the high performance path through the IOBN.
                                                                 Software typically must have IOB(0)_NCB0_HP[HP] set, and IOB(1)_NCB0_HP[HP] clear. */
        uint64_t lp                    : 1;  /**< [  3:  3](R/W) For IOBN0 the reset value for this is 0x1. For IOBN1 the reset value is 0x0.
                                                                 When set, NCBI 0 ARB 0 request port 3 will use the low performance path through ARB 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    struct bdk_iobnx_ncb0_hp_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t lp                    : 1;  /**< [  3:  3](R/W) When set, NCBI 0 ARB 0 request port 3 will use the low performance path through ARB 0. */
        uint64_t hp                    : 3;  /**< [  2:  0](R/W) When set, NCBI 0 ARB 0 for request ports 2..0 will use the high performance path through
                                                                 the IOBN.
                                                                 Software typically must have IOB(0)_NCB0_HP[HP] = 0x1. */
#else /* Word 0 - Little Endian */
        uint64_t hp                    : 3;  /**< [  2:  0](R/W) When set, NCBI 0 ARB 0 for request ports 2..0 will use the high performance path through
                                                                 the IOBN.
                                                                 Software typically must have IOB(0)_NCB0_HP[HP] = 0x1. */
        uint64_t lp                    : 1;  /**< [  3:  3](R/W) When set, NCBI 0 ARB 0 request port 3 will use the low performance path through ARB 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_iobnx_ncb0_hp_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t hp                    : 1;  /**< [  0:  0](R/W) When set, NCBI 0 ARB 0 will use the high performance path through the IOBN.
                                                                 Software typically must have IOB(0)_NCB0_HP[HP] set, and IOB(1)_NCB0_HP[HP] clear. */
#else /* Word 0 - Little Endian */
        uint64_t hp                    : 1;  /**< [  0:  0](R/W) When set, NCBI 0 ARB 0 will use the high performance path through the IOBN.
                                                                 Software typically must have IOB(0)_NCB0_HP[HP] set, and IOB(1)_NCB0_HP[HP] clear. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_iobnx_ncb0_hp_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t lp                    : 1;  /**< [  3:  3](R/W) For IOBN0 the reset value for this is 0x1. For IOBN1 the reset value is 0x0.
                                                                 When set, NCBI 0 ARB 0 request port 3 will use the low performance path through ARB 0. */
        uint64_t hp                    : 3;  /**< [  2:  0](R/W) For IOBN0 the reset value for this is 0x7. For IOBN1 the reset value is 0x0.
                                                                 When set, NCBI 0 ARB 0 for request ports 2..0 will use the high performance path through
                                                                 the IOBN.
                                                                 Software typically must have IOB(0)_NCB0_HP[HP] = 0x7, and IOB(1)_NCB0_HP[HP] = 0x0. */
#else /* Word 0 - Little Endian */
        uint64_t hp                    : 3;  /**< [  2:  0](R/W) For IOBN0 the reset value for this is 0x7. For IOBN1 the reset value is 0x0.
                                                                 When set, NCBI 0 ARB 0 for request ports 2..0 will use the high performance path through
                                                                 the IOBN.
                                                                 Software typically must have IOB(0)_NCB0_HP[HP] = 0x7, and IOB(1)_NCB0_HP[HP] = 0x0. */
        uint64_t lp                    : 1;  /**< [  3:  3](R/W) For IOBN0 the reset value for this is 0x1. For IOBN1 the reset value is 0x0.
                                                                 When set, NCBI 0 ARB 0 request port 3 will use the low performance path through ARB 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_iobnx_ncb0_hp bdk_iobnx_ncb0_hp_t;

static inline uint64_t BDK_IOBNX_NCB0_HP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCB0_HP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f0003008ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f0003008ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f0003008ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_NCB0_HP", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_NCB0_HP(a) bdk_iobnx_ncb0_hp_t
#define bustype_BDK_IOBNX_NCB0_HP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCB0_HP(a) "IOBNX_NCB0_HP"
#define device_bar_BDK_IOBNX_NCB0_HP(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCB0_HP(a) (a)
#define arguments_BDK_IOBNX_NCB0_HP(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_ncb0_sdis#
 *
 * IOBN NCB Secure Disable Register
 */
union bdk_iobnx_ncb0_sdisx
{
    uint64_t u;
    struct bdk_iobnx_ncb0_sdisx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t did                   : 64; /**< [ 63:  0](SR/W) When set a secure operation is required to access the NCBDID. If a nonsecure
                                                                 operation occurs it will result in a R/W to ECAM0_NOP_ZF.

                                                                 Index 0 corresponds to DIDs 63:0, index 1 to DIDs 127:64, index 2 to DISs
                                                                 191:128, and index 3 to DIDs 255:192.

                                                                 Each IOB instance should be programmed identically, and should also be identical
                                                                 to MRML_NCB()_SDEV. */
#else /* Word 0 - Little Endian */
        uint64_t did                   : 64; /**< [ 63:  0](SR/W) When set a secure operation is required to access the NCBDID. If a nonsecure
                                                                 operation occurs it will result in a R/W to ECAM0_NOP_ZF.

                                                                 Index 0 corresponds to DIDs 63:0, index 1 to DIDs 127:64, index 2 to DISs
                                                                 191:128, and index 3 to DIDs 255:192.

                                                                 Each IOB instance should be programmed identically, and should also be identical
                                                                 to MRML_NCB()_SDEV. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncb0_sdisx_s cn; */
};
typedef union bdk_iobnx_ncb0_sdisx bdk_iobnx_ncb0_sdisx_t;

static inline uint64_t BDK_IOBNX_NCB0_SDISX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCB0_SDISX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=3)))
        return 0x87e0f0002000ll + 0x1000000ll * ((a) & 0x0) + 0x100ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=3)))
        return 0x87e0f0002000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=3)))
        return 0x87e0f0002000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x3);
    __bdk_csr_fatal("IOBNX_NCB0_SDISX", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_NCB0_SDISX(a,b) bdk_iobnx_ncb0_sdisx_t
#define bustype_BDK_IOBNX_NCB0_SDISX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCB0_SDISX(a,b) "IOBNX_NCB0_SDISX"
#define device_bar_BDK_IOBNX_NCB0_SDISX(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCB0_SDISX(a,b) (a)
#define arguments_BDK_IOBNX_NCB0_SDISX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_ncb0_skill#
 *
 * IOBN NCB Secure Kill-Device Registers
 */
union bdk_iobnx_ncb0_skillx
{
    uint64_t u;
    struct bdk_iobnx_ncb0_skillx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t skill                 : 64; /**< [ 63:  0](SR/W1S) NCB function kill. If set, any operation to this device will be will be directed
                                                                 to ECAM0_NOP_ZF. Write one to set, once set cannot be cleared until soft reset.

                                                                 Index 0 corresponds to DIDs 63:0, index 1 to DIDs 127:64, index 2 to DISs
                                                                 191:128, and index 3 to DIDs 255:192.

                                                                 Each IOB instance should be programmed identically, and should also be identical
                                                                 to MRML_NCB()_SKILL. */
#else /* Word 0 - Little Endian */
        uint64_t skill                 : 64; /**< [ 63:  0](SR/W1S) NCB function kill. If set, any operation to this device will be will be directed
                                                                 to ECAM0_NOP_ZF. Write one to set, once set cannot be cleared until soft reset.

                                                                 Index 0 corresponds to DIDs 63:0, index 1 to DIDs 127:64, index 2 to DISs
                                                                 191:128, and index 3 to DIDs 255:192.

                                                                 Each IOB instance should be programmed identically, and should also be identical
                                                                 to MRML_NCB()_SKILL. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncb0_skillx_s cn; */
};
typedef union bdk_iobnx_ncb0_skillx bdk_iobnx_ncb0_skillx_t;

static inline uint64_t BDK_IOBNX_NCB0_SKILLX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCB0_SKILLX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=3)))
        return 0x87e0f000b000ll + 0x1000000ll * ((a) & 0x0) + 0x100ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=3)))
        return 0x87e0f000b000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS2_X) && ((a<=1) && (b<=3)))
        return 0x87e0f000b000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x3);
    __bdk_csr_fatal("IOBNX_NCB0_SKILLX", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_NCB0_SKILLX(a,b) bdk_iobnx_ncb0_skillx_t
#define bustype_BDK_IOBNX_NCB0_SKILLX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCB0_SKILLX(a,b) "IOBNX_NCB0_SKILLX"
#define device_bar_BDK_IOBNX_NCB0_SKILLX(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCB0_SKILLX(a,b) (a)
#define arguments_BDK_IOBNX_NCB0_SKILLX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_ncbo#_psn_status
 *
 * IOBN NCBO Poison Status Register
 */
union bdk_iobnx_ncbox_psn_status
{
    uint64_t u;
    struct bdk_iobnx_ncbox_psn_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_52_63        : 12;
        uint64_t address               : 52; /**< [ 51:  0](RO/H) Captured address when poison transaction was sent on NCBO. Valid when
                                                                 corresponding bit is set in IOBN()_INT_SUM .
                                                                 When corresponding bit in IOBN()_INT_SUM is cleared allows a new poison error to be latched. */
#else /* Word 0 - Little Endian */
        uint64_t address               : 52; /**< [ 51:  0](RO/H) Captured address when poison transaction was sent on NCBO. Valid when
                                                                 corresponding bit is set in IOBN()_INT_SUM .
                                                                 When corresponding bit in IOBN()_INT_SUM is cleared allows a new poison error to be latched. */
        uint64_t reserved_52_63        : 12;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncbox_psn_status_s cn; */
};
typedef union bdk_iobnx_ncbox_psn_status bdk_iobnx_ncbox_psn_status_t;

static inline uint64_t BDK_IOBNX_NCBOX_PSN_STATUS(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBOX_PSN_STATUS(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=2)))
        return 0x87e0f0003060ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x3);
    __bdk_csr_fatal("IOBNX_NCBOX_PSN_STATUS", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_NCBOX_PSN_STATUS(a,b) bdk_iobnx_ncbox_psn_status_t
#define bustype_BDK_IOBNX_NCBOX_PSN_STATUS(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBOX_PSN_STATUS(a,b) "IOBNX_NCBOX_PSN_STATUS"
#define device_bar_BDK_IOBNX_NCBOX_PSN_STATUS(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBOX_PSN_STATUS(a,b) (a)
#define arguments_BDK_IOBNX_NCBOX_PSN_STATUS(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_ncbo_ctl
 *
 * IOBN NCBO control Registers
 * This register set controls for NCBO processing.
 */
union bdk_iobnx_ncbo_ctl
{
    uint64_t u;
    struct bdk_iobnx_ncbo_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_7_63         : 57;
        uint64_t clken                 : 3;  /**< [  6:  4](R/W) Force the NCBO clock enable to be always on. For diagnostic use only.
                                                                 Each bit is control for a given ncb bus, where bit 4 is for ncb bus 0 and bit 6 is for ncb bus 2. */
        uint64_t reserved_3            : 1;
        uint64_t bypass_ena            : 3;  /**< [  2:  0](R/W) When set allows requests to bypass FIFO and go directly to output of NCBO bus, when FIFO is empty.
                                                                 Each bit is control for a given ncb bus, where bit 0 is for ncb bus 0 and bit 2 is for ncb bus 2.

                                                                 Internal:
                                                                 FIXME rename bypass_dis and default to on (0x0), and then describe as diag only. */
#else /* Word 0 - Little Endian */
        uint64_t bypass_ena            : 3;  /**< [  2:  0](R/W) When set allows requests to bypass FIFO and go directly to output of NCBO bus, when FIFO is empty.
                                                                 Each bit is control for a given ncb bus, where bit 0 is for ncb bus 0 and bit 2 is for ncb bus 2.

                                                                 Internal:
                                                                 FIXME rename bypass_dis and default to on (0x0), and then describe as diag only. */
        uint64_t reserved_3            : 1;
        uint64_t clken                 : 3;  /**< [  6:  4](R/W) Force the NCBO clock enable to be always on. For diagnostic use only.
                                                                 Each bit is control for a given ncb bus, where bit 4 is for ncb bus 0 and bit 6 is for ncb bus 2. */
        uint64_t reserved_7_63         : 57;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncbo_ctl_s cn; */
};
typedef union bdk_iobnx_ncbo_ctl bdk_iobnx_ncbo_ctl_t;

static inline uint64_t BDK_IOBNX_NCBO_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBO_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f0002200ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_NCBO_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_NCBO_CTL(a) bdk_iobnx_ncbo_ctl_t
#define bustype_BDK_IOBNX_NCBO_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBO_CTL(a) "IOBNX_NCBO_CTL"
#define device_bar_BDK_IOBNX_NCBO_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBO_CTL(a) (a)
#define arguments_BDK_IOBNX_NCBO_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_ncbo_to
 *
 * IOBN NCBO Timeout Counter Registers
 * This register set the counter value for expected return data on NCBI.
 */
union bdk_iobnx_ncbo_to
{
    uint64_t u;
    struct bdk_iobnx_ncbo_to_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t sub_time              : 32; /**< [ 31:  0](R/W) Timeout value. When a load operation is sent on NCBO, the timeout counter is
                                                                 started, and if return data on NCBI does not return within between three to four
                                                                 times the value of [SUB_TIME] in coprocessor-clocks, a timeout occurs and
                                                                 IOBN()_INT_SUM[NCBO_TO] is set. 0x0 disables. */
#else /* Word 0 - Little Endian */
        uint64_t sub_time              : 32; /**< [ 31:  0](R/W) Timeout value. When a load operation is sent on NCBO, the timeout counter is
                                                                 started, and if return data on NCBI does not return within between three to four
                                                                 times the value of [SUB_TIME] in coprocessor-clocks, a timeout occurs and
                                                                 IOBN()_INT_SUM[NCBO_TO] is set. 0x0 disables. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncbo_to_s cn; */
};
typedef union bdk_iobnx_ncbo_to bdk_iobnx_ncbo_to_t;

static inline uint64_t BDK_IOBNX_NCBO_TO(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBO_TO(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f0000008ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_NCBO_TO", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_NCBO_TO(a) bdk_iobnx_ncbo_to_t
#define bustype_BDK_IOBNX_NCBO_TO(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBO_TO(a) "IOBNX_NCBO_TO"
#define device_bar_BDK_IOBNX_NCBO_TO(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBO_TO(a) (a)
#define arguments_BDK_IOBNX_NCBO_TO(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_ncbo_to_err
 *
 * IOBN NCB Timeout Error Register
 * This register captures error information for a non-posted request that times out on
 * NCBO (when IOBN()_INT_SUM[NCBO_TO] is set).
 */
union bdk_iobnx_ncbo_to_err
{
    uint64_t u;
    struct bdk_iobnx_ncbo_to_err_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_17_63        : 47;
        uint64_t cpid                  : 9;  /**< [ 16:  8](RO/H) CPID for NP request that timed out on NCBO. */
        uint64_t reserved_5_7          : 3;
        uint64_t arbid                 : 5;  /**< [  4:  0](RO/H) Flat ARBID for NP request that timed out on NCBO. */
#else /* Word 0 - Little Endian */
        uint64_t arbid                 : 5;  /**< [  4:  0](RO/H) Flat ARBID for NP request that timed out on NCBO. */
        uint64_t reserved_5_7          : 3;
        uint64_t cpid                  : 9;  /**< [ 16:  8](RO/H) CPID for NP request that timed out on NCBO. */
        uint64_t reserved_17_63        : 47;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_ncbo_to_err_s cn; */
};
typedef union bdk_iobnx_ncbo_to_err bdk_iobnx_ncbo_to_err_t;

static inline uint64_t BDK_IOBNX_NCBO_TO_ERR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_NCBO_TO_ERR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f00a0000ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_NCBO_TO_ERR", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_NCBO_TO_ERR(a) bdk_iobnx_ncbo_to_err_t
#define bustype_BDK_IOBNX_NCBO_TO_ERR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_NCBO_TO_ERR(a) "IOBNX_NCBO_TO_ERR"
#define device_bar_BDK_IOBNX_NCBO_TO_ERR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_NCBO_TO_ERR(a) (a)
#define arguments_BDK_IOBNX_NCBO_TO_ERR(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_psn_ctl
 *
 * Poison Control Register
 */
union bdk_iobnx_psn_ctl
{
    uint64_t u;
    struct bdk_iobnx_psn_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t dispsn                : 1;  /**< [  0:  0](R/W) Disable poison code creation and detection in the mesh / NCB ECC
                                                                 checkers/generators. */
#else /* Word 0 - Little Endian */
        uint64_t dispsn                : 1;  /**< [  0:  0](R/W) Disable poison code creation and detection in the mesh / NCB ECC
                                                                 checkers/generators. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_psn_ctl_s cn; */
};
typedef union bdk_iobnx_psn_ctl bdk_iobnx_psn_ctl_t;

static inline uint64_t BDK_IOBNX_PSN_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_PSN_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f0003050ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_PSN_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_PSN_CTL(a) bdk_iobnx_psn_ctl_t
#define bustype_BDK_IOBNX_PSN_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_PSN_CTL(a) "IOBNX_PSN_CTL"
#define device_bar_BDK_IOBNX_PSN_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_PSN_CTL(a) (a)
#define arguments_BDK_IOBNX_PSN_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_roc_dll
 *
 * INTERNAL: IOBN Global Core-Clock DLL Status Register
 *
 * Status of the ROC core-clock DLL.
 */
union bdk_iobnx_roc_dll
{
    uint64_t u;
    struct bdk_iobnx_roc_dll_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_60_63        : 4;
        uint64_t max_dll_setting       : 12; /**< [ 59: 48](RO/H) Max reported DLL setting. */
        uint64_t min_dll_setting       : 12; /**< [ 47: 36](RO/H) Min reported DLL setting. */
        uint64_t reserved_32_35        : 4;
        uint64_t pdr_rclk_refclk       : 1;  /**< [ 31: 31](RO/H) Synchronized pdr_rclk_refclk from ROC core-clock DLL cmb0 phase detectors. */
        uint64_t pdl_rclk_refclk       : 1;  /**< [ 30: 30](RO/H) Synchronized pdl_rclk_refclk from ROC core-clock DLL cmb0 phase detectors. */
        uint64_t pd_pos_rclk_refclk    : 1;  /**< [ 29: 29](RO/H) Synchronized pd_pos_rclk_refclk from ROC core-clock DLL cmb0 phase detectors. */
        uint64_t dll_lock              : 1;  /**< [ 28: 28](RO/H) The dll_lock signal from ROC core-clock DLL, from the positive edge of refclk. */
        uint64_t dll_dly_elem_en       : 16; /**< [ 27: 12](RO/H) The ROC core-clock delay element enable setting, from the negative edge of refclk. */
        uint64_t dll_setting           : 12; /**< [ 11:  0](RO/H) The ROC core-clock DLL setting, from the negative edge of refclk. */
#else /* Word 0 - Little Endian */
        uint64_t dll_setting           : 12; /**< [ 11:  0](RO/H) The ROC core-clock DLL setting, from the negative edge of refclk. */
        uint64_t dll_dly_elem_en       : 16; /**< [ 27: 12](RO/H) The ROC core-clock delay element enable setting, from the negative edge of refclk. */
        uint64_t dll_lock              : 1;  /**< [ 28: 28](RO/H) The dll_lock signal from ROC core-clock DLL, from the positive edge of refclk. */
        uint64_t pd_pos_rclk_refclk    : 1;  /**< [ 29: 29](RO/H) Synchronized pd_pos_rclk_refclk from ROC core-clock DLL cmb0 phase detectors. */
        uint64_t pdl_rclk_refclk       : 1;  /**< [ 30: 30](RO/H) Synchronized pdl_rclk_refclk from ROC core-clock DLL cmb0 phase detectors. */
        uint64_t pdr_rclk_refclk       : 1;  /**< [ 31: 31](RO/H) Synchronized pdr_rclk_refclk from ROC core-clock DLL cmb0 phase detectors. */
        uint64_t reserved_32_35        : 4;
        uint64_t min_dll_setting       : 12; /**< [ 47: 36](RO/H) Min reported DLL setting. */
        uint64_t max_dll_setting       : 12; /**< [ 59: 48](RO/H) Max reported DLL setting. */
        uint64_t reserved_60_63        : 4;
#endif /* Word 0 - End */
    } s;
    struct bdk_iobnx_roc_dll_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t pdr_rclk_refclk       : 1;  /**< [ 31: 31](RO/H) Synchronized pdr_rclk_refclk from ROC core-clock DLL cmb0 phase detectors. */
        uint64_t pdl_rclk_refclk       : 1;  /**< [ 30: 30](RO/H) Synchronized pdl_rclk_refclk from ROC core-clock DLL cmb0 phase detectors. */
        uint64_t pd_pos_rclk_refclk    : 1;  /**< [ 29: 29](RO/H) Synchronized pd_pos_rclk_refclk from ROC core-clock DLL cmb0 phase detectors. */
        uint64_t dll_lock              : 1;  /**< [ 28: 28](RO/H) The dll_lock signal from ROC core-clock DLL, from the positive edge of refclk. */
        uint64_t dll_dly_elem_en       : 16; /**< [ 27: 12](RO/H) The ROC core-clock delay element enable setting, from the negative edge of refclk. */
        uint64_t dll_setting           : 12; /**< [ 11:  0](RO/H) The ROC core-clock DLL setting, from the negative edge of refclk. */
#else /* Word 0 - Little Endian */
        uint64_t dll_setting           : 12; /**< [ 11:  0](RO/H) The ROC core-clock DLL setting, from the negative edge of refclk. */
        uint64_t dll_dly_elem_en       : 16; /**< [ 27: 12](RO/H) The ROC core-clock delay element enable setting, from the negative edge of refclk. */
        uint64_t dll_lock              : 1;  /**< [ 28: 28](RO/H) The dll_lock signal from ROC core-clock DLL, from the positive edge of refclk. */
        uint64_t pd_pos_rclk_refclk    : 1;  /**< [ 29: 29](RO/H) Synchronized pd_pos_rclk_refclk from ROC core-clock DLL cmb0 phase detectors. */
        uint64_t pdl_rclk_refclk       : 1;  /**< [ 30: 30](RO/H) Synchronized pdl_rclk_refclk from ROC core-clock DLL cmb0 phase detectors. */
        uint64_t pdr_rclk_refclk       : 1;  /**< [ 31: 31](RO/H) Synchronized pdr_rclk_refclk from ROC core-clock DLL cmb0 phase detectors. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } cn88xxp1;
    /* struct bdk_iobnx_roc_dll_s cn81xx; */
    /* struct bdk_iobnx_roc_dll_s cn83xx; */
    /* struct bdk_iobnx_roc_dll_s cn88xxp2; */
};
typedef union bdk_iobnx_roc_dll bdk_iobnx_roc_dll_t;

static inline uint64_t BDK_IOBNX_ROC_DLL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_ROC_DLL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f000a008ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f000a008ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f000a008ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_ROC_DLL", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_ROC_DLL(a) bdk_iobnx_roc_dll_t
#define bustype_BDK_IOBNX_ROC_DLL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_ROC_DLL(a) "IOBNX_ROC_DLL"
#define device_bar_BDK_IOBNX_ROC_DLL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_ROC_DLL(a) (a)
#define arguments_BDK_IOBNX_ROC_DLL(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_rsl#_streams
 *
 * IOBN RSL Stream Permission Registers
 * This register sets the permissions for a NCBI transaction (which are DMA
 * transactions or MSI-X writes), for requests from a RSL device, i.e.
 * those where:
 *
 *   _ stream_id\<21:8\> = PCC_DEV_CON_E::MRML\<21:8\>
 *
 * Index {b} corresponds to the PCC function number for the RSL device
 * (stream_id\<7:0\>).
 *
 * For each given index {b} (the RSL function number), each index {a} (the IOB number)
 * must be programmed to the same value.
 */
union bdk_iobnx_rslx_streams
{
    uint64_t u;
    struct bdk_iobnx_rslx_streams_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t strm_nsec             : 1;  /**< [  1:  1](SR/W) Stream nonsecure.

                                                                   0 = The device's stream ID is marked secure headed into the SMMU.  If the
                                                                   device is making a non-physical request, the SMMU will use secure world
                                                                   lookup. The SMMU may, if properly configured, generate an outgoing physical
                                                                   address that is secure.

                                                                   1 = The device's stream ID is marked nonsecure headed into the SMMU.  If the
                                                                   device is making a non-physical request, the SMMU will use nonsecure world
                                                                   lookup. The SMMU outgoing physical address will be nonsecure.

                                                                   [STRM_NSEC] is ignored if the device is making a physical request (as these
                                                                   transactions bypass the SMMU translation process).

                                                                   Typically firmware sets [PHYS_NSEC] and [STRM_NSEC] the same. */
        uint64_t phys_nsec             : 1;  /**< [  0:  0](SR/W) Physical nonsecure.
                                                                   0 = When the device makes a physical request, IOB will use the device's
                                                                   requested secure bit to determine if the request to DRAM/LLC is secure or not.
                                                                   1 = When the device makes a physical request, IOB will squash the
                                                                   device's secure request and issue the request to DRAM/LLC as nonsecure.

                                                                   Ignored if a device makes a non-physical request.  (As non-physical requests
                                                                   cause the SMMU to generate the SMMU-outgoing secure bit based on the SMMU
                                                                   translation process, including [STRM_NSEC].)

                                                                   Typically firmware sets [PHYS_NSEC] and [STRM_NSEC] the same. */
#else /* Word 0 - Little Endian */
        uint64_t phys_nsec             : 1;  /**< [  0:  0](SR/W) Physical nonsecure.
                                                                   0 = When the device makes a physical request, IOB will use the device's
                                                                   requested secure bit to determine if the request to DRAM/LLC is secure or not.
                                                                   1 = When the device makes a physical request, IOB will squash the
                                                                   device's secure request and issue the request to DRAM/LLC as nonsecure.

                                                                   Ignored if a device makes a non-physical request.  (As non-physical requests
                                                                   cause the SMMU to generate the SMMU-outgoing secure bit based on the SMMU
                                                                   translation process, including [STRM_NSEC].)

                                                                   Typically firmware sets [PHYS_NSEC] and [STRM_NSEC] the same. */
        uint64_t strm_nsec             : 1;  /**< [  1:  1](SR/W) Stream nonsecure.

                                                                   0 = The device's stream ID is marked secure headed into the SMMU.  If the
                                                                   device is making a non-physical request, the SMMU will use secure world
                                                                   lookup. The SMMU may, if properly configured, generate an outgoing physical
                                                                   address that is secure.

                                                                   1 = The device's stream ID is marked nonsecure headed into the SMMU.  If the
                                                                   device is making a non-physical request, the SMMU will use nonsecure world
                                                                   lookup. The SMMU outgoing physical address will be nonsecure.

                                                                   [STRM_NSEC] is ignored if the device is making a physical request (as these
                                                                   transactions bypass the SMMU translation process).

                                                                   Typically firmware sets [PHYS_NSEC] and [STRM_NSEC] the same. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_rslx_streams_s cn; */
};
typedef union bdk_iobnx_rslx_streams bdk_iobnx_rslx_streams_t;

static inline uint64_t BDK_IOBNX_RSLX_STREAMS(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_RSLX_STREAMS(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=255)))
        return 0x87e0f0002800ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0xff);
    __bdk_csr_fatal("IOBNX_RSLX_STREAMS", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_RSLX_STREAMS(a,b) bdk_iobnx_rslx_streams_t
#define bustype_BDK_IOBNX_RSLX_STREAMS(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_RSLX_STREAMS(a,b) "IOBNX_RSLX_STREAMS"
#define device_bar_BDK_IOBNX_RSLX_STREAMS(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_RSLX_STREAMS(a,b) (a)
#define arguments_BDK_IOBNX_RSLX_STREAMS(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_rvu_block#_const
 *
 * IOBN RVU BLOCK Constant Registers
 * This register returns discovery information for each RVU BLOCK, where RVU BLOCK is
 * enumerated by RVU_BLOCK_ADDR_E, in rvu.csr
 * Each IOB returns identical information.
 */
union bdk_iobnx_rvu_blockx_const
{
    uint64_t u;
    struct bdk_iobnx_rvu_blockx_const_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t farbid                : 8;  /**< [ 23: 16](RO) Flat ARBID for the given NCB ID. */
        uint64_t arbid                 : 8;  /**< [ 15:  8](RO) ARBID for the given RVU BLOCK. */
        uint64_t ncb                   : 4;  /**< [  7:  4](RO) Physical bus number for the given RVU BLOCK. */
        uint64_t iob                   : 3;  /**< [  3:  1](RO) IOB number for the given RVU BLOCK. */
        uint64_t valid                 : 1;  /**< [  0:  0](RO) Set if this RVU BLOCK is a valid ID. */
#else /* Word 0 - Little Endian */
        uint64_t valid                 : 1;  /**< [  0:  0](RO) Set if this RVU BLOCK is a valid ID. */
        uint64_t iob                   : 3;  /**< [  3:  1](RO) IOB number for the given RVU BLOCK. */
        uint64_t ncb                   : 4;  /**< [  7:  4](RO) Physical bus number for the given RVU BLOCK. */
        uint64_t arbid                 : 8;  /**< [ 15:  8](RO) ARBID for the given RVU BLOCK. */
        uint64_t farbid                : 8;  /**< [ 23: 16](RO) Flat ARBID for the given NCB ID. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_rvu_blockx_const_s cn; */
};
typedef union bdk_iobnx_rvu_blockx_const bdk_iobnx_rvu_blockx_const_t;

static inline uint64_t BDK_IOBNX_RVU_BLOCKX_CONST(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_RVU_BLOCKX_CONST(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=31)))
        return 0x87e0f0001800ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x1f);
    __bdk_csr_fatal("IOBNX_RVU_BLOCKX_CONST", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_RVU_BLOCKX_CONST(a,b) bdk_iobnx_rvu_blockx_const_t
#define bustype_BDK_IOBNX_RVU_BLOCKX_CONST(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_RVU_BLOCKX_CONST(a,b) "IOBNX_RVU_BLOCKX_CONST"
#define device_bar_BDK_IOBNX_RVU_BLOCKX_CONST(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_RVU_BLOCKX_CONST(a,b) (a)
#define arguments_BDK_IOBNX_RVU_BLOCKX_CONST(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_scratch
 *
 * INTERNAL: IOBN Scratch Register
 */
union bdk_iobnx_scratch
{
    uint64_t u;
    struct bdk_iobnx_scratch_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) Test register for CSR access. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) Test register for CSR access. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_scratch_s cn; */
};
typedef union bdk_iobnx_scratch bdk_iobnx_scratch_t;

static inline uint64_t BDK_IOBNX_SCRATCH(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_SCRATCH(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f0003020ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f0003020ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f0003020ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0f0003020ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_SCRATCH", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_SCRATCH(a) bdk_iobnx_scratch_t
#define bustype_BDK_IOBNX_SCRATCH(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_SCRATCH(a) "IOBNX_SCRATCH"
#define device_bar_BDK_IOBNX_SCRATCH(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_SCRATCH(a) (a)
#define arguments_BDK_IOBNX_SCRATCH(a) (a),-1,-1,-1

/**
 * Register (RSL) iobn#_slitag#_control
 *
 * IOBN Control Register
 * This register contains various control bits for IOBN functionality.
 */
union bdk_iobnx_slitagx_control
{
    uint64_t u;
    struct bdk_iobnx_slitagx_control_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t bits_dis              : 1;  /**< [  8:  8](RAZ) Reserved. */
        uint64_t reserved_4_7          : 4;
        uint64_t ld_ld_ord             : 1;  /**< [  3:  3](R/W) Enforce load-following-load ordering for SLI operations. A load operation must
                                                                 wait for all previous load operations' FILLs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t ld_st_ord             : 1;  /**< [  2:  2](R/W) Enforce load-following-store ordering for SLI operations. A load operation must
                                                                 wait for all previous store operations' STDNs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t st_ld_ord             : 1;  /**< [  1:  1](R/W) Enforce store-following-load ordering for SLI operations. A store operation must
                                                                 wait for all previous load operations' FILLs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t st_st_ord             : 1;  /**< [  0:  0](R/W) Enforce store-following-store ordering for SLI operations. A store operation must
                                                                 wait for all previous store operations' STDNs before issuing. */
#else /* Word 0 - Little Endian */
        uint64_t st_st_ord             : 1;  /**< [  0:  0](R/W) Enforce store-following-store ordering for SLI operations. A store operation must
                                                                 wait for all previous store operations' STDNs before issuing. */
        uint64_t st_ld_ord             : 1;  /**< [  1:  1](R/W) Enforce store-following-load ordering for SLI operations. A store operation must
                                                                 wait for all previous load operations' FILLs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t ld_st_ord             : 1;  /**< [  2:  2](R/W) Enforce load-following-store ordering for SLI operations. A load operation must
                                                                 wait for all previous store operations' STDNs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t ld_ld_ord             : 1;  /**< [  3:  3](R/W) Enforce load-following-load ordering for SLI operations. A load operation must
                                                                 wait for all previous load operations' FILLs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t reserved_4_7          : 4;
        uint64_t bits_dis              : 1;  /**< [  8:  8](RAZ) Reserved. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_iobnx_slitagx_control_s cn88xxp1; */
    struct bdk_iobnx_slitagx_control_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t bits_dis              : 1;  /**< [  8:  8](R/W) When set, disables stream validity checking. For diagnostic use only. */
        uint64_t reserved_4_7          : 4;
        uint64_t ld_ld_ord             : 1;  /**< [  3:  3](R/W) Enforce load-following-load ordering for SLI operations. A load operation must
                                                                 wait for all previous load operations' FILLs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t ld_st_ord             : 1;  /**< [  2:  2](R/W) Enforce load-following-store ordering for SLI operations. A load operation must
                                                                 wait for all previous store operations' STDNs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t st_ld_ord             : 1;  /**< [  1:  1](R/W) Enforce store-following-load ordering for SLI operations. A store operation must
                                                                 wait for all previous load operations' FILLs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t st_st_ord             : 1;  /**< [  0:  0](R/W) Enforce store-following-store ordering for SLI operations. A store operation must
                                                                 wait for all previous store operations' STDNs before issuing. */
#else /* Word 0 - Little Endian */
        uint64_t st_st_ord             : 1;  /**< [  0:  0](R/W) Enforce store-following-store ordering for SLI operations. A store operation must
                                                                 wait for all previous store operations' STDNs before issuing. */
        uint64_t st_ld_ord             : 1;  /**< [  1:  1](R/W) Enforce store-following-load ordering for SLI operations. A store operation must
                                                                 wait for all previous load operations' FILLs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t ld_st_ord             : 1;  /**< [  2:  2](R/W) Enforce load-following-store ordering for SLI operations. A load operation must
                                                                 wait for all previous store operations' STDNs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t ld_ld_ord             : 1;  /**< [  3:  3](R/W) Enforce load-following-load ordering for SLI operations. A load operation must
                                                                 wait for all previous load operations' FILLs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t reserved_4_7          : 4;
        uint64_t bits_dis              : 1;  /**< [  8:  8](R/W) When set, disables stream validity checking. For diagnostic use only. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_iobnx_slitagx_control_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t bits_dis              : 1;  /**< [  8:  8](SR/W) Bitstream disable.
                                                                 0 = Check inbound stream IDs from a PEM are between the secondary and
                                                                 subordinate bus numbers corresponding to that PEM (used when PEM is in host
                                                                 mode), or from the stream ID PCC_DEV_CON_E::PCIERC() (used when PEM is in
                                                                 endpoint mode). This prevents SR-IOV security issues.
                                                                 1 = Do not check inbound stream IDs. See PEM()_CTL_STREAM[EPSBBASE]. */
        uint64_t reserved_4_7          : 4;
        uint64_t ld_ld_ord             : 1;  /**< [  3:  3](R/W) Enforce load-following-load ordering for SLI operations. A load operation must
                                                                 wait for all previous load operations' FILLs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t ld_st_ord             : 1;  /**< [  2:  2](R/W) Enforce load-following-store ordering for SLI operations. A load operation must
                                                                 wait for all previous store operations' STDNs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t st_ld_ord             : 1;  /**< [  1:  1](R/W) Enforce store-following-load ordering for SLI operations. A store operation must
                                                                 wait for all previous load operations' FILLs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t st_st_ord             : 1;  /**< [  0:  0](R/W) Enforce store-following-store ordering for SLI operations. A store operation must
                                                                 wait for all previous store operations' STDNs before issuing. */
#else /* Word 0 - Little Endian */
        uint64_t st_st_ord             : 1;  /**< [  0:  0](R/W) Enforce store-following-store ordering for SLI operations. A store operation must
                                                                 wait for all previous store operations' STDNs before issuing. */
        uint64_t st_ld_ord             : 1;  /**< [  1:  1](R/W) Enforce store-following-load ordering for SLI operations. A store operation must
                                                                 wait for all previous load operations' FILLs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t ld_st_ord             : 1;  /**< [  2:  2](R/W) Enforce load-following-store ordering for SLI operations. A load operation must
                                                                 wait for all previous store operations' STDNs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t ld_ld_ord             : 1;  /**< [  3:  3](R/W) Enforce load-following-load ordering for SLI operations. A load operation must
                                                                 wait for all previous load operations' FILLs before issuing.

                                                                 Atomic transactions (which for PCI are non-posted so not part of normal store
                                                                 ordering) are also considered loads for the purpose of this bit. */
        uint64_t reserved_4_7          : 4;
        uint64_t bits_dis              : 1;  /**< [  8:  8](SR/W) Bitstream disable.
                                                                 0 = Check inbound stream IDs from a PEM are between the secondary and
                                                                 subordinate bus numbers corresponding to that PEM (used when PEM is in host
                                                                 mode), or from the stream ID PCC_DEV_CON_E::PCIERC() (used when PEM is in
                                                                 endpoint mode). This prevents SR-IOV security issues.
                                                                 1 = Do not check inbound stream IDs. See PEM()_CTL_STREAM[EPSBBASE]. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } cn83xx;
    /* struct bdk_iobnx_slitagx_control_cn81xx cn88xxp2; */
};
typedef union bdk_iobnx_slitagx_control bdk_iobnx_slitagx_control_t;

static inline uint64_t BDK_IOBNX_SLITAGX_CONTROL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_SLITAGX_CONTROL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=2)))
        return 0x87e0f0000000ll + 0x1000000ll * ((a) & 0x0) + 0x100ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=3)))
        return 0x87e0f0000000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=2)))
        return 0x87e0f0000000ll + 0x1000000ll * ((a) & 0x1) + 0x100ll * ((b) & 0x3);
    __bdk_csr_fatal("IOBNX_SLITAGX_CONTROL", 2, a, b, 0, 0);
}

#define typedef_BDK_IOBNX_SLITAGX_CONTROL(a,b) bdk_iobnx_slitagx_control_t
#define bustype_BDK_IOBNX_SLITAGX_CONTROL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_SLITAGX_CONTROL(a,b) "IOBNX_SLITAGX_CONTROL"
#define device_bar_BDK_IOBNX_SLITAGX_CONTROL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_SLITAGX_CONTROL(a,b) (a)
#define arguments_BDK_IOBNX_SLITAGX_CONTROL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) iobn#_test
 *
 * INTERNAL: IOBN Test Register
 */
union bdk_iobnx_test
{
    uint64_t u;
    struct bdk_iobnx_test_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t gibarb_testmode       : 1;  /**< [  0:  0](R/W) When set, the IOBN GIB arbiters will only grant one requestor at a time. */
#else /* Word 0 - Little Endian */
        uint64_t gibarb_testmode       : 1;  /**< [  0:  0](R/W) When set, the IOBN GIB arbiters will only grant one requestor at a time. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    struct bdk_iobnx_test_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t gibarb_testmode       : 1;  /**< [  0:  0](RO) When set, the IOBN GIB arbiters will only grant one requestor at a time. */
#else /* Word 0 - Little Endian */
        uint64_t gibarb_testmode       : 1;  /**< [  0:  0](RO) When set, the IOBN GIB arbiters will only grant one requestor at a time. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_iobnx_test_s cn88xx; */
    /* struct bdk_iobnx_test_cn81xx cn83xx; */
};
typedef union bdk_iobnx_test bdk_iobnx_test_t;

static inline uint64_t BDK_IOBNX_TEST(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_IOBNX_TEST(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0f0003010ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0f0003010ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0f0003010ll + 0x1000000ll * ((a) & 0x1);
    __bdk_csr_fatal("IOBNX_TEST", 1, a, 0, 0, 0);
}

#define typedef_BDK_IOBNX_TEST(a) bdk_iobnx_test_t
#define bustype_BDK_IOBNX_TEST(a) BDK_CSR_TYPE_RSL
#define basename_BDK_IOBNX_TEST(a) "IOBNX_TEST"
#define device_bar_BDK_IOBNX_TEST(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_IOBNX_TEST(a) (a)
#define arguments_BDK_IOBNX_TEST(a) (a),-1,-1,-1

#endif /* __BDK_CSRS_IOBN_H__ */
