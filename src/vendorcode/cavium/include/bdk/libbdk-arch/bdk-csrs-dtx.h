#ifndef __BDK_CSRS_DTX_H__
#define __BDK_CSRS_DTX_H__
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
 * Cavium DTX.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Register (RSL) dtx_bch_bcst_rsp
 *
 * DTX BCH Control Register
 */
union bdk_dtx_bch_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_bch_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_bch_bcst_rsp_s cn; */
};
typedef union bdk_dtx_bch_bcst_rsp bdk_dtx_bch_bcst_rsp_t;

#define BDK_DTX_BCH_BCST_RSP BDK_DTX_BCH_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_BCH_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BCH_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 0x87e0feb88080ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0feb88080ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0feb88080ll;
    __bdk_csr_fatal("DTX_BCH_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_BCH_BCST_RSP bdk_dtx_bch_bcst_rsp_t
#define bustype_BDK_DTX_BCH_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BCH_BCST_RSP "DTX_BCH_BCST_RSP"
#define busnum_BDK_DTX_BCH_BCST_RSP 0
#define arguments_BDK_DTX_BCH_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_bch_ctl
 *
 * DTX BCH Control Register
 */
union bdk_dtx_bch_ctl
{
    uint64_t u;
    struct bdk_dtx_bch_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_bch_ctl_s cn; */
};
typedef union bdk_dtx_bch_ctl bdk_dtx_bch_ctl_t;

#define BDK_DTX_BCH_CTL BDK_DTX_BCH_CTL_FUNC()
static inline uint64_t BDK_DTX_BCH_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BCH_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 0x87e0feb88060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0feb88060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0feb88060ll;
    __bdk_csr_fatal("DTX_BCH_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_BCH_CTL bdk_dtx_bch_ctl_t
#define bustype_BDK_DTX_BCH_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BCH_CTL "DTX_BCH_CTL"
#define busnum_BDK_DTX_BCH_CTL 0
#define arguments_BDK_DTX_BCH_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_bch_dat#
 *
 * DTX BCH Raw Data Register
 */
union bdk_dtx_bch_datx
{
    uint64_t u;
    struct bdk_dtx_bch_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_bch_datx_s cn; */
};
typedef union bdk_dtx_bch_datx bdk_dtx_bch_datx_t;

static inline uint64_t BDK_DTX_BCH_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BCH_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0feb88040ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb88040ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb88040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_BCH_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_BCH_DATX(a) bdk_dtx_bch_datx_t
#define bustype_BDK_DTX_BCH_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BCH_DATX(a) "DTX_BCH_DATX"
#define busnum_BDK_DTX_BCH_DATX(a) (a)
#define arguments_BDK_DTX_BCH_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_bch_ena#
 *
 * DTX BCH Data Enable Register
 */
union bdk_dtx_bch_enax
{
    uint64_t u;
    struct bdk_dtx_bch_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_bch_enax_s cn; */
};
typedef union bdk_dtx_bch_enax bdk_dtx_bch_enax_t;

static inline uint64_t BDK_DTX_BCH_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BCH_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0feb88020ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb88020ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb88020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_BCH_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_BCH_ENAX(a) bdk_dtx_bch_enax_t
#define bustype_BDK_DTX_BCH_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BCH_ENAX(a) "DTX_BCH_ENAX"
#define busnum_BDK_DTX_BCH_ENAX(a) (a)
#define arguments_BDK_DTX_BCH_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_bch_sel#
 *
 * DTX BCH Select Register
 */
union bdk_dtx_bch_selx
{
    uint64_t u;
    struct bdk_dtx_bch_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_bch_selx_s cn; */
};
typedef union bdk_dtx_bch_selx bdk_dtx_bch_selx_t;

static inline uint64_t BDK_DTX_BCH_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BCH_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0feb88000ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb88000ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb88000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_BCH_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_BCH_SELX(a) bdk_dtx_bch_selx_t
#define bustype_BDK_DTX_BCH_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BCH_SELX(a) "DTX_BCH_SELX"
#define busnum_BDK_DTX_BCH_SELX(a) (a)
#define arguments_BDK_DTX_BCH_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_bgx#_bcst_rsp
 *
 * DTX BGX Control Register
 */
union bdk_dtx_bgxx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_bgxx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_bgxx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_bgxx_bcst_rsp bdk_dtx_bgxx_bcst_rsp_t;

static inline uint64_t BDK_DTX_BGXX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BGXX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0fe700080ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e0fe700080ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe700080ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_BGXX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_BGXX_BCST_RSP(a) bdk_dtx_bgxx_bcst_rsp_t
#define bustype_BDK_DTX_BGXX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BGXX_BCST_RSP(a) "DTX_BGXX_BCST_RSP"
#define busnum_BDK_DTX_BGXX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_BGXX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_bgx#_ctl
 *
 * DTX BGX Control Register
 */
union bdk_dtx_bgxx_ctl
{
    uint64_t u;
    struct bdk_dtx_bgxx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_bgxx_ctl_s cn; */
};
typedef union bdk_dtx_bgxx_ctl bdk_dtx_bgxx_ctl_t;

static inline uint64_t BDK_DTX_BGXX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BGXX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0fe700060ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e0fe700060ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe700060ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_BGXX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_BGXX_CTL(a) bdk_dtx_bgxx_ctl_t
#define bustype_BDK_DTX_BGXX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BGXX_CTL(a) "DTX_BGXX_CTL"
#define busnum_BDK_DTX_BGXX_CTL(a) (a)
#define arguments_BDK_DTX_BGXX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_bgx#_dat#
 *
 * DTX BGX Raw Data Register
 */
union bdk_dtx_bgxx_datx
{
    uint64_t u;
    struct bdk_dtx_bgxx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_bgxx_datx_s cn; */
};
typedef union bdk_dtx_bgxx_datx bdk_dtx_bgxx_datx_t;

static inline uint64_t BDK_DTX_BGXX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BGXX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe700040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe700040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe700040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_BGXX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_BGXX_DATX(a,b) bdk_dtx_bgxx_datx_t
#define bustype_BDK_DTX_BGXX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BGXX_DATX(a,b) "DTX_BGXX_DATX"
#define busnum_BDK_DTX_BGXX_DATX(a,b) (a)
#define arguments_BDK_DTX_BGXX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_bgx#_ena#
 *
 * DTX BGX Data Enable Register
 */
union bdk_dtx_bgxx_enax
{
    uint64_t u;
    struct bdk_dtx_bgxx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_bgxx_enax_s cn; */
};
typedef union bdk_dtx_bgxx_enax bdk_dtx_bgxx_enax_t;

static inline uint64_t BDK_DTX_BGXX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BGXX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe700020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe700020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe700020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_BGXX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_BGXX_ENAX(a,b) bdk_dtx_bgxx_enax_t
#define bustype_BDK_DTX_BGXX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BGXX_ENAX(a,b) "DTX_BGXX_ENAX"
#define busnum_BDK_DTX_BGXX_ENAX(a,b) (a)
#define arguments_BDK_DTX_BGXX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_bgx#_sel#
 *
 * DTX BGX Select Register
 */
union bdk_dtx_bgxx_selx
{
    uint64_t u;
    struct bdk_dtx_bgxx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_bgxx_selx_s cn; */
};
typedef union bdk_dtx_bgxx_selx bdk_dtx_bgxx_selx_t;

static inline uint64_t BDK_DTX_BGXX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BGXX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe700000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe700000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe700000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_BGXX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_BGXX_SELX(a,b) bdk_dtx_bgxx_selx_t
#define bustype_BDK_DTX_BGXX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BGXX_SELX(a,b) "DTX_BGXX_SELX"
#define busnum_BDK_DTX_BGXX_SELX(a,b) (a)
#define arguments_BDK_DTX_BGXX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_broadcast_ctl
 *
 * DTX BROADCAST Control Register
 */
union bdk_dtx_broadcast_ctl
{
    uint64_t u;
    struct bdk_dtx_broadcast_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_broadcast_ctl_s cn; */
};
typedef union bdk_dtx_broadcast_ctl bdk_dtx_broadcast_ctl_t;

#define BDK_DTX_BROADCAST_CTL BDK_DTX_BROADCAST_CTL_FUNC()
static inline uint64_t BDK_DTX_BROADCAST_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BROADCAST_CTL_FUNC(void)
{
    return 0x87e0fe7f0060ll;
}

#define typedef_BDK_DTX_BROADCAST_CTL bdk_dtx_broadcast_ctl_t
#define bustype_BDK_DTX_BROADCAST_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BROADCAST_CTL "DTX_BROADCAST_CTL"
#define busnum_BDK_DTX_BROADCAST_CTL 0
#define arguments_BDK_DTX_BROADCAST_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_broadcast_ena#
 *
 * DTX BROADCAST Data Enable Register
 */
union bdk_dtx_broadcast_enax
{
    uint64_t u;
    struct bdk_dtx_broadcast_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_broadcast_enax_s cn; */
};
typedef union bdk_dtx_broadcast_enax bdk_dtx_broadcast_enax_t;

static inline uint64_t BDK_DTX_BROADCAST_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BROADCAST_ENAX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe7f0020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_BROADCAST_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_BROADCAST_ENAX(a) bdk_dtx_broadcast_enax_t
#define bustype_BDK_DTX_BROADCAST_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BROADCAST_ENAX(a) "DTX_BROADCAST_ENAX"
#define busnum_BDK_DTX_BROADCAST_ENAX(a) (a)
#define arguments_BDK_DTX_BROADCAST_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_broadcast_sel#
 *
 * DTX BROADCAST Select Register
 */
union bdk_dtx_broadcast_selx
{
    uint64_t u;
    struct bdk_dtx_broadcast_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_broadcast_selx_s cn; */
};
typedef union bdk_dtx_broadcast_selx bdk_dtx_broadcast_selx_t;

static inline uint64_t BDK_DTX_BROADCAST_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BROADCAST_SELX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe7f0000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_BROADCAST_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_BROADCAST_SELX(a) bdk_dtx_broadcast_selx_t
#define bustype_BDK_DTX_BROADCAST_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BROADCAST_SELX(a) "DTX_BROADCAST_SELX"
#define busnum_BDK_DTX_BROADCAST_SELX(a) (a)
#define arguments_BDK_DTX_BROADCAST_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_bts_bcst_rsp
 *
 * DTX BTS Control Register
 */
union bdk_dtx_bts_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_bts_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_bts_bcst_rsp_s cn; */
};
typedef union bdk_dtx_bts_bcst_rsp bdk_dtx_bts_bcst_rsp_t;

#define BDK_DTX_BTS_BCST_RSP BDK_DTX_BTS_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_BTS_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BTS_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0fe090080ll;
    __bdk_csr_fatal("DTX_BTS_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_BTS_BCST_RSP bdk_dtx_bts_bcst_rsp_t
#define bustype_BDK_DTX_BTS_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BTS_BCST_RSP "DTX_BTS_BCST_RSP"
#define busnum_BDK_DTX_BTS_BCST_RSP 0
#define arguments_BDK_DTX_BTS_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_bts_ctl
 *
 * DTX BTS Control Register
 */
union bdk_dtx_bts_ctl
{
    uint64_t u;
    struct bdk_dtx_bts_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_bts_ctl_s cn; */
};
typedef union bdk_dtx_bts_ctl bdk_dtx_bts_ctl_t;

#define BDK_DTX_BTS_CTL BDK_DTX_BTS_CTL_FUNC()
static inline uint64_t BDK_DTX_BTS_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BTS_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0fe090060ll;
    __bdk_csr_fatal("DTX_BTS_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_BTS_CTL bdk_dtx_bts_ctl_t
#define bustype_BDK_DTX_BTS_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BTS_CTL "DTX_BTS_CTL"
#define busnum_BDK_DTX_BTS_CTL 0
#define arguments_BDK_DTX_BTS_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_bts_dat#
 *
 * DTX BTS Raw Data Register
 */
union bdk_dtx_bts_datx
{
    uint64_t u;
    struct bdk_dtx_bts_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_bts_datx_s cn; */
};
typedef union bdk_dtx_bts_datx bdk_dtx_bts_datx_t;

static inline uint64_t BDK_DTX_BTS_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BTS_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe090040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_BTS_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_BTS_DATX(a) bdk_dtx_bts_datx_t
#define bustype_BDK_DTX_BTS_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BTS_DATX(a) "DTX_BTS_DATX"
#define busnum_BDK_DTX_BTS_DATX(a) (a)
#define arguments_BDK_DTX_BTS_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_bts_ena#
 *
 * DTX BTS Data Enable Register
 */
union bdk_dtx_bts_enax
{
    uint64_t u;
    struct bdk_dtx_bts_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_bts_enax_s cn; */
};
typedef union bdk_dtx_bts_enax bdk_dtx_bts_enax_t;

static inline uint64_t BDK_DTX_BTS_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BTS_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe090020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_BTS_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_BTS_ENAX(a) bdk_dtx_bts_enax_t
#define bustype_BDK_DTX_BTS_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BTS_ENAX(a) "DTX_BTS_ENAX"
#define busnum_BDK_DTX_BTS_ENAX(a) (a)
#define arguments_BDK_DTX_BTS_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_bts_sel#
 *
 * DTX BTS Select Register
 */
union bdk_dtx_bts_selx
{
    uint64_t u;
    struct bdk_dtx_bts_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_bts_selx_s cn; */
};
typedef union bdk_dtx_bts_selx bdk_dtx_bts_selx_t;

static inline uint64_t BDK_DTX_BTS_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_BTS_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe090000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_BTS_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_BTS_SELX(a) bdk_dtx_bts_selx_t
#define bustype_BDK_DTX_BTS_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_BTS_SELX(a) "DTX_BTS_SELX"
#define busnum_BDK_DTX_BTS_SELX(a) (a)
#define arguments_BDK_DTX_BTS_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ccu#_msw_bcst_rsp
 *
 * DTX CCU_MSW Control Register
 */
union bdk_dtx_ccux_msw_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_ccux_msw_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ccux_msw_bcst_rsp_s cn; */
};
typedef union bdk_dtx_ccux_msw_bcst_rsp bdk_dtx_ccux_msw_bcst_rsp_t;

static inline uint64_t BDK_DTX_CCUX_MSW_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CCUX_MSW_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x87e0fec80080ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_CCUX_MSW_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CCUX_MSW_BCST_RSP(a) bdk_dtx_ccux_msw_bcst_rsp_t
#define bustype_BDK_DTX_CCUX_MSW_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CCUX_MSW_BCST_RSP(a) "DTX_CCUX_MSW_BCST_RSP"
#define busnum_BDK_DTX_CCUX_MSW_BCST_RSP(a) (a)
#define arguments_BDK_DTX_CCUX_MSW_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ccu#_msw_ctl
 *
 * DTX CCU_MSW Control Register
 */
union bdk_dtx_ccux_msw_ctl
{
    uint64_t u;
    struct bdk_dtx_ccux_msw_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ccux_msw_ctl_s cn; */
};
typedef union bdk_dtx_ccux_msw_ctl bdk_dtx_ccux_msw_ctl_t;

static inline uint64_t BDK_DTX_CCUX_MSW_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CCUX_MSW_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x87e0fec80060ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_CCUX_MSW_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CCUX_MSW_CTL(a) bdk_dtx_ccux_msw_ctl_t
#define bustype_BDK_DTX_CCUX_MSW_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CCUX_MSW_CTL(a) "DTX_CCUX_MSW_CTL"
#define busnum_BDK_DTX_CCUX_MSW_CTL(a) (a)
#define arguments_BDK_DTX_CCUX_MSW_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ccu#_msw_dat#
 *
 * DTX CCU_MSW Raw Data Register
 */
union bdk_dtx_ccux_msw_datx
{
    uint64_t u;
    struct bdk_dtx_ccux_msw_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ccux_msw_datx_s cn; */
};
typedef union bdk_dtx_ccux_msw_datx bdk_dtx_ccux_msw_datx_t;

static inline uint64_t BDK_DTX_CCUX_MSW_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CCUX_MSW_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=3) && (b<=1)))
        return 0x87e0fec80040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_CCUX_MSW_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_CCUX_MSW_DATX(a,b) bdk_dtx_ccux_msw_datx_t
#define bustype_BDK_DTX_CCUX_MSW_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CCUX_MSW_DATX(a,b) "DTX_CCUX_MSW_DATX"
#define busnum_BDK_DTX_CCUX_MSW_DATX(a,b) (a)
#define arguments_BDK_DTX_CCUX_MSW_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_ccu#_msw_ena#
 *
 * DTX CCU_MSW Data Enable Register
 */
union bdk_dtx_ccux_msw_enax
{
    uint64_t u;
    struct bdk_dtx_ccux_msw_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ccux_msw_enax_s cn; */
};
typedef union bdk_dtx_ccux_msw_enax bdk_dtx_ccux_msw_enax_t;

static inline uint64_t BDK_DTX_CCUX_MSW_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CCUX_MSW_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=3) && (b<=1)))
        return 0x87e0fec80020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_CCUX_MSW_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_CCUX_MSW_ENAX(a,b) bdk_dtx_ccux_msw_enax_t
#define bustype_BDK_DTX_CCUX_MSW_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CCUX_MSW_ENAX(a,b) "DTX_CCUX_MSW_ENAX"
#define busnum_BDK_DTX_CCUX_MSW_ENAX(a,b) (a)
#define arguments_BDK_DTX_CCUX_MSW_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_ccu#_msw_sel#
 *
 * DTX CCU_MSW Select Register
 */
union bdk_dtx_ccux_msw_selx
{
    uint64_t u;
    struct bdk_dtx_ccux_msw_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ccux_msw_selx_s cn; */
};
typedef union bdk_dtx_ccux_msw_selx bdk_dtx_ccux_msw_selx_t;

static inline uint64_t BDK_DTX_CCUX_MSW_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CCUX_MSW_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=3) && (b<=1)))
        return 0x87e0fec80000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_CCUX_MSW_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_CCUX_MSW_SELX(a,b) bdk_dtx_ccux_msw_selx_t
#define bustype_BDK_DTX_CCUX_MSW_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CCUX_MSW_SELX(a,b) "DTX_CCUX_MSW_SELX"
#define busnum_BDK_DTX_CCUX_MSW_SELX(a,b) (a)
#define arguments_BDK_DTX_CCUX_MSW_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_ccu#_pic_bcst_rsp
 *
 * DTX CCU_PIC Control Register
 */
union bdk_dtx_ccux_pic_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_ccux_pic_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ccux_pic_bcst_rsp_s cn; */
};
typedef union bdk_dtx_ccux_pic_bcst_rsp bdk_dtx_ccux_pic_bcst_rsp_t;

static inline uint64_t BDK_DTX_CCUX_PIC_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CCUX_PIC_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x87e0fe280080ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_CCUX_PIC_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CCUX_PIC_BCST_RSP(a) bdk_dtx_ccux_pic_bcst_rsp_t
#define bustype_BDK_DTX_CCUX_PIC_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CCUX_PIC_BCST_RSP(a) "DTX_CCUX_PIC_BCST_RSP"
#define busnum_BDK_DTX_CCUX_PIC_BCST_RSP(a) (a)
#define arguments_BDK_DTX_CCUX_PIC_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ccu#_pic_ctl
 *
 * DTX CCU_PIC Control Register
 */
union bdk_dtx_ccux_pic_ctl
{
    uint64_t u;
    struct bdk_dtx_ccux_pic_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ccux_pic_ctl_s cn; */
};
typedef union bdk_dtx_ccux_pic_ctl bdk_dtx_ccux_pic_ctl_t;

static inline uint64_t BDK_DTX_CCUX_PIC_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CCUX_PIC_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x87e0fe280060ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_CCUX_PIC_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CCUX_PIC_CTL(a) bdk_dtx_ccux_pic_ctl_t
#define bustype_BDK_DTX_CCUX_PIC_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CCUX_PIC_CTL(a) "DTX_CCUX_PIC_CTL"
#define busnum_BDK_DTX_CCUX_PIC_CTL(a) (a)
#define arguments_BDK_DTX_CCUX_PIC_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ccu#_pic_dat#
 *
 * DTX CCU_PIC Raw Data Register
 */
union bdk_dtx_ccux_pic_datx
{
    uint64_t u;
    struct bdk_dtx_ccux_pic_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ccux_pic_datx_s cn; */
};
typedef union bdk_dtx_ccux_pic_datx bdk_dtx_ccux_pic_datx_t;

static inline uint64_t BDK_DTX_CCUX_PIC_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CCUX_PIC_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=3) && (b<=1)))
        return 0x87e0fe280040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_CCUX_PIC_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_CCUX_PIC_DATX(a,b) bdk_dtx_ccux_pic_datx_t
#define bustype_BDK_DTX_CCUX_PIC_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CCUX_PIC_DATX(a,b) "DTX_CCUX_PIC_DATX"
#define busnum_BDK_DTX_CCUX_PIC_DATX(a,b) (a)
#define arguments_BDK_DTX_CCUX_PIC_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_ccu#_pic_ena#
 *
 * DTX CCU_PIC Data Enable Register
 */
union bdk_dtx_ccux_pic_enax
{
    uint64_t u;
    struct bdk_dtx_ccux_pic_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ccux_pic_enax_s cn; */
};
typedef union bdk_dtx_ccux_pic_enax bdk_dtx_ccux_pic_enax_t;

static inline uint64_t BDK_DTX_CCUX_PIC_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CCUX_PIC_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=3) && (b<=1)))
        return 0x87e0fe280020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_CCUX_PIC_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_CCUX_PIC_ENAX(a,b) bdk_dtx_ccux_pic_enax_t
#define bustype_BDK_DTX_CCUX_PIC_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CCUX_PIC_ENAX(a,b) "DTX_CCUX_PIC_ENAX"
#define busnum_BDK_DTX_CCUX_PIC_ENAX(a,b) (a)
#define arguments_BDK_DTX_CCUX_PIC_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_ccu#_pic_sel#
 *
 * DTX CCU_PIC Select Register
 */
union bdk_dtx_ccux_pic_selx
{
    uint64_t u;
    struct bdk_dtx_ccux_pic_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ccux_pic_selx_s cn; */
};
typedef union bdk_dtx_ccux_pic_selx bdk_dtx_ccux_pic_selx_t;

static inline uint64_t BDK_DTX_CCUX_PIC_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CCUX_PIC_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=3) && (b<=1)))
        return 0x87e0fe280000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_CCUX_PIC_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_CCUX_PIC_SELX(a,b) bdk_dtx_ccux_pic_selx_t
#define bustype_BDK_DTX_CCUX_PIC_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CCUX_PIC_SELX(a,b) "DTX_CCUX_PIC_SELX"
#define busnum_BDK_DTX_CCUX_PIC_SELX(a,b) (a)
#define arguments_BDK_DTX_CCUX_PIC_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_ccu#_tad#_bcst_rsp
 *
 * DTX CCU_TAD Control Register
 */
union bdk_dtx_ccux_tadx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_ccux_tadx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ccux_tadx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_ccux_tadx_bcst_rsp bdk_dtx_ccux_tadx_bcst_rsp_t;

static inline uint64_t BDK_DTX_CCUX_TADX_BCST_RSP(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CCUX_TADX_BCST_RSP(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=3) && (b<=1)))
        return 0x87e0fed00080ll + 0x10000ll * ((a) & 0x3) + 0x8000ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_CCUX_TADX_BCST_RSP", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_CCUX_TADX_BCST_RSP(a,b) bdk_dtx_ccux_tadx_bcst_rsp_t
#define bustype_BDK_DTX_CCUX_TADX_BCST_RSP(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CCUX_TADX_BCST_RSP(a,b) "DTX_CCUX_TADX_BCST_RSP"
#define busnum_BDK_DTX_CCUX_TADX_BCST_RSP(a,b) (a)
#define arguments_BDK_DTX_CCUX_TADX_BCST_RSP(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_ccu#_tad#_ctl
 *
 * DTX CCU_TAD Control Register
 */
union bdk_dtx_ccux_tadx_ctl
{
    uint64_t u;
    struct bdk_dtx_ccux_tadx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ccux_tadx_ctl_s cn; */
};
typedef union bdk_dtx_ccux_tadx_ctl bdk_dtx_ccux_tadx_ctl_t;

static inline uint64_t BDK_DTX_CCUX_TADX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CCUX_TADX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=3) && (b<=1)))
        return 0x87e0fed00060ll + 0x10000ll * ((a) & 0x3) + 0x8000ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_CCUX_TADX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_CCUX_TADX_CTL(a,b) bdk_dtx_ccux_tadx_ctl_t
#define bustype_BDK_DTX_CCUX_TADX_CTL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CCUX_TADX_CTL(a,b) "DTX_CCUX_TADX_CTL"
#define busnum_BDK_DTX_CCUX_TADX_CTL(a,b) (a)
#define arguments_BDK_DTX_CCUX_TADX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_ccu#_tad#_dat#
 *
 * DTX CCU_TAD Raw Data Register
 */
union bdk_dtx_ccux_tadx_datx
{
    uint64_t u;
    struct bdk_dtx_ccux_tadx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ccux_tadx_datx_s cn; */
};
typedef union bdk_dtx_ccux_tadx_datx bdk_dtx_ccux_tadx_datx_t;

static inline uint64_t BDK_DTX_CCUX_TADX_DATX(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CCUX_TADX_DATX(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=3) && (b<=1) && (c<=1)))
        return 0x87e0fed00040ll + 0x10000ll * ((a) & 0x3) + 0x8000ll * ((b) & 0x1) + 8ll * ((c) & 0x1);
    __bdk_csr_fatal("DTX_CCUX_TADX_DATX", 3, a, b, c, 0);
}

#define typedef_BDK_DTX_CCUX_TADX_DATX(a,b,c) bdk_dtx_ccux_tadx_datx_t
#define bustype_BDK_DTX_CCUX_TADX_DATX(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CCUX_TADX_DATX(a,b,c) "DTX_CCUX_TADX_DATX"
#define busnum_BDK_DTX_CCUX_TADX_DATX(a,b,c) (a)
#define arguments_BDK_DTX_CCUX_TADX_DATX(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) dtx_ccu#_tad#_ena#
 *
 * DTX CCU_TAD Data Enable Register
 */
union bdk_dtx_ccux_tadx_enax
{
    uint64_t u;
    struct bdk_dtx_ccux_tadx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ccux_tadx_enax_s cn; */
};
typedef union bdk_dtx_ccux_tadx_enax bdk_dtx_ccux_tadx_enax_t;

static inline uint64_t BDK_DTX_CCUX_TADX_ENAX(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CCUX_TADX_ENAX(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=3) && (b<=1) && (c<=1)))
        return 0x87e0fed00020ll + 0x10000ll * ((a) & 0x3) + 0x8000ll * ((b) & 0x1) + 8ll * ((c) & 0x1);
    __bdk_csr_fatal("DTX_CCUX_TADX_ENAX", 3, a, b, c, 0);
}

#define typedef_BDK_DTX_CCUX_TADX_ENAX(a,b,c) bdk_dtx_ccux_tadx_enax_t
#define bustype_BDK_DTX_CCUX_TADX_ENAX(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CCUX_TADX_ENAX(a,b,c) "DTX_CCUX_TADX_ENAX"
#define busnum_BDK_DTX_CCUX_TADX_ENAX(a,b,c) (a)
#define arguments_BDK_DTX_CCUX_TADX_ENAX(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) dtx_ccu#_tad#_sel#
 *
 * DTX CCU_TAD Select Register
 */
union bdk_dtx_ccux_tadx_selx
{
    uint64_t u;
    struct bdk_dtx_ccux_tadx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ccux_tadx_selx_s cn; */
};
typedef union bdk_dtx_ccux_tadx_selx bdk_dtx_ccux_tadx_selx_t;

static inline uint64_t BDK_DTX_CCUX_TADX_SELX(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CCUX_TADX_SELX(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=3) && (b<=1) && (c<=1)))
        return 0x87e0fed00000ll + 0x10000ll * ((a) & 0x3) + 0x8000ll * ((b) & 0x1) + 8ll * ((c) & 0x1);
    __bdk_csr_fatal("DTX_CCUX_TADX_SELX", 3, a, b, c, 0);
}

#define typedef_BDK_DTX_CCUX_TADX_SELX(a,b,c) bdk_dtx_ccux_tadx_selx_t
#define bustype_BDK_DTX_CCUX_TADX_SELX(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CCUX_TADX_SELX(a,b,c) "DTX_CCUX_TADX_SELX"
#define busnum_BDK_DTX_CCUX_TADX_SELX(a,b,c) (a)
#define arguments_BDK_DTX_CCUX_TADX_SELX(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) dtx_cde_bcst_rsp
 *
 * INTERNAL: DTX CDE Control Register
 */
union bdk_dtx_cde_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_cde_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cde_bcst_rsp_s cn; */
};
typedef union bdk_dtx_cde_bcst_rsp bdk_dtx_cde_bcst_rsp_t;

#define BDK_DTX_CDE_BCST_RSP BDK_DTX_CDE_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_CDE_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CDE_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 0x87e0fe7f8080ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0fe7f8080ll;
    __bdk_csr_fatal("DTX_CDE_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_CDE_BCST_RSP bdk_dtx_cde_bcst_rsp_t
#define bustype_BDK_DTX_CDE_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CDE_BCST_RSP "DTX_CDE_BCST_RSP"
#define busnum_BDK_DTX_CDE_BCST_RSP 0
#define arguments_BDK_DTX_CDE_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_cde_ctl
 *
 * INTERNAL: DTX CDE Control Register
 */
union bdk_dtx_cde_ctl
{
    uint64_t u;
    struct bdk_dtx_cde_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cde_ctl_s cn; */
};
typedef union bdk_dtx_cde_ctl bdk_dtx_cde_ctl_t;

#define BDK_DTX_CDE_CTL BDK_DTX_CDE_CTL_FUNC()
static inline uint64_t BDK_DTX_CDE_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CDE_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 0x87e0fe7f8060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0fe7f8060ll;
    __bdk_csr_fatal("DTX_CDE_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_CDE_CTL bdk_dtx_cde_ctl_t
#define bustype_BDK_DTX_CDE_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CDE_CTL "DTX_CDE_CTL"
#define busnum_BDK_DTX_CDE_CTL 0
#define arguments_BDK_DTX_CDE_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_cde_dat#
 *
 * INTERNAL: DTX CDE Raw Data Register
 */
union bdk_dtx_cde_datx
{
    uint64_t u;
    struct bdk_dtx_cde_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cde_datx_s cn; */
};
typedef union bdk_dtx_cde_datx bdk_dtx_cde_datx_t;

static inline uint64_t BDK_DTX_CDE_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CDE_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0fe7f8040ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe7f8040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_CDE_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CDE_DATX(a) bdk_dtx_cde_datx_t
#define bustype_BDK_DTX_CDE_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CDE_DATX(a) "DTX_CDE_DATX"
#define busnum_BDK_DTX_CDE_DATX(a) (a)
#define arguments_BDK_DTX_CDE_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_cde_ena#
 *
 * INTERNAL: DTX CDE Data Enable Register
 */
union bdk_dtx_cde_enax
{
    uint64_t u;
    struct bdk_dtx_cde_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cde_enax_s cn; */
};
typedef union bdk_dtx_cde_enax bdk_dtx_cde_enax_t;

static inline uint64_t BDK_DTX_CDE_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CDE_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0fe7f8020ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe7f8020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_CDE_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CDE_ENAX(a) bdk_dtx_cde_enax_t
#define bustype_BDK_DTX_CDE_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CDE_ENAX(a) "DTX_CDE_ENAX"
#define busnum_BDK_DTX_CDE_ENAX(a) (a)
#define arguments_BDK_DTX_CDE_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_cde_sel#
 *
 * INTERNAL: DTX CDE Select Register
 */
union bdk_dtx_cde_selx
{
    uint64_t u;
    struct bdk_dtx_cde_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cde_selx_s cn; */
};
typedef union bdk_dtx_cde_selx bdk_dtx_cde_selx_t;

static inline uint64_t BDK_DTX_CDE_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CDE_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0fe7f8000ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe7f8000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_CDE_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CDE_SELX(a) bdk_dtx_cde_selx_t
#define bustype_BDK_DTX_CDE_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CDE_SELX(a) "DTX_CDE_SELX"
#define busnum_BDK_DTX_CDE_SELX(a) (a)
#define arguments_BDK_DTX_CDE_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_cgx#_bcst_rsp
 *
 * DTX CGX Control Register
 */
union bdk_dtx_cgxx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_cgxx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cgxx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_cgxx_bcst_rsp bdk_dtx_cgxx_bcst_rsp_t;

static inline uint64_t BDK_DTX_CGXX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CGXX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=2))
        return 0x87e0fe700080ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_CGXX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CGXX_BCST_RSP(a) bdk_dtx_cgxx_bcst_rsp_t
#define bustype_BDK_DTX_CGXX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CGXX_BCST_RSP(a) "DTX_CGXX_BCST_RSP"
#define busnum_BDK_DTX_CGXX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_CGXX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_cgx#_ctl
 *
 * DTX CGX Control Register
 */
union bdk_dtx_cgxx_ctl
{
    uint64_t u;
    struct bdk_dtx_cgxx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cgxx_ctl_s cn; */
};
typedef union bdk_dtx_cgxx_ctl bdk_dtx_cgxx_ctl_t;

static inline uint64_t BDK_DTX_CGXX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CGXX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=2))
        return 0x87e0fe700060ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_CGXX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CGXX_CTL(a) bdk_dtx_cgxx_ctl_t
#define bustype_BDK_DTX_CGXX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CGXX_CTL(a) "DTX_CGXX_CTL"
#define busnum_BDK_DTX_CGXX_CTL(a) (a)
#define arguments_BDK_DTX_CGXX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_cgx#_dat#
 *
 * DTX CGX Raw Data Register
 */
union bdk_dtx_cgxx_datx
{
    uint64_t u;
    struct bdk_dtx_cgxx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cgxx_datx_s cn; */
};
typedef union bdk_dtx_cgxx_datx bdk_dtx_cgxx_datx_t;

static inline uint64_t BDK_DTX_CGXX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CGXX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=2) && (b<=1)))
        return 0x87e0fe700040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_CGXX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_CGXX_DATX(a,b) bdk_dtx_cgxx_datx_t
#define bustype_BDK_DTX_CGXX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CGXX_DATX(a,b) "DTX_CGXX_DATX"
#define busnum_BDK_DTX_CGXX_DATX(a,b) (a)
#define arguments_BDK_DTX_CGXX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_cgx#_ena#
 *
 * DTX CGX Data Enable Register
 */
union bdk_dtx_cgxx_enax
{
    uint64_t u;
    struct bdk_dtx_cgxx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cgxx_enax_s cn; */
};
typedef union bdk_dtx_cgxx_enax bdk_dtx_cgxx_enax_t;

static inline uint64_t BDK_DTX_CGXX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CGXX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=2) && (b<=1)))
        return 0x87e0fe700020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_CGXX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_CGXX_ENAX(a,b) bdk_dtx_cgxx_enax_t
#define bustype_BDK_DTX_CGXX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CGXX_ENAX(a,b) "DTX_CGXX_ENAX"
#define busnum_BDK_DTX_CGXX_ENAX(a,b) (a)
#define arguments_BDK_DTX_CGXX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_cgx#_sel#
 *
 * DTX CGX Select Register
 */
union bdk_dtx_cgxx_selx
{
    uint64_t u;
    struct bdk_dtx_cgxx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cgxx_selx_s cn; */
};
typedef union bdk_dtx_cgxx_selx bdk_dtx_cgxx_selx_t;

static inline uint64_t BDK_DTX_CGXX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CGXX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=2) && (b<=1)))
        return 0x87e0fe700000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_CGXX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_CGXX_SELX(a,b) bdk_dtx_cgxx_selx_t
#define bustype_BDK_DTX_CGXX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CGXX_SELX(a,b) "DTX_CGXX_SELX"
#define busnum_BDK_DTX_CGXX_SELX(a,b) (a)
#define arguments_BDK_DTX_CGXX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_cpc_bcst_rsp
 *
 * DTX CPC Control Register
 */
union bdk_dtx_cpc_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_cpc_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cpc_bcst_rsp_s cn; */
};
typedef union bdk_dtx_cpc_bcst_rsp bdk_dtx_cpc_bcst_rsp_t;

#define BDK_DTX_CPC_BCST_RSP BDK_DTX_CPC_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_CPC_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPC_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0feb68080ll;
    __bdk_csr_fatal("DTX_CPC_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_CPC_BCST_RSP bdk_dtx_cpc_bcst_rsp_t
#define bustype_BDK_DTX_CPC_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPC_BCST_RSP "DTX_CPC_BCST_RSP"
#define busnum_BDK_DTX_CPC_BCST_RSP 0
#define arguments_BDK_DTX_CPC_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_cpc_ctl
 *
 * DTX CPC Control Register
 */
union bdk_dtx_cpc_ctl
{
    uint64_t u;
    struct bdk_dtx_cpc_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cpc_ctl_s cn; */
};
typedef union bdk_dtx_cpc_ctl bdk_dtx_cpc_ctl_t;

#define BDK_DTX_CPC_CTL BDK_DTX_CPC_CTL_FUNC()
static inline uint64_t BDK_DTX_CPC_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPC_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0feb68060ll;
    __bdk_csr_fatal("DTX_CPC_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_CPC_CTL bdk_dtx_cpc_ctl_t
#define bustype_BDK_DTX_CPC_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPC_CTL "DTX_CPC_CTL"
#define busnum_BDK_DTX_CPC_CTL 0
#define arguments_BDK_DTX_CPC_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_cpc_dat#
 *
 * DTX CPC Raw Data Register
 */
union bdk_dtx_cpc_datx
{
    uint64_t u;
    struct bdk_dtx_cpc_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cpc_datx_s cn; */
};
typedef union bdk_dtx_cpc_datx bdk_dtx_cpc_datx_t;

static inline uint64_t BDK_DTX_CPC_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPC_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb68040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_CPC_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CPC_DATX(a) bdk_dtx_cpc_datx_t
#define bustype_BDK_DTX_CPC_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPC_DATX(a) "DTX_CPC_DATX"
#define busnum_BDK_DTX_CPC_DATX(a) (a)
#define arguments_BDK_DTX_CPC_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_cpc_ena#
 *
 * DTX CPC Data Enable Register
 */
union bdk_dtx_cpc_enax
{
    uint64_t u;
    struct bdk_dtx_cpc_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cpc_enax_s cn; */
};
typedef union bdk_dtx_cpc_enax bdk_dtx_cpc_enax_t;

static inline uint64_t BDK_DTX_CPC_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPC_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb68020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_CPC_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CPC_ENAX(a) bdk_dtx_cpc_enax_t
#define bustype_BDK_DTX_CPC_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPC_ENAX(a) "DTX_CPC_ENAX"
#define busnum_BDK_DTX_CPC_ENAX(a) (a)
#define arguments_BDK_DTX_CPC_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_cpc_sel#
 *
 * DTX CPC Select Register
 */
union bdk_dtx_cpc_selx
{
    uint64_t u;
    struct bdk_dtx_cpc_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cpc_selx_s cn; */
};
typedef union bdk_dtx_cpc_selx bdk_dtx_cpc_selx_t;

static inline uint64_t BDK_DTX_CPC_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPC_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb68000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_CPC_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CPC_SELX(a) bdk_dtx_cpc_selx_t
#define bustype_BDK_DTX_CPC_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPC_SELX(a) "DTX_CPC_SELX"
#define busnum_BDK_DTX_CPC_SELX(a) (a)
#define arguments_BDK_DTX_CPC_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_cpt#_bcst_rsp
 *
 * DTX CPT Control Register
 */
union bdk_dtx_cptx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_cptx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cptx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_cptx_bcst_rsp bdk_dtx_cptx_bcst_rsp_t;

static inline uint64_t BDK_DTX_CPTX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPTX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb90080ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_CPTX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CPTX_BCST_RSP(a) bdk_dtx_cptx_bcst_rsp_t
#define bustype_BDK_DTX_CPTX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPTX_BCST_RSP(a) "DTX_CPTX_BCST_RSP"
#define busnum_BDK_DTX_CPTX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_CPTX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_cpt#_ctl
 *
 * DTX CPT Control Register
 */
union bdk_dtx_cptx_ctl
{
    uint64_t u;
    struct bdk_dtx_cptx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cptx_ctl_s cn; */
};
typedef union bdk_dtx_cptx_ctl bdk_dtx_cptx_ctl_t;

static inline uint64_t BDK_DTX_CPTX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPTX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb90060ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_CPTX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CPTX_CTL(a) bdk_dtx_cptx_ctl_t
#define bustype_BDK_DTX_CPTX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPTX_CTL(a) "DTX_CPTX_CTL"
#define busnum_BDK_DTX_CPTX_CTL(a) (a)
#define arguments_BDK_DTX_CPTX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_cpt#_dat#
 *
 * DTX CPT Raw Data Register
 */
union bdk_dtx_cptx_datx
{
    uint64_t u;
    struct bdk_dtx_cptx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cptx_datx_s cn; */
};
typedef union bdk_dtx_cptx_datx bdk_dtx_cptx_datx_t;

static inline uint64_t BDK_DTX_CPTX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPTX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0feb90040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_CPTX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_CPTX_DATX(a,b) bdk_dtx_cptx_datx_t
#define bustype_BDK_DTX_CPTX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPTX_DATX(a,b) "DTX_CPTX_DATX"
#define busnum_BDK_DTX_CPTX_DATX(a,b) (a)
#define arguments_BDK_DTX_CPTX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_cpt#_ena#
 *
 * DTX CPT Data Enable Register
 */
union bdk_dtx_cptx_enax
{
    uint64_t u;
    struct bdk_dtx_cptx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cptx_enax_s cn; */
};
typedef union bdk_dtx_cptx_enax bdk_dtx_cptx_enax_t;

static inline uint64_t BDK_DTX_CPTX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPTX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0feb90020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_CPTX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_CPTX_ENAX(a,b) bdk_dtx_cptx_enax_t
#define bustype_BDK_DTX_CPTX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPTX_ENAX(a,b) "DTX_CPTX_ENAX"
#define busnum_BDK_DTX_CPTX_ENAX(a,b) (a)
#define arguments_BDK_DTX_CPTX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_cpt#_sel#
 *
 * DTX CPT Select Register
 */
union bdk_dtx_cptx_selx
{
    uint64_t u;
    struct bdk_dtx_cptx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cptx_selx_s cn; */
};
typedef union bdk_dtx_cptx_selx bdk_dtx_cptx_selx_t;

static inline uint64_t BDK_DTX_CPTX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPTX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0feb90000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_CPTX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_CPTX_SELX(a,b) bdk_dtx_cptx_selx_t
#define bustype_BDK_DTX_CPTX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPTX_SELX(a,b) "DTX_CPTX_SELX"
#define busnum_BDK_DTX_CPTX_SELX(a,b) (a)
#define arguments_BDK_DTX_CPTX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_cpt0_bcst_rsp
 *
 * DTX CPT0 Control Register
 */
union bdk_dtx_cpt0_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_cpt0_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cpt0_bcst_rsp_s cn; */
};
typedef union bdk_dtx_cpt0_bcst_rsp bdk_dtx_cpt0_bcst_rsp_t;

#define BDK_DTX_CPT0_BCST_RSP BDK_DTX_CPT0_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_CPT0_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPT0_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 0x87e0feb90080ll;
    __bdk_csr_fatal("DTX_CPT0_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_CPT0_BCST_RSP bdk_dtx_cpt0_bcst_rsp_t
#define bustype_BDK_DTX_CPT0_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPT0_BCST_RSP "DTX_CPT0_BCST_RSP"
#define busnum_BDK_DTX_CPT0_BCST_RSP 0
#define arguments_BDK_DTX_CPT0_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_cpt0_ctl
 *
 * DTX CPT0 Control Register
 */
union bdk_dtx_cpt0_ctl
{
    uint64_t u;
    struct bdk_dtx_cpt0_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cpt0_ctl_s cn; */
};
typedef union bdk_dtx_cpt0_ctl bdk_dtx_cpt0_ctl_t;

#define BDK_DTX_CPT0_CTL BDK_DTX_CPT0_CTL_FUNC()
static inline uint64_t BDK_DTX_CPT0_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPT0_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 0x87e0feb90060ll;
    __bdk_csr_fatal("DTX_CPT0_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_CPT0_CTL bdk_dtx_cpt0_ctl_t
#define bustype_BDK_DTX_CPT0_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPT0_CTL "DTX_CPT0_CTL"
#define busnum_BDK_DTX_CPT0_CTL 0
#define arguments_BDK_DTX_CPT0_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_cpt0_dat#
 *
 * DTX CPT0 Raw Data Register
 */
union bdk_dtx_cpt0_datx
{
    uint64_t u;
    struct bdk_dtx_cpt0_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cpt0_datx_s cn; */
};
typedef union bdk_dtx_cpt0_datx bdk_dtx_cpt0_datx_t;

static inline uint64_t BDK_DTX_CPT0_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPT0_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0feb90040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_CPT0_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CPT0_DATX(a) bdk_dtx_cpt0_datx_t
#define bustype_BDK_DTX_CPT0_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPT0_DATX(a) "DTX_CPT0_DATX"
#define busnum_BDK_DTX_CPT0_DATX(a) (a)
#define arguments_BDK_DTX_CPT0_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_cpt0_ena#
 *
 * DTX CPT0 Data Enable Register
 */
union bdk_dtx_cpt0_enax
{
    uint64_t u;
    struct bdk_dtx_cpt0_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cpt0_enax_s cn; */
};
typedef union bdk_dtx_cpt0_enax bdk_dtx_cpt0_enax_t;

static inline uint64_t BDK_DTX_CPT0_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPT0_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0feb90020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_CPT0_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CPT0_ENAX(a) bdk_dtx_cpt0_enax_t
#define bustype_BDK_DTX_CPT0_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPT0_ENAX(a) "DTX_CPT0_ENAX"
#define busnum_BDK_DTX_CPT0_ENAX(a) (a)
#define arguments_BDK_DTX_CPT0_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_cpt0_sel#
 *
 * DTX CPT0 Select Register
 */
union bdk_dtx_cpt0_selx
{
    uint64_t u;
    struct bdk_dtx_cpt0_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cpt0_selx_s cn; */
};
typedef union bdk_dtx_cpt0_selx bdk_dtx_cpt0_selx_t;

static inline uint64_t BDK_DTX_CPT0_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPT0_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0feb90000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_CPT0_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CPT0_SELX(a) bdk_dtx_cpt0_selx_t
#define bustype_BDK_DTX_CPT0_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPT0_SELX(a) "DTX_CPT0_SELX"
#define busnum_BDK_DTX_CPT0_SELX(a) (a)
#define arguments_BDK_DTX_CPT0_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_cpt_bcst_rsp
 *
 * DTX CPT Control Register
 */
union bdk_dtx_cpt_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_cpt_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cpt_bcst_rsp_s cn; */
};
typedef union bdk_dtx_cpt_bcst_rsp bdk_dtx_cpt_bcst_rsp_t;

#define BDK_DTX_CPT_BCST_RSP BDK_DTX_CPT_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_CPT_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPT_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0feb90080ll;
    __bdk_csr_fatal("DTX_CPT_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_CPT_BCST_RSP bdk_dtx_cpt_bcst_rsp_t
#define bustype_BDK_DTX_CPT_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPT_BCST_RSP "DTX_CPT_BCST_RSP"
#define busnum_BDK_DTX_CPT_BCST_RSP 0
#define arguments_BDK_DTX_CPT_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_cpt_ctl
 *
 * DTX CPT Control Register
 */
union bdk_dtx_cpt_ctl
{
    uint64_t u;
    struct bdk_dtx_cpt_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cpt_ctl_s cn; */
};
typedef union bdk_dtx_cpt_ctl bdk_dtx_cpt_ctl_t;

#define BDK_DTX_CPT_CTL BDK_DTX_CPT_CTL_FUNC()
static inline uint64_t BDK_DTX_CPT_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPT_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0feb90060ll;
    __bdk_csr_fatal("DTX_CPT_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_CPT_CTL bdk_dtx_cpt_ctl_t
#define bustype_BDK_DTX_CPT_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPT_CTL "DTX_CPT_CTL"
#define busnum_BDK_DTX_CPT_CTL 0
#define arguments_BDK_DTX_CPT_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_cpt_dat#
 *
 * DTX CPT Raw Data Register
 */
union bdk_dtx_cpt_datx
{
    uint64_t u;
    struct bdk_dtx_cpt_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cpt_datx_s cn; */
};
typedef union bdk_dtx_cpt_datx bdk_dtx_cpt_datx_t;

static inline uint64_t BDK_DTX_CPT_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPT_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb90040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_CPT_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CPT_DATX(a) bdk_dtx_cpt_datx_t
#define bustype_BDK_DTX_CPT_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPT_DATX(a) "DTX_CPT_DATX"
#define busnum_BDK_DTX_CPT_DATX(a) (a)
#define arguments_BDK_DTX_CPT_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_cpt_ena#
 *
 * DTX CPT Data Enable Register
 */
union bdk_dtx_cpt_enax
{
    uint64_t u;
    struct bdk_dtx_cpt_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cpt_enax_s cn; */
};
typedef union bdk_dtx_cpt_enax bdk_dtx_cpt_enax_t;

static inline uint64_t BDK_DTX_CPT_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPT_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb90020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_CPT_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CPT_ENAX(a) bdk_dtx_cpt_enax_t
#define bustype_BDK_DTX_CPT_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPT_ENAX(a) "DTX_CPT_ENAX"
#define busnum_BDK_DTX_CPT_ENAX(a) (a)
#define arguments_BDK_DTX_CPT_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_cpt_sel#
 *
 * DTX CPT Select Register
 */
union bdk_dtx_cpt_selx
{
    uint64_t u;
    struct bdk_dtx_cpt_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_cpt_selx_s cn; */
};
typedef union bdk_dtx_cpt_selx bdk_dtx_cpt_selx_t;

static inline uint64_t BDK_DTX_CPT_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_CPT_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb90000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_CPT_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_CPT_SELX(a) bdk_dtx_cpt_selx_t
#define bustype_BDK_DTX_CPT_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_CPT_SELX(a) "DTX_CPT_SELX"
#define busnum_BDK_DTX_CPT_SELX(a) (a)
#define arguments_BDK_DTX_CPT_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_dap_bcst_rsp
 *
 * DTX DAP Control Register
 */
union bdk_dtx_dap_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_dap_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_dap_bcst_rsp_s cn; */
};
typedef union bdk_dtx_dap_bcst_rsp bdk_dtx_dap_bcst_rsp_t;

#define BDK_DTX_DAP_BCST_RSP BDK_DTX_DAP_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_DAP_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DAP_BCST_RSP_FUNC(void)
{
    return 0x87e0fe010080ll;
}

#define typedef_BDK_DTX_DAP_BCST_RSP bdk_dtx_dap_bcst_rsp_t
#define bustype_BDK_DTX_DAP_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DAP_BCST_RSP "DTX_DAP_BCST_RSP"
#define busnum_BDK_DTX_DAP_BCST_RSP 0
#define arguments_BDK_DTX_DAP_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_dap_ctl
 *
 * DTX DAP Control Register
 */
union bdk_dtx_dap_ctl
{
    uint64_t u;
    struct bdk_dtx_dap_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_dap_ctl_s cn; */
};
typedef union bdk_dtx_dap_ctl bdk_dtx_dap_ctl_t;

#define BDK_DTX_DAP_CTL BDK_DTX_DAP_CTL_FUNC()
static inline uint64_t BDK_DTX_DAP_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DAP_CTL_FUNC(void)
{
    return 0x87e0fe010060ll;
}

#define typedef_BDK_DTX_DAP_CTL bdk_dtx_dap_ctl_t
#define bustype_BDK_DTX_DAP_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DAP_CTL "DTX_DAP_CTL"
#define busnum_BDK_DTX_DAP_CTL 0
#define arguments_BDK_DTX_DAP_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_dap_dat#
 *
 * DTX DAP Raw Data Register
 */
union bdk_dtx_dap_datx
{
    uint64_t u;
    struct bdk_dtx_dap_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_dap_datx_s cn; */
};
typedef union bdk_dtx_dap_datx bdk_dtx_dap_datx_t;

static inline uint64_t BDK_DTX_DAP_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DAP_DATX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe010040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_DAP_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_DAP_DATX(a) bdk_dtx_dap_datx_t
#define bustype_BDK_DTX_DAP_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DAP_DATX(a) "DTX_DAP_DATX"
#define busnum_BDK_DTX_DAP_DATX(a) (a)
#define arguments_BDK_DTX_DAP_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_dap_ena#
 *
 * DTX DAP Data Enable Register
 */
union bdk_dtx_dap_enax
{
    uint64_t u;
    struct bdk_dtx_dap_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_dap_enax_s cn; */
};
typedef union bdk_dtx_dap_enax bdk_dtx_dap_enax_t;

static inline uint64_t BDK_DTX_DAP_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DAP_ENAX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe010020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_DAP_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_DAP_ENAX(a) bdk_dtx_dap_enax_t
#define bustype_BDK_DTX_DAP_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DAP_ENAX(a) "DTX_DAP_ENAX"
#define busnum_BDK_DTX_DAP_ENAX(a) (a)
#define arguments_BDK_DTX_DAP_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_dap_sel#
 *
 * DTX DAP Select Register
 */
union bdk_dtx_dap_selx
{
    uint64_t u;
    struct bdk_dtx_dap_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_dap_selx_s cn; */
};
typedef union bdk_dtx_dap_selx bdk_dtx_dap_selx_t;

static inline uint64_t BDK_DTX_DAP_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DAP_SELX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe010000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_DAP_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_DAP_SELX(a) bdk_dtx_dap_selx_t
#define bustype_BDK_DTX_DAP_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DAP_SELX(a) "DTX_DAP_SELX"
#define busnum_BDK_DTX_DAP_SELX(a) (a)
#define arguments_BDK_DTX_DAP_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ddf_bcst_rsp
 *
 * DTX DDF Control Register
 */
union bdk_dtx_ddf_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_ddf_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ddf_bcst_rsp_s cn; */
};
typedef union bdk_dtx_ddf_bcst_rsp bdk_dtx_ddf_bcst_rsp_t;

#define BDK_DTX_DDF_BCST_RSP BDK_DTX_DDF_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_DDF_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DDF_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0fe848080ll;
    __bdk_csr_fatal("DTX_DDF_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_DDF_BCST_RSP bdk_dtx_ddf_bcst_rsp_t
#define bustype_BDK_DTX_DDF_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DDF_BCST_RSP "DTX_DDF_BCST_RSP"
#define busnum_BDK_DTX_DDF_BCST_RSP 0
#define arguments_BDK_DTX_DDF_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_ddf_ctl
 *
 * DTX DDF Control Register
 */
union bdk_dtx_ddf_ctl
{
    uint64_t u;
    struct bdk_dtx_ddf_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ddf_ctl_s cn; */
};
typedef union bdk_dtx_ddf_ctl bdk_dtx_ddf_ctl_t;

#define BDK_DTX_DDF_CTL BDK_DTX_DDF_CTL_FUNC()
static inline uint64_t BDK_DTX_DDF_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DDF_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0fe848060ll;
    __bdk_csr_fatal("DTX_DDF_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_DDF_CTL bdk_dtx_ddf_ctl_t
#define bustype_BDK_DTX_DDF_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DDF_CTL "DTX_DDF_CTL"
#define busnum_BDK_DTX_DDF_CTL 0
#define arguments_BDK_DTX_DDF_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_ddf_dat#
 *
 * DTX DDF Raw Data Register
 */
union bdk_dtx_ddf_datx
{
    uint64_t u;
    struct bdk_dtx_ddf_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ddf_datx_s cn; */
};
typedef union bdk_dtx_ddf_datx bdk_dtx_ddf_datx_t;

static inline uint64_t BDK_DTX_DDF_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DDF_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe848040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_DDF_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_DDF_DATX(a) bdk_dtx_ddf_datx_t
#define bustype_BDK_DTX_DDF_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DDF_DATX(a) "DTX_DDF_DATX"
#define busnum_BDK_DTX_DDF_DATX(a) (a)
#define arguments_BDK_DTX_DDF_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ddf_ena#
 *
 * DTX DDF Data Enable Register
 */
union bdk_dtx_ddf_enax
{
    uint64_t u;
    struct bdk_dtx_ddf_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ddf_enax_s cn; */
};
typedef union bdk_dtx_ddf_enax bdk_dtx_ddf_enax_t;

static inline uint64_t BDK_DTX_DDF_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DDF_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe848020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_DDF_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_DDF_ENAX(a) bdk_dtx_ddf_enax_t
#define bustype_BDK_DTX_DDF_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DDF_ENAX(a) "DTX_DDF_ENAX"
#define busnum_BDK_DTX_DDF_ENAX(a) (a)
#define arguments_BDK_DTX_DDF_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ddf_sel#
 *
 * DTX DDF Select Register
 */
union bdk_dtx_ddf_selx
{
    uint64_t u;
    struct bdk_dtx_ddf_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ddf_selx_s cn; */
};
typedef union bdk_dtx_ddf_selx bdk_dtx_ddf_selx_t;

static inline uint64_t BDK_DTX_DDF_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DDF_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe848000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_DDF_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_DDF_SELX(a) bdk_dtx_ddf_selx_t
#define bustype_BDK_DTX_DDF_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DDF_SELX(a) "DTX_DDF_SELX"
#define busnum_BDK_DTX_DDF_SELX(a) (a)
#define arguments_BDK_DTX_DDF_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_dfa_bcst_rsp
 *
 * DTX DFA Control Register
 */
union bdk_dtx_dfa_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_dfa_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_dfa_bcst_rsp_s cn; */
};
typedef union bdk_dtx_dfa_bcst_rsp bdk_dtx_dfa_bcst_rsp_t;

#define BDK_DTX_DFA_BCST_RSP BDK_DTX_DFA_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_DFA_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DFA_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0fea30080ll;
    __bdk_csr_fatal("DTX_DFA_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_DFA_BCST_RSP bdk_dtx_dfa_bcst_rsp_t
#define bustype_BDK_DTX_DFA_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DFA_BCST_RSP "DTX_DFA_BCST_RSP"
#define busnum_BDK_DTX_DFA_BCST_RSP 0
#define arguments_BDK_DTX_DFA_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_dfa_ctl
 *
 * DTX DFA Control Register
 */
union bdk_dtx_dfa_ctl
{
    uint64_t u;
    struct bdk_dtx_dfa_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_dfa_ctl_s cn; */
};
typedef union bdk_dtx_dfa_ctl bdk_dtx_dfa_ctl_t;

#define BDK_DTX_DFA_CTL BDK_DTX_DFA_CTL_FUNC()
static inline uint64_t BDK_DTX_DFA_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DFA_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0fea30060ll;
    __bdk_csr_fatal("DTX_DFA_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_DFA_CTL bdk_dtx_dfa_ctl_t
#define bustype_BDK_DTX_DFA_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DFA_CTL "DTX_DFA_CTL"
#define busnum_BDK_DTX_DFA_CTL 0
#define arguments_BDK_DTX_DFA_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_dfa_dat#
 *
 * DTX DFA Raw Data Register
 */
union bdk_dtx_dfa_datx
{
    uint64_t u;
    struct bdk_dtx_dfa_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_dfa_datx_s cn; */
};
typedef union bdk_dtx_dfa_datx bdk_dtx_dfa_datx_t;

static inline uint64_t BDK_DTX_DFA_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DFA_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fea30040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_DFA_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_DFA_DATX(a) bdk_dtx_dfa_datx_t
#define bustype_BDK_DTX_DFA_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DFA_DATX(a) "DTX_DFA_DATX"
#define busnum_BDK_DTX_DFA_DATX(a) (a)
#define arguments_BDK_DTX_DFA_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_dfa_ena#
 *
 * DTX DFA Data Enable Register
 */
union bdk_dtx_dfa_enax
{
    uint64_t u;
    struct bdk_dtx_dfa_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_dfa_enax_s cn; */
};
typedef union bdk_dtx_dfa_enax bdk_dtx_dfa_enax_t;

static inline uint64_t BDK_DTX_DFA_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DFA_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fea30020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_DFA_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_DFA_ENAX(a) bdk_dtx_dfa_enax_t
#define bustype_BDK_DTX_DFA_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DFA_ENAX(a) "DTX_DFA_ENAX"
#define busnum_BDK_DTX_DFA_ENAX(a) (a)
#define arguments_BDK_DTX_DFA_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_dfa_sel#
 *
 * DTX DFA Select Register
 */
union bdk_dtx_dfa_selx
{
    uint64_t u;
    struct bdk_dtx_dfa_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_dfa_selx_s cn; */
};
typedef union bdk_dtx_dfa_selx bdk_dtx_dfa_selx_t;

static inline uint64_t BDK_DTX_DFA_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DFA_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fea30000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_DFA_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_DFA_SELX(a) bdk_dtx_dfa_selx_t
#define bustype_BDK_DTX_DFA_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DFA_SELX(a) "DTX_DFA_SELX"
#define busnum_BDK_DTX_DFA_SELX(a) (a)
#define arguments_BDK_DTX_DFA_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_dpi_bcst_rsp
 *
 * DTX DPI Control Register
 */
union bdk_dtx_dpi_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_dpi_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_dpi_bcst_rsp_s cn; */
};
typedef union bdk_dtx_dpi_bcst_rsp bdk_dtx_dpi_bcst_rsp_t;

#define BDK_DTX_DPI_BCST_RSP BDK_DTX_DPI_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_DPI_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DPI_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0feb70080ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0feb70080ll;
    __bdk_csr_fatal("DTX_DPI_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_DPI_BCST_RSP bdk_dtx_dpi_bcst_rsp_t
#define bustype_BDK_DTX_DPI_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DPI_BCST_RSP "DTX_DPI_BCST_RSP"
#define busnum_BDK_DTX_DPI_BCST_RSP 0
#define arguments_BDK_DTX_DPI_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_dpi_ctl
 *
 * DTX DPI Control Register
 */
union bdk_dtx_dpi_ctl
{
    uint64_t u;
    struct bdk_dtx_dpi_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_dpi_ctl_s cn; */
};
typedef union bdk_dtx_dpi_ctl bdk_dtx_dpi_ctl_t;

#define BDK_DTX_DPI_CTL BDK_DTX_DPI_CTL_FUNC()
static inline uint64_t BDK_DTX_DPI_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DPI_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0feb70060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0feb70060ll;
    __bdk_csr_fatal("DTX_DPI_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_DPI_CTL bdk_dtx_dpi_ctl_t
#define bustype_BDK_DTX_DPI_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DPI_CTL "DTX_DPI_CTL"
#define busnum_BDK_DTX_DPI_CTL 0
#define arguments_BDK_DTX_DPI_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_dpi_dat#
 *
 * DTX DPI Raw Data Register
 */
union bdk_dtx_dpi_datx
{
    uint64_t u;
    struct bdk_dtx_dpi_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_dpi_datx_s cn; */
};
typedef union bdk_dtx_dpi_datx bdk_dtx_dpi_datx_t;

static inline uint64_t BDK_DTX_DPI_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DPI_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb70040ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb70040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_DPI_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_DPI_DATX(a) bdk_dtx_dpi_datx_t
#define bustype_BDK_DTX_DPI_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DPI_DATX(a) "DTX_DPI_DATX"
#define busnum_BDK_DTX_DPI_DATX(a) (a)
#define arguments_BDK_DTX_DPI_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_dpi_ena#
 *
 * DTX DPI Data Enable Register
 */
union bdk_dtx_dpi_enax
{
    uint64_t u;
    struct bdk_dtx_dpi_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_dpi_enax_s cn; */
};
typedef union bdk_dtx_dpi_enax bdk_dtx_dpi_enax_t;

static inline uint64_t BDK_DTX_DPI_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DPI_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb70020ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb70020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_DPI_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_DPI_ENAX(a) bdk_dtx_dpi_enax_t
#define bustype_BDK_DTX_DPI_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DPI_ENAX(a) "DTX_DPI_ENAX"
#define busnum_BDK_DTX_DPI_ENAX(a) (a)
#define arguments_BDK_DTX_DPI_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_dpi_sel#
 *
 * DTX DPI Select Register
 */
union bdk_dtx_dpi_selx
{
    uint64_t u;
    struct bdk_dtx_dpi_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_dpi_selx_s cn; */
};
typedef union bdk_dtx_dpi_selx bdk_dtx_dpi_selx_t;

static inline uint64_t BDK_DTX_DPI_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_DPI_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb70000ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb70000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_DPI_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_DPI_SELX(a) bdk_dtx_dpi_selx_t
#define bustype_BDK_DTX_DPI_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_DPI_SELX(a) "DTX_DPI_SELX"
#define busnum_BDK_DTX_DPI_SELX(a) (a)
#define arguments_BDK_DTX_DPI_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_fpa_bcst_rsp
 *
 * DTX FPA Control Register
 */
union bdk_dtx_fpa_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_fpa_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_fpa_bcst_rsp_s cn; */
};
typedef union bdk_dtx_fpa_bcst_rsp bdk_dtx_fpa_bcst_rsp_t;

#define BDK_DTX_FPA_BCST_RSP BDK_DTX_FPA_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_FPA_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_FPA_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0fe940080ll;
    __bdk_csr_fatal("DTX_FPA_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_FPA_BCST_RSP bdk_dtx_fpa_bcst_rsp_t
#define bustype_BDK_DTX_FPA_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_FPA_BCST_RSP "DTX_FPA_BCST_RSP"
#define busnum_BDK_DTX_FPA_BCST_RSP 0
#define arguments_BDK_DTX_FPA_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_fpa_ctl
 *
 * DTX FPA Control Register
 */
union bdk_dtx_fpa_ctl
{
    uint64_t u;
    struct bdk_dtx_fpa_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_fpa_ctl_s cn; */
};
typedef union bdk_dtx_fpa_ctl bdk_dtx_fpa_ctl_t;

#define BDK_DTX_FPA_CTL BDK_DTX_FPA_CTL_FUNC()
static inline uint64_t BDK_DTX_FPA_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_FPA_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0fe940060ll;
    __bdk_csr_fatal("DTX_FPA_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_FPA_CTL bdk_dtx_fpa_ctl_t
#define bustype_BDK_DTX_FPA_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_FPA_CTL "DTX_FPA_CTL"
#define busnum_BDK_DTX_FPA_CTL 0
#define arguments_BDK_DTX_FPA_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_fpa_dat#
 *
 * DTX FPA Raw Data Register
 */
union bdk_dtx_fpa_datx
{
    uint64_t u;
    struct bdk_dtx_fpa_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_fpa_datx_s cn; */
};
typedef union bdk_dtx_fpa_datx bdk_dtx_fpa_datx_t;

static inline uint64_t BDK_DTX_FPA_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_FPA_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe940040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_FPA_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_FPA_DATX(a) bdk_dtx_fpa_datx_t
#define bustype_BDK_DTX_FPA_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_FPA_DATX(a) "DTX_FPA_DATX"
#define busnum_BDK_DTX_FPA_DATX(a) (a)
#define arguments_BDK_DTX_FPA_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_fpa_ena#
 *
 * DTX FPA Data Enable Register
 */
union bdk_dtx_fpa_enax
{
    uint64_t u;
    struct bdk_dtx_fpa_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_fpa_enax_s cn; */
};
typedef union bdk_dtx_fpa_enax bdk_dtx_fpa_enax_t;

static inline uint64_t BDK_DTX_FPA_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_FPA_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe940020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_FPA_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_FPA_ENAX(a) bdk_dtx_fpa_enax_t
#define bustype_BDK_DTX_FPA_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_FPA_ENAX(a) "DTX_FPA_ENAX"
#define busnum_BDK_DTX_FPA_ENAX(a) (a)
#define arguments_BDK_DTX_FPA_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_fpa_sel#
 *
 * DTX FPA Select Register
 */
union bdk_dtx_fpa_selx
{
    uint64_t u;
    struct bdk_dtx_fpa_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_fpa_selx_s cn; */
};
typedef union bdk_dtx_fpa_selx bdk_dtx_fpa_selx_t;

static inline uint64_t BDK_DTX_FPA_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_FPA_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe940000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_FPA_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_FPA_SELX(a) bdk_dtx_fpa_selx_t
#define bustype_BDK_DTX_FPA_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_FPA_SELX(a) "DTX_FPA_SELX"
#define busnum_BDK_DTX_FPA_SELX(a) (a)
#define arguments_BDK_DTX_FPA_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_gic_bcst_rsp
 *
 * DTX GIC Control Register
 */
union bdk_dtx_gic_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_gic_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_gic_bcst_rsp_s cn; */
};
typedef union bdk_dtx_gic_bcst_rsp bdk_dtx_gic_bcst_rsp_t;

#define BDK_DTX_GIC_BCST_RSP BDK_DTX_GIC_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_GIC_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_GIC_BCST_RSP_FUNC(void)
{
    return 0x87e0fe808080ll;
}

#define typedef_BDK_DTX_GIC_BCST_RSP bdk_dtx_gic_bcst_rsp_t
#define bustype_BDK_DTX_GIC_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_GIC_BCST_RSP "DTX_GIC_BCST_RSP"
#define busnum_BDK_DTX_GIC_BCST_RSP 0
#define arguments_BDK_DTX_GIC_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_gic_ctl
 *
 * DTX GIC Control Register
 */
union bdk_dtx_gic_ctl
{
    uint64_t u;
    struct bdk_dtx_gic_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_gic_ctl_s cn; */
};
typedef union bdk_dtx_gic_ctl bdk_dtx_gic_ctl_t;

#define BDK_DTX_GIC_CTL BDK_DTX_GIC_CTL_FUNC()
static inline uint64_t BDK_DTX_GIC_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_GIC_CTL_FUNC(void)
{
    return 0x87e0fe808060ll;
}

#define typedef_BDK_DTX_GIC_CTL bdk_dtx_gic_ctl_t
#define bustype_BDK_DTX_GIC_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_GIC_CTL "DTX_GIC_CTL"
#define busnum_BDK_DTX_GIC_CTL 0
#define arguments_BDK_DTX_GIC_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_gic_dat#
 *
 * DTX GIC Raw Data Register
 */
union bdk_dtx_gic_datx
{
    uint64_t u;
    struct bdk_dtx_gic_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_gic_datx_s cn; */
};
typedef union bdk_dtx_gic_datx bdk_dtx_gic_datx_t;

static inline uint64_t BDK_DTX_GIC_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_GIC_DATX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe808040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_GIC_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_GIC_DATX(a) bdk_dtx_gic_datx_t
#define bustype_BDK_DTX_GIC_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_GIC_DATX(a) "DTX_GIC_DATX"
#define busnum_BDK_DTX_GIC_DATX(a) (a)
#define arguments_BDK_DTX_GIC_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_gic_ena#
 *
 * DTX GIC Data Enable Register
 */
union bdk_dtx_gic_enax
{
    uint64_t u;
    struct bdk_dtx_gic_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_gic_enax_s cn; */
};
typedef union bdk_dtx_gic_enax bdk_dtx_gic_enax_t;

static inline uint64_t BDK_DTX_GIC_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_GIC_ENAX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe808020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_GIC_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_GIC_ENAX(a) bdk_dtx_gic_enax_t
#define bustype_BDK_DTX_GIC_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_GIC_ENAX(a) "DTX_GIC_ENAX"
#define busnum_BDK_DTX_GIC_ENAX(a) (a)
#define arguments_BDK_DTX_GIC_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_gic_sel#
 *
 * DTX GIC Select Register
 */
union bdk_dtx_gic_selx
{
    uint64_t u;
    struct bdk_dtx_gic_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_gic_selx_s cn; */
};
typedef union bdk_dtx_gic_selx bdk_dtx_gic_selx_t;

static inline uint64_t BDK_DTX_GIC_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_GIC_SELX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe808000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_GIC_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_GIC_SELX(a) bdk_dtx_gic_selx_t
#define bustype_BDK_DTX_GIC_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_GIC_SELX(a) "DTX_GIC_SELX"
#define busnum_BDK_DTX_GIC_SELX(a) (a)
#define arguments_BDK_DTX_GIC_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_gser#_bcst_rsp
 *
 * DTX GSER Control Register
 */
union bdk_dtx_gserx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_gserx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_gserx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_gserx_bcst_rsp bdk_dtx_gserx_bcst_rsp_t;

static inline uint64_t BDK_DTX_GSERX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_GSERX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0fe480080ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0fe480080ll + 0x8000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0fe480080ll + 0x8000ll * ((a) & 0xf);
    __bdk_csr_fatal("DTX_GSERX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_GSERX_BCST_RSP(a) bdk_dtx_gserx_bcst_rsp_t
#define bustype_BDK_DTX_GSERX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_GSERX_BCST_RSP(a) "DTX_GSERX_BCST_RSP"
#define busnum_BDK_DTX_GSERX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_GSERX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_gser#_ctl
 *
 * DTX GSER Control Register
 */
union bdk_dtx_gserx_ctl
{
    uint64_t u;
    struct bdk_dtx_gserx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_gserx_ctl_s cn; */
};
typedef union bdk_dtx_gserx_ctl bdk_dtx_gserx_ctl_t;

static inline uint64_t BDK_DTX_GSERX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_GSERX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0fe480060ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0fe480060ll + 0x8000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0fe480060ll + 0x8000ll * ((a) & 0xf);
    __bdk_csr_fatal("DTX_GSERX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_GSERX_CTL(a) bdk_dtx_gserx_ctl_t
#define bustype_BDK_DTX_GSERX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_GSERX_CTL(a) "DTX_GSERX_CTL"
#define busnum_BDK_DTX_GSERX_CTL(a) (a)
#define arguments_BDK_DTX_GSERX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_gser#_dat#
 *
 * DTX GSER Raw Data Register
 */
union bdk_dtx_gserx_datx
{
    uint64_t u;
    struct bdk_dtx_gserx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_gserx_datx_s cn; */
};
typedef union bdk_dtx_gserx_datx bdk_dtx_gserx_datx_t;

static inline uint64_t BDK_DTX_GSERX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_GSERX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe480040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=1)))
        return 0x87e0fe480040ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=1)))
        return 0x87e0fe480040ll + 0x8000ll * ((a) & 0xf) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_GSERX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_GSERX_DATX(a,b) bdk_dtx_gserx_datx_t
#define bustype_BDK_DTX_GSERX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_GSERX_DATX(a,b) "DTX_GSERX_DATX"
#define busnum_BDK_DTX_GSERX_DATX(a,b) (a)
#define arguments_BDK_DTX_GSERX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_gser#_ena#
 *
 * DTX GSER Data Enable Register
 */
union bdk_dtx_gserx_enax
{
    uint64_t u;
    struct bdk_dtx_gserx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_gserx_enax_s cn; */
};
typedef union bdk_dtx_gserx_enax bdk_dtx_gserx_enax_t;

static inline uint64_t BDK_DTX_GSERX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_GSERX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe480020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=1)))
        return 0x87e0fe480020ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=1)))
        return 0x87e0fe480020ll + 0x8000ll * ((a) & 0xf) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_GSERX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_GSERX_ENAX(a,b) bdk_dtx_gserx_enax_t
#define bustype_BDK_DTX_GSERX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_GSERX_ENAX(a,b) "DTX_GSERX_ENAX"
#define busnum_BDK_DTX_GSERX_ENAX(a,b) (a)
#define arguments_BDK_DTX_GSERX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_gser#_sel#
 *
 * DTX GSER Select Register
 */
union bdk_dtx_gserx_selx
{
    uint64_t u;
    struct bdk_dtx_gserx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_gserx_selx_s cn; */
};
typedef union bdk_dtx_gserx_selx bdk_dtx_gserx_selx_t;

static inline uint64_t BDK_DTX_GSERX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_GSERX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe480000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=1)))
        return 0x87e0fe480000ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=1)))
        return 0x87e0fe480000ll + 0x8000ll * ((a) & 0xf) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_GSERX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_GSERX_SELX(a,b) bdk_dtx_gserx_selx_t
#define bustype_BDK_DTX_GSERX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_GSERX_SELX(a,b) "DTX_GSERX_SELX"
#define busnum_BDK_DTX_GSERX_SELX(a,b) (a)
#define arguments_BDK_DTX_GSERX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_gti_bcst_rsp
 *
 * DTX GTI Control Register
 */
union bdk_dtx_gti_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_gti_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_gti_bcst_rsp_s cn; */
};
typedef union bdk_dtx_gti_bcst_rsp bdk_dtx_gti_bcst_rsp_t;

#define BDK_DTX_GTI_BCST_RSP BDK_DTX_GTI_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_GTI_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_GTI_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0fea20080ll;
    __bdk_csr_fatal("DTX_GTI_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_GTI_BCST_RSP bdk_dtx_gti_bcst_rsp_t
#define bustype_BDK_DTX_GTI_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_GTI_BCST_RSP "DTX_GTI_BCST_RSP"
#define busnum_BDK_DTX_GTI_BCST_RSP 0
#define arguments_BDK_DTX_GTI_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_gti_ctl
 *
 * DTX GTI Control Register
 */
union bdk_dtx_gti_ctl
{
    uint64_t u;
    struct bdk_dtx_gti_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_gti_ctl_s cn; */
};
typedef union bdk_dtx_gti_ctl bdk_dtx_gti_ctl_t;

#define BDK_DTX_GTI_CTL BDK_DTX_GTI_CTL_FUNC()
static inline uint64_t BDK_DTX_GTI_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_GTI_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0fea20060ll;
    __bdk_csr_fatal("DTX_GTI_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_GTI_CTL bdk_dtx_gti_ctl_t
#define bustype_BDK_DTX_GTI_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_GTI_CTL "DTX_GTI_CTL"
#define busnum_BDK_DTX_GTI_CTL 0
#define arguments_BDK_DTX_GTI_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_gti_dat#
 *
 * DTX GTI Raw Data Register
 */
union bdk_dtx_gti_datx
{
    uint64_t u;
    struct bdk_dtx_gti_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_gti_datx_s cn; */
};
typedef union bdk_dtx_gti_datx bdk_dtx_gti_datx_t;

static inline uint64_t BDK_DTX_GTI_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_GTI_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fea20040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_GTI_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_GTI_DATX(a) bdk_dtx_gti_datx_t
#define bustype_BDK_DTX_GTI_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_GTI_DATX(a) "DTX_GTI_DATX"
#define busnum_BDK_DTX_GTI_DATX(a) (a)
#define arguments_BDK_DTX_GTI_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_gti_ena#
 *
 * DTX GTI Data Enable Register
 */
union bdk_dtx_gti_enax
{
    uint64_t u;
    struct bdk_dtx_gti_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_gti_enax_s cn; */
};
typedef union bdk_dtx_gti_enax bdk_dtx_gti_enax_t;

static inline uint64_t BDK_DTX_GTI_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_GTI_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fea20020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_GTI_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_GTI_ENAX(a) bdk_dtx_gti_enax_t
#define bustype_BDK_DTX_GTI_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_GTI_ENAX(a) "DTX_GTI_ENAX"
#define busnum_BDK_DTX_GTI_ENAX(a) (a)
#define arguments_BDK_DTX_GTI_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_gti_sel#
 *
 * DTX GTI Select Register
 */
union bdk_dtx_gti_selx
{
    uint64_t u;
    struct bdk_dtx_gti_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_gti_selx_s cn; */
};
typedef union bdk_dtx_gti_selx bdk_dtx_gti_selx_t;

static inline uint64_t BDK_DTX_GTI_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_GTI_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fea20000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_GTI_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_GTI_SELX(a) bdk_dtx_gti_selx_t
#define bustype_BDK_DTX_GTI_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_GTI_SELX(a) "DTX_GTI_SELX"
#define busnum_BDK_DTX_GTI_SELX(a) (a)
#define arguments_BDK_DTX_GTI_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_iobn#_bcst_rsp
 *
 * DTX IOBN Control Register
 */
union bdk_dtx_iobnx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_iobnx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_iobnx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_iobnx_bcst_rsp bdk_dtx_iobnx_bcst_rsp_t;

static inline uint64_t BDK_DTX_IOBNX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_IOBNX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0fe780080ll + 0x8000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe780080ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe780080ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe780080ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_IOBNX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_IOBNX_BCST_RSP(a) bdk_dtx_iobnx_bcst_rsp_t
#define bustype_BDK_DTX_IOBNX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_IOBNX_BCST_RSP(a) "DTX_IOBNX_BCST_RSP"
#define busnum_BDK_DTX_IOBNX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_IOBNX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_iobn#_ctl
 *
 * DTX IOBN Control Register
 */
union bdk_dtx_iobnx_ctl
{
    uint64_t u;
    struct bdk_dtx_iobnx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_iobnx_ctl_s cn; */
};
typedef union bdk_dtx_iobnx_ctl bdk_dtx_iobnx_ctl_t;

static inline uint64_t BDK_DTX_IOBNX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_IOBNX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0fe780060ll + 0x8000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe780060ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe780060ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe780060ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_IOBNX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_IOBNX_CTL(a) bdk_dtx_iobnx_ctl_t
#define bustype_BDK_DTX_IOBNX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_IOBNX_CTL(a) "DTX_IOBNX_CTL"
#define busnum_BDK_DTX_IOBNX_CTL(a) (a)
#define arguments_BDK_DTX_IOBNX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_iobn#_dat#
 *
 * DTX IOBN Raw Data Register
 */
union bdk_dtx_iobnx_datx
{
    uint64_t u;
    struct bdk_dtx_iobnx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_iobnx_datx_s cn; */
};
typedef union bdk_dtx_iobnx_datx bdk_dtx_iobnx_datx_t;

static inline uint64_t BDK_DTX_IOBNX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_IOBNX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe780040ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe780040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe780040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe780040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_IOBNX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_IOBNX_DATX(a,b) bdk_dtx_iobnx_datx_t
#define bustype_BDK_DTX_IOBNX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_IOBNX_DATX(a,b) "DTX_IOBNX_DATX"
#define busnum_BDK_DTX_IOBNX_DATX(a,b) (a)
#define arguments_BDK_DTX_IOBNX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_iobn#_ena#
 *
 * DTX IOBN Data Enable Register
 */
union bdk_dtx_iobnx_enax
{
    uint64_t u;
    struct bdk_dtx_iobnx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_iobnx_enax_s cn; */
};
typedef union bdk_dtx_iobnx_enax bdk_dtx_iobnx_enax_t;

static inline uint64_t BDK_DTX_IOBNX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_IOBNX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe780020ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe780020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe780020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe780020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_IOBNX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_IOBNX_ENAX(a,b) bdk_dtx_iobnx_enax_t
#define bustype_BDK_DTX_IOBNX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_IOBNX_ENAX(a,b) "DTX_IOBNX_ENAX"
#define busnum_BDK_DTX_IOBNX_ENAX(a,b) (a)
#define arguments_BDK_DTX_IOBNX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_iobn#_inrx_bcst_rsp
 *
 * DTX IOBN_INRX Control Register
 */
union bdk_dtx_iobnx_inrx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_iobnx_inrx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_iobnx_inrx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_iobnx_inrx_bcst_rsp bdk_dtx_iobnx_inrx_bcst_rsp_t;

static inline uint64_t BDK_DTX_IOBNX_INRX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_IOBNX_INRX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe7a0080ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_IOBNX_INRX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_IOBNX_INRX_BCST_RSP(a) bdk_dtx_iobnx_inrx_bcst_rsp_t
#define bustype_BDK_DTX_IOBNX_INRX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_IOBNX_INRX_BCST_RSP(a) "DTX_IOBNX_INRX_BCST_RSP"
#define busnum_BDK_DTX_IOBNX_INRX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_IOBNX_INRX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_iobn#_inrx_ctl
 *
 * DTX IOBN_INRX Control Register
 */
union bdk_dtx_iobnx_inrx_ctl
{
    uint64_t u;
    struct bdk_dtx_iobnx_inrx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_iobnx_inrx_ctl_s cn; */
};
typedef union bdk_dtx_iobnx_inrx_ctl bdk_dtx_iobnx_inrx_ctl_t;

static inline uint64_t BDK_DTX_IOBNX_INRX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_IOBNX_INRX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe7a0060ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_IOBNX_INRX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_IOBNX_INRX_CTL(a) bdk_dtx_iobnx_inrx_ctl_t
#define bustype_BDK_DTX_IOBNX_INRX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_IOBNX_INRX_CTL(a) "DTX_IOBNX_INRX_CTL"
#define busnum_BDK_DTX_IOBNX_INRX_CTL(a) (a)
#define arguments_BDK_DTX_IOBNX_INRX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_iobn#_inrx_dat#
 *
 * DTX IOBN_INRX Raw Data Register
 */
union bdk_dtx_iobnx_inrx_datx
{
    uint64_t u;
    struct bdk_dtx_iobnx_inrx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_iobnx_inrx_datx_s cn; */
};
typedef union bdk_dtx_iobnx_inrx_datx bdk_dtx_iobnx_inrx_datx_t;

static inline uint64_t BDK_DTX_IOBNX_INRX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_IOBNX_INRX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe7a0040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_IOBNX_INRX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_IOBNX_INRX_DATX(a,b) bdk_dtx_iobnx_inrx_datx_t
#define bustype_BDK_DTX_IOBNX_INRX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_IOBNX_INRX_DATX(a,b) "DTX_IOBNX_INRX_DATX"
#define busnum_BDK_DTX_IOBNX_INRX_DATX(a,b) (a)
#define arguments_BDK_DTX_IOBNX_INRX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_iobn#_inrx_ena#
 *
 * DTX IOBN_INRX Data Enable Register
 */
union bdk_dtx_iobnx_inrx_enax
{
    uint64_t u;
    struct bdk_dtx_iobnx_inrx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_iobnx_inrx_enax_s cn; */
};
typedef union bdk_dtx_iobnx_inrx_enax bdk_dtx_iobnx_inrx_enax_t;

static inline uint64_t BDK_DTX_IOBNX_INRX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_IOBNX_INRX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe7a0020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_IOBNX_INRX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_IOBNX_INRX_ENAX(a,b) bdk_dtx_iobnx_inrx_enax_t
#define bustype_BDK_DTX_IOBNX_INRX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_IOBNX_INRX_ENAX(a,b) "DTX_IOBNX_INRX_ENAX"
#define busnum_BDK_DTX_IOBNX_INRX_ENAX(a,b) (a)
#define arguments_BDK_DTX_IOBNX_INRX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_iobn#_inrx_sel#
 *
 * DTX IOBN_INRX Select Register
 */
union bdk_dtx_iobnx_inrx_selx
{
    uint64_t u;
    struct bdk_dtx_iobnx_inrx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_iobnx_inrx_selx_s cn; */
};
typedef union bdk_dtx_iobnx_inrx_selx bdk_dtx_iobnx_inrx_selx_t;

static inline uint64_t BDK_DTX_IOBNX_INRX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_IOBNX_INRX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe7a0000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_IOBNX_INRX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_IOBNX_INRX_SELX(a,b) bdk_dtx_iobnx_inrx_selx_t
#define bustype_BDK_DTX_IOBNX_INRX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_IOBNX_INRX_SELX(a,b) "DTX_IOBNX_INRX_SELX"
#define busnum_BDK_DTX_IOBNX_INRX_SELX(a,b) (a)
#define arguments_BDK_DTX_IOBNX_INRX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_iobn#_sel#
 *
 * DTX IOBN Select Register
 */
union bdk_dtx_iobnx_selx
{
    uint64_t u;
    struct bdk_dtx_iobnx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_iobnx_selx_s cn; */
};
typedef union bdk_dtx_iobnx_selx bdk_dtx_iobnx_selx_t;

static inline uint64_t BDK_DTX_IOBNX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_IOBNX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe780000ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe780000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe780000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe780000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_IOBNX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_IOBNX_SELX(a,b) bdk_dtx_iobnx_selx_t
#define bustype_BDK_DTX_IOBNX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_IOBNX_SELX(a,b) "DTX_IOBNX_SELX"
#define busnum_BDK_DTX_IOBNX_SELX(a,b) (a)
#define arguments_BDK_DTX_IOBNX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_key_bcst_rsp
 *
 * DTX KEY Control Register
 */
union bdk_dtx_key_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_key_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_key_bcst_rsp_s cn; */
};
typedef union bdk_dtx_key_bcst_rsp bdk_dtx_key_bcst_rsp_t;

#define BDK_DTX_KEY_BCST_RSP BDK_DTX_KEY_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_KEY_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_KEY_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e0fe208080ll;
    __bdk_csr_fatal("DTX_KEY_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_KEY_BCST_RSP bdk_dtx_key_bcst_rsp_t
#define bustype_BDK_DTX_KEY_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_KEY_BCST_RSP "DTX_KEY_BCST_RSP"
#define busnum_BDK_DTX_KEY_BCST_RSP 0
#define arguments_BDK_DTX_KEY_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_key_ctl
 *
 * DTX KEY Control Register
 */
union bdk_dtx_key_ctl
{
    uint64_t u;
    struct bdk_dtx_key_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_key_ctl_s cn; */
};
typedef union bdk_dtx_key_ctl bdk_dtx_key_ctl_t;

#define BDK_DTX_KEY_CTL BDK_DTX_KEY_CTL_FUNC()
static inline uint64_t BDK_DTX_KEY_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_KEY_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e0fe208060ll;
    __bdk_csr_fatal("DTX_KEY_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_KEY_CTL bdk_dtx_key_ctl_t
#define bustype_BDK_DTX_KEY_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_KEY_CTL "DTX_KEY_CTL"
#define busnum_BDK_DTX_KEY_CTL 0
#define arguments_BDK_DTX_KEY_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_key_dat#
 *
 * DTX KEY Raw Data Register
 */
union bdk_dtx_key_datx
{
    uint64_t u;
    struct bdk_dtx_key_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_key_datx_s cn; */
};
typedef union bdk_dtx_key_datx bdk_dtx_key_datx_t;

static inline uint64_t BDK_DTX_KEY_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_KEY_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX) && (a<=1))
        return 0x87e0fe208040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_KEY_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_KEY_DATX(a) bdk_dtx_key_datx_t
#define bustype_BDK_DTX_KEY_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_KEY_DATX(a) "DTX_KEY_DATX"
#define busnum_BDK_DTX_KEY_DATX(a) (a)
#define arguments_BDK_DTX_KEY_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_key_ena#
 *
 * DTX KEY Data Enable Register
 */
union bdk_dtx_key_enax
{
    uint64_t u;
    struct bdk_dtx_key_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_key_enax_s cn; */
};
typedef union bdk_dtx_key_enax bdk_dtx_key_enax_t;

static inline uint64_t BDK_DTX_KEY_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_KEY_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX) && (a<=1))
        return 0x87e0fe208020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_KEY_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_KEY_ENAX(a) bdk_dtx_key_enax_t
#define bustype_BDK_DTX_KEY_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_KEY_ENAX(a) "DTX_KEY_ENAX"
#define busnum_BDK_DTX_KEY_ENAX(a) (a)
#define arguments_BDK_DTX_KEY_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_key_sel#
 *
 * DTX KEY Select Register
 */
union bdk_dtx_key_selx
{
    uint64_t u;
    struct bdk_dtx_key_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_key_selx_s cn; */
};
typedef union bdk_dtx_key_selx bdk_dtx_key_selx_t;

static inline uint64_t BDK_DTX_KEY_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_KEY_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX) && (a<=1))
        return 0x87e0fe208000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_KEY_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_KEY_SELX(a) bdk_dtx_key_selx_t
#define bustype_BDK_DTX_KEY_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_KEY_SELX(a) "DTX_KEY_SELX"
#define busnum_BDK_DTX_KEY_SELX(a) (a)
#define arguments_BDK_DTX_KEY_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_l2c_cbc#_bcst_rsp
 *
 * DTX L2C_CBC Control Register
 */
union bdk_dtx_l2c_cbcx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_l2c_cbcx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_l2c_cbcx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_l2c_cbcx_bcst_rsp bdk_dtx_l2c_cbcx_bcst_rsp_t;

static inline uint64_t BDK_DTX_L2C_CBCX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_L2C_CBCX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0fe2c0080ll + 0x8000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe2c0080ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e0fe2c0080ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_L2C_CBCX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_L2C_CBCX_BCST_RSP(a) bdk_dtx_l2c_cbcx_bcst_rsp_t
#define bustype_BDK_DTX_L2C_CBCX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_L2C_CBCX_BCST_RSP(a) "DTX_L2C_CBCX_BCST_RSP"
#define busnum_BDK_DTX_L2C_CBCX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_L2C_CBCX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_l2c_cbc#_ctl
 *
 * DTX L2C_CBC Control Register
 */
union bdk_dtx_l2c_cbcx_ctl
{
    uint64_t u;
    struct bdk_dtx_l2c_cbcx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_l2c_cbcx_ctl_s cn; */
};
typedef union bdk_dtx_l2c_cbcx_ctl bdk_dtx_l2c_cbcx_ctl_t;

static inline uint64_t BDK_DTX_L2C_CBCX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_L2C_CBCX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0fe2c0060ll + 0x8000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe2c0060ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e0fe2c0060ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_L2C_CBCX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_L2C_CBCX_CTL(a) bdk_dtx_l2c_cbcx_ctl_t
#define bustype_BDK_DTX_L2C_CBCX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_L2C_CBCX_CTL(a) "DTX_L2C_CBCX_CTL"
#define busnum_BDK_DTX_L2C_CBCX_CTL(a) (a)
#define arguments_BDK_DTX_L2C_CBCX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_l2c_cbc#_dat#
 *
 * DTX L2C_CBC Raw Data Register
 */
union bdk_dtx_l2c_cbcx_datx
{
    uint64_t u;
    struct bdk_dtx_l2c_cbcx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_l2c_cbcx_datx_s cn; */
};
typedef union bdk_dtx_l2c_cbcx_datx bdk_dtx_l2c_cbcx_datx_t;

static inline uint64_t BDK_DTX_L2C_CBCX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_L2C_CBCX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe2c0040ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe2c0040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe2c0040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_L2C_CBCX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_L2C_CBCX_DATX(a,b) bdk_dtx_l2c_cbcx_datx_t
#define bustype_BDK_DTX_L2C_CBCX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_L2C_CBCX_DATX(a,b) "DTX_L2C_CBCX_DATX"
#define busnum_BDK_DTX_L2C_CBCX_DATX(a,b) (a)
#define arguments_BDK_DTX_L2C_CBCX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_l2c_cbc#_ena#
 *
 * DTX L2C_CBC Data Enable Register
 */
union bdk_dtx_l2c_cbcx_enax
{
    uint64_t u;
    struct bdk_dtx_l2c_cbcx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_l2c_cbcx_enax_s cn; */
};
typedef union bdk_dtx_l2c_cbcx_enax bdk_dtx_l2c_cbcx_enax_t;

static inline uint64_t BDK_DTX_L2C_CBCX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_L2C_CBCX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe2c0020ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe2c0020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe2c0020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_L2C_CBCX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_L2C_CBCX_ENAX(a,b) bdk_dtx_l2c_cbcx_enax_t
#define bustype_BDK_DTX_L2C_CBCX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_L2C_CBCX_ENAX(a,b) "DTX_L2C_CBCX_ENAX"
#define busnum_BDK_DTX_L2C_CBCX_ENAX(a,b) (a)
#define arguments_BDK_DTX_L2C_CBCX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_l2c_cbc#_sel#
 *
 * DTX L2C_CBC Select Register
 */
union bdk_dtx_l2c_cbcx_selx
{
    uint64_t u;
    struct bdk_dtx_l2c_cbcx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_l2c_cbcx_selx_s cn; */
};
typedef union bdk_dtx_l2c_cbcx_selx bdk_dtx_l2c_cbcx_selx_t;

static inline uint64_t BDK_DTX_L2C_CBCX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_L2C_CBCX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe2c0000ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe2c0000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe2c0000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_L2C_CBCX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_L2C_CBCX_SELX(a,b) bdk_dtx_l2c_cbcx_selx_t
#define bustype_BDK_DTX_L2C_CBCX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_L2C_CBCX_SELX(a,b) "DTX_L2C_CBCX_SELX"
#define busnum_BDK_DTX_L2C_CBCX_SELX(a,b) (a)
#define arguments_BDK_DTX_L2C_CBCX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_l2c_mci#_bcst_rsp
 *
 * DTX L2C_MCI Control Register
 */
union bdk_dtx_l2c_mcix_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_l2c_mcix_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_l2c_mcix_bcst_rsp_s cn; */
};
typedef union bdk_dtx_l2c_mcix_bcst_rsp bdk_dtx_l2c_mcix_bcst_rsp_t;

static inline uint64_t BDK_DTX_L2C_MCIX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_L2C_MCIX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0fe2e0080ll + 0x8000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0fe2e0080ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e0fe2e0080ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_L2C_MCIX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_L2C_MCIX_BCST_RSP(a) bdk_dtx_l2c_mcix_bcst_rsp_t
#define bustype_BDK_DTX_L2C_MCIX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_L2C_MCIX_BCST_RSP(a) "DTX_L2C_MCIX_BCST_RSP"
#define busnum_BDK_DTX_L2C_MCIX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_L2C_MCIX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_l2c_mci#_ctl
 *
 * DTX L2C_MCI Control Register
 */
union bdk_dtx_l2c_mcix_ctl
{
    uint64_t u;
    struct bdk_dtx_l2c_mcix_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_l2c_mcix_ctl_s cn; */
};
typedef union bdk_dtx_l2c_mcix_ctl bdk_dtx_l2c_mcix_ctl_t;

static inline uint64_t BDK_DTX_L2C_MCIX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_L2C_MCIX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0fe2e0060ll + 0x8000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0fe2e0060ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e0fe2e0060ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_L2C_MCIX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_L2C_MCIX_CTL(a) bdk_dtx_l2c_mcix_ctl_t
#define bustype_BDK_DTX_L2C_MCIX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_L2C_MCIX_CTL(a) "DTX_L2C_MCIX_CTL"
#define busnum_BDK_DTX_L2C_MCIX_CTL(a) (a)
#define arguments_BDK_DTX_L2C_MCIX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_l2c_mci#_dat#
 *
 * DTX L2C_MCI Raw Data Register
 */
union bdk_dtx_l2c_mcix_datx
{
    uint64_t u;
    struct bdk_dtx_l2c_mcix_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_l2c_mcix_datx_s cn; */
};
typedef union bdk_dtx_l2c_mcix_datx bdk_dtx_l2c_mcix_datx_t;

static inline uint64_t BDK_DTX_L2C_MCIX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_L2C_MCIX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe2e0040ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b<=1)))
        return 0x87e0fe2e0040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe2e0040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_L2C_MCIX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_L2C_MCIX_DATX(a,b) bdk_dtx_l2c_mcix_datx_t
#define bustype_BDK_DTX_L2C_MCIX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_L2C_MCIX_DATX(a,b) "DTX_L2C_MCIX_DATX"
#define busnum_BDK_DTX_L2C_MCIX_DATX(a,b) (a)
#define arguments_BDK_DTX_L2C_MCIX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_l2c_mci#_ena#
 *
 * DTX L2C_MCI Data Enable Register
 */
union bdk_dtx_l2c_mcix_enax
{
    uint64_t u;
    struct bdk_dtx_l2c_mcix_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_l2c_mcix_enax_s cn; */
};
typedef union bdk_dtx_l2c_mcix_enax bdk_dtx_l2c_mcix_enax_t;

static inline uint64_t BDK_DTX_L2C_MCIX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_L2C_MCIX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe2e0020ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b<=1)))
        return 0x87e0fe2e0020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe2e0020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_L2C_MCIX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_L2C_MCIX_ENAX(a,b) bdk_dtx_l2c_mcix_enax_t
#define bustype_BDK_DTX_L2C_MCIX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_L2C_MCIX_ENAX(a,b) "DTX_L2C_MCIX_ENAX"
#define busnum_BDK_DTX_L2C_MCIX_ENAX(a,b) (a)
#define arguments_BDK_DTX_L2C_MCIX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_l2c_mci#_sel#
 *
 * DTX L2C_MCI Select Register
 */
union bdk_dtx_l2c_mcix_selx
{
    uint64_t u;
    struct bdk_dtx_l2c_mcix_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_l2c_mcix_selx_s cn; */
};
typedef union bdk_dtx_l2c_mcix_selx bdk_dtx_l2c_mcix_selx_t;

static inline uint64_t BDK_DTX_L2C_MCIX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_L2C_MCIX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe2e0000ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b<=1)))
        return 0x87e0fe2e0000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe2e0000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_L2C_MCIX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_L2C_MCIX_SELX(a,b) bdk_dtx_l2c_mcix_selx_t
#define bustype_BDK_DTX_L2C_MCIX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_L2C_MCIX_SELX(a,b) "DTX_L2C_MCIX_SELX"
#define busnum_BDK_DTX_L2C_MCIX_SELX(a,b) (a)
#define arguments_BDK_DTX_L2C_MCIX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_l2c_tad#_bcst_rsp
 *
 * DTX L2C_TAD Control Register
 */
union bdk_dtx_l2c_tadx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_l2c_tadx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_l2c_tadx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_l2c_tadx_bcst_rsp bdk_dtx_l2c_tadx_bcst_rsp_t;

static inline uint64_t BDK_DTX_L2C_TADX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_L2C_TADX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0fe280080ll + 0x8000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e0fe280080ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e0fe280080ll + 0x8000ll * ((a) & 0x7);
    __bdk_csr_fatal("DTX_L2C_TADX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_L2C_TADX_BCST_RSP(a) bdk_dtx_l2c_tadx_bcst_rsp_t
#define bustype_BDK_DTX_L2C_TADX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_L2C_TADX_BCST_RSP(a) "DTX_L2C_TADX_BCST_RSP"
#define busnum_BDK_DTX_L2C_TADX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_L2C_TADX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_l2c_tad#_ctl
 *
 * DTX L2C_TAD Control Register
 */
union bdk_dtx_l2c_tadx_ctl
{
    uint64_t u;
    struct bdk_dtx_l2c_tadx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_l2c_tadx_ctl_s cn; */
};
typedef union bdk_dtx_l2c_tadx_ctl bdk_dtx_l2c_tadx_ctl_t;

static inline uint64_t BDK_DTX_L2C_TADX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_L2C_TADX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0fe280060ll + 0x8000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e0fe280060ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e0fe280060ll + 0x8000ll * ((a) & 0x7);
    __bdk_csr_fatal("DTX_L2C_TADX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_L2C_TADX_CTL(a) bdk_dtx_l2c_tadx_ctl_t
#define bustype_BDK_DTX_L2C_TADX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_L2C_TADX_CTL(a) "DTX_L2C_TADX_CTL"
#define busnum_BDK_DTX_L2C_TADX_CTL(a) (a)
#define arguments_BDK_DTX_L2C_TADX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_l2c_tad#_dat#
 *
 * DTX L2C_TAD Raw Data Register
 */
union bdk_dtx_l2c_tadx_datx
{
    uint64_t u;
    struct bdk_dtx_l2c_tadx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_l2c_tadx_datx_s cn; */
};
typedef union bdk_dtx_l2c_tadx_datx bdk_dtx_l2c_tadx_datx_t;

static inline uint64_t BDK_DTX_L2C_TADX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_L2C_TADX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe280040ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe280040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=7) && (b<=1)))
        return 0x87e0fe280040ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_L2C_TADX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_L2C_TADX_DATX(a,b) bdk_dtx_l2c_tadx_datx_t
#define bustype_BDK_DTX_L2C_TADX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_L2C_TADX_DATX(a,b) "DTX_L2C_TADX_DATX"
#define busnum_BDK_DTX_L2C_TADX_DATX(a,b) (a)
#define arguments_BDK_DTX_L2C_TADX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_l2c_tad#_ena#
 *
 * DTX L2C_TAD Data Enable Register
 */
union bdk_dtx_l2c_tadx_enax
{
    uint64_t u;
    struct bdk_dtx_l2c_tadx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_l2c_tadx_enax_s cn; */
};
typedef union bdk_dtx_l2c_tadx_enax bdk_dtx_l2c_tadx_enax_t;

static inline uint64_t BDK_DTX_L2C_TADX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_L2C_TADX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe280020ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe280020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=7) && (b<=1)))
        return 0x87e0fe280020ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_L2C_TADX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_L2C_TADX_ENAX(a,b) bdk_dtx_l2c_tadx_enax_t
#define bustype_BDK_DTX_L2C_TADX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_L2C_TADX_ENAX(a,b) "DTX_L2C_TADX_ENAX"
#define busnum_BDK_DTX_L2C_TADX_ENAX(a,b) (a)
#define arguments_BDK_DTX_L2C_TADX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_l2c_tad#_sel#
 *
 * DTX L2C_TAD Select Register
 */
union bdk_dtx_l2c_tadx_selx
{
    uint64_t u;
    struct bdk_dtx_l2c_tadx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_l2c_tadx_selx_s cn; */
};
typedef union bdk_dtx_l2c_tadx_selx bdk_dtx_l2c_tadx_selx_t;

static inline uint64_t BDK_DTX_L2C_TADX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_L2C_TADX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe280000ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe280000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=7) && (b<=1)))
        return 0x87e0fe280000ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_L2C_TADX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_L2C_TADX_SELX(a,b) bdk_dtx_l2c_tadx_selx_t
#define bustype_BDK_DTX_L2C_TADX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_L2C_TADX_SELX(a,b) "DTX_L2C_TADX_SELX"
#define busnum_BDK_DTX_L2C_TADX_SELX(a,b) (a)
#define arguments_BDK_DTX_L2C_TADX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_lbk_bcst_rsp
 *
 * DTX LBK Control Register
 */
union bdk_dtx_lbk_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_lbk_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_lbk_bcst_rsp_s cn; */
};
typedef union bdk_dtx_lbk_bcst_rsp bdk_dtx_lbk_bcst_rsp_t;

#define BDK_DTX_LBK_BCST_RSP BDK_DTX_LBK_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_LBK_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_LBK_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 0x87e0fe0c0080ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0fe0c0080ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0fe0c0080ll;
    __bdk_csr_fatal("DTX_LBK_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_LBK_BCST_RSP bdk_dtx_lbk_bcst_rsp_t
#define bustype_BDK_DTX_LBK_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_LBK_BCST_RSP "DTX_LBK_BCST_RSP"
#define busnum_BDK_DTX_LBK_BCST_RSP 0
#define arguments_BDK_DTX_LBK_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_lbk_ctl
 *
 * DTX LBK Control Register
 */
union bdk_dtx_lbk_ctl
{
    uint64_t u;
    struct bdk_dtx_lbk_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_lbk_ctl_s cn; */
};
typedef union bdk_dtx_lbk_ctl bdk_dtx_lbk_ctl_t;

#define BDK_DTX_LBK_CTL BDK_DTX_LBK_CTL_FUNC()
static inline uint64_t BDK_DTX_LBK_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_LBK_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 0x87e0fe0c0060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0fe0c0060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0fe0c0060ll;
    __bdk_csr_fatal("DTX_LBK_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_LBK_CTL bdk_dtx_lbk_ctl_t
#define bustype_BDK_DTX_LBK_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_LBK_CTL "DTX_LBK_CTL"
#define busnum_BDK_DTX_LBK_CTL 0
#define arguments_BDK_DTX_LBK_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_lbk_dat#
 *
 * DTX LBK Raw Data Register
 */
union bdk_dtx_lbk_datx
{
    uint64_t u;
    struct bdk_dtx_lbk_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_lbk_datx_s cn; */
};
typedef union bdk_dtx_lbk_datx bdk_dtx_lbk_datx_t;

static inline uint64_t BDK_DTX_LBK_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_LBK_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0fe0c0040ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe0c0040ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe0c0040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_LBK_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_LBK_DATX(a) bdk_dtx_lbk_datx_t
#define bustype_BDK_DTX_LBK_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_LBK_DATX(a) "DTX_LBK_DATX"
#define busnum_BDK_DTX_LBK_DATX(a) (a)
#define arguments_BDK_DTX_LBK_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_lbk_ena#
 *
 * DTX LBK Data Enable Register
 */
union bdk_dtx_lbk_enax
{
    uint64_t u;
    struct bdk_dtx_lbk_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_lbk_enax_s cn; */
};
typedef union bdk_dtx_lbk_enax bdk_dtx_lbk_enax_t;

static inline uint64_t BDK_DTX_LBK_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_LBK_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0fe0c0020ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe0c0020ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe0c0020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_LBK_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_LBK_ENAX(a) bdk_dtx_lbk_enax_t
#define bustype_BDK_DTX_LBK_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_LBK_ENAX(a) "DTX_LBK_ENAX"
#define busnum_BDK_DTX_LBK_ENAX(a) (a)
#define arguments_BDK_DTX_LBK_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_lbk_sel#
 *
 * DTX LBK Select Register
 */
union bdk_dtx_lbk_selx
{
    uint64_t u;
    struct bdk_dtx_lbk_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_lbk_selx_s cn; */
};
typedef union bdk_dtx_lbk_selx bdk_dtx_lbk_selx_t;

static inline uint64_t BDK_DTX_LBK_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_LBK_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0fe0c0000ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe0c0000ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe0c0000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_LBK_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_LBK_SELX(a) bdk_dtx_lbk_selx_t
#define bustype_BDK_DTX_LBK_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_LBK_SELX(a) "DTX_LBK_SELX"
#define busnum_BDK_DTX_LBK_SELX(a) (a)
#define arguments_BDK_DTX_LBK_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_lmc#_bcst_rsp
 *
 * DTX LMC Control Register
 */
union bdk_dtx_lmcx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_lmcx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_lmcx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_lmcx_bcst_rsp bdk_dtx_lmcx_bcst_rsp_t;

static inline uint64_t BDK_DTX_LMCX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_LMCX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0fe440080ll + 0x8000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe440080ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e0fe440080ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe440080ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_LMCX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_LMCX_BCST_RSP(a) bdk_dtx_lmcx_bcst_rsp_t
#define bustype_BDK_DTX_LMCX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_LMCX_BCST_RSP(a) "DTX_LMCX_BCST_RSP"
#define busnum_BDK_DTX_LMCX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_LMCX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_lmc#_ctl
 *
 * DTX LMC Control Register
 */
union bdk_dtx_lmcx_ctl
{
    uint64_t u;
    struct bdk_dtx_lmcx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_lmcx_ctl_s cn; */
};
typedef union bdk_dtx_lmcx_ctl bdk_dtx_lmcx_ctl_t;

static inline uint64_t BDK_DTX_LMCX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_LMCX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0fe440060ll + 0x8000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe440060ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=3))
        return 0x87e0fe440060ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe440060ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_LMCX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_LMCX_CTL(a) bdk_dtx_lmcx_ctl_t
#define bustype_BDK_DTX_LMCX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_LMCX_CTL(a) "DTX_LMCX_CTL"
#define busnum_BDK_DTX_LMCX_CTL(a) (a)
#define arguments_BDK_DTX_LMCX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_lmc#_dat#
 *
 * DTX LMC Raw Data Register
 */
union bdk_dtx_lmcx_datx
{
    uint64_t u;
    struct bdk_dtx_lmcx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_lmcx_datx_s cn; */
};
typedef union bdk_dtx_lmcx_datx bdk_dtx_lmcx_datx_t;

static inline uint64_t BDK_DTX_LMCX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_LMCX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe440040ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe440040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe440040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe440040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_LMCX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_LMCX_DATX(a,b) bdk_dtx_lmcx_datx_t
#define bustype_BDK_DTX_LMCX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_LMCX_DATX(a,b) "DTX_LMCX_DATX"
#define busnum_BDK_DTX_LMCX_DATX(a,b) (a)
#define arguments_BDK_DTX_LMCX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_lmc#_ena#
 *
 * DTX LMC Data Enable Register
 */
union bdk_dtx_lmcx_enax
{
    uint64_t u;
    struct bdk_dtx_lmcx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_lmcx_enax_s cn; */
};
typedef union bdk_dtx_lmcx_enax bdk_dtx_lmcx_enax_t;

static inline uint64_t BDK_DTX_LMCX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_LMCX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe440020ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe440020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe440020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe440020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_LMCX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_LMCX_ENAX(a,b) bdk_dtx_lmcx_enax_t
#define bustype_BDK_DTX_LMCX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_LMCX_ENAX(a,b) "DTX_LMCX_ENAX"
#define busnum_BDK_DTX_LMCX_ENAX(a,b) (a)
#define arguments_BDK_DTX_LMCX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_lmc#_sel#
 *
 * DTX LMC Select Register
 */
union bdk_dtx_lmcx_selx
{
    uint64_t u;
    struct bdk_dtx_lmcx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_lmcx_selx_s cn; */
};
typedef union bdk_dtx_lmcx_selx bdk_dtx_lmcx_selx_t;

static inline uint64_t BDK_DTX_LMCX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_LMCX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe440000ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe440000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe440000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe440000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_LMCX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_LMCX_SELX(a,b) bdk_dtx_lmcx_selx_t
#define bustype_BDK_DTX_LMCX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_LMCX_SELX(a,b) "DTX_LMCX_SELX"
#define busnum_BDK_DTX_LMCX_SELX(a,b) (a)
#define arguments_BDK_DTX_LMCX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_mcc#_bcst_rsp
 *
 * DTX MCC Control Register
 */
union bdk_dtx_mccx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_mccx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mccx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_mccx_bcst_rsp bdk_dtx_mccx_bcst_rsp_t;

static inline uint64_t BDK_DTX_MCCX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MCCX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe1e0080ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_MCCX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_MCCX_BCST_RSP(a) bdk_dtx_mccx_bcst_rsp_t
#define bustype_BDK_DTX_MCCX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MCCX_BCST_RSP(a) "DTX_MCCX_BCST_RSP"
#define busnum_BDK_DTX_MCCX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_MCCX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_mcc#_ctl
 *
 * DTX MCC Control Register
 */
union bdk_dtx_mccx_ctl
{
    uint64_t u;
    struct bdk_dtx_mccx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mccx_ctl_s cn; */
};
typedef union bdk_dtx_mccx_ctl bdk_dtx_mccx_ctl_t;

static inline uint64_t BDK_DTX_MCCX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MCCX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe1e0060ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_MCCX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_MCCX_CTL(a) bdk_dtx_mccx_ctl_t
#define bustype_BDK_DTX_MCCX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MCCX_CTL(a) "DTX_MCCX_CTL"
#define busnum_BDK_DTX_MCCX_CTL(a) (a)
#define arguments_BDK_DTX_MCCX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_mcc#_dat#
 *
 * DTX MCC Raw Data Register
 */
union bdk_dtx_mccx_datx
{
    uint64_t u;
    struct bdk_dtx_mccx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mccx_datx_s cn; */
};
typedef union bdk_dtx_mccx_datx bdk_dtx_mccx_datx_t;

static inline uint64_t BDK_DTX_MCCX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MCCX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe1e0040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_MCCX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_MCCX_DATX(a,b) bdk_dtx_mccx_datx_t
#define bustype_BDK_DTX_MCCX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MCCX_DATX(a,b) "DTX_MCCX_DATX"
#define busnum_BDK_DTX_MCCX_DATX(a,b) (a)
#define arguments_BDK_DTX_MCCX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_mcc#_ena#
 *
 * DTX MCC Data Enable Register
 */
union bdk_dtx_mccx_enax
{
    uint64_t u;
    struct bdk_dtx_mccx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mccx_enax_s cn; */
};
typedef union bdk_dtx_mccx_enax bdk_dtx_mccx_enax_t;

static inline uint64_t BDK_DTX_MCCX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MCCX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe1e0020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_MCCX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_MCCX_ENAX(a,b) bdk_dtx_mccx_enax_t
#define bustype_BDK_DTX_MCCX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MCCX_ENAX(a,b) "DTX_MCCX_ENAX"
#define busnum_BDK_DTX_MCCX_ENAX(a,b) (a)
#define arguments_BDK_DTX_MCCX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_mcc#_mci#_bcst_rsp
 *
 * DTX MCC_MCI Control Register
 */
union bdk_dtx_mccx_mcix_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_mccx_mcix_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mccx_mcix_bcst_rsp_s cn; */
};
typedef union bdk_dtx_mccx_mcix_bcst_rsp bdk_dtx_mccx_mcix_bcst_rsp_t;

static inline uint64_t BDK_DTX_MCCX_MCIX_BCST_RSP(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MCCX_MCIX_BCST_RSP(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fec00080ll + 0x18000ll * ((a) & 0x1) + 0x8000ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_MCCX_MCIX_BCST_RSP", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_MCCX_MCIX_BCST_RSP(a,b) bdk_dtx_mccx_mcix_bcst_rsp_t
#define bustype_BDK_DTX_MCCX_MCIX_BCST_RSP(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MCCX_MCIX_BCST_RSP(a,b) "DTX_MCCX_MCIX_BCST_RSP"
#define busnum_BDK_DTX_MCCX_MCIX_BCST_RSP(a,b) (a)
#define arguments_BDK_DTX_MCCX_MCIX_BCST_RSP(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_mcc#_mci#_ctl
 *
 * DTX MCC_MCI Control Register
 */
union bdk_dtx_mccx_mcix_ctl
{
    uint64_t u;
    struct bdk_dtx_mccx_mcix_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mccx_mcix_ctl_s cn; */
};
typedef union bdk_dtx_mccx_mcix_ctl bdk_dtx_mccx_mcix_ctl_t;

static inline uint64_t BDK_DTX_MCCX_MCIX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MCCX_MCIX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fec00060ll + 0x18000ll * ((a) & 0x1) + 0x8000ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_MCCX_MCIX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_MCCX_MCIX_CTL(a,b) bdk_dtx_mccx_mcix_ctl_t
#define bustype_BDK_DTX_MCCX_MCIX_CTL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MCCX_MCIX_CTL(a,b) "DTX_MCCX_MCIX_CTL"
#define busnum_BDK_DTX_MCCX_MCIX_CTL(a,b) (a)
#define arguments_BDK_DTX_MCCX_MCIX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_mcc#_mci#_dat#
 *
 * DTX MCC_MCI Raw Data Register
 */
union bdk_dtx_mccx_mcix_datx
{
    uint64_t u;
    struct bdk_dtx_mccx_mcix_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mccx_mcix_datx_s cn; */
};
typedef union bdk_dtx_mccx_mcix_datx bdk_dtx_mccx_mcix_datx_t;

static inline uint64_t BDK_DTX_MCCX_MCIX_DATX(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MCCX_MCIX_DATX(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1) && (c<=1)))
        return 0x87e0fec00040ll + 0x18000ll * ((a) & 0x1) + 0x8000ll * ((b) & 0x1) + 8ll * ((c) & 0x1);
    __bdk_csr_fatal("DTX_MCCX_MCIX_DATX", 3, a, b, c, 0);
}

#define typedef_BDK_DTX_MCCX_MCIX_DATX(a,b,c) bdk_dtx_mccx_mcix_datx_t
#define bustype_BDK_DTX_MCCX_MCIX_DATX(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MCCX_MCIX_DATX(a,b,c) "DTX_MCCX_MCIX_DATX"
#define busnum_BDK_DTX_MCCX_MCIX_DATX(a,b,c) (a)
#define arguments_BDK_DTX_MCCX_MCIX_DATX(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) dtx_mcc#_mci#_ena#
 *
 * DTX MCC_MCI Data Enable Register
 */
union bdk_dtx_mccx_mcix_enax
{
    uint64_t u;
    struct bdk_dtx_mccx_mcix_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mccx_mcix_enax_s cn; */
};
typedef union bdk_dtx_mccx_mcix_enax bdk_dtx_mccx_mcix_enax_t;

static inline uint64_t BDK_DTX_MCCX_MCIX_ENAX(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MCCX_MCIX_ENAX(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1) && (c<=1)))
        return 0x87e0fec00020ll + 0x18000ll * ((a) & 0x1) + 0x8000ll * ((b) & 0x1) + 8ll * ((c) & 0x1);
    __bdk_csr_fatal("DTX_MCCX_MCIX_ENAX", 3, a, b, c, 0);
}

#define typedef_BDK_DTX_MCCX_MCIX_ENAX(a,b,c) bdk_dtx_mccx_mcix_enax_t
#define bustype_BDK_DTX_MCCX_MCIX_ENAX(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MCCX_MCIX_ENAX(a,b,c) "DTX_MCCX_MCIX_ENAX"
#define busnum_BDK_DTX_MCCX_MCIX_ENAX(a,b,c) (a)
#define arguments_BDK_DTX_MCCX_MCIX_ENAX(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) dtx_mcc#_mci#_sel#
 *
 * DTX MCC_MCI Select Register
 */
union bdk_dtx_mccx_mcix_selx
{
    uint64_t u;
    struct bdk_dtx_mccx_mcix_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mccx_mcix_selx_s cn; */
};
typedef union bdk_dtx_mccx_mcix_selx bdk_dtx_mccx_mcix_selx_t;

static inline uint64_t BDK_DTX_MCCX_MCIX_SELX(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MCCX_MCIX_SELX(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1) && (c<=1)))
        return 0x87e0fec00000ll + 0x18000ll * ((a) & 0x1) + 0x8000ll * ((b) & 0x1) + 8ll * ((c) & 0x1);
    __bdk_csr_fatal("DTX_MCCX_MCIX_SELX", 3, a, b, c, 0);
}

#define typedef_BDK_DTX_MCCX_MCIX_SELX(a,b,c) bdk_dtx_mccx_mcix_selx_t
#define bustype_BDK_DTX_MCCX_MCIX_SELX(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MCCX_MCIX_SELX(a,b,c) "DTX_MCCX_MCIX_SELX"
#define busnum_BDK_DTX_MCCX_MCIX_SELX(a,b,c) (a)
#define arguments_BDK_DTX_MCCX_MCIX_SELX(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) dtx_mcc#_sel#
 *
 * DTX MCC Select Register
 */
union bdk_dtx_mccx_selx
{
    uint64_t u;
    struct bdk_dtx_mccx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mccx_selx_s cn; */
};
typedef union bdk_dtx_mccx_selx bdk_dtx_mccx_selx_t;

static inline uint64_t BDK_DTX_MCCX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MCCX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe1e0000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_MCCX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_MCCX_SELX(a,b) bdk_dtx_mccx_selx_t
#define bustype_BDK_DTX_MCCX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MCCX_SELX(a,b) "DTX_MCCX_SELX"
#define busnum_BDK_DTX_MCCX_SELX(a,b) (a)
#define arguments_BDK_DTX_MCCX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_mio_bcst_rsp
 *
 * DTX MIO Control Register
 */
union bdk_dtx_mio_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_mio_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mio_bcst_rsp_s cn; */
};
typedef union bdk_dtx_mio_bcst_rsp bdk_dtx_mio_bcst_rsp_t;

#define BDK_DTX_MIO_BCST_RSP BDK_DTX_MIO_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_MIO_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MIO_BCST_RSP_FUNC(void)
{
    return 0x87e0fe000080ll;
}

#define typedef_BDK_DTX_MIO_BCST_RSP bdk_dtx_mio_bcst_rsp_t
#define bustype_BDK_DTX_MIO_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MIO_BCST_RSP "DTX_MIO_BCST_RSP"
#define busnum_BDK_DTX_MIO_BCST_RSP 0
#define arguments_BDK_DTX_MIO_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_mio_ctl
 *
 * DTX MIO Control Register
 */
union bdk_dtx_mio_ctl
{
    uint64_t u;
    struct bdk_dtx_mio_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mio_ctl_s cn; */
};
typedef union bdk_dtx_mio_ctl bdk_dtx_mio_ctl_t;

#define BDK_DTX_MIO_CTL BDK_DTX_MIO_CTL_FUNC()
static inline uint64_t BDK_DTX_MIO_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MIO_CTL_FUNC(void)
{
    return 0x87e0fe000060ll;
}

#define typedef_BDK_DTX_MIO_CTL bdk_dtx_mio_ctl_t
#define bustype_BDK_DTX_MIO_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MIO_CTL "DTX_MIO_CTL"
#define busnum_BDK_DTX_MIO_CTL 0
#define arguments_BDK_DTX_MIO_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_mio_dat#
 *
 * DTX MIO Raw Data Register
 */
union bdk_dtx_mio_datx
{
    uint64_t u;
    struct bdk_dtx_mio_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mio_datx_s cn; */
};
typedef union bdk_dtx_mio_datx bdk_dtx_mio_datx_t;

static inline uint64_t BDK_DTX_MIO_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MIO_DATX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe000040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_MIO_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_MIO_DATX(a) bdk_dtx_mio_datx_t
#define bustype_BDK_DTX_MIO_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MIO_DATX(a) "DTX_MIO_DATX"
#define busnum_BDK_DTX_MIO_DATX(a) (a)
#define arguments_BDK_DTX_MIO_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_mio_ena#
 *
 * DTX MIO Data Enable Register
 */
union bdk_dtx_mio_enax
{
    uint64_t u;
    struct bdk_dtx_mio_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mio_enax_s cn; */
};
typedef union bdk_dtx_mio_enax bdk_dtx_mio_enax_t;

static inline uint64_t BDK_DTX_MIO_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MIO_ENAX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe000020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_MIO_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_MIO_ENAX(a) bdk_dtx_mio_enax_t
#define bustype_BDK_DTX_MIO_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MIO_ENAX(a) "DTX_MIO_ENAX"
#define busnum_BDK_DTX_MIO_ENAX(a) (a)
#define arguments_BDK_DTX_MIO_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_mio_sel#
 *
 * DTX MIO Select Register
 */
union bdk_dtx_mio_selx
{
    uint64_t u;
    struct bdk_dtx_mio_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mio_selx_s cn; */
};
typedef union bdk_dtx_mio_selx bdk_dtx_mio_selx_t;

static inline uint64_t BDK_DTX_MIO_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MIO_SELX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe000000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_MIO_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_MIO_SELX(a) bdk_dtx_mio_selx_t
#define bustype_BDK_DTX_MIO_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MIO_SELX(a) "DTX_MIO_SELX"
#define busnum_BDK_DTX_MIO_SELX(a) (a)
#define arguments_BDK_DTX_MIO_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_mpi#_bcst_rsp
 *
 * DTX MPI Control Register
 */
union bdk_dtx_mpix_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_mpix_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mpix_bcst_rsp_s cn; */
};
typedef union bdk_dtx_mpix_bcst_rsp bdk_dtx_mpix_bcst_rsp_t;

static inline uint64_t BDK_DTX_MPIX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MPIX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe820080ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_MPIX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_MPIX_BCST_RSP(a) bdk_dtx_mpix_bcst_rsp_t
#define bustype_BDK_DTX_MPIX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MPIX_BCST_RSP(a) "DTX_MPIX_BCST_RSP"
#define busnum_BDK_DTX_MPIX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_MPIX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_mpi#_ctl
 *
 * DTX MPI Control Register
 */
union bdk_dtx_mpix_ctl
{
    uint64_t u;
    struct bdk_dtx_mpix_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mpix_ctl_s cn; */
};
typedef union bdk_dtx_mpix_ctl bdk_dtx_mpix_ctl_t;

static inline uint64_t BDK_DTX_MPIX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MPIX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe820060ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_MPIX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_MPIX_CTL(a) bdk_dtx_mpix_ctl_t
#define bustype_BDK_DTX_MPIX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MPIX_CTL(a) "DTX_MPIX_CTL"
#define busnum_BDK_DTX_MPIX_CTL(a) (a)
#define arguments_BDK_DTX_MPIX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_mpi#_dat#
 *
 * DTX MPI Raw Data Register
 */
union bdk_dtx_mpix_datx
{
    uint64_t u;
    struct bdk_dtx_mpix_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mpix_datx_s cn; */
};
typedef union bdk_dtx_mpix_datx bdk_dtx_mpix_datx_t;

static inline uint64_t BDK_DTX_MPIX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MPIX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe820040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_MPIX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_MPIX_DATX(a,b) bdk_dtx_mpix_datx_t
#define bustype_BDK_DTX_MPIX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MPIX_DATX(a,b) "DTX_MPIX_DATX"
#define busnum_BDK_DTX_MPIX_DATX(a,b) (a)
#define arguments_BDK_DTX_MPIX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_mpi#_ena#
 *
 * DTX MPI Data Enable Register
 */
union bdk_dtx_mpix_enax
{
    uint64_t u;
    struct bdk_dtx_mpix_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mpix_enax_s cn; */
};
typedef union bdk_dtx_mpix_enax bdk_dtx_mpix_enax_t;

static inline uint64_t BDK_DTX_MPIX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MPIX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe820020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_MPIX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_MPIX_ENAX(a,b) bdk_dtx_mpix_enax_t
#define bustype_BDK_DTX_MPIX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MPIX_ENAX(a,b) "DTX_MPIX_ENAX"
#define busnum_BDK_DTX_MPIX_ENAX(a,b) (a)
#define arguments_BDK_DTX_MPIX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_mpi#_sel#
 *
 * DTX MPI Select Register
 */
union bdk_dtx_mpix_selx
{
    uint64_t u;
    struct bdk_dtx_mpix_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mpix_selx_s cn; */
};
typedef union bdk_dtx_mpix_selx bdk_dtx_mpix_selx_t;

static inline uint64_t BDK_DTX_MPIX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MPIX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe820000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_MPIX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_MPIX_SELX(a,b) bdk_dtx_mpix_selx_t
#define bustype_BDK_DTX_MPIX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MPIX_SELX(a,b) "DTX_MPIX_SELX"
#define busnum_BDK_DTX_MPIX_SELX(a,b) (a)
#define arguments_BDK_DTX_MPIX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_mrml_bcst_rsp
 *
 * DTX MRML Control Register
 */
union bdk_dtx_mrml_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_mrml_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mrml_bcst_rsp_s cn; */
};
typedef union bdk_dtx_mrml_bcst_rsp bdk_dtx_mrml_bcst_rsp_t;

#define BDK_DTX_MRML_BCST_RSP BDK_DTX_MRML_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_MRML_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MRML_BCST_RSP_FUNC(void)
{
    return 0x87e0fe7e0080ll;
}

#define typedef_BDK_DTX_MRML_BCST_RSP bdk_dtx_mrml_bcst_rsp_t
#define bustype_BDK_DTX_MRML_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MRML_BCST_RSP "DTX_MRML_BCST_RSP"
#define busnum_BDK_DTX_MRML_BCST_RSP 0
#define arguments_BDK_DTX_MRML_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_mrml_ctl
 *
 * DTX MRML Control Register
 */
union bdk_dtx_mrml_ctl
{
    uint64_t u;
    struct bdk_dtx_mrml_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mrml_ctl_s cn; */
};
typedef union bdk_dtx_mrml_ctl bdk_dtx_mrml_ctl_t;

#define BDK_DTX_MRML_CTL BDK_DTX_MRML_CTL_FUNC()
static inline uint64_t BDK_DTX_MRML_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MRML_CTL_FUNC(void)
{
    return 0x87e0fe7e0060ll;
}

#define typedef_BDK_DTX_MRML_CTL bdk_dtx_mrml_ctl_t
#define bustype_BDK_DTX_MRML_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MRML_CTL "DTX_MRML_CTL"
#define busnum_BDK_DTX_MRML_CTL 0
#define arguments_BDK_DTX_MRML_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_mrml_dat#
 *
 * DTX MRML Raw Data Register
 */
union bdk_dtx_mrml_datx
{
    uint64_t u;
    struct bdk_dtx_mrml_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mrml_datx_s cn; */
};
typedef union bdk_dtx_mrml_datx bdk_dtx_mrml_datx_t;

static inline uint64_t BDK_DTX_MRML_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MRML_DATX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe7e0040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_MRML_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_MRML_DATX(a) bdk_dtx_mrml_datx_t
#define bustype_BDK_DTX_MRML_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MRML_DATX(a) "DTX_MRML_DATX"
#define busnum_BDK_DTX_MRML_DATX(a) (a)
#define arguments_BDK_DTX_MRML_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_mrml_ena#
 *
 * DTX MRML Data Enable Register
 */
union bdk_dtx_mrml_enax
{
    uint64_t u;
    struct bdk_dtx_mrml_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mrml_enax_s cn; */
};
typedef union bdk_dtx_mrml_enax bdk_dtx_mrml_enax_t;

static inline uint64_t BDK_DTX_MRML_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MRML_ENAX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe7e0020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_MRML_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_MRML_ENAX(a) bdk_dtx_mrml_enax_t
#define bustype_BDK_DTX_MRML_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MRML_ENAX(a) "DTX_MRML_ENAX"
#define busnum_BDK_DTX_MRML_ENAX(a) (a)
#define arguments_BDK_DTX_MRML_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_mrml_sel#
 *
 * DTX MRML Select Register
 */
union bdk_dtx_mrml_selx
{
    uint64_t u;
    struct bdk_dtx_mrml_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_mrml_selx_s cn; */
};
typedef union bdk_dtx_mrml_selx bdk_dtx_mrml_selx_t;

static inline uint64_t BDK_DTX_MRML_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_MRML_SELX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe7e0000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_MRML_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_MRML_SELX(a) bdk_dtx_mrml_selx_t
#define bustype_BDK_DTX_MRML_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_MRML_SELX(a) "DTX_MRML_SELX"
#define busnum_BDK_DTX_MRML_SELX(a) (a)
#define arguments_BDK_DTX_MRML_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ncsi_bcst_rsp
 *
 * DTX NCSI Control Register
 */
union bdk_dtx_ncsi_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_ncsi_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ncsi_bcst_rsp_s cn; */
};
typedef union bdk_dtx_ncsi_bcst_rsp bdk_dtx_ncsi_bcst_rsp_t;

#define BDK_DTX_NCSI_BCST_RSP BDK_DTX_NCSI_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_NCSI_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_NCSI_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0fe058080ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0fe058080ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0fe058080ll;
    __bdk_csr_fatal("DTX_NCSI_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_NCSI_BCST_RSP bdk_dtx_ncsi_bcst_rsp_t
#define bustype_BDK_DTX_NCSI_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_NCSI_BCST_RSP "DTX_NCSI_BCST_RSP"
#define busnum_BDK_DTX_NCSI_BCST_RSP 0
#define arguments_BDK_DTX_NCSI_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_ncsi_ctl
 *
 * DTX NCSI Control Register
 */
union bdk_dtx_ncsi_ctl
{
    uint64_t u;
    struct bdk_dtx_ncsi_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ncsi_ctl_s cn; */
};
typedef union bdk_dtx_ncsi_ctl bdk_dtx_ncsi_ctl_t;

#define BDK_DTX_NCSI_CTL BDK_DTX_NCSI_CTL_FUNC()
static inline uint64_t BDK_DTX_NCSI_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_NCSI_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0fe058060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0fe058060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0fe058060ll;
    __bdk_csr_fatal("DTX_NCSI_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_NCSI_CTL bdk_dtx_ncsi_ctl_t
#define bustype_BDK_DTX_NCSI_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_NCSI_CTL "DTX_NCSI_CTL"
#define busnum_BDK_DTX_NCSI_CTL 0
#define arguments_BDK_DTX_NCSI_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_ncsi_dat#
 *
 * DTX NCSI Raw Data Register
 */
union bdk_dtx_ncsi_datx
{
    uint64_t u;
    struct bdk_dtx_ncsi_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ncsi_datx_s cn; */
};
typedef union bdk_dtx_ncsi_datx bdk_dtx_ncsi_datx_t;

static inline uint64_t BDK_DTX_NCSI_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_NCSI_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe058040ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe058040ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe058040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_NCSI_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_NCSI_DATX(a) bdk_dtx_ncsi_datx_t
#define bustype_BDK_DTX_NCSI_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_NCSI_DATX(a) "DTX_NCSI_DATX"
#define busnum_BDK_DTX_NCSI_DATX(a) (a)
#define arguments_BDK_DTX_NCSI_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ncsi_ena#
 *
 * DTX NCSI Data Enable Register
 */
union bdk_dtx_ncsi_enax
{
    uint64_t u;
    struct bdk_dtx_ncsi_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ncsi_enax_s cn; */
};
typedef union bdk_dtx_ncsi_enax bdk_dtx_ncsi_enax_t;

static inline uint64_t BDK_DTX_NCSI_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_NCSI_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe058020ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe058020ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe058020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_NCSI_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_NCSI_ENAX(a) bdk_dtx_ncsi_enax_t
#define bustype_BDK_DTX_NCSI_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_NCSI_ENAX(a) "DTX_NCSI_ENAX"
#define busnum_BDK_DTX_NCSI_ENAX(a) (a)
#define arguments_BDK_DTX_NCSI_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ncsi_sel#
 *
 * DTX NCSI Select Register
 */
union bdk_dtx_ncsi_selx
{
    uint64_t u;
    struct bdk_dtx_ncsi_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ncsi_selx_s cn; */
};
typedef union bdk_dtx_ncsi_selx bdk_dtx_ncsi_selx_t;

static inline uint64_t BDK_DTX_NCSI_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_NCSI_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe058000ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe058000ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe058000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_NCSI_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_NCSI_SELX(a) bdk_dtx_ncsi_selx_t
#define bustype_BDK_DTX_NCSI_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_NCSI_SELX(a) "DTX_NCSI_SELX"
#define busnum_BDK_DTX_NCSI_SELX(a) (a)
#define arguments_BDK_DTX_NCSI_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_nic_bcst_rsp
 *
 * DTX NIC Control Register
 */
union bdk_dtx_nic_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_nic_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_nic_bcst_rsp_s cn; */
};
typedef union bdk_dtx_nic_bcst_rsp bdk_dtx_nic_bcst_rsp_t;

#define BDK_DTX_NIC_BCST_RSP BDK_DTX_NIC_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_NIC_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_NIC_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e0fea18080ll;
    __bdk_csr_fatal("DTX_NIC_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_NIC_BCST_RSP bdk_dtx_nic_bcst_rsp_t
#define bustype_BDK_DTX_NIC_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_NIC_BCST_RSP "DTX_NIC_BCST_RSP"
#define busnum_BDK_DTX_NIC_BCST_RSP 0
#define arguments_BDK_DTX_NIC_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_nic_ctl
 *
 * DTX NIC Control Register
 */
union bdk_dtx_nic_ctl
{
    uint64_t u;
    struct bdk_dtx_nic_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_nic_ctl_s cn; */
};
typedef union bdk_dtx_nic_ctl bdk_dtx_nic_ctl_t;

#define BDK_DTX_NIC_CTL BDK_DTX_NIC_CTL_FUNC()
static inline uint64_t BDK_DTX_NIC_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_NIC_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e0fea18060ll;
    __bdk_csr_fatal("DTX_NIC_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_NIC_CTL bdk_dtx_nic_ctl_t
#define bustype_BDK_DTX_NIC_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_NIC_CTL "DTX_NIC_CTL"
#define busnum_BDK_DTX_NIC_CTL 0
#define arguments_BDK_DTX_NIC_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_nic_dat#
 *
 * DTX NIC Raw Data Register
 */
union bdk_dtx_nic_datx
{
    uint64_t u;
    struct bdk_dtx_nic_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_nic_datx_s cn; */
};
typedef union bdk_dtx_nic_datx bdk_dtx_nic_datx_t;

static inline uint64_t BDK_DTX_NIC_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_NIC_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX) && (a<=1))
        return 0x87e0fea18040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_NIC_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_NIC_DATX(a) bdk_dtx_nic_datx_t
#define bustype_BDK_DTX_NIC_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_NIC_DATX(a) "DTX_NIC_DATX"
#define busnum_BDK_DTX_NIC_DATX(a) (a)
#define arguments_BDK_DTX_NIC_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_nic_ena#
 *
 * DTX NIC Data Enable Register
 */
union bdk_dtx_nic_enax
{
    uint64_t u;
    struct bdk_dtx_nic_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_nic_enax_s cn; */
};
typedef union bdk_dtx_nic_enax bdk_dtx_nic_enax_t;

static inline uint64_t BDK_DTX_NIC_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_NIC_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX) && (a<=1))
        return 0x87e0fea18020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_NIC_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_NIC_ENAX(a) bdk_dtx_nic_enax_t
#define bustype_BDK_DTX_NIC_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_NIC_ENAX(a) "DTX_NIC_ENAX"
#define busnum_BDK_DTX_NIC_ENAX(a) (a)
#define arguments_BDK_DTX_NIC_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_nic_sel#
 *
 * DTX NIC Select Register
 */
union bdk_dtx_nic_selx
{
    uint64_t u;
    struct bdk_dtx_nic_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_nic_selx_s cn; */
};
typedef union bdk_dtx_nic_selx bdk_dtx_nic_selx_t;

static inline uint64_t BDK_DTX_NIC_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_NIC_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX) && (a<=1))
        return 0x87e0fea18000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_NIC_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_NIC_SELX(a) bdk_dtx_nic_selx_t
#define bustype_BDK_DTX_NIC_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_NIC_SELX(a) "DTX_NIC_SELX"
#define busnum_BDK_DTX_NIC_SELX(a) (a)
#define arguments_BDK_DTX_NIC_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_npc_bcst_rsp
 *
 * DTX NPC Control Register
 */
union bdk_dtx_npc_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_npc_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_npc_bcst_rsp_s cn; */
};
typedef union bdk_dtx_npc_bcst_rsp bdk_dtx_npc_bcst_rsp_t;

#define BDK_DTX_NPC_BCST_RSP BDK_DTX_NPC_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_NPC_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_NPC_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0fe2d0080ll;
    __bdk_csr_fatal("DTX_NPC_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_NPC_BCST_RSP bdk_dtx_npc_bcst_rsp_t
#define bustype_BDK_DTX_NPC_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_NPC_BCST_RSP "DTX_NPC_BCST_RSP"
#define busnum_BDK_DTX_NPC_BCST_RSP 0
#define arguments_BDK_DTX_NPC_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_npc_ctl
 *
 * DTX NPC Control Register
 */
union bdk_dtx_npc_ctl
{
    uint64_t u;
    struct bdk_dtx_npc_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_npc_ctl_s cn; */
};
typedef union bdk_dtx_npc_ctl bdk_dtx_npc_ctl_t;

#define BDK_DTX_NPC_CTL BDK_DTX_NPC_CTL_FUNC()
static inline uint64_t BDK_DTX_NPC_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_NPC_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0fe2d0060ll;
    __bdk_csr_fatal("DTX_NPC_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_NPC_CTL bdk_dtx_npc_ctl_t
#define bustype_BDK_DTX_NPC_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_NPC_CTL "DTX_NPC_CTL"
#define busnum_BDK_DTX_NPC_CTL 0
#define arguments_BDK_DTX_NPC_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_npc_dat#
 *
 * DTX NPC Raw Data Register
 */
union bdk_dtx_npc_datx
{
    uint64_t u;
    struct bdk_dtx_npc_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_npc_datx_s cn; */
};
typedef union bdk_dtx_npc_datx bdk_dtx_npc_datx_t;

static inline uint64_t BDK_DTX_NPC_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_NPC_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe2d0040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_NPC_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_NPC_DATX(a) bdk_dtx_npc_datx_t
#define bustype_BDK_DTX_NPC_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_NPC_DATX(a) "DTX_NPC_DATX"
#define busnum_BDK_DTX_NPC_DATX(a) (a)
#define arguments_BDK_DTX_NPC_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_npc_ena#
 *
 * DTX NPC Data Enable Register
 */
union bdk_dtx_npc_enax
{
    uint64_t u;
    struct bdk_dtx_npc_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_npc_enax_s cn; */
};
typedef union bdk_dtx_npc_enax bdk_dtx_npc_enax_t;

static inline uint64_t BDK_DTX_NPC_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_NPC_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe2d0020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_NPC_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_NPC_ENAX(a) bdk_dtx_npc_enax_t
#define bustype_BDK_DTX_NPC_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_NPC_ENAX(a) "DTX_NPC_ENAX"
#define busnum_BDK_DTX_NPC_ENAX(a) (a)
#define arguments_BDK_DTX_NPC_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_npc_sel#
 *
 * DTX NPC Select Register
 */
union bdk_dtx_npc_selx
{
    uint64_t u;
    struct bdk_dtx_npc_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_npc_selx_s cn; */
};
typedef union bdk_dtx_npc_selx bdk_dtx_npc_selx_t;

static inline uint64_t BDK_DTX_NPC_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_NPC_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe2d0000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_NPC_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_NPC_SELX(a) bdk_dtx_npc_selx_t
#define bustype_BDK_DTX_NPC_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_NPC_SELX(a) "DTX_NPC_SELX"
#define busnum_BDK_DTX_NPC_SELX(a) (a)
#define arguments_BDK_DTX_NPC_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ocx_bot_bcst_rsp
 *
 * INTERNAL: DTX OCX_BOT Control Register
 */
union bdk_dtx_ocx_bot_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_ocx_bot_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_bot_bcst_rsp_s cn; */
};
typedef union bdk_dtx_ocx_bot_bcst_rsp bdk_dtx_ocx_bot_bcst_rsp_t;

#define BDK_DTX_OCX_BOT_BCST_RSP BDK_DTX_OCX_BOT_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_OCX_BOT_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_BOT_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0fe198080ll;
    __bdk_csr_fatal("DTX_OCX_BOT_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_OCX_BOT_BCST_RSP bdk_dtx_ocx_bot_bcst_rsp_t
#define bustype_BDK_DTX_OCX_BOT_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_BOT_BCST_RSP "DTX_OCX_BOT_BCST_RSP"
#define busnum_BDK_DTX_OCX_BOT_BCST_RSP 0
#define arguments_BDK_DTX_OCX_BOT_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_ocx_bot_ctl
 *
 * INTERNAL: DTX OCX_BOT Control Register
 */
union bdk_dtx_ocx_bot_ctl
{
    uint64_t u;
    struct bdk_dtx_ocx_bot_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_bot_ctl_s cn; */
};
typedef union bdk_dtx_ocx_bot_ctl bdk_dtx_ocx_bot_ctl_t;

#define BDK_DTX_OCX_BOT_CTL BDK_DTX_OCX_BOT_CTL_FUNC()
static inline uint64_t BDK_DTX_OCX_BOT_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_BOT_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0fe198060ll;
    __bdk_csr_fatal("DTX_OCX_BOT_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_OCX_BOT_CTL bdk_dtx_ocx_bot_ctl_t
#define bustype_BDK_DTX_OCX_BOT_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_BOT_CTL "DTX_OCX_BOT_CTL"
#define busnum_BDK_DTX_OCX_BOT_CTL 0
#define arguments_BDK_DTX_OCX_BOT_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_ocx_bot_dat#
 *
 * INTERNAL: DTX OCX_BOT Raw Data Register
 */
union bdk_dtx_ocx_bot_datx
{
    uint64_t u;
    struct bdk_dtx_ocx_bot_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_bot_datx_s cn; */
};
typedef union bdk_dtx_ocx_bot_datx bdk_dtx_ocx_bot_datx_t;

static inline uint64_t BDK_DTX_OCX_BOT_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_BOT_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe198040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_OCX_BOT_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_OCX_BOT_DATX(a) bdk_dtx_ocx_bot_datx_t
#define bustype_BDK_DTX_OCX_BOT_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_BOT_DATX(a) "DTX_OCX_BOT_DATX"
#define busnum_BDK_DTX_OCX_BOT_DATX(a) (a)
#define arguments_BDK_DTX_OCX_BOT_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ocx_bot_ena#
 *
 * INTERNAL: DTX OCX_BOT Data Enable Register
 */
union bdk_dtx_ocx_bot_enax
{
    uint64_t u;
    struct bdk_dtx_ocx_bot_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_bot_enax_s cn; */
};
typedef union bdk_dtx_ocx_bot_enax bdk_dtx_ocx_bot_enax_t;

static inline uint64_t BDK_DTX_OCX_BOT_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_BOT_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe198020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_OCX_BOT_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_OCX_BOT_ENAX(a) bdk_dtx_ocx_bot_enax_t
#define bustype_BDK_DTX_OCX_BOT_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_BOT_ENAX(a) "DTX_OCX_BOT_ENAX"
#define busnum_BDK_DTX_OCX_BOT_ENAX(a) (a)
#define arguments_BDK_DTX_OCX_BOT_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ocx_bot_sel#
 *
 * INTERNAL: DTX OCX_BOT Select Register
 */
union bdk_dtx_ocx_bot_selx
{
    uint64_t u;
    struct bdk_dtx_ocx_bot_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_bot_selx_s cn; */
};
typedef union bdk_dtx_ocx_bot_selx bdk_dtx_ocx_bot_selx_t;

static inline uint64_t BDK_DTX_OCX_BOT_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_BOT_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe198000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_OCX_BOT_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_OCX_BOT_SELX(a) bdk_dtx_ocx_bot_selx_t
#define bustype_BDK_DTX_OCX_BOT_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_BOT_SELX(a) "DTX_OCX_BOT_SELX"
#define busnum_BDK_DTX_OCX_BOT_SELX(a) (a)
#define arguments_BDK_DTX_OCX_BOT_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ocx_lnk#_bcst_rsp
 *
 * DTX OCX_LNK Control Register
 */
union bdk_dtx_ocx_lnkx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_ocx_lnkx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_lnkx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_ocx_lnkx_bcst_rsp bdk_dtx_ocx_lnkx_bcst_rsp_t;

static inline uint64_t BDK_DTX_OCX_LNKX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_LNKX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e0fe180080ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_OCX_LNKX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_OCX_LNKX_BCST_RSP(a) bdk_dtx_ocx_lnkx_bcst_rsp_t
#define bustype_BDK_DTX_OCX_LNKX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_LNKX_BCST_RSP(a) "DTX_OCX_LNKX_BCST_RSP"
#define busnum_BDK_DTX_OCX_LNKX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_OCX_LNKX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ocx_lnk#_ctl
 *
 * DTX OCX_LNK Control Register
 */
union bdk_dtx_ocx_lnkx_ctl
{
    uint64_t u;
    struct bdk_dtx_ocx_lnkx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_lnkx_ctl_s cn; */
};
typedef union bdk_dtx_ocx_lnkx_ctl bdk_dtx_ocx_lnkx_ctl_t;

static inline uint64_t BDK_DTX_OCX_LNKX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_LNKX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e0fe180060ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_OCX_LNKX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_OCX_LNKX_CTL(a) bdk_dtx_ocx_lnkx_ctl_t
#define bustype_BDK_DTX_OCX_LNKX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_LNKX_CTL(a) "DTX_OCX_LNKX_CTL"
#define busnum_BDK_DTX_OCX_LNKX_CTL(a) (a)
#define arguments_BDK_DTX_OCX_LNKX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ocx_lnk#_dat#
 *
 * DTX OCX_LNK Raw Data Register
 */
union bdk_dtx_ocx_lnkx_datx
{
    uint64_t u;
    struct bdk_dtx_ocx_lnkx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_lnkx_datx_s cn; */
};
typedef union bdk_dtx_ocx_lnkx_datx bdk_dtx_ocx_lnkx_datx_t;

static inline uint64_t BDK_DTX_OCX_LNKX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_LNKX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=1)))
        return 0x87e0fe180040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_OCX_LNKX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_OCX_LNKX_DATX(a,b) bdk_dtx_ocx_lnkx_datx_t
#define bustype_BDK_DTX_OCX_LNKX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_LNKX_DATX(a,b) "DTX_OCX_LNKX_DATX"
#define busnum_BDK_DTX_OCX_LNKX_DATX(a,b) (a)
#define arguments_BDK_DTX_OCX_LNKX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_ocx_lnk#_ena#
 *
 * DTX OCX_LNK Data Enable Register
 */
union bdk_dtx_ocx_lnkx_enax
{
    uint64_t u;
    struct bdk_dtx_ocx_lnkx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_lnkx_enax_s cn; */
};
typedef union bdk_dtx_ocx_lnkx_enax bdk_dtx_ocx_lnkx_enax_t;

static inline uint64_t BDK_DTX_OCX_LNKX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_LNKX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=1)))
        return 0x87e0fe180020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_OCX_LNKX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_OCX_LNKX_ENAX(a,b) bdk_dtx_ocx_lnkx_enax_t
#define bustype_BDK_DTX_OCX_LNKX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_LNKX_ENAX(a,b) "DTX_OCX_LNKX_ENAX"
#define busnum_BDK_DTX_OCX_LNKX_ENAX(a,b) (a)
#define arguments_BDK_DTX_OCX_LNKX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_ocx_lnk#_sel#
 *
 * DTX OCX_LNK Select Register
 */
union bdk_dtx_ocx_lnkx_selx
{
    uint64_t u;
    struct bdk_dtx_ocx_lnkx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_lnkx_selx_s cn; */
};
typedef union bdk_dtx_ocx_lnkx_selx bdk_dtx_ocx_lnkx_selx_t;

static inline uint64_t BDK_DTX_OCX_LNKX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_LNKX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=1)))
        return 0x87e0fe180000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_OCX_LNKX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_OCX_LNKX_SELX(a,b) bdk_dtx_ocx_lnkx_selx_t
#define bustype_BDK_DTX_OCX_LNKX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_LNKX_SELX(a,b) "DTX_OCX_LNKX_SELX"
#define busnum_BDK_DTX_OCX_LNKX_SELX(a,b) (a)
#define arguments_BDK_DTX_OCX_LNKX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_ocx_ole#_bcst_rsp
 *
 * DTX OCX_OLE Control Register
 */
union bdk_dtx_ocx_olex_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_ocx_olex_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_olex_bcst_rsp_s cn; */
};
typedef union bdk_dtx_ocx_olex_bcst_rsp bdk_dtx_ocx_olex_bcst_rsp_t;

static inline uint64_t BDK_DTX_OCX_OLEX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_OLEX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e0fe1a0080ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_OCX_OLEX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_OCX_OLEX_BCST_RSP(a) bdk_dtx_ocx_olex_bcst_rsp_t
#define bustype_BDK_DTX_OCX_OLEX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_OLEX_BCST_RSP(a) "DTX_OCX_OLEX_BCST_RSP"
#define busnum_BDK_DTX_OCX_OLEX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_OCX_OLEX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ocx_ole#_ctl
 *
 * DTX OCX_OLE Control Register
 */
union bdk_dtx_ocx_olex_ctl
{
    uint64_t u;
    struct bdk_dtx_ocx_olex_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_olex_ctl_s cn; */
};
typedef union bdk_dtx_ocx_olex_ctl bdk_dtx_ocx_olex_ctl_t;

static inline uint64_t BDK_DTX_OCX_OLEX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_OLEX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=2))
        return 0x87e0fe1a0060ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_OCX_OLEX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_OCX_OLEX_CTL(a) bdk_dtx_ocx_olex_ctl_t
#define bustype_BDK_DTX_OCX_OLEX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_OLEX_CTL(a) "DTX_OCX_OLEX_CTL"
#define busnum_BDK_DTX_OCX_OLEX_CTL(a) (a)
#define arguments_BDK_DTX_OCX_OLEX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ocx_ole#_dat#
 *
 * DTX OCX_OLE Raw Data Register
 */
union bdk_dtx_ocx_olex_datx
{
    uint64_t u;
    struct bdk_dtx_ocx_olex_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_olex_datx_s cn; */
};
typedef union bdk_dtx_ocx_olex_datx bdk_dtx_ocx_olex_datx_t;

static inline uint64_t BDK_DTX_OCX_OLEX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_OLEX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=1)))
        return 0x87e0fe1a0040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_OCX_OLEX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_OCX_OLEX_DATX(a,b) bdk_dtx_ocx_olex_datx_t
#define bustype_BDK_DTX_OCX_OLEX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_OLEX_DATX(a,b) "DTX_OCX_OLEX_DATX"
#define busnum_BDK_DTX_OCX_OLEX_DATX(a,b) (a)
#define arguments_BDK_DTX_OCX_OLEX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_ocx_ole#_ena#
 *
 * DTX OCX_OLE Data Enable Register
 */
union bdk_dtx_ocx_olex_enax
{
    uint64_t u;
    struct bdk_dtx_ocx_olex_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_olex_enax_s cn; */
};
typedef union bdk_dtx_ocx_olex_enax bdk_dtx_ocx_olex_enax_t;

static inline uint64_t BDK_DTX_OCX_OLEX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_OLEX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=1)))
        return 0x87e0fe1a0020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_OCX_OLEX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_OCX_OLEX_ENAX(a,b) bdk_dtx_ocx_olex_enax_t
#define bustype_BDK_DTX_OCX_OLEX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_OLEX_ENAX(a,b) "DTX_OCX_OLEX_ENAX"
#define busnum_BDK_DTX_OCX_OLEX_ENAX(a,b) (a)
#define arguments_BDK_DTX_OCX_OLEX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_ocx_ole#_sel#
 *
 * DTX OCX_OLE Select Register
 */
union bdk_dtx_ocx_olex_selx
{
    uint64_t u;
    struct bdk_dtx_ocx_olex_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_olex_selx_s cn; */
};
typedef union bdk_dtx_ocx_olex_selx bdk_dtx_ocx_olex_selx_t;

static inline uint64_t BDK_DTX_OCX_OLEX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_OLEX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=2) && (b<=1)))
        return 0x87e0fe1a0000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_OCX_OLEX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_OCX_OLEX_SELX(a,b) bdk_dtx_ocx_olex_selx_t
#define bustype_BDK_DTX_OCX_OLEX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_OLEX_SELX(a,b) "DTX_OCX_OLEX_SELX"
#define busnum_BDK_DTX_OCX_OLEX_SELX(a,b) (a)
#define arguments_BDK_DTX_OCX_OLEX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_ocx_top_bcst_rsp
 *
 * DTX OCX_TOP Control Register
 */
union bdk_dtx_ocx_top_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_ocx_top_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_top_bcst_rsp_s cn; */
};
typedef union bdk_dtx_ocx_top_bcst_rsp bdk_dtx_ocx_top_bcst_rsp_t;

#define BDK_DTX_OCX_TOP_BCST_RSP BDK_DTX_OCX_TOP_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_OCX_TOP_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_TOP_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0fe088080ll;
    __bdk_csr_fatal("DTX_OCX_TOP_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_OCX_TOP_BCST_RSP bdk_dtx_ocx_top_bcst_rsp_t
#define bustype_BDK_DTX_OCX_TOP_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_TOP_BCST_RSP "DTX_OCX_TOP_BCST_RSP"
#define busnum_BDK_DTX_OCX_TOP_BCST_RSP 0
#define arguments_BDK_DTX_OCX_TOP_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_ocx_top_ctl
 *
 * DTX OCX_TOP Control Register
 */
union bdk_dtx_ocx_top_ctl
{
    uint64_t u;
    struct bdk_dtx_ocx_top_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_top_ctl_s cn; */
};
typedef union bdk_dtx_ocx_top_ctl bdk_dtx_ocx_top_ctl_t;

#define BDK_DTX_OCX_TOP_CTL BDK_DTX_OCX_TOP_CTL_FUNC()
static inline uint64_t BDK_DTX_OCX_TOP_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_TOP_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0fe088060ll;
    __bdk_csr_fatal("DTX_OCX_TOP_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_OCX_TOP_CTL bdk_dtx_ocx_top_ctl_t
#define bustype_BDK_DTX_OCX_TOP_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_TOP_CTL "DTX_OCX_TOP_CTL"
#define busnum_BDK_DTX_OCX_TOP_CTL 0
#define arguments_BDK_DTX_OCX_TOP_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_ocx_top_dat#
 *
 * DTX OCX_TOP Raw Data Register
 */
union bdk_dtx_ocx_top_datx
{
    uint64_t u;
    struct bdk_dtx_ocx_top_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_top_datx_s cn; */
};
typedef union bdk_dtx_ocx_top_datx bdk_dtx_ocx_top_datx_t;

static inline uint64_t BDK_DTX_OCX_TOP_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_TOP_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe088040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_OCX_TOP_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_OCX_TOP_DATX(a) bdk_dtx_ocx_top_datx_t
#define bustype_BDK_DTX_OCX_TOP_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_TOP_DATX(a) "DTX_OCX_TOP_DATX"
#define busnum_BDK_DTX_OCX_TOP_DATX(a) (a)
#define arguments_BDK_DTX_OCX_TOP_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ocx_top_ena#
 *
 * DTX OCX_TOP Data Enable Register
 */
union bdk_dtx_ocx_top_enax
{
    uint64_t u;
    struct bdk_dtx_ocx_top_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_top_enax_s cn; */
};
typedef union bdk_dtx_ocx_top_enax bdk_dtx_ocx_top_enax_t;

static inline uint64_t BDK_DTX_OCX_TOP_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_TOP_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe088020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_OCX_TOP_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_OCX_TOP_ENAX(a) bdk_dtx_ocx_top_enax_t
#define bustype_BDK_DTX_OCX_TOP_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_TOP_ENAX(a) "DTX_OCX_TOP_ENAX"
#define busnum_BDK_DTX_OCX_TOP_ENAX(a) (a)
#define arguments_BDK_DTX_OCX_TOP_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ocx_top_sel#
 *
 * DTX OCX_TOP Select Register
 */
union bdk_dtx_ocx_top_selx
{
    uint64_t u;
    struct bdk_dtx_ocx_top_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ocx_top_selx_s cn; */
};
typedef union bdk_dtx_ocx_top_selx bdk_dtx_ocx_top_selx_t;

static inline uint64_t BDK_DTX_OCX_TOP_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_OCX_TOP_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe088000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_OCX_TOP_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_OCX_TOP_SELX(a) bdk_dtx_ocx_top_selx_t
#define bustype_BDK_DTX_OCX_TOP_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_OCX_TOP_SELX(a) "DTX_OCX_TOP_SELX"
#define busnum_BDK_DTX_OCX_TOP_SELX(a) (a)
#define arguments_BDK_DTX_OCX_TOP_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_pem#_bcst_rsp
 *
 * DTX PEM Control Register
 */
union bdk_dtx_pemx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_pemx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_pemx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_pemx_bcst_rsp bdk_dtx_pemx_bcst_rsp_t;

static inline uint64_t BDK_DTX_PEMX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PEMX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=2))
        return 0x87e0fe600080ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e0fe600080ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=5))
        return 0x87e0fe600080ll + 0x8000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e0fef00080ll + 0x8000ll * ((a) & 0x7);
    __bdk_csr_fatal("DTX_PEMX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_PEMX_BCST_RSP(a) bdk_dtx_pemx_bcst_rsp_t
#define bustype_BDK_DTX_PEMX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PEMX_BCST_RSP(a) "DTX_PEMX_BCST_RSP"
#define busnum_BDK_DTX_PEMX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_PEMX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_pem#_ctl
 *
 * DTX PEM Control Register
 */
union bdk_dtx_pemx_ctl
{
    uint64_t u;
    struct bdk_dtx_pemx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_pemx_ctl_s cn; */
};
typedef union bdk_dtx_pemx_ctl bdk_dtx_pemx_ctl_t;

static inline uint64_t BDK_DTX_PEMX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PEMX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=2))
        return 0x87e0fe600060ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e0fe600060ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=5))
        return 0x87e0fe600060ll + 0x8000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e0fef00060ll + 0x8000ll * ((a) & 0x7);
    __bdk_csr_fatal("DTX_PEMX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_PEMX_CTL(a) bdk_dtx_pemx_ctl_t
#define bustype_BDK_DTX_PEMX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PEMX_CTL(a) "DTX_PEMX_CTL"
#define busnum_BDK_DTX_PEMX_CTL(a) (a)
#define arguments_BDK_DTX_PEMX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_pem#_dat#
 *
 * DTX PEM Raw Data Register
 */
union bdk_dtx_pemx_datx
{
    uint64_t u;
    struct bdk_dtx_pemx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_pemx_datx_s cn; */
};
typedef union bdk_dtx_pemx_datx bdk_dtx_pemx_datx_t;

static inline uint64_t BDK_DTX_PEMX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PEMX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=2) && (b<=1)))
        return 0x87e0fe600040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe600040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=5) && (b<=1)))
        return 0x87e0fe600040ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=7) && (b<=1)))
        return 0x87e0fef00040ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_PEMX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_PEMX_DATX(a,b) bdk_dtx_pemx_datx_t
#define bustype_BDK_DTX_PEMX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PEMX_DATX(a,b) "DTX_PEMX_DATX"
#define busnum_BDK_DTX_PEMX_DATX(a,b) (a)
#define arguments_BDK_DTX_PEMX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_pem#_ena#
 *
 * DTX PEM Data Enable Register
 */
union bdk_dtx_pemx_enax
{
    uint64_t u;
    struct bdk_dtx_pemx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_pemx_enax_s cn; */
};
typedef union bdk_dtx_pemx_enax bdk_dtx_pemx_enax_t;

static inline uint64_t BDK_DTX_PEMX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PEMX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=2) && (b<=1)))
        return 0x87e0fe600020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe600020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=5) && (b<=1)))
        return 0x87e0fe600020ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=7) && (b<=1)))
        return 0x87e0fef00020ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_PEMX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_PEMX_ENAX(a,b) bdk_dtx_pemx_enax_t
#define bustype_BDK_DTX_PEMX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PEMX_ENAX(a,b) "DTX_PEMX_ENAX"
#define busnum_BDK_DTX_PEMX_ENAX(a,b) (a)
#define arguments_BDK_DTX_PEMX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_pem#_sel#
 *
 * DTX PEM Select Register
 */
union bdk_dtx_pemx_selx
{
    uint64_t u;
    struct bdk_dtx_pemx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_pemx_selx_s cn; */
};
typedef union bdk_dtx_pemx_selx bdk_dtx_pemx_selx_t;

static inline uint64_t BDK_DTX_PEMX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PEMX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=2) && (b<=1)))
        return 0x87e0fe600000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe600000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=5) && (b<=1)))
        return 0x87e0fe600000ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=7) && (b<=1)))
        return 0x87e0fef00000ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_PEMX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_PEMX_SELX(a,b) bdk_dtx_pemx_selx_t
#define bustype_BDK_DTX_PEMX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PEMX_SELX(a,b) "DTX_PEMX_SELX"
#define busnum_BDK_DTX_PEMX_SELX(a,b) (a)
#define arguments_BDK_DTX_PEMX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_pki_bcst_rsp
 *
 * DTX PKI Control Register
 */
union bdk_dtx_pki_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_pki_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_pki_bcst_rsp_s cn; */
};
typedef union bdk_dtx_pki_bcst_rsp bdk_dtx_pki_bcst_rsp_t;

#define BDK_DTX_PKI_BCST_RSP BDK_DTX_PKI_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_PKI_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PKI_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0feb60080ll;
    __bdk_csr_fatal("DTX_PKI_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_PKI_BCST_RSP bdk_dtx_pki_bcst_rsp_t
#define bustype_BDK_DTX_PKI_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PKI_BCST_RSP "DTX_PKI_BCST_RSP"
#define busnum_BDK_DTX_PKI_BCST_RSP 0
#define arguments_BDK_DTX_PKI_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_pki_ctl
 *
 * DTX PKI Control Register
 */
union bdk_dtx_pki_ctl
{
    uint64_t u;
    struct bdk_dtx_pki_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_pki_ctl_s cn; */
};
typedef union bdk_dtx_pki_ctl bdk_dtx_pki_ctl_t;

#define BDK_DTX_PKI_CTL BDK_DTX_PKI_CTL_FUNC()
static inline uint64_t BDK_DTX_PKI_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PKI_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0feb60060ll;
    __bdk_csr_fatal("DTX_PKI_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_PKI_CTL bdk_dtx_pki_ctl_t
#define bustype_BDK_DTX_PKI_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PKI_CTL "DTX_PKI_CTL"
#define busnum_BDK_DTX_PKI_CTL 0
#define arguments_BDK_DTX_PKI_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_pki_dat#
 *
 * DTX PKI Raw Data Register
 */
union bdk_dtx_pki_datx
{
    uint64_t u;
    struct bdk_dtx_pki_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_pki_datx_s cn; */
};
typedef union bdk_dtx_pki_datx bdk_dtx_pki_datx_t;

static inline uint64_t BDK_DTX_PKI_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PKI_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb60040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_PKI_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_PKI_DATX(a) bdk_dtx_pki_datx_t
#define bustype_BDK_DTX_PKI_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PKI_DATX(a) "DTX_PKI_DATX"
#define busnum_BDK_DTX_PKI_DATX(a) (a)
#define arguments_BDK_DTX_PKI_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_pki_ena#
 *
 * DTX PKI Data Enable Register
 */
union bdk_dtx_pki_enax
{
    uint64_t u;
    struct bdk_dtx_pki_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_pki_enax_s cn; */
};
typedef union bdk_dtx_pki_enax bdk_dtx_pki_enax_t;

static inline uint64_t BDK_DTX_PKI_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PKI_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb60020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_PKI_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_PKI_ENAX(a) bdk_dtx_pki_enax_t
#define bustype_BDK_DTX_PKI_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PKI_ENAX(a) "DTX_PKI_ENAX"
#define busnum_BDK_DTX_PKI_ENAX(a) (a)
#define arguments_BDK_DTX_PKI_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_pki_sel#
 *
 * DTX PKI Select Register
 */
union bdk_dtx_pki_selx
{
    uint64_t u;
    struct bdk_dtx_pki_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_pki_selx_s cn; */
};
typedef union bdk_dtx_pki_selx bdk_dtx_pki_selx_t;

static inline uint64_t BDK_DTX_PKI_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PKI_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb60000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_PKI_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_PKI_SELX(a) bdk_dtx_pki_selx_t
#define bustype_BDK_DTX_PKI_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PKI_SELX(a) "DTX_PKI_SELX"
#define busnum_BDK_DTX_PKI_SELX(a) (a)
#define arguments_BDK_DTX_PKI_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_pko_bcst_rsp
 *
 * DTX PKO Control Register
 */
union bdk_dtx_pko_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_pko_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_pko_bcst_rsp_s cn; */
};
typedef union bdk_dtx_pko_bcst_rsp bdk_dtx_pko_bcst_rsp_t;

#define BDK_DTX_PKO_BCST_RSP BDK_DTX_PKO_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_PKO_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PKO_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0feaa0080ll;
    __bdk_csr_fatal("DTX_PKO_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_PKO_BCST_RSP bdk_dtx_pko_bcst_rsp_t
#define bustype_BDK_DTX_PKO_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PKO_BCST_RSP "DTX_PKO_BCST_RSP"
#define busnum_BDK_DTX_PKO_BCST_RSP 0
#define arguments_BDK_DTX_PKO_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_pko_ctl
 *
 * DTX PKO Control Register
 */
union bdk_dtx_pko_ctl
{
    uint64_t u;
    struct bdk_dtx_pko_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_pko_ctl_s cn; */
};
typedef union bdk_dtx_pko_ctl bdk_dtx_pko_ctl_t;

#define BDK_DTX_PKO_CTL BDK_DTX_PKO_CTL_FUNC()
static inline uint64_t BDK_DTX_PKO_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PKO_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0feaa0060ll;
    __bdk_csr_fatal("DTX_PKO_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_PKO_CTL bdk_dtx_pko_ctl_t
#define bustype_BDK_DTX_PKO_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PKO_CTL "DTX_PKO_CTL"
#define busnum_BDK_DTX_PKO_CTL 0
#define arguments_BDK_DTX_PKO_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_pko_dat#
 *
 * DTX PKO Raw Data Register
 */
union bdk_dtx_pko_datx
{
    uint64_t u;
    struct bdk_dtx_pko_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_pko_datx_s cn; */
};
typedef union bdk_dtx_pko_datx bdk_dtx_pko_datx_t;

static inline uint64_t BDK_DTX_PKO_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PKO_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feaa0040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_PKO_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_PKO_DATX(a) bdk_dtx_pko_datx_t
#define bustype_BDK_DTX_PKO_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PKO_DATX(a) "DTX_PKO_DATX"
#define busnum_BDK_DTX_PKO_DATX(a) (a)
#define arguments_BDK_DTX_PKO_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_pko_ena#
 *
 * DTX PKO Data Enable Register
 */
union bdk_dtx_pko_enax
{
    uint64_t u;
    struct bdk_dtx_pko_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_pko_enax_s cn; */
};
typedef union bdk_dtx_pko_enax bdk_dtx_pko_enax_t;

static inline uint64_t BDK_DTX_PKO_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PKO_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feaa0020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_PKO_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_PKO_ENAX(a) bdk_dtx_pko_enax_t
#define bustype_BDK_DTX_PKO_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PKO_ENAX(a) "DTX_PKO_ENAX"
#define busnum_BDK_DTX_PKO_ENAX(a) (a)
#define arguments_BDK_DTX_PKO_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_pko_sel#
 *
 * DTX PKO Select Register
 */
union bdk_dtx_pko_selx
{
    uint64_t u;
    struct bdk_dtx_pko_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_pko_selx_s cn; */
};
typedef union bdk_dtx_pko_selx bdk_dtx_pko_selx_t;

static inline uint64_t BDK_DTX_PKO_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PKO_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feaa0000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_PKO_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_PKO_SELX(a) bdk_dtx_pko_selx_t
#define bustype_BDK_DTX_PKO_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PKO_SELX(a) "DTX_PKO_SELX"
#define busnum_BDK_DTX_PKO_SELX(a) (a)
#define arguments_BDK_DTX_PKO_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ptp_bcst_rsp
 *
 * DTX PTP Control Register
 */
union bdk_dtx_ptp_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_ptp_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ptp_bcst_rsp_s cn; */
};
typedef union bdk_dtx_ptp_bcst_rsp bdk_dtx_ptp_bcst_rsp_t;

#define BDK_DTX_PTP_BCST_RSP BDK_DTX_PTP_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_PTP_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PTP_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0fe838080ll;
    __bdk_csr_fatal("DTX_PTP_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_PTP_BCST_RSP bdk_dtx_ptp_bcst_rsp_t
#define bustype_BDK_DTX_PTP_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PTP_BCST_RSP "DTX_PTP_BCST_RSP"
#define busnum_BDK_DTX_PTP_BCST_RSP 0
#define arguments_BDK_DTX_PTP_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_ptp_ctl
 *
 * DTX PTP Control Register
 */
union bdk_dtx_ptp_ctl
{
    uint64_t u;
    struct bdk_dtx_ptp_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ptp_ctl_s cn; */
};
typedef union bdk_dtx_ptp_ctl bdk_dtx_ptp_ctl_t;

#define BDK_DTX_PTP_CTL BDK_DTX_PTP_CTL_FUNC()
static inline uint64_t BDK_DTX_PTP_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PTP_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0fe838060ll;
    __bdk_csr_fatal("DTX_PTP_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_PTP_CTL bdk_dtx_ptp_ctl_t
#define bustype_BDK_DTX_PTP_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PTP_CTL "DTX_PTP_CTL"
#define busnum_BDK_DTX_PTP_CTL 0
#define arguments_BDK_DTX_PTP_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_ptp_dat#
 *
 * DTX PTP Raw Data Register
 */
union bdk_dtx_ptp_datx
{
    uint64_t u;
    struct bdk_dtx_ptp_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ptp_datx_s cn; */
};
typedef union bdk_dtx_ptp_datx bdk_dtx_ptp_datx_t;

static inline uint64_t BDK_DTX_PTP_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PTP_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe838040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_PTP_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_PTP_DATX(a) bdk_dtx_ptp_datx_t
#define bustype_BDK_DTX_PTP_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PTP_DATX(a) "DTX_PTP_DATX"
#define busnum_BDK_DTX_PTP_DATX(a) (a)
#define arguments_BDK_DTX_PTP_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ptp_ena#
 *
 * DTX PTP Data Enable Register
 */
union bdk_dtx_ptp_enax
{
    uint64_t u;
    struct bdk_dtx_ptp_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ptp_enax_s cn; */
};
typedef union bdk_dtx_ptp_enax bdk_dtx_ptp_enax_t;

static inline uint64_t BDK_DTX_PTP_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PTP_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe838020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_PTP_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_PTP_ENAX(a) bdk_dtx_ptp_enax_t
#define bustype_BDK_DTX_PTP_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PTP_ENAX(a) "DTX_PTP_ENAX"
#define busnum_BDK_DTX_PTP_ENAX(a) (a)
#define arguments_BDK_DTX_PTP_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_ptp_sel#
 *
 * DTX PTP Select Register
 */
union bdk_dtx_ptp_selx
{
    uint64_t u;
    struct bdk_dtx_ptp_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_ptp_selx_s cn; */
};
typedef union bdk_dtx_ptp_selx bdk_dtx_ptp_selx_t;

static inline uint64_t BDK_DTX_PTP_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_PTP_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe838000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_PTP_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_PTP_SELX(a) bdk_dtx_ptp_selx_t
#define bustype_BDK_DTX_PTP_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_PTP_SELX(a) "DTX_PTP_SELX"
#define busnum_BDK_DTX_PTP_SELX(a) (a)
#define arguments_BDK_DTX_PTP_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_rad_bcst_rsp
 *
 * DTX RAD Control Register
 */
union bdk_dtx_rad_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_rad_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rad_bcst_rsp_s cn; */
};
typedef union bdk_dtx_rad_bcst_rsp bdk_dtx_rad_bcst_rsp_t;

#define BDK_DTX_RAD_BCST_RSP BDK_DTX_RAD_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_RAD_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RAD_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0feb80080ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0feb80080ll;
    __bdk_csr_fatal("DTX_RAD_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_RAD_BCST_RSP bdk_dtx_rad_bcst_rsp_t
#define bustype_BDK_DTX_RAD_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RAD_BCST_RSP "DTX_RAD_BCST_RSP"
#define busnum_BDK_DTX_RAD_BCST_RSP 0
#define arguments_BDK_DTX_RAD_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_rad_ctl
 *
 * DTX RAD Control Register
 */
union bdk_dtx_rad_ctl
{
    uint64_t u;
    struct bdk_dtx_rad_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rad_ctl_s cn; */
};
typedef union bdk_dtx_rad_ctl bdk_dtx_rad_ctl_t;

#define BDK_DTX_RAD_CTL BDK_DTX_RAD_CTL_FUNC()
static inline uint64_t BDK_DTX_RAD_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RAD_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0feb80060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0feb80060ll;
    __bdk_csr_fatal("DTX_RAD_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_RAD_CTL bdk_dtx_rad_ctl_t
#define bustype_BDK_DTX_RAD_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RAD_CTL "DTX_RAD_CTL"
#define busnum_BDK_DTX_RAD_CTL 0
#define arguments_BDK_DTX_RAD_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_rad_dat#
 *
 * DTX RAD Raw Data Register
 */
union bdk_dtx_rad_datx
{
    uint64_t u;
    struct bdk_dtx_rad_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rad_datx_s cn; */
};
typedef union bdk_dtx_rad_datx bdk_dtx_rad_datx_t;

static inline uint64_t BDK_DTX_RAD_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RAD_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb80040ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0feb80040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_RAD_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_RAD_DATX(a) bdk_dtx_rad_datx_t
#define bustype_BDK_DTX_RAD_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RAD_DATX(a) "DTX_RAD_DATX"
#define busnum_BDK_DTX_RAD_DATX(a) (a)
#define arguments_BDK_DTX_RAD_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_rad_ena#
 *
 * DTX RAD Data Enable Register
 */
union bdk_dtx_rad_enax
{
    uint64_t u;
    struct bdk_dtx_rad_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rad_enax_s cn; */
};
typedef union bdk_dtx_rad_enax bdk_dtx_rad_enax_t;

static inline uint64_t BDK_DTX_RAD_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RAD_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb80020ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0feb80020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_RAD_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_RAD_ENAX(a) bdk_dtx_rad_enax_t
#define bustype_BDK_DTX_RAD_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RAD_ENAX(a) "DTX_RAD_ENAX"
#define busnum_BDK_DTX_RAD_ENAX(a) (a)
#define arguments_BDK_DTX_RAD_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_rad_sel#
 *
 * DTX RAD Select Register
 */
union bdk_dtx_rad_selx
{
    uint64_t u;
    struct bdk_dtx_rad_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rad_selx_s cn; */
};
typedef union bdk_dtx_rad_selx bdk_dtx_rad_selx_t;

static inline uint64_t BDK_DTX_RAD_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RAD_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb80000ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0feb80000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_RAD_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_RAD_SELX(a) bdk_dtx_rad_selx_t
#define bustype_BDK_DTX_RAD_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RAD_SELX(a) "DTX_RAD_SELX"
#define busnum_BDK_DTX_RAD_SELX(a) (a)
#define arguments_BDK_DTX_RAD_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_rgx#_bcst_rsp
 *
 * DTX RGX Control Register
 */
union bdk_dtx_rgxx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_rgxx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rgxx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_rgxx_bcst_rsp bdk_dtx_rgxx_bcst_rsp_t;

static inline uint64_t BDK_DTX_RGXX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RGXX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0fe740080ll + 0x8000ll * ((a) & 0x0);
    __bdk_csr_fatal("DTX_RGXX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_RGXX_BCST_RSP(a) bdk_dtx_rgxx_bcst_rsp_t
#define bustype_BDK_DTX_RGXX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RGXX_BCST_RSP(a) "DTX_RGXX_BCST_RSP"
#define busnum_BDK_DTX_RGXX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_RGXX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_rgx#_ctl
 *
 * DTX RGX Control Register
 */
union bdk_dtx_rgxx_ctl
{
    uint64_t u;
    struct bdk_dtx_rgxx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rgxx_ctl_s cn; */
};
typedef union bdk_dtx_rgxx_ctl bdk_dtx_rgxx_ctl_t;

static inline uint64_t BDK_DTX_RGXX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RGXX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0fe740060ll + 0x8000ll * ((a) & 0x0);
    __bdk_csr_fatal("DTX_RGXX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_RGXX_CTL(a) bdk_dtx_rgxx_ctl_t
#define bustype_BDK_DTX_RGXX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RGXX_CTL(a) "DTX_RGXX_CTL"
#define busnum_BDK_DTX_RGXX_CTL(a) (a)
#define arguments_BDK_DTX_RGXX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_rgx#_dat#
 *
 * DTX RGX Raw Data Register
 */
union bdk_dtx_rgxx_datx
{
    uint64_t u;
    struct bdk_dtx_rgxx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rgxx_datx_s cn; */
};
typedef union bdk_dtx_rgxx_datx bdk_dtx_rgxx_datx_t;

static inline uint64_t BDK_DTX_RGXX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RGXX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe740040ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_RGXX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_RGXX_DATX(a,b) bdk_dtx_rgxx_datx_t
#define bustype_BDK_DTX_RGXX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RGXX_DATX(a,b) "DTX_RGXX_DATX"
#define busnum_BDK_DTX_RGXX_DATX(a,b) (a)
#define arguments_BDK_DTX_RGXX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_rgx#_ena#
 *
 * DTX RGX Data Enable Register
 */
union bdk_dtx_rgxx_enax
{
    uint64_t u;
    struct bdk_dtx_rgxx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rgxx_enax_s cn; */
};
typedef union bdk_dtx_rgxx_enax bdk_dtx_rgxx_enax_t;

static inline uint64_t BDK_DTX_RGXX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RGXX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe740020ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_RGXX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_RGXX_ENAX(a,b) bdk_dtx_rgxx_enax_t
#define bustype_BDK_DTX_RGXX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RGXX_ENAX(a,b) "DTX_RGXX_ENAX"
#define busnum_BDK_DTX_RGXX_ENAX(a,b) (a)
#define arguments_BDK_DTX_RGXX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_rgx#_sel#
 *
 * DTX RGX Select Register
 */
union bdk_dtx_rgxx_selx
{
    uint64_t u;
    struct bdk_dtx_rgxx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rgxx_selx_s cn; */
};
typedef union bdk_dtx_rgxx_selx bdk_dtx_rgxx_selx_t;

static inline uint64_t BDK_DTX_RGXX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RGXX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0fe740000ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_RGXX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_RGXX_SELX(a,b) bdk_dtx_rgxx_selx_t
#define bustype_BDK_DTX_RGXX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RGXX_SELX(a,b) "DTX_RGXX_SELX"
#define busnum_BDK_DTX_RGXX_SELX(a,b) (a)
#define arguments_BDK_DTX_RGXX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_rnm_bcst_rsp
 *
 * DTX RNM Control Register
 */
union bdk_dtx_rnm_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_rnm_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rnm_bcst_rsp_s cn; */
};
typedef union bdk_dtx_rnm_bcst_rsp bdk_dtx_rnm_bcst_rsp_t;

#define BDK_DTX_RNM_BCST_RSP BDK_DTX_RNM_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_RNM_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RNM_BCST_RSP_FUNC(void)
{
    return 0x87e0fe200080ll;
}

#define typedef_BDK_DTX_RNM_BCST_RSP bdk_dtx_rnm_bcst_rsp_t
#define bustype_BDK_DTX_RNM_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RNM_BCST_RSP "DTX_RNM_BCST_RSP"
#define busnum_BDK_DTX_RNM_BCST_RSP 0
#define arguments_BDK_DTX_RNM_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_rnm_ctl
 *
 * DTX RNM Control Register
 */
union bdk_dtx_rnm_ctl
{
    uint64_t u;
    struct bdk_dtx_rnm_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rnm_ctl_s cn; */
};
typedef union bdk_dtx_rnm_ctl bdk_dtx_rnm_ctl_t;

#define BDK_DTX_RNM_CTL BDK_DTX_RNM_CTL_FUNC()
static inline uint64_t BDK_DTX_RNM_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RNM_CTL_FUNC(void)
{
    return 0x87e0fe200060ll;
}

#define typedef_BDK_DTX_RNM_CTL bdk_dtx_rnm_ctl_t
#define bustype_BDK_DTX_RNM_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RNM_CTL "DTX_RNM_CTL"
#define busnum_BDK_DTX_RNM_CTL 0
#define arguments_BDK_DTX_RNM_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_rnm_dat#
 *
 * DTX RNM Raw Data Register
 */
union bdk_dtx_rnm_datx
{
    uint64_t u;
    struct bdk_dtx_rnm_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rnm_datx_s cn; */
};
typedef union bdk_dtx_rnm_datx bdk_dtx_rnm_datx_t;

static inline uint64_t BDK_DTX_RNM_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RNM_DATX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe200040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_RNM_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_RNM_DATX(a) bdk_dtx_rnm_datx_t
#define bustype_BDK_DTX_RNM_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RNM_DATX(a) "DTX_RNM_DATX"
#define busnum_BDK_DTX_RNM_DATX(a) (a)
#define arguments_BDK_DTX_RNM_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_rnm_ena#
 *
 * DTX RNM Data Enable Register
 */
union bdk_dtx_rnm_enax
{
    uint64_t u;
    struct bdk_dtx_rnm_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rnm_enax_s cn; */
};
typedef union bdk_dtx_rnm_enax bdk_dtx_rnm_enax_t;

static inline uint64_t BDK_DTX_RNM_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RNM_ENAX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe200020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_RNM_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_RNM_ENAX(a) bdk_dtx_rnm_enax_t
#define bustype_BDK_DTX_RNM_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RNM_ENAX(a) "DTX_RNM_ENAX"
#define busnum_BDK_DTX_RNM_ENAX(a) (a)
#define arguments_BDK_DTX_RNM_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_rnm_sel#
 *
 * DTX RNM Select Register
 */
union bdk_dtx_rnm_selx
{
    uint64_t u;
    struct bdk_dtx_rnm_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rnm_selx_s cn; */
};
typedef union bdk_dtx_rnm_selx bdk_dtx_rnm_selx_t;

static inline uint64_t BDK_DTX_RNM_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RNM_SELX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe200000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_RNM_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_RNM_SELX(a) bdk_dtx_rnm_selx_t
#define bustype_BDK_DTX_RNM_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RNM_SELX(a) "DTX_RNM_SELX"
#define busnum_BDK_DTX_RNM_SELX(a) (a)
#define arguments_BDK_DTX_RNM_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_rst_bcst_rsp
 *
 * DTX RST Control Register
 */
union bdk_dtx_rst_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_rst_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rst_bcst_rsp_s cn; */
};
typedef union bdk_dtx_rst_bcst_rsp bdk_dtx_rst_bcst_rsp_t;

#define BDK_DTX_RST_BCST_RSP BDK_DTX_RST_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_RST_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RST_BCST_RSP_FUNC(void)
{
    return 0x87e0fe030080ll;
}

#define typedef_BDK_DTX_RST_BCST_RSP bdk_dtx_rst_bcst_rsp_t
#define bustype_BDK_DTX_RST_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RST_BCST_RSP "DTX_RST_BCST_RSP"
#define busnum_BDK_DTX_RST_BCST_RSP 0
#define arguments_BDK_DTX_RST_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_rst_ctl
 *
 * DTX RST Control Register
 */
union bdk_dtx_rst_ctl
{
    uint64_t u;
    struct bdk_dtx_rst_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rst_ctl_s cn; */
};
typedef union bdk_dtx_rst_ctl bdk_dtx_rst_ctl_t;

#define BDK_DTX_RST_CTL BDK_DTX_RST_CTL_FUNC()
static inline uint64_t BDK_DTX_RST_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RST_CTL_FUNC(void)
{
    return 0x87e0fe030060ll;
}

#define typedef_BDK_DTX_RST_CTL bdk_dtx_rst_ctl_t
#define bustype_BDK_DTX_RST_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RST_CTL "DTX_RST_CTL"
#define busnum_BDK_DTX_RST_CTL 0
#define arguments_BDK_DTX_RST_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_rst_dat#
 *
 * DTX RST Raw Data Register
 */
union bdk_dtx_rst_datx
{
    uint64_t u;
    struct bdk_dtx_rst_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rst_datx_s cn; */
};
typedef union bdk_dtx_rst_datx bdk_dtx_rst_datx_t;

static inline uint64_t BDK_DTX_RST_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RST_DATX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe030040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_RST_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_RST_DATX(a) bdk_dtx_rst_datx_t
#define bustype_BDK_DTX_RST_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RST_DATX(a) "DTX_RST_DATX"
#define busnum_BDK_DTX_RST_DATX(a) (a)
#define arguments_BDK_DTX_RST_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_rst_ena#
 *
 * DTX RST Data Enable Register
 */
union bdk_dtx_rst_enax
{
    uint64_t u;
    struct bdk_dtx_rst_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rst_enax_s cn; */
};
typedef union bdk_dtx_rst_enax bdk_dtx_rst_enax_t;

static inline uint64_t BDK_DTX_RST_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RST_ENAX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe030020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_RST_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_RST_ENAX(a) bdk_dtx_rst_enax_t
#define bustype_BDK_DTX_RST_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RST_ENAX(a) "DTX_RST_ENAX"
#define busnum_BDK_DTX_RST_ENAX(a) (a)
#define arguments_BDK_DTX_RST_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_rst_sel#
 *
 * DTX RST Select Register
 */
union bdk_dtx_rst_selx
{
    uint64_t u;
    struct bdk_dtx_rst_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_rst_selx_s cn; */
};
typedef union bdk_dtx_rst_selx bdk_dtx_rst_selx_t;

static inline uint64_t BDK_DTX_RST_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_RST_SELX(unsigned long a)
{
    if (a<=1)
        return 0x87e0fe030000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_RST_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_RST_SELX(a) bdk_dtx_rst_selx_t
#define bustype_BDK_DTX_RST_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_RST_SELX(a) "DTX_RST_SELX"
#define busnum_BDK_DTX_RST_SELX(a) (a)
#define arguments_BDK_DTX_RST_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_sata#_bcst_rsp
 *
 * DTX SATA Control Register
 */
union bdk_dtx_satax_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_satax_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_satax_bcst_rsp_s cn; */
};
typedef union bdk_dtx_satax_bcst_rsp bdk_dtx_satax_bcst_rsp_t;

static inline uint64_t BDK_DTX_SATAX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SATAX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0fe880080ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=5))
        return 0x87e0fe880080ll + 0x8000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=15))
        return 0x87e0fe880080ll + 0x8000ll * ((a) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x87e0fe880080ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_SATAX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_SATAX_BCST_RSP(a) bdk_dtx_satax_bcst_rsp_t
#define bustype_BDK_DTX_SATAX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SATAX_BCST_RSP(a) "DTX_SATAX_BCST_RSP"
#define busnum_BDK_DTX_SATAX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_SATAX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_sata#_ctl
 *
 * DTX SATA Control Register
 */
union bdk_dtx_satax_ctl
{
    uint64_t u;
    struct bdk_dtx_satax_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_satax_ctl_s cn; */
};
typedef union bdk_dtx_satax_ctl bdk_dtx_satax_ctl_t;

static inline uint64_t BDK_DTX_SATAX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SATAX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0fe880060ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=5))
        return 0x87e0fe880060ll + 0x8000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=15))
        return 0x87e0fe880060ll + 0x8000ll * ((a) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=3))
        return 0x87e0fe880060ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_SATAX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_SATAX_CTL(a) bdk_dtx_satax_ctl_t
#define bustype_BDK_DTX_SATAX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SATAX_CTL(a) "DTX_SATAX_CTL"
#define busnum_BDK_DTX_SATAX_CTL(a) (a)
#define arguments_BDK_DTX_SATAX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_sata#_dat#
 *
 * DTX SATA Raw Data Register
 */
union bdk_dtx_satax_datx
{
    uint64_t u;
    struct bdk_dtx_satax_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_satax_datx_s cn; */
};
typedef union bdk_dtx_satax_datx bdk_dtx_satax_datx_t;

static inline uint64_t BDK_DTX_SATAX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SATAX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe880040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=5) && (b<=1)))
        return 0x87e0fe880040ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=15) && (b<=1)))
        return 0x87e0fe880040ll + 0x8000ll * ((a) & 0xf) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=3) && (b<=1)))
        return 0x87e0fe880040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_SATAX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_SATAX_DATX(a,b) bdk_dtx_satax_datx_t
#define bustype_BDK_DTX_SATAX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SATAX_DATX(a,b) "DTX_SATAX_DATX"
#define busnum_BDK_DTX_SATAX_DATX(a,b) (a)
#define arguments_BDK_DTX_SATAX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_sata#_ena#
 *
 * DTX SATA Data Enable Register
 */
union bdk_dtx_satax_enax
{
    uint64_t u;
    struct bdk_dtx_satax_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_satax_enax_s cn; */
};
typedef union bdk_dtx_satax_enax bdk_dtx_satax_enax_t;

static inline uint64_t BDK_DTX_SATAX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SATAX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe880020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=5) && (b<=1)))
        return 0x87e0fe880020ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=15) && (b<=1)))
        return 0x87e0fe880020ll + 0x8000ll * ((a) & 0xf) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=3) && (b<=1)))
        return 0x87e0fe880020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_SATAX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_SATAX_ENAX(a,b) bdk_dtx_satax_enax_t
#define bustype_BDK_DTX_SATAX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SATAX_ENAX(a,b) "DTX_SATAX_ENAX"
#define busnum_BDK_DTX_SATAX_ENAX(a,b) (a)
#define arguments_BDK_DTX_SATAX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_sata#_sel#
 *
 * DTX SATA Select Register
 */
union bdk_dtx_satax_selx
{
    uint64_t u;
    struct bdk_dtx_satax_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_satax_selx_s cn; */
};
typedef union bdk_dtx_satax_selx bdk_dtx_satax_selx_t;

static inline uint64_t BDK_DTX_SATAX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SATAX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe880000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=5) && (b<=1)))
        return 0x87e0fe880000ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=15) && (b<=1)))
        return 0x87e0fe880000ll + 0x8000ll * ((a) & 0xf) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=3) && (b<=1)))
        return 0x87e0fe880000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_SATAX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_SATAX_SELX(a,b) bdk_dtx_satax_selx_t
#define bustype_BDK_DTX_SATAX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SATAX_SELX(a,b) "DTX_SATAX_SELX"
#define busnum_BDK_DTX_SATAX_SELX(a,b) (a)
#define arguments_BDK_DTX_SATAX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_sli#_bcst_rsp
 *
 * DTX SLI Control Register
 */
union bdk_dtx_slix_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_slix_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_slix_bcst_rsp_s cn; */
};
typedef union bdk_dtx_slix_bcst_rsp bdk_dtx_slix_bcst_rsp_t;

static inline uint64_t BDK_DTX_SLIX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SLIX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0feba0080ll + 0x8000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x87e0feba0080ll + 0x8000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0feba0080ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_SLIX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_SLIX_BCST_RSP(a) bdk_dtx_slix_bcst_rsp_t
#define bustype_BDK_DTX_SLIX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SLIX_BCST_RSP(a) "DTX_SLIX_BCST_RSP"
#define busnum_BDK_DTX_SLIX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_SLIX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_sli#_ctl
 *
 * DTX SLI Control Register
 */
union bdk_dtx_slix_ctl
{
    uint64_t u;
    struct bdk_dtx_slix_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_slix_ctl_s cn; */
};
typedef union bdk_dtx_slix_ctl bdk_dtx_slix_ctl_t;

static inline uint64_t BDK_DTX_SLIX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SLIX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e0feba0060ll + 0x8000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a==0))
        return 0x87e0feba0060ll + 0x8000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0feba0060ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_SLIX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_SLIX_CTL(a) bdk_dtx_slix_ctl_t
#define bustype_BDK_DTX_SLIX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SLIX_CTL(a) "DTX_SLIX_CTL"
#define busnum_BDK_DTX_SLIX_CTL(a) (a)
#define arguments_BDK_DTX_SLIX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_sli#_dat#
 *
 * DTX SLI Raw Data Register
 */
union bdk_dtx_slix_datx
{
    uint64_t u;
    struct bdk_dtx_slix_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_slix_datx_s cn; */
};
typedef union bdk_dtx_slix_datx bdk_dtx_slix_datx_t;

static inline uint64_t BDK_DTX_SLIX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SLIX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0feba0040ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x87e0feba0040ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0feba0040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_SLIX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_SLIX_DATX(a,b) bdk_dtx_slix_datx_t
#define bustype_BDK_DTX_SLIX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SLIX_DATX(a,b) "DTX_SLIX_DATX"
#define busnum_BDK_DTX_SLIX_DATX(a,b) (a)
#define arguments_BDK_DTX_SLIX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_sli#_ena#
 *
 * DTX SLI Data Enable Register
 */
union bdk_dtx_slix_enax
{
    uint64_t u;
    struct bdk_dtx_slix_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_slix_enax_s cn; */
};
typedef union bdk_dtx_slix_enax bdk_dtx_slix_enax_t;

static inline uint64_t BDK_DTX_SLIX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SLIX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0feba0020ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x87e0feba0020ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0feba0020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_SLIX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_SLIX_ENAX(a,b) bdk_dtx_slix_enax_t
#define bustype_BDK_DTX_SLIX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SLIX_ENAX(a,b) "DTX_SLIX_ENAX"
#define busnum_BDK_DTX_SLIX_ENAX(a,b) (a)
#define arguments_BDK_DTX_SLIX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_sli#_sel#
 *
 * DTX SLI Select Register
 */
union bdk_dtx_slix_selx
{
    uint64_t u;
    struct bdk_dtx_slix_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_slix_selx_s cn; */
};
typedef union bdk_dtx_slix_selx bdk_dtx_slix_selx_t;

static inline uint64_t BDK_DTX_SLIX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SLIX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b<=1)))
        return 0x87e0feba0000ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a==0) && (b<=1)))
        return 0x87e0feba0000ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0feba0000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_SLIX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_SLIX_SELX(a,b) bdk_dtx_slix_selx_t
#define bustype_BDK_DTX_SLIX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SLIX_SELX(a,b) "DTX_SLIX_SELX"
#define busnum_BDK_DTX_SLIX_SELX(a,b) (a)
#define arguments_BDK_DTX_SLIX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_smmu#_bcst_rsp
 *
 * DTX SMMU Control Register
 */
union bdk_dtx_smmux_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_smmux_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_smmux_bcst_rsp_s cn; */
};
typedef union bdk_dtx_smmux_bcst_rsp bdk_dtx_smmux_bcst_rsp_t;

static inline uint64_t BDK_DTX_SMMUX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SMMUX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a==0))
        return 0x87e0fe980080ll + 0x8000ll * ((a) & 0x0);
    __bdk_csr_fatal("DTX_SMMUX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_SMMUX_BCST_RSP(a) bdk_dtx_smmux_bcst_rsp_t
#define bustype_BDK_DTX_SMMUX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SMMUX_BCST_RSP(a) "DTX_SMMUX_BCST_RSP"
#define busnum_BDK_DTX_SMMUX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_SMMUX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_smmu#_ctl
 *
 * DTX SMMU Control Register
 */
union bdk_dtx_smmux_ctl
{
    uint64_t u;
    struct bdk_dtx_smmux_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_smmux_ctl_s cn; */
};
typedef union bdk_dtx_smmux_ctl bdk_dtx_smmux_ctl_t;

static inline uint64_t BDK_DTX_SMMUX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SMMUX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a==0))
        return 0x87e0fe980060ll + 0x8000ll * ((a) & 0x0);
    __bdk_csr_fatal("DTX_SMMUX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_SMMUX_CTL(a) bdk_dtx_smmux_ctl_t
#define bustype_BDK_DTX_SMMUX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SMMUX_CTL(a) "DTX_SMMUX_CTL"
#define busnum_BDK_DTX_SMMUX_CTL(a) (a)
#define arguments_BDK_DTX_SMMUX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_smmu#_dat#
 *
 * DTX SMMU Raw Data Register
 */
union bdk_dtx_smmux_datx
{
    uint64_t u;
    struct bdk_dtx_smmux_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_smmux_datx_s cn; */
};
typedef union bdk_dtx_smmux_datx bdk_dtx_smmux_datx_t;

static inline uint64_t BDK_DTX_SMMUX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SMMUX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a==0) && (b<=1)))
        return 0x87e0fe980040ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_SMMUX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_SMMUX_DATX(a,b) bdk_dtx_smmux_datx_t
#define bustype_BDK_DTX_SMMUX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SMMUX_DATX(a,b) "DTX_SMMUX_DATX"
#define busnum_BDK_DTX_SMMUX_DATX(a,b) (a)
#define arguments_BDK_DTX_SMMUX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_smmu#_ena#
 *
 * DTX SMMU Data Enable Register
 */
union bdk_dtx_smmux_enax
{
    uint64_t u;
    struct bdk_dtx_smmux_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_smmux_enax_s cn; */
};
typedef union bdk_dtx_smmux_enax bdk_dtx_smmux_enax_t;

static inline uint64_t BDK_DTX_SMMUX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SMMUX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a==0) && (b<=1)))
        return 0x87e0fe980020ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_SMMUX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_SMMUX_ENAX(a,b) bdk_dtx_smmux_enax_t
#define bustype_BDK_DTX_SMMUX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SMMUX_ENAX(a,b) "DTX_SMMUX_ENAX"
#define busnum_BDK_DTX_SMMUX_ENAX(a,b) (a)
#define arguments_BDK_DTX_SMMUX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_smmu#_sel#
 *
 * DTX SMMU Select Register
 */
union bdk_dtx_smmux_selx
{
    uint64_t u;
    struct bdk_dtx_smmux_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_smmux_selx_s cn; */
};
typedef union bdk_dtx_smmux_selx bdk_dtx_smmux_selx_t;

static inline uint64_t BDK_DTX_SMMUX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SMMUX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a==0) && (b<=1)))
        return 0x87e0fe980000ll + 0x8000ll * ((a) & 0x0) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_SMMUX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_SMMUX_SELX(a,b) bdk_dtx_smmux_selx_t
#define bustype_BDK_DTX_SMMUX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SMMUX_SELX(a,b) "DTX_SMMUX_SELX"
#define busnum_BDK_DTX_SMMUX_SELX(a,b) (a)
#define arguments_BDK_DTX_SMMUX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_smmus#_bcst_rsp
 *
 * DTX SMMUS Control Register
 */
union bdk_dtx_smmusx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_smmusx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_smmusx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_smmusx_bcst_rsp bdk_dtx_smmusx_bcst_rsp_t;

static inline uint64_t BDK_DTX_SMMUSX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SMMUSX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=2))
        return 0x87e0fe988080ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_SMMUSX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_SMMUSX_BCST_RSP(a) bdk_dtx_smmusx_bcst_rsp_t
#define bustype_BDK_DTX_SMMUSX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SMMUSX_BCST_RSP(a) "DTX_SMMUSX_BCST_RSP"
#define busnum_BDK_DTX_SMMUSX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_SMMUSX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_smmus#_ctl
 *
 * DTX SMMUS Control Register
 */
union bdk_dtx_smmusx_ctl
{
    uint64_t u;
    struct bdk_dtx_smmusx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_smmusx_ctl_s cn; */
};
typedef union bdk_dtx_smmusx_ctl bdk_dtx_smmusx_ctl_t;

static inline uint64_t BDK_DTX_SMMUSX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SMMUSX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=2))
        return 0x87e0fe988060ll + 0x8000ll * ((a) & 0x3);
    __bdk_csr_fatal("DTX_SMMUSX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_SMMUSX_CTL(a) bdk_dtx_smmusx_ctl_t
#define bustype_BDK_DTX_SMMUSX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SMMUSX_CTL(a) "DTX_SMMUSX_CTL"
#define busnum_BDK_DTX_SMMUSX_CTL(a) (a)
#define arguments_BDK_DTX_SMMUSX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_smmus#_dat#
 *
 * DTX SMMUS Raw Data Register
 */
union bdk_dtx_smmusx_datx
{
    uint64_t u;
    struct bdk_dtx_smmusx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_smmusx_datx_s cn; */
};
typedef union bdk_dtx_smmusx_datx bdk_dtx_smmusx_datx_t;

static inline uint64_t BDK_DTX_SMMUSX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SMMUSX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=2) && (b<=1)))
        return 0x87e0fe988040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_SMMUSX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_SMMUSX_DATX(a,b) bdk_dtx_smmusx_datx_t
#define bustype_BDK_DTX_SMMUSX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SMMUSX_DATX(a,b) "DTX_SMMUSX_DATX"
#define busnum_BDK_DTX_SMMUSX_DATX(a,b) (a)
#define arguments_BDK_DTX_SMMUSX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_smmus#_ena#
 *
 * DTX SMMUS Data Enable Register
 */
union bdk_dtx_smmusx_enax
{
    uint64_t u;
    struct bdk_dtx_smmusx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_smmusx_enax_s cn; */
};
typedef union bdk_dtx_smmusx_enax bdk_dtx_smmusx_enax_t;

static inline uint64_t BDK_DTX_SMMUSX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SMMUSX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=2) && (b<=1)))
        return 0x87e0fe988020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_SMMUSX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_SMMUSX_ENAX(a,b) bdk_dtx_smmusx_enax_t
#define bustype_BDK_DTX_SMMUSX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SMMUSX_ENAX(a,b) "DTX_SMMUSX_ENAX"
#define busnum_BDK_DTX_SMMUSX_ENAX(a,b) (a)
#define arguments_BDK_DTX_SMMUSX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_smmus#_sel#
 *
 * DTX SMMUS Select Register
 */
union bdk_dtx_smmusx_selx
{
    uint64_t u;
    struct bdk_dtx_smmusx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_smmusx_selx_s cn; */
};
typedef union bdk_dtx_smmusx_selx bdk_dtx_smmusx_selx_t;

static inline uint64_t BDK_DTX_SMMUSX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SMMUSX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=2) && (b<=1)))
        return 0x87e0fe988000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_SMMUSX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_SMMUSX_SELX(a,b) bdk_dtx_smmusx_selx_t
#define bustype_BDK_DTX_SMMUSX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SMMUSX_SELX(a,b) "DTX_SMMUSX_SELX"
#define busnum_BDK_DTX_SMMUSX_SELX(a,b) (a)
#define arguments_BDK_DTX_SMMUSX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_sso_bcst_rsp
 *
 * DTX SSO Control Register
 */
union bdk_dtx_sso_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_sso_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_sso_bcst_rsp_s cn; */
};
typedef union bdk_dtx_sso_bcst_rsp bdk_dtx_sso_bcst_rsp_t;

#define BDK_DTX_SSO_BCST_RSP BDK_DTX_SSO_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_SSO_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SSO_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0feb00080ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0feb00080ll;
    __bdk_csr_fatal("DTX_SSO_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_SSO_BCST_RSP bdk_dtx_sso_bcst_rsp_t
#define bustype_BDK_DTX_SSO_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SSO_BCST_RSP "DTX_SSO_BCST_RSP"
#define busnum_BDK_DTX_SSO_BCST_RSP 0
#define arguments_BDK_DTX_SSO_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_sso_ctl
 *
 * DTX SSO Control Register
 */
union bdk_dtx_sso_ctl
{
    uint64_t u;
    struct bdk_dtx_sso_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_sso_ctl_s cn; */
};
typedef union bdk_dtx_sso_ctl bdk_dtx_sso_ctl_t;

#define BDK_DTX_SSO_CTL BDK_DTX_SSO_CTL_FUNC()
static inline uint64_t BDK_DTX_SSO_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SSO_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0feb00060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0feb00060ll;
    __bdk_csr_fatal("DTX_SSO_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_SSO_CTL bdk_dtx_sso_ctl_t
#define bustype_BDK_DTX_SSO_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SSO_CTL "DTX_SSO_CTL"
#define busnum_BDK_DTX_SSO_CTL 0
#define arguments_BDK_DTX_SSO_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_sso_dat#
 *
 * DTX SSO Raw Data Register
 */
union bdk_dtx_sso_datx
{
    uint64_t u;
    struct bdk_dtx_sso_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_sso_datx_s cn; */
};
typedef union bdk_dtx_sso_datx bdk_dtx_sso_datx_t;

static inline uint64_t BDK_DTX_SSO_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SSO_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb00040ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb00040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_SSO_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_SSO_DATX(a) bdk_dtx_sso_datx_t
#define bustype_BDK_DTX_SSO_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SSO_DATX(a) "DTX_SSO_DATX"
#define busnum_BDK_DTX_SSO_DATX(a) (a)
#define arguments_BDK_DTX_SSO_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_sso_ena#
 *
 * DTX SSO Data Enable Register
 */
union bdk_dtx_sso_enax
{
    uint64_t u;
    struct bdk_dtx_sso_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_sso_enax_s cn; */
};
typedef union bdk_dtx_sso_enax bdk_dtx_sso_enax_t;

static inline uint64_t BDK_DTX_SSO_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SSO_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb00020ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb00020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_SSO_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_SSO_ENAX(a) bdk_dtx_sso_enax_t
#define bustype_BDK_DTX_SSO_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SSO_ENAX(a) "DTX_SSO_ENAX"
#define busnum_BDK_DTX_SSO_ENAX(a) (a)
#define arguments_BDK_DTX_SSO_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_sso_sel#
 *
 * DTX SSO Select Register
 */
union bdk_dtx_sso_selx
{
    uint64_t u;
    struct bdk_dtx_sso_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_sso_selx_s cn; */
};
typedef union bdk_dtx_sso_selx bdk_dtx_sso_selx_t;

static inline uint64_t BDK_DTX_SSO_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_SSO_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb00000ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb00000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_SSO_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_SSO_SELX(a) bdk_dtx_sso_selx_t
#define bustype_BDK_DTX_SSO_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_SSO_SELX(a) "DTX_SSO_SELX"
#define busnum_BDK_DTX_SSO_SELX(a) (a)
#define arguments_BDK_DTX_SSO_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_tim_bcst_rsp
 *
 * DTX TIM Control Register
 */
union bdk_dtx_tim_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_tim_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_tim_bcst_rsp_s cn; */
};
typedef union bdk_dtx_tim_bcst_rsp bdk_dtx_tim_bcst_rsp_t;

#define BDK_DTX_TIM_BCST_RSP BDK_DTX_TIM_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_TIM_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_TIM_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0feac0080ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0feac0080ll;
    __bdk_csr_fatal("DTX_TIM_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_TIM_BCST_RSP bdk_dtx_tim_bcst_rsp_t
#define bustype_BDK_DTX_TIM_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_TIM_BCST_RSP "DTX_TIM_BCST_RSP"
#define busnum_BDK_DTX_TIM_BCST_RSP 0
#define arguments_BDK_DTX_TIM_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_tim_ctl
 *
 * DTX TIM Control Register
 */
union bdk_dtx_tim_ctl
{
    uint64_t u;
    struct bdk_dtx_tim_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_tim_ctl_s cn; */
};
typedef union bdk_dtx_tim_ctl bdk_dtx_tim_ctl_t;

#define BDK_DTX_TIM_CTL BDK_DTX_TIM_CTL_FUNC()
static inline uint64_t BDK_DTX_TIM_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_TIM_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0feac0060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0feac0060ll;
    __bdk_csr_fatal("DTX_TIM_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_TIM_CTL bdk_dtx_tim_ctl_t
#define bustype_BDK_DTX_TIM_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_TIM_CTL "DTX_TIM_CTL"
#define busnum_BDK_DTX_TIM_CTL 0
#define arguments_BDK_DTX_TIM_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_tim_dat#
 *
 * DTX TIM Raw Data Register
 */
union bdk_dtx_tim_datx
{
    uint64_t u;
    struct bdk_dtx_tim_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_tim_datx_s cn; */
};
typedef union bdk_dtx_tim_datx bdk_dtx_tim_datx_t;

static inline uint64_t BDK_DTX_TIM_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_TIM_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feac0040ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feac0040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_TIM_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_TIM_DATX(a) bdk_dtx_tim_datx_t
#define bustype_BDK_DTX_TIM_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_TIM_DATX(a) "DTX_TIM_DATX"
#define busnum_BDK_DTX_TIM_DATX(a) (a)
#define arguments_BDK_DTX_TIM_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_tim_ena#
 *
 * DTX TIM Data Enable Register
 */
union bdk_dtx_tim_enax
{
    uint64_t u;
    struct bdk_dtx_tim_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_tim_enax_s cn; */
};
typedef union bdk_dtx_tim_enax bdk_dtx_tim_enax_t;

static inline uint64_t BDK_DTX_TIM_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_TIM_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feac0020ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feac0020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_TIM_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_TIM_ENAX(a) bdk_dtx_tim_enax_t
#define bustype_BDK_DTX_TIM_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_TIM_ENAX(a) "DTX_TIM_ENAX"
#define busnum_BDK_DTX_TIM_ENAX(a) (a)
#define arguments_BDK_DTX_TIM_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_tim_sel#
 *
 * DTX TIM Select Register
 */
union bdk_dtx_tim_selx
{
    uint64_t u;
    struct bdk_dtx_tim_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_tim_selx_s cn; */
};
typedef union bdk_dtx_tim_selx bdk_dtx_tim_selx_t;

static inline uint64_t BDK_DTX_TIM_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_TIM_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feac0000ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feac0000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_TIM_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_TIM_SELX(a) bdk_dtx_tim_selx_t
#define bustype_BDK_DTX_TIM_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_TIM_SELX(a) "DTX_TIM_SELX"
#define busnum_BDK_DTX_TIM_SELX(a) (a)
#define arguments_BDK_DTX_TIM_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_tns_bcst_rsp
 *
 * DTX TNS Control Register
 */
union bdk_dtx_tns_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_tns_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_tns_bcst_rsp_s cn; */
};
typedef union bdk_dtx_tns_bcst_rsp bdk_dtx_tns_bcst_rsp_t;

#define BDK_DTX_TNS_BCST_RSP BDK_DTX_TNS_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_TNS_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_TNS_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0fea10080ll;
    __bdk_csr_fatal("DTX_TNS_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_TNS_BCST_RSP bdk_dtx_tns_bcst_rsp_t
#define bustype_BDK_DTX_TNS_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_TNS_BCST_RSP "DTX_TNS_BCST_RSP"
#define busnum_BDK_DTX_TNS_BCST_RSP 0
#define arguments_BDK_DTX_TNS_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_tns_ctl
 *
 * DTX TNS Control Register
 */
union bdk_dtx_tns_ctl
{
    uint64_t u;
    struct bdk_dtx_tns_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_tns_ctl_s cn; */
};
typedef union bdk_dtx_tns_ctl bdk_dtx_tns_ctl_t;

#define BDK_DTX_TNS_CTL BDK_DTX_TNS_CTL_FUNC()
static inline uint64_t BDK_DTX_TNS_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_TNS_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0fea10060ll;
    __bdk_csr_fatal("DTX_TNS_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_TNS_CTL bdk_dtx_tns_ctl_t
#define bustype_BDK_DTX_TNS_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_TNS_CTL "DTX_TNS_CTL"
#define busnum_BDK_DTX_TNS_CTL 0
#define arguments_BDK_DTX_TNS_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_tns_dat#
 *
 * DTX TNS Raw Data Register
 */
union bdk_dtx_tns_datx
{
    uint64_t u;
    struct bdk_dtx_tns_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_tns_datx_s cn; */
};
typedef union bdk_dtx_tns_datx bdk_dtx_tns_datx_t;

static inline uint64_t BDK_DTX_TNS_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_TNS_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fea10040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_TNS_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_TNS_DATX(a) bdk_dtx_tns_datx_t
#define bustype_BDK_DTX_TNS_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_TNS_DATX(a) "DTX_TNS_DATX"
#define busnum_BDK_DTX_TNS_DATX(a) (a)
#define arguments_BDK_DTX_TNS_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_tns_ena#
 *
 * DTX TNS Data Enable Register
 */
union bdk_dtx_tns_enax
{
    uint64_t u;
    struct bdk_dtx_tns_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_tns_enax_s cn; */
};
typedef union bdk_dtx_tns_enax bdk_dtx_tns_enax_t;

static inline uint64_t BDK_DTX_TNS_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_TNS_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fea10020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_TNS_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_TNS_ENAX(a) bdk_dtx_tns_enax_t
#define bustype_BDK_DTX_TNS_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_TNS_ENAX(a) "DTX_TNS_ENAX"
#define busnum_BDK_DTX_TNS_ENAX(a) (a)
#define arguments_BDK_DTX_TNS_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_tns_sel#
 *
 * DTX TNS Select Register
 */
union bdk_dtx_tns_selx
{
    uint64_t u;
    struct bdk_dtx_tns_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_tns_selx_s cn; */
};
typedef union bdk_dtx_tns_selx bdk_dtx_tns_selx_t;

static inline uint64_t BDK_DTX_TNS_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_TNS_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fea10000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_TNS_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_TNS_SELX(a) bdk_dtx_tns_selx_t
#define bustype_BDK_DTX_TNS_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_TNS_SELX(a) "DTX_TNS_SELX"
#define busnum_BDK_DTX_TNS_SELX(a) (a)
#define arguments_BDK_DTX_TNS_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_uaa#_bcst_rsp
 *
 * DTX UAA Control Register
 */
union bdk_dtx_uaax_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_uaax_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_uaax_bcst_rsp_s cn; */
};
typedef union bdk_dtx_uaax_bcst_rsp bdk_dtx_uaax_bcst_rsp_t;

static inline uint64_t BDK_DTX_UAAX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_UAAX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0fe140080ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e0fe140080ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe120080ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e0fe140080ll + 0x8000ll * ((a) & 0x7);
    __bdk_csr_fatal("DTX_UAAX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_UAAX_BCST_RSP(a) bdk_dtx_uaax_bcst_rsp_t
#define bustype_BDK_DTX_UAAX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_UAAX_BCST_RSP(a) "DTX_UAAX_BCST_RSP"
#define busnum_BDK_DTX_UAAX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_UAAX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_uaa#_ctl
 *
 * DTX UAA Control Register
 */
union bdk_dtx_uaax_ctl
{
    uint64_t u;
    struct bdk_dtx_uaax_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_uaax_ctl_s cn; */
};
typedef union bdk_dtx_uaax_ctl bdk_dtx_uaax_ctl_t;

static inline uint64_t BDK_DTX_UAAX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_UAAX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0fe140060ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e0fe140060ll + 0x8000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe120060ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e0fe140060ll + 0x8000ll * ((a) & 0x7);
    __bdk_csr_fatal("DTX_UAAX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_UAAX_CTL(a) bdk_dtx_uaax_ctl_t
#define bustype_BDK_DTX_UAAX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_UAAX_CTL(a) "DTX_UAAX_CTL"
#define busnum_BDK_DTX_UAAX_CTL(a) (a)
#define arguments_BDK_DTX_UAAX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_uaa#_dat#
 *
 * DTX UAA Raw Data Register
 */
union bdk_dtx_uaax_datx
{
    uint64_t u;
    struct bdk_dtx_uaax_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_uaax_datx_s cn; */
};
typedef union bdk_dtx_uaax_datx bdk_dtx_uaax_datx_t;

static inline uint64_t BDK_DTX_UAAX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_UAAX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe140040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe140040ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe120040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=7) && (b<=1)))
        return 0x87e0fe140040ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_UAAX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_UAAX_DATX(a,b) bdk_dtx_uaax_datx_t
#define bustype_BDK_DTX_UAAX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_UAAX_DATX(a,b) "DTX_UAAX_DATX"
#define busnum_BDK_DTX_UAAX_DATX(a,b) (a)
#define arguments_BDK_DTX_UAAX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_uaa#_ena#
 *
 * DTX UAA Data Enable Register
 */
union bdk_dtx_uaax_enax
{
    uint64_t u;
    struct bdk_dtx_uaax_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_uaax_enax_s cn; */
};
typedef union bdk_dtx_uaax_enax bdk_dtx_uaax_enax_t;

static inline uint64_t BDK_DTX_UAAX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_UAAX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe140020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe140020ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe120020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=7) && (b<=1)))
        return 0x87e0fe140020ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_UAAX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_UAAX_ENAX(a,b) bdk_dtx_uaax_enax_t
#define bustype_BDK_DTX_UAAX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_UAAX_ENAX(a,b) "DTX_UAAX_ENAX"
#define busnum_BDK_DTX_UAAX_ENAX(a,b) (a)
#define arguments_BDK_DTX_UAAX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_uaa#_sel#
 *
 * DTX UAA Select Register
 */
union bdk_dtx_uaax_selx
{
    uint64_t u;
    struct bdk_dtx_uaax_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_uaax_selx_s cn; */
};
typedef union bdk_dtx_uaax_selx bdk_dtx_uaax_selx_t;

static inline uint64_t BDK_DTX_UAAX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_UAAX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe140000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b<=1)))
        return 0x87e0fe140000ll + 0x8000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0fe120000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=7) && (b<=1)))
        return 0x87e0fe140000ll + 0x8000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_UAAX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_UAAX_SELX(a,b) bdk_dtx_uaax_selx_t
#define bustype_BDK_DTX_UAAX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_UAAX_SELX(a,b) "DTX_UAAX_SELX"
#define busnum_BDK_DTX_UAAX_SELX(a,b) (a)
#define arguments_BDK_DTX_UAAX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_usbdrd#_bcst_rsp
 *
 * DTX USBDRD Control Register
 */
union bdk_dtx_usbdrdx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_usbdrdx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_usbdrdx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_usbdrdx_bcst_rsp bdk_dtx_usbdrdx_bcst_rsp_t;

static inline uint64_t BDK_DTX_USBDRDX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_USBDRDX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0feb40080ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb40080ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb40080ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_USBDRDX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_USBDRDX_BCST_RSP(a) bdk_dtx_usbdrdx_bcst_rsp_t
#define bustype_BDK_DTX_USBDRDX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_USBDRDX_BCST_RSP(a) "DTX_USBDRDX_BCST_RSP"
#define busnum_BDK_DTX_USBDRDX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_USBDRDX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_usbdrd#_ctl
 *
 * DTX USBDRD Control Register
 */
union bdk_dtx_usbdrdx_ctl
{
    uint64_t u;
    struct bdk_dtx_usbdrdx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_usbdrdx_ctl_s cn; */
};
typedef union bdk_dtx_usbdrdx_ctl bdk_dtx_usbdrdx_ctl_t;

static inline uint64_t BDK_DTX_USBDRDX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_USBDRDX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0feb40060ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0feb40060ll + 0x8000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0feb40060ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_USBDRDX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_USBDRDX_CTL(a) bdk_dtx_usbdrdx_ctl_t
#define bustype_BDK_DTX_USBDRDX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_USBDRDX_CTL(a) "DTX_USBDRDX_CTL"
#define busnum_BDK_DTX_USBDRDX_CTL(a) (a)
#define arguments_BDK_DTX_USBDRDX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_usbdrd#_dat#
 *
 * DTX USBDRD Raw Data Register
 */
union bdk_dtx_usbdrdx_datx
{
    uint64_t u;
    struct bdk_dtx_usbdrdx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_usbdrdx_datx_s cn; */
};
typedef union bdk_dtx_usbdrdx_datx bdk_dtx_usbdrdx_datx_t;

static inline uint64_t BDK_DTX_USBDRDX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_USBDRDX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=1)))
        return 0x87e0feb40040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0feb40040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0feb40040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_USBDRDX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_USBDRDX_DATX(a,b) bdk_dtx_usbdrdx_datx_t
#define bustype_BDK_DTX_USBDRDX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_USBDRDX_DATX(a,b) "DTX_USBDRDX_DATX"
#define busnum_BDK_DTX_USBDRDX_DATX(a,b) (a)
#define arguments_BDK_DTX_USBDRDX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_usbdrd#_ena#
 *
 * DTX USBDRD Data Enable Register
 */
union bdk_dtx_usbdrdx_enax
{
    uint64_t u;
    struct bdk_dtx_usbdrdx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_usbdrdx_enax_s cn; */
};
typedef union bdk_dtx_usbdrdx_enax bdk_dtx_usbdrdx_enax_t;

static inline uint64_t BDK_DTX_USBDRDX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_USBDRDX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=1)))
        return 0x87e0feb40020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0feb40020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0feb40020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_USBDRDX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_USBDRDX_ENAX(a,b) bdk_dtx_usbdrdx_enax_t
#define bustype_BDK_DTX_USBDRDX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_USBDRDX_ENAX(a,b) "DTX_USBDRDX_ENAX"
#define busnum_BDK_DTX_USBDRDX_ENAX(a,b) (a)
#define arguments_BDK_DTX_USBDRDX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_usbdrd#_sel#
 *
 * DTX USBDRD Select Register
 */
union bdk_dtx_usbdrdx_selx
{
    uint64_t u;
    struct bdk_dtx_usbdrdx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_usbdrdx_selx_s cn; */
};
typedef union bdk_dtx_usbdrdx_selx bdk_dtx_usbdrdx_selx_t;

static inline uint64_t BDK_DTX_USBDRDX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_USBDRDX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=1)))
        return 0x87e0feb40000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b<=1)))
        return 0x87e0feb40000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0feb40000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_USBDRDX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_USBDRDX_SELX(a,b) bdk_dtx_usbdrdx_selx_t
#define bustype_BDK_DTX_USBDRDX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_USBDRDX_SELX(a,b) "DTX_USBDRDX_SELX"
#define busnum_BDK_DTX_USBDRDX_SELX(a,b) (a)
#define arguments_BDK_DTX_USBDRDX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_usbh#_bcst_rsp
 *
 * DTX USBH Control Register
 */
union bdk_dtx_usbhx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_usbhx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_usbhx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_usbhx_bcst_rsp bdk_dtx_usbhx_bcst_rsp_t;

static inline uint64_t BDK_DTX_USBHX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_USBHX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0feb40080ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_USBHX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_USBHX_BCST_RSP(a) bdk_dtx_usbhx_bcst_rsp_t
#define bustype_BDK_DTX_USBHX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_USBHX_BCST_RSP(a) "DTX_USBHX_BCST_RSP"
#define busnum_BDK_DTX_USBHX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_USBHX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_usbh#_ctl
 *
 * DTX USBH Control Register
 */
union bdk_dtx_usbhx_ctl
{
    uint64_t u;
    struct bdk_dtx_usbhx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_usbhx_ctl_s cn; */
};
typedef union bdk_dtx_usbhx_ctl bdk_dtx_usbhx_ctl_t;

static inline uint64_t BDK_DTX_USBHX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_USBHX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0feb40060ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_USBHX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_USBHX_CTL(a) bdk_dtx_usbhx_ctl_t
#define bustype_BDK_DTX_USBHX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_USBHX_CTL(a) "DTX_USBHX_CTL"
#define busnum_BDK_DTX_USBHX_CTL(a) (a)
#define arguments_BDK_DTX_USBHX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_usbh#_dat#
 *
 * DTX USBH Raw Data Register
 */
union bdk_dtx_usbhx_datx
{
    uint64_t u;
    struct bdk_dtx_usbhx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_usbhx_datx_s cn; */
};
typedef union bdk_dtx_usbhx_datx bdk_dtx_usbhx_datx_t;

static inline uint64_t BDK_DTX_USBHX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_USBHX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0feb40040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_USBHX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_USBHX_DATX(a,b) bdk_dtx_usbhx_datx_t
#define bustype_BDK_DTX_USBHX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_USBHX_DATX(a,b) "DTX_USBHX_DATX"
#define busnum_BDK_DTX_USBHX_DATX(a,b) (a)
#define arguments_BDK_DTX_USBHX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_usbh#_ena#
 *
 * DTX USBH Data Enable Register
 */
union bdk_dtx_usbhx_enax
{
    uint64_t u;
    struct bdk_dtx_usbhx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_usbhx_enax_s cn; */
};
typedef union bdk_dtx_usbhx_enax bdk_dtx_usbhx_enax_t;

static inline uint64_t BDK_DTX_USBHX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_USBHX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0feb40020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_USBHX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_USBHX_ENAX(a,b) bdk_dtx_usbhx_enax_t
#define bustype_BDK_DTX_USBHX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_USBHX_ENAX(a,b) "DTX_USBHX_ENAX"
#define busnum_BDK_DTX_USBHX_ENAX(a,b) (a)
#define arguments_BDK_DTX_USBHX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_usbh#_sel#
 *
 * DTX USBH Select Register
 */
union bdk_dtx_usbhx_selx
{
    uint64_t u;
    struct bdk_dtx_usbhx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_usbhx_selx_s cn; */
};
typedef union bdk_dtx_usbhx_selx bdk_dtx_usbhx_selx_t;

static inline uint64_t BDK_DTX_USBHX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_USBHX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e0feb40000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_USBHX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_USBHX_SELX(a,b) bdk_dtx_usbhx_selx_t
#define bustype_BDK_DTX_USBHX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_USBHX_SELX(a,b) "DTX_USBHX_SELX"
#define busnum_BDK_DTX_USBHX_SELX(a,b) (a)
#define arguments_BDK_DTX_USBHX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_xcp#_bcst_rsp
 *
 * DTX XCP Control Register
 */
union bdk_dtx_xcpx_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_xcpx_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_xcpx_bcst_rsp_s cn; */
};
typedef union bdk_dtx_xcpx_bcst_rsp bdk_dtx_xcpx_bcst_rsp_t;

static inline uint64_t BDK_DTX_XCPX_BCST_RSP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_XCPX_BCST_RSP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe960080ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_XCPX_BCST_RSP", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_XCPX_BCST_RSP(a) bdk_dtx_xcpx_bcst_rsp_t
#define bustype_BDK_DTX_XCPX_BCST_RSP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_XCPX_BCST_RSP(a) "DTX_XCPX_BCST_RSP"
#define busnum_BDK_DTX_XCPX_BCST_RSP(a) (a)
#define arguments_BDK_DTX_XCPX_BCST_RSP(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_xcp#_ctl
 *
 * DTX XCP Control Register
 */
union bdk_dtx_xcpx_ctl
{
    uint64_t u;
    struct bdk_dtx_xcpx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_xcpx_ctl_s cn; */
};
typedef union bdk_dtx_xcpx_ctl bdk_dtx_xcpx_ctl_t;

static inline uint64_t BDK_DTX_XCPX_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_XCPX_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe960060ll + 0x8000ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_XCPX_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_XCPX_CTL(a) bdk_dtx_xcpx_ctl_t
#define bustype_BDK_DTX_XCPX_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_XCPX_CTL(a) "DTX_XCPX_CTL"
#define busnum_BDK_DTX_XCPX_CTL(a) (a)
#define arguments_BDK_DTX_XCPX_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_xcp#_dat#
 *
 * DTX XCP Raw Data Register
 */
union bdk_dtx_xcpx_datx
{
    uint64_t u;
    struct bdk_dtx_xcpx_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_xcpx_datx_s cn; */
};
typedef union bdk_dtx_xcpx_datx bdk_dtx_xcpx_datx_t;

static inline uint64_t BDK_DTX_XCPX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_XCPX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe960040ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_XCPX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_XCPX_DATX(a,b) bdk_dtx_xcpx_datx_t
#define bustype_BDK_DTX_XCPX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_XCPX_DATX(a,b) "DTX_XCPX_DATX"
#define busnum_BDK_DTX_XCPX_DATX(a,b) (a)
#define arguments_BDK_DTX_XCPX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_xcp#_ena#
 *
 * DTX XCP Data Enable Register
 */
union bdk_dtx_xcpx_enax
{
    uint64_t u;
    struct bdk_dtx_xcpx_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_xcpx_enax_s cn; */
};
typedef union bdk_dtx_xcpx_enax bdk_dtx_xcpx_enax_t;

static inline uint64_t BDK_DTX_XCPX_ENAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_XCPX_ENAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe960020ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_XCPX_ENAX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_XCPX_ENAX(a,b) bdk_dtx_xcpx_enax_t
#define bustype_BDK_DTX_XCPX_ENAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_XCPX_ENAX(a,b) "DTX_XCPX_ENAX"
#define busnum_BDK_DTX_XCPX_ENAX(a,b) (a)
#define arguments_BDK_DTX_XCPX_ENAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_xcp#_sel#
 *
 * DTX XCP Select Register
 */
union bdk_dtx_xcpx_selx
{
    uint64_t u;
    struct bdk_dtx_xcpx_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_xcpx_selx_s cn; */
};
typedef union bdk_dtx_xcpx_selx bdk_dtx_xcpx_selx_t;

static inline uint64_t BDK_DTX_XCPX_SELX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_XCPX_SELX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=1) && (b<=1)))
        return 0x87e0fe960000ll + 0x8000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("DTX_XCPX_SELX", 2, a, b, 0, 0);
}

#define typedef_BDK_DTX_XCPX_SELX(a,b) bdk_dtx_xcpx_selx_t
#define bustype_BDK_DTX_XCPX_SELX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_XCPX_SELX(a,b) "DTX_XCPX_SELX"
#define busnum_BDK_DTX_XCPX_SELX(a,b) (a)
#define arguments_BDK_DTX_XCPX_SELX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) dtx_xcv_bcst_rsp
 *
 * DTX XCV Control Register
 */
union bdk_dtx_xcv_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_xcv_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_xcv_bcst_rsp_s cn; */
};
typedef union bdk_dtx_xcv_bcst_rsp bdk_dtx_xcv_bcst_rsp_t;

#define BDK_DTX_XCV_BCST_RSP BDK_DTX_XCV_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_XCV_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_XCV_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 0x87e0fe6d8080ll;
    __bdk_csr_fatal("DTX_XCV_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_XCV_BCST_RSP bdk_dtx_xcv_bcst_rsp_t
#define bustype_BDK_DTX_XCV_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_XCV_BCST_RSP "DTX_XCV_BCST_RSP"
#define busnum_BDK_DTX_XCV_BCST_RSP 0
#define arguments_BDK_DTX_XCV_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_xcv_ctl
 *
 * DTX XCV Control Register
 */
union bdk_dtx_xcv_ctl
{
    uint64_t u;
    struct bdk_dtx_xcv_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_xcv_ctl_s cn; */
};
typedef union bdk_dtx_xcv_ctl bdk_dtx_xcv_ctl_t;

#define BDK_DTX_XCV_CTL BDK_DTX_XCV_CTL_FUNC()
static inline uint64_t BDK_DTX_XCV_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_XCV_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 0x87e0fe6d8060ll;
    __bdk_csr_fatal("DTX_XCV_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_XCV_CTL bdk_dtx_xcv_ctl_t
#define bustype_BDK_DTX_XCV_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_XCV_CTL "DTX_XCV_CTL"
#define busnum_BDK_DTX_XCV_CTL 0
#define arguments_BDK_DTX_XCV_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_xcv_dat#
 *
 * DTX XCV Raw Data Register
 */
union bdk_dtx_xcv_datx
{
    uint64_t u;
    struct bdk_dtx_xcv_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_xcv_datx_s cn; */
};
typedef union bdk_dtx_xcv_datx bdk_dtx_xcv_datx_t;

static inline uint64_t BDK_DTX_XCV_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_XCV_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0fe6d8040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_XCV_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_XCV_DATX(a) bdk_dtx_xcv_datx_t
#define bustype_BDK_DTX_XCV_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_XCV_DATX(a) "DTX_XCV_DATX"
#define busnum_BDK_DTX_XCV_DATX(a) (a)
#define arguments_BDK_DTX_XCV_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_xcv_ena#
 *
 * DTX XCV Data Enable Register
 */
union bdk_dtx_xcv_enax
{
    uint64_t u;
    struct bdk_dtx_xcv_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_xcv_enax_s cn; */
};
typedef union bdk_dtx_xcv_enax bdk_dtx_xcv_enax_t;

static inline uint64_t BDK_DTX_XCV_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_XCV_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0fe6d8020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_XCV_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_XCV_ENAX(a) bdk_dtx_xcv_enax_t
#define bustype_BDK_DTX_XCV_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_XCV_ENAX(a) "DTX_XCV_ENAX"
#define busnum_BDK_DTX_XCV_ENAX(a) (a)
#define arguments_BDK_DTX_XCV_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_xcv_sel#
 *
 * DTX XCV Select Register
 */
union bdk_dtx_xcv_selx
{
    uint64_t u;
    struct bdk_dtx_xcv_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_xcv_selx_s cn; */
};
typedef union bdk_dtx_xcv_selx bdk_dtx_xcv_selx_t;

static inline uint64_t BDK_DTX_XCV_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_XCV_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0fe6d8000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_XCV_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_XCV_SELX(a) bdk_dtx_xcv_selx_t
#define bustype_BDK_DTX_XCV_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_XCV_SELX(a) "DTX_XCV_SELX"
#define busnum_BDK_DTX_XCV_SELX(a) (a)
#define arguments_BDK_DTX_XCV_SELX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_zip_bcst_rsp
 *
 * DTX ZIP Control Register
 */
union bdk_dtx_zip_bcst_rsp
{
    uint64_t u;
    struct bdk_dtx_zip_bcst_rsp_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 1;  /**< [  0:  0](R/W) Enable this DTX instance as the responder to DTX broadcast read/write operations. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_zip_bcst_rsp_s cn; */
};
typedef union bdk_dtx_zip_bcst_rsp bdk_dtx_zip_bcst_rsp_t;

#define BDK_DTX_ZIP_BCST_RSP BDK_DTX_ZIP_BCST_RSP_FUNC()
static inline uint64_t BDK_DTX_ZIP_BCST_RSP_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_ZIP_BCST_RSP_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0fe9c0080ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0fe9c0080ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0fe9c0080ll;
    __bdk_csr_fatal("DTX_ZIP_BCST_RSP", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_ZIP_BCST_RSP bdk_dtx_zip_bcst_rsp_t
#define bustype_BDK_DTX_ZIP_BCST_RSP BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_ZIP_BCST_RSP "DTX_ZIP_BCST_RSP"
#define busnum_BDK_DTX_ZIP_BCST_RSP 0
#define arguments_BDK_DTX_ZIP_BCST_RSP -1,-1,-1,-1

/**
 * Register (RSL) dtx_zip_ctl
 *
 * DTX ZIP Control Register
 */
union bdk_dtx_zip_ctl
{
    uint64_t u;
    struct bdk_dtx_zip_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_2_3          : 2;
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
#else /* Word 0 - Little Endian */
        uint64_t swap                  : 1;  /**< [  0:  0](R/W) Swap the high and low 36-bit debug bus outputs. */
        uint64_t echoen                : 1;  /**< [  1:  1](R/W) Drive debug bus with the value in DTX_MIO_ENA(0..1) instead of normal block
                                                                 debug data. Not applicable when software directly reads the DAT(0..1) registers.
                                                                 For diagnostic use only. */
        uint64_t reserved_2_3          : 2;
        uint64_t active                : 1;  /**< [  4:  4](R/W) Force block's gated clocks on, so that the state of idle signals may be captured. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_zip_ctl_s cn; */
};
typedef union bdk_dtx_zip_ctl bdk_dtx_zip_ctl_t;

#define BDK_DTX_ZIP_CTL BDK_DTX_ZIP_CTL_FUNC()
static inline uint64_t BDK_DTX_ZIP_CTL_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_ZIP_CTL_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 0x87e0fe9c0060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 0x87e0fe9c0060ll;
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e0fe9c0060ll;
    __bdk_csr_fatal("DTX_ZIP_CTL", 0, 0, 0, 0, 0);
}

#define typedef_BDK_DTX_ZIP_CTL bdk_dtx_zip_ctl_t
#define bustype_BDK_DTX_ZIP_CTL BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_ZIP_CTL "DTX_ZIP_CTL"
#define busnum_BDK_DTX_ZIP_CTL 0
#define arguments_BDK_DTX_ZIP_CTL -1,-1,-1,-1

/**
 * Register (RSL) dtx_zip_dat#
 *
 * DTX ZIP Raw Data Register
 */
union bdk_dtx_zip_datx
{
    uint64_t u;
    struct bdk_dtx_zip_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw debug data captured by the DTX before the ENA is applied. This gives the
                                                                 ability to peek into blocks during an OCLA capture without OCLA reconfiguration. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_zip_datx_s cn; */
};
typedef union bdk_dtx_zip_datx bdk_dtx_zip_datx_t;

static inline uint64_t BDK_DTX_ZIP_DATX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_ZIP_DATX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe9c0040ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe9c0040ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe9c0040ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_ZIP_DATX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_ZIP_DATX(a) bdk_dtx_zip_datx_t
#define bustype_BDK_DTX_ZIP_DATX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_ZIP_DATX(a) "DTX_ZIP_DATX"
#define busnum_BDK_DTX_ZIP_DATX(a) (a)
#define arguments_BDK_DTX_ZIP_DATX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_zip_ena#
 *
 * DTX ZIP Data Enable Register
 */
union bdk_dtx_zip_enax
{
    uint64_t u;
    struct bdk_dtx_zip_enax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
#else /* Word 0 - Little Endian */
        uint64_t ena                   : 36; /**< [ 35:  0](R/W) Output enable vector of which bits to drive onto the low/high 36-bit debug
                                                                 buses. Normally only one block will drive each bit. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_zip_enax_s cn; */
};
typedef union bdk_dtx_zip_enax bdk_dtx_zip_enax_t;

static inline uint64_t BDK_DTX_ZIP_ENAX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_ZIP_ENAX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe9c0020ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe9c0020ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe9c0020ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_ZIP_ENAX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_ZIP_ENAX(a) bdk_dtx_zip_enax_t
#define bustype_BDK_DTX_ZIP_ENAX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_ZIP_ENAX(a) "DTX_ZIP_ENAX"
#define busnum_BDK_DTX_ZIP_ENAX(a) (a)
#define arguments_BDK_DTX_ZIP_ENAX(a) (a),-1,-1,-1

/**
 * Register (RSL) dtx_zip_sel#
 *
 * DTX ZIP Select Register
 */
union bdk_dtx_zip_selx
{
    uint64_t u;
    struct bdk_dtx_zip_selx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_24_63        : 40;
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
#else /* Word 0 - Little Endian */
        uint64_t value                 : 24; /**< [ 23:  0](R/W) Debug select. Selects which signals to drive onto low/high 36-bit debug buses. */
        uint64_t reserved_24_63        : 40;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_dtx_zip_selx_s cn; */
};
typedef union bdk_dtx_zip_selx bdk_dtx_zip_selx_t;

static inline uint64_t BDK_DTX_ZIP_SELX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_DTX_ZIP_SELX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0fe9c0000ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0fe9c0000ll + 8ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e0fe9c0000ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("DTX_ZIP_SELX", 1, a, 0, 0, 0);
}

#define typedef_BDK_DTX_ZIP_SELX(a) bdk_dtx_zip_selx_t
#define bustype_BDK_DTX_ZIP_SELX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_DTX_ZIP_SELX(a) "DTX_ZIP_SELX"
#define busnum_BDK_DTX_ZIP_SELX(a) (a)
#define arguments_BDK_DTX_ZIP_SELX(a) (a),-1,-1,-1

#endif /* __BDK_CSRS_DTX_H__ */
