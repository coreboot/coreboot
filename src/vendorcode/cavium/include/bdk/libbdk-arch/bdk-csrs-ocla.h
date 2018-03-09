#ifndef __BDK_CSRS_OCLA_H__
#define __BDK_CSRS_OCLA_H__
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
 * Cavium OCLA.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration ocla_bar_e
 *
 * OCLA Base Address Register Enumeration
 * Enumerates the base address registers.
 */
#define BDK_OCLA_BAR_E_OCLAX_PF_BAR0_CN8(a) (0x87e0a8000000ll + 0x1000000ll * (a))
#define BDK_OCLA_BAR_E_OCLAX_PF_BAR0_CN8_SIZE 0x800000ull
#define BDK_OCLA_BAR_E_OCLAX_PF_BAR0_CN9(a) (0x87e0b0000000ll + 0x1000000ll * (a))
#define BDK_OCLA_BAR_E_OCLAX_PF_BAR0_CN9_SIZE 0x800000ull
#define BDK_OCLA_BAR_E_OCLAX_PF_BAR4_CN8(a) (0x87e0a8f00000ll + 0x1000000ll * (a))
#define BDK_OCLA_BAR_E_OCLAX_PF_BAR4_CN8_SIZE 0x100000ull
#define BDK_OCLA_BAR_E_OCLAX_PF_BAR4_CN9(a) (0x87e0b0f00000ll + 0x1000000ll * (a))
#define BDK_OCLA_BAR_E_OCLAX_PF_BAR4_CN9_SIZE 0x100000ull

/**
 * Enumeration ocla_int_vec_e
 *
 * OCLA MSI-X Vector Enumeration
 * Enumerates the MSI-X interrupt vectors.
 */
#define BDK_OCLA_INT_VEC_E_INTS (0)

/**
 * Structure ocla_cap_ctl_s
 *
 * OCLA Capture Control Structure
 * Control packets are indicated by an entry's bit 37 being set, and described by the
 * OCLA_CAP_CTL_S structure:
 */
union bdk_ocla_cap_ctl_s
{
    uint64_t u;
    struct bdk_ocla_cap_ctl_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_38_63        : 26;
        uint64_t ctl                   : 1;  /**< [ 37: 37] Indicates a control word. Always set for control structures. */
        uint64_t sinfo                 : 1;  /**< [ 36: 36] Indicates OCLA()_FSM()_STATE()[SINFO_SET] was set for the state that led to the
                                                                 capture state. This allows the FSM to optionally communicate its current state
                                                                 to observing software; [SINFO] is otherwise opaque to reassembling the trace
                                                                 information. */
        uint64_t eot1                  : 1;  /**< [ 35: 35] End of duplicated capture for high data. Symmetric with EOT0 description; see [EOT0]. */
        uint64_t eot0                  : 1;  /**< [ 34: 34] End of duplicated capture for low data. When set, [CYCLE] indicates the cycle at which the
                                                                 previous entry of low data stopped being replicated. This may be set along with SOT0 to
                                                                 indicate a repeat followed by new sequence. */
        uint64_t sot1                  : 1;  /**< [ 33: 33] Start transition from no-capture to capture or duplicated data stopped while capturing for
                                                                 high data. When set, [CYCLE] indicates the cycle number of the next new high data, minus one
                                                                 cycle. */
        uint64_t sot0                  : 1;  /**< [ 32: 32] Start transition from no-capture to capture or duplicated data stopped while capturing for
                                                                 low data. When set, [CYCLE] indicates the cycle number of the next new low data, minus one
                                                                 cycle. */
        uint64_t cycle                 : 32; /**< [ 31:  0] Cycle at which this control entry was written, from OCLA()_TIME. */
#else /* Word 0 - Little Endian */
        uint64_t cycle                 : 32; /**< [ 31:  0] Cycle at which this control entry was written, from OCLA()_TIME. */
        uint64_t sot0                  : 1;  /**< [ 32: 32] Start transition from no-capture to capture or duplicated data stopped while capturing for
                                                                 low data. When set, [CYCLE] indicates the cycle number of the next new low data, minus one
                                                                 cycle. */
        uint64_t sot1                  : 1;  /**< [ 33: 33] Start transition from no-capture to capture or duplicated data stopped while capturing for
                                                                 high data. When set, [CYCLE] indicates the cycle number of the next new high data, minus one
                                                                 cycle. */
        uint64_t eot0                  : 1;  /**< [ 34: 34] End of duplicated capture for low data. When set, [CYCLE] indicates the cycle at which the
                                                                 previous entry of low data stopped being replicated. This may be set along with SOT0 to
                                                                 indicate a repeat followed by new sequence. */
        uint64_t eot1                  : 1;  /**< [ 35: 35] End of duplicated capture for high data. Symmetric with EOT0 description; see [EOT0]. */
        uint64_t sinfo                 : 1;  /**< [ 36: 36] Indicates OCLA()_FSM()_STATE()[SINFO_SET] was set for the state that led to the
                                                                 capture state. This allows the FSM to optionally communicate its current state
                                                                 to observing software; [SINFO] is otherwise opaque to reassembling the trace
                                                                 information. */
        uint64_t ctl                   : 1;  /**< [ 37: 37] Indicates a control word. Always set for control structures. */
        uint64_t reserved_38_63        : 26;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocla_cap_ctl_s_s cn; */
};

/**
 * Structure ocla_cap_dat_s
 *
 * OCLA Capture Data Structure
 * Data packets are indicated by an entry's bit 37 being clear, and described by the
 * OCLA_CAP_DAT_S structure:
 */
union bdk_ocla_cap_dat_s
{
    uint64_t u;
    struct bdk_ocla_cap_dat_s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_38_63        : 26;
        uint64_t ctl                   : 1;  /**< [ 37: 37] Indicates a control word. Always clear for data structures. */
        uint64_t hi                    : 1;  /**< [ 36: 36] Set to indicate a sample of high data, clear for a sample of low data. */
        uint64_t data                  : 36; /**< [ 35:  0] Captured trace data. */
#else /* Word 0 - Little Endian */
        uint64_t data                  : 36; /**< [ 35:  0] Captured trace data. */
        uint64_t hi                    : 1;  /**< [ 36: 36] Set to indicate a sample of high data, clear for a sample of low data. */
        uint64_t ctl                   : 1;  /**< [ 37: 37] Indicates a control word. Always clear for data structures. */
        uint64_t reserved_38_63        : 26;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_ocla_cap_dat_s_s cn; */
};

/**
 * Register (RSL) ocla#_active_pc
 *
 * OCLA Conditional Clock Counter Register
 * This register counts conditional clocks for power management.
 *
 * This register is reset on ocla domain reset.
 */
union bdk_oclax_active_pc
{
    uint64_t u;
    struct bdk_oclax_active_pc_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Count of conditional clock cycles since reset. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 64; /**< [ 63:  0](R/W/H) Count of conditional clock cycles since reset. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_active_pc_s cn; */
};
typedef union bdk_oclax_active_pc bdk_oclax_active_pc_t;

static inline uint64_t BDK_OCLAX_ACTIVE_PC(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_ACTIVE_PC(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000620ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_ACTIVE_PC", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_ACTIVE_PC(a) bdk_oclax_active_pc_t
#define bustype_BDK_OCLAX_ACTIVE_PC(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_ACTIVE_PC(a) "OCLAX_ACTIVE_PC"
#define device_bar_BDK_OCLAX_ACTIVE_PC(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_ACTIVE_PC(a) (a)
#define arguments_BDK_OCLAX_ACTIVE_PC(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_bist_result
 *
 * OCLA BIST Result Registers
 * This register provides access to the internal BIST results. Each bit is the BIST result of an
 * individual memory (per bit, 0 = pass and 1 = fail).
 *
 * Internal:
 * FIXME remove fields and add deprecated: "RAZ" per MDC common changes.
 */
union bdk_oclax_bist_result
{
    uint64_t u;
    struct bdk_oclax_bist_result_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t dat                   : 1;  /**< [  0:  0](RO) BIST result of the DAT memory. */
#else /* Word 0 - Little Endian */
        uint64_t dat                   : 1;  /**< [  0:  0](RO) BIST result of the DAT memory. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_bist_result_s cn; */
};
typedef union bdk_oclax_bist_result bdk_oclax_bist_result_t;

static inline uint64_t BDK_OCLAX_BIST_RESULT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_BIST_RESULT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a8000040ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a8000040ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a8000040ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000040ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_BIST_RESULT", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_BIST_RESULT(a) bdk_oclax_bist_result_t
#define bustype_BDK_OCLAX_BIST_RESULT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_BIST_RESULT(a) "OCLAX_BIST_RESULT"
#define device_bar_BDK_OCLAX_BIST_RESULT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_BIST_RESULT(a) (a)
#define arguments_BDK_OCLAX_BIST_RESULT(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_cdh#_ctl
 *
 * OCLA Capture Data Half Control Registers
 */
union bdk_oclax_cdhx_ctl
{
    uint64_t u;
    struct bdk_oclax_cdhx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t dup                   : 1;  /**< [  5:  5](R/W) Retain duplicates in the data stream. */
        uint64_t dis_stamp             : 1;  /**< [  4:  4](R/W) Remove time stamps from data stream. */
        uint64_t cap_ctl               : 4;  /**< [  3:  0](R/W) Minterms that will cause data to be captured. These minterms are the four inputs
                                                                 to a four-to-one mux selected by PLA1 and zero. The output is thus calculated
                                                                 from the equation:

                                                                 \<pre\>
                                                                   fsmcap0 = OCLA(0..1)_FSM(0)_STATE[state0][CAP].
                                                                   fsmcap1 = OCLA(0..1)_FSM(1)_STATE[state1][CAP].
                                                                   out = (   (\<3\> & fsmcap1 & fsmcap0)
                                                                          || (\<2\> & fsmcap1 & !fsmcap0)
                                                                          || (\<1\> & !fsmcap1 & fsmcap0)
                                                                          || (\<0\> & !fsmcap1 & !fsmcap0)).
                                                                 \</pre\>

                                                                 Common examples:
                                                                 0x0 = No capture.
                                                                 0xA = Capture when fsmcap0 requests capture.
                                                                 0xC = Capture when fsmcap1 requests capture.
                                                                 0x6 = Capture on fsmcap0 EXOR fsmcap1.
                                                                 0x8 = Capture on fsmcap0 & fsmcap1.
                                                                 0xE = Capture on fsmcap0 | fsmcap1.
                                                                 0xF = Always capture. */
#else /* Word 0 - Little Endian */
        uint64_t cap_ctl               : 4;  /**< [  3:  0](R/W) Minterms that will cause data to be captured. These minterms are the four inputs
                                                                 to a four-to-one mux selected by PLA1 and zero. The output is thus calculated
                                                                 from the equation:

                                                                 \<pre\>
                                                                   fsmcap0 = OCLA(0..1)_FSM(0)_STATE[state0][CAP].
                                                                   fsmcap1 = OCLA(0..1)_FSM(1)_STATE[state1][CAP].
                                                                   out = (   (\<3\> & fsmcap1 & fsmcap0)
                                                                          || (\<2\> & fsmcap1 & !fsmcap0)
                                                                          || (\<1\> & !fsmcap1 & fsmcap0)
                                                                          || (\<0\> & !fsmcap1 & !fsmcap0)).
                                                                 \</pre\>

                                                                 Common examples:
                                                                 0x0 = No capture.
                                                                 0xA = Capture when fsmcap0 requests capture.
                                                                 0xC = Capture when fsmcap1 requests capture.
                                                                 0x6 = Capture on fsmcap0 EXOR fsmcap1.
                                                                 0x8 = Capture on fsmcap0 & fsmcap1.
                                                                 0xE = Capture on fsmcap0 | fsmcap1.
                                                                 0xF = Always capture. */
        uint64_t dis_stamp             : 1;  /**< [  4:  4](R/W) Remove time stamps from data stream. */
        uint64_t dup                   : 1;  /**< [  5:  5](R/W) Retain duplicates in the data stream. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } s;
    struct bdk_oclax_cdhx_ctl_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t dup                   : 1;  /**< [  5:  5](R/W) Retain duplicates in the data stream. */
        uint64_t dis_stamp             : 1;  /**< [  4:  4](R/W) Remove time stamps from data stream. */
        uint64_t cap_ctl               : 4;  /**< [  3:  0](R/W) Minterms that will cause data to be captured. These minterms are the four inputs
                                                                 to a four-to-one mux selected by PLA1 and zero. The output is thus calculated
                                                                 from the equation:

                                                                 \<pre\>
                                                                   fsmcap0 = OCLA(0..4)_FSM(0)_STATE[state0][CAP].
                                                                   fsmcap1 = OCLA(0..4)_FSM(1)_STATE[state1][CAP].
                                                                   out = (   (\<3\> & fsmcap1 & fsmcap0)
                                                                          || (\<2\> & fsmcap1 & !fsmcap0)
                                                                          || (\<1\> & !fsmcap1 & fsmcap0)
                                                                          || (\<0\> & !fsmcap1 & !fsmcap0)).
                                                                 \</pre\>

                                                                 Common examples:
                                                                 0x0 = No capture.
                                                                 0xA = Capture when fsmcap0 requests capture.
                                                                 0xC = Capture when fsmcap1 requests capture.
                                                                 0x6 = Capture on fsmcap0 EXOR fsmcap1.
                                                                 0x8 = Capture on fsmcap0 & fsmcap1.
                                                                 0xE = Capture on fsmcap0 | fsmcap1.
                                                                 0xF = Always capture. */
#else /* Word 0 - Little Endian */
        uint64_t cap_ctl               : 4;  /**< [  3:  0](R/W) Minterms that will cause data to be captured. These minterms are the four inputs
                                                                 to a four-to-one mux selected by PLA1 and zero. The output is thus calculated
                                                                 from the equation:

                                                                 \<pre\>
                                                                   fsmcap0 = OCLA(0..4)_FSM(0)_STATE[state0][CAP].
                                                                   fsmcap1 = OCLA(0..4)_FSM(1)_STATE[state1][CAP].
                                                                   out = (   (\<3\> & fsmcap1 & fsmcap0)
                                                                          || (\<2\> & fsmcap1 & !fsmcap0)
                                                                          || (\<1\> & !fsmcap1 & fsmcap0)
                                                                          || (\<0\> & !fsmcap1 & !fsmcap0)).
                                                                 \</pre\>

                                                                 Common examples:
                                                                 0x0 = No capture.
                                                                 0xA = Capture when fsmcap0 requests capture.
                                                                 0xC = Capture when fsmcap1 requests capture.
                                                                 0x6 = Capture on fsmcap0 EXOR fsmcap1.
                                                                 0x8 = Capture on fsmcap0 & fsmcap1.
                                                                 0xE = Capture on fsmcap0 | fsmcap1.
                                                                 0xF = Always capture. */
        uint64_t dis_stamp             : 1;  /**< [  4:  4](R/W) Remove time stamps from data stream. */
        uint64_t dup                   : 1;  /**< [  5:  5](R/W) Retain duplicates in the data stream. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } cn9;
    /* struct bdk_oclax_cdhx_ctl_s cn81xx; */
    /* struct bdk_oclax_cdhx_ctl_cn9 cn88xx; */
    struct bdk_oclax_cdhx_ctl_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t dup                   : 1;  /**< [  5:  5](R/W) Retain duplicates in the data stream. */
        uint64_t dis_stamp             : 1;  /**< [  4:  4](R/W) Remove time stamps from data stream. */
        uint64_t cap_ctl               : 4;  /**< [  3:  0](R/W) Minterms that will cause data to be captured. These minterms are the four inputs
                                                                 to a four-to-one mux selected by PLA1 and zero. The output is thus calculated
                                                                 from the equation:

                                                                 \<pre\>
                                                                   fsmcap0 = OCLA(0..2)_FSM(0)_STATE[state0][CAP].
                                                                   fsmcap1 = OCLA(0..2)_FSM(1)_STATE[state1][CAP].
                                                                   out = (   (\<3\> & fsmcap1 & fsmcap0)
                                                                          || (\<2\> & fsmcap1 & !fsmcap0)
                                                                          || (\<1\> & !fsmcap1 & fsmcap0)
                                                                          || (\<0\> & !fsmcap1 & !fsmcap0)).
                                                                 \</pre\>

                                                                 Common examples:
                                                                 0x0 = No capture.
                                                                 0xA = Capture when fsmcap0 requests capture.
                                                                 0xC = Capture when fsmcap1 requests capture.
                                                                 0x6 = Capture on fsmcap0 EXOR fsmcap1.
                                                                 0x8 = Capture on fsmcap0 & fsmcap1.
                                                                 0xE = Capture on fsmcap0 | fsmcap1.
                                                                 0xF = Always capture. */
#else /* Word 0 - Little Endian */
        uint64_t cap_ctl               : 4;  /**< [  3:  0](R/W) Minterms that will cause data to be captured. These minterms are the four inputs
                                                                 to a four-to-one mux selected by PLA1 and zero. The output is thus calculated
                                                                 from the equation:

                                                                 \<pre\>
                                                                   fsmcap0 = OCLA(0..2)_FSM(0)_STATE[state0][CAP].
                                                                   fsmcap1 = OCLA(0..2)_FSM(1)_STATE[state1][CAP].
                                                                   out = (   (\<3\> & fsmcap1 & fsmcap0)
                                                                          || (\<2\> & fsmcap1 & !fsmcap0)
                                                                          || (\<1\> & !fsmcap1 & fsmcap0)
                                                                          || (\<0\> & !fsmcap1 & !fsmcap0)).
                                                                 \</pre\>

                                                                 Common examples:
                                                                 0x0 = No capture.
                                                                 0xA = Capture when fsmcap0 requests capture.
                                                                 0xC = Capture when fsmcap1 requests capture.
                                                                 0x6 = Capture on fsmcap0 EXOR fsmcap1.
                                                                 0x8 = Capture on fsmcap0 & fsmcap1.
                                                                 0xE = Capture on fsmcap0 | fsmcap1.
                                                                 0xF = Always capture. */
        uint64_t dis_stamp             : 1;  /**< [  4:  4](R/W) Remove time stamps from data stream. */
        uint64_t dup                   : 1;  /**< [  5:  5](R/W) Retain duplicates in the data stream. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_oclax_cdhx_ctl bdk_oclax_cdhx_ctl_t;

static inline uint64_t BDK_OCLAX_CDHX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_CDHX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=1)))
        return 0x87e0a8000600ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b<=1)))
        return 0x87e0a8000600ll + 0x1000000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=4) && (b<=1)))
        return 0x87e0a8000600ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b<=1)))
        return 0x87e0b0000600ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("OCLAX_CDHX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_OCLAX_CDHX_CTL(a,b) bdk_oclax_cdhx_ctl_t
#define bustype_BDK_OCLAX_CDHX_CTL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_CDHX_CTL(a,b) "OCLAX_CDHX_CTL"
#define device_bar_BDK_OCLAX_CDHX_CTL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_CDHX_CTL(a,b) (a)
#define arguments_BDK_OCLAX_CDHX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocla#_cdh#_inject_state
 *
 * OCLA Capture Inject State Register
 * This register allows various state inputs to be inserted into the captured stream
 * data, to assist debugging of OCLA FSMs. Each input has two insertion positions
 * (i.e. [MCD] and [ALT_MCD]), so that some of the normal non-inject capture stream data
 * may still be observable.
 */
union bdk_oclax_cdhx_inject_state
{
    uint64_t u;
    struct bdk_oclax_cdhx_inject_state_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t alt_trig              : 1;  /**< [ 31: 31](R/W) When set, insert FSM trigger input into captured stream \<31\>. */
        uint64_t alt_mcd               : 3;  /**< [ 30: 28](R/W) When set, insert multichip debug (MCD) 0..2 FSM inputs into captured stream \<30:28\>. */
        uint64_t alt_match             : 4;  /**< [ 27: 24](R/W) When set, insert matcher FSM inputs into captured stream \<27:24\>. */
        uint64_t alt_fsm1_state        : 4;  /**< [ 23: 20](R/W) When set, insert FSM 1 state input into captured stream \<23:20\>. */
        uint64_t alt_fsm0_state        : 4;  /**< [ 19: 16](R/W) When set, insert FSM 0 state input into captured stream \<19:16\>. */
        uint64_t trig                  : 1;  /**< [ 15: 15](R/W) When set, insert FSM trigger input into captured stream \<15\>. */
        uint64_t mcd                   : 3;  /**< [ 14: 12](R/W) When set, insert multichip debug (MCD) 0..2 FSM inputs into captured stream \<14:12\>. */
        uint64_t match                 : 4;  /**< [ 11:  8](R/W) When set, insert matcher FSM inputs into captured stream \<11:8\>. */
        uint64_t fsm1_state            : 4;  /**< [  7:  4](R/W) When set, insert FSM 1 state input into captured stream \<7:4\>. */
        uint64_t fsm0_state            : 4;  /**< [  3:  0](R/W) When set, insert FSM 0 state input into captured stream \<3:0\>. */
#else /* Word 0 - Little Endian */
        uint64_t fsm0_state            : 4;  /**< [  3:  0](R/W) When set, insert FSM 0 state input into captured stream \<3:0\>. */
        uint64_t fsm1_state            : 4;  /**< [  7:  4](R/W) When set, insert FSM 1 state input into captured stream \<7:4\>. */
        uint64_t match                 : 4;  /**< [ 11:  8](R/W) When set, insert matcher FSM inputs into captured stream \<11:8\>. */
        uint64_t mcd                   : 3;  /**< [ 14: 12](R/W) When set, insert multichip debug (MCD) 0..2 FSM inputs into captured stream \<14:12\>. */
        uint64_t trig                  : 1;  /**< [ 15: 15](R/W) When set, insert FSM trigger input into captured stream \<15\>. */
        uint64_t alt_fsm0_state        : 4;  /**< [ 19: 16](R/W) When set, insert FSM 0 state input into captured stream \<19:16\>. */
        uint64_t alt_fsm1_state        : 4;  /**< [ 23: 20](R/W) When set, insert FSM 1 state input into captured stream \<23:20\>. */
        uint64_t alt_match             : 4;  /**< [ 27: 24](R/W) When set, insert matcher FSM inputs into captured stream \<27:24\>. */
        uint64_t alt_mcd               : 3;  /**< [ 30: 28](R/W) When set, insert multichip debug (MCD) 0..2 FSM inputs into captured stream \<30:28\>. */
        uint64_t alt_trig              : 1;  /**< [ 31: 31](R/W) When set, insert FSM trigger input into captured stream \<31\>. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_cdhx_inject_state_s cn; */
};
typedef union bdk_oclax_cdhx_inject_state bdk_oclax_cdhx_inject_state_t;

static inline uint64_t BDK_OCLAX_CDHX_INJECT_STATE(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_CDHX_INJECT_STATE(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b<=1)))
        return 0x87e0b0000610ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("OCLAX_CDHX_INJECT_STATE", 2, a, b, 0, 0);
}

#define typedef_BDK_OCLAX_CDHX_INJECT_STATE(a,b) bdk_oclax_cdhx_inject_state_t
#define bustype_BDK_OCLAX_CDHX_INJECT_STATE(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_CDHX_INJECT_STATE(a,b) "OCLAX_CDHX_INJECT_STATE"
#define device_bar_BDK_OCLAX_CDHX_INJECT_STATE(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_CDHX_INJECT_STATE(a,b) (a)
#define arguments_BDK_OCLAX_CDHX_INJECT_STATE(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocla#_const
 *
 * OCLA Constants Registers
 * Internal:
 * FIXME add new NO_DDR bit \<16\> which is RO 0, indicating:
 *
 * No DDR supported.
 * 0 = DDR dumping is supported, the OCLA()_STACK* registers exist and function.
 * 1 = DDR dumping is not supported.
 */
union bdk_oclax_const
{
    uint64_t u;
    struct bdk_oclax_const_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t dat_size              : 16; /**< [ 15:  0](RO) Size of data RAM in units of 36-bit entries. This value is subject to change between chip
                                                                 passes, and software should thus use this value rather than a hard coded constant.
                                                                 OCLA(0..1) size is 4096, OCLA(2) size is 8192. */
#else /* Word 0 - Little Endian */
        uint64_t dat_size              : 16; /**< [ 15:  0](RO) Size of data RAM in units of 36-bit entries. This value is subject to change between chip
                                                                 passes, and software should thus use this value rather than a hard coded constant.
                                                                 OCLA(0..1) size is 4096, OCLA(2) size is 8192. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    struct bdk_oclax_const_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t dat_size              : 16; /**< [ 15:  0](RO) Size of data RAM in units of 36-bit entries. This value is subject to change between chip
                                                                 passes, and software should thus use this value rather than a hard coded constant.
                                                                 OCLA(0..3) size is 4096, OCLA(4) size is 16384. */
#else /* Word 0 - Little Endian */
        uint64_t dat_size              : 16; /**< [ 15:  0](RO) Size of data RAM in units of 36-bit entries. This value is subject to change between chip
                                                                 passes, and software should thus use this value rather than a hard coded constant.
                                                                 OCLA(0..3) size is 4096, OCLA(4) size is 16384. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } cn9;
    /* struct bdk_oclax_const_s cn81xx; */
    struct bdk_oclax_const_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t dat_size              : 16; /**< [ 15:  0](RO) Size of data RAM in units of 36-bit entries. This value is subject to change between chip
                                                                 passes, and software should thus use this value rather than a hard coded constant.
                                                                 OCLA(0..3) size is 4096, OCLA(4) size is 8192. */
#else /* Word 0 - Little Endian */
        uint64_t dat_size              : 16; /**< [ 15:  0](RO) Size of data RAM in units of 36-bit entries. This value is subject to change between chip
                                                                 passes, and software should thus use this value rather than a hard coded constant.
                                                                 OCLA(0..3) size is 4096, OCLA(4) size is 8192. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } cn88xx;
    /* struct bdk_oclax_const_s cn83xx; */
};
typedef union bdk_oclax_const bdk_oclax_const_t;

static inline uint64_t BDK_OCLAX_CONST(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_CONST(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a8000000ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a8000000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a8000000ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000000ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_CONST", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_CONST(a) bdk_oclax_const_t
#define bustype_BDK_OCLAX_CONST(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_CONST(a) "OCLAX_CONST"
#define device_bar_BDK_OCLAX_CONST(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_CONST(a) (a)
#define arguments_BDK_OCLAX_CONST(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_dat#
 *
 * OCLA Data Registers
 */
union bdk_oclax_datx
{
    uint64_t u;
    struct bdk_oclax_datx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_38_63        : 26;
        uint64_t entry                 : 38; /**< [ 37:  0](RO/H) Captured entry. Data is in the format described by OCLA_CAP_DAT_S or OCLA_CAP_CTL_S. */
#else /* Word 0 - Little Endian */
        uint64_t entry                 : 38; /**< [ 37:  0](RO/H) Captured entry. Data is in the format described by OCLA_CAP_DAT_S or OCLA_CAP_CTL_S. */
        uint64_t reserved_38_63        : 26;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_datx_s cn; */
};
typedef union bdk_oclax_datx bdk_oclax_datx_t;

static inline uint64_t BDK_OCLAX_DATX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_DATX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=8191)))
        return 0x87e0a8400000ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x1fff);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b<=8191)))
        return 0x87e0a8400000ll + 0x1000000ll * ((a) & 0x3) + 8ll * ((b) & 0x1fff);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=4) && (b<=8191)))
        return 0x87e0a8400000ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x1fff);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b<=16383)))
        return 0x87e0b0400000ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x3fff);
    __bdk_csr_fatal("OCLAX_DATX", 2, a, b, 0, 0);
}

#define typedef_BDK_OCLAX_DATX(a,b) bdk_oclax_datx_t
#define bustype_BDK_OCLAX_DATX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_DATX(a,b) "OCLAX_DATX"
#define device_bar_BDK_OCLAX_DATX(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_DATX(a,b) (a)
#define arguments_BDK_OCLAX_DATX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocla#_dat_pop
 *
 * OCLA Data Pop Registers
 */
union bdk_oclax_dat_pop
{
    uint64_t u;
    struct bdk_oclax_dat_pop_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t valid                 : 1;  /**< [ 63: 63](RC/H) Valid entry. Indicates the FIFO contains data, and equivalent to OCLA()_FIFO_DEPTH[DEPTH] != 0. */
        uint64_t trig                  : 1;  /**< [ 62: 62](RO/H) Internal trigger set. Equivalent to OCLA()_STATE_INT[TRIG]. */
        uint64_t wmark                 : 1;  /**< [ 61: 61](RO/H) Internal buffer watermark reached. Equivalent to OCLA()_STATE_INT[WMARK]. */
        uint64_t reserved_38_60        : 23;
        uint64_t entry                 : 38; /**< [ 37:  0](RC/H) Captured entry. If [VALID] is set, has read side effect of unloading data by decrementing
                                                                 OCLA()_FIFO_DEPTH[DEPTH]. Data is in the format described by OCLA_CAP_DAT_S or
                                                                 OCLA_CAP_CTL_S.

                                                                 Note that unloading data will cause that data not to be sent to memory, therefore
                                                                 OCLA()_DAT_POP should not be read when OCLA()_FIFO_LIMIT[DDR] != all-ones. */
#else /* Word 0 - Little Endian */
        uint64_t entry                 : 38; /**< [ 37:  0](RC/H) Captured entry. If [VALID] is set, has read side effect of unloading data by decrementing
                                                                 OCLA()_FIFO_DEPTH[DEPTH]. Data is in the format described by OCLA_CAP_DAT_S or
                                                                 OCLA_CAP_CTL_S.

                                                                 Note that unloading data will cause that data not to be sent to memory, therefore
                                                                 OCLA()_DAT_POP should not be read when OCLA()_FIFO_LIMIT[DDR] != all-ones. */
        uint64_t reserved_38_60        : 23;
        uint64_t wmark                 : 1;  /**< [ 61: 61](RO/H) Internal buffer watermark reached. Equivalent to OCLA()_STATE_INT[WMARK]. */
        uint64_t trig                  : 1;  /**< [ 62: 62](RO/H) Internal trigger set. Equivalent to OCLA()_STATE_INT[TRIG]. */
        uint64_t valid                 : 1;  /**< [ 63: 63](RC/H) Valid entry. Indicates the FIFO contains data, and equivalent to OCLA()_FIFO_DEPTH[DEPTH] != 0. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_dat_pop_s cn; */
};
typedef union bdk_oclax_dat_pop bdk_oclax_dat_pop_t;

static inline uint64_t BDK_OCLAX_DAT_POP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_DAT_POP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a8000800ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a8000800ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a8000800ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000800ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_DAT_POP", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_DAT_POP(a) bdk_oclax_dat_pop_t
#define bustype_BDK_OCLAX_DAT_POP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_DAT_POP(a) "OCLAX_DAT_POP"
#define device_bar_BDK_OCLAX_DAT_POP(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_DAT_POP(a) (a)
#define arguments_BDK_OCLAX_DAT_POP(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_eco
 *
 * INTERNAL: OCLA ECO Register
 */
union bdk_oclax_eco
{
    uint64_t u;
    struct bdk_oclax_eco_s
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
    /* struct bdk_oclax_eco_s cn; */
};
typedef union bdk_oclax_eco bdk_oclax_eco_t;

static inline uint64_t BDK_OCLAX_ECO(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_ECO(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a83200d0ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a83200d0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS2_X) && (a<=4))
        return 0x87e0a83200d0ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b00000d0ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_ECO", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_ECO(a) bdk_oclax_eco_t
#define bustype_BDK_OCLAX_ECO(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_ECO(a) "OCLAX_ECO"
#define device_bar_BDK_OCLAX_ECO(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_ECO(a) (a)
#define arguments_BDK_OCLAX_ECO(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_fifo_depth
 *
 * OCLA Capture FIFO Depth Registers
 */
union bdk_oclax_fifo_depth
{
    uint64_t u;
    struct bdk_oclax_fifo_depth_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t depth                 : 16; /**< [ 15:  0](RO/H) Current capture FIFO depth in 36-bit words. */
#else /* Word 0 - Little Endian */
        uint64_t depth                 : 16; /**< [ 15:  0](RO/H) Current capture FIFO depth in 36-bit words. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_fifo_depth_s cn; */
};
typedef union bdk_oclax_fifo_depth bdk_oclax_fifo_depth_t;

static inline uint64_t BDK_OCLAX_FIFO_DEPTH(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_FIFO_DEPTH(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a8000200ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a8000200ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a8000200ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000200ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_FIFO_DEPTH", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_FIFO_DEPTH(a) bdk_oclax_fifo_depth_t
#define bustype_BDK_OCLAX_FIFO_DEPTH(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_FIFO_DEPTH(a) "OCLAX_FIFO_DEPTH"
#define device_bar_BDK_OCLAX_FIFO_DEPTH(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_FIFO_DEPTH(a) (a)
#define arguments_BDK_OCLAX_FIFO_DEPTH(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_fifo_limit
 *
 * OCLA Capture FIFO Limit Registers
 */
union bdk_oclax_fifo_limit
{
    uint64_t u;
    struct bdk_oclax_fifo_limit_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t overfull              : 16; /**< [ 63: 48](R/W) Stop level. When OCLA()_FIFO_DEPTH \> [OVERFULL], stop capturing and set
                                                                 OCLA()_STATE_INT[OVERFULL]. This should be set to no more than
                                                                 OCLA()_CONST[DAT_SIZE] minus 26 when using DDR capture to insure that overflow can be
                                                                 detected. */
        uint64_t ddr                   : 16; /**< [ 47: 32](R/W) DDR level. When OCLA()_FIFO_DEPTH \> [DDR], FIFO entries will be removed, packed into a
                                                                 cache line, and overflowed to DDR/L2. All-ones disables overflow to DDR/L2. If nonzero
                                                                 must be at least 52. */
        uint64_t bp                    : 16; /**< [ 31: 16](R/W) Backpressure level. When OCLA()_FIFO_DEPTH \> [BP], OCLA will signal backpressure to
                                                                 coprocessors. All-ones disables indicating backpressure. */
        uint64_t wmark                 : 16; /**< [ 15:  0](R/W) Interrupt watermark level. When OCLA()_FIFO_DEPTH \> [WMARK], OCLA will set
                                                                 OCLA()_STATE_INT[WMARK] interrupt. All-ones disables setting the interrupt. */
#else /* Word 0 - Little Endian */
        uint64_t wmark                 : 16; /**< [ 15:  0](R/W) Interrupt watermark level. When OCLA()_FIFO_DEPTH \> [WMARK], OCLA will set
                                                                 OCLA()_STATE_INT[WMARK] interrupt. All-ones disables setting the interrupt. */
        uint64_t bp                    : 16; /**< [ 31: 16](R/W) Backpressure level. When OCLA()_FIFO_DEPTH \> [BP], OCLA will signal backpressure to
                                                                 coprocessors. All-ones disables indicating backpressure. */
        uint64_t ddr                   : 16; /**< [ 47: 32](R/W) DDR level. When OCLA()_FIFO_DEPTH \> [DDR], FIFO entries will be removed, packed into a
                                                                 cache line, and overflowed to DDR/L2. All-ones disables overflow to DDR/L2. If nonzero
                                                                 must be at least 52. */
        uint64_t overfull              : 16; /**< [ 63: 48](R/W) Stop level. When OCLA()_FIFO_DEPTH \> [OVERFULL], stop capturing and set
                                                                 OCLA()_STATE_INT[OVERFULL]. This should be set to no more than
                                                                 OCLA()_CONST[DAT_SIZE] minus 26 when using DDR capture to insure that overflow can be
                                                                 detected. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_fifo_limit_s cn8; */
    struct bdk_oclax_fifo_limit_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t overfull              : 16; /**< [ 63: 48](R/W) Stop level. When OCLA()_FIFO_DEPTH \> [OVERFULL], stop capturing and set
                                                                 OCLA()_STATE_INT[OVERFULL]. This should be set to no more than
                                                                 OCLA()_CONST[DAT_SIZE] minus 26 when using DDR capture to insure that overflow can be
                                                                 detected. */
        uint64_t ddr                   : 16; /**< [ 47: 32](R/W) DDR level. When OCLA()_FIFO_DEPTH \> [DDR], FIFO entries will be removed, packed into a
                                                                 cache line, and overflowed to LLC/DRAM. All-ones disables overflow to DDR/L2. If nonzero
                                                                 must be at least 52. */
        uint64_t bp                    : 16; /**< [ 31: 16](R/W) Backpressure level. When OCLA()_FIFO_DEPTH \> [BP], OCLA will signal backpressure to
                                                                 coprocessors. All-ones disables indicating backpressure. */
        uint64_t wmark                 : 16; /**< [ 15:  0](R/W) Interrupt watermark level. When OCLA()_FIFO_DEPTH \> [WMARK], OCLA will set
                                                                 OCLA()_STATE_INT[WMARK] interrupt. All-ones disables setting the interrupt. */
#else /* Word 0 - Little Endian */
        uint64_t wmark                 : 16; /**< [ 15:  0](R/W) Interrupt watermark level. When OCLA()_FIFO_DEPTH \> [WMARK], OCLA will set
                                                                 OCLA()_STATE_INT[WMARK] interrupt. All-ones disables setting the interrupt. */
        uint64_t bp                    : 16; /**< [ 31: 16](R/W) Backpressure level. When OCLA()_FIFO_DEPTH \> [BP], OCLA will signal backpressure to
                                                                 coprocessors. All-ones disables indicating backpressure. */
        uint64_t ddr                   : 16; /**< [ 47: 32](R/W) DDR level. When OCLA()_FIFO_DEPTH \> [DDR], FIFO entries will be removed, packed into a
                                                                 cache line, and overflowed to LLC/DRAM. All-ones disables overflow to DDR/L2. If nonzero
                                                                 must be at least 52. */
        uint64_t overfull              : 16; /**< [ 63: 48](R/W) Stop level. When OCLA()_FIFO_DEPTH \> [OVERFULL], stop capturing and set
                                                                 OCLA()_STATE_INT[OVERFULL]. This should be set to no more than
                                                                 OCLA()_CONST[DAT_SIZE] minus 26 when using DDR capture to insure that overflow can be
                                                                 detected. */
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_oclax_fifo_limit bdk_oclax_fifo_limit_t;

static inline uint64_t BDK_OCLAX_FIFO_LIMIT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_FIFO_LIMIT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a8000240ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a8000240ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a8000240ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000240ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_FIFO_LIMIT", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_FIFO_LIMIT(a) bdk_oclax_fifo_limit_t
#define bustype_BDK_OCLAX_FIFO_LIMIT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_FIFO_LIMIT(a) "OCLAX_FIFO_LIMIT"
#define device_bar_BDK_OCLAX_FIFO_LIMIT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_FIFO_LIMIT(a) (a)
#define arguments_BDK_OCLAX_FIFO_LIMIT(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_fifo_tail
 *
 * OCLA Capture FIFO Tail Registers
 */
union bdk_oclax_fifo_tail
{
    uint64_t u;
    struct bdk_oclax_fifo_tail_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t tail                  : 16; /**< [ 15:  0](RO/H) Address last written into entry FIFO. */
#else /* Word 0 - Little Endian */
        uint64_t tail                  : 16; /**< [ 15:  0](RO/H) Address last written into entry FIFO. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_fifo_tail_s cn; */
};
typedef union bdk_oclax_fifo_tail bdk_oclax_fifo_tail_t;

static inline uint64_t BDK_OCLAX_FIFO_TAIL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_FIFO_TAIL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a8000260ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a8000260ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a8000260ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000260ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_FIFO_TAIL", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_FIFO_TAIL(a) bdk_oclax_fifo_tail_t
#define bustype_BDK_OCLAX_FIFO_TAIL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_FIFO_TAIL(a) "OCLAX_FIFO_TAIL"
#define device_bar_BDK_OCLAX_FIFO_TAIL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_FIFO_TAIL(a) (a)
#define arguments_BDK_OCLAX_FIFO_TAIL(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_fifo_trig
 *
 * OCLA Capture FIFO Trigger Level Registers
 */
union bdk_oclax_fifo_trig
{
    uint64_t u;
    struct bdk_oclax_fifo_trig_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t limit                 : 16; /**< [ 31: 16](R/W) Post-trigger number of entries to collect before stopping collection. If zero, collection
                                                                 will never stop, which may be desirable when overflowing to DDR/L2. Must be \<
                                                                 OCLA()_CONST[DAT_SIZE] - 5. */
        uint64_t cnt                   : 16; /**< [ 15:  0](R/W/H) Number of entries collected since trigger. Cleared when OCLA()_STATE_INT[TRIG] clear. */
#else /* Word 0 - Little Endian */
        uint64_t cnt                   : 16; /**< [ 15:  0](R/W/H) Number of entries collected since trigger. Cleared when OCLA()_STATE_INT[TRIG] clear. */
        uint64_t limit                 : 16; /**< [ 31: 16](R/W) Post-trigger number of entries to collect before stopping collection. If zero, collection
                                                                 will never stop, which may be desirable when overflowing to DDR/L2. Must be \<
                                                                 OCLA()_CONST[DAT_SIZE] - 5. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_fifo_trig_s cn8; */
    struct bdk_oclax_fifo_trig_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t limit                 : 16; /**< [ 31: 16](R/W) Post-trigger number of entries to collect before stopping collection. If zero, collection
                                                                 will never stop, which may be desirable when overflowing to LLC/DRAM. Must be \<
                                                                 OCLA()_CONST[DAT_SIZE] - 5. */
        uint64_t cnt                   : 16; /**< [ 15:  0](R/W/H) Number of entries collected since trigger. Cleared when OCLA()_STATE_INT[TRIG] clear. */
#else /* Word 0 - Little Endian */
        uint64_t cnt                   : 16; /**< [ 15:  0](R/W/H) Number of entries collected since trigger. Cleared when OCLA()_STATE_INT[TRIG] clear. */
        uint64_t limit                 : 16; /**< [ 31: 16](R/W) Post-trigger number of entries to collect before stopping collection. If zero, collection
                                                                 will never stop, which may be desirable when overflowing to LLC/DRAM. Must be \<
                                                                 OCLA()_CONST[DAT_SIZE] - 5. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } cn9;
};
typedef union bdk_oclax_fifo_trig bdk_oclax_fifo_trig_t;

static inline uint64_t BDK_OCLAX_FIFO_TRIG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_FIFO_TRIG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a80002a0ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a80002a0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a80002a0ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b00002a0ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_FIFO_TRIG", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_FIFO_TRIG(a) bdk_oclax_fifo_trig_t
#define bustype_BDK_OCLAX_FIFO_TRIG(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_FIFO_TRIG(a) "OCLAX_FIFO_TRIG"
#define device_bar_BDK_OCLAX_FIFO_TRIG(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_FIFO_TRIG(a) (a)
#define arguments_BDK_OCLAX_FIFO_TRIG(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_fifo_wrap
 *
 * OCLA Capture FIFO Wrap Counter Registers
 */
union bdk_oclax_fifo_wrap
{
    uint64_t u;
    struct bdk_oclax_fifo_wrap_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t wraps                 : 32; /**< [ 31:  0](R/W/H) Number of times FIFO has wrapped since trigger.
                                                                 Cleared when OCLA()_STATE_INT[TRIG] is clear.
                                                                 This count has a one cycle lag observing when a trigger event occurs. */
#else /* Word 0 - Little Endian */
        uint64_t wraps                 : 32; /**< [ 31:  0](R/W/H) Number of times FIFO has wrapped since trigger.
                                                                 Cleared when OCLA()_STATE_INT[TRIG] is clear.
                                                                 This count has a one cycle lag observing when a trigger event occurs. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_fifo_wrap_s cn; */
};
typedef union bdk_oclax_fifo_wrap bdk_oclax_fifo_wrap_t;

static inline uint64_t BDK_OCLAX_FIFO_WRAP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_FIFO_WRAP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a8000280ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a8000280ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a8000280ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000280ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_FIFO_WRAP", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_FIFO_WRAP(a) bdk_oclax_fifo_wrap_t
#define bustype_BDK_OCLAX_FIFO_WRAP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_FIFO_WRAP(a) "OCLAX_FIFO_WRAP"
#define device_bar_BDK_OCLAX_FIFO_WRAP(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_FIFO_WRAP(a) (a)
#define arguments_BDK_OCLAX_FIFO_WRAP(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_fsm#_and#_i#
 *
 * OCLA FSM PLA AND Tree Registers
 * Values for PLA-AND plane. AND(0..15) represents the 16 allowed AND terms. I(0..1) for I=0
 * indicates the term non-inverted, for I=1 indicates the term inverted. Any AND tree may be
 * disabled by setting the same bit in both _I(0) and _I(1), as '((1) & !(1))' is always false.
 */
union bdk_oclax_fsmx_andx_ix
{
    uint64_t u;
    struct bdk_oclax_fsmx_andx_ix_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t trig                  : 1;  /**< [ 15: 15](R/W) AND plane control for trigger FSM input. */
        uint64_t mcd                   : 3;  /**< [ 14: 12](R/W) AND plane control for multichip debug (MCD) 0..2 FSM inputs. */
        uint64_t match                 : 4;  /**< [ 11:  8](R/W) AND plane control for matcher 0..3 FSM inputs. */
        uint64_t fsm1_state            : 4;  /**< [  7:  4](R/W) AND plane control for FSM 1 last state input. */
        uint64_t fsm0_state            : 4;  /**< [  3:  0](R/W) AND plane control for FSM 0 last state input. */
#else /* Word 0 - Little Endian */
        uint64_t fsm0_state            : 4;  /**< [  3:  0](R/W) AND plane control for FSM 0 last state input. */
        uint64_t fsm1_state            : 4;  /**< [  7:  4](R/W) AND plane control for FSM 1 last state input. */
        uint64_t match                 : 4;  /**< [ 11:  8](R/W) AND plane control for matcher 0..3 FSM inputs. */
        uint64_t mcd                   : 3;  /**< [ 14: 12](R/W) AND plane control for multichip debug (MCD) 0..2 FSM inputs. */
        uint64_t trig                  : 1;  /**< [ 15: 15](R/W) AND plane control for trigger FSM input. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    struct bdk_oclax_fsmx_andx_ix_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t trig                  : 1;  /**< [ 15: 15](RAZ) Reserved. */
        uint64_t mcd                   : 3;  /**< [ 14: 12](R/W) AND plane control for multichip debug (MCD) 0..2 FSM inputs. */
        uint64_t match                 : 4;  /**< [ 11:  8](R/W) AND plane control for matcher 0..3 FSM inputs. */
        uint64_t fsm1_state            : 4;  /**< [  7:  4](R/W) AND plane control for FSM 1 last state input. */
        uint64_t fsm0_state            : 4;  /**< [  3:  0](R/W) AND plane control for FSM 0 last state input. */
#else /* Word 0 - Little Endian */
        uint64_t fsm0_state            : 4;  /**< [  3:  0](R/W) AND plane control for FSM 0 last state input. */
        uint64_t fsm1_state            : 4;  /**< [  7:  4](R/W) AND plane control for FSM 1 last state input. */
        uint64_t match                 : 4;  /**< [ 11:  8](R/W) AND plane control for matcher 0..3 FSM inputs. */
        uint64_t mcd                   : 3;  /**< [ 14: 12](R/W) AND plane control for multichip debug (MCD) 0..2 FSM inputs. */
        uint64_t trig                  : 1;  /**< [ 15: 15](RAZ) Reserved. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } cn88xxp1;
    /* struct bdk_oclax_fsmx_andx_ix_s cn9; */
    /* struct bdk_oclax_fsmx_andx_ix_s cn81xx; */
    /* struct bdk_oclax_fsmx_andx_ix_s cn83xx; */
    /* struct bdk_oclax_fsmx_andx_ix_s cn88xxp2; */
};
typedef union bdk_oclax_fsmx_andx_ix bdk_oclax_fsmx_andx_ix_t;

static inline uint64_t BDK_OCLAX_FSMX_ANDX_IX(unsigned long a, unsigned long b, unsigned long c, unsigned long d) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_FSMX_ANDX_IX(unsigned long a, unsigned long b, unsigned long c, unsigned long d)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=1) && (c<=15) && (d<=1)))
        return 0x87e0a8300000ll + 0x1000000ll * ((a) & 0x1) + 0x1000ll * ((b) & 0x1) + 0x10ll * ((c) & 0xf) + 8ll * ((d) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b<=1) && (c<=15) && (d<=1)))
        return 0x87e0a8300000ll + 0x1000000ll * ((a) & 0x3) + 0x1000ll * ((b) & 0x1) + 0x10ll * ((c) & 0xf) + 8ll * ((d) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=4) && (b<=1) && (c<=15) && (d<=1)))
        return 0x87e0a8300000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x1) + 0x10ll * ((c) & 0xf) + 8ll * ((d) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b<=1) && (c<=15) && (d<=1)))
        return 0x87e0b0300000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x1) + 0x10ll * ((c) & 0xf) + 8ll * ((d) & 0x1);
    __bdk_csr_fatal("OCLAX_FSMX_ANDX_IX", 4, a, b, c, d);
}

#define typedef_BDK_OCLAX_FSMX_ANDX_IX(a,b,c,d) bdk_oclax_fsmx_andx_ix_t
#define bustype_BDK_OCLAX_FSMX_ANDX_IX(a,b,c,d) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_FSMX_ANDX_IX(a,b,c,d) "OCLAX_FSMX_ANDX_IX"
#define device_bar_BDK_OCLAX_FSMX_ANDX_IX(a,b,c,d) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_FSMX_ANDX_IX(a,b,c,d) (a)
#define arguments_BDK_OCLAX_FSMX_ANDX_IX(a,b,c,d) (a),(b),(c),(d)

/**
 * Register (RSL) ocla#_fsm#_or#
 *
 * OCLA FSM PLA AND Tree Registers
 */
union bdk_oclax_fsmx_orx
{
    uint64_t u;
    struct bdk_oclax_fsmx_orx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t or_state              : 16; /**< [ 15:  0](R/W) Column to drive on PLA OR-plane. */
#else /* Word 0 - Little Endian */
        uint64_t or_state              : 16; /**< [ 15:  0](R/W) Column to drive on PLA OR-plane. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_fsmx_orx_s cn; */
};
typedef union bdk_oclax_fsmx_orx bdk_oclax_fsmx_orx_t;

static inline uint64_t BDK_OCLAX_FSMX_ORX(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_FSMX_ORX(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=1) && (c<=15)))
        return 0x87e0a8310000ll + 0x1000000ll * ((a) & 0x1) + 0x1000ll * ((b) & 0x1) + 8ll * ((c) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b<=1) && (c<=15)))
        return 0x87e0a8310000ll + 0x1000000ll * ((a) & 0x3) + 0x1000ll * ((b) & 0x1) + 8ll * ((c) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=4) && (b<=1) && (c<=15)))
        return 0x87e0a8310000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x1) + 8ll * ((c) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b<=1) && (c<=15)))
        return 0x87e0b0310000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x1) + 8ll * ((c) & 0xf);
    __bdk_csr_fatal("OCLAX_FSMX_ORX", 3, a, b, c, 0);
}

#define typedef_BDK_OCLAX_FSMX_ORX(a,b,c) bdk_oclax_fsmx_orx_t
#define bustype_BDK_OCLAX_FSMX_ORX(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_FSMX_ORX(a,b,c) "OCLAX_FSMX_ORX"
#define device_bar_BDK_OCLAX_FSMX_ORX(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_FSMX_ORX(a,b,c) (a)
#define arguments_BDK_OCLAX_FSMX_ORX(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) ocla#_fsm#_state#
 *
 * OCLA FSM State Registers
 * See the OCLA chapter text for more details on each of these actions.
 */
union bdk_oclax_fsmx_statex
{
    uint64_t u;
    struct bdk_oclax_fsmx_statex_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_31_63        : 33;
        uint64_t sinfo_set             : 1;  /**< [ 30: 30](R/W) If a control packet is generated in this state (due to capture starting
                                                                 in the next cycle), set OCLA_CAP_CTL_S[SINFO]. */
        uint64_t set_int               : 1;  /**< [ 29: 29](R/W) In this state set interrupt. */
        uint64_t cap                   : 1;  /**< [ 28: 28](R/W) In this state request capture this cycle. */
        uint64_t set_mcd               : 3;  /**< [ 27: 25](R/W) In this state set MCD. */
        uint64_t set_trig              : 1;  /**< [ 24: 24](R/W) In this state set internal trigger indication. */
        uint64_t reserved_20_23        : 4;
        uint64_t set_val               : 4;  /**< [ 19: 16](R/W) In this state store match value into matcher 0..3. */
        uint64_t reserved_12_15        : 4;
        uint64_t clr_cnt               : 4;  /**< [ 11:  8](R/W) In this state clear match counter. */
        uint64_t reserved_4_7          : 4;
        uint64_t inc_cnt               : 4;  /**< [  3:  0](R/W) In this state increment match counter. */
#else /* Word 0 - Little Endian */
        uint64_t inc_cnt               : 4;  /**< [  3:  0](R/W) In this state increment match counter. */
        uint64_t reserved_4_7          : 4;
        uint64_t clr_cnt               : 4;  /**< [ 11:  8](R/W) In this state clear match counter. */
        uint64_t reserved_12_15        : 4;
        uint64_t set_val               : 4;  /**< [ 19: 16](R/W) In this state store match value into matcher 0..3. */
        uint64_t reserved_20_23        : 4;
        uint64_t set_trig              : 1;  /**< [ 24: 24](R/W) In this state set internal trigger indication. */
        uint64_t set_mcd               : 3;  /**< [ 27: 25](R/W) In this state set MCD. */
        uint64_t cap                   : 1;  /**< [ 28: 28](R/W) In this state request capture this cycle. */
        uint64_t set_int               : 1;  /**< [ 29: 29](R/W) In this state set interrupt. */
        uint64_t sinfo_set             : 1;  /**< [ 30: 30](R/W) If a control packet is generated in this state (due to capture starting
                                                                 in the next cycle), set OCLA_CAP_CTL_S[SINFO]. */
        uint64_t reserved_31_63        : 33;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_fsmx_statex_s cn; */
};
typedef union bdk_oclax_fsmx_statex bdk_oclax_fsmx_statex_t;

static inline uint64_t BDK_OCLAX_FSMX_STATEX(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_FSMX_STATEX(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=1) && (c<=15)))
        return 0x87e0a8320000ll + 0x1000000ll * ((a) & 0x1) + 0x1000ll * ((b) & 0x1) + 8ll * ((c) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b<=1) && (c<=15)))
        return 0x87e0a8320000ll + 0x1000000ll * ((a) & 0x3) + 0x1000ll * ((b) & 0x1) + 8ll * ((c) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=4) && (b<=1) && (c<=15)))
        return 0x87e0a8320000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x1) + 8ll * ((c) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b<=1) && (c<=15)))
        return 0x87e0b0320000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x1) + 8ll * ((c) & 0xf);
    __bdk_csr_fatal("OCLAX_FSMX_STATEX", 3, a, b, c, 0);
}

#define typedef_BDK_OCLAX_FSMX_STATEX(a,b,c) bdk_oclax_fsmx_statex_t
#define bustype_BDK_OCLAX_FSMX_STATEX(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_FSMX_STATEX(a,b,c) "OCLAX_FSMX_STATEX"
#define device_bar_BDK_OCLAX_FSMX_STATEX(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_FSMX_STATEX(a,b,c) (a)
#define arguments_BDK_OCLAX_FSMX_STATEX(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) ocla#_gen_ctl
 *
 * OCLA General Control Registers
 */
union bdk_oclax_gen_ctl
{
    uint64_t u;
    struct bdk_oclax_gen_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_7_63         : 57;
        uint64_t mcdtrig               : 3;  /**< [  6:  4](R/W) Enable MCD triggering. For each bit corresponding to the three MCDs:
                                                                 0 = MCD does not cause trigger.
                                                                 1 = When the corresponding MCD is received it will cause
                                                                 triggering and set OCLA()_STATE_SET[TRIG]. */
        uint64_t exten                 : 1;  /**< [  3:  3](R/W) Enable external triggering.
                                                                 0 = External triggering ignored.
                                                                 1 = When the external trigger pin selected with GPIO_PIN_SEL_E::OCLA_EXT_TRIGGER
                                                                 is high it will cause
                                                                 triggering and set OCLA()_STATE_SET[TRIG]. The external device must de-assert the
                                                                 signal (it is not edge sensitive.) */
        uint64_t den                   : 1;  /**< [  2:  2](R/W) Enable data bus and counter clocking. When set, the OCLA inbound data bus may be used and
                                                                 counters may increment. When clear, the bus is always zero and internal flops may be clock
                                                                 gated off to save power. Must be set for normal operation. */
        uint64_t stt                   : 1;  /**< [  1:  1](R/W) Store to DDR directly, bypassing L2 cache. */
        uint64_t reserved_0            : 1;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0            : 1;
        uint64_t stt                   : 1;  /**< [  1:  1](R/W) Store to DDR directly, bypassing L2 cache. */
        uint64_t den                   : 1;  /**< [  2:  2](R/W) Enable data bus and counter clocking. When set, the OCLA inbound data bus may be used and
                                                                 counters may increment. When clear, the bus is always zero and internal flops may be clock
                                                                 gated off to save power. Must be set for normal operation. */
        uint64_t exten                 : 1;  /**< [  3:  3](R/W) Enable external triggering.
                                                                 0 = External triggering ignored.
                                                                 1 = When the external trigger pin selected with GPIO_PIN_SEL_E::OCLA_EXT_TRIGGER
                                                                 is high it will cause
                                                                 triggering and set OCLA()_STATE_SET[TRIG]. The external device must de-assert the
                                                                 signal (it is not edge sensitive.) */
        uint64_t mcdtrig               : 3;  /**< [  6:  4](R/W) Enable MCD triggering. For each bit corresponding to the three MCDs:
                                                                 0 = MCD does not cause trigger.
                                                                 1 = When the corresponding MCD is received it will cause
                                                                 triggering and set OCLA()_STATE_SET[TRIG]. */
        uint64_t reserved_7_63         : 57;
#endif /* Word 0 - End */
    } s;
    struct bdk_oclax_gen_ctl_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_7_63         : 57;
        uint64_t mcdtrig               : 3;  /**< [  6:  4](RAZ) Reserved. */
        uint64_t exten                 : 1;  /**< [  3:  3](R/W) Enable external triggering.
                                                                 0 = External triggering ignored.
                                                                 1 = When the external trigger pin selected with GPIO_PIN_SEL_E::OCLA_EXT_TRIGGER
                                                                 is high it will cause
                                                                 triggering and set OCLA()_STATE_SET[TRIG]. The external device must de-assert the
                                                                 signal (it is not edge sensitive.) */
        uint64_t den                   : 1;  /**< [  2:  2](R/W) Enable data bus and counter clocking. When set, the OCLA inbound data bus may be used and
                                                                 counters may increment. When clear, the bus is always zero and internal flops may be clock
                                                                 gated off to save power. Must be set for normal operation. */
        uint64_t stt                   : 1;  /**< [  1:  1](R/W) Store to DDR directly, bypassing L2 cache. */
        uint64_t reserved_0            : 1;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0            : 1;
        uint64_t stt                   : 1;  /**< [  1:  1](R/W) Store to DDR directly, bypassing L2 cache. */
        uint64_t den                   : 1;  /**< [  2:  2](R/W) Enable data bus and counter clocking. When set, the OCLA inbound data bus may be used and
                                                                 counters may increment. When clear, the bus is always zero and internal flops may be clock
                                                                 gated off to save power. Must be set for normal operation. */
        uint64_t exten                 : 1;  /**< [  3:  3](R/W) Enable external triggering.
                                                                 0 = External triggering ignored.
                                                                 1 = When the external trigger pin selected with GPIO_PIN_SEL_E::OCLA_EXT_TRIGGER
                                                                 is high it will cause
                                                                 triggering and set OCLA()_STATE_SET[TRIG]. The external device must de-assert the
                                                                 signal (it is not edge sensitive.) */
        uint64_t mcdtrig               : 3;  /**< [  6:  4](RAZ) Reserved. */
        uint64_t reserved_7_63         : 57;
#endif /* Word 0 - End */
    } cn88xxp1;
    struct bdk_oclax_gen_ctl_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_7_63         : 57;
        uint64_t mcdtrig               : 3;  /**< [  6:  4](R/W) Enable MCD triggering. For each bit corresponding to the three MCDs:
                                                                 0 = MCD does not cause trigger.
                                                                 1 = When the corresponding MCD is received it will cause
                                                                 triggering and set OCLA()_STATE_SET[TRIG]. */
        uint64_t exten                 : 1;  /**< [  3:  3](R/W) Enable external triggering.
                                                                 0 = External triggering ignored.
                                                                 1 = When the external trigger pin selected with GPIO_PIN_SEL_E::OCLA_EXT_TRIGGER
                                                                 is high it will cause
                                                                 triggering and set OCLA()_STATE_SET[TRIG]. The external device must de-assert the
                                                                 signal (it is not edge sensitive.) */
        uint64_t den                   : 1;  /**< [  2:  2](R/W) Enable data bus and counter clocking. When set, the OCLA inbound data bus may be used and
                                                                 counters may increment. When clear, the bus is always zero and internal flops may be clock
                                                                 gated off to save power. Must be set for normal operation. */
        uint64_t stt                   : 1;  /**< [  1:  1](R/W) Store to DRAM directly, bypassing LLC. */
        uint64_t reserved_0            : 1;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0            : 1;
        uint64_t stt                   : 1;  /**< [  1:  1](R/W) Store to DRAM directly, bypassing LLC. */
        uint64_t den                   : 1;  /**< [  2:  2](R/W) Enable data bus and counter clocking. When set, the OCLA inbound data bus may be used and
                                                                 counters may increment. When clear, the bus is always zero and internal flops may be clock
                                                                 gated off to save power. Must be set for normal operation. */
        uint64_t exten                 : 1;  /**< [  3:  3](R/W) Enable external triggering.
                                                                 0 = External triggering ignored.
                                                                 1 = When the external trigger pin selected with GPIO_PIN_SEL_E::OCLA_EXT_TRIGGER
                                                                 is high it will cause
                                                                 triggering and set OCLA()_STATE_SET[TRIG]. The external device must de-assert the
                                                                 signal (it is not edge sensitive.) */
        uint64_t mcdtrig               : 3;  /**< [  6:  4](R/W) Enable MCD triggering. For each bit corresponding to the three MCDs:
                                                                 0 = MCD does not cause trigger.
                                                                 1 = When the corresponding MCD is received it will cause
                                                                 triggering and set OCLA()_STATE_SET[TRIG]. */
        uint64_t reserved_7_63         : 57;
#endif /* Word 0 - End */
    } cn9;
    /* struct bdk_oclax_gen_ctl_s cn81xx; */
    /* struct bdk_oclax_gen_ctl_s cn83xx; */
    /* struct bdk_oclax_gen_ctl_s cn88xxp2; */
};
typedef union bdk_oclax_gen_ctl bdk_oclax_gen_ctl_t;

static inline uint64_t BDK_OCLAX_GEN_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_GEN_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a8000060ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a8000060ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a8000060ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000060ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_GEN_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_GEN_CTL(a) bdk_oclax_gen_ctl_t
#define bustype_BDK_OCLAX_GEN_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_GEN_CTL(a) "OCLAX_GEN_CTL"
#define device_bar_BDK_OCLAX_GEN_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_GEN_CTL(a) (a)
#define arguments_BDK_OCLAX_GEN_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_mat#_count
 *
 * OCLA Matcher Count Registers
 */
union bdk_oclax_matx_count
{
    uint64_t u;
    struct bdk_oclax_matx_count_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t count                 : 32; /**< [ 31:  0](R/W/H) Current counter value. Note software must reset this to zero (or the appropriate count)
                                                                 before starting capture. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 32; /**< [ 31:  0](R/W/H) Current counter value. Note software must reset this to zero (or the appropriate count)
                                                                 before starting capture. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_matx_count_s cn; */
};
typedef union bdk_oclax_matx_count bdk_oclax_matx_count_t;

static inline uint64_t BDK_OCLAX_MATX_COUNT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_MATX_COUNT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=3)))
        return 0x87e0a8230000ll + 0x1000000ll * ((a) & 0x1) + 0x1000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b<=3)))
        return 0x87e0a8230000ll + 0x1000000ll * ((a) & 0x3) + 0x1000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=4) && (b<=3)))
        return 0x87e0a8230000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b<=3)))
        return 0x87e0b0230000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x3);
    __bdk_csr_fatal("OCLAX_MATX_COUNT", 2, a, b, 0, 0);
}

#define typedef_BDK_OCLAX_MATX_COUNT(a,b) bdk_oclax_matx_count_t
#define bustype_BDK_OCLAX_MATX_COUNT(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_MATX_COUNT(a,b) "OCLAX_MATX_COUNT"
#define device_bar_BDK_OCLAX_MATX_COUNT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_MATX_COUNT(a,b) (a)
#define arguments_BDK_OCLAX_MATX_COUNT(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocla#_mat#_ctl
 *
 * OCLA Matcher Control Registers
 */
union bdk_oclax_matx_ctl
{
    uint64_t u;
    struct bdk_oclax_matx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t fsm_ctr               : 1;  /**< [  7:  7](R/W) What output matcher provides to FSM:
                                                                 0 = FSM receives raw match signal, asserting only in those cycles with matches.
                                                                 1 = FSM receives OCLA()_MAT()_COUNT \>= OCLA()_MAT()_THRESH. */
        uint64_t inc_match             : 1;  /**< [  6:  6](R/W) Increment OCLA()_MAT()_COUNT counter automatically on each match. */
        uint64_t shift                 : 6;  /**< [  5:  0](R/W) Right rotation amount to apply to data loaded into OCLA()_MAT()_VALUE()
                                                                 register when FSM requests a value load. */
#else /* Word 0 - Little Endian */
        uint64_t shift                 : 6;  /**< [  5:  0](R/W) Right rotation amount to apply to data loaded into OCLA()_MAT()_VALUE()
                                                                 register when FSM requests a value load. */
        uint64_t inc_match             : 1;  /**< [  6:  6](R/W) Increment OCLA()_MAT()_COUNT counter automatically on each match. */
        uint64_t fsm_ctr               : 1;  /**< [  7:  7](R/W) What output matcher provides to FSM:
                                                                 0 = FSM receives raw match signal, asserting only in those cycles with matches.
                                                                 1 = FSM receives OCLA()_MAT()_COUNT \>= OCLA()_MAT()_THRESH. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_matx_ctl_s cn; */
};
typedef union bdk_oclax_matx_ctl bdk_oclax_matx_ctl_t;

static inline uint64_t BDK_OCLAX_MATX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_MATX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=3)))
        return 0x87e0a8200000ll + 0x1000000ll * ((a) & 0x1) + 0x1000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b<=3)))
        return 0x87e0a8200000ll + 0x1000000ll * ((a) & 0x3) + 0x1000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=4) && (b<=3)))
        return 0x87e0a8200000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b<=3)))
        return 0x87e0b0200000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x3);
    __bdk_csr_fatal("OCLAX_MATX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_OCLAX_MATX_CTL(a,b) bdk_oclax_matx_ctl_t
#define bustype_BDK_OCLAX_MATX_CTL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_MATX_CTL(a,b) "OCLAX_MATX_CTL"
#define device_bar_BDK_OCLAX_MATX_CTL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_MATX_CTL(a,b) (a)
#define arguments_BDK_OCLAX_MATX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocla#_mat#_mask#
 *
 * OCLA Matcher Compare Mask Registers
 */
union bdk_oclax_matx_maskx
{
    uint64_t u;
    struct bdk_oclax_matx_maskx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t mask                  : 36; /**< [ 35:  0](R/W) Bitmask of which bits in OCLA()_MAT()_VALUE() are to be compared.

                                                                 Each bit of OCLA()_MAT()_VALUE() and OCLA()_MAT()_MASK() are combined as
                                                                 follows:

                                                                 _ If MASK = 1 and VALUE = 0, matches when corresponding bit of data = "0".
                                                                 _ If MASK = 1 and VALUE = 1, matches when corresponding bit of data = "1".
                                                                 _ If MASK = 0, matches regardless of corresponding bit of data. */
#else /* Word 0 - Little Endian */
        uint64_t mask                  : 36; /**< [ 35:  0](R/W) Bitmask of which bits in OCLA()_MAT()_VALUE() are to be compared.

                                                                 Each bit of OCLA()_MAT()_VALUE() and OCLA()_MAT()_MASK() are combined as
                                                                 follows:

                                                                 _ If MASK = 1 and VALUE = 0, matches when corresponding bit of data = "0".
                                                                 _ If MASK = 1 and VALUE = 1, matches when corresponding bit of data = "1".
                                                                 _ If MASK = 0, matches regardless of corresponding bit of data. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_matx_maskx_s cn9; */
    /* struct bdk_oclax_matx_maskx_s cn81xx; */
    struct bdk_oclax_matx_maskx_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t mask                  : 36; /**< [ 35:  0](R/W) Bitmask of which bits in OCLA()_MAT()_VALUE() are to be compared.

                                                                 Each bit of OCLA()_MAT()_VALUE() and OCLA()_MAT()_MASK() are combined as
                                                                 follows:

                                                                 _ If MASK = 1 and VALUE = 0, matches when data = "0".
                                                                 _ If MASK = 1 and VALUE = 1, matches when data = "1".
                                                                 _ If MASK = 0 and VALUE = 0, matches any data.
                                                                 _ If MASK = 0 and VALUE = 1, reserved in pass 1, matches any data pass 2 and later. */
#else /* Word 0 - Little Endian */
        uint64_t mask                  : 36; /**< [ 35:  0](R/W) Bitmask of which bits in OCLA()_MAT()_VALUE() are to be compared.

                                                                 Each bit of OCLA()_MAT()_VALUE() and OCLA()_MAT()_MASK() are combined as
                                                                 follows:

                                                                 _ If MASK = 1 and VALUE = 0, matches when data = "0".
                                                                 _ If MASK = 1 and VALUE = 1, matches when data = "1".
                                                                 _ If MASK = 0 and VALUE = 0, matches any data.
                                                                 _ If MASK = 0 and VALUE = 1, reserved in pass 1, matches any data pass 2 and later. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } cn88xx;
    /* struct bdk_oclax_matx_maskx_s cn83xx; */
};
typedef union bdk_oclax_matx_maskx bdk_oclax_matx_maskx_t;

static inline uint64_t BDK_OCLAX_MATX_MASKX(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_MATX_MASKX(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=3) && (c<=1)))
        return 0x87e0a8220000ll + 0x1000000ll * ((a) & 0x1) + 0x1000ll * ((b) & 0x3) + 8ll * ((c) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b<=3) && (c<=1)))
        return 0x87e0a8220000ll + 0x1000000ll * ((a) & 0x3) + 0x1000ll * ((b) & 0x3) + 8ll * ((c) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=4) && (b<=3) && (c<=1)))
        return 0x87e0a8220000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x3) + 8ll * ((c) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b<=3) && (c<=1)))
        return 0x87e0b0220000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x3) + 8ll * ((c) & 0x1);
    __bdk_csr_fatal("OCLAX_MATX_MASKX", 3, a, b, c, 0);
}

#define typedef_BDK_OCLAX_MATX_MASKX(a,b,c) bdk_oclax_matx_maskx_t
#define bustype_BDK_OCLAX_MATX_MASKX(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_MATX_MASKX(a,b,c) "OCLAX_MATX_MASKX"
#define device_bar_BDK_OCLAX_MATX_MASKX(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_MATX_MASKX(a,b,c) (a)
#define arguments_BDK_OCLAX_MATX_MASKX(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) ocla#_mat#_thresh
 *
 * OCLA Matcher Count Threshold Registers
 */
union bdk_oclax_matx_thresh
{
    uint64_t u;
    struct bdk_oclax_matx_thresh_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t thresh                : 32; /**< [ 31:  0](R/W) Counter threshold value. Compared against OCLA()_MAT()_COUNT to assert matcher
                                                                 output, and set OCLA()_STATE_INT[OVFL]. */
#else /* Word 0 - Little Endian */
        uint64_t thresh                : 32; /**< [ 31:  0](R/W) Counter threshold value. Compared against OCLA()_MAT()_COUNT to assert matcher
                                                                 output, and set OCLA()_STATE_INT[OVFL]. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_matx_thresh_s cn; */
};
typedef union bdk_oclax_matx_thresh bdk_oclax_matx_thresh_t;

static inline uint64_t BDK_OCLAX_MATX_THRESH(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_MATX_THRESH(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=3)))
        return 0x87e0a8240000ll + 0x1000000ll * ((a) & 0x1) + 0x1000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b<=3)))
        return 0x87e0a8240000ll + 0x1000000ll * ((a) & 0x3) + 0x1000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=4) && (b<=3)))
        return 0x87e0a8240000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b<=3)))
        return 0x87e0b0240000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x3);
    __bdk_csr_fatal("OCLAX_MATX_THRESH", 2, a, b, 0, 0);
}

#define typedef_BDK_OCLAX_MATX_THRESH(a,b) bdk_oclax_matx_thresh_t
#define bustype_BDK_OCLAX_MATX_THRESH(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_MATX_THRESH(a,b) "OCLAX_MATX_THRESH"
#define device_bar_BDK_OCLAX_MATX_THRESH(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_MATX_THRESH(a,b) (a)
#define arguments_BDK_OCLAX_MATX_THRESH(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocla#_mat#_value#
 *
 * OCLA Matcher Compare Value Registers
 */
union bdk_oclax_matx_valuex
{
    uint64_t u;
    struct bdk_oclax_matx_valuex_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t mask                  : 36; /**< [ 35:  0](R/W/H) Data value to compare against when corresponding bits of OCLA()_MAT()_MASK()
                                                                 are set. Value may be updated with OCLA()_FSM()_STATE()[SET_VAL]. */
#else /* Word 0 - Little Endian */
        uint64_t mask                  : 36; /**< [ 35:  0](R/W/H) Data value to compare against when corresponding bits of OCLA()_MAT()_MASK()
                                                                 are set. Value may be updated with OCLA()_FSM()_STATE()[SET_VAL]. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_matx_valuex_s cn; */
};
typedef union bdk_oclax_matx_valuex bdk_oclax_matx_valuex_t;

static inline uint64_t BDK_OCLAX_MATX_VALUEX(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_MATX_VALUEX(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=3) && (c<=1)))
        return 0x87e0a8210000ll + 0x1000000ll * ((a) & 0x1) + 0x1000ll * ((b) & 0x3) + 8ll * ((c) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b<=3) && (c<=1)))
        return 0x87e0a8210000ll + 0x1000000ll * ((a) & 0x3) + 0x1000ll * ((b) & 0x3) + 8ll * ((c) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=4) && (b<=3) && (c<=1)))
        return 0x87e0a8210000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x3) + 8ll * ((c) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b<=3) && (c<=1)))
        return 0x87e0b0210000ll + 0x1000000ll * ((a) & 0x7) + 0x1000ll * ((b) & 0x3) + 8ll * ((c) & 0x1);
    __bdk_csr_fatal("OCLAX_MATX_VALUEX", 3, a, b, c, 0);
}

#define typedef_BDK_OCLAX_MATX_VALUEX(a,b,c) bdk_oclax_matx_valuex_t
#define bustype_BDK_OCLAX_MATX_VALUEX(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_MATX_VALUEX(a,b,c) "OCLAX_MATX_VALUEX"
#define device_bar_BDK_OCLAX_MATX_VALUEX(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_MATX_VALUEX(a,b,c) (a)
#define arguments_BDK_OCLAX_MATX_VALUEX(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) ocla#_mparid
 *
 * OCLA Memory Partition ID Register
 */
union bdk_oclax_mparid
{
    uint64_t u;
    struct bdk_oclax_mparid_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t mparid                : 8;  /**< [  7:  0](R/W) Memory partition ID. Only used for OCLAs inside clusters. */
#else /* Word 0 - Little Endian */
        uint64_t mparid                : 8;  /**< [  7:  0](R/W) Memory partition ID. Only used for OCLAs inside clusters. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_mparid_s cn; */
};
typedef union bdk_oclax_mparid bdk_oclax_mparid_t;

static inline uint64_t BDK_OCLAX_MPARID(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_MPARID(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b00000e0ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_MPARID", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_MPARID(a) bdk_oclax_mparid_t
#define bustype_BDK_OCLAX_MPARID(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_MPARID(a) "OCLAX_MPARID"
#define device_bar_BDK_OCLAX_MPARID(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_MPARID(a) (a)
#define arguments_BDK_OCLAX_MPARID(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_msix_pba#
 *
 * OCLA MSI-X Pending Bit Array Registers
 * This register is the MSI-X PBA table; the bit number is indexed by the OCLA_INT_VEC_E enumeration.
 */
union bdk_oclax_msix_pbax
{
    uint64_t u;
    struct bdk_oclax_msix_pbax_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO/H) Pending message for the associated OCLA()_MSIX_VEC()_CTL, enumerated by OCLA_INT_VEC_E.
                                                                 Bits that have no associated OCLA_INT_VEC_E are 0. */
#else /* Word 0 - Little Endian */
        uint64_t pend                  : 64; /**< [ 63:  0](RO/H) Pending message for the associated OCLA()_MSIX_VEC()_CTL, enumerated by OCLA_INT_VEC_E.
                                                                 Bits that have no associated OCLA_INT_VEC_E are 0. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_msix_pbax_s cn; */
};
typedef union bdk_oclax_msix_pbax bdk_oclax_msix_pbax_t;

static inline uint64_t BDK_OCLAX_MSIX_PBAX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_MSIX_PBAX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b==0)))
        return 0x87e0a8ff0000ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b==0)))
        return 0x87e0a8ff0000ll + 0x1000000ll * ((a) & 0x3) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=4) && (b==0)))
        return 0x87e0a8ff0000ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b==0)))
        return 0x87e0b0ff0000ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x0);
    __bdk_csr_fatal("OCLAX_MSIX_PBAX", 2, a, b, 0, 0);
}

#define typedef_BDK_OCLAX_MSIX_PBAX(a,b) bdk_oclax_msix_pbax_t
#define bustype_BDK_OCLAX_MSIX_PBAX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_MSIX_PBAX(a,b) "OCLAX_MSIX_PBAX"
#define device_bar_BDK_OCLAX_MSIX_PBAX(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_OCLAX_MSIX_PBAX(a,b) (a)
#define arguments_BDK_OCLAX_MSIX_PBAX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocla#_msix_vec#_addr
 *
 * OCLA MSI-X Vector-Table Address Register
 * This register is the MSI-X vector table, indexed by the OCLA_INT_VEC_E enumeration.
 */
union bdk_oclax_msix_vecx_addr
{
    uint64_t u;
    struct bdk_oclax_msix_vecx_addr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_53_63        : 11;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's OCLA()_MSIX_VEC()_ADDR, OCLA()_MSIX_VEC()_CTL, and
                                                                 corresponding bit of OCLA()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_OCLA()_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's OCLA()_MSIX_VEC()_ADDR, OCLA()_MSIX_VEC()_CTL, and
                                                                 corresponding bit of OCLA()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_OCLA()_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 51; /**< [ 52:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_53_63        : 11;
#endif /* Word 0 - End */
    } s;
    struct bdk_oclax_msix_vecx_addr_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_1            : 1;
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's OCLA()_MSIX_VEC()_ADDR, OCLA()_MSIX_VEC()_CTL, and
                                                                 corresponding bit of OCLA()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_OCLA()_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
#else /* Word 0 - Little Endian */
        uint64_t secvec                : 1;  /**< [  0:  0](SR/W) Secure vector.
                                                                 0 = This vector may be read or written by either secure or nonsecure states.
                                                                 1 = This vector's OCLA()_MSIX_VEC()_ADDR, OCLA()_MSIX_VEC()_CTL, and
                                                                 corresponding bit of OCLA()_MSIX_PBA() are RAZ/WI and does not cause a fault when accessed
                                                                 by the nonsecure world.

                                                                 If PCCPF_OCLA()_VSEC_SCTL[MSIX_SEC] (for documentation, see
                                                                 PCCPF_XXX_VSEC_SCTL[MSIX_SEC]) is set, all vectors are secure and function as if [SECVEC]
                                                                 was set. */
        uint64_t reserved_1            : 1;
        uint64_t addr                  : 47; /**< [ 48:  2](R/W) IOVA to use for MSI-X delivery of this vector. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } cn8;
    /* struct bdk_oclax_msix_vecx_addr_s cn9; */
};
typedef union bdk_oclax_msix_vecx_addr bdk_oclax_msix_vecx_addr_t;

static inline uint64_t BDK_OCLAX_MSIX_VECX_ADDR(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_MSIX_VECX_ADDR(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b==0)))
        return 0x87e0a8f00000ll + 0x1000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b==0)))
        return 0x87e0a8f00000ll + 0x1000000ll * ((a) & 0x3) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=4) && (b==0)))
        return 0x87e0a8f00000ll + 0x1000000ll * ((a) & 0x7) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b==0)))
        return 0x87e0b0f00000ll + 0x1000000ll * ((a) & 0x7) + 0x10ll * ((b) & 0x0);
    __bdk_csr_fatal("OCLAX_MSIX_VECX_ADDR", 2, a, b, 0, 0);
}

#define typedef_BDK_OCLAX_MSIX_VECX_ADDR(a,b) bdk_oclax_msix_vecx_addr_t
#define bustype_BDK_OCLAX_MSIX_VECX_ADDR(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_MSIX_VECX_ADDR(a,b) "OCLAX_MSIX_VECX_ADDR"
#define device_bar_BDK_OCLAX_MSIX_VECX_ADDR(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_OCLAX_MSIX_VECX_ADDR(a,b) (a)
#define arguments_BDK_OCLAX_MSIX_VECX_ADDR(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocla#_msix_vec#_ctl
 *
 * OCLA MSI-X Vector-Table Control and Data Register
 * This register is the MSI-X vector table, indexed by the OCLA_INT_VEC_E enumeration.
 */
union bdk_oclax_msix_vecx_ctl
{
    uint64_t u;
    struct bdk_oclax_msix_vecx_ctl_s
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
    struct bdk_oclax_msix_vecx_ctl_cn8
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
    /* struct bdk_oclax_msix_vecx_ctl_s cn9; */
};
typedef union bdk_oclax_msix_vecx_ctl bdk_oclax_msix_vecx_ctl_t;

static inline uint64_t BDK_OCLAX_MSIX_VECX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_MSIX_VECX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b==0)))
        return 0x87e0a8f00008ll + 0x1000000ll * ((a) & 0x1) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b==0)))
        return 0x87e0a8f00008ll + 0x1000000ll * ((a) & 0x3) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=4) && (b==0)))
        return 0x87e0a8f00008ll + 0x1000000ll * ((a) & 0x7) + 0x10ll * ((b) & 0x0);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b==0)))
        return 0x87e0b0f00008ll + 0x1000000ll * ((a) & 0x7) + 0x10ll * ((b) & 0x0);
    __bdk_csr_fatal("OCLAX_MSIX_VECX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_OCLAX_MSIX_VECX_CTL(a,b) bdk_oclax_msix_vecx_ctl_t
#define bustype_BDK_OCLAX_MSIX_VECX_CTL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_MSIX_VECX_CTL(a,b) "OCLAX_MSIX_VECX_CTL"
#define device_bar_BDK_OCLAX_MSIX_VECX_CTL(a,b) 0x4 /* PF_BAR4 */
#define busnum_BDK_OCLAX_MSIX_VECX_CTL(a,b) (a)
#define arguments_BDK_OCLAX_MSIX_VECX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocla#_raw#
 *
 * OCLA Raw Input Registers
 */
union bdk_oclax_rawx
{
    uint64_t u;
    struct bdk_oclax_rawx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_36_63        : 28;
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw value of debug bus input signals into OCLA. */
#else /* Word 0 - Little Endian */
        uint64_t raw                   : 36; /**< [ 35:  0](RO/H) Raw value of debug bus input signals into OCLA. */
        uint64_t reserved_36_63        : 28;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_rawx_s cn; */
};
typedef union bdk_oclax_rawx bdk_oclax_rawx_t;

static inline uint64_t BDK_OCLAX_RAWX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_RAWX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=1)))
        return 0x87e0a8000100ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b<=1)))
        return 0x87e0a8000100ll + 0x1000000ll * ((a) & 0x3) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=4) && (b<=1)))
        return 0x87e0a8000100ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b<=1)))
        return 0x87e0b0000100ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x1);
    __bdk_csr_fatal("OCLAX_RAWX", 2, a, b, 0, 0);
}

#define typedef_BDK_OCLAX_RAWX(a,b) bdk_oclax_rawx_t
#define bustype_BDK_OCLAX_RAWX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_RAWX(a,b) "OCLAX_RAWX"
#define device_bar_BDK_OCLAX_RAWX(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_RAWX(a,b) (a)
#define arguments_BDK_OCLAX_RAWX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocla#_sft_rst
 *
 * OCLA Reset Registers
 */
union bdk_oclax_sft_rst
{
    uint64_t u;
    struct bdk_oclax_sft_rst_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t reset                 : 1;  /**< [  0:  0](R/W1) Reset. When written with one, reset OCLA excluding the RSL interface. Software
                                                                 must wait at least 1024 coprocessor-clocks after resetting before sending any
                                                                 other CSR read/write operations into OCLA. */
#else /* Word 0 - Little Endian */
        uint64_t reset                 : 1;  /**< [  0:  0](R/W1) Reset. When written with one, reset OCLA excluding the RSL interface. Software
                                                                 must wait at least 1024 coprocessor-clocks after resetting before sending any
                                                                 other CSR read/write operations into OCLA. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_sft_rst_s cn; */
};
typedef union bdk_oclax_sft_rst bdk_oclax_sft_rst_t;

static inline uint64_t BDK_OCLAX_SFT_RST(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_SFT_RST(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a8000020ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a8000020ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a8000020ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000020ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_SFT_RST", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_SFT_RST(a) bdk_oclax_sft_rst_t
#define bustype_BDK_OCLAX_SFT_RST(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_SFT_RST(a) "OCLAX_SFT_RST"
#define device_bar_BDK_OCLAX_SFT_RST(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_SFT_RST(a) (a)
#define arguments_BDK_OCLAX_SFT_RST(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_stack_base
 *
 * OCLA Stack Base Registers
 */
union bdk_oclax_stack_base
{
    uint64_t u;
    struct bdk_oclax_stack_base_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_53_63        : 11;
        uint64_t ptr                   : 46; /**< [ 52:  7](R/W) Memory address for base of overflow stack. This address must be on the local node in a
                                                                 CCPI system.
                                                                 This may be an IOVA or physical address; see [PA]. */
        uint64_t reserved_2_6          : 5;
        uint64_t sec                   : 1;  /**< [  1:  1](SR/W) If set, and physical addressing is used as described under [PA], the physical address
                                                                 is in the secure world. */
        uint64_t pa                    : 1;  /**< [  0:  0](R/W) When set, [PTR] and all DMA addresses are physical addresses and will not be translated by
                                                                 the SMMU.  When clear, is a virtual address which is subject to SMMU translation.

                                                                 Only used for OCLA(4) in the coprocessor-clock domain; for OCLA(0..3) in the core-clock
                                                                 domains this bit is ignored, addresses are always physical. */
#else /* Word 0 - Little Endian */
        uint64_t pa                    : 1;  /**< [  0:  0](R/W) When set, [PTR] and all DMA addresses are physical addresses and will not be translated by
                                                                 the SMMU.  When clear, is a virtual address which is subject to SMMU translation.

                                                                 Only used for OCLA(4) in the coprocessor-clock domain; for OCLA(0..3) in the core-clock
                                                                 domains this bit is ignored, addresses are always physical. */
        uint64_t sec                   : 1;  /**< [  1:  1](SR/W) If set, and physical addressing is used as described under [PA], the physical address
                                                                 is in the secure world. */
        uint64_t reserved_2_6          : 5;
        uint64_t ptr                   : 46; /**< [ 52:  7](R/W) Memory address for base of overflow stack. This address must be on the local node in a
                                                                 CCPI system.
                                                                 This may be an IOVA or physical address; see [PA]. */
        uint64_t reserved_53_63        : 11;
#endif /* Word 0 - End */
    } s;
    struct bdk_oclax_stack_base_cn9
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_53_63        : 11;
        uint64_t ptr                   : 46; /**< [ 52:  7](R/W) Memory address for base of overflow stack. This address must be on the local node in a
                                                                 CCPI system.
                                                                 This may be an IOVA or physical address; see [PA]. */
        uint64_t reserved_2_6          : 5;
        uint64_t sec                   : 1;  /**< [  1:  1](SR/W) If set, and physical addressing is used as described under [PA], the physical address
                                                                 is in the secure world. */
        uint64_t pa                    : 1;  /**< [  0:  0](R/W) When set, [PTR] and all DMA addresses are physical addresses and will not be translated by
                                                                 the SMMU.  When clear, is a virtual address which is subject to SMMU translation.

                                                                 Only used for the OCLA in the coprocessor-clock domain; for OCLAs in the core-clock
                                                                 domains this bit is ignored, addresses are always physical. */
#else /* Word 0 - Little Endian */
        uint64_t pa                    : 1;  /**< [  0:  0](R/W) When set, [PTR] and all DMA addresses are physical addresses and will not be translated by
                                                                 the SMMU.  When clear, is a virtual address which is subject to SMMU translation.

                                                                 Only used for the OCLA in the coprocessor-clock domain; for OCLAs in the core-clock
                                                                 domains this bit is ignored, addresses are always physical. */
        uint64_t sec                   : 1;  /**< [  1:  1](SR/W) If set, and physical addressing is used as described under [PA], the physical address
                                                                 is in the secure world. */
        uint64_t reserved_2_6          : 5;
        uint64_t ptr                   : 46; /**< [ 52:  7](R/W) Memory address for base of overflow stack. This address must be on the local node in a
                                                                 CCPI system.
                                                                 This may be an IOVA or physical address; see [PA]. */
        uint64_t reserved_53_63        : 11;
#endif /* Word 0 - End */
    } cn9;
    struct bdk_oclax_stack_base_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t ptr                   : 42; /**< [ 48:  7](R/W) Memory address for base of overflow stack. This address must be on the local node in a
                                                                 CCPI system.
                                                                 This may be an IOVA or physical address; see [PA]. */
        uint64_t reserved_2_6          : 5;
        uint64_t sec                   : 1;  /**< [  1:  1](SR/W) If set, and physical addressing is used as described under [PA], the physical address
                                                                 is in the secure world. */
        uint64_t pa                    : 1;  /**< [  0:  0](R/W) When set, [PTR] and all DMA addresses are physical addresses and will not be translated by
                                                                 the SMMU.  When clear, is a virtual address which is subject to SMMU translation.

                                                                 Only used for OCLA(2) in the coprocessor-clock domain; for OCLA(0..1) in the core-clock
                                                                 domains this bit is ignored, addresses are always physical. */
#else /* Word 0 - Little Endian */
        uint64_t pa                    : 1;  /**< [  0:  0](R/W) When set, [PTR] and all DMA addresses are physical addresses and will not be translated by
                                                                 the SMMU.  When clear, is a virtual address which is subject to SMMU translation.

                                                                 Only used for OCLA(2) in the coprocessor-clock domain; for OCLA(0..1) in the core-clock
                                                                 domains this bit is ignored, addresses are always physical. */
        uint64_t sec                   : 1;  /**< [  1:  1](SR/W) If set, and physical addressing is used as described under [PA], the physical address
                                                                 is in the secure world. */
        uint64_t reserved_2_6          : 5;
        uint64_t ptr                   : 42; /**< [ 48:  7](R/W) Memory address for base of overflow stack. This address must be on the local node in a
                                                                 CCPI system.
                                                                 This may be an IOVA or physical address; see [PA]. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_oclax_stack_base_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t ptr                   : 42; /**< [ 48:  7](R/W) Memory address for base of overflow stack. This address must be on the local node in a
                                                                 CCPI system.
                                                                 This may be an IOVA or physical address; see [PA]. */
        uint64_t reserved_2_6          : 5;
        uint64_t sec                   : 1;  /**< [  1:  1](SR/W) If set, and physical addressing is used as described under [PA], the physical address
                                                                 is in the secure world. */
        uint64_t pa                    : 1;  /**< [  0:  0](R/W) When set, [PTR] and all DMA addresses are physical addresses and will not be translated by
                                                                 the SMMU.  When clear, is a virtual address which is subject to SMMU translation.

                                                                 Only used for OCLA(4) in the coprocessor-clock domain; for OCLA(0..3) in the core-clock
                                                                 domains this bit is ignored, addresses are always physical. */
#else /* Word 0 - Little Endian */
        uint64_t pa                    : 1;  /**< [  0:  0](R/W) When set, [PTR] and all DMA addresses are physical addresses and will not be translated by
                                                                 the SMMU.  When clear, is a virtual address which is subject to SMMU translation.

                                                                 Only used for OCLA(4) in the coprocessor-clock domain; for OCLA(0..3) in the core-clock
                                                                 domains this bit is ignored, addresses are always physical. */
        uint64_t sec                   : 1;  /**< [  1:  1](SR/W) If set, and physical addressing is used as described under [PA], the physical address
                                                                 is in the secure world. */
        uint64_t reserved_2_6          : 5;
        uint64_t ptr                   : 42; /**< [ 48:  7](R/W) Memory address for base of overflow stack. This address must be on the local node in a
                                                                 CCPI system.
                                                                 This may be an IOVA or physical address; see [PA]. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_oclax_stack_base_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t ptr                   : 42; /**< [ 48:  7](R/W) Memory address for base of overflow stack. This address must be on the local node in a
                                                                 CCPI system.
                                                                 This may be an IOVA or physical address; see [PA]. */
        uint64_t reserved_2_6          : 5;
        uint64_t sec                   : 1;  /**< [  1:  1](SR/W) If set, and physical addressing is used as described under [PA], the physical address
                                                                 is in the secure world. */
        uint64_t pa                    : 1;  /**< [  0:  0](R/W) When set, [PTR] and all DMA addresses are physical addresses and will not be translated by
                                                                 the SMMU.  When clear, is a virtual address which is subject to SMMU translation.

                                                                 Only used for the OCLA in the coprocessor-clock domain; for OCLAs in the core-clock
                                                                 domains this bit is ignored, addresses are always physical. */
#else /* Word 0 - Little Endian */
        uint64_t pa                    : 1;  /**< [  0:  0](R/W) When set, [PTR] and all DMA addresses are physical addresses and will not be translated by
                                                                 the SMMU.  When clear, is a virtual address which is subject to SMMU translation.

                                                                 Only used for the OCLA in the coprocessor-clock domain; for OCLAs in the core-clock
                                                                 domains this bit is ignored, addresses are always physical. */
        uint64_t sec                   : 1;  /**< [  1:  1](SR/W) If set, and physical addressing is used as described under [PA], the physical address
                                                                 is in the secure world. */
        uint64_t reserved_2_6          : 5;
        uint64_t ptr                   : 42; /**< [ 48:  7](R/W) Memory address for base of overflow stack. This address must be on the local node in a
                                                                 CCPI system.
                                                                 This may be an IOVA or physical address; see [PA]. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_oclax_stack_base bdk_oclax_stack_base_t;

static inline uint64_t BDK_OCLAX_STACK_BASE(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_STACK_BASE(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a8000400ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a8000400ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a8000400ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000400ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_STACK_BASE", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_STACK_BASE(a) bdk_oclax_stack_base_t
#define bustype_BDK_OCLAX_STACK_BASE(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_STACK_BASE(a) "OCLAX_STACK_BASE"
#define device_bar_BDK_OCLAX_STACK_BASE(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_STACK_BASE(a) (a)
#define arguments_BDK_OCLAX_STACK_BASE(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_stack_cur
 *
 * OCLA Stack Current Registers
 */
union bdk_oclax_stack_cur
{
    uint64_t u;
    struct bdk_oclax_stack_cur_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_53_63        : 11;
        uint64_t ptr                   : 46; /**< [ 52:  7](R/W/H) Next address to write for overflow stack. This address must be on the local node in a
                                                                 CCPI system. During initialization this must be between OCLA()_STACK_BASE and
                                                                 OCLA()_STACK_TOP.
                                                                 This may be an IOVA or physical address; see OCLA()_STACK_BASE[PA]. */
        uint64_t reserved_0_6          : 7;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_6          : 7;
        uint64_t ptr                   : 46; /**< [ 52:  7](R/W/H) Next address to write for overflow stack. This address must be on the local node in a
                                                                 CCPI system. During initialization this must be between OCLA()_STACK_BASE and
                                                                 OCLA()_STACK_TOP.
                                                                 This may be an IOVA or physical address; see OCLA()_STACK_BASE[PA]. */
        uint64_t reserved_53_63        : 11;
#endif /* Word 0 - End */
    } s;
    struct bdk_oclax_stack_cur_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t ptr                   : 42; /**< [ 48:  7](R/W/H) Next address to write for overflow stack. This address must be on the local node in a
                                                                 CCPI system. During initialization this must be between OCLA()_STACK_BASE and
                                                                 OCLA()_STACK_TOP.
                                                                 This may be an IOVA or physical address; see OCLA()_STACK_BASE[PA]. */
        uint64_t reserved_0_6          : 7;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_6          : 7;
        uint64_t ptr                   : 42; /**< [ 48:  7](R/W/H) Next address to write for overflow stack. This address must be on the local node in a
                                                                 CCPI system. During initialization this must be between OCLA()_STACK_BASE and
                                                                 OCLA()_STACK_TOP.
                                                                 This may be an IOVA or physical address; see OCLA()_STACK_BASE[PA]. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } cn8;
    /* struct bdk_oclax_stack_cur_s cn9; */
};
typedef union bdk_oclax_stack_cur bdk_oclax_stack_cur_t;

static inline uint64_t BDK_OCLAX_STACK_CUR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_STACK_CUR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a8000480ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a8000480ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a8000480ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000480ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_STACK_CUR", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_STACK_CUR(a) bdk_oclax_stack_cur_t
#define bustype_BDK_OCLAX_STACK_CUR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_STACK_CUR(a) "OCLAX_STACK_CUR"
#define device_bar_BDK_OCLAX_STACK_CUR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_STACK_CUR(a) (a)
#define arguments_BDK_OCLAX_STACK_CUR(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_stack_store_cnt
 *
 * OCLA Stack Stores Performance Counter Registers
 */
union bdk_oclax_stack_store_cnt
{
    uint64_t u;
    struct bdk_oclax_stack_store_cnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t stores                : 32; /**< [ 31:  0](R/W/H) Number of cache line stores sent to memory subsystem. Not cleared by hardware. */
#else /* Word 0 - Little Endian */
        uint64_t stores                : 32; /**< [ 31:  0](R/W/H) Number of cache line stores sent to memory subsystem. Not cleared by hardware. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_stack_store_cnt_s cn; */
};
typedef union bdk_oclax_stack_store_cnt bdk_oclax_stack_store_cnt_t;

static inline uint64_t BDK_OCLAX_STACK_STORE_CNT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_STACK_STORE_CNT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a8000460ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a8000460ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a8000460ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000460ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_STACK_STORE_CNT", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_STACK_STORE_CNT(a) bdk_oclax_stack_store_cnt_t
#define bustype_BDK_OCLAX_STACK_STORE_CNT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_STACK_STORE_CNT(a) "OCLAX_STACK_STORE_CNT"
#define device_bar_BDK_OCLAX_STACK_STORE_CNT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_STACK_STORE_CNT(a) (a)
#define arguments_BDK_OCLAX_STACK_STORE_CNT(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_stack_top
 *
 * OCLA Stack Top Address Registers
 */
union bdk_oclax_stack_top
{
    uint64_t u;
    struct bdk_oclax_stack_top_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_53_63        : 11;
        uint64_t ptr                   : 46; /**< [ 52:  7](R/W) Memory address for top of overflow stack plus one. This address must be on the local node
                                                                 in a CCPI system.
                                                                 This may be an IOVA or physical address; see OCLA()_STACK_BASE[PA]. */
        uint64_t reserved_0_6          : 7;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_6          : 7;
        uint64_t ptr                   : 46; /**< [ 52:  7](R/W) Memory address for top of overflow stack plus one. This address must be on the local node
                                                                 in a CCPI system.
                                                                 This may be an IOVA or physical address; see OCLA()_STACK_BASE[PA]. */
        uint64_t reserved_53_63        : 11;
#endif /* Word 0 - End */
    } s;
    struct bdk_oclax_stack_top_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_49_63        : 15;
        uint64_t ptr                   : 42; /**< [ 48:  7](R/W) Memory address for top of overflow stack plus one. This address must be on the local node
                                                                 in a CCPI system.
                                                                 This may be an IOVA or physical address; see OCLA()_STACK_BASE[PA]. */
        uint64_t reserved_0_6          : 7;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_6          : 7;
        uint64_t ptr                   : 42; /**< [ 48:  7](R/W) Memory address for top of overflow stack plus one. This address must be on the local node
                                                                 in a CCPI system.
                                                                 This may be an IOVA or physical address; see OCLA()_STACK_BASE[PA]. */
        uint64_t reserved_49_63        : 15;
#endif /* Word 0 - End */
    } cn8;
    /* struct bdk_oclax_stack_top_s cn9; */
};
typedef union bdk_oclax_stack_top bdk_oclax_stack_top_t;

static inline uint64_t BDK_OCLAX_STACK_TOP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_STACK_TOP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a8000420ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a8000420ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a8000420ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000420ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_STACK_TOP", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_STACK_TOP(a) bdk_oclax_stack_top_t
#define bustype_BDK_OCLAX_STACK_TOP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_STACK_TOP(a) "OCLAX_STACK_TOP"
#define device_bar_BDK_OCLAX_STACK_TOP(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_STACK_TOP(a) (a)
#define arguments_BDK_OCLAX_STACK_TOP(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_stack_wrap
 *
 * OCLA Stack Wrap Counter Registers
 */
union bdk_oclax_stack_wrap
{
    uint64_t u;
    struct bdk_oclax_stack_wrap_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t wraps                 : 32; /**< [ 31:  0](R/W/H) Number of times stack has been reset to OCLA()_STACK_BASE since trigger. Cleared when
                                                                 OCLA()_STATE_INT[TRIG] clear. */
#else /* Word 0 - Little Endian */
        uint64_t wraps                 : 32; /**< [ 31:  0](R/W/H) Number of times stack has been reset to OCLA()_STACK_BASE since trigger. Cleared when
                                                                 OCLA()_STATE_INT[TRIG] clear. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_stack_wrap_s cn; */
};
typedef union bdk_oclax_stack_wrap bdk_oclax_stack_wrap_t;

static inline uint64_t BDK_OCLAX_STACK_WRAP(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_STACK_WRAP(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a8000440ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a8000440ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a8000440ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000440ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_STACK_WRAP", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_STACK_WRAP(a) bdk_oclax_stack_wrap_t
#define bustype_BDK_OCLAX_STACK_WRAP(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_STACK_WRAP(a) "OCLAX_STACK_WRAP"
#define device_bar_BDK_OCLAX_STACK_WRAP(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_STACK_WRAP(a) (a)
#define arguments_BDK_OCLAX_STACK_WRAP(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_stage#
 *
 * OCLA Input Staging Registers
 */
union bdk_oclax_stagex
{
    uint64_t u;
    struct bdk_oclax_stagex_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t dly                   : 4;  /**< [  3:  0](R/W) Cycles of delay staging to apply to corresponding input bit. */
#else /* Word 0 - Little Endian */
        uint64_t dly                   : 4;  /**< [  3:  0](R/W) Cycles of delay staging to apply to corresponding input bit. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    struct bdk_oclax_stagex_cn8
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_3_63         : 61;
        uint64_t dly                   : 3;  /**< [  2:  0](R/W) Cycles of delay staging to apply to corresponding input bit. */
#else /* Word 0 - Little Endian */
        uint64_t dly                   : 3;  /**< [  2:  0](R/W) Cycles of delay staging to apply to corresponding input bit. */
        uint64_t reserved_3_63         : 61;
#endif /* Word 0 - End */
    } cn8;
    /* struct bdk_oclax_stagex_s cn9; */
};
typedef union bdk_oclax_stagex bdk_oclax_stagex_t;

static inline uint64_t BDK_OCLAX_STAGEX(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_STAGEX(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=1) && (b<=71)))
        return 0x87e0a8100000ll + 0x1000000ll * ((a) & 0x1) + 8ll * ((b) & 0x7f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=2) && (b<=71)))
        return 0x87e0a8100000ll + 0x1000000ll * ((a) & 0x3) + 8ll * ((b) & 0x7f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=4) && (b<=71)))
        return 0x87e0a8100000ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x7f);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && ((a<=4) && (b<=71)))
        return 0x87e0b0100000ll + 0x1000000ll * ((a) & 0x7) + 8ll * ((b) & 0x7f);
    __bdk_csr_fatal("OCLAX_STAGEX", 2, a, b, 0, 0);
}

#define typedef_BDK_OCLAX_STAGEX(a,b) bdk_oclax_stagex_t
#define bustype_BDK_OCLAX_STAGEX(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_STAGEX(a,b) "OCLAX_STAGEX"
#define device_bar_BDK_OCLAX_STAGEX(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_STAGEX(a,b) (a)
#define arguments_BDK_OCLAX_STAGEX(a,b) (a),(b),-1,-1

/**
 * Register (RSL) ocla#_state_ena_w1c
 *
 * OCLA State Interrupt Enable Clear Registers
 * This register clears interrupt enable bits.
 */
union bdk_oclax_state_ena_w1c
{
    uint64_t u;
    struct bdk_oclax_state_ena_w1c_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_19_63        : 45;
        uint64_t ddrfull               : 1;  /**< [ 18: 18](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[DDRFULL]. */
        uint64_t wmark                 : 1;  /**< [ 17: 17](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[WMARK]. */
        uint64_t overfull              : 1;  /**< [ 16: 16](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[OVERFULL]. */
        uint64_t trigfull              : 1;  /**< [ 15: 15](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[TRIGFULL]. */
        uint64_t captured              : 1;  /**< [ 14: 14](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[CAPTURED]. */
        uint64_t fsm1_int              : 1;  /**< [ 13: 13](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[FSM1_INT]. */
        uint64_t fsm0_int              : 1;  /**< [ 12: 12](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[FSM0_INT]. */
        uint64_t mcd                   : 3;  /**< [ 11:  9](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[MCD]. */
        uint64_t trig                  : 1;  /**< [  8:  8](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[TRIG]. */
        uint64_t reserved_4_7          : 4;
        uint64_t ovfl                  : 4;  /**< [  3:  0](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[OVFL]. */
#else /* Word 0 - Little Endian */
        uint64_t ovfl                  : 4;  /**< [  3:  0](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[OVFL]. */
        uint64_t reserved_4_7          : 4;
        uint64_t trig                  : 1;  /**< [  8:  8](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[TRIG]. */
        uint64_t mcd                   : 3;  /**< [ 11:  9](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[MCD]. */
        uint64_t fsm0_int              : 1;  /**< [ 12: 12](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[FSM0_INT]. */
        uint64_t fsm1_int              : 1;  /**< [ 13: 13](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[FSM1_INT]. */
        uint64_t captured              : 1;  /**< [ 14: 14](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[CAPTURED]. */
        uint64_t trigfull              : 1;  /**< [ 15: 15](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[TRIGFULL]. */
        uint64_t overfull              : 1;  /**< [ 16: 16](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[OVERFULL]. */
        uint64_t wmark                 : 1;  /**< [ 17: 17](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[WMARK]. */
        uint64_t ddrfull               : 1;  /**< [ 18: 18](R/W1C/H) Reads or clears OCLA()_STATE_ENA_W1S[DDRFULL]. */
        uint64_t reserved_19_63        : 45;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_state_ena_w1c_s cn; */
};
typedef union bdk_oclax_state_ena_w1c bdk_oclax_state_ena_w1c_t;

static inline uint64_t BDK_OCLAX_STATE_ENA_W1C(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_STATE_ENA_W1C(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a80000b8ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a80000b8ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a80000b8ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b00000b8ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_STATE_ENA_W1C", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_STATE_ENA_W1C(a) bdk_oclax_state_ena_w1c_t
#define bustype_BDK_OCLAX_STATE_ENA_W1C(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_STATE_ENA_W1C(a) "OCLAX_STATE_ENA_W1C"
#define device_bar_BDK_OCLAX_STATE_ENA_W1C(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_STATE_ENA_W1C(a) (a)
#define arguments_BDK_OCLAX_STATE_ENA_W1C(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_state_ena_w1s
 *
 * OCLA State Interrupt Enable Set Registers
 * This register sets interrupt enable bits.
 */
union bdk_oclax_state_ena_w1s
{
    uint64_t u;
    struct bdk_oclax_state_ena_w1s_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_19_63        : 45;
        uint64_t ddrfull               : 1;  /**< [ 18: 18](R/W1S/H) Enables reporting of OCLA()_STATE_INT[DDRFULL]. */
        uint64_t wmark                 : 1;  /**< [ 17: 17](R/W1S/H) Enables reporting of OCLA()_STATE_INT[WMARK]. */
        uint64_t overfull              : 1;  /**< [ 16: 16](R/W1S/H) Enables reporting of OCLA()_STATE_INT[OVERFULL]. */
        uint64_t trigfull              : 1;  /**< [ 15: 15](R/W1S/H) Enables reporting of OCLA()_STATE_INT[TRIGFULL]. */
        uint64_t captured              : 1;  /**< [ 14: 14](R/W1S/H) Enables reporting of OCLA()_STATE_INT[CAPTURED]. */
        uint64_t fsm1_int              : 1;  /**< [ 13: 13](R/W1S/H) Enables reporting of OCLA()_STATE_INT[FSM1_INT]. */
        uint64_t fsm0_int              : 1;  /**< [ 12: 12](R/W1S/H) Enables reporting of OCLA()_STATE_INT[FSM0_INT]. */
        uint64_t mcd                   : 3;  /**< [ 11:  9](R/W1S/H) Enables reporting of OCLA()_STATE_INT[MCD]. */
        uint64_t trig                  : 1;  /**< [  8:  8](R/W1S/H) Enables reporting of OCLA()_STATE_INT[TRIG]. */
        uint64_t reserved_4_7          : 4;
        uint64_t ovfl                  : 4;  /**< [  3:  0](R/W1S/H) Enables reporting of OCLA()_STATE_INT[OVFL]. */
#else /* Word 0 - Little Endian */
        uint64_t ovfl                  : 4;  /**< [  3:  0](R/W1S/H) Enables reporting of OCLA()_STATE_INT[OVFL]. */
        uint64_t reserved_4_7          : 4;
        uint64_t trig                  : 1;  /**< [  8:  8](R/W1S/H) Enables reporting of OCLA()_STATE_INT[TRIG]. */
        uint64_t mcd                   : 3;  /**< [ 11:  9](R/W1S/H) Enables reporting of OCLA()_STATE_INT[MCD]. */
        uint64_t fsm0_int              : 1;  /**< [ 12: 12](R/W1S/H) Enables reporting of OCLA()_STATE_INT[FSM0_INT]. */
        uint64_t fsm1_int              : 1;  /**< [ 13: 13](R/W1S/H) Enables reporting of OCLA()_STATE_INT[FSM1_INT]. */
        uint64_t captured              : 1;  /**< [ 14: 14](R/W1S/H) Enables reporting of OCLA()_STATE_INT[CAPTURED]. */
        uint64_t trigfull              : 1;  /**< [ 15: 15](R/W1S/H) Enables reporting of OCLA()_STATE_INT[TRIGFULL]. */
        uint64_t overfull              : 1;  /**< [ 16: 16](R/W1S/H) Enables reporting of OCLA()_STATE_INT[OVERFULL]. */
        uint64_t wmark                 : 1;  /**< [ 17: 17](R/W1S/H) Enables reporting of OCLA()_STATE_INT[WMARK]. */
        uint64_t ddrfull               : 1;  /**< [ 18: 18](R/W1S/H) Enables reporting of OCLA()_STATE_INT[DDRFULL]. */
        uint64_t reserved_19_63        : 45;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_state_ena_w1s_s cn; */
};
typedef union bdk_oclax_state_ena_w1s bdk_oclax_state_ena_w1s_t;

static inline uint64_t BDK_OCLAX_STATE_ENA_W1S(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_STATE_ENA_W1S(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a80000b0ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a80000b0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a80000b0ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b00000b0ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_STATE_ENA_W1S", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_STATE_ENA_W1S(a) bdk_oclax_state_ena_w1s_t
#define bustype_BDK_OCLAX_STATE_ENA_W1S(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_STATE_ENA_W1S(a) "OCLAX_STATE_ENA_W1S"
#define device_bar_BDK_OCLAX_STATE_ENA_W1S(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_STATE_ENA_W1S(a) (a)
#define arguments_BDK_OCLAX_STATE_ENA_W1S(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_state_int
 *
 * OCLA State and Interrupt Registers
 */
union bdk_oclax_state_int
{
    uint64_t u;
    struct bdk_oclax_state_int_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t fsm1_state            : 4;  /**< [ 63: 60](RO/H) FSM1 current state. */
        uint64_t fsm0_state            : 4;  /**< [ 59: 56](RO/H) FSM0 current state. */
        uint64_t reserved_36_55        : 20;
        uint64_t fsm1_rst              : 1;  /**< [ 35: 35](R/W1C) FSM1 hold in state zero. Writing one to OCLA()_STATE_SET[FSM1_RST] sets this bit and
                                                                 holds FSM1 in state zero, writing one to OCLA()_STATE_INT[FSM1_RST] removes the hold. */
        uint64_t fsm0_rst              : 1;  /**< [ 34: 34](R/W1C) FSM0 hold in state zero. Writing one to OCLA()_STATE_SET[FSM0_RST] sets this bit and
                                                                 holds FSM0 in state zero, writing one to OCLA()_STATE_INT[FSM0_RST] removes the hold. */
        uint64_t fsm1_ena              : 1;  /**< [ 33: 33](R/W1C/H) FSM1 sequencing enabled. */
        uint64_t fsm0_ena              : 1;  /**< [ 32: 32](R/W1C/H) FSM0 sequencing enabled. */
        uint64_t reserved_19_31        : 13;
        uint64_t ddrfull               : 1;  /**< [ 18: 18](R/W1C/H) DDR buffer wrapped. Asserted when OCLA()_STACK_CUR has wrapped and been re-initialized
                                                                 to OCLA()_STACK_BASE. */
        uint64_t wmark                 : 1;  /**< [ 17: 17](R/W1C/H) Internal buffer watermark reached. Asserted when OCLA()_FIFO_DEPTH \>
                                                                 OCLA()_FIFO_LIMIT[WMARK]. */
        uint64_t overfull              : 1;  /**< [ 16: 16](R/W1C/H) Capture ended due to FIFO overflow. Asserted when OCLA()_FIFO_DEPTH \>
                                                                 OCLA()_FIFO_LIMIT[OVERFULL]. */
        uint64_t trigfull              : 1;  /**< [ 15: 15](R/W1C/H) Capture ended due to buffer full. Asserted when OCLA()_FIFO_TRIG[LIMIT] \>=
                                                                 OCLA()_FIFO_TRIG[CNT]. */
        uint64_t captured              : 1;  /**< [ 14: 14](R/W1C/H) Capture started. Asserted when the first capture is made. Informational only; often masked. */
        uint64_t fsm1_int              : 1;  /**< [ 13: 13](R/W1C/H) FSM1 interrupt requested. */
        uint64_t fsm0_int              : 1;  /**< [ 12: 12](R/W1C/H) FSM0 interrupt requested. */
        uint64_t mcd                   : 3;  /**< [ 11:  9](R/W1C/H) Multichip debug (MCD0..2) set. Asserted on MCD received from another coprocessor or code,
                                                                 or FSM MCD request or W1S to OCLA()_STATE_SET[MCD]. */
        uint64_t trig                  : 1;  /**< [  8:  8](R/W1C/H) Internal trigger set. Asserted on FSM internal trigger request or W1S to OCLA()_STATE_SET[TRIG]. */
        uint64_t reserved_4_7          : 4;
        uint64_t ovfl                  : 4;  /**< [  3:  0](R/W1C/H) Match counter has overflowed. Asserted when OCLA()_MAT()_COUNT \>=
                                                                 OCLA()_MAT()_THRESH. Informational only; often masked. Writing 1 clears the
                                                                 counter, not just the interrupt. */
#else /* Word 0 - Little Endian */
        uint64_t ovfl                  : 4;  /**< [  3:  0](R/W1C/H) Match counter has overflowed. Asserted when OCLA()_MAT()_COUNT \>=
                                                                 OCLA()_MAT()_THRESH. Informational only; often masked. Writing 1 clears the
                                                                 counter, not just the interrupt. */
        uint64_t reserved_4_7          : 4;
        uint64_t trig                  : 1;  /**< [  8:  8](R/W1C/H) Internal trigger set. Asserted on FSM internal trigger request or W1S to OCLA()_STATE_SET[TRIG]. */
        uint64_t mcd                   : 3;  /**< [ 11:  9](R/W1C/H) Multichip debug (MCD0..2) set. Asserted on MCD received from another coprocessor or code,
                                                                 or FSM MCD request or W1S to OCLA()_STATE_SET[MCD]. */
        uint64_t fsm0_int              : 1;  /**< [ 12: 12](R/W1C/H) FSM0 interrupt requested. */
        uint64_t fsm1_int              : 1;  /**< [ 13: 13](R/W1C/H) FSM1 interrupt requested. */
        uint64_t captured              : 1;  /**< [ 14: 14](R/W1C/H) Capture started. Asserted when the first capture is made. Informational only; often masked. */
        uint64_t trigfull              : 1;  /**< [ 15: 15](R/W1C/H) Capture ended due to buffer full. Asserted when OCLA()_FIFO_TRIG[LIMIT] \>=
                                                                 OCLA()_FIFO_TRIG[CNT]. */
        uint64_t overfull              : 1;  /**< [ 16: 16](R/W1C/H) Capture ended due to FIFO overflow. Asserted when OCLA()_FIFO_DEPTH \>
                                                                 OCLA()_FIFO_LIMIT[OVERFULL]. */
        uint64_t wmark                 : 1;  /**< [ 17: 17](R/W1C/H) Internal buffer watermark reached. Asserted when OCLA()_FIFO_DEPTH \>
                                                                 OCLA()_FIFO_LIMIT[WMARK]. */
        uint64_t ddrfull               : 1;  /**< [ 18: 18](R/W1C/H) DDR buffer wrapped. Asserted when OCLA()_STACK_CUR has wrapped and been re-initialized
                                                                 to OCLA()_STACK_BASE. */
        uint64_t reserved_19_31        : 13;
        uint64_t fsm0_ena              : 1;  /**< [ 32: 32](R/W1C/H) FSM0 sequencing enabled. */
        uint64_t fsm1_ena              : 1;  /**< [ 33: 33](R/W1C/H) FSM1 sequencing enabled. */
        uint64_t fsm0_rst              : 1;  /**< [ 34: 34](R/W1C) FSM0 hold in state zero. Writing one to OCLA()_STATE_SET[FSM0_RST] sets this bit and
                                                                 holds FSM0 in state zero, writing one to OCLA()_STATE_INT[FSM0_RST] removes the hold. */
        uint64_t fsm1_rst              : 1;  /**< [ 35: 35](R/W1C) FSM1 hold in state zero. Writing one to OCLA()_STATE_SET[FSM1_RST] sets this bit and
                                                                 holds FSM1 in state zero, writing one to OCLA()_STATE_INT[FSM1_RST] removes the hold. */
        uint64_t reserved_36_55        : 20;
        uint64_t fsm0_state            : 4;  /**< [ 59: 56](RO/H) FSM0 current state. */
        uint64_t fsm1_state            : 4;  /**< [ 63: 60](RO/H) FSM1 current state. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_state_int_s cn; */
};
typedef union bdk_oclax_state_int bdk_oclax_state_int_t;

static inline uint64_t BDK_OCLAX_STATE_INT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_STATE_INT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a8000080ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a8000080ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a8000080ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b0000080ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_STATE_INT", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_STATE_INT(a) bdk_oclax_state_int_t
#define bustype_BDK_OCLAX_STATE_INT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_STATE_INT(a) "OCLAX_STATE_INT"
#define device_bar_BDK_OCLAX_STATE_INT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_STATE_INT(a) (a)
#define arguments_BDK_OCLAX_STATE_INT(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_state_set
 *
 * OCLA State Set Registers
 * This register reads identically to OCLA()_STATE_INT, but allows R/W1S instead of R/W1C access.
 */
union bdk_oclax_state_set
{
    uint64_t u;
    struct bdk_oclax_state_set_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t fsm1_state            : 4;  /**< [ 63: 60](RO/H) See OCLA()_STATE_INT[FSM1_STATE]. */
        uint64_t fsm0_state            : 4;  /**< [ 59: 56](RO/H) See OCLA()_STATE_INT[FSM0_STATE]. */
        uint64_t reserved_36_55        : 20;
        uint64_t fsm1_rst              : 1;  /**< [ 35: 35](R/W1S) See OCLA()_STATE_INT[FSM1_RST]. */
        uint64_t fsm0_rst              : 1;  /**< [ 34: 34](R/W1S) See OCLA()_STATE_INT[FSM0_RST]. */
        uint64_t fsm1_ena              : 1;  /**< [ 33: 33](R/W1S/H) See OCLA()_STATE_INT[FSM1_ENA]. */
        uint64_t fsm0_ena              : 1;  /**< [ 32: 32](R/W1S/H) See OCLA()_STATE_INT[FSM0_ENA]. */
        uint64_t reserved_19_31        : 13;
        uint64_t ddrfull               : 1;  /**< [ 18: 18](R/W1S/H) See OCLA()_STATE_INT[DDRFULL]. */
        uint64_t wmark                 : 1;  /**< [ 17: 17](R/W1S/H) See OCLA()_STATE_INT[WMARK]. */
        uint64_t overfull              : 1;  /**< [ 16: 16](R/W1S/H) See OCLA()_STATE_INT[OVERFULL]. */
        uint64_t trigfull              : 1;  /**< [ 15: 15](R/W1S/H) See OCLA()_STATE_INT[TRIGFULL]. */
        uint64_t captured              : 1;  /**< [ 14: 14](R/W1S/H) See OCLA()_STATE_INT[CAPTURED]. */
        uint64_t fsm1_int              : 1;  /**< [ 13: 13](R/W1S/H) See OCLA()_STATE_INT[FSM1_INT]. */
        uint64_t fsm0_int              : 1;  /**< [ 12: 12](R/W1S/H) See OCLA()_STATE_INT[FSM0_INT]. */
        uint64_t mcd                   : 3;  /**< [ 11:  9](R/W1S/H) See OCLA()_STATE_INT[MCD]. */
        uint64_t trig                  : 1;  /**< [  8:  8](R/W1S/H) See OCLA()_STATE_INT[TRIG]. */
        uint64_t reserved_4_7          : 4;
        uint64_t ovfl                  : 4;  /**< [  3:  0](R/W1S/H) See OCLA()_STATE_INT[OVFL]. */
#else /* Word 0 - Little Endian */
        uint64_t ovfl                  : 4;  /**< [  3:  0](R/W1S/H) See OCLA()_STATE_INT[OVFL]. */
        uint64_t reserved_4_7          : 4;
        uint64_t trig                  : 1;  /**< [  8:  8](R/W1S/H) See OCLA()_STATE_INT[TRIG]. */
        uint64_t mcd                   : 3;  /**< [ 11:  9](R/W1S/H) See OCLA()_STATE_INT[MCD]. */
        uint64_t fsm0_int              : 1;  /**< [ 12: 12](R/W1S/H) See OCLA()_STATE_INT[FSM0_INT]. */
        uint64_t fsm1_int              : 1;  /**< [ 13: 13](R/W1S/H) See OCLA()_STATE_INT[FSM1_INT]. */
        uint64_t captured              : 1;  /**< [ 14: 14](R/W1S/H) See OCLA()_STATE_INT[CAPTURED]. */
        uint64_t trigfull              : 1;  /**< [ 15: 15](R/W1S/H) See OCLA()_STATE_INT[TRIGFULL]. */
        uint64_t overfull              : 1;  /**< [ 16: 16](R/W1S/H) See OCLA()_STATE_INT[OVERFULL]. */
        uint64_t wmark                 : 1;  /**< [ 17: 17](R/W1S/H) See OCLA()_STATE_INT[WMARK]. */
        uint64_t ddrfull               : 1;  /**< [ 18: 18](R/W1S/H) See OCLA()_STATE_INT[DDRFULL]. */
        uint64_t reserved_19_31        : 13;
        uint64_t fsm0_ena              : 1;  /**< [ 32: 32](R/W1S/H) See OCLA()_STATE_INT[FSM0_ENA]. */
        uint64_t fsm1_ena              : 1;  /**< [ 33: 33](R/W1S/H) See OCLA()_STATE_INT[FSM1_ENA]. */
        uint64_t fsm0_rst              : 1;  /**< [ 34: 34](R/W1S) See OCLA()_STATE_INT[FSM0_RST]. */
        uint64_t fsm1_rst              : 1;  /**< [ 35: 35](R/W1S) See OCLA()_STATE_INT[FSM1_RST]. */
        uint64_t reserved_36_55        : 20;
        uint64_t fsm0_state            : 4;  /**< [ 59: 56](RO/H) See OCLA()_STATE_INT[FSM0_STATE]. */
        uint64_t fsm1_state            : 4;  /**< [ 63: 60](RO/H) See OCLA()_STATE_INT[FSM1_STATE]. */
#endif /* Word 0 - End */
    } s;
    /* struct bdk_oclax_state_set_s cn; */
};
typedef union bdk_oclax_state_set bdk_oclax_state_set_t;

static inline uint64_t BDK_OCLAX_STATE_SET(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_STATE_SET(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a80000a0ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a80000a0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a80000a0ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b00000a0ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_STATE_SET", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_STATE_SET(a) bdk_oclax_state_set_t
#define bustype_BDK_OCLAX_STATE_SET(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_STATE_SET(a) "OCLAX_STATE_SET"
#define device_bar_BDK_OCLAX_STATE_SET(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_STATE_SET(a) (a)
#define arguments_BDK_OCLAX_STATE_SET(a) (a),-1,-1,-1

/**
 * Register (RSL) ocla#_time
 *
 * OCLA Current Time Registers
 */
union bdk_oclax_time
{
    uint64_t u;
    struct bdk_oclax_time_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t cycle                 : 64; /**< [ 63:  0](R/W/H) Current time as free running counter. Loaded into captured control packets.
                                                                 Unconditionally clocked, independent of OCLA()_SFT_RST. */
#else /* Word 0 - Little Endian */
        uint64_t cycle                 : 64; /**< [ 63:  0](R/W/H) Current time as free running counter. Loaded into captured control packets.
                                                                 Unconditionally clocked, independent of OCLA()_SFT_RST. */
#endif /* Word 0 - End */
    } s;
    struct bdk_oclax_time_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t cycle                 : 32; /**< [ 31:  0](R/W/H) Current time as free running counter. Loaded into captured control packets.
                                                                 Unconditionally clocked, independent of OCLA()_SFT_RST. */
#else /* Word 0 - Little Endian */
        uint64_t cycle                 : 32; /**< [ 31:  0](R/W/H) Current time as free running counter. Loaded into captured control packets.
                                                                 Unconditionally clocked, independent of OCLA()_SFT_RST. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } cn88xxp1;
    /* struct bdk_oclax_time_s cn9; */
    /* struct bdk_oclax_time_s cn81xx; */
    /* struct bdk_oclax_time_s cn83xx; */
    /* struct bdk_oclax_time_s cn88xxp2; */
};
typedef union bdk_oclax_time bdk_oclax_time_t;

static inline uint64_t BDK_OCLAX_TIME(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_OCLAX_TIME(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=1))
        return 0x87e0a80000c0ll + 0x1000000ll * ((a) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=2))
        return 0x87e0a80000c0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=4))
        return 0x87e0a80000c0ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN9XXX) && (a<=4))
        return 0x87e0b00000c0ll + 0x1000000ll * ((a) & 0x7);
    __bdk_csr_fatal("OCLAX_TIME", 1, a, 0, 0, 0);
}

#define typedef_BDK_OCLAX_TIME(a) bdk_oclax_time_t
#define bustype_BDK_OCLAX_TIME(a) BDK_CSR_TYPE_RSL
#define basename_BDK_OCLAX_TIME(a) "OCLAX_TIME"
#define device_bar_BDK_OCLAX_TIME(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_OCLAX_TIME(a) (a)
#define arguments_BDK_OCLAX_TIME(a) (a),-1,-1,-1

#endif /* __BDK_CSRS_OCLA_H__ */
