#ifndef __BDK_CSRS_RVU_H__
#define __BDK_CSRS_RVU_H__
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
 * Cavium RVU.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration rvu_af_int_vec_e
 *
 * RVU Admin Function Interrupt Vector Enumeration
 * Enumerates the MSI-X interrupt vectors.
 * Internal:
 * RVU maintains the state of these vectors internally, and generates GIB
 * messages for it without accessing the MSI-X table region in LLC/DRAM.
 */
#define BDK_RVU_AF_INT_VEC_E_GEN (3)
#define BDK_RVU_AF_INT_VEC_E_MBOX (4)
#define BDK_RVU_AF_INT_VEC_E_PFFLR (1)
#define BDK_RVU_AF_INT_VEC_E_PFME (2)
#define BDK_RVU_AF_INT_VEC_E_POISON (0)

/**
 * Enumeration rvu_bar_e
 *
 * RVU Base Address Register Enumeration
 * Enumerates the base address registers.
 * Internal:
 * For documentation only.
 */
#define BDK_RVU_BAR_E_RVU_PFX_BAR0(a) (0x850000000000ll + 0x1000000000ll * (a))
#define BDK_RVU_BAR_E_RVU_PFX_BAR0_SIZE 0x200000000ull
#define BDK_RVU_BAR_E_RVU_PFX_FUNCX_BAR2(a,b) (0x850200000000ll + 0x1000000000ll * (a) + 0x2000000ll * (b))
#define BDK_RVU_BAR_E_RVU_PFX_FUNCX_BAR2_SIZE 0x2000000ull

/**
 * Enumeration rvu_block_addr_e
 *
 * RVU Block Address Enumeration
 * Enumerates addressing of RVU resource blocks within each RVU BAR, i.e. values
 * of RVU_FUNC_ADDR_S[BLOCK] and RVU_AF_ADDR_S[BLOCK].
 */
#define BDK_RVU_BLOCK_ADDR_E_CPTX(a) (0xa + (a))
#define BDK_RVU_BLOCK_ADDR_E_LMT (1)
#define BDK_RVU_BLOCK_ADDR_E_MSIX (2)
#define BDK_RVU_BLOCK_ADDR_E_NDCX(a) (0xc + (a))
#define BDK_RVU_BLOCK_ADDR_E_NIXX(a) (4 + (a))
#define BDK_RVU_BLOCK_ADDR_E_NPA (3)
#define BDK_RVU_BLOCK_ADDR_E_NPC (6)
#define BDK_RVU_BLOCK_ADDR_E_RX(a) (0 + (a))
#define BDK_RVU_BLOCK_ADDR_E_RVUM (0)
#define BDK_RVU_BLOCK_ADDR_E_SSO (7)
#define BDK_RVU_BLOCK_ADDR_E_SSOW (8)
#define BDK_RVU_BLOCK_ADDR_E_TIM (9)

/**
 * Enumeration rvu_block_type_e
 *
 * RVU Block Type Enumeration
 * Enumerates values of RVU_PF/RVU_VF_BLOCK_ADDR()_DISC[BTYPE].
 */
#define BDK_RVU_BLOCK_TYPE_E_CPT (9)
#define BDK_RVU_BLOCK_TYPE_E_DDF (0xb)
#define BDK_RVU_BLOCK_TYPE_E_DFA (0xe)
#define BDK_RVU_BLOCK_TYPE_E_HNA (0xf)
#define BDK_RVU_BLOCK_TYPE_E_LMT (2)
#define BDK_RVU_BLOCK_TYPE_E_MSIX (1)
#define BDK_RVU_BLOCK_TYPE_E_NDC (0xa)
#define BDK_RVU_BLOCK_TYPE_E_NIX (3)
#define BDK_RVU_BLOCK_TYPE_E_NPA (4)
#define BDK_RVU_BLOCK_TYPE_E_NPC (5)
#define BDK_RVU_BLOCK_TYPE_E_RAD (0xd)
#define BDK_RVU_BLOCK_TYPE_E_RVUM (0)
#define BDK_RVU_BLOCK_TYPE_E_SSO (6)
#define BDK_RVU_BLOCK_TYPE_E_SSOW (7)
#define BDK_RVU_BLOCK_TYPE_E_TIM (8)
#define BDK_RVU_BLOCK_TYPE_E_ZIP (0xc)

/**
 * Enumeration rvu_bus_lf_e
 *
 * INTERNAL: RVU Bus LF Range Enumeration
 *
 * Enumerates the LF range for the RVU bus.
 * Internal:
 * This is an enum used in csr3 virtual equations.
 */
#define BDK_RVU_BUS_LF_E_RVU_BUS_LFX(a) (0 + 0x2000000 * (a))

/**
 * Enumeration rvu_bus_pf_e
 *
 * INTERNAL: RVU Bus PF Range Enumeration
 *
 * Enumerates the PF range for the RVU bus.
 * Internal:
 * This is an enum used in csr3 virtual equations.
 */
#define BDK_RVU_BUS_PF_E_RVU_BUS_PFX(a) (0ll + 0x1000000000ll * (a))

/**
 * Enumeration rvu_bus_pfvf_e
 *
 * INTERNAL: RVU Bus PFVF Range Enumeration
 *
 * Enumerates the PF and VF ranges for the RVU bus.
 * Internal:
 * This is an enum used in csr3 virtual equations.
 */
#define BDK_RVU_BUS_PFVF_E_RVU_BUS_PFX(a) (0 + 0x2000000 * (a))
#define BDK_RVU_BUS_PFVF_E_RVU_BUS_VFX(a) (0 + 0x2000000 * (a))

/**
 * Enumeration rvu_busbar_e
 *
 * INTERNAL: RVU Bus Base Address Region Enumeration
 *
 * Enumerates the base address region for the RVU bus.
 * Internal:
 * This is an enum used in csr3 virtual equations.
 */
#define BDK_RVU_BUSBAR_E_RVU_BUSBAR0 (0)
#define BDK_RVU_BUSBAR_E_RVU_BUSBAR2 (0x200000000ll)

/**
 * Enumeration rvu_busdid_e
 *
 * INTERNAL: RVU Bus DID Enumeration
 *
 * Enumerates the DID offset for the RVU bus.
 * Internal:
 * This is an enum used in csr3 virtual equations.
 */
#define BDK_RVU_BUSDID_E_RVU_BUSDID (0x850000000000ll)

/**
 * Enumeration rvu_ndc_idx_e
 *
 * RVU NDC Index Enumeration
 * Enumerates NDC instances and index of RVU_BLOCK_ADDR_E::NDC().
 */
#define BDK_RVU_NDC_IDX_E_NIXX_RX(a) (0 + 4 * (a))
#define BDK_RVU_NDC_IDX_E_NIXX_TX(a) (1 + 4 * (a))
#define BDK_RVU_NDC_IDX_E_NPA_UX(a) (2 + 0 * (a))

/**
 * Enumeration rvu_pf_int_vec_e
 *
 * RVU PF Interrupt Vector Enumeration
 * Enumerates the MSI-X interrupt vectors.
 */
#define BDK_RVU_PF_INT_VEC_E_AFPF_MBOX (0xc)
#define BDK_RVU_PF_INT_VEC_E_VFFLRX(a) (0 + (a))
#define BDK_RVU_PF_INT_VEC_E_VFMEX(a) (4 + (a))
#define BDK_RVU_PF_INT_VEC_E_VFPF_MBOXX(a) (8 + (a))

/**
 * Enumeration rvu_vf_int_vec_e
 *
 * RVU VF Interrupt Vector Enumeration
 * Enumerates the MSI-X interrupt vectors.
 */
#define BDK_RVU_VF_INT_VEC_E_MBOX (0)

/**
 * Structure rvu_af_addr_s
 *
 * RVU Admin Function Register Address Structure
 * Address format for accessing shared Admin Function (AF) registers in
 * RVU PF BAR0. These registers may be accessed by all RVU PFs whose
 * RVU_PRIV_PF()_CFG[AF_ENA] bit is set.
 */
union bdk_rvu_af_addr_s
{
    uint64_t u;
    struct bdk_rvu_af_addr_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_33_63        : 31;
        uint64_t block                 : 5;  /**< [ 32: 28] Resource block enumerated by RVU_BLOCK_ADDR_E. */
        uint64_t addr                  : 28; /**< [ 27:  0] Register address within [BLOCK]. */
#else /* Word 0 - Little Endian */
        uint64_t addr                  : 28; /**< [ 27:  0] Register address within [BLOCK]. */
        uint64_t block                 : 5;  /**< [ 32: 28] Resource block enumerated by RVU_BLOCK_ADDR_E. */
        uint64_t reserved_33_63        : 31;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_addr_s_s cn; */
};

/**
 * Structure rvu_func_addr_s
 *
 * RVU Function-unique Address Structure
 * Address format for accessing function-unique registers in RVU PF/FUNC BAR2.
 */
union bdk_rvu_func_addr_s
{
    uint32_t u;
    struct bdk_rvu_func_addr_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_25_31        : 7;
        uint32_t block                 : 5;  /**< [ 24: 20] Resource block enumerated by RVU_BLOCK_ADDR_E. */
        uint32_t lf_slot               : 8;  /**< [ 19: 12] Local function slot, or extended register address within the block's LF
                                                                 slot 0, depending on [BLOCK]. */
        uint32_t addr                  : 12; /**< [ 11:  0] Register address within the block and LF slot. */
#else /* Word 0 - Little Endian */
        uint32_t addr                  : 12; /**< [ 11:  0] Register address within the block and LF slot. */
        uint32_t lf_slot               : 8;  /**< [ 19: 12] Local function slot, or extended register address within the block's LF
                                                                 slot 0, depending on [BLOCK]. */
        uint32_t block                 : 5;  /**< [ 24: 20] Resource block enumerated by RVU_BLOCK_ADDR_E. */
        uint32_t reserved_25_31        : 7;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_func_addr_s_s cn; */
};

/**
 * Structure rvu_msix_vec_s
 *
 * RVU MSI-X Vector Structure
 * Format of entries in the RVU MSI-X table region in LLC/DRAM. See
 * RVU_PRIV_PF()_MSIX_CFG.
 */
union bdk_rvu_msix_vec_s
{
    uint64_t u[2];
    struct bdk_rvu_msix_vec_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t addr                  : 64; /**< [ 63:  0] IOVA to use for MSI-X delivery of this vector. Bits \<63:53\> are reserved.
                                                                 Bit \<1:0\> are reserved for alignment. */
#else /* Word 0 - Little Endian */
        uint64_t addr                  : 64; /**< [ 63:  0] IOVA to use for MSI-X delivery of this vector. Bits \<63:53\> are reserved.
                                                                 Bit \<1:0\> are reserved for alignment. */
#endif /* Word 0 - End */
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 1 - Big Endian */
        uint64_t reserved_98_127       : 30;
        uint64_t pend                  : 1;  /**< [ 97: 97] Vector's pending bit in the MSI-X PBA. */
        uint64_t mask                  : 1;  /**< [ 96: 96] When set, no MSI-X interrupts are sent to this vector. */
        uint64_t data                  : 32; /**< [ 95: 64] Data to use for MSI-X delivery of this vector. */
#else /* Word 1 - Little Endian */
        uint64_t data                  : 32; /**< [ 95: 64] Data to use for MSI-X delivery of this vector. */
        uint64_t mask                  : 1;  /**< [ 96: 96] When set, no MSI-X interrupts are sent to this vector. */
        uint64_t pend                  : 1;  /**< [ 97: 97] Vector's pending bit in the MSI-X PBA. */
        uint64_t reserved_98_127       : 30;
#endif /* Word 1 - End */
    } s;
    /* struct bdk_rvu_msix_vec_s_s cn; */
};

/**
 * Structure rvu_pf_func_s
 *
 * RVU PF Function Identification Structure
 * Identifies an RVU PF/VF, and format of *_PRIV_LF()_CFG[PF_FUNC] in RVU
 * resource blocks, e.g. NPA_PRIV_LF()_CFG[PF_FUNC].
 *
 * Internal:
 * Also used for PF/VF identification on inter-coprocessor hardware
 * interfaces (NPA, SSO, CPT, ...).
 */
union bdk_rvu_pf_func_s
{
    uint32_t u;
    struct bdk_rvu_pf_func_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_16_31        : 16;
        uint32_t pf                    : 6;  /**< [ 15: 10] RVU PF number. */
        uint32_t func                  : 10; /**< [  9:  0] Function within [PF]; 0 for the PF itself, else VF number plus 1. */
#else /* Word 0 - Little Endian */
        uint32_t func                  : 10; /**< [  9:  0] Function within [PF]; 0 for the PF itself, else VF number plus 1. */
        uint32_t pf                    : 6;  /**< [ 15: 10] RVU PF number. */
        uint32_t reserved_16_31        : 16;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_func_s_s cn; */
};

/**
 * Register (RVU_PF_BAR0) rvu_af_afpf#_mbox#
 *
 * RVU Admin Function AF/PF Mailbox Registers
 */
union bdk_rvu_af_afpfx_mboxx
{
    uint64_t u;
    struct bdk_rvu_af_afpfx_mboxx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W/H) Mailbox data. These AF registers access the 16-byte-per-PF PF/AF
                                                                 mailbox.  Each corresponding PF may access the same storage using
                                                                 RVU_PF_PFAF_MBOX(). MBOX(0) is typically used for AF to PF
                                                                 signaling, MBOX(1) for PF to AF.
                                                                 Writing RVU_AF_AFPF()_MBOX(0) (but not RVU_PF_PFAF_MBOX(0)) will
                                                                 set the corresponding
                                                                 RVU_PF_INT[MBOX] which if appropriately enabled will send an
                                                                 interrupt to the PF. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W/H) Mailbox data. These AF registers access the 16-byte-per-PF PF/AF
                                                                 mailbox.  Each corresponding PF may access the same storage using
                                                                 RVU_PF_PFAF_MBOX(). MBOX(0) is typically used for AF to PF
                                                                 signaling, MBOX(1) for PF to AF.
                                                                 Writing RVU_AF_AFPF()_MBOX(0) (but not RVU_PF_PFAF_MBOX(0)) will
                                                                 set the corresponding
                                                                 RVU_PF_INT[MBOX] which if appropriately enabled will send an
                                                                 interrupt to the PF. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_afpfx_mboxx_s cn; */
};
typedef union bdk_rvu_af_afpfx_mboxx bdk_rvu_af_afpfx_mboxx_t;

static inline uint64_t BDK_RVU_AF_AFPFX_MBOXX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_AFPFX_MBOXX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=15) && (b<=1)))
        return 0x850000002000ll + 0x10ll * ((a) & 0xf) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("RVU_AF_AFPFX_MBOXX", 2, a, b, 0, 0);
}

#define typedef_BDK_RVU_AF_AFPFX_MBOXX(a,b) bdk_rvu_af_afpfx_mboxx_t
#define bustype_BDK_RVU_AF_AFPFX_MBOXX(a,b) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_AFPFX_MBOXX(a,b) "RVU_AF_AFPFX_MBOXX"
#define device_bar_BDK_RVU_AF_AFPFX_MBOXX(a,b) 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_AFPFX_MBOXX(a,b) (a)
#define arguments_BDK_RVU_AF_AFPFX_MBOXX(a,b) (a),(b),-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_blk_rst
 *
 * RVU Master Admin Function Block Reset Register
 */
union bdk_rvu_af_blk_rst
{
    uint64_t u;
    struct bdk_rvu_af_blk_rst_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t busy                  : 1;  /**< [  1:  1](RO/H) When one, RVUM is busy completing reset. No access except the reading of this
                                                                 bit should occur to RVUM until this is clear. */
        uint64_t rst                   : 1;  /**< [  0:  0](WO) Write one to reset RVUM, except for privileged AF registers (RVU_PRIV_*).
                                                                 Software must ensure that all RVUM activity is quiesced before writing one. */
#else /* Word 0 - Little Endian */
        uint64_t rst                   : 1;  /**< [  0:  0](WO) Write one to reset RVUM, except for privileged AF registers (RVU_PRIV_*).
                                                                 Software must ensure that all RVUM activity is quiesced before writing one. */
        uint64_t busy                  : 1;  /**< [  1:  1](RO/H) When one, RVUM is busy completing reset. No access except the reading of this
                                                                 bit should occur to RVUM until this is clear. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_blk_rst_s cn; */
};
typedef union bdk_rvu_af_blk_rst bdk_rvu_af_blk_rst_t;

#define BDK_RVU_AF_BLK_RST BDK_RVU_AF_BLK_RST_FUNC()
static inline uint64_t BDK_RVU_AF_BLK_RST_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_BLK_RST_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000000030ll;
    __bdk_csr_fatal("RVU_AF_BLK_RST", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_BLK_RST bdk_rvu_af_blk_rst_t
#define bustype_BDK_RVU_AF_BLK_RST BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_BLK_RST "RVU_AF_BLK_RST"
#define device_bar_BDK_RVU_AF_BLK_RST 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_BLK_RST 0
#define arguments_BDK_RVU_AF_BLK_RST -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_eco
 *
 * INTERNAL: RVU Admin Function ECO Register
 */
union bdk_rvu_af_eco
{
    uint64_t u;
    struct bdk_rvu_af_eco_s
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
    /* struct bdk_rvu_af_eco_s cn; */
};
typedef union bdk_rvu_af_eco bdk_rvu_af_eco_t;

#define BDK_RVU_AF_ECO BDK_RVU_AF_ECO_FUNC()
static inline uint64_t BDK_RVU_AF_ECO_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_ECO_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000000020ll;
    __bdk_csr_fatal("RVU_AF_ECO", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_ECO bdk_rvu_af_eco_t
#define bustype_BDK_RVU_AF_ECO BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_ECO "RVU_AF_ECO"
#define device_bar_BDK_RVU_AF_ECO 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_ECO 0
#define arguments_BDK_RVU_AF_ECO -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_gen_int
 *
 * RVU Admin Function General Interrupt Register
 * This register contains General interrupt summary bits.
 */
union bdk_rvu_af_gen_int
{
    uint64_t u;
    struct bdk_rvu_af_gen_int_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t msix_fault            : 1;  /**< [  1:  1](R/W1C/H) Received MSIX-X table read response with fault data */
        uint64_t unmapped              : 1;  /**< [  0:  0](R/W1C/H) Received a register read or write request to an unmapped or disabled PF or
                                                                 VF. Specifically:
                                                                 * A PF/VF  BAR2 access in a PF whose RVU_PRIV_PF()_CFG[ENA] is
                                                                 clear.
                                                                 * A VF BAR2 access to a VF number that is greater than or equal to the
                                                                 associated PF's RVU_PRIV_PF()_CFG[NVF]. */
#else /* Word 0 - Little Endian */
        uint64_t unmapped              : 1;  /**< [  0:  0](R/W1C/H) Received a register read or write request to an unmapped or disabled PF or
                                                                 VF. Specifically:
                                                                 * A PF/VF  BAR2 access in a PF whose RVU_PRIV_PF()_CFG[ENA] is
                                                                 clear.
                                                                 * A VF BAR2 access to a VF number that is greater than or equal to the
                                                                 associated PF's RVU_PRIV_PF()_CFG[NVF]. */
        uint64_t msix_fault            : 1;  /**< [  1:  1](R/W1C/H) Received MSIX-X table read response with fault data */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_gen_int_s cn; */
};
typedef union bdk_rvu_af_gen_int bdk_rvu_af_gen_int_t;

#define BDK_RVU_AF_GEN_INT BDK_RVU_AF_GEN_INT_FUNC()
static inline uint64_t BDK_RVU_AF_GEN_INT_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_GEN_INT_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000000120ll;
    __bdk_csr_fatal("RVU_AF_GEN_INT", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_GEN_INT bdk_rvu_af_gen_int_t
#define bustype_BDK_RVU_AF_GEN_INT BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_GEN_INT "RVU_AF_GEN_INT"
#define device_bar_BDK_RVU_AF_GEN_INT 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_GEN_INT 0
#define arguments_BDK_RVU_AF_GEN_INT -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_gen_int_ena_w1c
 *
 * RVU Admin Function General Interrupt Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_rvu_af_gen_int_ena_w1c
{
    uint64_t u;
    struct bdk_rvu_af_gen_int_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t msix_fault            : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for RVU_AF_GEN_INT[MSIX_FAULT]. */
        uint64_t unmapped              : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for RVU_AF_GEN_INT[UNMAPPED]. */
#else /* Word 0 - Little Endian */
        uint64_t unmapped              : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for RVU_AF_GEN_INT[UNMAPPED]. */
        uint64_t msix_fault            : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for RVU_AF_GEN_INT[MSIX_FAULT]. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_gen_int_ena_w1c_s cn; */
};
typedef union bdk_rvu_af_gen_int_ena_w1c bdk_rvu_af_gen_int_ena_w1c_t;

#define BDK_RVU_AF_GEN_INT_ENA_W1C BDK_RVU_AF_GEN_INT_ENA_W1C_FUNC()
static inline uint64_t BDK_RVU_AF_GEN_INT_ENA_W1C_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_GEN_INT_ENA_W1C_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000000138ll;
    __bdk_csr_fatal("RVU_AF_GEN_INT_ENA_W1C", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_GEN_INT_ENA_W1C bdk_rvu_af_gen_int_ena_w1c_t
#define bustype_BDK_RVU_AF_GEN_INT_ENA_W1C BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_GEN_INT_ENA_W1C "RVU_AF_GEN_INT_ENA_W1C"
#define device_bar_BDK_RVU_AF_GEN_INT_ENA_W1C 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_GEN_INT_ENA_W1C 0
#define arguments_BDK_RVU_AF_GEN_INT_ENA_W1C -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_gen_int_ena_w1s
 *
 * RVU Admin Function General Interrupt Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_rvu_af_gen_int_ena_w1s
{
    uint64_t u;
    struct bdk_rvu_af_gen_int_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t msix_fault            : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for RVU_AF_GEN_INT[MSIX_FAULT]. */
        uint64_t unmapped              : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for RVU_AF_GEN_INT[UNMAPPED]. */
#else /* Word 0 - Little Endian */
        uint64_t unmapped              : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for RVU_AF_GEN_INT[UNMAPPED]. */
        uint64_t msix_fault            : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for RVU_AF_GEN_INT[MSIX_FAULT]. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_gen_int_ena_w1s_s cn; */
};
typedef union bdk_rvu_af_gen_int_ena_w1s bdk_rvu_af_gen_int_ena_w1s_t;

#define BDK_RVU_AF_GEN_INT_ENA_W1S BDK_RVU_AF_GEN_INT_ENA_W1S_FUNC()
static inline uint64_t BDK_RVU_AF_GEN_INT_ENA_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_GEN_INT_ENA_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000000130ll;
    __bdk_csr_fatal("RVU_AF_GEN_INT_ENA_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_GEN_INT_ENA_W1S bdk_rvu_af_gen_int_ena_w1s_t
#define bustype_BDK_RVU_AF_GEN_INT_ENA_W1S BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_GEN_INT_ENA_W1S "RVU_AF_GEN_INT_ENA_W1S"
#define device_bar_BDK_RVU_AF_GEN_INT_ENA_W1S 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_GEN_INT_ENA_W1S 0
#define arguments_BDK_RVU_AF_GEN_INT_ENA_W1S -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_gen_int_w1s
 *
 * RVU Admin Function General Interrupt Set Register
 * This register sets interrupt bits.
 */
union bdk_rvu_af_gen_int_w1s
{
    uint64_t u;
    struct bdk_rvu_af_gen_int_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t msix_fault            : 1;  /**< [  1:  1](R/W1S/H) Reads or sets RVU_AF_GEN_INT[MSIX_FAULT]. */
        uint64_t unmapped              : 1;  /**< [  0:  0](R/W1S/H) Reads or sets RVU_AF_GEN_INT[UNMAPPED]. */
#else /* Word 0 - Little Endian */
        uint64_t unmapped              : 1;  /**< [  0:  0](R/W1S/H) Reads or sets RVU_AF_GEN_INT[UNMAPPED]. */
        uint64_t msix_fault            : 1;  /**< [  1:  1](R/W1S/H) Reads or sets RVU_AF_GEN_INT[MSIX_FAULT]. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_gen_int_w1s_s cn; */
};
typedef union bdk_rvu_af_gen_int_w1s bdk_rvu_af_gen_int_w1s_t;

#define BDK_RVU_AF_GEN_INT_W1S BDK_RVU_AF_GEN_INT_W1S_FUNC()
static inline uint64_t BDK_RVU_AF_GEN_INT_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_GEN_INT_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000000128ll;
    __bdk_csr_fatal("RVU_AF_GEN_INT_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_GEN_INT_W1S bdk_rvu_af_gen_int_w1s_t
#define bustype_BDK_RVU_AF_GEN_INT_W1S BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_GEN_INT_W1S "RVU_AF_GEN_INT_W1S"
#define device_bar_BDK_RVU_AF_GEN_INT_W1S 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_GEN_INT_W1S 0
#define arguments_BDK_RVU_AF_GEN_INT_W1S -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_hwvf_rst
 *
 * RVU Admin Function Hardware VF Soft Reset Register
 */
union bdk_rvu_af_hwvf_rst
{
    uint64_t u;
    struct bdk_rvu_af_hwvf_rst_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_13_63        : 51;
        uint64_t exec                  : 1;  /**< [ 12: 12](R/W1S/H) Execute HWVF soft reset. When software writes a one to set this bit, hardware
                                                                 resets the RVUM resources of the hardware VF selected by [HWVF] and the
                                                                 associated MSI-X table in LLC/DRAM specified by
                                                                 RVU_PRIV_PF()_MSIX_CFG[VF_MSIXT_OFFSET,VF_MSIXT_SIZEM1].
                                                                 Hardware clears this bit when done. */
        uint64_t reserved_8_11         : 4;
        uint64_t hwvf                  : 8;  /**< [  7:  0](R/W) Hardware VF that is reset when [EXEC] is set. */
#else /* Word 0 - Little Endian */
        uint64_t hwvf                  : 8;  /**< [  7:  0](R/W) Hardware VF that is reset when [EXEC] is set. */
        uint64_t reserved_8_11         : 4;
        uint64_t exec                  : 1;  /**< [ 12: 12](R/W1S/H) Execute HWVF soft reset. When software writes a one to set this bit, hardware
                                                                 resets the RVUM resources of the hardware VF selected by [HWVF] and the
                                                                 associated MSI-X table in LLC/DRAM specified by
                                                                 RVU_PRIV_PF()_MSIX_CFG[VF_MSIXT_OFFSET,VF_MSIXT_SIZEM1].
                                                                 Hardware clears this bit when done. */
        uint64_t reserved_13_63        : 51;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_hwvf_rst_s cn; */
};
typedef union bdk_rvu_af_hwvf_rst bdk_rvu_af_hwvf_rst_t;

#define BDK_RVU_AF_HWVF_RST BDK_RVU_AF_HWVF_RST_FUNC()
static inline uint64_t BDK_RVU_AF_HWVF_RST_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_HWVF_RST_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000002850ll;
    __bdk_csr_fatal("RVU_AF_HWVF_RST", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_HWVF_RST bdk_rvu_af_hwvf_rst_t
#define bustype_BDK_RVU_AF_HWVF_RST BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_HWVF_RST "RVU_AF_HWVF_RST"
#define device_bar_BDK_RVU_AF_HWVF_RST 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_HWVF_RST 0
#define arguments_BDK_RVU_AF_HWVF_RST -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_msixtr_base
 *
 * RVU Admin Function MSI-X Table Region Base-Address Register
 */
union bdk_rvu_af_msixtr_base
{
    uint64_t u;
    struct bdk_rvu_af_msixtr_base_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_53_63        : 11;
        uint64_t addr                  : 46; /**< [ 52:  7](R/W) Base IOVA of MSI-X table region in LLC/DRAM. IOVA bits \<6:0\> are always zero.
                                                                 See RVU_PRIV_PF()_MSIX_CFG. */
        uint64_t reserved_0_6          : 7;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_6          : 7;
        uint64_t addr                  : 46; /**< [ 52:  7](R/W) Base IOVA of MSI-X table region in LLC/DRAM. IOVA bits \<6:0\> are always zero.
                                                                 See RVU_PRIV_PF()_MSIX_CFG. */
        uint64_t reserved_53_63        : 11;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_msixtr_base_s cn; */
};
typedef union bdk_rvu_af_msixtr_base bdk_rvu_af_msixtr_base_t;

#define BDK_RVU_AF_MSIXTR_BASE BDK_RVU_AF_MSIXTR_BASE_FUNC()
static inline uint64_t BDK_RVU_AF_MSIXTR_BASE_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_MSIXTR_BASE_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000000010ll;
    __bdk_csr_fatal("RVU_AF_MSIXTR_BASE", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_MSIXTR_BASE bdk_rvu_af_msixtr_base_t
#define bustype_BDK_RVU_AF_MSIXTR_BASE BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_MSIXTR_BASE "RVU_AF_MSIXTR_BASE"
#define device_bar_BDK_RVU_AF_MSIXTR_BASE 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_MSIXTR_BASE 0
#define arguments_BDK_RVU_AF_MSIXTR_BASE -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pf_bar4_addr
 *
 * RVU Admin Function PF BAR4 Address Registers
 */
union bdk_rvu_af_pf_bar4_addr
{
    uint64_t u;
    struct bdk_rvu_af_pf_bar4_addr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t addr                  : 48; /**< [ 63: 16](R/W) Programmable base address of up to 16 consecutive 64 KB
                                                                 pages in DRAM (one per PF). May be used as PF/AF mailbox memory in addition to
                                                                 RVU_AF_AFPF()_MBOX()/RVU_PF_PFAF_MBOX().
                                                                 Provides PCC_EA_ENTRY_S[BASEH,BASEL] value advertised by PF BAR4's entry in
                                                                 PCCPF_XXX_EA_ENTRY(). */
        uint64_t reserved_0_15         : 16;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_15         : 16;
        uint64_t addr                  : 48; /**< [ 63: 16](R/W) Programmable base address of up to 16 consecutive 64 KB
                                                                 pages in DRAM (one per PF). May be used as PF/AF mailbox memory in addition to
                                                                 RVU_AF_AFPF()_MBOX()/RVU_PF_PFAF_MBOX().
                                                                 Provides PCC_EA_ENTRY_S[BASEH,BASEL] value advertised by PF BAR4's entry in
                                                                 PCCPF_XXX_EA_ENTRY(). */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pf_bar4_addr_s cn; */
};
typedef union bdk_rvu_af_pf_bar4_addr bdk_rvu_af_pf_bar4_addr_t;

#define BDK_RVU_AF_PF_BAR4_ADDR BDK_RVU_AF_PF_BAR4_ADDR_FUNC()
static inline uint64_t BDK_RVU_AF_PF_BAR4_ADDR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PF_BAR4_ADDR_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000000040ll;
    __bdk_csr_fatal("RVU_AF_PF_BAR4_ADDR", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PF_BAR4_ADDR bdk_rvu_af_pf_bar4_addr_t
#define bustype_BDK_RVU_AF_PF_BAR4_ADDR BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PF_BAR4_ADDR "RVU_AF_PF_BAR4_ADDR"
#define device_bar_BDK_RVU_AF_PF_BAR4_ADDR 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PF_BAR4_ADDR 0
#define arguments_BDK_RVU_AF_PF_BAR4_ADDR -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pf_rst
 *
 * RVU Admin Function PF Soft Reset Register
 */
union bdk_rvu_af_pf_rst
{
    uint64_t u;
    struct bdk_rvu_af_pf_rst_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_13_63        : 51;
        uint64_t exec                  : 1;  /**< [ 12: 12](R/W1S/H) Execute PF soft reset. When software writes a one to set this bit, hardware
                                                                 resets the RVUM resources of the physical function selected by [PF] and the
                                                                 associated MSI-X table in LLC/DRAM specified by
                                                                 RVU_PRIV_PF()_MSIX_CFG[PF_MSIXT_OFFSET,PF_MSIXT_SIZEM1].
                                                                 Hardware clears this bit when done.
                                                                 Note this does not reset HWVFs which are mapped to the PF. */
        uint64_t reserved_4_11         : 8;
        uint64_t pf                    : 4;  /**< [  3:  0](R/W) Physical function that is reset when [EXEC] is set. */
#else /* Word 0 - Little Endian */
        uint64_t pf                    : 4;  /**< [  3:  0](R/W) Physical function that is reset when [EXEC] is set. */
        uint64_t reserved_4_11         : 8;
        uint64_t exec                  : 1;  /**< [ 12: 12](R/W1S/H) Execute PF soft reset. When software writes a one to set this bit, hardware
                                                                 resets the RVUM resources of the physical function selected by [PF] and the
                                                                 associated MSI-X table in LLC/DRAM specified by
                                                                 RVU_PRIV_PF()_MSIX_CFG[PF_MSIXT_OFFSET,PF_MSIXT_SIZEM1].
                                                                 Hardware clears this bit when done.
                                                                 Note this does not reset HWVFs which are mapped to the PF. */
        uint64_t reserved_13_63        : 51;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pf_rst_s cn; */
};
typedef union bdk_rvu_af_pf_rst bdk_rvu_af_pf_rst_t;

#define BDK_RVU_AF_PF_RST BDK_RVU_AF_PF_RST_FUNC()
static inline uint64_t BDK_RVU_AF_PF_RST_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PF_RST_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000002840ll;
    __bdk_csr_fatal("RVU_AF_PF_RST", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PF_RST bdk_rvu_af_pf_rst_t
#define bustype_BDK_RVU_AF_PF_RST BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PF_RST "RVU_AF_PF_RST"
#define device_bar_BDK_RVU_AF_PF_RST 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PF_RST 0
#define arguments_BDK_RVU_AF_PF_RST -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pfaf_mbox_int
 *
 * RVU Admin Function PF to AF Mailbox Interrupt Registers
 */
union bdk_rvu_af_pfaf_mbox_int
{
    uint64_t u;
    struct bdk_rvu_af_pfaf_mbox_int_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1C/H) Mailbox interrupt bit per PF.
                                                                 Each bit is set when the PF writes to the corresponding
                                                                 RVU_PF_PFAF_MBOX(1) register. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1C/H) Mailbox interrupt bit per PF.
                                                                 Each bit is set when the PF writes to the corresponding
                                                                 RVU_PF_PFAF_MBOX(1) register. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pfaf_mbox_int_s cn; */
};
typedef union bdk_rvu_af_pfaf_mbox_int bdk_rvu_af_pfaf_mbox_int_t;

#define BDK_RVU_AF_PFAF_MBOX_INT BDK_RVU_AF_PFAF_MBOX_INT_FUNC()
static inline uint64_t BDK_RVU_AF_PFAF_MBOX_INT_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PFAF_MBOX_INT_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000002880ll;
    __bdk_csr_fatal("RVU_AF_PFAF_MBOX_INT", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PFAF_MBOX_INT bdk_rvu_af_pfaf_mbox_int_t
#define bustype_BDK_RVU_AF_PFAF_MBOX_INT BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PFAF_MBOX_INT "RVU_AF_PFAF_MBOX_INT"
#define device_bar_BDK_RVU_AF_PFAF_MBOX_INT 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PFAF_MBOX_INT 0
#define arguments_BDK_RVU_AF_PFAF_MBOX_INT -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pfaf_mbox_int_ena_w1c
 *
 * RVU Admin Function PF to AF Mailbox Interrupt Enable Clear Registers
 * This register clears interrupt enable bits.
 */
union bdk_rvu_af_pfaf_mbox_int_ena_w1c
{
    uint64_t u;
    struct bdk_rvu_af_pfaf_mbox_int_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for RVU_AF_PFAF_MBOX_INT[MBOX]. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for RVU_AF_PFAF_MBOX_INT[MBOX]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pfaf_mbox_int_ena_w1c_s cn; */
};
typedef union bdk_rvu_af_pfaf_mbox_int_ena_w1c bdk_rvu_af_pfaf_mbox_int_ena_w1c_t;

#define BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1C BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1C_FUNC()
static inline uint64_t BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1C_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1C_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000002898ll;
    __bdk_csr_fatal("RVU_AF_PFAF_MBOX_INT_ENA_W1C", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1C bdk_rvu_af_pfaf_mbox_int_ena_w1c_t
#define bustype_BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1C BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1C "RVU_AF_PFAF_MBOX_INT_ENA_W1C"
#define device_bar_BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1C 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1C 0
#define arguments_BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1C -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pfaf_mbox_int_ena_w1s
 *
 * RVU Admin Function PF to AF Mailbox Interrupt Enable Set Registers
 * This register sets interrupt enable bits.
 */
union bdk_rvu_af_pfaf_mbox_int_ena_w1s
{
    uint64_t u;
    struct bdk_rvu_af_pfaf_mbox_int_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for RVU_AF_PFAF_MBOX_INT[MBOX]. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for RVU_AF_PFAF_MBOX_INT[MBOX]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pfaf_mbox_int_ena_w1s_s cn; */
};
typedef union bdk_rvu_af_pfaf_mbox_int_ena_w1s bdk_rvu_af_pfaf_mbox_int_ena_w1s_t;

#define BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1S BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1S_FUNC()
static inline uint64_t BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000002890ll;
    __bdk_csr_fatal("RVU_AF_PFAF_MBOX_INT_ENA_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1S bdk_rvu_af_pfaf_mbox_int_ena_w1s_t
#define bustype_BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1S BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1S "RVU_AF_PFAF_MBOX_INT_ENA_W1S"
#define device_bar_BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1S 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1S 0
#define arguments_BDK_RVU_AF_PFAF_MBOX_INT_ENA_W1S -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pfaf_mbox_int_w1s
 *
 * RVU Admin Function PF to AF Mailbox Interrupt Set Registers
 * This register sets interrupt bits.
 */
union bdk_rvu_af_pfaf_mbox_int_w1s
{
    uint64_t u;
    struct bdk_rvu_af_pfaf_mbox_int_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_AF_PFAF_MBOX_INT[MBOX]. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_AF_PFAF_MBOX_INT[MBOX]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pfaf_mbox_int_w1s_s cn; */
};
typedef union bdk_rvu_af_pfaf_mbox_int_w1s bdk_rvu_af_pfaf_mbox_int_w1s_t;

#define BDK_RVU_AF_PFAF_MBOX_INT_W1S BDK_RVU_AF_PFAF_MBOX_INT_W1S_FUNC()
static inline uint64_t BDK_RVU_AF_PFAF_MBOX_INT_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PFAF_MBOX_INT_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000002888ll;
    __bdk_csr_fatal("RVU_AF_PFAF_MBOX_INT_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PFAF_MBOX_INT_W1S bdk_rvu_af_pfaf_mbox_int_w1s_t
#define bustype_BDK_RVU_AF_PFAF_MBOX_INT_W1S BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PFAF_MBOX_INT_W1S "RVU_AF_PFAF_MBOX_INT_W1S"
#define device_bar_BDK_RVU_AF_PFAF_MBOX_INT_W1S 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PFAF_MBOX_INT_W1S 0
#define arguments_BDK_RVU_AF_PFAF_MBOX_INT_W1S -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pfflr_int
 *
 * RVU Admin Function PF Function Level Reset Interrupt Registers
 */
union bdk_rvu_af_pfflr_int
{
    uint64_t u;
    struct bdk_rvu_af_pfflr_int_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1C/H) FLR interrupt bit per PF.

                                                                 If RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set, each bit is set along with
                                                                 the corresponding bit in RVU_AF_PFTRPEND when function level reset is
                                                                 initiated for the associated PF, i.e. a one is written to
                                                                 PCCPF_XXX_E_DEV_CTL[BCR_FLR]. */
#else /* Word 0 - Little Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1C/H) FLR interrupt bit per PF.

                                                                 If RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set, each bit is set along with
                                                                 the corresponding bit in RVU_AF_PFTRPEND when function level reset is
                                                                 initiated for the associated PF, i.e. a one is written to
                                                                 PCCPF_XXX_E_DEV_CTL[BCR_FLR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pfflr_int_s cn; */
};
typedef union bdk_rvu_af_pfflr_int bdk_rvu_af_pfflr_int_t;

#define BDK_RVU_AF_PFFLR_INT BDK_RVU_AF_PFFLR_INT_FUNC()
static inline uint64_t BDK_RVU_AF_PFFLR_INT_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PFFLR_INT_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x8500000028a0ll;
    __bdk_csr_fatal("RVU_AF_PFFLR_INT", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PFFLR_INT bdk_rvu_af_pfflr_int_t
#define bustype_BDK_RVU_AF_PFFLR_INT BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PFFLR_INT "RVU_AF_PFFLR_INT"
#define device_bar_BDK_RVU_AF_PFFLR_INT 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PFFLR_INT 0
#define arguments_BDK_RVU_AF_PFFLR_INT -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pfflr_int_ena_w1c
 *
 * RVU Admin Function PF Function Level Reset Interrupt Enable Clear Registers
 * This register clears interrupt enable bits.
 */
union bdk_rvu_af_pfflr_int_ena_w1c
{
    uint64_t u;
    struct bdk_rvu_af_pfflr_int_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for RVU_AF_PFFLR_INT[FLR]. */
#else /* Word 0 - Little Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for RVU_AF_PFFLR_INT[FLR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pfflr_int_ena_w1c_s cn; */
};
typedef union bdk_rvu_af_pfflr_int_ena_w1c bdk_rvu_af_pfflr_int_ena_w1c_t;

#define BDK_RVU_AF_PFFLR_INT_ENA_W1C BDK_RVU_AF_PFFLR_INT_ENA_W1C_FUNC()
static inline uint64_t BDK_RVU_AF_PFFLR_INT_ENA_W1C_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PFFLR_INT_ENA_W1C_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x8500000028b8ll;
    __bdk_csr_fatal("RVU_AF_PFFLR_INT_ENA_W1C", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PFFLR_INT_ENA_W1C bdk_rvu_af_pfflr_int_ena_w1c_t
#define bustype_BDK_RVU_AF_PFFLR_INT_ENA_W1C BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PFFLR_INT_ENA_W1C "RVU_AF_PFFLR_INT_ENA_W1C"
#define device_bar_BDK_RVU_AF_PFFLR_INT_ENA_W1C 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PFFLR_INT_ENA_W1C 0
#define arguments_BDK_RVU_AF_PFFLR_INT_ENA_W1C -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pfflr_int_ena_w1s
 *
 * RVU Admin Function PF Function Level Reset Interrupt Enable Set Registers
 * This register sets interrupt enable bits.
 */
union bdk_rvu_af_pfflr_int_ena_w1s
{
    uint64_t u;
    struct bdk_rvu_af_pfflr_int_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for RVU_AF_PFFLR_INT[FLR]. */
#else /* Word 0 - Little Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for RVU_AF_PFFLR_INT[FLR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pfflr_int_ena_w1s_s cn; */
};
typedef union bdk_rvu_af_pfflr_int_ena_w1s bdk_rvu_af_pfflr_int_ena_w1s_t;

#define BDK_RVU_AF_PFFLR_INT_ENA_W1S BDK_RVU_AF_PFFLR_INT_ENA_W1S_FUNC()
static inline uint64_t BDK_RVU_AF_PFFLR_INT_ENA_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PFFLR_INT_ENA_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x8500000028b0ll;
    __bdk_csr_fatal("RVU_AF_PFFLR_INT_ENA_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PFFLR_INT_ENA_W1S bdk_rvu_af_pfflr_int_ena_w1s_t
#define bustype_BDK_RVU_AF_PFFLR_INT_ENA_W1S BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PFFLR_INT_ENA_W1S "RVU_AF_PFFLR_INT_ENA_W1S"
#define device_bar_BDK_RVU_AF_PFFLR_INT_ENA_W1S 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PFFLR_INT_ENA_W1S 0
#define arguments_BDK_RVU_AF_PFFLR_INT_ENA_W1S -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pfflr_int_w1s
 *
 * RVU Admin Function PF Function Level Reset Interrupt Set Registers
 * This register sets interrupt bits.
 */
union bdk_rvu_af_pfflr_int_w1s
{
    uint64_t u;
    struct bdk_rvu_af_pfflr_int_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_AF_PFFLR_INT[FLR]. */
#else /* Word 0 - Little Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_AF_PFFLR_INT[FLR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pfflr_int_w1s_s cn; */
};
typedef union bdk_rvu_af_pfflr_int_w1s bdk_rvu_af_pfflr_int_w1s_t;

#define BDK_RVU_AF_PFFLR_INT_W1S BDK_RVU_AF_PFFLR_INT_W1S_FUNC()
static inline uint64_t BDK_RVU_AF_PFFLR_INT_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PFFLR_INT_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x8500000028a8ll;
    __bdk_csr_fatal("RVU_AF_PFFLR_INT_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PFFLR_INT_W1S bdk_rvu_af_pfflr_int_w1s_t
#define bustype_BDK_RVU_AF_PFFLR_INT_W1S BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PFFLR_INT_W1S "RVU_AF_PFFLR_INT_W1S"
#define device_bar_BDK_RVU_AF_PFFLR_INT_W1S 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PFFLR_INT_W1S 0
#define arguments_BDK_RVU_AF_PFFLR_INT_W1S -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pfme_int
 *
 * RVU Admin Function PF Bus Master Enable Interrupt Registers
 */
union bdk_rvu_af_pfme_int
{
    uint64_t u;
    struct bdk_rvu_af_pfme_int_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1C/H) Master enable interrupt bit per PF.
                                                                 A device-dependent AF driver typically uses these bits to handle state
                                                                 changes to PCCPF_XXX_CMD[ME], which are typically modified by
                                                                 non-device-dependent software only.

                                                                 If RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set, each bit is set when the
                                                                 corresponding PCCPF_XXX_CMD[ME] bit is either set or cleared for the
                                                                 associated PF. The corresponding bit in RVU_AF_PFME_STATUS returns the
                                                                 current value of PCCPF_XXX_CMD[ME].

                                                                 Note that if RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set, the corresponding
                                                                 bit in RVU_AF_PFTRPEND is also set when PCCPF_XXX_CMD[ME] is set, but not
                                                                 when PCCPF_XXX_CMD[ME] is cleared. */
#else /* Word 0 - Little Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1C/H) Master enable interrupt bit per PF.
                                                                 A device-dependent AF driver typically uses these bits to handle state
                                                                 changes to PCCPF_XXX_CMD[ME], which are typically modified by
                                                                 non-device-dependent software only.

                                                                 If RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set, each bit is set when the
                                                                 corresponding PCCPF_XXX_CMD[ME] bit is either set or cleared for the
                                                                 associated PF. The corresponding bit in RVU_AF_PFME_STATUS returns the
                                                                 current value of PCCPF_XXX_CMD[ME].

                                                                 Note that if RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set, the corresponding
                                                                 bit in RVU_AF_PFTRPEND is also set when PCCPF_XXX_CMD[ME] is set, but not
                                                                 when PCCPF_XXX_CMD[ME] is cleared. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pfme_int_s cn; */
};
typedef union bdk_rvu_af_pfme_int bdk_rvu_af_pfme_int_t;

#define BDK_RVU_AF_PFME_INT BDK_RVU_AF_PFME_INT_FUNC()
static inline uint64_t BDK_RVU_AF_PFME_INT_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PFME_INT_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x8500000028c0ll;
    __bdk_csr_fatal("RVU_AF_PFME_INT", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PFME_INT bdk_rvu_af_pfme_int_t
#define bustype_BDK_RVU_AF_PFME_INT BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PFME_INT "RVU_AF_PFME_INT"
#define device_bar_BDK_RVU_AF_PFME_INT 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PFME_INT 0
#define arguments_BDK_RVU_AF_PFME_INT -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pfme_int_ena_w1c
 *
 * RVU Admin Function PF Bus Master Enable Interrupt Enable Clear Registers
 * This register clears interrupt enable bits.
 */
union bdk_rvu_af_pfme_int_ena_w1c
{
    uint64_t u;
    struct bdk_rvu_af_pfme_int_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for RVU_AF_PFME_INT[ME]. */
#else /* Word 0 - Little Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for RVU_AF_PFME_INT[ME]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pfme_int_ena_w1c_s cn; */
};
typedef union bdk_rvu_af_pfme_int_ena_w1c bdk_rvu_af_pfme_int_ena_w1c_t;

#define BDK_RVU_AF_PFME_INT_ENA_W1C BDK_RVU_AF_PFME_INT_ENA_W1C_FUNC()
static inline uint64_t BDK_RVU_AF_PFME_INT_ENA_W1C_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PFME_INT_ENA_W1C_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x8500000028d8ll;
    __bdk_csr_fatal("RVU_AF_PFME_INT_ENA_W1C", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PFME_INT_ENA_W1C bdk_rvu_af_pfme_int_ena_w1c_t
#define bustype_BDK_RVU_AF_PFME_INT_ENA_W1C BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PFME_INT_ENA_W1C "RVU_AF_PFME_INT_ENA_W1C"
#define device_bar_BDK_RVU_AF_PFME_INT_ENA_W1C 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PFME_INT_ENA_W1C 0
#define arguments_BDK_RVU_AF_PFME_INT_ENA_W1C -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pfme_int_ena_w1s
 *
 * RVU Admin Function PF Bus Master Enable Interrupt Enable Set Registers
 * This register sets interrupt enable bits.
 */
union bdk_rvu_af_pfme_int_ena_w1s
{
    uint64_t u;
    struct bdk_rvu_af_pfme_int_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for RVU_AF_PFME_INT[ME]. */
#else /* Word 0 - Little Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for RVU_AF_PFME_INT[ME]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pfme_int_ena_w1s_s cn; */
};
typedef union bdk_rvu_af_pfme_int_ena_w1s bdk_rvu_af_pfme_int_ena_w1s_t;

#define BDK_RVU_AF_PFME_INT_ENA_W1S BDK_RVU_AF_PFME_INT_ENA_W1S_FUNC()
static inline uint64_t BDK_RVU_AF_PFME_INT_ENA_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PFME_INT_ENA_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x8500000028d0ll;
    __bdk_csr_fatal("RVU_AF_PFME_INT_ENA_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PFME_INT_ENA_W1S bdk_rvu_af_pfme_int_ena_w1s_t
#define bustype_BDK_RVU_AF_PFME_INT_ENA_W1S BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PFME_INT_ENA_W1S "RVU_AF_PFME_INT_ENA_W1S"
#define device_bar_BDK_RVU_AF_PFME_INT_ENA_W1S 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PFME_INT_ENA_W1S 0
#define arguments_BDK_RVU_AF_PFME_INT_ENA_W1S -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pfme_int_w1s
 *
 * RVU Admin Function PF Bus Master Enable Interrupt Set Registers
 * This register sets interrupt bits.
 */
union bdk_rvu_af_pfme_int_w1s
{
    uint64_t u;
    struct bdk_rvu_af_pfme_int_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_AF_PFME_INT[ME]. */
#else /* Word 0 - Little Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_AF_PFME_INT[ME]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pfme_int_w1s_s cn; */
};
typedef union bdk_rvu_af_pfme_int_w1s bdk_rvu_af_pfme_int_w1s_t;

#define BDK_RVU_AF_PFME_INT_W1S BDK_RVU_AF_PFME_INT_W1S_FUNC()
static inline uint64_t BDK_RVU_AF_PFME_INT_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PFME_INT_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x8500000028c8ll;
    __bdk_csr_fatal("RVU_AF_PFME_INT_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PFME_INT_W1S bdk_rvu_af_pfme_int_w1s_t
#define bustype_BDK_RVU_AF_PFME_INT_W1S BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PFME_INT_W1S "RVU_AF_PFME_INT_W1S"
#define device_bar_BDK_RVU_AF_PFME_INT_W1S 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PFME_INT_W1S 0
#define arguments_BDK_RVU_AF_PFME_INT_W1S -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pfme_status
 *
 * RVU Admin Function PF Bus Master Enable Status Registers
 */
union bdk_rvu_af_pfme_status
{
    uint64_t u;
    struct bdk_rvu_af_pfme_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t me                    : 64; /**< [ 63:  0](RO/H) Bus master enable bit per PF. Each bit returns the PF's
                                                                 PCCPF_XXX_CMD[ME] value. */
#else /* Word 0 - Little Endian */
        uint64_t me                    : 64; /**< [ 63:  0](RO/H) Bus master enable bit per PF. Each bit returns the PF's
                                                                 PCCPF_XXX_CMD[ME] value. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pfme_status_s cn; */
};
typedef union bdk_rvu_af_pfme_status bdk_rvu_af_pfme_status_t;

#define BDK_RVU_AF_PFME_STATUS BDK_RVU_AF_PFME_STATUS_FUNC()
static inline uint64_t BDK_RVU_AF_PFME_STATUS_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PFME_STATUS_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000002800ll;
    __bdk_csr_fatal("RVU_AF_PFME_STATUS", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PFME_STATUS bdk_rvu_af_pfme_status_t
#define bustype_BDK_RVU_AF_PFME_STATUS BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PFME_STATUS "RVU_AF_PFME_STATUS"
#define device_bar_BDK_RVU_AF_PFME_STATUS 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PFME_STATUS 0
#define arguments_BDK_RVU_AF_PFME_STATUS -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pftrpend
 *
 * RVU Admin Function PF Transaction Pending Registers
 */
union bdk_rvu_af_pftrpend
{
    uint64_t u;
    struct bdk_rvu_af_pftrpend_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t trpend                : 64; /**< [ 63:  0](R/W1C/H) Transaction pending bit per PF.

                                                                 A PF's bit is set when RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set and:
                                                                 * A one is written to the corresponding PCCPF_XXX_E_DEV_CTL[BCR_FLR], or
                                                                 * PCCPF_XXX_CMD[ME] is set or cleared.

                                                                 When a PF's bit is set, forces the corresponding
                                                                 PCCPF_XXX_E_DEV_CTL[TRPEND] to be set.

                                                                 Software (typically a device-dependent AF driver) can clear the bit by
                                                                 writing a 1. */
#else /* Word 0 - Little Endian */
        uint64_t trpend                : 64; /**< [ 63:  0](R/W1C/H) Transaction pending bit per PF.

                                                                 A PF's bit is set when RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set and:
                                                                 * A one is written to the corresponding PCCPF_XXX_E_DEV_CTL[BCR_FLR], or
                                                                 * PCCPF_XXX_CMD[ME] is set or cleared.

                                                                 When a PF's bit is set, forces the corresponding
                                                                 PCCPF_XXX_E_DEV_CTL[TRPEND] to be set.

                                                                 Software (typically a device-dependent AF driver) can clear the bit by
                                                                 writing a 1. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pftrpend_s cn; */
};
typedef union bdk_rvu_af_pftrpend bdk_rvu_af_pftrpend_t;

#define BDK_RVU_AF_PFTRPEND BDK_RVU_AF_PFTRPEND_FUNC()
static inline uint64_t BDK_RVU_AF_PFTRPEND_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PFTRPEND_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000002810ll;
    __bdk_csr_fatal("RVU_AF_PFTRPEND", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PFTRPEND bdk_rvu_af_pftrpend_t
#define bustype_BDK_RVU_AF_PFTRPEND BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PFTRPEND "RVU_AF_PFTRPEND"
#define device_bar_BDK_RVU_AF_PFTRPEND 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PFTRPEND 0
#define arguments_BDK_RVU_AF_PFTRPEND -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_pftrpend_w1s
 *
 * RVU Admin Function PF Transaction Pending Set Registers
 * This register reads or sets bits.
 */
union bdk_rvu_af_pftrpend_w1s
{
    uint64_t u;
    struct bdk_rvu_af_pftrpend_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t trpend                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_AF_PFTRPEND[TRPEND]. */
#else /* Word 0 - Little Endian */
        uint64_t trpend                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_AF_PFTRPEND[TRPEND]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_pftrpend_w1s_s cn; */
};
typedef union bdk_rvu_af_pftrpend_w1s bdk_rvu_af_pftrpend_w1s_t;

#define BDK_RVU_AF_PFTRPEND_W1S BDK_RVU_AF_PFTRPEND_W1S_FUNC()
static inline uint64_t BDK_RVU_AF_PFTRPEND_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_PFTRPEND_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000002820ll;
    __bdk_csr_fatal("RVU_AF_PFTRPEND_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_PFTRPEND_W1S bdk_rvu_af_pftrpend_w1s_t
#define bustype_BDK_RVU_AF_PFTRPEND_W1S BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_PFTRPEND_W1S "RVU_AF_PFTRPEND_W1S"
#define device_bar_BDK_RVU_AF_PFTRPEND_W1S 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_PFTRPEND_W1S 0
#define arguments_BDK_RVU_AF_PFTRPEND_W1S -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_ras
 *
 * RVU Admin Function RAS Interrupt Register
 * This register is intended for delivery of RAS events to the SCP, so should be
 * ignored by OS drivers.
 */
union bdk_rvu_af_ras
{
    uint64_t u;
    struct bdk_rvu_af_ras_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t msix_poison           : 1;  /**< [  0:  0](R/W1C/H) Received MSI-X table read response with poisoned data. */
#else /* Word 0 - Little Endian */
        uint64_t msix_poison           : 1;  /**< [  0:  0](R/W1C/H) Received MSI-X table read response with poisoned data. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_ras_s cn; */
};
typedef union bdk_rvu_af_ras bdk_rvu_af_ras_t;

#define BDK_RVU_AF_RAS BDK_RVU_AF_RAS_FUNC()
static inline uint64_t BDK_RVU_AF_RAS_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_RAS_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000000100ll;
    __bdk_csr_fatal("RVU_AF_RAS", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_RAS bdk_rvu_af_ras_t
#define bustype_BDK_RVU_AF_RAS BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_RAS "RVU_AF_RAS"
#define device_bar_BDK_RVU_AF_RAS 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_RAS 0
#define arguments_BDK_RVU_AF_RAS -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_ras_ena_w1c
 *
 * RVU Admin Function RAS Interrupt Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_rvu_af_ras_ena_w1c
{
    uint64_t u;
    struct bdk_rvu_af_ras_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t msix_poison           : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for RVU_AF_RAS[MSIX_POISON]. */
#else /* Word 0 - Little Endian */
        uint64_t msix_poison           : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for RVU_AF_RAS[MSIX_POISON]. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_ras_ena_w1c_s cn; */
};
typedef union bdk_rvu_af_ras_ena_w1c bdk_rvu_af_ras_ena_w1c_t;

#define BDK_RVU_AF_RAS_ENA_W1C BDK_RVU_AF_RAS_ENA_W1C_FUNC()
static inline uint64_t BDK_RVU_AF_RAS_ENA_W1C_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_RAS_ENA_W1C_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000000118ll;
    __bdk_csr_fatal("RVU_AF_RAS_ENA_W1C", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_RAS_ENA_W1C bdk_rvu_af_ras_ena_w1c_t
#define bustype_BDK_RVU_AF_RAS_ENA_W1C BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_RAS_ENA_W1C "RVU_AF_RAS_ENA_W1C"
#define device_bar_BDK_RVU_AF_RAS_ENA_W1C 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_RAS_ENA_W1C 0
#define arguments_BDK_RVU_AF_RAS_ENA_W1C -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_ras_ena_w1s
 *
 * RVU Admin Function RAS Interrupt Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_rvu_af_ras_ena_w1s
{
    uint64_t u;
    struct bdk_rvu_af_ras_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t msix_poison           : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for RVU_AF_RAS[MSIX_POISON]. */
#else /* Word 0 - Little Endian */
        uint64_t msix_poison           : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for RVU_AF_RAS[MSIX_POISON]. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_ras_ena_w1s_s cn; */
};
typedef union bdk_rvu_af_ras_ena_w1s bdk_rvu_af_ras_ena_w1s_t;

#define BDK_RVU_AF_RAS_ENA_W1S BDK_RVU_AF_RAS_ENA_W1S_FUNC()
static inline uint64_t BDK_RVU_AF_RAS_ENA_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_RAS_ENA_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000000110ll;
    __bdk_csr_fatal("RVU_AF_RAS_ENA_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_RAS_ENA_W1S bdk_rvu_af_ras_ena_w1s_t
#define bustype_BDK_RVU_AF_RAS_ENA_W1S BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_RAS_ENA_W1S "RVU_AF_RAS_ENA_W1S"
#define device_bar_BDK_RVU_AF_RAS_ENA_W1S 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_RAS_ENA_W1S 0
#define arguments_BDK_RVU_AF_RAS_ENA_W1S -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_af_ras_w1s
 *
 * RVU Admin Function RAS Interrupt Set Register
 * This register sets interrupt bits.
 */
union bdk_rvu_af_ras_w1s
{
    uint64_t u;
    struct bdk_rvu_af_ras_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t msix_poison           : 1;  /**< [  0:  0](R/W1S/H) Reads or sets RVU_AF_RAS[MSIX_POISON]. */
#else /* Word 0 - Little Endian */
        uint64_t msix_poison           : 1;  /**< [  0:  0](R/W1S/H) Reads or sets RVU_AF_RAS[MSIX_POISON]. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_af_ras_w1s_s cn; */
};
typedef union bdk_rvu_af_ras_w1s bdk_rvu_af_ras_w1s_t;

#define BDK_RVU_AF_RAS_W1S BDK_RVU_AF_RAS_W1S_FUNC()
static inline uint64_t BDK_RVU_AF_RAS_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_AF_RAS_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850000000108ll;
    __bdk_csr_fatal("RVU_AF_RAS_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_AF_RAS_W1S bdk_rvu_af_ras_w1s_t
#define bustype_BDK_RVU_AF_RAS_W1S BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_AF_RAS_W1S "RVU_AF_RAS_W1S"
#define device_bar_BDK_RVU_AF_RAS_W1S 0x0 /* BAR0 */
#define busnum_BDK_RVU_AF_RAS_W1S 0
#define arguments_BDK_RVU_AF_RAS_W1S -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_block_addr#_disc
 *
 * RVU PF Block Address Discovery Registers
 * These registers allow each PF driver to discover block resources that are
 * provisioned to its PF. The register's block address index is enumerated by
 * RVU_BLOCK_ADDR_E.
 */
union bdk_rvu_pf_block_addrx_disc
{
    uint64_t u;
    struct bdk_rvu_pf_block_addrx_disc_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_28_63        : 36;
        uint64_t btype                 : 8;  /**< [ 27: 20](RO/H) Block type enumerated by RVU_BLOCK_TYPE_E. */
        uint64_t rid                   : 8;  /**< [ 19: 12](RO/H) Revision ID of the block from RVU_PRIV_BLOCK_TYPE()_REV[RID]. */
        uint64_t imp                   : 1;  /**< [ 11: 11](RO/H) Implemented. When set, a block is present at this block address index as
                                                                 enumerated by RVU_BLOCK_ADDR_E. When clear, a block is not present and the
                                                                 remaining fields in the register are RAZ.

                                                                 Internal:
                                                                 Returns zero if the block is implemented but fused out. */
        uint64_t reserved_9_10         : 2;
        uint64_t num_lfs               : 9;  /**< [  8:  0](RO/H) Number of local functions from the block that are provisioned to the VF/PF.
                                                                 When non-zero, the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in
                                                                 the the block.
                                                                 Returns 0 for block types that do not have local functions, 0 or 1 for
                                                                 single-slot blocks; see RVU_BLOCK_TYPE_E. */
#else /* Word 0 - Little Endian */
        uint64_t num_lfs               : 9;  /**< [  8:  0](RO/H) Number of local functions from the block that are provisioned to the VF/PF.
                                                                 When non-zero, the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in
                                                                 the the block.
                                                                 Returns 0 for block types that do not have local functions, 0 or 1 for
                                                                 single-slot blocks; see RVU_BLOCK_TYPE_E. */
        uint64_t reserved_9_10         : 2;
        uint64_t imp                   : 1;  /**< [ 11: 11](RO/H) Implemented. When set, a block is present at this block address index as
                                                                 enumerated by RVU_BLOCK_ADDR_E. When clear, a block is not present and the
                                                                 remaining fields in the register are RAZ.

                                                                 Internal:
                                                                 Returns zero if the block is implemented but fused out. */
        uint64_t rid                   : 8;  /**< [ 19: 12](RO/H) Revision ID of the block from RVU_PRIV_BLOCK_TYPE()_REV[RID]. */
        uint64_t btype                 : 8;  /**< [ 27: 20](RO/H) Block type enumerated by RVU_BLOCK_TYPE_E. */
        uint64_t reserved_28_63        : 36;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_block_addrx_disc_s cn; */
};
typedef union bdk_rvu_pf_block_addrx_disc bdk_rvu_pf_block_addrx_disc_t;

static inline uint64_t BDK_RVU_PF_BLOCK_ADDRX_DISC(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_BLOCK_ADDRX_DISC(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=31))
        return 0x850200000200ll + 8ll * ((a) & 0x1f);
    __bdk_csr_fatal("RVU_PF_BLOCK_ADDRX_DISC", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_BLOCK_ADDRX_DISC(a) bdk_rvu_pf_block_addrx_disc_t
#define bustype_BDK_RVU_PF_BLOCK_ADDRX_DISC(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_BLOCK_ADDRX_DISC(a) "RVU_PF_BLOCK_ADDRX_DISC"
#define device_bar_BDK_RVU_PF_BLOCK_ADDRX_DISC(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_BLOCK_ADDRX_DISC(a) (a)
#define arguments_BDK_RVU_PF_BLOCK_ADDRX_DISC(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_int
 *
 * RVU PF Interrupt Registers
 */
union bdk_rvu_pf_int
{
    uint64_t u;
    struct bdk_rvu_pf_int_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1C/H) AF to PF mailbox interrupt. Set when RVU_AF_AFPF()_MBOX(0) is written. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1C/H) AF to PF mailbox interrupt. Set when RVU_AF_AFPF()_MBOX(0) is written. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_int_s cn; */
};
typedef union bdk_rvu_pf_int bdk_rvu_pf_int_t;

#define BDK_RVU_PF_INT BDK_RVU_PF_INT_FUNC()
static inline uint64_t BDK_RVU_PF_INT_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_INT_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850200000c20ll;
    __bdk_csr_fatal("RVU_PF_INT", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_INT bdk_rvu_pf_int_t
#define bustype_BDK_RVU_PF_INT BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_INT "RVU_PF_INT"
#define device_bar_BDK_RVU_PF_INT 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_INT 0
#define arguments_BDK_RVU_PF_INT -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_int_ena_w1c
 *
 * RVU PF Interrupt Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_rvu_pf_int_ena_w1c
{
    uint64_t u;
    struct bdk_rvu_pf_int_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for RVU_PF_INT[MBOX]. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for RVU_PF_INT[MBOX]. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_int_ena_w1c_s cn; */
};
typedef union bdk_rvu_pf_int_ena_w1c bdk_rvu_pf_int_ena_w1c_t;

#define BDK_RVU_PF_INT_ENA_W1C BDK_RVU_PF_INT_ENA_W1C_FUNC()
static inline uint64_t BDK_RVU_PF_INT_ENA_W1C_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_INT_ENA_W1C_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850200000c38ll;
    __bdk_csr_fatal("RVU_PF_INT_ENA_W1C", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_INT_ENA_W1C bdk_rvu_pf_int_ena_w1c_t
#define bustype_BDK_RVU_PF_INT_ENA_W1C BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_INT_ENA_W1C "RVU_PF_INT_ENA_W1C"
#define device_bar_BDK_RVU_PF_INT_ENA_W1C 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_INT_ENA_W1C 0
#define arguments_BDK_RVU_PF_INT_ENA_W1C -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_int_ena_w1s
 *
 * RVU PF Interrupt Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_rvu_pf_int_ena_w1s
{
    uint64_t u;
    struct bdk_rvu_pf_int_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for RVU_PF_INT[MBOX]. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for RVU_PF_INT[MBOX]. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_int_ena_w1s_s cn; */
};
typedef union bdk_rvu_pf_int_ena_w1s bdk_rvu_pf_int_ena_w1s_t;

#define BDK_RVU_PF_INT_ENA_W1S BDK_RVU_PF_INT_ENA_W1S_FUNC()
static inline uint64_t BDK_RVU_PF_INT_ENA_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_INT_ENA_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850200000c30ll;
    __bdk_csr_fatal("RVU_PF_INT_ENA_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_INT_ENA_W1S bdk_rvu_pf_int_ena_w1s_t
#define bustype_BDK_RVU_PF_INT_ENA_W1S BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_INT_ENA_W1S "RVU_PF_INT_ENA_W1S"
#define device_bar_BDK_RVU_PF_INT_ENA_W1S 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_INT_ENA_W1S 0
#define arguments_BDK_RVU_PF_INT_ENA_W1S -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_int_w1s
 *
 * RVU PF Interrupt Set Register
 * This register sets interrupt bits.
 */
union bdk_rvu_pf_int_w1s
{
    uint64_t u;
    struct bdk_rvu_pf_int_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1S/H) Reads or sets RVU_PF_INT[MBOX]. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1S/H) Reads or sets RVU_PF_INT[MBOX]. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_int_w1s_s cn; */
};
typedef union bdk_rvu_pf_int_w1s bdk_rvu_pf_int_w1s_t;

#define BDK_RVU_PF_INT_W1S BDK_RVU_PF_INT_W1S_FUNC()
static inline uint64_t BDK_RVU_PF_INT_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_INT_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850200000c28ll;
    __bdk_csr_fatal("RVU_PF_INT_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_INT_W1S bdk_rvu_pf_int_w1s_t
#define bustype_BDK_RVU_PF_INT_W1S BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_INT_W1S "RVU_PF_INT_W1S"
#define device_bar_BDK_RVU_PF_INT_W1S 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_INT_W1S 0
#define arguments_BDK_RVU_PF_INT_W1S -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_msix_pba#
 *
 * RVU PF MSI-X Pending-Bit-Array Registers
 * This register is the MSI-X PF PBA table.
 */
union bdk_rvu_pf_msix_pbax
{
    uint64_t u;
    struct bdk_rvu_pf_msix_pbax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO/H) Pending message bit for each MSI-X vector, i.e. one bit per
                                                                 RVU_PF_MSIX_VEC()_CTL register.
                                                                 The total number of bits for a given PF (and thus the number of PBA
                                                                 registers) is determined by RVU_PRIV_PF()_MSIX_CFG[VF_MSIXT_SIZEM1]
                                                                 (plus 1). */
#else /* Word 0 - Little Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO/H) Pending message bit for each MSI-X vector, i.e. one bit per
                                                                 RVU_PF_MSIX_VEC()_CTL register.
                                                                 The total number of bits for a given PF (and thus the number of PBA
                                                                 registers) is determined by RVU_PRIV_PF()_MSIX_CFG[VF_MSIXT_SIZEM1]
                                                                 (plus 1). */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_msix_pbax_s cn; */
};
typedef union bdk_rvu_pf_msix_pbax bdk_rvu_pf_msix_pbax_t;

static inline uint64_t BDK_RVU_PF_MSIX_PBAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_MSIX_PBAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a==0))
        return 0x8502002f0000ll + 8ll * ((a) & 0x0);
    __bdk_csr_fatal("RVU_PF_MSIX_PBAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_MSIX_PBAX(a) bdk_rvu_pf_msix_pbax_t
#define bustype_BDK_RVU_PF_MSIX_PBAX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_MSIX_PBAX(a) "RVU_PF_MSIX_PBAX"
#define device_bar_BDK_RVU_PF_MSIX_PBAX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_MSIX_PBAX(a) (a)
#define arguments_BDK_RVU_PF_MSIX_PBAX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_msix_vec#_addr
 *
 * RVU PF MSI-X Vector-Table Address Registers
 * These registers and RVU_PF_MSIX_VEC()_CTL form the PF MSI-X vector table.
 * The number of MSI-X vectors for a given PF is specified by
 * RVU_PRIV_PF()_MSIX_CFG[PF_MSIXT_SIZEM1] (plus 1).
 *
 * Internal:
 * PF vector count of 256 is sized to allow up to 120 for AF, 4 for PF/VF
 * mailboxes, and 128 for LF resources from various blocks that are directly
 * provisioned to the PF.
 */
union bdk_rvu_pf_msix_vecx_addr
{
    uint64_t u;
    struct bdk_rvu_pf_msix_vecx_addr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_53_63        : 11;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's RVU_PF_MSIX_VEC()_ADDR, RVU_PF_MSIX_VEC()_CTL, and
                                                                 corresponding bit of RVU_PF_MSIX_PBA() are RAZ/WI and does not cause a
                                                                 fault when accessed by the nonsecure world.

                                                                 If PCCPF_RVU_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors of the function are
                                                                 secure as if [SECVEC] was set. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's RVU_PF_MSIX_VEC()_ADDR, RVU_PF_MSIX_VEC()_CTL, and
                                                                 corresponding bit of RVU_PF_MSIX_PBA() are RAZ/WI and does not cause a
                                                                 fault when accessed by the nonsecure world.

                                                                 If PCCPF_RVU_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors of the function are
                                                                 secure as if [SECVEC] was set. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_53_63        : 11;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_msix_vecx_addr_s cn; */
};
typedef union bdk_rvu_pf_msix_vecx_addr bdk_rvu_pf_msix_vecx_addr_t;

static inline uint64_t BDK_RVU_PF_MSIX_VECX_ADDR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_MSIX_VECX_ADDR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a==0))
        return 0x850200200000ll + 0x10ll * ((a) & 0x0);
    __bdk_csr_fatal("RVU_PF_MSIX_VECX_ADDR", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_MSIX_VECX_ADDR(a) bdk_rvu_pf_msix_vecx_addr_t
#define bustype_BDK_RVU_PF_MSIX_VECX_ADDR(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_MSIX_VECX_ADDR(a) "RVU_PF_MSIX_VECX_ADDR"
#define device_bar_BDK_RVU_PF_MSIX_VECX_ADDR(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_MSIX_VECX_ADDR(a) (a)
#define arguments_BDK_RVU_PF_MSIX_VECX_ADDR(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_msix_vec#_ctl
 *
 * RVU PF MSI-X Vector-Table Control and Data Registers
 * These registers and RVU_PF_MSIX_VEC()_ADDR form the PF MSI-X vector table.
 */
union bdk_rvu_pf_msix_vecx_ctl
{
    uint64_t u;
    struct bdk_rvu_pf_msix_vecx_ctl_s
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
    /* struct bdk_rvu_pf_msix_vecx_ctl_s cn; */
};
typedef union bdk_rvu_pf_msix_vecx_ctl bdk_rvu_pf_msix_vecx_ctl_t;

static inline uint64_t BDK_RVU_PF_MSIX_VECX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_MSIX_VECX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a==0))
        return 0x850200200008ll + 0x10ll * ((a) & 0x0);
    __bdk_csr_fatal("RVU_PF_MSIX_VECX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_MSIX_VECX_CTL(a) bdk_rvu_pf_msix_vecx_ctl_t
#define bustype_BDK_RVU_PF_MSIX_VECX_CTL(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_MSIX_VECX_CTL(a) "RVU_PF_MSIX_VECX_CTL"
#define device_bar_BDK_RVU_PF_MSIX_VECX_CTL(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_MSIX_VECX_CTL(a) (a)
#define arguments_BDK_RVU_PF_MSIX_VECX_CTL(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_pfaf_mbox#
 *
 * RVU PF/AF Mailbox Registers
 */
union bdk_rvu_pf_pfaf_mboxx
{
    uint64_t u;
    struct bdk_rvu_pf_pfaf_mboxx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W/H) Mailbox data. These PF registers access the 16-byte-per-PF PF/AF
                                                                 mailbox.  The AF may access the same storage using
                                                                 RVU_AF_AFPF()_MBOX(). MBOX(0) is typically used for AF to PF
                                                                 signaling, MBOX(1) for PF to AF.
                                                                 Writing RVU_PF_PFAF_MBOX(1) (but not RVU_AF_AFPF()_MBOX(1))
                                                                 will set the corresponding RVU_AF_PFAF_MBOX_INT bit, which if appropriately
                                                                 enabled will send an interrupt to the AF. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W/H) Mailbox data. These PF registers access the 16-byte-per-PF PF/AF
                                                                 mailbox.  The AF may access the same storage using
                                                                 RVU_AF_AFPF()_MBOX(). MBOX(0) is typically used for AF to PF
                                                                 signaling, MBOX(1) for PF to AF.
                                                                 Writing RVU_PF_PFAF_MBOX(1) (but not RVU_AF_AFPF()_MBOX(1))
                                                                 will set the corresponding RVU_AF_PFAF_MBOX_INT bit, which if appropriately
                                                                 enabled will send an interrupt to the AF. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_pfaf_mboxx_s cn; */
};
typedef union bdk_rvu_pf_pfaf_mboxx bdk_rvu_pf_pfaf_mboxx_t;

static inline uint64_t BDK_RVU_PF_PFAF_MBOXX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_PFAF_MBOXX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x850200000c00ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("RVU_PF_PFAF_MBOXX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_PFAF_MBOXX(a) bdk_rvu_pf_pfaf_mboxx_t
#define bustype_BDK_RVU_PF_PFAF_MBOXX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_PFAF_MBOXX(a) "RVU_PF_PFAF_MBOXX"
#define device_bar_BDK_RVU_PF_PFAF_MBOXX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_PFAF_MBOXX(a) (a)
#define arguments_BDK_RVU_PF_PFAF_MBOXX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vf#_pfvf_mbox#
 *
 * RVU PF/VF Mailbox Registers
 */
union bdk_rvu_pf_vfx_pfvf_mboxx
{
    uint64_t u;
    struct bdk_rvu_pf_vfx_pfvf_mboxx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W/H) Mailbox data. These PF registers access the 16-byte-per-VF VF/PF mailbox
                                                                 RAM. Each corresponding VF may access the same storage using
                                                                 RVU_VF_VFPF_MBOX(). MBOX(0) is typically used for PF to VF
                                                                 signaling, MBOX(1) for VF to PF. Writing RVU_PF_VF()_PFVF_MBOX(0) (but
                                                                 not RVU_VF_VFPF_MBOX(0)) will set the corresponding
                                                                 RVU_VF_INT[MBOX] which if appropriately enabled will send an
                                                                 interrupt to the VF. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W/H) Mailbox data. These PF registers access the 16-byte-per-VF VF/PF mailbox
                                                                 RAM. Each corresponding VF may access the same storage using
                                                                 RVU_VF_VFPF_MBOX(). MBOX(0) is typically used for PF to VF
                                                                 signaling, MBOX(1) for VF to PF. Writing RVU_PF_VF()_PFVF_MBOX(0) (but
                                                                 not RVU_VF_VFPF_MBOX(0)) will set the corresponding
                                                                 RVU_VF_INT[MBOX] which if appropriately enabled will send an
                                                                 interrupt to the VF. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vfx_pfvf_mboxx_s cn; */
};
typedef union bdk_rvu_pf_vfx_pfvf_mboxx bdk_rvu_pf_vfx_pfvf_mboxx_t;

static inline uint64_t BDK_RVU_PF_VFX_PFVF_MBOXX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFX_PFVF_MBOXX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=127) && (b<=1)))
        return 0x850200000000ll + 0x1000ll * ((a) & 0x7f) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("RVU_PF_VFX_PFVF_MBOXX", 2, a, b, 0, 0);
}

#define typedef_BDK_RVU_PF_VFX_PFVF_MBOXX(a,b) bdk_rvu_pf_vfx_pfvf_mboxx_t
#define bustype_BDK_RVU_PF_VFX_PFVF_MBOXX(a,b) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFX_PFVF_MBOXX(a,b) "RVU_PF_VFX_PFVF_MBOXX"
#define device_bar_BDK_RVU_PF_VFX_PFVF_MBOXX(a,b) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFX_PFVF_MBOXX(a,b) (a)
#define arguments_BDK_RVU_PF_VFX_PFVF_MBOXX(a,b) (a),(b),-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vf_bar4_addr
 *
 * RVU PF VF BAR4 Address Registers
 */
union bdk_rvu_pf_vf_bar4_addr
{
    uint64_t u;
    struct bdk_rvu_pf_vf_bar4_addr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t addr                  : 48; /**< [ 63: 16](R/W) Programmable base address of RVU_PRIV_PF()_CFG[NVF] consecutive 64 KB
                                                                 pages in DRAM. May be used as VF/PF mailbox memory in addition to
                                                                 RVU_PF_VF()_PFVF_MBOX()/RVU_VF_VFPF_MBOX().
                                                                 Provides PCC_EA_ENTRY_S[BASEH,BASEL] value advertised by VF BAR4's entry in
                                                                 PCCPF_XXX_EA_ENTRY(). */
        uint64_t reserved_0_15         : 16;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_15         : 16;
        uint64_t addr                  : 48; /**< [ 63: 16](R/W) Programmable base address of RVU_PRIV_PF()_CFG[NVF] consecutive 64 KB
                                                                 pages in DRAM. May be used as VF/PF mailbox memory in addition to
                                                                 RVU_PF_VF()_PFVF_MBOX()/RVU_VF_VFPF_MBOX().
                                                                 Provides PCC_EA_ENTRY_S[BASEH,BASEL] value advertised by VF BAR4's entry in
                                                                 PCCPF_XXX_EA_ENTRY(). */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vf_bar4_addr_s cn; */
};
typedef union bdk_rvu_pf_vf_bar4_addr bdk_rvu_pf_vf_bar4_addr_t;

#define BDK_RVU_PF_VF_BAR4_ADDR BDK_RVU_PF_VF_BAR4_ADDR_FUNC()
static inline uint64_t BDK_RVU_PF_VF_BAR4_ADDR_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VF_BAR4_ADDR_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850200000010ll;
    __bdk_csr_fatal("RVU_PF_VF_BAR4_ADDR", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VF_BAR4_ADDR bdk_rvu_pf_vf_bar4_addr_t
#define bustype_BDK_RVU_PF_VF_BAR4_ADDR BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VF_BAR4_ADDR "RVU_PF_VF_BAR4_ADDR"
#define device_bar_BDK_RVU_PF_VF_BAR4_ADDR 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VF_BAR4_ADDR 0
#define arguments_BDK_RVU_PF_VF_BAR4_ADDR -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vfflr_int#
 *
 * RVU PF VF Function Level Reset Interrupt Registers
 */
union bdk_rvu_pf_vfflr_intx
{
    uint64_t u;
    struct bdk_rvu_pf_vfflr_intx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1C/H) FLR interrupt bit per VF (RVU_PF_VFFLR_INT({a})[FLR]\<{b}\> for VF
                                                                 number 64*{a} + {b}).
                                                                 If RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set, each bit is set along with
                                                                 the corresponding bit in RVU_PF_VFTRPEND() when function level reset is
                                                                 initiated for the associated VF, i.e. a one is written to
                                                                 PCCVF_XXX_E_DEV_CTL[BCR_FLR]. */
#else /* Word 0 - Little Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1C/H) FLR interrupt bit per VF (RVU_PF_VFFLR_INT({a})[FLR]\<{b}\> for VF
                                                                 number 64*{a} + {b}).
                                                                 If RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set, each bit is set along with
                                                                 the corresponding bit in RVU_PF_VFTRPEND() when function level reset is
                                                                 initiated for the associated VF, i.e. a one is written to
                                                                 PCCVF_XXX_E_DEV_CTL[BCR_FLR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vfflr_intx_s cn; */
};
typedef union bdk_rvu_pf_vfflr_intx bdk_rvu_pf_vfflr_intx_t;

static inline uint64_t BDK_RVU_PF_VFFLR_INTX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFFLR_INTX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x850200000900ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("RVU_PF_VFFLR_INTX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VFFLR_INTX(a) bdk_rvu_pf_vfflr_intx_t
#define bustype_BDK_RVU_PF_VFFLR_INTX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFFLR_INTX(a) "RVU_PF_VFFLR_INTX"
#define device_bar_BDK_RVU_PF_VFFLR_INTX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFFLR_INTX(a) (a)
#define arguments_BDK_RVU_PF_VFFLR_INTX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vfflr_int_ena_w1c#
 *
 * RVU PF VF Function Level Reset Interrupt Enable Clear Registers
 * This register clears interrupt enable bits.
 */
union bdk_rvu_pf_vfflr_int_ena_w1cx
{
    uint64_t u;
    struct bdk_rvu_pf_vfflr_int_ena_w1cx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for RVU_PF_VFFLR_INT(0..3)[FLR]. */
#else /* Word 0 - Little Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for RVU_PF_VFFLR_INT(0..3)[FLR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vfflr_int_ena_w1cx_s cn; */
};
typedef union bdk_rvu_pf_vfflr_int_ena_w1cx bdk_rvu_pf_vfflr_int_ena_w1cx_t;

static inline uint64_t BDK_RVU_PF_VFFLR_INT_ENA_W1CX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFFLR_INT_ENA_W1CX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x850200000960ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("RVU_PF_VFFLR_INT_ENA_W1CX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VFFLR_INT_ENA_W1CX(a) bdk_rvu_pf_vfflr_int_ena_w1cx_t
#define bustype_BDK_RVU_PF_VFFLR_INT_ENA_W1CX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFFLR_INT_ENA_W1CX(a) "RVU_PF_VFFLR_INT_ENA_W1CX"
#define device_bar_BDK_RVU_PF_VFFLR_INT_ENA_W1CX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFFLR_INT_ENA_W1CX(a) (a)
#define arguments_BDK_RVU_PF_VFFLR_INT_ENA_W1CX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vfflr_int_ena_w1s#
 *
 * RVU PF VF Function Level Reset Interrupt Enable Set Registers
 * This register sets interrupt enable bits.
 */
union bdk_rvu_pf_vfflr_int_ena_w1sx
{
    uint64_t u;
    struct bdk_rvu_pf_vfflr_int_ena_w1sx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for RVU_PF_VFFLR_INT(0..3)[FLR]. */
#else /* Word 0 - Little Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for RVU_PF_VFFLR_INT(0..3)[FLR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vfflr_int_ena_w1sx_s cn; */
};
typedef union bdk_rvu_pf_vfflr_int_ena_w1sx bdk_rvu_pf_vfflr_int_ena_w1sx_t;

static inline uint64_t BDK_RVU_PF_VFFLR_INT_ENA_W1SX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFFLR_INT_ENA_W1SX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x850200000940ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("RVU_PF_VFFLR_INT_ENA_W1SX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VFFLR_INT_ENA_W1SX(a) bdk_rvu_pf_vfflr_int_ena_w1sx_t
#define bustype_BDK_RVU_PF_VFFLR_INT_ENA_W1SX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFFLR_INT_ENA_W1SX(a) "RVU_PF_VFFLR_INT_ENA_W1SX"
#define device_bar_BDK_RVU_PF_VFFLR_INT_ENA_W1SX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFFLR_INT_ENA_W1SX(a) (a)
#define arguments_BDK_RVU_PF_VFFLR_INT_ENA_W1SX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vfflr_int_w1s#
 *
 * RVU PF VF Function Level Reset Interrupt Set Registers
 * This register sets interrupt bits.
 */
union bdk_rvu_pf_vfflr_int_w1sx
{
    uint64_t u;
    struct bdk_rvu_pf_vfflr_int_w1sx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_PF_VFFLR_INT(0..3)[FLR]. */
#else /* Word 0 - Little Endian */
        uint64_t flr                   : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_PF_VFFLR_INT(0..3)[FLR]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vfflr_int_w1sx_s cn; */
};
typedef union bdk_rvu_pf_vfflr_int_w1sx bdk_rvu_pf_vfflr_int_w1sx_t;

static inline uint64_t BDK_RVU_PF_VFFLR_INT_W1SX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFFLR_INT_W1SX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x850200000920ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("RVU_PF_VFFLR_INT_W1SX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VFFLR_INT_W1SX(a) bdk_rvu_pf_vfflr_int_w1sx_t
#define bustype_BDK_RVU_PF_VFFLR_INT_W1SX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFFLR_INT_W1SX(a) "RVU_PF_VFFLR_INT_W1SX"
#define device_bar_BDK_RVU_PF_VFFLR_INT_W1SX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFFLR_INT_W1SX(a) (a)
#define arguments_BDK_RVU_PF_VFFLR_INT_W1SX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vfme_int#
 *
 * RVU PF VF Bus Master Enable Interrupt Registers
 */
union bdk_rvu_pf_vfme_intx
{
    uint64_t u;
    struct bdk_rvu_pf_vfme_intx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1C/H) Master enable interrupt bit per VF (RVU_PF_VFME_INT({a})[ME]\<{b}\> for VF
                                                                 number 64*{a} + {b}).
                                                                 A device-dependent PF driver typically uses these bits to handle state
                                                                 changes to PCCPF_XXX_CMD[ME], which are typically modified by
                                                                 non-device-dependent software only.

                                                                 If RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set, each bit is set when the
                                                                 corresponding PCCVF_XXX_CMD[ME] bit is either set or cleared for the
                                                                 associated PF. The corresponding bit in RVU_PF_VFME_STATUS() returns the
                                                                 current value of PCCVF_XXX_CMD[ME].

                                                                 If RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set, the corresponding bit in
                                                                 RVU_PF_VFTRPEND() is also set when PCCVF_XXX_CMD[ME] is set, but not
                                                                 when PCCVF_XXX_CMD[ME] is cleared. */
#else /* Word 0 - Little Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1C/H) Master enable interrupt bit per VF (RVU_PF_VFME_INT({a})[ME]\<{b}\> for VF
                                                                 number 64*{a} + {b}).
                                                                 A device-dependent PF driver typically uses these bits to handle state
                                                                 changes to PCCPF_XXX_CMD[ME], which are typically modified by
                                                                 non-device-dependent software only.

                                                                 If RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set, each bit is set when the
                                                                 corresponding PCCVF_XXX_CMD[ME] bit is either set or cleared for the
                                                                 associated PF. The corresponding bit in RVU_PF_VFME_STATUS() returns the
                                                                 current value of PCCVF_XXX_CMD[ME].

                                                                 If RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set, the corresponding bit in
                                                                 RVU_PF_VFTRPEND() is also set when PCCVF_XXX_CMD[ME] is set, but not
                                                                 when PCCVF_XXX_CMD[ME] is cleared. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vfme_intx_s cn; */
};
typedef union bdk_rvu_pf_vfme_intx bdk_rvu_pf_vfme_intx_t;

static inline uint64_t BDK_RVU_PF_VFME_INTX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFME_INTX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x850200000980ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("RVU_PF_VFME_INTX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VFME_INTX(a) bdk_rvu_pf_vfme_intx_t
#define bustype_BDK_RVU_PF_VFME_INTX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFME_INTX(a) "RVU_PF_VFME_INTX"
#define device_bar_BDK_RVU_PF_VFME_INTX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFME_INTX(a) (a)
#define arguments_BDK_RVU_PF_VFME_INTX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vfme_int_ena_w1c#
 *
 * RVU PF VF Bus Master Enable Interrupt Enable Clear Registers
 * This register clears interrupt enable bits.
 */
union bdk_rvu_pf_vfme_int_ena_w1cx
{
    uint64_t u;
    struct bdk_rvu_pf_vfme_int_ena_w1cx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for RVU_PF_VFME_INT(0..3)[ME]. */
#else /* Word 0 - Little Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for RVU_PF_VFME_INT(0..3)[ME]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vfme_int_ena_w1cx_s cn; */
};
typedef union bdk_rvu_pf_vfme_int_ena_w1cx bdk_rvu_pf_vfme_int_ena_w1cx_t;

static inline uint64_t BDK_RVU_PF_VFME_INT_ENA_W1CX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFME_INT_ENA_W1CX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x8502000009e0ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("RVU_PF_VFME_INT_ENA_W1CX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VFME_INT_ENA_W1CX(a) bdk_rvu_pf_vfme_int_ena_w1cx_t
#define bustype_BDK_RVU_PF_VFME_INT_ENA_W1CX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFME_INT_ENA_W1CX(a) "RVU_PF_VFME_INT_ENA_W1CX"
#define device_bar_BDK_RVU_PF_VFME_INT_ENA_W1CX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFME_INT_ENA_W1CX(a) (a)
#define arguments_BDK_RVU_PF_VFME_INT_ENA_W1CX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vfme_int_ena_w1s#
 *
 * RVU PF VF Bus Master Enable Interrupt Enable Set Registers
 * This register sets interrupt enable bits.
 */
union bdk_rvu_pf_vfme_int_ena_w1sx
{
    uint64_t u;
    struct bdk_rvu_pf_vfme_int_ena_w1sx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for RVU_PF_VFME_INT(0..3)[ME]. */
#else /* Word 0 - Little Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for RVU_PF_VFME_INT(0..3)[ME]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vfme_int_ena_w1sx_s cn; */
};
typedef union bdk_rvu_pf_vfme_int_ena_w1sx bdk_rvu_pf_vfme_int_ena_w1sx_t;

static inline uint64_t BDK_RVU_PF_VFME_INT_ENA_W1SX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFME_INT_ENA_W1SX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x8502000009c0ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("RVU_PF_VFME_INT_ENA_W1SX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VFME_INT_ENA_W1SX(a) bdk_rvu_pf_vfme_int_ena_w1sx_t
#define bustype_BDK_RVU_PF_VFME_INT_ENA_W1SX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFME_INT_ENA_W1SX(a) "RVU_PF_VFME_INT_ENA_W1SX"
#define device_bar_BDK_RVU_PF_VFME_INT_ENA_W1SX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFME_INT_ENA_W1SX(a) (a)
#define arguments_BDK_RVU_PF_VFME_INT_ENA_W1SX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vfme_int_w1s#
 *
 * RVU PF VF Bus Master Enable Interrupt Set Registers
 * This register sets interrupt bits.
 */
union bdk_rvu_pf_vfme_int_w1sx
{
    uint64_t u;
    struct bdk_rvu_pf_vfme_int_w1sx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_PF_VFME_INT(0..3)[ME]. */
#else /* Word 0 - Little Endian */
        uint64_t me                    : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_PF_VFME_INT(0..3)[ME]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vfme_int_w1sx_s cn; */
};
typedef union bdk_rvu_pf_vfme_int_w1sx bdk_rvu_pf_vfme_int_w1sx_t;

static inline uint64_t BDK_RVU_PF_VFME_INT_W1SX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFME_INT_W1SX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x8502000009a0ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("RVU_PF_VFME_INT_W1SX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VFME_INT_W1SX(a) bdk_rvu_pf_vfme_int_w1sx_t
#define bustype_BDK_RVU_PF_VFME_INT_W1SX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFME_INT_W1SX(a) "RVU_PF_VFME_INT_W1SX"
#define device_bar_BDK_RVU_PF_VFME_INT_W1SX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFME_INT_W1SX(a) (a)
#define arguments_BDK_RVU_PF_VFME_INT_W1SX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vfme_status#
 *
 * RVU PF VF Bus Master Enable Status Registers
 */
union bdk_rvu_pf_vfme_statusx
{
    uint64_t u;
    struct bdk_rvu_pf_vfme_statusx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t me                    : 64; /**< [ 63:  0](RO/H) Bus master enable bit per VF (RVU_PF_VFME_STATUS({a})[ME]\<{b}\> for VF
                                                                 number 64*{a} + {b}).
                                                                 Each bit returns the VF's PCCVF_XXX_CMD[ME] value. */
#else /* Word 0 - Little Endian */
        uint64_t me                    : 64; /**< [ 63:  0](RO/H) Bus master enable bit per VF (RVU_PF_VFME_STATUS({a})[ME]\<{b}\> for VF
                                                                 number 64*{a} + {b}).
                                                                 Each bit returns the VF's PCCVF_XXX_CMD[ME] value. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vfme_statusx_s cn; */
};
typedef union bdk_rvu_pf_vfme_statusx bdk_rvu_pf_vfme_statusx_t;

static inline uint64_t BDK_RVU_PF_VFME_STATUSX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFME_STATUSX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x850200000800ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("RVU_PF_VFME_STATUSX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VFME_STATUSX(a) bdk_rvu_pf_vfme_statusx_t
#define bustype_BDK_RVU_PF_VFME_STATUSX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFME_STATUSX(a) "RVU_PF_VFME_STATUSX"
#define device_bar_BDK_RVU_PF_VFME_STATUSX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFME_STATUSX(a) (a)
#define arguments_BDK_RVU_PF_VFME_STATUSX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vfpf_mbox_int#
 *
 * RVU VF to PF Mailbox Interrupt Registers
 */
union bdk_rvu_pf_vfpf_mbox_intx
{
    uint64_t u;
    struct bdk_rvu_pf_vfpf_mbox_intx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1C/H) Mailbox interrupt bit per VF (RVU_PF_VFPF_MBOX_INT({a})[MBOX]\<{b}\> for VF
                                                                 number 64*{a} + {b}).
                                                                 Each bit is set when the VF writes to the corresponding
                                                                 RVU_VF_VFPF_MBOX(1) register. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1C/H) Mailbox interrupt bit per VF (RVU_PF_VFPF_MBOX_INT({a})[MBOX]\<{b}\> for VF
                                                                 number 64*{a} + {b}).
                                                                 Each bit is set when the VF writes to the corresponding
                                                                 RVU_VF_VFPF_MBOX(1) register. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vfpf_mbox_intx_s cn; */
};
typedef union bdk_rvu_pf_vfpf_mbox_intx bdk_rvu_pf_vfpf_mbox_intx_t;

static inline uint64_t BDK_RVU_PF_VFPF_MBOX_INTX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFPF_MBOX_INTX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x850200000880ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("RVU_PF_VFPF_MBOX_INTX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VFPF_MBOX_INTX(a) bdk_rvu_pf_vfpf_mbox_intx_t
#define bustype_BDK_RVU_PF_VFPF_MBOX_INTX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFPF_MBOX_INTX(a) "RVU_PF_VFPF_MBOX_INTX"
#define device_bar_BDK_RVU_PF_VFPF_MBOX_INTX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFPF_MBOX_INTX(a) (a)
#define arguments_BDK_RVU_PF_VFPF_MBOX_INTX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vfpf_mbox_int_ena_w1c#
 *
 * RVU VF to PF Mailbox Interrupt Enable Clear Registers
 * This register clears interrupt enable bits.
 */
union bdk_rvu_pf_vfpf_mbox_int_ena_w1cx
{
    uint64_t u;
    struct bdk_rvu_pf_vfpf_mbox_int_ena_w1cx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for RVU_PF_VFPF_MBOX_INT(0..3)[MBOX]. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1C/H) Reads or clears enable for RVU_PF_VFPF_MBOX_INT(0..3)[MBOX]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vfpf_mbox_int_ena_w1cx_s cn; */
};
typedef union bdk_rvu_pf_vfpf_mbox_int_ena_w1cx bdk_rvu_pf_vfpf_mbox_int_ena_w1cx_t;

static inline uint64_t BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1CX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1CX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x8502000008e0ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("RVU_PF_VFPF_MBOX_INT_ENA_W1CX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1CX(a) bdk_rvu_pf_vfpf_mbox_int_ena_w1cx_t
#define bustype_BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1CX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1CX(a) "RVU_PF_VFPF_MBOX_INT_ENA_W1CX"
#define device_bar_BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1CX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1CX(a) (a)
#define arguments_BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1CX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vfpf_mbox_int_ena_w1s#
 *
 * RVU VF to PF Mailbox Interrupt Enable Set Registers
 * This register sets interrupt enable bits.
 */
union bdk_rvu_pf_vfpf_mbox_int_ena_w1sx
{
    uint64_t u;
    struct bdk_rvu_pf_vfpf_mbox_int_ena_w1sx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for RVU_PF_VFPF_MBOX_INT(0..3)[MBOX]. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1S/H) Reads or sets enable for RVU_PF_VFPF_MBOX_INT(0..3)[MBOX]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vfpf_mbox_int_ena_w1sx_s cn; */
};
typedef union bdk_rvu_pf_vfpf_mbox_int_ena_w1sx bdk_rvu_pf_vfpf_mbox_int_ena_w1sx_t;

static inline uint64_t BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1SX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1SX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x8502000008c0ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("RVU_PF_VFPF_MBOX_INT_ENA_W1SX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1SX(a) bdk_rvu_pf_vfpf_mbox_int_ena_w1sx_t
#define bustype_BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1SX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1SX(a) "RVU_PF_VFPF_MBOX_INT_ENA_W1SX"
#define device_bar_BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1SX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1SX(a) (a)
#define arguments_BDK_RVU_PF_VFPF_MBOX_INT_ENA_W1SX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vfpf_mbox_int_w1s#
 *
 * RVU VF to PF Mailbox Interrupt Set Registers
 * This register sets interrupt bits.
 */
union bdk_rvu_pf_vfpf_mbox_int_w1sx
{
    uint64_t u;
    struct bdk_rvu_pf_vfpf_mbox_int_w1sx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_PF_VFPF_MBOX_INT(0..3)[MBOX]. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_PF_VFPF_MBOX_INT(0..3)[MBOX]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vfpf_mbox_int_w1sx_s cn; */
};
typedef union bdk_rvu_pf_vfpf_mbox_int_w1sx bdk_rvu_pf_vfpf_mbox_int_w1sx_t;

static inline uint64_t BDK_RVU_PF_VFPF_MBOX_INT_W1SX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFPF_MBOX_INT_W1SX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x8502000008a0ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("RVU_PF_VFPF_MBOX_INT_W1SX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VFPF_MBOX_INT_W1SX(a) bdk_rvu_pf_vfpf_mbox_int_w1sx_t
#define bustype_BDK_RVU_PF_VFPF_MBOX_INT_W1SX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFPF_MBOX_INT_W1SX(a) "RVU_PF_VFPF_MBOX_INT_W1SX"
#define device_bar_BDK_RVU_PF_VFPF_MBOX_INT_W1SX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFPF_MBOX_INT_W1SX(a) (a)
#define arguments_BDK_RVU_PF_VFPF_MBOX_INT_W1SX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vftrpend#
 *
 * RVU PF VF Transaction Pending Registers
 */
union bdk_rvu_pf_vftrpendx
{
    uint64_t u;
    struct bdk_rvu_pf_vftrpendx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t trpend                : 64; /**< [ 63:  0](R/W1C/H) Transaction pending bit per VF (RVU_PF_VFTRPEND({a})[TRPEND]\<{b}\> for VF
                                                                 number 64*{a} + {b}).

                                                                 A VF's bit is set when RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set and:
                                                                 * A one is written to the corresponding PCCVF_XXX_E_DEV_CTL[BCR_FLR], or
                                                                 * PCCVF_XXX_CMD[ME] is set or cleared.

                                                                 When a VF's bit is set, forces the corresponding
                                                                 PCCVF_XXX_E_DEV_CTL[TRPEND] to be set.

                                                                 Software (typically a device-dependent PF driver) can clear the bit by
                                                                 writing a 1. */
#else /* Word 0 - Little Endian */
        uint64_t trpend                : 64; /**< [ 63:  0](R/W1C/H) Transaction pending bit per VF (RVU_PF_VFTRPEND({a})[TRPEND]\<{b}\> for VF
                                                                 number 64*{a} + {b}).

                                                                 A VF's bit is set when RVU_PRIV_PF()_CFG[ME_FLR_ENA] is set and:
                                                                 * A one is written to the corresponding PCCVF_XXX_E_DEV_CTL[BCR_FLR], or
                                                                 * PCCVF_XXX_CMD[ME] is set or cleared.

                                                                 When a VF's bit is set, forces the corresponding
                                                                 PCCVF_XXX_E_DEV_CTL[TRPEND] to be set.

                                                                 Software (typically a device-dependent PF driver) can clear the bit by
                                                                 writing a 1. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vftrpendx_s cn; */
};
typedef union bdk_rvu_pf_vftrpendx bdk_rvu_pf_vftrpendx_t;

static inline uint64_t BDK_RVU_PF_VFTRPENDX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFTRPENDX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x850200000820ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("RVU_PF_VFTRPENDX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VFTRPENDX(a) bdk_rvu_pf_vftrpendx_t
#define bustype_BDK_RVU_PF_VFTRPENDX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFTRPENDX(a) "RVU_PF_VFTRPENDX"
#define device_bar_BDK_RVU_PF_VFTRPENDX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFTRPENDX(a) (a)
#define arguments_BDK_RVU_PF_VFTRPENDX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR2) rvu_pf_vftrpend_w1s#
 *
 * RVU PF VF Transaction Pending Set Registers
 * This register reads or sets bits.
 */
union bdk_rvu_pf_vftrpend_w1sx
{
    uint64_t u;
    struct bdk_rvu_pf_vftrpend_w1sx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t trpend                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_PF_VFTRPEND(0..3)[TRPEND]. */
#else /* Word 0 - Little Endian */
        uint64_t trpend                : 64; /**< [ 63:  0](R/W1S/H) Reads or sets RVU_PF_VFTRPEND(0..3)[TRPEND]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_pf_vftrpend_w1sx_s cn; */
};
typedef union bdk_rvu_pf_vftrpend_w1sx bdk_rvu_pf_vftrpend_w1sx_t;

static inline uint64_t BDK_RVU_PF_VFTRPEND_W1SX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PF_VFTRPEND_W1SX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x850200000840ll + 8ll * ((a) & 0x3);
    __bdk_csr_fatal("RVU_PF_VFTRPEND_W1SX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PF_VFTRPEND_W1SX(a) bdk_rvu_pf_vftrpend_w1sx_t
#define bustype_BDK_RVU_PF_VFTRPEND_W1SX(a) BDK_CSR_TYPE_RVU_PF_BAR2
#define basename_BDK_RVU_PF_VFTRPEND_W1SX(a) "RVU_PF_VFTRPEND_W1SX"
#define device_bar_BDK_RVU_PF_VFTRPEND_W1SX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_PF_VFTRPEND_W1SX(a) (a)
#define arguments_BDK_RVU_PF_VFTRPEND_W1SX(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_active_pc
 *
 * RVU Active Program Counter Register
 */
union bdk_rvu_priv_active_pc
{
    uint64_t u;
    struct bdk_rvu_priv_active_pc_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t active_pc             : 64; /**< [ 63:  0](R/W/H) This register increments on every coprocessor-clock cycle that the RVU conditional clocks
                                                                 are enabled. */
#else /* Word 0 - Little Endian */
        uint64_t active_pc             : 64; /**< [ 63:  0](R/W/H) This register increments on every coprocessor-clock cycle that the RVU conditional clocks
                                                                 are enabled. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_active_pc_s cn; */
};
typedef union bdk_rvu_priv_active_pc bdk_rvu_priv_active_pc_t;

#define BDK_RVU_PRIV_ACTIVE_PC BDK_RVU_PRIV_ACTIVE_PC_FUNC()
static inline uint64_t BDK_RVU_PRIV_ACTIVE_PC_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_ACTIVE_PC_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850008000030ll;
    __bdk_csr_fatal("RVU_PRIV_ACTIVE_PC", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_ACTIVE_PC bdk_rvu_priv_active_pc_t
#define bustype_BDK_RVU_PRIV_ACTIVE_PC BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_ACTIVE_PC "RVU_PRIV_ACTIVE_PC"
#define device_bar_BDK_RVU_PRIV_ACTIVE_PC 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_ACTIVE_PC 0
#define arguments_BDK_RVU_PRIV_ACTIVE_PC -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_block_type#_rev
 *
 * RVU Privileged Block Type Revision Registers
 * These registers are used by configuration software to specify the revision ID
 * of each block type enumerated by RVU_BLOCK_TYPE_E, to assist VF/PF software
 * discovery.
 */
union bdk_rvu_priv_block_typex_rev
{
    uint64_t u;
    struct bdk_rvu_priv_block_typex_rev_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t rid                   : 8;  /**< [  7:  0](R/W) Revision ID of the block. This is the read value returned by
                                                                 RVU_VF_BLOCK_ADDR()_DISC[RID]. */
#else /* Word 0 - Little Endian */
        uint64_t rid                   : 8;  /**< [  7:  0](R/W) Revision ID of the block. This is the read value returned by
                                                                 RVU_VF_BLOCK_ADDR()_DISC[RID]. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_block_typex_rev_s cn; */
};
typedef union bdk_rvu_priv_block_typex_rev bdk_rvu_priv_block_typex_rev_t;

static inline uint64_t BDK_RVU_PRIV_BLOCK_TYPEX_REV(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_BLOCK_TYPEX_REV(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=15))
        return 0x850008000400ll + 8ll * ((a) & 0xf);
    __bdk_csr_fatal("RVU_PRIV_BLOCK_TYPEX_REV", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_BLOCK_TYPEX_REV(a) bdk_rvu_priv_block_typex_rev_t
#define bustype_BDK_RVU_PRIV_BLOCK_TYPEX_REV(a) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_BLOCK_TYPEX_REV(a) "RVU_PRIV_BLOCK_TYPEX_REV"
#define device_bar_BDK_RVU_PRIV_BLOCK_TYPEX_REV(a) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_BLOCK_TYPEX_REV(a) (a)
#define arguments_BDK_RVU_PRIV_BLOCK_TYPEX_REV(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_clk_cfg
 *
 * RVU Privileged General Configuration Register
 */
union bdk_rvu_priv_clk_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_clk_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t clk_ena               : 1;  /**< [  0:  0](R/W) Force conditional clock to always be enabled. For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t clk_ena               : 1;  /**< [  0:  0](R/W) Force conditional clock to always be enabled. For diagnostic use only. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_clk_cfg_s cn; */
};
typedef union bdk_rvu_priv_clk_cfg bdk_rvu_priv_clk_cfg_t;

#define BDK_RVU_PRIV_CLK_CFG BDK_RVU_PRIV_CLK_CFG_FUNC()
static inline uint64_t BDK_RVU_PRIV_CLK_CFG_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_CLK_CFG_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850008000020ll;
    __bdk_csr_fatal("RVU_PRIV_CLK_CFG", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_CLK_CFG bdk_rvu_priv_clk_cfg_t
#define bustype_BDK_RVU_PRIV_CLK_CFG BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_CLK_CFG "RVU_PRIV_CLK_CFG"
#define device_bar_BDK_RVU_PRIV_CLK_CFG 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_CLK_CFG 0
#define arguments_BDK_RVU_PRIV_CLK_CFG -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_const
 *
 * RVU Privileged Constants Register
 * This register contains constants for software discovery.
 */
union bdk_rvu_priv_const
{
    uint64_t u;
    struct bdk_rvu_priv_const_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_48_63        : 16;
        uint64_t max_vfs_per_pf        : 8;  /**< [ 47: 40](RO) Maximum number of VFs per RVU PF. */
        uint64_t pfs                   : 8;  /**< [ 39: 32](RO) Number of RVU PFs. */
        uint64_t hwvfs                 : 12; /**< [ 31: 20](RO) Number of RVU hardware VFs (HWVFs). */
        uint64_t max_msix              : 20; /**< [ 19:  0](RO) Combined maximum number of MSI-X vectors that may be provisioned to the RVU
                                                                 PFs and VFs. Also the maximum number of 16-byte RVU_MSIX_VEC_S structures
                                                                 in RVU's MSI-X table region in LLC/DRAM. See RVU_PRIV_PF()_MSIX_CFG.

                                                                 Internal:
                                                                 Also, size of RVU's internal PBA memory.

                                                                 Sized as follows:
                                                                 \<pre\>
                                                                 AP cores                     24
                                                                 Vectors per LF:
                                                                    NIX CINT                  32
                                                                    NIX QINT                  32
                                                                    NIX GINT                  1
                                                                    NPA QINT                  32
                                                                    NPA GINT                  1
                                                                    SSO                       1
                                                                    TIM                       1
                                                                    CPT                       1
                                                                    RVU                       1
                                                                    Total per LF:             \<128
                                                                 Num LFs                      256
                                                                 Total LF vectors             \<32K
                                                                 Total AF vectors             64 (budget 16 blocks * 4)
                                                                 Total vectors budget         32K
                                                                 \</pre\> */
#else /* Word 0 - Little Endian */
        uint64_t max_msix              : 20; /**< [ 19:  0](RO) Combined maximum number of MSI-X vectors that may be provisioned to the RVU
                                                                 PFs and VFs. Also the maximum number of 16-byte RVU_MSIX_VEC_S structures
                                                                 in RVU's MSI-X table region in LLC/DRAM. See RVU_PRIV_PF()_MSIX_CFG.

                                                                 Internal:
                                                                 Also, size of RVU's internal PBA memory.

                                                                 Sized as follows:
                                                                 \<pre\>
                                                                 AP cores                     24
                                                                 Vectors per LF:
                                                                    NIX CINT                  32
                                                                    NIX QINT                  32
                                                                    NIX GINT                  1
                                                                    NPA QINT                  32
                                                                    NPA GINT                  1
                                                                    SSO                       1
                                                                    TIM                       1
                                                                    CPT                       1
                                                                    RVU                       1
                                                                    Total per LF:             \<128
                                                                 Num LFs                      256
                                                                 Total LF vectors             \<32K
                                                                 Total AF vectors             64 (budget 16 blocks * 4)
                                                                 Total vectors budget         32K
                                                                 \</pre\> */
        uint64_t hwvfs                 : 12; /**< [ 31: 20](RO) Number of RVU hardware VFs (HWVFs). */
        uint64_t pfs                   : 8;  /**< [ 39: 32](RO) Number of RVU PFs. */
        uint64_t max_vfs_per_pf        : 8;  /**< [ 47: 40](RO) Maximum number of VFs per RVU PF. */
        uint64_t reserved_48_63        : 16;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_const_s cn; */
};
typedef union bdk_rvu_priv_const bdk_rvu_priv_const_t;

#define BDK_RVU_PRIV_CONST BDK_RVU_PRIV_CONST_FUNC()
static inline uint64_t BDK_RVU_PRIV_CONST_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_CONST_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850008000000ll;
    __bdk_csr_fatal("RVU_PRIV_CONST", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_CONST bdk_rvu_priv_const_t
#define bustype_BDK_RVU_PRIV_CONST BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_CONST "RVU_PRIV_CONST"
#define device_bar_BDK_RVU_PRIV_CONST 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_CONST 0
#define arguments_BDK_RVU_PRIV_CONST -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_gen_cfg
 *
 * RVU Privileged General Configuration Register
 */
union bdk_rvu_priv_gen_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_gen_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t lock                  : 1;  /**< [  0:  0](R/W1S) Lock privileged registers. When set, all privileged registers in RVU and
                                                                 its resource blocks are locked down and cannot be modified. Writing a 1
                                                                 sets this bit; once set, the bit can only be cleared by core reset. */
#else /* Word 0 - Little Endian */
        uint64_t lock                  : 1;  /**< [  0:  0](R/W1S) Lock privileged registers. When set, all privileged registers in RVU and
                                                                 its resource blocks are locked down and cannot be modified. Writing a 1
                                                                 sets this bit; once set, the bit can only be cleared by core reset. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_gen_cfg_s cn; */
};
typedef union bdk_rvu_priv_gen_cfg bdk_rvu_priv_gen_cfg_t;

#define BDK_RVU_PRIV_GEN_CFG BDK_RVU_PRIV_GEN_CFG_FUNC()
static inline uint64_t BDK_RVU_PRIV_GEN_CFG_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_GEN_CFG_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850008000010ll;
    __bdk_csr_fatal("RVU_PRIV_GEN_CFG", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_GEN_CFG bdk_rvu_priv_gen_cfg_t
#define bustype_BDK_RVU_PRIV_GEN_CFG BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_GEN_CFG "RVU_PRIV_GEN_CFG"
#define device_bar_BDK_RVU_PRIV_GEN_CFG 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_GEN_CFG 0
#define arguments_BDK_RVU_PRIV_GEN_CFG -1,-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_hwvf#_cpt#_cfg
 *
 * RVU Privileged Hardware VF CPT Configuration Registers
 * Similar to RVU_PRIV_HWVF()_NIX()_CFG, but for CPT({a}) block.
 */
union bdk_rvu_priv_hwvfx_cptx_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_hwvfx_cptx_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
#else /* Word 0 - Little Endian */
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_hwvfx_cptx_cfg_s cn; */
};
typedef union bdk_rvu_priv_hwvfx_cptx_cfg bdk_rvu_priv_hwvfx_cptx_cfg_t;

static inline uint64_t BDK_RVU_PRIV_HWVFX_CPTX_CFG(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_HWVFX_CPTX_CFG(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=255) && (b==0)))
        return 0x850008001350ll + 0x10000ll * ((a) & 0xff) + 8ll * ((b) & 0x0);
    __bdk_csr_fatal("RVU_PRIV_HWVFX_CPTX_CFG", 2, a, b, 0, 0);
}

#define typedef_BDK_RVU_PRIV_HWVFX_CPTX_CFG(a,b) bdk_rvu_priv_hwvfx_cptx_cfg_t
#define bustype_BDK_RVU_PRIV_HWVFX_CPTX_CFG(a,b) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_HWVFX_CPTX_CFG(a,b) "RVU_PRIV_HWVFX_CPTX_CFG"
#define device_bar_BDK_RVU_PRIV_HWVFX_CPTX_CFG(a,b) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_HWVFX_CPTX_CFG(a,b) (a)
#define arguments_BDK_RVU_PRIV_HWVFX_CPTX_CFG(a,b) (a),(b),-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_hwvf#_int_cfg
 *
 * RVU Privileged Hardware VF Interrupt Configuration Registers
 */
union bdk_rvu_priv_hwvfx_int_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_hwvfx_int_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_20_63        : 44;
        uint64_t msix_size             : 8;  /**< [ 19: 12](RO) Number of interrupt vectors enumerated by RVU_VF_INT_VEC_E. */
        uint64_t reserved_11           : 1;
        uint64_t msix_offset           : 11; /**< [ 10:  0](R/W) MSI-X offset. Offset of VF interrupt vectors enumerated by RVU_VF_INT_VEC_E
                                                                 in the HWVF's MSI-X table. This is added to each enumerated value to obtain
                                                                 the corresponding MSI-X vector index.
                                                                 The highest enumerated value plus [MSIX_OFFSET] must be less than or equal
                                                                 to RVU_PRIV_PF()_MSIX_CFG[VF_MSIXT_SIZEM1]. */
#else /* Word 0 - Little Endian */
        uint64_t msix_offset           : 11; /**< [ 10:  0](R/W) MSI-X offset. Offset of VF interrupt vectors enumerated by RVU_VF_INT_VEC_E
                                                                 in the HWVF's MSI-X table. This is added to each enumerated value to obtain
                                                                 the corresponding MSI-X vector index.
                                                                 The highest enumerated value plus [MSIX_OFFSET] must be less than or equal
                                                                 to RVU_PRIV_PF()_MSIX_CFG[VF_MSIXT_SIZEM1]. */
        uint64_t reserved_11           : 1;
        uint64_t msix_size             : 8;  /**< [ 19: 12](RO) Number of interrupt vectors enumerated by RVU_VF_INT_VEC_E. */
        uint64_t reserved_20_63        : 44;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_hwvfx_int_cfg_s cn; */
};
typedef union bdk_rvu_priv_hwvfx_int_cfg bdk_rvu_priv_hwvfx_int_cfg_t;

static inline uint64_t BDK_RVU_PRIV_HWVFX_INT_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_HWVFX_INT_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=255))
        return 0x850008001280ll + 0x10000ll * ((a) & 0xff);
    __bdk_csr_fatal("RVU_PRIV_HWVFX_INT_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_HWVFX_INT_CFG(a) bdk_rvu_priv_hwvfx_int_cfg_t
#define bustype_BDK_RVU_PRIV_HWVFX_INT_CFG(a) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_HWVFX_INT_CFG(a) "RVU_PRIV_HWVFX_INT_CFG"
#define device_bar_BDK_RVU_PRIV_HWVFX_INT_CFG(a) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_HWVFX_INT_CFG(a) (a)
#define arguments_BDK_RVU_PRIV_HWVFX_INT_CFG(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_hwvf#_nix#_cfg
 *
 * RVU Privileged Hardware VF NIX Configuration Registers
 * These registers are used to assist VF software discovery. For each HWVF, if the
 * HWVF is mapped to a VF by RVU_PRIV_PF()_CFG[FIRST_HWVF,NVF], software
 * writes NIX block's resource configuration for the VF in this register. The VF
 * driver can read RVU_VF_BLOCK_ADDR()_DISC to discover the configuration.
 */
union bdk_rvu_priv_hwvfx_nixx_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_hwvfx_nixx_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t has_lf                : 1;  /**< [  0:  0](R/W) Set when an LF from the block is provisioned to the VF, clear otherwise. */
#else /* Word 0 - Little Endian */
        uint64_t has_lf                : 1;  /**< [  0:  0](R/W) Set when an LF from the block is provisioned to the VF, clear otherwise. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_hwvfx_nixx_cfg_s cn; */
};
typedef union bdk_rvu_priv_hwvfx_nixx_cfg bdk_rvu_priv_hwvfx_nixx_cfg_t;

static inline uint64_t BDK_RVU_PRIV_HWVFX_NIXX_CFG(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_HWVFX_NIXX_CFG(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=255) && (b==0)))
        return 0x850008001300ll + 0x10000ll * ((a) & 0xff) + 8ll * ((b) & 0x0);
    __bdk_csr_fatal("RVU_PRIV_HWVFX_NIXX_CFG", 2, a, b, 0, 0);
}

#define typedef_BDK_RVU_PRIV_HWVFX_NIXX_CFG(a,b) bdk_rvu_priv_hwvfx_nixx_cfg_t
#define bustype_BDK_RVU_PRIV_HWVFX_NIXX_CFG(a,b) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_HWVFX_NIXX_CFG(a,b) "RVU_PRIV_HWVFX_NIXX_CFG"
#define device_bar_BDK_RVU_PRIV_HWVFX_NIXX_CFG(a,b) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_HWVFX_NIXX_CFG(a,b) (a)
#define arguments_BDK_RVU_PRIV_HWVFX_NIXX_CFG(a,b) (a),(b),-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_hwvf#_npa_cfg
 *
 * RVU Privileged Hardware VF NPA Configuration Registers
 * Similar to RVU_PRIV_HWVF()_NIX()_CFG, but for NPA block.
 */
union bdk_rvu_priv_hwvfx_npa_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_hwvfx_npa_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t has_lf                : 1;  /**< [  0:  0](R/W) Set when an LF from the block is provisioned to the VF, clear otherwise. */
#else /* Word 0 - Little Endian */
        uint64_t has_lf                : 1;  /**< [  0:  0](R/W) Set when an LF from the block is provisioned to the VF, clear otherwise. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_hwvfx_npa_cfg_s cn; */
};
typedef union bdk_rvu_priv_hwvfx_npa_cfg bdk_rvu_priv_hwvfx_npa_cfg_t;

static inline uint64_t BDK_RVU_PRIV_HWVFX_NPA_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_HWVFX_NPA_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=255))
        return 0x850008001310ll + 0x10000ll * ((a) & 0xff);
    __bdk_csr_fatal("RVU_PRIV_HWVFX_NPA_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_HWVFX_NPA_CFG(a) bdk_rvu_priv_hwvfx_npa_cfg_t
#define bustype_BDK_RVU_PRIV_HWVFX_NPA_CFG(a) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_HWVFX_NPA_CFG(a) "RVU_PRIV_HWVFX_NPA_CFG"
#define device_bar_BDK_RVU_PRIV_HWVFX_NPA_CFG(a) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_HWVFX_NPA_CFG(a) (a)
#define arguments_BDK_RVU_PRIV_HWVFX_NPA_CFG(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_hwvf#_sso_cfg
 *
 * RVU Privileged Hardware VF SSO Configuration Registers
 * Similar to RVU_PRIV_HWVF()_NIX()_CFG, but for SSO block.
 */
union bdk_rvu_priv_hwvfx_sso_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_hwvfx_sso_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
#else /* Word 0 - Little Endian */
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_hwvfx_sso_cfg_s cn; */
};
typedef union bdk_rvu_priv_hwvfx_sso_cfg bdk_rvu_priv_hwvfx_sso_cfg_t;

static inline uint64_t BDK_RVU_PRIV_HWVFX_SSO_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_HWVFX_SSO_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=255))
        return 0x850008001320ll + 0x10000ll * ((a) & 0xff);
    __bdk_csr_fatal("RVU_PRIV_HWVFX_SSO_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_HWVFX_SSO_CFG(a) bdk_rvu_priv_hwvfx_sso_cfg_t
#define bustype_BDK_RVU_PRIV_HWVFX_SSO_CFG(a) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_HWVFX_SSO_CFG(a) "RVU_PRIV_HWVFX_SSO_CFG"
#define device_bar_BDK_RVU_PRIV_HWVFX_SSO_CFG(a) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_HWVFX_SSO_CFG(a) (a)
#define arguments_BDK_RVU_PRIV_HWVFX_SSO_CFG(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_hwvf#_ssow_cfg
 *
 * RVU Privileged Hardware VF SSO Work Slot Configuration Registers
 * Similar to RVU_PRIV_HWVF()_NIX()_CFG, but for SSOW block.
 */
union bdk_rvu_priv_hwvfx_ssow_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_hwvfx_ssow_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
#else /* Word 0 - Little Endian */
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_hwvfx_ssow_cfg_s cn; */
};
typedef union bdk_rvu_priv_hwvfx_ssow_cfg bdk_rvu_priv_hwvfx_ssow_cfg_t;

static inline uint64_t BDK_RVU_PRIV_HWVFX_SSOW_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_HWVFX_SSOW_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=255))
        return 0x850008001330ll + 0x10000ll * ((a) & 0xff);
    __bdk_csr_fatal("RVU_PRIV_HWVFX_SSOW_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_HWVFX_SSOW_CFG(a) bdk_rvu_priv_hwvfx_ssow_cfg_t
#define bustype_BDK_RVU_PRIV_HWVFX_SSOW_CFG(a) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_HWVFX_SSOW_CFG(a) "RVU_PRIV_HWVFX_SSOW_CFG"
#define device_bar_BDK_RVU_PRIV_HWVFX_SSOW_CFG(a) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_HWVFX_SSOW_CFG(a) (a)
#define arguments_BDK_RVU_PRIV_HWVFX_SSOW_CFG(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_hwvf#_tim_cfg
 *
 * RVU Privileged Hardware VF SSO Work Slot Configuration Registers
 * Similar to RVU_PRIV_HWVF()_NIX()_CFG, but for TIM block.
 */
union bdk_rvu_priv_hwvfx_tim_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_hwvfx_tim_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
#else /* Word 0 - Little Endian */
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_hwvfx_tim_cfg_s cn; */
};
typedef union bdk_rvu_priv_hwvfx_tim_cfg bdk_rvu_priv_hwvfx_tim_cfg_t;

static inline uint64_t BDK_RVU_PRIV_HWVFX_TIM_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_HWVFX_TIM_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=255))
        return 0x850008001340ll + 0x10000ll * ((a) & 0xff);
    __bdk_csr_fatal("RVU_PRIV_HWVFX_TIM_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_HWVFX_TIM_CFG(a) bdk_rvu_priv_hwvfx_tim_cfg_t
#define bustype_BDK_RVU_PRIV_HWVFX_TIM_CFG(a) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_HWVFX_TIM_CFG(a) "RVU_PRIV_HWVFX_TIM_CFG"
#define device_bar_BDK_RVU_PRIV_HWVFX_TIM_CFG(a) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_HWVFX_TIM_CFG(a) (a)
#define arguments_BDK_RVU_PRIV_HWVFX_TIM_CFG(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_pf#_cfg
 *
 * RVU Privileged PF Configuration Registers
 */
union bdk_rvu_priv_pfx_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_pfx_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_23_63        : 41;
        uint64_t me_flr_ena            : 1;  /**< [ 22: 22](R/W) Bus master enable (ME) and function level reset (FLR) enable. This bit
                                                                 should be set when the PF is configured and associated PF and/or AF drivers
                                                                 that manage VF and/or PF ME/FLR are loaded.

                                                                 When clear, PCCPF/PCCVF_XXX_CMD[ME] state changes are ignored, and
                                                                 PCCPF/PCCVF_XXX_E_DEV_CTL[BCR_FLR] reset the PF/VF configuration space and
                                                                 MSI-X tables only.

                                                                 When set, hardware updates to the following registers in response to ME/FLR
                                                                 events are additionally enabled:
                                                                 RVU_PF_VFTRPEND(), RVU_PF_VFFLR_INT(), RVU_PF_VFME_INT(),
                                                                 RVU_AF_PFTRPEND, RVU_AF_PFFLR_INT, and RVU_AF_PFFLR_INT. */
        uint64_t af_ena                : 1;  /**< [ 21: 21](R/W) Admin function enable. When set, the PF is allowed to access AF
                                                                 (RVU PF BAR0) registers in all RVU blocks. When clear, the PF is not
                                                                 allowed to access AF registers. Must be clear when [ENA] is clear.

                                                                 Software should keep this bit set for PF(0) when RVU is used. */
        uint64_t ena                   : 1;  /**< [ 20: 20](R/W) Enable the PF. When clear, the PF is unused and hidden in the PCI config
                                                                 space, and access to the PF's MSI-X tables in RVU PF/FUNC BAR2 is
                                                                 disabled.
                                                                 When set, the PF is enabled and remaining fields in this register are
                                                                 valid.

                                                                 Software should keep this bit set for PF(0) when RVU is used. Hardware
                                                                 delivers all AF interrupts to PF(0). */
        uint64_t nvf                   : 8;  /**< [ 19: 12](R/W) Number of VFs in the PF. Must be less than or equal to
                                                                 RVU_PRIV_CONST[MAX_VFS_PER_PF]. */
        uint64_t first_hwvf            : 12; /**< [ 11:  0](R/W) HWVF index of the PF's first VF.  Valid when [NVF] is non-zero. The HWVF
                                                                 index range for the PF is [FIRST_HWVF] to [FIRST_HWVF]+[NVF]-1, inclusive.
                                                                 Different PFs must have non-overlapping HWVF ranges, and the maximum HWVF
                                                                 index in any range must be less than RVU_PRIV_CONST[HWVFS]. */
#else /* Word 0 - Little Endian */
        uint64_t first_hwvf            : 12; /**< [ 11:  0](R/W) HWVF index of the PF's first VF.  Valid when [NVF] is non-zero. The HWVF
                                                                 index range for the PF is [FIRST_HWVF] to [FIRST_HWVF]+[NVF]-1, inclusive.
                                                                 Different PFs must have non-overlapping HWVF ranges, and the maximum HWVF
                                                                 index in any range must be less than RVU_PRIV_CONST[HWVFS]. */
        uint64_t nvf                   : 8;  /**< [ 19: 12](R/W) Number of VFs in the PF. Must be less than or equal to
                                                                 RVU_PRIV_CONST[MAX_VFS_PER_PF]. */
        uint64_t ena                   : 1;  /**< [ 20: 20](R/W) Enable the PF. When clear, the PF is unused and hidden in the PCI config
                                                                 space, and access to the PF's MSI-X tables in RVU PF/FUNC BAR2 is
                                                                 disabled.
                                                                 When set, the PF is enabled and remaining fields in this register are
                                                                 valid.

                                                                 Software should keep this bit set for PF(0) when RVU is used. Hardware
                                                                 delivers all AF interrupts to PF(0). */
        uint64_t af_ena                : 1;  /**< [ 21: 21](R/W) Admin function enable. When set, the PF is allowed to access AF
                                                                 (RVU PF BAR0) registers in all RVU blocks. When clear, the PF is not
                                                                 allowed to access AF registers. Must be clear when [ENA] is clear.

                                                                 Software should keep this bit set for PF(0) when RVU is used. */
        uint64_t me_flr_ena            : 1;  /**< [ 22: 22](R/W) Bus master enable (ME) and function level reset (FLR) enable. This bit
                                                                 should be set when the PF is configured and associated PF and/or AF drivers
                                                                 that manage VF and/or PF ME/FLR are loaded.

                                                                 When clear, PCCPF/PCCVF_XXX_CMD[ME] state changes are ignored, and
                                                                 PCCPF/PCCVF_XXX_E_DEV_CTL[BCR_FLR] reset the PF/VF configuration space and
                                                                 MSI-X tables only.

                                                                 When set, hardware updates to the following registers in response to ME/FLR
                                                                 events are additionally enabled:
                                                                 RVU_PF_VFTRPEND(), RVU_PF_VFFLR_INT(), RVU_PF_VFME_INT(),
                                                                 RVU_AF_PFTRPEND, RVU_AF_PFFLR_INT, and RVU_AF_PFFLR_INT. */
        uint64_t reserved_23_63        : 41;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_pfx_cfg_s cn; */
};
typedef union bdk_rvu_priv_pfx_cfg bdk_rvu_priv_pfx_cfg_t;

static inline uint64_t BDK_RVU_PRIV_PFX_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_PFX_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=15))
        return 0x850008000100ll + 0x10000ll * ((a) & 0xf);
    __bdk_csr_fatal("RVU_PRIV_PFX_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_PFX_CFG(a) bdk_rvu_priv_pfx_cfg_t
#define bustype_BDK_RVU_PRIV_PFX_CFG(a) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_PFX_CFG(a) "RVU_PRIV_PFX_CFG"
#define device_bar_BDK_RVU_PRIV_PFX_CFG(a) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_PFX_CFG(a) (a)
#define arguments_BDK_RVU_PRIV_PFX_CFG(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_pf#_cpt#_cfg
 *
 * RVU Privileged PF CPT Configuration Registers
 * Similar to RVU_PRIV_PF()_NIX()_CFG, but for CPT({a}) block.
 */
union bdk_rvu_priv_pfx_cptx_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_pfx_cptx_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
#else /* Word 0 - Little Endian */
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_pfx_cptx_cfg_s cn; */
};
typedef union bdk_rvu_priv_pfx_cptx_cfg bdk_rvu_priv_pfx_cptx_cfg_t;

static inline uint64_t BDK_RVU_PRIV_PFX_CPTX_CFG(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_PFX_CPTX_CFG(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=15) && (b==0)))
        return 0x850008000350ll + 0x10000ll * ((a) & 0xf) + 8ll * ((b) & 0x0);
    __bdk_csr_fatal("RVU_PRIV_PFX_CPTX_CFG", 2, a, b, 0, 0);
}

#define typedef_BDK_RVU_PRIV_PFX_CPTX_CFG(a,b) bdk_rvu_priv_pfx_cptx_cfg_t
#define bustype_BDK_RVU_PRIV_PFX_CPTX_CFG(a,b) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_PFX_CPTX_CFG(a,b) "RVU_PRIV_PFX_CPTX_CFG"
#define device_bar_BDK_RVU_PRIV_PFX_CPTX_CFG(a,b) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_PFX_CPTX_CFG(a,b) (a)
#define arguments_BDK_RVU_PRIV_PFX_CPTX_CFG(a,b) (a),(b),-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_pf#_id_cfg
 *
 * RVU Privileged PF ID Configuration Registers
 */
union bdk_rvu_priv_pfx_id_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_pfx_id_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_40_63        : 24;
        uint64_t class_code            : 24; /**< [ 39: 16](R/W) Class code to be presented in PCCPF_XXX_REV[BCC,SC,PI] and
                                                                 PCCVF_XXX_REV[BCC,SC,PI]. Format specified by PCC_CLASS_CODE_S.
                                                                 Resets to PCC_DEV_IDL_E::RVU's class code. */
        uint64_t vf_devid              : 8;  /**< [ 15:  8](R/W) Lower bits of VF device ID to be presented in PCCPF_XXX_SRIOV_DEV[VFDEV]\<7:0\>.
                                                                 Resets to PCC_DEV_IDL_E::RVU_VF. */
        uint64_t pf_devid              : 8;  /**< [  7:  0](R/W) Lower bits of PF device ID to be presented in PCCPF_XXX_ID[DEVID]\<7:0\>.
                                                                 Resets to PCC_DEV_IDL_E::RVU_AF for PF(0), PCC_DEV_IDL_E::RVU for other
                                                                 PFs. */
#else /* Word 0 - Little Endian */
        uint64_t pf_devid              : 8;  /**< [  7:  0](R/W) Lower bits of PF device ID to be presented in PCCPF_XXX_ID[DEVID]\<7:0\>.
                                                                 Resets to PCC_DEV_IDL_E::RVU_AF for PF(0), PCC_DEV_IDL_E::RVU for other
                                                                 PFs. */
        uint64_t vf_devid              : 8;  /**< [ 15:  8](R/W) Lower bits of VF device ID to be presented in PCCPF_XXX_SRIOV_DEV[VFDEV]\<7:0\>.
                                                                 Resets to PCC_DEV_IDL_E::RVU_VF. */
        uint64_t class_code            : 24; /**< [ 39: 16](R/W) Class code to be presented in PCCPF_XXX_REV[BCC,SC,PI] and
                                                                 PCCVF_XXX_REV[BCC,SC,PI]. Format specified by PCC_CLASS_CODE_S.
                                                                 Resets to PCC_DEV_IDL_E::RVU's class code. */
        uint64_t reserved_40_63        : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_pfx_id_cfg_s cn; */
};
typedef union bdk_rvu_priv_pfx_id_cfg bdk_rvu_priv_pfx_id_cfg_t;

static inline uint64_t BDK_RVU_PRIV_PFX_ID_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_PFX_ID_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=15))
        return 0x850008000120ll + 0x10000ll * ((a) & 0xf);
    __bdk_csr_fatal("RVU_PRIV_PFX_ID_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_PFX_ID_CFG(a) bdk_rvu_priv_pfx_id_cfg_t
#define bustype_BDK_RVU_PRIV_PFX_ID_CFG(a) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_PFX_ID_CFG(a) "RVU_PRIV_PFX_ID_CFG"
#define device_bar_BDK_RVU_PRIV_PFX_ID_CFG(a) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_PFX_ID_CFG(a) (a)
#define arguments_BDK_RVU_PRIV_PFX_ID_CFG(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_pf#_int_cfg
 *
 * RVU Privileged PF Interrupt Configuration Registers
 */
union bdk_rvu_priv_pfx_int_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_pfx_int_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_20_63        : 44;
        uint64_t msix_size             : 8;  /**< [ 19: 12](RO) Number of interrupt vectors enumerated by RVU_PF_INT_VEC_E. */
        uint64_t reserved_11           : 1;
        uint64_t msix_offset           : 11; /**< [ 10:  0](R/W) MSI-X offset. Offset of PF interrupt vectors enumerated by RVU_PF_INT_VEC_E
                                                                 in the PF's MSI-X table. This is added to each enumerated value to obtain
                                                                 the corresponding MSI-X vector index.
                                                                 The highest enumerated value plus [MSIX_OFFSET] must be less than or equal
                                                                 to RVU_PRIV_PF()_MSIX_CFG[PF_MSIXT_SIZEM1].

                                                                 Note that the AF interrupt vectors enumerated by RVU_AF_INT_VEC_E have a
                                                                 fixed starting offset of 0 in RVU PF(0)'s MSI-X table. Other PF
                                                                 interrupt vectors should not be mapped at the offsets used by RVU_AF_INT_VEC_E. */
#else /* Word 0 - Little Endian */
        uint64_t msix_offset           : 11; /**< [ 10:  0](R/W) MSI-X offset. Offset of PF interrupt vectors enumerated by RVU_PF_INT_VEC_E
                                                                 in the PF's MSI-X table. This is added to each enumerated value to obtain
                                                                 the corresponding MSI-X vector index.
                                                                 The highest enumerated value plus [MSIX_OFFSET] must be less than or equal
                                                                 to RVU_PRIV_PF()_MSIX_CFG[PF_MSIXT_SIZEM1].

                                                                 Note that the AF interrupt vectors enumerated by RVU_AF_INT_VEC_E have a
                                                                 fixed starting offset of 0 in RVU PF(0)'s MSI-X table. Other PF
                                                                 interrupt vectors should not be mapped at the offsets used by RVU_AF_INT_VEC_E. */
        uint64_t reserved_11           : 1;
        uint64_t msix_size             : 8;  /**< [ 19: 12](RO) Number of interrupt vectors enumerated by RVU_PF_INT_VEC_E. */
        uint64_t reserved_20_63        : 44;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_pfx_int_cfg_s cn; */
};
typedef union bdk_rvu_priv_pfx_int_cfg bdk_rvu_priv_pfx_int_cfg_t;

static inline uint64_t BDK_RVU_PRIV_PFX_INT_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_PFX_INT_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=15))
        return 0x850008000200ll + 0x10000ll * ((a) & 0xf);
    __bdk_csr_fatal("RVU_PRIV_PFX_INT_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_PFX_INT_CFG(a) bdk_rvu_priv_pfx_int_cfg_t
#define bustype_BDK_RVU_PRIV_PFX_INT_CFG(a) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_PFX_INT_CFG(a) "RVU_PRIV_PFX_INT_CFG"
#define device_bar_BDK_RVU_PRIV_PFX_INT_CFG(a) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_PFX_INT_CFG(a) (a)
#define arguments_BDK_RVU_PRIV_PFX_INT_CFG(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_pf#_msix_cfg
 *
 * RVU Privileged PF MSI-X Configuration Registers
 * These registers specify MSI-X table sizes and locations for RVU PFs and
 * associated VFs. Hardware maintains all RVU MSI-X tables in a contiguous memory
 * region in LLC/DRAM called the MSI-X table region. The table region's base IOVA
 * is specified by RVU_AF_MSIXTR_BASE, and its size as a multiple of
 * 16-byte RVU_MSIX_VEC_S structures must be less than or equal to
 * RVU_PRIV_CONST[MAX_MSIX].
 *
 * A PF's MSI-X table consists of the following range of RVU_MSIX_VEC_S structures
 * in the table region:
 * * First index: [PF_MSIXT_OFFSET].
 * * Last index: [PF_MSIXT_OFFSET] + [PF_MSIXT_SIZEM1].
 *
 * If a PF has enabled VFs (associated RVU_PRIV_PF()_CFG[NVF] is nonzero),
 * then each VF's MSI-X table consumes the following range of RVU_MSIX_VEC_S structures:
 * * First index: [VF_MSIXT_OFFSET] + N*([VF_MSIXT_SIZEM1] + 1).
 * * Last index: [VF_MSIXT_OFFSET] + N*([VF_MSIXT_SIZEM1] + 1) + [VF_MSIXT_SIZEM1].
 *
 * N=0 for the first VF, N=1 for the second VF, etc.
 *
 * Different PFs and VFs must have non-overlapping vector ranges, and the last
 * index of any range must be less than RVU_PRIV_CONST[MAX_MSIX].
 */
union bdk_rvu_priv_pfx_msix_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_pfx_msix_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pf_msixt_offset       : 20; /**< [ 63: 44](R/W) Starting offset of PF's MSI-X table in the RVU MSI-X table region.
                                                                 Internal:
                                                                 Also, bit offset of the PF's PBA table in RVU's internal PBA memory. */
        uint64_t pf_msixt_sizem1       : 12; /**< [ 43: 32](R/W) PF's MSI-X table size (number of MSI-X vectors) minus one. */
        uint64_t vf_msixt_offset       : 20; /**< [ 31: 12](R/W) Starting offset of first VF's MSI-X table in the RVU MSI-X table region.
                                                                 Valid when RVU_PRIV_PF()_CFG[NVF] is nonzero.

                                                                 Internal:
                                                                 Also, bit offset of the first VF's PBA table in RVU's internal PBA memory. */
        uint64_t vf_msixt_sizem1       : 12; /**< [ 11:  0](R/W) Each VF's MSI-X table size (number of MSI-X vectors) minus one.
                                                                 Valid when RVU_PRIV_PF()_CFG[NVF] is nonzero. */
#else /* Word 0 - Little Endian */
        uint64_t vf_msixt_sizem1       : 12; /**< [ 11:  0](R/W) Each VF's MSI-X table size (number of MSI-X vectors) minus one.
                                                                 Valid when RVU_PRIV_PF()_CFG[NVF] is nonzero. */
        uint64_t vf_msixt_offset       : 20; /**< [ 31: 12](R/W) Starting offset of first VF's MSI-X table in the RVU MSI-X table region.
                                                                 Valid when RVU_PRIV_PF()_CFG[NVF] is nonzero.

                                                                 Internal:
                                                                 Also, bit offset of the first VF's PBA table in RVU's internal PBA memory. */
        uint64_t pf_msixt_sizem1       : 12; /**< [ 43: 32](R/W) PF's MSI-X table size (number of MSI-X vectors) minus one. */
        uint64_t pf_msixt_offset       : 20; /**< [ 63: 44](R/W) Starting offset of PF's MSI-X table in the RVU MSI-X table region.
                                                                 Internal:
                                                                 Also, bit offset of the PF's PBA table in RVU's internal PBA memory. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_pfx_msix_cfg_s cn; */
};
typedef union bdk_rvu_priv_pfx_msix_cfg bdk_rvu_priv_pfx_msix_cfg_t;

static inline uint64_t BDK_RVU_PRIV_PFX_MSIX_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_PFX_MSIX_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=15))
        return 0x850008000110ll + 0x10000ll * ((a) & 0xf);
    __bdk_csr_fatal("RVU_PRIV_PFX_MSIX_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_PFX_MSIX_CFG(a) bdk_rvu_priv_pfx_msix_cfg_t
#define bustype_BDK_RVU_PRIV_PFX_MSIX_CFG(a) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_PFX_MSIX_CFG(a) "RVU_PRIV_PFX_MSIX_CFG"
#define device_bar_BDK_RVU_PRIV_PFX_MSIX_CFG(a) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_PFX_MSIX_CFG(a) (a)
#define arguments_BDK_RVU_PRIV_PFX_MSIX_CFG(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_pf#_nix#_cfg
 *
 * RVU Privileged PF NIX Configuration Registers
 * These registers are used to assist PF software discovery. For each enabled RVU
 * PF, software writes the block's resource configuration for the PF in this
 * register. The PF driver can read RVU_PF_BLOCK_ADDR()_DISC to discover the
 * configuration.
 */
union bdk_rvu_priv_pfx_nixx_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_pfx_nixx_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t has_lf                : 1;  /**< [  0:  0](R/W) Set when an LF from the block is provisioned to the VF, clear otherwise. */
#else /* Word 0 - Little Endian */
        uint64_t has_lf                : 1;  /**< [  0:  0](R/W) Set when an LF from the block is provisioned to the VF, clear otherwise. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_pfx_nixx_cfg_s cn; */
};
typedef union bdk_rvu_priv_pfx_nixx_cfg bdk_rvu_priv_pfx_nixx_cfg_t;

static inline uint64_t BDK_RVU_PRIV_PFX_NIXX_CFG(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_PFX_NIXX_CFG(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=15) && (b==0)))
        return 0x850008000300ll + 0x10000ll * ((a) & 0xf) + 8ll * ((b) & 0x0);
    __bdk_csr_fatal("RVU_PRIV_PFX_NIXX_CFG", 2, a, b, 0, 0);
}

#define typedef_BDK_RVU_PRIV_PFX_NIXX_CFG(a,b) bdk_rvu_priv_pfx_nixx_cfg_t
#define bustype_BDK_RVU_PRIV_PFX_NIXX_CFG(a,b) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_PFX_NIXX_CFG(a,b) "RVU_PRIV_PFX_NIXX_CFG"
#define device_bar_BDK_RVU_PRIV_PFX_NIXX_CFG(a,b) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_PFX_NIXX_CFG(a,b) (a)
#define arguments_BDK_RVU_PRIV_PFX_NIXX_CFG(a,b) (a),(b),-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_pf#_npa_cfg
 *
 * RVU Privileged PF NPA Configuration Registers
 * Similar to RVU_PRIV_PF()_NIX()_CFG, but for NPA block.
 */
union bdk_rvu_priv_pfx_npa_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_pfx_npa_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t has_lf                : 1;  /**< [  0:  0](R/W) Set when an LF from the block is provisioned to the VF, clear otherwise. */
#else /* Word 0 - Little Endian */
        uint64_t has_lf                : 1;  /**< [  0:  0](R/W) Set when an LF from the block is provisioned to the VF, clear otherwise. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_pfx_npa_cfg_s cn; */
};
typedef union bdk_rvu_priv_pfx_npa_cfg bdk_rvu_priv_pfx_npa_cfg_t;

static inline uint64_t BDK_RVU_PRIV_PFX_NPA_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_PFX_NPA_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=15))
        return 0x850008000310ll + 0x10000ll * ((a) & 0xf);
    __bdk_csr_fatal("RVU_PRIV_PFX_NPA_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_PFX_NPA_CFG(a) bdk_rvu_priv_pfx_npa_cfg_t
#define bustype_BDK_RVU_PRIV_PFX_NPA_CFG(a) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_PFX_NPA_CFG(a) "RVU_PRIV_PFX_NPA_CFG"
#define device_bar_BDK_RVU_PRIV_PFX_NPA_CFG(a) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_PFX_NPA_CFG(a) (a)
#define arguments_BDK_RVU_PRIV_PFX_NPA_CFG(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_pf#_sso_cfg
 *
 * RVU Privileged PF SSO Configuration Registers
 * Similar to RVU_PRIV_PF()_NIX()_CFG, but for SSO block.
 */
union bdk_rvu_priv_pfx_sso_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_pfx_sso_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
#else /* Word 0 - Little Endian */
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_pfx_sso_cfg_s cn; */
};
typedef union bdk_rvu_priv_pfx_sso_cfg bdk_rvu_priv_pfx_sso_cfg_t;

static inline uint64_t BDK_RVU_PRIV_PFX_SSO_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_PFX_SSO_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=15))
        return 0x850008000320ll + 0x10000ll * ((a) & 0xf);
    __bdk_csr_fatal("RVU_PRIV_PFX_SSO_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_PFX_SSO_CFG(a) bdk_rvu_priv_pfx_sso_cfg_t
#define bustype_BDK_RVU_PRIV_PFX_SSO_CFG(a) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_PFX_SSO_CFG(a) "RVU_PRIV_PFX_SSO_CFG"
#define device_bar_BDK_RVU_PRIV_PFX_SSO_CFG(a) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_PFX_SSO_CFG(a) (a)
#define arguments_BDK_RVU_PRIV_PFX_SSO_CFG(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_pf#_ssow_cfg
 *
 * RVU Privileged PF SSO Work Slot Configuration Registers
 * Similar to RVU_PRIV_PF()_NIX()_CFG, but for SSOW block.
 */
union bdk_rvu_priv_pfx_ssow_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_pfx_ssow_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
#else /* Word 0 - Little Endian */
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_pfx_ssow_cfg_s cn; */
};
typedef union bdk_rvu_priv_pfx_ssow_cfg bdk_rvu_priv_pfx_ssow_cfg_t;

static inline uint64_t BDK_RVU_PRIV_PFX_SSOW_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_PFX_SSOW_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=15))
        return 0x850008000330ll + 0x10000ll * ((a) & 0xf);
    __bdk_csr_fatal("RVU_PRIV_PFX_SSOW_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_PFX_SSOW_CFG(a) bdk_rvu_priv_pfx_ssow_cfg_t
#define bustype_BDK_RVU_PRIV_PFX_SSOW_CFG(a) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_PFX_SSOW_CFG(a) "RVU_PRIV_PFX_SSOW_CFG"
#define device_bar_BDK_RVU_PRIV_PFX_SSOW_CFG(a) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_PFX_SSOW_CFG(a) (a)
#define arguments_BDK_RVU_PRIV_PFX_SSOW_CFG(a) (a),-1,-1,-1

/**
 * Register (RVU_PF_BAR0) rvu_priv_pf#_tim_cfg
 *
 * RVU Privileged PF SSO Work Slot Configuration Registers
 * Similar to RVU_PRIV_PF()_NIX()_CFG, but for TIM block.
 */
union bdk_rvu_priv_pfx_tim_cfg
{
    uint64_t u;
    struct bdk_rvu_priv_pfx_tim_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
#else /* Word 0 - Little Endian */
        uint64_t num_lfs               : 9;  /**< [  8:  0](R/W) Number of LFs from the block that are provisioned to the PF/VF. When non-zero,
                                                                 the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in the the block. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_priv_pfx_tim_cfg_s cn; */
};
typedef union bdk_rvu_priv_pfx_tim_cfg bdk_rvu_priv_pfx_tim_cfg_t;

static inline uint64_t BDK_RVU_PRIV_PFX_TIM_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_PRIV_PFX_TIM_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=15))
        return 0x850008000340ll + 0x10000ll * ((a) & 0xf);
    __bdk_csr_fatal("RVU_PRIV_PFX_TIM_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_PRIV_PFX_TIM_CFG(a) bdk_rvu_priv_pfx_tim_cfg_t
#define bustype_BDK_RVU_PRIV_PFX_TIM_CFG(a) BDK_CSR_TYPE_RVU_PF_BAR0
#define basename_BDK_RVU_PRIV_PFX_TIM_CFG(a) "RVU_PRIV_PFX_TIM_CFG"
#define device_bar_BDK_RVU_PRIV_PFX_TIM_CFG(a) 0x0 /* BAR0 */
#define busnum_BDK_RVU_PRIV_PFX_TIM_CFG(a) (a)
#define arguments_BDK_RVU_PRIV_PFX_TIM_CFG(a) (a),-1,-1,-1

/**
 * Register (RVU_VF_BAR2) rvu_vf_block_addr#_disc
 *
 * RVU VF Block Address Discovery Registers
 * These registers allow each VF driver to discover block resources that are
 * provisioned to its VF. The register's block address index is enumerated by
 * RVU_BLOCK_ADDR_E.
 */
union bdk_rvu_vf_block_addrx_disc
{
    uint64_t u;
    struct bdk_rvu_vf_block_addrx_disc_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_28_63        : 36;
        uint64_t btype                 : 8;  /**< [ 27: 20](RO/H) Block type enumerated by RVU_BLOCK_TYPE_E. */
        uint64_t rid                   : 8;  /**< [ 19: 12](RO/H) Revision ID of the block from RVU_PRIV_BLOCK_TYPE()_REV[RID]. */
        uint64_t imp                   : 1;  /**< [ 11: 11](RO/H) Implemented. When set, a block is present at this block address index as
                                                                 enumerated by RVU_BLOCK_ADDR_E. When clear, a block is not present and the
                                                                 remaining fields in the register are RAZ.

                                                                 Internal:
                                                                 Returns zero if the block is implemented but fused out. */
        uint64_t reserved_9_10         : 2;
        uint64_t num_lfs               : 9;  /**< [  8:  0](RO/H) Number of local functions from the block that are provisioned to the VF/PF.
                                                                 When non-zero, the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in
                                                                 the the block.
                                                                 Returns 0 for block types that do not have local functions, 0 or 1 for
                                                                 single-slot blocks; see RVU_BLOCK_TYPE_E. */
#else /* Word 0 - Little Endian */
        uint64_t num_lfs               : 9;  /**< [  8:  0](RO/H) Number of local functions from the block that are provisioned to the VF/PF.
                                                                 When non-zero, the provisioned LFs are mapped to slots 0 to [NUM_LFS]-1 in
                                                                 the the block.
                                                                 Returns 0 for block types that do not have local functions, 0 or 1 for
                                                                 single-slot blocks; see RVU_BLOCK_TYPE_E. */
        uint64_t reserved_9_10         : 2;
        uint64_t imp                   : 1;  /**< [ 11: 11](RO/H) Implemented. When set, a block is present at this block address index as
                                                                 enumerated by RVU_BLOCK_ADDR_E. When clear, a block is not present and the
                                                                 remaining fields in the register are RAZ.

                                                                 Internal:
                                                                 Returns zero if the block is implemented but fused out. */
        uint64_t rid                   : 8;  /**< [ 19: 12](RO/H) Revision ID of the block from RVU_PRIV_BLOCK_TYPE()_REV[RID]. */
        uint64_t btype                 : 8;  /**< [ 27: 20](RO/H) Block type enumerated by RVU_BLOCK_TYPE_E. */
        uint64_t reserved_28_63        : 36;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_vf_block_addrx_disc_s cn; */
};
typedef union bdk_rvu_vf_block_addrx_disc bdk_rvu_vf_block_addrx_disc_t;

static inline uint64_t BDK_RVU_VF_BLOCK_ADDRX_DISC(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_VF_BLOCK_ADDRX_DISC(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=31))
        return 0x850200000200ll + 8ll * ((a) & 0x1f);
    __bdk_csr_fatal("RVU_VF_BLOCK_ADDRX_DISC", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_VF_BLOCK_ADDRX_DISC(a) bdk_rvu_vf_block_addrx_disc_t
#define bustype_BDK_RVU_VF_BLOCK_ADDRX_DISC(a) BDK_CSR_TYPE_RVU_VF_BAR2
#define basename_BDK_RVU_VF_BLOCK_ADDRX_DISC(a) "RVU_VF_BLOCK_ADDRX_DISC"
#define device_bar_BDK_RVU_VF_BLOCK_ADDRX_DISC(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_VF_BLOCK_ADDRX_DISC(a) (a)
#define arguments_BDK_RVU_VF_BLOCK_ADDRX_DISC(a) (a),-1,-1,-1

/**
 * Register (RVU_VF_BAR2) rvu_vf_int
 *
 * RVU VF Interrupt Registers
 */
union bdk_rvu_vf_int
{
    uint64_t u;
    struct bdk_rvu_vf_int_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1C/H) PF to VF mailbox interrupt. Set when RVU_PF_VF()_PFVF_MBOX(0) is written. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1C/H) PF to VF mailbox interrupt. Set when RVU_PF_VF()_PFVF_MBOX(0) is written. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_vf_int_s cn; */
};
typedef union bdk_rvu_vf_int bdk_rvu_vf_int_t;

#define BDK_RVU_VF_INT BDK_RVU_VF_INT_FUNC()
static inline uint64_t BDK_RVU_VF_INT_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_VF_INT_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850200000020ll;
    __bdk_csr_fatal("RVU_VF_INT", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_VF_INT bdk_rvu_vf_int_t
#define bustype_BDK_RVU_VF_INT BDK_CSR_TYPE_RVU_VF_BAR2
#define basename_BDK_RVU_VF_INT "RVU_VF_INT"
#define device_bar_BDK_RVU_VF_INT 0x2 /* BAR2 */
#define busnum_BDK_RVU_VF_INT 0
#define arguments_BDK_RVU_VF_INT -1,-1,-1,-1

/**
 * Register (RVU_VF_BAR2) rvu_vf_int_ena_w1c
 *
 * RVU VF Interrupt Enable Clear Register
 * This register clears interrupt enable bits.
 */
union bdk_rvu_vf_int_ena_w1c
{
    uint64_t u;
    struct bdk_rvu_vf_int_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for RVU_VF_INT[MBOX]. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for RVU_VF_INT[MBOX]. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_vf_int_ena_w1c_s cn; */
};
typedef union bdk_rvu_vf_int_ena_w1c bdk_rvu_vf_int_ena_w1c_t;

#define BDK_RVU_VF_INT_ENA_W1C BDK_RVU_VF_INT_ENA_W1C_FUNC()
static inline uint64_t BDK_RVU_VF_INT_ENA_W1C_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_VF_INT_ENA_W1C_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850200000038ll;
    __bdk_csr_fatal("RVU_VF_INT_ENA_W1C", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_VF_INT_ENA_W1C bdk_rvu_vf_int_ena_w1c_t
#define bustype_BDK_RVU_VF_INT_ENA_W1C BDK_CSR_TYPE_RVU_VF_BAR2
#define basename_BDK_RVU_VF_INT_ENA_W1C "RVU_VF_INT_ENA_W1C"
#define device_bar_BDK_RVU_VF_INT_ENA_W1C 0x2 /* BAR2 */
#define busnum_BDK_RVU_VF_INT_ENA_W1C 0
#define arguments_BDK_RVU_VF_INT_ENA_W1C -1,-1,-1,-1

/**
 * Register (RVU_VF_BAR2) rvu_vf_int_ena_w1s
 *
 * RVU VF Interrupt Enable Set Register
 * This register sets interrupt enable bits.
 */
union bdk_rvu_vf_int_ena_w1s
{
    uint64_t u;
    struct bdk_rvu_vf_int_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for RVU_VF_INT[MBOX]. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for RVU_VF_INT[MBOX]. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_vf_int_ena_w1s_s cn; */
};
typedef union bdk_rvu_vf_int_ena_w1s bdk_rvu_vf_int_ena_w1s_t;

#define BDK_RVU_VF_INT_ENA_W1S BDK_RVU_VF_INT_ENA_W1S_FUNC()
static inline uint64_t BDK_RVU_VF_INT_ENA_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_VF_INT_ENA_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850200000030ll;
    __bdk_csr_fatal("RVU_VF_INT_ENA_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_VF_INT_ENA_W1S bdk_rvu_vf_int_ena_w1s_t
#define bustype_BDK_RVU_VF_INT_ENA_W1S BDK_CSR_TYPE_RVU_VF_BAR2
#define basename_BDK_RVU_VF_INT_ENA_W1S "RVU_VF_INT_ENA_W1S"
#define device_bar_BDK_RVU_VF_INT_ENA_W1S 0x2 /* BAR2 */
#define busnum_BDK_RVU_VF_INT_ENA_W1S 0
#define arguments_BDK_RVU_VF_INT_ENA_W1S -1,-1,-1,-1

/**
 * Register (RVU_VF_BAR2) rvu_vf_int_w1s
 *
 * RVU VF Interrupt Set Register
 * This register sets interrupt bits.
 */
union bdk_rvu_vf_int_w1s
{
    uint64_t u;
    struct bdk_rvu_vf_int_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1S/H) Reads or sets RVU_VF_INT[MBOX]. */
#else /* Word 0 - Little Endian */
        uint64_t mbox                  : 1;  /**< [  0:  0](R/W1S/H) Reads or sets RVU_VF_INT[MBOX]. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_vf_int_w1s_s cn; */
};
typedef union bdk_rvu_vf_int_w1s bdk_rvu_vf_int_w1s_t;

#define BDK_RVU_VF_INT_W1S BDK_RVU_VF_INT_W1S_FUNC()
static inline uint64_t BDK_RVU_VF_INT_W1S_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_VF_INT_W1S_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x850200000028ll;
    __bdk_csr_fatal("RVU_VF_INT_W1S", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RVU_VF_INT_W1S bdk_rvu_vf_int_w1s_t
#define bustype_BDK_RVU_VF_INT_W1S BDK_CSR_TYPE_RVU_VF_BAR2
#define basename_BDK_RVU_VF_INT_W1S "RVU_VF_INT_W1S"
#define device_bar_BDK_RVU_VF_INT_W1S 0x2 /* BAR2 */
#define busnum_BDK_RVU_VF_INT_W1S 0
#define arguments_BDK_RVU_VF_INT_W1S -1,-1,-1,-1

/**
 * Register (RVU_VF_BAR2) rvu_vf_msix_pba#
 *
 * RVU VF MSI-X Pending-Bit-Array Registers
 * This register is the MSI-X VF PBA table.
 */
union bdk_rvu_vf_msix_pbax
{
    uint64_t u;
    struct bdk_rvu_vf_msix_pbax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO/H) Pending message bit for each MSI-X vector, i.e. one bit per
                                                                 RVU_VF_MSIX_VEC()_CTL register.
                                                                 The total number of bits for a given VF (and thus the number of PBA
                                                                 registers) is determined by RVU_PRIV_PF()_MSIX_CFG[VF_MSIXT_SIZEM1]
                                                                 (plus 1). */
#else /* Word 0 - Little Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO/H) Pending message bit for each MSI-X vector, i.e. one bit per
                                                                 RVU_VF_MSIX_VEC()_CTL register.
                                                                 The total number of bits for a given VF (and thus the number of PBA
                                                                 registers) is determined by RVU_PRIV_PF()_MSIX_CFG[VF_MSIXT_SIZEM1]
                                                                 (plus 1). */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_vf_msix_pbax_s cn; */
};
typedef union bdk_rvu_vf_msix_pbax bdk_rvu_vf_msix_pbax_t;

static inline uint64_t BDK_RVU_VF_MSIX_PBAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_VF_MSIX_PBAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a==0))
        return 0x8502002f0000ll + 8ll * ((a) & 0x0);
    __bdk_csr_fatal("RVU_VF_MSIX_PBAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_VF_MSIX_PBAX(a) bdk_rvu_vf_msix_pbax_t
#define bustype_BDK_RVU_VF_MSIX_PBAX(a) BDK_CSR_TYPE_RVU_VF_BAR2
#define basename_BDK_RVU_VF_MSIX_PBAX(a) "RVU_VF_MSIX_PBAX"
#define device_bar_BDK_RVU_VF_MSIX_PBAX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_VF_MSIX_PBAX(a) (a)
#define arguments_BDK_RVU_VF_MSIX_PBAX(a) (a),-1,-1,-1

/**
 * Register (RVU_VF_BAR2) rvu_vf_msix_vec#_addr
 *
 * RVU VF MSI-X Vector-Table Address Registers
 * These registers and RVU_VF_MSIX_VEC()_CTL form the VF MSI-X vector table.
 * The number of MSI-X vectors for a given VF is specified by
 * RVU_PRIV_PF()_MSIX_CFG[VF_MSIXT_SIZEM1] (plus 1).
 *
 * Internal:
 * VF vector count of 128 allows up to that number to be provisioned to the VF
 * from LF resources of various blocks.
 */
union bdk_rvu_vf_msix_vecx_addr
{
    uint64_t u;
    struct bdk_rvu_vf_msix_vecx_addr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_53_63        : 11;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](RAZ) Secure vector. Zero as not supported for RVU vectors. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](RAZ) Secure vector. Zero as not supported for RVU vectors. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_53_63        : 11;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_vf_msix_vecx_addr_s cn; */
};
typedef union bdk_rvu_vf_msix_vecx_addr bdk_rvu_vf_msix_vecx_addr_t;

static inline uint64_t BDK_RVU_VF_MSIX_VECX_ADDR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_VF_MSIX_VECX_ADDR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a==0))
        return 0x850200200000ll + 0x10ll * ((a) & 0x0);
    __bdk_csr_fatal("RVU_VF_MSIX_VECX_ADDR", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_VF_MSIX_VECX_ADDR(a) bdk_rvu_vf_msix_vecx_addr_t
#define bustype_BDK_RVU_VF_MSIX_VECX_ADDR(a) BDK_CSR_TYPE_RVU_VF_BAR2
#define basename_BDK_RVU_VF_MSIX_VECX_ADDR(a) "RVU_VF_MSIX_VECX_ADDR"
#define device_bar_BDK_RVU_VF_MSIX_VECX_ADDR(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_VF_MSIX_VECX_ADDR(a) (a)
#define arguments_BDK_RVU_VF_MSIX_VECX_ADDR(a) (a),-1,-1,-1

/**
 * Register (RVU_VF_BAR2) rvu_vf_msix_vec#_ctl
 *
 * RVU VF MSI-X Vector-Table Control and Data Registers
 * These registers and RVU_VF_MSIX_VEC()_ADDR form the VF MSI-X vector table.
 */
union bdk_rvu_vf_msix_vecx_ctl
{
    uint64_t u;
    struct bdk_rvu_vf_msix_vecx_ctl_s
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
    /* struct bdk_rvu_vf_msix_vecx_ctl_s cn; */
};
typedef union bdk_rvu_vf_msix_vecx_ctl bdk_rvu_vf_msix_vecx_ctl_t;

static inline uint64_t BDK_RVU_VF_MSIX_VECX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_VF_MSIX_VECX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a==0))
        return 0x850200200008ll + 0x10ll * ((a) & 0x0);
    __bdk_csr_fatal("RVU_VF_MSIX_VECX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_VF_MSIX_VECX_CTL(a) bdk_rvu_vf_msix_vecx_ctl_t
#define bustype_BDK_RVU_VF_MSIX_VECX_CTL(a) BDK_CSR_TYPE_RVU_VF_BAR2
#define basename_BDK_RVU_VF_MSIX_VECX_CTL(a) "RVU_VF_MSIX_VECX_CTL"
#define device_bar_BDK_RVU_VF_MSIX_VECX_CTL(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_VF_MSIX_VECX_CTL(a) (a)
#define arguments_BDK_RVU_VF_MSIX_VECX_CTL(a) (a),-1,-1,-1

/**
 * Register (RVU_VF_BAR2) rvu_vf_vfpf_mbox#
 *
 * RVU VF/PF Mailbox Registers
 */
union bdk_rvu_vf_vfpf_mboxx
{
    uint64_t u;
    struct bdk_rvu_vf_vfpf_mboxx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W/H) Mailbox data. These VF registers access the 16-byte-per-VF VF/PF mailbox
                                                                 RAM. The PF may access the same storage using RVU_PF_VF()_PFVF_MBOX().
                                                                 MBOX(0) is typically used for PF to VF signaling, MBOX(1) for VF to PF.
                                                                 Writing RVU_VF_VFPF_MBOX(1) (but not RVU_PF_VF()_PFVF_MBOX(1))
                                                                 will set the corresponding RVU_PF_VFPF_MBOX_INT() bit, which if appropriately
                                                                 enabled will send an interrupt to the PF. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W/H) Mailbox data. These VF registers access the 16-byte-per-VF VF/PF mailbox
                                                                 RAM. The PF may access the same storage using RVU_PF_VF()_PFVF_MBOX().
                                                                 MBOX(0) is typically used for PF to VF signaling, MBOX(1) for VF to PF.
                                                                 Writing RVU_VF_VFPF_MBOX(1) (but not RVU_PF_VF()_PFVF_MBOX(1))
                                                                 will set the corresponding RVU_PF_VFPF_MBOX_INT() bit, which if appropriately
                                                                 enabled will send an interrupt to the PF. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rvu_vf_vfpf_mboxx_s cn; */
};
typedef union bdk_rvu_vf_vfpf_mboxx bdk_rvu_vf_vfpf_mboxx_t;

static inline uint64_t BDK_RVU_VF_VFPF_MBOXX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RVU_VF_VFPF_MBOXX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x850200000000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("RVU_VF_VFPF_MBOXX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RVU_VF_VFPF_MBOXX(a) bdk_rvu_vf_vfpf_mboxx_t
#define bustype_BDK_RVU_VF_VFPF_MBOXX(a) BDK_CSR_TYPE_RVU_VF_BAR2
#define basename_BDK_RVU_VF_VFPF_MBOXX(a) "RVU_VF_VFPF_MBOXX"
#define device_bar_BDK_RVU_VF_VFPF_MBOXX(a) 0x2 /* BAR2 */
#define busnum_BDK_RVU_VF_VFPF_MBOXX(a) (a)
#define arguments_BDK_RVU_VF_VFPF_MBOXX(a) (a),-1,-1,-1

#endif /* __BDK_CSRS_RVU_H__ */
