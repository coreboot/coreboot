#ifndef __BDK_CSRS_L2C_TAD_H__
#define __BDK_CSRS_L2C_TAD_H__
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
 * Cavium L2C_TAD.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration l2c_tad_bar_e
 *
 * L2C TAD Base Address Register Enumeration
 * Enumerates the base address registers.
 */
#define BDK_L2C_TAD_BAR_E_L2C_TADX_PF_BAR0(a) (0x87e050000000ll + 0x1000000ll * (a))
#define BDK_L2C_TAD_BAR_E_L2C_TADX_PF_BAR0_SIZE 0x800000ull
#define BDK_L2C_TAD_BAR_E_L2C_TADX_PF_BAR4(a) (0x87e050f00000ll + 0x1000000ll * (a))
#define BDK_L2C_TAD_BAR_E_L2C_TADX_PF_BAR4_SIZE 0x100000ull

/**
 * Enumeration l2c_tad_int_vec_e
 *
 * L2C TAD MSI-X Vector Enumeration
 * Enumerates the MSI-X interrupt vectors.
 */
#define BDK_L2C_TAD_INT_VEC_E_INTS (0)

/**
 * Register (RSL) l2c_tad#_int_ena_w1c
 *
 * L2C TAD Interrupt Enable Clear Registers
 * This register clears interrupt enable bits.
 */
union bdk_l2c_tadx_int_ena_w1c
{
    uint64_t u;
    struct bdk_l2c_tadx_int_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t reserved_19_31        : 13;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[GSYNCTO]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[GSYNCTO]. */
        uint64_t reserved_19_31        : 13;
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    struct bdk_l2c_tadx_int_ena_w1c_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t reserved_19_31        : 13;
        uint64_t reserved_18           : 1;
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t reserved_18           : 1;
        uint64_t reserved_19_31        : 13;
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_l2c_tadx_int_ena_w1c_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[WRDISOCI]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[RDDISOCI]. */
        uint64_t reserved_19_33        : 15;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[GSYNCTO]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[LFBTO]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[WRDISLMC]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[RDDISLMC]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[RDNXM]. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[WRNXM]. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[NOWAY]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[TAGDBE]. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[TAGSBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[FBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[FBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[SBFDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[SBFSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[L2DDBE]. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[L2DSBE]. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[L2DSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[L2DDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[SBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[SBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[FBFSBE]. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[FBFDBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[TAGSBE]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[TAGDBE]. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[NOWAY]. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[WRNXM]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[RDNXM]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[RDDISLMC]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[WRDISLMC]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[LFBTO]. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[GSYNCTO]. */
        uint64_t reserved_19_33        : 15;
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[RDDISOCI]. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Reads or clears enable for L2C_TAD(0)_INT_W1C[WRDISOCI]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_l2c_tadx_int_ena_w1c_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[WRDISOCI]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[RDDISOCI]. */
        uint64_t reserved_19_33        : 15;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[GSYNCTO]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[LFBTO]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[WRDISLMC]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[RDDISLMC]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[RDNXM]. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[WRNXM]. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[NOWAY]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[TAGDBE]. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[TAGSBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[FBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[FBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[SBFDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[SBFSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[L2DDBE]. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[L2DSBE]. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[L2DSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[L2DDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[SBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[SBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[FBFSBE]. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[FBFDBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[TAGSBE]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[TAGDBE]. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[NOWAY]. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[WRNXM]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[RDNXM]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[RDDISLMC]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[WRDISLMC]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[LFBTO]. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[GSYNCTO]. */
        uint64_t reserved_19_33        : 15;
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[RDDISOCI]. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Reads or clears enable for L2C_TAD(0..3)_INT_W1C[WRDISOCI]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn83xx;
    struct bdk_l2c_tadx_int_ena_w1c_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t reserved_19_31        : 13;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[GSYNCTO]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[GSYNCTO]. */
        uint64_t reserved_19_31        : 13;
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Reads or clears enable for L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_l2c_tadx_int_ena_w1c bdk_l2c_tadx_int_ena_w1c_t;

static inline uint64_t BDK_L2C_TADX_INT_ENA_W1C(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_INT_ENA_W1C(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050040020ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050040020ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050040020ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_INT_ENA_W1C", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_INT_ENA_W1C(a) bdk_l2c_tadx_int_ena_w1c_t
#define bustype_BDK_L2C_TADX_INT_ENA_W1C(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_INT_ENA_W1C(a) "L2C_TADX_INT_ENA_W1C"
#define device_bar_BDK_L2C_TADX_INT_ENA_W1C(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_INT_ENA_W1C(a) (a)
#define arguments_BDK_L2C_TADX_INT_ENA_W1C(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad#_int_ena_w1s
 *
 * L2C TAD Interrupt Enable Set Registers
 * This register sets interrupt enable bits.
 */
union bdk_l2c_tadx_int_ena_w1s
{
    uint64_t u;
    struct bdk_l2c_tadx_int_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t reserved_19_31        : 13;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[GSYNCTO]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[GSYNCTO]. */
        uint64_t reserved_19_31        : 13;
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    struct bdk_l2c_tadx_int_ena_w1s_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t reserved_19_31        : 13;
        uint64_t reserved_18           : 1;
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t reserved_18           : 1;
        uint64_t reserved_19_31        : 13;
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_l2c_tadx_int_ena_w1s_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[WRDISOCI]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[RDDISOCI]. */
        uint64_t reserved_19_33        : 15;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[GSYNCTO]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[LFBTO]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[WRDISLMC]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[RDDISLMC]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[RDNXM]. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[WRNXM]. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[NOWAY]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[TAGDBE]. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[TAGSBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[FBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[FBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[SBFDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[SBFSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[L2DDBE]. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[L2DSBE]. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[L2DSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[L2DDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[SBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[SBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[FBFSBE]. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[FBFDBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[TAGSBE]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[TAGDBE]. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[NOWAY]. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[WRNXM]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[RDNXM]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[RDDISLMC]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[WRDISLMC]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[LFBTO]. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[GSYNCTO]. */
        uint64_t reserved_19_33        : 15;
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[RDDISOCI]. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets enable for L2C_TAD(0)_INT_W1C[WRDISOCI]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_l2c_tadx_int_ena_w1s_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[WRDISOCI]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[RDDISOCI]. */
        uint64_t reserved_19_33        : 15;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[GSYNCTO]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[LFBTO]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[WRDISLMC]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[RDDISLMC]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[RDNXM]. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[WRNXM]. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[NOWAY]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[TAGDBE]. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[TAGSBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[FBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[FBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[SBFDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[SBFSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[L2DDBE]. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[L2DSBE]. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[L2DSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[L2DDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[SBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[SBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[FBFSBE]. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[FBFDBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[TAGSBE]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[TAGDBE]. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[NOWAY]. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[WRNXM]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[RDNXM]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[RDDISLMC]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[WRDISLMC]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[LFBTO]. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[GSYNCTO]. */
        uint64_t reserved_19_33        : 15;
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[RDDISOCI]. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets enable for L2C_TAD(0..3)_INT_W1C[WRDISOCI]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn83xx;
    struct bdk_l2c_tadx_int_ena_w1s_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t reserved_19_31        : 13;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[GSYNCTO]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[GSYNCTO]. */
        uint64_t reserved_19_31        : 13;
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets enable for L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_l2c_tadx_int_ena_w1s bdk_l2c_tadx_int_ena_w1s_t;

static inline uint64_t BDK_L2C_TADX_INT_ENA_W1S(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_INT_ENA_W1S(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050040028ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050040028ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050040028ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_INT_ENA_W1S", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_INT_ENA_W1S(a) bdk_l2c_tadx_int_ena_w1s_t
#define bustype_BDK_L2C_TADX_INT_ENA_W1S(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_INT_ENA_W1S(a) "L2C_TADX_INT_ENA_W1S"
#define device_bar_BDK_L2C_TADX_INT_ENA_W1S(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_INT_ENA_W1S(a) (a)
#define arguments_BDK_L2C_TADX_INT_ENA_W1S(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad#_int_w1c
 *
 * L2C TAD Interrupt Registers
 * This register is for TAD-based interrupts.
 */
union bdk_l2c_tadx_int_w1c
{
    uint64_t u;
    struct bdk_l2c_tadx_int_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Illegal write operation to a remote node with L2C_OCI_CTL[ENAOCI][node] clear. See
                                                                 L2C_TAD()_ERR for logged information.
                                                                 During normal hardware operation, an indication of a software failure and may be
                                                                 considered fatal. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Illegal read operation to a remote node with L2C_OCI_CTL[ENAOCI][node]
                                                                 clear. Note [RDDISOCI] interrupts can occur during normal operation as the cores
                                                                 are allowed to prefetch to nonexistent memory locations. Therefore, [RDDISOCI]
                                                                 is for informational purposes only. See L2C_TAD()_ERR for logged information. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1C/H) RTG double-bit error.
                                                                 See L2C_TAD()_RTG_ERR for logged information.
                                                                 An indication of a hardware failure and may be considered fatal. */
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1C/H) RTG single-bit error on a read. See L2C_TAD()_RTG_ERR for logged
                                                                 information. When [RTGSBE] is set, hardware corrected the error before using the
                                                                 RTG tag, but did not correct any stored value. When [RTGSBE] is set, software
                                                                 should eject the RTG location indicated by the corresponding
                                                                 L2C_TAD()_RTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [RTGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same RTG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24\> = 1
                                                                   payload\<23:20\> = L2C_TAD()_RTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_RTG_ERR[L2IDX]
                                                                 \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on the payload. */
        uint64_t reserved_19_31        : 13;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Global sync timeout. Should not occur during normal operation. This may be an
                                                                 indication of hardware failure, and may be considered fatal. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) An LFB entry (or more) has encountered a timeout condition When [LFBTO] timeout
                                                                 condition occurs L2C_TAD()_TIMEOUT is loaded. L2C_TAD()_TIMEOUT is loaded with
                                                                 info from the first LFB that timed out. if multiple LFB timed out
                                                                 simultaneously, then the it will capture info from the lowest LFB number that
                                                                 timed out.
                                                                 Should not occur during normal operation.  OCI/CCPI link failures may cause this
                                                                 failure. This may be an indication of hardware failure, and may be considered
                                                                 fatal. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Illegal write to disabled LMC error. A DRAM write arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Illegal read to disabled LMC error. A DRAM read arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Read reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure read reference to an ASC region
                                                                 not enabled for secure access, or nonsecure read reference to an ASC
                                                                 region not enabled for nonsecure access.
                                                                 [RDNXM] interrupts can occur during normal operation as the cores are
                                                                 allowed to prefetch to nonexistent memory locations.  Therefore,
                                                                 [RDNXM] is for informational purposes only.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Write reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure write reference to an ASC region
                                                                 not enabled for secure access, or nonsecure write reference to an
                                                                 ASC region not enabled for nonsecure access.
                                                                 This may be an indication of software
                                                                 failure, and may be considered fatal.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) No way was available for allocation. L2C sets [NOWAY] during its processing of a
                                                                 transaction whenever it needed/wanted to allocate a WAY in the L2 cache, but was
                                                                 unable to. When this bit = 1, it is (generally) not an indication that L2C
                                                                 failed to complete transactions. Rather, it is a hint of possible performance
                                                                 degradation. (For example, L2C must read- modify-write DRAM for every
                                                                 transaction that updates some, but not all, of the bytes in a cache block,
                                                                 misses in the L2 cache, and cannot allocate a WAY.) There is one 'failure' case
                                                                 where L2C sets [NOWAY]: when it cannot leave a block locked in the L2 cache as
                                                                 part of a LCKL2 transaction. See L2C_TTG()_ERR for logged information. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) TAG double-bit error occurred. See L2C_TTG()_ERR for logged information.
                                                                 This is an indication of a hardware failure and may be considered fatal. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) TAG single-bit error on a read. See L2C_TAD()_TTG_ERR for logged
                                                                 information. When [TAGSBE] is set, hardware corrected the error before using the
                                                                 tag, but did not correct any stored value. When [TAGSBE] is set, software should
                                                                 eject the TAG location indicated by the corresponding
                                                                 L2C_TAD()_TTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [TAGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same TAG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                   \<pre\>
                                                                   payload\<24\> = 0
                                                                   payload\<23:20\> = L2C_TAD()_TTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_TTG_ERR[L2IDX]
                                                                   \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on this payload. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) FBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) FBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) SBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) SBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) L2D double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) L2D single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. When [L2DSBE] is set, hardware corrected the error before using the
                                                                 data, but did not correct any stored value. When [L2DSBE] is set, software
                                                                 should eject the cache block indicated by the corresponding
                                                                 L2C_TAD()_TQD_ERR[QDNUM,L2DIDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [L2DSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same L2D location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24\>    = 0
                                                                   payload\<23:20\> = L2C_TAD()_TQD_ERR[L2DIDX]\<10:7\>  // way
                                                                   payload\<19:13\> = L2C_TAD()_TQD_ERR[L2DIDX]\<6:0\>   // index\<12:6\>
                                                                   payload\<12:11\> = L2C_TAD()_TQD_ERR[L2DIDX]\<12:11\> // index\<5:4\>
                                                                   payload\<10\>    = L2C_TAD()_TQD_ERR[QDNUM]\<2\>      // index\<3\>
                                                                   payload\<9:7\>   = tad             // index\<2:0\>
                                                                 \</pre\>

                                                                 where tad is the TAD index from this CSR. Note that L2C_CTL[DISIDXALIAS] has no
                                                                 effect on the payload. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) L2D single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. When [L2DSBE] is set, hardware corrected the error before using the
                                                                 data, but did not correct any stored value. When [L2DSBE] is set, software
                                                                 should eject the cache block indicated by the corresponding
                                                                 L2C_TAD()_TQD_ERR[QDNUM,L2DIDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [L2DSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same L2D location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24\>    = 0
                                                                   payload\<23:20\> = L2C_TAD()_TQD_ERR[L2DIDX]\<10:7\>  // way
                                                                   payload\<19:13\> = L2C_TAD()_TQD_ERR[L2DIDX]\<6:0\>   // index\<12:6\>
                                                                   payload\<12:11\> = L2C_TAD()_TQD_ERR[L2DIDX]\<12:11\> // index\<5:4\>
                                                                   payload\<10\>    = L2C_TAD()_TQD_ERR[QDNUM]\<2\>      // index\<3\>
                                                                   payload\<9:7\>   = tad             // index\<2:0\>
                                                                 \</pre\>

                                                                 where tad is the TAD index from this CSR. Note that L2C_CTL[DISIDXALIAS] has no
                                                                 effect on the payload. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) L2D double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) SBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) SBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) FBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) FBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) TAG single-bit error on a read. See L2C_TAD()_TTG_ERR for logged
                                                                 information. When [TAGSBE] is set, hardware corrected the error before using the
                                                                 tag, but did not correct any stored value. When [TAGSBE] is set, software should
                                                                 eject the TAG location indicated by the corresponding
                                                                 L2C_TAD()_TTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [TAGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same TAG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                   \<pre\>
                                                                   payload\<24\> = 0
                                                                   payload\<23:20\> = L2C_TAD()_TTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_TTG_ERR[L2IDX]
                                                                   \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on this payload. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) TAG double-bit error occurred. See L2C_TTG()_ERR for logged information.
                                                                 This is an indication of a hardware failure and may be considered fatal. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) No way was available for allocation. L2C sets [NOWAY] during its processing of a
                                                                 transaction whenever it needed/wanted to allocate a WAY in the L2 cache, but was
                                                                 unable to. When this bit = 1, it is (generally) not an indication that L2C
                                                                 failed to complete transactions. Rather, it is a hint of possible performance
                                                                 degradation. (For example, L2C must read- modify-write DRAM for every
                                                                 transaction that updates some, but not all, of the bytes in a cache block,
                                                                 misses in the L2 cache, and cannot allocate a WAY.) There is one 'failure' case
                                                                 where L2C sets [NOWAY]: when it cannot leave a block locked in the L2 cache as
                                                                 part of a LCKL2 transaction. See L2C_TTG()_ERR for logged information. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Write reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure write reference to an ASC region
                                                                 not enabled for secure access, or nonsecure write reference to an
                                                                 ASC region not enabled for nonsecure access.
                                                                 This may be an indication of software
                                                                 failure, and may be considered fatal.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Read reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure read reference to an ASC region
                                                                 not enabled for secure access, or nonsecure read reference to an ASC
                                                                 region not enabled for nonsecure access.
                                                                 [RDNXM] interrupts can occur during normal operation as the cores are
                                                                 allowed to prefetch to nonexistent memory locations.  Therefore,
                                                                 [RDNXM] is for informational purposes only.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Illegal read to disabled LMC error. A DRAM read arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Illegal write to disabled LMC error. A DRAM write arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) An LFB entry (or more) has encountered a timeout condition When [LFBTO] timeout
                                                                 condition occurs L2C_TAD()_TIMEOUT is loaded. L2C_TAD()_TIMEOUT is loaded with
                                                                 info from the first LFB that timed out. if multiple LFB timed out
                                                                 simultaneously, then the it will capture info from the lowest LFB number that
                                                                 timed out.
                                                                 Should not occur during normal operation.  OCI/CCPI link failures may cause this
                                                                 failure. This may be an indication of hardware failure, and may be considered
                                                                 fatal. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Global sync timeout. Should not occur during normal operation. This may be an
                                                                 indication of hardware failure, and may be considered fatal. */
        uint64_t reserved_19_31        : 13;
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1C/H) RTG single-bit error on a read. See L2C_TAD()_RTG_ERR for logged
                                                                 information. When [RTGSBE] is set, hardware corrected the error before using the
                                                                 RTG tag, but did not correct any stored value. When [RTGSBE] is set, software
                                                                 should eject the RTG location indicated by the corresponding
                                                                 L2C_TAD()_RTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [RTGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same RTG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24\> = 1
                                                                   payload\<23:20\> = L2C_TAD()_RTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_RTG_ERR[L2IDX]
                                                                 \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on the payload. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1C/H) RTG double-bit error.
                                                                 See L2C_TAD()_RTG_ERR for logged information.
                                                                 An indication of a hardware failure and may be considered fatal. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Illegal read operation to a remote node with L2C_OCI_CTL[ENAOCI][node]
                                                                 clear. Note [RDDISOCI] interrupts can occur during normal operation as the cores
                                                                 are allowed to prefetch to nonexistent memory locations. Therefore, [RDDISOCI]
                                                                 is for informational purposes only. See L2C_TAD()_ERR for logged information. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Illegal write operation to a remote node with L2C_OCI_CTL[ENAOCI][node] clear. See
                                                                 L2C_TAD()_ERR for logged information.
                                                                 During normal hardware operation, an indication of a software failure and may be
                                                                 considered fatal. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    struct bdk_l2c_tadx_int_w1c_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Illegal write operation to a remote node with L2C_OCI_CTL[ENAOCI][node] clear. See
                                                                 L2C_TAD()_ERR for logged information.
                                                                 During normal hardware operation, an indication of a software failure and may be
                                                                 considered fatal. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Illegal read operation to a remote node with L2C_OCI_CTL[ENAOCI][node]
                                                                 clear. Note [RDDISOCI] interrupts can occur during normal operation as the cores
                                                                 are allowed to prefetch to nonexistent memory locations. Therefore, [RDDISOCI]
                                                                 is for informational purposes only. See L2C_TAD()_ERR for logged information. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1C/H) RTG double-bit error.
                                                                 See L2C_TAD()_RTG_ERR for logged information.
                                                                 An indication of a hardware failure and may be considered fatal. */
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1C/H) RTG single-bit error on a read. See L2C_TAD()_RTG_ERR for logged
                                                                 information. When [RTGSBE] is set, hardware corrected the error before using the
                                                                 RTG tag, but did not correct any stored value. When [RTGSBE] is set, software
                                                                 should eject the RTG location indicated by the corresponding
                                                                 L2C_TAD()_RTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [RTGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same RTG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24\> = 1
                                                                   payload\<23:20\> = L2C_TAD()_RTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_RTG_ERR[L2IDX]
                                                                 \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on the payload. */
        uint64_t reserved_19_31        : 13;
        uint64_t reserved_18           : 1;
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) An LFB entry (or more) has encountered a timeout condition When [LFBTO] timeout
                                                                 condition occurs L2C_TAD()_TIMEOUT is loaded. L2C_TAD()_TIMEOUT is loaded with
                                                                 info from the first LFB that timed out. if multiple LFB timed out
                                                                 simultaneously, then the it will capture info from the lowest LFB number that
                                                                 timed out.
                                                                 Should not occur during normal operation.  OCI/CCPI link failures may cause this
                                                                 failure. This may be an indication of hardware failure, and may be considered
                                                                 fatal. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Illegal write to disabled LMC error. A DRAM write arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Illegal read to disabled LMC error. A DRAM read arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Read reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure read reference to an ASC region
                                                                 not enabled for secure access, or nonsecure read reference to an ASC
                                                                 region not enabled for nonsecure access.
                                                                 [RDNXM] interrupts can occur during normal operation as the cores are
                                                                 allowed to prefetch to nonexistent memory locations.  Therefore,
                                                                 [RDNXM] is for informational purposes only.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Write reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure write reference to an ASC region
                                                                 not enabled for secure access, or nonsecure write reference to an
                                                                 ASC region not enabled for nonsecure access.
                                                                 This may be an indication of software
                                                                 failure, and may be considered fatal.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) No way was available for allocation. L2C sets [NOWAY] during its processing of a
                                                                 transaction whenever it needed/wanted to allocate a WAY in the L2 cache, but was
                                                                 unable to. When this bit = 1, it is (generally) not an indication that L2C
                                                                 failed to complete transactions. Rather, it is a hint of possible performance
                                                                 degradation. (For example, L2C must read- modify-write DRAM for every
                                                                 transaction that updates some, but not all, of the bytes in a cache block,
                                                                 misses in the L2 cache, and cannot allocate a WAY.) There is one 'failure' case
                                                                 where L2C sets [NOWAY]: when it cannot leave a block locked in the L2 cache as
                                                                 part of a LCKL2 transaction. See L2C_TTG()_ERR for logged information. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) TAG double-bit error occurred. See L2C_TTG()_ERR for logged information.
                                                                 This is an indication of a hardware failure and may be considered fatal. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) TAG single-bit error on a read. See L2C_TAD()_TTG_ERR for logged
                                                                 information. When [TAGSBE] is set, hardware corrected the error before using the
                                                                 tag, but did not correct any stored value. When [TAGSBE] is set, software should
                                                                 eject the TAG location indicated by the corresponding
                                                                 L2C_TAD()_TTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [TAGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same TAG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                   \<pre\>
                                                                   payload\<24\> = 0
                                                                   payload\<23:20\> = L2C_TAD()_TTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_TTG_ERR[L2IDX]
                                                                   \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on this payload. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) FBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) FBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) SBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) SBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) L2D double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) L2D single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. When [L2DSBE] is set, hardware corrected the error before using the
                                                                 data, but did not correct any stored value. When [L2DSBE] is set, software
                                                                 should eject the cache block indicated by the corresponding
                                                                 L2C_TAD()_TQD_ERR[QDNUM,L2DIDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [L2DSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same L2D location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24\>    = 0
                                                                   payload\<23:20\> = L2C_TAD()_TQD_ERR[L2DIDX]\<10:7\>  // way
                                                                   payload\<19:13\> = L2C_TAD()_TQD_ERR[L2DIDX]\<6:0\>   // index\<12:6\>
                                                                   payload\<12:11\> = L2C_TAD()_TQD_ERR[L2DIDX]\<12:11\> // index\<5:4\>
                                                                   payload\<10\>    = L2C_TAD()_TQD_ERR[QDNUM]\<2\>      // index\<3\>
                                                                   payload\<9:7\>   = tad             // index\<2:0\>
                                                                 \</pre\>

                                                                 where tad is the TAD index from this CSR. Note that L2C_CTL[DISIDXALIAS] has no
                                                                 effect on the payload. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) L2D single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. When [L2DSBE] is set, hardware corrected the error before using the
                                                                 data, but did not correct any stored value. When [L2DSBE] is set, software
                                                                 should eject the cache block indicated by the corresponding
                                                                 L2C_TAD()_TQD_ERR[QDNUM,L2DIDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [L2DSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same L2D location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24\>    = 0
                                                                   payload\<23:20\> = L2C_TAD()_TQD_ERR[L2DIDX]\<10:7\>  // way
                                                                   payload\<19:13\> = L2C_TAD()_TQD_ERR[L2DIDX]\<6:0\>   // index\<12:6\>
                                                                   payload\<12:11\> = L2C_TAD()_TQD_ERR[L2DIDX]\<12:11\> // index\<5:4\>
                                                                   payload\<10\>    = L2C_TAD()_TQD_ERR[QDNUM]\<2\>      // index\<3\>
                                                                   payload\<9:7\>   = tad             // index\<2:0\>
                                                                 \</pre\>

                                                                 where tad is the TAD index from this CSR. Note that L2C_CTL[DISIDXALIAS] has no
                                                                 effect on the payload. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) L2D double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) SBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) SBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) FBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) FBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) TAG single-bit error on a read. See L2C_TAD()_TTG_ERR for logged
                                                                 information. When [TAGSBE] is set, hardware corrected the error before using the
                                                                 tag, but did not correct any stored value. When [TAGSBE] is set, software should
                                                                 eject the TAG location indicated by the corresponding
                                                                 L2C_TAD()_TTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [TAGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same TAG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                   \<pre\>
                                                                   payload\<24\> = 0
                                                                   payload\<23:20\> = L2C_TAD()_TTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_TTG_ERR[L2IDX]
                                                                   \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on this payload. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) TAG double-bit error occurred. See L2C_TTG()_ERR for logged information.
                                                                 This is an indication of a hardware failure and may be considered fatal. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) No way was available for allocation. L2C sets [NOWAY] during its processing of a
                                                                 transaction whenever it needed/wanted to allocate a WAY in the L2 cache, but was
                                                                 unable to. When this bit = 1, it is (generally) not an indication that L2C
                                                                 failed to complete transactions. Rather, it is a hint of possible performance
                                                                 degradation. (For example, L2C must read- modify-write DRAM for every
                                                                 transaction that updates some, but not all, of the bytes in a cache block,
                                                                 misses in the L2 cache, and cannot allocate a WAY.) There is one 'failure' case
                                                                 where L2C sets [NOWAY]: when it cannot leave a block locked in the L2 cache as
                                                                 part of a LCKL2 transaction. See L2C_TTG()_ERR for logged information. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Write reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure write reference to an ASC region
                                                                 not enabled for secure access, or nonsecure write reference to an
                                                                 ASC region not enabled for nonsecure access.
                                                                 This may be an indication of software
                                                                 failure, and may be considered fatal.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Read reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure read reference to an ASC region
                                                                 not enabled for secure access, or nonsecure read reference to an ASC
                                                                 region not enabled for nonsecure access.
                                                                 [RDNXM] interrupts can occur during normal operation as the cores are
                                                                 allowed to prefetch to nonexistent memory locations.  Therefore,
                                                                 [RDNXM] is for informational purposes only.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Illegal read to disabled LMC error. A DRAM read arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Illegal write to disabled LMC error. A DRAM write arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) An LFB entry (or more) has encountered a timeout condition When [LFBTO] timeout
                                                                 condition occurs L2C_TAD()_TIMEOUT is loaded. L2C_TAD()_TIMEOUT is loaded with
                                                                 info from the first LFB that timed out. if multiple LFB timed out
                                                                 simultaneously, then the it will capture info from the lowest LFB number that
                                                                 timed out.
                                                                 Should not occur during normal operation.  OCI/CCPI link failures may cause this
                                                                 failure. This may be an indication of hardware failure, and may be considered
                                                                 fatal. */
        uint64_t reserved_18           : 1;
        uint64_t reserved_19_31        : 13;
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1C/H) RTG single-bit error on a read. See L2C_TAD()_RTG_ERR for logged
                                                                 information. When [RTGSBE] is set, hardware corrected the error before using the
                                                                 RTG tag, but did not correct any stored value. When [RTGSBE] is set, software
                                                                 should eject the RTG location indicated by the corresponding
                                                                 L2C_TAD()_RTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [RTGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same RTG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24\> = 1
                                                                   payload\<23:20\> = L2C_TAD()_RTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_RTG_ERR[L2IDX]
                                                                 \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on the payload. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1C/H) RTG double-bit error.
                                                                 See L2C_TAD()_RTG_ERR for logged information.
                                                                 An indication of a hardware failure and may be considered fatal. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Illegal read operation to a remote node with L2C_OCI_CTL[ENAOCI][node]
                                                                 clear. Note [RDDISOCI] interrupts can occur during normal operation as the cores
                                                                 are allowed to prefetch to nonexistent memory locations. Therefore, [RDDISOCI]
                                                                 is for informational purposes only. See L2C_TAD()_ERR for logged information. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Illegal write operation to a remote node with L2C_OCI_CTL[ENAOCI][node] clear. See
                                                                 L2C_TAD()_ERR for logged information.
                                                                 During normal hardware operation, an indication of a software failure and may be
                                                                 considered fatal. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_l2c_tadx_int_w1c_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Illegal write operation to a remote node with L2C_OCI_CTL[ENAOCI][node] clear. See
                                                                 L2C_TAD()_ERR for logged information.
                                                                 During normal hardware operation, an indication of a software failure and may be
                                                                 considered fatal. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Illegal read operation to a remote node with L2C_OCI_CTL[ENAOCI][node]
                                                                 clear. Note [RDDISOCI] interrupts can occur during normal operation as the cores
                                                                 are allowed to prefetch to nonexistent memory locations. Therefore, [RDDISOCI]
                                                                 is for informational purposes only. See L2C_TAD()_ERR for logged information. */
        uint64_t reserved_19_33        : 15;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Global sync timeout. Should not occur during normal operation. This may be an
                                                                 indication of hardware failure, and may be considered fatal. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) An LFB entry (or more) has encountered a timeout condition When [LFBTO] timeout
                                                                 condition occurs L2C_TAD()_TIMEOUT is loaded. L2C_TAD()_TIMEOUT is loaded with
                                                                 info from the first LFB that timed out. if multiple LFB timed out
                                                                 simultaneously, then the it will capture info from the lowest LFB number that
                                                                 timed out.
                                                                 Should not occur during normal operation.  OCI/CCPI link failures may cause this
                                                                 failure. This may be an indication of hardware failure, and may be considered
                                                                 fatal. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Illegal write to disabled LMC error. A DRAM write arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Illegal read to disabled LMC error. A DRAM read arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Read reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure read reference to an ASC region
                                                                 not enabled for secure access, or nonsecure read reference to an ASC
                                                                 region not enabled for nonsecure access.
                                                                 [RDNXM] interrupts can occur during normal operation as the cores are
                                                                 allowed to prefetch to nonexistent memory locations.  Therefore,
                                                                 [RDNXM] is for informational purposes only.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Write reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure write reference to an ASC region
                                                                 not enabled for secure access, or nonsecure write reference to an
                                                                 ASC region not enabled for nonsecure access.
                                                                 This may be an indication of software
                                                                 failure, and may be considered fatal.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) No way was available for allocation. L2C sets [NOWAY] during its processing of a
                                                                 transaction whenever it needed/wanted to allocate a WAY in the L2 cache, but was
                                                                 unable to. When this bit = 1, it is (generally) not an indication that L2C
                                                                 failed to complete transactions. Rather, it is a hint of possible performance
                                                                 degradation. (For example, L2C must read- modify-write DRAM for every
                                                                 transaction that updates some, but not all, of the bytes in a cache block,
                                                                 misses in the L2 cache, and cannot allocate a WAY.) There is one 'failure' case
                                                                 where L2C sets [NOWAY]: when it cannot leave a block locked in the L2 cache as
                                                                 part of a LCKL2 transaction. See L2C_TTG()_ERR for logged information. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) TAG double-bit error occurred. See L2C_TTG()_ERR for logged information.
                                                                 This is an indication of a hardware failure and may be considered fatal. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) TAG single-bit error on a read. See L2C_TAD()_TTG_ERR for logged
                                                                 information. When [TAGSBE] is set, hardware corrected the error before using the
                                                                 tag, but did not correct any stored value. When [TAGSBE] is set, software should
                                                                 eject the TAG location indicated by the corresponding
                                                                 L2C_TAD()_TTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [TAGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same TAG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                   \<pre\>
                                                                   payload\<24\> = 0
                                                                   payload\<23:20\> = L2C_TAD()_TTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_TTG_ERR[L2IDX]
                                                                   \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on this payload. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) FBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) FBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) SBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) SBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) L2D double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) L2D single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. When [L2DSBE] is set, hardware corrected the error before using the
                                                                 data, but did not correct any stored value. When [L2DSBE] is set, software
                                                                 should eject the cache block indicated by the corresponding
                                                                 L2C_TAD()_TQD_ERR[QDNUM,L2DIDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [L2DSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same L2D location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24:21\> = 0
                                                                   payload\<20:17\> = L2C_TAD()_TQD_ERR[L2DIDX]\<10:7\>  // way
                                                                   payload\<16:10\> = L2C_TAD()_TQD_ERR[L2DIDX]\<6:0\>   // index\<9:3\>
                                                                   payload\<9:8\>   = L2C_TAD()_TQD_ERR[L2DIDX]\<12:11\> // index\<2:1\>
                                                                   payload\<7\>     = L2C_TAD()_TQD_ERR[QDNUM]\<2\>      // index\<0\>
                                                                 \</pre\>

                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on the payload. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) L2D single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. When [L2DSBE] is set, hardware corrected the error before using the
                                                                 data, but did not correct any stored value. When [L2DSBE] is set, software
                                                                 should eject the cache block indicated by the corresponding
                                                                 L2C_TAD()_TQD_ERR[QDNUM,L2DIDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [L2DSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same L2D location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24:21\> = 0
                                                                   payload\<20:17\> = L2C_TAD()_TQD_ERR[L2DIDX]\<10:7\>  // way
                                                                   payload\<16:10\> = L2C_TAD()_TQD_ERR[L2DIDX]\<6:0\>   // index\<9:3\>
                                                                   payload\<9:8\>   = L2C_TAD()_TQD_ERR[L2DIDX]\<12:11\> // index\<2:1\>
                                                                   payload\<7\>     = L2C_TAD()_TQD_ERR[QDNUM]\<2\>      // index\<0\>
                                                                 \</pre\>

                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on the payload. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) L2D double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) SBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) SBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) FBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) FBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) TAG single-bit error on a read. See L2C_TAD()_TTG_ERR for logged
                                                                 information. When [TAGSBE] is set, hardware corrected the error before using the
                                                                 tag, but did not correct any stored value. When [TAGSBE] is set, software should
                                                                 eject the TAG location indicated by the corresponding
                                                                 L2C_TAD()_TTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [TAGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same TAG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                   \<pre\>
                                                                   payload\<24\> = 0
                                                                   payload\<23:20\> = L2C_TAD()_TTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_TTG_ERR[L2IDX]
                                                                   \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on this payload. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) TAG double-bit error occurred. See L2C_TTG()_ERR for logged information.
                                                                 This is an indication of a hardware failure and may be considered fatal. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) No way was available for allocation. L2C sets [NOWAY] during its processing of a
                                                                 transaction whenever it needed/wanted to allocate a WAY in the L2 cache, but was
                                                                 unable to. When this bit = 1, it is (generally) not an indication that L2C
                                                                 failed to complete transactions. Rather, it is a hint of possible performance
                                                                 degradation. (For example, L2C must read- modify-write DRAM for every
                                                                 transaction that updates some, but not all, of the bytes in a cache block,
                                                                 misses in the L2 cache, and cannot allocate a WAY.) There is one 'failure' case
                                                                 where L2C sets [NOWAY]: when it cannot leave a block locked in the L2 cache as
                                                                 part of a LCKL2 transaction. See L2C_TTG()_ERR for logged information. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Write reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure write reference to an ASC region
                                                                 not enabled for secure access, or nonsecure write reference to an
                                                                 ASC region not enabled for nonsecure access.
                                                                 This may be an indication of software
                                                                 failure, and may be considered fatal.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Read reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure read reference to an ASC region
                                                                 not enabled for secure access, or nonsecure read reference to an ASC
                                                                 region not enabled for nonsecure access.
                                                                 [RDNXM] interrupts can occur during normal operation as the cores are
                                                                 allowed to prefetch to nonexistent memory locations.  Therefore,
                                                                 [RDNXM] is for informational purposes only.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Illegal read to disabled LMC error. A DRAM read arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Illegal write to disabled LMC error. A DRAM write arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) An LFB entry (or more) has encountered a timeout condition When [LFBTO] timeout
                                                                 condition occurs L2C_TAD()_TIMEOUT is loaded. L2C_TAD()_TIMEOUT is loaded with
                                                                 info from the first LFB that timed out. if multiple LFB timed out
                                                                 simultaneously, then the it will capture info from the lowest LFB number that
                                                                 timed out.
                                                                 Should not occur during normal operation.  OCI/CCPI link failures may cause this
                                                                 failure. This may be an indication of hardware failure, and may be considered
                                                                 fatal. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Global sync timeout. Should not occur during normal operation. This may be an
                                                                 indication of hardware failure, and may be considered fatal. */
        uint64_t reserved_19_33        : 15;
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Illegal read operation to a remote node with L2C_OCI_CTL[ENAOCI][node]
                                                                 clear. Note [RDDISOCI] interrupts can occur during normal operation as the cores
                                                                 are allowed to prefetch to nonexistent memory locations. Therefore, [RDDISOCI]
                                                                 is for informational purposes only. See L2C_TAD()_ERR for logged information. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Illegal write operation to a remote node with L2C_OCI_CTL[ENAOCI][node] clear. See
                                                                 L2C_TAD()_ERR for logged information.
                                                                 During normal hardware operation, an indication of a software failure and may be
                                                                 considered fatal. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_l2c_tadx_int_w1c_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Illegal write operation to a remote node with L2C_OCI_CTL[ENAOCI][node] clear. See
                                                                 L2C_TAD()_ERR for logged information.
                                                                 During normal hardware operation, an indication of a software failure and may be
                                                                 considered fatal. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Illegal read operation to a remote node with L2C_OCI_CTL[ENAOCI][node]
                                                                 clear. Note [RDDISOCI] interrupts can occur during normal operation as the cores
                                                                 are allowed to prefetch to nonexistent memory locations. Therefore, [RDDISOCI]
                                                                 is for informational purposes only. See L2C_TAD()_ERR for logged information. */
        uint64_t reserved_19_33        : 15;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Global sync timeout. Should not occur during normal operation. This may be an
                                                                 indication of hardware failure, and may be considered fatal. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) An LFB entry (or more) has encountered a timeout condition When [LFBTO] timeout
                                                                 condition occurs L2C_TAD()_TIMEOUT is loaded. L2C_TAD()_TIMEOUT is loaded with
                                                                 info from the first LFB that timed out. if multiple LFB timed out
                                                                 simultaneously, then the it will capture info from the lowest LFB number that
                                                                 timed out.
                                                                 Should not occur during normal operation.  OCI/CCPI link failures may cause this
                                                                 failure. This may be an indication of hardware failure, and may be considered
                                                                 fatal. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Illegal write to disabled LMC error. A DRAM write arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Illegal read to disabled LMC error. A DRAM read arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Read reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure read reference to an ASC region
                                                                 not enabled for secure access, or nonsecure read reference to an ASC
                                                                 region not enabled for nonsecure access.
                                                                 [RDNXM] interrupts can occur during normal operation as the cores are
                                                                 allowed to prefetch to nonexistent memory locations.  Therefore,
                                                                 [RDNXM] is for informational purposes only.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Write reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure write reference to an ASC region
                                                                 not enabled for secure access, or nonsecure write reference to an
                                                                 ASC region not enabled for nonsecure access.
                                                                 This may be an indication of software
                                                                 failure, and may be considered fatal.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) No way was available for allocation. L2C sets [NOWAY] during its processing of a
                                                                 transaction whenever it needed/wanted to allocate a WAY in the L2 cache, but was
                                                                 unable to. When this bit = 1, it is (generally) not an indication that L2C
                                                                 failed to complete transactions. Rather, it is a hint of possible performance
                                                                 degradation. (For example, L2C must read- modify-write DRAM for every
                                                                 transaction that updates some, but not all, of the bytes in a cache block,
                                                                 misses in the L2 cache, and cannot allocate a WAY.) There is one 'failure' case
                                                                 where L2C sets [NOWAY]: when it cannot leave a block locked in the L2 cache as
                                                                 part of a LCKL2 transaction. See L2C_TTG()_ERR for logged information. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) TAG double-bit error occurred. See L2C_TTG()_ERR for logged information.
                                                                 This is an indication of a hardware failure and may be considered fatal. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) TAG single-bit error on a read. See L2C_TAD()_TTG_ERR for logged
                                                                 information. When [TAGSBE] is set, hardware corrected the error before using the
                                                                 tag, but did not correct any stored value. When [TAGSBE] is set, software should
                                                                 eject the TAG location indicated by the corresponding
                                                                 L2C_TAD()_TTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [TAGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same TAG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                   \<pre\>
                                                                   payload\<24\> = 0
                                                                   payload\<23:20\> = L2C_TAD()_TTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_TTG_ERR[L2IDX]
                                                                   \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on this payload. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) FBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) FBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) SBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) SBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) L2D double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) L2D single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. When [L2DSBE] is set, hardware corrected the error before using the
                                                                 data, but did not correct any stored value. When [L2DSBE] is set, software
                                                                 should eject the cache block indicated by the corresponding
                                                                 L2C_TAD()_TQD_ERR[QDNUM,L2DIDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [L2DSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same L2D location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24:23\> = 0
                                                                   payload\<22:19\> = L2C_TAD()_TQD_ERR[L2DIDX]\<10:7\>  // way
                                                                   payload\<18:12\> = L2C_TAD()_TQD_ERR[L2DIDX]\<6:0\>   // index\<11:5\>
                                                                   payload\<11:10\> = L2C_TAD()_TQD_ERR[L2DIDX]\<12:11\> // index\<4:3\>
                                                                   payload\<9\>     = L2C_TAD()_TQD_ERR[QDNUM]\<2\>      // index\<2\>
                                                                   payload\<8:7\>   = tad             // index\<1:0\>
                                                                 \</pre\>

                                                                 where tad is the TAD index from this CSR. Note that L2C_CTL[DISIDXALIAS] has no
                                                                 effect on the payload. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) L2D single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. When [L2DSBE] is set, hardware corrected the error before using the
                                                                 data, but did not correct any stored value. When [L2DSBE] is set, software
                                                                 should eject the cache block indicated by the corresponding
                                                                 L2C_TAD()_TQD_ERR[QDNUM,L2DIDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [L2DSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same L2D location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24:23\> = 0
                                                                   payload\<22:19\> = L2C_TAD()_TQD_ERR[L2DIDX]\<10:7\>  // way
                                                                   payload\<18:12\> = L2C_TAD()_TQD_ERR[L2DIDX]\<6:0\>   // index\<11:5\>
                                                                   payload\<11:10\> = L2C_TAD()_TQD_ERR[L2DIDX]\<12:11\> // index\<4:3\>
                                                                   payload\<9\>     = L2C_TAD()_TQD_ERR[QDNUM]\<2\>      // index\<2\>
                                                                   payload\<8:7\>   = tad             // index\<1:0\>
                                                                 \</pre\>

                                                                 where tad is the TAD index from this CSR. Note that L2C_CTL[DISIDXALIAS] has no
                                                                 effect on the payload. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) L2D double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) SBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) SBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) FBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) FBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) TAG single-bit error on a read. See L2C_TAD()_TTG_ERR for logged
                                                                 information. When [TAGSBE] is set, hardware corrected the error before using the
                                                                 tag, but did not correct any stored value. When [TAGSBE] is set, software should
                                                                 eject the TAG location indicated by the corresponding
                                                                 L2C_TAD()_TTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [TAGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same TAG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                   \<pre\>
                                                                   payload\<24\> = 0
                                                                   payload\<23:20\> = L2C_TAD()_TTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_TTG_ERR[L2IDX]
                                                                   \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on this payload. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) TAG double-bit error occurred. See L2C_TTG()_ERR for logged information.
                                                                 This is an indication of a hardware failure and may be considered fatal. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) No way was available for allocation. L2C sets [NOWAY] during its processing of a
                                                                 transaction whenever it needed/wanted to allocate a WAY in the L2 cache, but was
                                                                 unable to. When this bit = 1, it is (generally) not an indication that L2C
                                                                 failed to complete transactions. Rather, it is a hint of possible performance
                                                                 degradation. (For example, L2C must read- modify-write DRAM for every
                                                                 transaction that updates some, but not all, of the bytes in a cache block,
                                                                 misses in the L2 cache, and cannot allocate a WAY.) There is one 'failure' case
                                                                 where L2C sets [NOWAY]: when it cannot leave a block locked in the L2 cache as
                                                                 part of a LCKL2 transaction. See L2C_TTG()_ERR for logged information. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Write reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure write reference to an ASC region
                                                                 not enabled for secure access, or nonsecure write reference to an
                                                                 ASC region not enabled for nonsecure access.
                                                                 This may be an indication of software
                                                                 failure, and may be considered fatal.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Read reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure read reference to an ASC region
                                                                 not enabled for secure access, or nonsecure read reference to an ASC
                                                                 region not enabled for nonsecure access.
                                                                 [RDNXM] interrupts can occur during normal operation as the cores are
                                                                 allowed to prefetch to nonexistent memory locations.  Therefore,
                                                                 [RDNXM] is for informational purposes only.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Illegal read to disabled LMC error. A DRAM read arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Illegal write to disabled LMC error. A DRAM write arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) An LFB entry (or more) has encountered a timeout condition When [LFBTO] timeout
                                                                 condition occurs L2C_TAD()_TIMEOUT is loaded. L2C_TAD()_TIMEOUT is loaded with
                                                                 info from the first LFB that timed out. if multiple LFB timed out
                                                                 simultaneously, then the it will capture info from the lowest LFB number that
                                                                 timed out.
                                                                 Should not occur during normal operation.  OCI/CCPI link failures may cause this
                                                                 failure. This may be an indication of hardware failure, and may be considered
                                                                 fatal. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Global sync timeout. Should not occur during normal operation. This may be an
                                                                 indication of hardware failure, and may be considered fatal. */
        uint64_t reserved_19_33        : 15;
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Illegal read operation to a remote node with L2C_OCI_CTL[ENAOCI][node]
                                                                 clear. Note [RDDISOCI] interrupts can occur during normal operation as the cores
                                                                 are allowed to prefetch to nonexistent memory locations. Therefore, [RDDISOCI]
                                                                 is for informational purposes only. See L2C_TAD()_ERR for logged information. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Illegal write operation to a remote node with L2C_OCI_CTL[ENAOCI][node] clear. See
                                                                 L2C_TAD()_ERR for logged information.
                                                                 During normal hardware operation, an indication of a software failure and may be
                                                                 considered fatal. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn83xx;
    struct bdk_l2c_tadx_int_w1c_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Illegal write operation to a remote node with L2C_OCI_CTL[ENAOCI][node] clear. See
                                                                 L2C_TAD()_ERR for logged information.
                                                                 During normal hardware operation, an indication of a software failure and may be
                                                                 considered fatal. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Illegal read operation to a remote node with L2C_OCI_CTL[ENAOCI][node]
                                                                 clear. Note [RDDISOCI] interrupts can occur during normal operation as the cores
                                                                 are allowed to prefetch to nonexistent memory locations. Therefore, [RDDISOCI]
                                                                 is for informational purposes only. See L2C_TAD()_ERR for logged information. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1C/H) RTG double-bit error.
                                                                 See L2C_TAD()_RTG_ERR for logged information.
                                                                 An indication of a hardware failure and may be considered fatal. */
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1C/H) RTG single-bit error on a read. See L2C_TAD()_RTG_ERR for logged
                                                                 information. When [RTGSBE] is set, hardware corrected the error before using the
                                                                 RTG tag, but did not correct any stored value. When [RTGSBE] is set, software
                                                                 should eject the RTG location indicated by the corresponding
                                                                 L2C_TAD()_RTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [RTGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same RTG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24\> = 1
                                                                   payload\<23:20\> = L2C_TAD()_RTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_RTG_ERR[L2IDX]
                                                                 \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on the payload. */
        uint64_t reserved_19_31        : 13;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Global sync OCI timeout. Should not occur during normal operation. OCI/CCPI link
                                                                 failures may cause this failure. This may be an indication of hardware failure,
                                                                 and may be considered fatal. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) An LFB entry (or more) has encountered a timeout condition When [LFBTO] timeout
                                                                 condition occurs L2C_TAD()_TIMEOUT is loaded. L2C_TAD()_TIMEOUT is loaded with
                                                                 info from the first LFB that timed out. if multiple LFB timed out
                                                                 simultaneously, then the it will capture info from the lowest LFB number that
                                                                 timed out.
                                                                 Should not occur during normal operation.  OCI/CCPI link failures may cause this
                                                                 failure. This may be an indication of hardware failure, and may be considered
                                                                 fatal. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Illegal write to disabled LMC error. A DRAM write arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Illegal read to disabled LMC error. A DRAM read arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Read reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure read reference to an ASC region
                                                                 not enabled for secure access, or nonsecure read reference to an ASC
                                                                 region not enabled for nonsecure access.
                                                                 [RDNXM] interrupts can occur during normal operation as the cores are
                                                                 allowed to prefetch to nonexistent memory locations.  Therefore,
                                                                 [RDNXM] is for informational purposes only.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Write reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure write reference to an ASC region
                                                                 not enabled for secure access, or nonsecure write reference to an
                                                                 ASC region not enabled for nonsecure access.
                                                                 This may be an indication of software
                                                                 failure, and may be considered fatal.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) No way was available for allocation. L2C sets [NOWAY] during its processing of a
                                                                 transaction whenever it needed/wanted to allocate a WAY in the L2 cache, but was
                                                                 unable to. When this bit = 1, it is (generally) not an indication that L2C
                                                                 failed to complete transactions. Rather, it is a hint of possible performance
                                                                 degradation. (For example, L2C must read- modify-write DRAM for every
                                                                 transaction that updates some, but not all, of the bytes in a cache block,
                                                                 misses in the L2 cache, and cannot allocate a WAY.) There is one 'failure' case
                                                                 where L2C sets [NOWAY]: when it cannot leave a block locked in the L2 cache as
                                                                 part of a LCKL2 transaction. See L2C_TTG()_ERR for logged information. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) TAG double-bit error occurred. See L2C_TTG()_ERR for logged information.
                                                                 This is an indication of a hardware failure and may be considered fatal. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) TAG single-bit error on a read. See L2C_TAD()_TTG_ERR for logged
                                                                 information. When [TAGSBE] is set, hardware corrected the error before using the
                                                                 tag, but did not correct any stored value. When [TAGSBE] is set, software should
                                                                 eject the TAG location indicated by the corresponding
                                                                 L2C_TAD()_TTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [TAGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same TAG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                   \<pre\>
                                                                   payload\<24\> = 0
                                                                   payload\<23:20\> = L2C_TAD()_TTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_TTG_ERR[L2IDX]
                                                                   \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on this payload. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) FBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) FBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) SBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) SBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) L2D double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) L2D single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. When [L2DSBE] is set, hardware corrected the error before using the
                                                                 data, but did not correct any stored value. When [L2DSBE] is set, software
                                                                 should eject the cache block indicated by the corresponding
                                                                 L2C_TAD()_TQD_ERR[QDNUM,L2DIDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [L2DSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same L2D location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24\>    = 0
                                                                   payload\<23:20\> = L2C_TAD()_TQD_ERR[L2DIDX]\<10:7\>  // way
                                                                   payload\<19:13\> = L2C_TAD()_TQD_ERR[L2DIDX]\<6:0\>   // index\<12:6\>
                                                                   payload\<12:11\> = L2C_TAD()_TQD_ERR[L2DIDX]\<12:11\> // index\<5:4\>
                                                                   payload\<10\>    = L2C_TAD()_TQD_ERR[QDNUM]\<2\>      // index\<3\>
                                                                   payload\<9:7\>   = tad             // index\<2:0\>
                                                                 \</pre\>

                                                                 where tad is the TAD index from this CSR. Note that L2C_CTL[DISIDXALIAS] has no
                                                                 effect on the payload. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1C/H) L2D single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. When [L2DSBE] is set, hardware corrected the error before using the
                                                                 data, but did not correct any stored value. When [L2DSBE] is set, software
                                                                 should eject the cache block indicated by the corresponding
                                                                 L2C_TAD()_TQD_ERR[QDNUM,L2DIDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [L2DSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same L2D location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24\>    = 0
                                                                   payload\<23:20\> = L2C_TAD()_TQD_ERR[L2DIDX]\<10:7\>  // way
                                                                   payload\<19:13\> = L2C_TAD()_TQD_ERR[L2DIDX]\<6:0\>   // index\<12:6\>
                                                                   payload\<12:11\> = L2C_TAD()_TQD_ERR[L2DIDX]\<12:11\> // index\<5:4\>
                                                                   payload\<10\>    = L2C_TAD()_TQD_ERR[QDNUM]\<2\>      // index\<3\>
                                                                   payload\<9:7\>   = tad             // index\<2:0\>
                                                                 \</pre\>

                                                                 where tad is the TAD index from this CSR. Note that L2C_CTL[DISIDXALIAS] has no
                                                                 effect on the payload. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1C/H) L2D double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1C/H) SBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1C/H) SBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1C/H) FBF single-bit error on a read. See L2C_TAD()_TQD_ERR for logged
                                                                 information. Hardware automatically corrected the error. Software may choose to
                                                                 count the number of these single-bit errors. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1C/H) FBF double-bit error occurred. See L2C_TAD()_TQD_ERR for logged information. An
                                                                 indication of a hardware failure and may be considered fatal. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1C/H) TAG single-bit error on a read. See L2C_TAD()_TTG_ERR for logged
                                                                 information. When [TAGSBE] is set, hardware corrected the error before using the
                                                                 tag, but did not correct any stored value. When [TAGSBE] is set, software should
                                                                 eject the TAG location indicated by the corresponding
                                                                 L2C_TAD()_TTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [TAGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same TAG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                   \<pre\>
                                                                   payload\<24\> = 0
                                                                   payload\<23:20\> = L2C_TAD()_TTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_TTG_ERR[L2IDX]
                                                                   \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on this payload. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1C/H) TAG double-bit error occurred. See L2C_TTG()_ERR for logged information.
                                                                 This is an indication of a hardware failure and may be considered fatal. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1C/H) No way was available for allocation. L2C sets [NOWAY] during its processing of a
                                                                 transaction whenever it needed/wanted to allocate a WAY in the L2 cache, but was
                                                                 unable to. When this bit = 1, it is (generally) not an indication that L2C
                                                                 failed to complete transactions. Rather, it is a hint of possible performance
                                                                 degradation. (For example, L2C must read- modify-write DRAM for every
                                                                 transaction that updates some, but not all, of the bytes in a cache block,
                                                                 misses in the L2 cache, and cannot allocate a WAY.) There is one 'failure' case
                                                                 where L2C sets [NOWAY]: when it cannot leave a block locked in the L2 cache as
                                                                 part of a LCKL2 transaction. See L2C_TTG()_ERR for logged information. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1C/H) Write reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure write reference to an ASC region
                                                                 not enabled for secure access, or nonsecure write reference to an
                                                                 ASC region not enabled for nonsecure access.
                                                                 This may be an indication of software
                                                                 failure, and may be considered fatal.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1C/H) Read reference outside all the defined and enabled address space
                                                                 control (ASC) regions, or secure read reference to an ASC region
                                                                 not enabled for secure access, or nonsecure read reference to an ASC
                                                                 region not enabled for nonsecure access.
                                                                 [RDNXM] interrupts can occur during normal operation as the cores are
                                                                 allowed to prefetch to nonexistent memory locations.  Therefore,
                                                                 [RDNXM] is for informational purposes only.
                                                                 See L2C_TAD()_ERR for logged information.
                                                                 See L2C_ASC_REGION()_START, L2C_ASC_REGION()_END, and
                                                                 L2C_ASC_REGION()_ATTR for ASC region specification. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1C/H) Illegal read to disabled LMC error. A DRAM read arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1C/H) Illegal write to disabled LMC error. A DRAM write arrived before LMC was enabled.
                                                                 Should not occur during normal operation.
                                                                 This may be considered fatal. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1C/H) An LFB entry (or more) has encountered a timeout condition When [LFBTO] timeout
                                                                 condition occurs L2C_TAD()_TIMEOUT is loaded. L2C_TAD()_TIMEOUT is loaded with
                                                                 info from the first LFB that timed out. if multiple LFB timed out
                                                                 simultaneously, then the it will capture info from the lowest LFB number that
                                                                 timed out.
                                                                 Should not occur during normal operation.  OCI/CCPI link failures may cause this
                                                                 failure. This may be an indication of hardware failure, and may be considered
                                                                 fatal. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1C/H) Global sync OCI timeout. Should not occur during normal operation. OCI/CCPI link
                                                                 failures may cause this failure. This may be an indication of hardware failure,
                                                                 and may be considered fatal. */
        uint64_t reserved_19_31        : 13;
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1C/H) RTG single-bit error on a read. See L2C_TAD()_RTG_ERR for logged
                                                                 information. When [RTGSBE] is set, hardware corrected the error before using the
                                                                 RTG tag, but did not correct any stored value. When [RTGSBE] is set, software
                                                                 should eject the RTG location indicated by the corresponding
                                                                 L2C_TAD()_RTG_ERR[WAY,L2IDX] (via a SYS CVMCACHEWBIL2I instruction below)
                                                                 before clearing [RTGSBE]. Otherwise, hardware may encounter the error again the
                                                                 next time the same RTG location is referenced. Software may also choose to count
                                                                 the number of these single-bit errors.

                                                                 The SYS CVMCACHEWBIL2I instruction payload should have:
                                                                 \<pre\>
                                                                   payload\<24\> = 1
                                                                   payload\<23:20\> = L2C_TAD()_RTG_ERR[WAY]
                                                                   payload\<19:7\>  = L2C_TAD()_RTG_ERR[L2IDX]
                                                                 \</pre\>
                                                                 Note that L2C_CTL[DISIDXALIAS] has no effect on the payload. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1C/H) RTG double-bit error.
                                                                 See L2C_TAD()_RTG_ERR for logged information.
                                                                 An indication of a hardware failure and may be considered fatal. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1C/H) Illegal read operation to a remote node with L2C_OCI_CTL[ENAOCI][node]
                                                                 clear. Note [RDDISOCI] interrupts can occur during normal operation as the cores
                                                                 are allowed to prefetch to nonexistent memory locations. Therefore, [RDDISOCI]
                                                                 is for informational purposes only. See L2C_TAD()_ERR for logged information. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1C/H) Illegal write operation to a remote node with L2C_OCI_CTL[ENAOCI][node] clear. See
                                                                 L2C_TAD()_ERR for logged information.
                                                                 During normal hardware operation, an indication of a software failure and may be
                                                                 considered fatal. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_l2c_tadx_int_w1c bdk_l2c_tadx_int_w1c_t;

static inline uint64_t BDK_L2C_TADX_INT_W1C(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_INT_W1C(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050040000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050040000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050040000ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_INT_W1C", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_INT_W1C(a) bdk_l2c_tadx_int_w1c_t
#define bustype_BDK_L2C_TADX_INT_W1C(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_INT_W1C(a) "L2C_TADX_INT_W1C"
#define device_bar_BDK_L2C_TADX_INT_W1C(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_INT_W1C(a) (a)
#define arguments_BDK_L2C_TADX_INT_W1C(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad#_int_w1s
 *
 * L2C TAD Interrupt Set Registers
 * This register sets interrupt bits.
 */
union bdk_l2c_tadx_int_w1s
{
    uint64_t u;
    struct bdk_l2c_tadx_int_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t reserved_19_31        : 13;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[GSYNCTO]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[GSYNCTO]. */
        uint64_t reserved_19_31        : 13;
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    struct bdk_l2c_tadx_int_w1s_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t reserved_19_31        : 13;
        uint64_t reserved_18           : 1;
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t reserved_18           : 1;
        uint64_t reserved_19_31        : 13;
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_l2c_tadx_int_w1s_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[WRDISOCI]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[RDDISOCI]. */
        uint64_t reserved_19_33        : 15;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[GSYNCTO]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[LFBTO]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[WRDISLMC]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[RDDISLMC]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[RDNXM]. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[WRNXM]. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[NOWAY]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[TAGDBE]. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[TAGSBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[FBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[FBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[SBFDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[SBFSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[L2DDBE]. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[L2DSBE]. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[L2DSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[L2DDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[SBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[SBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[FBFSBE]. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[FBFDBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[TAGSBE]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[TAGDBE]. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[NOWAY]. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[WRNXM]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[RDNXM]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[RDDISLMC]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[WRDISLMC]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[LFBTO]. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[GSYNCTO]. */
        uint64_t reserved_19_33        : 15;
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[RDDISOCI]. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets L2C_TAD(0)_INT_W1C[WRDISOCI]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_l2c_tadx_int_w1s_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[WRDISOCI]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[RDDISOCI]. */
        uint64_t reserved_19_33        : 15;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[GSYNCTO]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[LFBTO]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[WRDISLMC]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[RDDISLMC]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[RDNXM]. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[WRNXM]. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[NOWAY]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[TAGDBE]. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[TAGSBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[FBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[FBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[SBFDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[SBFSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[L2DDBE]. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[L2DSBE]. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[L2DSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[L2DDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[SBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[SBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[FBFSBE]. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[FBFDBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[TAGSBE]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[TAGDBE]. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[NOWAY]. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[WRNXM]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[RDNXM]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[RDDISLMC]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[WRDISLMC]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[LFBTO]. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[GSYNCTO]. */
        uint64_t reserved_19_33        : 15;
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[RDDISOCI]. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets L2C_TAD(0..3)_INT_W1C[WRDISOCI]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn83xx;
    struct bdk_l2c_tadx_int_w1s_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t reserved_19_31        : 13;
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[GSYNCTO]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t reserved_11_12        : 2;
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
#else /* Word 0 - Little Endian */
        uint64_t l2dsbe                : 1;  /**< [  0:  0](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[L2DSBE]. */
        uint64_t l2ddbe                : 1;  /**< [  1:  1](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[L2DDBE]. */
        uint64_t sbfsbe                : 1;  /**< [  2:  2](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[SBFSBE]. */
        uint64_t sbfdbe                : 1;  /**< [  3:  3](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[SBFDBE]. */
        uint64_t fbfsbe                : 1;  /**< [  4:  4](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[FBFSBE]. */
        uint64_t fbfdbe                : 1;  /**< [  5:  5](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[FBFDBE]. */
        uint64_t reserved_6_7          : 2;
        uint64_t tagsbe                : 1;  /**< [  8:  8](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[TAGSBE]. */
        uint64_t tagdbe                : 1;  /**< [  9:  9](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[TAGDBE]. */
        uint64_t noway                 : 1;  /**< [ 10: 10](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[NOWAY]. */
        uint64_t reserved_11_12        : 2;
        uint64_t wrnxm                 : 1;  /**< [ 13: 13](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRNXM]. */
        uint64_t rdnxm                 : 1;  /**< [ 14: 14](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDNXM]. */
        uint64_t rddislmc              : 1;  /**< [ 15: 15](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDDISLMC]. */
        uint64_t wrdislmc              : 1;  /**< [ 16: 16](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRDISLMC]. */
        uint64_t lfbto                 : 1;  /**< [ 17: 17](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[LFBTO]. */
        uint64_t gsyncto               : 1;  /**< [ 18: 18](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[GSYNCTO]. */
        uint64_t reserved_19_31        : 13;
        uint64_t rtgsbe                : 1;  /**< [ 32: 32](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RTGSBE]. */
        uint64_t rtgdbe                : 1;  /**< [ 33: 33](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RTGDBE]. */
        uint64_t rddisoci              : 1;  /**< [ 34: 34](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[RDDISOCI]. */
        uint64_t wrdisoci              : 1;  /**< [ 35: 35](R/W1S/H) Reads or sets L2C_TAD(0..7)_INT_W1C[WRDISOCI]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_l2c_tadx_int_w1s bdk_l2c_tadx_int_w1s_t;

static inline uint64_t BDK_L2C_TADX_INT_W1S(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_INT_W1S(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050040008ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050040008ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050040008ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_INT_W1S", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_INT_W1S(a) bdk_l2c_tadx_int_w1s_t
#define bustype_BDK_L2C_TADX_INT_W1S(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_INT_W1S(a) "L2C_TADX_INT_W1S"
#define device_bar_BDK_L2C_TADX_INT_W1S(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_INT_W1S(a) (a)
#define arguments_BDK_L2C_TADX_INT_W1S(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad#_msix_pba#
 *
 * L2C TAD MSI-X Pending Bit Array Registers
 * This register is the MSI-X PBA table; the bit number is indexed by the L2C_TAD_INT_VEC_E
 * enumeration.
 */
union bdk_l2c_tadx_msix_pbax
{
    uint64_t u;
    struct bdk_l2c_tadx_msix_pbax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO) Pending message for the associated L2C_TAD()_MSIX_VEC()_CTL, enumerated by
                                                                 L2C_TAD_INT_VEC_E. Bits
                                                                 that have no associated L2C_TAD_INT_VEC_E are 0. */
#else /* Word 0 - Little Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO) Pending message for the associated L2C_TAD()_MSIX_VEC()_CTL, enumerated by
                                                                 L2C_TAD_INT_VEC_E. Bits
                                                                 that have no associated L2C_TAD_INT_VEC_E are 0. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_tadx_msix_pbax_s cn; */
};
typedef union bdk_l2c_tadx_msix_pbax bdk_l2c_tadx_msix_pbax_t;

static inline uint64_t BDK_L2C_TADX_MSIX_PBAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_MSIX_PBAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b==0)))
        return 0x87e050ff0000ll + 0x1000000ll * ((a) & 0x0) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b==0)))
        return 0x87e050ff0000ll + 0x1000000ll * ((a) & 0x3) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=7) && (b==0)))
        return 0x87e050ff0000ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x0);
    __bdk_csr_fatal("L2C_TADX_MSIX_PBAX", 2, a, b, 0, 0);
}

#define typedef_BDK_L2C_TADX_MSIX_PBAX(a,b) bdk_l2c_tadx_msix_pbax_t
#define bustype_BDK_L2C_TADX_MSIX_PBAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_MSIX_PBAX(a,b) "L2C_TADX_MSIX_PBAX"
#define device_bar_BDK_L2C_TADX_MSIX_PBAX(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_L2C_TADX_MSIX_PBAX(a,b) (a)
#define arguments_BDK_L2C_TADX_MSIX_PBAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) l2c_tad#_msix_vec#_addr
 *
 * L2C TAD MSI-X Vector-Table Address Register
 * This register is the MSI-X vector table, indexed by the L2C_TAD_INT_VEC_E enumeration.
 */
union bdk_l2c_tadx_msix_vecx_addr
{
    uint64_t u;
    struct bdk_l2c_tadx_msix_vecx_addr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's L2C_TAD()_MSIX_VEC()_ADDR, L2C_TAD()_MSIX_VEC()_CTL, and corresponding
                                                                 bit of L2C_TAD()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_L2C_TAD_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is
                                                                 set, all vectors are secure and function as if [SECVEC] was set. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's L2C_TAD()_MSIX_VEC()_ADDR, L2C_TAD()_MSIX_VEC()_CTL, and corresponding
                                                                 bit of L2C_TAD()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_L2C_TAD_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is
                                                                 set, all vectors are secure and function as if [SECVEC] was set. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_tadx_msix_vecx_addr_s cn; */
};
typedef union bdk_l2c_tadx_msix_vecx_addr bdk_l2c_tadx_msix_vecx_addr_t;

static inline uint64_t BDK_L2C_TADX_MSIX_VECX_ADDR(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_MSIX_VECX_ADDR(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b==0)))
        return 0x87e050f00000ll + 0x1000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b==0)))
        return 0x87e050f00000ll + 0x1000000ll * ((a) & 0x3) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=7) && (b==0)))
        return 0x87e050f00000ll + 0x1000000ll * ((a) & 0x7) + 0x10ll * ((b) & 0x0);
    __bdk_csr_fatal("L2C_TADX_MSIX_VECX_ADDR", 2, a, b, 0, 0);
}

#define typedef_BDK_L2C_TADX_MSIX_VECX_ADDR(a,b) bdk_l2c_tadx_msix_vecx_addr_t
#define bustype_BDK_L2C_TADX_MSIX_VECX_ADDR(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_MSIX_VECX_ADDR(a,b) "L2C_TADX_MSIX_VECX_ADDR"
#define device_bar_BDK_L2C_TADX_MSIX_VECX_ADDR(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_L2C_TADX_MSIX_VECX_ADDR(a,b) (a)
#define arguments_BDK_L2C_TADX_MSIX_VECX_ADDR(a,b) (a),(b),-1,-1

/**
 * Register (RSL) l2c_tad#_msix_vec#_ctl
 *
 * L2C TAD MSI-X Vector-Table Control and Data Register
 * This register is the MSI-X vector table, indexed by the L2C_TAD_INT_VEC_E enumeration.
 */
union bdk_l2c_tadx_msix_vecx_ctl
{
    uint64_t u;
    struct bdk_l2c_tadx_msix_vecx_ctl_s
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
    /* struct bdk_l2c_tadx_msix_vecx_ctl_s cn; */
};
typedef union bdk_l2c_tadx_msix_vecx_ctl bdk_l2c_tadx_msix_vecx_ctl_t;

static inline uint64_t BDK_L2C_TADX_MSIX_VECX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_MSIX_VECX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a==0) && (b==0)))
        return 0x87e050f00008ll + 0x1000000ll * ((a) & 0x0) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=3) && (b==0)))
        return 0x87e050f00008ll + 0x1000000ll * ((a) & 0x3) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=7) && (b==0)))
        return 0x87e050f00008ll + 0x1000000ll * ((a) & 0x7) + 0x10ll * ((b) & 0x0);
    __bdk_csr_fatal("L2C_TADX_MSIX_VECX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_L2C_TADX_MSIX_VECX_CTL(a,b) bdk_l2c_tadx_msix_vecx_ctl_t
#define bustype_BDK_L2C_TADX_MSIX_VECX_CTL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_MSIX_VECX_CTL(a,b) "L2C_TADX_MSIX_VECX_CTL"
#define device_bar_BDK_L2C_TADX_MSIX_VECX_CTL(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_L2C_TADX_MSIX_VECX_CTL(a,b) (a)
#define arguments_BDK_L2C_TADX_MSIX_VECX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) l2c_tad#_rtg_err
 *
 * Level 2 Cache TAD RTG Error Information Registers
 * This register records error information for all RTG SBE/DBE errors.
 * The priority of errors (lowest to highest) is SBE, DBE. An error locks [SYN], [WAY],
 * and [L2IDX] for equal or lower priority errors until cleared by software.
 * The syndrome is recorded for DBE errors, though the utility of the value is not clear.
 * [L2IDX]\<19:7\> is the L2 block index associated with the command which had no way to allocate.
 */
union bdk_l2c_tadx_rtg_err
{
    uint64_t u;
    struct bdk_l2c_tadx_rtg_err_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t rtgdbe                : 1;  /**< [ 63: 63](RO/H) Information refers to a double-bit RTG ECC error. */
        uint64_t rtgsbe                : 1;  /**< [ 62: 62](RO/H) Information refers to a single-bit RTG ECC error. */
        uint64_t reserved_39_61        : 23;
        uint64_t syn                   : 7;  /**< [ 38: 32](RO/H) Syndrome for the single-bit error. */
        uint64_t reserved_24_31        : 8;
        uint64_t way                   : 4;  /**< [ 23: 20](RO/H) Way of the L2 block containing the error. */
        uint64_t l2idx                 : 13; /**< [ 19:  7](RO/H) Index of the L2 block containing the error.
                                                                 See L2C_TAD()_INT_W1C[RTGSBE] for an important use of this field. */
        uint64_t reserved_0_6          : 7;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_6          : 7;
        uint64_t l2idx                 : 13; /**< [ 19:  7](RO/H) Index of the L2 block containing the error.
                                                                 See L2C_TAD()_INT_W1C[RTGSBE] for an important use of this field. */
        uint64_t way                   : 4;  /**< [ 23: 20](RO/H) Way of the L2 block containing the error. */
        uint64_t reserved_24_31        : 8;
        uint64_t syn                   : 7;  /**< [ 38: 32](RO/H) Syndrome for the single-bit error. */
        uint64_t reserved_39_61        : 23;
        uint64_t rtgsbe                : 1;  /**< [ 62: 62](RO/H) Information refers to a single-bit RTG ECC error. */
        uint64_t rtgdbe                : 1;  /**< [ 63: 63](RO/H) Information refers to a double-bit RTG ECC error. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_tadx_rtg_err_s cn; */
};
typedef union bdk_l2c_tadx_rtg_err bdk_l2c_tadx_rtg_err_t;

static inline uint64_t BDK_L2C_TADX_RTG_ERR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_RTG_ERR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS2_X) && (a<=7))
        return 0x87e050060300ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_RTG_ERR", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_RTG_ERR(a) bdk_l2c_tadx_rtg_err_t
#define bustype_BDK_L2C_TADX_RTG_ERR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_RTG_ERR(a) "L2C_TADX_RTG_ERR"
#define device_bar_BDK_L2C_TADX_RTG_ERR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_RTG_ERR(a) (a)
#define arguments_BDK_L2C_TADX_RTG_ERR(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad#_tbf_bist_status
 *
 * L2C TAD Quad Buffer BIST Status Registers
 */
union bdk_l2c_tadx_tbf_bist_status
{
    uint64_t u;
    struct bdk_l2c_tadx_tbf_bist_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t vbffl                 : 16; /**< [ 63: 48](RO/H) BIST failure status for VBF ({QD7H1,QD7H0, ... , QD0H1, QD0H0}). */
        uint64_t sbffl                 : 16; /**< [ 47: 32](RO/H) BIST failure status for SBF ({QD7H1,QD7H0, ... , QD0H1, QD0H0}). */
        uint64_t fbfrspfl              : 16; /**< [ 31: 16](RO/H) BIST failure status for FBF RSP port ({QD7H1,QD7H0, ... , QD0H1, QD0H0}). */
        uint64_t fbfwrpfl              : 16; /**< [ 15:  0](RO/H) BIST failure status for FBF WRP port ({QD7H1,QD7H0, ... , QD0H1, QD0H0}). */
#else /* Word 0 - Little Endian */
        uint64_t fbfwrpfl              : 16; /**< [ 15:  0](RO/H) BIST failure status for FBF WRP port ({QD7H1,QD7H0, ... , QD0H1, QD0H0}). */
        uint64_t fbfrspfl              : 16; /**< [ 31: 16](RO/H) BIST failure status for FBF RSP port ({QD7H1,QD7H0, ... , QD0H1, QD0H0}). */
        uint64_t sbffl                 : 16; /**< [ 47: 32](RO/H) BIST failure status for SBF ({QD7H1,QD7H0, ... , QD0H1, QD0H0}). */
        uint64_t vbffl                 : 16; /**< [ 63: 48](RO/H) BIST failure status for VBF ({QD7H1,QD7H0, ... , QD0H1, QD0H0}). */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_tadx_tbf_bist_status_s cn; */
};
typedef union bdk_l2c_tadx_tbf_bist_status bdk_l2c_tadx_tbf_bist_status_t;

static inline uint64_t BDK_L2C_TADX_TBF_BIST_STATUS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_TBF_BIST_STATUS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050070000ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050070000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050070000ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_TBF_BIST_STATUS", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_TBF_BIST_STATUS(a) bdk_l2c_tadx_tbf_bist_status_t
#define bustype_BDK_L2C_TADX_TBF_BIST_STATUS(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_TBF_BIST_STATUS(a) "L2C_TADX_TBF_BIST_STATUS"
#define device_bar_BDK_L2C_TADX_TBF_BIST_STATUS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_TBF_BIST_STATUS(a) (a)
#define arguments_BDK_L2C_TADX_TBF_BIST_STATUS(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad#_tdt_bist_status
 *
 * L2C TAD Data BIST Status Registers
 */
union bdk_l2c_tadx_tdt_bist_status
{
    uint64_t u;
    struct bdk_l2c_tadx_tdt_bist_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t l2dfl                 : 16; /**< [ 15:  0](RO/H) BIST failure status for L2D ({QD7H1,QD7H0, ... , QD0H1, QD0H0}). */
#else /* Word 0 - Little Endian */
        uint64_t l2dfl                 : 16; /**< [ 15:  0](RO/H) BIST failure status for L2D ({QD7H1,QD7H0, ... , QD0H1, QD0H0}). */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_tadx_tdt_bist_status_s cn; */
};
typedef union bdk_l2c_tadx_tdt_bist_status bdk_l2c_tadx_tdt_bist_status_t;

static inline uint64_t BDK_L2C_TADX_TDT_BIST_STATUS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_TDT_BIST_STATUS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050070100ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050070100ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050070100ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_TDT_BIST_STATUS", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_TDT_BIST_STATUS(a) bdk_l2c_tadx_tdt_bist_status_t
#define bustype_BDK_L2C_TADX_TDT_BIST_STATUS(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_TDT_BIST_STATUS(a) "L2C_TADX_TDT_BIST_STATUS"
#define device_bar_BDK_L2C_TADX_TDT_BIST_STATUS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_TDT_BIST_STATUS(a) (a)
#define arguments_BDK_L2C_TADX_TDT_BIST_STATUS(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad#_tqd_err
 *
 * L2C TAD Quad Error Information Registers
 * This register records error information for all L2D/SBF/FBF errors.
 * An error locks the [L2DIDX] and [SYN] fields and sets the bit corresponding to the error
 * received.
 * DBE errors take priority and overwrite an earlier logged SBE error. Only one of SBE/DBE is set
 * at any given time and serves to document which error the [L2DIDX]/[SYN] is associated with.
 * The syndrome is recorded for DBE errors, though the utility of the value is not clear.
 */
union bdk_l2c_tadx_tqd_err
{
    uint64_t u;
    struct bdk_l2c_tadx_tqd_err_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t l2ddbe                : 1;  /**< [ 63: 63](RO/H) L2DIDX/SYN corresponds to a double-bit L2D ECC error. */
        uint64_t sbfdbe                : 1;  /**< [ 62: 62](RO/H) L2DIDX/SYN corresponds to a double-bit SBF ECC error. */
        uint64_t fbfdbe                : 1;  /**< [ 61: 61](RO/H) L2DIDX/SYN corresponds to a double-bit FBF ECC error. */
        uint64_t l2dsbe                : 1;  /**< [ 60: 60](RO/H) L2DIDX/SYN corresponds to a single-bit L2D ECC error. */
        uint64_t sbfsbe                : 1;  /**< [ 59: 59](RO/H) L2DIDX/SYN corresponds to a single-bit SBF ECC error. */
        uint64_t fbfsbe                : 1;  /**< [ 58: 58](RO/H) L2DIDX/SYN corresponds to a single-bit FBF ECC error. */
        uint64_t reserved_40_57        : 18;
        uint64_t syn                   : 8;  /**< [ 39: 32](RO/H) Error syndrome. */
        uint64_t reserved_18_31        : 14;
        uint64_t qdnum                 : 3;  /**< [ 17: 15](RO/H) Quad containing the error. */
        uint64_t qdhlf                 : 1;  /**< [ 14: 14](RO/H) Quad half of the containing the error. */
        uint64_t l2didx                : 14; /**< [ 13:  0](RO/H) For L2D errors, index within the quad-half containing the error. For SBF and FBF errors
                                                                 \<13:5\> is 0x0 and \<4:0\> is the index of the error (\<4:1\> is lfbnum\<3:0\>, \<0\> is addr\<5\>).
                                                                 See L2C_TAD()_INT_W1C[L2DSBE] for an important use of this field. */
#else /* Word 0 - Little Endian */
        uint64_t l2didx                : 14; /**< [ 13:  0](RO/H) For L2D errors, index within the quad-half containing the error. For SBF and FBF errors
                                                                 \<13:5\> is 0x0 and \<4:0\> is the index of the error (\<4:1\> is lfbnum\<3:0\>, \<0\> is addr\<5\>).
                                                                 See L2C_TAD()_INT_W1C[L2DSBE] for an important use of this field. */
        uint64_t qdhlf                 : 1;  /**< [ 14: 14](RO/H) Quad half of the containing the error. */
        uint64_t qdnum                 : 3;  /**< [ 17: 15](RO/H) Quad containing the error. */
        uint64_t reserved_18_31        : 14;
        uint64_t syn                   : 8;  /**< [ 39: 32](RO/H) Error syndrome. */
        uint64_t reserved_40_57        : 18;
        uint64_t fbfsbe                : 1;  /**< [ 58: 58](RO/H) L2DIDX/SYN corresponds to a single-bit FBF ECC error. */
        uint64_t sbfsbe                : 1;  /**< [ 59: 59](RO/H) L2DIDX/SYN corresponds to a single-bit SBF ECC error. */
        uint64_t l2dsbe                : 1;  /**< [ 60: 60](RO/H) L2DIDX/SYN corresponds to a single-bit L2D ECC error. */
        uint64_t fbfdbe                : 1;  /**< [ 61: 61](RO/H) L2DIDX/SYN corresponds to a double-bit FBF ECC error. */
        uint64_t sbfdbe                : 1;  /**< [ 62: 62](RO/H) L2DIDX/SYN corresponds to a double-bit SBF ECC error. */
        uint64_t l2ddbe                : 1;  /**< [ 63: 63](RO/H) L2DIDX/SYN corresponds to a double-bit L2D ECC error. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_l2c_tadx_tqd_err_s cn; */
};
typedef union bdk_l2c_tadx_tqd_err bdk_l2c_tadx_tqd_err_t;

static inline uint64_t BDK_L2C_TADX_TQD_ERR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_TQD_ERR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050060100ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050060100ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050060100ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_TQD_ERR", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_TQD_ERR(a) bdk_l2c_tadx_tqd_err_t
#define bustype_BDK_L2C_TADX_TQD_ERR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_TQD_ERR(a) "L2C_TADX_TQD_ERR"
#define device_bar_BDK_L2C_TADX_TQD_ERR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_TQD_ERR(a) (a)
#define arguments_BDK_L2C_TADX_TQD_ERR(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad#_ttg_bist_status
 *
 * L2C TAD Tag BIST Status Registers
 */
union bdk_l2c_tadx_ttg_bist_status
{
    uint64_t u;
    struct bdk_l2c_tadx_ttg_bist_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_50_63        : 14;
        uint64_t xmdmskfl              : 2;  /**< [ 49: 48](RO/H) BIST failure status for RSTP XMDMSK memories. */
        uint64_t rtgfl                 : 16; /**< [ 47: 32](RO/H) BIST failure status for RTG ways. */
        uint64_t reserved_18_31        : 14;
        uint64_t lrulfbfl              : 1;  /**< [ 17: 17](RO) Reserved, always zero. */
        uint64_t lrufl                 : 1;  /**< [ 16: 16](RO/H) BIST failure status for tag LRU. */
        uint64_t tagfl                 : 16; /**< [ 15:  0](RO/H) BIST failure status for TAG ways. */
#else /* Word 0 - Little Endian */
        uint64_t tagfl                 : 16; /**< [ 15:  0](RO/H) BIST failure status for TAG ways. */
        uint64_t lrufl                 : 1;  /**< [ 16: 16](RO/H) BIST failure status for tag LRU. */
        uint64_t lrulfbfl              : 1;  /**< [ 17: 17](RO) Reserved, always zero. */
        uint64_t reserved_18_31        : 14;
        uint64_t rtgfl                 : 16; /**< [ 47: 32](RO/H) BIST failure status for RTG ways. */
        uint64_t xmdmskfl              : 2;  /**< [ 49: 48](RO/H) BIST failure status for RSTP XMDMSK memories. */
        uint64_t reserved_50_63        : 14;
#endif /* Word 0 - End */
    } s;
    struct bdk_l2c_tadx_ttg_bist_status_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_50_63        : 14;
        uint64_t xmdmskfl              : 2;  /**< [ 49: 48](RO/H) Reserved, always zero. */
        uint64_t reserved_18_47        : 30;
        uint64_t lrulfbfl              : 1;  /**< [ 17: 17](RO) Reserved, always zero. */
        uint64_t lrufl                 : 1;  /**< [ 16: 16](RO/H) BIST failure status for tag LRU. */
        uint64_t tagfl                 : 16; /**< [ 15:  0](RO/H) BIST failure status for TAG ways. */
#else /* Word 0 - Little Endian */
        uint64_t tagfl                 : 16; /**< [ 15:  0](RO/H) BIST failure status for TAG ways. */
        uint64_t lrufl                 : 1;  /**< [ 16: 16](RO/H) BIST failure status for tag LRU. */
        uint64_t lrulfbfl              : 1;  /**< [ 17: 17](RO) Reserved, always zero. */
        uint64_t reserved_18_47        : 30;
        uint64_t xmdmskfl              : 2;  /**< [ 49: 48](RO/H) Reserved, always zero. */
        uint64_t reserved_50_63        : 14;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_l2c_tadx_ttg_bist_status_s cn88xx; */
    struct bdk_l2c_tadx_ttg_bist_status_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_50_63        : 14;
        uint64_t xmdmskfl              : 2;  /**< [ 49: 48](RO/H) BIST failure status for RSTP XMDMSK memories. */
        uint64_t reserved_18_47        : 30;
        uint64_t lrulfbfl              : 1;  /**< [ 17: 17](RO) Reserved, always zero. */
        uint64_t lrufl                 : 1;  /**< [ 16: 16](RO/H) BIST failure status for tag LRU. */
        uint64_t tagfl                 : 16; /**< [ 15:  0](RO/H) BIST failure status for TAG ways. */
#else /* Word 0 - Little Endian */
        uint64_t tagfl                 : 16; /**< [ 15:  0](RO/H) BIST failure status for TAG ways. */
        uint64_t lrufl                 : 1;  /**< [ 16: 16](RO/H) BIST failure status for tag LRU. */
        uint64_t lrulfbfl              : 1;  /**< [ 17: 17](RO) Reserved, always zero. */
        uint64_t reserved_18_47        : 30;
        uint64_t xmdmskfl              : 2;  /**< [ 49: 48](RO/H) BIST failure status for RSTP XMDMSK memories. */
        uint64_t reserved_50_63        : 14;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_l2c_tadx_ttg_bist_status bdk_l2c_tadx_ttg_bist_status_t;

static inline uint64_t BDK_L2C_TADX_TTG_BIST_STATUS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_TTG_BIST_STATUS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050070200ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050070200ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050070200ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_TTG_BIST_STATUS", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_TTG_BIST_STATUS(a) bdk_l2c_tadx_ttg_bist_status_t
#define bustype_BDK_L2C_TADX_TTG_BIST_STATUS(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_TTG_BIST_STATUS(a) "L2C_TADX_TTG_BIST_STATUS"
#define device_bar_BDK_L2C_TADX_TTG_BIST_STATUS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_TTG_BIST_STATUS(a) (a)
#define arguments_BDK_L2C_TADX_TTG_BIST_STATUS(a) (a),-1,-1,-1

/**
 * Register (RSL) l2c_tad#_ttg_err
 *
 * L2C TAD Tag Error Information Registers
 * This register records error information for all TAG SBE/DBE/NOWAY errors.
 * The priority of errors (lowest to highest) is NOWAY, SBE, DBE. An error locks [SYN], [WAY],
 * and [L2IDX] for equal or lower priority errors until cleared by software.
 * The syndrome is recorded for DBE errors, though the utility of the value is not clear.
 * A NOWAY error does not change the value of the [SYN] field, and leaves [WAY] unpredictable.
 * [L2IDX]\<19:7\> is the L2 block index associated with the command which had no way to allocate.
 */
union bdk_l2c_tadx_ttg_err
{
    uint64_t u;
    struct bdk_l2c_tadx_ttg_err_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t tagdbe                : 1;  /**< [ 63: 63](RO/H) Information refers to a double-bit TAG ECC error. */
        uint64_t tagsbe                : 1;  /**< [ 62: 62](RO/H) Information refers to a single-bit TAG ECC error. */
        uint64_t noway                 : 1;  /**< [ 61: 61](RO/H) Information refers to a NOWAY error. */
        uint64_t reserved_39_60        : 22;
        uint64_t syn                   : 7;  /**< [ 38: 32](RO/H) Syndrome for the single-bit error. */
        uint64_t reserved_0_31         : 32;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_31         : 32;
        uint64_t syn                   : 7;  /**< [ 38: 32](RO/H) Syndrome for the single-bit error. */
        uint64_t reserved_39_60        : 22;
        uint64_t noway                 : 1;  /**< [ 61: 61](RO/H) Information refers to a NOWAY error. */
        uint64_t tagsbe                : 1;  /**< [ 62: 62](RO/H) Information refers to a single-bit TAG ECC error. */
        uint64_t tagdbe                : 1;  /**< [ 63: 63](RO/H) Information refers to a double-bit TAG ECC error. */
#endif /* Word 0 - End */
    } s;
    struct bdk_l2c_tadx_ttg_err_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t tagdbe                : 1;  /**< [ 63: 63](RO/H) Information refers to a double-bit TAG ECC error. */
        uint64_t tagsbe                : 1;  /**< [ 62: 62](RO/H) Information refers to a single-bit TAG ECC error. */
        uint64_t noway                 : 1;  /**< [ 61: 61](RO/H) Information refers to a NOWAY error. */
        uint64_t reserved_39_60        : 22;
        uint64_t syn                   : 7;  /**< [ 38: 32](RO/H) Syndrome for the single-bit error. */
        uint64_t reserved_21_31        : 11;
        uint64_t way                   : 4;  /**< [ 20: 17](RO/H) Way of the L2 block containing the error. */
        uint64_t l2idx                 : 10; /**< [ 16:  7](RO/H) Index of the L2 block containing the error.
                                                                 See L2C_TAD()_INT_W1C[TAGSBE] for an important use of this field. */
        uint64_t reserved_0_6          : 7;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_6          : 7;
        uint64_t l2idx                 : 10; /**< [ 16:  7](RO/H) Index of the L2 block containing the error.
                                                                 See L2C_TAD()_INT_W1C[TAGSBE] for an important use of this field. */
        uint64_t way                   : 4;  /**< [ 20: 17](RO/H) Way of the L2 block containing the error. */
        uint64_t reserved_21_31        : 11;
        uint64_t syn                   : 7;  /**< [ 38: 32](RO/H) Syndrome for the single-bit error. */
        uint64_t reserved_39_60        : 22;
        uint64_t noway                 : 1;  /**< [ 61: 61](RO/H) Information refers to a NOWAY error. */
        uint64_t tagsbe                : 1;  /**< [ 62: 62](RO/H) Information refers to a single-bit TAG ECC error. */
        uint64_t tagdbe                : 1;  /**< [ 63: 63](RO/H) Information refers to a double-bit TAG ECC error. */
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_l2c_tadx_ttg_err_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t tagdbe                : 1;  /**< [ 63: 63](RO/H) Information refers to a double-bit TAG ECC error. */
        uint64_t tagsbe                : 1;  /**< [ 62: 62](RO/H) Information refers to a single-bit TAG ECC error. */
        uint64_t noway                 : 1;  /**< [ 61: 61](RO/H) Information refers to a NOWAY error. */
        uint64_t reserved_39_60        : 22;
        uint64_t syn                   : 7;  /**< [ 38: 32](RO/H) Syndrome for the single-bit error. */
        uint64_t reserved_24_31        : 8;
        uint64_t way                   : 4;  /**< [ 23: 20](RO/H) Way of the L2 block containing the error. */
        uint64_t l2idx                 : 13; /**< [ 19:  7](RO/H) Index of the L2 block containing the error.
                                                                 See L2C_TAD()_INT_W1C[TAGSBE] for an important use of this field. */
        uint64_t reserved_0_6          : 7;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_6          : 7;
        uint64_t l2idx                 : 13; /**< [ 19:  7](RO/H) Index of the L2 block containing the error.
                                                                 See L2C_TAD()_INT_W1C[TAGSBE] for an important use of this field. */
        uint64_t way                   : 4;  /**< [ 23: 20](RO/H) Way of the L2 block containing the error. */
        uint64_t reserved_24_31        : 8;
        uint64_t syn                   : 7;  /**< [ 38: 32](RO/H) Syndrome for the single-bit error. */
        uint64_t reserved_39_60        : 22;
        uint64_t noway                 : 1;  /**< [ 61: 61](RO/H) Information refers to a NOWAY error. */
        uint64_t tagsbe                : 1;  /**< [ 62: 62](RO/H) Information refers to a single-bit TAG ECC error. */
        uint64_t tagdbe                : 1;  /**< [ 63: 63](RO/H) Information refers to a double-bit TAG ECC error. */
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_l2c_tadx_ttg_err_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t tagdbe                : 1;  /**< [ 63: 63](RO/H) Information refers to a double-bit TAG ECC error. */
        uint64_t tagsbe                : 1;  /**< [ 62: 62](RO/H) Information refers to a single-bit TAG ECC error. */
        uint64_t noway                 : 1;  /**< [ 61: 61](RO/H) Information refers to a NOWAY error. */
        uint64_t reserved_39_60        : 22;
        uint64_t syn                   : 7;  /**< [ 38: 32](RO/H) Syndrome for the single-bit error. */
        uint64_t reserved_23_31        : 9;
        uint64_t way                   : 4;  /**< [ 22: 19](RO/H) Way of the L2 block containing the error. */
        uint64_t l2idx                 : 12; /**< [ 18:  7](RO/H) Index of the L2 block containing the error.
                                                                 See L2C_TAD()_INT_W1C[TAGSBE] for an important use of this field. */
        uint64_t reserved_0_6          : 7;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_6          : 7;
        uint64_t l2idx                 : 12; /**< [ 18:  7](RO/H) Index of the L2 block containing the error.
                                                                 See L2C_TAD()_INT_W1C[TAGSBE] for an important use of this field. */
        uint64_t way                   : 4;  /**< [ 22: 19](RO/H) Way of the L2 block containing the error. */
        uint64_t reserved_23_31        : 9;
        uint64_t syn                   : 7;  /**< [ 38: 32](RO/H) Syndrome for the single-bit error. */
        uint64_t reserved_39_60        : 22;
        uint64_t noway                 : 1;  /**< [ 61: 61](RO/H) Information refers to a NOWAY error. */
        uint64_t tagsbe                : 1;  /**< [ 62: 62](RO/H) Information refers to a single-bit TAG ECC error. */
        uint64_t tagdbe                : 1;  /**< [ 63: 63](RO/H) Information refers to a double-bit TAG ECC error. */
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_l2c_tadx_ttg_err bdk_l2c_tadx_ttg_err_t;

static inline uint64_t BDK_L2C_TADX_TTG_ERR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_L2C_TADX_TTG_ERR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a==0))
        return 0x87e050060200ll + 0x1000000ll * ((a) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=3))
        return 0x87e050060200ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=7))
        return 0x87e050060200ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("L2C_TADX_TTG_ERR", 1, a, 0, 0, 0);
}

#define typedef_BDK_L2C_TADX_TTG_ERR(a) bdk_l2c_tadx_ttg_err_t
#define bustype_BDK_L2C_TADX_TTG_ERR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_L2C_TADX_TTG_ERR(a) "L2C_TADX_TTG_ERR"
#define device_bar_BDK_L2C_TADX_TTG_ERR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_L2C_TADX_TTG_ERR(a) (a)
#define arguments_BDK_L2C_TADX_TTG_ERR(a) (a),-1,-1,-1

#endif /* __BDK_CSRS_L2C_TAD_H__ */
