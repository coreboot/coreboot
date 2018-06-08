#ifndef __BDK_CSRS_UAA_H__
#define __BDK_CSRS_UAA_H__
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
 * Cavium UAA.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration uaa_bar_e
 *
 * UART Base Address Register Enumeration
 * Enumerates the base address registers.
 */
#define BDK_UAA_BAR_E_UAAX_PF_BAR0_CN9(a) (0x87e028000000ll + 0x1000000ll * (a))
#define BDK_UAA_BAR_E_UAAX_PF_BAR0_CN9_SIZE 0x10000ull
#define BDK_UAA_BAR_E_UAAX_PF_BAR0_CN81XX(a) (0x87e028000000ll + 0x1000000ll * (a))
#define BDK_UAA_BAR_E_UAAX_PF_BAR0_CN81XX_SIZE 0x100000ull
#define BDK_UAA_BAR_E_UAAX_PF_BAR0_CN88XX(a) (0x87e024000000ll + 0x1000000ll * (a))
#define BDK_UAA_BAR_E_UAAX_PF_BAR0_CN88XX_SIZE 0x100000ull
#define BDK_UAA_BAR_E_UAAX_PF_BAR0_CN83XX(a) (0x87e028000000ll + 0x1000000ll * (a))
#define BDK_UAA_BAR_E_UAAX_PF_BAR0_CN83XX_SIZE 0x100000ull
#define BDK_UAA_BAR_E_UAAX_PF_BAR4_CN9(a) (0x87e028f00000ll + 0x1000000ll * (a))
#define BDK_UAA_BAR_E_UAAX_PF_BAR4_CN9_SIZE 0x100000ull
#define BDK_UAA_BAR_E_UAAX_PF_BAR4_CN81XX(a) (0x87e028f00000ll + 0x1000000ll * (a))
#define BDK_UAA_BAR_E_UAAX_PF_BAR4_CN81XX_SIZE 0x100000ull
#define BDK_UAA_BAR_E_UAAX_PF_BAR4_CN88XX(a) (0x87e024f00000ll + 0x1000000ll * (a))
#define BDK_UAA_BAR_E_UAAX_PF_BAR4_CN88XX_SIZE 0x100000ull
#define BDK_UAA_BAR_E_UAAX_PF_BAR4_CN83XX(a) (0x87e028f00000ll + 0x1000000ll * (a))
#define BDK_UAA_BAR_E_UAAX_PF_BAR4_CN83XX_SIZE 0x100000ull

/**
 * Enumeration uaa_int_vec_e
 *
 * UART MSI-X Vector Enumeration
 * Enumerates the MSI-X interrupt vectors.
 */
#define BDK_UAA_INT_VEC_E_INTS (0)
#define BDK_UAA_INT_VEC_E_INTS_CLEAR (1)

/**
 * Register (RSL32b) uaa#_cidr0
 *
 * UART Component Identification Register 0
 */
union bdk_uaax_cidr0
{
    uint32_t u;
    struct bdk_uaax_cidr0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_cidr0_s cn; */
};
typedef union bdk_uaax_cidr0 bdk_uaax_cidr0_t;

static inline uint64_t BDK_UAAX_CIDR0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_CIDR0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000ff0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000ff0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000ff0ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000ff0ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_CIDR0", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_CIDR0(a) bdk_uaax_cidr0_t
#define bustype_BDK_UAAX_CIDR0(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_CIDR0(a) "UAAX_CIDR0"
#define device_bar_BDK_UAAX_CIDR0(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_CIDR0(a) (a)
#define arguments_BDK_UAAX_CIDR0(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_cidr1
 *
 * UART Component Identification Register 1
 */
union bdk_uaax_cidr1
{
    uint32_t u;
    struct bdk_uaax_cidr1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_cidr1_s cn; */
};
typedef union bdk_uaax_cidr1 bdk_uaax_cidr1_t;

static inline uint64_t BDK_UAAX_CIDR1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_CIDR1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000ff4ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000ff4ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000ff4ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000ff4ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_CIDR1", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_CIDR1(a) bdk_uaax_cidr1_t
#define bustype_BDK_UAAX_CIDR1(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_CIDR1(a) "UAAX_CIDR1"
#define device_bar_BDK_UAAX_CIDR1(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_CIDR1(a) (a)
#define arguments_BDK_UAAX_CIDR1(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_cidr2
 *
 * UART Component Identification Register 2
 */
union bdk_uaax_cidr2
{
    uint32_t u;
    struct bdk_uaax_cidr2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_cidr2_s cn; */
};
typedef union bdk_uaax_cidr2 bdk_uaax_cidr2_t;

static inline uint64_t BDK_UAAX_CIDR2(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_CIDR2(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000ff8ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000ff8ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000ff8ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000ff8ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_CIDR2", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_CIDR2(a) bdk_uaax_cidr2_t
#define bustype_BDK_UAAX_CIDR2(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_CIDR2(a) "UAAX_CIDR2"
#define device_bar_BDK_UAAX_CIDR2(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_CIDR2(a) (a)
#define arguments_BDK_UAAX_CIDR2(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_cidr3
 *
 * UART Component Identification Register 3
 */
union bdk_uaax_cidr3
{
    uint32_t u;
    struct bdk_uaax_cidr3_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
#else /* Word 0 - Little Endian */
        uint32_t preamble              : 8;  /**< [  7:  0](RO) Preamble identification value. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_cidr3_s cn; */
};
typedef union bdk_uaax_cidr3 bdk_uaax_cidr3_t;

static inline uint64_t BDK_UAAX_CIDR3(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_CIDR3(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000ffcll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000ffcll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000ffcll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000ffcll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_CIDR3", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_CIDR3(a) bdk_uaax_cidr3_t
#define bustype_BDK_UAAX_CIDR3(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_CIDR3(a) "UAAX_CIDR3"
#define device_bar_BDK_UAAX_CIDR3(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_CIDR3(a) (a)
#define arguments_BDK_UAAX_CIDR3(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_cr
 *
 * UART Control Register
 */
union bdk_uaax_cr
{
    uint32_t u;
    struct bdk_uaax_cr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_16_31        : 16;
        uint32_t ctsen                 : 1;  /**< [ 15: 15](R/W) "CTS hardware flow control enable. If set, data is only transmitted when UART#_CTS_L is
                                                                 asserted (low)." */
        uint32_t rtsen                 : 1;  /**< [ 14: 14](R/W) RTS hardware flow control enable. If set, data is only requested when space in the receive FIFO. */
        uint32_t out2                  : 1;  /**< [ 13: 13](R/W) Unused. */
        uint32_t out1                  : 1;  /**< [ 12: 12](R/W) Data carrier detect. If set, drive UART#_DCD_L asserted (low). */
        uint32_t rts                   : 1;  /**< [ 11: 11](R/W) Request to send. If set, assert UART#_RTS_L. */
        uint32_t dtr                   : 1;  /**< [ 10: 10](R/W) Data terminal ready. If set, assert UART#_DTR_N. */
        uint32_t rxe                   : 1;  /**< [  9:  9](R/W) Receive enable. If set, receive section is enabled. */
        uint32_t txe                   : 1;  /**< [  8:  8](R/W) Transmit enable. */
        uint32_t lbe                   : 1;  /**< [  7:  7](R/W) "Loopback enable. If set, the serial output is looped into the serial input as if
                                                                 UART#_SIN
                                                                 was physically attached to UART#_SOUT." */
        uint32_t reserved_1_6          : 6;
        uint32_t uarten                : 1;  /**< [  0:  0](R/W) UART enable.
                                                                 0 = UART is disabled. If the UART is disabled in the middle of transmission or reception,
                                                                 it completes the current character.
                                                                 1 = UART enabled. */
#else /* Word 0 - Little Endian */
        uint32_t uarten                : 1;  /**< [  0:  0](R/W) UART enable.
                                                                 0 = UART is disabled. If the UART is disabled in the middle of transmission or reception,
                                                                 it completes the current character.
                                                                 1 = UART enabled. */
        uint32_t reserved_1_6          : 6;
        uint32_t lbe                   : 1;  /**< [  7:  7](R/W) "Loopback enable. If set, the serial output is looped into the serial input as if
                                                                 UART#_SIN
                                                                 was physically attached to UART#_SOUT." */
        uint32_t txe                   : 1;  /**< [  8:  8](R/W) Transmit enable. */
        uint32_t rxe                   : 1;  /**< [  9:  9](R/W) Receive enable. If set, receive section is enabled. */
        uint32_t dtr                   : 1;  /**< [ 10: 10](R/W) Data terminal ready. If set, assert UART#_DTR_N. */
        uint32_t rts                   : 1;  /**< [ 11: 11](R/W) Request to send. If set, assert UART#_RTS_L. */
        uint32_t out1                  : 1;  /**< [ 12: 12](R/W) Data carrier detect. If set, drive UART#_DCD_L asserted (low). */
        uint32_t out2                  : 1;  /**< [ 13: 13](R/W) Unused. */
        uint32_t rtsen                 : 1;  /**< [ 14: 14](R/W) RTS hardware flow control enable. If set, data is only requested when space in the receive FIFO. */
        uint32_t ctsen                 : 1;  /**< [ 15: 15](R/W) "CTS hardware flow control enable. If set, data is only transmitted when UART#_CTS_L is
                                                                 asserted (low)." */
        uint32_t reserved_16_31        : 16;
#endif /* Word 0 - End */
    } s;
    struct bdk_uaax_cr_cn
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_16_31        : 16;
        uint32_t ctsen                 : 1;  /**< [ 15: 15](R/W) "CTS hardware flow control enable. If set, data is only transmitted when UART#_CTS_L is
                                                                 asserted (low)." */
        uint32_t rtsen                 : 1;  /**< [ 14: 14](R/W) RTS hardware flow control enable. If set, data is only requested when space in the receive FIFO. */
        uint32_t out2                  : 1;  /**< [ 13: 13](R/W) Unused. */
        uint32_t out1                  : 1;  /**< [ 12: 12](R/W) Data carrier detect. If set, drive UART#_DCD_L asserted (low). */
        uint32_t rts                   : 1;  /**< [ 11: 11](R/W) Request to send. If set, assert UART#_RTS_L. */
        uint32_t dtr                   : 1;  /**< [ 10: 10](R/W) Data terminal ready. If set, assert UART#_DTR_N. */
        uint32_t rxe                   : 1;  /**< [  9:  9](R/W) Receive enable. If set, receive section is enabled. */
        uint32_t txe                   : 1;  /**< [  8:  8](R/W) Transmit enable. */
        uint32_t lbe                   : 1;  /**< [  7:  7](R/W) "Loopback enable. If set, the serial output is looped into the serial input as if
                                                                 UART#_SIN
                                                                 was physically attached to UART#_SOUT." */
        uint32_t reserved_3_6          : 4;
        uint32_t reserved_2            : 1;
        uint32_t reserved_1            : 1;
        uint32_t uarten                : 1;  /**< [  0:  0](R/W) UART enable.
                                                                 0 = UART is disabled. If the UART is disabled in the middle of transmission or reception,
                                                                 it completes the current character.
                                                                 1 = UART enabled. */
#else /* Word 0 - Little Endian */
        uint32_t uarten                : 1;  /**< [  0:  0](R/W) UART enable.
                                                                 0 = UART is disabled. If the UART is disabled in the middle of transmission or reception,
                                                                 it completes the current character.
                                                                 1 = UART enabled. */
        uint32_t reserved_1            : 1;
        uint32_t reserved_2            : 1;
        uint32_t reserved_3_6          : 4;
        uint32_t lbe                   : 1;  /**< [  7:  7](R/W) "Loopback enable. If set, the serial output is looped into the serial input as if
                                                                 UART#_SIN
                                                                 was physically attached to UART#_SOUT." */
        uint32_t txe                   : 1;  /**< [  8:  8](R/W) Transmit enable. */
        uint32_t rxe                   : 1;  /**< [  9:  9](R/W) Receive enable. If set, receive section is enabled. */
        uint32_t dtr                   : 1;  /**< [ 10: 10](R/W) Data terminal ready. If set, assert UART#_DTR_N. */
        uint32_t rts                   : 1;  /**< [ 11: 11](R/W) Request to send. If set, assert UART#_RTS_L. */
        uint32_t out1                  : 1;  /**< [ 12: 12](R/W) Data carrier detect. If set, drive UART#_DCD_L asserted (low). */
        uint32_t out2                  : 1;  /**< [ 13: 13](R/W) Unused. */
        uint32_t rtsen                 : 1;  /**< [ 14: 14](R/W) RTS hardware flow control enable. If set, data is only requested when space in the receive FIFO. */
        uint32_t ctsen                 : 1;  /**< [ 15: 15](R/W) "CTS hardware flow control enable. If set, data is only transmitted when UART#_CTS_L is
                                                                 asserted (low)." */
        uint32_t reserved_16_31        : 16;
#endif /* Word 0 - End */
    } cn;
};
typedef union bdk_uaax_cr bdk_uaax_cr_t;

static inline uint64_t BDK_UAAX_CR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_CR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000030ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000030ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000030ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000030ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_CR", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_CR(a) bdk_uaax_cr_t
#define bustype_BDK_UAAX_CR(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_CR(a) "UAAX_CR"
#define device_bar_BDK_UAAX_CR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_CR(a) (a)
#define arguments_BDK_UAAX_CR(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_dr
 *
 * UART Data Register
 * Writing to this register pushes data to the FIFO for transmission. Reading it retrieves
 * received data from the receive FIFO.
 */
union bdk_uaax_dr
{
    uint32_t u;
    struct bdk_uaax_dr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_12_31        : 20;
        uint32_t oe                    : 1;  /**< [ 11: 11](RO/H) Overrun error. Set if data is received and FIFO was full. Cleared once a new character is
                                                                 written to the FIFO. */
        uint32_t be                    : 1;  /**< [ 10: 10](RO/H) Break error. Indicates received data input was held low for longer than a full-transmission time. */
        uint32_t pe                    : 1;  /**< [  9:  9](RO/H) Parity error. Indicates the parity did not match that expected. */
        uint32_t fe                    : 1;  /**< [  8:  8](RO/H) Framing error. Indicates that the received character did not have a stop bit. */
        uint32_t data                  : 8;  /**< [  7:  0](R/W/H) On write operations, data to transmit. On read operations, received data. */
#else /* Word 0 - Little Endian */
        uint32_t data                  : 8;  /**< [  7:  0](R/W/H) On write operations, data to transmit. On read operations, received data. */
        uint32_t fe                    : 1;  /**< [  8:  8](RO/H) Framing error. Indicates that the received character did not have a stop bit. */
        uint32_t pe                    : 1;  /**< [  9:  9](RO/H) Parity error. Indicates the parity did not match that expected. */
        uint32_t be                    : 1;  /**< [ 10: 10](RO/H) Break error. Indicates received data input was held low for longer than a full-transmission time. */
        uint32_t oe                    : 1;  /**< [ 11: 11](RO/H) Overrun error. Set if data is received and FIFO was full. Cleared once a new character is
                                                                 written to the FIFO. */
        uint32_t reserved_12_31        : 20;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_dr_s cn; */
};
typedef union bdk_uaax_dr bdk_uaax_dr_t;

static inline uint64_t BDK_UAAX_DR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_DR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000000ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_DR", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_DR(a) bdk_uaax_dr_t
#define bustype_BDK_UAAX_DR(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_DR(a) "UAAX_DR"
#define device_bar_BDK_UAAX_DR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_DR(a) (a)
#define arguments_BDK_UAAX_DR(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_fbrd
 *
 * UART Fractional Baud Rate Register
 */
union bdk_uaax_fbrd
{
    uint32_t u;
    struct bdk_uaax_fbrd_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_6_31         : 26;
        uint32_t baud_divfrac          : 6;  /**< [  5:  0](R/W) Fractional part of baud rate divisor. The output baud rate is equal to the HCLK frequency
                                                                 divided by sixteen times the value of the baud-rate divisor, as follows:

                                                                 _ baud rate = HCLK / (16 * divisor).

                                                                 Where the HCLK frequency is controlled by UAA()_UCTL_CTL[H_CLKDIV_SEL].

                                                                 Once both divisor-latch registers are set, at least eight HCLK
                                                                 cycles should be allowed to pass before transmitting or receiving data. */
#else /* Word 0 - Little Endian */
        uint32_t baud_divfrac          : 6;  /**< [  5:  0](R/W) Fractional part of baud rate divisor. The output baud rate is equal to the HCLK frequency
                                                                 divided by sixteen times the value of the baud-rate divisor, as follows:

                                                                 _ baud rate = HCLK / (16 * divisor).

                                                                 Where the HCLK frequency is controlled by UAA()_UCTL_CTL[H_CLKDIV_SEL].

                                                                 Once both divisor-latch registers are set, at least eight HCLK
                                                                 cycles should be allowed to pass before transmitting or receiving data. */
        uint32_t reserved_6_31         : 26;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_fbrd_s cn9; */
    struct bdk_uaax_fbrd_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_6_31         : 26;
        uint32_t baud_divfrac          : 6;  /**< [  5:  0](R/W) Fractional part of baud rate divisor. The output baud rate is equal to the coprocessor-
                                                                 clock frequency divided by sixteen times the value of the baud-rate divisor, as follows:

                                                                 _ baud rate = coprocessor-clock frequency / (16 * divisor).

                                                                 Note that once both divisor-latch registers are set, at least eight coprocessor-clock
                                                                 cycles should be allowed to pass before transmitting or receiving data. */
#else /* Word 0 - Little Endian */
        uint32_t baud_divfrac          : 6;  /**< [  5:  0](R/W) Fractional part of baud rate divisor. The output baud rate is equal to the coprocessor-
                                                                 clock frequency divided by sixteen times the value of the baud-rate divisor, as follows:

                                                                 _ baud rate = coprocessor-clock frequency / (16 * divisor).

                                                                 Note that once both divisor-latch registers are set, at least eight coprocessor-clock
                                                                 cycles should be allowed to pass before transmitting or receiving data. */
        uint32_t reserved_6_31         : 26;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_uaax_fbrd_s cn88xx; */
    /* struct bdk_uaax_fbrd_cn81xx cn83xx; */
};
typedef union bdk_uaax_fbrd bdk_uaax_fbrd_t;

static inline uint64_t BDK_UAAX_FBRD(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_FBRD(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000028ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000028ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000028ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000028ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_FBRD", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_FBRD(a) bdk_uaax_fbrd_t
#define bustype_BDK_UAAX_FBRD(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_FBRD(a) "UAAX_FBRD"
#define device_bar_BDK_UAAX_FBRD(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_FBRD(a) (a)
#define arguments_BDK_UAAX_FBRD(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_fr
 *
 * UART Flag Register
 */
union bdk_uaax_fr
{
    uint32_t u;
    struct bdk_uaax_fr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_9_31         : 23;
        uint32_t ri                    : 1;  /**< [  8:  8](RO/H) Complement of ring indicator. not supported. */
        uint32_t txfe                  : 1;  /**< [  7:  7](RO/H) Transmit FIFO empty. */
        uint32_t rxff                  : 1;  /**< [  6:  6](RO/H) Receive FIFO full. */
        uint32_t txff                  : 1;  /**< [  5:  5](RO/H) Transmit FIFO full. */
        uint32_t rxfe                  : 1;  /**< [  4:  4](RO/H) Receive FIFO empty. */
        uint32_t busy                  : 1;  /**< [  3:  3](RO/H) UART busy transmitting data. */
        uint32_t dcd                   : 1;  /**< [  2:  2](RO/H) Data carrier detect. */
        uint32_t dsr                   : 1;  /**< [  1:  1](RO/H) Data set ready. */
        uint32_t cts                   : 1;  /**< [  0:  0](RO/H) Clear to send. Complement of the UART#_CTS_L modem status input pin. */
#else /* Word 0 - Little Endian */
        uint32_t cts                   : 1;  /**< [  0:  0](RO/H) Clear to send. Complement of the UART#_CTS_L modem status input pin. */
        uint32_t dsr                   : 1;  /**< [  1:  1](RO/H) Data set ready. */
        uint32_t dcd                   : 1;  /**< [  2:  2](RO/H) Data carrier detect. */
        uint32_t busy                  : 1;  /**< [  3:  3](RO/H) UART busy transmitting data. */
        uint32_t rxfe                  : 1;  /**< [  4:  4](RO/H) Receive FIFO empty. */
        uint32_t txff                  : 1;  /**< [  5:  5](RO/H) Transmit FIFO full. */
        uint32_t rxff                  : 1;  /**< [  6:  6](RO/H) Receive FIFO full. */
        uint32_t txfe                  : 1;  /**< [  7:  7](RO/H) Transmit FIFO empty. */
        uint32_t ri                    : 1;  /**< [  8:  8](RO/H) Complement of ring indicator. not supported. */
        uint32_t reserved_9_31         : 23;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_fr_s cn; */
};
typedef union bdk_uaax_fr bdk_uaax_fr_t;

static inline uint64_t BDK_UAAX_FR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_FR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000018ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000018ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000018ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000018ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_FR", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_FR(a) bdk_uaax_fr_t
#define bustype_BDK_UAAX_FR(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_FR(a) "UAAX_FR"
#define device_bar_BDK_UAAX_FR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_FR(a) (a)
#define arguments_BDK_UAAX_FR(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_ibrd
 *
 * UART Integer Baud Rate Register
 */
union bdk_uaax_ibrd
{
    uint32_t u;
    struct bdk_uaax_ibrd_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_16_31        : 16;
        uint32_t baud_divint           : 16; /**< [ 15:  0](R/W) Integer part of baud-rate divisor. See UAA()_FBRD. */
#else /* Word 0 - Little Endian */
        uint32_t baud_divint           : 16; /**< [ 15:  0](R/W) Integer part of baud-rate divisor. See UAA()_FBRD. */
        uint32_t reserved_16_31        : 16;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_ibrd_s cn; */
};
typedef union bdk_uaax_ibrd bdk_uaax_ibrd_t;

static inline uint64_t BDK_UAAX_IBRD(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_IBRD(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000024ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000024ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000024ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000024ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_IBRD", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_IBRD(a) bdk_uaax_ibrd_t
#define bustype_BDK_UAAX_IBRD(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_IBRD(a) "UAAX_IBRD"
#define device_bar_BDK_UAAX_IBRD(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_IBRD(a) (a)
#define arguments_BDK_UAAX_IBRD(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_icr
 *
 * UART Interrupt Clear Register
 * Read value is zero for this register, not the interrupt state.
 */
union bdk_uaax_icr
{
    uint32_t u;
    struct bdk_uaax_icr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_11_31        : 21;
        uint32_t oeic                  : 1;  /**< [ 10: 10](R/W1C) Overrun error interrupt clear. */
        uint32_t beic                  : 1;  /**< [  9:  9](R/W1C) Break error interrupt clear. */
        uint32_t peic                  : 1;  /**< [  8:  8](R/W1C) Parity error interrupt clear. */
        uint32_t feic                  : 1;  /**< [  7:  7](R/W1C) Framing error interrupt clear. */
        uint32_t rtic                  : 1;  /**< [  6:  6](R/W1C) Receive timeout interrupt clear. */
        uint32_t txic                  : 1;  /**< [  5:  5](R/W1C) Transmit interrupt clear. */
        uint32_t rxic                  : 1;  /**< [  4:  4](R/W1C) Receive interrupt clear. */
        uint32_t dsrmic                : 1;  /**< [  3:  3](R/W1C) DSR modem interrupt clear. */
        uint32_t dcdmic                : 1;  /**< [  2:  2](R/W1C) DCD modem interrupt clear. */
        uint32_t ctsmic                : 1;  /**< [  1:  1](R/W1C) CTS modem interrupt clear. */
        uint32_t rimic                 : 1;  /**< [  0:  0](R/W1C) Ring indicator interrupt clear. Not implemented. */
#else /* Word 0 - Little Endian */
        uint32_t rimic                 : 1;  /**< [  0:  0](R/W1C) Ring indicator interrupt clear. Not implemented. */
        uint32_t ctsmic                : 1;  /**< [  1:  1](R/W1C) CTS modem interrupt clear. */
        uint32_t dcdmic                : 1;  /**< [  2:  2](R/W1C) DCD modem interrupt clear. */
        uint32_t dsrmic                : 1;  /**< [  3:  3](R/W1C) DSR modem interrupt clear. */
        uint32_t rxic                  : 1;  /**< [  4:  4](R/W1C) Receive interrupt clear. */
        uint32_t txic                  : 1;  /**< [  5:  5](R/W1C) Transmit interrupt clear. */
        uint32_t rtic                  : 1;  /**< [  6:  6](R/W1C) Receive timeout interrupt clear. */
        uint32_t feic                  : 1;  /**< [  7:  7](R/W1C) Framing error interrupt clear. */
        uint32_t peic                  : 1;  /**< [  8:  8](R/W1C) Parity error interrupt clear. */
        uint32_t beic                  : 1;  /**< [  9:  9](R/W1C) Break error interrupt clear. */
        uint32_t oeic                  : 1;  /**< [ 10: 10](R/W1C) Overrun error interrupt clear. */
        uint32_t reserved_11_31        : 21;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_icr_s cn; */
};
typedef union bdk_uaax_icr bdk_uaax_icr_t;

static inline uint64_t BDK_UAAX_ICR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_ICR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000044ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000044ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000044ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000044ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_ICR", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_ICR(a) bdk_uaax_icr_t
#define bustype_BDK_UAAX_ICR(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_ICR(a) "UAAX_ICR"
#define device_bar_BDK_UAAX_ICR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_ICR(a) (a)
#define arguments_BDK_UAAX_ICR(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_ifls
 *
 * UART Interrupt FIFO Level Select Register
 */
union bdk_uaax_ifls
{
    uint32_t u;
    struct bdk_uaax_ifls_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_6_31         : 26;
        uint32_t rxiflsel              : 3;  /**< [  5:  3](R/W) Receive interrupt FIFO level select.
                                                                 0x0 = Receive FIFO becomes \>= 1/8 full.
                                                                 0x1 = Receive FIFO becomes \>= 1/4 full.
                                                                 0x2 = Receive FIFO becomes \>= 1/2 full.
                                                                 0x3 = Receive FIFO becomes \>= 3/4 full.
                                                                 0x4 = Receive FIFO becomes \>= 7/8 full.
                                                                 0x5-0x7 = Reserved. */
        uint32_t txiflsel              : 3;  /**< [  2:  0](R/W) Transmit interrupt FIFO level select.
                                                                 0x0 = Transmit FIFO becomes \<= 1/8 full.
                                                                 0x1 = Transmit FIFO becomes \<= 1/4 full.
                                                                 0x2 = Transmit FIFO becomes \<= 1/2 full.
                                                                 0x3 = Transmit FIFO becomes \<= 3/4 full.
                                                                 0x4 = Transmit FIFO becomes \<= 7/8 full.
                                                                 0x5-0x7 = Reserved. */
#else /* Word 0 - Little Endian */
        uint32_t txiflsel              : 3;  /**< [  2:  0](R/W) Transmit interrupt FIFO level select.
                                                                 0x0 = Transmit FIFO becomes \<= 1/8 full.
                                                                 0x1 = Transmit FIFO becomes \<= 1/4 full.
                                                                 0x2 = Transmit FIFO becomes \<= 1/2 full.
                                                                 0x3 = Transmit FIFO becomes \<= 3/4 full.
                                                                 0x4 = Transmit FIFO becomes \<= 7/8 full.
                                                                 0x5-0x7 = Reserved. */
        uint32_t rxiflsel              : 3;  /**< [  5:  3](R/W) Receive interrupt FIFO level select.
                                                                 0x0 = Receive FIFO becomes \>= 1/8 full.
                                                                 0x1 = Receive FIFO becomes \>= 1/4 full.
                                                                 0x2 = Receive FIFO becomes \>= 1/2 full.
                                                                 0x3 = Receive FIFO becomes \>= 3/4 full.
                                                                 0x4 = Receive FIFO becomes \>= 7/8 full.
                                                                 0x5-0x7 = Reserved. */
        uint32_t reserved_6_31         : 26;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_ifls_s cn; */
};
typedef union bdk_uaax_ifls bdk_uaax_ifls_t;

static inline uint64_t BDK_UAAX_IFLS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_IFLS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000034ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000034ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000034ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000034ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_IFLS", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_IFLS(a) bdk_uaax_ifls_t
#define bustype_BDK_UAAX_IFLS(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_IFLS(a) "UAAX_IFLS"
#define device_bar_BDK_UAAX_IFLS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_IFLS(a) (a)
#define arguments_BDK_UAAX_IFLS(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_imsc
 *
 * UART Interrupt Mask Set/Clear Register
 */
union bdk_uaax_imsc
{
    uint32_t u;
    struct bdk_uaax_imsc_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_11_31        : 21;
        uint32_t oeim                  : 1;  /**< [ 10: 10](R/W) Overrun error interrupt mask. */
        uint32_t beim                  : 1;  /**< [  9:  9](R/W) Break error interrupt mask. */
        uint32_t peim                  : 1;  /**< [  8:  8](R/W) Parity error interrupt mask. */
        uint32_t feim                  : 1;  /**< [  7:  7](R/W) Framing error interrupt mask. */
        uint32_t rtim                  : 1;  /**< [  6:  6](R/W) Receive timeout interrupt mask. */
        uint32_t txim                  : 1;  /**< [  5:  5](R/W) Transmit interrupt mask. */
        uint32_t rxim                  : 1;  /**< [  4:  4](R/W) Receive interrupt mask. */
        uint32_t dsrmim                : 1;  /**< [  3:  3](R/W) DSR modem interrupt mask. */
        uint32_t dcdmim                : 1;  /**< [  2:  2](R/W) DCD modem interrupt mask. */
        uint32_t ctsmim                : 1;  /**< [  1:  1](R/W) CTS modem interrupt mask. */
        uint32_t rimim                 : 1;  /**< [  0:  0](R/W) Ring indicator interrupt mask. Not implemented. */
#else /* Word 0 - Little Endian */
        uint32_t rimim                 : 1;  /**< [  0:  0](R/W) Ring indicator interrupt mask. Not implemented. */
        uint32_t ctsmim                : 1;  /**< [  1:  1](R/W) CTS modem interrupt mask. */
        uint32_t dcdmim                : 1;  /**< [  2:  2](R/W) DCD modem interrupt mask. */
        uint32_t dsrmim                : 1;  /**< [  3:  3](R/W) DSR modem interrupt mask. */
        uint32_t rxim                  : 1;  /**< [  4:  4](R/W) Receive interrupt mask. */
        uint32_t txim                  : 1;  /**< [  5:  5](R/W) Transmit interrupt mask. */
        uint32_t rtim                  : 1;  /**< [  6:  6](R/W) Receive timeout interrupt mask. */
        uint32_t feim                  : 1;  /**< [  7:  7](R/W) Framing error interrupt mask. */
        uint32_t peim                  : 1;  /**< [  8:  8](R/W) Parity error interrupt mask. */
        uint32_t beim                  : 1;  /**< [  9:  9](R/W) Break error interrupt mask. */
        uint32_t oeim                  : 1;  /**< [ 10: 10](R/W) Overrun error interrupt mask. */
        uint32_t reserved_11_31        : 21;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_imsc_s cn; */
};
typedef union bdk_uaax_imsc bdk_uaax_imsc_t;

static inline uint64_t BDK_UAAX_IMSC(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_IMSC(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000038ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000038ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000038ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000038ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_IMSC", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_IMSC(a) bdk_uaax_imsc_t
#define bustype_BDK_UAAX_IMSC(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_IMSC(a) "UAAX_IMSC"
#define device_bar_BDK_UAAX_IMSC(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_IMSC(a) (a)
#define arguments_BDK_UAAX_IMSC(a) (a),-1,-1,-1

/**
 * Register (RSL) uaa#_io_ctl
 *
 * UART IO Control Register
 * This register controls the UAA[0..1] IO drive strength and slew rates.  The additional
 * UAA interfaces are controlled by GPIO_IO_CTL[DRIVEx] and GPIO_IO_CTL[SLEWx] based
 * on the selected pins.
 */
union bdk_uaax_io_ctl
{
    uint64_t u;
    struct bdk_uaax_io_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t drive                 : 2;  /**< [  3:  2](R/W) UART bus pin output drive strength.
                                                                 0x0 = 2 mA.
                                                                 0x1 = 4 mA.
                                                                 0x2 = 8 mA.
                                                                 0x3 = 16 mA. */
        uint64_t reserved_1            : 1;
        uint64_t slew                  : 1;  /**< [  0:  0](R/W) UART bus pin output slew rate control.
                                                                 0 = Low slew rate.
                                                                 1 = High slew rate. */
#else /* Word 0 - Little Endian */
        uint64_t slew                  : 1;  /**< [  0:  0](R/W) UART bus pin output slew rate control.
                                                                 0 = Low slew rate.
                                                                 1 = High slew rate. */
        uint64_t reserved_1            : 1;
        uint64_t drive                 : 2;  /**< [  3:  2](R/W) UART bus pin output drive strength.
                                                                 0x0 = 2 mA.
                                                                 0x1 = 4 mA.
                                                                 0x2 = 8 mA.
                                                                 0x3 = 16 mA. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_io_ctl_s cn; */
};
typedef union bdk_uaax_io_ctl bdk_uaax_io_ctl_t;

static inline uint64_t BDK_UAAX_IO_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_IO_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028001028ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_IO_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_IO_CTL(a) bdk_uaax_io_ctl_t
#define bustype_BDK_UAAX_IO_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_UAAX_IO_CTL(a) "UAAX_IO_CTL"
#define device_bar_BDK_UAAX_IO_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_IO_CTL(a) (a)
#define arguments_BDK_UAAX_IO_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_lcr_h
 *
 * UART Line Control Register
 */
union bdk_uaax_lcr_h
{
    uint32_t u;
    struct bdk_uaax_lcr_h_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t sps                   : 1;  /**< [  7:  7](R/W) Stick parity select. If set, and [PEN] is set, forces the parity bit to the opposite of EPS. */
        uint32_t wlen                  : 2;  /**< [  6:  5](R/W) Word length:
                                                                 0x0 = 5 bits.
                                                                 0x1 = 6 bits.
                                                                 0x2 = 7 bits.
                                                                 0x3 = 8 bits. */
        uint32_t fen                   : 1;  /**< [  4:  4](R/W) Enable FIFOs.
                                                                 0 = FIFOs disabled, FIFOs are single character deep.
                                                                 1 = FIFO enabled. */
        uint32_t stp2                  : 1;  /**< [  3:  3](R/W) Two stop bits select. */
        uint32_t eps                   : 1;  /**< [  2:  2](R/W) Even parity select. */
        uint32_t pen                   : 1;  /**< [  1:  1](R/W) Parity enable. */
        uint32_t brk                   : 1;  /**< [  0:  0](R/W) Send break. A low level is continually transmitted after completion of the current character. */
#else /* Word 0 - Little Endian */
        uint32_t brk                   : 1;  /**< [  0:  0](R/W) Send break. A low level is continually transmitted after completion of the current character. */
        uint32_t pen                   : 1;  /**< [  1:  1](R/W) Parity enable. */
        uint32_t eps                   : 1;  /**< [  2:  2](R/W) Even parity select. */
        uint32_t stp2                  : 1;  /**< [  3:  3](R/W) Two stop bits select. */
        uint32_t fen                   : 1;  /**< [  4:  4](R/W) Enable FIFOs.
                                                                 0 = FIFOs disabled, FIFOs are single character deep.
                                                                 1 = FIFO enabled. */
        uint32_t wlen                  : 2;  /**< [  6:  5](R/W) Word length:
                                                                 0x0 = 5 bits.
                                                                 0x1 = 6 bits.
                                                                 0x2 = 7 bits.
                                                                 0x3 = 8 bits. */
        uint32_t sps                   : 1;  /**< [  7:  7](R/W) Stick parity select. If set, and [PEN] is set, forces the parity bit to the opposite of EPS. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_lcr_h_s cn; */
};
typedef union bdk_uaax_lcr_h bdk_uaax_lcr_h_t;

static inline uint64_t BDK_UAAX_LCR_H(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_LCR_H(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e02800002cll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e02800002cll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e02400002cll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e02800002cll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_LCR_H", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_LCR_H(a) bdk_uaax_lcr_h_t
#define bustype_BDK_UAAX_LCR_H(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_LCR_H(a) "UAAX_LCR_H"
#define device_bar_BDK_UAAX_LCR_H(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_LCR_H(a) (a)
#define arguments_BDK_UAAX_LCR_H(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_mis
 *
 * UART Masked Interrupt Status Register
 * Indicates state of interrupts after masking.
 * Internal:
 * Note this register was not present in SBSA 2.3, but is referenced
 * by the Linux driver, so has been defined here.
 */
union bdk_uaax_mis
{
    uint32_t u;
    struct bdk_uaax_mis_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_11_31        : 21;
        uint32_t oemis                 : 1;  /**< [ 10: 10](RO/H) Overrun error interrupt status. */
        uint32_t bemis                 : 1;  /**< [  9:  9](RO/H) Break error interrupt status. */
        uint32_t pemis                 : 1;  /**< [  8:  8](RO/H) Parity error interrupt status. */
        uint32_t femis                 : 1;  /**< [  7:  7](RO/H) Framing error interrupt status. */
        uint32_t rtmis                 : 1;  /**< [  6:  6](RO/H) Receive timeout interrupt status. */
        uint32_t txmis                 : 1;  /**< [  5:  5](RO/H) Transmit interrupt status. */
        uint32_t rxmis                 : 1;  /**< [  4:  4](RO/H) Receive interrupt status. */
        uint32_t dsrmmis               : 1;  /**< [  3:  3](RO/H) DSR modem interrupt status. */
        uint32_t dcdmmis               : 1;  /**< [  2:  2](RO/H) DCD modem interrupt status. */
        uint32_t ctsmmis               : 1;  /**< [  1:  1](RO/H) CTS modem interrupt status. */
        uint32_t rimmis                : 1;  /**< [  0:  0](RO/H) Ring indicator interrupt status. Not implemented. */
#else /* Word 0 - Little Endian */
        uint32_t rimmis                : 1;  /**< [  0:  0](RO/H) Ring indicator interrupt status. Not implemented. */
        uint32_t ctsmmis               : 1;  /**< [  1:  1](RO/H) CTS modem interrupt status. */
        uint32_t dcdmmis               : 1;  /**< [  2:  2](RO/H) DCD modem interrupt status. */
        uint32_t dsrmmis               : 1;  /**< [  3:  3](RO/H) DSR modem interrupt status. */
        uint32_t rxmis                 : 1;  /**< [  4:  4](RO/H) Receive interrupt status. */
        uint32_t txmis                 : 1;  /**< [  5:  5](RO/H) Transmit interrupt status. */
        uint32_t rtmis                 : 1;  /**< [  6:  6](RO/H) Receive timeout interrupt status. */
        uint32_t femis                 : 1;  /**< [  7:  7](RO/H) Framing error interrupt status. */
        uint32_t pemis                 : 1;  /**< [  8:  8](RO/H) Parity error interrupt status. */
        uint32_t bemis                 : 1;  /**< [  9:  9](RO/H) Break error interrupt status. */
        uint32_t oemis                 : 1;  /**< [ 10: 10](RO/H) Overrun error interrupt status. */
        uint32_t reserved_11_31        : 21;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_mis_s cn; */
};
typedef union bdk_uaax_mis bdk_uaax_mis_t;

static inline uint64_t BDK_UAAX_MIS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_MIS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000040ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000040ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000040ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000040ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_MIS", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_MIS(a) bdk_uaax_mis_t
#define bustype_BDK_UAAX_MIS(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_MIS(a) "UAAX_MIS"
#define device_bar_BDK_UAAX_MIS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_MIS(a) (a)
#define arguments_BDK_UAAX_MIS(a) (a),-1,-1,-1

/**
 * Register (RSL) uaa#_msix_pba#
 *
 * UART MSI-X Pending Bit Array Registers
 * This register is the MSI-X PBA table, the bit number is indexed by the UAA_INT_VEC_E enumeration.
 */
union bdk_uaax_msix_pbax
{
    uint64_t u;
    struct bdk_uaax_msix_pbax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO/H) Pending message for each interrupt, enumerated by UAA_INT_VEC_E. Bits that have no
                                                                 associated UAA_INT_VEC_E are zero. */
#else /* Word 0 - Little Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO/H) Pending message for each interrupt, enumerated by UAA_INT_VEC_E. Bits that have no
                                                                 associated UAA_INT_VEC_E are zero. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_msix_pbax_s cn; */
};
typedef union bdk_uaax_msix_pbax bdk_uaax_msix_pbax_t;

static inline uint64_t BDK_UAAX_MSIX_PBAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_MSIX_PBAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b==0)))
        return 0x87e028ff0000ll + 0x1000000ll * ((a) & 0x3) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b==0)))
        return 0x87e028ff0000ll + 0x1000000ll * ((a) & 0x3) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b==0)))
        return 0x87e024ff0000ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=7) && (b==0)))
        return 0x87e028ff0000ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x0);
    __bdk_csr_fatal("UAAX_MSIX_PBAX", 2, a, b, 0, 0);
}

#define typedef_BDK_UAAX_MSIX_PBAX(a,b) bdk_uaax_msix_pbax_t
#define bustype_BDK_UAAX_MSIX_PBAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_UAAX_MSIX_PBAX(a,b) "UAAX_MSIX_PBAX"
#define device_bar_BDK_UAAX_MSIX_PBAX(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_UAAX_MSIX_PBAX(a,b) (a)
#define arguments_BDK_UAAX_MSIX_PBAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) uaa#_msix_vec#_addr
 *
 * UART MSI-X Vector Table Address Registers
 * This register is the MSI-X vector table, indexed by the UAA_INT_VEC_E enumeration.
 */
union bdk_uaax_msix_vecx_addr
{
    uint64_t u;
    struct bdk_uaax_msix_vecx_addr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_53_63        : 11;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's UAA()_MSIX_VEC()_ADDR, UAA()_MSIX_VEC()_CTL, and
                                                                 corresponding bit of UAA()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_UAA(0..1)_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's UAA()_MSIX_VEC()_ADDR, UAA()_MSIX_VEC()_CTL, and
                                                                 corresponding bit of UAA()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_UAA(0..1)_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_53_63        : 11;
#endif /* Word 0 - End */
    } s;
    struct bdk_uaax_msix_vecx_addr_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_53_63        : 11;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's UAA()_MSIX_VEC()_ADDR, UAA()_MSIX_VEC()_CTL, and
                                                                 corresponding bit of UAA()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_UAA()_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's UAA()_MSIX_VEC()_ADDR, UAA()_MSIX_VEC()_CTL, and
                                                                 corresponding bit of UAA()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_UAA()_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_53_63        : 11;
#endif /* Word 0 - End */
    } cn9;
    struct bdk_uaax_msix_vecx_addr_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's UAA()_MSIX_VEC()_ADDR, UAA()_MSIX_VEC()_CTL, and
                                                                 corresponding bit of UAA()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_UAA(0..3)_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's UAA()_MSIX_VEC()_ADDR, UAA()_MSIX_VEC()_CTL, and
                                                                 corresponding bit of UAA()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_UAA(0..3)_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_uaax_msix_vecx_addr_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's UAA()_MSIX_VEC()_ADDR, UAA()_MSIX_VEC()_CTL, and
                                                                 corresponding bit of UAA()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_UAA(0..1)_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's UAA()_MSIX_VEC()_ADDR, UAA()_MSIX_VEC()_CTL, and
                                                                 corresponding bit of UAA()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_UAA(0..1)_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } cn88xx;
    /* struct bdk_uaax_msix_vecx_addr_cn81xx cn83xx; */
};
typedef union bdk_uaax_msix_vecx_addr bdk_uaax_msix_vecx_addr_t;

static inline uint64_t BDK_UAAX_MSIX_VECX_ADDR(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_MSIX_VECX_ADDR(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e028f00000ll + 0x1000000ll * ((a) & 0x3) + 0x10ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b<=1)))
        return 0x87e028f00000ll + 0x1000000ll * ((a) & 0x3) + 0x10ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e024f00000ll + 0x1000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=7) && (b<=1)))
        return 0x87e028f00000ll + 0x1000000ll * ((a) & 0x7) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("UAAX_MSIX_VECX_ADDR", 2, a, b, 0, 0);
}

#define typedef_BDK_UAAX_MSIX_VECX_ADDR(a,b) bdk_uaax_msix_vecx_addr_t
#define bustype_BDK_UAAX_MSIX_VECX_ADDR(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_UAAX_MSIX_VECX_ADDR(a,b) "UAAX_MSIX_VECX_ADDR"
#define device_bar_BDK_UAAX_MSIX_VECX_ADDR(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_UAAX_MSIX_VECX_ADDR(a,b) (a)
#define arguments_BDK_UAAX_MSIX_VECX_ADDR(a,b) (a),(b),-1,-1

/**
 * Register (RSL) uaa#_msix_vec#_ctl
 *
 * UART MSI-X Vector Table Control and Data Registers
 * This register is the MSI-X vector table, indexed by the UAA_INT_VEC_E enumeration.
 */
union bdk_uaax_msix_vecx_ctl
{
    uint64_t u;
    struct bdk_uaax_msix_vecx_ctl_s
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
    struct bdk_uaax_msix_vecx_ctl_cn8
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
    /* struct bdk_uaax_msix_vecx_ctl_s cn9; */
};
typedef union bdk_uaax_msix_vecx_ctl bdk_uaax_msix_vecx_ctl_t;

static inline uint64_t BDK_UAAX_MSIX_VECX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_MSIX_VECX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e028f00008ll + 0x1000000ll * ((a) & 0x3) + 0x10ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b<=1)))
        return 0x87e028f00008ll + 0x1000000ll * ((a) & 0x3) + 0x10ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=1) && (b<=1)))
        return 0x87e024f00008ll + 0x1000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=7) && (b<=1)))
        return 0x87e028f00008ll + 0x1000000ll * ((a) & 0x7) + 0x10ll * ((b) & 0x1);
    __bdk_csr_fatal("UAAX_MSIX_VECX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_UAAX_MSIX_VECX_CTL(a,b) bdk_uaax_msix_vecx_ctl_t
#define bustype_BDK_UAAX_MSIX_VECX_CTL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_UAAX_MSIX_VECX_CTL(a,b) "UAAX_MSIX_VECX_CTL"
#define device_bar_BDK_UAAX_MSIX_VECX_CTL(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_UAAX_MSIX_VECX_CTL(a,b) (a)
#define arguments_BDK_UAAX_MSIX_VECX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (RSL32b) uaa#_pidr0
 *
 * UART Component Identification Register 0
 */
union bdk_uaax_pidr0
{
    uint32_t u;
    struct bdk_uaax_pidr0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t partnum0              : 8;  /**< [  7:  0](RO) Part number \<7:0\>.  ARM-assigned PL011 compatible. */
#else /* Word 0 - Little Endian */
        uint32_t partnum0              : 8;  /**< [  7:  0](RO) Part number \<7:0\>.  ARM-assigned PL011 compatible. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_pidr0_s cn; */
};
typedef union bdk_uaax_pidr0 bdk_uaax_pidr0_t;

static inline uint64_t BDK_UAAX_PIDR0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_PIDR0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000fe0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000fe0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000fe0ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000fe0ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_PIDR0", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_PIDR0(a) bdk_uaax_pidr0_t
#define bustype_BDK_UAAX_PIDR0(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_PIDR0(a) "UAAX_PIDR0"
#define device_bar_BDK_UAAX_PIDR0(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_PIDR0(a) (a)
#define arguments_BDK_UAAX_PIDR0(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_pidr1
 *
 * UART Peripheral Identification Register 1
 */
union bdk_uaax_pidr1
{
    uint32_t u;
    struct bdk_uaax_pidr1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t idcode                : 4;  /**< [  7:  4](RO) ARM identification. */
        uint32_t partnum1              : 4;  /**< [  3:  0](RO) Part number \<11:8\>.  ARM-assigned PL011 compatible. */
#else /* Word 0 - Little Endian */
        uint32_t partnum1              : 4;  /**< [  3:  0](RO) Part number \<11:8\>.  ARM-assigned PL011 compatible. */
        uint32_t idcode                : 4;  /**< [  7:  4](RO) ARM identification. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_pidr1_s cn; */
};
typedef union bdk_uaax_pidr1 bdk_uaax_pidr1_t;

static inline uint64_t BDK_UAAX_PIDR1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_PIDR1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000fe4ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000fe4ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000fe4ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000fe4ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_PIDR1", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_PIDR1(a) bdk_uaax_pidr1_t
#define bustype_BDK_UAAX_PIDR1(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_PIDR1(a) "UAAX_PIDR1"
#define device_bar_BDK_UAAX_PIDR1(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_PIDR1(a) (a)
#define arguments_BDK_UAAX_PIDR1(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_pidr2
 *
 * UART Peripheral Identification Register 2
 */
union bdk_uaax_pidr2
{
    uint32_t u;
    struct bdk_uaax_pidr2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t revision              : 4;  /**< [  7:  4](RO) UART architectural revision.
                                                                 0x3 = r1p5. */
        uint32_t jedec                 : 1;  /**< [  3:  3](RO) JEDEC assigned. 0 = Legacy UART assignment. */
        uint32_t idcode                : 3;  /**< [  2:  0](RO) ARM-design compatible. */
#else /* Word 0 - Little Endian */
        uint32_t idcode                : 3;  /**< [  2:  0](RO) ARM-design compatible. */
        uint32_t jedec                 : 1;  /**< [  3:  3](RO) JEDEC assigned. 0 = Legacy UART assignment. */
        uint32_t revision              : 4;  /**< [  7:  4](RO) UART architectural revision.
                                                                 0x3 = r1p5. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_pidr2_s cn; */
};
typedef union bdk_uaax_pidr2 bdk_uaax_pidr2_t;

static inline uint64_t BDK_UAAX_PIDR2(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_PIDR2(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000fe8ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000fe8ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000fe8ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000fe8ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_PIDR2", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_PIDR2(a) bdk_uaax_pidr2_t
#define bustype_BDK_UAAX_PIDR2(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_PIDR2(a) "UAAX_PIDR2"
#define device_bar_BDK_UAAX_PIDR2(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_PIDR2(a) (a)
#define arguments_BDK_UAAX_PIDR2(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_pidr3
 *
 * UART Peripheral Identification Register 3
 */
union bdk_uaax_pidr3
{
    uint32_t u;
    struct bdk_uaax_pidr3_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_8_31         : 24;
        uint32_t revand                : 4;  /**< [  7:  4](RO) Manufacturer revision number. For CNXXXX always 0x0. */
        uint32_t cust                  : 4;  /**< [  3:  0](RO) Customer modified. 0x1 = Overall product information should be consulted for
                                                                 product, major and minor pass numbers. */
#else /* Word 0 - Little Endian */
        uint32_t cust                  : 4;  /**< [  3:  0](RO) Customer modified. 0x1 = Overall product information should be consulted for
                                                                 product, major and minor pass numbers. */
        uint32_t revand                : 4;  /**< [  7:  4](RO) Manufacturer revision number. For CNXXXX always 0x0. */
        uint32_t reserved_8_31         : 24;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_pidr3_s cn; */
};
typedef union bdk_uaax_pidr3 bdk_uaax_pidr3_t;

static inline uint64_t BDK_UAAX_PIDR3(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_PIDR3(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000fecll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000fecll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000fecll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000fecll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_PIDR3", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_PIDR3(a) bdk_uaax_pidr3_t
#define bustype_BDK_UAAX_PIDR3(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_PIDR3(a) "UAAX_PIDR3"
#define device_bar_BDK_UAAX_PIDR3(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_PIDR3(a) (a)
#define arguments_BDK_UAAX_PIDR3(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_pidr4
 *
 * UART Peripheral Identification Register 4
 */
union bdk_uaax_pidr4
{
    uint32_t u;
    struct bdk_uaax_pidr4_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint32_t reserved_0_31         : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_pidr4_s cn; */
};
typedef union bdk_uaax_pidr4 bdk_uaax_pidr4_t;

static inline uint64_t BDK_UAAX_PIDR4(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_PIDR4(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000fd0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000fd0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000fd0ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000fd0ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_PIDR4", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_PIDR4(a) bdk_uaax_pidr4_t
#define bustype_BDK_UAAX_PIDR4(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_PIDR4(a) "UAAX_PIDR4"
#define device_bar_BDK_UAAX_PIDR4(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_PIDR4(a) (a)
#define arguments_BDK_UAAX_PIDR4(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_pidr5
 *
 * UART Peripheral Identification Register 5
 */
union bdk_uaax_pidr5
{
    uint32_t u;
    struct bdk_uaax_pidr5_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint32_t reserved_0_31         : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_pidr5_s cn; */
};
typedef union bdk_uaax_pidr5 bdk_uaax_pidr5_t;

static inline uint64_t BDK_UAAX_PIDR5(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_PIDR5(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000fd4ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000fd4ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000fd4ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000fd4ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_PIDR5", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_PIDR5(a) bdk_uaax_pidr5_t
#define bustype_BDK_UAAX_PIDR5(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_PIDR5(a) "UAAX_PIDR5"
#define device_bar_BDK_UAAX_PIDR5(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_PIDR5(a) (a)
#define arguments_BDK_UAAX_PIDR5(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_pidr6
 *
 * UART Peripheral Identification Register 6
 */
union bdk_uaax_pidr6
{
    uint32_t u;
    struct bdk_uaax_pidr6_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint32_t reserved_0_31         : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_pidr6_s cn; */
};
typedef union bdk_uaax_pidr6 bdk_uaax_pidr6_t;

static inline uint64_t BDK_UAAX_PIDR6(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_PIDR6(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000fd8ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000fd8ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000fd8ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000fd8ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_PIDR6", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_PIDR6(a) bdk_uaax_pidr6_t
#define bustype_BDK_UAAX_PIDR6(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_PIDR6(a) "UAAX_PIDR6"
#define device_bar_BDK_UAAX_PIDR6(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_PIDR6(a) (a)
#define arguments_BDK_UAAX_PIDR6(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_pidr7
 *
 * UART Peripheral Identification Register 7
 */
union bdk_uaax_pidr7
{
    uint32_t u;
    struct bdk_uaax_pidr7_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint32_t reserved_0_31         : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_pidr7_s cn; */
};
typedef union bdk_uaax_pidr7 bdk_uaax_pidr7_t;

static inline uint64_t BDK_UAAX_PIDR7(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_PIDR7(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000fdcll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000fdcll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000fdcll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000fdcll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_PIDR7", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_PIDR7(a) bdk_uaax_pidr7_t
#define bustype_BDK_UAAX_PIDR7(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_PIDR7(a) "UAAX_PIDR7"
#define device_bar_BDK_UAAX_PIDR7(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_PIDR7(a) (a)
#define arguments_BDK_UAAX_PIDR7(a) (a),-1,-1,-1

/**
 * Register (RSL) uaa#_redirect
 *
 * UART REDIRECT Control Register
 */
union bdk_uaax_redirect
{
    uint64_t u;
    struct bdk_uaax_redirect_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t in_ena                : 1;  /**< [  3:  3](SR/W) 0 = UAA receive and modem control inputs are from hard-assigned pins or GPIO virtual pins.
                                                                 1 = UAA receive and modem control come from the UAA specified by [IN_SEL]. */
        uint64_t in_sel                : 3;  /**< [  2:  0](SR/W) 0x0 = Inputs from UAA0.
                                                                 0x1 = Inputs from UAA1.
                                                                 0x2 = Inputs from UAA2.
                                                                 0x3 = Inputs from UAA3.
                                                                 0x4 = Inputs from UAA4.
                                                                 0x5 = Inputs from UAA5.
                                                                 0x6 = Inputs from UAA6.
                                                                 0x7 = Inputs from UAA7. */
#else /* Word 0 - Little Endian */
        uint64_t in_sel                : 3;  /**< [  2:  0](SR/W) 0x0 = Inputs from UAA0.
                                                                 0x1 = Inputs from UAA1.
                                                                 0x2 = Inputs from UAA2.
                                                                 0x3 = Inputs from UAA3.
                                                                 0x4 = Inputs from UAA4.
                                                                 0x5 = Inputs from UAA5.
                                                                 0x6 = Inputs from UAA6.
                                                                 0x7 = Inputs from UAA7. */
        uint64_t in_ena                : 1;  /**< [  3:  3](SR/W) 0 = UAA receive and modem control inputs are from hard-assigned pins or GPIO virtual pins.
                                                                 1 = UAA receive and modem control come from the UAA specified by [IN_SEL]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_redirect_s cn; */
};
typedef union bdk_uaax_redirect bdk_uaax_redirect_t;

static inline uint64_t BDK_UAAX_REDIRECT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_REDIRECT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028001020ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_REDIRECT", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_REDIRECT(a) bdk_uaax_redirect_t
#define bustype_BDK_UAAX_REDIRECT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_UAAX_REDIRECT(a) "UAAX_REDIRECT"
#define device_bar_BDK_UAAX_REDIRECT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_REDIRECT(a) (a)
#define arguments_BDK_UAAX_REDIRECT(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_ris
 *
 * UART Raw Interrupt Status Register
 * Indicates state of interrupts before masking.
 */
union bdk_uaax_ris
{
    uint32_t u;
    struct bdk_uaax_ris_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_11_31        : 21;
        uint32_t oeris                 : 1;  /**< [ 10: 10](RO/H) Overrun error interrupt status. */
        uint32_t beris                 : 1;  /**< [  9:  9](RO/H) Break error interrupt status. */
        uint32_t peris                 : 1;  /**< [  8:  8](RO/H) Parity error interrupt status. */
        uint32_t feris                 : 1;  /**< [  7:  7](RO/H) Framing error interrupt status. */
        uint32_t rtris                 : 1;  /**< [  6:  6](RO/H) Receive timeout interrupt status. */
        uint32_t txris                 : 1;  /**< [  5:  5](RO/H) Transmit interrupt status. */
        uint32_t rxris                 : 1;  /**< [  4:  4](RO/H) Receive interrupt status. */
        uint32_t dsrrmis               : 1;  /**< [  3:  3](RO/H) DSR modem interrupt status. */
        uint32_t dcdrmis               : 1;  /**< [  2:  2](RO/H) DCD modem interrupt status. */
        uint32_t ctsrmis               : 1;  /**< [  1:  1](RO/H) CTS modem interrupt status. */
        uint32_t rirmis                : 1;  /**< [  0:  0](RO/H) Ring indicator interrupt status. Not implemented. */
#else /* Word 0 - Little Endian */
        uint32_t rirmis                : 1;  /**< [  0:  0](RO/H) Ring indicator interrupt status. Not implemented. */
        uint32_t ctsrmis               : 1;  /**< [  1:  1](RO/H) CTS modem interrupt status. */
        uint32_t dcdrmis               : 1;  /**< [  2:  2](RO/H) DCD modem interrupt status. */
        uint32_t dsrrmis               : 1;  /**< [  3:  3](RO/H) DSR modem interrupt status. */
        uint32_t rxris                 : 1;  /**< [  4:  4](RO/H) Receive interrupt status. */
        uint32_t txris                 : 1;  /**< [  5:  5](RO/H) Transmit interrupt status. */
        uint32_t rtris                 : 1;  /**< [  6:  6](RO/H) Receive timeout interrupt status. */
        uint32_t feris                 : 1;  /**< [  7:  7](RO/H) Framing error interrupt status. */
        uint32_t peris                 : 1;  /**< [  8:  8](RO/H) Parity error interrupt status. */
        uint32_t beris                 : 1;  /**< [  9:  9](RO/H) Break error interrupt status. */
        uint32_t oeris                 : 1;  /**< [ 10: 10](RO/H) Overrun error interrupt status. */
        uint32_t reserved_11_31        : 21;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_ris_s cn; */
};
typedef union bdk_uaax_ris bdk_uaax_ris_t;

static inline uint64_t BDK_UAAX_RIS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_RIS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e02800003cll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e02800003cll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e02400003cll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e02800003cll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_RIS", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_RIS(a) bdk_uaax_ris_t
#define bustype_BDK_UAAX_RIS(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_RIS(a) "UAAX_RIS"
#define device_bar_BDK_UAAX_RIS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_RIS(a) (a)
#define arguments_BDK_UAAX_RIS(a) (a),-1,-1,-1

/**
 * Register (RSL32b) uaa#_rsr_ecr
 *
 * UART Receive Status Register/Error Clear Register
 */
union bdk_uaax_rsr_ecr
{
    uint32_t u;
    struct bdk_uaax_rsr_ecr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint32_t reserved_4_31         : 28;
        uint32_t oe                    : 1;  /**< [  3:  3](R/W1/H) Overrun error. Write of any value clears. */
        uint32_t be                    : 1;  /**< [  2:  2](R/W1/H) Break error. Associated with the character at the top of the FIFO; only one 0 character is
                                                                 loaded. The next character is only enabled after the receive data goes to 1. Write of any
                                                                 value clears. */
        uint32_t pe                    : 1;  /**< [  1:  1](R/W1/H) Parity error. Associated with character at top of the FIFO. Write of any value clears. */
        uint32_t fe                    : 1;  /**< [  0:  0](R/W1/H) Framing error. Associated with character at top of the FIFO. Write of any value clears. */
#else /* Word 0 - Little Endian */
        uint32_t fe                    : 1;  /**< [  0:  0](R/W1/H) Framing error. Associated with character at top of the FIFO. Write of any value clears. */
        uint32_t pe                    : 1;  /**< [  1:  1](R/W1/H) Parity error. Associated with character at top of the FIFO. Write of any value clears. */
        uint32_t be                    : 1;  /**< [  2:  2](R/W1/H) Break error. Associated with the character at the top of the FIFO; only one 0 character is
                                                                 loaded. The next character is only enabled after the receive data goes to 1. Write of any
                                                                 value clears. */
        uint32_t oe                    : 1;  /**< [  3:  3](R/W1/H) Overrun error. Write of any value clears. */
        uint32_t reserved_4_31         : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_rsr_ecr_s cn; */
};
typedef union bdk_uaax_rsr_ecr bdk_uaax_rsr_ecr_t;

static inline uint64_t BDK_UAAX_RSR_ECR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_RSR_ECR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028000004ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028000004ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024000004ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028000004ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_RSR_ECR", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_RSR_ECR(a) bdk_uaax_rsr_ecr_t
#define bustype_BDK_UAAX_RSR_ECR(a) BDK_CSR_TYPE_RSL32b
#define basename_BDK_UAAX_RSR_ECR(a) "UAAX_RSR_ECR"
#define device_bar_BDK_UAAX_RSR_ECR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_RSR_ECR(a) (a)
#define arguments_BDK_UAAX_RSR_ECR(a) (a),-1,-1,-1

/**
 * Register (RSL) uaa#_uctl_csclk_active_pc
 *
 * UAA UCTL Conditional Clock Counter Register
 * This register counts conditional clocks, for power analysis.
 * Reset by RSL reset.
 */
union bdk_uaax_uctl_csclk_active_pc
{
    uint64_t u;
    struct bdk_uaax_uctl_csclk_active_pc_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Counts conditional-clock active cycles since reset. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Counts conditional-clock active cycles since reset. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_uctl_csclk_active_pc_s cn; */
};
typedef union bdk_uaax_uctl_csclk_active_pc bdk_uaax_uctl_csclk_active_pc_t;

static inline uint64_t BDK_UAAX_UCTL_CSCLK_ACTIVE_PC(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_UCTL_CSCLK_ACTIVE_PC(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028001018ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_UCTL_CSCLK_ACTIVE_PC", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_UCTL_CSCLK_ACTIVE_PC(a) bdk_uaax_uctl_csclk_active_pc_t
#define bustype_BDK_UAAX_UCTL_CSCLK_ACTIVE_PC(a) BDK_CSR_TYPE_RSL
#define basename_BDK_UAAX_UCTL_CSCLK_ACTIVE_PC(a) "UAAX_UCTL_CSCLK_ACTIVE_PC"
#define device_bar_BDK_UAAX_UCTL_CSCLK_ACTIVE_PC(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_UCTL_CSCLK_ACTIVE_PC(a) (a)
#define arguments_BDK_UAAX_UCTL_CSCLK_ACTIVE_PC(a) (a),-1,-1,-1

/**
 * Register (RSL) uaa#_uctl_ctl
 *
 * UART UCTL Control Register
 */
union bdk_uaax_uctl_ctl
{
    uint64_t u;
    struct bdk_uaax_uctl_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_31_63        : 33;
        uint64_t h_clk_en              : 1;  /**< [ 30: 30](R/W) UART controller clock enable. When set to 1, the UART controller clock is generated. This
                                                                 also enables access to UCTL registers 0x30-0xF8. */
        uint64_t h_clk_byp_sel         : 1;  /**< [ 29: 29](R/W) Select the bypass input to the UART controller clock divider.
                                                                 0 = Use the divided coprocessor clock from the [H_CLKDIV_SEL] divider.
                                                                 1 = Use the bypass clock from the GPIO pins.

                                                                 This signal is just a multiplexer-select signal; it does not enable the UART
                                                                 controller and APB clock. Software must still set [H_CLK_EN]
                                                                 separately. [H_CLK_BYP_SEL] select should not be changed unless [H_CLK_EN] is
                                                                 disabled.  The bypass clock can be selected and running even if the UART
                                                                 controller clock dividers are not running.

                                                                 Internal:
                                                                 Generally bypass is only used for scan purposes. */
        uint64_t h_clkdiv_rst          : 1;  /**< [ 28: 28](R/W) UART controller clock divider reset. Divided clocks are not generated while the divider is
                                                                 being reset.
                                                                 This also resets the suspend-clock divider. */
        uint64_t reserved_27           : 1;
        uint64_t h_clkdiv_sel          : 3;  /**< [ 26: 24](R/W) The UARTCLK and APB CLK frequency select.
                                                                 The divider values are the following:
                                                                 0x0 = Divide by 1.
                                                                 0x1 = Divide by 2.
                                                                 0x2 = Divide by 4.
                                                                 0x3 = Divide by 6.
                                                                 0x4 = Divide by 8.
                                                                 0x5 = Divide by 16.
                                                                 0x6 = Divide by 24.
                                                                 0x7 = Divide by 32.

                                                                 The max and min frequency of the UARTCLK is determined by the following:
                                                                 _ f_uartclk(min) \>= 16 * baud_rate(max)
                                                                 _ f_uartclk(max) \<= 16 * 65535 * baud_rate(min) */
        uint64_t reserved_5_23         : 19;
        uint64_t csclk_en              : 1;  /**< [  4:  4](R/W) Turns on the UCTL interface clock (coprocessor clock).
                                                                 This enables the UCTL registers starting from 0x30 via the RSL bus. */
        uint64_t reserved_2_3          : 2;
        uint64_t uaa_rst               : 1;  /**< [  1:  1](R/W) Software reset; resets UAA controller; active-high.
                                                                 Internal:
                                                                 Note that soft-resetting the UAHC while it is active may cause violations of RSL
                                                                 protocols. */
        uint64_t uctl_rst              : 1;  /**< [  0:  0](R/W) Software reset; resets UCTL; active-high.
                                                                 Resets UCTL RSL registers 0x30-0xF8.
                                                                 Does not reset UCTL RSL registers 0x0-0x28.
                                                                 UCTL RSL registers starting from 0x30 can be accessed only after the UART controller clock
                                                                 is active and [UCTL_RST] is deasserted.

                                                                 Internal:
                                                                 Note that soft-resetting the UCTL while it is active may cause violations of
                                                                 RSL and CIB protocols. */
#else /* Word 0 - Little Endian */
        uint64_t uctl_rst              : 1;  /**< [  0:  0](R/W) Software reset; resets UCTL; active-high.
                                                                 Resets UCTL RSL registers 0x30-0xF8.
                                                                 Does not reset UCTL RSL registers 0x0-0x28.
                                                                 UCTL RSL registers starting from 0x30 can be accessed only after the UART controller clock
                                                                 is active and [UCTL_RST] is deasserted.

                                                                 Internal:
                                                                 Note that soft-resetting the UCTL while it is active may cause violations of
                                                                 RSL and CIB protocols. */
        uint64_t uaa_rst               : 1;  /**< [  1:  1](R/W) Software reset; resets UAA controller; active-high.
                                                                 Internal:
                                                                 Note that soft-resetting the UAHC while it is active may cause violations of RSL
                                                                 protocols. */
        uint64_t reserved_2_3          : 2;
        uint64_t csclk_en              : 1;  /**< [  4:  4](R/W) Turns on the UCTL interface clock (coprocessor clock).
                                                                 This enables the UCTL registers starting from 0x30 via the RSL bus. */
        uint64_t reserved_5_23         : 19;
        uint64_t h_clkdiv_sel          : 3;  /**< [ 26: 24](R/W) The UARTCLK and APB CLK frequency select.
                                                                 The divider values are the following:
                                                                 0x0 = Divide by 1.
                                                                 0x1 = Divide by 2.
                                                                 0x2 = Divide by 4.
                                                                 0x3 = Divide by 6.
                                                                 0x4 = Divide by 8.
                                                                 0x5 = Divide by 16.
                                                                 0x6 = Divide by 24.
                                                                 0x7 = Divide by 32.

                                                                 The max and min frequency of the UARTCLK is determined by the following:
                                                                 _ f_uartclk(min) \>= 16 * baud_rate(max)
                                                                 _ f_uartclk(max) \<= 16 * 65535 * baud_rate(min) */
        uint64_t reserved_27           : 1;
        uint64_t h_clkdiv_rst          : 1;  /**< [ 28: 28](R/W) UART controller clock divider reset. Divided clocks are not generated while the divider is
                                                                 being reset.
                                                                 This also resets the suspend-clock divider. */
        uint64_t h_clk_byp_sel         : 1;  /**< [ 29: 29](R/W) Select the bypass input to the UART controller clock divider.
                                                                 0 = Use the divided coprocessor clock from the [H_CLKDIV_SEL] divider.
                                                                 1 = Use the bypass clock from the GPIO pins.

                                                                 This signal is just a multiplexer-select signal; it does not enable the UART
                                                                 controller and APB clock. Software must still set [H_CLK_EN]
                                                                 separately. [H_CLK_BYP_SEL] select should not be changed unless [H_CLK_EN] is
                                                                 disabled.  The bypass clock can be selected and running even if the UART
                                                                 controller clock dividers are not running.

                                                                 Internal:
                                                                 Generally bypass is only used for scan purposes. */
        uint64_t h_clk_en              : 1;  /**< [ 30: 30](R/W) UART controller clock enable. When set to 1, the UART controller clock is generated. This
                                                                 also enables access to UCTL registers 0x30-0xF8. */
        uint64_t reserved_31_63        : 33;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_uctl_ctl_s cn8; */
    struct bdk_uaax_uctl_ctl_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_31_63        : 33;
        uint64_t h_clk_en              : 1;  /**< [ 30: 30](R/W) UART controller clock enable. When set to 1, the UART controller clock is generated. This
                                                                 also enables access to UCTL registers 0x30-0xF8. */
        uint64_t h_clk_byp_sel         : 1;  /**< [ 29: 29](R/W) Select the bypass input to the UART controller clock divider.
                                                                 0 = Use the divided coprocessor clock from the [H_CLKDIV_SEL] divider.
                                                                 1 = Use the bypass clock from the GPIO pins.

                                                                 This signal is just a multiplexer-select signal; it does not enable the UART
                                                                 controller and APB clock. Software must still set [H_CLK_EN]
                                                                 separately. [H_CLK_BYP_SEL] select should not be changed unless [H_CLK_EN] is
                                                                 disabled.  The bypass clock can be selected and running even if the UART
                                                                 controller clock dividers are not running.

                                                                 Internal:
                                                                 Generally bypass is only used for scan purposes. */
        uint64_t h_clkdiv_rst          : 1;  /**< [ 28: 28](R/W) UART controller clock divider reset. Divided clocks are not generated while the divider is
                                                                 being reset.
                                                                 This also resets the suspend-clock divider. */
        uint64_t reserved_27           : 1;
        uint64_t h_clkdiv_sel          : 3;  /**< [ 26: 24](R/W) The UARTCLK and APB CLK frequency select.
                                                                 The divider values are the following:
                                                                 0x0 = Divide by 1 (100 MHz).
                                                                 0x1 = Divide by 2 (50 MHz).
                                                                 0x2 = Divide by 4 (25 MHz).
                                                                 0x3 = Divide by 6 (16.66 MHz).
                                                                 0x4 = Divide by 8 (12.50 MHz).
                                                                 0x5 = Divide by 16 (6.25 MHz).
                                                                 0x6 = Divide by 24 (4.167 MHz).
                                                                 0x7 = Divide by 32 (3.125 MHz).

                                                                 The max and min frequency of the UARTCLK is determined by the following:
                                                                 _ f_uartclk(min) \>= 16 * baud_rate(max)
                                                                 _ f_uartclk(max) \<= 16 * 65535 * baud_rate(min) */
        uint64_t reserved_5_23         : 19;
        uint64_t csclk_en              : 1;  /**< [  4:  4](R/W) Turns on the UCTL interface clock (coprocessor clock).
                                                                 This enables the UCTL registers starting from 0x30 via the RSL bus. */
        uint64_t reserved_2_3          : 2;
        uint64_t uaa_rst               : 1;  /**< [  1:  1](R/W) Software reset; resets UAA controller; active-high.
                                                                 Internal:
                                                                 Note that soft-resetting the UAHC while it is active may cause violations of RSL
                                                                 protocols. */
        uint64_t uctl_rst              : 1;  /**< [  0:  0](R/W) Software reset; resets UCTL; active-high.
                                                                 Resets UCTL RSL registers 0x30-0xF8.
                                                                 Does not reset UCTL RSL registers 0x0-0x28.
                                                                 UCTL RSL registers starting from 0x30 can be accessed only after the UART controller clock
                                                                 is active and [UCTL_RST] is deasserted.

                                                                 Internal:
                                                                 Note that soft-resetting the UCTL while it is active may cause violations of
                                                                 RSL and CIB protocols. */
#else /* Word 0 - Little Endian */
        uint64_t uctl_rst              : 1;  /**< [  0:  0](R/W) Software reset; resets UCTL; active-high.
                                                                 Resets UCTL RSL registers 0x30-0xF8.
                                                                 Does not reset UCTL RSL registers 0x0-0x28.
                                                                 UCTL RSL registers starting from 0x30 can be accessed only after the UART controller clock
                                                                 is active and [UCTL_RST] is deasserted.

                                                                 Internal:
                                                                 Note that soft-resetting the UCTL while it is active may cause violations of
                                                                 RSL and CIB protocols. */
        uint64_t uaa_rst               : 1;  /**< [  1:  1](R/W) Software reset; resets UAA controller; active-high.
                                                                 Internal:
                                                                 Note that soft-resetting the UAHC while it is active may cause violations of RSL
                                                                 protocols. */
        uint64_t reserved_2_3          : 2;
        uint64_t csclk_en              : 1;  /**< [  4:  4](R/W) Turns on the UCTL interface clock (coprocessor clock).
                                                                 This enables the UCTL registers starting from 0x30 via the RSL bus. */
        uint64_t reserved_5_23         : 19;
        uint64_t h_clkdiv_sel          : 3;  /**< [ 26: 24](R/W) The UARTCLK and APB CLK frequency select.
                                                                 The divider values are the following:
                                                                 0x0 = Divide by 1 (100 MHz).
                                                                 0x1 = Divide by 2 (50 MHz).
                                                                 0x2 = Divide by 4 (25 MHz).
                                                                 0x3 = Divide by 6 (16.66 MHz).
                                                                 0x4 = Divide by 8 (12.50 MHz).
                                                                 0x5 = Divide by 16 (6.25 MHz).
                                                                 0x6 = Divide by 24 (4.167 MHz).
                                                                 0x7 = Divide by 32 (3.125 MHz).

                                                                 The max and min frequency of the UARTCLK is determined by the following:
                                                                 _ f_uartclk(min) \>= 16 * baud_rate(max)
                                                                 _ f_uartclk(max) \<= 16 * 65535 * baud_rate(min) */
        uint64_t reserved_27           : 1;
        uint64_t h_clkdiv_rst          : 1;  /**< [ 28: 28](R/W) UART controller clock divider reset. Divided clocks are not generated while the divider is
                                                                 being reset.
                                                                 This also resets the suspend-clock divider. */
        uint64_t h_clk_byp_sel         : 1;  /**< [ 29: 29](R/W) Select the bypass input to the UART controller clock divider.
                                                                 0 = Use the divided coprocessor clock from the [H_CLKDIV_SEL] divider.
                                                                 1 = Use the bypass clock from the GPIO pins.

                                                                 This signal is just a multiplexer-select signal; it does not enable the UART
                                                                 controller and APB clock. Software must still set [H_CLK_EN]
                                                                 separately. [H_CLK_BYP_SEL] select should not be changed unless [H_CLK_EN] is
                                                                 disabled.  The bypass clock can be selected and running even if the UART
                                                                 controller clock dividers are not running.

                                                                 Internal:
                                                                 Generally bypass is only used for scan purposes. */
        uint64_t h_clk_en              : 1;  /**< [ 30: 30](R/W) UART controller clock enable. When set to 1, the UART controller clock is generated. This
                                                                 also enables access to UCTL registers 0x30-0xF8. */
        uint64_t reserved_31_63        : 33;
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_uaax_uctl_ctl bdk_uaax_uctl_ctl_t;

static inline uint64_t BDK_UAAX_UCTL_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_UCTL_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028001000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028001000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024001000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028001000ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_UCTL_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_UCTL_CTL(a) bdk_uaax_uctl_ctl_t
#define bustype_BDK_UAAX_UCTL_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_UAAX_UCTL_CTL(a) "UAAX_UCTL_CTL"
#define device_bar_BDK_UAAX_UCTL_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_UCTL_CTL(a) (a)
#define arguments_BDK_UAAX_UCTL_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) uaa#_uctl_spare0
 *
 * UART UCTL Spare Register 0
 * This register is a spare register. This register can be reset by NCB reset.
 */
union bdk_uaax_uctl_spare0
{
    uint64_t u;
    struct bdk_uaax_uctl_spare0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t spare                 : 64; /**< [ 63:  0](R/W) Spare. */
#else /* Word 0 - Little Endian */
        uint64_t spare                 : 64; /**< [ 63:  0](R/W) Spare. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_uctl_spare0_s cn; */
};
typedef union bdk_uaax_uctl_spare0 bdk_uaax_uctl_spare0_t;

static inline uint64_t BDK_UAAX_UCTL_SPARE0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_UCTL_SPARE0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e028001010ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e028001010ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e024001010ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e028001010ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_UCTL_SPARE0", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_UCTL_SPARE0(a) bdk_uaax_uctl_spare0_t
#define bustype_BDK_UAAX_UCTL_SPARE0(a) BDK_CSR_TYPE_RSL
#define basename_BDK_UAAX_UCTL_SPARE0(a) "UAAX_UCTL_SPARE0"
#define device_bar_BDK_UAAX_UCTL_SPARE0(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_UCTL_SPARE0(a) (a)
#define arguments_BDK_UAAX_UCTL_SPARE0(a) (a),-1,-1,-1

/**
 * Register (RSL) uaa#_uctl_spare1
 *
 * UART UCTL Spare Register 1
 * This register is a spare register. This register can be reset by NCB reset.
 */
union bdk_uaax_uctl_spare1
{
    uint64_t u;
    struct bdk_uaax_uctl_spare1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t spare                 : 64; /**< [ 63:  0](R/W) Spare. */
#else /* Word 0 - Little Endian */
        uint64_t spare                 : 64; /**< [ 63:  0](R/W) Spare. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_uaax_uctl_spare1_s cn; */
};
typedef union bdk_uaax_uctl_spare1 bdk_uaax_uctl_spare1_t;

static inline uint64_t BDK_UAAX_UCTL_SPARE1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_UAAX_UCTL_SPARE1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0280010f8ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e0280010f8ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=1))
        return 0x87e0240010f8ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=7))
        return 0x87e0280010f8ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("UAAX_UCTL_SPARE1", 1, a, 0, 0, 0);
}

#define typedef_BDK_UAAX_UCTL_SPARE1(a) bdk_uaax_uctl_spare1_t
#define bustype_BDK_UAAX_UCTL_SPARE1(a) BDK_CSR_TYPE_RSL
#define basename_BDK_UAAX_UCTL_SPARE1(a) "UAAX_UCTL_SPARE1"
#define device_bar_BDK_UAAX_UCTL_SPARE1(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_UAAX_UCTL_SPARE1(a) (a)
#define arguments_BDK_UAAX_UCTL_SPARE1(a) (a),-1,-1,-1

#endif /* __BDK_CSRS_UAA_H__ */
