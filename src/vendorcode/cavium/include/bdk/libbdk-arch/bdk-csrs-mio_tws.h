#ifndef __BDK_CSRS_MIO_TWS_H__
#define __BDK_CSRS_MIO_TWS_H__
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
 * Cavium MIO_TWS.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration mio_tws_bar_e
 *
 * TWSI Base Address Register Enumeration
 * Enumerates the base address registers.
 */
#define BDK_MIO_TWS_BAR_E_MIO_TWSX_PF_BAR0_CN8(a) (0x87e0d0000000ll + 0x1000000ll * (a))
#define BDK_MIO_TWS_BAR_E_MIO_TWSX_PF_BAR0_CN8_SIZE 0x800000ull
#define BDK_MIO_TWS_BAR_E_MIO_TWSX_PF_BAR0_CN9(a) (0x87e0d0000000ll + 0x1000000ll * (a))
#define BDK_MIO_TWS_BAR_E_MIO_TWSX_PF_BAR0_CN9_SIZE 0x10000ull
#define BDK_MIO_TWS_BAR_E_MIO_TWSX_PF_BAR4(a) (0x87e0d0f00000ll + 0x1000000ll * (a))
#define BDK_MIO_TWS_BAR_E_MIO_TWSX_PF_BAR4_SIZE 0x100000ull

/**
 * Enumeration mio_tws_int_vec_e
 *
 * TWSI MSI-X Vector Enumeration
 * Enumerates the MSI-X interrupt vectors.
 */
#define BDK_MIO_TWS_INT_VEC_E_INT_ST (0)

/**
 * Register (RSL) mio_tws#_access_wdog
 *
 * TWSI Watch Dog Register
 * This register contains the watch dog control register.
 */
union bdk_mio_twsx_access_wdog
{
    uint64_t u;
    struct bdk_mio_twsx_access_wdog_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t wdog_cnt              : 32; /**< [ 31:  0](R/W) Number of maximum TCLK clocks (defined by TWSI_CLK control THP) allowed for the
                                                                 TWSI high-level controller (HLC) to stay in one state other than idle
                                                                 state. Hardware will halt the operation if HLC is stuck longer than this delay
                                                                 and MIO_TWS()_INT[ST_INT] interrupt will be generated and error code 0xF0 also
                                                                 reported by MIO_TWS()_SW_TWSI[DATA]. Setting [WDOG_CNT] to 0x0 disables the
                                                                 watch dog function. */
#else /* Word 0 - Little Endian */
        uint64_t wdog_cnt              : 32; /**< [ 31:  0](R/W) Number of maximum TCLK clocks (defined by TWSI_CLK control THP) allowed for the
                                                                 TWSI high-level controller (HLC) to stay in one state other than idle
                                                                 state. Hardware will halt the operation if HLC is stuck longer than this delay
                                                                 and MIO_TWS()_INT[ST_INT] interrupt will be generated and error code 0xF0 also
                                                                 reported by MIO_TWS()_SW_TWSI[DATA]. Setting [WDOG_CNT] to 0x0 disables the
                                                                 watch dog function. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_twsx_access_wdog_s cn; */
};
typedef union bdk_mio_twsx_access_wdog bdk_mio_twsx_access_wdog_t;

static inline uint64_t BDK_MIO_TWSX_ACCESS_WDOG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_ACCESS_WDOG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001040ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_ACCESS_WDOG", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_ACCESS_WDOG(a) bdk_mio_twsx_access_wdog_t
#define bustype_BDK_MIO_TWSX_ACCESS_WDOG(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_ACCESS_WDOG(a) "MIO_TWSX_ACCESS_WDOG"
#define device_bar_BDK_MIO_TWSX_ACCESS_WDOG(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_ACCESS_WDOG(a) (a)
#define arguments_BDK_MIO_TWSX_ACCESS_WDOG(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_clken
 *
 * MIO Clock Enable Register
 * This register controls conditional clocks.
 */
union bdk_mio_twsx_clken
{
    uint64_t u;
    struct bdk_mio_twsx_clken_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t clken                 : 1;  /**< [  0:  0](R/W) Force the TWSI interface conditional clocking to be always on. For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t clken                 : 1;  /**< [  0:  0](R/W) Force the TWSI interface conditional clocking to be always on. For diagnostic use only. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_twsx_clken_s cn; */
};
typedef union bdk_mio_twsx_clken bdk_mio_twsx_clken_t;

static inline uint64_t BDK_MIO_TWSX_CLKEN(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_CLKEN(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001078ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_CLKEN", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_CLKEN(a) bdk_mio_twsx_clken_t
#define bustype_BDK_MIO_TWSX_CLKEN(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_CLKEN(a) "MIO_TWSX_CLKEN"
#define device_bar_BDK_MIO_TWSX_CLKEN(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_CLKEN(a) (a)
#define arguments_BDK_MIO_TWSX_CLKEN(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_const
 *
 * TWSI Constants Register
 * This register contains constants for software discovery.
 */
union bdk_mio_twsx_const
{
    uint64_t u;
    struct bdk_mio_twsx_const_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t ver                   : 4;  /**< [  3:  0](RO) TWSI version.
                                                                 Internal:
                                                                 FIXME spec values.  Make 8 bits? */
#else /* Word 0 - Little Endian */
        uint64_t ver                   : 4;  /**< [  3:  0](RO) TWSI version.
                                                                 Internal:
                                                                 FIXME spec values.  Make 8 bits? */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_twsx_const_s cn; */
};
typedef union bdk_mio_twsx_const bdk_mio_twsx_const_t;

static inline uint64_t BDK_MIO_TWSX_CONST(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_CONST(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0000000ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_CONST", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_CONST(a) bdk_mio_twsx_const_t
#define bustype_BDK_MIO_TWSX_CONST(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_CONST(a) "MIO_TWSX_CONST"
#define device_bar_BDK_MIO_TWSX_CONST(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_CONST(a) (a)
#define arguments_BDK_MIO_TWSX_CONST(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_int
 *
 * TWSI Interrupt Register
 * This register contains the TWSI interrupt-source bits and SDA/SCL override bits.
 */
union bdk_mio_twsx_int
{
    uint64_t u;
    struct bdk_mio_twsx_int_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t scl                   : 1;  /**< [ 11: 11](RO/H) SCL signal. */
        uint64_t sda                   : 1;  /**< [ 10: 10](RO/H) SDA signal. */
        uint64_t scl_ovr               : 1;  /**< [  9:  9](R/W) SCL testing override:
                                                                 0 = Normal operation, SCL bus controlled by TWSI core.
                                                                 1 = Pull SCL low. */
        uint64_t sda_ovr               : 1;  /**< [  8:  8](R/W) SDA testing override:
                                                                 0 = Normal operation, SDA bus controlled by TWSI core.
                                                                 1 = Pull SDA low. */
        uint64_t reserved_4_7          : 4;
        uint64_t block_int             : 1;  /**< [  3:  3](R/W1C/H) BLOCK transaction threshold interrupt. Interrupt fires when the remaining
                                                                 bytes to be sent/received is less than threshold MIO_TWS()_TWSI_BLOCK_CTL[BLOCK_THRESH].
                                                                 Only valid in HLC BlOCK_MODE. Ignored when BLOCK mode is disabled. */
        uint64_t core_int              : 1;  /**< [  2:  2](RO/H) TWSI core interrupt, whenever IFLG is set. Ignored when the HLC is enabled. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1C/H) MIO_TWS()_TWSI_SW register-update interrupt. Ignored when the HLC is disabled. */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1C/H) MIO_TWS()_SW_TWSI register-update interrupt. Ignored when the HLC is disabled. */
#else /* Word 0 - Little Endian */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1C/H) MIO_TWS()_SW_TWSI register-update interrupt. Ignored when the HLC is disabled. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1C/H) MIO_TWS()_TWSI_SW register-update interrupt. Ignored when the HLC is disabled. */
        uint64_t core_int              : 1;  /**< [  2:  2](RO/H) TWSI core interrupt, whenever IFLG is set. Ignored when the HLC is enabled. */
        uint64_t block_int             : 1;  /**< [  3:  3](R/W1C/H) BLOCK transaction threshold interrupt. Interrupt fires when the remaining
                                                                 bytes to be sent/received is less than threshold MIO_TWS()_TWSI_BLOCK_CTL[BLOCK_THRESH].
                                                                 Only valid in HLC BlOCK_MODE. Ignored when BLOCK mode is disabled. */
        uint64_t reserved_4_7          : 4;
        uint64_t sda_ovr               : 1;  /**< [  8:  8](R/W) SDA testing override:
                                                                 0 = Normal operation, SDA bus controlled by TWSI core.
                                                                 1 = Pull SDA low. */
        uint64_t scl_ovr               : 1;  /**< [  9:  9](R/W) SCL testing override:
                                                                 0 = Normal operation, SCL bus controlled by TWSI core.
                                                                 1 = Pull SCL low. */
        uint64_t sda                   : 1;  /**< [ 10: 10](RO/H) SDA signal. */
        uint64_t scl                   : 1;  /**< [ 11: 11](RO/H) SCL signal. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } s;
    struct bdk_mio_twsx_int_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t scl                   : 1;  /**< [ 11: 11](RO/H) SCL signal. */
        uint64_t sda                   : 1;  /**< [ 10: 10](RO/H) SDA signal. */
        uint64_t scl_ovr               : 1;  /**< [  9:  9](R/W) SCL testing override:
                                                                 0 = Normal operation, SCL bus controlled by TWSI core.
                                                                 1 = Pull SCL low. */
        uint64_t sda_ovr               : 1;  /**< [  8:  8](R/W) SDA testing override:
                                                                 0 = Normal operation, SDA bus controlled by TWSI core.
                                                                 1 = Pull SDA low. */
        uint64_t reserved_3_7          : 5;
        uint64_t core_int              : 1;  /**< [  2:  2](RO/H) TWSI core interrupt, whenever IFLG is set. Ignored when the HLC is enabled. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1C/H) MIO_TWS()_TWSI_SW register-update interrupt. Ignored when the HLC is disabled. */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1C/H) MIO_TWS()_SW_TWSI register-update interrupt. Ignored when the HLC is disabled. */
#else /* Word 0 - Little Endian */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1C/H) MIO_TWS()_SW_TWSI register-update interrupt. Ignored when the HLC is disabled. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1C/H) MIO_TWS()_TWSI_SW register-update interrupt. Ignored when the HLC is disabled. */
        uint64_t core_int              : 1;  /**< [  2:  2](RO/H) TWSI core interrupt, whenever IFLG is set. Ignored when the HLC is enabled. */
        uint64_t reserved_3_7          : 5;
        uint64_t sda_ovr               : 1;  /**< [  8:  8](R/W) SDA testing override:
                                                                 0 = Normal operation, SDA bus controlled by TWSI core.
                                                                 1 = Pull SDA low. */
        uint64_t scl_ovr               : 1;  /**< [  9:  9](R/W) SCL testing override:
                                                                 0 = Normal operation, SCL bus controlled by TWSI core.
                                                                 1 = Pull SCL low. */
        uint64_t sda                   : 1;  /**< [ 10: 10](RO/H) SDA signal. */
        uint64_t scl                   : 1;  /**< [ 11: 11](RO/H) SCL signal. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } cn8;
    struct bdk_mio_twsx_int_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t scl                   : 1;  /**< [ 11: 11](RO/H) SCL signal. */
        uint64_t sda                   : 1;  /**< [ 10: 10](RO/H) SDA signal. */
        uint64_t scl_ovr               : 1;  /**< [  9:  9](R/W) SCL testing override:
                                                                 0 = Normal operation, SCL bus controlled by TWSI core.
                                                                 1 = Pull SCL low. */
        uint64_t sda_ovr               : 1;  /**< [  8:  8](R/W) SDA testing override:
                                                                 0 = Normal operation, SDA bus controlled by TWSI core.
                                                                 1 = Pull SDA low. */
        uint64_t reserved_4_7          : 4;
        uint64_t block_int             : 1;  /**< [  3:  3](R/W1C/H) BLOCK transaction threshold interrupt. Interrupt fires when the remaining
                                                                 bytes to be sent/received is less than threshold MIO_TWS()_TWSI_BLOCK_CTL[BLOCK_THRESH].
                                                                 Only valid in HLC BlOCK_MODE. Ignored when BLOCK mode is disabled. */
        uint64_t core_int              : 1;  /**< [  2:  2](RO/H) TWSI core interrupt, whenever IFLG is set. Ignored when the HLC is enabled.
                                                                 In order to clear [CORE_INT], software needs to write 0 to TWSI_CTL[IFLG]. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1C/H) MIO_TWS()_TWSI_SW register-update interrupt. Only valid in HLC mode.
                                                                 Ignored when HLC is disabled. */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1C/H) MIO_TWS()_SW_TWSI register-update interrupt.  Only valid in HLC mode.
                                                                 Ignored when HLC is disabled. */
#else /* Word 0 - Little Endian */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1C/H) MIO_TWS()_SW_TWSI register-update interrupt.  Only valid in HLC mode.
                                                                 Ignored when HLC is disabled. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1C/H) MIO_TWS()_TWSI_SW register-update interrupt. Only valid in HLC mode.
                                                                 Ignored when HLC is disabled. */
        uint64_t core_int              : 1;  /**< [  2:  2](RO/H) TWSI core interrupt, whenever IFLG is set. Ignored when the HLC is enabled.
                                                                 In order to clear [CORE_INT], software needs to write 0 to TWSI_CTL[IFLG]. */
        uint64_t block_int             : 1;  /**< [  3:  3](R/W1C/H) BLOCK transaction threshold interrupt. Interrupt fires when the remaining
                                                                 bytes to be sent/received is less than threshold MIO_TWS()_TWSI_BLOCK_CTL[BLOCK_THRESH].
                                                                 Only valid in HLC BlOCK_MODE. Ignored when BLOCK mode is disabled. */
        uint64_t reserved_4_7          : 4;
        uint64_t sda_ovr               : 1;  /**< [  8:  8](R/W) SDA testing override:
                                                                 0 = Normal operation, SDA bus controlled by TWSI core.
                                                                 1 = Pull SDA low. */
        uint64_t scl_ovr               : 1;  /**< [  9:  9](R/W) SCL testing override:
                                                                 0 = Normal operation, SCL bus controlled by TWSI core.
                                                                 1 = Pull SCL low. */
        uint64_t sda                   : 1;  /**< [ 10: 10](RO/H) SDA signal. */
        uint64_t scl                   : 1;  /**< [ 11: 11](RO/H) SCL signal. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_mio_twsx_int bdk_mio_twsx_int_t;

static inline uint64_t BDK_MIO_TWSX_INT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_INT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0d0001010ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0d0001010ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=5))
        return 0x87e0d0001010ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001010ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_INT", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_INT(a) bdk_mio_twsx_int_t
#define bustype_BDK_MIO_TWSX_INT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_INT(a) "MIO_TWSX_INT"
#define device_bar_BDK_MIO_TWSX_INT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_INT(a) (a)
#define arguments_BDK_MIO_TWSX_INT(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_int_ena_w1c
 *
 * TWSI Interrupt Enable Clear Register
 */
union bdk_mio_twsx_int_ena_w1c
{
    uint64_t u;
    struct bdk_mio_twsx_int_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t block_int             : 1;  /**< [  3:  3](R/W1C/H) Reads or clears MIO_TWS()_INT[BLOCK_INT]. */
        uint64_t core_int              : 1;  /**< [  2:  2](R/W1C/H) Reads or clears MIO_TWS()_INT[CORE_INT]. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1C/H) Reads or clears MIO_TWS()_INT[TS_INT]. */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears MIO_TWS()_INT[ST_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears MIO_TWS()_INT[ST_INT]. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1C/H) Reads or clears MIO_TWS()_INT[TS_INT]. */
        uint64_t core_int              : 1;  /**< [  2:  2](R/W1C/H) Reads or clears MIO_TWS()_INT[CORE_INT]. */
        uint64_t block_int             : 1;  /**< [  3:  3](R/W1C/H) Reads or clears MIO_TWS()_INT[BLOCK_INT]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    struct bdk_mio_twsx_int_ena_w1c_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_3_63         : 61;
        uint64_t core_int              : 1;  /**< [  2:  2](R/W1C/H) Reads or clears MIO_TWS()_INT[CORE_INT]. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1C/H) Reads or clears MIO_TWS()_INT[TS_INT]. */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears MIO_TWS()_INT[ST_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears MIO_TWS()_INT[ST_INT]. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1C/H) Reads or clears MIO_TWS()_INT[TS_INT]. */
        uint64_t core_int              : 1;  /**< [  2:  2](R/W1C/H) Reads or clears MIO_TWS()_INT[CORE_INT]. */
        uint64_t reserved_3_63         : 61;
#endif /* Word 0 - End */
    } cn8;
    /* struct bdk_mio_twsx_int_ena_w1c_s cn9; */
};
typedef union bdk_mio_twsx_int_ena_w1c bdk_mio_twsx_int_ena_w1c_t;

static inline uint64_t BDK_MIO_TWSX_INT_ENA_W1C(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_INT_ENA_W1C(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0d0001028ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0d0001028ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=5))
        return 0x87e0d0001028ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001028ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_INT_ENA_W1C", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_INT_ENA_W1C(a) bdk_mio_twsx_int_ena_w1c_t
#define bustype_BDK_MIO_TWSX_INT_ENA_W1C(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_INT_ENA_W1C(a) "MIO_TWSX_INT_ENA_W1C"
#define device_bar_BDK_MIO_TWSX_INT_ENA_W1C(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_INT_ENA_W1C(a) (a)
#define arguments_BDK_MIO_TWSX_INT_ENA_W1C(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_int_ena_w1s
 *
 * TWSI Interrupt Enable Set Register
 */
union bdk_mio_twsx_int_ena_w1s
{
    uint64_t u;
    struct bdk_mio_twsx_int_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t block_int             : 1;  /**< [  3:  3](R/W1S/H) Enables reporting of MIO_TWS()_INT[BLOCK_INT]. */
        uint64_t core_int              : 1;  /**< [  2:  2](R/W1S/H) Enables reporting of MIO_TWS()_INT[CORE_INT]. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1S/H) Enables reporting of MIO_TWS()_INT[TS_INT]. */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1S/H) Enables reporting of MIO_TWS()_INT[ST_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1S/H) Enables reporting of MIO_TWS()_INT[ST_INT]. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1S/H) Enables reporting of MIO_TWS()_INT[TS_INT]. */
        uint64_t core_int              : 1;  /**< [  2:  2](R/W1S/H) Enables reporting of MIO_TWS()_INT[CORE_INT]. */
        uint64_t block_int             : 1;  /**< [  3:  3](R/W1S/H) Enables reporting of MIO_TWS()_INT[BLOCK_INT]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    struct bdk_mio_twsx_int_ena_w1s_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_3_63         : 61;
        uint64_t core_int              : 1;  /**< [  2:  2](R/W1S/H) Enables reporting of MIO_TWS()_INT[CORE_INT]. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1S/H) Enables reporting of MIO_TWS()_INT[TS_INT]. */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1S/H) Enables reporting of MIO_TWS()_INT[ST_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1S/H) Enables reporting of MIO_TWS()_INT[ST_INT]. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1S/H) Enables reporting of MIO_TWS()_INT[TS_INT]. */
        uint64_t core_int              : 1;  /**< [  2:  2](R/W1S/H) Enables reporting of MIO_TWS()_INT[CORE_INT]. */
        uint64_t reserved_3_63         : 61;
#endif /* Word 0 - End */
    } cn8;
    /* struct bdk_mio_twsx_int_ena_w1s_s cn9; */
};
typedef union bdk_mio_twsx_int_ena_w1s bdk_mio_twsx_int_ena_w1s_t;

static inline uint64_t BDK_MIO_TWSX_INT_ENA_W1S(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_INT_ENA_W1S(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0d0001030ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0d0001030ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=5))
        return 0x87e0d0001030ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001030ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_INT_ENA_W1S", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_INT_ENA_W1S(a) bdk_mio_twsx_int_ena_w1s_t
#define bustype_BDK_MIO_TWSX_INT_ENA_W1S(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_INT_ENA_W1S(a) "MIO_TWSX_INT_ENA_W1S"
#define device_bar_BDK_MIO_TWSX_INT_ENA_W1S(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_INT_ENA_W1S(a) (a)
#define arguments_BDK_MIO_TWSX_INT_ENA_W1S(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_int_w1s
 *
 * TWSI Interrupt Set Register
 */
union bdk_mio_twsx_int_w1s
{
    uint64_t u;
    struct bdk_mio_twsx_int_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t block_int             : 1;  /**< [  3:  3](R/W1S/H) Reads or sets MIO_TWS()_INT[BLOCK_INT]. */
        uint64_t core_int              : 1;  /**< [  2:  2](RO/H) Reads MIO_TWS()_INT[CORE_INT]. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets MIO_TWS()_INT[TS_INT]. */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets MIO_TWS()_INT[ST_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets MIO_TWS()_INT[ST_INT]. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets MIO_TWS()_INT[TS_INT]. */
        uint64_t core_int              : 1;  /**< [  2:  2](RO/H) Reads MIO_TWS()_INT[CORE_INT]. */
        uint64_t block_int             : 1;  /**< [  3:  3](R/W1S/H) Reads or sets MIO_TWS()_INT[BLOCK_INT]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    struct bdk_mio_twsx_int_w1s_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_3_63         : 61;
        uint64_t core_int              : 1;  /**< [  2:  2](RO/H) Reads MIO_TWS()_INT[CORE_INT]. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets MIO_TWS()_INT[TS_INT]. */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets MIO_TWS()_INT[ST_INT]. */
#else /* Word 0 - Little Endian */
        uint64_t st_int                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets MIO_TWS()_INT[ST_INT]. */
        uint64_t ts_int                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets MIO_TWS()_INT[TS_INT]. */
        uint64_t core_int              : 1;  /**< [  2:  2](RO/H) Reads MIO_TWS()_INT[CORE_INT]. */
        uint64_t reserved_3_63         : 61;
#endif /* Word 0 - End */
    } cn8;
    /* struct bdk_mio_twsx_int_w1s_s cn9; */
};
typedef union bdk_mio_twsx_int_w1s bdk_mio_twsx_int_w1s_t;

static inline uint64_t BDK_MIO_TWSX_INT_W1S(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_INT_W1S(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0d0001020ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0d0001020ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=5))
        return 0x87e0d0001020ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001020ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_INT_W1S", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_INT_W1S(a) bdk_mio_twsx_int_w1s_t
#define bustype_BDK_MIO_TWSX_INT_W1S(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_INT_W1S(a) "MIO_TWSX_INT_W1S"
#define device_bar_BDK_MIO_TWSX_INT_W1S(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_INT_W1S(a) (a)
#define arguments_BDK_MIO_TWSX_INT_W1S(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_io_ctl
 *
 * MIO TWSI IO Control Register
 * This register control the TWSI IO drive strength and slew rates. Index {a} of zero
 * (MIO_TWS(0)_IO_CTL) is used to control all TWSI outputs on CNXXXX; other index
 * values have no effect.
 */
union bdk_mio_twsx_io_ctl
{
    uint64_t u;
    struct bdk_mio_twsx_io_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t drive                 : 2;  /**< [  3:  2](R/W) TWSI bus pin output drive strength.
                                                                 0x0 = 2 mA.
                                                                 0x1 = 4 mA.
                                                                 0x2 = 8 mA.
                                                                 0x3 = 16 mA. */
        uint64_t reserved_1            : 1;
        uint64_t slew                  : 1;  /**< [  0:  0](R/W) TWSI bus pins output slew rate control.
                                                                 0 = Low slew rate.
                                                                 1 = High slew rate. */
#else /* Word 0 - Little Endian */
        uint64_t slew                  : 1;  /**< [  0:  0](R/W) TWSI bus pins output slew rate control.
                                                                 0 = Low slew rate.
                                                                 1 = High slew rate. */
        uint64_t reserved_1            : 1;
        uint64_t drive                 : 2;  /**< [  3:  2](R/W) TWSI bus pin output drive strength.
                                                                 0x0 = 2 mA.
                                                                 0x1 = 4 mA.
                                                                 0x2 = 8 mA.
                                                                 0x3 = 16 mA. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_twsx_io_ctl_s cn; */
};
typedef union bdk_mio_twsx_io_ctl bdk_mio_twsx_io_ctl_t;

static inline uint64_t BDK_MIO_TWSX_IO_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_IO_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001070ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_IO_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_IO_CTL(a) bdk_mio_twsx_io_ctl_t
#define bustype_BDK_MIO_TWSX_IO_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_IO_CTL(a) "MIO_TWSX_IO_CTL"
#define device_bar_BDK_MIO_TWSX_IO_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_IO_CTL(a) (a)
#define arguments_BDK_MIO_TWSX_IO_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_mode
 *
 * TWSI Mode and Control Register
 */
union bdk_mio_twsx_mode
{
    uint64_t u;
    struct bdk_mio_twsx_mode_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t refclk_src            : 1;  /**< [  4:  4](R/W) Reference clock source used to generate TWSI clock. See TWSI_CLK and TWSI_CLKCTL for
                                                                 details.
                                                                   0 = 100 MHz input reference generates TWSI clock.
                                                                   1 = Coprocessor clock generates TWSI clock. */
        uint64_t bus_mon_rst           : 1;  /**< [  3:  3](WO/H) Reset the TWSI bus monitor for both HLC and non-HLC mode.
                                                                 When TWSI bus in hang state with a timeout interrupt, it is possible that the TWSI bus
                                                                 monitor still waiting for STP on the bus before accepting any new commands.
                                                                 Write 1 to send a pulse to reset interface monitor back to the initial condition. */
        uint64_t block_mode            : 1;  /**< [  2:  2](R/W) Block transfer mode in HLC, only valid in HLC mode.
                                                                 When device is enabled to block transfer mode, software can access TWSI data through a
                                                                 FIFO interface.  Software needs to write to MIO_TWS()_TWSI_BLOCK_CTL[BLOCK_SIZE] with
                                                                 the number of bytes to be transfered/received. */
        uint64_t stretch               : 1;  /**< [  1:  1](R/W) Clock stretching enable.
                                                                 When enabled and device in master mode, it allows slave device
                                                                 to extend low period of the clock. During the clock extension period, the SCL output from
                                                                 master device is disabled. */
        uint64_t hs_mode               : 1;  /**< [  0:  0](R/W) I2C bus high-speed mode.

                                                                 0 = Open drain drive on TWS_SCL. TWS_SCL clock signal high-to-low ratio is 1 to 1.
                                                                 OSCL output frequency divisor is 10.

                                                                 1 = Current sourced circuit is used to drive TWS_SCL pin when device is in master mode,
                                                                 but disabled after each repeated start condition
                                                                 and after each ACK or NACK to give a slave a chance to stretch the clock.
                                                                 TWS_SCL clock signal high-to-low ratio is 1 to 2.
                                                                 OSCL output frequency divisor is 15. */
#else /* Word 0 - Little Endian */
        uint64_t hs_mode               : 1;  /**< [  0:  0](R/W) I2C bus high-speed mode.

                                                                 0 = Open drain drive on TWS_SCL. TWS_SCL clock signal high-to-low ratio is 1 to 1.
                                                                 OSCL output frequency divisor is 10.

                                                                 1 = Current sourced circuit is used to drive TWS_SCL pin when device is in master mode,
                                                                 but disabled after each repeated start condition
                                                                 and after each ACK or NACK to give a slave a chance to stretch the clock.
                                                                 TWS_SCL clock signal high-to-low ratio is 1 to 2.
                                                                 OSCL output frequency divisor is 15. */
        uint64_t stretch               : 1;  /**< [  1:  1](R/W) Clock stretching enable.
                                                                 When enabled and device in master mode, it allows slave device
                                                                 to extend low period of the clock. During the clock extension period, the SCL output from
                                                                 master device is disabled. */
        uint64_t block_mode            : 1;  /**< [  2:  2](R/W) Block transfer mode in HLC, only valid in HLC mode.
                                                                 When device is enabled to block transfer mode, software can access TWSI data through a
                                                                 FIFO interface.  Software needs to write to MIO_TWS()_TWSI_BLOCK_CTL[BLOCK_SIZE] with
                                                                 the number of bytes to be transfered/received. */
        uint64_t bus_mon_rst           : 1;  /**< [  3:  3](WO/H) Reset the TWSI bus monitor for both HLC and non-HLC mode.
                                                                 When TWSI bus in hang state with a timeout interrupt, it is possible that the TWSI bus
                                                                 monitor still waiting for STP on the bus before accepting any new commands.
                                                                 Write 1 to send a pulse to reset interface monitor back to the initial condition. */
        uint64_t refclk_src            : 1;  /**< [  4:  4](R/W) Reference clock source used to generate TWSI clock. See TWSI_CLK and TWSI_CLKCTL for
                                                                 details.
                                                                   0 = 100 MHz input reference generates TWSI clock.
                                                                   1 = Coprocessor clock generates TWSI clock. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    struct bdk_mio_twsx_mode_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t stretch               : 1;  /**< [  1:  1](R/W) Clock stretching enable.
                                                                 When enabled and device in master mode, it allows slave device
                                                                 to extend low period of the clock. During the clock extension period, the SCL output from
                                                                 master device is disabled. */
        uint64_t hs_mode               : 1;  /**< [  0:  0](R/W) I2C bus high-speed mode.

                                                                 0 = Open drain drive on TWS_SCL. TWS_SCL clock signal high-to-low ratio is 1 to 1.
                                                                 OSCL output frequency divisor is 10.

                                                                 1 = Current sourced circuit is used to drive TWS_SCL pin when device is in master mode,
                                                                 but disabled after each repeated start condition
                                                                 and after each ACK or NACK to give a slave a chance to stretch the clock.
                                                                 TWS_SCL clock signal high-to-low ratio is 1 to 2.
                                                                 OSCL output frequency divisor is 15. */
#else /* Word 0 - Little Endian */
        uint64_t hs_mode               : 1;  /**< [  0:  0](R/W) I2C bus high-speed mode.

                                                                 0 = Open drain drive on TWS_SCL. TWS_SCL clock signal high-to-low ratio is 1 to 1.
                                                                 OSCL output frequency divisor is 10.

                                                                 1 = Current sourced circuit is used to drive TWS_SCL pin when device is in master mode,
                                                                 but disabled after each repeated start condition
                                                                 and after each ACK or NACK to give a slave a chance to stretch the clock.
                                                                 TWS_SCL clock signal high-to-low ratio is 1 to 2.
                                                                 OSCL output frequency divisor is 15. */
        uint64_t stretch               : 1;  /**< [  1:  1](R/W) Clock stretching enable.
                                                                 When enabled and device in master mode, it allows slave device
                                                                 to extend low period of the clock. During the clock extension period, the SCL output from
                                                                 master device is disabled. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } cn8;
    struct bdk_mio_twsx_mode_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t refclk_src            : 1;  /**< [  4:  4](R/W) Reference clock source used to generate TWSI clock. See TWSI_CLK and TWSI_CLKCTL for
                                                                 details.
                                                                   0 = 100 MHz input reference generates TWSI clock.
                                                                   1 = Coprocessor clock generates TWSI clock. */
        uint64_t bus_mon_rst           : 1;  /**< [  3:  3](WO/H) Reset the TWSI bus monitor for both HLC and non-HLC mode.
                                                                 When TWSI bus in hang state with a timeout interrupt, it is possible that the TWSI bus
                                                                 monitor still waiting for STP on the bus before accepting any new commands.
                                                                 Write 1 to send a pulse to reset interface monitor back to the initial condition. */
        uint64_t block_mode            : 1;  /**< [  2:  2](R/W) Block transfer mode in HLC, only valid in HLC mode.
                                                                 When device is enabled to block transfer mode, software can access TWSI data through a
                                                                 FIFO interface.  Software needs to write to MIO_TWS()_TWSI_BLOCK_CTL[BLOCK_SIZE] with
                                                                 the number of bytes to be transfered/received. */
        uint64_t stretch               : 1;  /**< [  1:  1](R/W) Clock stretching enable.
                                                                 When enabled and device in non-HLC master mode, it allows slave device
                                                                 to extend low period of the clock. During the clock extension period, the SCL output from
                                                                 master device is disabled. */
        uint64_t hs_mode               : 1;  /**< [  0:  0](R/W) I2C bus high-speed mode.

                                                                 0 = Open drain drive on TWS_SCL. TWS_SCL clock signal high-to-low ratio is 1 to 1.
                                                                 OSCL output frequency divisor is 10.

                                                                 1 = Current sourced circuit is used to drive TWS_SCL pin when device is in master mode,
                                                                 but disabled after each repeated start condition
                                                                 and after each ACK or NACK to give a slave a chance to stretch the clock.
                                                                 TWS_SCL clock signal high-to-low ratio is 1 to 2.
                                                                 OSCL output frequency divisor is 15. */
#else /* Word 0 - Little Endian */
        uint64_t hs_mode               : 1;  /**< [  0:  0](R/W) I2C bus high-speed mode.

                                                                 0 = Open drain drive on TWS_SCL. TWS_SCL clock signal high-to-low ratio is 1 to 1.
                                                                 OSCL output frequency divisor is 10.

                                                                 1 = Current sourced circuit is used to drive TWS_SCL pin when device is in master mode,
                                                                 but disabled after each repeated start condition
                                                                 and after each ACK or NACK to give a slave a chance to stretch the clock.
                                                                 TWS_SCL clock signal high-to-low ratio is 1 to 2.
                                                                 OSCL output frequency divisor is 15. */
        uint64_t stretch               : 1;  /**< [  1:  1](R/W) Clock stretching enable.
                                                                 When enabled and device in non-HLC master mode, it allows slave device
                                                                 to extend low period of the clock. During the clock extension period, the SCL output from
                                                                 master device is disabled. */
        uint64_t block_mode            : 1;  /**< [  2:  2](R/W) Block transfer mode in HLC, only valid in HLC mode.
                                                                 When device is enabled to block transfer mode, software can access TWSI data through a
                                                                 FIFO interface.  Software needs to write to MIO_TWS()_TWSI_BLOCK_CTL[BLOCK_SIZE] with
                                                                 the number of bytes to be transfered/received. */
        uint64_t bus_mon_rst           : 1;  /**< [  3:  3](WO/H) Reset the TWSI bus monitor for both HLC and non-HLC mode.
                                                                 When TWSI bus in hang state with a timeout interrupt, it is possible that the TWSI bus
                                                                 monitor still waiting for STP on the bus before accepting any new commands.
                                                                 Write 1 to send a pulse to reset interface monitor back to the initial condition. */
        uint64_t refclk_src            : 1;  /**< [  4:  4](R/W) Reference clock source used to generate TWSI clock. See TWSI_CLK and TWSI_CLKCTL for
                                                                 details.
                                                                   0 = 100 MHz input reference generates TWSI clock.
                                                                   1 = Coprocessor clock generates TWSI clock. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_mio_twsx_mode bdk_mio_twsx_mode_t;

static inline uint64_t BDK_MIO_TWSX_MODE(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_MODE(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0d0001038ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0d0001038ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=5))
        return 0x87e0d0001038ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001038ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_MODE", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_MODE(a) bdk_mio_twsx_mode_t
#define bustype_BDK_MIO_TWSX_MODE(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_MODE(a) "MIO_TWSX_MODE"
#define device_bar_BDK_MIO_TWSX_MODE(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_MODE(a) (a)
#define arguments_BDK_MIO_TWSX_MODE(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_msix_pba#
 *
 * TWSI MSI-X Pending Bit Array Registers
 * This register is the MSI-X PBA table, the bit number is indexed by the MIO_TWS_INT_VEC_E
 * enumeration.
 */
union bdk_mio_twsx_msix_pbax
{
    uint64_t u;
    struct bdk_mio_twsx_msix_pbax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO/H) Pending message for the associated MIO_TWS()_MSIX_VEC()_CTL, enumerated by
                                                                 MIO_TWS_INT_VEC_E. Bits that have no associated MIO_TWS_INT_VEC_E are zero. */
#else /* Word 0 - Little Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO/H) Pending message for the associated MIO_TWS()_MSIX_VEC()_CTL, enumerated by
                                                                 MIO_TWS_INT_VEC_E. Bits that have no associated MIO_TWS_INT_VEC_E are zero. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_twsx_msix_pbax_s cn; */
};
typedef union bdk_mio_twsx_msix_pbax bdk_mio_twsx_msix_pbax_t;

static inline uint64_t BDK_MIO_TWSX_MSIX_PBAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_MSIX_PBAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b==0)))
        return 0x87e0d0ff0000ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b==0)))
        return 0x87e0d0ff0000ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=5) && (b==0)))
        return 0x87e0d0ff0000ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=5) && (b==0)))
        return 0x87e0d0ff0000ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x0);
    __bdk_csr_fatal("MIO_TWSX_MSIX_PBAX", 2, a, b, 0, 0);
}

#define typedef_BDK_MIO_TWSX_MSIX_PBAX(a,b) bdk_mio_twsx_msix_pbax_t
#define bustype_BDK_MIO_TWSX_MSIX_PBAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_MSIX_PBAX(a,b) "MIO_TWSX_MSIX_PBAX"
#define device_bar_BDK_MIO_TWSX_MSIX_PBAX(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_MIO_TWSX_MSIX_PBAX(a,b) (a)
#define arguments_BDK_MIO_TWSX_MSIX_PBAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) mio_tws#_msix_vec#_addr
 *
 * TWSI MSI-X Vector Table Address Registers
 * This register is the MSI-X vector table, indexed by the MIO_TWS_INT_VEC_E enumeration.
 */
union bdk_mio_twsx_msix_vecx_addr
{
    uint64_t u;
    struct bdk_mio_twsx_msix_vecx_addr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_53_63        : 11;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's MIO_TWS()_MSIX_VEC()_ADDR, MIO_TWS()_MSIX_VEC()_CTL, and corresponding
                                                                 bit of MIO_TWS()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_MIO_TWS()_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's MIO_TWS()_MSIX_VEC()_ADDR, MIO_TWS()_MSIX_VEC()_CTL, and corresponding
                                                                 bit of MIO_TWS()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_MIO_TWS()_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_53_63        : 11;
#endif /* Word 0 - End */
    } s;
    struct bdk_mio_twsx_msix_vecx_addr_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's MIO_TWS()_MSIX_VEC()_ADDR, MIO_TWS()_MSIX_VEC()_CTL, and corresponding
                                                                 bit of MIO_TWS()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_MIO_TWS()_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's MIO_TWS()_MSIX_VEC()_ADDR, MIO_TWS()_MSIX_VEC()_CTL, and corresponding
                                                                 bit of MIO_TWS()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_MIO_TWS()_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } cn8;
    /* struct bdk_mio_twsx_msix_vecx_addr_s cn9; */
};
typedef union bdk_mio_twsx_msix_vecx_addr bdk_mio_twsx_msix_vecx_addr_t;

static inline uint64_t BDK_MIO_TWSX_MSIX_VECX_ADDR(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_MSIX_VECX_ADDR(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b==0)))
        return 0x87e0d0f00000ll + 0x1000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b==0)))
        return 0x87e0d0f00000ll + 0x1000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=5) && (b==0)))
        return 0x87e0d0f00000ll + 0x1000000ll * ((a) & 0x7) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=5) && (b==0)))
        return 0x87e0d0f00000ll + 0x1000000ll * ((a) & 0x7) + 0x10ll * ((b) & 0x0);
    __bdk_csr_fatal("MIO_TWSX_MSIX_VECX_ADDR", 2, a, b, 0, 0);
}

#define typedef_BDK_MIO_TWSX_MSIX_VECX_ADDR(a,b) bdk_mio_twsx_msix_vecx_addr_t
#define bustype_BDK_MIO_TWSX_MSIX_VECX_ADDR(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_MSIX_VECX_ADDR(a,b) "MIO_TWSX_MSIX_VECX_ADDR"
#define device_bar_BDK_MIO_TWSX_MSIX_VECX_ADDR(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_MIO_TWSX_MSIX_VECX_ADDR(a,b) (a)
#define arguments_BDK_MIO_TWSX_MSIX_VECX_ADDR(a,b) (a),(b),-1,-1

/**
 * Register (RSL) mio_tws#_msix_vec#_ctl
 *
 * TWSI MSI-X Vector Table Control and Data Registers
 * This register is the MSI-X vector table, indexed by the MIO_TWS_INT_VEC_E enumeration.
 */
union bdk_mio_twsx_msix_vecx_ctl
{
    uint64_t u;
    struct bdk_mio_twsx_msix_vecx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_33_63        : 31;
        uint64_t mask                  : 1;  /**< [ 32: 32](R/W) When set, no MSI-X interrupts will be sent to this vector. */
        uint64_t data                  : 32; /**< [ 31:  0](R/W) Data to use for MSI-X delivery of this vector. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 32; /**< [ 31:  0](R/W) Data to use for MSI-X delivery of this vector. */
        uint64_t mask                  : 1;  /**< [ 32: 32](R/W) When set, no MSI-X interrupts will be sent to this vector. */
        uint64_t reserved_33_63        : 31;
#endif /* Word 0 - End */
    } s;
    struct bdk_mio_twsx_msix_vecx_ctl_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_33_63        : 31;
        uint64_t mask                  : 1;  /**< [ 32: 32](R/W) When set, no MSI-X interrupts will be sent to this vector. */
        uint64_t reserved_20_31        : 12;
        uint64_t data                  : 20; /**< [ 19:  0](R/W) Data to use for MSI-X delivery of this vector. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 20; /**< [ 19:  0](R/W) Data to use for MSI-X delivery of this vector. */
        uint64_t reserved_20_31        : 12;
        uint64_t mask                  : 1;  /**< [ 32: 32](R/W) When set, no MSI-X interrupts will be sent to this vector. */
        uint64_t reserved_33_63        : 31;
#endif /* Word 0 - End */
    } cn8;
    /* struct bdk_mio_twsx_msix_vecx_ctl_s cn9; */
};
typedef union bdk_mio_twsx_msix_vecx_ctl bdk_mio_twsx_msix_vecx_ctl_t;

static inline uint64_t BDK_MIO_TWSX_MSIX_VECX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_MSIX_VECX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b==0)))
        return 0x87e0d0f00008ll + 0x1000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=1) && (b==0)))
        return 0x87e0d0f00008ll + 0x1000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=5) && (b==0)))
        return 0x87e0d0f00008ll + 0x1000000ll * ((a) & 0x7) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=5) && (b==0)))
        return 0x87e0d0f00008ll + 0x1000000ll * ((a) & 0x7) + 0x10ll * ((b) & 0x0);
    __bdk_csr_fatal("MIO_TWSX_MSIX_VECX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_MIO_TWSX_MSIX_VECX_CTL(a,b) bdk_mio_twsx_msix_vecx_ctl_t
#define bustype_BDK_MIO_TWSX_MSIX_VECX_CTL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_MSIX_VECX_CTL(a,b) "MIO_TWSX_MSIX_VECX_CTL"
#define device_bar_BDK_MIO_TWSX_MSIX_VECX_CTL(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_MIO_TWSX_MSIX_VECX_CTL(a,b) (a)
#define arguments_BDK_MIO_TWSX_MSIX_VECX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) mio_tws#_sw_twsi
 *
 * TWSI Software to TWSI Register
 * This register allows software to:
 * * Initiate master-mode operations with a write operation, and read the result with a
 * read operation.
 * * Load four bytes for later retrieval (slave mode) with a write operation and check validity
 * with a read operation.
 * * Launch a configuration read/write operation with a write operation and read the result with
 * a read operation.
 *
 * This register should be read or written by software, and read by the TWSI device. The TWSI
 * device can use either two-byte or five-byte read operations to reference this register.
 * The TWSI device considers this register valid when [V] = 1 and [SLONLY] = 1.
 */
union bdk_mio_twsx_sw_twsi
{
    uint64_t u;
    struct bdk_mio_twsx_sw_twsi_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t v                     : 1;  /**< [ 63: 63](RC/W/H) Valid bit. Set on a write operation (should always be written with a 1). Cleared when a
                                                                 TWSI master-mode operation completes, a TWSI configuration register access completes, or
                                                                 when the TWSI device reads the register if [SLONLY] = 1. */
        uint64_t slonly                : 1;  /**< [ 62: 62](R/W) Slave only mode.

                                                                 When this bit is set, no operations are initiated with a write operation. Only the D field
                                                                 is updated in this case.

                                                                 When this bit is clear, a write operation initiates either a master-mode operation or a
                                                                 TWSI configuration register access. */
        uint64_t eia                   : 1;  /**< [ 61: 61](R/W) Extended internal address. Sends an additional internal address byte (the MSB of [IA] is
                                                                 from MIO_TWS()_SW_TWSI_EXT[IA]). */
        uint64_t op                    : 4;  /**< [ 60: 57](R/W) Opcode field. When the register is written with [SLONLY] = 0, this field initiates one of
                                                                 the following read or write operations:
                                                                 0x0 = 7-bit byte master-mode operation.
                                                                 0x1 = 7-bit byte combined-read master-mode operation, 7-bit byte write-with-IA master-mode
                                                                 operation.
                                                                 0x2 = 10-bit byte master-mode operation.
                                                                 0x3 = 10-bit byte combined-read master-mode operation, 10-bit byte write-with-IA master-
                                                                 mode operation.
                                                                 0x4 = TWSI master-clock register, TWSI_CLK in TWSI Master Clock Register.
                                                                 0x6 = See [EOP_IA] field.
                                                                 0x8 = 7-bit 4-byte master-mode operation.
                                                                 0x9 = 7-bit 4-byte combined-read master-mode operation, 7-bit 4-byte write-with-IA master-
                                                                 mode operation.
                                                                 0xA = 10-bit 4-byte master-mode operation.
                                                                 0xB = 10-bit 4-byte combined-read master-mode operation, 10-bit 4-byte write-with-IA
                                                                 master-mode operation. */
        uint64_t r                     : 1;  /**< [ 56: 56](R/W/H) Read bit or result. If this bit is set on a CSR write when [SLONLY] = 0, the
                                                                 operation is a read operation (if clear, it is a write operation).
                                                                 On a CSR read, this bit returns the result indication for the most recent
                                                                 master-mode operation, 1 = success, 0 = failure. */
        uint64_t sovr                  : 1;  /**< [ 55: 55](R/W) Size override. If this bit is set, use the [SIZE] field to determine the master-mode
                                                                 operation size rather than what [OP] specifies. For operations greater than four bytes, the
                                                                 additional data is contained in MIO_TWS()_SW_TWSI_EXT[DATA]. */
        uint64_t size                  : 3;  /**< [ 54: 52](R/W) Size minus one. Specifies the size in bytes of the master-mode operation if
                                                                 [SOVR] = 1. (0 = 1 byte, 1 = 2 bytes, ... 7 = 8 bytes). */
        uint64_t scr                   : 2;  /**< [ 51: 50](R/W) Scratch. Unused, but retain state. */
        uint64_t addr                  : 10; /**< [ 49: 40](R/W) Address field. The address of the remote device for a master-mode operation. ADDR\<9:7\> are
                                                                 only used for 10-bit addressing.

                                                                 Note that when mastering a 7-bit OP, ADDR\<6:0\> should not take any of the values 0x78,
                                                                 0x79, 0x7A nor 0x7B. (These 7-bit addresses are reserved to extend to 10-bit addressing). */
        uint64_t ia                    : 5;  /**< [ 39: 35](R/W) Internal address. Used when launching a combined master-mode operation. The lower 3
                                                                 address bits are contained in [EOP_IA]. */
        uint64_t eop_ia                : 3;  /**< [ 34: 32](R/W) Extra opcode, used when OP\<3:0\> = 0x6 and [SLONLY] = 0.
                                                                 0x0 = TWSI slave address register (TWSI_SLAVE_ADD).
                                                                 0x1 = TWSI data register (TWSI_DATA).
                                                                 0x2 = TWSI control register (TWSI_CTL).
                                                                 0x3 = (when [R] = 0) TWSI clock control register (TWSI_CLKCTL).
                                                                 0x3 = (when [R] = 1) TWSI status register (TWSI_STAT).
                                                                 0x4 = TWSI extended slave register (TWSI_SLAVE_ADD_EXT).
                                                                 0x7 = TWSI soft reset register (TWSI_RST).

                                                                 Also provides the lower three bits of internal address when launching a combined master-mode
                                                                 operation. */
        uint64_t data                  : 32; /**< [ 31:  0](R/W/H) Data field.
                                                                 Used on a write operation when:
                                                                 * Initiating a master-mode write operation ([SLONLY] = 0).
                                                                 * Writing a TWSI configuration register ([SLONLY] = 0).
                                                                 * A slave-mode write operation ([SLONLY] = 1).

                                                                 The read value is updated by:
                                                                 * A write operation to this register.
                                                                 * Master-mode completion (contains error code).
                                                                 * TWSI configuration-register read (contains result). */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 32; /**< [ 31:  0](R/W/H) Data field.
                                                                 Used on a write operation when:
                                                                 * Initiating a master-mode write operation ([SLONLY] = 0).
                                                                 * Writing a TWSI configuration register ([SLONLY] = 0).
                                                                 * A slave-mode write operation ([SLONLY] = 1).

                                                                 The read value is updated by:
                                                                 * A write operation to this register.
                                                                 * Master-mode completion (contains error code).
                                                                 * TWSI configuration-register read (contains result). */
        uint64_t eop_ia                : 3;  /**< [ 34: 32](R/W) Extra opcode, used when OP\<3:0\> = 0x6 and [SLONLY] = 0.
                                                                 0x0 = TWSI slave address register (TWSI_SLAVE_ADD).
                                                                 0x1 = TWSI data register (TWSI_DATA).
                                                                 0x2 = TWSI control register (TWSI_CTL).
                                                                 0x3 = (when [R] = 0) TWSI clock control register (TWSI_CLKCTL).
                                                                 0x3 = (when [R] = 1) TWSI status register (TWSI_STAT).
                                                                 0x4 = TWSI extended slave register (TWSI_SLAVE_ADD_EXT).
                                                                 0x7 = TWSI soft reset register (TWSI_RST).

                                                                 Also provides the lower three bits of internal address when launching a combined master-mode
                                                                 operation. */
        uint64_t ia                    : 5;  /**< [ 39: 35](R/W) Internal address. Used when launching a combined master-mode operation. The lower 3
                                                                 address bits are contained in [EOP_IA]. */
        uint64_t addr                  : 10; /**< [ 49: 40](R/W) Address field. The address of the remote device for a master-mode operation. ADDR\<9:7\> are
                                                                 only used for 10-bit addressing.

                                                                 Note that when mastering a 7-bit OP, ADDR\<6:0\> should not take any of the values 0x78,
                                                                 0x79, 0x7A nor 0x7B. (These 7-bit addresses are reserved to extend to 10-bit addressing). */
        uint64_t scr                   : 2;  /**< [ 51: 50](R/W) Scratch. Unused, but retain state. */
        uint64_t size                  : 3;  /**< [ 54: 52](R/W) Size minus one. Specifies the size in bytes of the master-mode operation if
                                                                 [SOVR] = 1. (0 = 1 byte, 1 = 2 bytes, ... 7 = 8 bytes). */
        uint64_t sovr                  : 1;  /**< [ 55: 55](R/W) Size override. If this bit is set, use the [SIZE] field to determine the master-mode
                                                                 operation size rather than what [OP] specifies. For operations greater than four bytes, the
                                                                 additional data is contained in MIO_TWS()_SW_TWSI_EXT[DATA]. */
        uint64_t r                     : 1;  /**< [ 56: 56](R/W/H) Read bit or result. If this bit is set on a CSR write when [SLONLY] = 0, the
                                                                 operation is a read operation (if clear, it is a write operation).
                                                                 On a CSR read, this bit returns the result indication for the most recent
                                                                 master-mode operation, 1 = success, 0 = failure. */
        uint64_t op                    : 4;  /**< [ 60: 57](R/W) Opcode field. When the register is written with [SLONLY] = 0, this field initiates one of
                                                                 the following read or write operations:
                                                                 0x0 = 7-bit byte master-mode operation.
                                                                 0x1 = 7-bit byte combined-read master-mode operation, 7-bit byte write-with-IA master-mode
                                                                 operation.
                                                                 0x2 = 10-bit byte master-mode operation.
                                                                 0x3 = 10-bit byte combined-read master-mode operation, 10-bit byte write-with-IA master-
                                                                 mode operation.
                                                                 0x4 = TWSI master-clock register, TWSI_CLK in TWSI Master Clock Register.
                                                                 0x6 = See [EOP_IA] field.
                                                                 0x8 = 7-bit 4-byte master-mode operation.
                                                                 0x9 = 7-bit 4-byte combined-read master-mode operation, 7-bit 4-byte write-with-IA master-
                                                                 mode operation.
                                                                 0xA = 10-bit 4-byte master-mode operation.
                                                                 0xB = 10-bit 4-byte combined-read master-mode operation, 10-bit 4-byte write-with-IA
                                                                 master-mode operation. */
        uint64_t eia                   : 1;  /**< [ 61: 61](R/W) Extended internal address. Sends an additional internal address byte (the MSB of [IA] is
                                                                 from MIO_TWS()_SW_TWSI_EXT[IA]). */
        uint64_t slonly                : 1;  /**< [ 62: 62](R/W) Slave only mode.

                                                                 When this bit is set, no operations are initiated with a write operation. Only the D field
                                                                 is updated in this case.

                                                                 When this bit is clear, a write operation initiates either a master-mode operation or a
                                                                 TWSI configuration register access. */
        uint64_t v                     : 1;  /**< [ 63: 63](RC/W/H) Valid bit. Set on a write operation (should always be written with a 1). Cleared when a
                                                                 TWSI master-mode operation completes, a TWSI configuration register access completes, or
                                                                 when the TWSI device reads the register if [SLONLY] = 1. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_twsx_sw_twsi_s cn8; */
    struct bdk_mio_twsx_sw_twsi_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t v                     : 1;  /**< [ 63: 63](RC/W/H) Valid bit. Set on a write operation (should always be written with a 1). Cleared when a
                                                                 TWSI master-mode operation completes, a TWSI configuration register access completes, or
                                                                 when the TWSI device reads the register if [SLONLY] = 1. */
        uint64_t slonly                : 1;  /**< [ 62: 62](R/W) Slave only mode.

                                                                 When this bit is set, no operations are initiated with a write operation. Only the D field
                                                                 is updated in this case.

                                                                 When this bit is clear, a write operation initiates either a master-mode operation or a
                                                                 TWSI configuration register access. */
        uint64_t eia                   : 1;  /**< [ 61: 61](R/W) Extended internal address. Sends an additional internal address byte (the MSB of [IA] is
                                                                 from MIO_TWS()_SW_TWSI_EXT[IA]). */
        uint64_t op                    : 4;  /**< [ 60: 57](R/W) Opcode field. When the register is written with [SLONLY] = 0, this field initiates one of
                                                                 the following read or write operations:
                                                                 0x0 = 7-bit byte master-mode operation.
                                                                 0x1 = 7-bit byte combined-read master-mode operation, 7-bit byte write-with-IA master-mode
                                                                 operation.
                                                                 0x2 = 10-bit byte master-mode operation.
                                                                 0x3 = 10-bit byte combined-read master-mode operation, 10-bit byte write-with-IA master-
                                                                 mode operation.
                                                                 0x4 = TWSI master-clock register, TWSI_CLK in TWSI Master Clock Register.
                                                                 0x6 = See [EOP_IA] field.
                                                                 0x8 = 7-bit 4-byte master-mode operation.
                                                                 0x9 = 7-bit 4-byte combined-read master-mode operation, 7-bit 4-byte write-with-IA master-
                                                                 mode operation.
                                                                 0xA = 10-bit 4-byte master-mode operation.
                                                                 0xB = 10-bit 4-byte combined-read master-mode operation, 10-bit 4-byte write-with-IA
                                                                 master-mode operation. */
        uint64_t r                     : 1;  /**< [ 56: 56](R/W/H) Read bit or result. If this bit is set on a CSR write when [SLONLY] = 0, the
                                                                 operation is a read operation (if clear, it is a write operation).
                                                                 On a CSR read, this bit returns the result indication for the most recent
                                                                 master-mode operation, 1 = success, 0 = failure. */
        uint64_t sovr                  : 1;  /**< [ 55: 55](R/W) Size override. If this bit is set, use the [SIZE] field to determine the master-mode
                                                                 operation size rather than what [OP] specifies. For operations greater than four bytes, the
                                                                 additional data is contained in MIO_TWS()_SW_TWSI_EXT[DATA]. In block mode,
                                                                 all data can be accessible from FIFO interface MIO_TWS()_TWSI_BLOCK_FIFO. */
        uint64_t size                  : 3;  /**< [ 54: 52](R/W) Size minus one for HLC non block mode. Specifies the size in bytes of the master-mode
                                                                 operation if [SOVR] = 1. (0 = 1 byte, 1 = 2 bytes, ... 7 = 8 bytes).
                                                                 block mode's block size is defined by MIO_TWS()_TWSI_BLOCK_CTL[BLOCK_SIZE]. */
        uint64_t scr                   : 2;  /**< [ 51: 50](R/W) Scratch. Unused, but retain state. */
        uint64_t addr                  : 10; /**< [ 49: 40](R/W) Address field. The address of the remote device for a master-mode operation. ADDR\<9:7\> are
                                                                 only used for 10-bit addressing.

                                                                 Note that when mastering a 7-bit OP, ADDR\<6:0\> should not take any of the values 0x78,
                                                                 0x79, 0x7A nor 0x7B. (These 7-bit addresses are reserved to extend to 10-bit addressing). */
        uint64_t ia                    : 5;  /**< [ 39: 35](R/W) Internal address. Used when launching a combined master-mode operation. The lower 3
                                                                 address bits are contained in [EOP_IA]. */
        uint64_t eop_ia                : 3;  /**< [ 34: 32](R/W) Extra opcode, used when OP\<3:0\> = 0x6 and [SLONLY] = 0.
                                                                 0x0 = TWSI slave address register (TWSI_SLAVE_ADD).
                                                                 0x1 = TWSI data register (TWSI_DATA).
                                                                 0x2 = TWSI control register (TWSI_CTL).
                                                                 0x3 = (when [R] = 0) TWSI clock control register (TWSI_CLKCTL).
                                                                 0x3 = (when [R] = 1) TWSI status register (TWSI_STAT).
                                                                 0x4 = TWSI extended slave register (TWSI_SLAVE_ADD_EXT).
                                                                 0x7 = TWSI soft reset register (TWSI_RST).

                                                                 Also provides the lower three bits of internal address when launching a combined master-mode
                                                                 operation. */
        uint64_t data                  : 32; /**< [ 31:  0](R/W/H) Data field, bytes 0-3.
                                                                 Used on a write operation when:
                                                                 * Initiating a master-mode write operation ([SLONLY] = 0).
                                                                 * Writing a TWSI configuration register ([SLONLY] = 0).
                                                                 * A slave-mode write operation ([SLONLY] = 1).

                                                                 The read value is updated by:
                                                                 * A write operation to this register.
                                                                 * Master-mode completion (contains error code).
                                                                 * TWSI configuration-register read (contains result). */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 32; /**< [ 31:  0](R/W/H) Data field, bytes 0-3.
                                                                 Used on a write operation when:
                                                                 * Initiating a master-mode write operation ([SLONLY] = 0).
                                                                 * Writing a TWSI configuration register ([SLONLY] = 0).
                                                                 * A slave-mode write operation ([SLONLY] = 1).

                                                                 The read value is updated by:
                                                                 * A write operation to this register.
                                                                 * Master-mode completion (contains error code).
                                                                 * TWSI configuration-register read (contains result). */
        uint64_t eop_ia                : 3;  /**< [ 34: 32](R/W) Extra opcode, used when OP\<3:0\> = 0x6 and [SLONLY] = 0.
                                                                 0x0 = TWSI slave address register (TWSI_SLAVE_ADD).
                                                                 0x1 = TWSI data register (TWSI_DATA).
                                                                 0x2 = TWSI control register (TWSI_CTL).
                                                                 0x3 = (when [R] = 0) TWSI clock control register (TWSI_CLKCTL).
                                                                 0x3 = (when [R] = 1) TWSI status register (TWSI_STAT).
                                                                 0x4 = TWSI extended slave register (TWSI_SLAVE_ADD_EXT).
                                                                 0x7 = TWSI soft reset register (TWSI_RST).

                                                                 Also provides the lower three bits of internal address when launching a combined master-mode
                                                                 operation. */
        uint64_t ia                    : 5;  /**< [ 39: 35](R/W) Internal address. Used when launching a combined master-mode operation. The lower 3
                                                                 address bits are contained in [EOP_IA]. */
        uint64_t addr                  : 10; /**< [ 49: 40](R/W) Address field. The address of the remote device for a master-mode operation. ADDR\<9:7\> are
                                                                 only used for 10-bit addressing.

                                                                 Note that when mastering a 7-bit OP, ADDR\<6:0\> should not take any of the values 0x78,
                                                                 0x79, 0x7A nor 0x7B. (These 7-bit addresses are reserved to extend to 10-bit addressing). */
        uint64_t scr                   : 2;  /**< [ 51: 50](R/W) Scratch. Unused, but retain state. */
        uint64_t size                  : 3;  /**< [ 54: 52](R/W) Size minus one for HLC non block mode. Specifies the size in bytes of the master-mode
                                                                 operation if [SOVR] = 1. (0 = 1 byte, 1 = 2 bytes, ... 7 = 8 bytes).
                                                                 block mode's block size is defined by MIO_TWS()_TWSI_BLOCK_CTL[BLOCK_SIZE]. */
        uint64_t sovr                  : 1;  /**< [ 55: 55](R/W) Size override. If this bit is set, use the [SIZE] field to determine the master-mode
                                                                 operation size rather than what [OP] specifies. For operations greater than four bytes, the
                                                                 additional data is contained in MIO_TWS()_SW_TWSI_EXT[DATA]. In block mode,
                                                                 all data can be accessible from FIFO interface MIO_TWS()_TWSI_BLOCK_FIFO. */
        uint64_t r                     : 1;  /**< [ 56: 56](R/W/H) Read bit or result. If this bit is set on a CSR write when [SLONLY] = 0, the
                                                                 operation is a read operation (if clear, it is a write operation).
                                                                 On a CSR read, this bit returns the result indication for the most recent
                                                                 master-mode operation, 1 = success, 0 = failure. */
        uint64_t op                    : 4;  /**< [ 60: 57](R/W) Opcode field. When the register is written with [SLONLY] = 0, this field initiates one of
                                                                 the following read or write operations:
                                                                 0x0 = 7-bit byte master-mode operation.
                                                                 0x1 = 7-bit byte combined-read master-mode operation, 7-bit byte write-with-IA master-mode
                                                                 operation.
                                                                 0x2 = 10-bit byte master-mode operation.
                                                                 0x3 = 10-bit byte combined-read master-mode operation, 10-bit byte write-with-IA master-
                                                                 mode operation.
                                                                 0x4 = TWSI master-clock register, TWSI_CLK in TWSI Master Clock Register.
                                                                 0x6 = See [EOP_IA] field.
                                                                 0x8 = 7-bit 4-byte master-mode operation.
                                                                 0x9 = 7-bit 4-byte combined-read master-mode operation, 7-bit 4-byte write-with-IA master-
                                                                 mode operation.
                                                                 0xA = 10-bit 4-byte master-mode operation.
                                                                 0xB = 10-bit 4-byte combined-read master-mode operation, 10-bit 4-byte write-with-IA
                                                                 master-mode operation. */
        uint64_t eia                   : 1;  /**< [ 61: 61](R/W) Extended internal address. Sends an additional internal address byte (the MSB of [IA] is
                                                                 from MIO_TWS()_SW_TWSI_EXT[IA]). */
        uint64_t slonly                : 1;  /**< [ 62: 62](R/W) Slave only mode.

                                                                 When this bit is set, no operations are initiated with a write operation. Only the D field
                                                                 is updated in this case.

                                                                 When this bit is clear, a write operation initiates either a master-mode operation or a
                                                                 TWSI configuration register access. */
        uint64_t v                     : 1;  /**< [ 63: 63](RC/W/H) Valid bit. Set on a write operation (should always be written with a 1). Cleared when a
                                                                 TWSI master-mode operation completes, a TWSI configuration register access completes, or
                                                                 when the TWSI device reads the register if [SLONLY] = 1. */
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_mio_twsx_sw_twsi bdk_mio_twsx_sw_twsi_t;

static inline uint64_t BDK_MIO_TWSX_SW_TWSI(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_SW_TWSI(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0d0001000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0d0001000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=5))
        return 0x87e0d0001000ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001000ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_SW_TWSI", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_SW_TWSI(a) bdk_mio_twsx_sw_twsi_t
#define bustype_BDK_MIO_TWSX_SW_TWSI(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_SW_TWSI(a) "MIO_TWSX_SW_TWSI"
#define device_bar_BDK_MIO_TWSX_SW_TWSI(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_SW_TWSI(a) (a)
#define arguments_BDK_MIO_TWSX_SW_TWSI(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_sw_twsi_ext
 *
 * TWSI Software to TWSI Extension Register
 * This register contains an additional byte of internal address and four additional bytes of
 * data to be used with TWSI master-mode operations.
 *
 * The IA field is sent as the first byte of internal address when performing master-mode
 * combined-read/write-with-IA operations and MIO_TWS()_SW_TWSI[EIA] is set. The D field
 * extends the data field of MIO_TWS()_SW_TWSI for a total of 8 bytes (SOVR must be set to
 * perform operations greater than four bytes).
 */
union bdk_mio_twsx_sw_twsi_ext
{
    uint64_t u;
    struct bdk_mio_twsx_sw_twsi_ext_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_40_63        : 24;
        uint64_t ia                    : 8;  /**< [ 39: 32](R/W) Extended internal address. Sent as the first byte of internal address when performing
                                                                 master-mode combined-read/write-with-IA operations and MIO_TWS()_SW_TWSI[EIA] is set. */
        uint64_t data                  : 32; /**< [ 31:  0](R/W/H) Extended data. Extends the data field of MIO_TWS()_SW_TWSI for a total of eight bytes
                                                                 (MIO_TWS()_SW_TWSI[SOVR] must be set to 1 to perform operations greater than four
                                                                 bytes). */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 32; /**< [ 31:  0](R/W/H) Extended data. Extends the data field of MIO_TWS()_SW_TWSI for a total of eight bytes
                                                                 (MIO_TWS()_SW_TWSI[SOVR] must be set to 1 to perform operations greater than four
                                                                 bytes). */
        uint64_t ia                    : 8;  /**< [ 39: 32](R/W) Extended internal address. Sent as the first byte of internal address when performing
                                                                 master-mode combined-read/write-with-IA operations and MIO_TWS()_SW_TWSI[EIA] is set. */
        uint64_t reserved_40_63        : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_twsx_sw_twsi_ext_s cn; */
};
typedef union bdk_mio_twsx_sw_twsi_ext bdk_mio_twsx_sw_twsi_ext_t;

static inline uint64_t BDK_MIO_TWSX_SW_TWSI_EXT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_SW_TWSI_EXT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0d0001018ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0d0001018ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=5))
        return 0x87e0d0001018ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001018ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_SW_TWSI_EXT", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_SW_TWSI_EXT(a) bdk_mio_twsx_sw_twsi_ext_t
#define bustype_BDK_MIO_TWSX_SW_TWSI_EXT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_SW_TWSI_EXT(a) "MIO_TWSX_SW_TWSI_EXT"
#define device_bar_BDK_MIO_TWSX_SW_TWSI_EXT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_SW_TWSI_EXT(a) (a)
#define arguments_BDK_MIO_TWSX_SW_TWSI_EXT(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_tstamp_rx
 *
 * MIO TWSI Receive Timestamp Register
 * This register contains the timestamp latched when TWSI device receives the first bit on TWSI
 * SCL falling edge. This function is only supported in non-block mode.
 */
union bdk_mio_twsx_tstamp_rx
{
    uint64_t u;
    struct bdk_mio_twsx_tstamp_rx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t nanosec               : 64; /**< [ 63:  0](RO/H) Timestamp in nanoseconds. */
#else /* Word 0 - Little Endian */
        uint64_t nanosec               : 64; /**< [ 63:  0](RO/H) Timestamp in nanoseconds. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_twsx_tstamp_rx_s cn; */
};
typedef union bdk_mio_twsx_tstamp_rx bdk_mio_twsx_tstamp_rx_t;

static inline uint64_t BDK_MIO_TWSX_TSTAMP_RX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_TSTAMP_RX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001068ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_TSTAMP_RX", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_TSTAMP_RX(a) bdk_mio_twsx_tstamp_rx_t
#define bustype_BDK_MIO_TWSX_TSTAMP_RX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_TSTAMP_RX(a) "MIO_TWSX_TSTAMP_RX"
#define device_bar_BDK_MIO_TWSX_TSTAMP_RX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_TSTAMP_RX(a) (a)
#define arguments_BDK_MIO_TWSX_TSTAMP_RX(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_tstamp_tx
 *
 * MIO TWSI Transmit Timestamp Register
 * This register contains the timestamp latched when TWSI device drives the first bit on TWSI
 * SCL falling edge. This function is only supported in non-block mode.
 */
union bdk_mio_twsx_tstamp_tx
{
    uint64_t u;
    struct bdk_mio_twsx_tstamp_tx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t nanosec               : 64; /**< [ 63:  0](RO/H) Timestamp in nanoseconds. */
#else /* Word 0 - Little Endian */
        uint64_t nanosec               : 64; /**< [ 63:  0](RO/H) Timestamp in nanoseconds. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_twsx_tstamp_tx_s cn; */
};
typedef union bdk_mio_twsx_tstamp_tx bdk_mio_twsx_tstamp_tx_t;

static inline uint64_t BDK_MIO_TWSX_TSTAMP_TX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_TSTAMP_TX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001060ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_TSTAMP_TX", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_TSTAMP_TX(a) bdk_mio_twsx_tstamp_tx_t
#define bustype_BDK_MIO_TWSX_TSTAMP_TX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_TSTAMP_TX(a) "MIO_TWSX_TSTAMP_TX"
#define device_bar_BDK_MIO_TWSX_TSTAMP_TX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_TSTAMP_TX(a) (a)
#define arguments_BDK_MIO_TWSX_TSTAMP_TX(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_twsi_block_ctl
 *
 * TWSI Block Mode Control Register
 * This register contains the control bits when TWSI is in block mode. Data can be written/read
 * from MIO_TWS()_TWSI_BLOCK_FIFO[DATA] sequentially in block mode.
 */
union bdk_mio_twsx_twsi_block_ctl
{
    uint64_t u;
    struct bdk_mio_twsx_twsi_block_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_33_63        : 31;
        uint64_t slave_vbyte           : 1;  /**< [ 32: 32](R/W) When this mode set to 1 in block mode, slave mode read response will include
                                                                 an extra V byte rest of the data transmission. The definition of V byte is compatible
                                                                 with 1 byte/4 byte slave response. */
        uint64_t reserved_27_31        : 5;
        uint64_t block_thresh          : 11; /**< [ 26: 16](R/W) Block mode interrupt threshold, from 0-1024, 0x0 disables the
                                                                 interrupt. MIO_TWS()_INT[BLOCK_INT] interrupt will fire when the number of
                                                                 remaining bytes to be sent/received is less than threshold. If the number of
                                                                 bytes to be sent/received is less than threshold [BLOCK_THRESH], the interrupt
                                                                 will fire immediately. This interrupt is enabled only in HLC block mode. */
        uint64_t reserved_10_15        : 6;
        uint64_t block_size            : 10; /**< [  9:  0](R/W) Block mode FIFO transmission/receiving data size minus one,
                                                                 valid value from 0-1023, corresponding to 1-1024 bytes to be sent/received. */
#else /* Word 0 - Little Endian */
        uint64_t block_size            : 10; /**< [  9:  0](R/W) Block mode FIFO transmission/receiving data size minus one,
                                                                 valid value from 0-1023, corresponding to 1-1024 bytes to be sent/received. */
        uint64_t reserved_10_15        : 6;
        uint64_t block_thresh          : 11; /**< [ 26: 16](R/W) Block mode interrupt threshold, from 0-1024, 0x0 disables the
                                                                 interrupt. MIO_TWS()_INT[BLOCK_INT] interrupt will fire when the number of
                                                                 remaining bytes to be sent/received is less than threshold. If the number of
                                                                 bytes to be sent/received is less than threshold [BLOCK_THRESH], the interrupt
                                                                 will fire immediately. This interrupt is enabled only in HLC block mode. */
        uint64_t reserved_27_31        : 5;
        uint64_t slave_vbyte           : 1;  /**< [ 32: 32](R/W) When this mode set to 1 in block mode, slave mode read response will include
                                                                 an extra V byte rest of the data transmission. The definition of V byte is compatible
                                                                 with 1 byte/4 byte slave response. */
        uint64_t reserved_33_63        : 31;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_twsx_twsi_block_ctl_s cn; */
};
typedef union bdk_mio_twsx_twsi_block_ctl bdk_mio_twsx_twsi_block_ctl_t;

static inline uint64_t BDK_MIO_TWSX_TWSI_BLOCK_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_TWSI_BLOCK_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001048ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_TWSI_BLOCK_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_TWSI_BLOCK_CTL(a) bdk_mio_twsx_twsi_block_ctl_t
#define bustype_BDK_MIO_TWSX_TWSI_BLOCK_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_TWSI_BLOCK_CTL(a) "MIO_TWSX_TWSI_BLOCK_CTL"
#define device_bar_BDK_MIO_TWSX_TWSI_BLOCK_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_TWSI_BLOCK_CTL(a) (a)
#define arguments_BDK_MIO_TWSX_TWSI_BLOCK_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_twsi_block_fifo
 *
 * TWSI Block Mode Data Register
 * This register is only valid in HLC block mode. This register allows software to
 * push or pop block of data (up to 1024 bytes) to/from TWSI device through FIFO interface.
 * For TWSI writes, software does a serial of writes to fill up the FIFO before
 * starting the TWSI HLC transaction. For TWSI reads, software needs to do a serial of reads
 * after TWSI transaction finished indicated by MIO_TWS()_TWSI_BLOCK_STS[BUSY] or interrupt.
 * The order of data transmitted on TWSI bus is:
 * _  {D1, D2, D3, D4, D5, ...., D[block_size]}, with MSB of each byte transmitted first.
 *
 * The FIFO pointer needs to be reset by writing 1 to MIO_TWS()_TWSI_BLOCK_STS[RESET_PTR] before
 * software accesses.
 * The order of software writes or reads through MIO_TWS()_TWSI_BLOCK_FIFO[DATA] is:
 *   _ 1st  push/pop {D1, D2, D3, D4, D5, D6, D7}.
 *   _ 2nd  push/pop {D8, D9, D10, D11, D12, D13, D14, D15}.
 *   _ 3rd ....
 *   _ last push/pop {D[block_size-1], D[block_size], 0, 0, 0, 0, 0, 0}.
 *
 * Where block_size is MIO_TWS()_TWSI_BLOCK_CTL[BLOCK_SIZE].
 */
union bdk_mio_twsx_twsi_block_fifo
{
    uint64_t u;
    struct bdk_mio_twsx_twsi_block_fifo_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W/H) Data field that linked to the entires in FIFO based on current software pointer.
                                                                 [DATA] contains 8 bytes, corresponding to {Dn, Dn+1, Dn+2, Dn+3, Dn+4, Dn+5, Dn+6,
                                                                 Dn+7}. In case of less than 8 bytes based on MIO_TWS()_TWSI_BLOCK_CTL[BLOCK_SIZE],
                                                                 lower bits will be ignored. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 64; /**< [ 63:  0](R/W/H) Data field that linked to the entires in FIFO based on current software pointer.
                                                                 [DATA] contains 8 bytes, corresponding to {Dn, Dn+1, Dn+2, Dn+3, Dn+4, Dn+5, Dn+6,
                                                                 Dn+7}. In case of less than 8 bytes based on MIO_TWS()_TWSI_BLOCK_CTL[BLOCK_SIZE],
                                                                 lower bits will be ignored. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_twsx_twsi_block_fifo_s cn; */
};
typedef union bdk_mio_twsx_twsi_block_fifo bdk_mio_twsx_twsi_block_fifo_t;

static inline uint64_t BDK_MIO_TWSX_TWSI_BLOCK_FIFO(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_TWSI_BLOCK_FIFO(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001058ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_TWSI_BLOCK_FIFO", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_TWSI_BLOCK_FIFO(a) bdk_mio_twsx_twsi_block_fifo_t
#define bustype_BDK_MIO_TWSX_TWSI_BLOCK_FIFO(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_TWSI_BLOCK_FIFO(a) "MIO_TWSX_TWSI_BLOCK_FIFO"
#define device_bar_BDK_MIO_TWSX_TWSI_BLOCK_FIFO(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_TWSI_BLOCK_FIFO(a) (a)
#define arguments_BDK_MIO_TWSX_TWSI_BLOCK_FIFO(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_twsi_block_sts
 *
 * TWSI Block Mode Stauts Register
 * This register contains maintenance and status in block mode.
 */
union bdk_mio_twsx_twsi_block_sts
{
    uint64_t u;
    struct bdk_mio_twsx_twsi_block_sts_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_20_63        : 44;
        uint64_t sw_ptr                : 7;  /**< [ 19: 13](RO/H) Latest software reading/writing pointer to MIO_TWS()_TWSI_BLOCK_FIFO, valid from 0-127.
                                                                 For debugging purpose. */
        uint64_t count                 : 11; /**< [ 12:  2](RO/H) Remaining number of bytes waiting to be sent/received on TWSI bus in block mode.
                                                                 Value 0x0 is expected when no TWSI transaction pending. */
        uint64_t busy                  : 1;  /**< [  1:  1](RO/H) Reading back with value '1' means TWSI BLOCK transaction is still in progress, and
                                                                 0x0 is expected when no TWSI block transaction pending or in progress. */
        uint64_t reset_ptr             : 1;  /**< [  0:  0](R/W1/H) Reset software side.  FIFO pointer that accessible by MIO_TWS()_TWSI_BLOCK_FIFO.
                                                                 This bit needs to be written with 1 before any software accesses to
                                                                 MIO_TWS()_TWSI_BLOCK_FIFO.  Resetting the pointer won't affect the data
                                                                 stored in the FIFO as well as hardware side pointer. Reading back 0 indicates
                                                                 the software write/read pointers are reset to initial value zero. */
#else /* Word 0 - Little Endian */
        uint64_t reset_ptr             : 1;  /**< [  0:  0](R/W1/H) Reset software side.  FIFO pointer that accessible by MIO_TWS()_TWSI_BLOCK_FIFO.
                                                                 This bit needs to be written with 1 before any software accesses to
                                                                 MIO_TWS()_TWSI_BLOCK_FIFO.  Resetting the pointer won't affect the data
                                                                 stored in the FIFO as well as hardware side pointer. Reading back 0 indicates
                                                                 the software write/read pointers are reset to initial value zero. */
        uint64_t busy                  : 1;  /**< [  1:  1](RO/H) Reading back with value '1' means TWSI BLOCK transaction is still in progress, and
                                                                 0x0 is expected when no TWSI block transaction pending or in progress. */
        uint64_t count                 : 11; /**< [ 12:  2](RO/H) Remaining number of bytes waiting to be sent/received on TWSI bus in block mode.
                                                                 Value 0x0 is expected when no TWSI transaction pending. */
        uint64_t sw_ptr                : 7;  /**< [ 19: 13](RO/H) Latest software reading/writing pointer to MIO_TWS()_TWSI_BLOCK_FIFO, valid from 0-127.
                                                                 For debugging purpose. */
        uint64_t reserved_20_63        : 44;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_twsx_twsi_block_sts_s cn; */
};
typedef union bdk_mio_twsx_twsi_block_sts bdk_mio_twsx_twsi_block_sts_t;

static inline uint64_t BDK_MIO_TWSX_TWSI_BLOCK_STS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_TWSI_BLOCK_STS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001050ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_TWSI_BLOCK_STS", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_TWSI_BLOCK_STS(a) bdk_mio_twsx_twsi_block_sts_t
#define bustype_BDK_MIO_TWSX_TWSI_BLOCK_STS(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_TWSI_BLOCK_STS(a) "MIO_TWSX_TWSI_BLOCK_STS"
#define device_bar_BDK_MIO_TWSX_TWSI_BLOCK_STS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_TWSI_BLOCK_STS(a) (a)
#define arguments_BDK_MIO_TWSX_TWSI_BLOCK_STS(a) (a),-1,-1,-1

/**
 * Register (RSL) mio_tws#_twsi_sw
 *
 * TWSI to Software Register
 * This register allows the TWSI device to transfer data to software and later check that
 * software has received the information.
 * This register should be read or written by the TWSI device, and read by software. The TWSI
 * device can use one-byte or four-byte payload write operations, and two-byte payload read
 * operations. The TWSI device considers this register valid when [V] = 1.
 */
union bdk_mio_twsx_twsi_sw
{
    uint64_t u;
    struct bdk_mio_twsx_twsi_sw_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t v                     : 2;  /**< [ 63: 62](RC/W/H) Valid bits. These bits are not directly writable. They are set to 11 on any write
                                                                 operation by the TWSI device. They are cleared to 00 on any read operation by software. */
        uint64_t reserved_32_61        : 30;
        uint64_t data                  : 32; /**< [ 31:  0](RO/H) Data field. Updated on a write operation by the TWSI device. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 32; /**< [ 31:  0](RO/H) Data field. Updated on a write operation by the TWSI device. */
        uint64_t reserved_32_61        : 30;
        uint64_t v                     : 2;  /**< [ 63: 62](RC/W/H) Valid bits. These bits are not directly writable. They are set to 11 on any write
                                                                 operation by the TWSI device. They are cleared to 00 on any read operation by software. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_mio_twsx_twsi_sw_s cn8; */
    struct bdk_mio_twsx_twsi_sw_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t v                     : 2;  /**< [ 63: 62](RC/W/H) Valid bits. These bits are not directly writable. They are set to 11 on any write
                                                                 operation by the TWSI device. They are cleared to 00 on any read operation by software. */
        uint64_t reserved_32_61        : 30;
        uint64_t data                  : 32; /**< [ 31:  0](RO/H) Data field, bytes 3-7. Updated on a write operation by the TWSI device. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 32; /**< [ 31:  0](RO/H) Data field, bytes 3-7. Updated on a write operation by the TWSI device. */
        uint64_t reserved_32_61        : 30;
        uint64_t v                     : 2;  /**< [ 63: 62](RC/W/H) Valid bits. These bits are not directly writable. They are set to 11 on any write
                                                                 operation by the TWSI device. They are cleared to 00 on any read operation by software. */
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_mio_twsx_twsi_sw bdk_mio_twsx_twsi_sw_t;

static inline uint64_t BDK_MIO_TWSX_TWSI_SW(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_MIO_TWSX_TWSI_SW(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0d0001008ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=1))
        return 0x87e0d0001008ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=5))
        return 0x87e0d0001008ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=5))
        return 0x87e0d0001008ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("MIO_TWSX_TWSI_SW", 1, a, 0, 0, 0);
}

#define typedef_BDK_MIO_TWSX_TWSI_SW(a) bdk_mio_twsx_twsi_sw_t
#define bustype_BDK_MIO_TWSX_TWSI_SW(a) BDK_CSR_TYPE_RSL
#define basename_BDK_MIO_TWSX_TWSI_SW(a) "MIO_TWSX_TWSI_SW"
#define device_bar_BDK_MIO_TWSX_TWSI_SW(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_MIO_TWSX_TWSI_SW(a) (a)
#define arguments_BDK_MIO_TWSX_TWSI_SW(a) (a),-1,-1,-1

#endif /* __BDK_CSRS_MIO_TWS_H__ */
