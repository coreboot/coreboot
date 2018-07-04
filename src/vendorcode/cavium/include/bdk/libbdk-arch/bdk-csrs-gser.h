#ifndef __BDK_CSRS_GSER_H__
#define __BDK_CSRS_GSER_H__
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
 * Cavium GSER.
 *
 * This file is auto generated. Do not edit.
 *
 */

/**
 * Enumeration gser_bar_e
 *
 * GSER Base Address Register Enumeration
 * Enumerates the base address registers.
 */
#define BDK_GSER_BAR_E_GSERX_PF_BAR0(a) (0x87e090000000ll + 0x1000000ll * (a))
#define BDK_GSER_BAR_E_GSERX_PF_BAR0_SIZE 0x800000ull

/**
 * Enumeration gser_lmode_e
 *
 * GSER Lane Mode Enumeration
 * Enumerates the SerDes lane modes. See GSER()_LANE_MODE[LMODE].
 */
#define BDK_GSER_LMODE_E_R_103125G_REFCLK15625_KR (5)
#define BDK_GSER_LMODE_E_R_125G_REFCLK15625_KX (3)
#define BDK_GSER_LMODE_E_R_125G_REFCLK15625_SGMII (6)
#define BDK_GSER_LMODE_E_R_25G_REFCLK100 (0)
#define BDK_GSER_LMODE_E_R_25G_REFCLK125 (9)
#define BDK_GSER_LMODE_E_R_3125G_REFCLK15625_XAUI (4)
#define BDK_GSER_LMODE_E_R_5G_REFCLK100 (1)
#define BDK_GSER_LMODE_E_R_5G_REFCLK125 (0xa)
#define BDK_GSER_LMODE_E_R_5G_REFCLK15625_QSGMII (7)
#define BDK_GSER_LMODE_E_R_625G_REFCLK15625_RXAUI (8)
#define BDK_GSER_LMODE_E_R_8G_REFCLK100 (2)
#define BDK_GSER_LMODE_E_R_8G_REFCLK125 (0xb)

/**
 * Enumeration gser_qlm_e
 *
 * GSER QLM/CCPI Enumeration
 * Enumerates the GSER to QLM.
 */
#define BDK_GSER_QLM_E_GSER0 (0)
#define BDK_GSER_QLM_E_GSER1 (1)
#define BDK_GSER_QLM_E_GSER10 (0xa)
#define BDK_GSER_QLM_E_GSER11 (0xb)
#define BDK_GSER_QLM_E_GSER12 (0xc)
#define BDK_GSER_QLM_E_GSER13 (0xd)
#define BDK_GSER_QLM_E_GSER2 (2)
#define BDK_GSER_QLM_E_GSER3 (3)
#define BDK_GSER_QLM_E_GSER4 (4)
#define BDK_GSER_QLM_E_GSER5 (5)
#define BDK_GSER_QLM_E_GSER6 (6)
#define BDK_GSER_QLM_E_GSER7 (7)
#define BDK_GSER_QLM_E_GSER8 (8)
#define BDK_GSER_QLM_E_GSER9 (9)

/**
 * Register (RSL) gser#_ana_atest
 *
 * GSER Analog Test Register
 */
union bdk_gserx_ana_atest
{
    uint64_t u;
    struct bdk_gserx_ana_atest_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t ana_dac_b             : 7;  /**< [ 11:  5](R/W) Controls the B-side DAC input to the analog test block. Note that the GSER2
                                                                 register
                                                                 is tied to the analog test block.  The other GSER()_ANA_ATEST registers are
                                                                 unused. For diagnostic use only. */
        uint64_t ana_dac_a             : 5;  /**< [  4:  0](R/W) Controls the A-side DAC input to the analog test block. Note that the GSER2 register is
                                                                 tied to the analog test block.  The other GSER()_ANA_ATEST registers are unused.
                                                                 For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t ana_dac_a             : 5;  /**< [  4:  0](R/W) Controls the A-side DAC input to the analog test block. Note that the GSER2 register is
                                                                 tied to the analog test block.  The other GSER()_ANA_ATEST registers are unused.
                                                                 For diagnostic use only. */
        uint64_t ana_dac_b             : 7;  /**< [ 11:  5](R/W) Controls the B-side DAC input to the analog test block. Note that the GSER2
                                                                 register
                                                                 is tied to the analog test block.  The other GSER()_ANA_ATEST registers are
                                                                 unused. For diagnostic use only. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_ana_atest_s cn81xx; */
    struct bdk_gserx_ana_atest_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t ana_dac_b             : 7;  /**< [ 11:  5](R/W) Controls the B-side DAC input to the analog test block. Note that the QLM4 register
                                                                 is tied to the analog test block, for non-CCPI links. Note that the CCPI4 register is tied
                                                                 to the analog test block, for CCPI links. The other GSER()_ANA_ATEST registers are
                                                                 unused. For diagnostic use only. */
        uint64_t ana_dac_a             : 5;  /**< [  4:  0](R/W) Controls the A-side DAC input to the analog test block. Note that the QLM4 register is
                                                                 tied to the analog test block, for non-CCPI links. Note that the CCPI4 register is tied to
                                                                 the analog test block, for CCPI links. The other GSER()_ANA_ATEST registers are unused.
                                                                 For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t ana_dac_a             : 5;  /**< [  4:  0](R/W) Controls the A-side DAC input to the analog test block. Note that the QLM4 register is
                                                                 tied to the analog test block, for non-CCPI links. Note that the CCPI4 register is tied to
                                                                 the analog test block, for CCPI links. The other GSER()_ANA_ATEST registers are unused.
                                                                 For diagnostic use only. */
        uint64_t ana_dac_b             : 7;  /**< [ 11:  5](R/W) Controls the B-side DAC input to the analog test block. Note that the QLM4 register
                                                                 is tied to the analog test block, for non-CCPI links. Note that the CCPI4 register is tied
                                                                 to the analog test block, for CCPI links. The other GSER()_ANA_ATEST registers are
                                                                 unused. For diagnostic use only. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_gserx_ana_atest_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t ana_dac_b             : 7;  /**< [ 11:  5](R/W) Controls the B-side DAC input to the analog test block. Note that only
                                                                 the GSER(4)_ANA_TEST[ANA_DAC_B] is tied to the analog test block.
                                                                 The other GSER(0..3,5..6)_ANA_ATEST[ANA_DAC_B] are unused.
                                                                 For diagnostic use only. */
        uint64_t ana_dac_a             : 5;  /**< [  4:  0](R/W) Controls A-side DAC input to the analog test block. Note that only
                                                                 the GSER(4)_ANA_TEST[ANA_DAC_A] is tied to the analog test bloc.
                                                                 The other GSER(0..3,5..6)_ANA_ATEST[ANA_DAC_A] are unused.
                                                                 For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t ana_dac_a             : 5;  /**< [  4:  0](R/W) Controls A-side DAC input to the analog test block. Note that only
                                                                 the GSER(4)_ANA_TEST[ANA_DAC_A] is tied to the analog test bloc.
                                                                 The other GSER(0..3,5..6)_ANA_ATEST[ANA_DAC_A] are unused.
                                                                 For diagnostic use only. */
        uint64_t ana_dac_b             : 7;  /**< [ 11:  5](R/W) Controls the B-side DAC input to the analog test block. Note that only
                                                                 the GSER(4)_ANA_TEST[ANA_DAC_B] is tied to the analog test block.
                                                                 The other GSER(0..3,5..6)_ANA_ATEST[ANA_DAC_B] are unused.
                                                                 For diagnostic use only. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_ana_atest bdk_gserx_ana_atest_t;

static inline uint64_t BDK_GSERX_ANA_ATEST(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_ANA_ATEST(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000800ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000800ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000800ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_ANA_ATEST", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_ANA_ATEST(a) bdk_gserx_ana_atest_t
#define bustype_BDK_GSERX_ANA_ATEST(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_ANA_ATEST(a) "GSERX_ANA_ATEST"
#define device_bar_BDK_GSERX_ANA_ATEST(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_ANA_ATEST(a) (a)
#define arguments_BDK_GSERX_ANA_ATEST(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_ana_sel
 *
 * GSER Analog Select Register
 */
union bdk_gserx_ana_sel
{
    uint64_t u;
    struct bdk_gserx_ana_sel_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t ana_sel               : 9;  /**< [  8:  0](R/W) Controls the adr_global input to the analog test block. Note that the
                                                                 GSER(2)_ANA_SEL.ANA_SEL register is tied to the analog test block.
                                                                 The other GSER()_ANA_SEL registers are unused.
                                                                 For diagnostic use only.

                                                                 Used to power down the common clock input receiver to reduce power
                                                                 consumption if the common clock input is not used.
                                                                 If the common clock DLMC_REFCLK1_P/N input is unused program the GSER(2)_ANA_SEL.ANA_SEL
                                                                 field to 0x1fd.
                                                                 If the common clock DLMC_REFCLK0_P/N input is unused program the GSER(2)_ANA_SEL.ANA_SEL
                                                                 field to 0x1fe.
                                                                 If both common clock DLMC_REFCLK0_P/N and DLMC_REFCLK1_P/N inputs are unused program the
                                                                 GSER(2)_ANA_SEL.ANA_SEL field to 0x1fc. */
#else /* Word 0 - Little Endian */
        uint64_t ana_sel               : 9;  /**< [  8:  0](R/W) Controls the adr_global input to the analog test block. Note that the
                                                                 GSER(2)_ANA_SEL.ANA_SEL register is tied to the analog test block.
                                                                 The other GSER()_ANA_SEL registers are unused.
                                                                 For diagnostic use only.

                                                                 Used to power down the common clock input receiver to reduce power
                                                                 consumption if the common clock input is not used.
                                                                 If the common clock DLMC_REFCLK1_P/N input is unused program the GSER(2)_ANA_SEL.ANA_SEL
                                                                 field to 0x1fd.
                                                                 If the common clock DLMC_REFCLK0_P/N input is unused program the GSER(2)_ANA_SEL.ANA_SEL
                                                                 field to 0x1fe.
                                                                 If both common clock DLMC_REFCLK0_P/N and DLMC_REFCLK1_P/N inputs are unused program the
                                                                 GSER(2)_ANA_SEL.ANA_SEL field to 0x1fc. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    struct bdk_gserx_ana_sel_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t ana_sel               : 9;  /**< [  8:  0](R/W) Controls the adr_global input to the analog test block. Note that the QLM0 register
                                                                 is tied to the analog test block, for non-CCPI links. Note that the QLM8 register is tied
                                                                 to the analog test block, for CCPI links. The other GSER()_ANA_SEL registers are unused.
                                                                 For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t ana_sel               : 9;  /**< [  8:  0](R/W) Controls the adr_global input to the analog test block. Note that the QLM0 register
                                                                 is tied to the analog test block, for non-CCPI links. Note that the QLM8 register is tied
                                                                 to the analog test block, for CCPI links. The other GSER()_ANA_SEL registers are unused.
                                                                 For diagnostic use only. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } cn88xxp1;
    /* struct bdk_gserx_ana_sel_s cn81xx; */
    struct bdk_gserx_ana_sel_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t ana_sel               : 9;  /**< [  8:  0](R/W) Controls the adr_global input to the analog test block. Note that only
                                                                 the GSER(4)_ANA_SEL.ANA_SEL register is tied to the analog test block.
                                                                 The GSER(0..3,5..6)_ANA_SEL.ANA_SEL registers are unused.

                                                                 Used to power down the common clock input receiver to reduce power consumption
                                                                 if the common clock input is not used.
                                                                 If the common clock QLMC_REFCLK1_P/N input is unused program the
                                                                 GSER(4)_ANA_SEL.ANA_SEL field to 0x1FD.
                                                                 If the common clock QLMC_REFCLK0_P/N input is unused program the
                                                                 GSER(4)_ANA_SEL.ANA_SEL field to 0x1FE.
                                                                 If both common clock QLMC_REFCLK0_P/N and QLMC_REFCLK1_P/N inputs are unused program the
                                                                 GSER(4)_ANA_SEL[ANA_SEL] field to 0x1FC.
                                                                 For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t ana_sel               : 9;  /**< [  8:  0](R/W) Controls the adr_global input to the analog test block. Note that only
                                                                 the GSER(4)_ANA_SEL.ANA_SEL register is tied to the analog test block.
                                                                 The GSER(0..3,5..6)_ANA_SEL.ANA_SEL registers are unused.

                                                                 Used to power down the common clock input receiver to reduce power consumption
                                                                 if the common clock input is not used.
                                                                 If the common clock QLMC_REFCLK1_P/N input is unused program the
                                                                 GSER(4)_ANA_SEL.ANA_SEL field to 0x1FD.
                                                                 If the common clock QLMC_REFCLK0_P/N input is unused program the
                                                                 GSER(4)_ANA_SEL.ANA_SEL field to 0x1FE.
                                                                 If both common clock QLMC_REFCLK0_P/N and QLMC_REFCLK1_P/N inputs are unused program the
                                                                 GSER(4)_ANA_SEL[ANA_SEL] field to 0x1FC.
                                                                 For diagnostic use only. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } cn83xx;
    struct bdk_gserx_ana_sel_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t ana_sel               : 9;  /**< [  8:  0](R/W) Controls the adr_global input to the analog test block. Note that the
                                                                 GSER(0)_ANA_SEL.ANA_SEL register is tied to the analog test block, for non-CCPI links.
                                                                 Note that the GSER(8)_ANA_SEL.ANA_SEL register is tied to the analog test block, for
                                                                 CCPI links. The other GSER()_ANA_SEL registers are unused.
                                                                 For diagnostic use only.

                                                                 For non-CCPI links used to power down the common clock input receiver to reduce power
                                                                 consumption if the common clock input is not used.
                                                                 If the common clock QLMC_REFCLK1_P/N input is unused, program GSER(0)_ANA_SEL[ANA_SEL]
                                                                 to 0x1FD.
                                                                 If the common clock QLMC_REFCLK0_P/N input is unused, program GSER(0)_ANA_SEL[ANA_SEL]
                                                                 to 0x1FE.
                                                                 If both common clock QLMC_REFCLK0_P/N and QLMC_REFCLK1_P/N inputs are unused, program
                                                                 GSER(0)_ANA_SEL[ANA_SEL] to 0x1FC.

                                                                 For CCPI links used to power down the common clock input receiver to reduce power
                                                                 consumption if the common clock input is not used.
                                                                 If the common clock OCIC_REF_CLK_P/N input is unused, program GSER(8)_ANA_SEL[ANA_SEL]
                                                                 field to 0x1FC. */
#else /* Word 0 - Little Endian */
        uint64_t ana_sel               : 9;  /**< [  8:  0](R/W) Controls the adr_global input to the analog test block. Note that the
                                                                 GSER(0)_ANA_SEL.ANA_SEL register is tied to the analog test block, for non-CCPI links.
                                                                 Note that the GSER(8)_ANA_SEL.ANA_SEL register is tied to the analog test block, for
                                                                 CCPI links. The other GSER()_ANA_SEL registers are unused.
                                                                 For diagnostic use only.

                                                                 For non-CCPI links used to power down the common clock input receiver to reduce power
                                                                 consumption if the common clock input is not used.
                                                                 If the common clock QLMC_REFCLK1_P/N input is unused, program GSER(0)_ANA_SEL[ANA_SEL]
                                                                 to 0x1FD.
                                                                 If the common clock QLMC_REFCLK0_P/N input is unused, program GSER(0)_ANA_SEL[ANA_SEL]
                                                                 to 0x1FE.
                                                                 If both common clock QLMC_REFCLK0_P/N and QLMC_REFCLK1_P/N inputs are unused, program
                                                                 GSER(0)_ANA_SEL[ANA_SEL] to 0x1FC.

                                                                 For CCPI links used to power down the common clock input receiver to reduce power
                                                                 consumption if the common clock input is not used.
                                                                 If the common clock OCIC_REF_CLK_P/N input is unused, program GSER(8)_ANA_SEL[ANA_SEL]
                                                                 field to 0x1FC. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_gserx_ana_sel bdk_gserx_ana_sel_t;

static inline uint64_t BDK_GSERX_ANA_SEL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_ANA_SEL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000808ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000808ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000808ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_ANA_SEL", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_ANA_SEL(a) bdk_gserx_ana_sel_t
#define bustype_BDK_GSERX_ANA_SEL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_ANA_SEL(a) "GSERX_ANA_SEL"
#define device_bar_BDK_GSERX_ANA_SEL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_ANA_SEL(a) (a)
#define arguments_BDK_GSERX_ANA_SEL(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_br_rx#_ctl
 *
 * GSER Base-R RX Control Register
 */
union bdk_gserx_br_rxx_ctl
{
    uint64_t u;
    struct bdk_gserx_br_rxx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t rxt_adtmout_disable   : 1;  /**< [  3:  3](R/W) For BASE-R links the terminating condition for link training receiver adaptation
                                                                 is a 330 milliseconds time-out timer.  When the receiver adaptation time-out timer
                                                                 expires the receiver adaptation process is concluded and the link is considered good.
                                                                 Note that when BASE-R link training is performed under software control,
                                                                 (GSER()_BR_RX()_CTL[RXT_SWM] is set), the receiver adaptation time-out timer is disabled
                                                                 and not used.

                                                                 Set this bit to a one to disable the link training receiver adaptation time-out
                                                                 timer during BASE-R link training under hardware control.  For diagnostic use only. */
        uint64_t rxt_swm               : 1;  /**< [  2:  2](R/W) Set when RX BASE-R link training is to be performed under software control.

                                                                 See GSER()_BR_RX()_EER[EXT_EER]. */
        uint64_t rxt_preset            : 1;  /**< [  1:  1](R/W) For all link training, this bit determines how to configure the preset bit in the
                                                                 coefficient update message that is sent to the far end transmitter. When set, a one time
                                                                 request is made that the coefficients be set to a state where equalization is turned off.

                                                                 To perform a preset, set this bit prior to link training. Link training needs to be
                                                                 disabled to complete the request and get the rxtrain state machine back to idle. Note that
                                                                 it is illegal to set both the preset and initialize bits at the same time. For diagnostic
                                                                 use only. */
        uint64_t rxt_initialize        : 1;  /**< [  0:  0](R/W) For all link training, this bit determines how to configure the initialize bit in the
                                                                 coefficient update message that is sent to the far end transmitter of RX training. When
                                                                 set, a request is made that the coefficients be set to its INITIALIZE state. To perform an
                                                                 initialize prior to link training, set this bit prior to performing link training. Note
                                                                 that it is illegal to set both the preset and initialize bits at the same time. Since the
                                                                 far end transmitter is required to be initialized prior to starting link training, it is
                                                                 not expected that software will need to set this bit. For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t rxt_initialize        : 1;  /**< [  0:  0](R/W) For all link training, this bit determines how to configure the initialize bit in the
                                                                 coefficient update message that is sent to the far end transmitter of RX training. When
                                                                 set, a request is made that the coefficients be set to its INITIALIZE state. To perform an
                                                                 initialize prior to link training, set this bit prior to performing link training. Note
                                                                 that it is illegal to set both the preset and initialize bits at the same time. Since the
                                                                 far end transmitter is required to be initialized prior to starting link training, it is
                                                                 not expected that software will need to set this bit. For diagnostic use only. */
        uint64_t rxt_preset            : 1;  /**< [  1:  1](R/W) For all link training, this bit determines how to configure the preset bit in the
                                                                 coefficient update message that is sent to the far end transmitter. When set, a one time
                                                                 request is made that the coefficients be set to a state where equalization is turned off.

                                                                 To perform a preset, set this bit prior to link training. Link training needs to be
                                                                 disabled to complete the request and get the rxtrain state machine back to idle. Note that
                                                                 it is illegal to set both the preset and initialize bits at the same time. For diagnostic
                                                                 use only. */
        uint64_t rxt_swm               : 1;  /**< [  2:  2](R/W) Set when RX BASE-R link training is to be performed under software control.

                                                                 See GSER()_BR_RX()_EER[EXT_EER]. */
        uint64_t rxt_adtmout_disable   : 1;  /**< [  3:  3](R/W) For BASE-R links the terminating condition for link training receiver adaptation
                                                                 is a 330 milliseconds time-out timer.  When the receiver adaptation time-out timer
                                                                 expires the receiver adaptation process is concluded and the link is considered good.
                                                                 Note that when BASE-R link training is performed under software control,
                                                                 (GSER()_BR_RX()_CTL[RXT_SWM] is set), the receiver adaptation time-out timer is disabled
                                                                 and not used.

                                                                 Set this bit to a one to disable the link training receiver adaptation time-out
                                                                 timer during BASE-R link training under hardware control.  For diagnostic use only. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    struct bdk_gserx_br_rxx_ctl_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t reserved_3            : 1;
        uint64_t rxt_swm               : 1;  /**< [  2:  2](R/W) Set when RX BASE-R link training is to be performed under software control.

                                                                 See GSER()_BR_RX()_EER[EXT_EER]. */
        uint64_t rxt_preset            : 1;  /**< [  1:  1](R/W) For all link training, this bit determines how to configure the preset bit in the
                                                                 coefficient update message that is sent to the far end transmitter. When set, a one time
                                                                 request is made that the coefficients be set to a state where equalization is turned off.

                                                                 To perform a preset, set this bit prior to link training. Link training needs to be
                                                                 disabled to complete the request and get the rxtrain state machine back to idle. Note that
                                                                 it is illegal to set both the preset and initialize bits at the same time. For diagnostic
                                                                 use only. */
        uint64_t rxt_initialize        : 1;  /**< [  0:  0](R/W) For all link training, this bit determines how to configure the initialize bit in the
                                                                 coefficient update message that is sent to the far end transmitter of RX training. When
                                                                 set, a request is made that the coefficients be set to its INITIALIZE state. To perform an
                                                                 initialize prior to link training, set this bit prior to performing link training. Note
                                                                 that it is illegal to set both the preset and initialize bits at the same time. Since the
                                                                 far end transmitter is required to be initialized prior to starting link training, it is
                                                                 not expected that software will need to set this bit. For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t rxt_initialize        : 1;  /**< [  0:  0](R/W) For all link training, this bit determines how to configure the initialize bit in the
                                                                 coefficient update message that is sent to the far end transmitter of RX training. When
                                                                 set, a request is made that the coefficients be set to its INITIALIZE state. To perform an
                                                                 initialize prior to link training, set this bit prior to performing link training. Note
                                                                 that it is illegal to set both the preset and initialize bits at the same time. Since the
                                                                 far end transmitter is required to be initialized prior to starting link training, it is
                                                                 not expected that software will need to set this bit. For diagnostic use only. */
        uint64_t rxt_preset            : 1;  /**< [  1:  1](R/W) For all link training, this bit determines how to configure the preset bit in the
                                                                 coefficient update message that is sent to the far end transmitter. When set, a one time
                                                                 request is made that the coefficients be set to a state where equalization is turned off.

                                                                 To perform a preset, set this bit prior to link training. Link training needs to be
                                                                 disabled to complete the request and get the rxtrain state machine back to idle. Note that
                                                                 it is illegal to set both the preset and initialize bits at the same time. For diagnostic
                                                                 use only. */
        uint64_t rxt_swm               : 1;  /**< [  2:  2](R/W) Set when RX BASE-R link training is to be performed under software control.

                                                                 See GSER()_BR_RX()_EER[EXT_EER]. */
        uint64_t reserved_3            : 1;
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn88xxp1;
    /* struct bdk_gserx_br_rxx_ctl_s cn81xx; */
    /* struct bdk_gserx_br_rxx_ctl_s cn83xx; */
    /* struct bdk_gserx_br_rxx_ctl_s cn88xxp2; */
};
typedef union bdk_gserx_br_rxx_ctl bdk_gserx_br_rxx_ctl_t;

static inline uint64_t BDK_GSERX_BR_RXX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_BR_RXX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=3)))
        return 0x87e090000400ll + 0x1000000ll * ((a) & 0x3) + 0x80ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090000400ll + 0x1000000ll * ((a) & 0x7) + 0x80ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090000400ll + 0x1000000ll * ((a) & 0xf) + 0x80ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_BR_RXX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_BR_RXX_CTL(a,b) bdk_gserx_br_rxx_ctl_t
#define bustype_BDK_GSERX_BR_RXX_CTL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_BR_RXX_CTL(a,b) "GSERX_BR_RXX_CTL"
#define device_bar_BDK_GSERX_BR_RXX_CTL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_BR_RXX_CTL(a,b) (a)
#define arguments_BDK_GSERX_BR_RXX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_br_rx#_eer
 *
 * GSER Base-R RX Equalization Evaluation Request Register
 * GSER software BASE-R RX link training equalization evaluation request (EER). A write to
 * [RXT_EER] initiates a equalization request to the RAW PCS. A read of this register returns the
 * equalization status message and a valid bit indicating it was updated. These registers are for
 * diagnostic use only.
 */
union bdk_gserx_br_rxx_eer
{
    uint64_t u;
    struct bdk_gserx_br_rxx_eer_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t rxt_eer               : 1;  /**< [ 15: 15](WO) When RX BASE-R link training is being performed under software control,
                                                                 (GSER()_BR_RX()_CTL[RXT_SWM] is set), writing this bit initiates an equalization
                                                                 request to the RAW PCS. Reading this bit always returns a zero.

                                                                 When auto-negotiated link training is not present and link speed \>= 5 Gbaud,
                                                                 including XFI, receiver (only) equalization should be manually performed.
                                                                 After GSER()_BR_RX()_CTL[RXT_SWM] is set, writing this CSR with
                                                                 [RXT_EER]=1 initiates this manual equalization. The operation may take up to
                                                                 2 milliseconds, and then hardware sets [RXT_ESV]. The SerDes input should
                                                                 be a pattern (something similar to the BASE-R training sequence, ideally)
                                                                 during this receiver-only training. If DFE is to be disabled
                                                                 (recommended for 5 Gbaud and below), do it prior to this receiver-only
                                                                 initialization. (GSER()_LANE()_RX_VALBBD_CTRL_0, GSER()_LANE()_RX_VALBBD_CTRL_1,
                                                                 and GSER()_LANE()_RX_VALBBD_CTRL_2 configure the DFE.) */
        uint64_t rxt_esv               : 1;  /**< [ 14: 14](R/W) When performing an equalization request (RXT_EER), this bit, when set, indicates that the
                                                                 Equalization Status (RXT_ESM) is valid. When issuing a RXT_EER request, it is expected
                                                                 that RXT_ESV will get written to zero so that a valid RXT_ESM can be determined. */
        uint64_t rxt_esm               : 14; /**< [ 13:  0](RO) When performing an equalization request (RXT_EER), this is the equalization status message
                                                                 from the RAW PCS. It is valid when RXT_ESV is set.

                                                                 _ \<13:6\>: Figure of merit. An 8-bit output from the PHY indicating the quality of the
                                                                 received data eye. A higher value indicates better link equalization, with 8'd0 indicating
                                                                 worst equalization setting and 8'd255 indicating the best equalization setting.

                                                                 _ \<5:4\>: RX recommended TXPOST direction change.

                                                                 _ \<3:2\>: RX recommended TXMAIN direction change.

                                                                 _ \<1:0\>: RX recommended TXPRE direction change.

                                                                 Recommended direction change outputs from the PHY for the link partner transmitter
                                                                 coefficients.
                                                                 0x0 = Hold.
                                                                 0x1 = Increment.
                                                                 0x2 = Decrement.
                                                                 0x3 = Hold. */
#else /* Word 0 - Little Endian */
        uint64_t rxt_esm               : 14; /**< [ 13:  0](RO) When performing an equalization request (RXT_EER), this is the equalization status message
                                                                 from the RAW PCS. It is valid when RXT_ESV is set.

                                                                 _ \<13:6\>: Figure of merit. An 8-bit output from the PHY indicating the quality of the
                                                                 received data eye. A higher value indicates better link equalization, with 8'd0 indicating
                                                                 worst equalization setting and 8'd255 indicating the best equalization setting.

                                                                 _ \<5:4\>: RX recommended TXPOST direction change.

                                                                 _ \<3:2\>: RX recommended TXMAIN direction change.

                                                                 _ \<1:0\>: RX recommended TXPRE direction change.

                                                                 Recommended direction change outputs from the PHY for the link partner transmitter
                                                                 coefficients.
                                                                 0x0 = Hold.
                                                                 0x1 = Increment.
                                                                 0x2 = Decrement.
                                                                 0x3 = Hold. */
        uint64_t rxt_esv               : 1;  /**< [ 14: 14](R/W) When performing an equalization request (RXT_EER), this bit, when set, indicates that the
                                                                 Equalization Status (RXT_ESM) is valid. When issuing a RXT_EER request, it is expected
                                                                 that RXT_ESV will get written to zero so that a valid RXT_ESM can be determined. */
        uint64_t rxt_eer               : 1;  /**< [ 15: 15](WO) When RX BASE-R link training is being performed under software control,
                                                                 (GSER()_BR_RX()_CTL[RXT_SWM] is set), writing this bit initiates an equalization
                                                                 request to the RAW PCS. Reading this bit always returns a zero.

                                                                 When auto-negotiated link training is not present and link speed \>= 5 Gbaud,
                                                                 including XFI, receiver (only) equalization should be manually performed.
                                                                 After GSER()_BR_RX()_CTL[RXT_SWM] is set, writing this CSR with
                                                                 [RXT_EER]=1 initiates this manual equalization. The operation may take up to
                                                                 2 milliseconds, and then hardware sets [RXT_ESV]. The SerDes input should
                                                                 be a pattern (something similar to the BASE-R training sequence, ideally)
                                                                 during this receiver-only training. If DFE is to be disabled
                                                                 (recommended for 5 Gbaud and below), do it prior to this receiver-only
                                                                 initialization. (GSER()_LANE()_RX_VALBBD_CTRL_0, GSER()_LANE()_RX_VALBBD_CTRL_1,
                                                                 and GSER()_LANE()_RX_VALBBD_CTRL_2 configure the DFE.) */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_br_rxx_eer_s cn; */
};
typedef union bdk_gserx_br_rxx_eer bdk_gserx_br_rxx_eer_t;

static inline uint64_t BDK_GSERX_BR_RXX_EER(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_BR_RXX_EER(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=3)))
        return 0x87e090000418ll + 0x1000000ll * ((a) & 0x3) + 0x80ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090000418ll + 0x1000000ll * ((a) & 0x7) + 0x80ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090000418ll + 0x1000000ll * ((a) & 0xf) + 0x80ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_BR_RXX_EER", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_BR_RXX_EER(a,b) bdk_gserx_br_rxx_eer_t
#define bustype_BDK_GSERX_BR_RXX_EER(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_BR_RXX_EER(a,b) "GSERX_BR_RXX_EER"
#define device_bar_BDK_GSERX_BR_RXX_EER(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_BR_RXX_EER(a,b) (a)
#define arguments_BDK_GSERX_BR_RXX_EER(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_br_tx#_ctl
 *
 * GSER Base-R TX Control Register
 */
union bdk_gserx_br_txx_ctl
{
    uint64_t u;
    struct bdk_gserx_br_txx_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t txt_swm               : 1;  /**< [  0:  0](R/W) Set when TX BASE-R link training is to be performed under software control. For diagnostic
                                                                 use only. */
#else /* Word 0 - Little Endian */
        uint64_t txt_swm               : 1;  /**< [  0:  0](R/W) Set when TX BASE-R link training is to be performed under software control. For diagnostic
                                                                 use only. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_br_txx_ctl_s cn; */
};
typedef union bdk_gserx_br_txx_ctl bdk_gserx_br_txx_ctl_t;

static inline uint64_t BDK_GSERX_BR_TXX_CTL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_BR_TXX_CTL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=3)))
        return 0x87e090000420ll + 0x1000000ll * ((a) & 0x3) + 0x80ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090000420ll + 0x1000000ll * ((a) & 0x7) + 0x80ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090000420ll + 0x1000000ll * ((a) & 0xf) + 0x80ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_BR_TXX_CTL", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_BR_TXX_CTL(a,b) bdk_gserx_br_txx_ctl_t
#define bustype_BDK_GSERX_BR_TXX_CTL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_BR_TXX_CTL(a,b) "GSERX_BR_TXX_CTL"
#define device_bar_BDK_GSERX_BR_TXX_CTL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_BR_TXX_CTL(a,b) (a)
#define arguments_BDK_GSERX_BR_TXX_CTL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_br_tx#_cur
 *
 * GSER Base-R TX Coefficient Update Register
 */
union bdk_gserx_br_txx_cur
{
    uint64_t u;
    struct bdk_gserx_br_txx_cur_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t txt_cur               : 14; /**< [ 13:  0](R/W) When TX BASE-R link training is being performed under software control,
                                                                 (GSER()_BR_TX()_CTL[TXT_SWM] is set), this is the coefficient update to be written to the
                                                                 PHY.
                                                                 For diagnostic use only.
                                                                 \<13:9\> = TX_POST\<4:0\>.
                                                                 \<8:4\> = TX_SWING\<4:0\>.
                                                                 \<3:0\> = TX_PRE\<3:0\>. */
#else /* Word 0 - Little Endian */
        uint64_t txt_cur               : 14; /**< [ 13:  0](R/W) When TX BASE-R link training is being performed under software control,
                                                                 (GSER()_BR_TX()_CTL[TXT_SWM] is set), this is the coefficient update to be written to the
                                                                 PHY.
                                                                 For diagnostic use only.
                                                                 \<13:9\> = TX_POST\<4:0\>.
                                                                 \<8:4\> = TX_SWING\<4:0\>.
                                                                 \<3:0\> = TX_PRE\<3:0\>. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_br_txx_cur_s cn; */
};
typedef union bdk_gserx_br_txx_cur bdk_gserx_br_txx_cur_t;

static inline uint64_t BDK_GSERX_BR_TXX_CUR(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_BR_TXX_CUR(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=3)))
        return 0x87e090000438ll + 0x1000000ll * ((a) & 0x3) + 0x80ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090000438ll + 0x1000000ll * ((a) & 0x7) + 0x80ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090000438ll + 0x1000000ll * ((a) & 0xf) + 0x80ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_BR_TXX_CUR", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_BR_TXX_CUR(a,b) bdk_gserx_br_txx_cur_t
#define bustype_BDK_GSERX_BR_TXX_CUR(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_BR_TXX_CUR(a,b) "GSERX_BR_TXX_CUR"
#define device_bar_BDK_GSERX_BR_TXX_CUR(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_BR_TXX_CUR(a,b) (a)
#define arguments_BDK_GSERX_BR_TXX_CUR(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_br_tx#_ini
 *
 * GSER Base-R TX Coefficient Tap Initialize Register
 * GSER BASE-R link training TX taps equalization initialize value. When BASE-R hardware link
 * training is enabled the transmitter
 * equalizer taps (Pre/Swing/Post) are initialized with the values in this register.  Also,
 * during 10GBase-KR hardware link training if a
 * coefficient update request message is received from the link partner with the initialize
 * control bit set the local device transmitter
 * taps (Pre/Swing/Post) will be updated with the values in this register.
 */
union bdk_gserx_br_txx_ini
{
    uint64_t u;
    struct bdk_gserx_br_txx_ini_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t txt_post_init         : 5;  /**< [ 13:  9](R/W/H) During TX BASE-R link training, the TX post-tap value that is used
                                                                 when the initialize coefficients update is received. It is also the TX post-tap
                                                                 value used when the BASE-R link training begins.
                                                                 For diagnostic use only. */
        uint64_t txt_swing_init        : 5;  /**< [  8:  4](R/W/H) During TX BASE-R link training, the TX swing-tap value that is used
                                                                 when the initialize coefficients update is received. It is also the TX swing-tap
                                                                 value used when the BASE-R link training begins.
                                                                 For diagnostic use only. */
        uint64_t txt_pre_init          : 4;  /**< [  3:  0](R/W/H) During TX BASE-R link training, the TX pre-tap value that is used
                                                                 when the initialize coefficients update is received. It is also the TX pre-tap
                                                                 value used when the BASE-R link training begins.
                                                                 For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t txt_pre_init          : 4;  /**< [  3:  0](R/W/H) During TX BASE-R link training, the TX pre-tap value that is used
                                                                 when the initialize coefficients update is received. It is also the TX pre-tap
                                                                 value used when the BASE-R link training begins.
                                                                 For diagnostic use only. */
        uint64_t txt_swing_init        : 5;  /**< [  8:  4](R/W/H) During TX BASE-R link training, the TX swing-tap value that is used
                                                                 when the initialize coefficients update is received. It is also the TX swing-tap
                                                                 value used when the BASE-R link training begins.
                                                                 For diagnostic use only. */
        uint64_t txt_post_init         : 5;  /**< [ 13:  9](R/W/H) During TX BASE-R link training, the TX post-tap value that is used
                                                                 when the initialize coefficients update is received. It is also the TX post-tap
                                                                 value used when the BASE-R link training begins.
                                                                 For diagnostic use only. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_br_txx_ini_s cn; */
};
typedef union bdk_gserx_br_txx_ini bdk_gserx_br_txx_ini_t;

static inline uint64_t BDK_GSERX_BR_TXX_INI(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_BR_TXX_INI(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=3)))
        return 0x87e090000448ll + 0x1000000ll * ((a) & 0x3) + 0x80ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090000448ll + 0x1000000ll * ((a) & 0x7) + 0x80ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_BR_TXX_INI", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_BR_TXX_INI(a,b) bdk_gserx_br_txx_ini_t
#define bustype_BDK_GSERX_BR_TXX_INI(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_BR_TXX_INI(a,b) "GSERX_BR_TXX_INI"
#define device_bar_BDK_GSERX_BR_TXX_INI(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_BR_TXX_INI(a,b) (a)
#define arguments_BDK_GSERX_BR_TXX_INI(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_br_tx#_tap
 *
 * GSER Base-R TX Coefficient Tap Register
 */
union bdk_gserx_br_txx_tap
{
    uint64_t u;
    struct bdk_gserx_br_txx_tap_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t txt_pre               : 4;  /**< [ 13: 10](RO/H) After TX BASE-R link training, this is the resultant POST Tap value that was
                                                                 written to the PHY.  This field has no meaning if TX BASE-R link training was
                                                                 not performed.
                                                                 For diagnostic use only. */
        uint64_t txt_swing             : 5;  /**< [  9:  5](RO/H) After TX BASE-R link training, this is the resultant SWING Tap value that was
                                                                 written to the PHY.  This field has no meaning if TX BASE-R link training was
                                                                 not performed.
                                                                 For diagnostic use only. */
        uint64_t txt_post              : 5;  /**< [  4:  0](RO/H) After TX BASE-R link training, this is the resultant POST Tap value that was
                                                                 written to the PHY.  This field has no meaning if TX BASE-R link training was
                                                                 not performed.
                                                                 For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t txt_post              : 5;  /**< [  4:  0](RO/H) After TX BASE-R link training, this is the resultant POST Tap value that was
                                                                 written to the PHY.  This field has no meaning if TX BASE-R link training was
                                                                 not performed.
                                                                 For diagnostic use only. */
        uint64_t txt_swing             : 5;  /**< [  9:  5](RO/H) After TX BASE-R link training, this is the resultant SWING Tap value that was
                                                                 written to the PHY.  This field has no meaning if TX BASE-R link training was
                                                                 not performed.
                                                                 For diagnostic use only. */
        uint64_t txt_pre               : 4;  /**< [ 13: 10](RO/H) After TX BASE-R link training, this is the resultant POST Tap value that was
                                                                 written to the PHY.  This field has no meaning if TX BASE-R link training was
                                                                 not performed.
                                                                 For diagnostic use only. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_br_txx_tap_s cn; */
};
typedef union bdk_gserx_br_txx_tap bdk_gserx_br_txx_tap_t;

static inline uint64_t BDK_GSERX_BR_TXX_TAP(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_BR_TXX_TAP(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=3)))
        return 0x87e090000440ll + 0x1000000ll * ((a) & 0x3) + 0x80ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090000440ll + 0x1000000ll * ((a) & 0x7) + 0x80ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090000440ll + 0x1000000ll * ((a) & 0xf) + 0x80ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_BR_TXX_TAP", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_BR_TXX_TAP(a,b) bdk_gserx_br_txx_tap_t
#define bustype_BDK_GSERX_BR_TXX_TAP(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_BR_TXX_TAP(a,b) "GSERX_BR_TXX_TAP"
#define device_bar_BDK_GSERX_BR_TXX_TAP(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_BR_TXX_TAP(a,b) (a)
#define arguments_BDK_GSERX_BR_TXX_TAP(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_cfg
 *
 * GSER Configuration Register
 */
union bdk_gserx_cfg
{
    uint64_t u;
    struct bdk_gserx_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t sata                  : 1;  /**< [  5:  5](R/W) When set, indicates the GSER is configured for SATA mode. [SATA] must not be set
                                                                 when either of [BGX,PCIE] are set. */
        uint64_t bgx_quad              : 1;  /**< [  4:  4](R/W) When set, indicates the QLM is in BGX quad aggregation mode. [BGX_QUAD] must only be
                                                                 set when [BGX] is set and [BGX_DUAL] is clear.

                                                                 When [BGX_QUAD] is set, GSER bundles all four lanes for one BGX controller.
                                                                 [BGX_QUAD] must only be set for the XAUI/DXAUI protocols.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t bgx_dual              : 1;  /**< [  3:  3](R/W) When set, indicates the QLM is in BGX dual aggregation mode. [BGX_DUAL] must only be
                                                                 set when [BGX] is also set and [BGX_QUAD] is clear.

                                                                 When [BGX_DUAL] is set, GSER bundles lanes 0 and 1 for one BGX controller and bundles
                                                                 lanes 2 and 3 for another BGX controller. [BGX_DUAL] must only be set for the RXAUI
                                                                 protocol.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t bgx                   : 1;  /**< [  2:  2](R/W) When set, indicates the GSER is configured for BGX mode. [BGX] must not be set
                                                                 when either of [PCIE,SATA] are set.

                                                                 When [BGX] is set and both [BGX_DUAL,BGX_QUAD] are clear, GSER exposes each lane to an
                                                                 independent BGX controller.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t ila                   : 1;  /**< [  1:  1](R/W) Reserved. */
        uint64_t pcie                  : 1;  /**< [  0:  0](R/W/H) When set, indicates the GSER is configured for PCIE mode. [PCIE] must not be
                                                                 set when either of [BGX,SATA] is set.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
#else /* Word 0 - Little Endian */
        uint64_t pcie                  : 1;  /**< [  0:  0](R/W/H) When set, indicates the GSER is configured for PCIE mode. [PCIE] must not be
                                                                 set when either of [BGX,SATA] is set.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t ila                   : 1;  /**< [  1:  1](R/W) Reserved. */
        uint64_t bgx                   : 1;  /**< [  2:  2](R/W) When set, indicates the GSER is configured for BGX mode. [BGX] must not be set
                                                                 when either of [PCIE,SATA] are set.

                                                                 When [BGX] is set and both [BGX_DUAL,BGX_QUAD] are clear, GSER exposes each lane to an
                                                                 independent BGX controller.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t bgx_dual              : 1;  /**< [  3:  3](R/W) When set, indicates the QLM is in BGX dual aggregation mode. [BGX_DUAL] must only be
                                                                 set when [BGX] is also set and [BGX_QUAD] is clear.

                                                                 When [BGX_DUAL] is set, GSER bundles lanes 0 and 1 for one BGX controller and bundles
                                                                 lanes 2 and 3 for another BGX controller. [BGX_DUAL] must only be set for the RXAUI
                                                                 protocol.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t bgx_quad              : 1;  /**< [  4:  4](R/W) When set, indicates the QLM is in BGX quad aggregation mode. [BGX_QUAD] must only be
                                                                 set when [BGX] is set and [BGX_DUAL] is clear.

                                                                 When [BGX_QUAD] is set, GSER bundles all four lanes for one BGX controller.
                                                                 [BGX_QUAD] must only be set for the XAUI/DXAUI protocols.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t sata                  : 1;  /**< [  5:  5](R/W) When set, indicates the GSER is configured for SATA mode. [SATA] must not be set
                                                                 when either of [BGX,PCIE] are set. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } s;
    struct bdk_gserx_cfg_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t sata                  : 1;  /**< [  5:  5](R/W) When set, indicates the GSER is configured for SATA mode. [SATA] must not be set
                                                                 when either of [BGX,PCIE] are set. [SATA] must only be set for DLM3 (i.e. GSER3). */
        uint64_t bgx_quad              : 1;  /**< [  4:  4](R/W) When set, indicates the QLM is in BGX quad aggregation mode. [BGX_QUAD] must only be
                                                                 set when [BGX] is set and [BGX_DUAL] is clear.

                                                                 When [BGX_QUAD] is set, GSER bundles all four lanes for one BGX controller.
                                                                 [BGX_QUAD] must only be set for the XAUI/DXAUI protocols.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t bgx_dual              : 1;  /**< [  3:  3](R/W) When set, indicates the QLM is in BGX dual aggregation mode. [BGX_DUAL] must only be
                                                                 set when [BGX] is also set and [BGX_QUAD] is clear.

                                                                 When [BGX_DUAL] is set, GSER bundles lanes 0 and 1 for one BGX controller and bundles
                                                                 lanes 2 and 3 for another BGX controller. [BGX_DUAL] must only be set for the RXAUI
                                                                 protocol.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t bgx                   : 1;  /**< [  2:  2](R/W) When set, indicates the GSER is configured for BGX mode. [BGX] must not be set
                                                                 when either of [PCIE,SATA] are set.

                                                                 When [BGX] is set and both [BGX_DUAL,BGX_QUAD] are clear, GSER exposes each lane to an
                                                                 independent BGX controller.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t ila                   : 1;  /**< [  1:  1](R/W) Reserved. */
        uint64_t pcie                  : 1;  /**< [  0:  0](R/W/H) When set, indicates the GSER is configured for PCIE mode. [PCIE] must not be
                                                                 set when either of [BGX,SATA] is set.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
#else /* Word 0 - Little Endian */
        uint64_t pcie                  : 1;  /**< [  0:  0](R/W/H) When set, indicates the GSER is configured for PCIE mode. [PCIE] must not be
                                                                 set when either of [BGX,SATA] is set.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t ila                   : 1;  /**< [  1:  1](R/W) Reserved. */
        uint64_t bgx                   : 1;  /**< [  2:  2](R/W) When set, indicates the GSER is configured for BGX mode. [BGX] must not be set
                                                                 when either of [PCIE,SATA] are set.

                                                                 When [BGX] is set and both [BGX_DUAL,BGX_QUAD] are clear, GSER exposes each lane to an
                                                                 independent BGX controller.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t bgx_dual              : 1;  /**< [  3:  3](R/W) When set, indicates the QLM is in BGX dual aggregation mode. [BGX_DUAL] must only be
                                                                 set when [BGX] is also set and [BGX_QUAD] is clear.

                                                                 When [BGX_DUAL] is set, GSER bundles lanes 0 and 1 for one BGX controller and bundles
                                                                 lanes 2 and 3 for another BGX controller. [BGX_DUAL] must only be set for the RXAUI
                                                                 protocol.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t bgx_quad              : 1;  /**< [  4:  4](R/W) When set, indicates the QLM is in BGX quad aggregation mode. [BGX_QUAD] must only be
                                                                 set when [BGX] is set and [BGX_DUAL] is clear.

                                                                 When [BGX_QUAD] is set, GSER bundles all four lanes for one BGX controller.
                                                                 [BGX_QUAD] must only be set for the XAUI/DXAUI protocols.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t sata                  : 1;  /**< [  5:  5](R/W) When set, indicates the GSER is configured for SATA mode. [SATA] must not be set
                                                                 when either of [BGX,PCIE] are set. [SATA] must only be set for DLM3 (i.e. GSER3). */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } cn81xx;
    struct bdk_gserx_cfg_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t sata                  : 1;  /**< [  5:  5](R/W) When set, indicates the GSER is configured for SATA mode. [SATA] must not be set
                                                                 when either of [BGX,PCIE] are set. */
        uint64_t bgx_quad              : 1;  /**< [  4:  4](R/W) When set, indicates the QLM is in BGX quad aggregation mode. [BGX_QUAD] must only be
                                                                 set when [BGX] is set and [BGX_DUAL] is clear.

                                                                 When [BGX_QUAD] is set, GSER bundles all four lanes for one BGX controller.
                                                                 [BGX_QUAD] must only be set for the XAUI/DXAUI and XLAUI protocols.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t bgx_dual              : 1;  /**< [  3:  3](R/W) When set, indicates the QLM is in BGX dual aggregation mode. [BGX_DUAL] must only be
                                                                 set when [BGX] is also set and [BGX_QUAD] is clear.

                                                                 When [BGX_DUAL] is set, GSER bundles lanes 0 and 1 for one BGX controller and bundles
                                                                 lanes 2 and 3 for another BGX controller. [BGX_DUAL] must only be set for the RXAUI
                                                                 protocol.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t bgx                   : 1;  /**< [  2:  2](R/W) When set, indicates the GSER is configured for BGX mode. [BGX] must not be set
                                                                 when either of [PCIE,SATA] are set. For CCPI links, [BGX] must be clear.

                                                                 When [BGX] is set and both [BGX_DUAL,BGX_QUAD] are clear, GSER exposes each lane to an
                                                                 independent BGX controller.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t ila                   : 1;  /**< [  1:  1](R/W) Reserved. */
        uint64_t pcie                  : 1;  /**< [  0:  0](R/W/H) When set, indicates the GSER is configured for PCIE mode. [PCIE] must not be
                                                                 set when either of [BGX,SATA] is set. For CCPI QLMs, [PCIE] must be clear.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
#else /* Word 0 - Little Endian */
        uint64_t pcie                  : 1;  /**< [  0:  0](R/W/H) When set, indicates the GSER is configured for PCIE mode. [PCIE] must not be
                                                                 set when either of [BGX,SATA] is set. For CCPI QLMs, [PCIE] must be clear.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t ila                   : 1;  /**< [  1:  1](R/W) Reserved. */
        uint64_t bgx                   : 1;  /**< [  2:  2](R/W) When set, indicates the GSER is configured for BGX mode. [BGX] must not be set
                                                                 when either of [PCIE,SATA] are set. For CCPI links, [BGX] must be clear.

                                                                 When [BGX] is set and both [BGX_DUAL,BGX_QUAD] are clear, GSER exposes each lane to an
                                                                 independent BGX controller.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t bgx_dual              : 1;  /**< [  3:  3](R/W) When set, indicates the QLM is in BGX dual aggregation mode. [BGX_DUAL] must only be
                                                                 set when [BGX] is also set and [BGX_QUAD] is clear.

                                                                 When [BGX_DUAL] is set, GSER bundles lanes 0 and 1 for one BGX controller and bundles
                                                                 lanes 2 and 3 for another BGX controller. [BGX_DUAL] must only be set for the RXAUI
                                                                 protocol.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t bgx_quad              : 1;  /**< [  4:  4](R/W) When set, indicates the QLM is in BGX quad aggregation mode. [BGX_QUAD] must only be
                                                                 set when [BGX] is set and [BGX_DUAL] is clear.

                                                                 When [BGX_QUAD] is set, GSER bundles all four lanes for one BGX controller.
                                                                 [BGX_QUAD] must only be set for the XAUI/DXAUI and XLAUI protocols.

                                                                 Internal:
                                                                 Not used in CCPI QLMs. */
        uint64_t sata                  : 1;  /**< [  5:  5](R/W) When set, indicates the GSER is configured for SATA mode. [SATA] must not be set
                                                                 when either of [BGX,PCIE] are set. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_gserx_cfg_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t sata                  : 1;  /**< [  5:  5](R/W) When set, indicates the GSER is configured for SATA mode. [SATA] must not be set
                                                                 when either of [BGX,PCIE] are set. */
        uint64_t bgx_quad              : 1;  /**< [  4:  4](R/W) When set, indicates the QLM is in BGX quad aggregation mode. [BGX_QUAD] must only be
                                                                 set when [BGX] is set and [BGX_DUAL] is clear.

                                                                 When [BGX_QUAD] is set, GSER bundles all four lanes for one BGX controller.
                                                                 [BGX_QUAD] must only be set for the XAUI/DXAUI and XLAUI protocols.

                                                                 Internal:
                                                                 There is hardware to pair DLM 5 and 6 together when [BGX_QUAD] is set in DLM5.
                                                                 But we currently do not support XAUI/DXAUI/XLAUI on DLM's. */
        uint64_t bgx_dual              : 1;  /**< [  3:  3](R/W) When set, indicates the QLM is in BGX dual aggregation mode. [BGX_DUAL] must only be
                                                                 set when [BGX] is also set and [BGX_QUAD] is clear.

                                                                 When [BGX_DUAL] is set, GSER bundles lanes 0 and 1 for one BGX controller and bundles
                                                                 lanes 2 and 3 for another BGX controller. [BGX_DUAL] must only be set for the RXAUI
                                                                 protocol.

                                                                 [BGX_DUAL] must not be set in a DLM.

                                                                 Internal:
                                                                 [BGX_DUAL] should work in a DLM (lanes 0 and 1 bundled for one BGX controller), but
                                                                 we currently do not support RXAUI in a DLM. */
        uint64_t bgx                   : 1;  /**< [  2:  2](R/W) When set, indicates the GSER is configured for BGX mode. [BGX] must not be set
                                                                 when either of [PCIE,SATA] are set.

                                                                 When [BGX] is set and both [BGX_DUAL,BGX_QUAD] are clear, GSER exposes each lane to an
                                                                 independent BGX controller. */
        uint64_t ila                   : 1;  /**< [  1:  1](R/W) Reserved. */
        uint64_t pcie                  : 1;  /**< [  0:  0](R/W/H) When set, indicates the GSER is configured for PCIE mode. [PCIE] must not be
                                                                 set when either of [BGX,SATA] is set. */
#else /* Word 0 - Little Endian */
        uint64_t pcie                  : 1;  /**< [  0:  0](R/W/H) When set, indicates the GSER is configured for PCIE mode. [PCIE] must not be
                                                                 set when either of [BGX,SATA] is set. */
        uint64_t ila                   : 1;  /**< [  1:  1](R/W) Reserved. */
        uint64_t bgx                   : 1;  /**< [  2:  2](R/W) When set, indicates the GSER is configured for BGX mode. [BGX] must not be set
                                                                 when either of [PCIE,SATA] are set.

                                                                 When [BGX] is set and both [BGX_DUAL,BGX_QUAD] are clear, GSER exposes each lane to an
                                                                 independent BGX controller. */
        uint64_t bgx_dual              : 1;  /**< [  3:  3](R/W) When set, indicates the QLM is in BGX dual aggregation mode. [BGX_DUAL] must only be
                                                                 set when [BGX] is also set and [BGX_QUAD] is clear.

                                                                 When [BGX_DUAL] is set, GSER bundles lanes 0 and 1 for one BGX controller and bundles
                                                                 lanes 2 and 3 for another BGX controller. [BGX_DUAL] must only be set for the RXAUI
                                                                 protocol.

                                                                 [BGX_DUAL] must not be set in a DLM.

                                                                 Internal:
                                                                 [BGX_DUAL] should work in a DLM (lanes 0 and 1 bundled for one BGX controller), but
                                                                 we currently do not support RXAUI in a DLM. */
        uint64_t bgx_quad              : 1;  /**< [  4:  4](R/W) When set, indicates the QLM is in BGX quad aggregation mode. [BGX_QUAD] must only be
                                                                 set when [BGX] is set and [BGX_DUAL] is clear.

                                                                 When [BGX_QUAD] is set, GSER bundles all four lanes for one BGX controller.
                                                                 [BGX_QUAD] must only be set for the XAUI/DXAUI and XLAUI protocols.

                                                                 Internal:
                                                                 There is hardware to pair DLM 5 and 6 together when [BGX_QUAD] is set in DLM5.
                                                                 But we currently do not support XAUI/DXAUI/XLAUI on DLM's. */
        uint64_t sata                  : 1;  /**< [  5:  5](R/W) When set, indicates the GSER is configured for SATA mode. [SATA] must not be set
                                                                 when either of [BGX,PCIE] are set. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_cfg bdk_gserx_cfg_t;

static inline uint64_t BDK_GSERX_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000080ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000080ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000080ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_CFG(a) bdk_gserx_cfg_t
#define bustype_BDK_GSERX_CFG(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_CFG(a) "GSERX_CFG"
#define device_bar_BDK_GSERX_CFG(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_CFG(a) (a)
#define arguments_BDK_GSERX_CFG(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_dbg
 *
 * GSER Debug Control Register
 */
union bdk_gserx_dbg
{
    uint64_t u;
    struct bdk_gserx_dbg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t rxqtm_on              : 1;  /**< [  0:  0](R/W) For non-BGX configurations, setting this bit enables the RX FIFOs. This allows
                                                                 received data to become visible to the RSL debug port. For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t rxqtm_on              : 1;  /**< [  0:  0](R/W) For non-BGX configurations, setting this bit enables the RX FIFOs. This allows
                                                                 received data to become visible to the RSL debug port. For diagnostic use only. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_dbg_s cn; */
};
typedef union bdk_gserx_dbg bdk_gserx_dbg_t;

static inline uint64_t BDK_GSERX_DBG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_DBG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000098ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000098ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000098ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_DBG", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_DBG(a) bdk_gserx_dbg_t
#define bustype_BDK_GSERX_DBG(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_DBG(a) "GSERX_DBG"
#define device_bar_BDK_GSERX_DBG(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_DBG(a) (a)
#define arguments_BDK_GSERX_DBG(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_eq_wait_time
 *
 * GSER TX and RX Equalization Wait Times Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_eq_wait_time
{
    uint64_t u;
    struct bdk_gserx_eq_wait_time_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t rxeq_wait_cnt         : 4;  /**< [  7:  4](R/W) Determines the wait time after VMA RX-EQ completes and before sampling
                                                                 tap1 and starting the precorrelation check. */
        uint64_t txeq_wait_cnt         : 4;  /**< [  3:  0](R/W) Determines the wait time from applying the TX-EQ controls (swing/pre/post)
                                                                 to the sampling of the sds_pcs_tx_comp_out. */
#else /* Word 0 - Little Endian */
        uint64_t txeq_wait_cnt         : 4;  /**< [  3:  0](R/W) Determines the wait time from applying the TX-EQ controls (swing/pre/post)
                                                                 to the sampling of the sds_pcs_tx_comp_out. */
        uint64_t rxeq_wait_cnt         : 4;  /**< [  7:  4](R/W) Determines the wait time after VMA RX-EQ completes and before sampling
                                                                 tap1 and starting the precorrelation check. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_eq_wait_time_s cn; */
};
typedef union bdk_gserx_eq_wait_time bdk_gserx_eq_wait_time_t;

static inline uint64_t BDK_GSERX_EQ_WAIT_TIME(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_EQ_WAIT_TIME(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0904e0000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0904e0000ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0904e0000ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_EQ_WAIT_TIME", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_EQ_WAIT_TIME(a) bdk_gserx_eq_wait_time_t
#define bustype_BDK_GSERX_EQ_WAIT_TIME(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_EQ_WAIT_TIME(a) "GSERX_EQ_WAIT_TIME"
#define device_bar_BDK_GSERX_EQ_WAIT_TIME(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_EQ_WAIT_TIME(a) (a)
#define arguments_BDK_GSERX_EQ_WAIT_TIME(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_glbl_misc_config_1
 *
 * GSER Global Miscellaneous Configuration 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_glbl_misc_config_1
{
    uint64_t u;
    struct bdk_gserx_glbl_misc_config_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t pcs_sds_vref_tr       : 4;  /**< [  9:  6](R/W) Trim the BGR (band gap reference) reference (all external and internal currents
                                                                 are affected).
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_trim_chp_reg  : 2;  /**< [  5:  4](R/W) Trim current going to CML-CMOS stage at output of VCO.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_vco_reg_tr    : 2;  /**< [  3:  2](R/W) Trims regulator voltage.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_cvbg_en       : 1;  /**< [  1:  1](R/W) Forces 0.6 V from VDDHV onto VBG node.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_extvbg_en     : 1;  /**< [  0:  0](R/W) Force external VBG through AMON pin in TMA5 mode.
                                                                 For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_extvbg_en     : 1;  /**< [  0:  0](R/W) Force external VBG through AMON pin in TMA5 mode.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_cvbg_en       : 1;  /**< [  1:  1](R/W) Forces 0.6 V from VDDHV onto VBG node.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_vco_reg_tr    : 2;  /**< [  3:  2](R/W) Trims regulator voltage.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_trim_chp_reg  : 2;  /**< [  5:  4](R/W) Trim current going to CML-CMOS stage at output of VCO.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_vref_tr       : 4;  /**< [  9:  6](R/W) Trim the BGR (band gap reference) reference (all external and internal currents
                                                                 are affected).
                                                                 For diagnostic use only. */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_glbl_misc_config_1_s cn; */
};
typedef union bdk_gserx_glbl_misc_config_1 bdk_gserx_glbl_misc_config_1_t;

static inline uint64_t BDK_GSERX_GLBL_MISC_CONFIG_1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_GLBL_MISC_CONFIG_1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090460030ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090460030ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090460030ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_GLBL_MISC_CONFIG_1", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_GLBL_MISC_CONFIG_1(a) bdk_gserx_glbl_misc_config_1_t
#define bustype_BDK_GSERX_GLBL_MISC_CONFIG_1(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_GLBL_MISC_CONFIG_1(a) "GSERX_GLBL_MISC_CONFIG_1"
#define device_bar_BDK_GSERX_GLBL_MISC_CONFIG_1(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_GLBL_MISC_CONFIG_1(a) (a)
#define arguments_BDK_GSERX_GLBL_MISC_CONFIG_1(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_glbl_pll_cfg_0
 *
 * GSER Global PLL Configuration 0 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_glbl_pll_cfg_0
{
    uint64_t u;
    struct bdk_gserx_glbl_pll_cfg_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t pcs_sds_pll_vco_reset_b : 1;/**< [ 13: 13](R/W) VCO reset, active low.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_pll_strt_cal_b : 1; /**< [ 12: 12](R/W) Start PLL calibration, active low.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_pll_cripple   : 1;  /**< [ 11: 11](R/W) Ripple capacitor tuning.
                                                                 For diagnostic use only. */
        uint64_t reserved_8_10         : 3;
        uint64_t pcs_sds_pll_fthresh   : 2;  /**< [  7:  6](R/W/H) PLL frequency comparison threshold.
                                                                 For diagnostic use only. */
        uint64_t reserved_0_5          : 6;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_5          : 6;
        uint64_t pcs_sds_pll_fthresh   : 2;  /**< [  7:  6](R/W/H) PLL frequency comparison threshold.
                                                                 For diagnostic use only. */
        uint64_t reserved_8_10         : 3;
        uint64_t pcs_sds_pll_cripple   : 1;  /**< [ 11: 11](R/W) Ripple capacitor tuning.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_pll_strt_cal_b : 1; /**< [ 12: 12](R/W) Start PLL calibration, active low.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_pll_vco_reset_b : 1;/**< [ 13: 13](R/W) VCO reset, active low.
                                                                 For diagnostic use only. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_glbl_pll_cfg_0_s cn; */
};
typedef union bdk_gserx_glbl_pll_cfg_0 bdk_gserx_glbl_pll_cfg_0_t;

static inline uint64_t BDK_GSERX_GLBL_PLL_CFG_0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_GLBL_PLL_CFG_0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090460000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090460000ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090460000ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_GLBL_PLL_CFG_0", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_GLBL_PLL_CFG_0(a) bdk_gserx_glbl_pll_cfg_0_t
#define bustype_BDK_GSERX_GLBL_PLL_CFG_0(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_GLBL_PLL_CFG_0(a) "GSERX_GLBL_PLL_CFG_0"
#define device_bar_BDK_GSERX_GLBL_PLL_CFG_0(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_GLBL_PLL_CFG_0(a) (a)
#define arguments_BDK_GSERX_GLBL_PLL_CFG_0(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_glbl_pll_cfg_1
 *
 * GSER Global PLL Configuration 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_glbl_pll_cfg_1
{
    uint64_t u;
    struct bdk_gserx_glbl_pll_cfg_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t cfg_pll_ctrl_en       : 1;  /**< [  9:  9](R/W) PLL reset control enable.
                                                                 0 = PLL RESETs/cal start are not active.
                                                                 1 = All PLL RESETs/cal start are enabled.

                                                                 For diagnostic use only. */
        uint64_t pcs_sds_pll_calmode   : 3;  /**< [  8:  6](R/W) PLL calibration mode.
                                                                 0 = Force PLL loop into calibration mode.
                                                                 1 = Normal operation.

                                                                 For diagnostic use only. */
        uint64_t pcs_sds_pll_cal_ovrd_en : 1;/**< [  5:  5](R/W) Manual PLL coarse calibration override enable.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_pll_cal_ovrd  : 5;  /**< [  4:  0](R/W) Manual PLL coarse calibration override value.
                                                                 For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_pll_cal_ovrd  : 5;  /**< [  4:  0](R/W) Manual PLL coarse calibration override value.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_pll_cal_ovrd_en : 1;/**< [  5:  5](R/W) Manual PLL coarse calibration override enable.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_pll_calmode   : 3;  /**< [  8:  6](R/W) PLL calibration mode.
                                                                 0 = Force PLL loop into calibration mode.
                                                                 1 = Normal operation.

                                                                 For diagnostic use only. */
        uint64_t cfg_pll_ctrl_en       : 1;  /**< [  9:  9](R/W) PLL reset control enable.
                                                                 0 = PLL RESETs/cal start are not active.
                                                                 1 = All PLL RESETs/cal start are enabled.

                                                                 For diagnostic use only. */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_glbl_pll_cfg_1_s cn; */
};
typedef union bdk_gserx_glbl_pll_cfg_1 bdk_gserx_glbl_pll_cfg_1_t;

static inline uint64_t BDK_GSERX_GLBL_PLL_CFG_1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_GLBL_PLL_CFG_1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090460008ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090460008ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090460008ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_GLBL_PLL_CFG_1", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_GLBL_PLL_CFG_1(a) bdk_gserx_glbl_pll_cfg_1_t
#define bustype_BDK_GSERX_GLBL_PLL_CFG_1(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_GLBL_PLL_CFG_1(a) "GSERX_GLBL_PLL_CFG_1"
#define device_bar_BDK_GSERX_GLBL_PLL_CFG_1(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_GLBL_PLL_CFG_1(a) (a)
#define arguments_BDK_GSERX_GLBL_PLL_CFG_1(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_glbl_pll_cfg_2
 *
 * GSER Global PLL Configuration 2 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_glbl_pll_cfg_2
{
    uint64_t u;
    struct bdk_gserx_glbl_pll_cfg_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t pll_div_ovrrd_en      : 1;  /**< [ 14: 14](R/W) Override global power state machine and mac_pcs_pll_div control signal.
                                                                 When asserted, pcs_sds_pll_div is specified from
                                                                 GSER()_LANE()_PCS_PLL_CTLIFC_0[PLL_DIV_OVRRD_VAL],
                                                                 global power state machine and mac_pcs_pll_div control signals are ignored.
                                                                 For diagnostic use only. */
        uint64_t reserved_10_13        : 4;
        uint64_t pcs_sds_pll_lock_override : 1;/**< [  9:  9](R/W) Not used.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_pll_counter_resetn : 1;/**< [  8:  8](R/W) Not used.
                                                                 For diagnostic use only. */
        uint64_t pll_sdsck_pd_ovrrd_val : 1; /**< [  7:  7](R/W) Clock tree powerdown override value.
                                                                 For diagnostic use only. */
        uint64_t pll_sdsck_pd_ovrrd_en : 1;  /**< [  6:  6](R/W) Clock tree powerdown override enable.
                                                                 For diagnostic use only. */
        uint64_t pll_pd_ovrrd_val      : 1;  /**< [  5:  5](R/W) PLL powerdown override value.
                                                                 For diagnostic use only. */
        uint64_t pll_pd_ovrrd_en       : 1;  /**< [  4:  4](R/W) When asserted, overrides PLL powerdown from state machine.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_pll_div5_byp  : 1;  /**< [  3:  3](R/W) Not used.
                                                                 For diagnostic use only. */
        uint64_t pll_band_sel_ovrrd_val : 1; /**< [  2:  2](R/W) State machine override value for VCO band select.
                                                                 0 = Low band VCO0 (RO-VCO).
                                                                 1 = High band VCO1 (LC-VCO).

                                                                 For diagnostic use only. */
        uint64_t pll_band_sel_ovrrd_en : 1;  /**< [  1:  1](R/W) PLL band select override enable.
                                                                 For diagnostic use only. */
        uint64_t pll_pcs_div_ovrrd_en  : 1;  /**< [  0:  0](R/W) Override global power state machine and mac_pcs_pll_div control signal.
                                                                 When asserted, pcs_sds_pll_div is specified from
                                                                 GSER()_LANE()_PCS_PLL_CTLIFC_1[PLL_PCS_DIV_OVRRD_VAL],
                                                                 global power state machine and mac_pcs_pll_div control signals are ignored.
                                                                 For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t pll_pcs_div_ovrrd_en  : 1;  /**< [  0:  0](R/W) Override global power state machine and mac_pcs_pll_div control signal.
                                                                 When asserted, pcs_sds_pll_div is specified from
                                                                 GSER()_LANE()_PCS_PLL_CTLIFC_1[PLL_PCS_DIV_OVRRD_VAL],
                                                                 global power state machine and mac_pcs_pll_div control signals are ignored.
                                                                 For diagnostic use only. */
        uint64_t pll_band_sel_ovrrd_en : 1;  /**< [  1:  1](R/W) PLL band select override enable.
                                                                 For diagnostic use only. */
        uint64_t pll_band_sel_ovrrd_val : 1; /**< [  2:  2](R/W) State machine override value for VCO band select.
                                                                 0 = Low band VCO0 (RO-VCO).
                                                                 1 = High band VCO1 (LC-VCO).

                                                                 For diagnostic use only. */
        uint64_t pcs_sds_pll_div5_byp  : 1;  /**< [  3:  3](R/W) Not used.
                                                                 For diagnostic use only. */
        uint64_t pll_pd_ovrrd_en       : 1;  /**< [  4:  4](R/W) When asserted, overrides PLL powerdown from state machine.
                                                                 For diagnostic use only. */
        uint64_t pll_pd_ovrrd_val      : 1;  /**< [  5:  5](R/W) PLL powerdown override value.
                                                                 For diagnostic use only. */
        uint64_t pll_sdsck_pd_ovrrd_en : 1;  /**< [  6:  6](R/W) Clock tree powerdown override enable.
                                                                 For diagnostic use only. */
        uint64_t pll_sdsck_pd_ovrrd_val : 1; /**< [  7:  7](R/W) Clock tree powerdown override value.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_pll_counter_resetn : 1;/**< [  8:  8](R/W) Not used.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_pll_lock_override : 1;/**< [  9:  9](R/W) Not used.
                                                                 For diagnostic use only. */
        uint64_t reserved_10_13        : 4;
        uint64_t pll_div_ovrrd_en      : 1;  /**< [ 14: 14](R/W) Override global power state machine and mac_pcs_pll_div control signal.
                                                                 When asserted, pcs_sds_pll_div is specified from
                                                                 GSER()_LANE()_PCS_PLL_CTLIFC_0[PLL_DIV_OVRRD_VAL],
                                                                 global power state machine and mac_pcs_pll_div control signals are ignored.
                                                                 For diagnostic use only. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_glbl_pll_cfg_2_s cn; */
};
typedef union bdk_gserx_glbl_pll_cfg_2 bdk_gserx_glbl_pll_cfg_2_t;

static inline uint64_t BDK_GSERX_GLBL_PLL_CFG_2(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_GLBL_PLL_CFG_2(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090460010ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090460010ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090460010ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_GLBL_PLL_CFG_2", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_GLBL_PLL_CFG_2(a) bdk_gserx_glbl_pll_cfg_2_t
#define bustype_BDK_GSERX_GLBL_PLL_CFG_2(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_GLBL_PLL_CFG_2(a) "GSERX_GLBL_PLL_CFG_2"
#define device_bar_BDK_GSERX_GLBL_PLL_CFG_2(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_GLBL_PLL_CFG_2(a) (a)
#define arguments_BDK_GSERX_GLBL_PLL_CFG_2(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_glbl_pll_cfg_3
 *
 * GSER Global PLL Configuration 3 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_glbl_pll_cfg_3
{
    uint64_t u;
    struct bdk_gserx_glbl_pll_cfg_3_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t pcs_sds_pll_vco_amp   : 2;  /**< [  9:  8](R/W) Adjusts the VCO amplitude control current.
                                                                 For diagnostic use only.
                                                                 0x0 = Add 25 uA.
                                                                 0x1 = OFF (default).
                                                                 0x2 = Sink 25 uA.
                                                                 0x3 = Sink 50 uA. */
        uint64_t pll_bypass_uq         : 1;  /**< [  7:  7](R/W) PLL bypass enable. When asserted, multiplexes in the feedback divider clock.
                                                                 For diagnostic use only. */
        uint64_t pll_vctrl_sel_ovrrd_en : 1; /**< [  6:  6](R/W) Override enable for selecting current for Vctrl in open loop operation.
                                                                 For diagnostic use only. */
        uint64_t pll_vctrl_sel_ovrrd_val : 2;/**< [  5:  4](R/W) Override value for selecting current for Vctrl in open loop operation.
                                                                 For diagnostic use only. */
        uint64_t pll_vctrl_sel_lcvco_val : 2;/**< [  3:  2](R/W) Selects current for Vctrl in open loop operation for LC-tank VCO.
                                                                 For diagnostic use only. */
        uint64_t pll_vctrl_sel_rovco_val : 2;/**< [  1:  0](R/W) Selects current for Vctrl in open loop operation for ring oscillator VCO.
                                                                 For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t pll_vctrl_sel_rovco_val : 2;/**< [  1:  0](R/W) Selects current for Vctrl in open loop operation for ring oscillator VCO.
                                                                 For diagnostic use only. */
        uint64_t pll_vctrl_sel_lcvco_val : 2;/**< [  3:  2](R/W) Selects current for Vctrl in open loop operation for LC-tank VCO.
                                                                 For diagnostic use only. */
        uint64_t pll_vctrl_sel_ovrrd_val : 2;/**< [  5:  4](R/W) Override value for selecting current for Vctrl in open loop operation.
                                                                 For diagnostic use only. */
        uint64_t pll_vctrl_sel_ovrrd_en : 1; /**< [  6:  6](R/W) Override enable for selecting current for Vctrl in open loop operation.
                                                                 For diagnostic use only. */
        uint64_t pll_bypass_uq         : 1;  /**< [  7:  7](R/W) PLL bypass enable. When asserted, multiplexes in the feedback divider clock.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_pll_vco_amp   : 2;  /**< [  9:  8](R/W) Adjusts the VCO amplitude control current.
                                                                 For diagnostic use only.
                                                                 0x0 = Add 25 uA.
                                                                 0x1 = OFF (default).
                                                                 0x2 = Sink 25 uA.
                                                                 0x3 = Sink 50 uA. */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_glbl_pll_cfg_3_s cn; */
};
typedef union bdk_gserx_glbl_pll_cfg_3 bdk_gserx_glbl_pll_cfg_3_t;

static inline uint64_t BDK_GSERX_GLBL_PLL_CFG_3(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_GLBL_PLL_CFG_3(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090460018ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090460018ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090460018ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_GLBL_PLL_CFG_3", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_GLBL_PLL_CFG_3(a) bdk_gserx_glbl_pll_cfg_3_t
#define bustype_BDK_GSERX_GLBL_PLL_CFG_3(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_GLBL_PLL_CFG_3(a) "GSERX_GLBL_PLL_CFG_3"
#define device_bar_BDK_GSERX_GLBL_PLL_CFG_3(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_GLBL_PLL_CFG_3(a) (a)
#define arguments_BDK_GSERX_GLBL_PLL_CFG_3(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_glbl_pll_monitor
 *
 * GSER Monitor for SerDes Global to Raw PCS Global interface Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_glbl_pll_monitor
{
    uint64_t u;
    struct bdk_gserx_glbl_pll_monitor_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t sds_pcs_glbl_status   : 6;  /**< [ 13:  8](RO/H) Spare reserved for future use. Read data should be ignored. */
        uint64_t sds_pcs_pll_lock      : 1;  /**< [  7:  7](RO/H) Status signal from global indicates that PLL is locked. Not a true "lock" signal.
                                                                 Used to debug/test the PLL. */
        uint64_t sds_pcs_clock_ready   : 1;  /**< [  6:  6](RO/H) Clock status signal, can be overridden with (I_PLL_CTRL_EN == 1).
                                                                 0 = Clock not ready.
                                                                 1 = Clock ready. */
        uint64_t sds_pcs_pll_calstates : 5;  /**< [  5:  1](RO/H) PLL calibration code. */
        uint64_t sds_pcs_pll_caldone   : 1;  /**< [  0:  0](RO/H) PLL calibration done signal. */
#else /* Word 0 - Little Endian */
        uint64_t sds_pcs_pll_caldone   : 1;  /**< [  0:  0](RO/H) PLL calibration done signal. */
        uint64_t sds_pcs_pll_calstates : 5;  /**< [  5:  1](RO/H) PLL calibration code. */
        uint64_t sds_pcs_clock_ready   : 1;  /**< [  6:  6](RO/H) Clock status signal, can be overridden with (I_PLL_CTRL_EN == 1).
                                                                 0 = Clock not ready.
                                                                 1 = Clock ready. */
        uint64_t sds_pcs_pll_lock      : 1;  /**< [  7:  7](RO/H) Status signal from global indicates that PLL is locked. Not a true "lock" signal.
                                                                 Used to debug/test the PLL. */
        uint64_t sds_pcs_glbl_status   : 6;  /**< [ 13:  8](RO/H) Spare reserved for future use. Read data should be ignored. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    struct bdk_gserx_glbl_pll_monitor_cn
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t reserved_14_15        : 2;
        uint64_t sds_pcs_glbl_status   : 6;  /**< [ 13:  8](RO/H) Spare reserved for future use. Read data should be ignored. */
        uint64_t sds_pcs_pll_lock      : 1;  /**< [  7:  7](RO/H) Status signal from global indicates that PLL is locked. Not a true "lock" signal.
                                                                 Used to debug/test the PLL. */
        uint64_t sds_pcs_clock_ready   : 1;  /**< [  6:  6](RO/H) Clock status signal, can be overridden with (I_PLL_CTRL_EN == 1).
                                                                 0 = Clock not ready.
                                                                 1 = Clock ready. */
        uint64_t sds_pcs_pll_calstates : 5;  /**< [  5:  1](RO/H) PLL calibration code. */
        uint64_t sds_pcs_pll_caldone   : 1;  /**< [  0:  0](RO/H) PLL calibration done signal. */
#else /* Word 0 - Little Endian */
        uint64_t sds_pcs_pll_caldone   : 1;  /**< [  0:  0](RO/H) PLL calibration done signal. */
        uint64_t sds_pcs_pll_calstates : 5;  /**< [  5:  1](RO/H) PLL calibration code. */
        uint64_t sds_pcs_clock_ready   : 1;  /**< [  6:  6](RO/H) Clock status signal, can be overridden with (I_PLL_CTRL_EN == 1).
                                                                 0 = Clock not ready.
                                                                 1 = Clock ready. */
        uint64_t sds_pcs_pll_lock      : 1;  /**< [  7:  7](RO/H) Status signal from global indicates that PLL is locked. Not a true "lock" signal.
                                                                 Used to debug/test the PLL. */
        uint64_t sds_pcs_glbl_status   : 6;  /**< [ 13:  8](RO/H) Spare reserved for future use. Read data should be ignored. */
        uint64_t reserved_14_15        : 2;
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } cn;
};
typedef union bdk_gserx_glbl_pll_monitor bdk_gserx_glbl_pll_monitor_t;

static inline uint64_t BDK_GSERX_GLBL_PLL_MONITOR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_GLBL_PLL_MONITOR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090460100ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090460100ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090460100ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_GLBL_PLL_MONITOR", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_GLBL_PLL_MONITOR(a) bdk_gserx_glbl_pll_monitor_t
#define bustype_BDK_GSERX_GLBL_PLL_MONITOR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_GLBL_PLL_MONITOR(a) "GSERX_GLBL_PLL_MONITOR"
#define device_bar_BDK_GSERX_GLBL_PLL_MONITOR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_GLBL_PLL_MONITOR(a) (a)
#define arguments_BDK_GSERX_GLBL_PLL_MONITOR(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_glbl_tad
 *
 * GSER Global Test Analog and Digital Monitor Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_glbl_tad
{
    uint64_t u;
    struct bdk_gserx_glbl_tad_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t pcs_sds_tad_8_5       : 4;  /**< [  8:  5](R/W) AMON specific mode selection.
                                                                 Set GSER()_GLBL_TM_ADMON[AMON_ON].
                                                                 Decodes 0x0 - 0x4 require GSER()_GLBL_TM_ADMON[LSEL] set.
                                                                 Decodes 0x5 - 0x5 do not require GSER()_GLBL_TM_ADMON[LSEL] set.
                                                                 In both cases, the resulting signals can be observed on the AMON pin.

                                                                 0x0 = TX txdrv DAC 100ua sink current monitor.
                                                                 0x1 = TX vcnt precision dcc.
                                                                 0x2 = RX sdll topregout.
                                                                 0x3 = RX ldll vctrl_i.
                                                                 0x4 = RX RX term VCM voltage.
                                                                 0x5 = Global bandgap voltage.
                                                                 0x6 = Global CTAT voltage.
                                                                 0x7 = Global internal 100ua reference current.
                                                                 0x8 = Global external 100ua reference current.
                                                                 0x9 = Global Rterm calibration reference voltage.
                                                                 0xA = Global Rterm calibration comparator voltage.
                                                                 0xB = Global force VCNT through DAC.
                                                                 0xC = Global VDD voltage.
                                                                 0xD = Global VDDCLK voltage.
                                                                 0xE = Global PLL regulate VCO supply.
                                                                 0xF = Global VCTRL for VCO varactor control. */
        uint64_t pcs_sds_tad_4_0       : 5;  /**< [  4:  0](R/W) DMON specific mode selection.
                                                                 Set GSER()_GLBL_TM_ADMON[DMON_ON].
                                                                 Decodes 0x0 - 0xe require GSER()_GLBL_TM_ADMON[LSEL] set.
                                                                 Decodes 0xf - 0x1f do not require GSER()_GLBL_TM_ADMON[LSEL] set.
                                                                 In both cases, the resulting signals can be observed on the DMON pin.

                                                                 0x00 = DFE Data Q.
                                                                 0x01 = DFE Edge I.
                                                                 0x02 = DFE CK Q.
                                                                 0x03 = DFE CK I.
                                                                 0x04 = DLL use GSER()_SLICE()_RX_SDLL_CTRL.PCS_SDS_RX_SDLL_SWSEL to select signal
                                                                 in the slice DLL.
                                                                 0x05-0x7 = Reserved.
                                                                 0x08 = RX ld_rx[0].
                                                                 0x09 = RX rx_clk.
                                                                 0x0A = RX q_error_stg.
                                                                 0x0B = RX q_data_stg.
                                                                 0x0C-0x0E = Reserved.
                                                                 0x0F = Special case to observe supply in global. Sds_vdda and a internal regulated supply
                                                                 can be observed on DMON and DMONB
                                                                 respectively.  sds_vss can be observed on AMON. GSER()_GLBL_TM_ADMON[AMON_ON]
                                                                 must not be set.
                                                                 0x10 = PLL_CLK 0 degree.
                                                                 0x11 = Sds_tst_fb_clk.
                                                                 0x12 = Buffered refclk.
                                                                 0x13 = Div 8 of core clock (core_clk_out).
                                                                 0x14-0x1F: Reserved. */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_tad_4_0       : 5;  /**< [  4:  0](R/W) DMON specific mode selection.
                                                                 Set GSER()_GLBL_TM_ADMON[DMON_ON].
                                                                 Decodes 0x0 - 0xe require GSER()_GLBL_TM_ADMON[LSEL] set.
                                                                 Decodes 0xf - 0x1f do not require GSER()_GLBL_TM_ADMON[LSEL] set.
                                                                 In both cases, the resulting signals can be observed on the DMON pin.

                                                                 0x00 = DFE Data Q.
                                                                 0x01 = DFE Edge I.
                                                                 0x02 = DFE CK Q.
                                                                 0x03 = DFE CK I.
                                                                 0x04 = DLL use GSER()_SLICE()_RX_SDLL_CTRL.PCS_SDS_RX_SDLL_SWSEL to select signal
                                                                 in the slice DLL.
                                                                 0x05-0x7 = Reserved.
                                                                 0x08 = RX ld_rx[0].
                                                                 0x09 = RX rx_clk.
                                                                 0x0A = RX q_error_stg.
                                                                 0x0B = RX q_data_stg.
                                                                 0x0C-0x0E = Reserved.
                                                                 0x0F = Special case to observe supply in global. Sds_vdda and a internal regulated supply
                                                                 can be observed on DMON and DMONB
                                                                 respectively.  sds_vss can be observed on AMON. GSER()_GLBL_TM_ADMON[AMON_ON]
                                                                 must not be set.
                                                                 0x10 = PLL_CLK 0 degree.
                                                                 0x11 = Sds_tst_fb_clk.
                                                                 0x12 = Buffered refclk.
                                                                 0x13 = Div 8 of core clock (core_clk_out).
                                                                 0x14-0x1F: Reserved. */
        uint64_t pcs_sds_tad_8_5       : 4;  /**< [  8:  5](R/W) AMON specific mode selection.
                                                                 Set GSER()_GLBL_TM_ADMON[AMON_ON].
                                                                 Decodes 0x0 - 0x4 require GSER()_GLBL_TM_ADMON[LSEL] set.
                                                                 Decodes 0x5 - 0x5 do not require GSER()_GLBL_TM_ADMON[LSEL] set.
                                                                 In both cases, the resulting signals can be observed on the AMON pin.

                                                                 0x0 = TX txdrv DAC 100ua sink current monitor.
                                                                 0x1 = TX vcnt precision dcc.
                                                                 0x2 = RX sdll topregout.
                                                                 0x3 = RX ldll vctrl_i.
                                                                 0x4 = RX RX term VCM voltage.
                                                                 0x5 = Global bandgap voltage.
                                                                 0x6 = Global CTAT voltage.
                                                                 0x7 = Global internal 100ua reference current.
                                                                 0x8 = Global external 100ua reference current.
                                                                 0x9 = Global Rterm calibration reference voltage.
                                                                 0xA = Global Rterm calibration comparator voltage.
                                                                 0xB = Global force VCNT through DAC.
                                                                 0xC = Global VDD voltage.
                                                                 0xD = Global VDDCLK voltage.
                                                                 0xE = Global PLL regulate VCO supply.
                                                                 0xF = Global VCTRL for VCO varactor control. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_glbl_tad_s cn; */
};
typedef union bdk_gserx_glbl_tad bdk_gserx_glbl_tad_t;

static inline uint64_t BDK_GSERX_GLBL_TAD(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_GLBL_TAD(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090460400ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090460400ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090460400ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_GLBL_TAD", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_GLBL_TAD(a) bdk_gserx_glbl_tad_t
#define bustype_BDK_GSERX_GLBL_TAD(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_GLBL_TAD(a) "GSERX_GLBL_TAD"
#define device_bar_BDK_GSERX_GLBL_TAD(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_GLBL_TAD(a) (a)
#define arguments_BDK_GSERX_GLBL_TAD(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_glbl_tm_admon
 *
 * GSER Global Test Mode Analog/Digital Monitor Enable Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_glbl_tm_admon
{
    uint64_t u;
    struct bdk_gserx_glbl_tm_admon_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t amon_on               : 1;  /**< [  7:  7](R/W) When set, AMON test mode is enabled; see GSER()_GLBL_TAD. */
        uint64_t dmon_on               : 1;  /**< [  6:  6](R/W) When set, DMON test mode is enabled; see GSER()_GLBL_TAD. */
        uint64_t reserved_3_5          : 3;
        uint64_t lsel                  : 3;  /**< [  2:  0](R/W) Three bits to select 1 out of 4 lanes for AMON/DMON test.
                                                                 0x0 = Selects lane 0.
                                                                 0x1 = Selects lane 1.
                                                                 0x2 = Selects lane 2.
                                                                 0x3 = Selects lane 3.
                                                                 0x4-0x7 = Reserved. */
#else /* Word 0 - Little Endian */
        uint64_t lsel                  : 3;  /**< [  2:  0](R/W) Three bits to select 1 out of 4 lanes for AMON/DMON test.
                                                                 0x0 = Selects lane 0.
                                                                 0x1 = Selects lane 1.
                                                                 0x2 = Selects lane 2.
                                                                 0x3 = Selects lane 3.
                                                                 0x4-0x7 = Reserved. */
        uint64_t reserved_3_5          : 3;
        uint64_t dmon_on               : 1;  /**< [  6:  6](R/W) When set, DMON test mode is enabled; see GSER()_GLBL_TAD. */
        uint64_t amon_on               : 1;  /**< [  7:  7](R/W) When set, AMON test mode is enabled; see GSER()_GLBL_TAD. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_glbl_tm_admon_s cn; */
};
typedef union bdk_gserx_glbl_tm_admon bdk_gserx_glbl_tm_admon_t;

static inline uint64_t BDK_GSERX_GLBL_TM_ADMON(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_GLBL_TM_ADMON(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090460408ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090460408ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090460408ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_GLBL_TM_ADMON", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_GLBL_TM_ADMON(a) bdk_gserx_glbl_tm_admon_t
#define bustype_BDK_GSERX_GLBL_TM_ADMON(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_GLBL_TM_ADMON(a) "GSERX_GLBL_TM_ADMON"
#define device_bar_BDK_GSERX_GLBL_TM_ADMON(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_GLBL_TM_ADMON(a) (a)
#define arguments_BDK_GSERX_GLBL_TM_ADMON(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_iddq_mode
 *
 * GSER IDDQ Mode Register
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_iddq_mode
{
    uint64_t u;
    struct bdk_gserx_iddq_mode_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t phy_iddq_mode         : 1;  /**< [  0:  0](R/W) When set, power downs all circuitry in PHY for IDDQ testing */
#else /* Word 0 - Little Endian */
        uint64_t phy_iddq_mode         : 1;  /**< [  0:  0](R/W) When set, power downs all circuitry in PHY for IDDQ testing */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_iddq_mode_s cn; */
};
typedef union bdk_gserx_iddq_mode bdk_gserx_iddq_mode_t;

static inline uint64_t BDK_GSERX_IDDQ_MODE(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_IDDQ_MODE(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000018ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000018ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000018ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_IDDQ_MODE", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_IDDQ_MODE(a) bdk_gserx_iddq_mode_t
#define bustype_BDK_GSERX_IDDQ_MODE(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_IDDQ_MODE(a) "GSERX_IDDQ_MODE"
#define device_bar_BDK_GSERX_IDDQ_MODE(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_IDDQ_MODE(a) (a)
#define arguments_BDK_GSERX_IDDQ_MODE(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_lane#_lbert_cfg
 *
 * GSER Lane LBERT Configuration Registers
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_lbert_cfg
{
    uint64_t u;
    struct bdk_gserx_lanex_lbert_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t lbert_pg_err_insert   : 1;  /**< [ 15: 15](WO/H) Insert one bit error into the LSB of the LBERT generated
                                                                 stream.  A single write to this bit inserts a single bit
                                                                 error. */
        uint64_t lbert_pm_sync_start   : 1;  /**< [ 14: 14](WO/H) Synchronize the pattern matcher LFSR with the incoming
                                                                 data.  Writing this bit resets the error counter and
                                                                 starts a synchronization of the PM.  There is no need
                                                                 to write this bit back to a zero to run normally. */
        uint64_t lbert_pg_en           : 1;  /**< [ 13: 13](R/W) Enable the LBERT pattern generator. */
        uint64_t lbert_pg_width        : 2;  /**< [ 12: 11](R/W) LBERT pattern generator data width:
                                                                 0x0 = 8-bit data.
                                                                 0x1 = 10-bit data.
                                                                 0x2 = 16-bit data.
                                                                 0x3 = 20-bit data. */
        uint64_t lbert_pg_mode         : 4;  /**< [ 10:  7](R/W) LBERT pattern generator mode; when changing modes,
                                                                 must be disabled first:
                                                                 0x0 = Disabled.
                                                                 0x1 = lfsr31 = X^31 + X^28 + 1.
                                                                 0x2 = lfsr23 = X^23 + X^18 + 1.
                                                                 0x3 = lfsr23 = X^23 + X^21 + X^16 + X^8 + X^5 + X^2 + 1.
                                                                 0x4 = lfsr16 = X^16 + X^5 + X^4 + X^3 + 1.
                                                                 0x5 = lfsr15 = X^15 + X^14 + 1.
                                                                 0x6 = lfsr11 = X^11 + X^9 + 1.
                                                                 0x7 = lfsr7  = X^7 + X^6 + 1.
                                                                 0x8 = Fixed word (PAT0).
                                                                 0x9 = DC-balanced word (PAT0, ~PAT0).
                                                                 0xA = Fixed Pattern (000, PAT0, 3ff, ~PAT0).
                                                                 0xB-F = Reserved. */
        uint64_t lbert_pm_en           : 1;  /**< [  6:  6](R/W) Enable LBERT pattern matcher. */
        uint64_t lbert_pm_width        : 2;  /**< [  5:  4](R/W) LBERT pattern matcher data width.
                                                                 0x0 = 8-bit data.
                                                                 0x1 = 10-bit data.
                                                                 0x2 = 16-bit data.
                                                                 0x3 = 20-bit data. */
        uint64_t lbert_pm_mode         : 4;  /**< [  3:  0](R/W) LBERT pattern matcher mode; when changing modes,
                                                                 must be disabled first:
                                                                 0x0 = Disabled.
                                                                 0x1 = lfsr31 = X^31 + X^28 + 1.
                                                                 0x2 = lfsr23 = X^23 + X^18 + 1.
                                                                 0x3 = lfsr23 = X^23 + X^21 + X^16 + X^8 + X^5 + X^2 + 1.
                                                                 0x4 = lfsr16 = X^16 + X^5 + X^4 + X^3 + 1.
                                                                 0x5 = lfsr15 = X^15 + X^14 + 1.
                                                                 0x6 = lfsr11 = X^11 + X^9 + 1.
                                                                 0x7 = lfsr7  = X^7 + X^6 + 1.
                                                                 0x8 = Fixed word (PAT0).
                                                                 0x9 = DC-balanced word (PAT0, ~PAT0).
                                                                 0xA = Fixed Pattern: (000, PAT0, 3ff, ~PAT0).
                                                                 0xB-F = Reserved. */
#else /* Word 0 - Little Endian */
        uint64_t lbert_pm_mode         : 4;  /**< [  3:  0](R/W) LBERT pattern matcher mode; when changing modes,
                                                                 must be disabled first:
                                                                 0x0 = Disabled.
                                                                 0x1 = lfsr31 = X^31 + X^28 + 1.
                                                                 0x2 = lfsr23 = X^23 + X^18 + 1.
                                                                 0x3 = lfsr23 = X^23 + X^21 + X^16 + X^8 + X^5 + X^2 + 1.
                                                                 0x4 = lfsr16 = X^16 + X^5 + X^4 + X^3 + 1.
                                                                 0x5 = lfsr15 = X^15 + X^14 + 1.
                                                                 0x6 = lfsr11 = X^11 + X^9 + 1.
                                                                 0x7 = lfsr7  = X^7 + X^6 + 1.
                                                                 0x8 = Fixed word (PAT0).
                                                                 0x9 = DC-balanced word (PAT0, ~PAT0).
                                                                 0xA = Fixed Pattern: (000, PAT0, 3ff, ~PAT0).
                                                                 0xB-F = Reserved. */
        uint64_t lbert_pm_width        : 2;  /**< [  5:  4](R/W) LBERT pattern matcher data width.
                                                                 0x0 = 8-bit data.
                                                                 0x1 = 10-bit data.
                                                                 0x2 = 16-bit data.
                                                                 0x3 = 20-bit data. */
        uint64_t lbert_pm_en           : 1;  /**< [  6:  6](R/W) Enable LBERT pattern matcher. */
        uint64_t lbert_pg_mode         : 4;  /**< [ 10:  7](R/W) LBERT pattern generator mode; when changing modes,
                                                                 must be disabled first:
                                                                 0x0 = Disabled.
                                                                 0x1 = lfsr31 = X^31 + X^28 + 1.
                                                                 0x2 = lfsr23 = X^23 + X^18 + 1.
                                                                 0x3 = lfsr23 = X^23 + X^21 + X^16 + X^8 + X^5 + X^2 + 1.
                                                                 0x4 = lfsr16 = X^16 + X^5 + X^4 + X^3 + 1.
                                                                 0x5 = lfsr15 = X^15 + X^14 + 1.
                                                                 0x6 = lfsr11 = X^11 + X^9 + 1.
                                                                 0x7 = lfsr7  = X^7 + X^6 + 1.
                                                                 0x8 = Fixed word (PAT0).
                                                                 0x9 = DC-balanced word (PAT0, ~PAT0).
                                                                 0xA = Fixed Pattern (000, PAT0, 3ff, ~PAT0).
                                                                 0xB-F = Reserved. */
        uint64_t lbert_pg_width        : 2;  /**< [ 12: 11](R/W) LBERT pattern generator data width:
                                                                 0x0 = 8-bit data.
                                                                 0x1 = 10-bit data.
                                                                 0x2 = 16-bit data.
                                                                 0x3 = 20-bit data. */
        uint64_t lbert_pg_en           : 1;  /**< [ 13: 13](R/W) Enable the LBERT pattern generator. */
        uint64_t lbert_pm_sync_start   : 1;  /**< [ 14: 14](WO/H) Synchronize the pattern matcher LFSR with the incoming
                                                                 data.  Writing this bit resets the error counter and
                                                                 starts a synchronization of the PM.  There is no need
                                                                 to write this bit back to a zero to run normally. */
        uint64_t lbert_pg_err_insert   : 1;  /**< [ 15: 15](WO/H) Insert one bit error into the LSB of the LBERT generated
                                                                 stream.  A single write to this bit inserts a single bit
                                                                 error. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_lbert_cfg_s cn; */
};
typedef union bdk_gserx_lanex_lbert_cfg bdk_gserx_lanex_lbert_cfg_t;

static inline uint64_t BDK_GSERX_LANEX_LBERT_CFG(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_LBERT_CFG(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904c0020ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904c0020ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904c0020ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_LBERT_CFG", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_LBERT_CFG(a,b) bdk_gserx_lanex_lbert_cfg_t
#define bustype_BDK_GSERX_LANEX_LBERT_CFG(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_LBERT_CFG(a,b) "GSERX_LANEX_LBERT_CFG"
#define device_bar_BDK_GSERX_LANEX_LBERT_CFG(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_LBERT_CFG(a,b) (a)
#define arguments_BDK_GSERX_LANEX_LBERT_CFG(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_lbert_ecnt
 *
 * GSER Lane LBERT Error Counter Registers
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 * The error registers are reset on a read-only when the pattern matcher is enabled.
 * If the pattern matcher is disabled, the registers return the error count that was
 * indicated when the pattern matcher was disabled and never reset.
 */
union bdk_gserx_lanex_lbert_ecnt
{
    uint64_t u;
    struct bdk_gserx_lanex_lbert_ecnt_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t lbert_err_ovbit14     : 1;  /**< [ 15: 15](RO/H) If this bit is set, multiply [LBERT_ERR_CNT] by 128.
                                                                 If this bit is set and [LBERT_ERR_CNT] = 2^15-1, signals
                                                                 overflow of the counter. */
        uint64_t lbert_err_cnt         : 15; /**< [ 14:  0](RO/H) Current bit error count.
                                                                 If [LBERT_ERR_OVBIT14] is active, then multiply
                                                                 count by 128. */
#else /* Word 0 - Little Endian */
        uint64_t lbert_err_cnt         : 15; /**< [ 14:  0](RO/H) Current bit error count.
                                                                 If [LBERT_ERR_OVBIT14] is active, then multiply
                                                                 count by 128. */
        uint64_t lbert_err_ovbit14     : 1;  /**< [ 15: 15](RO/H) If this bit is set, multiply [LBERT_ERR_CNT] by 128.
                                                                 If this bit is set and [LBERT_ERR_CNT] = 2^15-1, signals
                                                                 overflow of the counter. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_lbert_ecnt_s cn; */
};
typedef union bdk_gserx_lanex_lbert_ecnt bdk_gserx_lanex_lbert_ecnt_t;

static inline uint64_t BDK_GSERX_LANEX_LBERT_ECNT(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_LBERT_ECNT(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904c0028ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904c0028ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904c0028ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_LBERT_ECNT", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_LBERT_ECNT(a,b) bdk_gserx_lanex_lbert_ecnt_t
#define bustype_BDK_GSERX_LANEX_LBERT_ECNT(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_LBERT_ECNT(a,b) "GSERX_LANEX_LBERT_ECNT"
#define device_bar_BDK_GSERX_LANEX_LBERT_ECNT(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_LBERT_ECNT(a,b) (a)
#define arguments_BDK_GSERX_LANEX_LBERT_ECNT(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_lbert_pat_cfg
 *
 * GSER Lane LBERT Pattern Configuration Registers
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_lbert_pat_cfg
{
    uint64_t u;
    struct bdk_gserx_lanex_lbert_pat_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t lbert_pg_pat          : 10; /**< [  9:  0](R/W) Programmable 10-bit pattern to be used in the LBERT pattern mode;
                                                                 applies when GSER()_LANE()_LBERT_CFG[LBERT_PG_MODE]
                                                                 is equal to 8, 9, or 10. */
#else /* Word 0 - Little Endian */
        uint64_t lbert_pg_pat          : 10; /**< [  9:  0](R/W) Programmable 10-bit pattern to be used in the LBERT pattern mode;
                                                                 applies when GSER()_LANE()_LBERT_CFG[LBERT_PG_MODE]
                                                                 is equal to 8, 9, or 10. */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_lbert_pat_cfg_s cn; */
};
typedef union bdk_gserx_lanex_lbert_pat_cfg bdk_gserx_lanex_lbert_pat_cfg_t;

static inline uint64_t BDK_GSERX_LANEX_LBERT_PAT_CFG(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_LBERT_PAT_CFG(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904c0018ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904c0018ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904c0018ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_LBERT_PAT_CFG", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_LBERT_PAT_CFG(a,b) bdk_gserx_lanex_lbert_pat_cfg_t
#define bustype_BDK_GSERX_LANEX_LBERT_PAT_CFG(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_LBERT_PAT_CFG(a,b) "GSERX_LANEX_LBERT_PAT_CFG"
#define device_bar_BDK_GSERX_LANEX_LBERT_PAT_CFG(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_LBERT_PAT_CFG(a,b) (a)
#define arguments_BDK_GSERX_LANEX_LBERT_PAT_CFG(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_misc_cfg_0
 *
 * GSER Lane Miscellaneous Configuration 0 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_misc_cfg_0
{
    uint64_t u;
    struct bdk_gserx_lanex_misc_cfg_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t use_pma_polarity      : 1;  /**< [ 15: 15](R/W) If set, the PMA control is used to define the polarity.
                                                                 If not set, GSER()_LANE()_RX_CFG_0[CFG_RX_POL_INVERT]
                                                                 is used. */
        uint64_t cfg_pcs_loopback      : 1;  /**< [ 14: 14](R/W) Assert for parallel loopback raw PCS TX to Raw PCS RX. */
        uint64_t pcs_tx_mode_ovrrd_en  : 1;  /**< [ 13: 13](R/W) Override enable for raw PCS TX data width. */
        uint64_t pcs_rx_mode_ovrrd_en  : 1;  /**< [ 12: 12](R/W) Override enable for raw PCS RX data width. */
        uint64_t cfg_eie_det_cnt       : 4;  /**< [ 11:  8](R/W) EIE detect state machine required number of consecutive
                                                                 PHY EIE status assertions to determine EIE and assert Raw
                                                                 PCS output pcs_mac_rx_eie_det_sts. */
        uint64_t eie_det_stl_on_time   : 3;  /**< [  7:  5](R/W) EIE detect state machine "on" delay prior to sampling
                                                                 PHY EIE status.  Software needs to set this field to 0x4 if
                                                                 in SATA mode (GSER()_CFG[SATA] is set). */
        uint64_t eie_det_stl_off_time  : 3;  /**< [  4:  2](R/W) EIE detect state machine "off" delay prior to sampling
                                                                 PHY EIE status. */
        uint64_t tx_bit_order          : 1;  /**< [  1:  1](R/W) Specify transmit bit order.
                                                                 0 = Maintain bit order of parallel data to SerDes TX.
                                                                 1 = Reverse bit order of parallel data to SerDes TX. */
        uint64_t rx_bit_order          : 1;  /**< [  0:  0](R/W) Specify receive bit order:
                                                                 0 = Maintain bit order of parallel data to SerDes RX.
                                                                 1 = Reverse bit order of parallel data to SerDes RX. */
#else /* Word 0 - Little Endian */
        uint64_t rx_bit_order          : 1;  /**< [  0:  0](R/W) Specify receive bit order:
                                                                 0 = Maintain bit order of parallel data to SerDes RX.
                                                                 1 = Reverse bit order of parallel data to SerDes RX. */
        uint64_t tx_bit_order          : 1;  /**< [  1:  1](R/W) Specify transmit bit order.
                                                                 0 = Maintain bit order of parallel data to SerDes TX.
                                                                 1 = Reverse bit order of parallel data to SerDes TX. */
        uint64_t eie_det_stl_off_time  : 3;  /**< [  4:  2](R/W) EIE detect state machine "off" delay prior to sampling
                                                                 PHY EIE status. */
        uint64_t eie_det_stl_on_time   : 3;  /**< [  7:  5](R/W) EIE detect state machine "on" delay prior to sampling
                                                                 PHY EIE status.  Software needs to set this field to 0x4 if
                                                                 in SATA mode (GSER()_CFG[SATA] is set). */
        uint64_t cfg_eie_det_cnt       : 4;  /**< [ 11:  8](R/W) EIE detect state machine required number of consecutive
                                                                 PHY EIE status assertions to determine EIE and assert Raw
                                                                 PCS output pcs_mac_rx_eie_det_sts. */
        uint64_t pcs_rx_mode_ovrrd_en  : 1;  /**< [ 12: 12](R/W) Override enable for raw PCS RX data width. */
        uint64_t pcs_tx_mode_ovrrd_en  : 1;  /**< [ 13: 13](R/W) Override enable for raw PCS TX data width. */
        uint64_t cfg_pcs_loopback      : 1;  /**< [ 14: 14](R/W) Assert for parallel loopback raw PCS TX to Raw PCS RX. */
        uint64_t use_pma_polarity      : 1;  /**< [ 15: 15](R/W) If set, the PMA control is used to define the polarity.
                                                                 If not set, GSER()_LANE()_RX_CFG_0[CFG_RX_POL_INVERT]
                                                                 is used. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_misc_cfg_0_s cn; */
};
typedef union bdk_gserx_lanex_misc_cfg_0 bdk_gserx_lanex_misc_cfg_0_t;

static inline uint64_t BDK_GSERX_LANEX_MISC_CFG_0(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_MISC_CFG_0(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904c0000ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904c0000ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904c0000ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_MISC_CFG_0", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_MISC_CFG_0(a,b) bdk_gserx_lanex_misc_cfg_0_t
#define bustype_BDK_GSERX_LANEX_MISC_CFG_0(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_MISC_CFG_0(a,b) "GSERX_LANEX_MISC_CFG_0"
#define device_bar_BDK_GSERX_LANEX_MISC_CFG_0(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_MISC_CFG_0(a,b) (a)
#define arguments_BDK_GSERX_LANEX_MISC_CFG_0(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_misc_cfg_1
 *
 * GSER Lane Miscellaneous Configuration 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_misc_cfg_1
{
    uint64_t u;
    struct bdk_gserx_lanex_misc_cfg_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_13_63        : 51;
        uint64_t par_tx_init           : 1;  /**< [ 12: 12](R/W) Performs parallel initialization of SerDes interface TX
                                                                 FIFO pointers. */
        uint64_t tx_polarity           : 1;  /**< [ 11: 11](R/W) Invert polarity of transmitted bit stream. Inversion is
                                                                 performed in the SerDes interface transmit datapath. */
        uint64_t rx_polarity_ovrrd_en  : 1;  /**< [ 10: 10](R/W) Override mac_pcs_rxX_polarity control pin values
                                                                 When set, RX polarity inversion is specified from
                                                                 RX_POLARITY_OVRRD_VAL, and mac_pcs_rxX_polarity is ignored. */
        uint64_t rx_polarity_ovrrd_val : 1;  /**< [  9:  9](R/W) Controls RX polarity inversion when RX_POLARITY_OVRRD_EN
                                                                 is set. Inversion is performed in the SerDes interface receive
                                                                 datapath. */
        uint64_t reserved_2_8          : 7;
        uint64_t mac_tx_fifo_rd_ptr_ival : 2;/**< [  1:  0](R/W/H) Initial value for MAC to PCS TX FIFO read pointer. */
#else /* Word 0 - Little Endian */
        uint64_t mac_tx_fifo_rd_ptr_ival : 2;/**< [  1:  0](R/W/H) Initial value for MAC to PCS TX FIFO read pointer. */
        uint64_t reserved_2_8          : 7;
        uint64_t rx_polarity_ovrrd_val : 1;  /**< [  9:  9](R/W) Controls RX polarity inversion when RX_POLARITY_OVRRD_EN
                                                                 is set. Inversion is performed in the SerDes interface receive
                                                                 datapath. */
        uint64_t rx_polarity_ovrrd_en  : 1;  /**< [ 10: 10](R/W) Override mac_pcs_rxX_polarity control pin values
                                                                 When set, RX polarity inversion is specified from
                                                                 RX_POLARITY_OVRRD_VAL, and mac_pcs_rxX_polarity is ignored. */
        uint64_t tx_polarity           : 1;  /**< [ 11: 11](R/W) Invert polarity of transmitted bit stream. Inversion is
                                                                 performed in the SerDes interface transmit datapath. */
        uint64_t par_tx_init           : 1;  /**< [ 12: 12](R/W) Performs parallel initialization of SerDes interface TX
                                                                 FIFO pointers. */
        uint64_t reserved_13_63        : 51;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_misc_cfg_1_s cn; */
};
typedef union bdk_gserx_lanex_misc_cfg_1 bdk_gserx_lanex_misc_cfg_1_t;

static inline uint64_t BDK_GSERX_LANEX_MISC_CFG_1(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_MISC_CFG_1(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904c0008ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904c0008ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904c0008ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_MISC_CFG_1", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_MISC_CFG_1(a,b) bdk_gserx_lanex_misc_cfg_1_t
#define bustype_BDK_GSERX_LANEX_MISC_CFG_1(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_MISC_CFG_1(a,b) "GSERX_LANEX_MISC_CFG_1"
#define device_bar_BDK_GSERX_LANEX_MISC_CFG_1(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_MISC_CFG_1(a,b) (a)
#define arguments_BDK_GSERX_LANEX_MISC_CFG_1(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_pcs_ctlifc_0
 *
 * GSER Lane Raw PCS Control Interface Configuration 0 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_pcs_ctlifc_0
{
    uint64_t u;
    struct bdk_gserx_lanex_pcs_ctlifc_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t cfg_tx_vboost_en_ovrrd_val : 1;/**< [ 13: 13](R/W) Specifies TX VBOOST enable request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_VBOOST_EN_OVRRD_EN]. */
        uint64_t cfg_tx_coeff_req_ovrrd_val : 1;/**< [ 12: 12](R/W) Specifies TX coefficient request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_COEFF_REQ_OVRRD_EN].
                                                                 See GSER()_LANE()_PCS_CTLIFC_2[CTLIFC_OVRRD_REQ]. */
        uint64_t cfg_rx_cdr_coast_req_ovrrd_val : 1;/**< [ 11: 11](R/W) Specifies RX CDR coast request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_RX_COAST_REQ_OVRRD_EN]. */
        uint64_t cfg_tx_detrx_en_req_ovrrd_val : 1;/**< [ 10: 10](R/W) Specifies TX detect RX request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_DETRX_EN_REQ_OVRRD_EN]. */
        uint64_t cfg_soft_reset_req_ovrrd_val : 1;/**< [  9:  9](R/W) Specifies Soft reset request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_SOFT_RESET_REQ_OVRRD_EN]. */
        uint64_t cfg_lane_pwr_off_ovrrd_val : 1;/**< [  8:  8](R/W) Specifies lane power off reset request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_LANE_PWR_OFF_OVRRD_EN]. */
        uint64_t cfg_tx_mode_ovrrd_val : 2;  /**< [  7:  6](R/W) Override PCS TX mode (data width) when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_MODE_OVRRD_EN].
                                                                 0x0 = 8-bit raw data (not supported).
                                                                 0x1 = 10-bit raw data (not supported).
                                                                 0x2 = 16-bit raw data (for PCIe Gen3 8Gb only).
                                                                 0x3 = 20-bit raw data. */
        uint64_t cfg_tx_pstate_req_ovrrd_val : 2;/**< [  5:  4](R/W) Override TX pstate request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_PSTATE_REQ_OVRRD_EN]. */
        uint64_t cfg_lane_mode_req_ovrrd_val : 4;/**< [  3:  0](R/W) Override lane mode request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_LANE_MODE_REQ_OVRRD_EN]. */
#else /* Word 0 - Little Endian */
        uint64_t cfg_lane_mode_req_ovrrd_val : 4;/**< [  3:  0](R/W) Override lane mode request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_LANE_MODE_REQ_OVRRD_EN]. */
        uint64_t cfg_tx_pstate_req_ovrrd_val : 2;/**< [  5:  4](R/W) Override TX pstate request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_PSTATE_REQ_OVRRD_EN]. */
        uint64_t cfg_tx_mode_ovrrd_val : 2;  /**< [  7:  6](R/W) Override PCS TX mode (data width) when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_MODE_OVRRD_EN].
                                                                 0x0 = 8-bit raw data (not supported).
                                                                 0x1 = 10-bit raw data (not supported).
                                                                 0x2 = 16-bit raw data (for PCIe Gen3 8Gb only).
                                                                 0x3 = 20-bit raw data. */
        uint64_t cfg_lane_pwr_off_ovrrd_val : 1;/**< [  8:  8](R/W) Specifies lane power off reset request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_LANE_PWR_OFF_OVRRD_EN]. */
        uint64_t cfg_soft_reset_req_ovrrd_val : 1;/**< [  9:  9](R/W) Specifies Soft reset request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_SOFT_RESET_REQ_OVRRD_EN]. */
        uint64_t cfg_tx_detrx_en_req_ovrrd_val : 1;/**< [ 10: 10](R/W) Specifies TX detect RX request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_DETRX_EN_REQ_OVRRD_EN]. */
        uint64_t cfg_rx_cdr_coast_req_ovrrd_val : 1;/**< [ 11: 11](R/W) Specifies RX CDR coast request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_RX_COAST_REQ_OVRRD_EN]. */
        uint64_t cfg_tx_coeff_req_ovrrd_val : 1;/**< [ 12: 12](R/W) Specifies TX coefficient request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_COEFF_REQ_OVRRD_EN].
                                                                 See GSER()_LANE()_PCS_CTLIFC_2[CTLIFC_OVRRD_REQ]. */
        uint64_t cfg_tx_vboost_en_ovrrd_val : 1;/**< [ 13: 13](R/W) Specifies TX VBOOST enable request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_VBOOST_EN_OVRRD_EN]. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_pcs_ctlifc_0_s cn; */
};
typedef union bdk_gserx_lanex_pcs_ctlifc_0 bdk_gserx_lanex_pcs_ctlifc_0_t;

static inline uint64_t BDK_GSERX_LANEX_PCS_CTLIFC_0(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_PCS_CTLIFC_0(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904c0060ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904c0060ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904c0060ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_PCS_CTLIFC_0", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_PCS_CTLIFC_0(a,b) bdk_gserx_lanex_pcs_ctlifc_0_t
#define bustype_BDK_GSERX_LANEX_PCS_CTLIFC_0(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_PCS_CTLIFC_0(a,b) "GSERX_LANEX_PCS_CTLIFC_0"
#define device_bar_BDK_GSERX_LANEX_PCS_CTLIFC_0(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_PCS_CTLIFC_0(a,b) (a)
#define arguments_BDK_GSERX_LANEX_PCS_CTLIFC_0(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_pcs_ctlifc_1
 *
 * GSER Lane Raw PCS Control Interface Configuration 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_pcs_ctlifc_1
{
    uint64_t u;
    struct bdk_gserx_lanex_pcs_ctlifc_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t cfg_rx_pstate_req_ovrrd_val : 2;/**< [  8:  7](R/W) Override RX pstate request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_PSTATE_REQ_OVRRD_EN]. */
        uint64_t reserved_2_6          : 5;
        uint64_t cfg_rx_mode_ovrrd_val : 2;  /**< [  1:  0](R/W) Override PCS RX mode (data width) when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_RX_MODE_OVRRD_EN].
                                                                 0x0 = 8-bit raw data (not supported).
                                                                 0x1 = 10-bit raw data (not supported).
                                                                 0x2 = 16-bit raw data (not supported).
                                                                 0x3 = 20-bit raw data. */
#else /* Word 0 - Little Endian */
        uint64_t cfg_rx_mode_ovrrd_val : 2;  /**< [  1:  0](R/W) Override PCS RX mode (data width) when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_RX_MODE_OVRRD_EN].
                                                                 0x0 = 8-bit raw data (not supported).
                                                                 0x1 = 10-bit raw data (not supported).
                                                                 0x2 = 16-bit raw data (not supported).
                                                                 0x3 = 20-bit raw data. */
        uint64_t reserved_2_6          : 5;
        uint64_t cfg_rx_pstate_req_ovrrd_val : 2;/**< [  8:  7](R/W) Override RX pstate request when its override bit
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_PSTATE_REQ_OVRRD_EN]. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_pcs_ctlifc_1_s cn; */
};
typedef union bdk_gserx_lanex_pcs_ctlifc_1 bdk_gserx_lanex_pcs_ctlifc_1_t;

static inline uint64_t BDK_GSERX_LANEX_PCS_CTLIFC_1(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_PCS_CTLIFC_1(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904c0068ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904c0068ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904c0068ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_PCS_CTLIFC_1", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_PCS_CTLIFC_1(a,b) bdk_gserx_lanex_pcs_ctlifc_1_t
#define bustype_BDK_GSERX_LANEX_PCS_CTLIFC_1(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_PCS_CTLIFC_1(a,b) "GSERX_LANEX_PCS_CTLIFC_1"
#define device_bar_BDK_GSERX_LANEX_PCS_CTLIFC_1(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_PCS_CTLIFC_1(a,b) (a)
#define arguments_BDK_GSERX_LANEX_PCS_CTLIFC_1(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_pcs_ctlifc_2
 *
 * GSER Lane Raw PCS Control Interface Configuration 2 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_pcs_ctlifc_2
{
    uint64_t u;
    struct bdk_gserx_lanex_pcs_ctlifc_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t ctlifc_ovrrd_req      : 1;  /**< [ 15: 15](WO) Writing to set this bit initiates a state machine interface request
                                                                 for GSER()_LANE()_PCS_CTLIFC_0 and GSER()_LANE()_PCS_CTLIFC_1
                                                                 override values.

                                                                 [CTLIFC_OVRRD_REQ] should be written with a one (with
                                                                 [CFG_TX_COEFF_REQ_OVRRD_EN]=1 and
                                                                 GSER()_LANE()_PCS_CTLIFC_0[CFG_TX_COEFF_REQ_OVRRD_VAL]=1) to initiate
                                                                 a control interface configuration over-ride after manually programming
                                                                 transmitter settings. See GSER()_LANE()_TX_PRE_EMPHASIS[CFG_TX_PREMPTAP]
                                                                 and GSER()_LANE()_TX_CFG_0[CFG_TX_SWING]. */
        uint64_t reserved_9_14         : 6;
        uint64_t cfg_tx_vboost_en_ovrrd_en : 1;/**< [  8:  8](R/W) Override mac_pcs_txX vboost_en signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_VBOOST_EN_OVRRD_VAL]. */
        uint64_t cfg_tx_coeff_req_ovrrd_en : 1;/**< [  7:  7](R/W) Override mac_pcs_txX_coeff_req signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_0[CFG_TX_COEFF_REQ_OVRRD_VAL]. See
                                                                 [CTLIFC_OVRRD_REQ]. */
        uint64_t cfg_rx_cdr_coast_req_ovrrd_en : 1;/**< [  6:  6](R/W) Override mac_pcs_rxX_cdr_coast signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CFG_RX_COAST_REQ_OVRRD_VAL]. */
        uint64_t cfg_tx_detrx_en_req_ovrrd_en : 1;/**< [  5:  5](R/W) Override mac_pcs_txX_detrx_en signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_DETRX_EN_REQ_OVRRD_VAL]. */
        uint64_t cfg_soft_reset_req_ovrrd_en : 1;/**< [  4:  4](R/W) Override mac_pcs_laneX_soft_rst signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CFG_SOFT_RESET_REQ_OVRRD_VAL]. */
        uint64_t cfg_lane_pwr_off_ovrrd_en : 1;/**< [  3:  3](R/W) Override mac_pcs_laneX_pwr_off signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CFG_LANE_PWR_OFF_OVRRD_VAL]. */
        uint64_t cfg_tx_pstate_req_ovrrd_en : 1;/**< [  2:  2](R/W) Override mac_pcs_txX_pstate[1:0] signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_PSTATE_REQ_OVRRD_VAL].
                                                                 When using this field to change the TX power state, you must also set
                                                                 the override enable bits for the lane_mode, soft_reset and lane_pwr_off
                                                                 fields.  The corresponding orrd_val fields should be programmed so as
                                                                 not to cause undesired changes. */
        uint64_t cfg_rx_pstate_req_ovrrd_en : 1;/**< [  1:  1](R/W) Override mac_pcs_rxX_pstate[1:0] signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CFG_RX_PSTATE_REQ_OVRRD_VAL].
                                                                 When using this field to change the RX power state, you must also set
                                                                 the override enable bits for the lane_mode, soft_reset and lane_pwr_off
                                                                 fields.  The corresponding orrd_val fields should be programmed so as
                                                                 not to cause undesired changes. */
        uint64_t cfg_lane_mode_req_ovrrd_en : 1;/**< [  0:  0](R/W) Override mac_pcs_laneX_mode[3:0] signal with the value specified in
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_LANE_MODE_REQ_OVRRD_VAL]. */
#else /* Word 0 - Little Endian */
        uint64_t cfg_lane_mode_req_ovrrd_en : 1;/**< [  0:  0](R/W) Override mac_pcs_laneX_mode[3:0] signal with the value specified in
                                                                 is asserted GSER()_LANE()_PCS_CTLIFC_2[CFG_LANE_MODE_REQ_OVRRD_VAL]. */
        uint64_t cfg_rx_pstate_req_ovrrd_en : 1;/**< [  1:  1](R/W) Override mac_pcs_rxX_pstate[1:0] signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CFG_RX_PSTATE_REQ_OVRRD_VAL].
                                                                 When using this field to change the RX power state, you must also set
                                                                 the override enable bits for the lane_mode, soft_reset and lane_pwr_off
                                                                 fields.  The corresponding orrd_val fields should be programmed so as
                                                                 not to cause undesired changes. */
        uint64_t cfg_tx_pstate_req_ovrrd_en : 1;/**< [  2:  2](R/W) Override mac_pcs_txX_pstate[1:0] signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_PSTATE_REQ_OVRRD_VAL].
                                                                 When using this field to change the TX power state, you must also set
                                                                 the override enable bits for the lane_mode, soft_reset and lane_pwr_off
                                                                 fields.  The corresponding orrd_val fields should be programmed so as
                                                                 not to cause undesired changes. */
        uint64_t cfg_lane_pwr_off_ovrrd_en : 1;/**< [  3:  3](R/W) Override mac_pcs_laneX_pwr_off signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CFG_LANE_PWR_OFF_OVRRD_VAL]. */
        uint64_t cfg_soft_reset_req_ovrrd_en : 1;/**< [  4:  4](R/W) Override mac_pcs_laneX_soft_rst signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CFG_SOFT_RESET_REQ_OVRRD_VAL]. */
        uint64_t cfg_tx_detrx_en_req_ovrrd_en : 1;/**< [  5:  5](R/W) Override mac_pcs_txX_detrx_en signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_DETRX_EN_REQ_OVRRD_VAL]. */
        uint64_t cfg_rx_cdr_coast_req_ovrrd_en : 1;/**< [  6:  6](R/W) Override mac_pcs_rxX_cdr_coast signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CFG_RX_COAST_REQ_OVRRD_VAL]. */
        uint64_t cfg_tx_coeff_req_ovrrd_en : 1;/**< [  7:  7](R/W) Override mac_pcs_txX_coeff_req signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_0[CFG_TX_COEFF_REQ_OVRRD_VAL]. See
                                                                 [CTLIFC_OVRRD_REQ]. */
        uint64_t cfg_tx_vboost_en_ovrrd_en : 1;/**< [  8:  8](R/W) Override mac_pcs_txX vboost_en signal with the value specified in
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CFG_TX_VBOOST_EN_OVRRD_VAL]. */
        uint64_t reserved_9_14         : 6;
        uint64_t ctlifc_ovrrd_req      : 1;  /**< [ 15: 15](WO) Writing to set this bit initiates a state machine interface request
                                                                 for GSER()_LANE()_PCS_CTLIFC_0 and GSER()_LANE()_PCS_CTLIFC_1
                                                                 override values.

                                                                 [CTLIFC_OVRRD_REQ] should be written with a one (with
                                                                 [CFG_TX_COEFF_REQ_OVRRD_EN]=1 and
                                                                 GSER()_LANE()_PCS_CTLIFC_0[CFG_TX_COEFF_REQ_OVRRD_VAL]=1) to initiate
                                                                 a control interface configuration over-ride after manually programming
                                                                 transmitter settings. See GSER()_LANE()_TX_PRE_EMPHASIS[CFG_TX_PREMPTAP]
                                                                 and GSER()_LANE()_TX_CFG_0[CFG_TX_SWING]. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_pcs_ctlifc_2_s cn; */
};
typedef union bdk_gserx_lanex_pcs_ctlifc_2 bdk_gserx_lanex_pcs_ctlifc_2_t;

static inline uint64_t BDK_GSERX_LANEX_PCS_CTLIFC_2(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_PCS_CTLIFC_2(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904c0070ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904c0070ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904c0070ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_PCS_CTLIFC_2", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_PCS_CTLIFC_2(a,b) bdk_gserx_lanex_pcs_ctlifc_2_t
#define bustype_BDK_GSERX_LANEX_PCS_CTLIFC_2(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_PCS_CTLIFC_2(a,b) "GSERX_LANEX_PCS_CTLIFC_2"
#define device_bar_BDK_GSERX_LANEX_PCS_CTLIFC_2(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_PCS_CTLIFC_2(a,b) (a)
#define arguments_BDK_GSERX_LANEX_PCS_CTLIFC_2(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_pcs_macifc_mon_0
 *
 * GSER Lane MAC to Raw PCS Interface Monitor 0 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_pcs_macifc_mon_0
{
    uint64_t u;
    struct bdk_gserx_lanex_pcs_macifc_mon_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t mac_pcs_tx_pstate     : 2;  /**< [ 15: 14](RO/H) Current state of the MAC to PCS TX power state\<2:0\> input.

                                                                 Internal:
                                                                 mac_pcs_txX_pstate[2:0]. */
        uint64_t mac_pcs_rx_pstate     : 2;  /**< [ 13: 12](RO/H) Current state of the MAC to PCS RX power state\<2:0\> input.

                                                                 Internal:
                                                                 mac_pcs_rxX_pstate[2:0]. */
        uint64_t mac_pcs_lane_pwr_off  : 1;  /**< [ 11: 11](RO/H) Current state of the MAC to PCS lane power off input.
                                                                 Internal:
                                                                 mac_pcs_laneX_pwr_off. */
        uint64_t reserved_10           : 1;
        uint64_t mac_pcs_lane_soft_reset : 1;/**< [  9:  9](RO/H) Current state of the MAC to PCS soft reset input.
                                                                 Internal:
                                                                 mac_pcs_laneX_soft_reset. */
        uint64_t mac_pcs_lane_loopbk_en : 1; /**< [  8:  8](RO/H) Current state of the MAC to PCS lane loopback enable input.
                                                                 Internal:
                                                                 mac_pcs_laneX_loopbk_en. */
        uint64_t mac_pcs_rx_eie_det_en : 1;  /**< [  7:  7](RO/H) Current state of the MAC to PCS receiver electrical idle exit
                                                                 detect enable input.

                                                                 Internal:
                                                                 mac_pcs_rxX_eie_det_en. */
        uint64_t mac_pcs_rx_cdr_coast  : 1;  /**< [  6:  6](RO/H) Current state of the MAC to PCS lane receiver CDR coast input.
                                                                 Internal:
                                                                 mac_pcs_rxX_cdr_coast. */
        uint64_t mac_pcs_tx_detrx_en   : 1;  /**< [  5:  5](RO/H) Current state of the MAC to PCS transmitter receiver detect
                                                                 enable input.

                                                                 Internal:
                                                                 mac_pcs_txX_detrx_en. */
        uint64_t mac_pcs_rx_eq_eval    : 1;  /**< [  4:  4](RO/H) Current state of the MAC to PCS receiver equalizer evaluation
                                                                 request input.

                                                                 Internal:
                                                                 mac_pcs_rxX_eq_eval. */
        uint64_t mac_pcs_lane_mode     : 4;  /**< [  3:  0](RO/H) Current state of the MAC to PCS lane mode input.
                                                                 Internal:
                                                                 mac_pcs_laneX_mode[3:0]. */
#else /* Word 0 - Little Endian */
        uint64_t mac_pcs_lane_mode     : 4;  /**< [  3:  0](RO/H) Current state of the MAC to PCS lane mode input.
                                                                 Internal:
                                                                 mac_pcs_laneX_mode[3:0]. */
        uint64_t mac_pcs_rx_eq_eval    : 1;  /**< [  4:  4](RO/H) Current state of the MAC to PCS receiver equalizer evaluation
                                                                 request input.

                                                                 Internal:
                                                                 mac_pcs_rxX_eq_eval. */
        uint64_t mac_pcs_tx_detrx_en   : 1;  /**< [  5:  5](RO/H) Current state of the MAC to PCS transmitter receiver detect
                                                                 enable input.

                                                                 Internal:
                                                                 mac_pcs_txX_detrx_en. */
        uint64_t mac_pcs_rx_cdr_coast  : 1;  /**< [  6:  6](RO/H) Current state of the MAC to PCS lane receiver CDR coast input.
                                                                 Internal:
                                                                 mac_pcs_rxX_cdr_coast. */
        uint64_t mac_pcs_rx_eie_det_en : 1;  /**< [  7:  7](RO/H) Current state of the MAC to PCS receiver electrical idle exit
                                                                 detect enable input.

                                                                 Internal:
                                                                 mac_pcs_rxX_eie_det_en. */
        uint64_t mac_pcs_lane_loopbk_en : 1; /**< [  8:  8](RO/H) Current state of the MAC to PCS lane loopback enable input.
                                                                 Internal:
                                                                 mac_pcs_laneX_loopbk_en. */
        uint64_t mac_pcs_lane_soft_reset : 1;/**< [  9:  9](RO/H) Current state of the MAC to PCS soft reset input.
                                                                 Internal:
                                                                 mac_pcs_laneX_soft_reset. */
        uint64_t reserved_10           : 1;
        uint64_t mac_pcs_lane_pwr_off  : 1;  /**< [ 11: 11](RO/H) Current state of the MAC to PCS lane power off input.
                                                                 Internal:
                                                                 mac_pcs_laneX_pwr_off. */
        uint64_t mac_pcs_rx_pstate     : 2;  /**< [ 13: 12](RO/H) Current state of the MAC to PCS RX power state\<2:0\> input.

                                                                 Internal:
                                                                 mac_pcs_rxX_pstate[2:0]. */
        uint64_t mac_pcs_tx_pstate     : 2;  /**< [ 15: 14](RO/H) Current state of the MAC to PCS TX power state\<2:0\> input.

                                                                 Internal:
                                                                 mac_pcs_txX_pstate[2:0]. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_pcs_macifc_mon_0_s cn; */
};
typedef union bdk_gserx_lanex_pcs_macifc_mon_0 bdk_gserx_lanex_pcs_macifc_mon_0_t;

static inline uint64_t BDK_GSERX_LANEX_PCS_MACIFC_MON_0(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_PCS_MACIFC_MON_0(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904c0108ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904c0108ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904c0108ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_PCS_MACIFC_MON_0", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_PCS_MACIFC_MON_0(a,b) bdk_gserx_lanex_pcs_macifc_mon_0_t
#define bustype_BDK_GSERX_LANEX_PCS_MACIFC_MON_0(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_PCS_MACIFC_MON_0(a,b) "GSERX_LANEX_PCS_MACIFC_MON_0"
#define device_bar_BDK_GSERX_LANEX_PCS_MACIFC_MON_0(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_PCS_MACIFC_MON_0(a,b) (a)
#define arguments_BDK_GSERX_LANEX_PCS_MACIFC_MON_0(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_pcs_macifc_mon_2
 *
 * GSER Lane MAC to Raw PCS Interface Monitor 2 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_pcs_macifc_mon_2
{
    uint64_t u;
    struct bdk_gserx_lanex_pcs_macifc_mon_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t tx_coeff_req          : 1;  /**< [ 15: 15](RO/H) Current state of the MAC to PCS TX coefficient request input.
                                                                 Internal:
                                                                 mac_pcs_txX_coeff_req. */
        uint64_t tx_vboost_en          : 1;  /**< [ 14: 14](RO/H) Current state of the MAC to PCS TX Vboost enable input.
                                                                 Internal:
                                                                 mac_pcs_txX_vboost_en. */
        uint64_t tx_swing              : 5;  /**< [ 13:  9](RO/H) Current state of the MAC to PCS TX equalizer swing\<4:0\> input.

                                                                 Internal:
                                                                 mac_pcs_txX_swing[4:0]. */
        uint64_t tx_pre                : 4;  /**< [  8:  5](RO/H) Current state of the MAC to PCS TX equalizer preemphasis\<3:0\> input.

                                                                 Internal:
                                                                 mac_pcs_txX_pre[3:0]. */
        uint64_t tx_post               : 5;  /**< [  4:  0](RO/H) Current state of the MAC to PCS TX equalizer postemphasis\<4:0\> input.

                                                                 Internal:
                                                                 mac_pcs_txX_post[4:0]. */
#else /* Word 0 - Little Endian */
        uint64_t tx_post               : 5;  /**< [  4:  0](RO/H) Current state of the MAC to PCS TX equalizer postemphasis\<4:0\> input.

                                                                 Internal:
                                                                 mac_pcs_txX_post[4:0]. */
        uint64_t tx_pre                : 4;  /**< [  8:  5](RO/H) Current state of the MAC to PCS TX equalizer preemphasis\<3:0\> input.

                                                                 Internal:
                                                                 mac_pcs_txX_pre[3:0]. */
        uint64_t tx_swing              : 5;  /**< [ 13:  9](RO/H) Current state of the MAC to PCS TX equalizer swing\<4:0\> input.

                                                                 Internal:
                                                                 mac_pcs_txX_swing[4:0]. */
        uint64_t tx_vboost_en          : 1;  /**< [ 14: 14](RO/H) Current state of the MAC to PCS TX Vboost enable input.
                                                                 Internal:
                                                                 mac_pcs_txX_vboost_en. */
        uint64_t tx_coeff_req          : 1;  /**< [ 15: 15](RO/H) Current state of the MAC to PCS TX coefficient request input.
                                                                 Internal:
                                                                 mac_pcs_txX_coeff_req. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_pcs_macifc_mon_2_s cn; */
};
typedef union bdk_gserx_lanex_pcs_macifc_mon_2 bdk_gserx_lanex_pcs_macifc_mon_2_t;

static inline uint64_t BDK_GSERX_LANEX_PCS_MACIFC_MON_2(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_PCS_MACIFC_MON_2(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904c0118ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904c0118ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904c0118ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_PCS_MACIFC_MON_2", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_PCS_MACIFC_MON_2(a,b) bdk_gserx_lanex_pcs_macifc_mon_2_t
#define bustype_BDK_GSERX_LANEX_PCS_MACIFC_MON_2(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_PCS_MACIFC_MON_2(a,b) "GSERX_LANEX_PCS_MACIFC_MON_2"
#define device_bar_BDK_GSERX_LANEX_PCS_MACIFC_MON_2(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_PCS_MACIFC_MON_2(a,b) (a)
#define arguments_BDK_GSERX_LANEX_PCS_MACIFC_MON_2(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_pma_loopback_ctrl
 *
 * GSER Lane PMA Loopback Control Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_pma_loopback_ctrl
{
    uint64_t u;
    struct bdk_gserx_lanex_pma_loopback_ctrl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t cfg_ln_lpbk_mode_ovrrd_en : 1;/**< [  1:  1](R/W) Enable override mac_pcs_loopbk_mode[3:0] with value of FG_LN_LPBK_MODE. */
        uint64_t cfg_ln_lpbk_mode      : 1;  /**< [  0:  0](R/W) Override value when CFG_LN_LPBK_MODE_OVRRD_EN is set. */
#else /* Word 0 - Little Endian */
        uint64_t cfg_ln_lpbk_mode      : 1;  /**< [  0:  0](R/W) Override value when CFG_LN_LPBK_MODE_OVRRD_EN is set. */
        uint64_t cfg_ln_lpbk_mode_ovrrd_en : 1;/**< [  1:  1](R/W) Enable override mac_pcs_loopbk_mode[3:0] with value of FG_LN_LPBK_MODE. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_pma_loopback_ctrl_s cn; */
};
typedef union bdk_gserx_lanex_pma_loopback_ctrl bdk_gserx_lanex_pma_loopback_ctrl_t;

static inline uint64_t BDK_GSERX_LANEX_PMA_LOOPBACK_CTRL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_PMA_LOOPBACK_CTRL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904400d0ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904400d0ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904400d0ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_PMA_LOOPBACK_CTRL", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_PMA_LOOPBACK_CTRL(a,b) bdk_gserx_lanex_pma_loopback_ctrl_t
#define bustype_BDK_GSERX_LANEX_PMA_LOOPBACK_CTRL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_PMA_LOOPBACK_CTRL(a,b) "GSERX_LANEX_PMA_LOOPBACK_CTRL"
#define device_bar_BDK_GSERX_LANEX_PMA_LOOPBACK_CTRL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_PMA_LOOPBACK_CTRL(a,b) (a)
#define arguments_BDK_GSERX_LANEX_PMA_LOOPBACK_CTRL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_pwr_ctrl
 *
 * GSER Lane Power Control Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_pwr_ctrl
{
    uint64_t u;
    struct bdk_gserx_lanex_pwr_ctrl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t tx_sds_fifo_reset_ovrrd_en : 1;/**< [ 14: 14](R/W) When asserted, TX_SDS_FIFO_RESET_OVRRD_VAL is used to specify the value of the reset
                                                                 signal for the TX FIFO supplying data to the SerDes p2s interface. */
        uint64_t tx_sds_fifo_reset_ovrrd_val : 1;/**< [ 13: 13](R/W) When asserted, TX_SDS_FIFO_RESET_OVRRD_EN is asserted, this field is
                                                                 used to specify the value of the reset
                                                                 signal for the TX FIFO supplying data to the SerDes p2s interface. */
        uint64_t tx_pcs_reset_ovrrd_val : 1; /**< [ 12: 12](R/W) When TX_PCS_RESET_OVRRD_EN is
                                                                 asserted, this field is used to specify the value of
                                                                 the reset signal for PCS TX logic. */
        uint64_t rx_pcs_reset_ovrrd_val : 1; /**< [ 11: 11](R/W) When RX_PCS_RESET_OVRRD_EN is
                                                                 asserted, this field is used to specify the value of
                                                                 the reset signal for PCS RX logic. */
        uint64_t reserved_9_10         : 2;
        uint64_t rx_resetn_ovrrd_en    : 1;  /**< [  8:  8](R/W) Override RX power state machine rx_resetn
                                                                 control signal.  When set, the rx_resetn control signal is taken
                                                                 from the GSER()_LANE()_RX_CFG_0[RX_RESETN_OVRRD_VAL]
                                                                 control bit. */
        uint64_t rx_resetn_ovrrd_val   : 1;  /**< [  7:  7](R/W) Override RX power state machine reset control
                                                                 signal. When set, reset control signals are specified in
                                                                 [RX_PCS_RESET_OVRRD_VAL]. */
        uint64_t rx_lctrl_ovrrd_en     : 1;  /**< [  6:  6](R/W) Override RX power state machine loop control
                                                                 signals.  When set, the loop control settings are
                                                                 specified in the GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL] field. */
        uint64_t rx_lctrl_ovrrd_val    : 1;  /**< [  5:  5](R/W) Override RX power state machine power down
                                                                 control signal. When set, the power down control signal is
                                                                 specified by GSER()_LANE()_RX_CFG_1[RX_CHPD_OVRRD_VAL]. */
        uint64_t tx_tristate_en_ovrrd_en : 1;/**< [  4:  4](R/W) Override TX power state machine TX tristate
                                                                 control signal. When set, TX tristate control signal is specified
                                                                 in GSER()_LANE()_TX_CFG_0[TX_TRISTATE_EN_OVRRD_VAL]. */
        uint64_t tx_pcs_reset_ovrrd_en : 1;  /**< [  3:  3](R/W) Override TX power state machine reset control
                                                                 signal. When set, reset control signals is specified in
                                                                 [TX_PCS_RESET_OVRRD_VAL]. */
        uint64_t tx_elec_idle_ovrrd_en : 1;  /**< [  2:  2](R/W) Override mac_pcs_txX_elec_idle signal
                                                                 When set, TX electrical idle is controlled from
                                                                 GSER()_LANE()_TX_CFG_1[TX_ELEC_IDLE_OVRRD_VAL]
                                                                 mac_pcs_txX_elec_idle signal is ignored. */
        uint64_t tx_pd_ovrrd_en        : 1;  /**< [  1:  1](R/W) Override TX power state machine TX lane
                                                                 power-down control signal
                                                                 When set, TX lane power down is controlled by
                                                                 GSER()_LANE()_TX_CFG_0[TX_CHPD_OVRRD_VAL]. */
        uint64_t tx_p2s_resetn_ovrrd_en : 1; /**< [  0:  0](R/W) Override TX power state machine TX reset
                                                                 control signal
                                                                 When set, TX reset is controlled by
                                                                 GSER()_LANE()_TX_CFG_0[TX_RESETN_OVRRD_VAL]. */
#else /* Word 0 - Little Endian */
        uint64_t tx_p2s_resetn_ovrrd_en : 1; /**< [  0:  0](R/W) Override TX power state machine TX reset
                                                                 control signal
                                                                 When set, TX reset is controlled by
                                                                 GSER()_LANE()_TX_CFG_0[TX_RESETN_OVRRD_VAL]. */
        uint64_t tx_pd_ovrrd_en        : 1;  /**< [  1:  1](R/W) Override TX power state machine TX lane
                                                                 power-down control signal
                                                                 When set, TX lane power down is controlled by
                                                                 GSER()_LANE()_TX_CFG_0[TX_CHPD_OVRRD_VAL]. */
        uint64_t tx_elec_idle_ovrrd_en : 1;  /**< [  2:  2](R/W) Override mac_pcs_txX_elec_idle signal
                                                                 When set, TX electrical idle is controlled from
                                                                 GSER()_LANE()_TX_CFG_1[TX_ELEC_IDLE_OVRRD_VAL]
                                                                 mac_pcs_txX_elec_idle signal is ignored. */
        uint64_t tx_pcs_reset_ovrrd_en : 1;  /**< [  3:  3](R/W) Override TX power state machine reset control
                                                                 signal. When set, reset control signals is specified in
                                                                 [TX_PCS_RESET_OVRRD_VAL]. */
        uint64_t tx_tristate_en_ovrrd_en : 1;/**< [  4:  4](R/W) Override TX power state machine TX tristate
                                                                 control signal. When set, TX tristate control signal is specified
                                                                 in GSER()_LANE()_TX_CFG_0[TX_TRISTATE_EN_OVRRD_VAL]. */
        uint64_t rx_lctrl_ovrrd_val    : 1;  /**< [  5:  5](R/W) Override RX power state machine power down
                                                                 control signal. When set, the power down control signal is
                                                                 specified by GSER()_LANE()_RX_CFG_1[RX_CHPD_OVRRD_VAL]. */
        uint64_t rx_lctrl_ovrrd_en     : 1;  /**< [  6:  6](R/W) Override RX power state machine loop control
                                                                 signals.  When set, the loop control settings are
                                                                 specified in the GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL] field. */
        uint64_t rx_resetn_ovrrd_val   : 1;  /**< [  7:  7](R/W) Override RX power state machine reset control
                                                                 signal. When set, reset control signals are specified in
                                                                 [RX_PCS_RESET_OVRRD_VAL]. */
        uint64_t rx_resetn_ovrrd_en    : 1;  /**< [  8:  8](R/W) Override RX power state machine rx_resetn
                                                                 control signal.  When set, the rx_resetn control signal is taken
                                                                 from the GSER()_LANE()_RX_CFG_0[RX_RESETN_OVRRD_VAL]
                                                                 control bit. */
        uint64_t reserved_9_10         : 2;
        uint64_t rx_pcs_reset_ovrrd_val : 1; /**< [ 11: 11](R/W) When RX_PCS_RESET_OVRRD_EN is
                                                                 asserted, this field is used to specify the value of
                                                                 the reset signal for PCS RX logic. */
        uint64_t tx_pcs_reset_ovrrd_val : 1; /**< [ 12: 12](R/W) When TX_PCS_RESET_OVRRD_EN is
                                                                 asserted, this field is used to specify the value of
                                                                 the reset signal for PCS TX logic. */
        uint64_t tx_sds_fifo_reset_ovrrd_val : 1;/**< [ 13: 13](R/W) When asserted, TX_SDS_FIFO_RESET_OVRRD_EN is asserted, this field is
                                                                 used to specify the value of the reset
                                                                 signal for the TX FIFO supplying data to the SerDes p2s interface. */
        uint64_t tx_sds_fifo_reset_ovrrd_en : 1;/**< [ 14: 14](R/W) When asserted, TX_SDS_FIFO_RESET_OVRRD_VAL is used to specify the value of the reset
                                                                 signal for the TX FIFO supplying data to the SerDes p2s interface. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_pwr_ctrl_s cn; */
};
typedef union bdk_gserx_lanex_pwr_ctrl bdk_gserx_lanex_pwr_ctrl_t;

static inline uint64_t BDK_GSERX_LANEX_PWR_CTRL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_PWR_CTRL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904400d8ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904400d8ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904400d8ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_PWR_CTRL", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_PWR_CTRL(a,b) bdk_gserx_lanex_pwr_ctrl_t
#define bustype_BDK_GSERX_LANEX_PWR_CTRL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_PWR_CTRL(a,b) "GSERX_LANEX_PWR_CTRL"
#define device_bar_BDK_GSERX_LANEX_PWR_CTRL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_PWR_CTRL(a,b) (a)
#define arguments_BDK_GSERX_LANEX_PWR_CTRL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_aeq_out_0
 *
 * GSER Lane SerDes RX Adaptive Equalizer 0 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_aeq_out_0
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_aeq_out_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t sds_pcs_rx_aeq_out    : 10; /**< [  9:  0](RO/H) \<9:5\>: DFE TAP5.
                                                                 \<4:0\>: DFE TAP4. */
#else /* Word 0 - Little Endian */
        uint64_t sds_pcs_rx_aeq_out    : 10; /**< [  9:  0](RO/H) \<9:5\>: DFE TAP5.
                                                                 \<4:0\>: DFE TAP4. */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_aeq_out_0_s cn; */
};
typedef union bdk_gserx_lanex_rx_aeq_out_0 bdk_gserx_lanex_rx_aeq_out_0_t;

static inline uint64_t BDK_GSERX_LANEX_RX_AEQ_OUT_0(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_AEQ_OUT_0(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440280ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440280ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440280ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_AEQ_OUT_0", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_AEQ_OUT_0(a,b) bdk_gserx_lanex_rx_aeq_out_0_t
#define bustype_BDK_GSERX_LANEX_RX_AEQ_OUT_0(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_AEQ_OUT_0(a,b) "GSERX_LANEX_RX_AEQ_OUT_0"
#define device_bar_BDK_GSERX_LANEX_RX_AEQ_OUT_0(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_AEQ_OUT_0(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_AEQ_OUT_0(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_aeq_out_1
 *
 * GSER Lane SerDes RX Adaptive Equalizer 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_aeq_out_1
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_aeq_out_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t sds_pcs_rx_aeq_out    : 15; /**< [ 14:  0](RO/H) \<14:10\> = DFE TAP3.
                                                                 \<9:5\> = DFE TAP2.
                                                                 \<4:0\> = DFE TAP1. */
#else /* Word 0 - Little Endian */
        uint64_t sds_pcs_rx_aeq_out    : 15; /**< [ 14:  0](RO/H) \<14:10\> = DFE TAP3.
                                                                 \<9:5\> = DFE TAP2.
                                                                 \<4:0\> = DFE TAP1. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_aeq_out_1_s cn; */
};
typedef union bdk_gserx_lanex_rx_aeq_out_1 bdk_gserx_lanex_rx_aeq_out_1_t;

static inline uint64_t BDK_GSERX_LANEX_RX_AEQ_OUT_1(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_AEQ_OUT_1(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440288ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440288ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440288ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_AEQ_OUT_1", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_AEQ_OUT_1(a,b) bdk_gserx_lanex_rx_aeq_out_1_t
#define bustype_BDK_GSERX_LANEX_RX_AEQ_OUT_1(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_AEQ_OUT_1(a,b) "GSERX_LANEX_RX_AEQ_OUT_1"
#define device_bar_BDK_GSERX_LANEX_RX_AEQ_OUT_1(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_AEQ_OUT_1(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_AEQ_OUT_1(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_aeq_out_2
 *
 * GSER Lane SerDes RX Adaptive Equalizer 2 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_aeq_out_2
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_aeq_out_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t sds_pcs_rx_aeq_out    : 15; /**< [ 14:  0](RO/H) \<9:8\> = Reserved.
                                                                 \<7:4\> = Pre-CTLE gain.
                                                                 \<3:0\> = Post-CTLE gain. */
#else /* Word 0 - Little Endian */
        uint64_t sds_pcs_rx_aeq_out    : 15; /**< [ 14:  0](RO/H) \<9:8\> = Reserved.
                                                                 \<7:4\> = Pre-CTLE gain.
                                                                 \<3:0\> = Post-CTLE gain. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_aeq_out_2_s cn; */
};
typedef union bdk_gserx_lanex_rx_aeq_out_2 bdk_gserx_lanex_rx_aeq_out_2_t;

static inline uint64_t BDK_GSERX_LANEX_RX_AEQ_OUT_2(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_AEQ_OUT_2(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440290ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440290ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440290ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_AEQ_OUT_2", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_AEQ_OUT_2(a,b) bdk_gserx_lanex_rx_aeq_out_2_t
#define bustype_BDK_GSERX_LANEX_RX_AEQ_OUT_2(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_AEQ_OUT_2(a,b) "GSERX_LANEX_RX_AEQ_OUT_2"
#define device_bar_BDK_GSERX_LANEX_RX_AEQ_OUT_2(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_AEQ_OUT_2(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_AEQ_OUT_2(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_cdr_ctrl_1
 *
 * GSER Lane SerDes RX CDR Control 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_cdr_ctrl_1
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_cdr_ctrl_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t cfg_rx_cdr_ctrl_ovrrd_val : 16;/**< [ 15:  0](R/W) Set CFG_RX_CDR_CTRL_OVRRD_EN in register
                                                                 GSER()_LANE()_RX_MISC_OVRRD to override pcs_sds_rx_cdr_ctrl.
                                                                 \<15:13\> = CDR frequency gain.
                                                                 \<12\>    = Frequency accumulator manual enable.
                                                                 \<11:5\>  = Frequency accumulator manual value.
                                                                 \<4\>     = CDR phase offset override enable.
                                                                 \<3:0\>   = CDR phase offset override, DLL IQ. */
#else /* Word 0 - Little Endian */
        uint64_t cfg_rx_cdr_ctrl_ovrrd_val : 16;/**< [ 15:  0](R/W) Set CFG_RX_CDR_CTRL_OVRRD_EN in register
                                                                 GSER()_LANE()_RX_MISC_OVRRD to override pcs_sds_rx_cdr_ctrl.
                                                                 \<15:13\> = CDR frequency gain.
                                                                 \<12\>    = Frequency accumulator manual enable.
                                                                 \<11:5\>  = Frequency accumulator manual value.
                                                                 \<4\>     = CDR phase offset override enable.
                                                                 \<3:0\>   = CDR phase offset override, DLL IQ. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_cdr_ctrl_1_s cn; */
};
typedef union bdk_gserx_lanex_rx_cdr_ctrl_1 bdk_gserx_lanex_rx_cdr_ctrl_1_t;

static inline uint64_t BDK_GSERX_LANEX_RX_CDR_CTRL_1(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_CDR_CTRL_1(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440038ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440038ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440038ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_CDR_CTRL_1", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_CDR_CTRL_1(a,b) bdk_gserx_lanex_rx_cdr_ctrl_1_t
#define bustype_BDK_GSERX_LANEX_RX_CDR_CTRL_1(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_CDR_CTRL_1(a,b) "GSERX_LANEX_RX_CDR_CTRL_1"
#define device_bar_BDK_GSERX_LANEX_RX_CDR_CTRL_1(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_CDR_CTRL_1(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_CDR_CTRL_1(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_cdr_ctrl_2
 *
 * GSER Lane SerDes RX CDR Control 2 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_cdr_ctrl_2
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_cdr_ctrl_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t cfg_rx_cdr_ctrl_ovrrd_val : 16;/**< [ 15:  0](R/W) Set CFG_RX_CDR_CTRL_OVRRD_EN in register
                                                                 GSER()_LANE()_RX_MISC_OVRRD to override pcs_sds_rx_cdr_ctrl.
                                                                 \<15\>   = Shadow PI phase enable.
                                                                 \<14:8\> = Shadow PI phase value.
                                                                 \<7\>    = CDR manual phase enable.
                                                                 \<6:0\>  = CDR manual phase value. */
#else /* Word 0 - Little Endian */
        uint64_t cfg_rx_cdr_ctrl_ovrrd_val : 16;/**< [ 15:  0](R/W) Set CFG_RX_CDR_CTRL_OVRRD_EN in register
                                                                 GSER()_LANE()_RX_MISC_OVRRD to override pcs_sds_rx_cdr_ctrl.
                                                                 \<15\>   = Shadow PI phase enable.
                                                                 \<14:8\> = Shadow PI phase value.
                                                                 \<7\>    = CDR manual phase enable.
                                                                 \<6:0\>  = CDR manual phase value. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_cdr_ctrl_2_s cn; */
};
typedef union bdk_gserx_lanex_rx_cdr_ctrl_2 bdk_gserx_lanex_rx_cdr_ctrl_2_t;

static inline uint64_t BDK_GSERX_LANEX_RX_CDR_CTRL_2(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_CDR_CTRL_2(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440040ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440040ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440040ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_CDR_CTRL_2", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_CDR_CTRL_2(a,b) bdk_gserx_lanex_rx_cdr_ctrl_2_t
#define bustype_BDK_GSERX_LANEX_RX_CDR_CTRL_2(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_CDR_CTRL_2(a,b) "GSERX_LANEX_RX_CDR_CTRL_2"
#define device_bar_BDK_GSERX_LANEX_RX_CDR_CTRL_2(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_CDR_CTRL_2(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_CDR_CTRL_2(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_cdr_misc_ctrl_0
 *
 * GSER Lane SerDes RX CDR Miscellaneous Control 0 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_cdr_misc_ctrl_0
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_cdr_misc_ctrl_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t pcs_sds_rx_cdr_misc_ctrl : 8;/**< [  7:  0](R/W) Per lane RX miscellaneous CDR control:
                                                                 \<7\> = RT-Eyemon counter enable, will start counting 5.4e9 bits.
                                                                 \<6\> = RT-Eyemon shadow PI control enable.
                                                                 \<5:4\> = RT-Eyemon error counter byte selection observable on
                                                                         SDS_OCS_RX_CDR_STATUS[14:7] in register GSER_LANE_RX_CDR_STATUS_1.
                                                                 \<3:0\> = LBW adjustment thresholds. */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_rx_cdr_misc_ctrl : 8;/**< [  7:  0](R/W) Per lane RX miscellaneous CDR control:
                                                                 \<7\> = RT-Eyemon counter enable, will start counting 5.4e9 bits.
                                                                 \<6\> = RT-Eyemon shadow PI control enable.
                                                                 \<5:4\> = RT-Eyemon error counter byte selection observable on
                                                                         SDS_OCS_RX_CDR_STATUS[14:7] in register GSER_LANE_RX_CDR_STATUS_1.
                                                                 \<3:0\> = LBW adjustment thresholds. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    struct bdk_gserx_lanex_rx_cdr_misc_ctrl_0_cn
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t reserved_8_15         : 8;
        uint64_t pcs_sds_rx_cdr_misc_ctrl : 8;/**< [  7:  0](R/W) Per lane RX miscellaneous CDR control:
                                                                 \<7\> = RT-Eyemon counter enable, will start counting 5.4e9 bits.
                                                                 \<6\> = RT-Eyemon shadow PI control enable.
                                                                 \<5:4\> = RT-Eyemon error counter byte selection observable on
                                                                         SDS_OCS_RX_CDR_STATUS[14:7] in register GSER_LANE_RX_CDR_STATUS_1.
                                                                 \<3:0\> = LBW adjustment thresholds. */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_rx_cdr_misc_ctrl : 8;/**< [  7:  0](R/W) Per lane RX miscellaneous CDR control:
                                                                 \<7\> = RT-Eyemon counter enable, will start counting 5.4e9 bits.
                                                                 \<6\> = RT-Eyemon shadow PI control enable.
                                                                 \<5:4\> = RT-Eyemon error counter byte selection observable on
                                                                         SDS_OCS_RX_CDR_STATUS[14:7] in register GSER_LANE_RX_CDR_STATUS_1.
                                                                 \<3:0\> = LBW adjustment thresholds. */
        uint64_t reserved_8_15         : 8;
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } cn;
};
typedef union bdk_gserx_lanex_rx_cdr_misc_ctrl_0 bdk_gserx_lanex_rx_cdr_misc_ctrl_0_t;

static inline uint64_t BDK_GSERX_LANEX_RX_CDR_MISC_CTRL_0(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_CDR_MISC_CTRL_0(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440208ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440208ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440208ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_CDR_MISC_CTRL_0", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_CDR_MISC_CTRL_0(a,b) bdk_gserx_lanex_rx_cdr_misc_ctrl_0_t
#define bustype_BDK_GSERX_LANEX_RX_CDR_MISC_CTRL_0(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_CDR_MISC_CTRL_0(a,b) "GSERX_LANEX_RX_CDR_MISC_CTRL_0"
#define device_bar_BDK_GSERX_LANEX_RX_CDR_MISC_CTRL_0(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_CDR_MISC_CTRL_0(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_CDR_MISC_CTRL_0(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_cdr_status_1
 *
 * GSER Lane SerDes RX CDR Status 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_cdr_status_1
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_cdr_status_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t sds_pcs_rx_cdr_status : 15; /**< [ 14:  0](RO/H) Per lane RX CDR status:
                                                                 \<14:7\> = RT-Eyemon error counter.
                                                                 \<6:4\>  = LBW adjustment value.
                                                                 \<3:0\>  = LBW adjustment state. */
#else /* Word 0 - Little Endian */
        uint64_t sds_pcs_rx_cdr_status : 15; /**< [ 14:  0](RO/H) Per lane RX CDR status:
                                                                 \<14:7\> = RT-Eyemon error counter.
                                                                 \<6:4\>  = LBW adjustment value.
                                                                 \<3:0\>  = LBW adjustment state. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    struct bdk_gserx_lanex_rx_cdr_status_1_cn
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t reserved_15           : 1;
        uint64_t sds_pcs_rx_cdr_status : 15; /**< [ 14:  0](RO/H) Per lane RX CDR status:
                                                                 \<14:7\> = RT-Eyemon error counter.
                                                                 \<6:4\>  = LBW adjustment value.
                                                                 \<3:0\>  = LBW adjustment state. */
#else /* Word 0 - Little Endian */
        uint64_t sds_pcs_rx_cdr_status : 15; /**< [ 14:  0](RO/H) Per lane RX CDR status:
                                                                 \<14:7\> = RT-Eyemon error counter.
                                                                 \<6:4\>  = LBW adjustment value.
                                                                 \<3:0\>  = LBW adjustment state. */
        uint64_t reserved_15           : 1;
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } cn;
};
typedef union bdk_gserx_lanex_rx_cdr_status_1 bdk_gserx_lanex_rx_cdr_status_1_t;

static inline uint64_t BDK_GSERX_LANEX_RX_CDR_STATUS_1(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_CDR_STATUS_1(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904402d0ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904402d0ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904402d0ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_CDR_STATUS_1", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_CDR_STATUS_1(a,b) bdk_gserx_lanex_rx_cdr_status_1_t
#define bustype_BDK_GSERX_LANEX_RX_CDR_STATUS_1(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_CDR_STATUS_1(a,b) "GSERX_LANEX_RX_CDR_STATUS_1"
#define device_bar_BDK_GSERX_LANEX_RX_CDR_STATUS_1(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_CDR_STATUS_1(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_CDR_STATUS_1(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_cdr_status_2
 *
 * GSER Lane SerDes RX CDR Status 2 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_cdr_status_2
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_cdr_status_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t sds_pcs_rx_cdr_status : 14; /**< [ 13:  0](RO/H) CDR status.
                                                                 \<13:7\> = CDR phase control output.
                                                                 \<6:0\> = CDR frequency accumulator output. */
#else /* Word 0 - Little Endian */
        uint64_t sds_pcs_rx_cdr_status : 14; /**< [ 13:  0](RO/H) CDR status.
                                                                 \<13:7\> = CDR phase control output.
                                                                 \<6:0\> = CDR frequency accumulator output. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_cdr_status_2_s cn; */
};
typedef union bdk_gserx_lanex_rx_cdr_status_2 bdk_gserx_lanex_rx_cdr_status_2_t;

static inline uint64_t BDK_GSERX_LANEX_RX_CDR_STATUS_2(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_CDR_STATUS_2(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904402d8ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904402d8ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904402d8ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_CDR_STATUS_2", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_CDR_STATUS_2(a,b) bdk_gserx_lanex_rx_cdr_status_2_t
#define bustype_BDK_GSERX_LANEX_RX_CDR_STATUS_2(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_CDR_STATUS_2(a,b) "GSERX_LANEX_RX_CDR_STATUS_2"
#define device_bar_BDK_GSERX_LANEX_RX_CDR_STATUS_2(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_CDR_STATUS_2(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_CDR_STATUS_2(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_cfg_0
 *
 * GSER Lane SerDes RX Configuration 0 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_cfg_0
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_cfg_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t rx_datarate_ovrrd_en  : 1;  /**< [ 15: 15](R/W) Override enable for RX power state machine data rate signal. */
        uint64_t pcs_sds_rx_tristate_enable : 1;/**< [ 14: 14](R/W) RX termination high-Z enable. */
        uint64_t rx_resetn_ovrrd_val   : 1;  /**< [ 13: 13](R/W) This value overrides the RX power state machine rx_resetn control
                                                                 signal when GSER()_LANE()_PWR_CTRL[RX_RESETN_OVRRD_EN] is set. */
        uint64_t pcs_sds_rx_eyemon_en  : 1;  /**< [ 12: 12](R/W) RX eyemon test enable. */
        uint64_t pcs_sds_rx_pcm_ctrl   : 4;  /**< [ 11:  8](R/W) \<11\>: Reserved.
                                                                 \<10-8\>:
                                                                   0x0 = 540mV.
                                                                   0x1 = 540mV + 20mV.
                                                                   0x2-0x3 = Reserved.
                                                                   0x4 = 100-620mV (default).
                                                                   0x5-0x7 = Reserved. */
        uint64_t rx_datarate_ovrrd_val : 2;  /**< [  7:  6](R/W) Specifies the data rate when RX_DATARATE_OVRRD_EN is asserted:
                                                                 0x0 = Full rate.
                                                                 0x1 = 1/2 data rate.
                                                                 0x2 = 1/4 data rate.
                                                                 0x3 = 1/8 data rate. */
        uint64_t cfg_rx_pol_invert     : 1;  /**< [  5:  5](R/W) Invert the receive data. Allies with GSER()_LANE()_MISC_CFG_0[USE_PMA_POLARITY]
                                                                 is deasserted. */
        uint64_t rx_subblk_pd_ovrrd_val : 5; /**< [  4:  0](R/W) Not supported. */
#else /* Word 0 - Little Endian */
        uint64_t rx_subblk_pd_ovrrd_val : 5; /**< [  4:  0](R/W) Not supported. */
        uint64_t cfg_rx_pol_invert     : 1;  /**< [  5:  5](R/W) Invert the receive data. Allies with GSER()_LANE()_MISC_CFG_0[USE_PMA_POLARITY]
                                                                 is deasserted. */
        uint64_t rx_datarate_ovrrd_val : 2;  /**< [  7:  6](R/W) Specifies the data rate when RX_DATARATE_OVRRD_EN is asserted:
                                                                 0x0 = Full rate.
                                                                 0x1 = 1/2 data rate.
                                                                 0x2 = 1/4 data rate.
                                                                 0x3 = 1/8 data rate. */
        uint64_t pcs_sds_rx_pcm_ctrl   : 4;  /**< [ 11:  8](R/W) \<11\>: Reserved.
                                                                 \<10-8\>:
                                                                   0x0 = 540mV.
                                                                   0x1 = 540mV + 20mV.
                                                                   0x2-0x3 = Reserved.
                                                                   0x4 = 100-620mV (default).
                                                                   0x5-0x7 = Reserved. */
        uint64_t pcs_sds_rx_eyemon_en  : 1;  /**< [ 12: 12](R/W) RX eyemon test enable. */
        uint64_t rx_resetn_ovrrd_val   : 1;  /**< [ 13: 13](R/W) This value overrides the RX power state machine rx_resetn control
                                                                 signal when GSER()_LANE()_PWR_CTRL[RX_RESETN_OVRRD_EN] is set. */
        uint64_t pcs_sds_rx_tristate_enable : 1;/**< [ 14: 14](R/W) RX termination high-Z enable. */
        uint64_t rx_datarate_ovrrd_en  : 1;  /**< [ 15: 15](R/W) Override enable for RX power state machine data rate signal. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_cfg_0_s cn; */
};
typedef union bdk_gserx_lanex_rx_cfg_0 bdk_gserx_lanex_rx_cfg_0_t;

static inline uint64_t BDK_GSERX_LANEX_RX_CFG_0(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_CFG_0(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440000ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440000ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440000ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_CFG_0", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_CFG_0(a,b) bdk_gserx_lanex_rx_cfg_0_t
#define bustype_BDK_GSERX_LANEX_RX_CFG_0(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_CFG_0(a,b) "GSERX_LANEX_RX_CFG_0"
#define device_bar_BDK_GSERX_LANEX_RX_CFG_0(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_CFG_0(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_CFG_0(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_cfg_1
 *
 * GSER Lane SerDes RX Configuration 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_cfg_1
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_cfg_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t rx_chpd_ovrrd_val     : 1;  /**< [ 15: 15](R/W) Not supported. */
        uint64_t pcs_sds_rx_os_men     : 1;  /**< [ 14: 14](R/W) RX offset manual enable. */
        uint64_t eie_en_ovrrd_en       : 1;  /**< [ 13: 13](R/W) Override enable for electrical-idle-exit circuit. */
        uint64_t eie_en_ovrrd_val      : 1;  /**< [ 12: 12](R/W) Override value for electrical-idle-exit circuit. */
        uint64_t reserved_11           : 1;
        uint64_t rx_pcie_mode_ovrrd_en : 1;  /**< [ 10: 10](R/W) Override enable for RX_PCIE_MODE_OVRRD_VAL. */
        uint64_t rx_pcie_mode_ovrrd_val : 1; /**< [  9:  9](R/W) Override value for RX_PCIE_MODE_OVRRD_VAL;
                                                                 selects between RX terminations.
                                                                 0x0 = pcs_sds_rx_terminate_to_vdda.
                                                                 0x1 = VDDA. */
        uint64_t cfg_rx_dll_locken     : 1;  /**< [  8:  8](R/W) Enable DLL lock when GSER()_LANE()_RX_MISC_OVRRD[CFG_RX_DLL_LOCKEN_OVRRD_EN] is asserted. */
        uint64_t pcs_sds_rx_cdr_ssc_mode : 8;/**< [  7:  0](R/W) Per-lane RX CDR SSC control:
                                                                 \<7:4\> = Reserved.
                                                                 \<3\> = Clean SSC error flag.
                                                                 \<2\> = Disable SSC filter.
                                                                 \<1\> = Enable SSC value usage.
                                                                 \<0\> = Reserved. */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_rx_cdr_ssc_mode : 8;/**< [  7:  0](R/W) Per-lane RX CDR SSC control:
                                                                 \<7:4\> = Reserved.
                                                                 \<3\> = Clean SSC error flag.
                                                                 \<2\> = Disable SSC filter.
                                                                 \<1\> = Enable SSC value usage.
                                                                 \<0\> = Reserved. */
        uint64_t cfg_rx_dll_locken     : 1;  /**< [  8:  8](R/W) Enable DLL lock when GSER()_LANE()_RX_MISC_OVRRD[CFG_RX_DLL_LOCKEN_OVRRD_EN] is asserted. */
        uint64_t rx_pcie_mode_ovrrd_val : 1; /**< [  9:  9](R/W) Override value for RX_PCIE_MODE_OVRRD_VAL;
                                                                 selects between RX terminations.
                                                                 0x0 = pcs_sds_rx_terminate_to_vdda.
                                                                 0x1 = VDDA. */
        uint64_t rx_pcie_mode_ovrrd_en : 1;  /**< [ 10: 10](R/W) Override enable for RX_PCIE_MODE_OVRRD_VAL. */
        uint64_t reserved_11           : 1;
        uint64_t eie_en_ovrrd_val      : 1;  /**< [ 12: 12](R/W) Override value for electrical-idle-exit circuit. */
        uint64_t eie_en_ovrrd_en       : 1;  /**< [ 13: 13](R/W) Override enable for electrical-idle-exit circuit. */
        uint64_t pcs_sds_rx_os_men     : 1;  /**< [ 14: 14](R/W) RX offset manual enable. */
        uint64_t rx_chpd_ovrrd_val     : 1;  /**< [ 15: 15](R/W) Not supported. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_cfg_1_s cn; */
};
typedef union bdk_gserx_lanex_rx_cfg_1 bdk_gserx_lanex_rx_cfg_1_t;

static inline uint64_t BDK_GSERX_LANEX_RX_CFG_1(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_CFG_1(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440008ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440008ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440008ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_CFG_1", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_CFG_1(a,b) bdk_gserx_lanex_rx_cfg_1_t
#define bustype_BDK_GSERX_LANEX_RX_CFG_1(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_CFG_1(a,b) "GSERX_LANEX_RX_CFG_1"
#define device_bar_BDK_GSERX_LANEX_RX_CFG_1(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_CFG_1(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_CFG_1(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_cfg_2
 *
 * GSER Lane SerDes RX Configuration 2 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_cfg_2
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_cfg_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t pcs_sds_rx_terminate_to_vdda : 1;/**< [ 14: 14](R/W) RX termination control:
                                                                 0 = Floating.
                                                                 1 = Terminate to sds_vdda. */
        uint64_t pcs_sds_rx_sampler_boost : 2;/**< [ 13: 12](R/W) Controls amount of boost.
                                                                 Note that this control can negatively impact reliability. */
        uint64_t pcs_sds_rx_sampler_boost_en : 1;/**< [ 11: 11](R/W) Faster sampler c2q.
                                                                 For diagnostic use only. */
        uint64_t reserved_10           : 1;
        uint64_t rx_sds_rx_agc_mval    : 10; /**< [  9:  0](R/W) AGC manual value used when
                                                                 GSER()_LANE()_RX_CFG_5[RX_AGC_MEN_OVRRD_EN,RX_AGC_MEN_OVRRD_VAL]
                                                                 are set.

                                                                 \<9:8\>: Reserved.

                                                                 \<7:4\>: Pre-CTLE (continuous time linear equalizer) gain (steps of approximately 0.75dB):
                                                                 _ 0x0 = -6dB.
                                                                 _ 0x1 = -5dB.
                                                                 _ 0xF = +5dB.

                                                                 \<3:0\>: Post-CTLE gain (steps of 0.0875):
                                                                 _ 0x0 = lowest.
                                                                 _ 0xF = lowest * 2.3125.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<
                                                                 5 Gbaud, pre-CTLE, post-CTLE, and peaking control settings should be manually
                                                                 configured. GSER()_LANE()_RX_CFG_5[RX_AGC_MEN_OVRRD_EN,RX_AGC_MEN_OVRRD_VAL]
                                                                 should both be set, [RX_SDS_RX_AGC_MVAL] has the pre and post settings,
                                                                 and GSER()_LANE()_RX_CTLE_CTRL[PCS_SDS_RX_CTLE_ZERO] controls equalizer
                                                                 peaking.

                                                                 The [RX_SDS_RX_AGC_MVAL] settings should be derived from signal integrity
                                                                 simulations with the IBIS-AMI model supplied by Cavium when
                                                                 GSER()_LANE()_RX_CFG_5[RX_AGC_MEN_OVRRD_EN,RX_AGC_MEN_OVRRD_VAL] are set.

                                                                 Internal:
                                                                 reset value may be reasonable default settings. */
#else /* Word 0 - Little Endian */
        uint64_t rx_sds_rx_agc_mval    : 10; /**< [  9:  0](R/W) AGC manual value used when
                                                                 GSER()_LANE()_RX_CFG_5[RX_AGC_MEN_OVRRD_EN,RX_AGC_MEN_OVRRD_VAL]
                                                                 are set.

                                                                 \<9:8\>: Reserved.

                                                                 \<7:4\>: Pre-CTLE (continuous time linear equalizer) gain (steps of approximately 0.75dB):
                                                                 _ 0x0 = -6dB.
                                                                 _ 0x1 = -5dB.
                                                                 _ 0xF = +5dB.

                                                                 \<3:0\>: Post-CTLE gain (steps of 0.0875):
                                                                 _ 0x0 = lowest.
                                                                 _ 0xF = lowest * 2.3125.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<
                                                                 5 Gbaud, pre-CTLE, post-CTLE, and peaking control settings should be manually
                                                                 configured. GSER()_LANE()_RX_CFG_5[RX_AGC_MEN_OVRRD_EN,RX_AGC_MEN_OVRRD_VAL]
                                                                 should both be set, [RX_SDS_RX_AGC_MVAL] has the pre and post settings,
                                                                 and GSER()_LANE()_RX_CTLE_CTRL[PCS_SDS_RX_CTLE_ZERO] controls equalizer
                                                                 peaking.

                                                                 The [RX_SDS_RX_AGC_MVAL] settings should be derived from signal integrity
                                                                 simulations with the IBIS-AMI model supplied by Cavium when
                                                                 GSER()_LANE()_RX_CFG_5[RX_AGC_MEN_OVRRD_EN,RX_AGC_MEN_OVRRD_VAL] are set.

                                                                 Internal:
                                                                 reset value may be reasonable default settings. */
        uint64_t reserved_10           : 1;
        uint64_t pcs_sds_rx_sampler_boost_en : 1;/**< [ 11: 11](R/W) Faster sampler c2q.
                                                                 For diagnostic use only. */
        uint64_t pcs_sds_rx_sampler_boost : 2;/**< [ 13: 12](R/W) Controls amount of boost.
                                                                 Note that this control can negatively impact reliability. */
        uint64_t pcs_sds_rx_terminate_to_vdda : 1;/**< [ 14: 14](R/W) RX termination control:
                                                                 0 = Floating.
                                                                 1 = Terminate to sds_vdda. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_cfg_2_s cn; */
};
typedef union bdk_gserx_lanex_rx_cfg_2 bdk_gserx_lanex_rx_cfg_2_t;

static inline uint64_t BDK_GSERX_LANEX_RX_CFG_2(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_CFG_2(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440010ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440010ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440010ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_CFG_2", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_CFG_2(a,b) bdk_gserx_lanex_rx_cfg_2_t
#define bustype_BDK_GSERX_LANEX_RX_CFG_2(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_CFG_2(a,b) "GSERX_LANEX_RX_CFG_2"
#define device_bar_BDK_GSERX_LANEX_RX_CFG_2(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_CFG_2(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_CFG_2(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_cfg_3
 *
 * GSER Lane SerDes RX Configuration 3 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_cfg_3
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_cfg_3_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t cfg_rx_errdet_ctrl    : 16; /**< [ 15:  0](R/W) RX adaptive equalizer control.
                                                                 Value of pcs_sds_rx_err_det_ctrl when
                                                                 GSER()_LANE()_RX_MISC_OVRRD[CFG_RS_ERRDET_CTRL_OVRRD_EN}
                                                                 is set.

                                                                 \<15:13\>: Starting delta (6.7mV/step, 13.4mV + 6.7mV*N).

                                                                 \<12:10\>: Minimum delta to adapt to (6.7mV/step, 13.4mV + 6.7mV*N).

                                                                 \<9:7\>: Window mode (PM) delta (6.7mV/step, 13.4mV + 6.7mV*N).

                                                                 \<6\>: Enable DFE for edge samplers.

                                                                 \<5:4\>: Edge sampler DEF alpha:
                                                                 0x0 = 1/4.
                                                                 0x1 = 1/2.
                                                                 0x2 = 3/4.
                                                                 0x3 = 1.

                                                                 \<3:0\>: Q/QB error sampler 1 threshold, 6.7mV/step. */
#else /* Word 0 - Little Endian */
        uint64_t cfg_rx_errdet_ctrl    : 16; /**< [ 15:  0](R/W) RX adaptive equalizer control.
                                                                 Value of pcs_sds_rx_err_det_ctrl when
                                                                 GSER()_LANE()_RX_MISC_OVRRD[CFG_RS_ERRDET_CTRL_OVRRD_EN}
                                                                 is set.

                                                                 \<15:13\>: Starting delta (6.7mV/step, 13.4mV + 6.7mV*N).

                                                                 \<12:10\>: Minimum delta to adapt to (6.7mV/step, 13.4mV + 6.7mV*N).

                                                                 \<9:7\>: Window mode (PM) delta (6.7mV/step, 13.4mV + 6.7mV*N).

                                                                 \<6\>: Enable DFE for edge samplers.

                                                                 \<5:4\>: Edge sampler DEF alpha:
                                                                 0x0 = 1/4.
                                                                 0x1 = 1/2.
                                                                 0x2 = 3/4.
                                                                 0x3 = 1.

                                                                 \<3:0\>: Q/QB error sampler 1 threshold, 6.7mV/step. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_cfg_3_s cn; */
};
typedef union bdk_gserx_lanex_rx_cfg_3 bdk_gserx_lanex_rx_cfg_3_t;

static inline uint64_t BDK_GSERX_LANEX_RX_CFG_3(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_CFG_3(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440018ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440018ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440018ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_CFG_3", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_CFG_3(a,b) bdk_gserx_lanex_rx_cfg_3_t
#define bustype_BDK_GSERX_LANEX_RX_CFG_3(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_CFG_3(a,b) "GSERX_LANEX_RX_CFG_3"
#define device_bar_BDK_GSERX_LANEX_RX_CFG_3(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_CFG_3(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_CFG_3(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_cfg_4
 *
 * GSER Lane SerDes RX Configuration 4 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_cfg_4
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_cfg_4_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t cfg_rx_errdet_ctrl    : 16; /**< [ 15:  0](R/W) RX adaptive equalizer control.
                                                                 Value of pcs_sds_rx_err_det_ctrl when
                                                                 GSER()_LANE()_RX_MISC_OVRRD[CFG_RS_ERRDET_CTRL_OVRRD_EN] is set.

                                                                 \<15:14\>: Reserved.

                                                                 \<13:8\>: Q/QB error sampler 0 threshold, 6.7mV/step, used for training/LMS.

                                                                 \<7\>: Enable Window mode, after training has finished.

                                                                 \<6:5\>: Control sds_pcs_rx_vma_status[15:8].

                                                                      0x0 = window counter[19:12] (FOM).
                                                                      0x1 = window counter[11:4].
                                                                      0x2 = CTLE pole, SDLL_IQ.
                                                                      0x3 = pre-CTLE gain, CTLE peak.

                                                                 \<4\>: Offset cancellation enable.

                                                                 \<3:0\>: Max CTLE peak setting during training when pcs_sds_rx_vma_ctl[7] is set in
                                                                 GSER()_LANE()_RX_VMA_CTRL. */
#else /* Word 0 - Little Endian */
        uint64_t cfg_rx_errdet_ctrl    : 16; /**< [ 15:  0](R/W) RX adaptive equalizer control.
                                                                 Value of pcs_sds_rx_err_det_ctrl when
                                                                 GSER()_LANE()_RX_MISC_OVRRD[CFG_RS_ERRDET_CTRL_OVRRD_EN] is set.

                                                                 \<15:14\>: Reserved.

                                                                 \<13:8\>: Q/QB error sampler 0 threshold, 6.7mV/step, used for training/LMS.

                                                                 \<7\>: Enable Window mode, after training has finished.

                                                                 \<6:5\>: Control sds_pcs_rx_vma_status[15:8].

                                                                      0x0 = window counter[19:12] (FOM).
                                                                      0x1 = window counter[11:4].
                                                                      0x2 = CTLE pole, SDLL_IQ.
                                                                      0x3 = pre-CTLE gain, CTLE peak.

                                                                 \<4\>: Offset cancellation enable.

                                                                 \<3:0\>: Max CTLE peak setting during training when pcs_sds_rx_vma_ctl[7] is set in
                                                                 GSER()_LANE()_RX_VMA_CTRL. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_cfg_4_s cn; */
};
typedef union bdk_gserx_lanex_rx_cfg_4 bdk_gserx_lanex_rx_cfg_4_t;

static inline uint64_t BDK_GSERX_LANEX_RX_CFG_4(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_CFG_4(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440020ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440020ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440020ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_CFG_4", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_CFG_4(a,b) bdk_gserx_lanex_rx_cfg_4_t
#define bustype_BDK_GSERX_LANEX_RX_CFG_4(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_CFG_4(a,b) "GSERX_LANEX_RX_CFG_4"
#define device_bar_BDK_GSERX_LANEX_RX_CFG_4(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_CFG_4(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_CFG_4(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_cfg_5
 *
 * GSER Lane SerDes RX Configuration 5 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_cfg_5
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_cfg_5_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t rx_agc_men_ovrrd_en   : 1;  /**< [  4:  4](R/W) Override enable for AGC manual mode.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<
                                                                 5 Gbaud, pre-CTLE, post-CTLE, and peaking control settings should be manually
                                                                 configured. [RX_AGC_MEN_OVRRD_EN,RX_AGC_MEN_OVRRD_VAL] should both be set,
                                                                 GSER()_LANE()_RX_CFG_2[RX_SDS_RX_AGC_MVAL] has the pre and post settings,
                                                                 and GSER()_LANE()_RX_CTLE_CTRL[PCS_SDS_RX_CTLE_ZERO] controls equalizer
                                                                 peaking. */
        uint64_t rx_agc_men_ovrrd_val  : 1;  /**< [  3:  3](R/W) Override value for AGC manual mode.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<
                                                                 5 Gbaud, pre-CTLE, post-CTLE, and peaking control settings should be manually
                                                                 configured. [RX_AGC_MEN_OVRRD_EN,RX_AGC_MEN_OVRRD_VAL] should both be set,
                                                                 GSER()_LANE()_RX_CFG_2[RX_SDS_RX_AGC_MVAL] has the pre and post settings,
                                                                 and GSER()_LANE()_RX_CTLE_CTRL[PCS_SDS_RX_CTLE_ZERO] controls equalizer
                                                                 peaking. */
        uint64_t rx_widthsel_ovrrd_en  : 1;  /**< [  2:  2](R/W) Override enable for RX width select to the SerDes pcs_sds_rx_widthsel. */
        uint64_t rx_widthsel_ovrrd_val : 2;  /**< [  1:  0](R/W) Override value for RX width select to the SerDes pcs_sds_rx_widthsel.
                                                                 0x0 = 8-bit raw data.
                                                                 0x1 = 10-bit raw data.
                                                                 0x2 = 16-bit raw data.
                                                                 0x3 = 20-bit raw data. */
#else /* Word 0 - Little Endian */
        uint64_t rx_widthsel_ovrrd_val : 2;  /**< [  1:  0](R/W) Override value for RX width select to the SerDes pcs_sds_rx_widthsel.
                                                                 0x0 = 8-bit raw data.
                                                                 0x1 = 10-bit raw data.
                                                                 0x2 = 16-bit raw data.
                                                                 0x3 = 20-bit raw data. */
        uint64_t rx_widthsel_ovrrd_en  : 1;  /**< [  2:  2](R/W) Override enable for RX width select to the SerDes pcs_sds_rx_widthsel. */
        uint64_t rx_agc_men_ovrrd_val  : 1;  /**< [  3:  3](R/W) Override value for AGC manual mode.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<
                                                                 5 Gbaud, pre-CTLE, post-CTLE, and peaking control settings should be manually
                                                                 configured. [RX_AGC_MEN_OVRRD_EN,RX_AGC_MEN_OVRRD_VAL] should both be set,
                                                                 GSER()_LANE()_RX_CFG_2[RX_SDS_RX_AGC_MVAL] has the pre and post settings,
                                                                 and GSER()_LANE()_RX_CTLE_CTRL[PCS_SDS_RX_CTLE_ZERO] controls equalizer
                                                                 peaking. */
        uint64_t rx_agc_men_ovrrd_en   : 1;  /**< [  4:  4](R/W) Override enable for AGC manual mode.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<
                                                                 5 Gbaud, pre-CTLE, post-CTLE, and peaking control settings should be manually
                                                                 configured. [RX_AGC_MEN_OVRRD_EN,RX_AGC_MEN_OVRRD_VAL] should both be set,
                                                                 GSER()_LANE()_RX_CFG_2[RX_SDS_RX_AGC_MVAL] has the pre and post settings,
                                                                 and GSER()_LANE()_RX_CTLE_CTRL[PCS_SDS_RX_CTLE_ZERO] controls equalizer
                                                                 peaking. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_cfg_5_s cn; */
};
typedef union bdk_gserx_lanex_rx_cfg_5 bdk_gserx_lanex_rx_cfg_5_t;

static inline uint64_t BDK_GSERX_LANEX_RX_CFG_5(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_CFG_5(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440028ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440028ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440028ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_CFG_5", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_CFG_5(a,b) bdk_gserx_lanex_rx_cfg_5_t
#define bustype_BDK_GSERX_LANEX_RX_CFG_5(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_CFG_5(a,b) "GSERX_LANEX_RX_CFG_5"
#define device_bar_BDK_GSERX_LANEX_RX_CFG_5(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_CFG_5(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_CFG_5(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_ctle_ctrl
 *
 * GSER Lane RX Precorrelation Control Register
 * These are the RAW PCS per-lane RX CTLE control registers.
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_ctle_ctrl
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_ctle_ctrl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t pcs_sds_rx_ctle_bias_ctrl : 2;/**< [ 15: 14](R/W) CTLE bias trim bits.
                                                                 0x0 = -10%.
                                                                 0x1 =  0%.
                                                                 0x2 = +5%.
                                                                 0x3 = +10%. */
        uint64_t pcs_sds_rx_ctle_zero  : 4;  /**< [ 13: 10](R/W) Equalizer peaking control.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<
                                                                 5 Gbaud,
                                                                 pre-CTLE, post-CTLE, and peaking control settings should be manually
                                                                 configured. GSER()_LANE()_RX_CFG_5[RX_AGC_MEN_OVRRD_EN,RX_AGC_MEN_OVRRD_VAL]
                                                                 should both be set, GSER()_LANE()_RX_CFG_2[RX_SDS_RX_AGC_MVAL] has the
                                                                 pre and post settings, and [PCS_SDS_RX_CTLE_ZERO] controls equalizer
                                                                 peaking.

                                                                 The [PCS_SDS_RX_CTLE_ZERO] setting should be derived from signal integrity
                                                                 simulations with the IBIS-AMI model supplied by Cavium when auto-negotiated
                                                                 link training is not present and link speed \< 5 Gbaud. */
        uint64_t rx_ctle_pole_ovrrd_en : 1;  /**< [  9:  9](R/W) Equalizer pole adjustment override enable. */
        uint64_t rx_ctle_pole_ovrrd_val : 4; /**< [  8:  5](R/W) Equalizer pole adjustment override value.
                                                                 RX precorrelation sample counter control
                                                                 bit 3: Optimize CTLE during training.
                                                                 bit 2: Turn off DFE1 for edge samplers.
                                                                 bits 1:0:
                                                                 0x0 = ~ 5dB of peaking at 4.0 GHz.
                                                                 0x1 = ~10dB of peaking at 5.0 GHz.
                                                                 0x2 = ~15dB of peaking at 5.5 GHz.
                                                                 0x3 = ~20dB of peaking at 6.0 GHz. */
        uint64_t pcs_sds_rx_ctle_pole_max : 2;/**< [  4:  3](R/W) Maximum pole value (for VMA adaption, not applicable in manual mode). */
        uint64_t pcs_sds_rx_ctle_pole_min : 2;/**< [  2:  1](R/W) Minimum pole value (for VMA adaption, not applicable in manual mode). */
        uint64_t pcs_sds_rx_ctle_pole_step : 1;/**< [  0:  0](R/W) Step pole value (for VMA adaption, not applicable in manual mode). */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_rx_ctle_pole_step : 1;/**< [  0:  0](R/W) Step pole value (for VMA adaption, not applicable in manual mode). */
        uint64_t pcs_sds_rx_ctle_pole_min : 2;/**< [  2:  1](R/W) Minimum pole value (for VMA adaption, not applicable in manual mode). */
        uint64_t pcs_sds_rx_ctle_pole_max : 2;/**< [  4:  3](R/W) Maximum pole value (for VMA adaption, not applicable in manual mode). */
        uint64_t rx_ctle_pole_ovrrd_val : 4; /**< [  8:  5](R/W) Equalizer pole adjustment override value.
                                                                 RX precorrelation sample counter control
                                                                 bit 3: Optimize CTLE during training.
                                                                 bit 2: Turn off DFE1 for edge samplers.
                                                                 bits 1:0:
                                                                 0x0 = ~ 5dB of peaking at 4.0 GHz.
                                                                 0x1 = ~10dB of peaking at 5.0 GHz.
                                                                 0x2 = ~15dB of peaking at 5.5 GHz.
                                                                 0x3 = ~20dB of peaking at 6.0 GHz. */
        uint64_t rx_ctle_pole_ovrrd_en : 1;  /**< [  9:  9](R/W) Equalizer pole adjustment override enable. */
        uint64_t pcs_sds_rx_ctle_zero  : 4;  /**< [ 13: 10](R/W) Equalizer peaking control.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<
                                                                 5 Gbaud,
                                                                 pre-CTLE, post-CTLE, and peaking control settings should be manually
                                                                 configured. GSER()_LANE()_RX_CFG_5[RX_AGC_MEN_OVRRD_EN,RX_AGC_MEN_OVRRD_VAL]
                                                                 should both be set, GSER()_LANE()_RX_CFG_2[RX_SDS_RX_AGC_MVAL] has the
                                                                 pre and post settings, and [PCS_SDS_RX_CTLE_ZERO] controls equalizer
                                                                 peaking.

                                                                 The [PCS_SDS_RX_CTLE_ZERO] setting should be derived from signal integrity
                                                                 simulations with the IBIS-AMI model supplied by Cavium when auto-negotiated
                                                                 link training is not present and link speed \< 5 Gbaud. */
        uint64_t pcs_sds_rx_ctle_bias_ctrl : 2;/**< [ 15: 14](R/W) CTLE bias trim bits.
                                                                 0x0 = -10%.
                                                                 0x1 =  0%.
                                                                 0x2 = +5%.
                                                                 0x3 = +10%. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_ctle_ctrl_s cn; */
};
typedef union bdk_gserx_lanex_rx_ctle_ctrl bdk_gserx_lanex_rx_ctle_ctrl_t;

static inline uint64_t BDK_GSERX_LANEX_RX_CTLE_CTRL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_CTLE_CTRL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440058ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440058ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440058ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_CTLE_CTRL", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_CTLE_CTRL(a,b) bdk_gserx_lanex_rx_ctle_ctrl_t
#define bustype_BDK_GSERX_LANEX_RX_CTLE_CTRL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_CTLE_CTRL(a,b) "GSERX_LANEX_RX_CTLE_CTRL"
#define device_bar_BDK_GSERX_LANEX_RX_CTLE_CTRL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_CTLE_CTRL(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_CTLE_CTRL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_loop_ctrl
 *
 * GSER Lane RX Loop Control Registers
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_loop_ctrl
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_loop_ctrl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t fast_dll_lock         : 1;  /**< [ 11: 11](R/W/H) Assert to enable fast DLL lock (for simulation purposes only). */
        uint64_t fast_ofst_cncl        : 1;  /**< [ 10: 10](R/W/H) Assert to enable fast Offset cancellation (for simulation purposes only). */
        uint64_t cfg_rx_lctrl          : 10; /**< [  9:  0](R/W) Loop control settings.

                                                                 \<0\> = cdr_en_byp.
                                                                 \<1\> = dfe_en_byp.
                                                                 \<2\> = agc_en_byp.
                                                                 \<3\> = ofst_cncl_en_byp.
                                                                 \<4\> = CDR resetn.
                                                                 \<5\> = CTLE resetn.
                                                                 \<6\> = VMA resetn.
                                                                 \<7\> = ofst_cncl_rstn_byp.
                                                                 \<8\> = lctrl_men.
                                                                 \<9\> = Reserved.

                                                                 GSER()_LANE()_PWR_CTRL[RX_LCTRL_OVRRD_EN] controls \<9:7\> and \<3:0\>.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present, non-SATA/PCIe, and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL],
                                                                 setting [CFG_RX_LCTRL\<8\>], clearing [CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER(0..6)_LANE(0..1)_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,
                                                                 DFE_C4_MVAL,DFE_C4_MSGN], and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
#else /* Word 0 - Little Endian */
        uint64_t cfg_rx_lctrl          : 10; /**< [  9:  0](R/W) Loop control settings.

                                                                 \<0\> = cdr_en_byp.
                                                                 \<1\> = dfe_en_byp.
                                                                 \<2\> = agc_en_byp.
                                                                 \<3\> = ofst_cncl_en_byp.
                                                                 \<4\> = CDR resetn.
                                                                 \<5\> = CTLE resetn.
                                                                 \<6\> = VMA resetn.
                                                                 \<7\> = ofst_cncl_rstn_byp.
                                                                 \<8\> = lctrl_men.
                                                                 \<9\> = Reserved.

                                                                 GSER()_LANE()_PWR_CTRL[RX_LCTRL_OVRRD_EN] controls \<9:7\> and \<3:0\>.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present, non-SATA/PCIe, and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL],
                                                                 setting [CFG_RX_LCTRL\<8\>], clearing [CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER(0..6)_LANE(0..1)_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,
                                                                 DFE_C4_MVAL,DFE_C4_MSGN], and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t fast_ofst_cncl        : 1;  /**< [ 10: 10](R/W/H) Assert to enable fast Offset cancellation (for simulation purposes only). */
        uint64_t fast_dll_lock         : 1;  /**< [ 11: 11](R/W/H) Assert to enable fast DLL lock (for simulation purposes only). */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_loop_ctrl_s cn81xx; */
    struct bdk_gserx_lanex_rx_loop_ctrl_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t fast_dll_lock         : 1;  /**< [ 11: 11](R/W/H) Assert to enable fast DLL lock (for simulation purposes only). */
        uint64_t fast_ofst_cncl        : 1;  /**< [ 10: 10](R/W/H) Assert to enable fast Offset cancellation (for simulation purposes only). */
        uint64_t cfg_rx_lctrl          : 10; /**< [  9:  0](R/W) Loop control settings.

                                                                 \<0\> = cdr_en_byp.
                                                                 \<1\> = dfe_en_byp.
                                                                 \<2\> = agc_en_byp.
                                                                 \<3\> = ofst_cncl_en_byp.
                                                                 \<4\> = CDR resetn.
                                                                 \<5\> = CTLE resetn.
                                                                 \<6\> = VMA resetn.
                                                                 \<7\> = ofst_cncl_rstn_byp.
                                                                 \<8\> = lctrl_men.
                                                                 \<9\> = Reserved.

                                                                 GSER()_LANE()_PWR_CTRL[RX_LCTRL_OVRRD_EN] controls \<9:7\> and \<3:0\>.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present, non-SATA/PCIe, and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL],
                                                                 setting [CFG_RX_LCTRL\<8\>], clearing [CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER(0..6)_LANE(0..3)_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,
                                                                 DFE_C4_MVAL,DFE_C4_MSGN], and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
#else /* Word 0 - Little Endian */
        uint64_t cfg_rx_lctrl          : 10; /**< [  9:  0](R/W) Loop control settings.

                                                                 \<0\> = cdr_en_byp.
                                                                 \<1\> = dfe_en_byp.
                                                                 \<2\> = agc_en_byp.
                                                                 \<3\> = ofst_cncl_en_byp.
                                                                 \<4\> = CDR resetn.
                                                                 \<5\> = CTLE resetn.
                                                                 \<6\> = VMA resetn.
                                                                 \<7\> = ofst_cncl_rstn_byp.
                                                                 \<8\> = lctrl_men.
                                                                 \<9\> = Reserved.

                                                                 GSER()_LANE()_PWR_CTRL[RX_LCTRL_OVRRD_EN] controls \<9:7\> and \<3:0\>.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present, non-SATA/PCIe, and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL],
                                                                 setting [CFG_RX_LCTRL\<8\>], clearing [CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER(0..6)_LANE(0..3)_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,
                                                                 DFE_C4_MVAL,DFE_C4_MSGN], and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t fast_ofst_cncl        : 1;  /**< [ 10: 10](R/W/H) Assert to enable fast Offset cancellation (for simulation purposes only). */
        uint64_t fast_dll_lock         : 1;  /**< [ 11: 11](R/W/H) Assert to enable fast DLL lock (for simulation purposes only). */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } cn88xx;
    /* struct bdk_gserx_lanex_rx_loop_ctrl_cn88xx cn83xx; */
};
typedef union bdk_gserx_lanex_rx_loop_ctrl bdk_gserx_lanex_rx_loop_ctrl_t;

static inline uint64_t BDK_GSERX_LANEX_RX_LOOP_CTRL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_LOOP_CTRL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440048ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440048ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440048ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_LOOP_CTRL", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_LOOP_CTRL(a,b) bdk_gserx_lanex_rx_loop_ctrl_t
#define bustype_BDK_GSERX_LANEX_RX_LOOP_CTRL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_LOOP_CTRL(a,b) "GSERX_LANEX_RX_LOOP_CTRL"
#define device_bar_BDK_GSERX_LANEX_RX_LOOP_CTRL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_LOOP_CTRL(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_LOOP_CTRL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_misc_ctrl
 *
 * GSER Lane RX Miscellaneous Control Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_misc_ctrl
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_misc_ctrl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t pcs_sds_rx_misc_ctrl  : 8;  /**< [  7:  0](R/W/H) Miscellaneous receive control settings.

                                                                 \<0\> = Shadow PI control. Must set when using the RX internal eye monitor.
                                                                 \<1\> = Reserved.
                                                                 \<3:2\> = Offset cal.
                                                                 \<4\> = Reserved.
                                                                 \<5\> = Reserved.
                                                                 \<6\> = 1149 hysteresis control.
                                                                 \<7\> = Reserved. */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_rx_misc_ctrl  : 8;  /**< [  7:  0](R/W/H) Miscellaneous receive control settings.

                                                                 \<0\> = Shadow PI control. Must set when using the RX internal eye monitor.
                                                                 \<1\> = Reserved.
                                                                 \<3:2\> = Offset cal.
                                                                 \<4\> = Reserved.
                                                                 \<5\> = Reserved.
                                                                 \<6\> = 1149 hysteresis control.
                                                                 \<7\> = Reserved. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_misc_ctrl_s cn; */
};
typedef union bdk_gserx_lanex_rx_misc_ctrl bdk_gserx_lanex_rx_misc_ctrl_t;

static inline uint64_t BDK_GSERX_LANEX_RX_MISC_CTRL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_MISC_CTRL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440050ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440050ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440050ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_MISC_CTRL", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_MISC_CTRL(a,b) bdk_gserx_lanex_rx_misc_ctrl_t
#define bustype_BDK_GSERX_LANEX_RX_MISC_CTRL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_MISC_CTRL(a,b) "GSERX_LANEX_RX_MISC_CTRL"
#define device_bar_BDK_GSERX_LANEX_RX_MISC_CTRL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_MISC_CTRL(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_MISC_CTRL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_misc_ovrrd
 *
 * GSER Lane RX Miscellaneous Override Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_misc_ovrrd
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_misc_ovrrd_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t cfg_rx_oob_clk_en_ovrrd_val : 1;/**< [ 13: 13](R/W) Override value for RX OOB clock enable. */
        uint64_t cfg_rx_oob_clk_en_ovrrd_en : 1;/**< [ 12: 12](R/W) Override enable for RX OOB clock enable. */
        uint64_t cfg_rx_eie_det_ovrrd_val : 1;/**< [ 11: 11](R/W) Override value for RX electrical-idle-exit
                                                                 detect enable. */
        uint64_t cfg_rx_eie_det_ovrrd_en : 1;/**< [ 10: 10](R/W) Override enable for RX electrical-idle-exit
                                                                 detect enable. */
        uint64_t cfg_rx_cdr_ctrl_ovrrd_en : 1;/**< [  9:  9](R/W) Not supported. */
        uint64_t cfg_rx_eq_eval_ovrrd_val : 1;/**< [  8:  8](R/W) Training mode control in override mode. */
        uint64_t cfg_rx_eq_eval_ovrrd_en : 1;/**< [  7:  7](R/W) Override enable for RX-EQ eval.
                                                                 When asserted, training mode is controlled by
                                                                 CFG_RX_EQ_EVAL_OVRRD_VAL. */
        uint64_t reserved_6            : 1;
        uint64_t cfg_rx_dll_locken_ovrrd_en : 1;/**< [  5:  5](R/W) When asserted, override DLL lock enable
                                                                 signal from the RX power state machine with
                                                                 CFG_RX_DLL_LOCKEN in register
                                                                 GSER()_LANE()_RX_CFG_1. */
        uint64_t cfg_rx_errdet_ctrl_ovrrd_en : 1;/**< [  4:  4](R/W) When asserted, pcs_sds_rx_err_det_ctrl is set
                                                                 to cfg_rx_errdet_ctrl in registers
                                                                 GSER()_LANE()_RX_CFG_3 and GSER()_LANE()_RX_CFG_4. */
        uint64_t reserved_1_3          : 3;
        uint64_t cfg_rxeq_eval_restore_en : 1;/**< [  0:  0](R/W) When asserted, AGC and CTLE use the RX EQ settings determined from RX EQ
                                                                 evaluation process when VMA is not in manual mode. Otherwise, default settings are used. */
#else /* Word 0 - Little Endian */
        uint64_t cfg_rxeq_eval_restore_en : 1;/**< [  0:  0](R/W) When asserted, AGC and CTLE use the RX EQ settings determined from RX EQ
                                                                 evaluation process when VMA is not in manual mode. Otherwise, default settings are used. */
        uint64_t reserved_1_3          : 3;
        uint64_t cfg_rx_errdet_ctrl_ovrrd_en : 1;/**< [  4:  4](R/W) When asserted, pcs_sds_rx_err_det_ctrl is set
                                                                 to cfg_rx_errdet_ctrl in registers
                                                                 GSER()_LANE()_RX_CFG_3 and GSER()_LANE()_RX_CFG_4. */
        uint64_t cfg_rx_dll_locken_ovrrd_en : 1;/**< [  5:  5](R/W) When asserted, override DLL lock enable
                                                                 signal from the RX power state machine with
                                                                 CFG_RX_DLL_LOCKEN in register
                                                                 GSER()_LANE()_RX_CFG_1. */
        uint64_t reserved_6            : 1;
        uint64_t cfg_rx_eq_eval_ovrrd_en : 1;/**< [  7:  7](R/W) Override enable for RX-EQ eval.
                                                                 When asserted, training mode is controlled by
                                                                 CFG_RX_EQ_EVAL_OVRRD_VAL. */
        uint64_t cfg_rx_eq_eval_ovrrd_val : 1;/**< [  8:  8](R/W) Training mode control in override mode. */
        uint64_t cfg_rx_cdr_ctrl_ovrrd_en : 1;/**< [  9:  9](R/W) Not supported. */
        uint64_t cfg_rx_eie_det_ovrrd_en : 1;/**< [ 10: 10](R/W) Override enable for RX electrical-idle-exit
                                                                 detect enable. */
        uint64_t cfg_rx_eie_det_ovrrd_val : 1;/**< [ 11: 11](R/W) Override value for RX electrical-idle-exit
                                                                 detect enable. */
        uint64_t cfg_rx_oob_clk_en_ovrrd_en : 1;/**< [ 12: 12](R/W) Override enable for RX OOB clock enable. */
        uint64_t cfg_rx_oob_clk_en_ovrrd_val : 1;/**< [ 13: 13](R/W) Override value for RX OOB clock enable. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    struct bdk_gserx_lanex_rx_misc_ovrrd_cn88xxp1
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t cfg_rx_oob_clk_en_ovrrd_val : 1;/**< [ 13: 13](R/W) Override value for RX OOB clock enable. */
        uint64_t cfg_rx_oob_clk_en_ovrrd_en : 1;/**< [ 12: 12](R/W) Override enable for RX OOB clock enable. */
        uint64_t cfg_rx_eie_det_ovrrd_val : 1;/**< [ 11: 11](R/W) Override value for RX electrical-idle-exit
                                                                 detect enable. */
        uint64_t cfg_rx_eie_det_ovrrd_en : 1;/**< [ 10: 10](R/W) Override enable for RX electrical-idle-exit
                                                                 detect enable. */
        uint64_t cfg_rx_cdr_ctrl_ovrrd_en : 1;/**< [  9:  9](R/W) Not supported. */
        uint64_t cfg_rx_eq_eval_ovrrd_val : 1;/**< [  8:  8](R/W) Training mode control in override mode. */
        uint64_t cfg_rx_eq_eval_ovrrd_en : 1;/**< [  7:  7](R/W) Override enable for RX-EQ eval.
                                                                 When asserted, training mode is controlled by
                                                                 CFG_RX_EQ_EVAL_OVRRD_VAL. */
        uint64_t reserved_6            : 1;
        uint64_t cfg_rx_dll_locken_ovrrd_en : 1;/**< [  5:  5](R/W) When asserted, override DLL lock enable
                                                                 signal from the RX power state machine with
                                                                 CFG_RX_DLL_LOCKEN in register
                                                                 GSER()_LANE()_RX_CFG_1. */
        uint64_t cfg_rx_errdet_ctrl_ovrrd_en : 1;/**< [  4:  4](R/W) When asserted, pcs_sds_rx_err_det_ctrl is set
                                                                 to cfg_rx_errdet_ctrl in registers
                                                                 GSER()_LANE()_RX_CFG_3 and GSER()_LANE()_RX_CFG_4. */
        uint64_t reserved_0_3          : 4;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0_3          : 4;
        uint64_t cfg_rx_errdet_ctrl_ovrrd_en : 1;/**< [  4:  4](R/W) When asserted, pcs_sds_rx_err_det_ctrl is set
                                                                 to cfg_rx_errdet_ctrl in registers
                                                                 GSER()_LANE()_RX_CFG_3 and GSER()_LANE()_RX_CFG_4. */
        uint64_t cfg_rx_dll_locken_ovrrd_en : 1;/**< [  5:  5](R/W) When asserted, override DLL lock enable
                                                                 signal from the RX power state machine with
                                                                 CFG_RX_DLL_LOCKEN in register
                                                                 GSER()_LANE()_RX_CFG_1. */
        uint64_t reserved_6            : 1;
        uint64_t cfg_rx_eq_eval_ovrrd_en : 1;/**< [  7:  7](R/W) Override enable for RX-EQ eval.
                                                                 When asserted, training mode is controlled by
                                                                 CFG_RX_EQ_EVAL_OVRRD_VAL. */
        uint64_t cfg_rx_eq_eval_ovrrd_val : 1;/**< [  8:  8](R/W) Training mode control in override mode. */
        uint64_t cfg_rx_cdr_ctrl_ovrrd_en : 1;/**< [  9:  9](R/W) Not supported. */
        uint64_t cfg_rx_eie_det_ovrrd_en : 1;/**< [ 10: 10](R/W) Override enable for RX electrical-idle-exit
                                                                 detect enable. */
        uint64_t cfg_rx_eie_det_ovrrd_val : 1;/**< [ 11: 11](R/W) Override value for RX electrical-idle-exit
                                                                 detect enable. */
        uint64_t cfg_rx_oob_clk_en_ovrrd_en : 1;/**< [ 12: 12](R/W) Override enable for RX OOB clock enable. */
        uint64_t cfg_rx_oob_clk_en_ovrrd_val : 1;/**< [ 13: 13](R/W) Override value for RX OOB clock enable. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } cn88xxp1;
    /* struct bdk_gserx_lanex_rx_misc_ovrrd_s cn81xx; */
    /* struct bdk_gserx_lanex_rx_misc_ovrrd_s cn83xx; */
    /* struct bdk_gserx_lanex_rx_misc_ovrrd_s cn88xxp2; */
};
typedef union bdk_gserx_lanex_rx_misc_ovrrd bdk_gserx_lanex_rx_misc_ovrrd_t;

static inline uint64_t BDK_GSERX_LANEX_RX_MISC_OVRRD(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_MISC_OVRRD(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440258ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440258ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440258ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_MISC_OVRRD", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_MISC_OVRRD(a,b) bdk_gserx_lanex_rx_misc_ovrrd_t
#define bustype_BDK_GSERX_LANEX_RX_MISC_OVRRD(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_MISC_OVRRD(a,b) "GSERX_LANEX_RX_MISC_OVRRD"
#define device_bar_BDK_GSERX_LANEX_RX_MISC_OVRRD(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_MISC_OVRRD(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_MISC_OVRRD(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_os_mvalbbd_1
 *
 * GSER Lane SerDes RX Offset Calibration Manual Control 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_os_mvalbbd_1
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_os_mvalbbd_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t pcs_sds_rx_os_mval    : 16; /**< [ 15:  0](R/W/H) Offset calibration override value when GSER()_LANE()_RX_CFG_1[PCS_SDS_RX_OS_MEN] is set.
                                                                 Requires SIGN-MAG format.
                                                                 \<15:14\> = Not used.
                                                                 \<13:8\> = Qerr0.
                                                                 \<7:2\> = I.
                                                                 \<3:0\> = Ib. */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_rx_os_mval    : 16; /**< [ 15:  0](R/W/H) Offset calibration override value when GSER()_LANE()_RX_CFG_1[PCS_SDS_RX_OS_MEN] is set.
                                                                 Requires SIGN-MAG format.
                                                                 \<15:14\> = Not used.
                                                                 \<13:8\> = Qerr0.
                                                                 \<7:2\> = I.
                                                                 \<3:0\> = Ib. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_os_mvalbbd_1_s cn; */
};
typedef union bdk_gserx_lanex_rx_os_mvalbbd_1 bdk_gserx_lanex_rx_os_mvalbbd_1_t;

static inline uint64_t BDK_GSERX_LANEX_RX_OS_MVALBBD_1(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_OS_MVALBBD_1(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440230ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440230ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440230ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_OS_MVALBBD_1", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_OS_MVALBBD_1(a,b) bdk_gserx_lanex_rx_os_mvalbbd_1_t
#define bustype_BDK_GSERX_LANEX_RX_OS_MVALBBD_1(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_OS_MVALBBD_1(a,b) "GSERX_LANEX_RX_OS_MVALBBD_1"
#define device_bar_BDK_GSERX_LANEX_RX_OS_MVALBBD_1(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_OS_MVALBBD_1(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_OS_MVALBBD_1(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_os_mvalbbd_2
 *
 * GSER Lane SerDes RX Offset Calibration Manual Control 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_os_mvalbbd_2
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_os_mvalbbd_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t pcs_sds_rx_os_mval    : 16; /**< [ 15:  0](R/W/H) Offset calibration override value when GSER()_LANE()_RX_CFG_1[PCS_SDS_RX_OS_MEN] is set.
                                                                 Requires SIGN-MAG format.
                                                                 \<15:12\> = Ib.
                                                                 \<11:6\> = Q.
                                                                 \<5:0\> = Qb. */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_rx_os_mval    : 16; /**< [ 15:  0](R/W/H) Offset calibration override value when GSER()_LANE()_RX_CFG_1[PCS_SDS_RX_OS_MEN] is set.
                                                                 Requires SIGN-MAG format.
                                                                 \<15:12\> = Ib.
                                                                 \<11:6\> = Q.
                                                                 \<5:0\> = Qb. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_os_mvalbbd_2_s cn; */
};
typedef union bdk_gserx_lanex_rx_os_mvalbbd_2 bdk_gserx_lanex_rx_os_mvalbbd_2_t;

static inline uint64_t BDK_GSERX_LANEX_RX_OS_MVALBBD_2(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_OS_MVALBBD_2(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440238ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440238ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440238ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_OS_MVALBBD_2", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_OS_MVALBBD_2(a,b) bdk_gserx_lanex_rx_os_mvalbbd_2_t
#define bustype_BDK_GSERX_LANEX_RX_OS_MVALBBD_2(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_OS_MVALBBD_2(a,b) "GSERX_LANEX_RX_OS_MVALBBD_2"
#define device_bar_BDK_GSERX_LANEX_RX_OS_MVALBBD_2(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_OS_MVALBBD_2(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_OS_MVALBBD_2(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_os_out_1
 *
 * GSER Lane SerDes RX Calibration Status 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_os_out_1
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_os_out_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t sds_pcs_rx_os_out     : 12; /**< [ 11:  0](RO/H) Offset calibration code for readout, 2's complement.
                                                                 \<11:6\> = Not used.
                                                                 \<5:0\> = Qerr0. */
#else /* Word 0 - Little Endian */
        uint64_t sds_pcs_rx_os_out     : 12; /**< [ 11:  0](RO/H) Offset calibration code for readout, 2's complement.
                                                                 \<11:6\> = Not used.
                                                                 \<5:0\> = Qerr0. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_os_out_1_s cn; */
};
typedef union bdk_gserx_lanex_rx_os_out_1 bdk_gserx_lanex_rx_os_out_1_t;

static inline uint64_t BDK_GSERX_LANEX_RX_OS_OUT_1(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_OS_OUT_1(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904402a0ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904402a0ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904402a0ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_OS_OUT_1", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_OS_OUT_1(a,b) bdk_gserx_lanex_rx_os_out_1_t
#define bustype_BDK_GSERX_LANEX_RX_OS_OUT_1(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_OS_OUT_1(a,b) "GSERX_LANEX_RX_OS_OUT_1"
#define device_bar_BDK_GSERX_LANEX_RX_OS_OUT_1(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_OS_OUT_1(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_OS_OUT_1(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_os_out_2
 *
 * GSER Lane SerDes RX Calibration Status 2 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_os_out_2
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_os_out_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t sds_pcs_rx_os_out     : 12; /**< [ 11:  0](RO/H) Offset calibration code for readout, 2's complement.
                                                                 \<11:6\> = I.
                                                                 \<5:0\> = Ib. */
#else /* Word 0 - Little Endian */
        uint64_t sds_pcs_rx_os_out     : 12; /**< [ 11:  0](RO/H) Offset calibration code for readout, 2's complement.
                                                                 \<11:6\> = I.
                                                                 \<5:0\> = Ib. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_os_out_2_s cn; */
};
typedef union bdk_gserx_lanex_rx_os_out_2 bdk_gserx_lanex_rx_os_out_2_t;

static inline uint64_t BDK_GSERX_LANEX_RX_OS_OUT_2(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_OS_OUT_2(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904402a8ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904402a8ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904402a8ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_OS_OUT_2", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_OS_OUT_2(a,b) bdk_gserx_lanex_rx_os_out_2_t
#define bustype_BDK_GSERX_LANEX_RX_OS_OUT_2(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_OS_OUT_2(a,b) "GSERX_LANEX_RX_OS_OUT_2"
#define device_bar_BDK_GSERX_LANEX_RX_OS_OUT_2(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_OS_OUT_2(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_OS_OUT_2(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_os_out_3
 *
 * GSER Lane SerDes RX Calibration Status 3 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_os_out_3
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_os_out_3_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t sds_pcs_rx_os_out     : 12; /**< [ 11:  0](RO/H) Offset calibration code for readout, 2's complement.
                                                                 \<11:6\> = Q.
                                                                 \<5:0\> = Qb. */
#else /* Word 0 - Little Endian */
        uint64_t sds_pcs_rx_os_out     : 12; /**< [ 11:  0](RO/H) Offset calibration code for readout, 2's complement.
                                                                 \<11:6\> = Q.
                                                                 \<5:0\> = Qb. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_os_out_3_s cn; */
};
typedef union bdk_gserx_lanex_rx_os_out_3 bdk_gserx_lanex_rx_os_out_3_t;

static inline uint64_t BDK_GSERX_LANEX_RX_OS_OUT_3(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_OS_OUT_3(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904402b0ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904402b0ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904402b0ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_OS_OUT_3", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_OS_OUT_3(a,b) bdk_gserx_lanex_rx_os_out_3_t
#define bustype_BDK_GSERX_LANEX_RX_OS_OUT_3(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_OS_OUT_3(a,b) "GSERX_LANEX_RX_OS_OUT_3"
#define device_bar_BDK_GSERX_LANEX_RX_OS_OUT_3(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_OS_OUT_3(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_OS_OUT_3(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_precorr_ctrl
 *
 * GSER Lane RX Precorrelation Control Register
 * These are the RAW PCS per-lane RX precorrelation control registers. These registers are for
 * diagnostic use only.
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_precorr_ctrl
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_precorr_ctrl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_5_63         : 59;
        uint64_t rx_precorr_disable    : 1;  /**< [  4:  4](R/W) Disable RX precorrelation calculation. */
        uint64_t rx_precorr_en_ovrrd_en : 1; /**< [  3:  3](R/W) Override enable for RX precorrelation calculation enable. */
        uint64_t rx_precorr_en_ovrrd_val : 1;/**< [  2:  2](R/W) Override value for RX precorrelation calculation enable. */
        uint64_t pcs_sds_rx_precorr_scnt_ctrl : 2;/**< [  1:  0](R/W) RX precorrelation sample counter control.
                                                                 0x0 = Load max sample counter with 0x1FF.
                                                                 0x1 = Load max sample counter with 0x3FF.
                                                                 0x2 = Load max sample counter with 0x7FF.
                                                                 0x3 = Load max sample counter with 0xFFF. */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_rx_precorr_scnt_ctrl : 2;/**< [  1:  0](R/W) RX precorrelation sample counter control.
                                                                 0x0 = Load max sample counter with 0x1FF.
                                                                 0x1 = Load max sample counter with 0x3FF.
                                                                 0x2 = Load max sample counter with 0x7FF.
                                                                 0x3 = Load max sample counter with 0xFFF. */
        uint64_t rx_precorr_en_ovrrd_val : 1;/**< [  2:  2](R/W) Override value for RX precorrelation calculation enable. */
        uint64_t rx_precorr_en_ovrrd_en : 1; /**< [  3:  3](R/W) Override enable for RX precorrelation calculation enable. */
        uint64_t rx_precorr_disable    : 1;  /**< [  4:  4](R/W) Disable RX precorrelation calculation. */
        uint64_t reserved_5_63         : 59;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_precorr_ctrl_s cn; */
};
typedef union bdk_gserx_lanex_rx_precorr_ctrl bdk_gserx_lanex_rx_precorr_ctrl_t;

static inline uint64_t BDK_GSERX_LANEX_RX_PRECORR_CTRL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_PRECORR_CTRL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440060ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440060ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440060ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_PRECORR_CTRL", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_PRECORR_CTRL(a,b) bdk_gserx_lanex_rx_precorr_ctrl_t
#define bustype_BDK_GSERX_LANEX_RX_PRECORR_CTRL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_PRECORR_CTRL(a,b) "GSERX_LANEX_RX_PRECORR_CTRL"
#define device_bar_BDK_GSERX_LANEX_RX_PRECORR_CTRL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_PRECORR_CTRL(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_PRECORR_CTRL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_precorr_val
 *
 * GSER Lane RX Precorrelation Count Register
 * These are the RAW PCS per-lane RX precorrelation control registers. These registers are for
 * diagnostic use only.
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_precorr_val
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_precorr_val_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_13_63        : 51;
        uint64_t sds_pcs_rx_precorr_vld : 1; /**< [ 12: 12](RO/H) RX precorrelation count is valid. */
        uint64_t sds_pcs_rx_precorr_cnt : 12;/**< [ 11:  0](RO/H) RX precorrelation count. */
#else /* Word 0 - Little Endian */
        uint64_t sds_pcs_rx_precorr_cnt : 12;/**< [ 11:  0](RO/H) RX precorrelation count. */
        uint64_t sds_pcs_rx_precorr_vld : 1; /**< [ 12: 12](RO/H) RX precorrelation count is valid. */
        uint64_t reserved_13_63        : 51;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_precorr_val_s cn; */
};
typedef union bdk_gserx_lanex_rx_precorr_val bdk_gserx_lanex_rx_precorr_val_t;

static inline uint64_t BDK_GSERX_LANEX_RX_PRECORR_VAL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_PRECORR_VAL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440078ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440078ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440078ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_PRECORR_VAL", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_PRECORR_VAL(a,b) bdk_gserx_lanex_rx_precorr_val_t
#define bustype_BDK_GSERX_LANEX_RX_PRECORR_VAL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_PRECORR_VAL(a,b) "GSERX_LANEX_RX_PRECORR_VAL"
#define device_bar_BDK_GSERX_LANEX_RX_PRECORR_VAL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_PRECORR_VAL(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_PRECORR_VAL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_valbbd_ctrl_0
 *
 * GSER Lane RX Adaptive Equalizer Control Register 0
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_valbbd_ctrl_0
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_valbbd_ctrl_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t agc_gain              : 2;  /**< [ 13: 12](R/W) AGC gain. */
        uint64_t dfe_gain              : 2;  /**< [ 11: 10](R/W) DFE gain. */
        uint64_t dfe_c5_mval           : 4;  /**< [  9:  6](R/W) DFE Tap5 manual value when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 [DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,DFE_C4_MSGN], and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c5_msgn           : 1;  /**< [  5:  5](R/W) DFE Tap5 manual sign when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 [DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,DFE_C4_MSGN], and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c4_mval           : 4;  /**< [  4:  1](R/W) DFE Tap4 manual value when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 [DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,DFE_C4_MSGN], and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c4_msgn           : 1;  /**< [  0:  0](R/W) DFE Tap4 manual sign when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 [DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,DFE_C4_MSGN], and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
#else /* Word 0 - Little Endian */
        uint64_t dfe_c4_msgn           : 1;  /**< [  0:  0](R/W) DFE Tap4 manual sign when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 [DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,DFE_C4_MSGN], and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c4_mval           : 4;  /**< [  4:  1](R/W) DFE Tap4 manual value when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 [DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,DFE_C4_MSGN], and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c5_msgn           : 1;  /**< [  5:  5](R/W) DFE Tap5 manual sign when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 [DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,DFE_C4_MSGN], and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c5_mval           : 4;  /**< [  9:  6](R/W) DFE Tap5 manual value when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 [DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,DFE_C4_MSGN], and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_gain              : 2;  /**< [ 11: 10](R/W) DFE gain. */
        uint64_t agc_gain              : 2;  /**< [ 13: 12](R/W) AGC gain. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_valbbd_ctrl_0_s cn; */
};
typedef union bdk_gserx_lanex_rx_valbbd_ctrl_0 bdk_gserx_lanex_rx_valbbd_ctrl_0_t;

static inline uint64_t BDK_GSERX_LANEX_RX_VALBBD_CTRL_0(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_VALBBD_CTRL_0(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440240ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440240ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440240ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_VALBBD_CTRL_0", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_VALBBD_CTRL_0(a,b) bdk_gserx_lanex_rx_valbbd_ctrl_0_t
#define bustype_BDK_GSERX_LANEX_RX_VALBBD_CTRL_0(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_VALBBD_CTRL_0(a,b) "GSERX_LANEX_RX_VALBBD_CTRL_0"
#define device_bar_BDK_GSERX_LANEX_RX_VALBBD_CTRL_0(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_VALBBD_CTRL_0(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_VALBBD_CTRL_0(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_valbbd_ctrl_1
 *
 * GSER Lane RX Adaptive Equalizer Control Register 1
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_valbbd_ctrl_1
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_valbbd_ctrl_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t dfe_c3_mval           : 4;  /**< [ 14: 11](R/W) DFE Tap3 manual value when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,FE_C4_MSGN],
                                                                 and clearing all of [DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c3_msgn           : 1;  /**< [ 10: 10](R/W) DFE Tap3 manual sign when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,FE_C4_MSGN],
                                                                 and clearing all of [DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c2_mval           : 4;  /**< [  9:  6](R/W) DFE Tap2 manual value when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,FE_C4_MSGN],
                                                                 and clearing all of [DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c2_msgn           : 1;  /**< [  5:  5](R/W) DFE Tap2 manual sign when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,FE_C4_MSGN],
                                                                 and clearing all of [DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c1_mval           : 4;  /**< [  4:  1](R/W) DFE Tap1 manual value when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,FE_C4_MSGN],
                                                                 and clearing all of [DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c1_msgn           : 1;  /**< [  0:  0](R/W) DFE Tap1 manual sign when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,FE_C4_MSGN],
                                                                 and clearing all of [DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
#else /* Word 0 - Little Endian */
        uint64_t dfe_c1_msgn           : 1;  /**< [  0:  0](R/W) DFE Tap1 manual sign when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,FE_C4_MSGN],
                                                                 and clearing all of [DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c1_mval           : 4;  /**< [  4:  1](R/W) DFE Tap1 manual value when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,FE_C4_MSGN],
                                                                 and clearing all of [DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c2_msgn           : 1;  /**< [  5:  5](R/W) DFE Tap2 manual sign when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,FE_C4_MSGN],
                                                                 and clearing all of [DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c2_mval           : 4;  /**< [  9:  6](R/W) DFE Tap2 manual value when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,FE_C4_MSGN],
                                                                 and clearing all of [DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c3_msgn           : 1;  /**< [ 10: 10](R/W) DFE Tap3 manual sign when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,FE_C4_MSGN],
                                                                 and clearing all of [DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c3_mval           : 4;  /**< [ 14: 11](R/W) DFE Tap3 manual value when GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN] and
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_C5_OVRD_VAL] are both set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_2[DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,
                                                                 DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL,FE_C4_MSGN],
                                                                 and clearing all of [DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_valbbd_ctrl_1_s cn; */
};
typedef union bdk_gserx_lanex_rx_valbbd_ctrl_1 bdk_gserx_lanex_rx_valbbd_ctrl_1_t;

static inline uint64_t BDK_GSERX_LANEX_RX_VALBBD_CTRL_1(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_VALBBD_CTRL_1(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440248ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440248ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440248ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_VALBBD_CTRL_1", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_VALBBD_CTRL_1(a,b) bdk_gserx_lanex_rx_valbbd_ctrl_1_t
#define bustype_BDK_GSERX_LANEX_RX_VALBBD_CTRL_1(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_VALBBD_CTRL_1(a,b) "GSERX_LANEX_RX_VALBBD_CTRL_1"
#define device_bar_BDK_GSERX_LANEX_RX_VALBBD_CTRL_1(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_VALBBD_CTRL_1(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_VALBBD_CTRL_1(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_valbbd_ctrl_2
 *
 * GSER Lane RX Adaptive Equalizer Control Register 2
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_valbbd_ctrl_2
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_valbbd_ctrl_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_6_63         : 58;
        uint64_t dfe_ovrd_en           : 1;  /**< [  5:  5](R/W) Override enable for DFE tap controls. When asserted, the register bits in
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0 and GSER()_LANE()_RX_VALBBD_CTRL_1 are
                                                                 used for controlling the DFE tap manual mode, instead the manual mode signal indexed by
                                                                 GSER()_LANE_MODE[LMODE].

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 [DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,
                                                                 DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL, FE_C4_MSGN],
                                                                 and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c5_ovrd_val       : 1;  /**< [  4:  4](R/W) Override value for DFE Tap5 manual enable. Used when [DFE_OVRD_EN] is set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 [DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,
                                                                 DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL, FE_C4_MSGN],
                                                                 and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c4_ovrd_val       : 1;  /**< [  3:  3](R/W) Override value for DFE Tap4 manual enable. Used when [DFE_OVRD_EN] is set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 [DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,
                                                                 DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL, FE_C4_MSGN],
                                                                 and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c3_ovrd_val       : 1;  /**< [  2:  2](R/W) Override value for DFE Tap3 manual enable. Used when [DFE_OVRD_EN] is set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 [DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,
                                                                 DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL, FE_C4_MSGN],
                                                                 and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c2_ovrd_val       : 1;  /**< [  1:  1](R/W) Override value for DFE Tap2 manual enable. Used when [DFE_OVRD_EN] is set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 [DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,
                                                                 DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL, FE_C4_MSGN],
                                                                 and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c1_ovrd_val       : 1;  /**< [  0:  0](R/W) Override value for DFE Tap1 manual enable. Used when [DFE_OVRD_EN] is set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 [DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,
                                                                 DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL, FE_C4_MSGN],
                                                                 and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
#else /* Word 0 - Little Endian */
        uint64_t dfe_c1_ovrd_val       : 1;  /**< [  0:  0](R/W) Override value for DFE Tap1 manual enable. Used when [DFE_OVRD_EN] is set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 [DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,
                                                                 DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL, FE_C4_MSGN],
                                                                 and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c2_ovrd_val       : 1;  /**< [  1:  1](R/W) Override value for DFE Tap2 manual enable. Used when [DFE_OVRD_EN] is set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 [DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,
                                                                 DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL, FE_C4_MSGN],
                                                                 and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c3_ovrd_val       : 1;  /**< [  2:  2](R/W) Override value for DFE Tap3 manual enable. Used when [DFE_OVRD_EN] is set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 [DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,
                                                                 DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL, FE_C4_MSGN],
                                                                 and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c4_ovrd_val       : 1;  /**< [  3:  3](R/W) Override value for DFE Tap4 manual enable. Used when [DFE_OVRD_EN] is set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 [DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,
                                                                 DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL, FE_C4_MSGN],
                                                                 and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_c5_ovrd_val       : 1;  /**< [  4:  4](R/W) Override value for DFE Tap5 manual enable. Used when [DFE_OVRD_EN] is set.

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 [DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,
                                                                 DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL, FE_C4_MSGN],
                                                                 and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t dfe_ovrd_en           : 1;  /**< [  5:  5](R/W) Override enable for DFE tap controls. When asserted, the register bits in
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0 and GSER()_LANE()_RX_VALBBD_CTRL_1 are
                                                                 used for controlling the DFE tap manual mode, instead the manual mode signal indexed by
                                                                 GSER()_LANE_MODE[LMODE].

                                                                 Recommended settings:

                                                                 When auto-negotiated link training is not present (e.g. BGX) and link speed \<=
                                                                 5 Gbaud, the DFE should be completely disabled by setting all of
                                                                 [DFE_OVRD_EN,DFE_C5_OVRD_VAL,DFE_C4_OVRD_VAL,DFE_C3_OVRD_VAL,DFE_C2_OVRD_VAL,
                                                                 DFE_C1_OVRD_VAL], setting
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<8\>], clearing
                                                                 GSER()_LANE()_RX_LOOP_CTRL[CFG_RX_LCTRL\<1\>], clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_C5_MVAL,DFE_C5_MSGN,DFE_C4_MVAL, FE_C4_MSGN],
                                                                 and clearing all of
                                                                 GSER()_LANE()_RX_VALBBD_CTRL_1[DFE_C3_MVAL,DFE_C3_MSGN,DFE_C2_MVAL,DFE_C2_MSGN,
                                                                 DFE_C1_MVAL,DFE_C1_MSGN]. */
        uint64_t reserved_6_63         : 58;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_valbbd_ctrl_2_s cn; */
};
typedef union bdk_gserx_lanex_rx_valbbd_ctrl_2 bdk_gserx_lanex_rx_valbbd_ctrl_2_t;

static inline uint64_t BDK_GSERX_LANEX_RX_VALBBD_CTRL_2(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_VALBBD_CTRL_2(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440250ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440250ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440250ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_VALBBD_CTRL_2", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_VALBBD_CTRL_2(a,b) bdk_gserx_lanex_rx_valbbd_ctrl_2_t
#define bustype_BDK_GSERX_LANEX_RX_VALBBD_CTRL_2(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_VALBBD_CTRL_2(a,b) "GSERX_LANEX_RX_VALBBD_CTRL_2"
#define device_bar_BDK_GSERX_LANEX_RX_VALBBD_CTRL_2(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_VALBBD_CTRL_2(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_VALBBD_CTRL_2(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_vma_ctrl
 *
 * GSER Lane RX VMA Control Register
 * These are the RAW PCS per-lane RX VMA control registers. These registers are for diagnostic
 * use only.
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_vma_ctrl
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_vma_ctrl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t vma_fine_cfg_sel_ovrrd_en : 1;/**< [ 15: 15](R/W) Enable override of VMA fine configuration selection. */
        uint64_t vma_fine_cfg_sel_ovrrd_val : 1;/**< [ 14: 14](R/W) Override value of VMA fine configuration selection.
                                                                 0 = Coarse mode.
                                                                 1 = Fine mode. */
        uint64_t rx_fom_div_delta      : 1;  /**< [ 13: 13](R/W) TX figure of merit delta division-mode enable. */
        uint64_t rx_vna_ctrl_18_16     : 3;  /**< [ 12: 10](R/W) RX VMA loop control. */
        uint64_t rx_vna_ctrl_9_0       : 10; /**< [  9:  0](R/W) RX VMA loop control.
                                                                 \<9:8\> = Parameter settling wait time.
                                                                 \<7\> = Limit CTLE peak to max value.
                                                                 \<6\> = Long reach enabled.
                                                                 \<5\> = Short reach enabled.
                                                                 \<4\> = Training done override enable.
                                                                 \<3\> = Training done override value.
                                                                 \<2:0\> = VMA clock modulation. */
#else /* Word 0 - Little Endian */
        uint64_t rx_vna_ctrl_9_0       : 10; /**< [  9:  0](R/W) RX VMA loop control.
                                                                 \<9:8\> = Parameter settling wait time.
                                                                 \<7\> = Limit CTLE peak to max value.
                                                                 \<6\> = Long reach enabled.
                                                                 \<5\> = Short reach enabled.
                                                                 \<4\> = Training done override enable.
                                                                 \<3\> = Training done override value.
                                                                 \<2:0\> = VMA clock modulation. */
        uint64_t rx_vna_ctrl_18_16     : 3;  /**< [ 12: 10](R/W) RX VMA loop control. */
        uint64_t rx_fom_div_delta      : 1;  /**< [ 13: 13](R/W) TX figure of merit delta division-mode enable. */
        uint64_t vma_fine_cfg_sel_ovrrd_val : 1;/**< [ 14: 14](R/W) Override value of VMA fine configuration selection.
                                                                 0 = Coarse mode.
                                                                 1 = Fine mode. */
        uint64_t vma_fine_cfg_sel_ovrrd_en : 1;/**< [ 15: 15](R/W) Enable override of VMA fine configuration selection. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_vma_ctrl_s cn; */
};
typedef union bdk_gserx_lanex_rx_vma_ctrl bdk_gserx_lanex_rx_vma_ctrl_t;

static inline uint64_t BDK_GSERX_LANEX_RX_VMA_CTRL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_VMA_CTRL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440200ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440200ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440200ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_VMA_CTRL", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_VMA_CTRL(a,b) bdk_gserx_lanex_rx_vma_ctrl_t
#define bustype_BDK_GSERX_LANEX_RX_VMA_CTRL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_VMA_CTRL(a,b) "GSERX_LANEX_RX_VMA_CTRL"
#define device_bar_BDK_GSERX_LANEX_RX_VMA_CTRL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_VMA_CTRL(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_VMA_CTRL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_vma_status_0
 *
 * GSER Lane SerDes RX CDR Status 0 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_vma_status_0
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_vma_status_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t sds_pcs_rx_vma_status : 8;  /**< [  7:  0](RO/H) \<7\> = DFE powerdown.
                                                                 \<6\> = Reserved.
                                                                 \<5:2\> = CTLE Peak.
                                                                 \<1:0\> = CTLE Pole. */
#else /* Word 0 - Little Endian */
        uint64_t sds_pcs_rx_vma_status : 8;  /**< [  7:  0](RO/H) \<7\> = DFE powerdown.
                                                                 \<6\> = Reserved.
                                                                 \<5:2\> = CTLE Peak.
                                                                 \<1:0\> = CTLE Pole. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_vma_status_0_s cn; */
};
typedef union bdk_gserx_lanex_rx_vma_status_0 bdk_gserx_lanex_rx_vma_status_0_t;

static inline uint64_t BDK_GSERX_LANEX_RX_VMA_STATUS_0(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_VMA_STATUS_0(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904402b8ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904402b8ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904402b8ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_VMA_STATUS_0", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_VMA_STATUS_0(a,b) bdk_gserx_lanex_rx_vma_status_0_t
#define bustype_BDK_GSERX_LANEX_RX_VMA_STATUS_0(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_VMA_STATUS_0(a,b) "GSERX_LANEX_RX_VMA_STATUS_0"
#define device_bar_BDK_GSERX_LANEX_RX_VMA_STATUS_0(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_VMA_STATUS_0(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_VMA_STATUS_0(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_rx_vma_status_1
 *
 * GSER Lane SerDes RX CDR Status 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_rx_vma_status_1
{
    uint64_t u;
    struct bdk_gserx_lanex_rx_vma_status_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t sds_pcs_rx_vma_status : 16; /**< [ 15:  0](RO/H) \<15:8\>: Output is controlled by GSER()_LANE()_RX_CFG_4[CFG_RX_ERRDET_CTRL]\<6:5\>:
                                                                 0x0 = Window counter\<19:12\> (VMA RAW FOM).
                                                                 0x1 = Window counter\<11:4\>.
                                                                 0x2 = CTLE (continuous time linear equalizer) pole, SDLL_IQ.
                                                                 0x3 = Pre-CTLE gain, CTLE Peak.

                                                                 \<7\>: Training done.

                                                                 \<6:4\>: Internal state machine delta.

                                                                 \<3:0\>: Output is controlled by GSER()_LANE()_RX_CDR_CTRL_1[CDR phase offset override
                                                                 enable]\<4\>:
                                                                 0x0 = DLL IQ Training value.
                                                                 0x1 = CDR Phase Offset. */
#else /* Word 0 - Little Endian */
        uint64_t sds_pcs_rx_vma_status : 16; /**< [ 15:  0](RO/H) \<15:8\>: Output is controlled by GSER()_LANE()_RX_CFG_4[CFG_RX_ERRDET_CTRL]\<6:5\>:
                                                                 0x0 = Window counter\<19:12\> (VMA RAW FOM).
                                                                 0x1 = Window counter\<11:4\>.
                                                                 0x2 = CTLE (continuous time linear equalizer) pole, SDLL_IQ.
                                                                 0x3 = Pre-CTLE gain, CTLE Peak.

                                                                 \<7\>: Training done.

                                                                 \<6:4\>: Internal state machine delta.

                                                                 \<3:0\>: Output is controlled by GSER()_LANE()_RX_CDR_CTRL_1[CDR phase offset override
                                                                 enable]\<4\>:
                                                                 0x0 = DLL IQ Training value.
                                                                 0x1 = CDR Phase Offset. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_rx_vma_status_1_s cn; */
};
typedef union bdk_gserx_lanex_rx_vma_status_1 bdk_gserx_lanex_rx_vma_status_1_t;

static inline uint64_t BDK_GSERX_LANEX_RX_VMA_STATUS_1(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_RX_VMA_STATUS_1(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904402c0ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904402c0ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904402c0ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_RX_VMA_STATUS_1", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_RX_VMA_STATUS_1(a,b) bdk_gserx_lanex_rx_vma_status_1_t
#define bustype_BDK_GSERX_LANEX_RX_VMA_STATUS_1(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_RX_VMA_STATUS_1(a,b) "GSERX_LANEX_RX_VMA_STATUS_1"
#define device_bar_BDK_GSERX_LANEX_RX_VMA_STATUS_1(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_RX_VMA_STATUS_1(a,b) (a)
#define arguments_BDK_GSERX_LANEX_RX_VMA_STATUS_1(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_sds_pin_mon_0
 *
 * GSER Lane SerDes Pin Monitor 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_sds_pin_mon_0
{
    uint64_t u;
    struct bdk_gserx_lanex_sds_pin_mon_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t pcs_sds_tx_widthsel   : 2;  /**< [  9:  8](RO/H) TX parallel interface width settings (RAW PCS to
                                                                 SerDes TX).
                                                                 0x0 = 8-bit raw data (not supported).
                                                                 0x1 = 10-bit raw data (not supported).
                                                                 0x2 = 16-bit raw data (not supported).
                                                                 0x3 = 20-bit raw data. */
        uint64_t pcs_sds_rx_pcie_mode  : 1;  /**< [  7:  7](RO/H) Selects between RX terminations:
                                                                 0x0 = pcs_sds_rx_terminate_to_vdda.
                                                                 0x1 = VSS. */
        uint64_t reserved_5_6          : 2;
        uint64_t pcs_sds_rx_misc_ctrl_5 : 1; /**< [  4:  4](RO/H) Not used. */
        uint64_t tx_detrx_state        : 2;  /**< [  3:  2](RO/H) RX detection state:
                                                                 0x0 = IDLE.
                                                                 0x1 = Charge Up.
                                                                 0x2 = Detection.
                                                                 0x3 = Restore common mode. */
        uint64_t pcs_sds_tx_rx_detect_dis : 1;/**< [  1:  1](RO/H) TX detect RX, mode disable. */
        uint64_t pcs_sds_tx_detect_pulsen : 1;/**< [  0:  0](RO/H) TX detect RX, pulse enable. */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_tx_detect_pulsen : 1;/**< [  0:  0](RO/H) TX detect RX, pulse enable. */
        uint64_t pcs_sds_tx_rx_detect_dis : 1;/**< [  1:  1](RO/H) TX detect RX, mode disable. */
        uint64_t tx_detrx_state        : 2;  /**< [  3:  2](RO/H) RX detection state:
                                                                 0x0 = IDLE.
                                                                 0x1 = Charge Up.
                                                                 0x2 = Detection.
                                                                 0x3 = Restore common mode. */
        uint64_t pcs_sds_rx_misc_ctrl_5 : 1; /**< [  4:  4](RO/H) Not used. */
        uint64_t reserved_5_6          : 2;
        uint64_t pcs_sds_rx_pcie_mode  : 1;  /**< [  7:  7](RO/H) Selects between RX terminations:
                                                                 0x0 = pcs_sds_rx_terminate_to_vdda.
                                                                 0x1 = VSS. */
        uint64_t pcs_sds_tx_widthsel   : 2;  /**< [  9:  8](RO/H) TX parallel interface width settings (RAW PCS to
                                                                 SerDes TX).
                                                                 0x0 = 8-bit raw data (not supported).
                                                                 0x1 = 10-bit raw data (not supported).
                                                                 0x2 = 16-bit raw data (not supported).
                                                                 0x3 = 20-bit raw data. */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_sds_pin_mon_0_s cn; */
};
typedef union bdk_gserx_lanex_sds_pin_mon_0 bdk_gserx_lanex_sds_pin_mon_0_t;

static inline uint64_t BDK_GSERX_LANEX_SDS_PIN_MON_0(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_SDS_PIN_MON_0(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440130ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440130ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440130ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_SDS_PIN_MON_0", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_SDS_PIN_MON_0(a,b) bdk_gserx_lanex_sds_pin_mon_0_t
#define bustype_BDK_GSERX_LANEX_SDS_PIN_MON_0(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_SDS_PIN_MON_0(a,b) "GSERX_LANEX_SDS_PIN_MON_0"
#define device_bar_BDK_GSERX_LANEX_SDS_PIN_MON_0(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_SDS_PIN_MON_0(a,b) (a)
#define arguments_BDK_GSERX_LANEX_SDS_PIN_MON_0(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_sds_pin_mon_1
 *
 * GSER Lane SerDes Pin Monitor 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_sds_pin_mon_1
{
    uint64_t u;
    struct bdk_gserx_lanex_sds_pin_mon_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t pcs_sds_rx_chpd       : 1;  /**< [ 15: 15](RO/H) RX channel powerdown signal. */
        uint64_t pcs_sds_rx_eie_en     : 1;  /**< [ 14: 14](RO/H) Enable for electrical idle detection circuit
                                                                 in SerDes RX. */
        uint64_t reserved_13           : 1;
        uint64_t pcs_sds_ln_loopback_mode : 1;/**< [ 12: 12](RO/H) TX to RX on chip loopback control signal. */
        uint64_t pcs_sds_tx_chpd       : 1;  /**< [ 11: 11](RO/H) TX channel powerdown signal. */
        uint64_t pcs_sds_rx_widthsel   : 2;  /**< [ 10:  9](RO/H) Width select.
                                                                 0x0 = 8-bit raw data.
                                                                 0x1 = 10-bit raw data.
                                                                 0x2 = 16-bit raw data.
                                                                 0x3 = 20-bit raw data. */
        uint64_t reserved_8            : 1;
        uint64_t pcs_sds_tx_resetn     : 1;  /**< [  7:  7](RO/H) TX reset, active low (RAW PCS output to lane TX). */
        uint64_t pcs_sds_tx_tristate_en : 1; /**< [  6:  6](RO/H) TX driver tristate enable (RAW PCS output to lane TX). */
        uint64_t pcs_sds_tx_swing      : 5;  /**< [  5:  1](RO/H) TX swing (RAW PCS output to lane TX). */
        uint64_t pcs_sds_tx_elec_idle  : 1;  /**< [  0:  0](RO/H) TX electrical idle control (RAW PCS output to lane TX). */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_tx_elec_idle  : 1;  /**< [  0:  0](RO/H) TX electrical idle control (RAW PCS output to lane TX). */
        uint64_t pcs_sds_tx_swing      : 5;  /**< [  5:  1](RO/H) TX swing (RAW PCS output to lane TX). */
        uint64_t pcs_sds_tx_tristate_en : 1; /**< [  6:  6](RO/H) TX driver tristate enable (RAW PCS output to lane TX). */
        uint64_t pcs_sds_tx_resetn     : 1;  /**< [  7:  7](RO/H) TX reset, active low (RAW PCS output to lane TX). */
        uint64_t reserved_8            : 1;
        uint64_t pcs_sds_rx_widthsel   : 2;  /**< [ 10:  9](RO/H) Width select.
                                                                 0x0 = 8-bit raw data.
                                                                 0x1 = 10-bit raw data.
                                                                 0x2 = 16-bit raw data.
                                                                 0x3 = 20-bit raw data. */
        uint64_t pcs_sds_tx_chpd       : 1;  /**< [ 11: 11](RO/H) TX channel powerdown signal. */
        uint64_t pcs_sds_ln_loopback_mode : 1;/**< [ 12: 12](RO/H) TX to RX on chip loopback control signal. */
        uint64_t reserved_13           : 1;
        uint64_t pcs_sds_rx_eie_en     : 1;  /**< [ 14: 14](RO/H) Enable for electrical idle detection circuit
                                                                 in SerDes RX. */
        uint64_t pcs_sds_rx_chpd       : 1;  /**< [ 15: 15](RO/H) RX channel powerdown signal. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_sds_pin_mon_1_s cn; */
};
typedef union bdk_gserx_lanex_sds_pin_mon_1 bdk_gserx_lanex_sds_pin_mon_1_t;

static inline uint64_t BDK_GSERX_LANEX_SDS_PIN_MON_1(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_SDS_PIN_MON_1(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440138ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440138ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440138ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_SDS_PIN_MON_1", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_SDS_PIN_MON_1(a,b) bdk_gserx_lanex_sds_pin_mon_1_t
#define bustype_BDK_GSERX_LANEX_SDS_PIN_MON_1(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_SDS_PIN_MON_1(a,b) "GSERX_LANEX_SDS_PIN_MON_1"
#define device_bar_BDK_GSERX_LANEX_SDS_PIN_MON_1(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_SDS_PIN_MON_1(a,b) (a)
#define arguments_BDK_GSERX_LANEX_SDS_PIN_MON_1(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_sds_pin_mon_2
 *
 * GSER Lane SerDes Pin Monitor 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lanex_sds_pin_mon_2
{
    uint64_t u;
    struct bdk_gserx_lanex_sds_pin_mon_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_11_63        : 53;
        uint64_t pcs_sds_tx_vboost_en  : 1;  /**< [ 10: 10](RO/H) TX boost enable. */
        uint64_t pcs_sds_tx_turbos_en  : 1;  /**< [  9:  9](RO/H) TX turbo mode enable signal, increases swing of TX
                                                                 through current mode. */
        uint64_t pcs_sds_premptap      : 9;  /**< [  8:  0](RO/H) Preemphasis control.
                                                                 \<8:4\> = Postcursor.
                                                                 \<3:0\> = Precursor. */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_premptap      : 9;  /**< [  8:  0](RO/H) Preemphasis control.
                                                                 \<8:4\> = Postcursor.
                                                                 \<3:0\> = Precursor. */
        uint64_t pcs_sds_tx_turbos_en  : 1;  /**< [  9:  9](RO/H) TX turbo mode enable signal, increases swing of TX
                                                                 through current mode. */
        uint64_t pcs_sds_tx_vboost_en  : 1;  /**< [ 10: 10](RO/H) TX boost enable. */
        uint64_t reserved_11_63        : 53;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_sds_pin_mon_2_s cn; */
};
typedef union bdk_gserx_lanex_sds_pin_mon_2 bdk_gserx_lanex_sds_pin_mon_2_t;

static inline uint64_t BDK_GSERX_LANEX_SDS_PIN_MON_2(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_SDS_PIN_MON_2(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090440140ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e090440140ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e090440140ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_SDS_PIN_MON_2", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_SDS_PIN_MON_2(a,b) bdk_gserx_lanex_sds_pin_mon_2_t
#define bustype_BDK_GSERX_LANEX_SDS_PIN_MON_2(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_SDS_PIN_MON_2(a,b) "GSERX_LANEX_SDS_PIN_MON_2"
#define device_bar_BDK_GSERX_LANEX_SDS_PIN_MON_2(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_SDS_PIN_MON_2(a,b) (a)
#define arguments_BDK_GSERX_LANEX_SDS_PIN_MON_2(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_tx_cfg_0
 *
 * GSER Lane TX Configuration 0 Register
 * These registers are reset by hardware only during chip cold reset. The
 * values of the CSR fields in these registers do not change during chip
 * warm or soft resets.
 */
union bdk_gserx_lanex_tx_cfg_0
{
    uint64_t u;
    struct bdk_gserx_lanex_tx_cfg_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t tx_tristate_en_ovrrd_val : 1;/**< [ 15: 15](R/W) TX termination high-Z enable. Override value when
                                                                 GSER()_LANE()_PWR_CTRL[TX_TRISTATE_EN_OVRRD_EN] is set. */
        uint64_t tx_chpd_ovrrd_val     : 1;  /**< [ 14: 14](R/W) TX lane power down. Active high. Override value when
                                                                 GSER()_LANE()_PWR_CTRL[TX_PD_OVRRD_EN] is set. */
        uint64_t reserved_10_13        : 4;
        uint64_t tx_resetn_ovrrd_val   : 1;  /**< [  9:  9](R/W) TX P2S reset. Active high. Override value when
                                                                 GSER()_LANE()_PWR_CTRL[TX_P2S_RESET_OVRRD_EN] is set. */
        uint64_t tx_cm_mode            : 1;  /**< [  8:  8](R/W/H) Assert to enable fast common-mode charge up. For simulation purposes only. */
        uint64_t cfg_tx_swing          : 5;  /**< [  7:  3](R/W) TX output swing control.
                                                                 Default swing encoding when GSER()_LANE()_TX_CFG_1[TX_SWING_OVRRD_EN] is
                                                                 asserted.

                                                                 It is recommended to not use the GSER()_LANE()_TX_CFG_0[CFG_TX_SWING],
                                                                 GSER()_LANE()_TX_CFG_1[TX_SWING_OVRRD_EN,TX_PREMPTAP_OVRRD_VAL], or
                                                                 GSER()_LANE()_TX_PRE_EMPHASIS[CFG_TX_PREMPTAP] override registers for 10BASE-KR
                                                                 or PCIe links in which the transmitter is adapted by the respective
                                                                 hardware-controlled link training protocols.

                                                                 The [CFG_TX_SWING] value for transmitter swing should be derived from
                                                                 signal integrity simulations with IBIS-AMI models supplied by Cavium.

                                                                 A transmit swing change should be followed by a control interface configuration
                                                                 over-ride to force the new setting - see
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CTLIFC_OVRRD_REQ]. */
        uint64_t fast_rdet_mode        : 1;  /**< [  2:  2](R/W/H) Assert to enable fast RX detection. For simulation purposes only. */
        uint64_t fast_tristate_mode    : 1;  /**< [  1:  1](R/W/H) Assert to enable fast tristate power up. For simulation purposes only. */
        uint64_t reserved_0            : 1;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0            : 1;
        uint64_t fast_tristate_mode    : 1;  /**< [  1:  1](R/W/H) Assert to enable fast tristate power up. For simulation purposes only. */
        uint64_t fast_rdet_mode        : 1;  /**< [  2:  2](R/W/H) Assert to enable fast RX detection. For simulation purposes only. */
        uint64_t cfg_tx_swing          : 5;  /**< [  7:  3](R/W) TX output swing control.
                                                                 Default swing encoding when GSER()_LANE()_TX_CFG_1[TX_SWING_OVRRD_EN] is
                                                                 asserted.

                                                                 It is recommended to not use the GSER()_LANE()_TX_CFG_0[CFG_TX_SWING],
                                                                 GSER()_LANE()_TX_CFG_1[TX_SWING_OVRRD_EN,TX_PREMPTAP_OVRRD_VAL], or
                                                                 GSER()_LANE()_TX_PRE_EMPHASIS[CFG_TX_PREMPTAP] override registers for 10BASE-KR
                                                                 or PCIe links in which the transmitter is adapted by the respective
                                                                 hardware-controlled link training protocols.

                                                                 The [CFG_TX_SWING] value for transmitter swing should be derived from
                                                                 signal integrity simulations with IBIS-AMI models supplied by Cavium.

                                                                 A transmit swing change should be followed by a control interface configuration
                                                                 over-ride to force the new setting - see
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CTLIFC_OVRRD_REQ]. */
        uint64_t tx_cm_mode            : 1;  /**< [  8:  8](R/W/H) Assert to enable fast common-mode charge up. For simulation purposes only. */
        uint64_t tx_resetn_ovrrd_val   : 1;  /**< [  9:  9](R/W) TX P2S reset. Active high. Override value when
                                                                 GSER()_LANE()_PWR_CTRL[TX_P2S_RESET_OVRRD_EN] is set. */
        uint64_t reserved_10_13        : 4;
        uint64_t tx_chpd_ovrrd_val     : 1;  /**< [ 14: 14](R/W) TX lane power down. Active high. Override value when
                                                                 GSER()_LANE()_PWR_CTRL[TX_PD_OVRRD_EN] is set. */
        uint64_t tx_tristate_en_ovrrd_val : 1;/**< [ 15: 15](R/W) TX termination high-Z enable. Override value when
                                                                 GSER()_LANE()_PWR_CTRL[TX_TRISTATE_EN_OVRRD_EN] is set. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_tx_cfg_0_s cn; */
};
typedef union bdk_gserx_lanex_tx_cfg_0 bdk_gserx_lanex_tx_cfg_0_t;

static inline uint64_t BDK_GSERX_LANEX_TX_CFG_0(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_TX_CFG_0(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904400a8ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904400a8ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904400a8ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_TX_CFG_0", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_TX_CFG_0(a,b) bdk_gserx_lanex_tx_cfg_0_t
#define bustype_BDK_GSERX_LANEX_TX_CFG_0(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_TX_CFG_0(a,b) "GSERX_LANEX_TX_CFG_0"
#define device_bar_BDK_GSERX_LANEX_TX_CFG_0(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_TX_CFG_0(a,b) (a)
#define arguments_BDK_GSERX_LANEX_TX_CFG_0(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_tx_cfg_1
 *
 * GSER Lane TX Configuration 1 Register
 * These registers are reset by hardware only during chip cold reset. The
 * values of the CSR fields in these registers do not change during chip
 * warm or soft resets.
 */
union bdk_gserx_lanex_tx_cfg_1
{
    uint64_t u;
    struct bdk_gserx_lanex_tx_cfg_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t tx_widthsel_ovrrd_en  : 1;  /**< [ 14: 14](R/W) Override enable for pcs_sds_txX_widthsel, TX parallel interface width setting. */
        uint64_t tx_widthsel_ovrrd_val : 2;  /**< [ 13: 12](R/W) Override value for pcs_sds_widthsel, TX parallel interface width setting.
                                                                 0x0 = 8-bit (not supported).
                                                                 0x1 = 10-bit (not supported).
                                                                 0x2 = 16-bit (for PCIe Gen3 8Gb only).
                                                                 0x3 = 20-bit. */
        uint64_t tx_vboost_en_ovrrd_en : 1;  /**< [ 11: 11](R/W) Override enable for pcs_sds_txX_vboost_en, TX  vboost mode enable. */
        uint64_t tx_turbo_en_ovrrd_en  : 1;  /**< [ 10: 10](R/W) Override enable for pcs_sds_txX_turbo_en, Turbo mode enable. */
        uint64_t tx_swing_ovrrd_en     : 1;  /**< [  9:  9](R/W) Override enable for pcs_sds_txX_swing, TX swing. See
                                                                 GSER()_LANE()_TX_CFG_0[CFG_TX_SWING].

                                                                 It is recommended to not use the GSER()_LANE()_TX_CFG_0[CFG_TX_SWING],
                                                                 GSER()_LANE()_TX_CFG_1[TX_SWING_OVRRD_EN,TX_PREMPTAP_OVRRD_VAL], or
                                                                 GSER()_LANE()_TX_PRE_EMPHASIS[CFG_TX_PREMPTAP] override registers for 10BASE-KR
                                                                 or PCIe links in which the transmitter is adapted by the respective
                                                                 hardware-controlled link training protocols.

                                                                 A transmit swing change should be followed by a control interface
                                                                 configuration over-ride to force the new setting - see
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CTLIFC_OVRRD_REQ]. */
        uint64_t tx_premptap_ovrrd_val : 1;  /**< [  8:  8](R/W) Override enable for pcs_sds_txX_preemptap, preemphasis control. When
                                                                 over-riding,  [TX_PREMPTAP_OVRRD_VAL] should be set and
                                                                 GSER()_LANE()_TX_PRE_EMPHASIS[CFG_TX_PREMPTAP] has the precursor and
                                                                 postcursor values.

                                                                 It is recommended to not use the GSER()_LANE()_TX_CFG_0[CFG_TX_SWING],
                                                                 GSER()_LANE()_TX_CFG_1[TX_SWING_OVRRD_EN,TX_PREMPTAP_OVRRD_VAL], or
                                                                 GSER()_LANE()_TX_PRE_EMPHASIS[CFG_TX_PREMPTAP] override registers for 10BASE-KR
                                                                 or PCIe links in which the transmitter is adapted by the respective
                                                                 hardware-controlled link training protocols.

                                                                 A preemphasis control change should be followed by a control
                                                                 interface configuration override to force the new setting - see
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CTLIFC_OVRRD_REQ]. */
        uint64_t tx_elec_idle_ovrrd_en : 1;  /**< [  7:  7](R/W) Override enable for pcs_sds_txX_elec_idle, TX electrical idle. */
        uint64_t smpl_rate_ovrrd_en    : 1;  /**< [  6:  6](R/W) Override enable for TX power state machine sample rate. When asserted, the TX sample is
                                                                 specified from SMPL_RATE_OVRRD_VAL and the TX Power state machine control signal is
                                                                 ignored. */
        uint64_t smpl_rate_ovrrd_val   : 3;  /**< [  5:  3](R/W) Specifies the sample rate (strobe assertion) relative to mac_pcs_txX_clk when
                                                                 SMPL_RATE_OVRRD_EN is asserted.
                                                                 0x0 = full rate.
                                                                 0x1 = 1/2 data rate.
                                                                 0x2 = 1/4 data rate.
                                                                 0x3 = 1/8 data rate.
                                                                 0x4 = 1/16 data rate.
                                                                 0x5-7 = Reserved. */
        uint64_t tx_datarate_ovrrd_en  : 1;  /**< [  2:  2](R/W) Override enable for RX power state machine data rate signal. When set, rx_datarate is
                                                                 specified from [TX_DATARATE_OVRRD_VAL] and the RX power state machine control signal is
                                                                 ignored. */
        uint64_t tx_datarate_ovrrd_val : 2;  /**< [  1:  0](R/W) Specifies the TX data rate when TX_DATARATE_OVRRD_EN is asserted.
                                                                 0x0 = full rate.
                                                                 0x1 = 1/2 data rate.
                                                                 0x2 = 1/4 data rate.
                                                                 0x3 = 1/8 data rate. */
#else /* Word 0 - Little Endian */
        uint64_t tx_datarate_ovrrd_val : 2;  /**< [  1:  0](R/W) Specifies the TX data rate when TX_DATARATE_OVRRD_EN is asserted.
                                                                 0x0 = full rate.
                                                                 0x1 = 1/2 data rate.
                                                                 0x2 = 1/4 data rate.
                                                                 0x3 = 1/8 data rate. */
        uint64_t tx_datarate_ovrrd_en  : 1;  /**< [  2:  2](R/W) Override enable for RX power state machine data rate signal. When set, rx_datarate is
                                                                 specified from [TX_DATARATE_OVRRD_VAL] and the RX power state machine control signal is
                                                                 ignored. */
        uint64_t smpl_rate_ovrrd_val   : 3;  /**< [  5:  3](R/W) Specifies the sample rate (strobe assertion) relative to mac_pcs_txX_clk when
                                                                 SMPL_RATE_OVRRD_EN is asserted.
                                                                 0x0 = full rate.
                                                                 0x1 = 1/2 data rate.
                                                                 0x2 = 1/4 data rate.
                                                                 0x3 = 1/8 data rate.
                                                                 0x4 = 1/16 data rate.
                                                                 0x5-7 = Reserved. */
        uint64_t smpl_rate_ovrrd_en    : 1;  /**< [  6:  6](R/W) Override enable for TX power state machine sample rate. When asserted, the TX sample is
                                                                 specified from SMPL_RATE_OVRRD_VAL and the TX Power state machine control signal is
                                                                 ignored. */
        uint64_t tx_elec_idle_ovrrd_en : 1;  /**< [  7:  7](R/W) Override enable for pcs_sds_txX_elec_idle, TX electrical idle. */
        uint64_t tx_premptap_ovrrd_val : 1;  /**< [  8:  8](R/W) Override enable for pcs_sds_txX_preemptap, preemphasis control. When
                                                                 over-riding,  [TX_PREMPTAP_OVRRD_VAL] should be set and
                                                                 GSER()_LANE()_TX_PRE_EMPHASIS[CFG_TX_PREMPTAP] has the precursor and
                                                                 postcursor values.

                                                                 It is recommended to not use the GSER()_LANE()_TX_CFG_0[CFG_TX_SWING],
                                                                 GSER()_LANE()_TX_CFG_1[TX_SWING_OVRRD_EN,TX_PREMPTAP_OVRRD_VAL], or
                                                                 GSER()_LANE()_TX_PRE_EMPHASIS[CFG_TX_PREMPTAP] override registers for 10BASE-KR
                                                                 or PCIe links in which the transmitter is adapted by the respective
                                                                 hardware-controlled link training protocols.

                                                                 A preemphasis control change should be followed by a control
                                                                 interface configuration override to force the new setting - see
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CTLIFC_OVRRD_REQ]. */
        uint64_t tx_swing_ovrrd_en     : 1;  /**< [  9:  9](R/W) Override enable for pcs_sds_txX_swing, TX swing. See
                                                                 GSER()_LANE()_TX_CFG_0[CFG_TX_SWING].

                                                                 It is recommended to not use the GSER()_LANE()_TX_CFG_0[CFG_TX_SWING],
                                                                 GSER()_LANE()_TX_CFG_1[TX_SWING_OVRRD_EN,TX_PREMPTAP_OVRRD_VAL], or
                                                                 GSER()_LANE()_TX_PRE_EMPHASIS[CFG_TX_PREMPTAP] override registers for 10BASE-KR
                                                                 or PCIe links in which the transmitter is adapted by the respective
                                                                 hardware-controlled link training protocols.

                                                                 A transmit swing change should be followed by a control interface
                                                                 configuration over-ride to force the new setting - see
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CTLIFC_OVRRD_REQ]. */
        uint64_t tx_turbo_en_ovrrd_en  : 1;  /**< [ 10: 10](R/W) Override enable for pcs_sds_txX_turbo_en, Turbo mode enable. */
        uint64_t tx_vboost_en_ovrrd_en : 1;  /**< [ 11: 11](R/W) Override enable for pcs_sds_txX_vboost_en, TX  vboost mode enable. */
        uint64_t tx_widthsel_ovrrd_val : 2;  /**< [ 13: 12](R/W) Override value for pcs_sds_widthsel, TX parallel interface width setting.
                                                                 0x0 = 8-bit (not supported).
                                                                 0x1 = 10-bit (not supported).
                                                                 0x2 = 16-bit (for PCIe Gen3 8Gb only).
                                                                 0x3 = 20-bit. */
        uint64_t tx_widthsel_ovrrd_en  : 1;  /**< [ 14: 14](R/W) Override enable for pcs_sds_txX_widthsel, TX parallel interface width setting. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_tx_cfg_1_s cn; */
};
typedef union bdk_gserx_lanex_tx_cfg_1 bdk_gserx_lanex_tx_cfg_1_t;

static inline uint64_t BDK_GSERX_LANEX_TX_CFG_1(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_TX_CFG_1(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904400b0ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904400b0ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904400b0ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_TX_CFG_1", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_TX_CFG_1(a,b) bdk_gserx_lanex_tx_cfg_1_t
#define bustype_BDK_GSERX_LANEX_TX_CFG_1(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_TX_CFG_1(a,b) "GSERX_LANEX_TX_CFG_1"
#define device_bar_BDK_GSERX_LANEX_TX_CFG_1(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_TX_CFG_1(a,b) (a)
#define arguments_BDK_GSERX_LANEX_TX_CFG_1(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_tx_cfg_2
 *
 * GSER Lane TX Configuration 2 Register
 * These registers are for diagnostic use only. These registers are reset by hardware only during
 * chip cold reset. The values of the CSR fields in these registers do not change during chip
 * warm or soft resets.
 */
union bdk_gserx_lanex_tx_cfg_2
{
    uint64_t u;
    struct bdk_gserx_lanex_tx_cfg_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t pcs_sds_tx_dcc_en     : 1;  /**< [ 15: 15](R/W) DCC enable. */
        uint64_t reserved_3_14         : 12;
        uint64_t rcvr_test_ovrrd_en    : 1;  /**< [  2:  2](R/W) Override RX detect disable and test pulse. */
        uint64_t rcvr_test_ovrrd_val   : 1;  /**< [  1:  1](R/W) Override value for RX detect test pulse; used to create a pulse during which the receiver
                                                                 detect test operation is performed. */
        uint64_t tx_rx_detect_dis_ovrrd_val : 1;/**< [  0:  0](R/W) Override value of RX detect disable. */
#else /* Word 0 - Little Endian */
        uint64_t tx_rx_detect_dis_ovrrd_val : 1;/**< [  0:  0](R/W) Override value of RX detect disable. */
        uint64_t rcvr_test_ovrrd_val   : 1;  /**< [  1:  1](R/W) Override value for RX detect test pulse; used to create a pulse during which the receiver
                                                                 detect test operation is performed. */
        uint64_t rcvr_test_ovrrd_en    : 1;  /**< [  2:  2](R/W) Override RX detect disable and test pulse. */
        uint64_t reserved_3_14         : 12;
        uint64_t pcs_sds_tx_dcc_en     : 1;  /**< [ 15: 15](R/W) DCC enable. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_tx_cfg_2_s cn; */
};
typedef union bdk_gserx_lanex_tx_cfg_2 bdk_gserx_lanex_tx_cfg_2_t;

static inline uint64_t BDK_GSERX_LANEX_TX_CFG_2(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_TX_CFG_2(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904400b8ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904400b8ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904400b8ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_TX_CFG_2", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_TX_CFG_2(a,b) bdk_gserx_lanex_tx_cfg_2_t
#define bustype_BDK_GSERX_LANEX_TX_CFG_2(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_TX_CFG_2(a,b) "GSERX_LANEX_TX_CFG_2"
#define device_bar_BDK_GSERX_LANEX_TX_CFG_2(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_TX_CFG_2(a,b) (a)
#define arguments_BDK_GSERX_LANEX_TX_CFG_2(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_tx_cfg_3
 *
 * GSER Lane TX Configuration 3 Register
 * These registers are for diagnostic use only. These registers are reset by hardware only during
 * chip cold reset. The values of the CSR fields in these registers do not change during chip
 * warm or soft resets.
 */
union bdk_gserx_lanex_tx_cfg_3
{
    uint64_t u;
    struct bdk_gserx_lanex_tx_cfg_3_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t cfg_tx_vboost_en      : 1;  /**< [ 14: 14](R/W) Specifies the value of TX VBoost enable when
                                                                 GSER()_LANE()_TX_CFG_1[TX_VBOOST_EN_OVRRD_EN] is asserted. */
        uint64_t reserved_7_13         : 7;
        uint64_t pcs_sds_tx_gain       : 3;  /**< [  6:  4](R/W/H) TX gain. For debug use only. */
        uint64_t pcs_sds_tx_srate_sel  : 3;  /**< [  3:  1](R/W/H) Reserved. */
        uint64_t cfg_tx_turbo_en       : 1;  /**< [  0:  0](R/W) Specifies value of TX turbo enable when GSER()_LANE()_TX_CFG_1[TX_TURBO_EN] is set. */
#else /* Word 0 - Little Endian */
        uint64_t cfg_tx_turbo_en       : 1;  /**< [  0:  0](R/W) Specifies value of TX turbo enable when GSER()_LANE()_TX_CFG_1[TX_TURBO_EN] is set. */
        uint64_t pcs_sds_tx_srate_sel  : 3;  /**< [  3:  1](R/W/H) Reserved. */
        uint64_t pcs_sds_tx_gain       : 3;  /**< [  6:  4](R/W/H) TX gain. For debug use only. */
        uint64_t reserved_7_13         : 7;
        uint64_t cfg_tx_vboost_en      : 1;  /**< [ 14: 14](R/W) Specifies the value of TX VBoost enable when
                                                                 GSER()_LANE()_TX_CFG_1[TX_VBOOST_EN_OVRRD_EN] is asserted. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_tx_cfg_3_s cn; */
};
typedef union bdk_gserx_lanex_tx_cfg_3 bdk_gserx_lanex_tx_cfg_3_t;

static inline uint64_t BDK_GSERX_LANEX_TX_CFG_3(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_TX_CFG_3(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904400c0ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904400c0ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904400c0ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_TX_CFG_3", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_TX_CFG_3(a,b) bdk_gserx_lanex_tx_cfg_3_t
#define bustype_BDK_GSERX_LANEX_TX_CFG_3(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_TX_CFG_3(a,b) "GSERX_LANEX_TX_CFG_3"
#define device_bar_BDK_GSERX_LANEX_TX_CFG_3(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_TX_CFG_3(a,b) (a)
#define arguments_BDK_GSERX_LANEX_TX_CFG_3(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane#_tx_pre_emphasis
 *
 * GSER Lane TX Configuration Preemphasis Register
 * These registers are reset by hardware only during chip cold reset. The
 * values of the CSR fields in these registers do not change during chip
 * warm or soft resets.
 */
union bdk_gserx_lanex_tx_pre_emphasis
{
    uint64_t u;
    struct bdk_gserx_lanex_tx_pre_emphasis_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t cfg_tx_premptap       : 9;  /**< [  8:  0](R/W) Override preemphasis control. Applies when
                                                                 GSER()_LANE()_TX_CFG_1[TX_PREMPTAP_OVRRD_VAL] is asserted.
                                                                 \<8:4\> = Postcursor.
                                                                 \<3:0\> = Precursor.

                                                                 It is recommended to not use the GSER()_LANE()_TX_CFG_0[CFG_TX_SWING],
                                                                 GSER()_LANE()_TX_CFG_1[TX_SWING_OVRRD_EN,TX_PREMPTAP_OVRRD_VAL], or
                                                                 GSER()_LANE()_TX_PRE_EMPHASIS[CFG_TX_PREMPTAP] override registers for 10BASE-KR
                                                                 or PCIe links in which the transmitter is adapted by the respective
                                                                 hardware-controlled link training protocols.

                                                                 The [CFG_TX_PREEMPTAP] value for transmitter preemphasis and
                                                                 postemphasis should be derived from signal integrity simulations
                                                                 with IBIS-AMI models supplied by Cavium.

                                                                 A preemphasis control change should be followed by a control interface
                                                                 configuration over-ride to force the new setting - see
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CTLIFC_OVRRD_REQ]. */
#else /* Word 0 - Little Endian */
        uint64_t cfg_tx_premptap       : 9;  /**< [  8:  0](R/W) Override preemphasis control. Applies when
                                                                 GSER()_LANE()_TX_CFG_1[TX_PREMPTAP_OVRRD_VAL] is asserted.
                                                                 \<8:4\> = Postcursor.
                                                                 \<3:0\> = Precursor.

                                                                 It is recommended to not use the GSER()_LANE()_TX_CFG_0[CFG_TX_SWING],
                                                                 GSER()_LANE()_TX_CFG_1[TX_SWING_OVRRD_EN,TX_PREMPTAP_OVRRD_VAL], or
                                                                 GSER()_LANE()_TX_PRE_EMPHASIS[CFG_TX_PREMPTAP] override registers for 10BASE-KR
                                                                 or PCIe links in which the transmitter is adapted by the respective
                                                                 hardware-controlled link training protocols.

                                                                 The [CFG_TX_PREEMPTAP] value for transmitter preemphasis and
                                                                 postemphasis should be derived from signal integrity simulations
                                                                 with IBIS-AMI models supplied by Cavium.

                                                                 A preemphasis control change should be followed by a control interface
                                                                 configuration over-ride to force the new setting - see
                                                                 GSER()_LANE()_PCS_CTLIFC_2[CTLIFC_OVRRD_REQ]. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lanex_tx_pre_emphasis_s cn; */
};
typedef union bdk_gserx_lanex_tx_pre_emphasis bdk_gserx_lanex_tx_pre_emphasis_t;

static inline uint64_t BDK_GSERX_LANEX_TX_PRE_EMPHASIS(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANEX_TX_PRE_EMPHASIS(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e0904400c8ll + 0x1000000ll * ((a) & 0x3) + 0x100000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3)))
        return 0x87e0904400c8ll + 0x1000000ll * ((a) & 0x7) + 0x100000ll * ((b) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3)))
        return 0x87e0904400c8ll + 0x1000000ll * ((a) & 0xf) + 0x100000ll * ((b) & 0x3);
    __bdk_csr_fatal("GSERX_LANEX_TX_PRE_EMPHASIS", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANEX_TX_PRE_EMPHASIS(a,b) bdk_gserx_lanex_tx_pre_emphasis_t
#define bustype_BDK_GSERX_LANEX_TX_PRE_EMPHASIS(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANEX_TX_PRE_EMPHASIS(a,b) "GSERX_LANEX_TX_PRE_EMPHASIS"
#define device_bar_BDK_GSERX_LANEX_TX_PRE_EMPHASIS(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANEX_TX_PRE_EMPHASIS(a,b) (a)
#define arguments_BDK_GSERX_LANEX_TX_PRE_EMPHASIS(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane_lpbken
 *
 * GSER Lane Loopback Enable Register
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lane_lpbken
{
    uint64_t u;
    struct bdk_gserx_lane_lpbken_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t lpbken                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE or SATA mode. When asserted in P0 state,
                                                                 allows per-lane TX-to-RX serial loopback activation.
                                                                 \<3\>: Lane 3.  Reserved.
                                                                 \<2\>: Lane 2.  Reserved.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t lpbken                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE or SATA mode. When asserted in P0 state,
                                                                 allows per-lane TX-to-RX serial loopback activation.
                                                                 \<3\>: Lane 3.  Reserved.
                                                                 \<2\>: Lane 2.  Reserved.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lane_lpbken_s cn81xx; */
    struct bdk_gserx_lane_lpbken_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t lpbken                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE or SATA mode (including all CCPI links). When asserted in
                                                                 P0
                                                                 state,
                                                                 allows per-lane TX-to-RX serial loopback activation.
                                                                 \<3\>: Lane 3.
                                                                 \<2\>: Lane 2.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t lpbken                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE or SATA mode (including all CCPI links). When asserted in
                                                                 P0
                                                                 state,
                                                                 allows per-lane TX-to-RX serial loopback activation.
                                                                 \<3\>: Lane 3.
                                                                 \<2\>: Lane 2.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_gserx_lane_lpbken_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t lpbken                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE or SATA mode. When asserted in
                                                                 P0 state, allows per lane TX-to-RX serial loopback activation.
                                                                 \<3\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<2\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t lpbken                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE or SATA mode. When asserted in
                                                                 P0 state, allows per lane TX-to-RX serial loopback activation.
                                                                 \<3\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<2\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_lane_lpbken bdk_gserx_lane_lpbken_t;

static inline uint64_t BDK_GSERX_LANE_LPBKEN(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANE_LPBKEN(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000110ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000110ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000110ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_LANE_LPBKEN", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_LANE_LPBKEN(a) bdk_gserx_lane_lpbken_t
#define bustype_BDK_GSERX_LANE_LPBKEN(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANE_LPBKEN(a) "GSERX_LANE_LPBKEN"
#define device_bar_BDK_GSERX_LANE_LPBKEN(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANE_LPBKEN(a) (a)
#define arguments_BDK_GSERX_LANE_LPBKEN(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_lane_mode
 *
 * GSER Lane Mode Register
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lane_mode
{
    uint64_t u;
    struct bdk_gserx_lane_mode_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t lmode                 : 4;  /**< [  3:  0](R/W/H) For links that are not in PCIE or SATA mode, used to index into the PHY
                                                                 table to select electrical specs and link rate. Note that the PHY table can be modified
                                                                 such that any supported link rate can be derived regardless of the configured LMODE.

                                                                 0x0: R_25G_REFCLK100.
                                                                 0x1: R_5G_REFCLK100.
                                                                 0x2: R_8G_REFCLK100.
                                                                 0x3: R_125G_REFCLK15625_KX (not supported).
                                                                 0x4: R_3125G_REFCLK15625_XAUI.
                                                                 0x5: R_103125G_REFCLK15625_KR.
                                                                 0x6: R_125G_REFCLK15625_SGMII.
                                                                 0x7: R_5G_REFCLK15625_QSGMII.
                                                                 0x8: R_625G_REFCLK15625_RXAUI.
                                                                 0x9: R_25G_REFCLK125.
                                                                 0xA: R_5G_REFCLK125.
                                                                 0xB: R_8G_REFCLK125.
                                                                 0xC - 0xF: Reserved.

                                                                 This register is not used for PCIE configurations. For BGX links, this register
                                                                 defaults to R_625G_REFCLK15625_RXAUI.

                                                                 It is recommended that the PHY be in reset when reconfiguring the [LMODE]
                                                                 (GSER()_PHY_CTL[PHY_RESET] is set).

                                                                 Once the [LMODE] has been configured, and the PHY is out of reset, the table entries for
                                                                 the
                                                                 selected [LMODE] must be updated to reflect the reference clock speed. Refer to the
                                                                 register
                                                                 description and index into the table using the rate and reference speed to obtain the
                                                                 recommended values.

                                                                 _ Write GSER()_PLL_P()_MODE_0.
                                                                 _ Write GSER()_PLL_P()_MODE_1.
                                                                 _ Write GSER()_LANE_P()_MODE_0.
                                                                 _ Write GSER()_LANE_P()_MODE_1.

                                                                 where in "P(z)", z equals [LMODE]. */
#else /* Word 0 - Little Endian */
        uint64_t lmode                 : 4;  /**< [  3:  0](R/W/H) For links that are not in PCIE or SATA mode, used to index into the PHY
                                                                 table to select electrical specs and link rate. Note that the PHY table can be modified
                                                                 such that any supported link rate can be derived regardless of the configured LMODE.

                                                                 0x0: R_25G_REFCLK100.
                                                                 0x1: R_5G_REFCLK100.
                                                                 0x2: R_8G_REFCLK100.
                                                                 0x3: R_125G_REFCLK15625_KX (not supported).
                                                                 0x4: R_3125G_REFCLK15625_XAUI.
                                                                 0x5: R_103125G_REFCLK15625_KR.
                                                                 0x6: R_125G_REFCLK15625_SGMII.
                                                                 0x7: R_5G_REFCLK15625_QSGMII.
                                                                 0x8: R_625G_REFCLK15625_RXAUI.
                                                                 0x9: R_25G_REFCLK125.
                                                                 0xA: R_5G_REFCLK125.
                                                                 0xB: R_8G_REFCLK125.
                                                                 0xC - 0xF: Reserved.

                                                                 This register is not used for PCIE configurations. For BGX links, this register
                                                                 defaults to R_625G_REFCLK15625_RXAUI.

                                                                 It is recommended that the PHY be in reset when reconfiguring the [LMODE]
                                                                 (GSER()_PHY_CTL[PHY_RESET] is set).

                                                                 Once the [LMODE] has been configured, and the PHY is out of reset, the table entries for
                                                                 the
                                                                 selected [LMODE] must be updated to reflect the reference clock speed. Refer to the
                                                                 register
                                                                 description and index into the table using the rate and reference speed to obtain the
                                                                 recommended values.

                                                                 _ Write GSER()_PLL_P()_MODE_0.
                                                                 _ Write GSER()_PLL_P()_MODE_1.
                                                                 _ Write GSER()_LANE_P()_MODE_0.
                                                                 _ Write GSER()_LANE_P()_MODE_1.

                                                                 where in "P(z)", z equals [LMODE]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lane_mode_s cn81xx; */
    struct bdk_gserx_lane_mode_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t lmode                 : 4;  /**< [  3:  0](R/W/H) For links that are not in PCIE or SATA mode (including all CCPI links), used to index into
                                                                 the PHY
                                                                 table to select electrical specs and link rate. Note that the PHY table can be modified
                                                                 such that any supported link rate can be derived regardless of the configured LMODE.

                                                                 0x0: R_25G_REFCLK100.
                                                                 0x1: R_5G_REFCLK100.
                                                                 0x2: R_8G_REFCLK100.
                                                                 0x3: R_125G_REFCLK15625_KX (not supported).
                                                                 0x4: R_3125G_REFCLK15625_XAUI.
                                                                 0x5: R_103125G_REFCLK15625_KR.
                                                                 0x6: R_125G_REFCLK15625_SGMII.
                                                                 0x7: R_5G_REFCLK15625_QSGMII (not supported).
                                                                 0x8: R_625G_REFCLK15625_RXAUI.
                                                                 0x9: R_25G_REFCLK125.
                                                                 0xA: R_5G_REFCLK125.
                                                                 0xB: R_8G_REFCLK125.
                                                                 0xC - 0xF: Reserved.

                                                                 This register is not used for PCIE configurations. For non-CCPI links, this register
                                                                 defaults to R_625G_REFCLK15625_RXAUI. For CCPI links, the value is mapped at reset from
                                                                 the
                                                                 GSER()_SPD and the appropriate table updates are performed so the rate is obtained for the
                                                                 particular reference clock.

                                                                 It is recommended that the PHY be in reset when reconfiguring the [LMODE]
                                                                 (GSER()_PHY_CTL[PHY_RESET] is set).

                                                                 Once the [LMODE] has been configured, and the PHY is out of reset, the table entries for
                                                                 the
                                                                 selected [LMODE] must be updated to reflect the reference clock speed. Refer to the
                                                                 register
                                                                 description and index into the table using the rate and reference speed to obtain the
                                                                 recommended values.

                                                                 _ Write GSER()_PLL_P()_MODE_0.
                                                                 _ Write GSER()_PLL_P()_MODE_1.
                                                                 _ Write GSER()_LANE_P()_MODE_0.
                                                                 _ Write GSER()_LANE_P()_MODE_1.

                                                                 where in "P(z)", z equals [LMODE]. */
#else /* Word 0 - Little Endian */
        uint64_t lmode                 : 4;  /**< [  3:  0](R/W/H) For links that are not in PCIE or SATA mode (including all CCPI links), used to index into
                                                                 the PHY
                                                                 table to select electrical specs and link rate. Note that the PHY table can be modified
                                                                 such that any supported link rate can be derived regardless of the configured LMODE.

                                                                 0x0: R_25G_REFCLK100.
                                                                 0x1: R_5G_REFCLK100.
                                                                 0x2: R_8G_REFCLK100.
                                                                 0x3: R_125G_REFCLK15625_KX (not supported).
                                                                 0x4: R_3125G_REFCLK15625_XAUI.
                                                                 0x5: R_103125G_REFCLK15625_KR.
                                                                 0x6: R_125G_REFCLK15625_SGMII.
                                                                 0x7: R_5G_REFCLK15625_QSGMII (not supported).
                                                                 0x8: R_625G_REFCLK15625_RXAUI.
                                                                 0x9: R_25G_REFCLK125.
                                                                 0xA: R_5G_REFCLK125.
                                                                 0xB: R_8G_REFCLK125.
                                                                 0xC - 0xF: Reserved.

                                                                 This register is not used for PCIE configurations. For non-CCPI links, this register
                                                                 defaults to R_625G_REFCLK15625_RXAUI. For CCPI links, the value is mapped at reset from
                                                                 the
                                                                 GSER()_SPD and the appropriate table updates are performed so the rate is obtained for the
                                                                 particular reference clock.

                                                                 It is recommended that the PHY be in reset when reconfiguring the [LMODE]
                                                                 (GSER()_PHY_CTL[PHY_RESET] is set).

                                                                 Once the [LMODE] has been configured, and the PHY is out of reset, the table entries for
                                                                 the
                                                                 selected [LMODE] must be updated to reflect the reference clock speed. Refer to the
                                                                 register
                                                                 description and index into the table using the rate and reference speed to obtain the
                                                                 recommended values.

                                                                 _ Write GSER()_PLL_P()_MODE_0.
                                                                 _ Write GSER()_PLL_P()_MODE_1.
                                                                 _ Write GSER()_LANE_P()_MODE_0.
                                                                 _ Write GSER()_LANE_P()_MODE_1.

                                                                 where in "P(z)", z equals [LMODE]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_gserx_lane_mode_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t lmode                 : 4;  /**< [  3:  0](R/W/H) For links that are not in PCIE or SATA mode, used to index into the PHY
                                                                 table to select electrical specs and link rate. Note that the PHY table can be modified
                                                                 such that any supported link rate can be derived regardless of the configured LMODE.

                                                                 0x0: R_25G_REFCLK100.
                                                                 0x1: R_5G_REFCLK100.
                                                                 0x2: R_8G_REFCLK100.
                                                                 0x3: R_125G_REFCLK15625_KX (not supported).
                                                                 0x4: R_3125G_REFCLK15625_XAUI.
                                                                 0x5: R_103125G_REFCLK15625_KR.
                                                                 0x6: R_125G_REFCLK15625_SGMII.
                                                                 0x7: R_5G_REFCLK15625_QSGMII.
                                                                 0x8: R_625G_REFCLK15625_RXAUI.
                                                                 0x9: R_25G_REFCLK125.
                                                                 0xA: R_5G_REFCLK125.
                                                                 0xB: R_8G_REFCLK125.
                                                                 0xC - 0xF: Reserved.

                                                                 This register is not used for PCIE configurations. This register
                                                                 defaults to R_625G_REFCLK15625_RXAUI.

                                                                 It is recommended that the PHY be in reset when reconfiguring the [LMODE]
                                                                 (GSER()_PHY_CTL[PHY_RESET] is set).

                                                                 Once the [LMODE] has been configured, and the PHY is out of reset, the table entries for
                                                                 the
                                                                 selected [LMODE] must be updated to reflect the reference clock speed. Refer to the
                                                                 register
                                                                 description and index into the table using the rate and reference speed to obtain the
                                                                 recommended values.

                                                                 _ Write GSER()_PLL_P()_MODE_0.
                                                                 _ Write GSER()_PLL_P()_MODE_1.
                                                                 _ Write GSER()_LANE_P()_MODE_0.
                                                                 _ Write GSER()_LANE_P()_MODE_1.

                                                                 where in "P(z)", z equals [LMODE]. */
#else /* Word 0 - Little Endian */
        uint64_t lmode                 : 4;  /**< [  3:  0](R/W/H) For links that are not in PCIE or SATA mode, used to index into the PHY
                                                                 table to select electrical specs and link rate. Note that the PHY table can be modified
                                                                 such that any supported link rate can be derived regardless of the configured LMODE.

                                                                 0x0: R_25G_REFCLK100.
                                                                 0x1: R_5G_REFCLK100.
                                                                 0x2: R_8G_REFCLK100.
                                                                 0x3: R_125G_REFCLK15625_KX (not supported).
                                                                 0x4: R_3125G_REFCLK15625_XAUI.
                                                                 0x5: R_103125G_REFCLK15625_KR.
                                                                 0x6: R_125G_REFCLK15625_SGMII.
                                                                 0x7: R_5G_REFCLK15625_QSGMII.
                                                                 0x8: R_625G_REFCLK15625_RXAUI.
                                                                 0x9: R_25G_REFCLK125.
                                                                 0xA: R_5G_REFCLK125.
                                                                 0xB: R_8G_REFCLK125.
                                                                 0xC - 0xF: Reserved.

                                                                 This register is not used for PCIE configurations. This register
                                                                 defaults to R_625G_REFCLK15625_RXAUI.

                                                                 It is recommended that the PHY be in reset when reconfiguring the [LMODE]
                                                                 (GSER()_PHY_CTL[PHY_RESET] is set).

                                                                 Once the [LMODE] has been configured, and the PHY is out of reset, the table entries for
                                                                 the
                                                                 selected [LMODE] must be updated to reflect the reference clock speed. Refer to the
                                                                 register
                                                                 description and index into the table using the rate and reference speed to obtain the
                                                                 recommended values.

                                                                 _ Write GSER()_PLL_P()_MODE_0.
                                                                 _ Write GSER()_PLL_P()_MODE_1.
                                                                 _ Write GSER()_LANE_P()_MODE_0.
                                                                 _ Write GSER()_LANE_P()_MODE_1.

                                                                 where in "P(z)", z equals [LMODE]. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_lane_mode bdk_gserx_lane_mode_t;

static inline uint64_t BDK_GSERX_LANE_MODE(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANE_MODE(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000118ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000118ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000118ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_LANE_MODE", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_LANE_MODE(a) bdk_gserx_lane_mode_t
#define bustype_BDK_GSERX_LANE_MODE(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANE_MODE(a) "GSERX_LANE_MODE"
#define device_bar_BDK_GSERX_LANE_MODE(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANE_MODE(a) (a)
#define arguments_BDK_GSERX_LANE_MODE(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_lane_p#_mode_0
 *
 * GSER Lane Protocol Mode 0 Register
 * These are the RAW PCS lane settings mode 0 registers. There is one register per
 * 4 lanes per GSER per GSER_LMODE_E value (0..11). Only one entry is used at any given time in a
 * given GSER lane - the one selected by the corresponding GSER()_LANE_MODE[LMODE].
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lane_px_mode_0
{
    uint64_t u;
    struct bdk_gserx_lane_px_mode_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t ctle                  : 2;  /**< [ 14: 13](R/W/H) Continuous time linear equalizer pole configuration.
                                                                 0x0 = ~5dB of peaking at 4 GHz (Minimum bandwidth).
                                                                 0x1 =~10dB of peaking at 5 GHz.
                                                                 0x2 = ~15dB of peaking at 5.5 GHz.
                                                                 0x3 = ~20dB of peaking at 6 GHz (Maximum bandwidth).

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ R_25G_REFCLK100:          0x0
                                                                 _ R_5G_REFCLK100:           0x0
                                                                 _ R_8G_REFCLK100:           0x3
                                                                 _ R_125G_REFCLK15625_KX:    0x0
                                                                 _ R_3125G_REFCLK15625_XAUI: 0x0
                                                                 _ R_103125G_REFCLK15625_KR: 0x3
                                                                 _ R_125G_REFCLK15625_SGMII: 0x0
                                                                 _ R_5G_REFCLK15625_QSGMII:  0x0
                                                                 _ R_625G_REFCLK15625_RXAUI: 0x0
                                                                 _ R_25G_REFCLK125:          0x0
                                                                 _ R_5G_REFCLK125:           0x0
                                                                 _ R_8G_REFCLK125:           0x3
                                                                 \</pre\>

                                                                 For SATA, [CTLE] should always be 0. */
        uint64_t pcie                  : 1;  /**< [ 12: 12](R/W/H) Selects between RX terminations.
                                                                 0 = Differential termination.
                                                                 1 = Termination between pad and SDS_VDDS.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ R_25G_REFCLK100:          0x1
                                                                 _ R_5G_REFCLK100:           0x1
                                                                 _ R_8G_REFCLK100:           0x0
                                                                 _ R_125G_REFCLK15625_KX:    0x0
                                                                 _ R_3125G_REFCLK15625_XAUI: 0x0
                                                                 _ R_103125G_REFCLK15625_KR: 0x0
                                                                 _ R_125G_REFCLK15625_SGMII: 0x0
                                                                 _ R_5G_REFCLK15625_QSGMII:  0x0
                                                                 _ R_625G_REFCLK15625_RXAUI: 0x0
                                                                 _ R_25G_REFCLK125:          0x1
                                                                 _ R_5G_REFCLK125:           0x1
                                                                 _ R_8G_REFCLK125:           0x0
                                                                 \</pre\>

                                                                 For SATA, [PCIE] should always be 0. */
        uint64_t tx_ldiv               : 2;  /**< [ 11: 10](R/W/H) Configures clock divider used to determine the receive rate.
                                                                 0x0 = full data rate.
                                                                 0x1 = 1/2 data rate.
                                                                 0x2 = 1/4 data rate.
                                                                 0x3 = 1/8 data rate.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ R_25G_REFCLK100:          0x1
                                                                 _ R_5G_REFCLK100:           0x0
                                                                 _ R_8G_REFCLK100:           0x0
                                                                 _ R_125G_REFCLK15625_KX:    0x2
                                                                 _ R_3125G_REFCLK15625_XAUI: 0x1
                                                                 _ R_103125G_REFCLK15625_KR: 0x0
                                                                 _ R_125G_REFCLK15625_SGMII: 0x2
                                                                 _ R_5G_REFCLK15625_QSGMII:  0x0
                                                                 _ R_625G_REFCLK15625_RXAUI: 0x0
                                                                 _ R_25G_REFCLK125:          0x1
                                                                 _ R_5G_REFCLK125:           0x0
                                                                 _ R_8G_REFCLK125:           0x0
                                                                 \</pre\>

                                                                 For SATA, [TX_LDIV] should always be 0. */
        uint64_t rx_ldiv               : 2;  /**< [  9:  8](R/W/H) Configures clock divider used to determine the receive rate.
                                                                 0x0 = full data rate.
                                                                 0x1 = 1/2 data rate.
                                                                 0x2 = 1/4 data rate.
                                                                 0x3 = 1/8 data rate.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ R_25G_REFCLK100:          0x1
                                                                 _ R_5G_REFCLK100:           0x0
                                                                 _ R_8G_REFCLK100:           0x0
                                                                 _ R_125G_REFCLK15625_KX:    0x2
                                                                 _ R_3125G_REFCLK15625_XAUI: 0x1
                                                                 _ R_103125G_REFCLK15625_KR: 0x0
                                                                 _ R_125G_REFCLK15625_SGMII: 0x2
                                                                 _ R_5G_REFCLK15625_QSGMII:  0x0
                                                                 _ R_625G_REFCLK15625_RXAUI: 0x0
                                                                 _ R_25G_REFCLK125:          0x1
                                                                 _ R_5G_REFCLK125:           0x0
                                                                 _ R_8G_REFCLK125:           0x0
                                                                 \</pre\>

                                                                 For SATA, [RX_LDIV] should be 2 for R_25G_REFCLK100 (position 0, 1.5 Gbaud),
                                                                 1 for R_5G_REFCLK100 (position 1, 3 Gbaud), and 0 for R_8G_REFCLK100
                                                                 (position 2, 6 Gbaud). */
        uint64_t srate                 : 3;  /**< [  7:  5](R/W) Sample rate, used to generate strobe to effectively divide the clock down to a slower
                                                                 rate.

                                                                 0x0 = Full rate.
                                                                 0x1 = 1/2 data rate.
                                                                 0x2 = 1/4 data rate.
                                                                 0x3 = 1/8 data rate.
                                                                 0x4 = 1/16 data rate.
                                                                 else = Reserved.

                                                                 This field should always be cleared to zero (i.e. full rate selected). */
        uint64_t reserved_4            : 1;
        uint64_t tx_mode               : 2;  /**< [  3:  2](R/W/H) TX data width:
                                                                 0x0 = 8-bit raw data (not supported).
                                                                 0x1 = 10-bit raw data (not supported).
                                                                 0x2 = 16-bit raw data (for PCIe Gen3 8 Gb only - software should normally not select
                                                                 this).
                                                                 0x3 = 20-bit raw data (anything software-configured). */
        uint64_t rx_mode               : 2;  /**< [  1:  0](R/W/H) RX data width:
                                                                 0x0 = 8-bit raw data (not supported).
                                                                 0x1 = 10-bit raw data (not supported).
                                                                 0x2 = 16-bit raw data (for PCIe Gen3 8 Gb only - software should normally not select
                                                                 this).
                                                                 0x3 = 20-bit raw data (anything software-configured). */
#else /* Word 0 - Little Endian */
        uint64_t rx_mode               : 2;  /**< [  1:  0](R/W/H) RX data width:
                                                                 0x0 = 8-bit raw data (not supported).
                                                                 0x1 = 10-bit raw data (not supported).
                                                                 0x2 = 16-bit raw data (for PCIe Gen3 8 Gb only - software should normally not select
                                                                 this).
                                                                 0x3 = 20-bit raw data (anything software-configured). */
        uint64_t tx_mode               : 2;  /**< [  3:  2](R/W/H) TX data width:
                                                                 0x0 = 8-bit raw data (not supported).
                                                                 0x1 = 10-bit raw data (not supported).
                                                                 0x2 = 16-bit raw data (for PCIe Gen3 8 Gb only - software should normally not select
                                                                 this).
                                                                 0x3 = 20-bit raw data (anything software-configured). */
        uint64_t reserved_4            : 1;
        uint64_t srate                 : 3;  /**< [  7:  5](R/W) Sample rate, used to generate strobe to effectively divide the clock down to a slower
                                                                 rate.

                                                                 0x0 = Full rate.
                                                                 0x1 = 1/2 data rate.
                                                                 0x2 = 1/4 data rate.
                                                                 0x3 = 1/8 data rate.
                                                                 0x4 = 1/16 data rate.
                                                                 else = Reserved.

                                                                 This field should always be cleared to zero (i.e. full rate selected). */
        uint64_t rx_ldiv               : 2;  /**< [  9:  8](R/W/H) Configures clock divider used to determine the receive rate.
                                                                 0x0 = full data rate.
                                                                 0x1 = 1/2 data rate.
                                                                 0x2 = 1/4 data rate.
                                                                 0x3 = 1/8 data rate.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ R_25G_REFCLK100:          0x1
                                                                 _ R_5G_REFCLK100:           0x0
                                                                 _ R_8G_REFCLK100:           0x0
                                                                 _ R_125G_REFCLK15625_KX:    0x2
                                                                 _ R_3125G_REFCLK15625_XAUI: 0x1
                                                                 _ R_103125G_REFCLK15625_KR: 0x0
                                                                 _ R_125G_REFCLK15625_SGMII: 0x2
                                                                 _ R_5G_REFCLK15625_QSGMII:  0x0
                                                                 _ R_625G_REFCLK15625_RXAUI: 0x0
                                                                 _ R_25G_REFCLK125:          0x1
                                                                 _ R_5G_REFCLK125:           0x0
                                                                 _ R_8G_REFCLK125:           0x0
                                                                 \</pre\>

                                                                 For SATA, [RX_LDIV] should be 2 for R_25G_REFCLK100 (position 0, 1.5 Gbaud),
                                                                 1 for R_5G_REFCLK100 (position 1, 3 Gbaud), and 0 for R_8G_REFCLK100
                                                                 (position 2, 6 Gbaud). */
        uint64_t tx_ldiv               : 2;  /**< [ 11: 10](R/W/H) Configures clock divider used to determine the receive rate.
                                                                 0x0 = full data rate.
                                                                 0x1 = 1/2 data rate.
                                                                 0x2 = 1/4 data rate.
                                                                 0x3 = 1/8 data rate.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ R_25G_REFCLK100:          0x1
                                                                 _ R_5G_REFCLK100:           0x0
                                                                 _ R_8G_REFCLK100:           0x0
                                                                 _ R_125G_REFCLK15625_KX:    0x2
                                                                 _ R_3125G_REFCLK15625_XAUI: 0x1
                                                                 _ R_103125G_REFCLK15625_KR: 0x0
                                                                 _ R_125G_REFCLK15625_SGMII: 0x2
                                                                 _ R_5G_REFCLK15625_QSGMII:  0x0
                                                                 _ R_625G_REFCLK15625_RXAUI: 0x0
                                                                 _ R_25G_REFCLK125:          0x1
                                                                 _ R_5G_REFCLK125:           0x0
                                                                 _ R_8G_REFCLK125:           0x0
                                                                 \</pre\>

                                                                 For SATA, [TX_LDIV] should always be 0. */
        uint64_t pcie                  : 1;  /**< [ 12: 12](R/W/H) Selects between RX terminations.
                                                                 0 = Differential termination.
                                                                 1 = Termination between pad and SDS_VDDS.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ R_25G_REFCLK100:          0x1
                                                                 _ R_5G_REFCLK100:           0x1
                                                                 _ R_8G_REFCLK100:           0x0
                                                                 _ R_125G_REFCLK15625_KX:    0x0
                                                                 _ R_3125G_REFCLK15625_XAUI: 0x0
                                                                 _ R_103125G_REFCLK15625_KR: 0x0
                                                                 _ R_125G_REFCLK15625_SGMII: 0x0
                                                                 _ R_5G_REFCLK15625_QSGMII:  0x0
                                                                 _ R_625G_REFCLK15625_RXAUI: 0x0
                                                                 _ R_25G_REFCLK125:          0x1
                                                                 _ R_5G_REFCLK125:           0x1
                                                                 _ R_8G_REFCLK125:           0x0
                                                                 \</pre\>

                                                                 For SATA, [PCIE] should always be 0. */
        uint64_t ctle                  : 2;  /**< [ 14: 13](R/W/H) Continuous time linear equalizer pole configuration.
                                                                 0x0 = ~5dB of peaking at 4 GHz (Minimum bandwidth).
                                                                 0x1 =~10dB of peaking at 5 GHz.
                                                                 0x2 = ~15dB of peaking at 5.5 GHz.
                                                                 0x3 = ~20dB of peaking at 6 GHz (Maximum bandwidth).

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ R_25G_REFCLK100:          0x0
                                                                 _ R_5G_REFCLK100:           0x0
                                                                 _ R_8G_REFCLK100:           0x3
                                                                 _ R_125G_REFCLK15625_KX:    0x0
                                                                 _ R_3125G_REFCLK15625_XAUI: 0x0
                                                                 _ R_103125G_REFCLK15625_KR: 0x3
                                                                 _ R_125G_REFCLK15625_SGMII: 0x0
                                                                 _ R_5G_REFCLK15625_QSGMII:  0x0
                                                                 _ R_625G_REFCLK15625_RXAUI: 0x0
                                                                 _ R_25G_REFCLK125:          0x0
                                                                 _ R_5G_REFCLK125:           0x0
                                                                 _ R_8G_REFCLK125:           0x3
                                                                 \</pre\>

                                                                 For SATA, [CTLE] should always be 0. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lane_px_mode_0_s cn; */
};
typedef union bdk_gserx_lane_px_mode_0 bdk_gserx_lane_px_mode_0_t;

static inline uint64_t BDK_GSERX_LANE_PX_MODE_0(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANE_PX_MODE_0(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=11)))
        return 0x87e0904e0040ll + 0x1000000ll * ((a) & 0x3) + 0x20ll * ((b) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=11)))
        return 0x87e0904e0040ll + 0x1000000ll * ((a) & 0x7) + 0x20ll * ((b) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=11)))
        return 0x87e0904e0040ll + 0x1000000ll * ((a) & 0xf) + 0x20ll * ((b) & 0xf);
    __bdk_csr_fatal("GSERX_LANE_PX_MODE_0", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANE_PX_MODE_0(a,b) bdk_gserx_lane_px_mode_0_t
#define bustype_BDK_GSERX_LANE_PX_MODE_0(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANE_PX_MODE_0(a,b) "GSERX_LANE_PX_MODE_0"
#define device_bar_BDK_GSERX_LANE_PX_MODE_0(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANE_PX_MODE_0(a,b) (a)
#define arguments_BDK_GSERX_LANE_PX_MODE_0(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane_p#_mode_1
 *
 * GSER Lane Protocol Mode 1 Register
 * These are the RAW PCS lane settings mode 1 registers. There is one register per 4 lanes,
 * (0..3) per GSER per GSER_LMODE_E value (0..11). Only one entry is used at any given time in a
 * given
 * GSER lane - the one selected by the corresponding GSER()_LANE_MODE[LMODE].
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lane_px_mode_1
{
    uint64_t u;
    struct bdk_gserx_lane_px_mode_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t vma_fine_cfg_sel      : 1;  /**< [ 15: 15](R/W/H) Recommended settings:
                                                                 0 = Disabled. Coarse step adaptation selected (rates lower than 10.3125 Gbaud).
                                                                 1 = Enabled. Fine step adaptation selected (10.3125 Gbaud rate).

                                                                 For SATA, [VMA_FINE_CFG_SEL] should always be 0. */
        uint64_t vma_mm                : 1;  /**< [ 14: 14](R/W/H) Manual DFE verses adaptive DFE mode.

                                                                 Recommended settings:
                                                                 0 = Adaptive DFE (5 Gbaud and higher).
                                                                 1 = Manual DFE, fixed tap (3.125 Gbaud and lower).

                                                                 For SATA, [VMA_MM] should always be 1. */
        uint64_t cdr_fgain             : 4;  /**< [ 13: 10](R/W/H) CDR frequency gain.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ R_25G_REFCLK100:          0xA
                                                                 _ R_5G_REFCLK100:           0xA
                                                                 _ R_8G_REFCLK100:           0xB
                                                                 _ R_125G_REFCLK15625_KX:    0xC
                                                                 _ R_3125G_REFCLK15625_XAUI: 0xC
                                                                 _ R_103125G_REFCLK15625_KR: 0xA
                                                                 _ R_125G_REFCLK15625_SGMII: 0xC
                                                                 _ R_5G_REFCLK15625_QSGMII:  0xC
                                                                 _ R_625G_REFCLK15625_RXAUI: 0xA
                                                                 _ R_25G_REFCLK125:          0xA
                                                                 _ R_5G_REFCLK125:           0xA
                                                                 _ R_8G_REFCLK125:           0xB
                                                                 \</pre\>

                                                                 For SATA, [CDR_FGAIN] should always be 0xA. */
        uint64_t ph_acc_adj            : 10; /**< [  9:  0](R/W/H) Phase accumulator adjust.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ R_25G_REFCLK100:          0x14
                                                                 _ R_5G_REFCLK100:           0x14
                                                                 _ R_8G_REFCLK100:           0x23
                                                                 _ R_125G_REFCLK15625_KX:    0x1E
                                                                 _ R_3125G_REFCLK15625_XAUI: 0x1E
                                                                 _ R_103125G_REFCLK15625_KR: 0xF
                                                                 _ R_125G_REFCLK15625_SGMII: 0x1E
                                                                 _ R_5G_REFCLK15625_QSGMII:  0x1E
                                                                 _ R_625G_REFCLK15625_RXAUI: 0x14
                                                                 _ R_25G_REFCLK125:          0x14
                                                                 _ R_5G_REFCLK125:           0x14
                                                                 _ R_8G_REFCLK125:           0x23
                                                                 \</pre\>

                                                                 For SATA, [PH_ACC_ADJ] should always be 0x15.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
#else /* Word 0 - Little Endian */
        uint64_t ph_acc_adj            : 10; /**< [  9:  0](R/W/H) Phase accumulator adjust.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ R_25G_REFCLK100:          0x14
                                                                 _ R_5G_REFCLK100:           0x14
                                                                 _ R_8G_REFCLK100:           0x23
                                                                 _ R_125G_REFCLK15625_KX:    0x1E
                                                                 _ R_3125G_REFCLK15625_XAUI: 0x1E
                                                                 _ R_103125G_REFCLK15625_KR: 0xF
                                                                 _ R_125G_REFCLK15625_SGMII: 0x1E
                                                                 _ R_5G_REFCLK15625_QSGMII:  0x1E
                                                                 _ R_625G_REFCLK15625_RXAUI: 0x14
                                                                 _ R_25G_REFCLK125:          0x14
                                                                 _ R_5G_REFCLK125:           0x14
                                                                 _ R_8G_REFCLK125:           0x23
                                                                 \</pre\>

                                                                 For SATA, [PH_ACC_ADJ] should always be 0x15.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t cdr_fgain             : 4;  /**< [ 13: 10](R/W/H) CDR frequency gain.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ R_25G_REFCLK100:          0xA
                                                                 _ R_5G_REFCLK100:           0xA
                                                                 _ R_8G_REFCLK100:           0xB
                                                                 _ R_125G_REFCLK15625_KX:    0xC
                                                                 _ R_3125G_REFCLK15625_XAUI: 0xC
                                                                 _ R_103125G_REFCLK15625_KR: 0xA
                                                                 _ R_125G_REFCLK15625_SGMII: 0xC
                                                                 _ R_5G_REFCLK15625_QSGMII:  0xC
                                                                 _ R_625G_REFCLK15625_RXAUI: 0xA
                                                                 _ R_25G_REFCLK125:          0xA
                                                                 _ R_5G_REFCLK125:           0xA
                                                                 _ R_8G_REFCLK125:           0xB
                                                                 \</pre\>

                                                                 For SATA, [CDR_FGAIN] should always be 0xA. */
        uint64_t vma_mm                : 1;  /**< [ 14: 14](R/W/H) Manual DFE verses adaptive DFE mode.

                                                                 Recommended settings:
                                                                 0 = Adaptive DFE (5 Gbaud and higher).
                                                                 1 = Manual DFE, fixed tap (3.125 Gbaud and lower).

                                                                 For SATA, [VMA_MM] should always be 1. */
        uint64_t vma_fine_cfg_sel      : 1;  /**< [ 15: 15](R/W/H) Recommended settings:
                                                                 0 = Disabled. Coarse step adaptation selected (rates lower than 10.3125 Gbaud).
                                                                 1 = Enabled. Fine step adaptation selected (10.3125 Gbaud rate).

                                                                 For SATA, [VMA_FINE_CFG_SEL] should always be 0. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lane_px_mode_1_s cn; */
};
typedef union bdk_gserx_lane_px_mode_1 bdk_gserx_lane_px_mode_1_t;

static inline uint64_t BDK_GSERX_LANE_PX_MODE_1(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANE_PX_MODE_1(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=11)))
        return 0x87e0904e0048ll + 0x1000000ll * ((a) & 0x3) + 0x20ll * ((b) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=11)))
        return 0x87e0904e0048ll + 0x1000000ll * ((a) & 0x7) + 0x20ll * ((b) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=11)))
        return 0x87e0904e0048ll + 0x1000000ll * ((a) & 0xf) + 0x20ll * ((b) & 0xf);
    __bdk_csr_fatal("GSERX_LANE_PX_MODE_1", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_LANE_PX_MODE_1(a,b) bdk_gserx_lane_px_mode_1_t
#define bustype_BDK_GSERX_LANE_PX_MODE_1(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANE_PX_MODE_1(a,b) "GSERX_LANE_PX_MODE_1"
#define device_bar_BDK_GSERX_LANE_PX_MODE_1(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANE_PX_MODE_1(a,b) (a)
#define arguments_BDK_GSERX_LANE_PX_MODE_1(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_lane_poff
 *
 * GSER Lane Power Off Register
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lane_poff
{
    uint64_t u;
    struct bdk_gserx_lane_poff_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t lpoff                 : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode, allows for per lane power down.
                                                                 \<3\>: Lane 3.  Reserved.
                                                                 \<2\>: Lane 2.  Reserved.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t lpoff                 : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode, allows for per lane power down.
                                                                 \<3\>: Lane 3.  Reserved.
                                                                 \<2\>: Lane 2.  Reserved.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lane_poff_s cn81xx; */
    struct bdk_gserx_lane_poff_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t lpoff                 : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode (including all CCPI links), allows for per-lane power
                                                                 down.
                                                                 \<3\>: Lane 3.
                                                                 \<2\>: Lane 2.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t lpoff                 : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode (including all CCPI links), allows for per-lane power
                                                                 down.
                                                                 \<3\>: Lane 3.
                                                                 \<2\>: Lane 2.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_gserx_lane_poff_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t lpoff                 : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode, allows for per lane power
                                                                 down.
                                                                 \<3\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<2\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t lpoff                 : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode, allows for per lane power
                                                                 down.
                                                                 \<3\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<2\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_lane_poff bdk_gserx_lane_poff_t;

static inline uint64_t BDK_GSERX_LANE_POFF(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANE_POFF(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000108ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000108ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000108ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_LANE_POFF", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_LANE_POFF(a) bdk_gserx_lane_poff_t
#define bustype_BDK_GSERX_LANE_POFF(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANE_POFF(a) "GSERX_LANE_POFF"
#define device_bar_BDK_GSERX_LANE_POFF(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANE_POFF(a) (a)
#define arguments_BDK_GSERX_LANE_POFF(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_lane_srst
 *
 * GSER Lane Soft Reset Register
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lane_srst
{
    uint64_t u;
    struct bdk_gserx_lane_srst_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t lsrst                 : 1;  /**< [  0:  0](R/W) For links that are not in PCIE or SATA mode, resets all lanes
                                                                 (equivalent to the P2 power state) after any pending requests (power state change, rate
                                                                 change) are complete. The lanes remain in reset state while this signal is asserted. When
                                                                 the signal deasserts, the lanes exit the reset state and the PHY returns to the power
                                                                 state the PHY was in prior. For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t lsrst                 : 1;  /**< [  0:  0](R/W) For links that are not in PCIE or SATA mode, resets all lanes
                                                                 (equivalent to the P2 power state) after any pending requests (power state change, rate
                                                                 change) are complete. The lanes remain in reset state while this signal is asserted. When
                                                                 the signal deasserts, the lanes exit the reset state and the PHY returns to the power
                                                                 state the PHY was in prior. For diagnostic use only. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lane_srst_s cn81xx; */
    struct bdk_gserx_lane_srst_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t lsrst                 : 1;  /**< [  0:  0](R/W) For links that are not in PCIE or SATA mode (including all CCPI links), resets all lanes
                                                                 (equivalent to the P2 power state) after any pending requests (power state change, rate
                                                                 change) are complete. The lanes remain in reset state while this signal is asserted. When
                                                                 the signal deasserts, the lanes exit the reset state and the PHY returns to the power
                                                                 state the PHY was in prior. For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t lsrst                 : 1;  /**< [  0:  0](R/W) For links that are not in PCIE or SATA mode (including all CCPI links), resets all lanes
                                                                 (equivalent to the P2 power state) after any pending requests (power state change, rate
                                                                 change) are complete. The lanes remain in reset state while this signal is asserted. When
                                                                 the signal deasserts, the lanes exit the reset state and the PHY returns to the power
                                                                 state the PHY was in prior. For diagnostic use only. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } cn88xx;
    /* struct bdk_gserx_lane_srst_s cn83xx; */
};
typedef union bdk_gserx_lane_srst bdk_gserx_lane_srst_t;

static inline uint64_t BDK_GSERX_LANE_SRST(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANE_SRST(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000100ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000100ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000100ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_LANE_SRST", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_LANE_SRST(a) bdk_gserx_lane_srst_t
#define bustype_BDK_GSERX_LANE_SRST(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANE_SRST(a) "GSERX_LANE_SRST"
#define device_bar_BDK_GSERX_LANE_SRST(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANE_SRST(a) (a)
#define arguments_BDK_GSERX_LANE_SRST(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_lane_vma_coarse_ctrl_0
 *
 * GSER Lane VMA Coarse Control 0 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lane_vma_coarse_ctrl_0
{
    uint64_t u;
    struct bdk_gserx_lane_vma_coarse_ctrl_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t iq_max                : 4;  /**< [ 15: 12](R/W) Slice DLL IQ maximum value in VMA coarse mode. */
        uint64_t iq_min                : 4;  /**< [ 11:  8](R/W) Slice DLL IQ minimum value in VMA coarse mode. */
        uint64_t iq_step               : 2;  /**< [  7:  6](R/W) Slice DLL IQ step size in VMA coarse mode. */
        uint64_t window_wait           : 3;  /**< [  5:  3](R/W) Adaptation window wait setting in VMA coarse mode. */
        uint64_t lms_wait              : 3;  /**< [  2:  0](R/W/H) LMS wait time setting used to control the number of samples taken during the collection of
                                                                 statistics in VMA coarse mode. */
#else /* Word 0 - Little Endian */
        uint64_t lms_wait              : 3;  /**< [  2:  0](R/W/H) LMS wait time setting used to control the number of samples taken during the collection of
                                                                 statistics in VMA coarse mode. */
        uint64_t window_wait           : 3;  /**< [  5:  3](R/W) Adaptation window wait setting in VMA coarse mode. */
        uint64_t iq_step               : 2;  /**< [  7:  6](R/W) Slice DLL IQ step size in VMA coarse mode. */
        uint64_t iq_min                : 4;  /**< [ 11:  8](R/W) Slice DLL IQ minimum value in VMA coarse mode. */
        uint64_t iq_max                : 4;  /**< [ 15: 12](R/W) Slice DLL IQ maximum value in VMA coarse mode. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lane_vma_coarse_ctrl_0_s cn; */
};
typedef union bdk_gserx_lane_vma_coarse_ctrl_0 bdk_gserx_lane_vma_coarse_ctrl_0_t;

static inline uint64_t BDK_GSERX_LANE_VMA_COARSE_CTRL_0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANE_VMA_COARSE_CTRL_0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0904e01b0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0904e01b0ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0904e01b0ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_LANE_VMA_COARSE_CTRL_0", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_LANE_VMA_COARSE_CTRL_0(a) bdk_gserx_lane_vma_coarse_ctrl_0_t
#define bustype_BDK_GSERX_LANE_VMA_COARSE_CTRL_0(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANE_VMA_COARSE_CTRL_0(a) "GSERX_LANE_VMA_COARSE_CTRL_0"
#define device_bar_BDK_GSERX_LANE_VMA_COARSE_CTRL_0(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANE_VMA_COARSE_CTRL_0(a) (a)
#define arguments_BDK_GSERX_LANE_VMA_COARSE_CTRL_0(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_lane_vma_coarse_ctrl_1
 *
 * GSER Lane VMA Coarse Control 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lane_vma_coarse_ctrl_1
{
    uint64_t u;
    struct bdk_gserx_lane_vma_coarse_ctrl_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t ctle_pmax             : 4;  /**< [  9:  6](R/W) RX CTLE peak maximum value in VMA coarse mode. */
        uint64_t ctle_pmin             : 4;  /**< [  5:  2](R/W) RX CTLE peak minimum value in VMA coarse mode. */
        uint64_t ctle_pstep            : 2;  /**< [  1:  0](R/W) CTLE peak step size in VMA coarse mode. */
#else /* Word 0 - Little Endian */
        uint64_t ctle_pstep            : 2;  /**< [  1:  0](R/W) CTLE peak step size in VMA coarse mode. */
        uint64_t ctle_pmin             : 4;  /**< [  5:  2](R/W) RX CTLE peak minimum value in VMA coarse mode. */
        uint64_t ctle_pmax             : 4;  /**< [  9:  6](R/W) RX CTLE peak maximum value in VMA coarse mode. */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lane_vma_coarse_ctrl_1_s cn; */
};
typedef union bdk_gserx_lane_vma_coarse_ctrl_1 bdk_gserx_lane_vma_coarse_ctrl_1_t;

static inline uint64_t BDK_GSERX_LANE_VMA_COARSE_CTRL_1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANE_VMA_COARSE_CTRL_1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0904e01b8ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0904e01b8ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0904e01b8ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_LANE_VMA_COARSE_CTRL_1", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_LANE_VMA_COARSE_CTRL_1(a) bdk_gserx_lane_vma_coarse_ctrl_1_t
#define bustype_BDK_GSERX_LANE_VMA_COARSE_CTRL_1(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANE_VMA_COARSE_CTRL_1(a) "GSERX_LANE_VMA_COARSE_CTRL_1"
#define device_bar_BDK_GSERX_LANE_VMA_COARSE_CTRL_1(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANE_VMA_COARSE_CTRL_1(a) (a)
#define arguments_BDK_GSERX_LANE_VMA_COARSE_CTRL_1(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_lane_vma_coarse_ctrl_2
 *
 * GSER Lane VMA Fine Control 2 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lane_vma_coarse_ctrl_2
{
    uint64_t u;
    struct bdk_gserx_lane_vma_coarse_ctrl_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t pctle_gmax            : 4;  /**< [  9:  6](R/W) RX PRE-CTLE gain maximum value in VMA coarse mode. */
        uint64_t pctle_gmin            : 4;  /**< [  5:  2](R/W) RX PRE-CTLE gain minimum value in VMA coarse mode. */
        uint64_t pctle_gstep           : 2;  /**< [  1:  0](R/W) CTLE PRE-peak gain step size in VMA coarse mode. */
#else /* Word 0 - Little Endian */
        uint64_t pctle_gstep           : 2;  /**< [  1:  0](R/W) CTLE PRE-peak gain step size in VMA coarse mode. */
        uint64_t pctle_gmin            : 4;  /**< [  5:  2](R/W) RX PRE-CTLE gain minimum value in VMA coarse mode. */
        uint64_t pctle_gmax            : 4;  /**< [  9:  6](R/W) RX PRE-CTLE gain maximum value in VMA coarse mode. */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lane_vma_coarse_ctrl_2_s cn; */
};
typedef union bdk_gserx_lane_vma_coarse_ctrl_2 bdk_gserx_lane_vma_coarse_ctrl_2_t;

static inline uint64_t BDK_GSERX_LANE_VMA_COARSE_CTRL_2(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANE_VMA_COARSE_CTRL_2(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0904e01c0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0904e01c0ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0904e01c0ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_LANE_VMA_COARSE_CTRL_2", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_LANE_VMA_COARSE_CTRL_2(a) bdk_gserx_lane_vma_coarse_ctrl_2_t
#define bustype_BDK_GSERX_LANE_VMA_COARSE_CTRL_2(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANE_VMA_COARSE_CTRL_2(a) "GSERX_LANE_VMA_COARSE_CTRL_2"
#define device_bar_BDK_GSERX_LANE_VMA_COARSE_CTRL_2(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANE_VMA_COARSE_CTRL_2(a) (a)
#define arguments_BDK_GSERX_LANE_VMA_COARSE_CTRL_2(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_lane_vma_fine_ctrl_0
 *
 * GSER Lane VMA Fine Control 0 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lane_vma_fine_ctrl_0
{
    uint64_t u;
    struct bdk_gserx_lane_vma_fine_ctrl_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t rx_sdll_iq_max_fine   : 4;  /**< [ 15: 12](R/W) RX slice DLL IQ maximum value in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and
                                                                 GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t rx_sdll_iq_min_fine   : 4;  /**< [ 11:  8](R/W) RX slice DLL IQ minimum value in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and
                                                                 GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t rx_sdll_iq_step_fine  : 2;  /**< [  7:  6](R/W) RX slice DLL IQ step size in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and
                                                                 GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t vma_window_wait_fine  : 3;  /**< [  5:  3](R/W) Adaptation window wait setting (in VMA fine mode); used to control the number of samples
                                                                 taken during the collection of statistics (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t lms_wait_time_fine    : 3;  /**< [  2:  0](R/W) LMS wait time setting (in VMA fine mode); used to control the number of samples taken
                                                                 during the collection of statistics (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
#else /* Word 0 - Little Endian */
        uint64_t lms_wait_time_fine    : 3;  /**< [  2:  0](R/W) LMS wait time setting (in VMA fine mode); used to control the number of samples taken
                                                                 during the collection of statistics (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t vma_window_wait_fine  : 3;  /**< [  5:  3](R/W) Adaptation window wait setting (in VMA fine mode); used to control the number of samples
                                                                 taken during the collection of statistics (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t rx_sdll_iq_step_fine  : 2;  /**< [  7:  6](R/W) RX slice DLL IQ step size in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and
                                                                 GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t rx_sdll_iq_min_fine   : 4;  /**< [ 11:  8](R/W) RX slice DLL IQ minimum value in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and
                                                                 GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t rx_sdll_iq_max_fine   : 4;  /**< [ 15: 12](R/W) RX slice DLL IQ maximum value in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and
                                                                 GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lane_vma_fine_ctrl_0_s cn; */
};
typedef union bdk_gserx_lane_vma_fine_ctrl_0 bdk_gserx_lane_vma_fine_ctrl_0_t;

static inline uint64_t BDK_GSERX_LANE_VMA_FINE_CTRL_0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANE_VMA_FINE_CTRL_0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0904e01c8ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0904e01c8ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0904e01c8ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_LANE_VMA_FINE_CTRL_0", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_LANE_VMA_FINE_CTRL_0(a) bdk_gserx_lane_vma_fine_ctrl_0_t
#define bustype_BDK_GSERX_LANE_VMA_FINE_CTRL_0(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANE_VMA_FINE_CTRL_0(a) "GSERX_LANE_VMA_FINE_CTRL_0"
#define device_bar_BDK_GSERX_LANE_VMA_FINE_CTRL_0(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANE_VMA_FINE_CTRL_0(a) (a)
#define arguments_BDK_GSERX_LANE_VMA_FINE_CTRL_0(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_lane_vma_fine_ctrl_1
 *
 * GSER Lane VMA Fine Control 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lane_vma_fine_ctrl_1
{
    uint64_t u;
    struct bdk_gserx_lane_vma_fine_ctrl_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t rx_ctle_peak_max_fine : 4;  /**< [  9:  6](R/W) RX CTLE peak maximum value in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t rx_ctle_peak_min_fine : 4;  /**< [  5:  2](R/W) RX CTLE peak minimum value in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t rx_ctle_peak_step_fine : 2; /**< [  1:  0](R/W) RX CTLE Peak step size in VMA Fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
#else /* Word 0 - Little Endian */
        uint64_t rx_ctle_peak_step_fine : 2; /**< [  1:  0](R/W) RX CTLE Peak step size in VMA Fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t rx_ctle_peak_min_fine : 4;  /**< [  5:  2](R/W) RX CTLE peak minimum value in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t rx_ctle_peak_max_fine : 4;  /**< [  9:  6](R/W) RX CTLE peak maximum value in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lane_vma_fine_ctrl_1_s cn; */
};
typedef union bdk_gserx_lane_vma_fine_ctrl_1 bdk_gserx_lane_vma_fine_ctrl_1_t;

static inline uint64_t BDK_GSERX_LANE_VMA_FINE_CTRL_1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANE_VMA_FINE_CTRL_1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0904e01d0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0904e01d0ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0904e01d0ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_LANE_VMA_FINE_CTRL_1", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_LANE_VMA_FINE_CTRL_1(a) bdk_gserx_lane_vma_fine_ctrl_1_t
#define bustype_BDK_GSERX_LANE_VMA_FINE_CTRL_1(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANE_VMA_FINE_CTRL_1(a) "GSERX_LANE_VMA_FINE_CTRL_1"
#define device_bar_BDK_GSERX_LANE_VMA_FINE_CTRL_1(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANE_VMA_FINE_CTRL_1(a) (a)
#define arguments_BDK_GSERX_LANE_VMA_FINE_CTRL_1(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_lane_vma_fine_ctrl_2
 *
 * GSER Lane VMA Fine Control 2 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_lane_vma_fine_ctrl_2
{
    uint64_t u;
    struct bdk_gserx_lane_vma_fine_ctrl_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_10_63        : 54;
        uint64_t rx_prectle_gain_max_fine : 4;/**< [  9:  6](R/W) RX PRE-CTLE gain maximum value in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t rx_prectle_gain_min_fine : 4;/**< [  5:  2](R/W) RX PRE-CTLE gain minimum value in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t rx_prectle_gain_step_fine : 2;/**< [  1:  0](R/W) RX PRE-CTLE gain step size in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
#else /* Word 0 - Little Endian */
        uint64_t rx_prectle_gain_step_fine : 2;/**< [  1:  0](R/W) RX PRE-CTLE gain step size in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t rx_prectle_gain_min_fine : 4;/**< [  5:  2](R/W) RX PRE-CTLE gain minimum value in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t rx_prectle_gain_max_fine : 4;/**< [  9:  6](R/W) RX PRE-CTLE gain maximum value in VMA fine mode (valid when
                                                                 GSER()_LANE_P()_MODE_1[VMA_FINE_CFG_SEL]=1 and GSER()_LANE_P()_MODE_1[VMA_MM]=0). */
        uint64_t reserved_10_63        : 54;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_lane_vma_fine_ctrl_2_s cn; */
};
typedef union bdk_gserx_lane_vma_fine_ctrl_2 bdk_gserx_lane_vma_fine_ctrl_2_t;

static inline uint64_t BDK_GSERX_LANE_VMA_FINE_CTRL_2(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_LANE_VMA_FINE_CTRL_2(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0904e01d8ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0904e01d8ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0904e01d8ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_LANE_VMA_FINE_CTRL_2", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_LANE_VMA_FINE_CTRL_2(a) bdk_gserx_lane_vma_fine_ctrl_2_t
#define bustype_BDK_GSERX_LANE_VMA_FINE_CTRL_2(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_LANE_VMA_FINE_CTRL_2(a) "GSERX_LANE_VMA_FINE_CTRL_2"
#define device_bar_BDK_GSERX_LANE_VMA_FINE_CTRL_2(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_LANE_VMA_FINE_CTRL_2(a) (a)
#define arguments_BDK_GSERX_LANE_VMA_FINE_CTRL_2(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_phy_ctl
 *
 * GSER PHY Control Register
 * This register contains general PHY/PLL control of the RAW PCS.
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_phy_ctl
{
    uint64_t u;
    struct bdk_gserx_phy_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t phy_reset             : 1;  /**< [  1:  1](R/W/H) When asserted, the PHY is held in reset. This bit is initialized as follows:
                                                                 0 = (not reset) = Bootable PCIe.
                                                                 1 = (reset) = Non-bootable PCIe, BGX, or SATA. */
        uint64_t phy_pd                : 1;  /**< [  0:  0](R/W) When asserted, the PHY is powered down. */
#else /* Word 0 - Little Endian */
        uint64_t phy_pd                : 1;  /**< [  0:  0](R/W) When asserted, the PHY is powered down. */
        uint64_t phy_reset             : 1;  /**< [  1:  1](R/W/H) When asserted, the PHY is held in reset. This bit is initialized as follows:
                                                                 0 = (not reset) = Bootable PCIe.
                                                                 1 = (reset) = Non-bootable PCIe, BGX, or SATA. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_phy_ctl_s cn81xx; */
    struct bdk_gserx_phy_ctl_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t phy_reset             : 1;  /**< [  1:  1](R/W/H) When asserted, the PHY is held in reset. This bit is initialized as follows:
                                                                 0 = (not reset) = Bootable PCIe, or CCPI when GSER(8..13)_SPD[SPD] comes up in a bootable
                                                                 mode.
                                                                 1 = (reset) = Non-bootable PCIe, BGX, SATA or CCPI when GSER(8..13)_SPD[SPD] comes up in
                                                                 SW_MODE. */
        uint64_t phy_pd                : 1;  /**< [  0:  0](R/W) When asserted, the PHY is powered down. */
#else /* Word 0 - Little Endian */
        uint64_t phy_pd                : 1;  /**< [  0:  0](R/W) When asserted, the PHY is powered down. */
        uint64_t phy_reset             : 1;  /**< [  1:  1](R/W/H) When asserted, the PHY is held in reset. This bit is initialized as follows:
                                                                 0 = (not reset) = Bootable PCIe, or CCPI when GSER(8..13)_SPD[SPD] comes up in a bootable
                                                                 mode.
                                                                 1 = (reset) = Non-bootable PCIe, BGX, SATA or CCPI when GSER(8..13)_SPD[SPD] comes up in
                                                                 SW_MODE. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } cn88xx;
    /* struct bdk_gserx_phy_ctl_s cn83xx; */
};
typedef union bdk_gserx_phy_ctl bdk_gserx_phy_ctl_t;

static inline uint64_t BDK_GSERX_PHY_CTL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_PHY_CTL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000000ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000000ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000000ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_PHY_CTL", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_PHY_CTL(a) bdk_gserx_phy_ctl_t
#define bustype_BDK_GSERX_PHY_CTL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_PHY_CTL(a) "GSERX_PHY_CTL"
#define device_bar_BDK_GSERX_PHY_CTL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_PHY_CTL(a) (a)
#define arguments_BDK_GSERX_PHY_CTL(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_pipe_lpbk
 *
 * GSER PCIE PCS PIPE Lookback Register
 */
union bdk_gserx_pipe_lpbk
{
    uint64_t u;
    struct bdk_gserx_pipe_lpbk_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t pcie_lpbk             : 1;  /**< [  0:  0](R/W) For links that are in PCIE mode, places the PHY in serial loopback mode, where the
                                                                 QLMn_TXN/QLMn_TXP data are looped back to the QLMn_RXN/QLMn_RXP.

                                                                 This register has no meaning for links that don't support PCIe. */
#else /* Word 0 - Little Endian */
        uint64_t pcie_lpbk             : 1;  /**< [  0:  0](R/W) For links that are in PCIE mode, places the PHY in serial loopback mode, where the
                                                                 QLMn_TXN/QLMn_TXP data are looped back to the QLMn_RXN/QLMn_RXP.

                                                                 This register has no meaning for links that don't support PCIe. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_pipe_lpbk_s cn; */
};
typedef union bdk_gserx_pipe_lpbk bdk_gserx_pipe_lpbk_t;

static inline uint64_t BDK_GSERX_PIPE_LPBK(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_PIPE_LPBK(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000200ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000200ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000200ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_PIPE_LPBK", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_PIPE_LPBK(a) bdk_gserx_pipe_lpbk_t
#define bustype_BDK_GSERX_PIPE_LPBK(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_PIPE_LPBK(a) "GSERX_PIPE_LPBK"
#define device_bar_BDK_GSERX_PIPE_LPBK(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_PIPE_LPBK(a) (a)
#define arguments_BDK_GSERX_PIPE_LPBK(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_pll_p#_mode_0
 *
 * GSER PLL Protocol Mode 0 Register
 * These are the RAW PCS PLL global settings mode 0 registers. There is one register per GSER per
 * GSER_LMODE_E value (0..11). Only one entry is used at any given time in a given GSER - the one
 * selected by the corresponding GSER()_LANE_MODE[LMODE].
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during subsequent chip warm or
 * soft resets.
 */
union bdk_gserx_pll_px_mode_0
{
    uint64_t u;
    struct bdk_gserx_pll_px_mode_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t pll_icp               : 4;  /**< [ 15: 12](R/W/H) PLL charge pump enable.

                                                                 Recommended settings, which are based on the reference clock speed:

                                                                 \<pre\>
                                                                          100MHz 125MHz 156.25MHz
                                                                 1.25G:    0x1    0x1    0x1
                                                                 2.5G:     0x4    0x3    0x3
                                                                 3.125G:   NS     0x1    0x1
                                                                 5.0G:     0x1    0x1    0x1
                                                                 6.25G:    NS     0x1    0x1
                                                                 8.0G:     0x3    0x2    NS
                                                                 10.3125G: NS     NS     0x1
                                                                 \</pre\>

                                                                 For SATA, [PLL_ICP] should always be 1.
                                                                 For PCIE 1.1 @100 MHz, [PLL_ICP] should be 4.
                                                                 For PCIE 2.1 @100 MHz, [PLL_ICP] should be 4.
                                                                 For PCIE 1.1 @125 MHz, [PLL_ICP] should be 3.
                                                                 For PCIE 2.1 @125 MHz, [PLL_ICP] should be 3.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t pll_rloop             : 3;  /**< [ 11:  9](R/W/H) Loop resistor tuning.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ 1.25G:    0x3
                                                                 _ 2.5G:     0x3
                                                                 _ 3.125G:   0x3
                                                                 _ 5.0G:     0x3
                                                                 _ 6.25G:    0x3
                                                                 _ 8.0G:     0x5
                                                                 _ 10.3125G: 0x5
                                                                 \</pre\>

                                                                 For SATA with 100 MHz reference clock, [PLL_RLOOP] should always be 3. */
        uint64_t pll_pcs_div           : 9;  /**< [  8:  0](R/W/H) The divider that generates PCS_MAC_TX_CLK. The frequency of the clock is (pll_frequency /
                                                                 PLL_PCS_DIV).

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                             PCIE   Other
                                                                 _ 1.25G:     NS     0x28
                                                                 _ 2.5G:      0x5    0x5
                                                                 _ 3.125G:    NS     0x14
                                                                 _ 5.0G:      0x5    0xA
                                                                 _ 6.25G:     NS     0xA
                                                                 _ 8.0G:      0x8    0xA
                                                                 _ 10.3125G:  NS     0xA
                                                                 \</pre\>

                                                                 For SATA, [PLL_PCS_DIV] should always be 5.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
#else /* Word 0 - Little Endian */
        uint64_t pll_pcs_div           : 9;  /**< [  8:  0](R/W/H) The divider that generates PCS_MAC_TX_CLK. The frequency of the clock is (pll_frequency /
                                                                 PLL_PCS_DIV).

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                             PCIE   Other
                                                                 _ 1.25G:     NS     0x28
                                                                 _ 2.5G:      0x5    0x5
                                                                 _ 3.125G:    NS     0x14
                                                                 _ 5.0G:      0x5    0xA
                                                                 _ 6.25G:     NS     0xA
                                                                 _ 8.0G:      0x8    0xA
                                                                 _ 10.3125G:  NS     0xA
                                                                 \</pre\>

                                                                 For SATA, [PLL_PCS_DIV] should always be 5.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t pll_rloop             : 3;  /**< [ 11:  9](R/W/H) Loop resistor tuning.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ 1.25G:    0x3
                                                                 _ 2.5G:     0x3
                                                                 _ 3.125G:   0x3
                                                                 _ 5.0G:     0x3
                                                                 _ 6.25G:    0x3
                                                                 _ 8.0G:     0x5
                                                                 _ 10.3125G: 0x5
                                                                 \</pre\>

                                                                 For SATA with 100 MHz reference clock, [PLL_RLOOP] should always be 3. */
        uint64_t pll_icp               : 4;  /**< [ 15: 12](R/W/H) PLL charge pump enable.

                                                                 Recommended settings, which are based on the reference clock speed:

                                                                 \<pre\>
                                                                          100MHz 125MHz 156.25MHz
                                                                 1.25G:    0x1    0x1    0x1
                                                                 2.5G:     0x4    0x3    0x3
                                                                 3.125G:   NS     0x1    0x1
                                                                 5.0G:     0x1    0x1    0x1
                                                                 6.25G:    NS     0x1    0x1
                                                                 8.0G:     0x3    0x2    NS
                                                                 10.3125G: NS     NS     0x1
                                                                 \</pre\>

                                                                 For SATA, [PLL_ICP] should always be 1.
                                                                 For PCIE 1.1 @100 MHz, [PLL_ICP] should be 4.
                                                                 For PCIE 2.1 @100 MHz, [PLL_ICP] should be 4.
                                                                 For PCIE 1.1 @125 MHz, [PLL_ICP] should be 3.
                                                                 For PCIE 2.1 @125 MHz, [PLL_ICP] should be 3.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_pll_px_mode_0_s cn81xx; */
    /* struct bdk_gserx_pll_px_mode_0_s cn88xx; */
    struct bdk_gserx_pll_px_mode_0_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t pll_icp               : 4;  /**< [ 15: 12](R/W/H) PLL charge pump enable.

                                                                 Recommended settings, which are based on the reference clock speed:

                                                                 \<pre\>
                                                                          100MHz 125MHz 156.25MHz
                                                                 1.25G:    0x1    0x1    0x1
                                                                 2.5G:     0x4    0x3    0x3
                                                                 3.125G:   NS     0x1    0x1
                                                                 5.0G:     0x1    0x1    0x1
                                                                 6.25G:    NS     0x1    0x1
                                                                 8.0G:     0x3    0x2    NS
                                                                 10.3125G: NS     NS     0x1
                                                                 \</pre\>

                                                                 For SATA, [PLL_ICP] should always be 1.
                                                                 For PCIE 1.1 @100MHz, [PLL_ICP] should be 4.
                                                                 For PCIE 2.1 @100MHz, [PLL_ICP] should be 4.
                                                                 For PCIE 1.1 @125MHz, [PLL_ICP] should be 3.
                                                                 For PCIE 2.1 @125MHz, [PLL_ICP] should be 3.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t pll_rloop             : 3;  /**< [ 11:  9](R/W/H) Loop resistor tuning.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ 1.25G:    0x3
                                                                 _ 2.5G:     0x3
                                                                 _ 3.125G:   0x3
                                                                 _ 5.0G:     0x3
                                                                 _ 6.25G:    0x3
                                                                 _ 8.0G:     0x5
                                                                 _ 10.3125G: 0x5
                                                                 \</pre\>

                                                                 For SATA with 100 MHz reference clock, [PLL_RLOOP] should always be 3. */
        uint64_t pll_pcs_div           : 9;  /**< [  8:  0](R/W/H) The divider that generates PCS_MAC_TX_CLK. The frequency of the clock is (pll_frequency /
                                                                 PLL_PCS_DIV).

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                             PCIE   Other
                                                                 _ 1.25G:     NS     0x28
                                                                 _ 2.5G:      0x5    0x5
                                                                 _ 3.125G:    NS     0x14
                                                                 _ 5.0G:      0x5    0xA
                                                                 _ 6.25G:     NS     0xA
                                                                 _ 8.0G:      0x8    0xA
                                                                 _ 10.3125G:  NS     0xA
                                                                 \</pre\>

                                                                 For SATA, [PLL_PCS_DIV] should always be 5.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
#else /* Word 0 - Little Endian */
        uint64_t pll_pcs_div           : 9;  /**< [  8:  0](R/W/H) The divider that generates PCS_MAC_TX_CLK. The frequency of the clock is (pll_frequency /
                                                                 PLL_PCS_DIV).

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                             PCIE   Other
                                                                 _ 1.25G:     NS     0x28
                                                                 _ 2.5G:      0x5    0x5
                                                                 _ 3.125G:    NS     0x14
                                                                 _ 5.0G:      0x5    0xA
                                                                 _ 6.25G:     NS     0xA
                                                                 _ 8.0G:      0x8    0xA
                                                                 _ 10.3125G:  NS     0xA
                                                                 \</pre\>

                                                                 For SATA, [PLL_PCS_DIV] should always be 5.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t pll_rloop             : 3;  /**< [ 11:  9](R/W/H) Loop resistor tuning.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                 _ 1.25G:    0x3
                                                                 _ 2.5G:     0x3
                                                                 _ 3.125G:   0x3
                                                                 _ 5.0G:     0x3
                                                                 _ 6.25G:    0x3
                                                                 _ 8.0G:     0x5
                                                                 _ 10.3125G: 0x5
                                                                 \</pre\>

                                                                 For SATA with 100 MHz reference clock, [PLL_RLOOP] should always be 3. */
        uint64_t pll_icp               : 4;  /**< [ 15: 12](R/W/H) PLL charge pump enable.

                                                                 Recommended settings, which are based on the reference clock speed:

                                                                 \<pre\>
                                                                          100MHz 125MHz 156.25MHz
                                                                 1.25G:    0x1    0x1    0x1
                                                                 2.5G:     0x4    0x3    0x3
                                                                 3.125G:   NS     0x1    0x1
                                                                 5.0G:     0x1    0x1    0x1
                                                                 6.25G:    NS     0x1    0x1
                                                                 8.0G:     0x3    0x2    NS
                                                                 10.3125G: NS     NS     0x1
                                                                 \</pre\>

                                                                 For SATA, [PLL_ICP] should always be 1.
                                                                 For PCIE 1.1 @100MHz, [PLL_ICP] should be 4.
                                                                 For PCIE 2.1 @100MHz, [PLL_ICP] should be 4.
                                                                 For PCIE 1.1 @125MHz, [PLL_ICP] should be 3.
                                                                 For PCIE 2.1 @125MHz, [PLL_ICP] should be 3.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_pll_px_mode_0 bdk_gserx_pll_px_mode_0_t;

static inline uint64_t BDK_GSERX_PLL_PX_MODE_0(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_PLL_PX_MODE_0(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=11)))
        return 0x87e0904e0030ll + 0x1000000ll * ((a) & 0x3) + 0x20ll * ((b) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=11)))
        return 0x87e0904e0030ll + 0x1000000ll * ((a) & 0x7) + 0x20ll * ((b) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=11)))
        return 0x87e0904e0030ll + 0x1000000ll * ((a) & 0xf) + 0x20ll * ((b) & 0xf);
    __bdk_csr_fatal("GSERX_PLL_PX_MODE_0", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_PLL_PX_MODE_0(a,b) bdk_gserx_pll_px_mode_0_t
#define bustype_BDK_GSERX_PLL_PX_MODE_0(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_PLL_PX_MODE_0(a,b) "GSERX_PLL_PX_MODE_0"
#define device_bar_BDK_GSERX_PLL_PX_MODE_0(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_PLL_PX_MODE_0(a,b) (a)
#define arguments_BDK_GSERX_PLL_PX_MODE_0(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_pll_p#_mode_1
 *
 * GSER PLL Protocol Mode 1 Register
 * These are the RAW PCS PLL global settings mode 1 registers. There is one register per GSER per
 * GSER_LMODE_E value (0..11). Only one entry is used at any given time in a given GSER - the one
 * selected by the corresponding GSER()_LANE_MODE[LMODE].
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in this register do not change during subsequent chip warm or
 * soft resets.
 */
union bdk_gserx_pll_px_mode_1
{
    uint64_t u;
    struct bdk_gserx_pll_px_mode_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t pll_16p5en            : 1;  /**< [ 13: 13](R/W/H) Enable for the DIV 16.5 divided down clock.

                                                                 Recommended settings, based on the reference clock speed:

                                                                 \<pre\>
                                                                          100MHz 125MHz 156.25MHz
                                                                 1.25G:    0x1    0x1     0x1
                                                                 2.5G:     0x0    0x0     0x0
                                                                 3.125G:   NS     0x1     0x1
                                                                 5.0G:     0x0    0x0     0x0
                                                                 6.25G:    NS     0x0     0x0
                                                                 8.0G:     0x0    0x0     NS
                                                                 10.3125G: NS     NS      0x1
                                                                 \</pre\>

                                                                 For SATA, [PLL_16P5EN] should always be 0.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t pll_cpadj             : 2;  /**< [ 12: 11](R/W/H) PLL charge adjust.

                                                                 Recommended settings, based on the reference clock speed:

                                                                 \<pre\>
                                                                           100MHz 125MHz 156.25MHz
                                                                 1.25G:     0x2     0x2    0x3
                                                                 2.5G:      0x2     0x1    0x2
                                                                 3.125G:    NS      0x2    0x2
                                                                 5.0G:      0x2     0x2    0x2
                                                                 6.25G:     NS      0x2    0x2
                                                                 8.0G:      0x2     0x1    NS
                                                                 10.3125G:  NS      NS     0x2
                                                                 \</pre\>

                                                                 For SATA with 100 MHz reference clock, [PLL_CPADJ] should always be 2.
                                                                 For PCIE 1.1 @100MHz, [PLL_CPADJ] should be 2.
                                                                 For PCIE 2.1 @100MHz, [PLL_CPADJ] should be 2.
                                                                 For PCIE 1.1 @125MHz, [PLL_CPADJ] should be 1.
                                                                 For PCIE 2.1 @125MHz, [PLL_CPADJ] should be 1.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t pll_pcie3en           : 1;  /**< [ 10: 10](R/W/H) Enable PCIE3 mode.

                                                                 Recommended settings:
                                                                 0 = Any rate other than 8 Gbaud.
                                                                 1 = Rate is equal to 8 Gbaud.

                                                                 For SATA, [PLL_PCIE3EN] should always be 0. */
        uint64_t pll_opr               : 1;  /**< [  9:  9](R/W/H) PLL op range:
                                                                 0 = Use ring oscillator VCO. Recommended for rates 6.25 Gbaud and lower.
                                                                 1 = Use LC-tank VCO. Recommended for rates 8 Gbaud and higher.

                                                                 For SATA, [PLL_OPR] should always be 0. */
        uint64_t pll_div               : 9;  /**< [  8:  0](R/W/H) PLL divider in feedback path which sets the PLL frequency.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                          100MHz 125MHz 156.25MHz
                                                                 1.25G:    0x19   0x14    0x10
                                                                 2.5G:     0x19   0x14    0x10
                                                                 3.125G:   NS     0x19    0x14
                                                                 5.0G:     0x19   0x14    0x10
                                                                 6.25G:    NS     0x19    0x14
                                                                 8.0G:     0x28   0x20    NS
                                                                 10.3125G: NS     NS      0x21
                                                                 \</pre\>

                                                                 For SATA with 100MHz reference clock, [PLL_DIV] should always be 0x1E.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
#else /* Word 0 - Little Endian */
        uint64_t pll_div               : 9;  /**< [  8:  0](R/W/H) PLL divider in feedback path which sets the PLL frequency.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                          100MHz 125MHz 156.25MHz
                                                                 1.25G:    0x19   0x14    0x10
                                                                 2.5G:     0x19   0x14    0x10
                                                                 3.125G:   NS     0x19    0x14
                                                                 5.0G:     0x19   0x14    0x10
                                                                 6.25G:    NS     0x19    0x14
                                                                 8.0G:     0x28   0x20    NS
                                                                 10.3125G: NS     NS      0x21
                                                                 \</pre\>

                                                                 For SATA with 100MHz reference clock, [PLL_DIV] should always be 0x1E.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t pll_opr               : 1;  /**< [  9:  9](R/W/H) PLL op range:
                                                                 0 = Use ring oscillator VCO. Recommended for rates 6.25 Gbaud and lower.
                                                                 1 = Use LC-tank VCO. Recommended for rates 8 Gbaud and higher.

                                                                 For SATA, [PLL_OPR] should always be 0. */
        uint64_t pll_pcie3en           : 1;  /**< [ 10: 10](R/W/H) Enable PCIE3 mode.

                                                                 Recommended settings:
                                                                 0 = Any rate other than 8 Gbaud.
                                                                 1 = Rate is equal to 8 Gbaud.

                                                                 For SATA, [PLL_PCIE3EN] should always be 0. */
        uint64_t pll_cpadj             : 2;  /**< [ 12: 11](R/W/H) PLL charge adjust.

                                                                 Recommended settings, based on the reference clock speed:

                                                                 \<pre\>
                                                                           100MHz 125MHz 156.25MHz
                                                                 1.25G:     0x2     0x2    0x3
                                                                 2.5G:      0x2     0x1    0x2
                                                                 3.125G:    NS      0x2    0x2
                                                                 5.0G:      0x2     0x2    0x2
                                                                 6.25G:     NS      0x2    0x2
                                                                 8.0G:      0x2     0x1    NS
                                                                 10.3125G:  NS      NS     0x2
                                                                 \</pre\>

                                                                 For SATA with 100 MHz reference clock, [PLL_CPADJ] should always be 2.
                                                                 For PCIE 1.1 @100MHz, [PLL_CPADJ] should be 2.
                                                                 For PCIE 2.1 @100MHz, [PLL_CPADJ] should be 2.
                                                                 For PCIE 1.1 @125MHz, [PLL_CPADJ] should be 1.
                                                                 For PCIE 2.1 @125MHz, [PLL_CPADJ] should be 1.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t pll_16p5en            : 1;  /**< [ 13: 13](R/W/H) Enable for the DIV 16.5 divided down clock.

                                                                 Recommended settings, based on the reference clock speed:

                                                                 \<pre\>
                                                                          100MHz 125MHz 156.25MHz
                                                                 1.25G:    0x1    0x1     0x1
                                                                 2.5G:     0x0    0x0     0x0
                                                                 3.125G:   NS     0x1     0x1
                                                                 5.0G:     0x0    0x0     0x0
                                                                 6.25G:    NS     0x0     0x0
                                                                 8.0G:     0x0    0x0     NS
                                                                 10.3125G: NS     NS      0x1
                                                                 \</pre\>

                                                                 For SATA, [PLL_16P5EN] should always be 0.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_pll_px_mode_1_s cn81xx; */
    /* struct bdk_gserx_pll_px_mode_1_s cn88xx; */
    struct bdk_gserx_pll_px_mode_1_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t pll_16p5en            : 1;  /**< [ 13: 13](R/W/H) Enable for the DIV 16.5 divided down clock.

                                                                 Recommended settings, based on the reference clock speed:

                                                                 \<pre\>
                                                                          100MHz 125MHz 156.25MHz
                                                                 1.25G:    0x1    0x1     0x1
                                                                 2.5G:     0x0    0x0     0x0
                                                                 3.125G:   NS     0x1     0x1
                                                                 5.0G:     0x0    0x0     0x0
                                                                 6.25G:    NS     0x0     0x0
                                                                 8.0G:     0x0    0x0     NS
                                                                 10.3125G: NS     NS      0x1
                                                                 \</pre\>

                                                                 For SATA, [PLL_16P5EN] should always be 0.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t pll_cpadj             : 2;  /**< [ 12: 11](R/W/H) PLL charge adjust.

                                                                 Recommended settings, based on the reference clock speed:

                                                                 \<pre\>
                                                                           100MHz 125MHz 156.25MHz
                                                                 1.25G:     0x2     0x2    0x3
                                                                 2.5G:      0x2     0x1    0x2
                                                                 3.125G:    NS      0x2    0x2
                                                                 5.0G:      0x2     0x2    0x2
                                                                 6.25G:     NS      0x2    0x2
                                                                 8.0G:      0x2     0x1    NS
                                                                 10.3125G:  NS      NS     0x2
                                                                 \</pre\>

                                                                 For SATA with 100 MHz reference clock, [PLL_CPADJ] should always be 2.
                                                                 For PCIE 1.1 @100MHz, [PLL_CPADJ] should be 2.
                                                                 For PCIE 2.1 @100MHz, [PLL_CPADJ] should be 2.
                                                                 For PCIE 1.1 @125MHz, [PLL_CPADJ] should be 1.
                                                                 For PCIE 2.1 @125MHz, [PLL_CPADJ] should be 1.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t pll_pcie3en           : 1;  /**< [ 10: 10](R/W/H) Enable PCIE3 mode.

                                                                 Recommended settings:
                                                                 0 = Any rate other than 8 Gbaud.
                                                                 1 = Rate is equal to 8 Gbaud.

                                                                 For SATA, [PLL_PCIE3EN] should always be 0. */
        uint64_t pll_opr               : 1;  /**< [  9:  9](R/W/H) PLL op range:
                                                                 0 = Use ring oscillator VCO. Recommended for rates 6.25 Gbaud and lower.
                                                                 1 = Use LC-tank VCO. Recommended for rates 8 Gbaud and higher.

                                                                 For SATA, [PLL_OPR] should always be 0. */
        uint64_t pll_div               : 9;  /**< [  8:  0](R/W/H) PLL divider in feedback path which sets the PLL frequency.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                          100MHz 125MHz 156.25MHz
                                                                 1.25G:    0x19   0x14    0x10
                                                                 2.5G:     0x19   0x14    0x10
                                                                 3.125G:   NS     0x19    0x14
                                                                 5.0G:     0x19   0x14    0x10
                                                                 6.25G:    NS     0x19    0x14
                                                                 8.0G:     0x28   0x20    NS
                                                                 10.3125G: NS     NS      0x21
                                                                 \</pre\>

                                                                 For SATA with 100 MHz reference clock, [PLL_DIV] should always be 0x1E.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
#else /* Word 0 - Little Endian */
        uint64_t pll_div               : 9;  /**< [  8:  0](R/W/H) PLL divider in feedback path which sets the PLL frequency.

                                                                 Recommended settings:

                                                                 \<pre\>
                                                                          100MHz 125MHz 156.25MHz
                                                                 1.25G:    0x19   0x14    0x10
                                                                 2.5G:     0x19   0x14    0x10
                                                                 3.125G:   NS     0x19    0x14
                                                                 5.0G:     0x19   0x14    0x10
                                                                 6.25G:    NS     0x19    0x14
                                                                 8.0G:     0x28   0x20    NS
                                                                 10.3125G: NS     NS      0x21
                                                                 \</pre\>

                                                                 For SATA with 100 MHz reference clock, [PLL_DIV] should always be 0x1E.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t pll_opr               : 1;  /**< [  9:  9](R/W/H) PLL op range:
                                                                 0 = Use ring oscillator VCO. Recommended for rates 6.25 Gbaud and lower.
                                                                 1 = Use LC-tank VCO. Recommended for rates 8 Gbaud and higher.

                                                                 For SATA, [PLL_OPR] should always be 0. */
        uint64_t pll_pcie3en           : 1;  /**< [ 10: 10](R/W/H) Enable PCIE3 mode.

                                                                 Recommended settings:
                                                                 0 = Any rate other than 8 Gbaud.
                                                                 1 = Rate is equal to 8 Gbaud.

                                                                 For SATA, [PLL_PCIE3EN] should always be 0. */
        uint64_t pll_cpadj             : 2;  /**< [ 12: 11](R/W/H) PLL charge adjust.

                                                                 Recommended settings, based on the reference clock speed:

                                                                 \<pre\>
                                                                           100MHz 125MHz 156.25MHz
                                                                 1.25G:     0x2     0x2    0x3
                                                                 2.5G:      0x2     0x1    0x2
                                                                 3.125G:    NS      0x2    0x2
                                                                 5.0G:      0x2     0x2    0x2
                                                                 6.25G:     NS      0x2    0x2
                                                                 8.0G:      0x2     0x1    NS
                                                                 10.3125G:  NS      NS     0x2
                                                                 \</pre\>

                                                                 For SATA with 100 MHz reference clock, [PLL_CPADJ] should always be 2.
                                                                 For PCIE 1.1 @100MHz, [PLL_CPADJ] should be 2.
                                                                 For PCIE 2.1 @100MHz, [PLL_CPADJ] should be 2.
                                                                 For PCIE 1.1 @125MHz, [PLL_CPADJ] should be 1.
                                                                 For PCIE 2.1 @125MHz, [PLL_CPADJ] should be 1.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t pll_16p5en            : 1;  /**< [ 13: 13](R/W/H) Enable for the DIV 16.5 divided down clock.

                                                                 Recommended settings, based on the reference clock speed:

                                                                 \<pre\>
                                                                          100MHz 125MHz 156.25MHz
                                                                 1.25G:    0x1    0x1     0x1
                                                                 2.5G:     0x0    0x0     0x0
                                                                 3.125G:   NS     0x1     0x1
                                                                 5.0G:     0x0    0x0     0x0
                                                                 6.25G:    NS     0x0     0x0
                                                                 8.0G:     0x0    0x0     NS
                                                                 10.3125G: NS     NS      0x1
                                                                 \</pre\>

                                                                 For SATA, [PLL_16P5EN] should always be 0.

                                                                 A 'NS' indicates that the rate is not supported at the specified reference clock. */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_pll_px_mode_1 bdk_gserx_pll_px_mode_1_t;

static inline uint64_t BDK_GSERX_PLL_PX_MODE_1(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_PLL_PX_MODE_1(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=11)))
        return 0x87e0904e0038ll + 0x1000000ll * ((a) & 0x3) + 0x20ll * ((b) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=11)))
        return 0x87e0904e0038ll + 0x1000000ll * ((a) & 0x7) + 0x20ll * ((b) & 0xf);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=11)))
        return 0x87e0904e0038ll + 0x1000000ll * ((a) & 0xf) + 0x20ll * ((b) & 0xf);
    __bdk_csr_fatal("GSERX_PLL_PX_MODE_1", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_PLL_PX_MODE_1(a,b) bdk_gserx_pll_px_mode_1_t
#define bustype_BDK_GSERX_PLL_PX_MODE_1(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_PLL_PX_MODE_1(a,b) "GSERX_PLL_PX_MODE_1"
#define device_bar_BDK_GSERX_PLL_PX_MODE_1(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_PLL_PX_MODE_1(a,b) (a)
#define arguments_BDK_GSERX_PLL_PX_MODE_1(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_pll_stat
 *
 * GSER PLL Status Register
 */
union bdk_gserx_pll_stat
{
    uint64_t u;
    struct bdk_gserx_pll_stat_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t pll_lock              : 1;  /**< [  0:  0](RO/H) When set, indicates that the PHY PLL is locked. */
#else /* Word 0 - Little Endian */
        uint64_t pll_lock              : 1;  /**< [  0:  0](RO/H) When set, indicates that the PHY PLL is locked. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_pll_stat_s cn; */
};
typedef union bdk_gserx_pll_stat bdk_gserx_pll_stat_t;

static inline uint64_t BDK_GSERX_PLL_STAT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_PLL_STAT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000010ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000010ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000010ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_PLL_STAT", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_PLL_STAT(a) bdk_gserx_pll_stat_t
#define bustype_BDK_GSERX_PLL_STAT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_PLL_STAT(a) "GSERX_PLL_STAT"
#define device_bar_BDK_GSERX_PLL_STAT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_PLL_STAT(a) (a)
#define arguments_BDK_GSERX_PLL_STAT(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_qlm_stat
 *
 * GSER QLM Status Register
 */
union bdk_gserx_qlm_stat
{
    uint64_t u;
    struct bdk_gserx_qlm_stat_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t rst_rdy               : 1;  /**< [  1:  1](RO/H) When asserted, the QLM is configured and the PLLs are stable. The GSER
                                                                 is ready to accept TX traffic from the MAC. */
        uint64_t dcok                  : 1;  /**< [  0:  0](RO) When asserted, there is a PLL reference clock indicating there is power to the QLM. */
#else /* Word 0 - Little Endian */
        uint64_t dcok                  : 1;  /**< [  0:  0](RO) When asserted, there is a PLL reference clock indicating there is power to the QLM. */
        uint64_t rst_rdy               : 1;  /**< [  1:  1](RO/H) When asserted, the QLM is configured and the PLLs are stable. The GSER
                                                                 is ready to accept TX traffic from the MAC. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_qlm_stat_s cn; */
};
typedef union bdk_gserx_qlm_stat bdk_gserx_qlm_stat_t;

static inline uint64_t BDK_GSERX_QLM_STAT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_QLM_STAT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0900000a0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0900000a0ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0900000a0ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_QLM_STAT", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_QLM_STAT(a) bdk_gserx_qlm_stat_t
#define bustype_BDK_GSERX_QLM_STAT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_QLM_STAT(a) "GSERX_QLM_STAT"
#define device_bar_BDK_GSERX_QLM_STAT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_QLM_STAT(a) (a)
#define arguments_BDK_GSERX_QLM_STAT(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_rdet_time
 *
 * GSER Receiver Detect Wait Times Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_rdet_time
{
    uint64_t u;
    struct bdk_gserx_rdet_time_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t rdet_time_3           : 4;  /**< [ 15: 12](R/W) Determines the time allocated for disabling the RX detect
                                                                 circuit, and returning to common mode. */
        uint64_t rdet_time_2           : 4;  /**< [ 11:  8](R/W) Determines the time allocated for the RX detect circuit to
                                                                 detect a receiver. */
        uint64_t rdet_time_1           : 8;  /**< [  7:  0](R/W) Determines the time allocated for enabling the RX detect circuit. */
#else /* Word 0 - Little Endian */
        uint64_t rdet_time_1           : 8;  /**< [  7:  0](R/W) Determines the time allocated for enabling the RX detect circuit. */
        uint64_t rdet_time_2           : 4;  /**< [ 11:  8](R/W) Determines the time allocated for the RX detect circuit to
                                                                 detect a receiver. */
        uint64_t rdet_time_3           : 4;  /**< [ 15: 12](R/W) Determines the time allocated for disabling the RX detect
                                                                 circuit, and returning to common mode. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_rdet_time_s cn; */
};
typedef union bdk_gserx_rdet_time bdk_gserx_rdet_time_t;

static inline uint64_t BDK_GSERX_RDET_TIME(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_RDET_TIME(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0904e0008ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0904e0008ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0904e0008ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_RDET_TIME", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_RDET_TIME(a) bdk_gserx_rdet_time_t
#define bustype_BDK_GSERX_RDET_TIME(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_RDET_TIME(a) "GSERX_RDET_TIME"
#define device_bar_BDK_GSERX_RDET_TIME(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_RDET_TIME(a) (a)
#define arguments_BDK_GSERX_RDET_TIME(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_refclk_evt_cntr
 *
 * GSER QLM Reference Clock Event Counter Register
 */
union bdk_gserx_refclk_evt_cntr
{
    uint64_t u;
    struct bdk_gserx_refclk_evt_cntr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t count                 : 32; /**< [ 31:  0](RO) This register can only be reliably read when GSER()_REFCLK_EVT_CTRL[ENB]
                                                                 is clear.

                                                                 When GSER()_REFCLK_EVT_CTRL[CLR] is set, [COUNT] goes to zero.

                                                                 When GSER()_REFCLK_EVT_CTRL[ENB] is set, [COUNT] is incremented
                                                                 in positive edges of the QLM reference clock.

                                                                 When GSER()_REFCLK_EVT_CTRL[ENB] is not set, [COUNT] is held; this must
                                                                 be used when [COUNT] is being read for reliable results. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 32; /**< [ 31:  0](RO) This register can only be reliably read when GSER()_REFCLK_EVT_CTRL[ENB]
                                                                 is clear.

                                                                 When GSER()_REFCLK_EVT_CTRL[CLR] is set, [COUNT] goes to zero.

                                                                 When GSER()_REFCLK_EVT_CTRL[ENB] is set, [COUNT] is incremented
                                                                 in positive edges of the QLM reference clock.

                                                                 When GSER()_REFCLK_EVT_CTRL[ENB] is not set, [COUNT] is held; this must
                                                                 be used when [COUNT] is being read for reliable results. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_refclk_evt_cntr_s cn; */
};
typedef union bdk_gserx_refclk_evt_cntr bdk_gserx_refclk_evt_cntr_t;

static inline uint64_t BDK_GSERX_REFCLK_EVT_CNTR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_REFCLK_EVT_CNTR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000178ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000178ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000178ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_REFCLK_EVT_CNTR", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_REFCLK_EVT_CNTR(a) bdk_gserx_refclk_evt_cntr_t
#define bustype_BDK_GSERX_REFCLK_EVT_CNTR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_REFCLK_EVT_CNTR(a) "GSERX_REFCLK_EVT_CNTR"
#define device_bar_BDK_GSERX_REFCLK_EVT_CNTR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_REFCLK_EVT_CNTR(a) (a)
#define arguments_BDK_GSERX_REFCLK_EVT_CNTR(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_refclk_evt_ctrl
 *
 * GSER QLM Reference Clock Event Counter Control Register
 */
union bdk_gserx_refclk_evt_ctrl
{
    uint64_t u;
    struct bdk_gserx_refclk_evt_ctrl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t clr                   : 1;  /**< [  1:  1](R/W) When set, clears GSER()_REFCLK_EVT_CNTR[COUNT]. */
        uint64_t enb                   : 1;  /**< [  0:  0](R/W) When set, enables the GSER()_REFCLK_EVT_CNTR[COUNT] to increment
                                                                 on positive edges of the QLM reference clock. */
#else /* Word 0 - Little Endian */
        uint64_t enb                   : 1;  /**< [  0:  0](R/W) When set, enables the GSER()_REFCLK_EVT_CNTR[COUNT] to increment
                                                                 on positive edges of the QLM reference clock. */
        uint64_t clr                   : 1;  /**< [  1:  1](R/W) When set, clears GSER()_REFCLK_EVT_CNTR[COUNT]. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_refclk_evt_ctrl_s cn; */
};
typedef union bdk_gserx_refclk_evt_ctrl bdk_gserx_refclk_evt_ctrl_t;

static inline uint64_t BDK_GSERX_REFCLK_EVT_CTRL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_REFCLK_EVT_CTRL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000170ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000170ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000170ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_REFCLK_EVT_CTRL", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_REFCLK_EVT_CTRL(a) bdk_gserx_refclk_evt_ctrl_t
#define bustype_BDK_GSERX_REFCLK_EVT_CTRL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_REFCLK_EVT_CTRL(a) "GSERX_REFCLK_EVT_CTRL"
#define device_bar_BDK_GSERX_REFCLK_EVT_CTRL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_REFCLK_EVT_CTRL(a) (a)
#define arguments_BDK_GSERX_REFCLK_EVT_CTRL(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_refclk_sel
 *
 * GSER Reference Clock Select Register
 * This register selects the reference clock.
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_refclk_sel
{
    uint64_t u;
    struct bdk_gserx_refclk_sel_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_3_63         : 61;
        uint64_t pcie_refclk125        : 1;  /**< [  2:  2](R/W/H) For bootable PCIe links, this is loaded with
                                                                 PCIE0/2_REFCLK_125 at cold reset and indicates a 125 MHz reference clock when set. For
                                                                 non-bootable PCIe links, this bit is set to zero at cold reset and indicates a 100 MHz
                                                                 reference clock. It is not used for non-PCIe links. */
        uint64_t com_clk_sel           : 1;  /**< [  1:  1](R/W/H) When set, the reference clock is sourced from the external clock mux. For bootable PCIe
                                                                 links, this bit is loaded with the PCIEn_COM0_CLK_EN pin at cold reset.

                                                                 For CN80XX, this field must be set. */
        uint64_t use_com1              : 1;  /**< [  0:  0](R/W) This bit controls the external mux select. When set, DLMC_REF_CLK1_N/P
                                                                 are selected as the reference clock. When clear, DLMC_REF_CLK0_N/P are selected as the
                                                                 reference clock. */
#else /* Word 0 - Little Endian */
        uint64_t use_com1              : 1;  /**< [  0:  0](R/W) This bit controls the external mux select. When set, DLMC_REF_CLK1_N/P
                                                                 are selected as the reference clock. When clear, DLMC_REF_CLK0_N/P are selected as the
                                                                 reference clock. */
        uint64_t com_clk_sel           : 1;  /**< [  1:  1](R/W/H) When set, the reference clock is sourced from the external clock mux. For bootable PCIe
                                                                 links, this bit is loaded with the PCIEn_COM0_CLK_EN pin at cold reset.

                                                                 For CN80XX, this field must be set. */
        uint64_t pcie_refclk125        : 1;  /**< [  2:  2](R/W/H) For bootable PCIe links, this is loaded with
                                                                 PCIE0/2_REFCLK_125 at cold reset and indicates a 125 MHz reference clock when set. For
                                                                 non-bootable PCIe links, this bit is set to zero at cold reset and indicates a 100 MHz
                                                                 reference clock. It is not used for non-PCIe links. */
        uint64_t reserved_3_63         : 61;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_refclk_sel_s cn81xx; */
    struct bdk_gserx_refclk_sel_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_3_63         : 61;
        uint64_t pcie_refclk125        : 1;  /**< [  2:  2](R/W/H) For bootable PCIe links, this is loaded with
                                                                 PCIE0/2_REFCLK_125 at cold reset and indicates a 125 MHz reference clock when set. For
                                                                 non-bootable PCIe links, this bit is set to zero at cold reset and indicates a 100 MHz
                                                                 reference clock. It is not used for non-PCIe links. */
        uint64_t com_clk_sel           : 1;  /**< [  1:  1](R/W/H) When set, the reference clock is sourced from the external clock mux. For bootable PCIe
                                                                 links, this bit is loaded with the PCIEn_COM0_CLK_EN pin at cold reset. */
        uint64_t use_com1              : 1;  /**< [  0:  0](R/W) For non-CCPI links, this bit controls the external mux select. When set, QLMC_REF_CLK1_N/P
                                                                 are selected as the reference clock. When clear, QLMC_REF_CLK0_N/P are selected as the
                                                                 reference clock. */
#else /* Word 0 - Little Endian */
        uint64_t use_com1              : 1;  /**< [  0:  0](R/W) For non-CCPI links, this bit controls the external mux select. When set, QLMC_REF_CLK1_N/P
                                                                 are selected as the reference clock. When clear, QLMC_REF_CLK0_N/P are selected as the
                                                                 reference clock. */
        uint64_t com_clk_sel           : 1;  /**< [  1:  1](R/W/H) When set, the reference clock is sourced from the external clock mux. For bootable PCIe
                                                                 links, this bit is loaded with the PCIEn_COM0_CLK_EN pin at cold reset. */
        uint64_t pcie_refclk125        : 1;  /**< [  2:  2](R/W/H) For bootable PCIe links, this is loaded with
                                                                 PCIE0/2_REFCLK_125 at cold reset and indicates a 125 MHz reference clock when set. For
                                                                 non-bootable PCIe links, this bit is set to zero at cold reset and indicates a 100 MHz
                                                                 reference clock. It is not used for non-PCIe links. */
        uint64_t reserved_3_63         : 61;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_gserx_refclk_sel_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_3_63         : 61;
        uint64_t pcie_refclk125        : 1;  /**< [  2:  2](R/W/H) For bootable PCIe links, this is loaded with
                                                                 PCIE0/2_REFCLK_125 at cold reset and indicates a 125 MHz reference clock when set. For
                                                                 non-bootable PCIe links, this bit is set to zero at cold reset and indicates a 100 MHz
                                                                 reference clock. It is not used for non-PCIe links. */
        uint64_t com_clk_sel           : 1;  /**< [  1:  1](R/W/H) When set, the reference clock is sourced from the external clock mux. For bootable PCIe
                                                                 links, this bit is loaded with the PCIEn_COM0_CLK_EN pin at cold reset. */
        uint64_t use_com1              : 1;  /**< [  0:  0](R/W) This bit controls the external mux select. When set, QLMC_REF_CLK1_N/P
                                                                 are selected as the reference clock. When clear, QLMC_REF_CLK0_N/P are selected as the
                                                                 reference clock. */
#else /* Word 0 - Little Endian */
        uint64_t use_com1              : 1;  /**< [  0:  0](R/W) This bit controls the external mux select. When set, QLMC_REF_CLK1_N/P
                                                                 are selected as the reference clock. When clear, QLMC_REF_CLK0_N/P are selected as the
                                                                 reference clock. */
        uint64_t com_clk_sel           : 1;  /**< [  1:  1](R/W/H) When set, the reference clock is sourced from the external clock mux. For bootable PCIe
                                                                 links, this bit is loaded with the PCIEn_COM0_CLK_EN pin at cold reset. */
        uint64_t pcie_refclk125        : 1;  /**< [  2:  2](R/W/H) For bootable PCIe links, this is loaded with
                                                                 PCIE0/2_REFCLK_125 at cold reset and indicates a 125 MHz reference clock when set. For
                                                                 non-bootable PCIe links, this bit is set to zero at cold reset and indicates a 100 MHz
                                                                 reference clock. It is not used for non-PCIe links. */
        uint64_t reserved_3_63         : 61;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_refclk_sel bdk_gserx_refclk_sel_t;

static inline uint64_t BDK_GSERX_REFCLK_SEL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_REFCLK_SEL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000008ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000008ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000008ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_REFCLK_SEL", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_REFCLK_SEL(a) bdk_gserx_refclk_sel_t
#define bustype_BDK_GSERX_REFCLK_SEL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_REFCLK_SEL(a) "GSERX_REFCLK_SEL"
#define device_bar_BDK_GSERX_REFCLK_SEL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_REFCLK_SEL(a) (a)
#define arguments_BDK_GSERX_REFCLK_SEL(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_rx_coast
 *
 * GSER RX Coast Register
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_rx_coast
{
    uint64_t u;
    struct bdk_gserx_rx_coast_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t coast                 : 4;  /**< [  3:  0](R/W/H) For links that are not in PCIE or SATA mode, control signals to freeze
                                                                 the frequency of the per lane CDR in the PHY. The COAST signals are only valid in P0
                                                                 state, come up asserted and are deasserted in hardware after detecting the electrical idle
                                                                 exit (GSER()_RX_EIE_DETSTS[EIESTS]). Once the COAST signal deasserts, the CDR is
                                                                 allowed to lock. In BGX mode, the BGX MAC can also control the COAST inputs to the PHY to
                                                                 allow Auto-Negotiation for backplane Ethernet. For diagnostic use only.
                                                                 \<3\>: Lane 3.  Reserved.
                                                                 \<2\>: Lane 2.  Reserved.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t coast                 : 4;  /**< [  3:  0](R/W/H) For links that are not in PCIE or SATA mode, control signals to freeze
                                                                 the frequency of the per lane CDR in the PHY. The COAST signals are only valid in P0
                                                                 state, come up asserted and are deasserted in hardware after detecting the electrical idle
                                                                 exit (GSER()_RX_EIE_DETSTS[EIESTS]). Once the COAST signal deasserts, the CDR is
                                                                 allowed to lock. In BGX mode, the BGX MAC can also control the COAST inputs to the PHY to
                                                                 allow Auto-Negotiation for backplane Ethernet. For diagnostic use only.
                                                                 \<3\>: Lane 3.  Reserved.
                                                                 \<2\>: Lane 2.  Reserved.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_rx_coast_s cn81xx; */
    struct bdk_gserx_rx_coast_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t coast                 : 4;  /**< [  3:  0](R/W/H) For links that are not in PCIE or SATA mode (including all CCPI links), control signals to
                                                                 freeze
                                                                 the frequency of the per lane CDR in the PHY. The COAST signals are only valid in P0
                                                                 state, come up asserted and are deasserted in hardware after detecting the electrical idle
                                                                 exit (GSER()_RX_EIE_DETSTS[EIESTS]). Once the COAST signal deasserts, the CDR is
                                                                 allowed to lock. In BGX mode, the BGX MAC can also control the COAST inputs to the PHY to
                                                                 allow Auto-Negotiation for backplane Ethernet. For diagnostic use only.
                                                                 \<3\>: Lane 3.
                                                                 \<2\>: Lane 2.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t coast                 : 4;  /**< [  3:  0](R/W/H) For links that are not in PCIE or SATA mode (including all CCPI links), control signals to
                                                                 freeze
                                                                 the frequency of the per lane CDR in the PHY. The COAST signals are only valid in P0
                                                                 state, come up asserted and are deasserted in hardware after detecting the electrical idle
                                                                 exit (GSER()_RX_EIE_DETSTS[EIESTS]). Once the COAST signal deasserts, the CDR is
                                                                 allowed to lock. In BGX mode, the BGX MAC can also control the COAST inputs to the PHY to
                                                                 allow Auto-Negotiation for backplane Ethernet. For diagnostic use only.
                                                                 \<3\>: Lane 3.
                                                                 \<2\>: Lane 2.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_gserx_rx_coast_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t coast                 : 4;  /**< [  3:  0](R/W/H) For links that are not in PCIE or SATA mode, control signals to freeze
                                                                 the frequency of the per lane CDR in the PHY. The COAST signals are only valid in P0
                                                                 state, come up asserted and are deasserted in hardware after detecting the electrical idle
                                                                 exit (GSER()_RX_EIE_DETSTS[EIESTS]). Once the COAST signal deasserts, the CDR is
                                                                 allowed to lock. In BGX mode, the BGX MAC can also control the COAST inputs to the PHY to
                                                                 allow Auto-Negotiation for backplane Ethernet. For diagnostic use only.
                                                                 \<3\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<2\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t coast                 : 4;  /**< [  3:  0](R/W/H) For links that are not in PCIE or SATA mode, control signals to freeze
                                                                 the frequency of the per lane CDR in the PHY. The COAST signals are only valid in P0
                                                                 state, come up asserted and are deasserted in hardware after detecting the electrical idle
                                                                 exit (GSER()_RX_EIE_DETSTS[EIESTS]). Once the COAST signal deasserts, the CDR is
                                                                 allowed to lock. In BGX mode, the BGX MAC can also control the COAST inputs to the PHY to
                                                                 allow Auto-Negotiation for backplane Ethernet. For diagnostic use only.
                                                                 \<3\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<2\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_rx_coast bdk_gserx_rx_coast_t;

static inline uint64_t BDK_GSERX_RX_COAST(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_RX_COAST(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000138ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000138ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000138ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_RX_COAST", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_RX_COAST(a) bdk_gserx_rx_coast_t
#define bustype_BDK_GSERX_RX_COAST(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_RX_COAST(a) "GSERX_RX_COAST"
#define device_bar_BDK_GSERX_RX_COAST(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_RX_COAST(a) (a)
#define arguments_BDK_GSERX_RX_COAST(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_rx_eie_deten
 *
 * GSER RX Electrical Idle Detect Enable Register
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_rx_eie_deten
{
    uint64_t u;
    struct bdk_gserx_rx_eie_deten_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t eiede                 : 4;  /**< [  3:  0](R/W) For links that are not in PCIE or SATA mode, these bits enable per lane
                                                                 electrical idle exit (EIE) detection. When EIE is detected,
                                                                 GSER()_RX_EIE_DETSTS[EIELTCH] is asserted. [EIEDE] defaults to the enabled state. Once
                                                                 EIE has been detected, [EIEDE] must be disabled, and then enabled again to perform another
                                                                 EIE detection.
                                                                 \<3\>: Lane 3.  Reserved.
                                                                 \<2\>: Lane 2.  Reserved.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t eiede                 : 4;  /**< [  3:  0](R/W) For links that are not in PCIE or SATA mode, these bits enable per lane
                                                                 electrical idle exit (EIE) detection. When EIE is detected,
                                                                 GSER()_RX_EIE_DETSTS[EIELTCH] is asserted. [EIEDE] defaults to the enabled state. Once
                                                                 EIE has been detected, [EIEDE] must be disabled, and then enabled again to perform another
                                                                 EIE detection.
                                                                 \<3\>: Lane 3.  Reserved.
                                                                 \<2\>: Lane 2.  Reserved.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_rx_eie_deten_s cn81xx; */
    struct bdk_gserx_rx_eie_deten_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t eiede                 : 4;  /**< [  3:  0](R/W) For links that are not in PCIE or SATA mode (including all CCPI links), these bits enable
                                                                 per lane
                                                                 electrical idle exit (EIE) detection. When EIE is detected,
                                                                 GSER()_RX_EIE_DETSTS[EIELTCH] is asserted. [EIEDE] defaults to the enabled state. Once
                                                                 EIE has been detected, [EIEDE] must be disabled, and then enabled again to perform another
                                                                 EIE detection.
                                                                 \<3\>: Lane 3.
                                                                 \<2\>: Lane 2.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t eiede                 : 4;  /**< [  3:  0](R/W) For links that are not in PCIE or SATA mode (including all CCPI links), these bits enable
                                                                 per lane
                                                                 electrical idle exit (EIE) detection. When EIE is detected,
                                                                 GSER()_RX_EIE_DETSTS[EIELTCH] is asserted. [EIEDE] defaults to the enabled state. Once
                                                                 EIE has been detected, [EIEDE] must be disabled, and then enabled again to perform another
                                                                 EIE detection.
                                                                 \<3\>: Lane 3.
                                                                 \<2\>: Lane 2.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_gserx_rx_eie_deten_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t eiede                 : 4;  /**< [  3:  0](R/W) For links that are not in PCIE or SATA mode, these bits enable per lane
                                                                 electrical idle exit (EIE) detection. When EIE is detected,
                                                                 GSER()_RX_EIE_DETSTS[EIELTCH] is asserted. [EIEDE] defaults to the enabled state. Once
                                                                 EIE has been detected, [EIEDE] must be disabled, and then enabled again to perform another
                                                                 EIE detection.
                                                                 \<3\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<2\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t eiede                 : 4;  /**< [  3:  0](R/W) For links that are not in PCIE or SATA mode, these bits enable per lane
                                                                 electrical idle exit (EIE) detection. When EIE is detected,
                                                                 GSER()_RX_EIE_DETSTS[EIELTCH] is asserted. [EIEDE] defaults to the enabled state. Once
                                                                 EIE has been detected, [EIEDE] must be disabled, and then enabled again to perform another
                                                                 EIE detection.
                                                                 \<3\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<2\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_rx_eie_deten bdk_gserx_rx_eie_deten_t;

static inline uint64_t BDK_GSERX_RX_EIE_DETEN(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_RX_EIE_DETEN(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000148ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000148ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000148ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_RX_EIE_DETEN", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_RX_EIE_DETEN(a) bdk_gserx_rx_eie_deten_t
#define bustype_BDK_GSERX_RX_EIE_DETEN(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_RX_EIE_DETEN(a) "GSERX_RX_EIE_DETEN"
#define device_bar_BDK_GSERX_RX_EIE_DETEN(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_RX_EIE_DETEN(a) (a)
#define arguments_BDK_GSERX_RX_EIE_DETEN(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_rx_eie_detsts
 *
 * GSER RX Electrical Idle Detect Status Register
 */
union bdk_gserx_rx_eie_detsts
{
    uint64_t u;
    struct bdk_gserx_rx_eie_detsts_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t cdrlock               : 4;  /**< [ 11:  8](RO/H) After an electrical idle exit condition (EIE) has been detected, the CDR needs 10000 UI to
                                                                 lock. During this time, there may be RX bit errors. These bits will set when the CDR is
                                                                 guaranteed to be locked. Note that link training can't start until the lane CDRLOCK is
                                                                 set. Software can use CDRLOCK to determine when to expect error free RX data.
                                                                 \<11\>: Lane 3.  Reserved.
                                                                 \<10\>: Lane 2.  Reserved.
                                                                 \<9\>: Lane 1.
                                                                 \<8\>: Lane 0. */
        uint64_t eiests                : 4;  /**< [  7:  4](RO/H) When electrical idle exit detection is enabled (GSER()_RX_EIE_DETEN[EIEDE] is
                                                                 asserted), indicates that an electrical idle exit condition (EIE) was detected. For higher
                                                                 data rates, the received data needs to have sufficient low frequency content (for example,
                                                                 idle symbols) for data transitions to be detected and for [EIESTS] to stay set
                                                                 accordingly.
                                                                 Under most conditions, [EIESTS]
                                                                 will stay asserted until GSER()_RX_EIE_DETEN[EIEDE] is deasserted.
                                                                 \<7\>: Lane 3.  Reserved.
                                                                 \<6\>: Lane 2.  Reserved.
                                                                 \<5\>: Lane 1.
                                                                 \<4\>: Lane 0. */
        uint64_t eieltch               : 4;  /**< [  3:  0](RO/H) When electrical idle exit detection is enabled (GSER()_RX_EIE_DETEN[EIEDE] is
                                                                 asserted), indicates that an electrical idle exit condition (EIE) was detected. Once an
                                                                 EIE condition has been detected, the per-lane [EIELTCH] will stay set until
                                                                 GSER()_RX_EIE_DETEN[EIEDE] is deasserted. Note that there may be RX bit errors until
                                                                 CDRLOCK
                                                                 is set.
                                                                 \<3\>: Lane 3.  Reserved.
                                                                 \<2\>: Lane 2.  Reserved.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t eieltch               : 4;  /**< [  3:  0](RO/H) When electrical idle exit detection is enabled (GSER()_RX_EIE_DETEN[EIEDE] is
                                                                 asserted), indicates that an electrical idle exit condition (EIE) was detected. Once an
                                                                 EIE condition has been detected, the per-lane [EIELTCH] will stay set until
                                                                 GSER()_RX_EIE_DETEN[EIEDE] is deasserted. Note that there may be RX bit errors until
                                                                 CDRLOCK
                                                                 is set.
                                                                 \<3\>: Lane 3.  Reserved.
                                                                 \<2\>: Lane 2.  Reserved.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t eiests                : 4;  /**< [  7:  4](RO/H) When electrical idle exit detection is enabled (GSER()_RX_EIE_DETEN[EIEDE] is
                                                                 asserted), indicates that an electrical idle exit condition (EIE) was detected. For higher
                                                                 data rates, the received data needs to have sufficient low frequency content (for example,
                                                                 idle symbols) for data transitions to be detected and for [EIESTS] to stay set
                                                                 accordingly.
                                                                 Under most conditions, [EIESTS]
                                                                 will stay asserted until GSER()_RX_EIE_DETEN[EIEDE] is deasserted.
                                                                 \<7\>: Lane 3.  Reserved.
                                                                 \<6\>: Lane 2.  Reserved.
                                                                 \<5\>: Lane 1.
                                                                 \<4\>: Lane 0. */
        uint64_t cdrlock               : 4;  /**< [ 11:  8](RO/H) After an electrical idle exit condition (EIE) has been detected, the CDR needs 10000 UI to
                                                                 lock. During this time, there may be RX bit errors. These bits will set when the CDR is
                                                                 guaranteed to be locked. Note that link training can't start until the lane CDRLOCK is
                                                                 set. Software can use CDRLOCK to determine when to expect error free RX data.
                                                                 \<11\>: Lane 3.  Reserved.
                                                                 \<10\>: Lane 2.  Reserved.
                                                                 \<9\>: Lane 1.
                                                                 \<8\>: Lane 0. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_rx_eie_detsts_s cn81xx; */
    struct bdk_gserx_rx_eie_detsts_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t cdrlock               : 4;  /**< [ 11:  8](RO/H) After an electrical idle exit condition (EIE) has been detected, the CDR needs 10000 UI to
                                                                 lock. During this time, there may be RX bit errors. These bits will set when the CDR is
                                                                 guaranteed to be locked. Note that link training can't start until the lane CDRLOCK is
                                                                 set. Software can use CDRLOCK to determine when to expect error free RX data.
                                                                 \<11\>: Lane 3.
                                                                 \<10\>: Lane 2.
                                                                 \<9\>: Lane 1.
                                                                 \<8\>: Lane 0. */
        uint64_t eiests                : 4;  /**< [  7:  4](RO/H) When electrical idle exit detection is enabled (GSER()_RX_EIE_DETEN[EIEDE] is
                                                                 asserted), indicates that an electrical idle exit condition (EIE) was detected. For higher
                                                                 data rates, the received data needs to have sufficient low frequency content (for example,
                                                                 idle symbols) for data transitions to be detected and for [EIESTS] to stay set
                                                                 accordingly.
                                                                 Under most conditions, [EIESTS]
                                                                 will stay asserted until GSER()_RX_EIE_DETEN[EIEDE] is deasserted.
                                                                 \<7\>: Lane 3.
                                                                 \<6\>: Lane 2.
                                                                 \<5\>: Lane 1.
                                                                 \<4\>: Lane 0. */
        uint64_t eieltch               : 4;  /**< [  3:  0](RO/H) When electrical idle exit detection is enabled (GSER()_RX_EIE_DETEN[EIEDE] is
                                                                 asserted), indicates that an electrical idle exit condition (EIE) was detected. Once an
                                                                 EIE condition has been detected, the per-lane [EIELTCH] will stay set until
                                                                 GSER()_RX_EIE_DETEN[EIEDE] is deasserted. Note that there may be RX bit errors until
                                                                 CDRLOCK
                                                                 is set.
                                                                 \<3\>: Lane 3.
                                                                 \<2\>: Lane 2.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t eieltch               : 4;  /**< [  3:  0](RO/H) When electrical idle exit detection is enabled (GSER()_RX_EIE_DETEN[EIEDE] is
                                                                 asserted), indicates that an electrical idle exit condition (EIE) was detected. Once an
                                                                 EIE condition has been detected, the per-lane [EIELTCH] will stay set until
                                                                 GSER()_RX_EIE_DETEN[EIEDE] is deasserted. Note that there may be RX bit errors until
                                                                 CDRLOCK
                                                                 is set.
                                                                 \<3\>: Lane 3.
                                                                 \<2\>: Lane 2.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t eiests                : 4;  /**< [  7:  4](RO/H) When electrical idle exit detection is enabled (GSER()_RX_EIE_DETEN[EIEDE] is
                                                                 asserted), indicates that an electrical idle exit condition (EIE) was detected. For higher
                                                                 data rates, the received data needs to have sufficient low frequency content (for example,
                                                                 idle symbols) for data transitions to be detected and for [EIESTS] to stay set
                                                                 accordingly.
                                                                 Under most conditions, [EIESTS]
                                                                 will stay asserted until GSER()_RX_EIE_DETEN[EIEDE] is deasserted.
                                                                 \<7\>: Lane 3.
                                                                 \<6\>: Lane 2.
                                                                 \<5\>: Lane 1.
                                                                 \<4\>: Lane 0. */
        uint64_t cdrlock               : 4;  /**< [ 11:  8](RO/H) After an electrical idle exit condition (EIE) has been detected, the CDR needs 10000 UI to
                                                                 lock. During this time, there may be RX bit errors. These bits will set when the CDR is
                                                                 guaranteed to be locked. Note that link training can't start until the lane CDRLOCK is
                                                                 set. Software can use CDRLOCK to determine when to expect error free RX data.
                                                                 \<11\>: Lane 3.
                                                                 \<10\>: Lane 2.
                                                                 \<9\>: Lane 1.
                                                                 \<8\>: Lane 0. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_gserx_rx_eie_detsts_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t cdrlock               : 4;  /**< [ 11:  8](RO/H) After an electrical idle exit condition (EIE) has been detected, the CDR needs 10000 UI to
                                                                 lock. During this time, there may be RX bit errors. These bits will set when the CDR is
                                                                 guaranteed to be locked. Note that link training can't start until the lane CDRLOCK is
                                                                 set. Software can use CDRLOCK to determine when to expect error free RX data.
                                                                 \<11\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<10\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<9\>: Lane 1.
                                                                 \<8\>: Lane 0. */
        uint64_t eiests                : 4;  /**< [  7:  4](RO/H) When electrical idle exit detection is enabled (GSER()_RX_EIE_DETEN[EIEDE] is
                                                                 asserted), indicates that an electrical idle exit condition (EIE) was detected. For higher
                                                                 data rates, the received data needs to have sufficient low frequency content (for example,
                                                                 idle symbols) for data transitions to be detected and for [EIESTS] to stay set
                                                                 accordingly.
                                                                 Under most conditions, [EIESTS]
                                                                 will stay asserted until GSER()_RX_EIE_DETEN[EIEDE] is deasserted.
                                                                 \<7\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<6\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<5\>: Lane 1.
                                                                 \<4\>: Lane 0. */
        uint64_t eieltch               : 4;  /**< [  3:  0](RO/H) When electrical idle exit detection is enabled (GSER()_RX_EIE_DETEN[EIEDE] is
                                                                 asserted), indicates that an electrical idle exit condition (EIE) was detected. Once an
                                                                 EIE condition has been detected, the per-lane [EIELTCH] will stay set until
                                                                 GSER()_RX_EIE_DETEN[EIEDE] is deasserted. Note that there may be RX bit errors until
                                                                 CDRLOCK
                                                                 is set.
                                                                 \<3\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<2\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t eieltch               : 4;  /**< [  3:  0](RO/H) When electrical idle exit detection is enabled (GSER()_RX_EIE_DETEN[EIEDE] is
                                                                 asserted), indicates that an electrical idle exit condition (EIE) was detected. Once an
                                                                 EIE condition has been detected, the per-lane [EIELTCH] will stay set until
                                                                 GSER()_RX_EIE_DETEN[EIEDE] is deasserted. Note that there may be RX bit errors until
                                                                 CDRLOCK
                                                                 is set.
                                                                 \<3\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<2\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t eiests                : 4;  /**< [  7:  4](RO/H) When electrical idle exit detection is enabled (GSER()_RX_EIE_DETEN[EIEDE] is
                                                                 asserted), indicates that an electrical idle exit condition (EIE) was detected. For higher
                                                                 data rates, the received data needs to have sufficient low frequency content (for example,
                                                                 idle symbols) for data transitions to be detected and for [EIESTS] to stay set
                                                                 accordingly.
                                                                 Under most conditions, [EIESTS]
                                                                 will stay asserted until GSER()_RX_EIE_DETEN[EIEDE] is deasserted.
                                                                 \<7\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<6\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<5\>: Lane 1.
                                                                 \<4\>: Lane 0. */
        uint64_t cdrlock               : 4;  /**< [ 11:  8](RO/H) After an electrical idle exit condition (EIE) has been detected, the CDR needs 10000 UI to
                                                                 lock. During this time, there may be RX bit errors. These bits will set when the CDR is
                                                                 guaranteed to be locked. Note that link training can't start until the lane CDRLOCK is
                                                                 set. Software can use CDRLOCK to determine when to expect error free RX data.
                                                                 \<11\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<10\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<9\>: Lane 1.
                                                                 \<8\>: Lane 0. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_rx_eie_detsts bdk_gserx_rx_eie_detsts_t;

static inline uint64_t BDK_GSERX_RX_EIE_DETSTS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_RX_EIE_DETSTS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000150ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000150ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000150ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_RX_EIE_DETSTS", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_RX_EIE_DETSTS(a) bdk_gserx_rx_eie_detsts_t
#define bustype_BDK_GSERX_RX_EIE_DETSTS(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_RX_EIE_DETSTS(a) "GSERX_RX_EIE_DETSTS"
#define device_bar_BDK_GSERX_RX_EIE_DETSTS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_RX_EIE_DETSTS(a) (a)
#define arguments_BDK_GSERX_RX_EIE_DETSTS(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_rx_eie_filter
 *
 * GSER RX Electrical Idle Detect Filter Settings Register
 */
union bdk_gserx_rx_eie_filter
{
    uint64_t u;
    struct bdk_gserx_rx_eie_filter_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t eii_filt              : 16; /**< [ 15:  0](R/W) The GSER uses electrical idle inference to determine when a RX lane has reentered
                                                                 electrical idle (EI). The PHY electrical idle exit detection supports a minimum pulse
                                                                 width of 400 ps, therefore configurations that run faster than 2.5 G can indicate EI when
                                                                 the serial lines are still driven. For rates faster than 2.5 G, it takes 16 K * 8 UI of
                                                                 consecutive deasserted GSER()_RX_EIE_DETSTS[EIESTS] for the GSER to infer EI. In the
                                                                 event of electrical idle inference, the following happens:
                                                                 * GSER()_RX_EIE_DETSTS[CDRLOCK]\<lane\> is zeroed.
                                                                 * GSER()_RX_EIE_DETSTS[EIELTCH]\<lane\> is zeroed.
                                                                 * GSER()_RX_EIE_DETSTS[EIESTS]\<lane\> is zeroed.
                                                                 * GSER()_RX_COAST[COAST]\<lane\> is asserted to prevent the CDR from trying to lock on
                                                                 the incoming data stream.
                                                                 * GSER()_RX_EIE_DETEN[EIEDE]\<lane\> deasserts for a short period of time, and then is
                                                                 asserted to begin looking for the Electrical idle Exit condition.

                                                                 Writing this register to a nonzero value causes the electrical idle inference to use the
                                                                 [EII_FILT] count instead of the default settings. Each [EII_FILT] count represents 20 ns
                                                                 of
                                                                 incremental EI inference time.

                                                                 It is not expected that software will need to use the Electrical Idle Inference logic. */
#else /* Word 0 - Little Endian */
        uint64_t eii_filt              : 16; /**< [ 15:  0](R/W) The GSER uses electrical idle inference to determine when a RX lane has reentered
                                                                 electrical idle (EI). The PHY electrical idle exit detection supports a minimum pulse
                                                                 width of 400 ps, therefore configurations that run faster than 2.5 G can indicate EI when
                                                                 the serial lines are still driven. For rates faster than 2.5 G, it takes 16 K * 8 UI of
                                                                 consecutive deasserted GSER()_RX_EIE_DETSTS[EIESTS] for the GSER to infer EI. In the
                                                                 event of electrical idle inference, the following happens:
                                                                 * GSER()_RX_EIE_DETSTS[CDRLOCK]\<lane\> is zeroed.
                                                                 * GSER()_RX_EIE_DETSTS[EIELTCH]\<lane\> is zeroed.
                                                                 * GSER()_RX_EIE_DETSTS[EIESTS]\<lane\> is zeroed.
                                                                 * GSER()_RX_COAST[COAST]\<lane\> is asserted to prevent the CDR from trying to lock on
                                                                 the incoming data stream.
                                                                 * GSER()_RX_EIE_DETEN[EIEDE]\<lane\> deasserts for a short period of time, and then is
                                                                 asserted to begin looking for the Electrical idle Exit condition.

                                                                 Writing this register to a nonzero value causes the electrical idle inference to use the
                                                                 [EII_FILT] count instead of the default settings. Each [EII_FILT] count represents 20 ns
                                                                 of
                                                                 incremental EI inference time.

                                                                 It is not expected that software will need to use the Electrical Idle Inference logic. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_rx_eie_filter_s cn; */
};
typedef union bdk_gserx_rx_eie_filter bdk_gserx_rx_eie_filter_t;

static inline uint64_t BDK_GSERX_RX_EIE_FILTER(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_RX_EIE_FILTER(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000158ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000158ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000158ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_RX_EIE_FILTER", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_RX_EIE_FILTER(a) bdk_gserx_rx_eie_filter_t
#define bustype_BDK_GSERX_RX_EIE_FILTER(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_RX_EIE_FILTER(a) "GSERX_RX_EIE_FILTER"
#define device_bar_BDK_GSERX_RX_EIE_FILTER(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_RX_EIE_FILTER(a) (a)
#define arguments_BDK_GSERX_RX_EIE_FILTER(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_rx_polarity
 *
 * GSER RX Polarity Register
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_rx_polarity
{
    uint64_t u;
    struct bdk_gserx_rx_polarity_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t rx_inv                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode, control signal to invert
                                                                 the polarity of received data. When asserted, the polarity of the received data is
                                                                 inverted.
                                                                 \<3\>: Lane 3.  Reserved.
                                                                 \<2\>: Lane 2.  Reserved.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t rx_inv                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode, control signal to invert
                                                                 the polarity of received data. When asserted, the polarity of the received data is
                                                                 inverted.
                                                                 \<3\>: Lane 3.  Reserved.
                                                                 \<2\>: Lane 2.  Reserved.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_rx_polarity_s cn81xx; */
    struct bdk_gserx_rx_polarity_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t rx_inv                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode (including all CCPI links), control signal to invert
                                                                 the polarity of received data. When asserted, the polarity of the received data is
                                                                 inverted.
                                                                 \<3\>: Lane 3.
                                                                 \<2\>: Lane 2.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t rx_inv                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode (including all CCPI links), control signal to invert
                                                                 the polarity of received data. When asserted, the polarity of the received data is
                                                                 inverted.
                                                                 \<3\>: Lane 3.
                                                                 \<2\>: Lane 2.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_gserx_rx_polarity_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t rx_inv                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode, control signal to invert
                                                                 the polarity of received data. When asserted, the polarity of the received data is
                                                                 inverted.
                                                                 \<3\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<2\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t rx_inv                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode, control signal to invert
                                                                 the polarity of received data. When asserted, the polarity of the received data is
                                                                 inverted.
                                                                 \<3\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<2\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_rx_polarity bdk_gserx_rx_polarity_t;

static inline uint64_t BDK_GSERX_RX_POLARITY(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_RX_POLARITY(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000160ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000160ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000160ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_RX_POLARITY", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_RX_POLARITY(a) bdk_gserx_rx_polarity_t
#define bustype_BDK_GSERX_RX_POLARITY(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_RX_POLARITY(a) "GSERX_RX_POLARITY"
#define device_bar_BDK_GSERX_RX_POLARITY(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_RX_POLARITY(a) (a)
#define arguments_BDK_GSERX_RX_POLARITY(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_rx_pwr_ctrl_p1
 *
 * GSER RX Power Control P1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_rx_pwr_ctrl_p1
{
    uint64_t u;
    struct bdk_gserx_rx_pwr_ctrl_p1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t p1_rx_resetn          : 1;  /**< [ 13: 13](R/W) Place the receiver in reset (active low). */
        uint64_t pq_rx_allow_pll_pd    : 1;  /**< [ 12: 12](R/W) When asserted, permit PLL powerdown (PLL is powered
                                                                 down if all other factors permit). */
        uint64_t pq_rx_pcs_reset       : 1;  /**< [ 11: 11](R/W) When asserted, the RX power state machine puts the raw PCS RX logic
                                                                 in reset state to save power. */
        uint64_t p1_rx_agc_en          : 1;  /**< [ 10: 10](R/W) AGC enable. */
        uint64_t p1_rx_dfe_en          : 1;  /**< [  9:  9](R/W) DFE enable. */
        uint64_t p1_rx_cdr_en          : 1;  /**< [  8:  8](R/W) CDR enable. */
        uint64_t p1_rx_cdr_coast       : 1;  /**< [  7:  7](R/W) CDR coast; freezes the frequency of the CDR. */
        uint64_t p1_rx_cdr_clr         : 1;  /**< [  6:  6](R/W) CDR clear; clears the frequency of the CDR. */
        uint64_t p1_rx_subblk_pd       : 5;  /**< [  5:  1](R/W) RX sub-block powerdown controls to RX:
                                                                 \<4\> = CTLE.
                                                                 \<3\> = Reserved.
                                                                 \<2\> = Lane DLL.
                                                                 \<1\> = DFE/samplers.
                                                                 \<0\> = Termination. */
        uint64_t p1_rx_chpd            : 1;  /**< [  0:  0](R/W) RX lane powerdown. */
#else /* Word 0 - Little Endian */
        uint64_t p1_rx_chpd            : 1;  /**< [  0:  0](R/W) RX lane powerdown. */
        uint64_t p1_rx_subblk_pd       : 5;  /**< [  5:  1](R/W) RX sub-block powerdown controls to RX:
                                                                 \<4\> = CTLE.
                                                                 \<3\> = Reserved.
                                                                 \<2\> = Lane DLL.
                                                                 \<1\> = DFE/samplers.
                                                                 \<0\> = Termination. */
        uint64_t p1_rx_cdr_clr         : 1;  /**< [  6:  6](R/W) CDR clear; clears the frequency of the CDR. */
        uint64_t p1_rx_cdr_coast       : 1;  /**< [  7:  7](R/W) CDR coast; freezes the frequency of the CDR. */
        uint64_t p1_rx_cdr_en          : 1;  /**< [  8:  8](R/W) CDR enable. */
        uint64_t p1_rx_dfe_en          : 1;  /**< [  9:  9](R/W) DFE enable. */
        uint64_t p1_rx_agc_en          : 1;  /**< [ 10: 10](R/W) AGC enable. */
        uint64_t pq_rx_pcs_reset       : 1;  /**< [ 11: 11](R/W) When asserted, the RX power state machine puts the raw PCS RX logic
                                                                 in reset state to save power. */
        uint64_t pq_rx_allow_pll_pd    : 1;  /**< [ 12: 12](R/W) When asserted, permit PLL powerdown (PLL is powered
                                                                 down if all other factors permit). */
        uint64_t p1_rx_resetn          : 1;  /**< [ 13: 13](R/W) Place the receiver in reset (active low). */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_rx_pwr_ctrl_p1_s cn; */
};
typedef union bdk_gserx_rx_pwr_ctrl_p1 bdk_gserx_rx_pwr_ctrl_p1_t;

static inline uint64_t BDK_GSERX_RX_PWR_CTRL_P1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_RX_PWR_CTRL_P1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0904600b0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0904600b0ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0904600b0ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_RX_PWR_CTRL_P1", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_RX_PWR_CTRL_P1(a) bdk_gserx_rx_pwr_ctrl_p1_t
#define bustype_BDK_GSERX_RX_PWR_CTRL_P1(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_RX_PWR_CTRL_P1(a) "GSERX_RX_PWR_CTRL_P1"
#define device_bar_BDK_GSERX_RX_PWR_CTRL_P1(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_RX_PWR_CTRL_P1(a) (a)
#define arguments_BDK_GSERX_RX_PWR_CTRL_P1(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_rx_pwr_ctrl_p2
 *
 * GSER RX Power Controls in Power State P2 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_rx_pwr_ctrl_p2
{
    uint64_t u;
    struct bdk_gserx_rx_pwr_ctrl_p2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_14_63        : 50;
        uint64_t p2_rx_resetn          : 1;  /**< [ 13: 13](R/W) Place the receiver in reset (active low). */
        uint64_t p2_rx_allow_pll_pd    : 1;  /**< [ 12: 12](R/W) When asserted, it permits PLL powerdown (PLL is
                                                                 powered down if all other factors permit). */
        uint64_t p2_rx_pcs_reset       : 1;  /**< [ 11: 11](R/W) When asserted, the RX Power state machine puts the Raw PCS
                                                                 RX logic in reset state to save power. */
        uint64_t p2_rx_agc_en          : 1;  /**< [ 10: 10](R/W) AGC enable. */
        uint64_t p2_rx_dfe_en          : 1;  /**< [  9:  9](R/W) DFE enable. */
        uint64_t p2_rx_cdr_en          : 1;  /**< [  8:  8](R/W) CDR enable. */
        uint64_t p2_rx_cdr_coast       : 1;  /**< [  7:  7](R/W) CDR coast; freezes the frequency of the CDR. */
        uint64_t p2_rx_cdr_clr         : 1;  /**< [  6:  6](R/W) CDR clear; clears the frequency register in the CDR. */
        uint64_t p2_rx_subblk_pd       : 5;  /**< [  5:  1](R/W) RX sub-block powerdown to RX:
                                                                 \<4\> = CTLE.
                                                                 \<3\> = Reserved.
                                                                 \<2\> = Lane DLL.
                                                                 \<1\> = DFE/Samplers.
                                                                 \<0\> = Termination.

                                                                 Software needs to clear the termination bit in SATA mode
                                                                 (i.e. when GSER()_CFG[SATA] is set). */
        uint64_t p2_rx_chpd            : 1;  /**< [  0:  0](R/W) RX lane power down. */
#else /* Word 0 - Little Endian */
        uint64_t p2_rx_chpd            : 1;  /**< [  0:  0](R/W) RX lane power down. */
        uint64_t p2_rx_subblk_pd       : 5;  /**< [  5:  1](R/W) RX sub-block powerdown to RX:
                                                                 \<4\> = CTLE.
                                                                 \<3\> = Reserved.
                                                                 \<2\> = Lane DLL.
                                                                 \<1\> = DFE/Samplers.
                                                                 \<0\> = Termination.

                                                                 Software needs to clear the termination bit in SATA mode
                                                                 (i.e. when GSER()_CFG[SATA] is set). */
        uint64_t p2_rx_cdr_clr         : 1;  /**< [  6:  6](R/W) CDR clear; clears the frequency register in the CDR. */
        uint64_t p2_rx_cdr_coast       : 1;  /**< [  7:  7](R/W) CDR coast; freezes the frequency of the CDR. */
        uint64_t p2_rx_cdr_en          : 1;  /**< [  8:  8](R/W) CDR enable. */
        uint64_t p2_rx_dfe_en          : 1;  /**< [  9:  9](R/W) DFE enable. */
        uint64_t p2_rx_agc_en          : 1;  /**< [ 10: 10](R/W) AGC enable. */
        uint64_t p2_rx_pcs_reset       : 1;  /**< [ 11: 11](R/W) When asserted, the RX Power state machine puts the Raw PCS
                                                                 RX logic in reset state to save power. */
        uint64_t p2_rx_allow_pll_pd    : 1;  /**< [ 12: 12](R/W) When asserted, it permits PLL powerdown (PLL is
                                                                 powered down if all other factors permit). */
        uint64_t p2_rx_resetn          : 1;  /**< [ 13: 13](R/W) Place the receiver in reset (active low). */
        uint64_t reserved_14_63        : 50;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_rx_pwr_ctrl_p2_s cn; */
};
typedef union bdk_gserx_rx_pwr_ctrl_p2 bdk_gserx_rx_pwr_ctrl_p2_t;

static inline uint64_t BDK_GSERX_RX_PWR_CTRL_P2(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_RX_PWR_CTRL_P2(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0904600b8ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0904600b8ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0904600b8ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_RX_PWR_CTRL_P2", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_RX_PWR_CTRL_P2(a) bdk_gserx_rx_pwr_ctrl_p2_t
#define bustype_BDK_GSERX_RX_PWR_CTRL_P2(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_RX_PWR_CTRL_P2(a) "GSERX_RX_PWR_CTRL_P2"
#define device_bar_BDK_GSERX_RX_PWR_CTRL_P2(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_RX_PWR_CTRL_P2(a) (a)
#define arguments_BDK_GSERX_RX_PWR_CTRL_P2(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_rx_txdir_ctrl_0
 *
 * GSER Far-end TX Direction Control 0 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_rx_txdir_ctrl_0
{
    uint64_t u;
    struct bdk_gserx_rx_txdir_ctrl_0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_13_63        : 51;
        uint64_t rx_boost_hi_thrs      : 4;  /**< [ 12:  9](R/W/H) The high threshold for RX boost.
                                                                 The far-end TX POST direction output, pcs_mac_rx_txpost_dir, is set to
                                                                 increment if the local RX boost value from the VMA (after RX-EQ) is
                                                                 higher than this value, and the local RX tap1 value is higher than its
                                                                 high threshold GSER()_RX_TXDIR_CTRL_1[RX_TAP1_HI_THRS].
                                                                 Note, that if GSER()_RX_TXDIR_CTRL_1[RX_TAP1_CHG_DIR]=1 then
                                                                 the direction is decrement. */
        uint64_t rx_boost_lo_thrs      : 4;  /**< [  8:  5](R/W/H) The low threshold for RX boost.
                                                                 The far-end TX POST direction output, pcs_mac_rx_txpost_dir, is set to
                                                                 decrement if the local RX boost value from the VMA (after RX-EQ) is
                                                                 lower than this value, and the local RX tap1 value is lower than its
                                                                 low threshold GSER()_RX_TXDIR_CTRL_1[RX_TAP1_LO_THRS].
                                                                 Note, that if GSER()_RX_TXDIR_CTRL_1[RX_TAP1_CHG_DIR]=1 then
                                                                 the direction is increment. */
        uint64_t rx_boost_hi_val       : 5;  /**< [  4:  0](R/W) The far-end TX POST direction output, pcs_mac_rx_txpost_dir,
                                                                 is set to increment if the local RX boost value from the VMA (after RX-EQ)
                                                                 equals RX_BOOST_HI_VAL.
                                                                 Note, that if GSER()_RX_TXDIR_CTRL_1[RX_TAP1_CHG_DIR]=1 then
                                                                 the direction is decrement.
                                                                 To disable the check against RX_BOOST_HI_VAL, assert RX_BOOST_HI_VAL[4]. */
#else /* Word 0 - Little Endian */
        uint64_t rx_boost_hi_val       : 5;  /**< [  4:  0](R/W) The far-end TX POST direction output, pcs_mac_rx_txpost_dir,
                                                                 is set to increment if the local RX boost value from the VMA (after RX-EQ)
                                                                 equals RX_BOOST_HI_VAL.
                                                                 Note, that if GSER()_RX_TXDIR_CTRL_1[RX_TAP1_CHG_DIR]=1 then
                                                                 the direction is decrement.
                                                                 To disable the check against RX_BOOST_HI_VAL, assert RX_BOOST_HI_VAL[4]. */
        uint64_t rx_boost_lo_thrs      : 4;  /**< [  8:  5](R/W/H) The low threshold for RX boost.
                                                                 The far-end TX POST direction output, pcs_mac_rx_txpost_dir, is set to
                                                                 decrement if the local RX boost value from the VMA (after RX-EQ) is
                                                                 lower than this value, and the local RX tap1 value is lower than its
                                                                 low threshold GSER()_RX_TXDIR_CTRL_1[RX_TAP1_LO_THRS].
                                                                 Note, that if GSER()_RX_TXDIR_CTRL_1[RX_TAP1_CHG_DIR]=1 then
                                                                 the direction is increment. */
        uint64_t rx_boost_hi_thrs      : 4;  /**< [ 12:  9](R/W/H) The high threshold for RX boost.
                                                                 The far-end TX POST direction output, pcs_mac_rx_txpost_dir, is set to
                                                                 increment if the local RX boost value from the VMA (after RX-EQ) is
                                                                 higher than this value, and the local RX tap1 value is higher than its
                                                                 high threshold GSER()_RX_TXDIR_CTRL_1[RX_TAP1_HI_THRS].
                                                                 Note, that if GSER()_RX_TXDIR_CTRL_1[RX_TAP1_CHG_DIR]=1 then
                                                                 the direction is decrement. */
        uint64_t reserved_13_63        : 51;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_rx_txdir_ctrl_0_s cn; */
};
typedef union bdk_gserx_rx_txdir_ctrl_0 bdk_gserx_rx_txdir_ctrl_0_t;

static inline uint64_t BDK_GSERX_RX_TXDIR_CTRL_0(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_RX_TXDIR_CTRL_0(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0904600e8ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0904600e8ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0904600e8ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_RX_TXDIR_CTRL_0", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_RX_TXDIR_CTRL_0(a) bdk_gserx_rx_txdir_ctrl_0_t
#define bustype_BDK_GSERX_RX_TXDIR_CTRL_0(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_RX_TXDIR_CTRL_0(a) "GSERX_RX_TXDIR_CTRL_0"
#define device_bar_BDK_GSERX_RX_TXDIR_CTRL_0(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_RX_TXDIR_CTRL_0(a) (a)
#define arguments_BDK_GSERX_RX_TXDIR_CTRL_0(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_rx_txdir_ctrl_1
 *
 * GSER Far-end TX Direction Control 1 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_rx_txdir_ctrl_1
{
    uint64_t u;
    struct bdk_gserx_rx_txdir_ctrl_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t rx_precorr_chg_dir    : 1;  /**< [ 11: 11](R/W/H) When asserted, the default direction output for the far-end TX Pre is reversed. */
        uint64_t rx_tap1_chg_dir       : 1;  /**< [ 10: 10](R/W/H) When asserted, the default direction output for the far-end TX Post is reversed. */
        uint64_t rx_tap1_hi_thrs       : 5;  /**< [  9:  5](R/W) The high threshold for the local RX Tap1 count.
                                                                 The far-end TX POST direction output, pcs_mac_rx_txpost_dir,
                                                                 is set to increment if the local RX tap1 value from the VMA (after RX-EQ)
                                                                 is higher than this value, and the local RX boost value is higher than
                                                                 its high threshold GSER()_RX_TXDIR_CTRL_0[RX_BOOST_HI_THRS].
                                                                 Note that if GSER()_RX_TXDIR_CTRL_1[RX_TAP1_CHG_DIR]=1 then
                                                                 the direction is decrement. */
        uint64_t rx_tap1_lo_thrs       : 5;  /**< [  4:  0](R/W) The low threshold for the local RX Tap1 count.
                                                                 The far-end TX POST direction output, pcs_mac_rx_txpost_dir,
                                                                 is set to decrement if the local RX tap1 value from the VMA (after RX-EQ)
                                                                 is lower than this value, and the local RX boost value is lower than
                                                                 its low threshold GSER()_RX_TXDIR_CTRL_0[RX_BOOST_LO_THRS].
                                                                 Note that if GSER()_RX_TXDIR_CTRL_1[RX_TAP1_CHG_DIR]=1 then
                                                                 the direction is increment. */
#else /* Word 0 - Little Endian */
        uint64_t rx_tap1_lo_thrs       : 5;  /**< [  4:  0](R/W) The low threshold for the local RX Tap1 count.
                                                                 The far-end TX POST direction output, pcs_mac_rx_txpost_dir,
                                                                 is set to decrement if the local RX tap1 value from the VMA (after RX-EQ)
                                                                 is lower than this value, and the local RX boost value is lower than
                                                                 its low threshold GSER()_RX_TXDIR_CTRL_0[RX_BOOST_LO_THRS].
                                                                 Note that if GSER()_RX_TXDIR_CTRL_1[RX_TAP1_CHG_DIR]=1 then
                                                                 the direction is increment. */
        uint64_t rx_tap1_hi_thrs       : 5;  /**< [  9:  5](R/W) The high threshold for the local RX Tap1 count.
                                                                 The far-end TX POST direction output, pcs_mac_rx_txpost_dir,
                                                                 is set to increment if the local RX tap1 value from the VMA (after RX-EQ)
                                                                 is higher than this value, and the local RX boost value is higher than
                                                                 its high threshold GSER()_RX_TXDIR_CTRL_0[RX_BOOST_HI_THRS].
                                                                 Note that if GSER()_RX_TXDIR_CTRL_1[RX_TAP1_CHG_DIR]=1 then
                                                                 the direction is decrement. */
        uint64_t rx_tap1_chg_dir       : 1;  /**< [ 10: 10](R/W/H) When asserted, the default direction output for the far-end TX Post is reversed. */
        uint64_t rx_precorr_chg_dir    : 1;  /**< [ 11: 11](R/W/H) When asserted, the default direction output for the far-end TX Pre is reversed. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_rx_txdir_ctrl_1_s cn81xx; */
    struct bdk_gserx_rx_txdir_ctrl_1_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t rx_precorr_chg_dir    : 1;  /**< [ 11: 11](R/W/H) When asserted, the default direction output for the far-end TX Pre is reversed. */
        uint64_t rx_tap1_chg_dir       : 1;  /**< [ 10: 10](R/W/H) When asserted, the default direction output for the far-end TX Post is reversed. */
        uint64_t rx_tap1_hi_thrs       : 5;  /**< [  9:  5](R/W) The high threshold for the local RX Tap1 count.
                                                                 The far-end TX POST direction output, pcs_mac_rx_txpost_dir,
                                                                 is set to increment if the local RX tap1 value from the VMA (after RX-EQ)
                                                                 is higher than this value, and the local RX boost value is higher than
                                                                 its high threshold GSER()_RX_TXDIR_CTRL_0[RX_BOOST_HI_THRS].
                                                                 If GSER()_RX_TXDIR_CTRL_1[RX_TAP1_CHG_DIR]=1 then
                                                                 the direction is decrement. */
        uint64_t rx_tap1_lo_thrs       : 5;  /**< [  4:  0](R/W) The low threshold for the local RX Tap1 count.
                                                                 The far-end TX POST direction output, pcs_mac_rx_txpost_dir,
                                                                 is set to decrement if the local RX tap1 value from the VMA (after RX-EQ)
                                                                 is lower than this value, and the local RX boost value is lower than
                                                                 its low threshold GSER()_RX_TXDIR_CTRL_0[RX_BOOST_LO_THRS].
                                                                 If GSER()_RX_TXDIR_CTRL_1[RX_TAP1_CHG_DIR]=1 then
                                                                 the direction is increment. */
#else /* Word 0 - Little Endian */
        uint64_t rx_tap1_lo_thrs       : 5;  /**< [  4:  0](R/W) The low threshold for the local RX Tap1 count.
                                                                 The far-end TX POST direction output, pcs_mac_rx_txpost_dir,
                                                                 is set to decrement if the local RX tap1 value from the VMA (after RX-EQ)
                                                                 is lower than this value, and the local RX boost value is lower than
                                                                 its low threshold GSER()_RX_TXDIR_CTRL_0[RX_BOOST_LO_THRS].
                                                                 If GSER()_RX_TXDIR_CTRL_1[RX_TAP1_CHG_DIR]=1 then
                                                                 the direction is increment. */
        uint64_t rx_tap1_hi_thrs       : 5;  /**< [  9:  5](R/W) The high threshold for the local RX Tap1 count.
                                                                 The far-end TX POST direction output, pcs_mac_rx_txpost_dir,
                                                                 is set to increment if the local RX tap1 value from the VMA (after RX-EQ)
                                                                 is higher than this value, and the local RX boost value is higher than
                                                                 its high threshold GSER()_RX_TXDIR_CTRL_0[RX_BOOST_HI_THRS].
                                                                 If GSER()_RX_TXDIR_CTRL_1[RX_TAP1_CHG_DIR]=1 then
                                                                 the direction is decrement. */
        uint64_t rx_tap1_chg_dir       : 1;  /**< [ 10: 10](R/W/H) When asserted, the default direction output for the far-end TX Post is reversed. */
        uint64_t rx_precorr_chg_dir    : 1;  /**< [ 11: 11](R/W/H) When asserted, the default direction output for the far-end TX Pre is reversed. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } cn88xx;
    /* struct bdk_gserx_rx_txdir_ctrl_1_s cn83xx; */
};
typedef union bdk_gserx_rx_txdir_ctrl_1 bdk_gserx_rx_txdir_ctrl_1_t;

static inline uint64_t BDK_GSERX_RX_TXDIR_CTRL_1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_RX_TXDIR_CTRL_1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0904600f0ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0904600f0ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0904600f0ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_RX_TXDIR_CTRL_1", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_RX_TXDIR_CTRL_1(a) bdk_gserx_rx_txdir_ctrl_1_t
#define bustype_BDK_GSERX_RX_TXDIR_CTRL_1(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_RX_TXDIR_CTRL_1(a) "GSERX_RX_TXDIR_CTRL_1"
#define device_bar_BDK_GSERX_RX_TXDIR_CTRL_1(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_RX_TXDIR_CTRL_1(a) (a)
#define arguments_BDK_GSERX_RX_TXDIR_CTRL_1(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_rx_txdir_ctrl_2
 *
 * GSER Far-end TX Direction Control 2 Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_rx_txdir_ctrl_2
{
    uint64_t u;
    struct bdk_gserx_rx_txdir_ctrl_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t rx_precorr_hi_thrs    : 8;  /**< [ 15:  8](R/W/H) High threshold for RX precursor correlation count.
                                                                 The far-end TX PRE direction output, pcs_mac_rx_txpre_dir, is set to
                                                                 decrement if the local RX precursor correlation count from the VMA (after RX-EQ)
                                                                 is lower than this value.
                                                                 Note, that if GSER()_RX_TXDIR_CTRL_1[RX_PRECORR_CHG_DIR]=1 then
                                                                 the direction is increment. */
        uint64_t rx_precorr_lo_thrs    : 8;  /**< [  7:  0](R/W/H) Low threshold for RX precursor correlation count.
                                                                 The far-end TX PRE direction output, pcs_mac_rx_txpre_dir, is set to
                                                                 increment if the local RX precursor correlation count from the VMA (after RX-EQ)
                                                                 is lower than this value.
                                                                 Note, that if GSER()_RX_TXDIR_CTRL_1[RX_PRECORR_CHG_DIR]=1 then
                                                                 the direction is decrement. */
#else /* Word 0 - Little Endian */
        uint64_t rx_precorr_lo_thrs    : 8;  /**< [  7:  0](R/W/H) Low threshold for RX precursor correlation count.
                                                                 The far-end TX PRE direction output, pcs_mac_rx_txpre_dir, is set to
                                                                 increment if the local RX precursor correlation count from the VMA (after RX-EQ)
                                                                 is lower than this value.
                                                                 Note, that if GSER()_RX_TXDIR_CTRL_1[RX_PRECORR_CHG_DIR]=1 then
                                                                 the direction is decrement. */
        uint64_t rx_precorr_hi_thrs    : 8;  /**< [ 15:  8](R/W/H) High threshold for RX precursor correlation count.
                                                                 The far-end TX PRE direction output, pcs_mac_rx_txpre_dir, is set to
                                                                 decrement if the local RX precursor correlation count from the VMA (after RX-EQ)
                                                                 is lower than this value.
                                                                 Note, that if GSER()_RX_TXDIR_CTRL_1[RX_PRECORR_CHG_DIR]=1 then
                                                                 the direction is increment. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_rx_txdir_ctrl_2_s cn; */
};
typedef union bdk_gserx_rx_txdir_ctrl_2 bdk_gserx_rx_txdir_ctrl_2_t;

static inline uint64_t BDK_GSERX_RX_TXDIR_CTRL_2(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_RX_TXDIR_CTRL_2(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e0904600f8ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e0904600f8ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e0904600f8ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_RX_TXDIR_CTRL_2", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_RX_TXDIR_CTRL_2(a) bdk_gserx_rx_txdir_ctrl_2_t
#define bustype_BDK_GSERX_RX_TXDIR_CTRL_2(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_RX_TXDIR_CTRL_2(a) "GSERX_RX_TXDIR_CTRL_2"
#define device_bar_BDK_GSERX_RX_TXDIR_CTRL_2(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_RX_TXDIR_CTRL_2(a) (a)
#define arguments_BDK_GSERX_RX_TXDIR_CTRL_2(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_sata_lane#_tx_amp#
 *
 * GSER SATA Lane Transmit Amplitude Gen Register
 * SATA lane TX launch amplitude at Gen 1, 2 and 3 speeds.
 * * AMP(0) is for Gen1.
 * * AMP(1) is for Gen2.
 * * AMP(2) is for Gen3.
 */
union bdk_gserx_sata_lanex_tx_ampx
{
    uint64_t u;
    struct bdk_gserx_sata_lanex_tx_ampx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_7_63         : 57;
        uint64_t tx_amp                : 7;  /**< [  6:  0](R/W) This status value sets the TX driver launch amplitude in the
                                                                 case where the PHY is running at the Gen1, Gen2, and Gen3
                                                                 rates. Used for tuning at the board level for RX eye compliance. */
#else /* Word 0 - Little Endian */
        uint64_t tx_amp                : 7;  /**< [  6:  0](R/W) This status value sets the TX driver launch amplitude in the
                                                                 case where the PHY is running at the Gen1, Gen2, and Gen3
                                                                 rates. Used for tuning at the board level for RX eye compliance. */
        uint64_t reserved_7_63         : 57;
#endif /* Word 0 - End */
    } s;
    struct bdk_gserx_sata_lanex_tx_ampx_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_7_63         : 57;
        uint64_t tx_amp                : 7;  /**< [  6:  0](R/W) This status value sets the TX driver launch amplitude in the
                                                                 case where the PHY is running at the Gen1, Gen2, and Gen3
                                                                 rates. Used for tuning at the board level for RX eye compliance.
                                                                 This register is used for SATA lanes only for GSER(3). */
#else /* Word 0 - Little Endian */
        uint64_t tx_amp                : 7;  /**< [  6:  0](R/W) This status value sets the TX driver launch amplitude in the
                                                                 case where the PHY is running at the Gen1, Gen2, and Gen3
                                                                 rates. Used for tuning at the board level for RX eye compliance.
                                                                 This register is used for SATA lanes only for GSER(3). */
        uint64_t reserved_7_63         : 57;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_gserx_sata_lanex_tx_ampx_s cn88xx; */
    struct bdk_gserx_sata_lanex_tx_ampx_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_7_63         : 57;
        uint64_t tx_amp                : 7;  /**< [  6:  0](R/W) This status value sets the TX driver launch amplitude in the
                                                                 case where the PHY is running at the Gen1, Gen2, and Gen3
                                                                 rates. Used for tuning at the board level for RX eye compliance.
                                                                 This register is used for SATA lanes only GSER(4..6).

                                                                 Only SATA lanes 0 and 1 are used. */
#else /* Word 0 - Little Endian */
        uint64_t tx_amp                : 7;  /**< [  6:  0](R/W) This status value sets the TX driver launch amplitude in the
                                                                 case where the PHY is running at the Gen1, Gen2, and Gen3
                                                                 rates. Used for tuning at the board level for RX eye compliance.
                                                                 This register is used for SATA lanes only GSER(4..6).

                                                                 Only SATA lanes 0 and 1 are used. */
        uint64_t reserved_7_63         : 57;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_sata_lanex_tx_ampx bdk_gserx_sata_lanex_tx_ampx_t;

static inline uint64_t BDK_GSERX_SATA_LANEX_TX_AMPX(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SATA_LANEX_TX_AMPX(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1) && (c<=2)))
        return 0x87e090000b00ll + 0x1000000ll * ((a) & 0x3) + 0x20ll * ((b) & 0x1) + 8ll * ((c) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3) && (c<=2)))
        return 0x87e090000b00ll + 0x1000000ll * ((a) & 0x7) + 0x20ll * ((b) & 0x3) + 8ll * ((c) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3) && (c<=2)))
        return 0x87e090000b00ll + 0x1000000ll * ((a) & 0xf) + 0x20ll * ((b) & 0x3) + 8ll * ((c) & 0x3);
    __bdk_csr_fatal("GSERX_SATA_LANEX_TX_AMPX", 3, a, b, c, 0);
}

#define typedef_BDK_GSERX_SATA_LANEX_TX_AMPX(a,b,c) bdk_gserx_sata_lanex_tx_ampx_t
#define bustype_BDK_GSERX_SATA_LANEX_TX_AMPX(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SATA_LANEX_TX_AMPX(a,b,c) "GSERX_SATA_LANEX_TX_AMPX"
#define device_bar_BDK_GSERX_SATA_LANEX_TX_AMPX(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SATA_LANEX_TX_AMPX(a,b,c) (a)
#define arguments_BDK_GSERX_SATA_LANEX_TX_AMPX(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) gser#_sata_lane#_tx_preemph#
 *
 * GSER SATA Lane Transmit Preemphsis Gen Register
 * SATA TX preemphasis at Gen 1, 2 and 3 speeds. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 * * PREEMPH(0) is for Gen1.
 * * PREEMPH(1) is for Gen2.
 * * PREEMPH(2) is for Gen3.
 */
union bdk_gserx_sata_lanex_tx_preemphx
{
    uint64_t u;
    struct bdk_gserx_sata_lanex_tx_preemphx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_7_63         : 57;
        uint64_t tx_preemph            : 7;  /**< [  6:  0](R/W/H) This static value sets the TX driver deemphasis value in the
                                                                 case where the PHY is running at the Gen1, Gen2, and Gen3
                                                                 rates. Used for tuning at the board level for RX eye compliance. */
#else /* Word 0 - Little Endian */
        uint64_t tx_preemph            : 7;  /**< [  6:  0](R/W/H) This static value sets the TX driver deemphasis value in the
                                                                 case where the PHY is running at the Gen1, Gen2, and Gen3
                                                                 rates. Used for tuning at the board level for RX eye compliance. */
        uint64_t reserved_7_63         : 57;
#endif /* Word 0 - End */
    } s;
    struct bdk_gserx_sata_lanex_tx_preemphx_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_7_63         : 57;
        uint64_t tx_preemph            : 7;  /**< [  6:  0](R/W/H) This static value sets the TX driver deemphasis value in the
                                                                 case where the PHY is running at the Gen1, Gen2, and Gen3
                                                                 rates. Used for tuning at the board level for RX eye compliance.
                                                                 This register is used for SATA lanes only for GSER(3). */
#else /* Word 0 - Little Endian */
        uint64_t tx_preemph            : 7;  /**< [  6:  0](R/W/H) This static value sets the TX driver deemphasis value in the
                                                                 case where the PHY is running at the Gen1, Gen2, and Gen3
                                                                 rates. Used for tuning at the board level for RX eye compliance.
                                                                 This register is used for SATA lanes only for GSER(3). */
        uint64_t reserved_7_63         : 57;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_gserx_sata_lanex_tx_preemphx_s cn88xx; */
    struct bdk_gserx_sata_lanex_tx_preemphx_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_7_63         : 57;
        uint64_t tx_preemph            : 7;  /**< [  6:  0](R/W/H) This static value sets the TX driver deemphasis value in the
                                                                 case where the PHY is running at the Gen1, Gen2, and Gen3
                                                                 rates. Used for tuning at the board level for RX eye compliance.

                                                                 This register is used for SATA lanes only GSER(4..6).
                                                                 Only SATA lanes 0 and 1 are used. */
#else /* Word 0 - Little Endian */
        uint64_t tx_preemph            : 7;  /**< [  6:  0](R/W/H) This static value sets the TX driver deemphasis value in the
                                                                 case where the PHY is running at the Gen1, Gen2, and Gen3
                                                                 rates. Used for tuning at the board level for RX eye compliance.

                                                                 This register is used for SATA lanes only GSER(4..6).
                                                                 Only SATA lanes 0 and 1 are used. */
        uint64_t reserved_7_63         : 57;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_sata_lanex_tx_preemphx bdk_gserx_sata_lanex_tx_preemphx_t;

static inline uint64_t BDK_GSERX_SATA_LANEX_TX_PREEMPHX(unsigned long a, unsigned long b, unsigned long c) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SATA_LANEX_TX_PREEMPHX(unsigned long a, unsigned long b, unsigned long c)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1) && (c<=2)))
        return 0x87e090000a00ll + 0x1000000ll * ((a) & 0x3) + 0x20ll * ((b) & 0x1) + 8ll * ((c) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=3) && (c<=2)))
        return 0x87e090000a00ll + 0x1000000ll * ((a) & 0x7) + 0x20ll * ((b) & 0x3) + 8ll * ((c) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=3) && (c<=2)))
        return 0x87e090000a00ll + 0x1000000ll * ((a) & 0xf) + 0x20ll * ((b) & 0x3) + 8ll * ((c) & 0x3);
    __bdk_csr_fatal("GSERX_SATA_LANEX_TX_PREEMPHX", 3, a, b, c, 0);
}

#define typedef_BDK_GSERX_SATA_LANEX_TX_PREEMPHX(a,b,c) bdk_gserx_sata_lanex_tx_preemphx_t
#define bustype_BDK_GSERX_SATA_LANEX_TX_PREEMPHX(a,b,c) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SATA_LANEX_TX_PREEMPHX(a,b,c) "GSERX_SATA_LANEX_TX_PREEMPHX"
#define device_bar_BDK_GSERX_SATA_LANEX_TX_PREEMPHX(a,b,c) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SATA_LANEX_TX_PREEMPHX(a,b,c) (a)
#define arguments_BDK_GSERX_SATA_LANEX_TX_PREEMPHX(a,b,c) (a),(b),(c),-1

/**
 * Register (RSL) gser#_sata_lane_rst
 *
 * GSER SATA Lane Reset Register
 * Lane Reset Control.
 */
union bdk_gserx_sata_lane_rst
{
    uint64_t u;
    struct bdk_gserx_sata_lane_rst_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t l3_rst                : 1;  /**< [  3:  3](R/W) Independent reset for Lane 3. */
        uint64_t l2_rst                : 1;  /**< [  2:  2](R/W) Independent reset for Lane 2. */
        uint64_t l1_rst                : 1;  /**< [  1:  1](R/W) Independent reset for Lane 1. */
        uint64_t l0_rst                : 1;  /**< [  0:  0](R/W) Independent reset for Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t l0_rst                : 1;  /**< [  0:  0](R/W) Independent reset for Lane 0. */
        uint64_t l1_rst                : 1;  /**< [  1:  1](R/W) Independent reset for Lane 1. */
        uint64_t l2_rst                : 1;  /**< [  2:  2](R/W) Independent reset for Lane 2. */
        uint64_t l3_rst                : 1;  /**< [  3:  3](R/W) Independent reset for Lane 3. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    struct bdk_gserx_sata_lane_rst_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t l3_rst                : 1;  /**< [  3:  3](RO/H) Reserved. */
        uint64_t l2_rst                : 1;  /**< [  2:  2](RO/H) Reserved. */
        uint64_t l1_rst                : 1;  /**< [  1:  1](R/W) Independent reset for lane 1.
                                                                 This register is used for SATA lanes only for GSER(3). */
        uint64_t l0_rst                : 1;  /**< [  0:  0](R/W) Independent reset for lane 0.
                                                                 This register is used for SATA lanes only for GSER(3). */
#else /* Word 0 - Little Endian */
        uint64_t l0_rst                : 1;  /**< [  0:  0](R/W) Independent reset for lane 0.
                                                                 This register is used for SATA lanes only for GSER(3). */
        uint64_t l1_rst                : 1;  /**< [  1:  1](R/W) Independent reset for lane 1.
                                                                 This register is used for SATA lanes only for GSER(3). */
        uint64_t l2_rst                : 1;  /**< [  2:  2](RO/H) Reserved. */
        uint64_t l3_rst                : 1;  /**< [  3:  3](RO/H) Reserved. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_gserx_sata_lane_rst_s cn88xx; */
    struct bdk_gserx_sata_lane_rst_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t l3_rst                : 1;  /**< [  3:  3](RO/H) Reserved. */
        uint64_t l2_rst                : 1;  /**< [  2:  2](RO/H) Reserved. */
        uint64_t l1_rst                : 1;  /**< [  1:  1](R/W) Independent reset for lane 1.
                                                                 This register is used for SATA lanes only for GSER(4..6). */
        uint64_t l0_rst                : 1;  /**< [  0:  0](R/W) Independent reset for lane 0.
                                                                 This register is used for SATA lanes only for GSER(4..6). */
#else /* Word 0 - Little Endian */
        uint64_t l0_rst                : 1;  /**< [  0:  0](R/W) Independent reset for lane 0.
                                                                 This register is used for SATA lanes only for GSER(4..6). */
        uint64_t l1_rst                : 1;  /**< [  1:  1](R/W) Independent reset for lane 1.
                                                                 This register is used for SATA lanes only for GSER(4..6). */
        uint64_t l2_rst                : 1;  /**< [  2:  2](RO/H) Reserved. */
        uint64_t l3_rst                : 1;  /**< [  3:  3](RO/H) Reserved. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_sata_lane_rst bdk_gserx_sata_lane_rst_t;

static inline uint64_t BDK_GSERX_SATA_LANE_RST(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SATA_LANE_RST(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000908ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000908ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000908ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_SATA_LANE_RST", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_SATA_LANE_RST(a) bdk_gserx_sata_lane_rst_t
#define bustype_BDK_GSERX_SATA_LANE_RST(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SATA_LANE_RST(a) "GSERX_SATA_LANE_RST"
#define device_bar_BDK_GSERX_SATA_LANE_RST(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SATA_LANE_RST(a) (a)
#define arguments_BDK_GSERX_SATA_LANE_RST(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_sata_status
 *
 * GSER SATA Status Register
 * SATA PHY Ready Status.
 */
union bdk_gserx_sata_status
{
    uint64_t u;
    struct bdk_gserx_sata_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t p3_rdy                : 1;  /**< [  3:  3](RO/H) PHY Lane 3 is ready to send and receive data. */
        uint64_t p2_rdy                : 1;  /**< [  2:  2](RO/H) PHY Lane 2 is ready to send and receive data. */
        uint64_t p1_rdy                : 1;  /**< [  1:  1](RO/H) PHY Lane 1 is ready to send and receive data. */
        uint64_t p0_rdy                : 1;  /**< [  0:  0](RO/H) PHY Lane 0 is ready to send and receive data. */
#else /* Word 0 - Little Endian */
        uint64_t p0_rdy                : 1;  /**< [  0:  0](RO/H) PHY Lane 0 is ready to send and receive data. */
        uint64_t p1_rdy                : 1;  /**< [  1:  1](RO/H) PHY Lane 1 is ready to send and receive data. */
        uint64_t p2_rdy                : 1;  /**< [  2:  2](RO/H) PHY Lane 2 is ready to send and receive data. */
        uint64_t p3_rdy                : 1;  /**< [  3:  3](RO/H) PHY Lane 3 is ready to send and receive data. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    struct bdk_gserx_sata_status_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t p3_rdy                : 1;  /**< [  3:  3](RO/H) Reserved. */
        uint64_t p2_rdy                : 1;  /**< [  2:  2](RO/H) Reserved. */
        uint64_t p1_rdy                : 1;  /**< [  1:  1](RO/H) PHY lane 1 is ready to send and receive data.
                                                                 This register is used for SATA lanes only for GSER(3). */
        uint64_t p0_rdy                : 1;  /**< [  0:  0](RO/H) PHY lane 0 is ready to send and receive data.
                                                                 This register is used for SATA lanes only for GSER(3). */
#else /* Word 0 - Little Endian */
        uint64_t p0_rdy                : 1;  /**< [  0:  0](RO/H) PHY lane 0 is ready to send and receive data.
                                                                 This register is used for SATA lanes only for GSER(3). */
        uint64_t p1_rdy                : 1;  /**< [  1:  1](RO/H) PHY lane 1 is ready to send and receive data.
                                                                 This register is used for SATA lanes only for GSER(3). */
        uint64_t p2_rdy                : 1;  /**< [  2:  2](RO/H) Reserved. */
        uint64_t p3_rdy                : 1;  /**< [  3:  3](RO/H) Reserved. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_gserx_sata_status_s cn88xx; */
    struct bdk_gserx_sata_status_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t p3_rdy                : 1;  /**< [  3:  3](RO/H) Reserved. */
        uint64_t p2_rdy                : 1;  /**< [  2:  2](RO/H) Reserved. */
        uint64_t p1_rdy                : 1;  /**< [  1:  1](RO/H) PHY lane 1 is ready to send and receive data.
                                                                 This register is used for SATA lanes only GSER(4..6). */
        uint64_t p0_rdy                : 1;  /**< [  0:  0](RO/H) PHY lane 0 is ready to send and receive data.
                                                                 This register is used for SATA lanes only GSER(4..6). */
#else /* Word 0 - Little Endian */
        uint64_t p0_rdy                : 1;  /**< [  0:  0](RO/H) PHY lane 0 is ready to send and receive data.
                                                                 This register is used for SATA lanes only GSER(4..6). */
        uint64_t p1_rdy                : 1;  /**< [  1:  1](RO/H) PHY lane 1 is ready to send and receive data.
                                                                 This register is used for SATA lanes only GSER(4..6). */
        uint64_t p2_rdy                : 1;  /**< [  2:  2](RO/H) Reserved. */
        uint64_t p3_rdy                : 1;  /**< [  3:  3](RO/H) Reserved. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_sata_status bdk_gserx_sata_status_t;

static inline uint64_t BDK_GSERX_SATA_STATUS(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SATA_STATUS(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090100900ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090100900ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090100900ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_SATA_STATUS", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_SATA_STATUS(a) bdk_gserx_sata_status_t
#define bustype_BDK_GSERX_SATA_STATUS(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SATA_STATUS(a) "GSERX_SATA_STATUS"
#define device_bar_BDK_GSERX_SATA_STATUS(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SATA_STATUS(a) (a)
#define arguments_BDK_GSERX_SATA_STATUS(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_sata_tx_invert
 *
 * GSER SATA TX Invert Register
 * Lane Reset Control.
 */
union bdk_gserx_sata_tx_invert
{
    uint64_t u;
    struct bdk_gserx_sata_tx_invert_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t l3_inv                : 1;  /**< [  3:  3](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 3 transmitted data. */
        uint64_t l2_inv                : 1;  /**< [  2:  2](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 2 transmitted data. */
        uint64_t l1_inv                : 1;  /**< [  1:  1](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 1 transmitted data. */
        uint64_t l0_inv                : 1;  /**< [  0:  0](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 0 transmitted data. */
#else /* Word 0 - Little Endian */
        uint64_t l0_inv                : 1;  /**< [  0:  0](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 0 transmitted data. */
        uint64_t l1_inv                : 1;  /**< [  1:  1](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 1 transmitted data. */
        uint64_t l2_inv                : 1;  /**< [  2:  2](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 2 transmitted data. */
        uint64_t l3_inv                : 1;  /**< [  3:  3](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 3 transmitted data. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    struct bdk_gserx_sata_tx_invert_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t l3_inv                : 1;  /**< [  3:  3](RO/H) Reserved. */
        uint64_t l2_inv                : 1;  /**< [  2:  2](RO/H) Reserved. */
        uint64_t l1_inv                : 1;  /**< [  1:  1](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 1 transmitted data.
                                                                 This register is used for SATA lanes only for GSER(3). */
        uint64_t l0_inv                : 1;  /**< [  0:  0](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 0 transmitted data.
                                                                 This register is used for SATA lanes only for GSER(3). */
#else /* Word 0 - Little Endian */
        uint64_t l0_inv                : 1;  /**< [  0:  0](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 0 transmitted data.
                                                                 This register is used for SATA lanes only for GSER(3). */
        uint64_t l1_inv                : 1;  /**< [  1:  1](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 1 transmitted data.
                                                                 This register is used for SATA lanes only for GSER(3). */
        uint64_t l2_inv                : 1;  /**< [  2:  2](RO/H) Reserved. */
        uint64_t l3_inv                : 1;  /**< [  3:  3](RO/H) Reserved. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_gserx_sata_tx_invert_s cn88xx; */
    struct bdk_gserx_sata_tx_invert_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t l3_inv                : 1;  /**< [  3:  3](RO/H) Reserved. */
        uint64_t l2_inv                : 1;  /**< [  2:  2](RO/H) Reserved. */
        uint64_t l1_inv                : 1;  /**< [  1:  1](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 1 transmitted data.
                                                                 This register is used for SATA lanes only for GSER(4..6). */
        uint64_t l0_inv                : 1;  /**< [  0:  0](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 0 transmitted data.
                                                                 This register is used for SATA lanes only for GSER(4..6). */
#else /* Word 0 - Little Endian */
        uint64_t l0_inv                : 1;  /**< [  0:  0](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 0 transmitted data.
                                                                 This register is used for SATA lanes only for GSER(4..6). */
        uint64_t l1_inv                : 1;  /**< [  1:  1](R/W) Instructs the SATA PCS to perform a polarity inversion on the
                                                                 lane 1 transmitted data.
                                                                 This register is used for SATA lanes only for GSER(4..6). */
        uint64_t l2_inv                : 1;  /**< [  2:  2](RO/H) Reserved. */
        uint64_t l3_inv                : 1;  /**< [  3:  3](RO/H) Reserved. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_sata_tx_invert bdk_gserx_sata_tx_invert_t;

static inline uint64_t BDK_GSERX_SATA_TX_INVERT(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SATA_TX_INVERT(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000910ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000910ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000910ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_SATA_TX_INVERT", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_SATA_TX_INVERT(a) bdk_gserx_sata_tx_invert_t
#define bustype_BDK_GSERX_SATA_TX_INVERT(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SATA_TX_INVERT(a) "GSERX_SATA_TX_INVERT"
#define device_bar_BDK_GSERX_SATA_TX_INVERT(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SATA_TX_INVERT(a) (a)
#define arguments_BDK_GSERX_SATA_TX_INVERT(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_scratch
 *
 * GSER General Purpose Scratch Register
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_scratch
{
    uint64_t u;
    struct bdk_gserx_scratch_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t scratch               : 16; /**< [ 15:  0](R/W) General purpose scratch register. */
#else /* Word 0 - Little Endian */
        uint64_t scratch               : 16; /**< [ 15:  0](R/W) General purpose scratch register. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_scratch_s cn; */
};
typedef union bdk_gserx_scratch bdk_gserx_scratch_t;

static inline uint64_t BDK_GSERX_SCRATCH(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SCRATCH(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000020ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000020ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000020ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_SCRATCH", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_SCRATCH(a) bdk_gserx_scratch_t
#define bustype_BDK_GSERX_SCRATCH(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SCRATCH(a) "GSERX_SCRATCH"
#define device_bar_BDK_GSERX_SCRATCH(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SCRATCH(a) (a)
#define arguments_BDK_GSERX_SCRATCH(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_slice#_cei_6g_sr_mode
 *
 * GSER Slice CEI_6G_SR MODE Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_slicex_cei_6g_sr_mode
{
    uint64_t u;
    struct bdk_gserx_slicex_cei_6g_sr_mode_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
#else /* Word 0 - Little Endian */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_slicex_cei_6g_sr_mode_s cn; */
};
typedef union bdk_gserx_slicex_cei_6g_sr_mode bdk_gserx_slicex_cei_6g_sr_mode_t;

static inline uint64_t BDK_GSERX_SLICEX_CEI_6G_SR_MODE(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SLICEX_CEI_6G_SR_MODE(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090460268ll + 0x1000000ll * ((a) & 0x3) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=1)))
        return 0x87e090460268ll + 0x1000000ll * ((a) & 0x7) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=1)))
        return 0x87e090460268ll + 0x1000000ll * ((a) & 0xf) + 0x200000ll * ((b) & 0x1);
    __bdk_csr_fatal("GSERX_SLICEX_CEI_6G_SR_MODE", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_SLICEX_CEI_6G_SR_MODE(a,b) bdk_gserx_slicex_cei_6g_sr_mode_t
#define bustype_BDK_GSERX_SLICEX_CEI_6G_SR_MODE(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SLICEX_CEI_6G_SR_MODE(a,b) "GSERX_SLICEX_CEI_6G_SR_MODE"
#define device_bar_BDK_GSERX_SLICEX_CEI_6G_SR_MODE(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SLICEX_CEI_6G_SR_MODE(a,b) (a)
#define arguments_BDK_GSERX_SLICEX_CEI_6G_SR_MODE(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_slice#_kr_mode
 *
 * GSER Slice KR MODE Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_slicex_kr_mode
{
    uint64_t u;
    struct bdk_gserx_slicex_kr_mode_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
#else /* Word 0 - Little Endian */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_slicex_kr_mode_s cn; */
};
typedef union bdk_gserx_slicex_kr_mode bdk_gserx_slicex_kr_mode_t;

static inline uint64_t BDK_GSERX_SLICEX_KR_MODE(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SLICEX_KR_MODE(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090460250ll + 0x1000000ll * ((a) & 0x3) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=1)))
        return 0x87e090460250ll + 0x1000000ll * ((a) & 0x7) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=1)))
        return 0x87e090460250ll + 0x1000000ll * ((a) & 0xf) + 0x200000ll * ((b) & 0x1);
    __bdk_csr_fatal("GSERX_SLICEX_KR_MODE", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_SLICEX_KR_MODE(a,b) bdk_gserx_slicex_kr_mode_t
#define bustype_BDK_GSERX_SLICEX_KR_MODE(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SLICEX_KR_MODE(a,b) "GSERX_SLICEX_KR_MODE"
#define device_bar_BDK_GSERX_SLICEX_KR_MODE(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SLICEX_KR_MODE(a,b) (a)
#define arguments_BDK_GSERX_SLICEX_KR_MODE(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_slice#_kx4_mode
 *
 * GSER Slice KX4 MODE Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_slicex_kx4_mode
{
    uint64_t u;
    struct bdk_gserx_slicex_kx4_mode_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
#else /* Word 0 - Little Endian */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_slicex_kx4_mode_s cn; */
};
typedef union bdk_gserx_slicex_kx4_mode bdk_gserx_slicex_kx4_mode_t;

static inline uint64_t BDK_GSERX_SLICEX_KX4_MODE(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SLICEX_KX4_MODE(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090460248ll + 0x1000000ll * ((a) & 0x3) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=1)))
        return 0x87e090460248ll + 0x1000000ll * ((a) & 0x7) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=1)))
        return 0x87e090460248ll + 0x1000000ll * ((a) & 0xf) + 0x200000ll * ((b) & 0x1);
    __bdk_csr_fatal("GSERX_SLICEX_KX4_MODE", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_SLICEX_KX4_MODE(a,b) bdk_gserx_slicex_kx4_mode_t
#define bustype_BDK_GSERX_SLICEX_KX4_MODE(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SLICEX_KX4_MODE(a,b) "GSERX_SLICEX_KX4_MODE"
#define device_bar_BDK_GSERX_SLICEX_KX4_MODE(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SLICEX_KX4_MODE(a,b) (a)
#define arguments_BDK_GSERX_SLICEX_KX4_MODE(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_slice#_kx_mode
 *
 * GSER Slice KX MODE Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_slicex_kx_mode
{
    uint64_t u;
    struct bdk_gserx_slicex_kx_mode_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
#else /* Word 0 - Little Endian */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_slicex_kx_mode_s cn; */
};
typedef union bdk_gserx_slicex_kx_mode bdk_gserx_slicex_kx_mode_t;

static inline uint64_t BDK_GSERX_SLICEX_KX_MODE(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SLICEX_KX_MODE(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090460240ll + 0x1000000ll * ((a) & 0x3) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=1)))
        return 0x87e090460240ll + 0x1000000ll * ((a) & 0x7) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=1)))
        return 0x87e090460240ll + 0x1000000ll * ((a) & 0xf) + 0x200000ll * ((b) & 0x1);
    __bdk_csr_fatal("GSERX_SLICEX_KX_MODE", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_SLICEX_KX_MODE(a,b) bdk_gserx_slicex_kx_mode_t
#define bustype_BDK_GSERX_SLICEX_KX_MODE(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SLICEX_KX_MODE(a,b) "GSERX_SLICEX_KX_MODE"
#define device_bar_BDK_GSERX_SLICEX_KX_MODE(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SLICEX_KX_MODE(a,b) (a)
#define arguments_BDK_GSERX_SLICEX_KX_MODE(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_slice#_pcie1_mode
 *
 * GSER Slice PCIE1 MODE Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_slicex_pcie1_mode
{
    uint64_t u;
    struct bdk_gserx_slicex_pcie1_mode_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved.

                                                                 In SATA Mode program RX_SDLL_BWSEL = 0x1. */
#else /* Word 0 - Little Endian */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved.

                                                                 In SATA Mode program RX_SDLL_BWSEL = 0x1. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_slicex_pcie1_mode_s cn81xx; */
    struct bdk_gserx_slicex_pcie1_mode_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
#else /* Word 0 - Little Endian */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } cn88xx;
    /* struct bdk_gserx_slicex_pcie1_mode_cn88xx cn83xx; */
};
typedef union bdk_gserx_slicex_pcie1_mode bdk_gserx_slicex_pcie1_mode_t;

static inline uint64_t BDK_GSERX_SLICEX_PCIE1_MODE(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SLICEX_PCIE1_MODE(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090460228ll + 0x1000000ll * ((a) & 0x3) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=1)))
        return 0x87e090460228ll + 0x1000000ll * ((a) & 0x7) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=1)))
        return 0x87e090460228ll + 0x1000000ll * ((a) & 0xf) + 0x200000ll * ((b) & 0x1);
    __bdk_csr_fatal("GSERX_SLICEX_PCIE1_MODE", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_SLICEX_PCIE1_MODE(a,b) bdk_gserx_slicex_pcie1_mode_t
#define bustype_BDK_GSERX_SLICEX_PCIE1_MODE(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SLICEX_PCIE1_MODE(a,b) "GSERX_SLICEX_PCIE1_MODE"
#define device_bar_BDK_GSERX_SLICEX_PCIE1_MODE(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SLICEX_PCIE1_MODE(a,b) (a)
#define arguments_BDK_GSERX_SLICEX_PCIE1_MODE(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_slice#_pcie2_mode
 *
 * GSER Slice PCIE2 MODE Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_slicex_pcie2_mode
{
    uint64_t u;
    struct bdk_gserx_slicex_pcie2_mode_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved.

                                                                 In SATA Mode program RX_SDLL_BWSEL = 0x1. */
#else /* Word 0 - Little Endian */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved.

                                                                 In SATA Mode program RX_SDLL_BWSEL = 0x1. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_slicex_pcie2_mode_s cn81xx; */
    /* struct bdk_gserx_slicex_pcie2_mode_s cn88xx; */
    struct bdk_gserx_slicex_pcie2_mode_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
#else /* Word 0 - Little Endian */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_slicex_pcie2_mode bdk_gserx_slicex_pcie2_mode_t;

static inline uint64_t BDK_GSERX_SLICEX_PCIE2_MODE(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SLICEX_PCIE2_MODE(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090460230ll + 0x1000000ll * ((a) & 0x3) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=1)))
        return 0x87e090460230ll + 0x1000000ll * ((a) & 0x7) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=1)))
        return 0x87e090460230ll + 0x1000000ll * ((a) & 0xf) + 0x200000ll * ((b) & 0x1);
    __bdk_csr_fatal("GSERX_SLICEX_PCIE2_MODE", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_SLICEX_PCIE2_MODE(a,b) bdk_gserx_slicex_pcie2_mode_t
#define bustype_BDK_GSERX_SLICEX_PCIE2_MODE(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SLICEX_PCIE2_MODE(a,b) "GSERX_SLICEX_PCIE2_MODE"
#define device_bar_BDK_GSERX_SLICEX_PCIE2_MODE(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SLICEX_PCIE2_MODE(a,b) (a)
#define arguments_BDK_GSERX_SLICEX_PCIE2_MODE(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_slice#_pcie3_mode
 *
 * GSER Slice PCIE3 MODE Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_slicex_pcie3_mode
{
    uint64_t u;
    struct bdk_gserx_slicex_pcie3_mode_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
#else /* Word 0 - Little Endian */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_slicex_pcie3_mode_s cn81xx; */
    struct bdk_gserx_slicex_pcie3_mode_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved.

                                                                 In SATA Mode program RX_SDLL_BWSEL = 0x1. */
#else /* Word 0 - Little Endian */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved.

                                                                 In SATA Mode program RX_SDLL_BWSEL = 0x1. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz, or SATA mode.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } cn88xx;
    /* struct bdk_gserx_slicex_pcie3_mode_s cn83xx; */
};
typedef union bdk_gserx_slicex_pcie3_mode bdk_gserx_slicex_pcie3_mode_t;

static inline uint64_t BDK_GSERX_SLICEX_PCIE3_MODE(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SLICEX_PCIE3_MODE(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090460238ll + 0x1000000ll * ((a) & 0x3) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=1)))
        return 0x87e090460238ll + 0x1000000ll * ((a) & 0x7) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=1)))
        return 0x87e090460238ll + 0x1000000ll * ((a) & 0xf) + 0x200000ll * ((b) & 0x1);
    __bdk_csr_fatal("GSERX_SLICEX_PCIE3_MODE", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_SLICEX_PCIE3_MODE(a,b) bdk_gserx_slicex_pcie3_mode_t
#define bustype_BDK_GSERX_SLICEX_PCIE3_MODE(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SLICEX_PCIE3_MODE(a,b) "GSERX_SLICEX_PCIE3_MODE"
#define device_bar_BDK_GSERX_SLICEX_PCIE3_MODE(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SLICEX_PCIE3_MODE(a,b) (a)
#define arguments_BDK_GSERX_SLICEX_PCIE3_MODE(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_slice#_qsgmii_mode
 *
 * GSER Slice QSGMII MODE Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_slicex_qsgmii_mode
{
    uint64_t u;
    struct bdk_gserx_slicex_qsgmii_mode_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
#else /* Word 0 - Little Endian */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_slicex_qsgmii_mode_s cn; */
};
typedef union bdk_gserx_slicex_qsgmii_mode bdk_gserx_slicex_qsgmii_mode_t;

static inline uint64_t BDK_GSERX_SLICEX_QSGMII_MODE(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SLICEX_QSGMII_MODE(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090460260ll + 0x1000000ll * ((a) & 0x3) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=1)))
        return 0x87e090460260ll + 0x1000000ll * ((a) & 0x7) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=1)))
        return 0x87e090460260ll + 0x1000000ll * ((a) & 0xf) + 0x200000ll * ((b) & 0x1);
    __bdk_csr_fatal("GSERX_SLICEX_QSGMII_MODE", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_SLICEX_QSGMII_MODE(a,b) bdk_gserx_slicex_qsgmii_mode_t
#define bustype_BDK_GSERX_SLICEX_QSGMII_MODE(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SLICEX_QSGMII_MODE(a,b) "GSERX_SLICEX_QSGMII_MODE"
#define device_bar_BDK_GSERX_SLICEX_QSGMII_MODE(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SLICEX_QSGMII_MODE(a,b) (a)
#define arguments_BDK_GSERX_SLICEX_QSGMII_MODE(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_slice#_rx_ldll_ctrl
 *
 * GSER Slice RX LDLL Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_slicex_rx_ldll_ctrl
{
    uint64_t u;
    struct bdk_gserx_slicex_rx_ldll_ctrl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_8_63         : 56;
        uint64_t pcs_sds_rx_ldll_tune  : 3;  /**< [  7:  5](R/W/H) Tuning bits for the regulator and loop filter.
                                                                 Bit 7 controls the initial value of the regulator output,
                                                                 0 for 0.9V and 1 for 0.925V.
                                                                 Bits 6:5 are connected to the loop filter, to reduce
                                                                 its corner frequency (for testing purposes).

                                                                 This parameter is for debugging purposes and should not
                                                                 be written in normal operation. */
        uint64_t pcs_sds_rx_ldll_swsel : 4;  /**< [  4:  1](R/W/H) DMON control, selects which signal is passed to the output
                                                                 of DMON:
                                                                 0x8 = vdda_int
                                                                 0x4 = pi clock (output of the PI)
                                                                 0x2 = dllout[1] (second output clock phase, out of 4 phases,
                                                                       of the Lane DLL)
                                                                 0x1 = dllout[0] (first output clock phase, out of 4 phases,
                                                                       of the Lane DLL).  Ensure that
                                                                       GSER()_SLICE_RX_SDLL_CTRL[PCS_SDS_RX_SDLL_SWSEL]=0x0 during
                                                                       this test.

                                                                 This parameter is for debugging purposes and should not
                                                                 be written in normal operation. */
        uint64_t reserved_0            : 1;
#else /* Word 0 - Little Endian */
        uint64_t reserved_0            : 1;
        uint64_t pcs_sds_rx_ldll_swsel : 4;  /**< [  4:  1](R/W/H) DMON control, selects which signal is passed to the output
                                                                 of DMON:
                                                                 0x8 = vdda_int
                                                                 0x4 = pi clock (output of the PI)
                                                                 0x2 = dllout[1] (second output clock phase, out of 4 phases,
                                                                       of the Lane DLL)
                                                                 0x1 = dllout[0] (first output clock phase, out of 4 phases,
                                                                       of the Lane DLL).  Ensure that
                                                                       GSER()_SLICE_RX_SDLL_CTRL[PCS_SDS_RX_SDLL_SWSEL]=0x0 during
                                                                       this test.

                                                                 This parameter is for debugging purposes and should not
                                                                 be written in normal operation. */
        uint64_t pcs_sds_rx_ldll_tune  : 3;  /**< [  7:  5](R/W/H) Tuning bits for the regulator and loop filter.
                                                                 Bit 7 controls the initial value of the regulator output,
                                                                 0 for 0.9V and 1 for 0.925V.
                                                                 Bits 6:5 are connected to the loop filter, to reduce
                                                                 its corner frequency (for testing purposes).

                                                                 This parameter is for debugging purposes and should not
                                                                 be written in normal operation. */
        uint64_t reserved_8_63         : 56;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_slicex_rx_ldll_ctrl_s cn; */
};
typedef union bdk_gserx_slicex_rx_ldll_ctrl bdk_gserx_slicex_rx_ldll_ctrl_t;

static inline uint64_t BDK_GSERX_SLICEX_RX_LDLL_CTRL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SLICEX_RX_LDLL_CTRL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090460218ll + 0x1000000ll * ((a) & 0x3) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=1)))
        return 0x87e090460218ll + 0x1000000ll * ((a) & 0x7) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=1)))
        return 0x87e090460218ll + 0x1000000ll * ((a) & 0xf) + 0x200000ll * ((b) & 0x1);
    __bdk_csr_fatal("GSERX_SLICEX_RX_LDLL_CTRL", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_SLICEX_RX_LDLL_CTRL(a,b) bdk_gserx_slicex_rx_ldll_ctrl_t
#define bustype_BDK_GSERX_SLICEX_RX_LDLL_CTRL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SLICEX_RX_LDLL_CTRL(a,b) "GSERX_SLICEX_RX_LDLL_CTRL"
#define device_bar_BDK_GSERX_SLICEX_RX_LDLL_CTRL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SLICEX_RX_LDLL_CTRL(a,b) (a)
#define arguments_BDK_GSERX_SLICEX_RX_LDLL_CTRL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_slice#_rx_sdll_ctrl
 *
 * GSER Slice RX SDLL Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_slicex_rx_sdll_ctrl
{
    uint64_t u;
    struct bdk_gserx_slicex_rx_sdll_ctrl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_16_63        : 48;
        uint64_t pcs_sds_oob_clk_ctrl  : 2;  /**< [ 15: 14](R/W/H) OOB clock oscillator output frequency selection:
                                                                 0x0 = 506 MHz (min) 682 MHz (typ) 782 MHz (max).
                                                                 0x1 = 439 MHz (min) 554 MHz (typ) 595 MHz (max).
                                                                 0x2 = 379 MHz (min) 453 MHz (typ) 482 MHz (max).
                                                                 0x3 = 303 MHz (min) 378 MHz (typ) 414 MHz (max).

                                                                 This parameter is for debugging purposes and should not
                                                                 be written in normal operation. */
        uint64_t reserved_7_13         : 7;
        uint64_t pcs_sds_rx_sdll_tune  : 3;  /**< [  6:  4](R/W) Tuning bits for the regulator and the loop filter. */
        uint64_t pcs_sds_rx_sdll_swsel : 4;  /**< [  3:  0](R/W) DMON control; selects which signal is passed to the output
                                                                 of DMON.
                                                                 0x1 = dllout[0] (first output clock phase, out of 8 phases,
                                                                 of the Slice DLL).
                                                                 0x2 = dllout[1] (second output clock phase, out of 8 phases,
                                                                 of the Slice DLL).
                                                                 0x4 = piclk (output clock of the PI).
                                                                 0x8 = vdda_int.

                                                                 All other values in this field are reserved. */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_rx_sdll_swsel : 4;  /**< [  3:  0](R/W) DMON control; selects which signal is passed to the output
                                                                 of DMON.
                                                                 0x1 = dllout[0] (first output clock phase, out of 8 phases,
                                                                 of the Slice DLL).
                                                                 0x2 = dllout[1] (second output clock phase, out of 8 phases,
                                                                 of the Slice DLL).
                                                                 0x4 = piclk (output clock of the PI).
                                                                 0x8 = vdda_int.

                                                                 All other values in this field are reserved. */
        uint64_t pcs_sds_rx_sdll_tune  : 3;  /**< [  6:  4](R/W) Tuning bits for the regulator and the loop filter. */
        uint64_t reserved_7_13         : 7;
        uint64_t pcs_sds_oob_clk_ctrl  : 2;  /**< [ 15: 14](R/W/H) OOB clock oscillator output frequency selection:
                                                                 0x0 = 506 MHz (min) 682 MHz (typ) 782 MHz (max).
                                                                 0x1 = 439 MHz (min) 554 MHz (typ) 595 MHz (max).
                                                                 0x2 = 379 MHz (min) 453 MHz (typ) 482 MHz (max).
                                                                 0x3 = 303 MHz (min) 378 MHz (typ) 414 MHz (max).

                                                                 This parameter is for debugging purposes and should not
                                                                 be written in normal operation. */
        uint64_t reserved_16_63        : 48;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_slicex_rx_sdll_ctrl_s cn; */
};
typedef union bdk_gserx_slicex_rx_sdll_ctrl bdk_gserx_slicex_rx_sdll_ctrl_t;

static inline uint64_t BDK_GSERX_SLICEX_RX_SDLL_CTRL(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SLICEX_RX_SDLL_CTRL(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090460220ll + 0x1000000ll * ((a) & 0x3) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=1)))
        return 0x87e090460220ll + 0x1000000ll * ((a) & 0x7) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=1)))
        return 0x87e090460220ll + 0x1000000ll * ((a) & 0xf) + 0x200000ll * ((b) & 0x1);
    __bdk_csr_fatal("GSERX_SLICEX_RX_SDLL_CTRL", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_SLICEX_RX_SDLL_CTRL(a,b) bdk_gserx_slicex_rx_sdll_ctrl_t
#define bustype_BDK_GSERX_SLICEX_RX_SDLL_CTRL(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SLICEX_RX_SDLL_CTRL(a,b) "GSERX_SLICEX_RX_SDLL_CTRL"
#define device_bar_BDK_GSERX_SLICEX_RX_SDLL_CTRL(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SLICEX_RX_SDLL_CTRL(a,b) (a)
#define arguments_BDK_GSERX_SLICEX_RX_SDLL_CTRL(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_slice#_sgmii_mode
 *
 * GSER Slice SGMII MODE Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_slicex_sgmii_mode
{
    uint64_t u;
    struct bdk_gserx_slicex_sgmii_mode_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_15_63        : 49;
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
#else /* Word 0 - Little Endian */
        uint64_t rx_sdll_bwsel         : 3;  /**< [  2:  0](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_ldll_bwsel         : 3;  /**< [  5:  3](R/W/H) Controls capacitors in delay line for different data rates; should be set
                                                                 based on the PLL clock frequency as follows:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x3 = 4 GHz.
                                                                 0x5 = 5.15625 GHz.
                                                                 0x6 = 5.65 GHz.
                                                                 0x7 = 6.25 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_pi_bwsel           : 3;  /**< [  8:  6](R/W/H) Controls PI different data rates:
                                                                 0x0 = 2.5 GHz.
                                                                 0x1 = 3.125 GHz.
                                                                 0x6 = 4 GHz.
                                                                 0x7 = 5.15625 GHz.

                                                                 All other values in this field are reserved. */
        uint64_t rx_sdll_isel          : 2;  /**< [ 10:  9](R/W/H) Controls charge pump current for slice DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t rx_ldll_isel          : 2;  /**< [ 12: 11](R/W/H) Controls charge pump current for lane DLL:
                                                                 0x0 = 500 uA.
                                                                 0x1 = 1000 uA.
                                                                 0x2 = 250 uA.
                                                                 0x3 = 330 uA. */
        uint64_t slice_spare_1_0       : 2;  /**< [ 14: 13](R/W/H) Controls enable of pcs_sds_rx_div33 for lane 0 and 1 in the slice:
                                                                 Bit 13 controls enable for lane 0.
                                                                 Bit 14 controls enable for lane 1. */
        uint64_t reserved_15_63        : 49;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_slicex_sgmii_mode_s cn; */
};
typedef union bdk_gserx_slicex_sgmii_mode bdk_gserx_slicex_sgmii_mode_t;

static inline uint64_t BDK_GSERX_SLICEX_SGMII_MODE(unsigned long a, unsigned long b) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SLICEX_SGMII_MODE(unsigned long a, unsigned long b)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && ((a<=3) && (b<=1)))
        return 0x87e090460258ll + 0x1000000ll * ((a) & 0x3) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && ((a<=6) && (b<=1)))
        return 0x87e090460258ll + 0x1000000ll * ((a) & 0x7) + 0x200000ll * ((b) & 0x1);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && ((a<=13) && (b<=1)))
        return 0x87e090460258ll + 0x1000000ll * ((a) & 0xf) + 0x200000ll * ((b) & 0x1);
    __bdk_csr_fatal("GSERX_SLICEX_SGMII_MODE", 2, a, b, 0, 0);
}

#define typedef_BDK_GSERX_SLICEX_SGMII_MODE(a,b) bdk_gserx_slicex_sgmii_mode_t
#define bustype_BDK_GSERX_SLICEX_SGMII_MODE(a,b) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SLICEX_SGMII_MODE(a,b) "GSERX_SLICEX_SGMII_MODE"
#define device_bar_BDK_GSERX_SLICEX_SGMII_MODE(a,b) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SLICEX_SGMII_MODE(a,b) (a)
#define arguments_BDK_GSERX_SLICEX_SGMII_MODE(a,b) (a),(b),-1,-1

/**
 * Register (RSL) gser#_slice_cfg
 *
 * GSER Slice Configuration Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_slice_cfg
{
    uint64_t u;
    struct bdk_gserx_slice_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_12_63        : 52;
        uint64_t tx_rx_detect_lvl_enc  : 4;  /**< [ 11:  8](R/W) Determines the RX detect level, pcs_sds_tx_rx_detect_lvl[9:0],
                                                                 (which is a 1-hot signal), where the level is equal to
                                                                 2^TX_RX_DETECT_LVL_ENC. */
        uint64_t reserved_6_7          : 2;
        uint64_t pcs_sds_rx_pcie_pterm : 2;  /**< [  5:  4](R/W) Reserved. */
        uint64_t pcs_sds_rx_pcie_nterm : 2;  /**< [  3:  2](R/W) Reserved. */
        uint64_t pcs_sds_tx_stress_eye : 2;  /**< [  1:  0](R/W) Controls TX stress eye. */
#else /* Word 0 - Little Endian */
        uint64_t pcs_sds_tx_stress_eye : 2;  /**< [  1:  0](R/W) Controls TX stress eye. */
        uint64_t pcs_sds_rx_pcie_nterm : 2;  /**< [  3:  2](R/W) Reserved. */
        uint64_t pcs_sds_rx_pcie_pterm : 2;  /**< [  5:  4](R/W) Reserved. */
        uint64_t reserved_6_7          : 2;
        uint64_t tx_rx_detect_lvl_enc  : 4;  /**< [ 11:  8](R/W) Determines the RX detect level, pcs_sds_tx_rx_detect_lvl[9:0],
                                                                 (which is a 1-hot signal), where the level is equal to
                                                                 2^TX_RX_DETECT_LVL_ENC. */
        uint64_t reserved_12_63        : 52;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_slice_cfg_s cn; */
};
typedef union bdk_gserx_slice_cfg bdk_gserx_slice_cfg_t;

static inline uint64_t BDK_GSERX_SLICE_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SLICE_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090460060ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090460060ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090460060ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_SLICE_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_SLICE_CFG(a) bdk_gserx_slice_cfg_t
#define bustype_BDK_GSERX_SLICE_CFG(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SLICE_CFG(a) "GSERX_SLICE_CFG"
#define device_bar_BDK_GSERX_SLICE_CFG(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SLICE_CFG(a) (a)
#define arguments_BDK_GSERX_SLICE_CFG(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_spd
 *
 * GSER Speed Bits Register
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_spd
{
    uint64_t u;
    struct bdk_gserx_spd_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t spd                   : 4;  /**< [  3:  0](R/W/H) For CCPI links (i.e. GSER8..13), the hardware loads this CSR field from the OCI_SPD\<3:0\>
                                                                 pins during chip cold reset. For non-CCPI links, this field is not used.
                                                                 For SPD settings that configure a non-default reference clock, hardware updates the PLL
                                                                 settings of the specific lane mode (LMODE) table entry to derive the correct link rate.

                                                                 \<pre\>
                                                                       REFCLK  Link Rate
                                                                 SPD   (MHz)   (Gb)      LMODE
                                                                 ----  ------  ------    -----------------------
                                                                 0x0:  100     1.25      R_125G_REFCLK15625_KX
                                                                 0x1:  100     2.5       R_25G_REFCLK100
                                                                 0x2:  100     5         R_5G_REFCLK100
                                                                 0x3:  100     8         R_8G_REFCLK100
                                                                 0x4:  125     1.25      R_125G_REFCLK15625_KX
                                                                 0x5:  125     2.5       R_25G_REFCLK125
                                                                 0x6:  125     3.125     R_3125G_REFCLK15625_XAUI
                                                                 0x7:  125     5         R_5G_REFCLK125
                                                                 0x8:  125     6.25      R_625G_REFCLK15625_RXAUI
                                                                 0x9:  125     8         R_8G_REFCLK125
                                                                 0xA:  156.25  2.5       R_25G_REFCLK100
                                                                 0xB:  156.25  3.125     R_3125G_REFCLK15625_XAUI
                                                                 0xC:  156.25  5         R_5G_REFCLK125
                                                                 0xD:  156.25  6.25      R_625G_REFCLK15625_RXAUI
                                                                 0xE:  156.25  10.3125   R_103125G_REFCLK15625_KR
                                                                 0xF:                    SW_MODE
                                                                 \</pre\>

                                                                 Note that a value of 0xF is called SW_MODE. The CCPI link does not come up configured in
                                                                 SW_MODE.
                                                                 (Software must do all the CCPI GSER configuration to use CCPI in the case of SW_MODE.)
                                                                 When SPD!=SW_MODE after a chip cold reset, the hardware has initialized the following
                                                                 registers (based on the OCI_SPD selection):

                                                                  * GSER()_LANE_MODE[LMODE]=Z.
                                                                  * GSER()_PLL_P()_MODE_0.
                                                                  * GSER()_PLL_P()_MODE_1.
                                                                  * GSER()_LANE_P()_MODE_0.
                                                                  * GSER()_LANE_P()_MODE_1.
                                                                  * GSER()_LANE()_RX_VALBBD_CTRL_0.
                                                                  * GSER()_LANE()_RX_VALBBD_CTRL_1.
                                                                  * GSER()_LANE()_RX_VALBBD_CTRL_2.

                                                                  where in "GSER(x)", x is 8..13, and in "P(z)", z equals LMODE. */
#else /* Word 0 - Little Endian */
        uint64_t spd                   : 4;  /**< [  3:  0](R/W/H) For CCPI links (i.e. GSER8..13), the hardware loads this CSR field from the OCI_SPD\<3:0\>
                                                                 pins during chip cold reset. For non-CCPI links, this field is not used.
                                                                 For SPD settings that configure a non-default reference clock, hardware updates the PLL
                                                                 settings of the specific lane mode (LMODE) table entry to derive the correct link rate.

                                                                 \<pre\>
                                                                       REFCLK  Link Rate
                                                                 SPD   (MHz)   (Gb)      LMODE
                                                                 ----  ------  ------    -----------------------
                                                                 0x0:  100     1.25      R_125G_REFCLK15625_KX
                                                                 0x1:  100     2.5       R_25G_REFCLK100
                                                                 0x2:  100     5         R_5G_REFCLK100
                                                                 0x3:  100     8         R_8G_REFCLK100
                                                                 0x4:  125     1.25      R_125G_REFCLK15625_KX
                                                                 0x5:  125     2.5       R_25G_REFCLK125
                                                                 0x6:  125     3.125     R_3125G_REFCLK15625_XAUI
                                                                 0x7:  125     5         R_5G_REFCLK125
                                                                 0x8:  125     6.25      R_625G_REFCLK15625_RXAUI
                                                                 0x9:  125     8         R_8G_REFCLK125
                                                                 0xA:  156.25  2.5       R_25G_REFCLK100
                                                                 0xB:  156.25  3.125     R_3125G_REFCLK15625_XAUI
                                                                 0xC:  156.25  5         R_5G_REFCLK125
                                                                 0xD:  156.25  6.25      R_625G_REFCLK15625_RXAUI
                                                                 0xE:  156.25  10.3125   R_103125G_REFCLK15625_KR
                                                                 0xF:                    SW_MODE
                                                                 \</pre\>

                                                                 Note that a value of 0xF is called SW_MODE. The CCPI link does not come up configured in
                                                                 SW_MODE.
                                                                 (Software must do all the CCPI GSER configuration to use CCPI in the case of SW_MODE.)
                                                                 When SPD!=SW_MODE after a chip cold reset, the hardware has initialized the following
                                                                 registers (based on the OCI_SPD selection):

                                                                  * GSER()_LANE_MODE[LMODE]=Z.
                                                                  * GSER()_PLL_P()_MODE_0.
                                                                  * GSER()_PLL_P()_MODE_1.
                                                                  * GSER()_LANE_P()_MODE_0.
                                                                  * GSER()_LANE_P()_MODE_1.
                                                                  * GSER()_LANE()_RX_VALBBD_CTRL_0.
                                                                  * GSER()_LANE()_RX_VALBBD_CTRL_1.
                                                                  * GSER()_LANE()_RX_VALBBD_CTRL_2.

                                                                  where in "GSER(x)", x is 8..13, and in "P(z)", z equals LMODE. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_spd_s cn88xxp1; */
    struct bdk_gserx_spd_cn81xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t spd                   : 4;  /**< [  3:  0](R/W/H) Not used. */
#else /* Word 0 - Little Endian */
        uint64_t spd                   : 4;  /**< [  3:  0](R/W/H) Not used. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn81xx;
    /* struct bdk_gserx_spd_cn81xx cn83xx; */
    struct bdk_gserx_spd_cn88xxp2
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t spd                   : 4;  /**< [  3:  0](R/W/H) For CCPI links (i.e. GSER8..13), the hardware loads this CSR field from the OCI_SPD\<3:0\>
                                                                 pins during chip cold reset. For non-CCPI links, this field is not used.
                                                                 For SPD settings that configure a non-default reference clock, hardware updates the PLL
                                                                 settings of the specific lane mode (LMODE) table entry to derive the correct link rate.

                                                                 \<pre\>
                                                                       REFCLK  Link Rate
                                                                 SPD   (MHz)   (Gb)     Train  LMODE
                                                                 ----  ------  ------   -----  -----------------------
                                                                 0x0:  100     5        TS     R_5G_REFCLK100
                                                                 0x1:  100     2.5      --     R_25G_REFCLK100
                                                                 0x2:  100     5        --     R_5G_REFCLK100
                                                                 0x3:  100     8        --     R_8G_REFCLK100
                                                                 0x4:  100     8        TS     R_8G_REFCLK100
                                                                 0x5:  100     8        KR     R_8G_REFCLK100
                                                                 0x6:  125     3.125    --     R_3125G_REFCLK15625_XAUI
                                                                 0x7:  125     5        --     R_5G_REFCLK125
                                                                 0x8:  125     6.25     --     R_625G_REFCLK15625_RXAUI
                                                                 0x9:  125     8        --     R_8G_REFCLK125
                                                                 0xA:  156.25  10.3125  TS     R_103125G_REFCLK15625_KR
                                                                 0xB:  156.25  3.125    --     R_3125G_REFCLK15625_XAUI
                                                                 0xC:  156.25  5        TS     R_5G_REFCLK125
                                                                 0xD:  156.25  6.25     TS     R_625G_REFCLK15625_RXAUI
                                                                 0xE:  156.25  10.3125  KR     R_103125G_REFCLK15625_KR
                                                                 0xF:                          SW_MODE
                                                                 \</pre\>

                                                                 Train column indicates training method.  TS indicates short training, i.e., local RX
                                                                 equalization only.  KR indicates KR training, i.e., local RX equalization and link
                                                                 partner TX equalizer adaptation.  -- indicates not applicable.
                                                                 Note that a value of 0xF is called SW_MODE. The CCPI link does not come up configured in
                                                                 SW_MODE.
                                                                 (Software must do all the CCPI GSER configuration to use CCPI in the case of SW_MODE.)
                                                                 When SPD!=SW_MODE after a chip cold reset, the hardware has initialized the following
                                                                 registers (based on the OCI_SPD selection):

                                                                  * GSER()_LANE_MODE[LMODE]=Z.
                                                                  * GSER()_PLL_P()_MODE_0.
                                                                  * GSER()_PLL_P()_MODE_1.
                                                                  * GSER()_LANE_P()_MODE_0.
                                                                  * GSER()_LANE_P()_MODE_1.
                                                                  * GSER()_LANE()_RX_VALBBD_CTRL_0.
                                                                  * GSER()_LANE()_RX_VALBBD_CTRL_1.
                                                                  * GSER()_LANE()_RX_VALBBD_CTRL_2.

                                                                  where in "GSER(x)", x is 8..13, and in "P(z)", z equals LMODE. */
#else /* Word 0 - Little Endian */
        uint64_t spd                   : 4;  /**< [  3:  0](R/W/H) For CCPI links (i.e. GSER8..13), the hardware loads this CSR field from the OCI_SPD\<3:0\>
                                                                 pins during chip cold reset. For non-CCPI links, this field is not used.
                                                                 For SPD settings that configure a non-default reference clock, hardware updates the PLL
                                                                 settings of the specific lane mode (LMODE) table entry to derive the correct link rate.

                                                                 \<pre\>
                                                                       REFCLK  Link Rate
                                                                 SPD   (MHz)   (Gb)     Train  LMODE
                                                                 ----  ------  ------   -----  -----------------------
                                                                 0x0:  100     5        TS     R_5G_REFCLK100
                                                                 0x1:  100     2.5      --     R_25G_REFCLK100
                                                                 0x2:  100     5        --     R_5G_REFCLK100
                                                                 0x3:  100     8        --     R_8G_REFCLK100
                                                                 0x4:  100     8        TS     R_8G_REFCLK100
                                                                 0x5:  100     8        KR     R_8G_REFCLK100
                                                                 0x6:  125     3.125    --     R_3125G_REFCLK15625_XAUI
                                                                 0x7:  125     5        --     R_5G_REFCLK125
                                                                 0x8:  125     6.25     --     R_625G_REFCLK15625_RXAUI
                                                                 0x9:  125     8        --     R_8G_REFCLK125
                                                                 0xA:  156.25  10.3125  TS     R_103125G_REFCLK15625_KR
                                                                 0xB:  156.25  3.125    --     R_3125G_REFCLK15625_XAUI
                                                                 0xC:  156.25  5        TS     R_5G_REFCLK125
                                                                 0xD:  156.25  6.25     TS     R_625G_REFCLK15625_RXAUI
                                                                 0xE:  156.25  10.3125  KR     R_103125G_REFCLK15625_KR
                                                                 0xF:                          SW_MODE
                                                                 \</pre\>

                                                                 Train column indicates training method.  TS indicates short training, i.e., local RX
                                                                 equalization only.  KR indicates KR training, i.e., local RX equalization and link
                                                                 partner TX equalizer adaptation.  -- indicates not applicable.
                                                                 Note that a value of 0xF is called SW_MODE. The CCPI link does not come up configured in
                                                                 SW_MODE.
                                                                 (Software must do all the CCPI GSER configuration to use CCPI in the case of SW_MODE.)
                                                                 When SPD!=SW_MODE after a chip cold reset, the hardware has initialized the following
                                                                 registers (based on the OCI_SPD selection):

                                                                  * GSER()_LANE_MODE[LMODE]=Z.
                                                                  * GSER()_PLL_P()_MODE_0.
                                                                  * GSER()_PLL_P()_MODE_1.
                                                                  * GSER()_LANE_P()_MODE_0.
                                                                  * GSER()_LANE_P()_MODE_1.
                                                                  * GSER()_LANE()_RX_VALBBD_CTRL_0.
                                                                  * GSER()_LANE()_RX_VALBBD_CTRL_1.
                                                                  * GSER()_LANE()_RX_VALBBD_CTRL_2.

                                                                  where in "GSER(x)", x is 8..13, and in "P(z)", z equals LMODE. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn88xxp2;
};
typedef union bdk_gserx_spd bdk_gserx_spd_t;

static inline uint64_t BDK_GSERX_SPD(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SPD(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000088ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000088ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000088ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_SPD", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_SPD(a) bdk_gserx_spd_t
#define bustype_BDK_GSERX_SPD(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SPD(a) "GSERX_SPD"
#define device_bar_BDK_GSERX_SPD(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SPD(a) (a)
#define arguments_BDK_GSERX_SPD(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_srst
 *
 * GSER Soft Reset Register
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_srst
{
    uint64_t u;
    struct bdk_gserx_srst_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_1_63         : 63;
        uint64_t srst                  : 1;  /**< [  0:  0](R/W) When asserted, resets all per-lane state in the GSER with the exception of the PHY and the
                                                                 GSER()_CFG. For diagnostic use only. */
#else /* Word 0 - Little Endian */
        uint64_t srst                  : 1;  /**< [  0:  0](R/W) When asserted, resets all per-lane state in the GSER with the exception of the PHY and the
                                                                 GSER()_CFG. For diagnostic use only. */
        uint64_t reserved_1_63         : 63;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_srst_s cn; */
};
typedef union bdk_gserx_srst bdk_gserx_srst_t;

static inline uint64_t BDK_GSERX_SRST(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_SRST(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000090ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000090ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000090ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_SRST", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_SRST(a) bdk_gserx_srst_t
#define bustype_BDK_GSERX_SRST(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_SRST(a) "GSERX_SRST"
#define device_bar_BDK_GSERX_SRST(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_SRST(a) (a)
#define arguments_BDK_GSERX_SRST(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_term_cfg
 *
 * GSER Termination Calibration Configuration Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_term_cfg
{
    uint64_t u;
    struct bdk_gserx_term_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_9_63         : 55;
        uint64_t fast_term_cal         : 1;  /**< [  8:  8](R/W/H) Set to enable fast termination calibration.
                                                                 For simulation use only. */
        uint64_t reserved_7            : 1;
        uint64_t cal_start_ovrd_en     : 1;  /**< [  6:  6](R/W/H) When set, calibration start is defined by
                                                                 GSER()_TERM_CFG[CAL_START_OVRD_VAL]. */
        uint64_t cal_start_ovrd_val    : 1;  /**< [  5:  5](R/W/H) Override calibration start value. */
        uint64_t cal_code_ovrd_en      : 1;  /**< [  4:  4](R/W/H) When set, calibration code is defined by
                                                                 GSER()_TERM_CFG[CAL_CODE_OVRD]. */
        uint64_t cal_code_ovrd         : 4;  /**< [  3:  0](R/W/H) Override calibration code value. */
#else /* Word 0 - Little Endian */
        uint64_t cal_code_ovrd         : 4;  /**< [  3:  0](R/W/H) Override calibration code value. */
        uint64_t cal_code_ovrd_en      : 1;  /**< [  4:  4](R/W/H) When set, calibration code is defined by
                                                                 GSER()_TERM_CFG[CAL_CODE_OVRD]. */
        uint64_t cal_start_ovrd_val    : 1;  /**< [  5:  5](R/W/H) Override calibration start value. */
        uint64_t cal_start_ovrd_en     : 1;  /**< [  6:  6](R/W/H) When set, calibration start is defined by
                                                                 GSER()_TERM_CFG[CAL_START_OVRD_VAL]. */
        uint64_t reserved_7            : 1;
        uint64_t fast_term_cal         : 1;  /**< [  8:  8](R/W/H) Set to enable fast termination calibration.
                                                                 For simulation use only. */
        uint64_t reserved_9_63         : 55;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_term_cfg_s cn; */
};
typedef union bdk_gserx_term_cfg bdk_gserx_term_cfg_t;

static inline uint64_t BDK_GSERX_TERM_CFG(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_TERM_CFG(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090460070ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090460070ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090460070ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_TERM_CFG", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_TERM_CFG(a) bdk_gserx_term_cfg_t
#define bustype_BDK_GSERX_TERM_CFG(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_TERM_CFG(a) "GSERX_TERM_CFG"
#define device_bar_BDK_GSERX_TERM_CFG(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_TERM_CFG(a) (a)
#define arguments_BDK_GSERX_TERM_CFG(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_term_mon_1
 *
 * GSER Termination Cal Code Monitor Register
 * These registers are for diagnostic use only.
 * These registers are reset by hardware only during chip cold reset.
 * The values of the CSR fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_term_mon_1
{
    uint64_t u;
    struct bdk_gserx_term_mon_1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t cal_code_mgmt         : 4;  /**< [  3:  0](RO/H) RX and TX termination resistance calibration code. */
#else /* Word 0 - Little Endian */
        uint64_t cal_code_mgmt         : 4;  /**< [  3:  0](RO/H) RX and TX termination resistance calibration code. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_term_mon_1_s cn; */
};
typedef union bdk_gserx_term_mon_1 bdk_gserx_term_mon_1_t;

static inline uint64_t BDK_GSERX_TERM_MON_1(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_TERM_MON_1(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090460110ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090460110ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090460110ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_TERM_MON_1", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_TERM_MON_1(a) bdk_gserx_term_mon_1_t
#define bustype_BDK_GSERX_TERM_MON_1(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_TERM_MON_1(a) "GSERX_TERM_MON_1"
#define device_bar_BDK_GSERX_TERM_MON_1(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_TERM_MON_1(a) (a)
#define arguments_BDK_GSERX_TERM_MON_1(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_tx_vboost
 *
 * GSER TX Voltage Boost Enable Register
 * These registers are reset by hardware only during chip cold reset. The values of the CSR
 * fields in these registers do not change during chip warm or soft resets.
 */
union bdk_gserx_tx_vboost
{
    uint64_t u;
    struct bdk_gserx_tx_vboost_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t vboost                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode, boosts the TX Vswing from
                                                                 VDD to 1.0 VPPD.
                                                                 \<3\>: Lane 3.  Reserved.
                                                                 \<2\>: Lane 2.  Reserved.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t vboost                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode, boosts the TX Vswing from
                                                                 VDD to 1.0 VPPD.
                                                                 \<3\>: Lane 3.  Reserved.
                                                                 \<2\>: Lane 2.  Reserved.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_tx_vboost_s cn81xx; */
    struct bdk_gserx_tx_vboost_cn88xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t vboost                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode (including all CCPI links), boosts the TX Vswing from
                                                                 VDD to 1.0 VPPD.
                                                                 \<3\>: Lane 3.
                                                                 \<2\>: Lane 2.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t vboost                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode (including all CCPI links), boosts the TX Vswing from
                                                                 VDD to 1.0 VPPD.
                                                                 \<3\>: Lane 3.
                                                                 \<2\>: Lane 2.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn88xx;
    struct bdk_gserx_tx_vboost_cn83xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_4_63         : 60;
        uint64_t vboost                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode, boosts the TX Vswing from
                                                                 VDD to 1.0 VPPD.
                                                                 \<3\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<2\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
#else /* Word 0 - Little Endian */
        uint64_t vboost                : 4;  /**< [  3:  0](R/W) For links that are not in PCIE mode, boosts the TX Vswing from
                                                                 VDD to 1.0 VPPD.
                                                                 \<3\>: Lane 3.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<2\>: Lane 2.  Not supported in GSER4, GSER5, or GSER6.
                                                                 \<1\>: Lane 1.
                                                                 \<0\>: Lane 0. */
        uint64_t reserved_4_63         : 60;
#endif /* Word 0 - End */
    } cn83xx;
};
typedef union bdk_gserx_tx_vboost bdk_gserx_tx_vboost_t;

static inline uint64_t BDK_GSERX_TX_VBOOST(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_TX_VBOOST(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000130ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000130ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000130ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_TX_VBOOST", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_TX_VBOOST(a) bdk_gserx_tx_vboost_t
#define bustype_BDK_GSERX_TX_VBOOST(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_TX_VBOOST(a) "GSERX_TX_VBOOST"
#define device_bar_BDK_GSERX_TX_VBOOST(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_TX_VBOOST(a) (a)
#define arguments_BDK_GSERX_TX_VBOOST(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_txclk_evt_cntr
 *
 * GSER QLM Transmit Clock Event Counter Register
 */
union bdk_gserx_txclk_evt_cntr
{
    uint64_t u;
    struct bdk_gserx_txclk_evt_cntr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_32_63        : 32;
        uint64_t count                 : 32; /**< [ 31:  0](RO) This register can only be reliably read when GSER()_TXCLK_EVT_CTRL[ENB]
                                                                 is clear.

                                                                 When GSER()_TXCLK_EVT_CTRL[CLR] is set, [COUNT] goes to zero.

                                                                 When GSER()_TXCLK_EVT_CTRL[ENB] is set, [COUNT] is incremented
                                                                 in positive edges of the QLM reference clock.

                                                                 When GSER()_TXCLK_EVT_CTRL[ENB] is not set, [COUNT] value is held;
                                                                 this must be used when [COUNT] is being read for reliable results. */
#else /* Word 0 - Little Endian */
        uint64_t count                 : 32; /**< [ 31:  0](RO) This register can only be reliably read when GSER()_TXCLK_EVT_CTRL[ENB]
                                                                 is clear.

                                                                 When GSER()_TXCLK_EVT_CTRL[CLR] is set, [COUNT] goes to zero.

                                                                 When GSER()_TXCLK_EVT_CTRL[ENB] is set, [COUNT] is incremented
                                                                 in positive edges of the QLM reference clock.

                                                                 When GSER()_TXCLK_EVT_CTRL[ENB] is not set, [COUNT] value is held;
                                                                 this must be used when [COUNT] is being read for reliable results. */
        uint64_t reserved_32_63        : 32;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_txclk_evt_cntr_s cn; */
};
typedef union bdk_gserx_txclk_evt_cntr bdk_gserx_txclk_evt_cntr_t;

static inline uint64_t BDK_GSERX_TXCLK_EVT_CNTR(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_TXCLK_EVT_CNTR(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000188ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000188ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000188ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_TXCLK_EVT_CNTR", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_TXCLK_EVT_CNTR(a) bdk_gserx_txclk_evt_cntr_t
#define bustype_BDK_GSERX_TXCLK_EVT_CNTR(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_TXCLK_EVT_CNTR(a) "GSERX_TXCLK_EVT_CNTR"
#define device_bar_BDK_GSERX_TXCLK_EVT_CNTR(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_TXCLK_EVT_CNTR(a) (a)
#define arguments_BDK_GSERX_TXCLK_EVT_CNTR(a) (a),-1,-1,-1

/**
 * Register (RSL) gser#_txclk_evt_ctrl
 *
 * GSER QLM Transmit Clock Event Counter Control Register
 */
union bdk_gserx_txclk_evt_ctrl
{
    uint64_t u;
    struct bdk_gserx_txclk_evt_ctrl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN /* Word 0 - Big Endian */
        uint64_t reserved_2_63         : 62;
        uint64_t clr                   : 1;  /**< [  1:  1](R/W) When set, clears GSER()_TXCLK_EVT_CNTR[COUNT]. */
        uint64_t enb                   : 1;  /**< [  0:  0](R/W) When set, enables the GSER()_TXCLK_EVT_CNTR[COUNT] to increment
                                                                 on positive edges of the QLM reference clock. */
#else /* Word 0 - Little Endian */
        uint64_t enb                   : 1;  /**< [  0:  0](R/W) When set, enables the GSER()_TXCLK_EVT_CNTR[COUNT] to increment
                                                                 on positive edges of the QLM reference clock. */
        uint64_t clr                   : 1;  /**< [  1:  1](R/W) When set, clears GSER()_TXCLK_EVT_CNTR[COUNT]. */
        uint64_t reserved_2_63         : 62;
#endif /* Word 0 - End */
    } s;
    /* struct bdk_gserx_txclk_evt_ctrl_s cn; */
};
typedef union bdk_gserx_txclk_evt_ctrl bdk_gserx_txclk_evt_ctrl_t;

static inline uint64_t BDK_GSERX_TXCLK_EVT_CTRL(unsigned long a) __attribute__ ((pure, always_inline));
static inline uint64_t BDK_GSERX_TXCLK_EVT_CTRL(unsigned long a)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) && (a<=3))
        return 0x87e090000180ll + 0x1000000ll * ((a) & 0x3);
    if (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (a<=6))
        return 0x87e090000180ll + 0x1000000ll * ((a) & 0x7);
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) && (a<=13))
        return 0x87e090000180ll + 0x1000000ll * ((a) & 0xf);
    __bdk_csr_fatal("GSERX_TXCLK_EVT_CTRL", 1, a, 0, 0, 0);
}

#define typedef_BDK_GSERX_TXCLK_EVT_CTRL(a) bdk_gserx_txclk_evt_ctrl_t
#define bustype_BDK_GSERX_TXCLK_EVT_CTRL(a) BDK_CSR_TYPE_RSL
#define basename_BDK_GSERX_TXCLK_EVT_CTRL(a) "GSERX_TXCLK_EVT_CTRL"
#define device_bar_BDK_GSERX_TXCLK_EVT_CTRL(a) 0x0 /* PF_BAR0 */
#define busnum_BDK_GSERX_TXCLK_EVT_CTRL(a) (a)
#define arguments_BDK_GSERX_TXCLK_EVT_CTRL(a) (a),-1,-1,-1

#endif /* __BDK_CSRS_GSER_H__ */
