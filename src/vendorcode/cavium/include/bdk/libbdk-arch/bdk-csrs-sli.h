#ifndef __BDK_CSRS_SLI_H__
#define __BDK_CSRS_SLI_H__
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
 * Cavium SLI.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration sdp_in_rams_e
 *
 * SDP Input RAMs Field Enumeration
 * Enumerates the relative bit positions within SDP(0)_ECC(1)_CTL[CDIS].
 */
#define BDK_SDP_IN_RAMS_E_CNTS (3)
#define BDK_SDP_IN_RAMS_E_DB (4)
#define BDK_SDP_IN_RAMS_E_DBELL (0xd)
#define BDK_SDP_IN_RAMS_E_DIR (5)
#define BDK_SDP_IN_RAMS_E_DMARSP0 (0)
#define BDK_SDP_IN_RAMS_E_DMARSP1 (1)
#define BDK_SDP_IN_RAMS_E_GTHR0 (8)
#define BDK_SDP_IN_RAMS_E_GTHR1 (9)
#define BDK_SDP_IN_RAMS_E_IHFD0 (6)
#define BDK_SDP_IN_RAMS_E_IHFD1 (7)
#define BDK_SDP_IN_RAMS_E_IND (0xb)
#define BDK_SDP_IN_RAMS_E_INFO (0xa)
#define BDK_SDP_IN_RAMS_E_LEVELS (0xc)
#define BDK_SDP_IN_RAMS_E_MBOX (0x10)
#define BDK_SDP_IN_RAMS_E_PERF (2)
#define BDK_SDP_IN_RAMS_E_PKTRSP (0xf)
#define BDK_SDP_IN_RAMS_E_X2P (0xe)

/**
 * Enumeration sdp_out_rams_e
 *
 * SDP Output RAMs Field Enumeration
 * Enumerates the relative bit positions within SDP(0)_ECC(0)_CTL[CDIS].
 */
#define BDK_SDP_OUT_RAMS_E_BISIZE (0)
#define BDK_SDP_OUT_RAMS_E_BPF0 (0xd)
#define BDK_SDP_OUT_RAMS_E_BPF1 (0xe)
#define BDK_SDP_OUT_RAMS_E_CNTS (2)
#define BDK_SDP_OUT_RAMS_E_DB (4)
#define BDK_SDP_OUT_RAMS_E_DBELL (3)
#define BDK_SDP_OUT_RAMS_E_DPLF_DIR (6)
#define BDK_SDP_OUT_RAMS_E_DPLF_IND (9)
#define BDK_SDP_OUT_RAMS_E_IB (7)
#define BDK_SDP_OUT_RAMS_E_INFO (0xa)
#define BDK_SDP_OUT_RAMS_E_IPLF_DIR (5)
#define BDK_SDP_OUT_RAMS_E_IPLF_IND (8)
#define BDK_SDP_OUT_RAMS_E_LEVELS (0xb)
#define BDK_SDP_OUT_RAMS_E_MSIX_ADDR (0x11)
#define BDK_SDP_OUT_RAMS_E_MSIX_DATA (0x12)
#define BDK_SDP_OUT_RAMS_E_P2X (0xc)
#define BDK_SDP_OUT_RAMS_E_PERF (1)
#define BDK_SDP_OUT_RAMS_E_TRACK0 (0xf)
#define BDK_SDP_OUT_RAMS_E_TRACK1 (0x10)

/**
 * Enumeration sli_bar_e
 *
 * SLI Base Address Register Enumeration
 * Enumerates the base address registers.
 */
#define BDK_SLI_BAR_E_SLIX_PF_BAR0_CN81XX(a) (0x874000000000ll + 0x1000000000ll * (a))
#define BDK_SLI_BAR_E_SLIX_PF_BAR0_CN81XX_SIZE 0x2000000ull
#define BDK_SLI_BAR_E_SLIX_PF_BAR0_CN88XX(a) (0x874000000000ll + 0x1000000000ll * (a))
#define BDK_SLI_BAR_E_SLIX_PF_BAR0_CN88XX_SIZE 0x2000000ull
#define BDK_SLI_BAR_E_SLIX_PF_BAR0_CN83XX(a) (0x874000000000ll + 0x1000000000ll * (a))
#define BDK_SLI_BAR_E_SLIX_PF_BAR0_CN83XX_SIZE 0x800000000ull
#define BDK_SLI_BAR_E_SLIX_PF_BAR4_CN81XX(a) (0x874010000000ll + 0x1000000000ll * (a))
#define BDK_SLI_BAR_E_SLIX_PF_BAR4_CN81XX_SIZE 0x100000ull
#define BDK_SLI_BAR_E_SLIX_PF_BAR4_CN88XX(a) (0x874010000000ll + 0x1000000000ll * (a))
#define BDK_SLI_BAR_E_SLIX_PF_BAR4_CN88XX_SIZE 0x100000ull
#define BDK_SLI_BAR_E_SLIX_PF_BAR4_CN83XX(a) (0x874c00000000ll + 0x1000000000ll * (a))
#define BDK_SLI_BAR_E_SLIX_PF_BAR4_CN83XX_SIZE 0x100000ull

/**
 * Enumeration sli_endianswap_e
 *
 * SLI/SDP Endian Swap Mode Enumeration
 * Enumerates the endian swap modes that SLI and SDP support.
 */
#define BDK_SLI_ENDIANSWAP_E_BYTE_SWAP_32B (2)
#define BDK_SLI_ENDIANSWAP_E_BYTE_SWAP_64B (1)
#define BDK_SLI_ENDIANSWAP_E_LW_SWAP_64B (3)
#define BDK_SLI_ENDIANSWAP_E_PASS_THRU (0)

/**
 * Enumeration sli_int_vec_e
 *
 * SLI MSI-X Vector Enumeration
 * Enumerates the MSI-X interrupt vectors.
 */
#define BDK_SLI_INT_VEC_E_MACX(a) (1 + (a))
#define BDK_SLI_INT_VEC_E_MBE (0)
#define BDK_SLI_INT_VEC_E_SDP_ECCX_LINT(a) (0xe + (a))
#define BDK_SLI_INT_VEC_E_SDP_EPFX_FLR_VF_LINT(a) (0 + (a))
#define BDK_SLI_INT_VEC_E_SDP_EPFX_IRERR_LINT(a) (0xa + (a))
#define BDK_SLI_INT_VEC_E_SDP_EPFX_ORERR_LINT(a) (0xc + (a))
#define BDK_SLI_INT_VEC_E_SLI_EPFX_DMA_VF_LINT(a) (8 + (a))
#define BDK_SLI_INT_VEC_E_SLI_EPFX_MISC_LINT(a) (2 + (a))
#define BDK_SLI_INT_VEC_E_SLI_EPFX_PP_VF_LINT(a) (6 + (a))
#define BDK_SLI_INT_VEC_E_SLI_MBE (0x10)

/**
 * Enumeration sli_rams_e
 *
 * SLI RAM Field Enumeration
 * Enumerates the relative bit positions within SLI()_MEM_CTL[CDIS].
 */
#define BDK_SLI_RAMS_E_CPL0_FIF (3)
#define BDK_SLI_RAMS_E_CPL1_FIF (2)
#define BDK_SLI_RAMS_E_CPL2_FIF (1)
#define BDK_SLI_RAMS_E_CPL3_FIF (0)
#define BDK_SLI_RAMS_E_DSI_FIF (0x1e)
#define BDK_SLI_RAMS_E_NOD_FIF (0x1d)
#define BDK_SLI_RAMS_E_P2NP0C_FIF (0xf)
#define BDK_SLI_RAMS_E_P2NP0N_FIF (0xe)
#define BDK_SLI_RAMS_E_P2NP0P_FIF (0xd)
#define BDK_SLI_RAMS_E_P2NP1C_FIF (0xc)
#define BDK_SLI_RAMS_E_P2NP1N_FIF (0xb)
#define BDK_SLI_RAMS_E_P2NP1P_FIF (0xa)
#define BDK_SLI_RAMS_E_P2NP2C_FIF (9)
#define BDK_SLI_RAMS_E_P2NP2N_FIF (8)
#define BDK_SLI_RAMS_E_P2NP2P_FIF (7)
#define BDK_SLI_RAMS_E_P2NP3C_FIF (6)
#define BDK_SLI_RAMS_E_P2NP3N_FIF (5)
#define BDK_SLI_RAMS_E_P2NP3P_FIF (4)
#define BDK_SLI_RAMS_E_REG_FIF (0x1c)
#define BDK_SLI_RAMS_E_SNCF0_FIF (0x1b)
#define BDK_SLI_RAMS_E_SNCF1_FIF (0x18)
#define BDK_SLI_RAMS_E_SNCF2_FIF (0x15)
#define BDK_SLI_RAMS_E_SNCF3_FIF (0x12)
#define BDK_SLI_RAMS_E_SNDFH0_FIF (0x1a)
#define BDK_SLI_RAMS_E_SNDFH1_FIF (0x17)
#define BDK_SLI_RAMS_E_SNDFH2_FIF (0x14)
#define BDK_SLI_RAMS_E_SNDFH3_FIF (0x11)
#define BDK_SLI_RAMS_E_SNDFL0_FIF (0x19)
#define BDK_SLI_RAMS_E_SNDFL1_FIF (0x16)
#define BDK_SLI_RAMS_E_SNDFL2_FIF (0x13)
#define BDK_SLI_RAMS_E_SNDFL3_FIF (0x10)

/**
 * Structure sli_s2m_op_s
 *
 * SLI to MAC Operation Structure
 * Core initiated load and store operations that are initiating MAC transactions form an address
 * with this structure. 8-bit, 16-bit, 32-bit and 64-bit reads and writes, in addition to atomics
 * are supported to this region.
 */
union bdk_sli_s2m_op_s
{
    uint64_t u;
    struct bdk_sli_s2m_op_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_48_63        : 16;
        uint64_t io                    : 1;  /**< [ 47: 47] Indicates IO space. */
        uint64_t reserved_46           : 1;
        uint64_t node                  : 2;  /**< [ 45: 44] CCPI node number. */
        uint64_t did_hi                : 4;  /**< [ 43: 40] SLI device ID high bits.  Specifies which SLI:
                                                                   0x8 = SLI0.
                                                                   0x9 = SLI1.

                                                                   else = Reserved. */
        uint64_t region                : 8;  /**< [ 39: 32] SLI region.  Indexes into SLI()_S2M_REG()_ACC. */
        uint64_t addr                  : 32; /**< [ 31:  0] Register address within the device. */
#else /* Word 0 - Little Endian */
        uint64_t addr                  : 32; /**< [ 31:  0] Register address within the device. */
        uint64_t region                : 8;  /**< [ 39: 32] SLI region.  Indexes into SLI()_S2M_REG()_ACC. */
        uint64_t did_hi                : 4;  /**< [ 43: 40] SLI device ID high bits.  Specifies which SLI:
                                                                   0x8 = SLI0.
                                                                   0x9 = SLI1.

                                                                   else = Reserved. */
        uint64_t node                  : 2;  /**< [ 45: 44] CCPI node number. */
        uint64_t reserved_46           : 1;
        uint64_t io                    : 1;  /**< [ 47: 47] Indicates IO space. */
        uint64_t reserved_48_63        : 16;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sli_s2m_op_s_s cn81xx; */
    /* struct bdk_sli_s2m_op_s_s cn88xx; */
    struct bdk_sli_s2m_op_s_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_48_63        : 16;
        uint64_t io                    : 1;  /**< [ 47: 47] Indicates IO space. */
        uint64_t reserved_46           : 1;
        uint64_t node                  : 2;  /**< [ 45: 44] CCPI node number. Must be zero for CN83XX. */
        uint64_t did_hi                : 4;  /**< [ 43: 40] SLI device ID high bits.  Must be 0x8 for CN83XX. */
        uint64_t region                : 8;  /**< [ 39: 32] SLI region.  Indexes into SLI()_S2M_REG()_ACC. */
        uint64_t addr                  : 32; /**< [ 31:  0] Register address within the device. */
#else /* Word 0 - Little Endian */
        uint64_t addr                  : 32; /**< [ 31:  0] Register address within the device. */
        uint64_t region                : 8;  /**< [ 39: 32] SLI region.  Indexes into SLI()_S2M_REG()_ACC. */
        uint64_t did_hi                : 4;  /**< [ 43: 40] SLI device ID high bits.  Must be 0x8 for CN83XX. */
        uint64_t node                  : 2;  /**< [ 45: 44] CCPI node number. Must be zero for CN83XX. */
        uint64_t reserved_46           : 1;
        uint64_t io                    : 1;  /**< [ 47: 47] Indicates IO space. */
        uint64_t reserved_48_63        : 16;
#endif /* Word 0 - End */
    } cn83xx;
};

/**
 * Structure sli_sdp_addr_s
 *
 * INTERNAL: SLI/SDP Address Structure
 *
 * Address decoding for SLI/SDP CSR address space
 */
union bdk_sli_sdp_addr_s
{
    uint64_t u;
    struct bdk_sli_sdp_addr_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_48_63        : 16;
        uint64_t bit47_46              : 2;  /**< [ 47: 46] NA */
        uint64_t nn                    : 2;  /**< [ 45: 44] NA */
        uint64_t did                   : 8;  /**< [ 43: 36] SLI DID */
        uint64_t region                : 2;  /**< [ 35: 34] NA */
        uint64_t r33_32                : 2;  /**< [ 33: 32] NA */
        uint64_t ncbonly               : 1;  /**< [ 31: 31] Set to 1 for registers that can only be accessed by AP cores */
        uint64_t r30_26                : 5;  /**< [ 30: 26]  */
        uint64_t epf                   : 3;  /**< [ 25: 23] EPF targeted by AP cores */
        uint64_t ring                  : 6;  /**< [ 22: 17] SDP Packet Ring */
        uint64_t space                 : 2;  /**< [ 16: 15] SDP and SLI decode space:
                                                                   0x2 = SDP ring space.
                                                                   0x0 = SDP common space.
                                                                   0x1 = SLI common space. */
        uint64_t offset                : 11; /**< [ 14:  4] Register offset */
        uint64_t bit3_0                : 4;  /**< [  3:  0] NA */
#else /* Word 0 - Little Endian */
        uint64_t bit3_0                : 4;  /**< [  3:  0] NA */
        uint64_t offset                : 11; /**< [ 14:  4] Register offset */
        uint64_t space                 : 2;  /**< [ 16: 15] SDP and SLI decode space:
                                                                   0x2 = SDP ring space.
                                                                   0x0 = SDP common space.
                                                                   0x1 = SLI common space. */
        uint64_t ring                  : 6;  /**< [ 22: 17] SDP Packet Ring */
        uint64_t epf                   : 3;  /**< [ 25: 23] EPF targeted by AP cores */
        uint64_t r30_26                : 5;  /**< [ 30: 26]  */
        uint64_t ncbonly               : 1;  /**< [ 31: 31] Set to 1 for registers that can only be accessed by AP cores */
        uint64_t r33_32                : 2;  /**< [ 33: 32] NA */
        uint64_t region                : 2;  /**< [ 35: 34] NA */
        uint64_t did                   : 8;  /**< [ 43: 36] SLI DID */
        uint64_t nn                    : 2;  /**< [ 45: 44] NA */
        uint64_t bit47_46              : 2;  /**< [ 47: 46] NA */
        uint64_t reserved_48_63        : 16;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sli_sdp_addr_s_s cn; */
};

/**
 * Register (NCB) sdp#_bist#_status
 *
 * SDP BIST Status Register
 * This register contains results from BIST runs of MAC's memories: 0 = pass (or BIST in
 * progress/never run), 1 = fail.
 */
union bdk_sdpx_bistx_status
{
    uint64_t u;
    struct bdk_sdpx_bistx_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t bstatus               : 32; /**< [ 31:  0](RO/H) BIST status. One bit per memory.
                                                                 SDP()_BIST(0)_STATUS enumerated by SDP_OUT_RAMS_E and SDP()_BIST(1)_STATUS
                                                                 enumerated by SDP_IN_RAMS_E. */
#else /* Word 0 - Little Endian */
        uint64_t bstatus               : 32; /**< [ 31:  0](RO/H) BIST status. One bit per memory.
                                                                 SDP()_BIST(0)_STATUS enumerated by SDP_OUT_RAMS_E and SDP()_BIST(1)_STATUS
                                                                 enumerated by SDP_IN_RAMS_E. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_bistx_status_s cn; */
};
typedef union bdk_sdpx_bistx_status bdk_sdpx_bistx_status_t;

static inline uint64_t BDK_SDPX_BISTX_STATUS(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_BISTX_STATUS(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880120ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_BISTX_STATUS", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_BISTX_STATUS(a,b) bdk_sdpx_bistx_status_t
#define bustype_BDK_SDPX_BISTX_STATUS(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_BISTX_STATUS(a,b) "SDPX_BISTX_STATUS"
#define device_bar_BDK_SDPX_BISTX_STATUS(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_BISTX_STATUS(a,b) (a)
#define arguments_BDK_SDPX_BISTX_STATUS(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_ecc#_ctl
 *
 * SDP ECC Control Register
 * This register controls the ECC of the SDP memories.
 */
union bdk_sdpx_eccx_ctl
{
    uint64_t u;
    struct bdk_sdpx_eccx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t cdis                  : 32; /**< [ 31:  0](R/W) Disables ECC correction on each RAM.
                                                                 SDP()_ECC(0)_CTL enumerated by SDP_OUT_RAMS_E and SDP()_ECC(1)_CTL
                                                                 enumerated by SDP_IN_RAMS_E. */
#else /* Word 0 - Little Endian */
        uint64_t cdis                  : 32; /**< [ 31:  0](R/W) Disables ECC correction on each RAM.
                                                                 SDP()_ECC(0)_CTL enumerated by SDP_OUT_RAMS_E and SDP()_ECC(1)_CTL
                                                                 enumerated by SDP_IN_RAMS_E. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_eccx_ctl_s cn; */
};
typedef union bdk_sdpx_eccx_ctl bdk_sdpx_eccx_ctl_t;

static inline uint64_t BDK_SDPX_ECCX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_ECCX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x8740008800a0ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_ECCX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_ECCX_CTL(a,b) bdk_sdpx_eccx_ctl_t
#define bustype_BDK_SDPX_ECCX_CTL(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_ECCX_CTL(a,b) "SDPX_ECCX_CTL"
#define device_bar_BDK_SDPX_ECCX_CTL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_ECCX_CTL(a,b) (a)
#define arguments_BDK_SDPX_ECCX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_ecc#_flip
 *
 * SDP ECC Control Register
 * This register controls the ECC of the SDP memories.
 */
union bdk_sdpx_eccx_flip
{
    uint64_t u;
    struct bdk_sdpx_eccx_flip_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t flip1                 : 32; /**< [ 63: 32](R/W) Flips syndrome bit 1 on writes.
                                                                 SDP()_ECC(0)_FLIP enumerated by SDP_OUT_RAMS_E and SDP()_ECC(1)_FLIP
                                                                 enumerated by SDP_IN_RAMS_E. */
        uint64_t flip0                 : 32; /**< [ 31:  0](R/W) Flips syndrome bit 0 on writes.
                                                                 SDP()_ECC(0)_FLIP enumerated by SDP_OUT_RAMS_E and SDP()_ECC(1)_FLIP
                                                                 enumerated by SDP_IN_RAMS_E. */
#else /* Word 0 - Little Endian */
        uint64_t flip0                 : 32; /**< [ 31:  0](R/W) Flips syndrome bit 0 on writes.
                                                                 SDP()_ECC(0)_FLIP enumerated by SDP_OUT_RAMS_E and SDP()_ECC(1)_FLIP
                                                                 enumerated by SDP_IN_RAMS_E. */
        uint64_t flip1                 : 32; /**< [ 63: 32](R/W) Flips syndrome bit 1 on writes.
                                                                 SDP()_ECC(0)_FLIP enumerated by SDP_OUT_RAMS_E and SDP()_ECC(1)_FLIP
                                                                 enumerated by SDP_IN_RAMS_E. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_eccx_flip_s cn; */
};
typedef union bdk_sdpx_eccx_flip bdk_sdpx_eccx_flip_t;

static inline uint64_t BDK_SDPX_ECCX_FLIP(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_ECCX_FLIP(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880100ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_ECCX_FLIP", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_ECCX_FLIP(a,b) bdk_sdpx_eccx_flip_t
#define bustype_BDK_SDPX_ECCX_FLIP(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_ECCX_FLIP(a,b) "SDPX_ECCX_FLIP"
#define device_bar_BDK_SDPX_ECCX_FLIP(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_ECCX_FLIP(a,b) (a)
#define arguments_BDK_SDPX_ECCX_FLIP(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_ecc#_lint
 *
 * SDP ECC Interrupt Status Register
 * This register contains the ECC interrupt-summary bits of the SDP.
 */
union bdk_sdpx_eccx_lint
{
    uint64_t u;
    struct bdk_sdpx_eccx_lint_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1C/H) Double-bit error detected in internal RAM. One bit per memory.
                                                                 SDP()_ECC(0)_LINT enumerated by SDP_OUT_RAMS_E and SDP()_ECC(1)_LINT
                                                                 enumerated by SDP_IN_RAMS_E. */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1C/H) Single-bit error detected in internal RAM. One bit per memory.
                                                                 SDP()_ECC(0)_LINT enumerated by SDP_OUT_RAMS_E and SDP()_ECC(1)_LINT
                                                                 enumerated by SDP_IN_RAMS_E. */
#else /* Word 0 - Little Endian */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1C/H) Single-bit error detected in internal RAM. One bit per memory.
                                                                 SDP()_ECC(0)_LINT enumerated by SDP_OUT_RAMS_E and SDP()_ECC(1)_LINT
                                                                 enumerated by SDP_IN_RAMS_E. */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1C/H) Double-bit error detected in internal RAM. One bit per memory.
                                                                 SDP()_ECC(0)_LINT enumerated by SDP_OUT_RAMS_E and SDP()_ECC(1)_LINT
                                                                 enumerated by SDP_IN_RAMS_E. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_eccx_lint_s cn; */
};
typedef union bdk_sdpx_eccx_lint bdk_sdpx_eccx_lint_t;

static inline uint64_t BDK_SDPX_ECCX_LINT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_ECCX_LINT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880020ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_ECCX_LINT", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_ECCX_LINT(a,b) bdk_sdpx_eccx_lint_t
#define bustype_BDK_SDPX_ECCX_LINT(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_ECCX_LINT(a,b) "SDPX_ECCX_LINT"
#define device_bar_BDK_SDPX_ECCX_LINT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_ECCX_LINT(a,b) (a)
#define arguments_BDK_SDPX_ECCX_LINT(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_ecc#_lint_ena_w1c
 *
 * SDP ECC Interrupt Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_sdpx_eccx_lint_ena_w1c
{
    uint64_t u;
    struct bdk_sdpx_eccx_lint_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1C/H) Reads or clears enable for SDP(0)_ECC(0..1)_LINT[DBE]. */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1C/H) Reads or clears enable for SDP(0)_ECC(0..1)_LINT[SBE]. */
#else /* Word 0 - Little Endian */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1C/H) Reads or clears enable for SDP(0)_ECC(0..1)_LINT[SBE]. */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1C/H) Reads or clears enable for SDP(0)_ECC(0..1)_LINT[DBE]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_eccx_lint_ena_w1c_s cn; */
};
typedef union bdk_sdpx_eccx_lint_ena_w1c bdk_sdpx_eccx_lint_ena_w1c_t;

static inline uint64_t BDK_SDPX_ECCX_LINT_ENA_W1C(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_ECCX_LINT_ENA_W1C(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880060ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_ECCX_LINT_ENA_W1C", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_ECCX_LINT_ENA_W1C(a,b) bdk_sdpx_eccx_lint_ena_w1c_t
#define bustype_BDK_SDPX_ECCX_LINT_ENA_W1C(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_ECCX_LINT_ENA_W1C(a,b) "SDPX_ECCX_LINT_ENA_W1C"
#define device_bar_BDK_SDPX_ECCX_LINT_ENA_W1C(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_ECCX_LINT_ENA_W1C(a,b) (a)
#define arguments_BDK_SDPX_ECCX_LINT_ENA_W1C(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_ecc#_lint_ena_w1s
 *
 * SDP ECC Interrupt Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_sdpx_eccx_lint_ena_w1s
{
    uint64_t u;
    struct bdk_sdpx_eccx_lint_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1S/H) Reads or sets enable for SDP(0)_ECC(0..1)_LINT[DBE]. */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1S/H) Reads or sets enable for SDP(0)_ECC(0..1)_LINT[SBE]. */
#else /* Word 0 - Little Endian */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1S/H) Reads or sets enable for SDP(0)_ECC(0..1)_LINT[SBE]. */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1S/H) Reads or sets enable for SDP(0)_ECC(0..1)_LINT[DBE]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_eccx_lint_ena_w1s_s cn; */
};
typedef union bdk_sdpx_eccx_lint_ena_w1s bdk_sdpx_eccx_lint_ena_w1s_t;

static inline uint64_t BDK_SDPX_ECCX_LINT_ENA_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_ECCX_LINT_ENA_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880080ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_ECCX_LINT_ENA_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_ECCX_LINT_ENA_W1S(a,b) bdk_sdpx_eccx_lint_ena_w1s_t
#define bustype_BDK_SDPX_ECCX_LINT_ENA_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_ECCX_LINT_ENA_W1S(a,b) "SDPX_ECCX_LINT_ENA_W1S"
#define device_bar_BDK_SDPX_ECCX_LINT_ENA_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_ECCX_LINT_ENA_W1S(a,b) (a)
#define arguments_BDK_SDPX_ECCX_LINT_ENA_W1S(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_ecc#_lint_w1s
 *
 * SDP ECC Interrupt Set Register
 * This register sets interrupt bits.
 */
union bdk_sdpx_eccx_lint_w1s
{
    uint64_t u;
    struct bdk_sdpx_eccx_lint_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1S/H) Reads or sets SDP(0)_ECC(0..1)_LINT[DBE]. */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1S/H) Reads or sets SDP(0)_ECC(0..1)_LINT[SBE]. */
#else /* Word 0 - Little Endian */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1S/H) Reads or sets SDP(0)_ECC(0..1)_LINT[SBE]. */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1S/H) Reads or sets SDP(0)_ECC(0..1)_LINT[DBE]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_eccx_lint_w1s_s cn; */
};
typedef union bdk_sdpx_eccx_lint_w1s bdk_sdpx_eccx_lint_w1s_t;

static inline uint64_t BDK_SDPX_ECCX_LINT_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_ECCX_LINT_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880040ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_ECCX_LINT_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_ECCX_LINT_W1S(a,b) bdk_sdpx_eccx_lint_w1s_t
#define bustype_BDK_SDPX_ECCX_LINT_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_ECCX_LINT_W1S(a,b) "SDPX_ECCX_LINT_W1S"
#define device_bar_BDK_SDPX_ECCX_LINT_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_ECCX_LINT_W1S(a,b) (a)
#define arguments_BDK_SDPX_ECCX_LINT_W1S(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_epf#_flr_vf_lint
 *
 * SDP Function Level Reset VF Bit Array Registers
 * These registers are only valid for PEM0 PF0 and PEM2 PF0.
 */
union bdk_sdpx_epfx_flr_vf_lint
{
    uint64_t u;
    struct bdk_sdpx_epfx_flr_vf_lint_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) When a VF causes an FLR the appropriate VF indexed bit is set. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) When a VF causes an FLR the appropriate VF indexed bit is set. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_flr_vf_lint_s cn; */
};
typedef union bdk_sdpx_epfx_flr_vf_lint bdk_sdpx_epfx_flr_vf_lint_t;

static inline uint64_t BDK_SDPX_EPFX_FLR_VF_LINT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_FLR_VF_LINT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880c00ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_FLR_VF_LINT", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_FLR_VF_LINT(a,b) bdk_sdpx_epfx_flr_vf_lint_t
#define bustype_BDK_SDPX_EPFX_FLR_VF_LINT(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_EPFX_FLR_VF_LINT(a,b) "SDPX_EPFX_FLR_VF_LINT"
#define device_bar_BDK_SDPX_EPFX_FLR_VF_LINT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_FLR_VF_LINT(a,b) (a)
#define arguments_BDK_SDPX_EPFX_FLR_VF_LINT(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_epf#_flr_vf_lint_ena_w1c
 *
 * SDP Function Level Reset VF Bit Array Local Enable Clear Registers
 * This register clears interrupt enable bits.
 */
union bdk_sdpx_epfx_flr_vf_lint_ena_w1c
{
    uint64_t u;
    struct bdk_sdpx_epfx_flr_vf_lint_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SDP(0)_EPF(0..1)_FLR_VF_LINT[VF_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SDP(0)_EPF(0..1)_FLR_VF_LINT[VF_INT]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_flr_vf_lint_ena_w1c_s cn; */
};
typedef union bdk_sdpx_epfx_flr_vf_lint_ena_w1c bdk_sdpx_epfx_flr_vf_lint_ena_w1c_t;

static inline uint64_t BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1C(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1C(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880e00ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_FLR_VF_LINT_ENA_W1C", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1C(a,b) bdk_sdpx_epfx_flr_vf_lint_ena_w1c_t
#define bustype_BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1C(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1C(a,b) "SDPX_EPFX_FLR_VF_LINT_ENA_W1C"
#define device_bar_BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1C(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1C(a,b) (a)
#define arguments_BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1C(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_epf#_flr_vf_lint_ena_w1s
 *
 * SDP Function Level Reset VF Bit Array Local Enable Set Registers
 * This register sets interrupt enable bits.
 */
union bdk_sdpx_epfx_flr_vf_lint_ena_w1s
{
    uint64_t u;
    struct bdk_sdpx_epfx_flr_vf_lint_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SDP(0)_EPF(0..1)_FLR_VF_LINT[VF_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SDP(0)_EPF(0..1)_FLR_VF_LINT[VF_INT]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_flr_vf_lint_ena_w1s_s cn; */
};
typedef union bdk_sdpx_epfx_flr_vf_lint_ena_w1s bdk_sdpx_epfx_flr_vf_lint_ena_w1s_t;

static inline uint64_t BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880f00ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_FLR_VF_LINT_ENA_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1S(a,b) bdk_sdpx_epfx_flr_vf_lint_ena_w1s_t
#define bustype_BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1S(a,b) "SDPX_EPFX_FLR_VF_LINT_ENA_W1S"
#define device_bar_BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1S(a,b) (a)
#define arguments_BDK_SDPX_EPFX_FLR_VF_LINT_ENA_W1S(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_epf#_flr_vf_lint_w1s
 *
 * SDP Function Level Reset VF Bit Array Set Registers
 * This register sets interrupt bits.
 */
union bdk_sdpx_epfx_flr_vf_lint_w1s
{
    uint64_t u;
    struct bdk_sdpx_epfx_flr_vf_lint_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SDP(0)_EPF(0..1)_FLR_VF_LINT[VF_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SDP(0)_EPF(0..1)_FLR_VF_LINT[VF_INT]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_flr_vf_lint_w1s_s cn; */
};
typedef union bdk_sdpx_epfx_flr_vf_lint_w1s bdk_sdpx_epfx_flr_vf_lint_w1s_t;

static inline uint64_t BDK_SDPX_EPFX_FLR_VF_LINT_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_FLR_VF_LINT_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880d00ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_FLR_VF_LINT_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_FLR_VF_LINT_W1S(a,b) bdk_sdpx_epfx_flr_vf_lint_w1s_t
#define bustype_BDK_SDPX_EPFX_FLR_VF_LINT_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_EPFX_FLR_VF_LINT_W1S(a,b) "SDPX_EPFX_FLR_VF_LINT_W1S"
#define device_bar_BDK_SDPX_EPFX_FLR_VF_LINT_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_FLR_VF_LINT_W1S(a,b) (a)
#define arguments_BDK_SDPX_EPFX_FLR_VF_LINT_W1S(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_epf#_irerr_lint
 *
 * SDP Input Error Status Register
 * This register indicates if an error has been detected on an input ring.
 * The given register associated with an EPF will be reset due to a PF FLR or MAC Reset.
 * These registers are not affected by VF FLR.
 */
union bdk_sdpx_epfx_irerr_lint
{
    uint64_t u;
    struct bdk_sdpx_epfx_irerr_lint_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Error has been detected on input ring i. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Error has been detected on input ring i. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_irerr_lint_s cn; */
};
typedef union bdk_sdpx_epfx_irerr_lint bdk_sdpx_epfx_irerr_lint_t;

static inline uint64_t BDK_SDPX_EPFX_IRERR_LINT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_IRERR_LINT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880400ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_IRERR_LINT", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_IRERR_LINT(a,b) bdk_sdpx_epfx_irerr_lint_t
#define bustype_BDK_SDPX_EPFX_IRERR_LINT(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_EPFX_IRERR_LINT(a,b) "SDPX_EPFX_IRERR_LINT"
#define device_bar_BDK_SDPX_EPFX_IRERR_LINT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_IRERR_LINT(a,b) (a)
#define arguments_BDK_SDPX_EPFX_IRERR_LINT(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_epf#_irerr_lint_ena_w1c
 *
 * SDP Input Error Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_sdpx_epfx_irerr_lint_ena_w1c
{
    uint64_t u;
    struct bdk_sdpx_epfx_irerr_lint_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SDP(0)_EPF(0..1)_IRERR_LINT[RING_ERR]. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SDP(0)_EPF(0..1)_IRERR_LINT[RING_ERR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_irerr_lint_ena_w1c_s cn; */
};
typedef union bdk_sdpx_epfx_irerr_lint_ena_w1c bdk_sdpx_epfx_irerr_lint_ena_w1c_t;

static inline uint64_t BDK_SDPX_EPFX_IRERR_LINT_ENA_W1C(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_IRERR_LINT_ENA_W1C(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880600ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_IRERR_LINT_ENA_W1C", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_IRERR_LINT_ENA_W1C(a,b) bdk_sdpx_epfx_irerr_lint_ena_w1c_t
#define bustype_BDK_SDPX_EPFX_IRERR_LINT_ENA_W1C(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_EPFX_IRERR_LINT_ENA_W1C(a,b) "SDPX_EPFX_IRERR_LINT_ENA_W1C"
#define device_bar_BDK_SDPX_EPFX_IRERR_LINT_ENA_W1C(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_IRERR_LINT_ENA_W1C(a,b) (a)
#define arguments_BDK_SDPX_EPFX_IRERR_LINT_ENA_W1C(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_epf#_irerr_lint_ena_w1s
 *
 * SDP Input Error Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_sdpx_epfx_irerr_lint_ena_w1s
{
    uint64_t u;
    struct bdk_sdpx_epfx_irerr_lint_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SDP(0)_EPF(0..1)_IRERR_LINT[RING_ERR]. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SDP(0)_EPF(0..1)_IRERR_LINT[RING_ERR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_irerr_lint_ena_w1s_s cn; */
};
typedef union bdk_sdpx_epfx_irerr_lint_ena_w1s bdk_sdpx_epfx_irerr_lint_ena_w1s_t;

static inline uint64_t BDK_SDPX_EPFX_IRERR_LINT_ENA_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_IRERR_LINT_ENA_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880700ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_IRERR_LINT_ENA_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_IRERR_LINT_ENA_W1S(a,b) bdk_sdpx_epfx_irerr_lint_ena_w1s_t
#define bustype_BDK_SDPX_EPFX_IRERR_LINT_ENA_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_EPFX_IRERR_LINT_ENA_W1S(a,b) "SDPX_EPFX_IRERR_LINT_ENA_W1S"
#define device_bar_BDK_SDPX_EPFX_IRERR_LINT_ENA_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_IRERR_LINT_ENA_W1S(a,b) (a)
#define arguments_BDK_SDPX_EPFX_IRERR_LINT_ENA_W1S(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_epf#_irerr_lint_w1s
 *
 * SDP Input Error Status Set Register
 * This register sets interrupt bits.
 */
union bdk_sdpx_epfx_irerr_lint_w1s
{
    uint64_t u;
    struct bdk_sdpx_epfx_irerr_lint_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SDP(0)_EPF(0..1)_IRERR_LINT[RING_ERR]. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SDP(0)_EPF(0..1)_IRERR_LINT[RING_ERR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_irerr_lint_w1s_s cn; */
};
typedef union bdk_sdpx_epfx_irerr_lint_w1s bdk_sdpx_epfx_irerr_lint_w1s_t;

static inline uint64_t BDK_SDPX_EPFX_IRERR_LINT_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_IRERR_LINT_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880500ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_IRERR_LINT_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_IRERR_LINT_W1S(a,b) bdk_sdpx_epfx_irerr_lint_w1s_t
#define bustype_BDK_SDPX_EPFX_IRERR_LINT_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_EPFX_IRERR_LINT_W1S(a,b) "SDPX_EPFX_IRERR_LINT_W1S"
#define device_bar_BDK_SDPX_EPFX_IRERR_LINT_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_IRERR_LINT_W1S(a,b) (a)
#define arguments_BDK_SDPX_EPFX_IRERR_LINT_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sdp#_epf#_irerr_rint
 *
 * SDP Input Error Status Register
 * This register indicates if an error has been detected on an input ring.
 * The given register associated with an EPF will be reset due to a PF FLR or MAC Reset.
 * These registers are not affected by VF FLR.
 */
union bdk_sdpx_epfx_irerr_rint
{
    uint64_t u;
    struct bdk_sdpx_epfx_irerr_rint_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Error has been detected on input ring i. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Error has been detected on input ring i. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_irerr_rint_s cn; */
};
typedef union bdk_sdpx_epfx_irerr_rint bdk_sdpx_epfx_irerr_rint_t;

static inline uint64_t BDK_SDPX_EPFX_IRERR_RINT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_IRERR_RINT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874080020080ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_IRERR_RINT", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_IRERR_RINT(a,b) bdk_sdpx_epfx_irerr_rint_t
#define bustype_BDK_SDPX_EPFX_IRERR_RINT(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_IRERR_RINT(a,b) "SDPX_EPFX_IRERR_RINT"
#define device_bar_BDK_SDPX_EPFX_IRERR_RINT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_IRERR_RINT(a,b) (a)
#define arguments_BDK_SDPX_EPFX_IRERR_RINT(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sdp#_epf#_irerr_rint_ena_w1c
 *
 * SDP Input Error Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_sdpx_epfx_irerr_rint_ena_w1c
{
    uint64_t u;
    struct bdk_sdpx_epfx_irerr_rint_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SDP(0)_EPF(0..1)_IRERR_RINT[RING_ERR]. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SDP(0)_EPF(0..1)_IRERR_RINT[RING_ERR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_irerr_rint_ena_w1c_s cn; */
};
typedef union bdk_sdpx_epfx_irerr_rint_ena_w1c bdk_sdpx_epfx_irerr_rint_ena_w1c_t;

static inline uint64_t BDK_SDPX_EPFX_IRERR_RINT_ENA_W1C(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_IRERR_RINT_ENA_W1C(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x8740800200a0ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_IRERR_RINT_ENA_W1C", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_IRERR_RINT_ENA_W1C(a,b) bdk_sdpx_epfx_irerr_rint_ena_w1c_t
#define bustype_BDK_SDPX_EPFX_IRERR_RINT_ENA_W1C(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_IRERR_RINT_ENA_W1C(a,b) "SDPX_EPFX_IRERR_RINT_ENA_W1C"
#define device_bar_BDK_SDPX_EPFX_IRERR_RINT_ENA_W1C(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_IRERR_RINT_ENA_W1C(a,b) (a)
#define arguments_BDK_SDPX_EPFX_IRERR_RINT_ENA_W1C(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sdp#_epf#_irerr_rint_ena_w1s
 *
 * SDP Input Error Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_sdpx_epfx_irerr_rint_ena_w1s
{
    uint64_t u;
    struct bdk_sdpx_epfx_irerr_rint_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SDP(0)_EPF(0..1)_IRERR_RINT[RING_ERR]. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SDP(0)_EPF(0..1)_IRERR_RINT[RING_ERR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_irerr_rint_ena_w1s_s cn; */
};
typedef union bdk_sdpx_epfx_irerr_rint_ena_w1s bdk_sdpx_epfx_irerr_rint_ena_w1s_t;

static inline uint64_t BDK_SDPX_EPFX_IRERR_RINT_ENA_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_IRERR_RINT_ENA_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x8740800200b0ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_IRERR_RINT_ENA_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_IRERR_RINT_ENA_W1S(a,b) bdk_sdpx_epfx_irerr_rint_ena_w1s_t
#define bustype_BDK_SDPX_EPFX_IRERR_RINT_ENA_W1S(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_IRERR_RINT_ENA_W1S(a,b) "SDPX_EPFX_IRERR_RINT_ENA_W1S"
#define device_bar_BDK_SDPX_EPFX_IRERR_RINT_ENA_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_IRERR_RINT_ENA_W1S(a,b) (a)
#define arguments_BDK_SDPX_EPFX_IRERR_RINT_ENA_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sdp#_epf#_irerr_rint_w1s
 *
 * SDP Input Error Status Set Register
 * This register sets interrupt bits.
 */
union bdk_sdpx_epfx_irerr_rint_w1s
{
    uint64_t u;
    struct bdk_sdpx_epfx_irerr_rint_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SDP(0)_EPF(0..1)_IRERR_RINT[RING_ERR]. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SDP(0)_EPF(0..1)_IRERR_RINT[RING_ERR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_irerr_rint_w1s_s cn; */
};
typedef union bdk_sdpx_epfx_irerr_rint_w1s bdk_sdpx_epfx_irerr_rint_w1s_t;

static inline uint64_t BDK_SDPX_EPFX_IRERR_RINT_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_IRERR_RINT_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874080020090ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_IRERR_RINT_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_IRERR_RINT_W1S(a,b) bdk_sdpx_epfx_irerr_rint_w1s_t
#define bustype_BDK_SDPX_EPFX_IRERR_RINT_W1S(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_IRERR_RINT_W1S(a,b) "SDPX_EPFX_IRERR_RINT_W1S"
#define device_bar_BDK_SDPX_EPFX_IRERR_RINT_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_IRERR_RINT_W1S(a,b) (a)
#define arguments_BDK_SDPX_EPFX_IRERR_RINT_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sdp#_epf#_mbox_rint
 *
 * SDP Mailbox Interrupt Status Register
 * This register indicates which VF/ring has signaled an interrupt.
 * The given register associated with an EPF will be reset due to a PF FLR or MAC Reset.
 * These registers are not affected by VF FLR.
 */
union bdk_sdpx_epfx_mbox_rint
{
    uint64_t u;
    struct bdk_sdpx_epfx_mbox_rint_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_num              : 64; /**< [ 63:  0](R/W1C/H) Each bit indicates a ring from 0-63. */
#else /* Word 0 - Little Endian */
        uint64_t ring_num              : 64; /**< [ 63:  0](R/W1C/H) Each bit indicates a ring from 0-63. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_mbox_rint_s cn; */
};
typedef union bdk_sdpx_epfx_mbox_rint bdk_sdpx_epfx_mbox_rint_t;

static inline uint64_t BDK_SDPX_EPFX_MBOX_RINT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_MBOX_RINT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874080020000ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_MBOX_RINT", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_MBOX_RINT(a,b) bdk_sdpx_epfx_mbox_rint_t
#define bustype_BDK_SDPX_EPFX_MBOX_RINT(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_MBOX_RINT(a,b) "SDPX_EPFX_MBOX_RINT"
#define device_bar_BDK_SDPX_EPFX_MBOX_RINT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_MBOX_RINT(a,b) (a)
#define arguments_BDK_SDPX_EPFX_MBOX_RINT(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sdp#_epf#_mbox_rint_ena_w1c
 *
 * SDP Mailbox Interrupt Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_sdpx_epfx_mbox_rint_ena_w1c
{
    uint64_t u;
    struct bdk_sdpx_epfx_mbox_rint_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_num              : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SDP(0)_EPF(0..1)_MBOX_RINT[RING_NUM]. */
#else /* Word 0 - Little Endian */
        uint64_t ring_num              : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SDP(0)_EPF(0..1)_MBOX_RINT[RING_NUM]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_mbox_rint_ena_w1c_s cn; */
};
typedef union bdk_sdpx_epfx_mbox_rint_ena_w1c bdk_sdpx_epfx_mbox_rint_ena_w1c_t;

static inline uint64_t BDK_SDPX_EPFX_MBOX_RINT_ENA_W1C(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_MBOX_RINT_ENA_W1C(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874080020020ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_MBOX_RINT_ENA_W1C", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_MBOX_RINT_ENA_W1C(a,b) bdk_sdpx_epfx_mbox_rint_ena_w1c_t
#define bustype_BDK_SDPX_EPFX_MBOX_RINT_ENA_W1C(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_MBOX_RINT_ENA_W1C(a,b) "SDPX_EPFX_MBOX_RINT_ENA_W1C"
#define device_bar_BDK_SDPX_EPFX_MBOX_RINT_ENA_W1C(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_MBOX_RINT_ENA_W1C(a,b) (a)
#define arguments_BDK_SDPX_EPFX_MBOX_RINT_ENA_W1C(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sdp#_epf#_mbox_rint_ena_w1s
 *
 * SDP Mailbox Interrupt Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_sdpx_epfx_mbox_rint_ena_w1s
{
    uint64_t u;
    struct bdk_sdpx_epfx_mbox_rint_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_num              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SDP(0)_EPF(0..1)_MBOX_RINT[RING_NUM]. */
#else /* Word 0 - Little Endian */
        uint64_t ring_num              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SDP(0)_EPF(0..1)_MBOX_RINT[RING_NUM]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_mbox_rint_ena_w1s_s cn; */
};
typedef union bdk_sdpx_epfx_mbox_rint_ena_w1s bdk_sdpx_epfx_mbox_rint_ena_w1s_t;

static inline uint64_t BDK_SDPX_EPFX_MBOX_RINT_ENA_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_MBOX_RINT_ENA_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874080020030ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_MBOX_RINT_ENA_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_MBOX_RINT_ENA_W1S(a,b) bdk_sdpx_epfx_mbox_rint_ena_w1s_t
#define bustype_BDK_SDPX_EPFX_MBOX_RINT_ENA_W1S(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_MBOX_RINT_ENA_W1S(a,b) "SDPX_EPFX_MBOX_RINT_ENA_W1S"
#define device_bar_BDK_SDPX_EPFX_MBOX_RINT_ENA_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_MBOX_RINT_ENA_W1S(a,b) (a)
#define arguments_BDK_SDPX_EPFX_MBOX_RINT_ENA_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sdp#_epf#_mbox_rint_w1s
 *
 * SDP Mailbox Interrupt Set Register
 * This register sets interrupt bits.
 */
union bdk_sdpx_epfx_mbox_rint_w1s
{
    uint64_t u;
    struct bdk_sdpx_epfx_mbox_rint_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_num              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SDP(0)_EPF(0..1)_MBOX_RINT[RING_NUM]. */
#else /* Word 0 - Little Endian */
        uint64_t ring_num              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SDP(0)_EPF(0..1)_MBOX_RINT[RING_NUM]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_mbox_rint_w1s_s cn; */
};
typedef union bdk_sdpx_epfx_mbox_rint_w1s bdk_sdpx_epfx_mbox_rint_w1s_t;

static inline uint64_t BDK_SDPX_EPFX_MBOX_RINT_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_MBOX_RINT_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874080020010ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_MBOX_RINT_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_MBOX_RINT_W1S(a,b) bdk_sdpx_epfx_mbox_rint_w1s_t
#define bustype_BDK_SDPX_EPFX_MBOX_RINT_W1S(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_MBOX_RINT_W1S(a,b) "SDPX_EPFX_MBOX_RINT_W1S"
#define device_bar_BDK_SDPX_EPFX_MBOX_RINT_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_MBOX_RINT_W1S(a,b) (a)
#define arguments_BDK_SDPX_EPFX_MBOX_RINT_W1S(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_epf#_orerr_lint
 *
 * SDP Output Error Status Register
 * This register indicates if an error has been detected on an output ring.
 * The given register associated with an EPF will be reset due to a PF FLR or MAC Reset.
 * These registers are not affected by VF FLR.
 */
union bdk_sdpx_epfx_orerr_lint
{
    uint64_t u;
    struct bdk_sdpx_epfx_orerr_lint_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Error has been detected on output ring i. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Error has been detected on output ring i. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_orerr_lint_s cn; */
};
typedef union bdk_sdpx_epfx_orerr_lint bdk_sdpx_epfx_orerr_lint_t;

static inline uint64_t BDK_SDPX_EPFX_ORERR_LINT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_ORERR_LINT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880800ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_ORERR_LINT", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_ORERR_LINT(a,b) bdk_sdpx_epfx_orerr_lint_t
#define bustype_BDK_SDPX_EPFX_ORERR_LINT(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_EPFX_ORERR_LINT(a,b) "SDPX_EPFX_ORERR_LINT"
#define device_bar_BDK_SDPX_EPFX_ORERR_LINT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_ORERR_LINT(a,b) (a)
#define arguments_BDK_SDPX_EPFX_ORERR_LINT(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_epf#_orerr_lint_ena_w1c
 *
 * SDP Output Error Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_sdpx_epfx_orerr_lint_ena_w1c
{
    uint64_t u;
    struct bdk_sdpx_epfx_orerr_lint_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SDP(0)_EPF(0..1)_ORERR_LINT[RING_ERR]. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SDP(0)_EPF(0..1)_ORERR_LINT[RING_ERR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_orerr_lint_ena_w1c_s cn; */
};
typedef union bdk_sdpx_epfx_orerr_lint_ena_w1c bdk_sdpx_epfx_orerr_lint_ena_w1c_t;

static inline uint64_t BDK_SDPX_EPFX_ORERR_LINT_ENA_W1C(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_ORERR_LINT_ENA_W1C(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880a00ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_ORERR_LINT_ENA_W1C", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_ORERR_LINT_ENA_W1C(a,b) bdk_sdpx_epfx_orerr_lint_ena_w1c_t
#define bustype_BDK_SDPX_EPFX_ORERR_LINT_ENA_W1C(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_EPFX_ORERR_LINT_ENA_W1C(a,b) "SDPX_EPFX_ORERR_LINT_ENA_W1C"
#define device_bar_BDK_SDPX_EPFX_ORERR_LINT_ENA_W1C(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_ORERR_LINT_ENA_W1C(a,b) (a)
#define arguments_BDK_SDPX_EPFX_ORERR_LINT_ENA_W1C(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_epf#_orerr_lint_ena_w1s
 *
 * SDP Output Error Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_sdpx_epfx_orerr_lint_ena_w1s
{
    uint64_t u;
    struct bdk_sdpx_epfx_orerr_lint_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SDP(0)_EPF(0..1)_ORERR_LINT[RING_ERR]. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SDP(0)_EPF(0..1)_ORERR_LINT[RING_ERR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_orerr_lint_ena_w1s_s cn; */
};
typedef union bdk_sdpx_epfx_orerr_lint_ena_w1s bdk_sdpx_epfx_orerr_lint_ena_w1s_t;

static inline uint64_t BDK_SDPX_EPFX_ORERR_LINT_ENA_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_ORERR_LINT_ENA_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880b00ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_ORERR_LINT_ENA_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_ORERR_LINT_ENA_W1S(a,b) bdk_sdpx_epfx_orerr_lint_ena_w1s_t
#define bustype_BDK_SDPX_EPFX_ORERR_LINT_ENA_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_EPFX_ORERR_LINT_ENA_W1S(a,b) "SDPX_EPFX_ORERR_LINT_ENA_W1S"
#define device_bar_BDK_SDPX_EPFX_ORERR_LINT_ENA_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_ORERR_LINT_ENA_W1S(a,b) (a)
#define arguments_BDK_SDPX_EPFX_ORERR_LINT_ENA_W1S(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sdp#_epf#_orerr_lint_w1s
 *
 * SDP Output Error Status Set Register
 * This register sets interrupt bits.
 */
union bdk_sdpx_epfx_orerr_lint_w1s
{
    uint64_t u;
    struct bdk_sdpx_epfx_orerr_lint_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SDP(0)_EPF(0..1)_ORERR_LINT[RING_ERR]. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SDP(0)_EPF(0..1)_ORERR_LINT[RING_ERR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_orerr_lint_w1s_s cn; */
};
typedef union bdk_sdpx_epfx_orerr_lint_w1s bdk_sdpx_epfx_orerr_lint_w1s_t;

static inline uint64_t BDK_SDPX_EPFX_ORERR_LINT_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_ORERR_LINT_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000880900ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_ORERR_LINT_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_ORERR_LINT_W1S(a,b) bdk_sdpx_epfx_orerr_lint_w1s_t
#define bustype_BDK_SDPX_EPFX_ORERR_LINT_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SDPX_EPFX_ORERR_LINT_W1S(a,b) "SDPX_EPFX_ORERR_LINT_W1S"
#define device_bar_BDK_SDPX_EPFX_ORERR_LINT_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_ORERR_LINT_W1S(a,b) (a)
#define arguments_BDK_SDPX_EPFX_ORERR_LINT_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sdp#_epf#_orerr_rint
 *
 * SDP Output Error Status Register
 * This register indicates if an error has been detected on an output ring.
 * The given register associated with an EPF will be reset due to a PF FLR or MAC Reset.
 * These registers are not affected by VF FLR.
 */
union bdk_sdpx_epfx_orerr_rint
{
    uint64_t u;
    struct bdk_sdpx_epfx_orerr_rint_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Error has been detected on ring output ring i. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Error has been detected on ring output ring i. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_orerr_rint_s cn; */
};
typedef union bdk_sdpx_epfx_orerr_rint bdk_sdpx_epfx_orerr_rint_t;

static inline uint64_t BDK_SDPX_EPFX_ORERR_RINT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_ORERR_RINT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874080020100ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_ORERR_RINT", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_ORERR_RINT(a,b) bdk_sdpx_epfx_orerr_rint_t
#define bustype_BDK_SDPX_EPFX_ORERR_RINT(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_ORERR_RINT(a,b) "SDPX_EPFX_ORERR_RINT"
#define device_bar_BDK_SDPX_EPFX_ORERR_RINT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_ORERR_RINT(a,b) (a)
#define arguments_BDK_SDPX_EPFX_ORERR_RINT(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sdp#_epf#_orerr_rint_ena_w1c
 *
 * SDP Output Error Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_sdpx_epfx_orerr_rint_ena_w1c
{
    uint64_t u;
    struct bdk_sdpx_epfx_orerr_rint_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SDP(0)_EPF(0..1)_ORERR_RINT[RING_ERR]. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SDP(0)_EPF(0..1)_ORERR_RINT[RING_ERR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_orerr_rint_ena_w1c_s cn; */
};
typedef union bdk_sdpx_epfx_orerr_rint_ena_w1c bdk_sdpx_epfx_orerr_rint_ena_w1c_t;

static inline uint64_t BDK_SDPX_EPFX_ORERR_RINT_ENA_W1C(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_ORERR_RINT_ENA_W1C(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874080020120ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_ORERR_RINT_ENA_W1C", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_ORERR_RINT_ENA_W1C(a,b) bdk_sdpx_epfx_orerr_rint_ena_w1c_t
#define bustype_BDK_SDPX_EPFX_ORERR_RINT_ENA_W1C(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_ORERR_RINT_ENA_W1C(a,b) "SDPX_EPFX_ORERR_RINT_ENA_W1C"
#define device_bar_BDK_SDPX_EPFX_ORERR_RINT_ENA_W1C(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_ORERR_RINT_ENA_W1C(a,b) (a)
#define arguments_BDK_SDPX_EPFX_ORERR_RINT_ENA_W1C(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sdp#_epf#_orerr_rint_ena_w1s
 *
 * SDP Output Error Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_sdpx_epfx_orerr_rint_ena_w1s
{
    uint64_t u;
    struct bdk_sdpx_epfx_orerr_rint_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SDP(0)_EPF(0..1)_ORERR_RINT[RING_ERR]. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SDP(0)_EPF(0..1)_ORERR_RINT[RING_ERR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_orerr_rint_ena_w1s_s cn; */
};
typedef union bdk_sdpx_epfx_orerr_rint_ena_w1s bdk_sdpx_epfx_orerr_rint_ena_w1s_t;

static inline uint64_t BDK_SDPX_EPFX_ORERR_RINT_ENA_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_ORERR_RINT_ENA_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874080020130ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_ORERR_RINT_ENA_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_ORERR_RINT_ENA_W1S(a,b) bdk_sdpx_epfx_orerr_rint_ena_w1s_t
#define bustype_BDK_SDPX_EPFX_ORERR_RINT_ENA_W1S(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_ORERR_RINT_ENA_W1S(a,b) "SDPX_EPFX_ORERR_RINT_ENA_W1S"
#define device_bar_BDK_SDPX_EPFX_ORERR_RINT_ENA_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_ORERR_RINT_ENA_W1S(a,b) (a)
#define arguments_BDK_SDPX_EPFX_ORERR_RINT_ENA_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sdp#_epf#_orerr_rint_w1s
 *
 * SDP Output Error Status Set Register
 * This register sets interrupt bits.
 */
union bdk_sdpx_epfx_orerr_rint_w1s
{
    uint64_t u;
    struct bdk_sdpx_epfx_orerr_rint_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SDP(0)_EPF(0..1)_ORERR_RINT[RING_ERR]. */
#else /* Word 0 - Little Endian */
        uint64_t ring_err              : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SDP(0)_EPF(0..1)_ORERR_RINT[RING_ERR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_orerr_rint_w1s_s cn; */
};
typedef union bdk_sdpx_epfx_orerr_rint_w1s bdk_sdpx_epfx_orerr_rint_w1s_t;

static inline uint64_t BDK_SDPX_EPFX_ORERR_RINT_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_ORERR_RINT_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874080020110ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SDPX_EPFX_ORERR_RINT_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_EPFX_ORERR_RINT_W1S(a,b) bdk_sdpx_epfx_orerr_rint_w1s_t
#define bustype_BDK_SDPX_EPFX_ORERR_RINT_W1S(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_ORERR_RINT_W1S(a,b) "SDPX_EPFX_ORERR_RINT_W1S"
#define device_bar_BDK_SDPX_EPFX_ORERR_RINT_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_ORERR_RINT_W1S(a,b) (a)
#define arguments_BDK_SDPX_EPFX_ORERR_RINT_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_all_int_status
 *
 * SDP Combined Interrupt Summary Status Register
 * This register contains interrupt status on a per-VF basis.  All rings for a given VF
 * are located in a single register. Note that access to any ring offset within a given
 * VF will return the same value.  When the PF reads any ring in this register it will
 * return the same value (64 bits each representing one ring.)
 *
 * Internal:
 * These interrupt bits may be set for some rings even after a PF/VF FLR.
 * They are not cleared becase the CNTS and LEVELS registers are not reset
 * and we wish to make the interrupt state consistent with CNTS/LEVELS even after FLR.
 * The CNTS register must be cleared by software as part of initialization after a reset
 * (including FLR) which will cause the interrupt state in this register to clear.
 */
union bdk_sdpx_epfx_rx_all_int_status
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_all_int_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t intr                  : 64; /**< [ 63:  0](RO) These bits are interpreted differently for PF access and VF access.

                                                                 For a PF read:

                                                                 Each of the 64 bits corresponds to a ring number that is signalling an
                                                                 interrupt.  [INTR]\<ring\> reads as one whenever any of the following are true for
                                                                 the respective ring R(ring):

                                                                  * SDP()_EPF()_R()_OUT_CNTS[CNT]   \> SDP()_EPF()_R()_OUT_INT_LEVELS[CNT],
                                                                  * SDP()_EPF()_R()_OUT_CNTS[TIMER] \> SDP()_EPF()_R()_OUT_INT_LEVELS[TIMET],
                                                                  * SDP()_EPF()_R()_IN_CNTS[CNT] \> SDP()_EPF()_R()_IN_INT_LEVELS[CNT],
                                                                  * Or, SDP()_EPF()_R()_MBOX_RINT_STATUS[INTR] is set.

                                                                  Reading this register will isolate the ring(s) that is signalling the interrupt.
                                                                  To determine the specific interrupt, other registers must be read.

                                                                  For a VF read:

                                                                  In this mode, this register identifies the ring number "i" and specific
                                                                 interrupt being signaled.

                                                                  Bits \<7:0\> indicate an input interrupt being signaled, where bit i is set if
                                                                  for the respective ring R(i):
                                                                   * SDP()_EPF()_R()_IN_CNTS[CNT] \> SDP()_EPF()_R()_IN_INT_LEVELS[CNT].

                                                                  Bits \<15:8\> indicate an output interrupt being signaled, where bit i is set if
                                                                  for the respective ring R(i):
                                                                  * SDP()_EPF()_R()_OUT_CNTS[CNT]   \> SDP()_EPF()_R()_OUT_INT_LEVELS[CNT].
                                                                  * Or, SDP()_EPF()_R()_OUT_CNTS[TIMER] \> SDP()_EPF()_R()_OUT_INT_LEVELS[TIMET].

                                                                  Bits \<23:16\> indicate a mailbox interrupt being signaled, where bit i is set if
                                                                  for the respective ring R(i):
                                                                  * SDP()_EPF()_R()_MBOX_RINT_STATUS[INTR] is set.

                                                                  Bits \<63:24\> are reserved. */
#else /* Word 0 - Little Endian */
        uint64_t intr                  : 64; /**< [ 63:  0](RO) These bits are interpreted differently for PF access and VF access.

                                                                 For a PF read:

                                                                 Each of the 64 bits corresponds to a ring number that is signalling an
                                                                 interrupt.  [INTR]\<ring\> reads as one whenever any of the following are true for
                                                                 the respective ring R(ring):

                                                                  * SDP()_EPF()_R()_OUT_CNTS[CNT]   \> SDP()_EPF()_R()_OUT_INT_LEVELS[CNT],
                                                                  * SDP()_EPF()_R()_OUT_CNTS[TIMER] \> SDP()_EPF()_R()_OUT_INT_LEVELS[TIMET],
                                                                  * SDP()_EPF()_R()_IN_CNTS[CNT] \> SDP()_EPF()_R()_IN_INT_LEVELS[CNT],
                                                                  * Or, SDP()_EPF()_R()_MBOX_RINT_STATUS[INTR] is set.

                                                                  Reading this register will isolate the ring(s) that is signalling the interrupt.
                                                                  To determine the specific interrupt, other registers must be read.

                                                                  For a VF read:

                                                                  In this mode, this register identifies the ring number "i" and specific
                                                                 interrupt being signaled.

                                                                  Bits \<7:0\> indicate an input interrupt being signaled, where bit i is set if
                                                                  for the respective ring R(i):
                                                                   * SDP()_EPF()_R()_IN_CNTS[CNT] \> SDP()_EPF()_R()_IN_INT_LEVELS[CNT].

                                                                  Bits \<15:8\> indicate an output interrupt being signaled, where bit i is set if
                                                                  for the respective ring R(i):
                                                                  * SDP()_EPF()_R()_OUT_CNTS[CNT]   \> SDP()_EPF()_R()_OUT_INT_LEVELS[CNT].
                                                                  * Or, SDP()_EPF()_R()_OUT_CNTS[TIMER] \> SDP()_EPF()_R()_OUT_INT_LEVELS[TIMET].

                                                                  Bits \<23:16\> indicate a mailbox interrupt being signaled, where bit i is set if
                                                                  for the respective ring R(i):
                                                                  * SDP()_EPF()_R()_MBOX_RINT_STATUS[INTR] is set.

                                                                  Bits \<63:24\> are reserved. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_all_int_status_s cn; */
};
typedef union bdk_sdpx_epfx_rx_all_int_status bdk_sdpx_epfx_rx_all_int_status_t;

static inline uint64_t BDK_SDPX_EPFX_RX_ALL_INT_STATUS(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_ALL_INT_STATUS(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010300ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_ALL_INT_STATUS", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_ALL_INT_STATUS(a,b,c) bdk_sdpx_epfx_rx_all_int_status_t
#define bustype_BDK_SDPX_EPFX_RX_ALL_INT_STATUS(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_ALL_INT_STATUS(a,b,c) "SDPX_EPFX_RX_ALL_INT_STATUS"
#define device_bar_BDK_SDPX_EPFX_RX_ALL_INT_STATUS(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_ALL_INT_STATUS(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_ALL_INT_STATUS(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_err_type
 *
 * SDP Ring Error Type Register
 * These registers indicate which type of error(s) have been detected when
 * SDP()_EPF()_IRERR_LINT\<i\> / SDP()_EPF()_ORERR_RINT\<i\> / SDP()_EPF()_ORERR_LINT\<i\> /
 * SDP()_EPF()_ORERR_RINT\<i\> is set. Multiple bits can be set at the same time
 * if multiple errors have occurred for that ring.
 *
 * All 64 registers associated with an EPF will be reset due to a PF FLR or MAC Reset.
 * These registers are not affected by VF FLR.
 */
union bdk_sdpx_epfx_rx_err_type
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_err_type_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_35_63        : 29;
        uint64_t port_dis              : 1;  /**< [ 34: 34](R/W1C/H) Output packet arrives targeting a port which is not enabled. */
        uint64_t dbell_empty           : 1;  /**< [ 33: 33](R/W1C/H) The watermark value is set too small, allowing doorbell count to drop below 8. */
        uint64_t oring_dma_err         : 1;  /**< [ 32: 32](R/W1C/H) DMA read error response on output pointer pair fetch. */
        uint64_t reserved_8_31         : 24;
        uint64_t illegal_fsz           : 1;  /**< [  7:  7](R/W1C/H) Illegal FSZ specified in instruction.
                                                                 For direct gather, FSZ must be \<= 32 for 64B instructions and 0 for 32B instructions.
                                                                 For direct data/indirect gather, FSZ must be \<= 55 for 64B instructions and \<= 23 for 32B
                                                                 instructions. This check is done before any length checks. */
        uint64_t pkt_dma_err           : 1;  /**< [  6:  6](R/W1C/H) DMA read error response on packet fetch. */
        uint64_t inst_dma_err          : 1;  /**< [  5:  5](R/W1C/H) DMA read error response on instruction fetch. */
        uint64_t pkt_toosmall          : 1;  /**< [  4:  4](R/W1C/H) Attempted packet read with LEN=0 or LEN \< FSZ. */
        uint64_t dir_len_toosmall      : 1;  /**< [  3:  3](R/W1C/H) Direct gather combined LEN fields are less than the packet length specified. */
        uint64_t ind_dma_err           : 1;  /**< [  2:  2](R/W1C/H) DMA read error response on indirect gather list fetch.  This could also be caused by
                                                                 an unaligned gather list, in which case SDP()_DIAG[IN_IND_UNALIGNED] will also be set. */
        uint64_t ind_zero_det          : 1;  /**< [  1:  1](R/W1C/H) Indirect gather list contains length of 0. */
        uint64_t ind_toosmall          : 1;  /**< [  0:  0](R/W1C/H) Indirect gather list length specified less than (packet length - FSZ) in instruction. */
#else /* Word 0 - Little Endian */
        uint64_t ind_toosmall          : 1;  /**< [  0:  0](R/W1C/H) Indirect gather list length specified less than (packet length - FSZ) in instruction. */
        uint64_t ind_zero_det          : 1;  /**< [  1:  1](R/W1C/H) Indirect gather list contains length of 0. */
        uint64_t ind_dma_err           : 1;  /**< [  2:  2](R/W1C/H) DMA read error response on indirect gather list fetch.  This could also be caused by
                                                                 an unaligned gather list, in which case SDP()_DIAG[IN_IND_UNALIGNED] will also be set. */
        uint64_t dir_len_toosmall      : 1;  /**< [  3:  3](R/W1C/H) Direct gather combined LEN fields are less than the packet length specified. */
        uint64_t pkt_toosmall          : 1;  /**< [  4:  4](R/W1C/H) Attempted packet read with LEN=0 or LEN \< FSZ. */
        uint64_t inst_dma_err          : 1;  /**< [  5:  5](R/W1C/H) DMA read error response on instruction fetch. */
        uint64_t pkt_dma_err           : 1;  /**< [  6:  6](R/W1C/H) DMA read error response on packet fetch. */
        uint64_t illegal_fsz           : 1;  /**< [  7:  7](R/W1C/H) Illegal FSZ specified in instruction.
                                                                 For direct gather, FSZ must be \<= 32 for 64B instructions and 0 for 32B instructions.
                                                                 For direct data/indirect gather, FSZ must be \<= 55 for 64B instructions and \<= 23 for 32B
                                                                 instructions. This check is done before any length checks. */
        uint64_t reserved_8_31         : 24;
        uint64_t oring_dma_err         : 1;  /**< [ 32: 32](R/W1C/H) DMA read error response on output pointer pair fetch. */
        uint64_t dbell_empty           : 1;  /**< [ 33: 33](R/W1C/H) The watermark value is set too small, allowing doorbell count to drop below 8. */
        uint64_t port_dis              : 1;  /**< [ 34: 34](R/W1C/H) Output packet arrives targeting a port which is not enabled. */
        uint64_t reserved_35_63        : 29;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_err_type_s cn; */
};
typedef union bdk_sdpx_epfx_rx_err_type bdk_sdpx_epfx_rx_err_type_t;

static inline uint64_t BDK_SDPX_EPFX_RX_ERR_TYPE(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_ERR_TYPE(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010400ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_ERR_TYPE", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_ERR_TYPE(a,b,c) bdk_sdpx_epfx_rx_err_type_t
#define bustype_BDK_SDPX_EPFX_RX_ERR_TYPE(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_ERR_TYPE(a,b,c) "SDPX_EPFX_RX_ERR_TYPE"
#define device_bar_BDK_SDPX_EPFX_RX_ERR_TYPE(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_ERR_TYPE(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_ERR_TYPE(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_in_byte_cnt
 *
 * SDP Packet Input Byte Count Register
 * This register contains byte counts per ring that have been read into SDP.
 * The counter will wrap when it reaches its maximum value. It should be cleared
 * before the ring is enabled for an accurate count.
 */
union bdk_sdpx_epfx_rx_in_byte_cnt
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_in_byte_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_48_63        : 16;
        uint64_t cnt                   : 48; /**< [ 47:  0](R/W/H) Byte count, can be reset by software by writing SDP()_EPF()_R()_IN_PKT_CNT[CNT]
                                                                 with 0xFFFFFFFFF. */
#else /* Word 0 - Little Endian */
        uint64_t cnt                   : 48; /**< [ 47:  0](R/W/H) Byte count, can be reset by software by writing SDP()_EPF()_R()_IN_PKT_CNT[CNT]
                                                                 with 0xFFFFFFFFF. */
        uint64_t reserved_48_63        : 16;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_in_byte_cnt_s cn; */
};
typedef union bdk_sdpx_epfx_rx_in_byte_cnt bdk_sdpx_epfx_rx_in_byte_cnt_t;

static inline uint64_t BDK_SDPX_EPFX_RX_IN_BYTE_CNT(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_IN_BYTE_CNT(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010090ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_IN_BYTE_CNT", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_IN_BYTE_CNT(a,b,c) bdk_sdpx_epfx_rx_in_byte_cnt_t
#define bustype_BDK_SDPX_EPFX_RX_IN_BYTE_CNT(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_IN_BYTE_CNT(a,b,c) "SDPX_EPFX_RX_IN_BYTE_CNT"
#define device_bar_BDK_SDPX_EPFX_RX_IN_BYTE_CNT(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_IN_BYTE_CNT(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_IN_BYTE_CNT(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_in_cnts
 *
 * SDP Input Instruction Ring Counts Register
 * This register contains the counters for the input instruction rings.
 * This register is not affected by reset (including FLR) and must be initialized
 * by the VF prior to enabling the ring.
 */
union bdk_sdpx_epfx_rx_in_cnts
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_in_cnts_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_63           : 1;
        uint64_t out_int               : 1;  /**< [ 62: 62](RO/H) Returns a 1 when:
                                                                  * SDP()_R()_OUT_CNTS[CNT]   \> SDP()_EPF()_R()_OUT_INT_LEVELS[CNT].
                                                                  * Or, SDP()_R()_OUT_CNTS[TIMER] \> SDP()_EPF()_R()_OUT_INT_LEVELS[TIMET].

                                                                 To clear the bit, the CNTS register must be written to clear the underlying condition. */
        uint64_t in_int                : 1;  /**< [ 61: 61](RO/H) Returns a 1 when:
                                                                  * SDP()_EPF()_R()_IN_CNTS[CNT] \> SDP()_EPF()_R()_IN_INT_LEVELS[CNT]

                                                                 To clear the bit, the SDP()_EPF()_R()_IN_CNTS register must be written to clear the
                                                                 underlying condition. */
        uint64_t mbox_int              : 1;  /**< [ 60: 60](RO/H) Returns a 1 when:
                                                                  * SDP()_EPF()_R()_MBOX_RINT_STATUS[INTR] is set

                                                                 To clear the bit, write SDP()_EPF()_R()_MBOX_PF_VF_INT[INTR] with 1.
                                                                 This bit is also cleared due to an FLR. */
        uint64_t resend                : 1;  /**< [ 59: 59](WO/H) A write of 1 will resend an MSI-X interrupt message if any of the following
                                                                 conditions are true for the respective ring:
                                                                  * SDP()_EPF()_R()_OUT_CNTS[CNT]   \> SDP()_EPF()_R()_OUT_INT_LEVELS[CNT].
                                                                  * SDP()_EPF()_R()_OUT_CNTS[TIMER] \> SDP()_EPF()_R()_OUT_INT_LEVELS[TIMET].
                                                                  * SDP()_EPF()_R()_IN_CNTS[CNT] \> SDP()_EPF()_R()_IN_INT_LEVELS[CNT].
                                                                  * SDP()_EPF()_R()_MBOX_RINT_STATUS[INTR] is set. */
        uint64_t reserved_32_58        : 27;
        uint64_t cnt                   : 32; /**< [ 31:  0](R/W/H) Packet counter. Hardware adds to [CNT] as it reads packets. On a write
                                                                 to this CSR, hardware subtracts the amount written to the [CNT] field from
                                                                 [CNT], which will clear PKT_IN()_INT_STATUS[INTR] if [CNT] becomes \<=
                                                                 SDP()_EPF()_R()_IN_INT_LEVELS[CNT]. This register should be cleared before
                                                                 enabling a ring by reading the current value and writing it back. */
#else /* Word 0 - Little Endian */
        uint64_t cnt                   : 32; /**< [ 31:  0](R/W/H) Packet counter. Hardware adds to [CNT] as it reads packets. On a write
                                                                 to this CSR, hardware subtracts the amount written to the [CNT] field from
                                                                 [CNT], which will clear PKT_IN()_INT_STATUS[INTR] if [CNT] becomes \<=
                                                                 SDP()_EPF()_R()_IN_INT_LEVELS[CNT]. This register should be cleared before
                                                                 enabling a ring by reading the current value and writing it back. */
        uint64_t reserved_32_58        : 27;
        uint64_t resend                : 1;  /**< [ 59: 59](WO/H) A write of 1 will resend an MSI-X interrupt message if any of the following
                                                                 conditions are true for the respective ring:
                                                                  * SDP()_EPF()_R()_OUT_CNTS[CNT]   \> SDP()_EPF()_R()_OUT_INT_LEVELS[CNT].
                                                                  * SDP()_EPF()_R()_OUT_CNTS[TIMER] \> SDP()_EPF()_R()_OUT_INT_LEVELS[TIMET].
                                                                  * SDP()_EPF()_R()_IN_CNTS[CNT] \> SDP()_EPF()_R()_IN_INT_LEVELS[CNT].
                                                                  * SDP()_EPF()_R()_MBOX_RINT_STATUS[INTR] is set. */
        uint64_t mbox_int              : 1;  /**< [ 60: 60](RO/H) Returns a 1 when:
                                                                  * SDP()_EPF()_R()_MBOX_RINT_STATUS[INTR] is set

                                                                 To clear the bit, write SDP()_EPF()_R()_MBOX_PF_VF_INT[INTR] with 1.
                                                                 This bit is also cleared due to an FLR. */
        uint64_t in_int                : 1;  /**< [ 61: 61](RO/H) Returns a 1 when:
                                                                  * SDP()_EPF()_R()_IN_CNTS[CNT] \> SDP()_EPF()_R()_IN_INT_LEVELS[CNT]

                                                                 To clear the bit, the SDP()_EPF()_R()_IN_CNTS register must be written to clear the
                                                                 underlying condition. */
        uint64_t out_int               : 1;  /**< [ 62: 62](RO/H) Returns a 1 when:
                                                                  * SDP()_R()_OUT_CNTS[CNT]   \> SDP()_EPF()_R()_OUT_INT_LEVELS[CNT].
                                                                  * Or, SDP()_R()_OUT_CNTS[TIMER] \> SDP()_EPF()_R()_OUT_INT_LEVELS[TIMET].

                                                                 To clear the bit, the CNTS register must be written to clear the underlying condition. */
        uint64_t reserved_63           : 1;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_in_cnts_s cn; */
};
typedef union bdk_sdpx_epfx_rx_in_cnts bdk_sdpx_epfx_rx_in_cnts_t;

static inline uint64_t BDK_SDPX_EPFX_RX_IN_CNTS(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_IN_CNTS(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010050ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_IN_CNTS", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_IN_CNTS(a,b,c) bdk_sdpx_epfx_rx_in_cnts_t
#define bustype_BDK_SDPX_EPFX_RX_IN_CNTS(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_IN_CNTS(a,b,c) "SDPX_EPFX_RX_IN_CNTS"
#define device_bar_BDK_SDPX_EPFX_RX_IN_CNTS(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_IN_CNTS(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_IN_CNTS(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_in_control
 *
 * SDP Input Instruction Ring Control Register
 * This register is the control for read operations on the input instruction rings.
 * This register is not affected by reset (including FLR) and must be initialized
 * by the VF prior to enabling the ring.  Also, this register cannot be written
 * while either of the following conditions is true:
 *   * [IDLE] is clear.
 *   * Or, SDP()_EPF()_R()_IN_ENABLE[ENB] is set.
 */
union bdk_sdpx_epfx_rx_in_control
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_in_control_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_52_63        : 12;
        uint64_t rpvf                  : 4;  /**< [ 51: 48](RO/H) The number of rings assigned to this VF.
                                                                 Read only copy of SDP()_EPF()_RINFO[RPVF] */
        uint64_t reserved_29_47        : 19;
        uint64_t idle                  : 1;  /**< [ 28: 28](RO/H) Asserted when this ring has no packets in-flight. */
        uint64_t reserved_27           : 1;
        uint64_t rdsize                : 2;  /**< [ 26: 25](R/W) Number of instructions to be read in one read request. Two-bit values are:
                                                                 0x0 = 1 instruction.
                                                                 0x1 = 2 instructions.
                                                                 0x2 = 4 instructions.
                                                                 0x3 = 8 instructions. */
        uint64_t is64b                 : 1;  /**< [ 24: 24](R/W) If 1, the ring uses 64-byte instructions.
                                                                 If 0, the ring uses 32-byte instructions. */
        uint64_t reserved_9_23         : 15;
        uint64_t d_nsr                 : 1;  /**< [  8:  8](R/W/H) [D_NSR] is ADDRTYPE\<1\> for first direct and gather DPTR reads. ADDRTYPE\<1\> is the
                                                                 no-snoop attribute for PCIe. */
        uint64_t d_esr                 : 2;  /**< [  7:  6](R/W/H) [D_ESR] is ES\<1:0\> for first direct and gather DPTR reads.
                                                                 ES\<1:0\> is the endian-swap attribute for these MAC memory space reads.
                                                                 Enumerated by SLI_ENDIANSWAP_E. */
        uint64_t d_ror                 : 1;  /**< [  5:  5](R/W/H) [D_ROR] is ADDRTYPE\<0\> for first direct and gather DPTR reads. ADDRTYPE\<0\> is the
                                                                 relaxed-order attribute for PCIe. */
        uint64_t reserved_4            : 1;
        uint64_t nsr                   : 1;  /**< [  3:  3](R/W/H) [NSR] is ADDRTYPE\<1\> for input instruction reads (from
                                                                 SDP()_EPF()_R()_IN_INSTR_BADDR) and first indirect DPTR reads. ADDRTYPE\<1\>
                                                                 is the no-snoop attribute for PCIe. */
        uint64_t esr                   : 2;  /**< [  2:  1](R/W/H) [ESR] is ES\<1:0\> for input instruction reads (from
                                                                 SDP()_EPF()_R()_IN_INSTR_BADDR) and first indirect DPTR reads. ES\<1:0\> is
                                                                 the endian-swap attribute for these MAC memory space reads.
                                                                 Enumerated by SLI_ENDIANSWAP_E. */
        uint64_t ror                   : 1;  /**< [  0:  0](R/W/H) [ROR] is ADDRTYPE\<0\> for input instruction reads (from
                                                                 SDP()_EPF()_R()_IN_INSTR_BADDR) and first indirect DPTR reads.
                                                                 ADDRTYPE\<0\> is the relaxed-order attribute for PCIe. */
#else /* Word 0 - Little Endian */
        uint64_t ror                   : 1;  /**< [  0:  0](R/W/H) [ROR] is ADDRTYPE\<0\> for input instruction reads (from
                                                                 SDP()_EPF()_R()_IN_INSTR_BADDR) and first indirect DPTR reads.
                                                                 ADDRTYPE\<0\> is the relaxed-order attribute for PCIe. */
        uint64_t esr                   : 2;  /**< [  2:  1](R/W/H) [ESR] is ES\<1:0\> for input instruction reads (from
                                                                 SDP()_EPF()_R()_IN_INSTR_BADDR) and first indirect DPTR reads. ES\<1:0\> is
                                                                 the endian-swap attribute for these MAC memory space reads.
                                                                 Enumerated by SLI_ENDIANSWAP_E. */
        uint64_t nsr                   : 1;  /**< [  3:  3](R/W/H) [NSR] is ADDRTYPE\<1\> for input instruction reads (from
                                                                 SDP()_EPF()_R()_IN_INSTR_BADDR) and first indirect DPTR reads. ADDRTYPE\<1\>
                                                                 is the no-snoop attribute for PCIe. */
        uint64_t reserved_4            : 1;
        uint64_t d_ror                 : 1;  /**< [  5:  5](R/W/H) [D_ROR] is ADDRTYPE\<0\> for first direct and gather DPTR reads. ADDRTYPE\<0\> is the
                                                                 relaxed-order attribute for PCIe. */
        uint64_t d_esr                 : 2;  /**< [  7:  6](R/W/H) [D_ESR] is ES\<1:0\> for first direct and gather DPTR reads.
                                                                 ES\<1:0\> is the endian-swap attribute for these MAC memory space reads.
                                                                 Enumerated by SLI_ENDIANSWAP_E. */
        uint64_t d_nsr                 : 1;  /**< [  8:  8](R/W/H) [D_NSR] is ADDRTYPE\<1\> for first direct and gather DPTR reads. ADDRTYPE\<1\> is the
                                                                 no-snoop attribute for PCIe. */
        uint64_t reserved_9_23         : 15;
        uint64_t is64b                 : 1;  /**< [ 24: 24](R/W) If 1, the ring uses 64-byte instructions.
                                                                 If 0, the ring uses 32-byte instructions. */
        uint64_t rdsize                : 2;  /**< [ 26: 25](R/W) Number of instructions to be read in one read request. Two-bit values are:
                                                                 0x0 = 1 instruction.
                                                                 0x1 = 2 instructions.
                                                                 0x2 = 4 instructions.
                                                                 0x3 = 8 instructions. */
        uint64_t reserved_27           : 1;
        uint64_t idle                  : 1;  /**< [ 28: 28](RO/H) Asserted when this ring has no packets in-flight. */
        uint64_t reserved_29_47        : 19;
        uint64_t rpvf                  : 4;  /**< [ 51: 48](RO/H) The number of rings assigned to this VF.
                                                                 Read only copy of SDP()_EPF()_RINFO[RPVF] */
        uint64_t reserved_52_63        : 12;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_in_control_s cn; */
};
typedef union bdk_sdpx_epfx_rx_in_control bdk_sdpx_epfx_rx_in_control_t;

static inline uint64_t BDK_SDPX_EPFX_RX_IN_CONTROL(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_IN_CONTROL(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010000ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_IN_CONTROL", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_IN_CONTROL(a,b,c) bdk_sdpx_epfx_rx_in_control_t
#define bustype_BDK_SDPX_EPFX_RX_IN_CONTROL(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_IN_CONTROL(a,b,c) "SDPX_EPFX_RX_IN_CONTROL"
#define device_bar_BDK_SDPX_EPFX_RX_IN_CONTROL(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_IN_CONTROL(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_IN_CONTROL(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_in_enable
 *
 * SDP Input Instruction Ring Enable Register
 * This register is the enable for read operations on the input instruction rings.
 */
union bdk_sdpx_epfx_rx_in_enable
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_in_enable_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t enb                   : 1;  /**< [  0:  0](R/W/H) Enable for the input ring.  Various errors and FLR events can clear this bit.
                                                                 Software can also clear this bit at anytime. The bit may not be set unless
                                                                 SDP()_EPF()_R()_IN_CONTROL[IDLE] == 0. */
#else /* Word 0 - Little Endian */
        uint64_t enb                   : 1;  /**< [  0:  0](R/W/H) Enable for the input ring.  Various errors and FLR events can clear this bit.
                                                                 Software can also clear this bit at anytime. The bit may not be set unless
                                                                 SDP()_EPF()_R()_IN_CONTROL[IDLE] == 0. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_in_enable_s cn; */
};
typedef union bdk_sdpx_epfx_rx_in_enable bdk_sdpx_epfx_rx_in_enable_t;

static inline uint64_t BDK_SDPX_EPFX_RX_IN_ENABLE(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_IN_ENABLE(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010010ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_IN_ENABLE", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_IN_ENABLE(a,b,c) bdk_sdpx_epfx_rx_in_enable_t
#define bustype_BDK_SDPX_EPFX_RX_IN_ENABLE(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_IN_ENABLE(a,b,c) "SDPX_EPFX_RX_IN_ENABLE"
#define device_bar_BDK_SDPX_EPFX_RX_IN_ENABLE(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_IN_ENABLE(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_IN_ENABLE(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_in_instr_baddr
 *
 * SDP Input Instruction Ring Base Address Register
 * This register contains the base address for the input instruction ring.
 * This register is not affected by reset (including FLR) and must be initialized
 * by the VF prior to enabling the ring.  Also, this register cannot be written
 * while either of the following conditions is true:
 *   * SDP()_EPF()_R()_IN_CONTROL[IDLE] is clear.
 *   * Or, SDP()_EPF()_R()_IN_ENABLE[ENB] is set.
 */
union bdk_sdpx_epfx_rx_in_instr_baddr
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_in_instr_baddr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t addr                  : 60; /**< [ 63:  4](R/W) Base address for input instruction ring. Must be 16-byte aligned. */
        uint64_t reserved_0_3          : 4;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_3          : 4;
        uint64_t addr                  : 60; /**< [ 63:  4](R/W) Base address for input instruction ring. Must be 16-byte aligned. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_in_instr_baddr_s cn; */
};
typedef union bdk_sdpx_epfx_rx_in_instr_baddr bdk_sdpx_epfx_rx_in_instr_baddr_t;

static inline uint64_t BDK_SDPX_EPFX_RX_IN_INSTR_BADDR(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_IN_INSTR_BADDR(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010020ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_IN_INSTR_BADDR", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_IN_INSTR_BADDR(a,b,c) bdk_sdpx_epfx_rx_in_instr_baddr_t
#define bustype_BDK_SDPX_EPFX_RX_IN_INSTR_BADDR(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_IN_INSTR_BADDR(a,b,c) "SDPX_EPFX_RX_IN_INSTR_BADDR"
#define device_bar_BDK_SDPX_EPFX_RX_IN_INSTR_BADDR(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_IN_INSTR_BADDR(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_IN_INSTR_BADDR(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_in_instr_dbell
 *
 * SDP Input Instruction Ring Input Doorbell Registers
 * This register contains the doorbell and base-address offset for the next read operation.
 * This register is not affected by reset (including FLR) and must be initialized
 * by the VF prior to enabling the ring.
 */
union bdk_sdpx_epfx_rx_in_instr_dbell
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_in_instr_dbell_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t aoff                  : 32; /**< [ 63: 32](RO/H) Address offset. The offset from the SDP()_EPF()_R()_IN_INSTR_BADDR where the
                                                                 next pointer is read. A write of 0xFFFFFFFF to [DBELL] clears [DBELL] and [AOFF]. */
        uint64_t dbell                 : 32; /**< [ 31:  0](R/W/H) Pointer list doorbell count. Write operations to this field increments the present
                                                                 value here. Read operations return the present value. The value of this field is
                                                                 decremented as read operations are issued for instructions. A write of 0xFFFFFFFF
                                                                 to this field clears [DBELL] and [AOFF].  This register should be cleared before
                                                                 enabling a ring. */
#else /* Word 0 - Little Endian */
        uint64_t dbell                 : 32; /**< [ 31:  0](R/W/H) Pointer list doorbell count. Write operations to this field increments the present
                                                                 value here. Read operations return the present value. The value of this field is
                                                                 decremented as read operations are issued for instructions. A write of 0xFFFFFFFF
                                                                 to this field clears [DBELL] and [AOFF].  This register should be cleared before
                                                                 enabling a ring. */
        uint64_t aoff                  : 32; /**< [ 63: 32](RO/H) Address offset. The offset from the SDP()_EPF()_R()_IN_INSTR_BADDR where the
                                                                 next pointer is read. A write of 0xFFFFFFFF to [DBELL] clears [DBELL] and [AOFF]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_in_instr_dbell_s cn; */
};
typedef union bdk_sdpx_epfx_rx_in_instr_dbell bdk_sdpx_epfx_rx_in_instr_dbell_t;

static inline uint64_t BDK_SDPX_EPFX_RX_IN_INSTR_DBELL(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_IN_INSTR_DBELL(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010040ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_IN_INSTR_DBELL", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_IN_INSTR_DBELL(a,b,c) bdk_sdpx_epfx_rx_in_instr_dbell_t
#define bustype_BDK_SDPX_EPFX_RX_IN_INSTR_DBELL(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_IN_INSTR_DBELL(a,b,c) "SDPX_EPFX_RX_IN_INSTR_DBELL"
#define device_bar_BDK_SDPX_EPFX_RX_IN_INSTR_DBELL(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_IN_INSTR_DBELL(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_IN_INSTR_DBELL(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_in_instr_rsize
 *
 * SDP Input Instruction Ring Size Register
 * This register contains the input instruction ring size.
 * This register is not affected by reset (including FLR) and must be initialized
 * by the VF prior to enabling the ring.  Also, this register cannot be written
 * while either of the following conditions is true:
 *   * SDP()_EPF()_R()_IN_CONTROL[IDLE] is clear.
 *   * or, SDP()_EPF()_R()_IN_ENABLE[ENB] is set.
 */
union bdk_sdpx_epfx_rx_in_instr_rsize
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_in_instr_rsize_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t rsize                 : 32; /**< [ 31:  0](R/W) Ring size (number of instructions). */
#else /* Word 0 - Little Endian */
        uint64_t rsize                 : 32; /**< [ 31:  0](R/W) Ring size (number of instructions). */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_in_instr_rsize_s cn; */
};
typedef union bdk_sdpx_epfx_rx_in_instr_rsize bdk_sdpx_epfx_rx_in_instr_rsize_t;

static inline uint64_t BDK_SDPX_EPFX_RX_IN_INSTR_RSIZE(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_IN_INSTR_RSIZE(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010030ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_IN_INSTR_RSIZE", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_IN_INSTR_RSIZE(a,b,c) bdk_sdpx_epfx_rx_in_instr_rsize_t
#define bustype_BDK_SDPX_EPFX_RX_IN_INSTR_RSIZE(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_IN_INSTR_RSIZE(a,b,c) "SDPX_EPFX_RX_IN_INSTR_RSIZE"
#define device_bar_BDK_SDPX_EPFX_RX_IN_INSTR_RSIZE(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_IN_INSTR_RSIZE(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_IN_INSTR_RSIZE(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_in_int_levels
 *
 * SDP Input Instruction Interrupt Levels Register
 * This register contains input instruction interrupt levels.
 * This register is not affected by reset (including FLR) and must be initialized
 * by the VF prior to enabling the ring.
 */
union bdk_sdpx_epfx_rx_in_int_levels
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_in_int_levels_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t cnt                   : 32; /**< [ 31:  0](R/W) Input packet counter interrupt threshold. An MSI-X interrupt will be generated
                                                                 whenever SDP()_EPF()_R()_IN_CNTS[CNT] \> [CNT]. Whenever software changes the value of
                                                                 [CNT], it should also subsequently write the corresponding SDP()_R()_IN_CNTS[CNT] CSR
                                                                 (with a value of zero if desired) to ensure that the hardware correspondingly updates
                                                                 SDP()_EPF()_R()_IN_CNTS[IN_INT] */
#else /* Word 0 - Little Endian */
        uint64_t cnt                   : 32; /**< [ 31:  0](R/W) Input packet counter interrupt threshold. An MSI-X interrupt will be generated
                                                                 whenever SDP()_EPF()_R()_IN_CNTS[CNT] \> [CNT]. Whenever software changes the value of
                                                                 [CNT], it should also subsequently write the corresponding SDP()_R()_IN_CNTS[CNT] CSR
                                                                 (with a value of zero if desired) to ensure that the hardware correspondingly updates
                                                                 SDP()_EPF()_R()_IN_CNTS[IN_INT] */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_in_int_levels_s cn; */
};
typedef union bdk_sdpx_epfx_rx_in_int_levels bdk_sdpx_epfx_rx_in_int_levels_t;

static inline uint64_t BDK_SDPX_EPFX_RX_IN_INT_LEVELS(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_IN_INT_LEVELS(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010060ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_IN_INT_LEVELS", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_IN_INT_LEVELS(a,b,c) bdk_sdpx_epfx_rx_in_int_levels_t
#define bustype_BDK_SDPX_EPFX_RX_IN_INT_LEVELS(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_IN_INT_LEVELS(a,b,c) "SDPX_EPFX_RX_IN_INT_LEVELS"
#define device_bar_BDK_SDPX_EPFX_RX_IN_INT_LEVELS(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_IN_INT_LEVELS(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_IN_INT_LEVELS(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_in_int_status
 *
 * SDP Ring Input Packet Interrupt Status Register
 * This register contains interrupt status on a per-VF basis.  All rings for a given VF
 * are located in a single register. Note that access to any ring offset within a given
 * VF will return the same value.  When the PF reads any ring in this register it will
 * return the same value (64 bits each representing one ring.)
 */
union bdk_sdpx_epfx_rx_in_int_status
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_in_int_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t intr                  : 64; /**< [ 63:  0](RO) Interrupt bits for VF rings (0..i). [INTR[i]] reads as one whenever:

                                                                  * SDP()_EPF()_R()_IN_CNTS[CNT] \> SDP()_EPF()_R()_IN_INT_LEVELS[CNT]

                                                                 [INTR] can cause an MSI-X interrupt.

                                                                 Note that "i" depends on the SDP()_EPF()_RINFO configuration.

                                                                 Internal:
                                                                 These interrupt bits are not cleared due to FLR becase the CNTS and
                                                                 LEVELS registers are not reset and we wish to make the interrupt state
                                                                 consistent with CNTS/LEVELS even after FLR. The CNTS register must be
                                                                 cleared by software as part of initialization after a reset (including FLR)
                                                                 which will cause the interrupt state to clear. */
#else /* Word 0 - Little Endian */
        uint64_t intr                  : 64; /**< [ 63:  0](RO) Interrupt bits for VF rings (0..i). [INTR[i]] reads as one whenever:

                                                                  * SDP()_EPF()_R()_IN_CNTS[CNT] \> SDP()_EPF()_R()_IN_INT_LEVELS[CNT]

                                                                 [INTR] can cause an MSI-X interrupt.

                                                                 Note that "i" depends on the SDP()_EPF()_RINFO configuration.

                                                                 Internal:
                                                                 These interrupt bits are not cleared due to FLR becase the CNTS and
                                                                 LEVELS registers are not reset and we wish to make the interrupt state
                                                                 consistent with CNTS/LEVELS even after FLR. The CNTS register must be
                                                                 cleared by software as part of initialization after a reset (including FLR)
                                                                 which will cause the interrupt state to clear. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_in_int_status_s cn; */
};
typedef union bdk_sdpx_epfx_rx_in_int_status bdk_sdpx_epfx_rx_in_int_status_t;

static inline uint64_t BDK_SDPX_EPFX_RX_IN_INT_STATUS(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_IN_INT_STATUS(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010070ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_IN_INT_STATUS", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_IN_INT_STATUS(a,b,c) bdk_sdpx_epfx_rx_in_int_status_t
#define bustype_BDK_SDPX_EPFX_RX_IN_INT_STATUS(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_IN_INT_STATUS(a,b,c) "SDPX_EPFX_RX_IN_INT_STATUS"
#define device_bar_BDK_SDPX_EPFX_RX_IN_INT_STATUS(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_IN_INT_STATUS(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_IN_INT_STATUS(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_in_pkt_cnt
 *
 * SDP Packet Input Packet Count Register
 * This register contains packet counts per ring that have been read into SDP.
 * The counter will wrap when it reaches its maximum value.  It should be cleared
 * before the ring is enabled for an accurate count.
 */
union bdk_sdpx_epfx_rx_in_pkt_cnt
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_in_pkt_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t cnt                   : 36; /**< [ 35:  0](R/W/H) Packet count, can be written by software to any value.  If a value of 0xFFFFFFFFF is
                                                                 written to this field, it will cause this field as well as SDP()_EPF()_R()_IN_BYTE_CNT to
                                                                 clear. */
#else /* Word 0 - Little Endian */
        uint64_t cnt                   : 36; /**< [ 35:  0](R/W/H) Packet count, can be written by software to any value.  If a value of 0xFFFFFFFFF is
                                                                 written to this field, it will cause this field as well as SDP()_EPF()_R()_IN_BYTE_CNT to
                                                                 clear. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_in_pkt_cnt_s cn; */
};
typedef union bdk_sdpx_epfx_rx_in_pkt_cnt bdk_sdpx_epfx_rx_in_pkt_cnt_t;

static inline uint64_t BDK_SDPX_EPFX_RX_IN_PKT_CNT(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_IN_PKT_CNT(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010080ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_IN_PKT_CNT", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_IN_PKT_CNT(a,b,c) bdk_sdpx_epfx_rx_in_pkt_cnt_t
#define bustype_BDK_SDPX_EPFX_RX_IN_PKT_CNT(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_IN_PKT_CNT(a,b,c) "SDPX_EPFX_RX_IN_PKT_CNT"
#define device_bar_BDK_SDPX_EPFX_RX_IN_PKT_CNT(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_IN_PKT_CNT(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_IN_PKT_CNT(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_mbox_pf_vf_data
 *
 * SDP PF to VF Mailbox Data Registers
 * These registers are used for communication of data from the PF to VF.
 * A write to this register from the PF will cause the corresponding bit in
 * SDP()_EPF()_R()_MBOX_PF_VF_INT[INTR] to be set, along with other bits in
 * SDP()_EPF()_R()_MBOX_RINT_STATUS, SDP()_EPF()_R()_OUT_CNTS[MBOX_INT], and
 * SDP()_EPF()_R()_IN_CNTS[MBOX_INT].
 */
union bdk_sdpx_epfx_rx_mbox_pf_vf_data
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_mbox_pf_vf_data_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) Communication data from PF to VF. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) Communication data from PF to VF. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_mbox_pf_vf_data_s cn; */
};
typedef union bdk_sdpx_epfx_rx_mbox_pf_vf_data bdk_sdpx_epfx_rx_mbox_pf_vf_data_t;

static inline uint64_t BDK_SDPX_EPFX_RX_MBOX_PF_VF_DATA(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_MBOX_PF_VF_DATA(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010210ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_MBOX_PF_VF_DATA", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_MBOX_PF_VF_DATA(a,b,c) bdk_sdpx_epfx_rx_mbox_pf_vf_data_t
#define bustype_BDK_SDPX_EPFX_RX_MBOX_PF_VF_DATA(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_MBOX_PF_VF_DATA(a,b,c) "SDPX_EPFX_RX_MBOX_PF_VF_DATA"
#define device_bar_BDK_SDPX_EPFX_RX_MBOX_PF_VF_DATA(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_MBOX_PF_VF_DATA(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_MBOX_PF_VF_DATA(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_mbox_pf_vf_int
 *
 * SDP Packet PF to VF Mailbox Interrupt Register
 * These registers contain interrupt status and enable for the PF to VF mailbox communication
 * registers. A write to SDP()_EPF()_R()_MBOX_VF_PF_DATA from the PF will cause the [INTR] bit
 * in this register to set, along with corresponding bits in SDP()_EPF()_R()_MBOX_RINT_STATUS,
 * SDP()_EPF()_R()_OUT_CNTS[MBOX_INT], and SDP()_EPF()_R()_IN_CNTS[MBOX_INT].
 * All of these bits are cleared by writing 1 to the [INTR] bit in this register.
 * If the [ENAB] bit is set, then an MSI-X interrupt will also be generated when the [INTR] bit
 * is set. This register is cleared also due to an FLR.
 */
union bdk_sdpx_epfx_rx_mbox_pf_vf_int
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_mbox_pf_vf_int_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t enab                  : 1;  /**< [  1:  1](R/W) PF to VF mailbox interrupt enable. */
        uint64_t intr                  : 1;  /**< [  0:  0](R/W1C/H) PF to VF mailbox interrupt signal. */
#else /* Word 0 - Little Endian */
        uint64_t intr                  : 1;  /**< [  0:  0](R/W1C/H) PF to VF mailbox interrupt signal. */
        uint64_t enab                  : 1;  /**< [  1:  1](R/W) PF to VF mailbox interrupt enable. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_mbox_pf_vf_int_s cn; */
};
typedef union bdk_sdpx_epfx_rx_mbox_pf_vf_int bdk_sdpx_epfx_rx_mbox_pf_vf_int_t;

static inline uint64_t BDK_SDPX_EPFX_RX_MBOX_PF_VF_INT(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_MBOX_PF_VF_INT(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010220ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_MBOX_PF_VF_INT", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_MBOX_PF_VF_INT(a,b,c) bdk_sdpx_epfx_rx_mbox_pf_vf_int_t
#define bustype_BDK_SDPX_EPFX_RX_MBOX_PF_VF_INT(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_MBOX_PF_VF_INT(a,b,c) "SDPX_EPFX_RX_MBOX_PF_VF_INT"
#define device_bar_BDK_SDPX_EPFX_RX_MBOX_PF_VF_INT(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_MBOX_PF_VF_INT(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_MBOX_PF_VF_INT(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_mbox_rint_status
 *
 * SDP Mailbox Interrupt Status Register
 * This register contains PF-\>VF mailbox interrupt status on a per-VF basis.
 * All rings for a given VF are located in a single register. Note that access to any ring offset
 * within a given VF will return the same value.  When the PF reads any ring in this register it
 * will return the same value (64 bits each representing one ring.)
 */
union bdk_sdpx_epfx_rx_mbox_rint_status
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_mbox_rint_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t intr                  : 64; /**< [ 63:  0](RO) Interrupt bits for VF rings (0..i). [INTR[i]] reads as one whenever a mailbox
                                                                 interrupt has been signaled by the PF and not cleared by the VF.
                                                                 These bits are cleared by writing SDP()_EPF()_R()_MBOX_PF_VF_INT[INTR]
                                                                 them with a 1, or due to an FLR.

                                                                 [INTR] can cause an MSI-X interrupt.

                                                                 Note that "i" depends on the SDP()_EPF()_RINFO configuration. */
#else /* Word 0 - Little Endian */
        uint64_t intr                  : 64; /**< [ 63:  0](RO) Interrupt bits for VF rings (0..i). [INTR[i]] reads as one whenever a mailbox
                                                                 interrupt has been signaled by the PF and not cleared by the VF.
                                                                 These bits are cleared by writing SDP()_EPF()_R()_MBOX_PF_VF_INT[INTR]
                                                                 them with a 1, or due to an FLR.

                                                                 [INTR] can cause an MSI-X interrupt.

                                                                 Note that "i" depends on the SDP()_EPF()_RINFO configuration. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_mbox_rint_status_s cn; */
};
typedef union bdk_sdpx_epfx_rx_mbox_rint_status bdk_sdpx_epfx_rx_mbox_rint_status_t;

static inline uint64_t BDK_SDPX_EPFX_RX_MBOX_RINT_STATUS(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_MBOX_RINT_STATUS(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010200ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_MBOX_RINT_STATUS", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_MBOX_RINT_STATUS(a,b,c) bdk_sdpx_epfx_rx_mbox_rint_status_t
#define bustype_BDK_SDPX_EPFX_RX_MBOX_RINT_STATUS(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_MBOX_RINT_STATUS(a,b,c) "SDPX_EPFX_RX_MBOX_RINT_STATUS"
#define device_bar_BDK_SDPX_EPFX_RX_MBOX_RINT_STATUS(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_MBOX_RINT_STATUS(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_MBOX_RINT_STATUS(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_mbox_vf_pf_data
 *
 * SDP VF to PF Mailbox Data Registers
 * These registers are used for communication of data from the VF to PF.
 * A write by the VF to this register will cause the corresponding bit in
 * SDP()_MBOX_EPF()_INT to be set to be set, and an MSI-X message to be generated.
 * To clear the interrupt condition, the PF should write a 1 to SDP()_MBOX_EPF()_INT.
 */
union bdk_sdpx_epfx_rx_mbox_vf_pf_data
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_mbox_vf_pf_data_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) Communication data from VF to PF. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) Communication data from VF to PF. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_mbox_vf_pf_data_s cn; */
};
typedef union bdk_sdpx_epfx_rx_mbox_vf_pf_data bdk_sdpx_epfx_rx_mbox_vf_pf_data_t;

static inline uint64_t BDK_SDPX_EPFX_RX_MBOX_VF_PF_DATA(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_MBOX_VF_PF_DATA(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010230ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_MBOX_VF_PF_DATA", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_MBOX_VF_PF_DATA(a,b,c) bdk_sdpx_epfx_rx_mbox_vf_pf_data_t
#define bustype_BDK_SDPX_EPFX_RX_MBOX_VF_PF_DATA(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_MBOX_VF_PF_DATA(a,b,c) "SDPX_EPFX_RX_MBOX_VF_PF_DATA"
#define device_bar_BDK_SDPX_EPFX_RX_MBOX_VF_PF_DATA(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_MBOX_VF_PF_DATA(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_MBOX_VF_PF_DATA(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_out_byte_cnt
 *
 * SDP Packet Output Byte Count Register
 * This register contains byte counts per ring that have been written to memory by SDP.
 * The counter will wrap when it reaches its maximum value.  It should be cleared
 * before the ring is enabled for an accurate count.
 */
union bdk_sdpx_epfx_rx_out_byte_cnt
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_out_byte_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_48_63        : 16;
        uint64_t cnt                   : 48; /**< [ 47:  0](R/W/H) Byte count, can be reset by software by writing SDP()_EPF()_R()_OUT_PKT_CNT[CNT]
                                                                 with 0xFFFFFFFFF. */
#else /* Word 0 - Little Endian */
        uint64_t cnt                   : 48; /**< [ 47:  0](R/W/H) Byte count, can be reset by software by writing SDP()_EPF()_R()_OUT_PKT_CNT[CNT]
                                                                 with 0xFFFFFFFFF. */
        uint64_t reserved_48_63        : 16;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_out_byte_cnt_s cn; */
};
typedef union bdk_sdpx_epfx_rx_out_byte_cnt bdk_sdpx_epfx_rx_out_byte_cnt_t;

static inline uint64_t BDK_SDPX_EPFX_RX_OUT_BYTE_CNT(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_OUT_BYTE_CNT(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010190ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_OUT_BYTE_CNT", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_OUT_BYTE_CNT(a,b,c) bdk_sdpx_epfx_rx_out_byte_cnt_t
#define bustype_BDK_SDPX_EPFX_RX_OUT_BYTE_CNT(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_OUT_BYTE_CNT(a,b,c) "SDPX_EPFX_RX_OUT_BYTE_CNT"
#define device_bar_BDK_SDPX_EPFX_RX_OUT_BYTE_CNT(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_OUT_BYTE_CNT(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_OUT_BYTE_CNT(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_out_cnts
 *
 * SDP Packet Output Counts Register
 * This register contains the counters for SDP output ports.
 * This register is not affected by reset (including FLR) and must be initialized
 * by the VF prior to enabling the ring.
 */
union bdk_sdpx_epfx_rx_out_cnts
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_out_cnts_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_63           : 1;
        uint64_t out_int               : 1;  /**< [ 62: 62](RO/H) Returns a 1 when:
                                                                  * SDP()_EPF()_R()_OUT_CNTS[CNT] \> SDP()_EPF()_R()_OUT_INT_LEVELS[CNT].
                                                                  * Or, SDP()_EPF()_R()_OUT_CNTS[TIMER] \> SDP()_EPF()_R()_OUT_INT_LEVELS[TIMET].

                                                                 To clear the bit, the CNTS register must be written to clear the underlying condition. */
        uint64_t in_int                : 1;  /**< [ 61: 61](RO/H) Returns a 1 when:
                                                                  * SDP()_EPF()_R()_IN_CNTS[CNT] \> SDP()_EPF()_R()_IN_INT_LEVELS[CNT].

                                                                 To clear the bit, the SDP()_EPF()_R()_IN_CNTS register must be written to clear the
                                                                 underlying condition. */
        uint64_t mbox_int              : 1;  /**< [ 60: 60](RO/H) Returns a 1 when:
                                                                  * SDP()_EPF()_R()_MBOX_RINT_STATUS[INTR] is set.

                                                                 To clear the bit, write SDP()_EPF()_R()_MBOX_PF_VF_INT[INTR] with 1.
                                                                 This bit is also cleared due to an FLR. */
        uint64_t resend                : 1;  /**< [ 59: 59](WO/H) A write of 1 will resend an MSI-X interrupt message if any of the following
                                                                 conditions are true for the respective ring R():
                                                                  * SDP()_EPF()_R()_OUT_CNTS[CNT]   \> SDP()_EPF()_R()_OUT_INT_LEVELS[CNT],
                                                                  * SDP()_EPF()_R()_OUT_CNTS[TIMER] \> SDP()_EPF()_R()_OUT_INT_LEVELS[TIMET],
                                                                  * SDP()_EPF()_R()_IN_CNTS[CNT] \> SDP()_EPF()_R()_IN_INT_LEVELS[CNT],
                                                                  * SDP()_EPF()_R()_MBOX_RINT_STATUS[INTR] is set. */
        uint64_t reserved_54_58        : 5;
        uint64_t timer                 : 22; /**< [ 53: 32](RO/H) Timer, incremented every 1024 coprocessor-clock cycles when [CNT] is
                                                                 not zero. The hardware clears [TIMER] when [CNT]
                                                                 goes to 0. The first increment of this count can occur between 0 to
                                                                 1023 coprocessor-clock cycles after [CNT] becomes nonzero. */
        uint64_t cnt                   : 32; /**< [ 31:  0](R/W/H) Packet counter. Hardware adds to [CNT] as it sends packets out. On a write
                                                                 to this CSR, hardware subtracts the amount written to the [CNT] field from
                                                                 [CNT], which will clear SDP()_EPF()_R()_OUT_INT_STATUS[INTR] if [CNT] becomes \<=
                                                                 SDP()_EPF()_R()_OUT_INT_LEVELS[CNT]. When SDP()_EPF()_R()_OUT_INT_LEVELS[BMODE] is clear,
                                                                 the hardware adds 1 to [CNT] per packet. When SDP()_EPF()_R()_OUT_INT_LEVELS[BMODE] is
                                                                 set,
                                                                 the hardware adds the packet length to [CNT] per packet. */
#else /* Word 0 - Little Endian */
        uint64_t cnt                   : 32; /**< [ 31:  0](R/W/H) Packet counter. Hardware adds to [CNT] as it sends packets out. On a write
                                                                 to this CSR, hardware subtracts the amount written to the [CNT] field from
                                                                 [CNT], which will clear SDP()_EPF()_R()_OUT_INT_STATUS[INTR] if [CNT] becomes \<=
                                                                 SDP()_EPF()_R()_OUT_INT_LEVELS[CNT]. When SDP()_EPF()_R()_OUT_INT_LEVELS[BMODE] is clear,
                                                                 the hardware adds 1 to [CNT] per packet. When SDP()_EPF()_R()_OUT_INT_LEVELS[BMODE] is
                                                                 set,
                                                                 the hardware adds the packet length to [CNT] per packet. */
        uint64_t timer                 : 22; /**< [ 53: 32](RO/H) Timer, incremented every 1024 coprocessor-clock cycles when [CNT] is
                                                                 not zero. The hardware clears [TIMER] when [CNT]
                                                                 goes to 0. The first increment of this count can occur between 0 to
                                                                 1023 coprocessor-clock cycles after [CNT] becomes nonzero. */
        uint64_t reserved_54_58        : 5;
        uint64_t resend                : 1;  /**< [ 59: 59](WO/H) A write of 1 will resend an MSI-X interrupt message if any of the following
                                                                 conditions are true for the respective ring R():
                                                                  * SDP()_EPF()_R()_OUT_CNTS[CNT]   \> SDP()_EPF()_R()_OUT_INT_LEVELS[CNT],
                                                                  * SDP()_EPF()_R()_OUT_CNTS[TIMER] \> SDP()_EPF()_R()_OUT_INT_LEVELS[TIMET],
                                                                  * SDP()_EPF()_R()_IN_CNTS[CNT] \> SDP()_EPF()_R()_IN_INT_LEVELS[CNT],
                                                                  * SDP()_EPF()_R()_MBOX_RINT_STATUS[INTR] is set. */
        uint64_t mbox_int              : 1;  /**< [ 60: 60](RO/H) Returns a 1 when:
                                                                  * SDP()_EPF()_R()_MBOX_RINT_STATUS[INTR] is set.

                                                                 To clear the bit, write SDP()_EPF()_R()_MBOX_PF_VF_INT[INTR] with 1.
                                                                 This bit is also cleared due to an FLR. */
        uint64_t in_int                : 1;  /**< [ 61: 61](RO/H) Returns a 1 when:
                                                                  * SDP()_EPF()_R()_IN_CNTS[CNT] \> SDP()_EPF()_R()_IN_INT_LEVELS[CNT].

                                                                 To clear the bit, the SDP()_EPF()_R()_IN_CNTS register must be written to clear the
                                                                 underlying condition. */
        uint64_t out_int               : 1;  /**< [ 62: 62](RO/H) Returns a 1 when:
                                                                  * SDP()_EPF()_R()_OUT_CNTS[CNT] \> SDP()_EPF()_R()_OUT_INT_LEVELS[CNT].
                                                                  * Or, SDP()_EPF()_R()_OUT_CNTS[TIMER] \> SDP()_EPF()_R()_OUT_INT_LEVELS[TIMET].

                                                                 To clear the bit, the CNTS register must be written to clear the underlying condition. */
        uint64_t reserved_63           : 1;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_out_cnts_s cn; */
};
typedef union bdk_sdpx_epfx_rx_out_cnts bdk_sdpx_epfx_rx_out_cnts_t;

static inline uint64_t BDK_SDPX_EPFX_RX_OUT_CNTS(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_OUT_CNTS(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010100ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_OUT_CNTS", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_OUT_CNTS(a,b,c) bdk_sdpx_epfx_rx_out_cnts_t
#define bustype_BDK_SDPX_EPFX_RX_OUT_CNTS(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_OUT_CNTS(a,b,c) "SDPX_EPFX_RX_OUT_CNTS"
#define device_bar_BDK_SDPX_EPFX_RX_OUT_CNTS(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_OUT_CNTS(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_OUT_CNTS(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_out_control
 *
 * SDP Packet Output Control Register
 * This register contains control bits for output packet rings.
 * This register is not affected by reset (including FLR) and must be initialized
 * by the VF prior to enabling the ring.  Also, this register cannot be written
 * while either of the following conditions is true:
 *   * SDP()_EPF()_R()_OUT_CONTROL[IDLE] is clear.
 *   * Or, SDP()_EPF()_R()_OUT_ENABLE[ENB] is set.
 */
union bdk_sdpx_epfx_rx_out_control
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_out_control_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_37_63        : 27;
        uint64_t idle                  : 1;  /**< [ 36: 36](RO/H) Asserted when this ring has no packets in-flight. */
        uint64_t es_i                  : 2;  /**< [ 35: 34](R/W) [ES_I] is ES\<1:0\> for info buffer write operations to buffer/info
                                                                 pair MAC memory space addresses fetched from packet output ring. ES\<1:0\> is the
                                                                 endian-swap attribute for these MAC memory space writes. */
        uint64_t nsr_i                 : 1;  /**< [ 33: 33](R/W) [NSR] is ADDRTYPE\<1\> for info buffer write operations to buffer/info
                                                                 pair MAC memory space addresses fetched from packet output ring. ADDRTYPE\<1\> is
                                                                 the no-snoop attribute for PCIe. */
        uint64_t ror_i                 : 1;  /**< [ 32: 32](R/W) [ROR] is ADDRTYPE\<0\> for info buffer write operations to buffer/info
                                                                 pair MAC memory space addresses fetched from packet output ring. ADDRTYPE\<0\> is
                                                                 the relaxed-order attribute for PCIe. */
        uint64_t es_d                  : 2;  /**< [ 31: 30](R/W) [ES] is ES\<1:0\> for data buffer write operations to buffer/info
                                                                 pair MAC memory space addresses fetched from packet output ring. ES\<1:0\> is the
                                                                 endian-swap attribute for these MAC memory space writes. */
        uint64_t nsr_d                 : 1;  /**< [ 29: 29](R/W) [NSR] is ADDRTYPE\<1\> for data buffer write operations to buffer/info
                                                                 pair MAC memory space addresses fetched from packet output ring. ADDRTYPE\<1\> is
                                                                 the no-snoop attribute for PCIe. */
        uint64_t ror_d                 : 1;  /**< [ 28: 28](R/W) [ROR] is ADDRTYPE\<0\> for data buffer write operations to buffer/info
                                                                 pair MAC memory space addresses fetched from packet output ring. ADDRTYPE\<0\> is
                                                                 the relaxed-order attribute for PCIe. */
        uint64_t es_p                  : 2;  /**< [ 27: 26](R/W) [ES_P] is ES\<1:0\> for the packet output ring reads that fetch buffer/info pointer pairs
                                                                 (from SLI_PKT()_SLIST_BADDR[ADDR]+). ES\<1:0\> is the endian-swap attribute for these
                                                                 MAC memory space reads. */
        uint64_t nsr_p                 : 1;  /**< [ 25: 25](R/W) [NSR_P] is ADDRTYPE\<1\> for the packet output ring reads that fetch buffer/info pointer
                                                                 pairs (from SLI_PKT()_SLIST_BADDR[ADDR]+). ADDRTYPE\<1\> is the no-snoop attribute for PCIe. */
        uint64_t ror_p                 : 1;  /**< [ 24: 24](R/W) [ROR_P] is ADDRTYPE\<0\> for the packet output ring reads that fetch buffer/info pointer
                                                                 pairs (from SLI_PKT()_SLIST_BADDR[ADDR]+). ADDRTYPE\<0\> is the relaxed-order attribute
                                                                 for PCIe. */
        uint64_t imode                 : 1;  /**< [ 23: 23](R/W) When IMODE=1, packet output ring is in info-pointer mode; otherwise the packet output ring
                                                                 is in buffer-pointer-only mode. */
        uint64_t isize                 : 7;  /**< [ 22: 16](R/W/H) Info bytes size (bytes) for the output port. Legal sizes are 0 to 120. Not used
                                                                 in buffer-pointer-only mode.  If a value is written that is between 120-127 then
                                                                 a value of 120 will be forced by hardware. */
        uint64_t bsize                 : 16; /**< [ 15:  0](R/W/H) Buffer size (bytes) for the output ring.  The minimum size is 128 bytes; if a value
                                                                 smaller than 128 is written, hardware will force a value of 128. */
#else /* Word 0 - Little Endian */
        uint64_t bsize                 : 16; /**< [ 15:  0](R/W/H) Buffer size (bytes) for the output ring.  The minimum size is 128 bytes; if a value
                                                                 smaller than 128 is written, hardware will force a value of 128. */
        uint64_t isize                 : 7;  /**< [ 22: 16](R/W/H) Info bytes size (bytes) for the output port. Legal sizes are 0 to 120. Not used
                                                                 in buffer-pointer-only mode.  If a value is written that is between 120-127 then
                                                                 a value of 120 will be forced by hardware. */
        uint64_t imode                 : 1;  /**< [ 23: 23](R/W) When IMODE=1, packet output ring is in info-pointer mode; otherwise the packet output ring
                                                                 is in buffer-pointer-only mode. */
        uint64_t ror_p                 : 1;  /**< [ 24: 24](R/W) [ROR_P] is ADDRTYPE\<0\> for the packet output ring reads that fetch buffer/info pointer
                                                                 pairs (from SLI_PKT()_SLIST_BADDR[ADDR]+). ADDRTYPE\<0\> is the relaxed-order attribute
                                                                 for PCIe. */
        uint64_t nsr_p                 : 1;  /**< [ 25: 25](R/W) [NSR_P] is ADDRTYPE\<1\> for the packet output ring reads that fetch buffer/info pointer
                                                                 pairs (from SLI_PKT()_SLIST_BADDR[ADDR]+). ADDRTYPE\<1\> is the no-snoop attribute for PCIe. */
        uint64_t es_p                  : 2;  /**< [ 27: 26](R/W) [ES_P] is ES\<1:0\> for the packet output ring reads that fetch buffer/info pointer pairs
                                                                 (from SLI_PKT()_SLIST_BADDR[ADDR]+). ES\<1:0\> is the endian-swap attribute for these
                                                                 MAC memory space reads. */
        uint64_t ror_d                 : 1;  /**< [ 28: 28](R/W) [ROR] is ADDRTYPE\<0\> for data buffer write operations to buffer/info
                                                                 pair MAC memory space addresses fetched from packet output ring. ADDRTYPE\<0\> is
                                                                 the relaxed-order attribute for PCIe. */
        uint64_t nsr_d                 : 1;  /**< [ 29: 29](R/W) [NSR] is ADDRTYPE\<1\> for data buffer write operations to buffer/info
                                                                 pair MAC memory space addresses fetched from packet output ring. ADDRTYPE\<1\> is
                                                                 the no-snoop attribute for PCIe. */
        uint64_t es_d                  : 2;  /**< [ 31: 30](R/W) [ES] is ES\<1:0\> for data buffer write operations to buffer/info
                                                                 pair MAC memory space addresses fetched from packet output ring. ES\<1:0\> is the
                                                                 endian-swap attribute for these MAC memory space writes. */
        uint64_t ror_i                 : 1;  /**< [ 32: 32](R/W) [ROR] is ADDRTYPE\<0\> for info buffer write operations to buffer/info
                                                                 pair MAC memory space addresses fetched from packet output ring. ADDRTYPE\<0\> is
                                                                 the relaxed-order attribute for PCIe. */
        uint64_t nsr_i                 : 1;  /**< [ 33: 33](R/W) [NSR] is ADDRTYPE\<1\> for info buffer write operations to buffer/info
                                                                 pair MAC memory space addresses fetched from packet output ring. ADDRTYPE\<1\> is
                                                                 the no-snoop attribute for PCIe. */
        uint64_t es_i                  : 2;  /**< [ 35: 34](R/W) [ES_I] is ES\<1:0\> for info buffer write operations to buffer/info
                                                                 pair MAC memory space addresses fetched from packet output ring. ES\<1:0\> is the
                                                                 endian-swap attribute for these MAC memory space writes. */
        uint64_t idle                  : 1;  /**< [ 36: 36](RO/H) Asserted when this ring has no packets in-flight. */
        uint64_t reserved_37_63        : 27;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_out_control_s cn; */
};
typedef union bdk_sdpx_epfx_rx_out_control bdk_sdpx_epfx_rx_out_control_t;

static inline uint64_t BDK_SDPX_EPFX_RX_OUT_CONTROL(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_OUT_CONTROL(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010150ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_OUT_CONTROL", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_OUT_CONTROL(a,b,c) bdk_sdpx_epfx_rx_out_control_t
#define bustype_BDK_SDPX_EPFX_RX_OUT_CONTROL(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_OUT_CONTROL(a,b,c) "SDPX_EPFX_RX_OUT_CONTROL"
#define device_bar_BDK_SDPX_EPFX_RX_OUT_CONTROL(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_OUT_CONTROL(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_OUT_CONTROL(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_out_enable
 *
 * SDP Packet Output Enable Register
 * This register is the enable for the output pointer rings.
 */
union bdk_sdpx_epfx_rx_out_enable
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_out_enable_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t enb                   : 1;  /**< [  0:  0](R/W/H) Enable for the output ring i. This bit can be cleared by hardware if certain
                                                                 errors occur or an FLR is indicated by the remote host. It can be cleared by
                                                                 software at any time. It cannot be set unless SDP()_EPF()_R()_OUT_CONTROL[IDLE] == 0. */
#else /* Word 0 - Little Endian */
        uint64_t enb                   : 1;  /**< [  0:  0](R/W/H) Enable for the output ring i. This bit can be cleared by hardware if certain
                                                                 errors occur or an FLR is indicated by the remote host. It can be cleared by
                                                                 software at any time. It cannot be set unless SDP()_EPF()_R()_OUT_CONTROL[IDLE] == 0. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_out_enable_s cn; */
};
typedef union bdk_sdpx_epfx_rx_out_enable bdk_sdpx_epfx_rx_out_enable_t;

static inline uint64_t BDK_SDPX_EPFX_RX_OUT_ENABLE(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_OUT_ENABLE(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010160ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_OUT_ENABLE", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_OUT_ENABLE(a,b,c) bdk_sdpx_epfx_rx_out_enable_t
#define bustype_BDK_SDPX_EPFX_RX_OUT_ENABLE(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_OUT_ENABLE(a,b,c) "SDPX_EPFX_RX_OUT_ENABLE"
#define device_bar_BDK_SDPX_EPFX_RX_OUT_ENABLE(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_OUT_ENABLE(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_OUT_ENABLE(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_out_int_levels
 *
 * SDP Packet Output Interrupt Levels Register
 * This register contains SDP output packet interrupt levels.
 * This register is not affected by reset (including FLR) and must be initialized
 * by the VF prior to enabling the ring.
 */
union bdk_sdpx_epfx_rx_out_int_levels
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_out_int_levels_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t bmode                 : 1;  /**< [ 63: 63](R/W) Determines whether SDP()_EPF()_R()_OUT_CNTS[CNT] is a byte or packet counter. When
                                                                 [BMODE]=1,
                                                                 SDP()_EPF()_R()_OUT_CNTS[CNT] is a byte counter, else SDP()_EPF()_R()_OUT_CNTS[CNT] is a
                                                                 packet
                                                                 counter. */
        uint64_t reserved_54_62        : 9;
        uint64_t timet                 : 22; /**< [ 53: 32](R/W) Output port counter time interrupt threshold. An MSI-X interrupt will be generated
                                                                 whenever SDP()_EPF()_R()_OUT_CNTS[TIMER] \> [TIMET]. Whenever software changes the value of
                                                                 [TIMET], it should also subsequently write the corresponding SDP()_EPF()_R()_OUT_CNTS CSR
                                                                 (with
                                                                 a value of zero if desired) to ensure that the hardware correspondingly updates
                                                                 SDP()_EPF()_R()_OUT_CNTS[OUT_INT]. */
        uint64_t cnt                   : 32; /**< [ 31:  0](R/W) Output port counter interrupt threshold. An MSI-X interrupt will be generated
                                                                 whenever SDP()_EPF()_R()_OUT_CNTS[CNT] \> [CNT]. Whenever software changes the value of
                                                                 [CNT], it should also subsequently write the corresponding SDP()_EPF()_R()_OUT_CNTS CSR
                                                                 (with a
                                                                 value of zero if desired) to ensure that the hardware correspondingly updates
                                                                 SDP()_EPF()_R()_OUT_CNTS[OUT_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t cnt                   : 32; /**< [ 31:  0](R/W) Output port counter interrupt threshold. An MSI-X interrupt will be generated
                                                                 whenever SDP()_EPF()_R()_OUT_CNTS[CNT] \> [CNT]. Whenever software changes the value of
                                                                 [CNT], it should also subsequently write the corresponding SDP()_EPF()_R()_OUT_CNTS CSR
                                                                 (with a
                                                                 value of zero if desired) to ensure that the hardware correspondingly updates
                                                                 SDP()_EPF()_R()_OUT_CNTS[OUT_INT]. */
        uint64_t timet                 : 22; /**< [ 53: 32](R/W) Output port counter time interrupt threshold. An MSI-X interrupt will be generated
                                                                 whenever SDP()_EPF()_R()_OUT_CNTS[TIMER] \> [TIMET]. Whenever software changes the value of
                                                                 [TIMET], it should also subsequently write the corresponding SDP()_EPF()_R()_OUT_CNTS CSR
                                                                 (with
                                                                 a value of zero if desired) to ensure that the hardware correspondingly updates
                                                                 SDP()_EPF()_R()_OUT_CNTS[OUT_INT]. */
        uint64_t reserved_54_62        : 9;
        uint64_t bmode                 : 1;  /**< [ 63: 63](R/W) Determines whether SDP()_EPF()_R()_OUT_CNTS[CNT] is a byte or packet counter. When
                                                                 [BMODE]=1,
                                                                 SDP()_EPF()_R()_OUT_CNTS[CNT] is a byte counter, else SDP()_EPF()_R()_OUT_CNTS[CNT] is a
                                                                 packet
                                                                 counter. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_out_int_levels_s cn; */
};
typedef union bdk_sdpx_epfx_rx_out_int_levels bdk_sdpx_epfx_rx_out_int_levels_t;

static inline uint64_t BDK_SDPX_EPFX_RX_OUT_INT_LEVELS(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_OUT_INT_LEVELS(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010110ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_OUT_INT_LEVELS", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_OUT_INT_LEVELS(a,b,c) bdk_sdpx_epfx_rx_out_int_levels_t
#define bustype_BDK_SDPX_EPFX_RX_OUT_INT_LEVELS(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_OUT_INT_LEVELS(a,b,c) "SDPX_EPFX_RX_OUT_INT_LEVELS"
#define device_bar_BDK_SDPX_EPFX_RX_OUT_INT_LEVELS(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_OUT_INT_LEVELS(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_OUT_INT_LEVELS(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_out_int_status
 *
 * SDP Output Packet Interrupt Status Register
 * This register contains interrupt status on a per-VF basis.  All rings for a given VF
 * are located in a single register. Note that access to any ring offset within a given
 * VF will return the same value.  When the PF reads any ring in this register it will
 * return the same value (64 bits each representing one ring.)
 */
union bdk_sdpx_epfx_rx_out_int_status
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_out_int_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t intr                  : 64; /**< [ 63:  0](RO) Packet output interrupt bit for a given VFR's ports (0..i). [INTR]\<ring\> reads
                                                                 as one whenever for the respective ring R(ring):

                                                                  * SDP()_EPF()_R()_OUT_CNTS[CNT]   \> SDP()_EPF()_R()_OUT_INT_LEVELS[CNT].

                                                                  * Or, SDP()_EPF()_R()_OUT_CNTS[TIMER] \> SDP()_EPF()_R()_OUT_INT_LEVELS[TIMET].

                                                                 [INTR] can cause an MSI-X interrupt.

                                                                 Internal:
                                                                 These interrupt bits are not cleared due to FLR becase the CNTS and
                                                                 LEVELS registers are not reset and we wish to make the interrupt state
                                                                 consistent with CNTS/LEVELS even after FLR. The CNTS register must be
                                                                 cleared by software as part of initialization after a reset (including FLR)
                                                                 which will cause the interrupt state to clear. */
#else /* Word 0 - Little Endian */
        uint64_t intr                  : 64; /**< [ 63:  0](RO) Packet output interrupt bit for a given VFR's ports (0..i). [INTR]\<ring\> reads
                                                                 as one whenever for the respective ring R(ring):

                                                                  * SDP()_EPF()_R()_OUT_CNTS[CNT]   \> SDP()_EPF()_R()_OUT_INT_LEVELS[CNT].

                                                                  * Or, SDP()_EPF()_R()_OUT_CNTS[TIMER] \> SDP()_EPF()_R()_OUT_INT_LEVELS[TIMET].

                                                                 [INTR] can cause an MSI-X interrupt.

                                                                 Internal:
                                                                 These interrupt bits are not cleared due to FLR becase the CNTS and
                                                                 LEVELS registers are not reset and we wish to make the interrupt state
                                                                 consistent with CNTS/LEVELS even after FLR. The CNTS register must be
                                                                 cleared by software as part of initialization after a reset (including FLR)
                                                                 which will cause the interrupt state to clear. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_out_int_status_s cn; */
};
typedef union bdk_sdpx_epfx_rx_out_int_status bdk_sdpx_epfx_rx_out_int_status_t;

static inline uint64_t BDK_SDPX_EPFX_RX_OUT_INT_STATUS(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_OUT_INT_STATUS(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010170ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_OUT_INT_STATUS", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_OUT_INT_STATUS(a,b,c) bdk_sdpx_epfx_rx_out_int_status_t
#define bustype_BDK_SDPX_EPFX_RX_OUT_INT_STATUS(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_OUT_INT_STATUS(a,b,c) "SDPX_EPFX_RX_OUT_INT_STATUS"
#define device_bar_BDK_SDPX_EPFX_RX_OUT_INT_STATUS(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_OUT_INT_STATUS(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_OUT_INT_STATUS(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_out_pkt_cnt
 *
 * SDP Packet Output Packet Count Register
 * This register contains packet counts per ring that have been written to memory by SDP.
 * The counter will wrap when it reaches its maximum value.  It should be cleared
 * before the ring is enabled for an accurate count.
 */
union bdk_sdpx_epfx_rx_out_pkt_cnt
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_out_pkt_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t cnt                   : 36; /**< [ 35:  0](R/W/H) Packet count, can be written by software to any value.  If a value of 0xFFFFFFFFF is
                                                                 written to this field, it will cause this field as well as SDP()_EPF()_R()_OUT_BYTE_CNT to
                                                                 clear. */
#else /* Word 0 - Little Endian */
        uint64_t cnt                   : 36; /**< [ 35:  0](R/W/H) Packet count, can be written by software to any value.  If a value of 0xFFFFFFFFF is
                                                                 written to this field, it will cause this field as well as SDP()_EPF()_R()_OUT_BYTE_CNT to
                                                                 clear. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_out_pkt_cnt_s cn; */
};
typedef union bdk_sdpx_epfx_rx_out_pkt_cnt bdk_sdpx_epfx_rx_out_pkt_cnt_t;

static inline uint64_t BDK_SDPX_EPFX_RX_OUT_PKT_CNT(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_OUT_PKT_CNT(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010180ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_OUT_PKT_CNT", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_OUT_PKT_CNT(a,b,c) bdk_sdpx_epfx_rx_out_pkt_cnt_t
#define bustype_BDK_SDPX_EPFX_RX_OUT_PKT_CNT(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_OUT_PKT_CNT(a,b,c) "SDPX_EPFX_RX_OUT_PKT_CNT"
#define device_bar_BDK_SDPX_EPFX_RX_OUT_PKT_CNT(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_OUT_PKT_CNT(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_OUT_PKT_CNT(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_out_slist_baddr
 *
 * SDP Packet Ring Base Address Register
 * This register contains the base address for the output ring.
 * This register is not affected by reset (including FLR) and must be initialized
 * by the VF prior to enabling the ring.  Also, this register cannot be written
 * while either of the following conditions is true:
 *   * SDP()_EPF()_R()_OUT_CONTROL[IDLE] is clear.
 *   * Or, SDP()_EPF()_R()_OUT_ENABLE[ENB] is set.
 */
union bdk_sdpx_epfx_rx_out_slist_baddr
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_out_slist_baddr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t addr                  : 60; /**< [ 63:  4](R/W) Base address for the output ring, which is an array with
                                                                 SDP()_EPF()_R()_OUT_SLIST_FIFO_RSIZE[RSIZE] entries, each entry being a
                                                                 SDP_BUF_INFO_PAIR_S.

                                                                 SDP()_EPF()_R()_OUT_SLIST_BADDR contains a byte address that must be 16-byte
                                                                 aligned, so SDP()_EPF()_R()_OUT_SLIST_BADDR\<3:0\> must be zero. */
        uint64_t reserved_0_3          : 4;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_3          : 4;
        uint64_t addr                  : 60; /**< [ 63:  4](R/W) Base address for the output ring, which is an array with
                                                                 SDP()_EPF()_R()_OUT_SLIST_FIFO_RSIZE[RSIZE] entries, each entry being a
                                                                 SDP_BUF_INFO_PAIR_S.

                                                                 SDP()_EPF()_R()_OUT_SLIST_BADDR contains a byte address that must be 16-byte
                                                                 aligned, so SDP()_EPF()_R()_OUT_SLIST_BADDR\<3:0\> must be zero. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_out_slist_baddr_s cn; */
};
typedef union bdk_sdpx_epfx_rx_out_slist_baddr bdk_sdpx_epfx_rx_out_slist_baddr_t;

static inline uint64_t BDK_SDPX_EPFX_RX_OUT_SLIST_BADDR(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_OUT_SLIST_BADDR(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010120ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_OUT_SLIST_BADDR", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_OUT_SLIST_BADDR(a,b,c) bdk_sdpx_epfx_rx_out_slist_baddr_t
#define bustype_BDK_SDPX_EPFX_RX_OUT_SLIST_BADDR(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_OUT_SLIST_BADDR(a,b,c) "SDPX_EPFX_RX_OUT_SLIST_BADDR"
#define device_bar_BDK_SDPX_EPFX_RX_OUT_SLIST_BADDR(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_OUT_SLIST_BADDR(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_OUT_SLIST_BADDR(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_out_slist_dbell
 *
 * SDP Packet Base-Address Offset and Doorbell Registers
 * This register contains the doorbell and base-address offset for the next read operation.
 * This register is not affected by reset (including FLR) and must be initialized
 * by the VF prior to enabling the ring.
 */
union bdk_sdpx_epfx_rx_out_slist_dbell
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_out_slist_dbell_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t aoff                  : 32; /**< [ 63: 32](RO/H) Address offset. The offset from the SDP()_EPF()_R()_OUT_SLIST_BADDR where the next pointer
                                                                 is read.
                                                                 A write of 0xFFFFFFFF to [DBELL] clears [DBELL] and [AOFF]. */
        uint64_t dbell                 : 32; /**< [ 31:  0](R/W/H) Pointer pair list doorbell count. Write operations to this field increments the present
                                                                 value here. Read operations return the present value. The value of this field is
                                                                 decremented as read operations are issued for scatter pointers. A write of 0xFFFFFFFF
                                                                 to this field clears [DBELL] and [AOFF]. The value of this field is in number of
                                                                 SDP_BUF_INFO_PAIR_S's.  This register should be cleared before enabling a ring. */
#else /* Word 0 - Little Endian */
        uint64_t dbell                 : 32; /**< [ 31:  0](R/W/H) Pointer pair list doorbell count. Write operations to this field increments the present
                                                                 value here. Read operations return the present value. The value of this field is
                                                                 decremented as read operations are issued for scatter pointers. A write of 0xFFFFFFFF
                                                                 to this field clears [DBELL] and [AOFF]. The value of this field is in number of
                                                                 SDP_BUF_INFO_PAIR_S's.  This register should be cleared before enabling a ring. */
        uint64_t aoff                  : 32; /**< [ 63: 32](RO/H) Address offset. The offset from the SDP()_EPF()_R()_OUT_SLIST_BADDR where the next pointer
                                                                 is read.
                                                                 A write of 0xFFFFFFFF to [DBELL] clears [DBELL] and [AOFF]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_out_slist_dbell_s cn; */
};
typedef union bdk_sdpx_epfx_rx_out_slist_dbell bdk_sdpx_epfx_rx_out_slist_dbell_t;

static inline uint64_t BDK_SDPX_EPFX_RX_OUT_SLIST_DBELL(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_OUT_SLIST_DBELL(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010140ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_OUT_SLIST_DBELL", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_OUT_SLIST_DBELL(a,b,c) bdk_sdpx_epfx_rx_out_slist_dbell_t
#define bustype_BDK_SDPX_EPFX_RX_OUT_SLIST_DBELL(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_OUT_SLIST_DBELL(a,b,c) "SDPX_EPFX_RX_OUT_SLIST_DBELL"
#define device_bar_BDK_SDPX_EPFX_RX_OUT_SLIST_DBELL(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_OUT_SLIST_DBELL(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_OUT_SLIST_DBELL(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_out_slist_rsize
 *
 * SDP Packet Ring Size Register
 * This register contains the output packet ring size.
 * This register is not affected by reset (including FLR) and must be initialized
 * by the VF prior to enabling the ring.  Also, this register cannot be written
 * while either of the following conditions is true:
 *   * SDP()_EPF()_R()_OUT_CONTROL[IDLE] is clear.
 *   * Or, SDP()_EPF()_R()_OUT_ENABLE[ENB] is set.
 */
union bdk_sdpx_epfx_rx_out_slist_rsize
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_out_slist_rsize_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t rsize                 : 32; /**< [ 31:  0](R/W/H) Ring size (number of SDP_BUF_INFO_PAIR_S's). This value must be 16 or
                                                                 greater. If a value is written that is less than 16, then hardware
                                                                 will force a value of 16 to be written. */
#else /* Word 0 - Little Endian */
        uint64_t rsize                 : 32; /**< [ 31:  0](R/W/H) Ring size (number of SDP_BUF_INFO_PAIR_S's). This value must be 16 or
                                                                 greater. If a value is written that is less than 16, then hardware
                                                                 will force a value of 16 to be written. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_out_slist_rsize_s cn; */
};
typedef union bdk_sdpx_epfx_rx_out_slist_rsize bdk_sdpx_epfx_rx_out_slist_rsize_t;

static inline uint64_t BDK_SDPX_EPFX_RX_OUT_SLIST_RSIZE(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_OUT_SLIST_RSIZE(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010130ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_OUT_SLIST_RSIZE", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_OUT_SLIST_RSIZE(a,b,c) bdk_sdpx_epfx_rx_out_slist_rsize_t
#define bustype_BDK_SDPX_EPFX_RX_OUT_SLIST_RSIZE(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_OUT_SLIST_RSIZE(a,b,c) "SDPX_EPFX_RX_OUT_SLIST_RSIZE"
#define device_bar_BDK_SDPX_EPFX_RX_OUT_SLIST_RSIZE(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_OUT_SLIST_RSIZE(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_OUT_SLIST_RSIZE(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_epf#_r#_vf_num
 *
 * SDP Ring Error Type Register
 * These registers provide the virtual function number for each ring (both input and
 * output). They must be programmed by the PF along with SDP()_EPF()_RINFO before
 * the given ring is enabled. They are not accessible by the VF.
 *
 * All 64 registers associated with an EPF will be reset due to a PF FLR or MAC Reset.
 * These registers are not affected by VF FLR.
 */
union bdk_sdpx_epfx_rx_vf_num
{
    uint64_t u;
    struct bdk_sdpx_epfx_rx_vf_num_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_7_63         : 57;
        uint64_t vf_num                : 7;  /**< [  6:  0](R/W) The function that the ring belongs to. If equal to 0, the ring belongs
                                                                 to the physical function.  If nonzero, this field is the virtual function
                                                                 that the ring belongs to.

                                                                 [VF_NUM] configuration must match SDP()_EPF()_RINFO configuration.

                                                                 [VF_NUM] applies to the ring pair, which includes both this input
                                                                 ring and to the output ring of the same index. */
#else /* Word 0 - Little Endian */
        uint64_t vf_num                : 7;  /**< [  6:  0](R/W) The function that the ring belongs to. If equal to 0, the ring belongs
                                                                 to the physical function.  If nonzero, this field is the virtual function
                                                                 that the ring belongs to.

                                                                 [VF_NUM] configuration must match SDP()_EPF()_RINFO configuration.

                                                                 [VF_NUM] applies to the ring pair, which includes both this input
                                                                 ring and to the output ring of the same index. */
        uint64_t reserved_7_63         : 57;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_epfx_rx_vf_num_s cn; */
};
typedef union bdk_sdpx_epfx_rx_vf_num bdk_sdpx_epfx_rx_vf_num_t;

static inline uint64_t BDK_SDPX_EPFX_RX_VF_NUM(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_EPFX_RX_VF_NUM(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1) && (c<=63)))
        return 0x874080010500ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1) + 0x20000ll * ((c) & 0x3f);
    __bdk_csr_fatal("SDPX_EPFX_RX_VF_NUM", 3, a, b, c, 0);
}

#define typedef_BDK_SDPX_EPFX_RX_VF_NUM(a,b,c) bdk_sdpx_epfx_rx_vf_num_t
#define bustype_BDK_SDPX_EPFX_RX_VF_NUM(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_EPFX_RX_VF_NUM(a,b,c) "SDPX_EPFX_RX_VF_NUM"
#define device_bar_BDK_SDPX_EPFX_RX_VF_NUM(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_EPFX_RX_VF_NUM(a,b,c) (a)
#define arguments_BDK_SDPX_EPFX_RX_VF_NUM(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sdp#_scratch#
 *
 * SDP Scratch Register
 * These registers are general purpose 64-bit scratch registers for software use.
 */
union bdk_sdpx_scratchx
{
    uint64_t u;
    struct bdk_sdpx_scratchx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) The value in this register is totally software defined. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) The value in this register is totally software defined. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_sdpx_scratchx_s cn; */
};
typedef union bdk_sdpx_scratchx bdk_sdpx_scratchx_t;

static inline uint64_t BDK_SDPX_SCRATCHX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SDPX_SCRATCHX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874080020180ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SDPX_SCRATCHX", 2, a, b, 0, 0);
}

#define typedef_BDK_SDPX_SCRATCHX(a,b) bdk_sdpx_scratchx_t
#define bustype_BDK_SDPX_SCRATCHX(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SDPX_SCRATCHX(a,b) "SDPX_SCRATCHX"
#define device_bar_BDK_SDPX_SCRATCHX(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SDPX_SCRATCHX(a,b) (a)
#define arguments_BDK_SDPX_SCRATCHX(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_bar3_addr
 *
 * SLI BAR3 Address Register
 * This register configures PEM BAR3 accesses.
 */
union bdk_slix_bar3_addr
{
    uint64_t u;
    struct bdk_slix_bar3_addr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t wvirt                 : 1;  /**< [ 63: 63](R/W) Virtual:
                                                                   0 = [RD_ADDR] is a physical addresses.
                                                                   1 = [RD_ADDR] is a virtual address. */
        uint64_t reserved_49_62        : 14;
        uint64_t rd_addr               : 30; /**< [ 48: 19](R/W) Base address for PEM BAR3 transactions that is appended to the 512KB offset.
                                                                 The reset value is the PEM base address of the EPROM,
                                                                 PEM()_EROM(). */
        uint64_t reserved_0_18         : 19;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_18         : 19;
        uint64_t rd_addr               : 30; /**< [ 48: 19](R/W) Base address for PEM BAR3 transactions that is appended to the 512KB offset.
                                                                 The reset value is the PEM base address of the EPROM,
                                                                 PEM()_EROM(). */
        uint64_t reserved_49_62        : 14;
        uint64_t wvirt                 : 1;  /**< [ 63: 63](R/W) Virtual:
                                                                   0 = [RD_ADDR] is a physical addresses.
                                                                   1 = [RD_ADDR] is a virtual address. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_bar3_addr_s cn; */
};
typedef union bdk_slix_bar3_addr bdk_slix_bar3_addr_t;

static inline uint64_t BDK_SLIX_BAR3_ADDR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_BAR3_ADDR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x874001002400ll + 0x1000000000ll * ((a) & 0x0);
    __bdk_csr_fatal("SLIX_BAR3_ADDR", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_BAR3_ADDR(a) bdk_slix_bar3_addr_t
#define bustype_BDK_SLIX_BAR3_ADDR(a) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_BAR3_ADDR(a) "SLIX_BAR3_ADDR"
#define device_bar_BDK_SLIX_BAR3_ADDR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_BAR3_ADDR(a) (a)
#define arguments_BDK_SLIX_BAR3_ADDR(a) (a),-1,-1,-1

/**
 * Register (NCB) sli#_bist_status
 *
 * SLI BIST Status Register
 * This register contains results from BIST runs of MAC's memories: 0 = pass (or BIST in
 * progress/never run), 1 = fail.
 */
union bdk_slix_bist_status
{
    uint64_t u;
    struct bdk_slix_bist_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_31_63        : 33;
        uint64_t status                : 31; /**< [ 30:  0](RO) BIST status.
                                                                 Internal:
                                                                 22 = sli_nod_nfif_bstatus.
                                                                 21 = csr_region_mem_bstatus.
                                                                 20 = sncf0_ffifo_bstatus.
                                                                 19 = sndfh0_ffifo_bstatus.
                                                                 18 = sndfl0_ffifo_bstatus.
                                                                 17 = sncf1_ffifo_bstatus.
                                                                 16 = sndfh1_ffifo_bstatus.
                                                                 15 = sndfl1_ffifo_bstatus.
                                                                 14 = sncf2_ffifo_bstatus.
                                                                 13 = sndfh2_ffifo_bstatus.
                                                                 12 = sndfl2_ffifo_bstatus.
                                                                 11 = p2n_port0_tlp_cpl_fifo_bstatus.
                                                                 10 = p2n_port0_tlp_n_fifo_bstatus.
                                                                 9 = p2n_port0_tlp_p_fifo_bstatus.
                                                                 8 = p2n_port1_tlp_cpl_fifo_bstatus.
                                                                 7 = p2n_port1_tlp_n_fifo_bstatus.
                                                                 6 = p2n_port1_tlp_p_fifo_bstatus.
                                                                 5 = p2n_port2_tlp_cpl_fifo_bstatus.
                                                                 4 = p2n_port2_tlp_n_fifo_bstatus.
                                                                 3 = p2n_port2_tlp_p_fifo_bstatus.
                                                                 2 = cpl0_fifo_bstatus.
                                                                 1 = cpl1_fifo_bstatus.
                                                                 0 = cpl2_fifo_bstatus. */
#else /* Word 0 - Little Endian */
        uint64_t status                : 31; /**< [ 30:  0](RO) BIST status.
                                                                 Internal:
                                                                 22 = sli_nod_nfif_bstatus.
                                                                 21 = csr_region_mem_bstatus.
                                                                 20 = sncf0_ffifo_bstatus.
                                                                 19 = sndfh0_ffifo_bstatus.
                                                                 18 = sndfl0_ffifo_bstatus.
                                                                 17 = sncf1_ffifo_bstatus.
                                                                 16 = sndfh1_ffifo_bstatus.
                                                                 15 = sndfl1_ffifo_bstatus.
                                                                 14 = sncf2_ffifo_bstatus.
                                                                 13 = sndfh2_ffifo_bstatus.
                                                                 12 = sndfl2_ffifo_bstatus.
                                                                 11 = p2n_port0_tlp_cpl_fifo_bstatus.
                                                                 10 = p2n_port0_tlp_n_fifo_bstatus.
                                                                 9 = p2n_port0_tlp_p_fifo_bstatus.
                                                                 8 = p2n_port1_tlp_cpl_fifo_bstatus.
                                                                 7 = p2n_port1_tlp_n_fifo_bstatus.
                                                                 6 = p2n_port1_tlp_p_fifo_bstatus.
                                                                 5 = p2n_port2_tlp_cpl_fifo_bstatus.
                                                                 4 = p2n_port2_tlp_n_fifo_bstatus.
                                                                 3 = p2n_port2_tlp_p_fifo_bstatus.
                                                                 2 = cpl0_fifo_bstatus.
                                                                 1 = cpl1_fifo_bstatus.
                                                                 0 = cpl2_fifo_bstatus. */
        uint64_t reserved_31_63        : 33;
#endif /* Word 0 - End */
    } s;
    struct bdk_slix_bist_status_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_23_63        : 41;
        uint64_t status                : 23; /**< [ 22:  0](RO) BIST status.
                                                                 Internal:
                                                                 22 = sli_nod_nfif_bstatus.
                                                                 21 = csr_region_mem_bstatus.
                                                                 20 = sncf0_ffifo_bstatus.
                                                                 19 = sndfh0_ffifo_bstatus.
                                                                 18 = sndfl0_ffifo_bstatus.
                                                                 17 = sncf1_ffifo_bstatus.
                                                                 16 = sndfh1_ffifo_bstatus.
                                                                 15 = sndfl1_ffifo_bstatus.
                                                                 14 = sncf2_ffifo_bstatus.
                                                                 13 = sndfh2_ffifo_bstatus.
                                                                 12 = sndfl2_ffifo_bstatus.
                                                                 11 = p2n_port0_tlp_cpl_fifo_bstatus.
                                                                 10 = p2n_port0_tlp_n_fifo_bstatus.
                                                                 9 = p2n_port0_tlp_p_fifo_bstatus.
                                                                 8 = p2n_port1_tlp_cpl_fifo_bstatus.
                                                                 7 = p2n_port1_tlp_n_fifo_bstatus.
                                                                 6 = p2n_port1_tlp_p_fifo_bstatus.
                                                                 5 = p2n_port2_tlp_cpl_fifo_bstatus.
                                                                 4 = p2n_port2_tlp_n_fifo_bstatus.
                                                                 3 = p2n_port2_tlp_p_fifo_bstatus.
                                                                 2 = cpl0_fifo_bstatus.
                                                                 1 = cpl1_fifo_bstatus.
                                                                 0 = cpl2_fifo_bstatus. */
#else /* Word 0 - Little Endian */
        uint64_t status                : 23; /**< [ 22:  0](RO) BIST status.
                                                                 Internal:
                                                                 22 = sli_nod_nfif_bstatus.
                                                                 21 = csr_region_mem_bstatus.
                                                                 20 = sncf0_ffifo_bstatus.
                                                                 19 = sndfh0_ffifo_bstatus.
                                                                 18 = sndfl0_ffifo_bstatus.
                                                                 17 = sncf1_ffifo_bstatus.
                                                                 16 = sndfh1_ffifo_bstatus.
                                                                 15 = sndfl1_ffifo_bstatus.
                                                                 14 = sncf2_ffifo_bstatus.
                                                                 13 = sndfh2_ffifo_bstatus.
                                                                 12 = sndfl2_ffifo_bstatus.
                                                                 11 = p2n_port0_tlp_cpl_fifo_bstatus.
                                                                 10 = p2n_port0_tlp_n_fifo_bstatus.
                                                                 9 = p2n_port0_tlp_p_fifo_bstatus.
                                                                 8 = p2n_port1_tlp_cpl_fifo_bstatus.
                                                                 7 = p2n_port1_tlp_n_fifo_bstatus.
                                                                 6 = p2n_port1_tlp_p_fifo_bstatus.
                                                                 5 = p2n_port2_tlp_cpl_fifo_bstatus.
                                                                 4 = p2n_port2_tlp_n_fifo_bstatus.
                                                                 3 = p2n_port2_tlp_p_fifo_bstatus.
                                                                 2 = cpl0_fifo_bstatus.
                                                                 1 = cpl1_fifo_bstatus.
                                                                 0 = cpl2_fifo_bstatus. */
        uint64_t reserved_23_63        : 41;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_slix_bist_status_cn81xx cn88xx; */
    struct bdk_slix_bist_status_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_31_63        : 33;
        uint64_t status                : 31; /**< [ 30:  0](RO/H) BIST status. One bit per memory, enumerated by SLI_RAMS_E. */
#else /* Word 0 - Little Endian */
        uint64_t status                : 31; /**< [ 30:  0](RO/H) BIST status. One bit per memory, enumerated by SLI_RAMS_E. */
        uint64_t reserved_31_63        : 33;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_slix_bist_status bdk_slix_bist_status_t;

static inline uint64_t BDK_SLIX_BIST_STATUS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_BIST_STATUS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x874001002180ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x874001002180ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x874001002180ll + 0x1000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_BIST_STATUS", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_BIST_STATUS(a) bdk_slix_bist_status_t
#define bustype_BDK_SLIX_BIST_STATUS(a) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_BIST_STATUS(a) "SLIX_BIST_STATUS"
#define device_bar_BDK_SLIX_BIST_STATUS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_BIST_STATUS(a) (a)
#define arguments_BDK_SLIX_BIST_STATUS(a) (a),-1,-1,-1

/**
 * Register (NCB) sli#_const
 *
 * SLI Constants Register
 * This register contains constants for software discovery.
 */
union bdk_slix_const
{
    uint64_t u;
    struct bdk_slix_const_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t pems                  : 32; /**< [ 31:  0](RO) Bit mask of which PEMs are connected to this SLI.
                                                                 If PEMs are fuse disabled they will still appear in this register.

                                                                 E.g. for a single SLI connected to PEM0, PEM1 and PEM2 is 0x7. If PEM1 is fuse
                                                                 disabled, still is 0x7, because software needs to continue to know that PEM2
                                                                 remains MAC number 2 as far as the SLI registers, e.g. SLI()_S2M_MAC()_CTL, are
                                                                 concerned. */
#else /* Word 0 - Little Endian */
        uint64_t pems                  : 32; /**< [ 31:  0](RO) Bit mask of which PEMs are connected to this SLI.
                                                                 If PEMs are fuse disabled they will still appear in this register.

                                                                 E.g. for a single SLI connected to PEM0, PEM1 and PEM2 is 0x7. If PEM1 is fuse
                                                                 disabled, still is 0x7, because software needs to continue to know that PEM2
                                                                 remains MAC number 2 as far as the SLI registers, e.g. SLI()_S2M_MAC()_CTL, are
                                                                 concerned. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_const_s cn; */
};
typedef union bdk_slix_const bdk_slix_const_t;

static inline uint64_t BDK_SLIX_CONST(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_CONST(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x874001002020ll + 0x1000000000ll * ((a) & 0x0);
    __bdk_csr_fatal("SLIX_CONST", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_CONST(a) bdk_slix_const_t
#define bustype_BDK_SLIX_CONST(a) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_CONST(a) "SLIX_CONST"
#define device_bar_BDK_SLIX_CONST(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_CONST(a) (a)
#define arguments_BDK_SLIX_CONST(a) (a),-1,-1,-1

/**
 * Register (NCB) sli#_const1
 *
 * SLI Constants Register 1
 * This register contains constants for software discovery.
 */
union bdk_slix_const1
{
    uint64_t u;
    struct bdk_slix_const1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_0_63         : 64;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_63         : 64;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_const1_s cn; */
};
typedef union bdk_slix_const1 bdk_slix_const1_t;

static inline uint64_t BDK_SLIX_CONST1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_CONST1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x874001002030ll + 0x1000000000ll * ((a) & 0x0);
    __bdk_csr_fatal("SLIX_CONST1", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_CONST1(a) bdk_slix_const1_t
#define bustype_BDK_SLIX_CONST1(a) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_CONST1(a) "SLIX_CONST1"
#define device_bar_BDK_SLIX_CONST1(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_CONST1(a) (a)
#define arguments_BDK_SLIX_CONST1(a) (a),-1,-1,-1

/**
 * Register (PEXP_NCB) sli#_data_out_cnt#
 *
 * SLI Data Out Count Register
 * This register contains the EXEC data out FIFO count and the data unload counter.
 */
union bdk_slix_data_out_cntx
{
    uint64_t u;
    struct bdk_slix_data_out_cntx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t ucnt                  : 16; /**< [ 23:  8](RO/H) FIFO unload count. This counter is incremented by 1 every time a word is removed from
                                                                 data out FIFO, whose count is shown in [FCNT]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fcnt                  : 6;  /**< [  5:  0](RO/H) FIFO data out count. Number of address data words presently buffered in the FIFO. */
#else /* Word 0 - Little Endian */
        uint64_t fcnt                  : 6;  /**< [  5:  0](RO/H) FIFO data out count. Number of address data words presently buffered in the FIFO. */
        uint64_t reserved_6_7          : 2;
        uint64_t ucnt                  : 16; /**< [ 23:  8](RO/H) FIFO unload count. This counter is incremented by 1 every time a word is removed from
                                                                 data out FIFO, whose count is shown in [FCNT]. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_data_out_cntx_s cn; */
};
typedef union bdk_slix_data_out_cntx bdk_slix_data_out_cntx_t;

static inline uint64_t BDK_SLIX_DATA_OUT_CNTX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_DATA_OUT_CNTX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=2)))
        return 0x874000001080ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=2)))
        return 0x874000001080ll + 0x1000000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_DATA_OUT_CNTX", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_DATA_OUT_CNTX(a,b) bdk_slix_data_out_cntx_t
#define bustype_BDK_SLIX_DATA_OUT_CNTX(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_DATA_OUT_CNTX(a,b) "SLIX_DATA_OUT_CNTX"
#define device_bar_BDK_SLIX_DATA_OUT_CNTX(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_DATA_OUT_CNTX(a,b) (a)
#define arguments_BDK_SLIX_DATA_OUT_CNTX(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_eco
 *
 * INTERNAL: SLI ECO Register
 */
union bdk_slix_eco
{
    uint64_t u;
    struct bdk_slix_eco_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t eco_rw                : 32; /**< [ 31:  0](R/W) Internal:
                                                                 Reserved for ECO usage. */
#else /* Word 0 - Little Endian */
        uint64_t eco_rw                : 32; /**< [ 31:  0](R/W) Internal:
                                                                 Reserved for ECO usage. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_eco_s cn; */
};
typedef union bdk_slix_eco bdk_slix_eco_t;

static inline uint64_t BDK_SLIX_ECO(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_ECO(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x874001002800ll + 0x1000000000ll * ((a) & 0x0);
    __bdk_csr_fatal("SLIX_ECO", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_ECO(a) bdk_slix_eco_t
#define bustype_BDK_SLIX_ECO(a) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_ECO(a) "SLIX_ECO"
#define device_bar_BDK_SLIX_ECO(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_ECO(a) (a)
#define arguments_BDK_SLIX_ECO(a) (a),-1,-1,-1

/**
 * Register (NCB) sli#_end_merge
 *
 * SLI End Merge Register
 * Writing this register will cause a merge to end.
 */
union bdk_slix_end_merge
{
    uint64_t u;
    struct bdk_slix_end_merge_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_0_63         : 64;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_63         : 64;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_end_merge_s cn; */
};
typedef union bdk_slix_end_merge bdk_slix_end_merge_t;

static inline uint64_t BDK_SLIX_END_MERGE(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_END_MERGE(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x874001002300ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x874001002300ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x874001002300ll + 0x1000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_END_MERGE", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_END_MERGE(a) bdk_slix_end_merge_t
#define bustype_BDK_SLIX_END_MERGE(a) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_END_MERGE(a) "SLIX_END_MERGE"
#define device_bar_BDK_SLIX_END_MERGE(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_END_MERGE(a) (a)
#define arguments_BDK_SLIX_END_MERGE(a) (a),-1,-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_data_out_cnt
 *
 * SLI Data Out Count Register
 * This register contains the EXEC data out FIFO count and the data unload counter.
 */
union bdk_slix_epfx_data_out_cnt
{
    uint64_t u;
    struct bdk_slix_epfx_data_out_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t ucnt                  : 16; /**< [ 23:  8](RO/H) FIFO unload count. This counter is incremented by 1 every time a word is removed from
                                                                 data out FIFO, whose count is shown in [FCNT]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fcnt                  : 6;  /**< [  5:  0](RO/H) FIFO data out count. Number of address data words presently buffered in the FIFO. */
#else /* Word 0 - Little Endian */
        uint64_t fcnt                  : 6;  /**< [  5:  0](RO/H) FIFO data out count. Number of address data words presently buffered in the FIFO. */
        uint64_t reserved_6_7          : 2;
        uint64_t ucnt                  : 16; /**< [ 23:  8](RO/H) FIFO unload count. This counter is incremented by 1 every time a word is removed from
                                                                 data out FIFO, whose count is shown in [FCNT]. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_data_out_cnt_s cn; */
};
typedef union bdk_slix_epfx_data_out_cnt bdk_slix_epfx_data_out_cnt_t;

static inline uint64_t BDK_SLIX_EPFX_DATA_OUT_CNT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DATA_OUT_CNT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874080028120ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_EPFX_DATA_OUT_CNT", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_DATA_OUT_CNT(a,b) bdk_slix_epfx_data_out_cnt_t
#define bustype_BDK_SLIX_EPFX_DATA_OUT_CNT(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_DATA_OUT_CNT(a,b) "SLIX_EPFX_DATA_OUT_CNT"
#define device_bar_BDK_SLIX_EPFX_DATA_OUT_CNT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DATA_OUT_CNT(a,b) (a)
#define arguments_BDK_SLIX_EPFX_DATA_OUT_CNT(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_dma_cnt#
 *
 * SLI DMA Count Registers
 * These registers contain the DMA count values.
 */
union bdk_slix_epfx_dma_cntx
{
    uint64_t u;
    struct bdk_slix_epfx_dma_cntx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t cnt                   : 32; /**< [ 31:  0](R/W/H) The DMA counter. SLI/DPI hardware subtracts the written value from
                                                                 the counter whenever software writes this CSR. SLI/DPI hardware increments this
                                                                 counter after completing an OUTBOUND or EXTERNAL-ONLY DMA instruction
                                                                 with DPI_DMA_INSTR_HDR_S[CA] set DPI_DMA_INSTR_HDR_S[CSEL] equal to this
                                                                 CSR index. These increments may cause interrupts.
                                                                 See SLI_EPF()_DMA_INT_LEVEL() and SLI_EPF()_DMA_RINT[DCNT,DTIME]. */
#else /* Word 0 - Little Endian */
        uint64_t cnt                   : 32; /**< [ 31:  0](R/W/H) The DMA counter. SLI/DPI hardware subtracts the written value from
                                                                 the counter whenever software writes this CSR. SLI/DPI hardware increments this
                                                                 counter after completing an OUTBOUND or EXTERNAL-ONLY DMA instruction
                                                                 with DPI_DMA_INSTR_HDR_S[CA] set DPI_DMA_INSTR_HDR_S[CSEL] equal to this
                                                                 CSR index. These increments may cause interrupts.
                                                                 See SLI_EPF()_DMA_INT_LEVEL() and SLI_EPF()_DMA_RINT[DCNT,DTIME]. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_dma_cntx_s cn; */
};
typedef union bdk_slix_epfx_dma_cntx bdk_slix_epfx_dma_cntx_t;

static inline uint64_t BDK_SLIX_EPFX_DMA_CNTX(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DMA_CNTX(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3) && (c<=1)))
        return 0x874080028680ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3) + 0x10ll * ((c) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_DMA_CNTX", 3, a, b, c, 0);
}

#define typedef_BDK_SLIX_EPFX_DMA_CNTX(a,b,c) bdk_slix_epfx_dma_cntx_t
#define bustype_BDK_SLIX_EPFX_DMA_CNTX(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_DMA_CNTX(a,b,c) "SLIX_EPFX_DMA_CNTX"
#define device_bar_BDK_SLIX_EPFX_DMA_CNTX(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DMA_CNTX(a,b,c) (a)
#define arguments_BDK_SLIX_EPFX_DMA_CNTX(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sli#_epf#_dma_int_level#
 *
 * SLI DMA Interrupt Level Registers
 * These registers contain the thresholds for DMA count and timer interrupts.
 */
union bdk_slix_epfx_dma_int_levelx
{
    uint64_t u;
    struct bdk_slix_epfx_dma_int_levelx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t tim                   : 32; /**< [ 63: 32](R/W) Whenever the SLI_EPF()_DMA_TIM()[TIM] timer exceeds this value,
                                                                 SLI_EPF()_DMA_RINT[DTIME\<x\>] is set. The SLI_EPF()_DMA_TIM()[TIM] timer
                                                                 increments every SLI clock whenever SLI_EPF()_DMA_CNT()[CNT] != 0, and is cleared
                                                                 when SLI_EPF()_DMA_CNT()[CNT] is written to a non zero value. */
        uint64_t cnt                   : 32; /**< [ 31:  0](R/W) Whenever SLI_EPF()_DMA_CNT()[CNT] exceeds this value, SLI_EPF()_DMA_RINT[DCNT\<x\>]
                                                                 is set. */
#else /* Word 0 - Little Endian */
        uint64_t cnt                   : 32; /**< [ 31:  0](R/W) Whenever SLI_EPF()_DMA_CNT()[CNT] exceeds this value, SLI_EPF()_DMA_RINT[DCNT\<x\>]
                                                                 is set. */
        uint64_t tim                   : 32; /**< [ 63: 32](R/W) Whenever the SLI_EPF()_DMA_TIM()[TIM] timer exceeds this value,
                                                                 SLI_EPF()_DMA_RINT[DTIME\<x\>] is set. The SLI_EPF()_DMA_TIM()[TIM] timer
                                                                 increments every SLI clock whenever SLI_EPF()_DMA_CNT()[CNT] != 0, and is cleared
                                                                 when SLI_EPF()_DMA_CNT()[CNT] is written to a non zero value. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_dma_int_levelx_s cn; */
};
typedef union bdk_slix_epfx_dma_int_levelx bdk_slix_epfx_dma_int_levelx_t;

static inline uint64_t BDK_SLIX_EPFX_DMA_INT_LEVELX(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DMA_INT_LEVELX(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3) && (c<=1)))
        return 0x874080028600ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3) + 0x10ll * ((c) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_DMA_INT_LEVELX", 3, a, b, c, 0);
}

#define typedef_BDK_SLIX_EPFX_DMA_INT_LEVELX(a,b,c) bdk_slix_epfx_dma_int_levelx_t
#define bustype_BDK_SLIX_EPFX_DMA_INT_LEVELX(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_DMA_INT_LEVELX(a,b,c) "SLIX_EPFX_DMA_INT_LEVELX"
#define device_bar_BDK_SLIX_EPFX_DMA_INT_LEVELX(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DMA_INT_LEVELX(a,b,c) (a)
#define arguments_BDK_SLIX_EPFX_DMA_INT_LEVELX(a,b,c) (a),(b),(c),-1

/**
 * Register (PEXP_NCB) sli#_epf#_dma_rint
 *
 * SLI/DPI DTIME/DCNT/DMAFI Interrupt Registers
 * These registers contain interrupts related to the DPI DMA engines.
 * The given register associated with an EPF will be reset due to a PF FLR or MAC reset.
 * These registers are not affected by VF FLR.
 */
union bdk_slix_epfx_dma_rint
{
    uint64_t u;
    struct bdk_slix_epfx_dma_rint_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t dtime                 : 2;  /**< [  5:  4](R/W1C/H) Whenever SLI_EPF()_DMA_CNT()[CNT] is not 0, the SLI_EPF()_DMA_TIM()[TIM]
                                                                 timer increments every SLI clock. [DTIME]\<x\> is set whenever
                                                                 SLI_EPF()_DMA_TIM()[TIM] \> SLI_EPF()_DMA_INT_LEVEL()[TIM].
                                                                 [DTIME]\<x\> is cleared when writing a non zero value to SLI_EPF()_DMA_CNT()[CNT]
                                                                 causing SLI_EPF()_DMA_TIM()[TIM] to clear to 0 and
                                                                 SLI_EPF()_DMA_TIM()[TIM] to fall below SLI_EPF()_DMA_INT_LEVEL()[TIM]. */
        uint64_t dcnt                  : 2;  /**< [  3:  2](R/W1C/H) [DCNT]\<x\> is set whenever SLI_EPF()_DMA_CNT()[CNT] \> SLI_EPF()_DMA_INT_LEVEL()[CNT].
                                                                 [DCNT]\<x\> is normally cleared by decreasing SLI_EPF()_DMA_CNT()[CNT]. */
        uint64_t dmafi                 : 2;  /**< [  1:  0](R/W1C/H) DMA set forced interrupts. Set by SLI/DPI after completing a DPI DMA
                                                                 Instruction with DPI_DMA_INSTR_HDR_S[FI] set. */
#else /* Word 0 - Little Endian */
        uint64_t dmafi                 : 2;  /**< [  1:  0](R/W1C/H) DMA set forced interrupts. Set by SLI/DPI after completing a DPI DMA
                                                                 Instruction with DPI_DMA_INSTR_HDR_S[FI] set. */
        uint64_t dcnt                  : 2;  /**< [  3:  2](R/W1C/H) [DCNT]\<x\> is set whenever SLI_EPF()_DMA_CNT()[CNT] \> SLI_EPF()_DMA_INT_LEVEL()[CNT].
                                                                 [DCNT]\<x\> is normally cleared by decreasing SLI_EPF()_DMA_CNT()[CNT]. */
        uint64_t dtime                 : 2;  /**< [  5:  4](R/W1C/H) Whenever SLI_EPF()_DMA_CNT()[CNT] is not 0, the SLI_EPF()_DMA_TIM()[TIM]
                                                                 timer increments every SLI clock. [DTIME]\<x\> is set whenever
                                                                 SLI_EPF()_DMA_TIM()[TIM] \> SLI_EPF()_DMA_INT_LEVEL()[TIM].
                                                                 [DTIME]\<x\> is cleared when writing a non zero value to SLI_EPF()_DMA_CNT()[CNT]
                                                                 causing SLI_EPF()_DMA_TIM()[TIM] to clear to 0 and
                                                                 SLI_EPF()_DMA_TIM()[TIM] to fall below SLI_EPF()_DMA_INT_LEVEL()[TIM]. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_dma_rint_s cn; */
};
typedef union bdk_slix_epfx_dma_rint bdk_slix_epfx_dma_rint_t;

static inline uint64_t BDK_SLIX_EPFX_DMA_RINT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DMA_RINT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874080028500ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_EPFX_DMA_RINT", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_DMA_RINT(a,b) bdk_slix_epfx_dma_rint_t
#define bustype_BDK_SLIX_EPFX_DMA_RINT(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_DMA_RINT(a,b) "SLIX_EPFX_DMA_RINT"
#define device_bar_BDK_SLIX_EPFX_DMA_RINT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DMA_RINT(a,b) (a)
#define arguments_BDK_SLIX_EPFX_DMA_RINT(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_dma_rint_ena_w1c
 *
 * SLI/DPI DTIME/DCNT/DMAFI Interrupt Remote Enable Clear Registers
 * This register clears interrupt enable bits.
 */
union bdk_slix_epfx_dma_rint_ena_w1c
{
    uint64_t u;
    struct bdk_slix_epfx_dma_rint_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t dtime                 : 2;  /**< [  5:  4](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_DMA_RINT[DTIME]. */
        uint64_t dcnt                  : 2;  /**< [  3:  2](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_DMA_RINT[DCNT]. */
        uint64_t dmafi                 : 2;  /**< [  1:  0](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_DMA_RINT[DMAFI]. */
#else /* Word 0 - Little Endian */
        uint64_t dmafi                 : 2;  /**< [  1:  0](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_DMA_RINT[DMAFI]. */
        uint64_t dcnt                  : 2;  /**< [  3:  2](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_DMA_RINT[DCNT]. */
        uint64_t dtime                 : 2;  /**< [  5:  4](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_DMA_RINT[DTIME]. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_dma_rint_ena_w1c_s cn; */
};
typedef union bdk_slix_epfx_dma_rint_ena_w1c bdk_slix_epfx_dma_rint_ena_w1c_t;

static inline uint64_t BDK_SLIX_EPFX_DMA_RINT_ENA_W1C(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DMA_RINT_ENA_W1C(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874080028540ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_EPFX_DMA_RINT_ENA_W1C", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_DMA_RINT_ENA_W1C(a,b) bdk_slix_epfx_dma_rint_ena_w1c_t
#define bustype_BDK_SLIX_EPFX_DMA_RINT_ENA_W1C(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_DMA_RINT_ENA_W1C(a,b) "SLIX_EPFX_DMA_RINT_ENA_W1C"
#define device_bar_BDK_SLIX_EPFX_DMA_RINT_ENA_W1C(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DMA_RINT_ENA_W1C(a,b) (a)
#define arguments_BDK_SLIX_EPFX_DMA_RINT_ENA_W1C(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_dma_rint_ena_w1s
 *
 * SLI/DPI DTIME/DCNT/DMAFI Interrupt Remote Enable Set Registers
 * This register sets interrupt enable bits.
 */
union bdk_slix_epfx_dma_rint_ena_w1s
{
    uint64_t u;
    struct bdk_slix_epfx_dma_rint_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t dtime                 : 2;  /**< [  5:  4](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_DMA_RINT[DTIME]. */
        uint64_t dcnt                  : 2;  /**< [  3:  2](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_DMA_RINT[DCNT]. */
        uint64_t dmafi                 : 2;  /**< [  1:  0](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_DMA_RINT[DMAFI]. */
#else /* Word 0 - Little Endian */
        uint64_t dmafi                 : 2;  /**< [  1:  0](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_DMA_RINT[DMAFI]. */
        uint64_t dcnt                  : 2;  /**< [  3:  2](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_DMA_RINT[DCNT]. */
        uint64_t dtime                 : 2;  /**< [  5:  4](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_DMA_RINT[DTIME]. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_dma_rint_ena_w1s_s cn; */
};
typedef union bdk_slix_epfx_dma_rint_ena_w1s bdk_slix_epfx_dma_rint_ena_w1s_t;

static inline uint64_t BDK_SLIX_EPFX_DMA_RINT_ENA_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DMA_RINT_ENA_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874080028550ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_EPFX_DMA_RINT_ENA_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_DMA_RINT_ENA_W1S(a,b) bdk_slix_epfx_dma_rint_ena_w1s_t
#define bustype_BDK_SLIX_EPFX_DMA_RINT_ENA_W1S(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_DMA_RINT_ENA_W1S(a,b) "SLIX_EPFX_DMA_RINT_ENA_W1S"
#define device_bar_BDK_SLIX_EPFX_DMA_RINT_ENA_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DMA_RINT_ENA_W1S(a,b) (a)
#define arguments_BDK_SLIX_EPFX_DMA_RINT_ENA_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_dma_rint_w1s
 *
 * SLI/DPI DTIME/DCNT/DMAFI Interrupt Set Registers
 * This register sets interrupt bits.
 */
union bdk_slix_epfx_dma_rint_w1s
{
    uint64_t u;
    struct bdk_slix_epfx_dma_rint_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t dtime                 : 2;  /**< [  5:  4](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_DMA_RINT[DTIME]. */
        uint64_t dcnt                  : 2;  /**< [  3:  2](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_DMA_RINT[DCNT]. */
        uint64_t dmafi                 : 2;  /**< [  1:  0](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_DMA_RINT[DMAFI]. */
#else /* Word 0 - Little Endian */
        uint64_t dmafi                 : 2;  /**< [  1:  0](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_DMA_RINT[DMAFI]. */
        uint64_t dcnt                  : 2;  /**< [  3:  2](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_DMA_RINT[DCNT]. */
        uint64_t dtime                 : 2;  /**< [  5:  4](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_DMA_RINT[DTIME]. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_dma_rint_w1s_s cn; */
};
typedef union bdk_slix_epfx_dma_rint_w1s bdk_slix_epfx_dma_rint_w1s_t;

static inline uint64_t BDK_SLIX_EPFX_DMA_RINT_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DMA_RINT_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874080028510ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_EPFX_DMA_RINT_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_DMA_RINT_W1S(a,b) bdk_slix_epfx_dma_rint_w1s_t
#define bustype_BDK_SLIX_EPFX_DMA_RINT_W1S(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_DMA_RINT_W1S(a,b) "SLIX_EPFX_DMA_RINT_W1S"
#define device_bar_BDK_SLIX_EPFX_DMA_RINT_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DMA_RINT_W1S(a,b) (a)
#define arguments_BDK_SLIX_EPFX_DMA_RINT_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_dma_tim#
 *
 * SLI DMA Timer Registers
 * These registers contain the DMA timer values.
 */
union bdk_slix_epfx_dma_timx
{
    uint64_t u;
    struct bdk_slix_epfx_dma_timx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t tim                   : 32; /**< [ 31:  0](RO/H) The DMA timer value. The timer increments when
                                                                 SLI_EPF()_DMA_CNT()[CNT]!=0 and clears when SLI_EPF()_DMA_RINT[DTIME\<x\>] is written with
                                                                 one. */
#else /* Word 0 - Little Endian */
        uint64_t tim                   : 32; /**< [ 31:  0](RO/H) The DMA timer value. The timer increments when
                                                                 SLI_EPF()_DMA_CNT()[CNT]!=0 and clears when SLI_EPF()_DMA_RINT[DTIME\<x\>] is written with
                                                                 one. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_dma_timx_s cn; */
};
typedef union bdk_slix_epfx_dma_timx bdk_slix_epfx_dma_timx_t;

static inline uint64_t BDK_SLIX_EPFX_DMA_TIMX(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DMA_TIMX(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3) && (c<=1)))
        return 0x874080028700ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3) + 0x10ll * ((c) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_DMA_TIMX", 3, a, b, c, 0);
}

#define typedef_BDK_SLIX_EPFX_DMA_TIMX(a,b,c) bdk_slix_epfx_dma_timx_t
#define bustype_BDK_SLIX_EPFX_DMA_TIMX(a,b,c) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_DMA_TIMX(a,b,c) "SLIX_EPFX_DMA_TIMX"
#define device_bar_BDK_SLIX_EPFX_DMA_TIMX(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DMA_TIMX(a,b,c) (a)
#define arguments_BDK_SLIX_EPFX_DMA_TIMX(a,b,c) (a),(b),(c),-1

/**
 * Register (NCB) sli#_epf#_dma_vf_lint
 *
 * SLI DMA Error Response VF Bit Array Registers
 * When an error response is received for a VF PP transaction read, the appropriate VF indexed
 * bit is set.  The appropriate PF should read the appropriate register.
 * These registers are only valid for PEM0 PF0 and PEM2 PF0.
 */
union bdk_slix_epfx_dma_vf_lint
{
    uint64_t u;
    struct bdk_slix_epfx_dma_vf_lint_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) When an error response is received for a VF DMA transaction read, the appropriate VF
                                                                 indexed bit is set. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) When an error response is received for a VF DMA transaction read, the appropriate VF
                                                                 indexed bit is set. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_dma_vf_lint_s cn; */
};
typedef union bdk_slix_epfx_dma_vf_lint bdk_slix_epfx_dma_vf_lint_t;

static inline uint64_t BDK_SLIX_EPFX_DMA_VF_LINT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DMA_VF_LINT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000002000ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_DMA_VF_LINT", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_DMA_VF_LINT(a,b) bdk_slix_epfx_dma_vf_lint_t
#define bustype_BDK_SLIX_EPFX_DMA_VF_LINT(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_EPFX_DMA_VF_LINT(a,b) "SLIX_EPFX_DMA_VF_LINT"
#define device_bar_BDK_SLIX_EPFX_DMA_VF_LINT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DMA_VF_LINT(a,b) (a)
#define arguments_BDK_SLIX_EPFX_DMA_VF_LINT(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_epf#_dma_vf_lint_ena_w1c
 *
 * SLI DMA Error Response VF Bit Array Local Enable Clear Registers
 * This register clears interrupt enable bits.
 */
union bdk_slix_epfx_dma_vf_lint_ena_w1c
{
    uint64_t u;
    struct bdk_slix_epfx_dma_vf_lint_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..1)_DMA_VF_LINT[VF_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..1)_DMA_VF_LINT[VF_INT]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_dma_vf_lint_ena_w1c_s cn; */
};
typedef union bdk_slix_epfx_dma_vf_lint_ena_w1c bdk_slix_epfx_dma_vf_lint_ena_w1c_t;

static inline uint64_t BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1C(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1C(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000002200ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_DMA_VF_LINT_ENA_W1C", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1C(a,b) bdk_slix_epfx_dma_vf_lint_ena_w1c_t
#define bustype_BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1C(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1C(a,b) "SLIX_EPFX_DMA_VF_LINT_ENA_W1C"
#define device_bar_BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1C(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1C(a,b) (a)
#define arguments_BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1C(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_epf#_dma_vf_lint_ena_w1s
 *
 * SLI DMA Error Response VF Bit Array Local Enable Set Registers
 * This register sets interrupt enable bits.
 */
union bdk_slix_epfx_dma_vf_lint_ena_w1s
{
    uint64_t u;
    struct bdk_slix_epfx_dma_vf_lint_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..1)_DMA_VF_LINT[VF_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..1)_DMA_VF_LINT[VF_INT]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_dma_vf_lint_ena_w1s_s cn; */
};
typedef union bdk_slix_epfx_dma_vf_lint_ena_w1s bdk_slix_epfx_dma_vf_lint_ena_w1s_t;

static inline uint64_t BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000002300ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_DMA_VF_LINT_ENA_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1S(a,b) bdk_slix_epfx_dma_vf_lint_ena_w1s_t
#define bustype_BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1S(a,b) "SLIX_EPFX_DMA_VF_LINT_ENA_W1S"
#define device_bar_BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1S(a,b) (a)
#define arguments_BDK_SLIX_EPFX_DMA_VF_LINT_ENA_W1S(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_epf#_dma_vf_lint_w1s
 *
 * SLI DMA Error Response VF Bit Array Set Registers
 * This register sets interrupt bits.
 */
union bdk_slix_epfx_dma_vf_lint_w1s
{
    uint64_t u;
    struct bdk_slix_epfx_dma_vf_lint_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SLI(0)_EPF(0..1)_DMA_VF_LINT[VF_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SLI(0)_EPF(0..1)_DMA_VF_LINT[VF_INT]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_dma_vf_lint_w1s_s cn; */
};
typedef union bdk_slix_epfx_dma_vf_lint_w1s bdk_slix_epfx_dma_vf_lint_w1s_t;

static inline uint64_t BDK_SLIX_EPFX_DMA_VF_LINT_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DMA_VF_LINT_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000002100ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_DMA_VF_LINT_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_DMA_VF_LINT_W1S(a,b) bdk_slix_epfx_dma_vf_lint_w1s_t
#define bustype_BDK_SLIX_EPFX_DMA_VF_LINT_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_EPFX_DMA_VF_LINT_W1S(a,b) "SLIX_EPFX_DMA_VF_LINT_W1S"
#define device_bar_BDK_SLIX_EPFX_DMA_VF_LINT_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DMA_VF_LINT_W1S(a,b) (a)
#define arguments_BDK_SLIX_EPFX_DMA_VF_LINT_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_dma_vf_rint
 *
 * SLI DMA Error Response VF Bit Array Registers
 * When an error response is received for a VF PP transaction read, the appropriate VF indexed
 * bit is set.  The appropriate PF should read the appropriate register.
 * The given register associated with an EPF will be reset due to a PF FLR or MAC reset.
 * These registers are not affected by VF FLR.
 * These registers are only valid for PEM0 PF0 and PEM2 PF0.
 */
union bdk_slix_epfx_dma_vf_rint
{
    uint64_t u;
    struct bdk_slix_epfx_dma_vf_rint_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) When an error response is received for a VF DMA transaction read, the appropriate VF
                                                                 indexed bit is set. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) When an error response is received for a VF DMA transaction read, the appropriate VF
                                                                 indexed bit is set. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_dma_vf_rint_s cn; */
};
typedef union bdk_slix_epfx_dma_vf_rint bdk_slix_epfx_dma_vf_rint_t;

static inline uint64_t BDK_SLIX_EPFX_DMA_VF_RINT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DMA_VF_RINT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874080028400ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_DMA_VF_RINT", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_DMA_VF_RINT(a,b) bdk_slix_epfx_dma_vf_rint_t
#define bustype_BDK_SLIX_EPFX_DMA_VF_RINT(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_DMA_VF_RINT(a,b) "SLIX_EPFX_DMA_VF_RINT"
#define device_bar_BDK_SLIX_EPFX_DMA_VF_RINT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DMA_VF_RINT(a,b) (a)
#define arguments_BDK_SLIX_EPFX_DMA_VF_RINT(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_dma_vf_rint_ena_w1c
 *
 * SLI DMA Error Response VF Bit Array Local Enable Clear Registers
 * This register clears interrupt enable bits.
 */
union bdk_slix_epfx_dma_vf_rint_ena_w1c
{
    uint64_t u;
    struct bdk_slix_epfx_dma_vf_rint_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..1)_DMA_VF_RINT[VF_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..1)_DMA_VF_RINT[VF_INT]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_dma_vf_rint_ena_w1c_s cn; */
};
typedef union bdk_slix_epfx_dma_vf_rint_ena_w1c bdk_slix_epfx_dma_vf_rint_ena_w1c_t;

static inline uint64_t BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1C(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1C(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874080028420ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_DMA_VF_RINT_ENA_W1C", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1C(a,b) bdk_slix_epfx_dma_vf_rint_ena_w1c_t
#define bustype_BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1C(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1C(a,b) "SLIX_EPFX_DMA_VF_RINT_ENA_W1C"
#define device_bar_BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1C(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1C(a,b) (a)
#define arguments_BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1C(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_dma_vf_rint_ena_w1s
 *
 * SLI DMA Error Response VF Bit Array Local Enable Set Registers
 * This register sets interrupt enable bits.
 */
union bdk_slix_epfx_dma_vf_rint_ena_w1s
{
    uint64_t u;
    struct bdk_slix_epfx_dma_vf_rint_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..1)_DMA_VF_RINT[VF_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..1)_DMA_VF_RINT[VF_INT]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_dma_vf_rint_ena_w1s_s cn; */
};
typedef union bdk_slix_epfx_dma_vf_rint_ena_w1s bdk_slix_epfx_dma_vf_rint_ena_w1s_t;

static inline uint64_t BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874080028430ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_DMA_VF_RINT_ENA_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1S(a,b) bdk_slix_epfx_dma_vf_rint_ena_w1s_t
#define bustype_BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1S(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1S(a,b) "SLIX_EPFX_DMA_VF_RINT_ENA_W1S"
#define device_bar_BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1S(a,b) (a)
#define arguments_BDK_SLIX_EPFX_DMA_VF_RINT_ENA_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_dma_vf_rint_w1s
 *
 * SLI DMA Error Response VF Bit Array Set Registers
 * This register sets interrupt bits.
 */
union bdk_slix_epfx_dma_vf_rint_w1s
{
    uint64_t u;
    struct bdk_slix_epfx_dma_vf_rint_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SLI(0)_EPF(0..1)_DMA_VF_RINT[VF_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SLI(0)_EPF(0..1)_DMA_VF_RINT[VF_INT]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_dma_vf_rint_w1s_s cn; */
};
typedef union bdk_slix_epfx_dma_vf_rint_w1s bdk_slix_epfx_dma_vf_rint_w1s_t;

static inline uint64_t BDK_SLIX_EPFX_DMA_VF_RINT_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_DMA_VF_RINT_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874080028410ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_DMA_VF_RINT_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_DMA_VF_RINT_W1S(a,b) bdk_slix_epfx_dma_vf_rint_w1s_t
#define bustype_BDK_SLIX_EPFX_DMA_VF_RINT_W1S(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_DMA_VF_RINT_W1S(a,b) "SLIX_EPFX_DMA_VF_RINT_W1S"
#define device_bar_BDK_SLIX_EPFX_DMA_VF_RINT_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_DMA_VF_RINT_W1S(a,b) (a)
#define arguments_BDK_SLIX_EPFX_DMA_VF_RINT_W1S(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_epf#_misc_lint
 *
 * SLI MAC Interrupt Summary Register
 * This register contains the different interrupt-summary bits for one MAC in the SLI.
 * This set of interrupt registers are aliased to SLI(0)_MAC(0..3)_INT_SUM.
 * SLI(0)_EPF(0..3)_MISC_LINT_W1S     aliases to SLI(0)_MAC(0..3)_INT_SUM_W1S.
 * SLI(0)_EPF(0..3)_MISC_LINT_ENA_W1C aliases to SLI(0)_MAC(0..3)_INT_ENA_W1C.
 * SLI(0)_EPF(0..3)_MISC_LINT_ENA_W1S aliases to SLI(0)_MAC(0..3)_INT_ENA_W1S.
 */
union bdk_slix_epfx_misc_lint
{
    uint64_t u;
    struct bdk_slix_epfx_misc_lint_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_7_63         : 57;
        uint64_t flr                   : 1;  /**< [  6:  6](R/W1C/H) A FLR occurred for the PF on the corresponding MAC. */
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1C/H) An error response was received for a PF DMA transaction read. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1C/H) Set when an error response is received for a PF PP transaction read. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Received unsupported N-TLP for window register from the corresponding MAC. This
                                                                 occurs when the window registers are disabled and a window register access occurs. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Received unsupported N-TLP for Bar 0 from the corresponding MAC. This occurs
                                                                 when the BAR 0 address space is disabled. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Received unsupported P-TLP for window register from the corresponding MAC. This
                                                                 occurs when the window registers are disabled and a window register access
                                                                 occurs. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Received unsupported P-TLP for Bar 0 from the corresponding MAC. This occurs
                                                                 when the BAR 0 address space is disabled. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Received unsupported P-TLP for Bar 0 from the corresponding MAC. This occurs
                                                                 when the BAR 0 address space is disabled. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Received unsupported P-TLP for window register from the corresponding MAC. This
                                                                 occurs when the window registers are disabled and a window register access
                                                                 occurs. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Received unsupported N-TLP for Bar 0 from the corresponding MAC. This occurs
                                                                 when the BAR 0 address space is disabled. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Received unsupported N-TLP for window register from the corresponding MAC. This
                                                                 occurs when the window registers are disabled and a window register access occurs. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1C/H) Set when an error response is received for a PF PP transaction read. */
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1C/H) An error response was received for a PF DMA transaction read. */
        uint64_t flr                   : 1;  /**< [  6:  6](R/W1C/H) A FLR occurred for the PF on the corresponding MAC. */
        uint64_t reserved_7_63         : 57;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_misc_lint_s cn; */
};
typedef union bdk_slix_epfx_misc_lint bdk_slix_epfx_misc_lint_t;

static inline uint64_t BDK_SLIX_EPFX_MISC_LINT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_MISC_LINT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874000002400ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_EPFX_MISC_LINT", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_MISC_LINT(a,b) bdk_slix_epfx_misc_lint_t
#define bustype_BDK_SLIX_EPFX_MISC_LINT(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_EPFX_MISC_LINT(a,b) "SLIX_EPFX_MISC_LINT"
#define device_bar_BDK_SLIX_EPFX_MISC_LINT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_MISC_LINT(a,b) (a)
#define arguments_BDK_SLIX_EPFX_MISC_LINT(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_epf#_misc_lint_ena_w1c
 *
 * SLI MAC Interrupt Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_slix_epfx_misc_lint_ena_w1c
{
    uint64_t u;
    struct bdk_slix_epfx_misc_lint_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_7_63         : 57;
        uint64_t flr                   : 1;  /**< [  6:  6](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_LINT[FLR]. */
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_LINT[DMAPF_ERR]. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_LINT[PPPF_ERR]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_LINT[UN_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_LINT[UN_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_LINT[UP_WI]. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_LINT[UP_B0]. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_LINT[UP_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_LINT[UP_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_LINT[UN_B0]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_LINT[UN_WI]. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_LINT[PPPF_ERR]. */
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_LINT[DMAPF_ERR]. */
        uint64_t flr                   : 1;  /**< [  6:  6](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_LINT[FLR]. */
        uint64_t reserved_7_63         : 57;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_misc_lint_ena_w1c_s cn; */
};
typedef union bdk_slix_epfx_misc_lint_ena_w1c bdk_slix_epfx_misc_lint_ena_w1c_t;

static inline uint64_t BDK_SLIX_EPFX_MISC_LINT_ENA_W1C(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_MISC_LINT_ENA_W1C(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874000002600ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_EPFX_MISC_LINT_ENA_W1C", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_MISC_LINT_ENA_W1C(a,b) bdk_slix_epfx_misc_lint_ena_w1c_t
#define bustype_BDK_SLIX_EPFX_MISC_LINT_ENA_W1C(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_EPFX_MISC_LINT_ENA_W1C(a,b) "SLIX_EPFX_MISC_LINT_ENA_W1C"
#define device_bar_BDK_SLIX_EPFX_MISC_LINT_ENA_W1C(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_MISC_LINT_ENA_W1C(a,b) (a)
#define arguments_BDK_SLIX_EPFX_MISC_LINT_ENA_W1C(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_epf#_misc_lint_ena_w1s
 *
 * SLI MAC Interrupt Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_slix_epfx_misc_lint_ena_w1s
{
    uint64_t u;
    struct bdk_slix_epfx_misc_lint_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_7_63         : 57;
        uint64_t flr                   : 1;  /**< [  6:  6](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_LINT[FLR]. */
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_LINT[DMAPF_ERR]. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_LINT[PPPF_ERR]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_LINT[UN_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_LINT[UN_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_LINT[UP_WI]. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_LINT[UP_B0]. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_LINT[UP_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_LINT[UP_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_LINT[UN_B0]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_LINT[UN_WI]. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_LINT[PPPF_ERR]. */
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_LINT[DMAPF_ERR]. */
        uint64_t flr                   : 1;  /**< [  6:  6](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_LINT[FLR]. */
        uint64_t reserved_7_63         : 57;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_misc_lint_ena_w1s_s cn; */
};
typedef union bdk_slix_epfx_misc_lint_ena_w1s bdk_slix_epfx_misc_lint_ena_w1s_t;

static inline uint64_t BDK_SLIX_EPFX_MISC_LINT_ENA_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_MISC_LINT_ENA_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874000002700ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_EPFX_MISC_LINT_ENA_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_MISC_LINT_ENA_W1S(a,b) bdk_slix_epfx_misc_lint_ena_w1s_t
#define bustype_BDK_SLIX_EPFX_MISC_LINT_ENA_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_EPFX_MISC_LINT_ENA_W1S(a,b) "SLIX_EPFX_MISC_LINT_ENA_W1S"
#define device_bar_BDK_SLIX_EPFX_MISC_LINT_ENA_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_MISC_LINT_ENA_W1S(a,b) (a)
#define arguments_BDK_SLIX_EPFX_MISC_LINT_ENA_W1S(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_epf#_misc_lint_w1s
 *
 * SLI MAC Interrupt Set Register
 * This register sets interrupt bits.
 */
union bdk_slix_epfx_misc_lint_w1s
{
    uint64_t u;
    struct bdk_slix_epfx_misc_lint_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_7_63         : 57;
        uint64_t flr                   : 1;  /**< [  6:  6](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_LINT[FLR]. */
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_LINT[DMAPF_ERR]. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_LINT[PPPF_ERR]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_LINT[UN_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_LINT[UN_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_LINT[UP_WI]. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_LINT[UP_B0]. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_LINT[UP_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_LINT[UP_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_LINT[UN_B0]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_LINT[UN_WI]. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_LINT[PPPF_ERR]. */
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_LINT[DMAPF_ERR]. */
        uint64_t flr                   : 1;  /**< [  6:  6](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_LINT[FLR]. */
        uint64_t reserved_7_63         : 57;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_misc_lint_w1s_s cn; */
};
typedef union bdk_slix_epfx_misc_lint_w1s bdk_slix_epfx_misc_lint_w1s_t;

static inline uint64_t BDK_SLIX_EPFX_MISC_LINT_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_MISC_LINT_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874000002500ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_EPFX_MISC_LINT_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_MISC_LINT_W1S(a,b) bdk_slix_epfx_misc_lint_w1s_t
#define bustype_BDK_SLIX_EPFX_MISC_LINT_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_EPFX_MISC_LINT_W1S(a,b) "SLIX_EPFX_MISC_LINT_W1S"
#define device_bar_BDK_SLIX_EPFX_MISC_LINT_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_MISC_LINT_W1S(a,b) (a)
#define arguments_BDK_SLIX_EPFX_MISC_LINT_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_misc_rint
 *
 * SLI MAC Interrupt Summary Register
 * This register contains the different interrupt-summary bits for one MAC in the SLI.
 * The given register associated with an EPF will be reset due to a PF FLR or MAC reset.
 * These registers are not affected by VF FLR.
 */
union bdk_slix_epfx_misc_rint
{
    uint64_t u;
    struct bdk_slix_epfx_misc_rint_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1C/H) Set when an error response is received for a PF DMA transaction read. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1C/H) Set when an error response is received for a PF PP transaction read. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Received unsupported N-TLP for window register from the corresponding MAC. This
                                                                 occurs when the window registers are disabled and a window register access
                                                                 occurs. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Received unsupported N-TLP for Bar 0 from the corresponding MAC. This occurs
                                                                 when the BAR 0 address space is disabled. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Received unsupported P-TLP for window register from the corresponding MAC. This
                                                                 occurs when the window registers are disabled and a window register access
                                                                 occurs. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Received unsupported P-TLP for Bar 0 from the corresponding MAC. This occurs
                                                                 when the BAR 0 address space is disabled. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Received unsupported P-TLP for Bar 0 from the corresponding MAC. This occurs
                                                                 when the BAR 0 address space is disabled. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Received unsupported P-TLP for window register from the corresponding MAC. This
                                                                 occurs when the window registers are disabled and a window register access
                                                                 occurs. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Received unsupported N-TLP for Bar 0 from the corresponding MAC. This occurs
                                                                 when the BAR 0 address space is disabled. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Received unsupported N-TLP for window register from the corresponding MAC. This
                                                                 occurs when the window registers are disabled and a window register access
                                                                 occurs. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1C/H) Set when an error response is received for a PF PP transaction read. */
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1C/H) Set when an error response is received for a PF DMA transaction read. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_misc_rint_s cn; */
};
typedef union bdk_slix_epfx_misc_rint bdk_slix_epfx_misc_rint_t;

static inline uint64_t BDK_SLIX_EPFX_MISC_RINT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_MISC_RINT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874080028240ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_EPFX_MISC_RINT", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_MISC_RINT(a,b) bdk_slix_epfx_misc_rint_t
#define bustype_BDK_SLIX_EPFX_MISC_RINT(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_MISC_RINT(a,b) "SLIX_EPFX_MISC_RINT"
#define device_bar_BDK_SLIX_EPFX_MISC_RINT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_MISC_RINT(a,b) (a)
#define arguments_BDK_SLIX_EPFX_MISC_RINT(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_misc_rint_ena_w1c
 *
 * SLI MAC Interrupt Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_slix_epfx_misc_rint_ena_w1c
{
    uint64_t u;
    struct bdk_slix_epfx_misc_rint_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_RINT[DMAPF_ERR]. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_RINT[PPPF_ERR]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_RINT[UN_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_RINT[UN_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_RINT[UP_WI]. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_RINT[UP_B0]. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_RINT[UP_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_RINT[UP_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_RINT[UN_B0]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_RINT[UN_WI]. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_RINT[PPPF_ERR]. */
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..3)_MISC_RINT[DMAPF_ERR]. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_misc_rint_ena_w1c_s cn; */
};
typedef union bdk_slix_epfx_misc_rint_ena_w1c bdk_slix_epfx_misc_rint_ena_w1c_t;

static inline uint64_t BDK_SLIX_EPFX_MISC_RINT_ENA_W1C(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_MISC_RINT_ENA_W1C(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874080028260ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_EPFX_MISC_RINT_ENA_W1C", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_MISC_RINT_ENA_W1C(a,b) bdk_slix_epfx_misc_rint_ena_w1c_t
#define bustype_BDK_SLIX_EPFX_MISC_RINT_ENA_W1C(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_MISC_RINT_ENA_W1C(a,b) "SLIX_EPFX_MISC_RINT_ENA_W1C"
#define device_bar_BDK_SLIX_EPFX_MISC_RINT_ENA_W1C(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_MISC_RINT_ENA_W1C(a,b) (a)
#define arguments_BDK_SLIX_EPFX_MISC_RINT_ENA_W1C(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_misc_rint_ena_w1s
 *
 * SLI MAC Interrupt Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_slix_epfx_misc_rint_ena_w1s
{
    uint64_t u;
    struct bdk_slix_epfx_misc_rint_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_RINT[DMAPF_ERR]. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_RINT[PPPF_ERR]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_RINT[UN_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_RINT[UN_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_RINT[UP_WI]. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_RINT[UP_B0]. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_RINT[UP_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_RINT[UP_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_RINT[UN_B0]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_RINT[UN_WI]. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_RINT[PPPF_ERR]. */
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..3)_MISC_RINT[DMAPF_ERR]. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_misc_rint_ena_w1s_s cn; */
};
typedef union bdk_slix_epfx_misc_rint_ena_w1s bdk_slix_epfx_misc_rint_ena_w1s_t;

static inline uint64_t BDK_SLIX_EPFX_MISC_RINT_ENA_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_MISC_RINT_ENA_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874080028270ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_EPFX_MISC_RINT_ENA_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_MISC_RINT_ENA_W1S(a,b) bdk_slix_epfx_misc_rint_ena_w1s_t
#define bustype_BDK_SLIX_EPFX_MISC_RINT_ENA_W1S(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_MISC_RINT_ENA_W1S(a,b) "SLIX_EPFX_MISC_RINT_ENA_W1S"
#define device_bar_BDK_SLIX_EPFX_MISC_RINT_ENA_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_MISC_RINT_ENA_W1S(a,b) (a)
#define arguments_BDK_SLIX_EPFX_MISC_RINT_ENA_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_misc_rint_w1s
 *
 * SLI MAC Interrupt Set Register
 * This register sets interrupt bits.
 */
union bdk_slix_epfx_misc_rint_w1s
{
    uint64_t u;
    struct bdk_slix_epfx_misc_rint_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_RINT[DMAPF_ERR]. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_RINT[PPPF_ERR]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_RINT[UN_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_RINT[UN_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_RINT[UP_WI]. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_RINT[UP_B0]. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_RINT[UP_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_RINT[UP_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_RINT[UN_B0]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_RINT[UN_WI]. */
        uint64_t pppf_err              : 1;  /**< [  4:  4](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_RINT[PPPF_ERR]. */
        uint64_t dmapf_err             : 1;  /**< [  5:  5](R/W1S/H) Reads or sets SLI(0)_EPF(0..3)_MISC_RINT[DMAPF_ERR]. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_misc_rint_w1s_s cn; */
};
typedef union bdk_slix_epfx_misc_rint_w1s bdk_slix_epfx_misc_rint_w1s_t;

static inline uint64_t BDK_SLIX_EPFX_MISC_RINT_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_MISC_RINT_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874080028250ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_EPFX_MISC_RINT_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_MISC_RINT_W1S(a,b) bdk_slix_epfx_misc_rint_w1s_t
#define bustype_BDK_SLIX_EPFX_MISC_RINT_W1S(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_MISC_RINT_W1S(a,b) "SLIX_EPFX_MISC_RINT_W1S"
#define device_bar_BDK_SLIX_EPFX_MISC_RINT_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_MISC_RINT_W1S(a,b) (a)
#define arguments_BDK_SLIX_EPFX_MISC_RINT_W1S(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_epf#_pp_vf_lint
 *
 * SLI PP Error Response VF Bit Array Registers
 * When an error response is received for a VF PP transaction read, the appropriate VF indexed
 * bit is set.  The appropriate PF should read the appropriate register.
 * These registers are only valid for PEM0 PF0 and PEM2 PF0.
 */
union bdk_slix_epfx_pp_vf_lint
{
    uint64_t u;
    struct bdk_slix_epfx_pp_vf_lint_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) When an error response is received for a VF PP transaction read, the appropriate VF
                                                                 indexed bit is set. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) When an error response is received for a VF PP transaction read, the appropriate VF
                                                                 indexed bit is set. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_pp_vf_lint_s cn; */
};
typedef union bdk_slix_epfx_pp_vf_lint bdk_slix_epfx_pp_vf_lint_t;

static inline uint64_t BDK_SLIX_EPFX_PP_VF_LINT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_PP_VF_LINT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000002800ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_PP_VF_LINT", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_PP_VF_LINT(a,b) bdk_slix_epfx_pp_vf_lint_t
#define bustype_BDK_SLIX_EPFX_PP_VF_LINT(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_EPFX_PP_VF_LINT(a,b) "SLIX_EPFX_PP_VF_LINT"
#define device_bar_BDK_SLIX_EPFX_PP_VF_LINT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_PP_VF_LINT(a,b) (a)
#define arguments_BDK_SLIX_EPFX_PP_VF_LINT(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_epf#_pp_vf_lint_ena_w1c
 *
 * SLI PP Error Response VF Bit Array Local Enable Clear Registers
 * This register clears interrupt enable bits.
 */
union bdk_slix_epfx_pp_vf_lint_ena_w1c
{
    uint64_t u;
    struct bdk_slix_epfx_pp_vf_lint_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..1)_PP_VF_LINT[VF_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..1)_PP_VF_LINT[VF_INT]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_pp_vf_lint_ena_w1c_s cn; */
};
typedef union bdk_slix_epfx_pp_vf_lint_ena_w1c bdk_slix_epfx_pp_vf_lint_ena_w1c_t;

static inline uint64_t BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1C(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1C(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000002a00ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_PP_VF_LINT_ENA_W1C", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1C(a,b) bdk_slix_epfx_pp_vf_lint_ena_w1c_t
#define bustype_BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1C(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1C(a,b) "SLIX_EPFX_PP_VF_LINT_ENA_W1C"
#define device_bar_BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1C(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1C(a,b) (a)
#define arguments_BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1C(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_epf#_pp_vf_lint_ena_w1s
 *
 * SLI PP Error Response VF Bit Array Local Enable Set Registers
 * This register sets interrupt enable bits.
 */
union bdk_slix_epfx_pp_vf_lint_ena_w1s
{
    uint64_t u;
    struct bdk_slix_epfx_pp_vf_lint_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..1)_PP_VF_LINT[VF_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..1)_PP_VF_LINT[VF_INT]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_pp_vf_lint_ena_w1s_s cn; */
};
typedef union bdk_slix_epfx_pp_vf_lint_ena_w1s bdk_slix_epfx_pp_vf_lint_ena_w1s_t;

static inline uint64_t BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000002b00ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_PP_VF_LINT_ENA_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1S(a,b) bdk_slix_epfx_pp_vf_lint_ena_w1s_t
#define bustype_BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1S(a,b) "SLIX_EPFX_PP_VF_LINT_ENA_W1S"
#define device_bar_BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1S(a,b) (a)
#define arguments_BDK_SLIX_EPFX_PP_VF_LINT_ENA_W1S(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_epf#_pp_vf_lint_w1s
 *
 * SLI PP Error Response VF Bit Array Set Registers
 * This register sets interrupt bits.
 */
union bdk_slix_epfx_pp_vf_lint_w1s
{
    uint64_t u;
    struct bdk_slix_epfx_pp_vf_lint_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SLI(0)_EPF(0..1)_PP_VF_LINT[VF_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SLI(0)_EPF(0..1)_PP_VF_LINT[VF_INT]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_pp_vf_lint_w1s_s cn; */
};
typedef union bdk_slix_epfx_pp_vf_lint_w1s bdk_slix_epfx_pp_vf_lint_w1s_t;

static inline uint64_t BDK_SLIX_EPFX_PP_VF_LINT_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_PP_VF_LINT_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x874000002900ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_PP_VF_LINT_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_PP_VF_LINT_W1S(a,b) bdk_slix_epfx_pp_vf_lint_w1s_t
#define bustype_BDK_SLIX_EPFX_PP_VF_LINT_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_EPFX_PP_VF_LINT_W1S(a,b) "SLIX_EPFX_PP_VF_LINT_W1S"
#define device_bar_BDK_SLIX_EPFX_PP_VF_LINT_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_PP_VF_LINT_W1S(a,b) (a)
#define arguments_BDK_SLIX_EPFX_PP_VF_LINT_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_pp_vf_rint
 *
 * SLI PP Error Response VF Bit Array Registers
 * When an error response is received for a VF PP transaction read, the appropriate VF indexed
 * bit is set.  The appropriate PF should read the appropriate register.
 * The given register associated with an EPF will be reset due to a PF FLR or MAC reset.
 * These registers are not affected by VF FLR.
 * These registers are only valid for PEM0 PF0 and PEM2 PF0.
 */
union bdk_slix_epfx_pp_vf_rint
{
    uint64_t u;
    struct bdk_slix_epfx_pp_vf_rint_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) When an error response is received for a VF PP transaction read, the appropriate VF
                                                                 indexed bit is set. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) When an error response is received for a VF PP transaction read, the appropriate VF
                                                                 indexed bit is set. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_pp_vf_rint_s cn; */
};
typedef union bdk_slix_epfx_pp_vf_rint bdk_slix_epfx_pp_vf_rint_t;

static inline uint64_t BDK_SLIX_EPFX_PP_VF_RINT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_PP_VF_RINT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x8740800282c0ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_PP_VF_RINT", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_PP_VF_RINT(a,b) bdk_slix_epfx_pp_vf_rint_t
#define bustype_BDK_SLIX_EPFX_PP_VF_RINT(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_PP_VF_RINT(a,b) "SLIX_EPFX_PP_VF_RINT"
#define device_bar_BDK_SLIX_EPFX_PP_VF_RINT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_PP_VF_RINT(a,b) (a)
#define arguments_BDK_SLIX_EPFX_PP_VF_RINT(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_pp_vf_rint_ena_w1c
 *
 * SLI PP Error Response VF Bit Array Remote Enable Clear Registers
 * This register clears interrupt enable bits.
 */
union bdk_slix_epfx_pp_vf_rint_ena_w1c
{
    uint64_t u;
    struct bdk_slix_epfx_pp_vf_rint_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..1)_PP_VF_RINT[VF_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for SLI(0)_EPF(0..1)_PP_VF_RINT[VF_INT]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_pp_vf_rint_ena_w1c_s cn; */
};
typedef union bdk_slix_epfx_pp_vf_rint_ena_w1c bdk_slix_epfx_pp_vf_rint_ena_w1c_t;

static inline uint64_t BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1C(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1C(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x8740800282e0ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_PP_VF_RINT_ENA_W1C", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1C(a,b) bdk_slix_epfx_pp_vf_rint_ena_w1c_t
#define bustype_BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1C(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1C(a,b) "SLIX_EPFX_PP_VF_RINT_ENA_W1C"
#define device_bar_BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1C(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1C(a,b) (a)
#define arguments_BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1C(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_pp_vf_rint_ena_w1s
 *
 * SLI PP Error Response VF Bit Array Remote Enable Set Registers
 * This register sets interrupt enable bits.
 */
union bdk_slix_epfx_pp_vf_rint_ena_w1s
{
    uint64_t u;
    struct bdk_slix_epfx_pp_vf_rint_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..1)_PP_VF_RINT[VF_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for SLI(0)_EPF(0..1)_PP_VF_RINT[VF_INT]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_pp_vf_rint_ena_w1s_s cn; */
};
typedef union bdk_slix_epfx_pp_vf_rint_ena_w1s bdk_slix_epfx_pp_vf_rint_ena_w1s_t;

static inline uint64_t BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x8740800282f0ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_PP_VF_RINT_ENA_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1S(a,b) bdk_slix_epfx_pp_vf_rint_ena_w1s_t
#define bustype_BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1S(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1S(a,b) "SLIX_EPFX_PP_VF_RINT_ENA_W1S"
#define device_bar_BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1S(a,b) (a)
#define arguments_BDK_SLIX_EPFX_PP_VF_RINT_ENA_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_pp_vf_rint_w1s
 *
 * SLI PP Error Response VF Bit Array Set Registers
 * This register sets interrupt bits.
 */
union bdk_slix_epfx_pp_vf_rint_w1s
{
    uint64_t u;
    struct bdk_slix_epfx_pp_vf_rint_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SLI(0)_EPF(0..1)_PP_VF_RINT[VF_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t vf_int                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets SLI(0)_EPF(0..1)_PP_VF_RINT[VF_INT]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_pp_vf_rint_w1s_s cn; */
};
typedef union bdk_slix_epfx_pp_vf_rint_w1s bdk_slix_epfx_pp_vf_rint_w1s_t;

static inline uint64_t BDK_SLIX_EPFX_PP_VF_RINT_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_PP_VF_RINT_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x8740800282d0ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x1);
    __bdk_csr_fatal("SLIX_EPFX_PP_VF_RINT_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_PP_VF_RINT_W1S(a,b) bdk_slix_epfx_pp_vf_rint_w1s_t
#define bustype_BDK_SLIX_EPFX_PP_VF_RINT_W1S(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_PP_VF_RINT_W1S(a,b) "SLIX_EPFX_PP_VF_RINT_W1S"
#define device_bar_BDK_SLIX_EPFX_PP_VF_RINT_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_PP_VF_RINT_W1S(a,b) (a)
#define arguments_BDK_SLIX_EPFX_PP_VF_RINT_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_epf#_scratch
 *
 * SLI Scratch Register
 * These registers are general purpose 64-bit scratch registers for software use.
 */
union bdk_slix_epfx_scratch
{
    uint64_t u;
    struct bdk_slix_epfx_scratch_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) The value in this register is totally software defined. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) The value in this register is totally software defined. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_epfx_scratch_s cn; */
};
typedef union bdk_slix_epfx_scratch bdk_slix_epfx_scratch_t;

static inline uint64_t BDK_SLIX_EPFX_SCRATCH(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_EPFX_SCRATCH(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874080028100ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_EPFX_SCRATCH", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_EPFX_SCRATCH(a,b) bdk_slix_epfx_scratch_t
#define bustype_BDK_SLIX_EPFX_SCRATCH(a,b) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_EPFX_SCRATCH(a,b) "SLIX_EPFX_SCRATCH"
#define device_bar_BDK_SLIX_EPFX_SCRATCH(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_EPFX_SCRATCH(a,b) (a)
#define arguments_BDK_SLIX_EPFX_SCRATCH(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_lmac_const0#
 *
 * SLI Logical MAC Capabilities Register 0
 * These registers along with SLI()_LMAC_CONST1() create a table of logical MAC
 * capabilities.  Each entry is 128 bits, with half the information in SLI()_LMAC_CONST0()
 * and half in SLI()_LMAC_CONST1().
 * The list ends with an entry where [V] is clear.
 *
 * Internal:
 * For CN81XX the table is as follows:
 * * SLI(0)_LMAC_CONST0/1(0) [ V=1 EP=0 IFTY=0 IFN=0 MAC=0 PF=0 EPF=0 VFS=0  RINGS=0  ].
 * * SLI(0)_LMAC_CONST0/1(1) [ V=1 EP=0 IFTY=0 IFN=1 MAC=1 PF=0 EPF=1 VFS=0  RINGS=0  ].
 * * SLI(0)_LMAC_CONST0/1(2) [ V=1 EP=0 IFTY=0 IFN=2 MAC=2 PF=0 EPF=2 VFS=0  RINGS=0  ].
 * * SLI(0)_LMAC_CONST0/1(3) [ V=0 ].
 *
 * For CN83XX the table is as follows:
 * * SLI(0)_LMAC_CONST0/1(0) [ V=1 EP=1 IFTY=0 IFN=0 MAC=0 PF=0 EPF=0 VFS=64 RINGS=64 ].
 * * SLI(0)_LMAC_CONST0/1(1) [ V=1 EP=1 IFTY=0 IFN=1 MAC=1 PF=0 EPF=2 VFS=0  RINGS=0  ].
 * * SLI(0)_LMAC_CONST0/1(2) [ V=1 EP=1 IFTY=0 IFN=2 MAC=2 PF=0 EPF=1 VFS=64 RINGS=64 ].
 * * SLI(0)_LMAC_CONST0/1(3) [ V=1 EP=1 IFTY=0 IFN=3 MAC=3 PF=0 EPF=3 VFS=0  RINGS=0  ].
 * * SLI(0)_LMAC_CONST0/1(4) [ V=0 ].
 */
union bdk_slix_lmac_const0x
{
    uint64_t u;
    struct bdk_slix_lmac_const0x_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_40_63        : 24;
        uint64_t epf                   : 8;  /**< [ 39: 32](RO) EPF number. Indicates the index number to EPF registers, e.g. the second index
                                                                 of SDP()_EPF()_MBOX_RINT. */
        uint64_t pf                    : 8;  /**< [ 31: 24](RO) Physical function number. Indicates the PF number as viewed from the external
                                                                 PCI bus. */
        uint64_t mac                   : 8;  /**< [ 23: 16](RO) Relative MAC number. Indicates the index number to MAC registers, e.g. the
                                                                 second index of SLI()_S2M_MAC()_CTL. */
        uint64_t ifn                   : 8;  /**< [ 15:  8](RO) Interface number. Indicates the physical PEM number. */
        uint64_t ifty                  : 4;  /**< [  7:  4](RO) Interface type.
                                                                 0x0 = PEM. */
        uint64_t reserved_2_3          : 2;
        uint64_t ep                    : 1;  /**< [  1:  1](RO) Endpoint.
                                                                 0 = This MAC/PF does not support endpoint mode; many registers are not
                                                                 implemented including input and output ring-based registers. MSI-X message
                                                                 generation is also not implemented.
                                                                 1 = This MAC/PF combination supports endpoint mode. */
        uint64_t v                     : 1;  /**< [  0:  0](RO) Valid entry.
                                                                 0 = Fields in this register will all be zero. This ends the list of capabilities.
                                                                 1 = Fields are valid. There will be at least one subsequent list entry. */
#else /* Word 0 - Little Endian */
        uint64_t v                     : 1;  /**< [  0:  0](RO) Valid entry.
                                                                 0 = Fields in this register will all be zero. This ends the list of capabilities.
                                                                 1 = Fields are valid. There will be at least one subsequent list entry. */
        uint64_t ep                    : 1;  /**< [  1:  1](RO) Endpoint.
                                                                 0 = This MAC/PF does not support endpoint mode; many registers are not
                                                                 implemented including input and output ring-based registers. MSI-X message
                                                                 generation is also not implemented.
                                                                 1 = This MAC/PF combination supports endpoint mode. */
        uint64_t reserved_2_3          : 2;
        uint64_t ifty                  : 4;  /**< [  7:  4](RO) Interface type.
                                                                 0x0 = PEM. */
        uint64_t ifn                   : 8;  /**< [ 15:  8](RO) Interface number. Indicates the physical PEM number. */
        uint64_t mac                   : 8;  /**< [ 23: 16](RO) Relative MAC number. Indicates the index number to MAC registers, e.g. the
                                                                 second index of SLI()_S2M_MAC()_CTL. */
        uint64_t pf                    : 8;  /**< [ 31: 24](RO) Physical function number. Indicates the PF number as viewed from the external
                                                                 PCI bus. */
        uint64_t epf                   : 8;  /**< [ 39: 32](RO) EPF number. Indicates the index number to EPF registers, e.g. the second index
                                                                 of SDP()_EPF()_MBOX_RINT. */
        uint64_t reserved_40_63        : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_lmac_const0x_s cn; */
};
typedef union bdk_slix_lmac_const0x bdk_slix_lmac_const0x_t;

static inline uint64_t BDK_SLIX_LMAC_CONST0X(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_LMAC_CONST0X(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=4)))
        return 0x874001004000ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=4)))
        return 0x874001004000ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x7);
    __bdk_csr_fatal("SLIX_LMAC_CONST0X", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_LMAC_CONST0X(a,b) bdk_slix_lmac_const0x_t
#define bustype_BDK_SLIX_LMAC_CONST0X(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_LMAC_CONST0X(a,b) "SLIX_LMAC_CONST0X"
#define device_bar_BDK_SLIX_LMAC_CONST0X(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_LMAC_CONST0X(a,b) (a)
#define arguments_BDK_SLIX_LMAC_CONST0X(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_lmac_const1#
 *
 * SLI Logical MAC Capabilities Register 1
 * See SLI()_LMAC_CONST0().
 */
union bdk_slix_lmac_const1x
{
    uint64_t u;
    struct bdk_slix_lmac_const1x_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t rings                 : 16; /**< [ 31: 16](RO) Number of rings.
                                                                 If [EP] is set then this field indicates the number of rings assigned
                                                                 to the physical function (which can also be shared with its associated
                                                                 virtual functions by means of the SLI()_EPF()_RINFO register.)
                                                                 If [EP] is clear then this field will be zero. */
        uint64_t vfs                   : 16; /**< [ 15:  0](RO) Number of virtual functions.
                                                                 The maximum number that may be programmed into SLI()_S2M_REG()_ACC2[VF]. */
#else /* Word 0 - Little Endian */
        uint64_t vfs                   : 16; /**< [ 15:  0](RO) Number of virtual functions.
                                                                 The maximum number that may be programmed into SLI()_S2M_REG()_ACC2[VF]. */
        uint64_t rings                 : 16; /**< [ 31: 16](RO) Number of rings.
                                                                 If [EP] is set then this field indicates the number of rings assigned
                                                                 to the physical function (which can also be shared with its associated
                                                                 virtual functions by means of the SLI()_EPF()_RINFO register.)
                                                                 If [EP] is clear then this field will be zero. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_lmac_const1x_s cn; */
};
typedef union bdk_slix_lmac_const1x bdk_slix_lmac_const1x_t;

static inline uint64_t BDK_SLIX_LMAC_CONST1X(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_LMAC_CONST1X(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=4)))
        return 0x874001004008ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=4)))
        return 0x874001004008ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x7);
    __bdk_csr_fatal("SLIX_LMAC_CONST1X", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_LMAC_CONST1X(a,b) bdk_slix_lmac_const1x_t
#define bustype_BDK_SLIX_LMAC_CONST1X(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_LMAC_CONST1X(a,b) "SLIX_LMAC_CONST1X"
#define device_bar_BDK_SLIX_LMAC_CONST1X(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_LMAC_CONST1X(a,b) (a)
#define arguments_BDK_SLIX_LMAC_CONST1X(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_m2s_mac#_ctl
 *
 * SLI Control Port Registers
 * This register controls the functionality of the SLI's M2S in regards to a MAC.
 * Internal:
 * In 78xx was SLI()_CTL_PORT() and SLI()_S2M_PORT()_CTL.
 */
union bdk_slix_m2s_macx_ctl
{
    uint64_t u;
    struct bdk_slix_m2s_macx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_21_63        : 43;
        uint64_t bige                  : 1;  /**< [ 20: 20](R/W) Atomics sent on NCBI will be marked as big endian.  If the link partner is
                                                                 big-endian and the processors are big-endian, this allows exchange of big-endian
                                                                 atomics without byte swapping. */
        uint64_t wait_pxfr             : 1;  /**< [ 19: 19](R/W) When set, will cause a posted TLP write from a MAC to follow the following sequence:
                                                                 (having this bit set will cut the posted-TLP performance about 50%).
                                                                 _ 1. Request the NCBI.
                                                                 _ 2. Wait for the grant and send the transfer on the NCBI.
                                                                 _ 3. Start the next posted TLP.

                                                                 For diagnostic use only. */
        uint64_t wvirt                 : 1;  /**< [ 18: 18](R/W) Write virtual:
                                                                   1 = Addresses in SLI()_WIN_WR_ADDR and SLI()_WIN_RD_ADDR are virtual addresses.
                                                                   0 = Addresses are physical addresses. */
        uint64_t dis_port              : 1;  /**< [ 17: 17](R/W1C/H) When set, the output to the MAC is disabled. This occurs when the MAC reset line
                                                                 transitions from de-asserted to asserted. Writing a 1 to this location clears this
                                                                 condition when the MAC is no longer in reset and the output to the MAC is at the beginning
                                                                 of a transfer. */
        uint64_t waitl_com             : 1;  /**< [ 16: 16](R/W) When set, causes the SLI to wait for a store done from the L2C for any
                                                                 previously sent stores, before sending additional completions to the L2C from
                                                                 the MAC.
                                                                 0 = More aggressive, higher-performance behavior. Suitable when device drivers are
                                                                 appropriately written for performance and do not assume that IO reads force all DMAs
                                                                 to be complete.
                                                                 1 = Compliant, lower-performing behavior. Enforce PCI-compliant completion
                                                                 versus posted/non-posted ordering. */
        uint64_t reserved_7_15         : 9;
        uint64_t ctlp_ro               : 1;  /**< [  6:  6](R/W) Relaxed ordering enable for completion TLPS. This permits the SLI to use the RO bit sent
                                                                 from
                                                                 the MACs. See WAITL_COM. */
        uint64_t ptlp_ro               : 1;  /**< [  5:  5](R/W) Relaxed ordering enable for posted TLPS. This permits the SLI to use the RO bit sent from
                                                                 the MACs. See WAIT_COM. */
        uint64_t wind_d                : 1;  /**< [  4:  4](R/W) Window disable. When set, disables access to the window registers from the MAC. */
        uint64_t bar0_d                : 1;  /**< [  3:  3](R/W) BAR0 disable. When set, disables access from the MAC to SLI BAR0 registers. */
        uint64_t ld_cmd                : 2;  /**< [  2:  1](R/W) When SLI issues a load command to the L2C that is to be cached, this field selects the
                                                                 type of load command to use. Un-cached loads will use LDT:
                                                                 0x0 = LDD.
                                                                 0x1 = LDI.
                                                                 0x2 = LDE.
                                                                 0x3 = LDY. */
        uint64_t wait_com              : 1;  /**< [  0:  0](R/W) Wait for commit. When set, causes the SLI to wait for a store done from the L2C before
                                                                 sending additional stores to the L2C from the MAC. The SLI requests a commit on the last
                                                                 store if more than one STORE operation is required on the NCB. Most applications will not
                                                                 notice a difference, so this bit should not be set. Setting the bit is more conservative
                                                                 on ordering, lower performance. */
#else /* Word 0 - Little Endian */
        uint64_t wait_com              : 1;  /**< [  0:  0](R/W) Wait for commit. When set, causes the SLI to wait for a store done from the L2C before
                                                                 sending additional stores to the L2C from the MAC. The SLI requests a commit on the last
                                                                 store if more than one STORE operation is required on the NCB. Most applications will not
                                                                 notice a difference, so this bit should not be set. Setting the bit is more conservative
                                                                 on ordering, lower performance. */
        uint64_t ld_cmd                : 2;  /**< [  2:  1](R/W) When SLI issues a load command to the L2C that is to be cached, this field selects the
                                                                 type of load command to use. Un-cached loads will use LDT:
                                                                 0x0 = LDD.
                                                                 0x1 = LDI.
                                                                 0x2 = LDE.
                                                                 0x3 = LDY. */
        uint64_t bar0_d                : 1;  /**< [  3:  3](R/W) BAR0 disable. When set, disables access from the MAC to SLI BAR0 registers. */
        uint64_t wind_d                : 1;  /**< [  4:  4](R/W) Window disable. When set, disables access to the window registers from the MAC. */
        uint64_t ptlp_ro               : 1;  /**< [  5:  5](R/W) Relaxed ordering enable for posted TLPS. This permits the SLI to use the RO bit sent from
                                                                 the MACs. See WAIT_COM. */
        uint64_t ctlp_ro               : 1;  /**< [  6:  6](R/W) Relaxed ordering enable for completion TLPS. This permits the SLI to use the RO bit sent
                                                                 from
                                                                 the MACs. See WAITL_COM. */
        uint64_t reserved_7_15         : 9;
        uint64_t waitl_com             : 1;  /**< [ 16: 16](R/W) When set, causes the SLI to wait for a store done from the L2C for any
                                                                 previously sent stores, before sending additional completions to the L2C from
                                                                 the MAC.
                                                                 0 = More aggressive, higher-performance behavior. Suitable when device drivers are
                                                                 appropriately written for performance and do not assume that IO reads force all DMAs
                                                                 to be complete.
                                                                 1 = Compliant, lower-performing behavior. Enforce PCI-compliant completion
                                                                 versus posted/non-posted ordering. */
        uint64_t dis_port              : 1;  /**< [ 17: 17](R/W1C/H) When set, the output to the MAC is disabled. This occurs when the MAC reset line
                                                                 transitions from de-asserted to asserted. Writing a 1 to this location clears this
                                                                 condition when the MAC is no longer in reset and the output to the MAC is at the beginning
                                                                 of a transfer. */
        uint64_t wvirt                 : 1;  /**< [ 18: 18](R/W) Write virtual:
                                                                   1 = Addresses in SLI()_WIN_WR_ADDR and SLI()_WIN_RD_ADDR are virtual addresses.
                                                                   0 = Addresses are physical addresses. */
        uint64_t wait_pxfr             : 1;  /**< [ 19: 19](R/W) When set, will cause a posted TLP write from a MAC to follow the following sequence:
                                                                 (having this bit set will cut the posted-TLP performance about 50%).
                                                                 _ 1. Request the NCBI.
                                                                 _ 2. Wait for the grant and send the transfer on the NCBI.
                                                                 _ 3. Start the next posted TLP.

                                                                 For diagnostic use only. */
        uint64_t bige                  : 1;  /**< [ 20: 20](R/W) Atomics sent on NCBI will be marked as big endian.  If the link partner is
                                                                 big-endian and the processors are big-endian, this allows exchange of big-endian
                                                                 atomics without byte swapping. */
        uint64_t reserved_21_63        : 43;
#endif /* Word 0 - End */
    } s;
    struct bdk_slix_m2s_macx_ctl_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_19_63        : 45;
        uint64_t wvirt                 : 1;  /**< [ 18: 18](R/W) Write virtual:
                                                                   1 = Addresses in SLI()_WIN_WR_ADDR and SLI()_WIN_RD_ADDR are virtual addresses.
                                                                   0 = Addresses are physical addresses. */
        uint64_t dis_port              : 1;  /**< [ 17: 17](R/W1C/H) When set, the output to the MAC is disabled. This occurs when the MAC reset line
                                                                 transitions from de-asserted to asserted. Writing a 1 to this location clears this
                                                                 condition when the MAC is no longer in reset and the output to the MAC is at the beginning
                                                                 of a transfer. */
        uint64_t waitl_com             : 1;  /**< [ 16: 16](R/W) When set, causes the SLI to wait for a store done from the L2C for any
                                                                 previously sent stores, before sending additional completions to the L2C from
                                                                 the MAC.
                                                                 0 = More aggressive, higher-performance behavior. Suitable when device drivers are
                                                                 appropriately written for performance and do not assume that IO reads force all DMAs
                                                                 to be complete.
                                                                 1 = Compliant, lower-performing behavior. Enforce PCI-compliant completion
                                                                 versus posted/non-posted ordering. */
        uint64_t reserved_7_15         : 9;
        uint64_t ctlp_ro               : 1;  /**< [  6:  6](R/W) Relaxed ordering enable for completion TLPS. This permits the SLI to use the RO bit sent
                                                                 from
                                                                 the MACs. See WAITL_COM. */
        uint64_t ptlp_ro               : 1;  /**< [  5:  5](R/W) Relaxed ordering enable for posted TLPS. This permits the SLI to use the RO bit sent from
                                                                 the MACs. See WAIT_COM. */
        uint64_t wind_d                : 1;  /**< [  4:  4](R/W) Window disable. When set, disables access to the window registers from the MAC. */
        uint64_t bar0_d                : 1;  /**< [  3:  3](R/W) BAR0 disable. When set, disables access from the MAC to SLI BAR0 registers. */
        uint64_t ld_cmd                : 2;  /**< [  2:  1](R/W) When SLI issues a load command to the L2C that is to be cached, this field selects the
                                                                 type of load command to use. Un-cached loads will use LDT:
                                                                 0x0 = LDD.
                                                                 0x1 = LDI.
                                                                 0x2 = LDE.
                                                                 0x3 = LDY. */
        uint64_t wait_com              : 1;  /**< [  0:  0](R/W) Wait for commit. When set, causes the SLI to wait for a store done from the L2C before
                                                                 sending additional stores to the L2C from the MAC. The SLI requests a commit on the last
                                                                 store if more than one STORE operation is required on the NCB. Most applications will not
                                                                 notice a difference, so this bit should not be set. Setting the bit is more conservative
                                                                 on ordering, lower performance. */
#else /* Word 0 - Little Endian */
        uint64_t wait_com              : 1;  /**< [  0:  0](R/W) Wait for commit. When set, causes the SLI to wait for a store done from the L2C before
                                                                 sending additional stores to the L2C from the MAC. The SLI requests a commit on the last
                                                                 store if more than one STORE operation is required on the NCB. Most applications will not
                                                                 notice a difference, so this bit should not be set. Setting the bit is more conservative
                                                                 on ordering, lower performance. */
        uint64_t ld_cmd                : 2;  /**< [  2:  1](R/W) When SLI issues a load command to the L2C that is to be cached, this field selects the
                                                                 type of load command to use. Un-cached loads will use LDT:
                                                                 0x0 = LDD.
                                                                 0x1 = LDI.
                                                                 0x2 = LDE.
                                                                 0x3 = LDY. */
        uint64_t bar0_d                : 1;  /**< [  3:  3](R/W) BAR0 disable. When set, disables access from the MAC to SLI BAR0 registers. */
        uint64_t wind_d                : 1;  /**< [  4:  4](R/W) Window disable. When set, disables access to the window registers from the MAC. */
        uint64_t ptlp_ro               : 1;  /**< [  5:  5](R/W) Relaxed ordering enable for posted TLPS. This permits the SLI to use the RO bit sent from
                                                                 the MACs. See WAIT_COM. */
        uint64_t ctlp_ro               : 1;  /**< [  6:  6](R/W) Relaxed ordering enable for completion TLPS. This permits the SLI to use the RO bit sent
                                                                 from
                                                                 the MACs. See WAITL_COM. */
        uint64_t reserved_7_15         : 9;
        uint64_t waitl_com             : 1;  /**< [ 16: 16](R/W) When set, causes the SLI to wait for a store done from the L2C for any
                                                                 previously sent stores, before sending additional completions to the L2C from
                                                                 the MAC.
                                                                 0 = More aggressive, higher-performance behavior. Suitable when device drivers are
                                                                 appropriately written for performance and do not assume that IO reads force all DMAs
                                                                 to be complete.
                                                                 1 = Compliant, lower-performing behavior. Enforce PCI-compliant completion
                                                                 versus posted/non-posted ordering. */
        uint64_t dis_port              : 1;  /**< [ 17: 17](R/W1C/H) When set, the output to the MAC is disabled. This occurs when the MAC reset line
                                                                 transitions from de-asserted to asserted. Writing a 1 to this location clears this
                                                                 condition when the MAC is no longer in reset and the output to the MAC is at the beginning
                                                                 of a transfer. */
        uint64_t wvirt                 : 1;  /**< [ 18: 18](R/W) Write virtual:
                                                                   1 = Addresses in SLI()_WIN_WR_ADDR and SLI()_WIN_RD_ADDR are virtual addresses.
                                                                   0 = Addresses are physical addresses. */
        uint64_t reserved_19_63        : 45;
#endif /* Word 0 - End */
    } cn88xxp1;
    /* struct bdk_slix_m2s_macx_ctl_s cn81xx; */
    /* struct bdk_slix_m2s_macx_ctl_s cn83xx; */
    struct bdk_slix_m2s_macx_ctl_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_20_63        : 44;
        uint64_t wait_pxfr             : 1;  /**< [ 19: 19](R/W) When set, will cause a posted TLP write from a MAC to follow the following sequence:
                                                                 (having this bit set will cut the posted-TLP performance about 50%).
                                                                 _ 1. Request the NCBI.
                                                                 _ 2. Wait for the grant and send the transfer on the NCBI.
                                                                 _ 3. Start the next posted TLP.

                                                                 For diagnostic use only. */
        uint64_t wvirt                 : 1;  /**< [ 18: 18](R/W) Write virtual:
                                                                   1 = Addresses in SLI()_WIN_WR_ADDR and SLI()_WIN_RD_ADDR are virtual addresses.
                                                                   0 = Addresses are physical addresses. */
        uint64_t dis_port              : 1;  /**< [ 17: 17](R/W1C/H) When set, the output to the MAC is disabled. This occurs when the MAC reset line
                                                                 transitions from de-asserted to asserted. Writing a 1 to this location clears this
                                                                 condition when the MAC is no longer in reset and the output to the MAC is at the beginning
                                                                 of a transfer. */
        uint64_t waitl_com             : 1;  /**< [ 16: 16](R/W) When set, causes the SLI to wait for a store done from the L2C for any
                                                                 previously sent stores, before sending additional completions to the L2C from
                                                                 the MAC.
                                                                 0 = More aggressive, higher-performance behavior. Suitable when device drivers are
                                                                 appropriately written for performance and do not assume that IO reads force all DMAs
                                                                 to be complete.
                                                                 1 = Compliant, lower-performing behavior. Enforce PCI-compliant completion
                                                                 versus posted/non-posted ordering. */
        uint64_t reserved_7_15         : 9;
        uint64_t ctlp_ro               : 1;  /**< [  6:  6](R/W) Relaxed ordering enable for completion TLPS. This permits the SLI to use the RO bit sent
                                                                 from
                                                                 the MACs. See WAITL_COM. */
        uint64_t ptlp_ro               : 1;  /**< [  5:  5](R/W) Relaxed ordering enable for posted TLPS. This permits the SLI to use the RO bit sent from
                                                                 the MACs. See WAIT_COM. */
        uint64_t wind_d                : 1;  /**< [  4:  4](R/W) Window disable. When set, disables access to the window registers from the MAC. */
        uint64_t bar0_d                : 1;  /**< [  3:  3](R/W) BAR0 disable. When set, disables access from the MAC to SLI BAR0 registers. */
        uint64_t ld_cmd                : 2;  /**< [  2:  1](R/W) When SLI issues a load command to the L2C that is to be cached, this field selects the
                                                                 type of load command to use. Un-cached loads will use LDT:
                                                                 0x0 = LDD.
                                                                 0x1 = LDI.
                                                                 0x2 = LDE.
                                                                 0x3 = LDY. */
        uint64_t wait_com              : 1;  /**< [  0:  0](R/W) Wait for commit. When set, causes the SLI to wait for a store done from the L2C before
                                                                 sending additional stores to the L2C from the MAC. The SLI requests a commit on the last
                                                                 store if more than one STORE operation is required on the NCB. Most applications will not
                                                                 notice a difference, so this bit should not be set. Setting the bit is more conservative
                                                                 on ordering, lower performance. */
#else /* Word 0 - Little Endian */
        uint64_t wait_com              : 1;  /**< [  0:  0](R/W) Wait for commit. When set, causes the SLI to wait for a store done from the L2C before
                                                                 sending additional stores to the L2C from the MAC. The SLI requests a commit on the last
                                                                 store if more than one STORE operation is required on the NCB. Most applications will not
                                                                 notice a difference, so this bit should not be set. Setting the bit is more conservative
                                                                 on ordering, lower performance. */
        uint64_t ld_cmd                : 2;  /**< [  2:  1](R/W) When SLI issues a load command to the L2C that is to be cached, this field selects the
                                                                 type of load command to use. Un-cached loads will use LDT:
                                                                 0x0 = LDD.
                                                                 0x1 = LDI.
                                                                 0x2 = LDE.
                                                                 0x3 = LDY. */
        uint64_t bar0_d                : 1;  /**< [  3:  3](R/W) BAR0 disable. When set, disables access from the MAC to SLI BAR0 registers. */
        uint64_t wind_d                : 1;  /**< [  4:  4](R/W) Window disable. When set, disables access to the window registers from the MAC. */
        uint64_t ptlp_ro               : 1;  /**< [  5:  5](R/W) Relaxed ordering enable for posted TLPS. This permits the SLI to use the RO bit sent from
                                                                 the MACs. See WAIT_COM. */
        uint64_t ctlp_ro               : 1;  /**< [  6:  6](R/W) Relaxed ordering enable for completion TLPS. This permits the SLI to use the RO bit sent
                                                                 from
                                                                 the MACs. See WAITL_COM. */
        uint64_t reserved_7_15         : 9;
        uint64_t waitl_com             : 1;  /**< [ 16: 16](R/W) When set, causes the SLI to wait for a store done from the L2C for any
                                                                 previously sent stores, before sending additional completions to the L2C from
                                                                 the MAC.
                                                                 0 = More aggressive, higher-performance behavior. Suitable when device drivers are
                                                                 appropriately written for performance and do not assume that IO reads force all DMAs
                                                                 to be complete.
                                                                 1 = Compliant, lower-performing behavior. Enforce PCI-compliant completion
                                                                 versus posted/non-posted ordering. */
        uint64_t dis_port              : 1;  /**< [ 17: 17](R/W1C/H) When set, the output to the MAC is disabled. This occurs when the MAC reset line
                                                                 transitions from de-asserted to asserted. Writing a 1 to this location clears this
                                                                 condition when the MAC is no longer in reset and the output to the MAC is at the beginning
                                                                 of a transfer. */
        uint64_t wvirt                 : 1;  /**< [ 18: 18](R/W) Write virtual:
                                                                   1 = Addresses in SLI()_WIN_WR_ADDR and SLI()_WIN_RD_ADDR are virtual addresses.
                                                                   0 = Addresses are physical addresses. */
        uint64_t wait_pxfr             : 1;  /**< [ 19: 19](R/W) When set, will cause a posted TLP write from a MAC to follow the following sequence:
                                                                 (having this bit set will cut the posted-TLP performance about 50%).
                                                                 _ 1. Request the NCBI.
                                                                 _ 2. Wait for the grant and send the transfer on the NCBI.
                                                                 _ 3. Start the next posted TLP.

                                                                 For diagnostic use only. */
        uint64_t reserved_20_63        : 44;
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_slix_m2s_macx_ctl bdk_slix_m2s_macx_ctl_t;

static inline uint64_t BDK_SLIX_M2S_MACX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_M2S_MACX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=2)))
        return 0x874001002100ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874001002100ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=2)))
        return 0x874001002100ll + 0x1000000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_M2S_MACX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_M2S_MACX_CTL(a,b) bdk_slix_m2s_macx_ctl_t
#define bustype_BDK_SLIX_M2S_MACX_CTL(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_M2S_MACX_CTL(a,b) "SLIX_M2S_MACX_CTL"
#define device_bar_BDK_SLIX_M2S_MACX_CTL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_M2S_MACX_CTL(a,b) (a)
#define arguments_BDK_SLIX_M2S_MACX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_mac#_int_ena_w1c
 *
 * SLI MAC Interrupt Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_slix_macx_int_ena_w1c
{
    uint64_t u;
    struct bdk_slix_macx_int_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for SLI(0..1)_MAC(0..2)_INT_SUM[UN_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for SLI(0..1)_MAC(0..2)_INT_SUM[UN_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for SLI(0..1)_MAC(0..2)_INT_SUM[UP_WI]. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for SLI(0..1)_MAC(0..2)_INT_SUM[UP_B0]. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for SLI(0..1)_MAC(0..2)_INT_SUM[UP_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for SLI(0..1)_MAC(0..2)_INT_SUM[UP_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for SLI(0..1)_MAC(0..2)_INT_SUM[UN_B0]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for SLI(0..1)_MAC(0..2)_INT_SUM[UN_WI]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    struct bdk_slix_macx_int_ena_w1c_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..2)_INT_SUM[UN_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..2)_INT_SUM[UN_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..2)_INT_SUM[UP_WI]. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..2)_INT_SUM[UP_B0]. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..2)_INT_SUM[UP_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..2)_INT_SUM[UP_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..2)_INT_SUM[UN_B0]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..2)_INT_SUM[UN_WI]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_slix_macx_int_ena_w1c_s cn88xx; */
    struct bdk_slix_macx_int_ena_w1c_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..3)_INT_SUM[UN_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..3)_INT_SUM[UN_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..3)_INT_SUM[UP_WI]. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..3)_INT_SUM[UP_B0]. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..3)_INT_SUM[UP_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..3)_INT_SUM[UP_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..3)_INT_SUM[UN_B0]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for SLI(0)_MAC(0..3)_INT_SUM[UN_WI]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_slix_macx_int_ena_w1c bdk_slix_macx_int_ena_w1c_t;

static inline uint64_t BDK_SLIX_MACX_INT_ENA_W1C(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_MACX_INT_ENA_W1C(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=2)))
        return 0x874000001200ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874000001200ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=2)))
        return 0x874000001200ll + 0x1000000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_MACX_INT_ENA_W1C", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_MACX_INT_ENA_W1C(a,b) bdk_slix_macx_int_ena_w1c_t
#define bustype_BDK_SLIX_MACX_INT_ENA_W1C(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_MACX_INT_ENA_W1C(a,b) "SLIX_MACX_INT_ENA_W1C"
#define device_bar_BDK_SLIX_MACX_INT_ENA_W1C(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_MACX_INT_ENA_W1C(a,b) (a)
#define arguments_BDK_SLIX_MACX_INT_ENA_W1C(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_mac#_int_ena_w1s
 *
 * SLI MAC Interrupt Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_slix_macx_int_ena_w1s
{
    uint64_t u;
    struct bdk_slix_macx_int_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for SLI(0..1)_MAC(0..2)_INT_SUM[UN_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for SLI(0..1)_MAC(0..2)_INT_SUM[UN_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for SLI(0..1)_MAC(0..2)_INT_SUM[UP_WI]. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for SLI(0..1)_MAC(0..2)_INT_SUM[UP_B0]. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for SLI(0..1)_MAC(0..2)_INT_SUM[UP_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for SLI(0..1)_MAC(0..2)_INT_SUM[UP_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for SLI(0..1)_MAC(0..2)_INT_SUM[UN_B0]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for SLI(0..1)_MAC(0..2)_INT_SUM[UN_WI]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    struct bdk_slix_macx_int_ena_w1s_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..2)_INT_SUM[UN_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..2)_INT_SUM[UN_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..2)_INT_SUM[UP_WI]. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..2)_INT_SUM[UP_B0]. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..2)_INT_SUM[UP_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..2)_INT_SUM[UP_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..2)_INT_SUM[UN_B0]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..2)_INT_SUM[UN_WI]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_slix_macx_int_ena_w1s_s cn88xx; */
    struct bdk_slix_macx_int_ena_w1s_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..3)_INT_SUM[UN_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..3)_INT_SUM[UN_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..3)_INT_SUM[UP_WI]. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..3)_INT_SUM[UP_B0]. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..3)_INT_SUM[UP_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..3)_INT_SUM[UP_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..3)_INT_SUM[UN_B0]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for SLI(0)_MAC(0..3)_INT_SUM[UN_WI]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_slix_macx_int_ena_w1s bdk_slix_macx_int_ena_w1s_t;

static inline uint64_t BDK_SLIX_MACX_INT_ENA_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_MACX_INT_ENA_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=2)))
        return 0x874000001280ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874000001280ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=2)))
        return 0x874000001280ll + 0x1000000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_MACX_INT_ENA_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_MACX_INT_ENA_W1S(a,b) bdk_slix_macx_int_ena_w1s_t
#define bustype_BDK_SLIX_MACX_INT_ENA_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_MACX_INT_ENA_W1S(a,b) "SLIX_MACX_INT_ENA_W1S"
#define device_bar_BDK_SLIX_MACX_INT_ENA_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_MACX_INT_ENA_W1S(a,b) (a)
#define arguments_BDK_SLIX_MACX_INT_ENA_W1S(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_mac#_int_sum
 *
 * SLI MAC Interrupt Summary Register
 * This register contains the different interrupt-summary bits for one MAC in the SLI.
 */
union bdk_slix_macx_int_sum
{
    uint64_t u;
    struct bdk_slix_macx_int_sum_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Received unsupported N-TLP for window register from MAC(0..2). This occurs when the window
                                                                 registers are disabled and a window register access occurs. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Received unsupported N-TLP for Bar 0 from MAC(0..2). This occurs when the BAR 0 address
                                                                 space is disabled. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Received unsupported P-TLP for window register from MAC(0..2). This occurs when the window
                                                                 registers are disabled and a window register access occurs. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Received unsupported P-TLP for Bar 0 from MAC(0..2). This occurs when the BAR 0 address
                                                                 space is disabled. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Received unsupported P-TLP for Bar 0 from MAC(0..2). This occurs when the BAR 0 address
                                                                 space is disabled. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Received unsupported P-TLP for window register from MAC(0..2). This occurs when the window
                                                                 registers are disabled and a window register access occurs. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Received unsupported N-TLP for Bar 0 from MAC(0..2). This occurs when the BAR 0 address
                                                                 space is disabled. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Received unsupported N-TLP for window register from MAC(0..2). This occurs when the window
                                                                 registers are disabled and a window register access occurs. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_macx_int_sum_s cn81xx; */
    /* struct bdk_slix_macx_int_sum_s cn88xx; */
    struct bdk_slix_macx_int_sum_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Received unsupported N-TLP for window register from the corresponding MAC. This
                                                                 occurs when the window registers are disabled and a window register access occurs. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Received unsupported N-TLP for Bar 0 from the corresponding MAC. This occurs
                                                                 when the BAR 0 address space is disabled. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Received unsupported P-TLP for window register from the corresponding MAC. This
                                                                 occurs when the window registers are disabled and a window register access occurs. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Received unsupported P-TLP for Bar 0 from the corresponding MAC. This occurs
                                                                 when the BAR 0 address space is disabled. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1C/H) Received unsupported P-TLP for Bar 0 from the corresponding MAC. This occurs
                                                                 when the BAR 0 address space is disabled. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1C/H) Received unsupported P-TLP for window register from the corresponding MAC. This
                                                                 occurs when the window registers are disabled and a window register access occurs. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1C/H) Received unsupported N-TLP for Bar 0 from the corresponding MAC. This occurs
                                                                 when the BAR 0 address space is disabled. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1C/H) Received unsupported N-TLP for window register from the corresponding MAC. This
                                                                 occurs when the window registers are disabled and a window register access occurs. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_slix_macx_int_sum bdk_slix_macx_int_sum_t;

static inline uint64_t BDK_SLIX_MACX_INT_SUM(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_MACX_INT_SUM(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=2)))
        return 0x874000001100ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874000001100ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=2)))
        return 0x874000001100ll + 0x1000000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_MACX_INT_SUM", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_MACX_INT_SUM(a,b) bdk_slix_macx_int_sum_t
#define bustype_BDK_SLIX_MACX_INT_SUM(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_MACX_INT_SUM(a,b) "SLIX_MACX_INT_SUM"
#define device_bar_BDK_SLIX_MACX_INT_SUM(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_MACX_INT_SUM(a,b) (a)
#define arguments_BDK_SLIX_MACX_INT_SUM(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_mac#_int_sum_w1s
 *
 * SLI MAC Interrupt Set Register
 * This register sets interrupt bits.
 */
union bdk_slix_macx_int_sum_w1s
{
    uint64_t u;
    struct bdk_slix_macx_int_sum_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets SLI(0..1)_MAC(0..2)_INT_SUM[UN_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets SLI(0..1)_MAC(0..2)_INT_SUM[UN_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets SLI(0..1)_MAC(0..2)_INT_SUM[UP_WI]. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets SLI(0..1)_MAC(0..2)_INT_SUM[UP_B0]. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets SLI(0..1)_MAC(0..2)_INT_SUM[UP_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets SLI(0..1)_MAC(0..2)_INT_SUM[UP_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets SLI(0..1)_MAC(0..2)_INT_SUM[UN_B0]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets SLI(0..1)_MAC(0..2)_INT_SUM[UN_WI]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    struct bdk_slix_macx_int_sum_w1s_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets SLI(0)_MAC(0..2)_INT_SUM[UN_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets SLI(0)_MAC(0..2)_INT_SUM[UN_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets SLI(0)_MAC(0..2)_INT_SUM[UP_WI]. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets SLI(0)_MAC(0..2)_INT_SUM[UP_B0]. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets SLI(0)_MAC(0..2)_INT_SUM[UP_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets SLI(0)_MAC(0..2)_INT_SUM[UP_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets SLI(0)_MAC(0..2)_INT_SUM[UN_B0]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets SLI(0)_MAC(0..2)_INT_SUM[UN_WI]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_slix_macx_int_sum_w1s_s cn88xx; */
    struct bdk_slix_macx_int_sum_w1s_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets SLI(0)_MAC(0..3)_INT_SUM[UN_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets SLI(0)_MAC(0..3)_INT_SUM[UN_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets SLI(0)_MAC(0..3)_INT_SUM[UP_WI]. */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets SLI(0)_MAC(0..3)_INT_SUM[UP_B0]. */
#else /* Word 0 - Little Endian */
        uint64_t up_b0                 : 1;  /**< [  0:  0](R/W1S/H) Reads or sets SLI(0)_MAC(0..3)_INT_SUM[UP_B0]. */
        uint64_t up_wi                 : 1;  /**< [  1:  1](R/W1S/H) Reads or sets SLI(0)_MAC(0..3)_INT_SUM[UP_WI]. */
        uint64_t un_b0                 : 1;  /**< [  2:  2](R/W1S/H) Reads or sets SLI(0)_MAC(0..3)_INT_SUM[UN_B0]. */
        uint64_t un_wi                 : 1;  /**< [  3:  3](R/W1S/H) Reads or sets SLI(0)_MAC(0..3)_INT_SUM[UN_WI]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_slix_macx_int_sum_w1s bdk_slix_macx_int_sum_w1s_t;

static inline uint64_t BDK_SLIX_MACX_INT_SUM_W1S(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_MACX_INT_SUM_W1S(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=2)))
        return 0x874000001180ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874000001180ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=2)))
        return 0x874000001180ll + 0x1000000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_MACX_INT_SUM_W1S", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_MACX_INT_SUM_W1S(a,b) bdk_slix_macx_int_sum_w1s_t
#define bustype_BDK_SLIX_MACX_INT_SUM_W1S(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_MACX_INT_SUM_W1S(a,b) "SLIX_MACX_INT_SUM_W1S"
#define device_bar_BDK_SLIX_MACX_INT_SUM_W1S(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_MACX_INT_SUM_W1S(a,b) (a)
#define arguments_BDK_SLIX_MACX_INT_SUM_W1S(a,b) (a),(b),-1,-1

/**
 * Register (PEXP) sli#_mac_number
 *
 * SLI MAC Number Register
 * When read from a MAC, this register returns the MAC's port number, otherwise returns zero.
 */
union bdk_slix_mac_number
{
    uint64_t u;
    struct bdk_slix_mac_number_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t chip_rev              : 8;  /**< [ 31: 24](RO/H) Chip revision. See MIO_FUS_DAT2[CHIP_ID]. */
        uint64_t reserved_20_23        : 4;
        uint64_t oci_id                : 4;  /**< [ 19: 16](RO) The CCPI node ID. */
        uint64_t reserved_9_15         : 7;
        uint64_t a_mode                : 1;  /**< [  8:  8](RO/H) Trusted mode.  See RST_BOOT[TRUSTED_MODE]. */
        uint64_t num                   : 8;  /**< [  7:  0](RO/H) MAC number. */
#else /* Word 0 - Little Endian */
        uint64_t num                   : 8;  /**< [  7:  0](RO/H) MAC number. */
        uint64_t a_mode                : 1;  /**< [  8:  8](RO/H) Trusted mode.  See RST_BOOT[TRUSTED_MODE]. */
        uint64_t reserved_9_15         : 7;
        uint64_t oci_id                : 4;  /**< [ 19: 16](RO) The CCPI node ID. */
        uint64_t reserved_20_23        : 4;
        uint64_t chip_rev              : 8;  /**< [ 31: 24](RO/H) Chip revision. See MIO_FUS_DAT2[CHIP_ID]. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_mac_number_s cn; */
};
typedef union bdk_slix_mac_number bdk_slix_mac_number_t;

static inline uint64_t BDK_SLIX_MAC_NUMBER(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_MAC_NUMBER(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x80ll + 0x10000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x80ll + 0x10000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_MAC_NUMBER", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_MAC_NUMBER(a) bdk_slix_mac_number_t
#define bustype_BDK_SLIX_MAC_NUMBER(a) BDK_CSR_TYPE_PEXP
#define basename_BDK_SLIX_MAC_NUMBER(a) "SLIX_MAC_NUMBER"
#define busnum_BDK_SLIX_MAC_NUMBER(a) (a)
#define arguments_BDK_SLIX_MAC_NUMBER(a) (a),-1,-1,-1

/**
 * Register (PEXP) sli#_mac_number#
 *
 * SLI MAC Number Register
 * When read from a MAC, this register returns the MAC's port number; otherwise returns zero.
 */
union bdk_slix_mac_numberx
{
    uint64_t u;
    struct bdk_slix_mac_numberx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t chip_rev              : 8;  /**< [ 31: 24](RO/H) Chip revision. See MIO_FUS_DAT2[CHIP_ID]. */
        uint64_t reserved_20_23        : 4;
        uint64_t oci_id                : 4;  /**< [ 19: 16](RO/H) The CCPI node ID. */
        uint64_t reserved_9_15         : 7;
        uint64_t a_mode                : 1;  /**< [  8:  8](RO/H) Trusted mode.  See RST_BOOT[TRUSTED_MODE]. */
        uint64_t num                   : 8;  /**< [  7:  0](RO/H) MAC number. */
#else /* Word 0 - Little Endian */
        uint64_t num                   : 8;  /**< [  7:  0](RO/H) MAC number. */
        uint64_t a_mode                : 1;  /**< [  8:  8](RO/H) Trusted mode.  See RST_BOOT[TRUSTED_MODE]. */
        uint64_t reserved_9_15         : 7;
        uint64_t oci_id                : 4;  /**< [ 19: 16](RO/H) The CCPI node ID. */
        uint64_t reserved_20_23        : 4;
        uint64_t chip_rev              : 8;  /**< [ 31: 24](RO/H) Chip revision. See MIO_FUS_DAT2[CHIP_ID]. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_mac_numberx_s cn; */
};
typedef union bdk_slix_mac_numberx bdk_slix_mac_numberx_t;

static inline uint64_t BDK_SLIX_MAC_NUMBERX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_MAC_NUMBERX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x2c050ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_MAC_NUMBERX", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_MAC_NUMBERX(a,b) bdk_slix_mac_numberx_t
#define bustype_BDK_SLIX_MAC_NUMBERX(a,b) BDK_CSR_TYPE_PEXP
#define basename_BDK_SLIX_MAC_NUMBERX(a,b) "SLIX_MAC_NUMBERX"
#define busnum_BDK_SLIX_MAC_NUMBERX(a,b) (a)
#define arguments_BDK_SLIX_MAC_NUMBERX(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_mbe_int_ena_w1c
 *
 * SLI Interrupt Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_slix_mbe_int_ena_w1c
{
    uint64_t u;
    struct bdk_slix_mbe_int_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_0_63         : 64;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_63         : 64;
#endif /* Word 0 - End */
    } s;
    struct bdk_slix_mbe_int_ena_w1c_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_54_63        : 10;
        uint64_t sed0_dbe              : 22; /**< [ 53: 32](R/W1C/H) Reads or clears enable for SLI(0)_MBE_INT_SUM[SED0_DBE]. */
        uint64_t reserved_22_31        : 10;
        uint64_t sed0_sbe              : 22; /**< [ 21:  0](R/W1C/H) Reads or clears enable for SLI(0)_MBE_INT_SUM[SED0_SBE]. */
#else /* Word 0 - Little Endian */
        uint64_t sed0_sbe              : 22; /**< [ 21:  0](R/W1C/H) Reads or clears enable for SLI(0)_MBE_INT_SUM[SED0_SBE]. */
        uint64_t reserved_22_31        : 10;
        uint64_t sed0_dbe              : 22; /**< [ 53: 32](R/W1C/H) Reads or clears enable for SLI(0)_MBE_INT_SUM[SED0_DBE]. */
        uint64_t reserved_54_63        : 10;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_slix_mbe_int_ena_w1c_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_54_63        : 10;
        uint64_t sed0_dbe              : 22; /**< [ 53: 32](R/W1C/H) Reads or clears enable for SLI(0..1)_MBE_INT_SUM[SED0_DBE]. */
        uint64_t reserved_22_31        : 10;
        uint64_t sed0_sbe              : 22; /**< [ 21:  0](R/W1C/H) Reads or clears enable for SLI(0..1)_MBE_INT_SUM[SED0_SBE]. */
#else /* Word 0 - Little Endian */
        uint64_t sed0_sbe              : 22; /**< [ 21:  0](R/W1C/H) Reads or clears enable for SLI(0..1)_MBE_INT_SUM[SED0_SBE]. */
        uint64_t reserved_22_31        : 10;
        uint64_t sed0_dbe              : 22; /**< [ 53: 32](R/W1C/H) Reads or clears enable for SLI(0..1)_MBE_INT_SUM[SED0_DBE]. */
        uint64_t reserved_54_63        : 10;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_slix_mbe_int_ena_w1c_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1C/H) Reads or clears enable for SLI(0)_MBE_INT_SUM[DBE]. */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1C/H) Reads or clears enable for SLI(0)_MBE_INT_SUM[SBE]. */
#else /* Word 0 - Little Endian */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1C/H) Reads or clears enable for SLI(0)_MBE_INT_SUM[SBE]. */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1C/H) Reads or clears enable for SLI(0)_MBE_INT_SUM[DBE]. */
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_slix_mbe_int_ena_w1c bdk_slix_mbe_int_ena_w1c_t;

static inline uint64_t BDK_SLIX_MBE_INT_ENA_W1C(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_MBE_INT_ENA_W1C(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x874001002260ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x874001002260ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x874001002260ll + 0x1000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_MBE_INT_ENA_W1C", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_MBE_INT_ENA_W1C(a) bdk_slix_mbe_int_ena_w1c_t
#define bustype_BDK_SLIX_MBE_INT_ENA_W1C(a) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_MBE_INT_ENA_W1C(a) "SLIX_MBE_INT_ENA_W1C"
#define device_bar_BDK_SLIX_MBE_INT_ENA_W1C(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_MBE_INT_ENA_W1C(a) (a)
#define arguments_BDK_SLIX_MBE_INT_ENA_W1C(a) (a),-1,-1,-1

/**
 * Register (NCB) sli#_mbe_int_ena_w1s
 *
 * SLI Interrupt Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_slix_mbe_int_ena_w1s
{
    uint64_t u;
    struct bdk_slix_mbe_int_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_0_63         : 64;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_63         : 64;
#endif /* Word 0 - End */
    } s;
    struct bdk_slix_mbe_int_ena_w1s_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_54_63        : 10;
        uint64_t sed0_dbe              : 22; /**< [ 53: 32](R/W1S/H) Reads or sets enable for SLI(0)_MBE_INT_SUM[SED0_DBE]. */
        uint64_t reserved_22_31        : 10;
        uint64_t sed0_sbe              : 22; /**< [ 21:  0](R/W1S/H) Reads or sets enable for SLI(0)_MBE_INT_SUM[SED0_SBE]. */
#else /* Word 0 - Little Endian */
        uint64_t sed0_sbe              : 22; /**< [ 21:  0](R/W1S/H) Reads or sets enable for SLI(0)_MBE_INT_SUM[SED0_SBE]. */
        uint64_t reserved_22_31        : 10;
        uint64_t sed0_dbe              : 22; /**< [ 53: 32](R/W1S/H) Reads or sets enable for SLI(0)_MBE_INT_SUM[SED0_DBE]. */
        uint64_t reserved_54_63        : 10;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_slix_mbe_int_ena_w1s_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_54_63        : 10;
        uint64_t sed0_dbe              : 22; /**< [ 53: 32](R/W1S/H) Reads or sets enable for SLI(0..1)_MBE_INT_SUM[SED0_DBE]. */
        uint64_t reserved_22_31        : 10;
        uint64_t sed0_sbe              : 22; /**< [ 21:  0](R/W1S/H) Reads or sets enable for SLI(0..1)_MBE_INT_SUM[SED0_SBE]. */
#else /* Word 0 - Little Endian */
        uint64_t sed0_sbe              : 22; /**< [ 21:  0](R/W1S/H) Reads or sets enable for SLI(0..1)_MBE_INT_SUM[SED0_SBE]. */
        uint64_t reserved_22_31        : 10;
        uint64_t sed0_dbe              : 22; /**< [ 53: 32](R/W1S/H) Reads or sets enable for SLI(0..1)_MBE_INT_SUM[SED0_DBE]. */
        uint64_t reserved_54_63        : 10;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_slix_mbe_int_ena_w1s_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1S/H) Reads or sets enable for SLI(0)_MBE_INT_SUM[DBE]. */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1S/H) Reads or sets enable for SLI(0)_MBE_INT_SUM[SBE]. */
#else /* Word 0 - Little Endian */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1S/H) Reads or sets enable for SLI(0)_MBE_INT_SUM[SBE]. */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1S/H) Reads or sets enable for SLI(0)_MBE_INT_SUM[DBE]. */
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_slix_mbe_int_ena_w1s bdk_slix_mbe_int_ena_w1s_t;

static inline uint64_t BDK_SLIX_MBE_INT_ENA_W1S(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_MBE_INT_ENA_W1S(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x874001002280ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x874001002280ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x874001002280ll + 0x1000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_MBE_INT_ENA_W1S", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_MBE_INT_ENA_W1S(a) bdk_slix_mbe_int_ena_w1s_t
#define bustype_BDK_SLIX_MBE_INT_ENA_W1S(a) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_MBE_INT_ENA_W1S(a) "SLIX_MBE_INT_ENA_W1S"
#define device_bar_BDK_SLIX_MBE_INT_ENA_W1S(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_MBE_INT_ENA_W1S(a) (a)
#define arguments_BDK_SLIX_MBE_INT_ENA_W1S(a) (a),-1,-1,-1

/**
 * Register (NCB) sli#_mbe_int_sum
 *
 * SLI MBE Interrupt Summary Register
 * This register contains the MBE interrupt-summary bits of the SLI.
 */
union bdk_slix_mbe_int_sum
{
    uint64_t u;
    struct bdk_slix_mbe_int_sum_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_0_63         : 64;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_63         : 64;
#endif /* Word 0 - End */
    } s;
    struct bdk_slix_mbe_int_sum_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_54_63        : 10;
        uint64_t sed0_dbe              : 22; /**< [ 53: 32](R/W1C/H) SED0 double-bit error. When set, a SED0 double-bit error has occurred. */
        uint64_t reserved_22_31        : 10;
        uint64_t sed0_sbe              : 22; /**< [ 21:  0](R/W1C/H) SED0 single-bit error. When set, a SED0 single-bit error has occurred. */
#else /* Word 0 - Little Endian */
        uint64_t sed0_sbe              : 22; /**< [ 21:  0](R/W1C/H) SED0 single-bit error. When set, a SED0 single-bit error has occurred. */
        uint64_t reserved_22_31        : 10;
        uint64_t sed0_dbe              : 22; /**< [ 53: 32](R/W1C/H) SED0 double-bit error. When set, a SED0 double-bit error has occurred. */
        uint64_t reserved_54_63        : 10;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_slix_mbe_int_sum_cn81xx cn88xx; */
    struct bdk_slix_mbe_int_sum_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1C/H) Double-bit error detected in internal RAM. One bit per memory, enumerated by
                                                                 SLI_RAMS_E. */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1C/H) Single-bit error detected in internal RAM. One bit per memory, enumerated by
                                                                 SLI_RAMS_E. */
#else /* Word 0 - Little Endian */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1C/H) Single-bit error detected in internal RAM. One bit per memory, enumerated by
                                                                 SLI_RAMS_E. */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1C/H) Double-bit error detected in internal RAM. One bit per memory, enumerated by
                                                                 SLI_RAMS_E. */
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_slix_mbe_int_sum bdk_slix_mbe_int_sum_t;

static inline uint64_t BDK_SLIX_MBE_INT_SUM(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_MBE_INT_SUM(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x874001002220ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x874001002220ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x874001002220ll + 0x1000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_MBE_INT_SUM", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_MBE_INT_SUM(a) bdk_slix_mbe_int_sum_t
#define bustype_BDK_SLIX_MBE_INT_SUM(a) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_MBE_INT_SUM(a) "SLIX_MBE_INT_SUM"
#define device_bar_BDK_SLIX_MBE_INT_SUM(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_MBE_INT_SUM(a) (a)
#define arguments_BDK_SLIX_MBE_INT_SUM(a) (a),-1,-1,-1

/**
 * Register (NCB) sli#_mbe_int_sum_w1s
 *
 * SLI Interrupt Set Register
 * This register sets interrupt bits.
 */
union bdk_slix_mbe_int_sum_w1s
{
    uint64_t u;
    struct bdk_slix_mbe_int_sum_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_0_63         : 64;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_63         : 64;
#endif /* Word 0 - End */
    } s;
    struct bdk_slix_mbe_int_sum_w1s_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_54_63        : 10;
        uint64_t sed0_dbe              : 22; /**< [ 53: 32](R/W1S/H) Reads or sets SLI(0)_MBE_INT_SUM[SED0_DBE]. */
        uint64_t reserved_22_31        : 10;
        uint64_t sed0_sbe              : 22; /**< [ 21:  0](R/W1S/H) Reads or sets SLI(0)_MBE_INT_SUM[SED0_SBE]. */
#else /* Word 0 - Little Endian */
        uint64_t sed0_sbe              : 22; /**< [ 21:  0](R/W1S/H) Reads or sets SLI(0)_MBE_INT_SUM[SED0_SBE]. */
        uint64_t reserved_22_31        : 10;
        uint64_t sed0_dbe              : 22; /**< [ 53: 32](R/W1S/H) Reads or sets SLI(0)_MBE_INT_SUM[SED0_DBE]. */
        uint64_t reserved_54_63        : 10;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_slix_mbe_int_sum_w1s_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_54_63        : 10;
        uint64_t sed0_dbe              : 22; /**< [ 53: 32](R/W1S/H) Reads or sets SLI(0..1)_MBE_INT_SUM[SED0_DBE]. */
        uint64_t reserved_22_31        : 10;
        uint64_t sed0_sbe              : 22; /**< [ 21:  0](R/W1S/H) Reads or sets SLI(0..1)_MBE_INT_SUM[SED0_SBE]. */
#else /* Word 0 - Little Endian */
        uint64_t sed0_sbe              : 22; /**< [ 21:  0](R/W1S/H) Reads or sets SLI(0..1)_MBE_INT_SUM[SED0_SBE]. */
        uint64_t reserved_22_31        : 10;
        uint64_t sed0_dbe              : 22; /**< [ 53: 32](R/W1S/H) Reads or sets SLI(0..1)_MBE_INT_SUM[SED0_DBE]. */
        uint64_t reserved_54_63        : 10;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_slix_mbe_int_sum_w1s_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1S/H) Reads or sets SLI(0)_MBE_INT_SUM[DBE]. */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1S/H) Reads or sets SLI(0)_MBE_INT_SUM[SBE]. */
#else /* Word 0 - Little Endian */
        uint64_t sbe                   : 32; /**< [ 31:  0](R/W1S/H) Reads or sets SLI(0)_MBE_INT_SUM[SBE]. */
        uint64_t dbe                   : 32; /**< [ 63: 32](R/W1S/H) Reads or sets SLI(0)_MBE_INT_SUM[DBE]. */
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_slix_mbe_int_sum_w1s bdk_slix_mbe_int_sum_w1s_t;

static inline uint64_t BDK_SLIX_MBE_INT_SUM_W1S(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_MBE_INT_SUM_W1S(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x874001002240ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x874001002240ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x874001002240ll + 0x1000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_MBE_INT_SUM_W1S", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_MBE_INT_SUM_W1S(a) bdk_slix_mbe_int_sum_w1s_t
#define bustype_BDK_SLIX_MBE_INT_SUM_W1S(a) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_MBE_INT_SUM_W1S(a) "SLIX_MBE_INT_SUM_W1S"
#define device_bar_BDK_SLIX_MBE_INT_SUM_W1S(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_MBE_INT_SUM_W1S(a) (a)
#define arguments_BDK_SLIX_MBE_INT_SUM_W1S(a) (a),-1,-1,-1

/**
 * Register (NCB) sli#_mem_ctl
 *
 * SLI Memory Control Register
 * This register controls the ECC of the SLI memories.
 */
union bdk_slix_mem_ctl
{
    uint64_t u;
    struct bdk_slix_mem_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_0_63         : 64;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_63         : 64;
#endif /* Word 0 - End */
    } s;
    struct bdk_slix_mem_ctl_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_30_63        : 34;
        uint64_t ctl                   : 30; /**< [ 29:  0](R/W) Control memory ECC functionality.
                                                                 \<29\>    = Correction disable for csr_region_mem_csr_cor_dis.
                                                                 \<28:29\> = Flip syndrome for csr_region_mem_csr_flip_synd.

                                                                 \<26\>    = Correction disable for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_cor_dis.
                                                                 \<25:24\> = Flip syndrome for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_flip_synd.
                                                                 \<23\>    = Correction disable for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_cor_dis.
                                                                 \<22:21\> = Flip syndrome for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_flip_synd.
                                                                 \<20\>    = Correction disable for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_cor_dis.
                                                                 \<19:18\> = Flip syndrome for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_flip_synd.

                                                                 \<17\>    = Correction disable for cpl0_fifo_csr_cor_dis.
                                                                 \<16:15\> = Flip syndrome for cpl0_fifo_csr_flip_synd.
                                                                 \<14\>    = Correction disable for cpl1_fifo_csr_cor_dis.
                                                                 \<13:12\> = Flip syndrome for cpl1_fifo_csr_flip_synd.
                                                                 \<11\>    = Correction disable for cpl2_fifo_csr_cor_dis.
                                                                 \<10:9\>  = Flip syndrome for cpl2_fifo_csr_flip_synd.

                                                                 \<8\>   = Correction disable for p2n0_tlp_\<p, n, cpl\>_fifo.
                                                                 \<7:6\> = Flip syndrome for p2n0_tlp_\<p,n,cpl\>_fifo.
                                                                 \<5\>   = Correction disable for p2n1_tlp_\<p, n, cpl\>_fifo.
                                                                 \<4:3\> = Flip syndrome for p2n1_tlp_\<p,n,cpl\>_fifo.
                                                                 \<2\>   = Correction disable for p2n2_tlp_\<p, n, cpl\>_fifo.
                                                                 \<1:0\> = Flip syndrome for p2n2_tlp_\<p,n,cpl\>_fifo. */
#else /* Word 0 - Little Endian */
        uint64_t ctl                   : 30; /**< [ 29:  0](R/W) Control memory ECC functionality.
                                                                 \<29\>    = Correction disable for csr_region_mem_csr_cor_dis.
                                                                 \<28:29\> = Flip syndrome for csr_region_mem_csr_flip_synd.

                                                                 \<26\>    = Correction disable for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_cor_dis.
                                                                 \<25:24\> = Flip syndrome for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_flip_synd.
                                                                 \<23\>    = Correction disable for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_cor_dis.
                                                                 \<22:21\> = Flip syndrome for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_flip_synd.
                                                                 \<20\>    = Correction disable for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_cor_dis.
                                                                 \<19:18\> = Flip syndrome for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_flip_synd.

                                                                 \<17\>    = Correction disable for cpl0_fifo_csr_cor_dis.
                                                                 \<16:15\> = Flip syndrome for cpl0_fifo_csr_flip_synd.
                                                                 \<14\>    = Correction disable for cpl1_fifo_csr_cor_dis.
                                                                 \<13:12\> = Flip syndrome for cpl1_fifo_csr_flip_synd.
                                                                 \<11\>    = Correction disable for cpl2_fifo_csr_cor_dis.
                                                                 \<10:9\>  = Flip syndrome for cpl2_fifo_csr_flip_synd.

                                                                 \<8\>   = Correction disable for p2n0_tlp_\<p, n, cpl\>_fifo.
                                                                 \<7:6\> = Flip syndrome for p2n0_tlp_\<p,n,cpl\>_fifo.
                                                                 \<5\>   = Correction disable for p2n1_tlp_\<p, n, cpl\>_fifo.
                                                                 \<4:3\> = Flip syndrome for p2n1_tlp_\<p,n,cpl\>_fifo.
                                                                 \<2\>   = Correction disable for p2n2_tlp_\<p, n, cpl\>_fifo.
                                                                 \<1:0\> = Flip syndrome for p2n2_tlp_\<p,n,cpl\>_fifo. */
        uint64_t reserved_30_63        : 34;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_slix_mem_ctl_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_30_63        : 34;
        uint64_t ctl                   : 30; /**< [ 29:  0](R/W) Control memory ECC functionality.
                                                                 \<29\>    = Correction Disable for csr_region_mem_csr_cor_dis.
                                                                 \<28:29\> = Flip Syndrome for csr_region_mem_csr_flip_synd.

                                                                 \<26\>    = Correction Disable for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_cor_dis.
                                                                 \<25:24\> = Flip Syndrome for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_flip_synd.
                                                                 \<23\>    = Correction Disable for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_cor_dis.
                                                                 \<22:21\> = Flip Syndrome for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_flip_synd.
                                                                 \<20\>    = Correction Disable for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_cor_dis.
                                                                 \<19:18\> = Flip Syndrome for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_flip_synd.

                                                                 \<17\>    = Correction Disable for cpl0_fifo_csr_cor_dis.
                                                                 \<16:15\> = Flip Syndrome for cpl0_fifo_csr_flip_synd.
                                                                 \<14\>    = Correction Disable for cpl1_fifo_csr_cor_dis.
                                                                 \<13:12\> = Flip Syndrome for cpl1_fifo_csr_flip_synd.
                                                                 \<11\>    = Correction Disable for cpl2_fifo_csr_cor_dis.
                                                                 \<10:9\>  = Flip Syndrome for cpl2_fifo_csr_flip_synd.

                                                                 \<8\>   = Correction Disable for p2n0_tlp_\<p, n, cpl\>_fifo.
                                                                 \<7:6\> = Flip Syndrome for p2n0_tlp_\<p,n,cpl\>_fifo.
                                                                 \<5\>   = Correction Disable for p2n1_tlp_\<p, n, cpl\>_fifo.
                                                                 \<4:3\> = Flip Syndrome for p2n1_tlp_\<p,n,cpl\>_fifo.
                                                                 \<2\>   = Correction Disable for p2n2_tlp_\<p, n, cpl\>_fifo.
                                                                 \<1:0\> = Flip Syndrome for p2n2_tlp_\<p,n,cpl\>_fifo. */
#else /* Word 0 - Little Endian */
        uint64_t ctl                   : 30; /**< [ 29:  0](R/W) Control memory ECC functionality.
                                                                 \<29\>    = Correction Disable for csr_region_mem_csr_cor_dis.
                                                                 \<28:29\> = Flip Syndrome for csr_region_mem_csr_flip_synd.

                                                                 \<26\>    = Correction Disable for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_cor_dis.
                                                                 \<25:24\> = Flip Syndrome for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_flip_synd.
                                                                 \<23\>    = Correction Disable for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_cor_dis.
                                                                 \<22:21\> = Flip Syndrome for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_flip_synd.
                                                                 \<20\>    = Correction Disable for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_cor_dis.
                                                                 \<19:18\> = Flip Syndrome for sndf\<h,l\>2_ffifo, sncf2_ffifo_csr_flip_synd.

                                                                 \<17\>    = Correction Disable for cpl0_fifo_csr_cor_dis.
                                                                 \<16:15\> = Flip Syndrome for cpl0_fifo_csr_flip_synd.
                                                                 \<14\>    = Correction Disable for cpl1_fifo_csr_cor_dis.
                                                                 \<13:12\> = Flip Syndrome for cpl1_fifo_csr_flip_synd.
                                                                 \<11\>    = Correction Disable for cpl2_fifo_csr_cor_dis.
                                                                 \<10:9\>  = Flip Syndrome for cpl2_fifo_csr_flip_synd.

                                                                 \<8\>   = Correction Disable for p2n0_tlp_\<p, n, cpl\>_fifo.
                                                                 \<7:6\> = Flip Syndrome for p2n0_tlp_\<p,n,cpl\>_fifo.
                                                                 \<5\>   = Correction Disable for p2n1_tlp_\<p, n, cpl\>_fifo.
                                                                 \<4:3\> = Flip Syndrome for p2n1_tlp_\<p,n,cpl\>_fifo.
                                                                 \<2\>   = Correction Disable for p2n2_tlp_\<p, n, cpl\>_fifo.
                                                                 \<1:0\> = Flip Syndrome for p2n2_tlp_\<p,n,cpl\>_fifo. */
        uint64_t reserved_30_63        : 34;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_slix_mem_ctl_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t cdis                  : 32; /**< [ 31:  0](R/W) Disables ECC correction on each RAM.  Bit positions enumerated with SLI_RAMS_E. */
#else /* Word 0 - Little Endian */
        uint64_t cdis                  : 32; /**< [ 31:  0](R/W) Disables ECC correction on each RAM.  Bit positions enumerated with SLI_RAMS_E. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_slix_mem_ctl bdk_slix_mem_ctl_t;

static inline uint64_t BDK_SLIX_MEM_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_MEM_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x874001002200ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x874001002200ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x874001002200ll + 0x1000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_MEM_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_MEM_CTL(a) bdk_slix_mem_ctl_t
#define bustype_BDK_SLIX_MEM_CTL(a) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_MEM_CTL(a) "SLIX_MEM_CTL"
#define device_bar_BDK_SLIX_MEM_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_MEM_CTL(a) (a)
#define arguments_BDK_SLIX_MEM_CTL(a) (a),-1,-1,-1

/**
 * Register (NCB) sli#_mem_flip
 *
 * SLI ECC Control Register
 * This register controls the ECC of the SLI memories.
 */
union bdk_slix_mem_flip
{
    uint64_t u;
    struct bdk_slix_mem_flip_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t flip1                 : 32; /**< [ 63: 32](R/W) Flips syndrome bit 1 on writes.  Bit positions enumerated with SLI_RAMS_E. */
        uint64_t flip0                 : 32; /**< [ 31:  0](R/W) Flips syndrome bit 0 on writes.  Bit positions enumerated with SLI_RAMS_E. */
#else /* Word 0 - Little Endian */
        uint64_t flip0                 : 32; /**< [ 31:  0](R/W) Flips syndrome bit 0 on writes.  Bit positions enumerated with SLI_RAMS_E. */
        uint64_t flip1                 : 32; /**< [ 63: 32](R/W) Flips syndrome bit 1 on writes.  Bit positions enumerated with SLI_RAMS_E. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_mem_flip_s cn; */
};
typedef union bdk_slix_mem_flip bdk_slix_mem_flip_t;

static inline uint64_t BDK_SLIX_MEM_FLIP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_MEM_FLIP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x874001002210ll + 0x1000000000ll * ((a) & 0x0);
    __bdk_csr_fatal("SLIX_MEM_FLIP", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_MEM_FLIP(a) bdk_slix_mem_flip_t
#define bustype_BDK_SLIX_MEM_FLIP(a) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_MEM_FLIP(a) "SLIX_MEM_FLIP"
#define device_bar_BDK_SLIX_MEM_FLIP(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_MEM_FLIP(a) (a)
#define arguments_BDK_SLIX_MEM_FLIP(a) (a),-1,-1,-1

/**
 * Register (NCB) sli#_msix_pba#
 *
 * SLI MSI-X Pending Bit Array Registers
 * This register is the MSI-X PBA table; the bit number is indexed by the SLI_INT_VEC_E enumeration.
 */
union bdk_slix_msix_pbax
{
    uint64_t u;
    struct bdk_slix_msix_pbax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO/H) Pending message for the associated SLI_MSIX_VEC()_CTL, enumerated by SLI_INT_VEC_E. Bits
                                                                 that have no associated SLI_INT_VEC_E are 0. */
#else /* Word 0 - Little Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO/H) Pending message for the associated SLI_MSIX_VEC()_CTL, enumerated by SLI_INT_VEC_E. Bits
                                                                 that have no associated SLI_INT_VEC_E are 0. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_msix_pbax_s cn; */
};
typedef union bdk_slix_msix_pbax bdk_slix_msix_pbax_t;

static inline uint64_t BDK_SLIX_MSIX_PBAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_MSIX_PBAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b==0)))
        return 0x8740100f0000ll + 0x1000000000ll * ((a) & 0x0) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b==0)))
        return 0x874c000f0000ll + 0x1000000000ll * ((a) & 0x0) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b==0)))
        return 0x8740100f0000ll + 0x1000000000ll * ((a) & 0x1) + 8ll * ((b) & 0x0);
    __bdk_csr_fatal("SLIX_MSIX_PBAX", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_MSIX_PBAX(a,b) bdk_slix_msix_pbax_t
#define bustype_BDK_SLIX_MSIX_PBAX(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_MSIX_PBAX(a,b) "SLIX_MSIX_PBAX"
#define device_bar_BDK_SLIX_MSIX_PBAX(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_SLIX_MSIX_PBAX(a,b) (a)
#define arguments_BDK_SLIX_MSIX_PBAX(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_msix_vec#_addr
 *
 * SLI MSI-X Vector-Table Address Register
 * This register is the MSI-X vector table, indexed by the SLI_INT_VEC_E enumeration.
 */
union bdk_slix_msix_vecx_addr
{
    uint64_t u;
    struct bdk_slix_msix_vecx_addr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's SLI_MSIX_VEC()_ADDR, SLI_MSIX_VEC()_CTL, and corresponding
                                                                 bit of SLI_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_SLI_VSEC_SCTL[MSIX_SEC] (for documentation, see PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is
                                                                 set, all vectors are secure and function as if [SECVEC] was set. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's SLI_MSIX_VEC()_ADDR, SLI_MSIX_VEC()_CTL, and corresponding
                                                                 bit of SLI_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_SLI_VSEC_SCTL[MSIX_SEC] (for documentation, see PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is
                                                                 set, all vectors are secure and function as if [SECVEC] was set. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_msix_vecx_addr_s cn; */
};
typedef union bdk_slix_msix_vecx_addr bdk_slix_msix_vecx_addr_t;

static inline uint64_t BDK_SLIX_MSIX_VECX_ADDR(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_MSIX_VECX_ADDR(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=3)))
        return 0x874010000000ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=16)))
        return 0x874c00000000ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=3)))
        return 0x874010000000ll + 0x1000000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_MSIX_VECX_ADDR", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_MSIX_VECX_ADDR(a,b) bdk_slix_msix_vecx_addr_t
#define bustype_BDK_SLIX_MSIX_VECX_ADDR(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_MSIX_VECX_ADDR(a,b) "SLIX_MSIX_VECX_ADDR"
#define device_bar_BDK_SLIX_MSIX_VECX_ADDR(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_SLIX_MSIX_VECX_ADDR(a,b) (a)
#define arguments_BDK_SLIX_MSIX_VECX_ADDR(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_msix_vec#_ctl
 *
 * SLI MSI-X Vector-Table Control and Data Register
 * This register is the MSI-X vector table, indexed by the SLI_INT_VEC_E enumeration.
 */
union bdk_slix_msix_vecx_ctl
{
    uint64_t u;
    struct bdk_slix_msix_vecx_ctl_s
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
    } s;
    /* struct bdk_slix_msix_vecx_ctl_s cn; */
};
typedef union bdk_slix_msix_vecx_ctl bdk_slix_msix_vecx_ctl_t;

static inline uint64_t BDK_SLIX_MSIX_VECX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_MSIX_VECX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=3)))
        return 0x874010000008ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=16)))
        return 0x874c00000008ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x1f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=3)))
        return 0x874010000008ll + 0x1000000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_MSIX_VECX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_MSIX_VECX_CTL(a,b) bdk_slix_msix_vecx_ctl_t
#define bustype_BDK_SLIX_MSIX_VECX_CTL(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_MSIX_VECX_CTL(a,b) "SLIX_MSIX_VECX_CTL"
#define device_bar_BDK_SLIX_MSIX_VECX_CTL(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_SLIX_MSIX_VECX_CTL(a,b) (a)
#define arguments_BDK_SLIX_MSIX_VECX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_s2m_ctl
 *
 * SLI S2M Control Register
 * This register contains control functionality of the S2M attached to the SLI. This register
 * impacts all MACs attached to the S2M.
 */
union bdk_slix_s2m_ctl
{
    uint64_t u;
    struct bdk_slix_s2m_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t rd_flt                : 1;  /**< [ 14: 14](R/W) Read fault.
                                                                 0 = A PCIe non-config read which is terminated by PCIe with an error (UR, etc) will return
                                                                 to the NCB/cores all-ones and non-fault.

                                                                 1 = A PCIe non-config read which is terminated by PCIe with an error (UR, etc) will return
                                                                 to the NCB/cores all-zeros and fault.  In the case of a read by a core, this fault will
                                                                 cause an synchronous external abort in the core.

                                                                 Config reads which are terminated by PCIe in with an error (UR, etc), or config reads when
                                                                 the PEM is disabled or link is down, will return to the NCB/cores all-ones and non-fault
                                                                 regardless of this bit. */
        uint64_t max_word              : 4;  /**< [ 13: 10](R/W) Maximum number of words. Specifies the maximum number of 8-byte words to merge into a
                                                                 single write operation from the cores to the MAC. Legal values are 1 to 8, with 0 treated
                                                                 as 16. */
        uint64_t timer                 : 10; /**< [  9:  0](R/W) Merge timer. When the SLI starts a core-to-MAC write, TIMER specifies the maximum wait, in
                                                                 coprocessor-clock cycles, to merge additional write operations from the cores into one
                                                                 large write. The values for this field range from 1 to 1024, with 0 treated as 1024. */
#else /* Word 0 - Little Endian */
        uint64_t timer                 : 10; /**< [  9:  0](R/W) Merge timer. When the SLI starts a core-to-MAC write, TIMER specifies the maximum wait, in
                                                                 coprocessor-clock cycles, to merge additional write operations from the cores into one
                                                                 large write. The values for this field range from 1 to 1024, with 0 treated as 1024. */
        uint64_t max_word              : 4;  /**< [ 13: 10](R/W) Maximum number of words. Specifies the maximum number of 8-byte words to merge into a
                                                                 single write operation from the cores to the MAC. Legal values are 1 to 8, with 0 treated
                                                                 as 16. */
        uint64_t rd_flt                : 1;  /**< [ 14: 14](R/W) Read fault.
                                                                 0 = A PCIe non-config read which is terminated by PCIe with an error (UR, etc) will return
                                                                 to the NCB/cores all-ones and non-fault.

                                                                 1 = A PCIe non-config read which is terminated by PCIe with an error (UR, etc) will return
                                                                 to the NCB/cores all-zeros and fault.  In the case of a read by a core, this fault will
                                                                 cause an synchronous external abort in the core.

                                                                 Config reads which are terminated by PCIe in with an error (UR, etc), or config reads when
                                                                 the PEM is disabled or link is down, will return to the NCB/cores all-ones and non-fault
                                                                 regardless of this bit. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    struct bdk_slix_s2m_ctl_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t max_word              : 4;  /**< [ 13: 10](R/W) Maximum number of words. Specifies the maximum number of 8-byte words to merge into a
                                                                 single write operation from the cores to the MAC. Legal values are 1 to 8, with 0 treated
                                                                 as 16. */
        uint64_t timer                 : 10; /**< [  9:  0](R/W) Merge timer. When the SLI starts a core-to-MAC write, TIMER specifies the maximum wait, in
                                                                 coprocessor-clock cycles, to merge additional write operations from the cores into one
                                                                 large write. The values for this field range from 1 to 1024, with 0 treated as 1024. */
#else /* Word 0 - Little Endian */
        uint64_t timer                 : 10; /**< [  9:  0](R/W) Merge timer. When the SLI starts a core-to-MAC write, TIMER specifies the maximum wait, in
                                                                 coprocessor-clock cycles, to merge additional write operations from the cores into one
                                                                 large write. The values for this field range from 1 to 1024, with 0 treated as 1024. */
        uint64_t max_word              : 4;  /**< [ 13: 10](R/W) Maximum number of words. Specifies the maximum number of 8-byte words to merge into a
                                                                 single write operation from the cores to the MAC. Legal values are 1 to 8, with 0 treated
                                                                 as 16. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } cn88xxp1;
    /* struct bdk_slix_s2m_ctl_s cn81xx; */
    /* struct bdk_slix_s2m_ctl_s cn83xx; */
    struct bdk_slix_s2m_ctl_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t rd_flt                : 1;  /**< [ 14: 14](R/W) Read fault.
                                                                 0 = A PCIe non-config read which is terminated by PCIe with an error (UR, etc) will return
                                                                 to the NCB/cores all-ones and non-fault.  This is compatible with CN88XX pass 1.0.

                                                                 1 = A PCIe non-config read which is terminated by PCIe with an error (UR, etc) will return
                                                                 to the NCB/cores all-zeros and fault.  In the case of a read by a core, this fault will
                                                                 cause an synchronous external abort in the core.

                                                                 Config reads which are terminated by PCIe in with an error (UR, etc), or config reads when
                                                                 the PEM is disabled or link is down, will return to the NCB/cores all-ones and non-fault
                                                                 regardless of this bit. */
        uint64_t max_word              : 4;  /**< [ 13: 10](R/W) Maximum number of words. Specifies the maximum number of 8-byte words to merge into a
                                                                 single write operation from the cores to the MAC. Legal values are 1 to 8, with 0 treated
                                                                 as 16. */
        uint64_t timer                 : 10; /**< [  9:  0](R/W) Merge timer. When the SLI starts a core-to-MAC write, TIMER specifies the maximum wait, in
                                                                 coprocessor-clock cycles, to merge additional write operations from the cores into one
                                                                 large write. The values for this field range from 1 to 1024, with 0 treated as 1024. */
#else /* Word 0 - Little Endian */
        uint64_t timer                 : 10; /**< [  9:  0](R/W) Merge timer. When the SLI starts a core-to-MAC write, TIMER specifies the maximum wait, in
                                                                 coprocessor-clock cycles, to merge additional write operations from the cores into one
                                                                 large write. The values for this field range from 1 to 1024, with 0 treated as 1024. */
        uint64_t max_word              : 4;  /**< [ 13: 10](R/W) Maximum number of words. Specifies the maximum number of 8-byte words to merge into a
                                                                 single write operation from the cores to the MAC. Legal values are 1 to 8, with 0 treated
                                                                 as 16. */
        uint64_t rd_flt                : 1;  /**< [ 14: 14](R/W) Read fault.
                                                                 0 = A PCIe non-config read which is terminated by PCIe with an error (UR, etc) will return
                                                                 to the NCB/cores all-ones and non-fault.  This is compatible with CN88XX pass 1.0.

                                                                 1 = A PCIe non-config read which is terminated by PCIe with an error (UR, etc) will return
                                                                 to the NCB/cores all-zeros and fault.  In the case of a read by a core, this fault will
                                                                 cause an synchronous external abort in the core.

                                                                 Config reads which are terminated by PCIe in with an error (UR, etc), or config reads when
                                                                 the PEM is disabled or link is down, will return to the NCB/cores all-ones and non-fault
                                                                 regardless of this bit. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_slix_s2m_ctl bdk_slix_s2m_ctl_t;

static inline uint64_t BDK_SLIX_S2M_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_S2M_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x874001002000ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x874001002000ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x874001002000ll + 0x1000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_S2M_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_S2M_CTL(a) bdk_slix_s2m_ctl_t
#define bustype_BDK_SLIX_S2M_CTL(a) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_S2M_CTL(a) "SLIX_S2M_CTL"
#define device_bar_BDK_SLIX_S2M_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_S2M_CTL(a) (a)
#define arguments_BDK_SLIX_S2M_CTL(a) (a),-1,-1,-1

/**
 * Register (NCB) sli#_s2m_mac#_ctl
 *
 * SLI MAC Control Register
 * This register controls the functionality of the SLI's S2M in regards to a MAC.
 * Internal:
 * In 78xx was SLI()_CTL_STATUS and SLI()_MAC_CREDIT_CNT.
 */
union bdk_slix_s2m_macx_ctl
{
    uint64_t u;
    struct bdk_slix_s2m_macx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t ccnt                  : 8;  /**< [ 31: 24](R/W/H) CPL-TLP FIFO credits. Legal values are 0x25 to 0x80. For diagnostic use only. */
        uint64_t ncnt                  : 8;  /**< [ 23: 16](R/W/H) NP-TLP FIFO credits. Legal values are 0x5 to 0x10. For diagnostic use only. */
        uint64_t pcnt                  : 8;  /**< [ 15:  8](R/W/H) P-TLP FIFO credits. Legal values are 0x25 to 0x80. For diagnostic use only. */
        uint64_t tags                  : 8;  /**< [  7:  0](R/W/H) Number of tags available for MAC.
                                                                 One tag is needed for each outbound TLP that requires a CPL TLP.
                                                                 This field should only be written as part of a reset sequence and before issuing any read
                                                                 operations, CFGs, or I/O transactions from the core(s). For diagnostic use only.
                                                                 Legal values are 1 to 32. */
#else /* Word 0 - Little Endian */
        uint64_t tags                  : 8;  /**< [  7:  0](R/W/H) Number of tags available for MAC.
                                                                 One tag is needed for each outbound TLP that requires a CPL TLP.
                                                                 This field should only be written as part of a reset sequence and before issuing any read
                                                                 operations, CFGs, or I/O transactions from the core(s). For diagnostic use only.
                                                                 Legal values are 1 to 32. */
        uint64_t pcnt                  : 8;  /**< [ 15:  8](R/W/H) P-TLP FIFO credits. Legal values are 0x25 to 0x80. For diagnostic use only. */
        uint64_t ncnt                  : 8;  /**< [ 23: 16](R/W/H) NP-TLP FIFO credits. Legal values are 0x5 to 0x10. For diagnostic use only. */
        uint64_t ccnt                  : 8;  /**< [ 31: 24](R/W/H) CPL-TLP FIFO credits. Legal values are 0x25 to 0x80. For diagnostic use only. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_s2m_macx_ctl_s cn81xx; */
    /* struct bdk_slix_s2m_macx_ctl_s cn88xx; */
    struct bdk_slix_s2m_macx_ctl_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t ccnt                  : 8;  /**< [ 31: 24](R/W) CPL-TLP FIFO credits. Legal values are 0x25 to 0xF4. For diagnostic use only. */
        uint64_t ncnt                  : 8;  /**< [ 23: 16](R/W) NP-TLP FIFO credits. Legal values are 0x5 to 0x20. For diagnostic use only. */
        uint64_t pcnt                  : 8;  /**< [ 15:  8](R/W) P-TLP FIFO credits. Legal values are 0x25 to 0xF4. For diagnostic use only. */
        uint64_t tags                  : 8;  /**< [  7:  0](R/W) Number of tags available for MAC.
                                                                 One tag is needed for each outbound TLP that requires a CPL TLP.
                                                                 This field should only be written as part of a reset sequence and before issuing any read
                                                                 operations, CFGs, or I/O transactions from the core(s). For diagnostic use only.
                                                                 Legal values are 1 to 32. */
#else /* Word 0 - Little Endian */
        uint64_t tags                  : 8;  /**< [  7:  0](R/W) Number of tags available for MAC.
                                                                 One tag is needed for each outbound TLP that requires a CPL TLP.
                                                                 This field should only be written as part of a reset sequence and before issuing any read
                                                                 operations, CFGs, or I/O transactions from the core(s). For diagnostic use only.
                                                                 Legal values are 1 to 32. */
        uint64_t pcnt                  : 8;  /**< [ 15:  8](R/W) P-TLP FIFO credits. Legal values are 0x25 to 0xF4. For diagnostic use only. */
        uint64_t ncnt                  : 8;  /**< [ 23: 16](R/W) NP-TLP FIFO credits. Legal values are 0x5 to 0x20. For diagnostic use only. */
        uint64_t ccnt                  : 8;  /**< [ 31: 24](R/W) CPL-TLP FIFO credits. Legal values are 0x25 to 0xF4. For diagnostic use only. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_slix_s2m_macx_ctl bdk_slix_s2m_macx_ctl_t;

static inline uint64_t BDK_SLIX_S2M_MACX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_S2M_MACX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=2)))
        return 0x874001002080ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x874001002080ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=2)))
        return 0x874001002080ll + 0x1000000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_S2M_MACX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_S2M_MACX_CTL(a,b) bdk_slix_s2m_macx_ctl_t
#define bustype_BDK_SLIX_S2M_MACX_CTL(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_S2M_MACX_CTL(a,b) "SLIX_S2M_MACX_CTL"
#define device_bar_BDK_SLIX_S2M_MACX_CTL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_S2M_MACX_CTL(a,b) (a)
#define arguments_BDK_SLIX_S2M_MACX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_s2m_reg#_acc
 *
 * SLI Region Access Registers
 * These registers contains address index and control bits for access to memory from cores.
 * Indexed using {NCBO DST[3:0], NCBO Address[35:32]}.
 */
union bdk_slix_s2m_regx_acc
{
    uint64_t u;
    struct bdk_slix_s2m_regx_acc_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_55_63        : 9;
        uint64_t ctype                 : 2;  /**< [ 54: 53](R/W) The command type to be generated:
                                                                 0x0 = PCI memory.
                                                                 0x1 = PCI configuration (only 8, 16, 32-bit loads are supported). Note normally the ECAM
                                                                 would be used in place of this CTYPE.
                                                                 0x2 = PCI I/O (Only 8, 16, 32-bit loads are supported).
                                                                 0x3 = Reserved. */
        uint64_t zero                  : 1;  /**< [ 52: 52](R/W) Causes all byte read operations to be zero-length read operations. Returns zeros to the
                                                                 EXEC for all read data. */
        uint64_t reserved_49_51        : 3;
        uint64_t nmerge                : 1;  /**< [ 48: 48](R/W) When set, no write merging is allowed in this window. */
        uint64_t esr                   : 2;  /**< [ 47: 46](RO) Reserved. */
        uint64_t esw                   : 2;  /**< [ 45: 44](RO) Reserved. */
        uint64_t wtype                 : 2;  /**< [ 43: 42](R/W) Write type. ADDRTYPE\<1:0\> for write operations to this region.
                                                                 ADDRTYPE\<0\> is the relaxed-order attribute.
                                                                 ADDRTYPE\<1\> is the no-snoop attribute. */
        uint64_t rtype                 : 2;  /**< [ 41: 40](R/W) Read type. ADDRTYPE\<1:0\> for read operations to this region.
                                                                 ADDRTYPE\<0\> is the relaxed-order attribute.
                                                                 ADDRTYPE\<1\> is the no-snoop attribute. */
        uint64_t reserved_32_39        : 8;
        uint64_t ba                    : 32; /**< [ 31:  0](R/W) Bus address. Address bits\<63:32\> for read/write operations that use this region. */
#else /* Word 0 - Little Endian */
        uint64_t ba                    : 32; /**< [ 31:  0](R/W) Bus address. Address bits\<63:32\> for read/write operations that use this region. */
        uint64_t reserved_32_39        : 8;
        uint64_t rtype                 : 2;  /**< [ 41: 40](R/W) Read type. ADDRTYPE\<1:0\> for read operations to this region.
                                                                 ADDRTYPE\<0\> is the relaxed-order attribute.
                                                                 ADDRTYPE\<1\> is the no-snoop attribute. */
        uint64_t wtype                 : 2;  /**< [ 43: 42](R/W) Write type. ADDRTYPE\<1:0\> for write operations to this region.
                                                                 ADDRTYPE\<0\> is the relaxed-order attribute.
                                                                 ADDRTYPE\<1\> is the no-snoop attribute. */
        uint64_t esw                   : 2;  /**< [ 45: 44](RO) Reserved. */
        uint64_t esr                   : 2;  /**< [ 47: 46](RO) Reserved. */
        uint64_t nmerge                : 1;  /**< [ 48: 48](R/W) When set, no write merging is allowed in this window. */
        uint64_t reserved_49_51        : 3;
        uint64_t zero                  : 1;  /**< [ 52: 52](R/W) Causes all byte read operations to be zero-length read operations. Returns zeros to the
                                                                 EXEC for all read data. */
        uint64_t ctype                 : 2;  /**< [ 54: 53](R/W) The command type to be generated:
                                                                 0x0 = PCI memory.
                                                                 0x1 = PCI configuration (only 8, 16, 32-bit loads are supported). Note normally the ECAM
                                                                 would be used in place of this CTYPE.
                                                                 0x2 = PCI I/O (Only 8, 16, 32-bit loads are supported).
                                                                 0x3 = Reserved. */
        uint64_t reserved_55_63        : 9;
#endif /* Word 0 - End */
    } s;
    struct bdk_slix_s2m_regx_acc_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_55_63        : 9;
        uint64_t ctype                 : 2;  /**< [ 54: 53](R/W) The command type to be generated:
                                                                 0x0 = PCI memory.
                                                                 0x1 = PCI configuration (only 8, 16, 32-bit loads are supported). Note normally the ECAM
                                                                 would be used in place of this CTYPE.
                                                                 0x2 = PCI I/O (Only 8, 16, 32-bit loads are supported).
                                                                 0x3 = Reserved. */
        uint64_t zero                  : 1;  /**< [ 52: 52](R/W) Causes all byte read operations to be zero-length read operations. Returns zeros to the
                                                                 EXEC for all read data. */
        uint64_t mac                   : 3;  /**< [ 51: 49](R/W) The MAC that reads/writes to this subid are sent. */
        uint64_t nmerge                : 1;  /**< [ 48: 48](R/W) When set, no write merging is allowed in this window. */
        uint64_t esr                   : 2;  /**< [ 47: 46](RO) Reserved. */
        uint64_t esw                   : 2;  /**< [ 45: 44](RO) Reserved. */
        uint64_t wtype                 : 2;  /**< [ 43: 42](R/W) Write type. ADDRTYPE\<1:0\> for write operations to this region.
                                                                 ADDRTYPE\<0\> is the relaxed-order attribute.
                                                                 ADDRTYPE\<1\> is the no-snoop attribute. */
        uint64_t rtype                 : 2;  /**< [ 41: 40](R/W) Read type. ADDRTYPE\<1:0\> for read operations to this region.
                                                                 ADDRTYPE\<0\> is the relaxed-order attribute.
                                                                 ADDRTYPE\<1\> is the no-snoop attribute. */
        uint64_t reserved_32_39        : 8;
        uint64_t ba                    : 32; /**< [ 31:  0](R/W) Bus address. Address bits\<63:32\> for read/write operations that use this region. */
#else /* Word 0 - Little Endian */
        uint64_t ba                    : 32; /**< [ 31:  0](R/W) Bus address. Address bits\<63:32\> for read/write operations that use this region. */
        uint64_t reserved_32_39        : 8;
        uint64_t rtype                 : 2;  /**< [ 41: 40](R/W) Read type. ADDRTYPE\<1:0\> for read operations to this region.
                                                                 ADDRTYPE\<0\> is the relaxed-order attribute.
                                                                 ADDRTYPE\<1\> is the no-snoop attribute. */
        uint64_t wtype                 : 2;  /**< [ 43: 42](R/W) Write type. ADDRTYPE\<1:0\> for write operations to this region.
                                                                 ADDRTYPE\<0\> is the relaxed-order attribute.
                                                                 ADDRTYPE\<1\> is the no-snoop attribute. */
        uint64_t esw                   : 2;  /**< [ 45: 44](RO) Reserved. */
        uint64_t esr                   : 2;  /**< [ 47: 46](RO) Reserved. */
        uint64_t nmerge                : 1;  /**< [ 48: 48](R/W) When set, no write merging is allowed in this window. */
        uint64_t mac                   : 3;  /**< [ 51: 49](R/W) The MAC that reads/writes to this subid are sent. */
        uint64_t zero                  : 1;  /**< [ 52: 52](R/W) Causes all byte read operations to be zero-length read operations. Returns zeros to the
                                                                 EXEC for all read data. */
        uint64_t ctype                 : 2;  /**< [ 54: 53](R/W) The command type to be generated:
                                                                 0x0 = PCI memory.
                                                                 0x1 = PCI configuration (only 8, 16, 32-bit loads are supported). Note normally the ECAM
                                                                 would be used in place of this CTYPE.
                                                                 0x2 = PCI I/O (Only 8, 16, 32-bit loads are supported).
                                                                 0x3 = Reserved. */
        uint64_t reserved_55_63        : 9;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_slix_s2m_regx_acc_cn81xx cn88xx; */
    struct bdk_slix_s2m_regx_acc_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_55_63        : 9;
        uint64_t ctype                 : 2;  /**< [ 54: 53](R/W) The command type to be generated:
                                                                 0x0 = PCI memory.
                                                                 0x1 = PCI configuration (only 8, 16, 32-bit loads are supported). Note normally the ECAM
                                                                 would be used in place of this CTYPE.
                                                                 0x2 = PCI I/O (Only 8, 16, 32-bit loads are supported).
                                                                 0x3 = Reserved. */
        uint64_t zero                  : 1;  /**< [ 52: 52](R/W) Causes all byte read operations to be zero-length read operations. Returns zeros to the
                                                                 EXEC for all read data. */
        uint64_t epf                   : 3;  /**< [ 51: 49](R/W) The EPF that reads/writes to this subid are sent. */
        uint64_t nmerge                : 1;  /**< [ 48: 48](R/W) When set, no write merging is allowed in this window. */
        uint64_t esr                   : 2;  /**< [ 47: 46](RO) Reserved. */
        uint64_t esw                   : 2;  /**< [ 45: 44](RO) Reserved. */
        uint64_t wtype                 : 2;  /**< [ 43: 42](R/W) Write type. ADDRTYPE\<1:0\> for write operations to this region.
                                                                 ADDRTYPE\<0\> is the relaxed-order attribute.
                                                                 ADDRTYPE\<1\> is the no-snoop attribute. */
        uint64_t rtype                 : 2;  /**< [ 41: 40](R/W) Read type. ADDRTYPE\<1:0\> for read operations to this region.
                                                                 ADDRTYPE\<0\> is the relaxed-order attribute.
                                                                 ADDRTYPE\<1\> is the no-snoop attribute. */
        uint64_t reserved_32_39        : 8;
        uint64_t ba                    : 32; /**< [ 31:  0](R/W) Bus address. Address bits\<63:32\> for read/write operations that use this region. */
#else /* Word 0 - Little Endian */
        uint64_t ba                    : 32; /**< [ 31:  0](R/W) Bus address. Address bits\<63:32\> for read/write operations that use this region. */
        uint64_t reserved_32_39        : 8;
        uint64_t rtype                 : 2;  /**< [ 41: 40](R/W) Read type. ADDRTYPE\<1:0\> for read operations to this region.
                                                                 ADDRTYPE\<0\> is the relaxed-order attribute.
                                                                 ADDRTYPE\<1\> is the no-snoop attribute. */
        uint64_t wtype                 : 2;  /**< [ 43: 42](R/W) Write type. ADDRTYPE\<1:0\> for write operations to this region.
                                                                 ADDRTYPE\<0\> is the relaxed-order attribute.
                                                                 ADDRTYPE\<1\> is the no-snoop attribute. */
        uint64_t esw                   : 2;  /**< [ 45: 44](RO) Reserved. */
        uint64_t esr                   : 2;  /**< [ 47: 46](RO) Reserved. */
        uint64_t nmerge                : 1;  /**< [ 48: 48](R/W) When set, no write merging is allowed in this window. */
        uint64_t epf                   : 3;  /**< [ 51: 49](R/W) The EPF that reads/writes to this subid are sent. */
        uint64_t zero                  : 1;  /**< [ 52: 52](R/W) Causes all byte read operations to be zero-length read operations. Returns zeros to the
                                                                 EXEC for all read data. */
        uint64_t ctype                 : 2;  /**< [ 54: 53](R/W) The command type to be generated:
                                                                 0x0 = PCI memory.
                                                                 0x1 = PCI configuration (only 8, 16, 32-bit loads are supported). Note normally the ECAM
                                                                 would be used in place of this CTYPE.
                                                                 0x2 = PCI I/O (Only 8, 16, 32-bit loads are supported).
                                                                 0x3 = Reserved. */
        uint64_t reserved_55_63        : 9;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_slix_s2m_regx_acc bdk_slix_s2m_regx_acc_t;

static inline uint64_t BDK_SLIX_S2M_REGX_ACC(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_S2M_REGX_ACC(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=255)))
        return 0x874001000000ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0xff);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=255)))
        return 0x874001000000ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0xff);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=255)))
        return 0x874001000000ll + 0x1000000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0xff);
    __bdk_csr_fatal("SLIX_S2M_REGX_ACC", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_S2M_REGX_ACC(a,b) bdk_slix_s2m_regx_acc_t
#define bustype_BDK_SLIX_S2M_REGX_ACC(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_S2M_REGX_ACC(a,b) "SLIX_S2M_REGX_ACC"
#define device_bar_BDK_SLIX_S2M_REGX_ACC(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_S2M_REGX_ACC(a,b) (a)
#define arguments_BDK_SLIX_S2M_REGX_ACC(a,b) (a),(b),-1,-1

/**
 * Register (NCB) sli#_s2m_reg#_acc2
 *
 * SLI Region Access 2 Registers
 * See SLI()_LMAC_CONST0().
 */
union bdk_slix_s2m_regx_acc2
{
    uint64_t u;
    struct bdk_slix_s2m_regx_acc2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t vf_rsvd               : 9;  /**< [ 15:  7](RO) For expansion of the [VF] field for compatibility with other chips with larger
                                                                 SLI()_LMAC_CONST1()[VFS]. */
        uint64_t pvf                   : 7;  /**< [  6:  0](R/W) The PF/VF number.  0x0=PF, 0x1-0x40 is VF number (i.e 0x1=VF1).
                                                                 Must be less than SLI()_LMAC_CONST1()[VFS]. */
#else /* Word 0 - Little Endian */
        uint64_t pvf                   : 7;  /**< [  6:  0](R/W) The PF/VF number.  0x0=PF, 0x1-0x40 is VF number (i.e 0x1=VF1).
                                                                 Must be less than SLI()_LMAC_CONST1()[VFS]. */
        uint64_t vf_rsvd               : 9;  /**< [ 15:  7](RO) For expansion of the [VF] field for compatibility with other chips with larger
                                                                 SLI()_LMAC_CONST1()[VFS]. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_s2m_regx_acc2_s cn; */
};
typedef union bdk_slix_s2m_regx_acc2 bdk_slix_s2m_regx_acc2_t;

static inline uint64_t BDK_SLIX_S2M_REGX_ACC2(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_S2M_REGX_ACC2(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=255)))
        return 0x874001005000ll + 0x1000000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0xff);
    __bdk_csr_fatal("SLIX_S2M_REGX_ACC2", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_S2M_REGX_ACC2(a,b) bdk_slix_s2m_regx_acc2_t
#define bustype_BDK_SLIX_S2M_REGX_ACC2(a,b) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_S2M_REGX_ACC2(a,b) "SLIX_S2M_REGX_ACC2"
#define device_bar_BDK_SLIX_S2M_REGX_ACC2(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_S2M_REGX_ACC2(a,b) (a)
#define arguments_BDK_SLIX_S2M_REGX_ACC2(a,b) (a),(b),-1,-1

/**
 * Register (PEXP_NCB) sli#_scratch_1
 *
 * SLI Scratch 1 Register
 * These registers are general purpose 64-bit scratch registers for software use.
 */
union bdk_slix_scratch_1
{
    uint64_t u;
    struct bdk_slix_scratch_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) The value in this register is totally software defined. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) The value in this register is totally software defined. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_scratch_1_s cn; */
};
typedef union bdk_slix_scratch_1 bdk_slix_scratch_1_t;

static inline uint64_t BDK_SLIX_SCRATCH_1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_SCRATCH_1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x874000001000ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x874000001000ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x874000001000ll + 0x1000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_SCRATCH_1", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_SCRATCH_1(a) bdk_slix_scratch_1_t
#define bustype_BDK_SLIX_SCRATCH_1(a) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_SCRATCH_1(a) "SLIX_SCRATCH_1"
#define device_bar_BDK_SLIX_SCRATCH_1(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_SCRATCH_1(a) (a)
#define arguments_BDK_SLIX_SCRATCH_1(a) (a),-1,-1,-1

/**
 * Register (PEXP_NCB) sli#_scratch_2
 *
 * SLI Scratch 2 Register
 * These registers are general purpose 64-bit scratch registers for software use.
 */
union bdk_slix_scratch_2
{
    uint64_t u;
    struct bdk_slix_scratch_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) The value in this register is totally software defined. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W) The value in this register is totally software defined. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_scratch_2_s cn; */
};
typedef union bdk_slix_scratch_2 bdk_slix_scratch_2_t;

static inline uint64_t BDK_SLIX_SCRATCH_2(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_SCRATCH_2(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x874000001010ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x874000001010ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x874000001010ll + 0x1000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_SCRATCH_2", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_SCRATCH_2(a) bdk_slix_scratch_2_t
#define bustype_BDK_SLIX_SCRATCH_2(a) BDK_CSR_TYPE_PEXP_NCB
#define basename_BDK_SLIX_SCRATCH_2(a) "SLIX_SCRATCH_2"
#define device_bar_BDK_SLIX_SCRATCH_2(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_SCRATCH_2(a) (a)
#define arguments_BDK_SLIX_SCRATCH_2(a) (a),-1,-1,-1

/**
 * Register (NCB) sli#_sctl
 *
 * SLI Secure Control Register
 */
union bdk_slix_sctl
{
    uint64_t u;
    struct bdk_slix_sctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t scen                  : 1;  /**< [  0:  0](SR/W) Allow SLI window transactions to request secure-world accesses.

                                                                 0 = SLI()_WIN_RD_ADDR[SECEN], SLI()_WIN_WR_ADDR[SECEN] are ignored and treated
                                                                 as if zero. Window transactions onto NCB are nonsecure, though the SMMU may
                                                                 later promote them to secure.

                                                                 1 = SLI()_WIN_RD_ADDR[SECEN], SLI()_WIN_WR_ADDR[SECEN] are honored. Window
                                                                 transactions may request nonsecure or secure world. This bit should not be set
                                                                 in trusted-mode.

                                                                 Resets to 0 when in trusted-mode (RST_BOOT[TRUSTED_MODE]), else resets to 1. */
#else /* Word 0 - Little Endian */
        uint64_t scen                  : 1;  /**< [  0:  0](SR/W) Allow SLI window transactions to request secure-world accesses.

                                                                 0 = SLI()_WIN_RD_ADDR[SECEN], SLI()_WIN_WR_ADDR[SECEN] are ignored and treated
                                                                 as if zero. Window transactions onto NCB are nonsecure, though the SMMU may
                                                                 later promote them to secure.

                                                                 1 = SLI()_WIN_RD_ADDR[SECEN], SLI()_WIN_WR_ADDR[SECEN] are honored. Window
                                                                 transactions may request nonsecure or secure world. This bit should not be set
                                                                 in trusted-mode.

                                                                 Resets to 0 when in trusted-mode (RST_BOOT[TRUSTED_MODE]), else resets to 1. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    struct bdk_slix_sctl_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t scen                  : 1;  /**< [  0:  0](SR/W) Allow SLI window transactions to request secure-world accesses.

                                                                 0 = SLI()_WIN_RD_ADDR[RD_SEC], SLI()_WIN_WR_ADDR[WR_SEC] are ignored and treated
                                                                 as if zero. Window transactions onto NCB are nonsecure, though the SMMU may
                                                                 later promote them to secure.

                                                                 1 = SLI()_WIN_RD_ADDR[RD_SEC], SLI()_WIN_WR_ADDR[WR_SEC] are honored. Window
                                                                 transactions may request nonsecure or secure world. This bit should not be set
                                                                 in trusted-mode.

                                                                 Resets to 0 when in trusted-mode (RST_BOOT[TRUSTED_MODE]), else resets to 1. */
#else /* Word 0 - Little Endian */
        uint64_t scen                  : 1;  /**< [  0:  0](SR/W) Allow SLI window transactions to request secure-world accesses.

                                                                 0 = SLI()_WIN_RD_ADDR[RD_SEC], SLI()_WIN_WR_ADDR[WR_SEC] are ignored and treated
                                                                 as if zero. Window transactions onto NCB are nonsecure, though the SMMU may
                                                                 later promote them to secure.

                                                                 1 = SLI()_WIN_RD_ADDR[RD_SEC], SLI()_WIN_WR_ADDR[WR_SEC] are honored. Window
                                                                 transactions may request nonsecure or secure world. This bit should not be set
                                                                 in trusted-mode.

                                                                 Resets to 0 when in trusted-mode (RST_BOOT[TRUSTED_MODE]), else resets to 1. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_slix_sctl_cn81xx cn88xx; */
    /* struct bdk_slix_sctl_s cn83xx; */
};
typedef union bdk_slix_sctl bdk_slix_sctl_t;

static inline uint64_t BDK_SLIX_SCTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_SCTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x874001002010ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x874001002010ll + 0x1000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS2_X) && (a<=1))
        return 0x874001002010ll + 0x1000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_SCTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_SCTL(a) bdk_slix_sctl_t
#define bustype_BDK_SLIX_SCTL(a) BDK_CSR_TYPE_NCB
#define basename_BDK_SLIX_SCTL(a) "SLIX_SCTL"
#define device_bar_BDK_SLIX_SCTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_SLIX_SCTL(a) (a)
#define arguments_BDK_SLIX_SCTL(a) (a),-1,-1,-1

/**
 * Register (PEXP) sli#_win_rd_addr
 *
 * SLI Window Read Address Register
 * This register contains the address to be read when SLI()_WIN_RD_DATA is read.
 * Writing this register causes a read operation to take place.
 * This register should not be used to read SLI_* registers.
 */
union bdk_slix_win_rd_addr
{
    uint64_t u;
    struct bdk_slix_win_rd_addr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t secen                 : 1;  /**< [ 63: 63](R/W) This request is a secure-world transaction. This is intended to be set only for
                                                                 transactions during early boot when the SMMU is in bypass mode; after SMMU
                                                                 initialization SMMU()_SDDR() may be used to control which SLI streams are secure.

                                                                 If SLI()_SCTL[SECEN] = 0, this bit is ignored and transactions are always nonsecure
                                                                 onto the NCB, though the SMMU may later promote them to secure. */
        uint64_t reserved_51_62        : 12;
        uint64_t ld_cmd                : 2;  /**< [ 50: 49](R/W) The load command size.
                                                                 0x3 = Load 8 bytes.
                                                                 0x2 = Load 4 bytes.
                                                                 0x1 = Load 2 bytes.
                                                                 0x0 = Load 1 bytes. */
        uint64_t rd_addr               : 49; /**< [ 48:  0](R/W) The IOVA sent to the NCB for this load request. */
#else /* Word 0 - Little Endian */
        uint64_t rd_addr               : 49; /**< [ 48:  0](R/W) The IOVA sent to the NCB for this load request. */
        uint64_t ld_cmd                : 2;  /**< [ 50: 49](R/W) The load command size.
                                                                 0x3 = Load 8 bytes.
                                                                 0x2 = Load 4 bytes.
                                                                 0x1 = Load 2 bytes.
                                                                 0x0 = Load 1 bytes. */
        uint64_t reserved_51_62        : 12;
        uint64_t secen                 : 1;  /**< [ 63: 63](R/W) This request is a secure-world transaction. This is intended to be set only for
                                                                 transactions during early boot when the SMMU is in bypass mode; after SMMU
                                                                 initialization SMMU()_SDDR() may be used to control which SLI streams are secure.

                                                                 If SLI()_SCTL[SECEN] = 0, this bit is ignored and transactions are always nonsecure
                                                                 onto the NCB, though the SMMU may later promote them to secure. */
#endif /* Word 0 - End */
    } s;
    struct bdk_slix_win_rd_addr_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_51_63        : 13;
        uint64_t ld_cmd                : 2;  /**< [ 50: 49](R/W) The load command size.
                                                                 0x3 = Load 8 bytes.
                                                                 0x2 = Load 4 bytes.
                                                                 0x1 = Load 2 bytes.
                                                                 0x0 = Load 1 bytes. */
        uint64_t rd_addr               : 49; /**< [ 48:  0](R/W) The IOVA sent to the NCB for this load request. */
#else /* Word 0 - Little Endian */
        uint64_t rd_addr               : 49; /**< [ 48:  0](R/W) The IOVA sent to the NCB for this load request. */
        uint64_t ld_cmd                : 2;  /**< [ 50: 49](R/W) The load command size.
                                                                 0x3 = Load 8 bytes.
                                                                 0x2 = Load 4 bytes.
                                                                 0x1 = Load 2 bytes.
                                                                 0x0 = Load 1 bytes. */
        uint64_t reserved_51_63        : 13;
#endif /* Word 0 - End */
    } cn88xxp1;
    /* struct bdk_slix_win_rd_addr_s cn81xx; */
    /* struct bdk_slix_win_rd_addr_s cn88xxp2; */
};
typedef union bdk_slix_win_rd_addr bdk_slix_win_rd_addr_t;

static inline uint64_t BDK_SLIX_WIN_RD_ADDR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_WIN_RD_ADDR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x10ll + 0x10000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x10ll + 0x10000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_WIN_RD_ADDR", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_WIN_RD_ADDR(a) bdk_slix_win_rd_addr_t
#define bustype_BDK_SLIX_WIN_RD_ADDR(a) BDK_CSR_TYPE_PEXP
#define basename_BDK_SLIX_WIN_RD_ADDR(a) "SLIX_WIN_RD_ADDR"
#define busnum_BDK_SLIX_WIN_RD_ADDR(a) (a)
#define arguments_BDK_SLIX_WIN_RD_ADDR(a) (a),-1,-1,-1

/**
 * Register (PEXP) sli#_win_rd_addr#
 *
 * SLI Window Read Address Register
 * This register contains the address to be read when SLI()_WIN_RD_DATA() is read.
 * Writing this register causes a read operation to take place.
 * This register should not be used to read SLI_* registers.
 */
union bdk_slix_win_rd_addrx
{
    uint64_t u;
    struct bdk_slix_win_rd_addrx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t secen                 : 1;  /**< [ 63: 63](R/W) This request is a secure-world transaction. This is intended to be set only for
                                                                 transactions during early boot when the SMMU is in bypass mode; after SMMU
                                                                 initialization SMMU()_SSDR() may be used to control which SLI streams are secure.

                                                                 If SLI()_SCTL[SCEN] = 0, this bit is ignored and transactions are always nonsecure
                                                                 onto the NCB, though the SMMU may later promote them to secure. */
        uint64_t reserved_51_62        : 12;
        uint64_t ld_cmd                : 2;  /**< [ 50: 49](R/W) The load command size.
                                                                 0x3 = Load 8 bytes.
                                                                 0x2 = Load 4 bytes.
                                                                 0x1 = Load 2 bytes.
                                                                 0x0 = Load 1 bytes. */
        uint64_t rd_addr               : 49; /**< [ 48:  0](R/W) The IOVA sent to the NCB for this load request. */
#else /* Word 0 - Little Endian */
        uint64_t rd_addr               : 49; /**< [ 48:  0](R/W) The IOVA sent to the NCB for this load request. */
        uint64_t ld_cmd                : 2;  /**< [ 50: 49](R/W) The load command size.
                                                                 0x3 = Load 8 bytes.
                                                                 0x2 = Load 4 bytes.
                                                                 0x1 = Load 2 bytes.
                                                                 0x0 = Load 1 bytes. */
        uint64_t reserved_51_62        : 12;
        uint64_t secen                 : 1;  /**< [ 63: 63](R/W) This request is a secure-world transaction. This is intended to be set only for
                                                                 transactions during early boot when the SMMU is in bypass mode; after SMMU
                                                                 initialization SMMU()_SSDR() may be used to control which SLI streams are secure.

                                                                 If SLI()_SCTL[SCEN] = 0, this bit is ignored and transactions are always nonsecure
                                                                 onto the NCB, though the SMMU may later promote them to secure. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_win_rd_addrx_s cn; */
};
typedef union bdk_slix_win_rd_addrx bdk_slix_win_rd_addrx_t;

static inline uint64_t BDK_SLIX_WIN_RD_ADDRX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_WIN_RD_ADDRX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x2c010ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_WIN_RD_ADDRX", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_WIN_RD_ADDRX(a,b) bdk_slix_win_rd_addrx_t
#define bustype_BDK_SLIX_WIN_RD_ADDRX(a,b) BDK_CSR_TYPE_PEXP
#define basename_BDK_SLIX_WIN_RD_ADDRX(a,b) "SLIX_WIN_RD_ADDRX"
#define busnum_BDK_SLIX_WIN_RD_ADDRX(a,b) (a)
#define arguments_BDK_SLIX_WIN_RD_ADDRX(a,b) (a),(b),-1,-1

/**
 * Register (PEXP) sli#_win_rd_data
 *
 * SLI Window Read Data Register
 * This register contains the address to be read when SLI()_WIN_RD_DATA is read.
 */
union bdk_slix_win_rd_data
{
    uint64_t u;
    struct bdk_slix_win_rd_data_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t rd_data               : 64; /**< [ 63:  0](RO/H) The read data. */
#else /* Word 0 - Little Endian */
        uint64_t rd_data               : 64; /**< [ 63:  0](RO/H) The read data. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_win_rd_data_s cn; */
};
typedef union bdk_slix_win_rd_data bdk_slix_win_rd_data_t;

static inline uint64_t BDK_SLIX_WIN_RD_DATA(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_WIN_RD_DATA(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x40ll + 0x10000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x40ll + 0x10000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_WIN_RD_DATA", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_WIN_RD_DATA(a) bdk_slix_win_rd_data_t
#define bustype_BDK_SLIX_WIN_RD_DATA(a) BDK_CSR_TYPE_PEXP
#define basename_BDK_SLIX_WIN_RD_DATA(a) "SLIX_WIN_RD_DATA"
#define busnum_BDK_SLIX_WIN_RD_DATA(a) (a)
#define arguments_BDK_SLIX_WIN_RD_DATA(a) (a),-1,-1,-1

/**
 * Register (PEXP) sli#_win_rd_data#
 *
 * SLI Window Read Data Register
 * This register contains the address to be read when SLI()_WIN_RD_DATA() is read.
 */
union bdk_slix_win_rd_datax
{
    uint64_t u;
    struct bdk_slix_win_rd_datax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t rd_data               : 64; /**< [ 63:  0](RO/H) The read data. */
#else /* Word 0 - Little Endian */
        uint64_t rd_data               : 64; /**< [ 63:  0](RO/H) The read data. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_win_rd_datax_s cn; */
};
typedef union bdk_slix_win_rd_datax bdk_slix_win_rd_datax_t;

static inline uint64_t BDK_SLIX_WIN_RD_DATAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_WIN_RD_DATAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x2c040ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_WIN_RD_DATAX", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_WIN_RD_DATAX(a,b) bdk_slix_win_rd_datax_t
#define bustype_BDK_SLIX_WIN_RD_DATAX(a,b) BDK_CSR_TYPE_PEXP
#define basename_BDK_SLIX_WIN_RD_DATAX(a,b) "SLIX_WIN_RD_DATAX"
#define busnum_BDK_SLIX_WIN_RD_DATAX(a,b) (a)
#define arguments_BDK_SLIX_WIN_RD_DATAX(a,b) (a),(b),-1,-1

/**
 * Register (PEXP) sli#_win_wr_addr
 *
 * SLI Window Write Address Register
 * Contains the address to be writen to when a write operation is started by writing
 * SLI()_WIN_WR_DATA.
 * This register should not be used to write SLI_* registers.
 */
union bdk_slix_win_wr_addr
{
    uint64_t u;
    struct bdk_slix_win_wr_addr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t secen                 : 1;  /**< [ 63: 63](R/W) This request is a secure-world transaction. This is intended to be set only for
                                                                 transactions during early boot when the SMMU is in bypass mode; after SMMU
                                                                 initialization SMMU()_SDDR() may be used to control which SLI streams are secure.

                                                                 If SLI()_SCTL[SECEN] = 0, this bit is ignored and transactions are always nonsecure
                                                                 onto the NCB, though the SMMU may later promote them to secure. */
        uint64_t reserved_49_62        : 14;
        uint64_t wr_addr               : 46; /**< [ 48:  3](R/W) The IOVA sent to the NCB for this store request. */
        uint64_t reserved_0_2          : 3;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_2          : 3;
        uint64_t wr_addr               : 46; /**< [ 48:  3](R/W) The IOVA sent to the NCB for this store request. */
        uint64_t reserved_49_62        : 14;
        uint64_t secen                 : 1;  /**< [ 63: 63](R/W) This request is a secure-world transaction. This is intended to be set only for
                                                                 transactions during early boot when the SMMU is in bypass mode; after SMMU
                                                                 initialization SMMU()_SDDR() may be used to control which SLI streams are secure.

                                                                 If SLI()_SCTL[SECEN] = 0, this bit is ignored and transactions are always nonsecure
                                                                 onto the NCB, though the SMMU may later promote them to secure. */
#endif /* Word 0 - End */
    } s;
    struct bdk_slix_win_wr_addr_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t wr_addr               : 46; /**< [ 48:  3](R/W) The IOVA sent to the NCB for this store request. */
        uint64_t reserved_0_2          : 3;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_2          : 3;
        uint64_t wr_addr               : 46; /**< [ 48:  3](R/W) The IOVA sent to the NCB for this store request. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } cn88xxp1;
    /* struct bdk_slix_win_wr_addr_s cn81xx; */
    /* struct bdk_slix_win_wr_addr_s cn88xxp2; */
};
typedef union bdk_slix_win_wr_addr bdk_slix_win_wr_addr_t;

static inline uint64_t BDK_SLIX_WIN_WR_ADDR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_WIN_WR_ADDR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0ll + 0x10000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0ll + 0x10000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_WIN_WR_ADDR", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_WIN_WR_ADDR(a) bdk_slix_win_wr_addr_t
#define bustype_BDK_SLIX_WIN_WR_ADDR(a) BDK_CSR_TYPE_PEXP
#define basename_BDK_SLIX_WIN_WR_ADDR(a) "SLIX_WIN_WR_ADDR"
#define busnum_BDK_SLIX_WIN_WR_ADDR(a) (a)
#define arguments_BDK_SLIX_WIN_WR_ADDR(a) (a),-1,-1,-1

/**
 * Register (PEXP) sli#_win_wr_addr#
 *
 * SLI Window Write Address Register
 * Contains the address to be written to when a write operation is started by writing
 * SLI()_WIN_WR_DATA().
 * This register should not be used to write SLI_* registers.
 */
union bdk_slix_win_wr_addrx
{
    uint64_t u;
    struct bdk_slix_win_wr_addrx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t secen                 : 1;  /**< [ 63: 63](R/W) This request is a secure-world transaction. This is intended to be set only for
                                                                 transactions during early boot when the SMMU is in bypass mode; after SMMU
                                                                 initialization SMMU()_SSDR() may be used to control which SLI streams are secure.

                                                                 If SLI()_SCTL[SCEN] = 0, this bit is ignored and transactions are always nonsecure
                                                                 onto the NCB, though the SMMU may later promote them to secure. */
        uint64_t reserved_49_62        : 14;
        uint64_t wr_addr               : 46; /**< [ 48:  3](R/W) The IOVA sent to the NCB for this store request. */
        uint64_t reserved_0_2          : 3;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_2          : 3;
        uint64_t wr_addr               : 46; /**< [ 48:  3](R/W) The IOVA sent to the NCB for this store request. */
        uint64_t reserved_49_62        : 14;
        uint64_t secen                 : 1;  /**< [ 63: 63](R/W) This request is a secure-world transaction. This is intended to be set only for
                                                                 transactions during early boot when the SMMU is in bypass mode; after SMMU
                                                                 initialization SMMU()_SSDR() may be used to control which SLI streams are secure.

                                                                 If SLI()_SCTL[SCEN] = 0, this bit is ignored and transactions are always nonsecure
                                                                 onto the NCB, though the SMMU may later promote them to secure. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_win_wr_addrx_s cn; */
};
typedef union bdk_slix_win_wr_addrx bdk_slix_win_wr_addrx_t;

static inline uint64_t BDK_SLIX_WIN_WR_ADDRX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_WIN_WR_ADDRX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x2c000ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_WIN_WR_ADDRX", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_WIN_WR_ADDRX(a,b) bdk_slix_win_wr_addrx_t
#define bustype_BDK_SLIX_WIN_WR_ADDRX(a,b) BDK_CSR_TYPE_PEXP
#define basename_BDK_SLIX_WIN_WR_ADDRX(a,b) "SLIX_WIN_WR_ADDRX"
#define busnum_BDK_SLIX_WIN_WR_ADDRX(a,b) (a)
#define arguments_BDK_SLIX_WIN_WR_ADDRX(a,b) (a),(b),-1,-1

/**
 * Register (PEXP) sli#_win_wr_data
 *
 * SLI Window Write Data Register
 * This register contains the data to write to the address located in SLI()_WIN_WR_ADDR.
 * Writing this register causes a write operation to take place.
 */
union bdk_slix_win_wr_data
{
    uint64_t u;
    struct bdk_slix_win_wr_data_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t wr_data               : 64; /**< [ 63:  0](R/W) The data to be written. */
#else /* Word 0 - Little Endian */
        uint64_t wr_data               : 64; /**< [ 63:  0](R/W) The data to be written. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_win_wr_data_s cn; */
};
typedef union bdk_slix_win_wr_data bdk_slix_win_wr_data_t;

static inline uint64_t BDK_SLIX_WIN_WR_DATA(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_WIN_WR_DATA(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x20ll + 0x10000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x20ll + 0x10000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_WIN_WR_DATA", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_WIN_WR_DATA(a) bdk_slix_win_wr_data_t
#define bustype_BDK_SLIX_WIN_WR_DATA(a) BDK_CSR_TYPE_PEXP
#define basename_BDK_SLIX_WIN_WR_DATA(a) "SLIX_WIN_WR_DATA"
#define busnum_BDK_SLIX_WIN_WR_DATA(a) (a)
#define arguments_BDK_SLIX_WIN_WR_DATA(a) (a),-1,-1,-1

/**
 * Register (PEXP) sli#_win_wr_data#
 *
 * SLI Window Write Data Register
 * This register contains the data to write to the address located in SLI()_WIN_WR_ADDR().
 * Writing this register causes a write operation to take place.
 */
union bdk_slix_win_wr_datax
{
    uint64_t u;
    struct bdk_slix_win_wr_datax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t wr_data               : 64; /**< [ 63:  0](R/W) The data to be written. */
#else /* Word 0 - Little Endian */
        uint64_t wr_data               : 64; /**< [ 63:  0](R/W) The data to be written. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_win_wr_datax_s cn; */
};
typedef union bdk_slix_win_wr_datax bdk_slix_win_wr_datax_t;

static inline uint64_t BDK_SLIX_WIN_WR_DATAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_WIN_WR_DATAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x2c020ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_WIN_WR_DATAX", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_WIN_WR_DATAX(a,b) bdk_slix_win_wr_datax_t
#define bustype_BDK_SLIX_WIN_WR_DATAX(a,b) BDK_CSR_TYPE_PEXP
#define basename_BDK_SLIX_WIN_WR_DATAX(a,b) "SLIX_WIN_WR_DATAX"
#define busnum_BDK_SLIX_WIN_WR_DATAX(a,b) (a)
#define arguments_BDK_SLIX_WIN_WR_DATAX(a,b) (a),(b),-1,-1

/**
 * Register (PEXP) sli#_win_wr_mask
 *
 * SLI Window Write Mask Register
 * This register contains the mask for the data in SLI()_WIN_WR_DATA.
 */
union bdk_slix_win_wr_mask
{
    uint64_t u;
    struct bdk_slix_win_wr_mask_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t wr_mask               : 8;  /**< [  7:  0](R/W) The byte enables sent to the NCB for this store request. */
#else /* Word 0 - Little Endian */
        uint64_t wr_mask               : 8;  /**< [  7:  0](R/W) The byte enables sent to the NCB for this store request. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_win_wr_mask_s cn; */
};
typedef union bdk_slix_win_wr_mask bdk_slix_win_wr_mask_t;

static inline uint64_t BDK_SLIX_WIN_WR_MASK(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_WIN_WR_MASK(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x30ll + 0x10000000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x30ll + 0x10000000000ll * ((a) & 0x1);
    __bdk_csr_fatal("SLIX_WIN_WR_MASK", 1, a, 0, 0, 0);
}

#define typedef_BDK_SLIX_WIN_WR_MASK(a) bdk_slix_win_wr_mask_t
#define bustype_BDK_SLIX_WIN_WR_MASK(a) BDK_CSR_TYPE_PEXP
#define basename_BDK_SLIX_WIN_WR_MASK(a) "SLIX_WIN_WR_MASK"
#define busnum_BDK_SLIX_WIN_WR_MASK(a) (a)
#define arguments_BDK_SLIX_WIN_WR_MASK(a) (a),-1,-1,-1

/**
 * Register (PEXP) sli#_win_wr_mask#
 *
 * SLI Window Write Mask Register
 * This register contains the mask for the data in SLI()_WIN_WR_DATA().
 */
union bdk_slix_win_wr_maskx
{
    uint64_t u;
    struct bdk_slix_win_wr_maskx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t wr_mask               : 8;  /**< [  7:  0](R/W) The byte enables sent to the NCB for this store request. */
#else /* Word 0 - Little Endian */
        uint64_t wr_mask               : 8;  /**< [  7:  0](R/W) The byte enables sent to the NCB for this store request. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_slix_win_wr_maskx_s cn; */
};
typedef union bdk_slix_win_wr_maskx bdk_slix_win_wr_maskx_t;

static inline uint64_t BDK_SLIX_WIN_WR_MASKX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_SLIX_WIN_WR_MASKX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=3)))
        return 0x2c030ll + 0x1000000000ll * ((a) & 0x0) + 0x800000ll * ((b) & 0x3);
    __bdk_csr_fatal("SLIX_WIN_WR_MASKX", 2, a, b, 0, 0);
}

#define typedef_BDK_SLIX_WIN_WR_MASKX(a,b) bdk_slix_win_wr_maskx_t
#define bustype_BDK_SLIX_WIN_WR_MASKX(a,b) BDK_CSR_TYPE_PEXP
#define basename_BDK_SLIX_WIN_WR_MASKX(a,b) "SLIX_WIN_WR_MASKX"
#define busnum_BDK_SLIX_WIN_WR_MASKX(a,b) (a)
#define arguments_BDK_SLIX_WIN_WR_MASKX(a,b) (a),(b),-1,-1

#endif /* __BDK_CSRS_SLI_H__ */
