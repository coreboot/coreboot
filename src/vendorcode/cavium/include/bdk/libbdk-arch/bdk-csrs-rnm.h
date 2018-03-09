#ifndef __BDK_CSRS_RNM_H__
#define __BDK_CSRS_RNM_H__
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
 * Cavium RNM.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration rnm_bar_e
 *
 * RNM Base Address Register Enumeration
 * Enumerates the base address registers.
 */
#define BDK_RNM_BAR_E_RNM_PF_BAR0 (0x87e040000000ll)
#define BDK_RNM_BAR_E_RNM_PF_BAR0_SIZE 0x100000ull
#define BDK_RNM_BAR_E_RNM_VF_BAR0 (0x840000800000ll)
#define BDK_RNM_BAR_E_RNM_VF_BAR0_SIZE 0x100000ull

/**
 * Register (RSL) rnm_bist_status
 *
 * RNM BIST Status Register
 * This register is the RNM memory BIST status register, indicating status of built-in self-
 * tests.  0 = passed BIST, 1 = failed BIST.
 */
union bdk_rnm_bist_status
{
    uint64_t u;
    struct bdk_rnm_bist_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_0_63         : 64;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_63         : 64;
#endif /* Word 0 - End */
    } s;
    struct bdk_rnm_bist_status_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t nz                    : 1;  /**< [  1:  1](RO/H) Status of the no-zeros memory BIST. 0 = passed BIST, 1 = failed BIST. */
        uint64_t nml                   : 1;  /**< [  0:  0](RO/H) Status of the normal memory BIST. 0 = passed BIST, 1 = failed BIST. */
#else /* Word 0 - Little Endian */
        uint64_t nml                   : 1;  /**< [  0:  0](RO/H) Status of the normal memory BIST. 0 = passed BIST, 1 = failed BIST. */
        uint64_t nz                    : 1;  /**< [  1:  1](RO/H) Status of the no-zeros memory BIST. 0 = passed BIST, 1 = failed BIST. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_rnm_bist_status_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t rrc                   : 1;  /**< [  1:  1](RO/H) Status of the RRC memory block BIST. 0 = passed BIST, 1 = failed BIST. */
        uint64_t mem                   : 1;  /**< [  0:  0](RO/H) Status of MEM memory block BIST. 0 = passed BIST, 1 = failed BIST. */
#else /* Word 0 - Little Endian */
        uint64_t mem                   : 1;  /**< [  0:  0](RO/H) Status of MEM memory block BIST. 0 = passed BIST, 1 = failed BIST. */
        uint64_t rrc                   : 1;  /**< [  1:  1](RO/H) Status of the RRC memory block BIST. 0 = passed BIST, 1 = failed BIST. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } cn88xx;
    /* struct bdk_rnm_bist_status_cn81xx cn83xx; */
};
typedef union bdk_rnm_bist_status bdk_rnm_bist_status_t;

#define BDK_RNM_BIST_STATUS BDK_RNM_BIST_STATUS_FUNC()
static inline uint64_t BDK_RNM_BIST_STATUS_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RNM_BIST_STATUS_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        return 0x87e040000008ll;
    __bdk_csr_fatal("RNM_BIST_STATUS", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RNM_BIST_STATUS bdk_rnm_bist_status_t
#define bustype_BDK_RNM_BIST_STATUS BDK_CSR_TYPE_RSL
#define basename_BDK_RNM_BIST_STATUS "RNM_BIST_STATUS"
#define device_bar_BDK_RNM_BIST_STATUS 0x0 /* PF_BAR0 */
#define busnum_BDK_RNM_BIST_STATUS 0
#define arguments_BDK_RNM_BIST_STATUS -1,-1,-1,-1

/**
 * Register (RSL) rnm_bp_test
 *
 * INTERNAL: RNM Backpressure Test Register
 */
union bdk_rnm_bp_test
{
    uint64_t u;
    struct bdk_rnm_bp_test_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t enable                : 3;  /**< [ 63: 61](R/W) Enable test mode. For diagnostic use only.
                                                                 Internal:
                                                                 Once a bit is set, random backpressure is generated
                                                                 at the corresponding point to allow for more frequent backpressure.
                                                                 \<63\> = Normal random number memory writes.
                                                                 \<62\> = No-zeros random number memory writes.
                                                                 \<61\> = No-zeros random number memory reads. */
        uint64_t reserved_24_60        : 37;
        uint64_t bp_cfg                : 6;  /**< [ 23: 18](R/W) Backpressure weight. For diagnostic use only.
                                                                 Internal:
                                                                 There are 2 backpressure configuration bits per enable, with the two bits
                                                                 defined as 0x0=100% of the time, 0x1=75% of the time, 0x2=50% of the time,
                                                                 0x3=25% of the time.
                                                                   \<23:22\> = Normal random number memory writes.
                                                                   \<21:20\> = No-zeros random number memory writes.
                                                                   \<19:18\> = No-zeros random number memory reads. */
        uint64_t reserved_12_17        : 6;
        uint64_t lfsr_freq             : 12; /**< [ 11:  0](R/W) Test LFSR update frequency in coprocessor-clocks minus one. */
#else /* Word 0 - Little Endian */
        uint64_t lfsr_freq             : 12; /**< [ 11:  0](R/W) Test LFSR update frequency in coprocessor-clocks minus one. */
        uint64_t reserved_12_17        : 6;
        uint64_t bp_cfg                : 6;  /**< [ 23: 18](R/W) Backpressure weight. For diagnostic use only.
                                                                 Internal:
                                                                 There are 2 backpressure configuration bits per enable, with the two bits
                                                                 defined as 0x0=100% of the time, 0x1=75% of the time, 0x2=50% of the time,
                                                                 0x3=25% of the time.
                                                                   \<23:22\> = Normal random number memory writes.
                                                                   \<21:20\> = No-zeros random number memory writes.
                                                                   \<19:18\> = No-zeros random number memory reads. */
        uint64_t reserved_24_60        : 37;
        uint64_t enable                : 3;  /**< [ 63: 61](R/W) Enable test mode. For diagnostic use only.
                                                                 Internal:
                                                                 Once a bit is set, random backpressure is generated
                                                                 at the corresponding point to allow for more frequent backpressure.
                                                                 \<63\> = Normal random number memory writes.
                                                                 \<62\> = No-zeros random number memory writes.
                                                                 \<61\> = No-zeros random number memory reads. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rnm_bp_test_s cn; */
};
typedef union bdk_rnm_bp_test bdk_rnm_bp_test_t;

#define BDK_RNM_BP_TEST BDK_RNM_BP_TEST_FUNC()
static inline uint64_t BDK_RNM_BP_TEST_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RNM_BP_TEST_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e040000028ll;
    __bdk_csr_fatal("RNM_BP_TEST", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RNM_BP_TEST bdk_rnm_bp_test_t
#define bustype_BDK_RNM_BP_TEST BDK_CSR_TYPE_RSL
#define basename_BDK_RNM_BP_TEST "RNM_BP_TEST"
#define device_bar_BDK_RNM_BP_TEST 0x0 /* PF_BAR0 */
#define busnum_BDK_RNM_BP_TEST 0
#define arguments_BDK_RNM_BP_TEST -1,-1,-1,-1

/**
 * Register (RSL) rnm_const
 *
 * RNM PF Constants Register
 * This register is used for software discovery.
 */
union bdk_rnm_const
{
    uint64_t u;
    struct bdk_rnm_const_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_0_63         : 64;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_63         : 64;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rnm_const_s cn; */
};
typedef union bdk_rnm_const bdk_rnm_const_t;

#define BDK_RNM_CONST BDK_RNM_CONST_FUNC()
static inline uint64_t BDK_RNM_CONST_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RNM_CONST_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX))
        return 0x87e040000030ll;
    __bdk_csr_fatal("RNM_CONST", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RNM_CONST bdk_rnm_const_t
#define bustype_BDK_RNM_CONST BDK_CSR_TYPE_RSL
#define basename_BDK_RNM_CONST "RNM_CONST"
#define device_bar_BDK_RNM_CONST 0x0 /* PF_BAR0 */
#define busnum_BDK_RNM_CONST 0
#define arguments_BDK_RNM_CONST -1,-1,-1,-1

/**
 * Register (RSL) rnm_ctl_status
 *
 * RNM Control and Status Register
 * This register is the RNM control register.
 * This register is secure only to prevent the nonsecure world from affecting
 * secure-world clients using true random numbers.
 */
union bdk_rnm_ctl_status
{
    uint64_t u;
    struct bdk_rnm_ctl_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_11_63        : 53;
        uint64_t eer_lck               : 1;  /**< [ 10: 10](SRO/H) Encryption enable register locked. */
        uint64_t eer_val               : 1;  /**< [  9:  9](SRO/H) Dormant encryption key match. */
        uint64_t ent_sel               : 4;  /**< [  8:  5](SR/W) Select input to RNM FIFO.
                                                                 0x0 = 0-7.
                                                                 0x1 = 8-15.
                                                                 0x2 = 16-23.
                                                                 0x3 = 24-31.
                                                                 0x4 = 32-39.
                                                                 0x5 = 40-47.
                                                                 0x6 = 48-55.
                                                                 0x7 = 56-63.
                                                                 0x8 = 64-71.
                                                                 0x9 = 72-79.
                                                                 0xA = 80-87.
                                                                 0xB = 88-95.
                                                                 0xC = 96-103.
                                                                 0xD = 104-111.
                                                                 0xE = 112-119.
                                                                 0xF = 120-127. */
        uint64_t exp_ent               : 1;  /**< [  4:  4](SR/W) Exported entropy enable for random number generator. The next random number is
                                                                 available 80 coprocessor-clock cycles after switching this bit from zero to one. The
                                                                 next random number is available 730 coprocessor-clock cycles after switching this
                                                                 bit from one to zero. */
        uint64_t rng_rst               : 1;  /**< [  3:  3](SR/W) Reset the RNG. Setting this bit to one cancels the generation of the current random
                                                                 number. The next random number is available 730 coprocessor-clock cycles after this
                                                                 bit is cleared if [EXP_ENT] is set to zero. The next random number is available 80
                                                                 coprocessor-clock cycles after this bit is cleared if [EXP_ENT] is set to one. This bit is
                                                                 not automatically cleared. */
        uint64_t rnm_rst               : 1;  /**< [  2:  2](SR/W) Reset the RNM. Setting this bit to 1 drops all RNM transactions in flight and clears
                                                                 all stored numbers in the random number memory. Any outstanding NCBO credits will
                                                                 not be returned. RNM will not respond to any pending NCBI grants. RNM can accept
                                                                 new requests immediately after reset is cleared. This bit is not automatically
                                                                 cleared and will not reset any CSR fields. */
        uint64_t rng_en                : 1;  /**< [  1:  1](SR/W) Enables the output of the RNG. */
        uint64_t ent_en                : 1;  /**< [  0:  0](SR/W) Entropy enable for random number generator. */
#else /* Word 0 - Little Endian */
        uint64_t ent_en                : 1;  /**< [  0:  0](SR/W) Entropy enable for random number generator. */
        uint64_t rng_en                : 1;  /**< [  1:  1](SR/W) Enables the output of the RNG. */
        uint64_t rnm_rst               : 1;  /**< [  2:  2](SR/W) Reset the RNM. Setting this bit to 1 drops all RNM transactions in flight and clears
                                                                 all stored numbers in the random number memory. Any outstanding NCBO credits will
                                                                 not be returned. RNM will not respond to any pending NCBI grants. RNM can accept
                                                                 new requests immediately after reset is cleared. This bit is not automatically
                                                                 cleared and will not reset any CSR fields. */
        uint64_t rng_rst               : 1;  /**< [  3:  3](SR/W) Reset the RNG. Setting this bit to one cancels the generation of the current random
                                                                 number. The next random number is available 730 coprocessor-clock cycles after this
                                                                 bit is cleared if [EXP_ENT] is set to zero. The next random number is available 80
                                                                 coprocessor-clock cycles after this bit is cleared if [EXP_ENT] is set to one. This bit is
                                                                 not automatically cleared. */
        uint64_t exp_ent               : 1;  /**< [  4:  4](SR/W) Exported entropy enable for random number generator. The next random number is
                                                                 available 80 coprocessor-clock cycles after switching this bit from zero to one. The
                                                                 next random number is available 730 coprocessor-clock cycles after switching this
                                                                 bit from one to zero. */
        uint64_t ent_sel               : 4;  /**< [  8:  5](SR/W) Select input to RNM FIFO.
                                                                 0x0 = 0-7.
                                                                 0x1 = 8-15.
                                                                 0x2 = 16-23.
                                                                 0x3 = 24-31.
                                                                 0x4 = 32-39.
                                                                 0x5 = 40-47.
                                                                 0x6 = 48-55.
                                                                 0x7 = 56-63.
                                                                 0x8 = 64-71.
                                                                 0x9 = 72-79.
                                                                 0xA = 80-87.
                                                                 0xB = 88-95.
                                                                 0xC = 96-103.
                                                                 0xD = 104-111.
                                                                 0xE = 112-119.
                                                                 0xF = 120-127. */
        uint64_t eer_val               : 1;  /**< [  9:  9](SRO/H) Dormant encryption key match. */
        uint64_t eer_lck               : 1;  /**< [ 10: 10](SRO/H) Encryption enable register locked. */
        uint64_t reserved_11_63        : 53;
#endif /* Word 0 - End */
    } s;
    struct bdk_rnm_ctl_status_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t dis_mak               : 1;  /**< [ 11: 11](SR/W1S/H) Disable use of master AES KEY. */
        uint64_t eer_lck               : 1;  /**< [ 10: 10](SRO/H) Encryption enable register locked. */
        uint64_t eer_val               : 1;  /**< [  9:  9](SRO/H) Dormant encryption key match. */
        uint64_t ent_sel               : 4;  /**< [  8:  5](SR/W) Select input to RNM FIFO.
                                                                 0x0 = 0-7.
                                                                 0x1 = 8-15.
                                                                 0x2 = 16-23.
                                                                 0x3 = 24-31.
                                                                 0x4 = 32-39.
                                                                 0x5 = 40-47.
                                                                 0x6 = 48-55.
                                                                 0x7 = 56-63.
                                                                 0x8 = 64-71.
                                                                 0x9 = 72-79.
                                                                 0xA = 80-87.
                                                                 0xB = 88-95.
                                                                 0xC = 96-103.
                                                                 0xD = 104-111.
                                                                 0xE = 112-119.
                                                                 0xF = 120-127. */
        uint64_t exp_ent               : 1;  /**< [  4:  4](SR/W) Exported entropy enable for random number generator. The next random number is
                                                                 available 80 coprocessor-clock cycles after switching this bit from zero to one. The
                                                                 next random number is available 730 coprocessor-clock cycles after switching this
                                                                 bit from one to zero. */
        uint64_t rng_rst               : 1;  /**< [  3:  3](SR/W) Reset the RNG. Setting this bit to one cancels the generation of the current random
                                                                 number. The next random number is available 730 coprocessor-clock cycles after this
                                                                 bit is cleared if [EXP_ENT] is set to zero. The next random number is available 80
                                                                 coprocessor-clock cycles after this bit is cleared if [EXP_ENT] is set to one. This bit is
                                                                 not automatically cleared. */
        uint64_t rnm_rst               : 1;  /**< [  2:  2](SR/W) Reset the RNM. Setting this bit to 1 drops all RNM transactions in flight and clears
                                                                 all stored numbers in the random number memory. Any outstanding NCBO credits will
                                                                 not be returned. RNM will not respond to any pending NCBI grants. RNM can accept
                                                                 new requests immediately after reset is cleared. This bit is not automatically
                                                                 cleared and will not reset any CSR fields. */
        uint64_t rng_en                : 1;  /**< [  1:  1](SR/W) Enables the output of the RNG. */
        uint64_t ent_en                : 1;  /**< [  0:  0](SR/W) Entropy enable for random number generator. */
#else /* Word 0 - Little Endian */
        uint64_t ent_en                : 1;  /**< [  0:  0](SR/W) Entropy enable for random number generator. */
        uint64_t rng_en                : 1;  /**< [  1:  1](SR/W) Enables the output of the RNG. */
        uint64_t rnm_rst               : 1;  /**< [  2:  2](SR/W) Reset the RNM. Setting this bit to 1 drops all RNM transactions in flight and clears
                                                                 all stored numbers in the random number memory. Any outstanding NCBO credits will
                                                                 not be returned. RNM will not respond to any pending NCBI grants. RNM can accept
                                                                 new requests immediately after reset is cleared. This bit is not automatically
                                                                 cleared and will not reset any CSR fields. */
        uint64_t rng_rst               : 1;  /**< [  3:  3](SR/W) Reset the RNG. Setting this bit to one cancels the generation of the current random
                                                                 number. The next random number is available 730 coprocessor-clock cycles after this
                                                                 bit is cleared if [EXP_ENT] is set to zero. The next random number is available 80
                                                                 coprocessor-clock cycles after this bit is cleared if [EXP_ENT] is set to one. This bit is
                                                                 not automatically cleared. */
        uint64_t exp_ent               : 1;  /**< [  4:  4](SR/W) Exported entropy enable for random number generator. The next random number is
                                                                 available 80 coprocessor-clock cycles after switching this bit from zero to one. The
                                                                 next random number is available 730 coprocessor-clock cycles after switching this
                                                                 bit from one to zero. */
        uint64_t ent_sel               : 4;  /**< [  8:  5](SR/W) Select input to RNM FIFO.
                                                                 0x0 = 0-7.
                                                                 0x1 = 8-15.
                                                                 0x2 = 16-23.
                                                                 0x3 = 24-31.
                                                                 0x4 = 32-39.
                                                                 0x5 = 40-47.
                                                                 0x6 = 48-55.
                                                                 0x7 = 56-63.
                                                                 0x8 = 64-71.
                                                                 0x9 = 72-79.
                                                                 0xA = 80-87.
                                                                 0xB = 88-95.
                                                                 0xC = 96-103.
                                                                 0xD = 104-111.
                                                                 0xE = 112-119.
                                                                 0xF = 120-127. */
        uint64_t eer_val               : 1;  /**< [  9:  9](SRO/H) Dormant encryption key match. */
        uint64_t eer_lck               : 1;  /**< [ 10: 10](SRO/H) Encryption enable register locked. */
        uint64_t dis_mak               : 1;  /**< [ 11: 11](SR/W1S/H) Disable use of master AES KEY. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } cn8;
    struct bdk_rnm_ctl_status_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t zuc_en                : 1;  /**< [ 11: 11](SR/W) Enable output of the ZUC engine. Before setting this bit software must write to
                                                                 all RNM_ZUC_INIT_LFSR(), and RNM_ZUC_INIT_NLF() registers. */
        uint64_t eer_lck               : 1;  /**< [ 10: 10](SRO/H) Encryption enable register locked. */
        uint64_t eer_val               : 1;  /**< [  9:  9](SRO/H) Dormant encryption key match. */
        uint64_t ent_sel               : 4;  /**< [  8:  5](SR/W) Select input to RNM FIFO.
                                                                 0x0 = 0-7.
                                                                 0x1 = 8-15.
                                                                 0x2 = 16-23.
                                                                 0x3 = 24-31.
                                                                 0x4 = 32-39.
                                                                 0x5 = 40-47.
                                                                 0x6 = 48-55.
                                                                 0x7 = 56-63.
                                                                 0x8 = 64-71.
                                                                 0x9 = 72-79.
                                                                 0xA = 80-87.
                                                                 0xB = 88-95.
                                                                 0xC = 96-103.
                                                                 0xD = 104-111.
                                                                 0xE = 112-119.
                                                                 0xF = 120-127. */
        uint64_t exp_ent               : 1;  /**< [  4:  4](SR/W) Exported entropy enable for random number generator. The next random number is
                                                                 available 80 coprocessor-clock cycles after switching this bit from zero to one. The
                                                                 next random number is available 730 coprocessor-clock cycles after switching this
                                                                 bit from one to zero. */
        uint64_t rng_rst               : 1;  /**< [  3:  3](SR/W) Reset the RNG. Setting this bit to one cancels the generation of the current random
                                                                 number. The next random number is available 730 coprocessor-clock cycles after this
                                                                 bit is cleared if [EXP_ENT] is set to zero. The next random number is available 80
                                                                 coprocessor-clock cycles after this bit is cleared if [EXP_ENT] is set to one. This bit is
                                                                 not automatically cleared. */
        uint64_t rnm_rst               : 1;  /**< [  2:  2](SRO) Reserved. Writes are ignored for backward compatibility. */
        uint64_t rng_en                : 1;  /**< [  1:  1](SR/W) Enables the output of the RNG. */
        uint64_t ent_en                : 1;  /**< [  0:  0](SR/W) Entropy enable for random number generator. */
#else /* Word 0 - Little Endian */
        uint64_t ent_en                : 1;  /**< [  0:  0](SR/W) Entropy enable for random number generator. */
        uint64_t rng_en                : 1;  /**< [  1:  1](SR/W) Enables the output of the RNG. */
        uint64_t rnm_rst               : 1;  /**< [  2:  2](SRO) Reserved. Writes are ignored for backward compatibility. */
        uint64_t rng_rst               : 1;  /**< [  3:  3](SR/W) Reset the RNG. Setting this bit to one cancels the generation of the current random
                                                                 number. The next random number is available 730 coprocessor-clock cycles after this
                                                                 bit is cleared if [EXP_ENT] is set to zero. The next random number is available 80
                                                                 coprocessor-clock cycles after this bit is cleared if [EXP_ENT] is set to one. This bit is
                                                                 not automatically cleared. */
        uint64_t exp_ent               : 1;  /**< [  4:  4](SR/W) Exported entropy enable for random number generator. The next random number is
                                                                 available 80 coprocessor-clock cycles after switching this bit from zero to one. The
                                                                 next random number is available 730 coprocessor-clock cycles after switching this
                                                                 bit from one to zero. */
        uint64_t ent_sel               : 4;  /**< [  8:  5](SR/W) Select input to RNM FIFO.
                                                                 0x0 = 0-7.
                                                                 0x1 = 8-15.
                                                                 0x2 = 16-23.
                                                                 0x3 = 24-31.
                                                                 0x4 = 32-39.
                                                                 0x5 = 40-47.
                                                                 0x6 = 48-55.
                                                                 0x7 = 56-63.
                                                                 0x8 = 64-71.
                                                                 0x9 = 72-79.
                                                                 0xA = 80-87.
                                                                 0xB = 88-95.
                                                                 0xC = 96-103.
                                                                 0xD = 104-111.
                                                                 0xE = 112-119.
                                                                 0xF = 120-127. */
        uint64_t eer_val               : 1;  /**< [  9:  9](SRO/H) Dormant encryption key match. */
        uint64_t eer_lck               : 1;  /**< [ 10: 10](SRO/H) Encryption enable register locked. */
        uint64_t zuc_en                : 1;  /**< [ 11: 11](SR/W) Enable output of the ZUC engine. Before setting this bit software must write to
                                                                 all RNM_ZUC_INIT_LFSR(), and RNM_ZUC_INIT_NLF() registers. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_rnm_ctl_status bdk_rnm_ctl_status_t;

#define BDK_RNM_CTL_STATUS BDK_RNM_CTL_STATUS_FUNC()
static inline uint64_t BDK_RNM_CTL_STATUS_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RNM_CTL_STATUS_FUNC(void)
{
    return 0x87e040000000ll;
}

#define typedef_BDK_RNM_CTL_STATUS bdk_rnm_ctl_status_t
#define bustype_BDK_RNM_CTL_STATUS BDK_CSR_TYPE_RSL
#define basename_BDK_RNM_CTL_STATUS "RNM_CTL_STATUS"
#define device_bar_BDK_RNM_CTL_STATUS 0x0 /* PF_BAR0 */
#define busnum_BDK_RNM_CTL_STATUS 0
#define arguments_BDK_RNM_CTL_STATUS -1,-1,-1,-1

/**
 * Register (RSL) rnm_eer_dbg
 *
 * INTERNAL: RNM Encryption Enable Debug Register
 *
 * This register is the encryption enable debug register.
 */
union bdk_rnm_eer_dbg
{
    uint64_t u;
    struct bdk_rnm_eer_dbg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SRO/H) Dormant encryption debug info. */
#else /* Word 0 - Little Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](SRO/H) Dormant encryption debug info. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rnm_eer_dbg_s cn; */
};
typedef union bdk_rnm_eer_dbg bdk_rnm_eer_dbg_t;

#define BDK_RNM_EER_DBG BDK_RNM_EER_DBG_FUNC()
static inline uint64_t BDK_RNM_EER_DBG_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RNM_EER_DBG_FUNC(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X))
        return 0x87e040000018ll;
    __bdk_csr_fatal("RNM_EER_DBG", 0, 0, 0, 0, 0);
}

#define typedef_BDK_RNM_EER_DBG bdk_rnm_eer_dbg_t
#define bustype_BDK_RNM_EER_DBG BDK_CSR_TYPE_RSL
#define basename_BDK_RNM_EER_DBG "RNM_EER_DBG"
#define device_bar_BDK_RNM_EER_DBG 0x0 /* PF_BAR0 */
#define busnum_BDK_RNM_EER_DBG 0
#define arguments_BDK_RNM_EER_DBG -1,-1,-1,-1

/**
 * Register (RSL) rnm_eer_key
 *
 * RNM Encryption Enable Register
 * This register is the encryption enable register.
 */
union bdk_rnm_eer_key
{
    uint64_t u;
    struct bdk_rnm_eer_key_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t key                   : 64; /**< [ 63:  0](SWO) Dormant encryption key. If dormant crypto is fuse-enabled, crypto can be enabled by
                                                                 writing this register with the correct key. */
#else /* Word 0 - Little Endian */
        uint64_t key                   : 64; /**< [ 63:  0](SWO) Dormant encryption key. If dormant crypto is fuse-enabled, crypto can be enabled by
                                                                 writing this register with the correct key. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rnm_eer_key_s cn; */
};
typedef union bdk_rnm_eer_key bdk_rnm_eer_key_t;

#define BDK_RNM_EER_KEY BDK_RNM_EER_KEY_FUNC()
static inline uint64_t BDK_RNM_EER_KEY_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RNM_EER_KEY_FUNC(void)
{
    return 0x87e040000010ll;
}

#define typedef_BDK_RNM_EER_KEY bdk_rnm_eer_key_t
#define bustype_BDK_RNM_EER_KEY BDK_CSR_TYPE_RSL
#define basename_BDK_RNM_EER_KEY "RNM_EER_KEY"
#define device_bar_BDK_RNM_EER_KEY 0x0 /* PF_BAR0 */
#define busnum_BDK_RNM_EER_KEY 0
#define arguments_BDK_RNM_EER_KEY -1,-1,-1,-1

/**
 * Register (NCB) rnm_random
 *
 * RNM Random Register
 */
union bdk_rnm_random
{
    uint64_t u;
    struct bdk_rnm_random_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](RO/H) Generated random number. This register may be accessed with a 8, 16, 32 or 64-bit
                                                                 operation. This register is on a independent page, and may be mapped into guest operating
                                                                 systems. */
#else /* Word 0 - Little Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](RO/H) Generated random number. This register may be accessed with a 8, 16, 32 or 64-bit
                                                                 operation. This register is on a independent page, and may be mapped into guest operating
                                                                 systems. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rnm_random_s cn8; */
    struct bdk_rnm_random_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](RO/H) Generated random number. This register may be accessed with a 8, 16, 32 or 64-bit
                                                                 operation. This register is on a independent page, and may be mapped into guest operating
                                                                 systems. Accesses to RNM_RANDOM larger than 64 bits will return 0x0 and fault. */
#else /* Word 0 - Little Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](RO/H) Generated random number. This register may be accessed with a 8, 16, 32 or 64-bit
                                                                 operation. This register is on a independent page, and may be mapped into guest operating
                                                                 systems. Accesses to RNM_RANDOM larger than 64 bits will return 0x0 and fault. */
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_rnm_random bdk_rnm_random_t;

#define BDK_RNM_RANDOM BDK_RNM_RANDOM_FUNC()
static inline uint64_t BDK_RNM_RANDOM_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RNM_RANDOM_FUNC(void)
{
    return 0x840000800000ll;
}

#define typedef_BDK_RNM_RANDOM bdk_rnm_random_t
#define bustype_BDK_RNM_RANDOM BDK_CSR_TYPE_NCB
#define basename_BDK_RNM_RANDOM "RNM_RANDOM"
#define device_bar_BDK_RNM_RANDOM 0x0 /* VF_BAR0 */
#define busnum_BDK_RNM_RANDOM 0
#define arguments_BDK_RNM_RANDOM -1,-1,-1,-1

/**
 * Register (RSL) rnm_serial_num
 *
 * RNM Fuse Serial Number Register
 */
union bdk_rnm_serial_num
{
    uint64_t u;
    struct bdk_rnm_serial_num_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](RO/H) Dormant encryption serial number. */
#else /* Word 0 - Little Endian */
        uint64_t dat                   : 64; /**< [ 63:  0](RO/H) Dormant encryption serial number. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rnm_serial_num_s cn; */
};
typedef union bdk_rnm_serial_num bdk_rnm_serial_num_t;

#define BDK_RNM_SERIAL_NUM BDK_RNM_SERIAL_NUM_FUNC()
static inline uint64_t BDK_RNM_SERIAL_NUM_FUNC(void) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RNM_SERIAL_NUM_FUNC(void)
{
    return 0x87e040000020ll;
}

#define typedef_BDK_RNM_SERIAL_NUM bdk_rnm_serial_num_t
#define bustype_BDK_RNM_SERIAL_NUM BDK_CSR_TYPE_RSL
#define basename_BDK_RNM_SERIAL_NUM "RNM_SERIAL_NUM"
#define device_bar_BDK_RNM_SERIAL_NUM 0x0 /* PF_BAR0 */
#define busnum_BDK_RNM_SERIAL_NUM 0
#define arguments_BDK_RNM_SERIAL_NUM -1,-1,-1,-1

/**
 * Register (RSL) rnm_zuc_init_lfsr#
 *
 * RNM ZUC LFSR Initialization Register
 * This register is used to initialize the state of the 16 state elements in RNM's ZUC
 * LFSR. See RNM_CTL_STATUS[ZUC_EN].
 *
 * Before writing to this register, RNM_CTL_STATUS[ZUC_EN] must be zero to turn off the
 * ZUC engine.
 */
union bdk_rnm_zuc_init_lfsrx
{
    uint64_t u;
    struct bdk_rnm_zuc_init_lfsrx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_31_63        : 33;
        uint64_t lfsr                  : 31; /**< [ 30:  0](SWO) Write the state of one ZUC LFSR element. */
#else /* Word 0 - Little Endian */
        uint64_t lfsr                  : 31; /**< [ 30:  0](SWO) Write the state of one ZUC LFSR element. */
        uint64_t reserved_31_63        : 33;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rnm_zuc_init_lfsrx_s cn; */
};
typedef union bdk_rnm_zuc_init_lfsrx bdk_rnm_zuc_init_lfsrx_t;

static inline uint64_t BDK_RNM_ZUC_INIT_LFSRX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RNM_ZUC_INIT_LFSRX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=15))
        return 0x87e040000100ll + 8ll * ((a) & 0xf);
    __bdk_csr_fatal("RNM_ZUC_INIT_LFSRX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RNM_ZUC_INIT_LFSRX(a) bdk_rnm_zuc_init_lfsrx_t
#define bustype_BDK_RNM_ZUC_INIT_LFSRX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_RNM_ZUC_INIT_LFSRX(a) "RNM_ZUC_INIT_LFSRX"
#define device_bar_BDK_RNM_ZUC_INIT_LFSRX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_RNM_ZUC_INIT_LFSRX(a) (a)
#define arguments_BDK_RNM_ZUC_INIT_LFSRX(a) (a),-1,-1,-1

/**
 * Register (RSL) rnm_zuc_init_nlf#
 *
 * RNM ZUC LFSR Initialization Register
 * This register is used to initialize the state of the two 32-bit memory cells in
 * ZUC's nonlinear function. See RNM_CTL_STATUS[ZUC_EN].
 *
 * Before writing to this register, RNM_CTL_STATUS[ZUC_EN] must be zero to turn off the
 * ZUC engine.
 */
union bdk_rnm_zuc_init_nlfx
{
    uint64_t u;
    struct bdk_rnm_zuc_init_nlfx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t r_state               : 32; /**< [ 31:  0](SWO) Write the state of one ZUC nonlinear function element. */
#else /* Word 0 - Little Endian */
        uint64_t r_state               : 32; /**< [ 31:  0](SWO) Write the state of one ZUC nonlinear function element. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_rnm_zuc_init_nlfx_s cn; */
};
typedef union bdk_rnm_zuc_init_nlfx bdk_rnm_zuc_init_nlfx_t;

static inline uint64_t BDK_RNM_ZUC_INIT_NLFX(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_RNM_ZUC_INIT_NLFX(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=1))
        return 0x87e040000200ll + 8ll * ((a) & 0x1);
    __bdk_csr_fatal("RNM_ZUC_INIT_NLFX", 1, a, 0, 0, 0);
}

#define typedef_BDK_RNM_ZUC_INIT_NLFX(a) bdk_rnm_zuc_init_nlfx_t
#define bustype_BDK_RNM_ZUC_INIT_NLFX(a) BDK_CSR_TYPE_RSL
#define basename_BDK_RNM_ZUC_INIT_NLFX(a) "RNM_ZUC_INIT_NLFX"
#define device_bar_BDK_RNM_ZUC_INIT_NLFX(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_RNM_ZUC_INIT_NLFX(a) (a)
#define arguments_BDK_RNM_ZUC_INIT_NLFX(a) (a),-1,-1,-1

#endif /* __BDK_CSRS_RNM_H__ */
