/***********************license start***********************************
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
*
*   * Neither the name of Cavium Inc. nor the names of
*     its contributors may be used to endorse or promote products
*     derived from this software without specific prior written
*     permission.
*
* This Software, including technical data, may be subject to U.S. export
* control laws, including the U.S. Export Administration Act and its
* associated regulations, and may be subject to export or import
* regulations in other countries.
*
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND CAVIUM INC. MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
* TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
* REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
* DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
* PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT,
* QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE  RISK
* ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
***********************license end**************************************/
#include <bdk.h>
#include "libbdk-arch/bdk-csrs-gser.h"
#include "libbdk-arch/bdk-csrs-rst.h"
#include "libbdk-hal/qlm/bdk-qlm-errata-cn8xxx.h"

/**
 * Delay for the specified microseconds. When this code runs on secondary nodes
 * before full init, the normal bdk-clock functions do not work. This function
 * serves as a replacement that runs everywhere.
 *
 * @param usec   Microseconds to wait
 */
static void wait_usec(bdk_node_t node, uint64_t usec)
{
    const uint64_t REF_CLOCK = 50000000; /* This is currently defined to be 50Mhz */
    uint64_t refclock = BDK_CSR_READ(node, BDK_RST_REF_CNTR);
    uint64_t timeout = refclock + REF_CLOCK * usec / 1000000;
    while (refclock < timeout)
    {
        refclock = BDK_CSR_READ(node, BDK_RST_REF_CNTR);
    }
}

/**
 * Errata GSER-25992 - RX EQ Default Settings Update<p>
 * For all GSER and all lanes when not PCIe EP:
 *     set GSER()_LANE()_RX_CFG_4[CFG_RX_ERRDET_CTRL<13:8>] = 13 (decimal)
 *     set GSER()_LANE()_RX_CTLE_CTRL[PCS_SDS_RX_CTLE_BIAS_CTRL] = 3
 * Applied when SERDES are configured for 8G and 10G.<p>
 * Applies to:
 *     CN88XX pass 1.x
 * Fixed in hardware:
 *     CN88XX pass 2.x
 *     CN81XX
 *     CN83XX
 *
 * @param node     Node to apply errata fix for
 * @param qlm      QLM to apply errata fix to
 * @param baud_mhz QLM speed in Mhz
 *
 * @return Zero on success, negative on failure
 */
int __bdk_qlm_errata_gser_25992(bdk_node_t node, int qlm, int baud_mhz)
{
    if (!CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X))
        return 0;
    if (baud_mhz < 8000)
        return 0;

    int num_lanes = 4; /* Only applies to CN88XX, where always 4 lanes */
    for (int lane = 0; lane < num_lanes; lane++)
    {
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_RX_CTLE_CTRL(qlm, lane),
            c.s.pcs_sds_rx_ctle_bias_ctrl = 3);
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_RX_CFG_4(qlm, lane),
            c.s.cfg_rx_errdet_ctrl = 0xcd6f);
    }
    return 0;
}

/**
 * (GSER-26150) 10G PHY PLL Temperature Failure
 *
 * 10 Gb temperature excursions can cause lock failure. Change
 * the calibration point of the VCO at start up to shift some
 * available range of the VCO from -deltaT direction to the
 * +deltaT ramp direction allowing a greater range of VCO
 * temperatures before experiencing the failure.
 *
 * Applies to:
 *     CN88XX pass 1.x
 * Fix in hardware:
 *     CN88XX pass 2.x
 *     CN81XX
 *     CN83XX
 *
 * Only applies to QLMs running 8G and 10G
 *
 * @param node   Node to apply errata to
 * @param qlm    QLM to apply errata fix to
 *
 * @return Zero on success, negative on failure
 */
int __bdk_qlm_errata_gser_26150(bdk_node_t node, int qlm, int baud_mhz)
{
    if (!CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X))
        return 0;
    if (baud_mhz < 8000)
        return 0;

    int num_lanes = 4; /* Only applies to CN88XX, where always 4 lanes */

    BDK_CSR_INIT(gserx_cfg, node, BDK_GSERX_CFG(qlm));
    if (gserx_cfg.s.pcie)
    {
        /* Update PLL parameters */
        /* Step 1: Set GSER()_GLBL_PLL_CFG_3[PLL_VCTRL_SEL_LCVCO_VAL] = 0x2, and
           GSER()_GLBL_PLL_CFG_3[PCS_SDS_PLL_VCO_AMP] = 0 */
        BDK_CSR_MODIFY(c, node, BDK_GSERX_GLBL_PLL_CFG_3(qlm),
            c.s.pll_vctrl_sel_lcvco_val = 0x2;
            c.s.pcs_sds_pll_vco_amp = 0);
        /* Step 2: Set GSER()_GLBL_MISC_CONFIG_1[PCS_SDS_TRIM_CHP_REG] = 0x2. */
        BDK_CSR_MODIFY(c, node, BDK_GSERX_GLBL_MISC_CONFIG_1(qlm),
            c.s.pcs_sds_trim_chp_reg = 0x2);
        return 0;
    }

    /* Applying this errata twice causes problems */
    BDK_CSR_INIT(pll_cfg_3, node, BDK_GSERX_GLBL_PLL_CFG_3(qlm));
    if (pll_cfg_3.s.pll_vctrl_sel_lcvco_val == 0x2)
        return 0;

    /* Put PHY in P2 Power-down state  Need to Power down all lanes in a
       QLM/DLM to force PHY to P2 state */
    for (int i=0; i<num_lanes; i++)
    {
        /* Step 1: Set GSER()_LANE(lane_n)_PCS_CTLIFC_0[CFG_TX_PSTATE_REQ_OVERRD_VAL] = 0x3
           Select P2 power state for Tx lane */
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PCS_CTLIFC_0(qlm, i),
            c.s.cfg_tx_pstate_req_ovrrd_val = 0x3);
        /* Step 2: Set GSER()_LANE(lane_n)_PCS_CTLIFC_1[CFG_RX_PSTATE_REQ_OVERRD_VAL] = 0x3
           Select P2 power state for Rx lane */
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PCS_CTLIFC_1(qlm, i),
            c.s.cfg_rx_pstate_req_ovrrd_val = 0x3);
        /* Step 3: Set GSER()_LANE(lane_n)_PCS_CTLIFC_2[CFG_TX_PSTATE_REQ_OVRRD_EN] = 1
           Enable Tx power state override and Set
           GSER()_LANE(lane_n)_PCS_CTLIFC_2[CFG_RX_PSTATE_REQ_OVRRD_EN] = 1
           Enable Rx power state override */
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PCS_CTLIFC_2(qlm, i),
            c.s.cfg_tx_pstate_req_ovrrd_en = 0x1;
            c.s.cfg_rx_pstate_req_ovrrd_en = 0X1);
        /* Step 4: Set GSER()_LANE(lane_n)_PCS_CTLIFC_2[CTLIFC_OVRRD_REQ] = 1
           Start the CTLIFC override state machine */
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PCS_CTLIFC_2(qlm, i),
            c.s.ctlifc_ovrrd_req = 0x1);
    }

    /* Update PLL parameters */
    /* Step 5: Set GSER()_GLBL_PLL_CFG_3[PLL_VCTRL_SEL_LCVCO_VAL] = 0x2, and
       GSER()_GLBL_PLL_CFG_3[PCS_SDS_PLL_VCO_AMP] = 0 */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_GLBL_PLL_CFG_3(qlm),
        c.s.pll_vctrl_sel_lcvco_val = 0x2;
        c.s.pcs_sds_pll_vco_amp = 0);
    /* Step 6: Set GSER()_GLBL_MISC_CONFIG_1[PCS_SDS_TRIM_CHP_REG] = 0x2. */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_GLBL_MISC_CONFIG_1(qlm),
        c.s.pcs_sds_trim_chp_reg = 0x2);
    /* Wake up PHY and transition to P0 Power-up state to bring-up the lanes,
       need to wake up all PHY lanes */
    for (int i=0; i<num_lanes; i++)
    {
        /* Step 7: Set GSER()_LANE(lane_n)_PCS_CTLIFC_0[CFG_TX_PSTATE_REQ_OVERRD_VAL] = 0x0
           Select P0 power state for Tx lane */
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PCS_CTLIFC_0(qlm, i),
            c.s.cfg_tx_pstate_req_ovrrd_val = 0x0);
        /* Step 8: Set GSER()_LANE(lane_n)_PCS_CTLIFC_1[CFG_RX_PSTATE_REQ_OVERRD_VAL] = 0x0
           Select P0 power state for Rx lane */
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PCS_CTLIFC_1(qlm, i),
            c.s.cfg_rx_pstate_req_ovrrd_val = 0x0);
        /* Step 9: Set GSER()_LANE(lane_n)_PCS_CTLIFC_2[CFG_TX_PSTATE_REQ_OVRRD_EN] = 1
           Enable Tx power state override and Set
           GSER()_LANE(lane_n)_PCS_CTLIFC_2[CFG_RX_PSTATE_REQ_OVRRD_EN] = 1
           Enable Rx power state override */
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PCS_CTLIFC_2(qlm, i),
            c.s.cfg_tx_pstate_req_ovrrd_en = 0x1;
            c.s.cfg_rx_pstate_req_ovrrd_en = 0X1);
        /* Step 10: Set GSER()_LANE(lane_n)_PCS_CTLIFC_2[CTLIFC_OVRRD_REQ] = 1
           Start the CTLIFC override state machine */
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PCS_CTLIFC_2(qlm, i),
            c.s.ctlifc_ovrrd_req = 0x1);
    }

    /* Step 11:  Wait 10 msec */
    wait_usec(node, 10000);

    /* Release Lane Tx/Rx Power state override enables. */
    for (int i=0; i<num_lanes; i++)
    {
        /* Step 12: Set GSER()_LANE(lane_n)_PCS_CTLIFC_2[CFG_TX_PSTATE_REQ_OVRRD_EN] = 0
           Disable Tx power state override and Set
           GSER()_LANE(lane_n)_PCS_CTLIFC_2[CFG_RX_PSTATE_REQ_OVRRD_EN] = 0
           Disable Rx power state override */
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PCS_CTLIFC_2(qlm, i),
            c.s.cfg_tx_pstate_req_ovrrd_en = 0x0;
            c.s.cfg_rx_pstate_req_ovrrd_en = 0X0);
    }
    /* Step 13:  Poll GSER()_PLL_STAT.[PLL_LOCK] = 1
       Poll and check that PLL is locked */
    if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_GSERX_PLL_STAT(qlm), pll_lock, ==, 1, 10000))
    {
        bdk_error("QLM%d: Timeout waiting for GSERX_PLL_STAT[pll_lock]\n", qlm);
        return -1;
    }

    /* Step 14:  Poll GSER()_QLM_STAT.[RST_RDY] = 1
       Poll and check that QLM/DLM is Ready */
    if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_GSERX_QLM_STAT(qlm), rst_rdy, ==, 1, 10000))
    {
        bdk_error("QLM%d: Timeout waiting for GSERX_QLM_STAT[rst_rdy]\n", qlm);
        return -1;
    }
    return 0;
}

/**
 * Errata (GSER-26636) 10G-KR/40G-KR - Inverted Tx Coefficient Direction Change
 * Applied to all 10G standards (required for KR) but also applied to other
 * standards in case software training is used.
 * Applies to:
 *     CN88XX pass 1.x
 * Fixed in hardware:
 *     CN88XX pass 2.x
 *     CN81XX
 *     CN83XX
 *
 * @param node     Node to apply errata fix for
 * @param qlm      QLM to apply errata fix to
 * @param baud_mhz QLM speed in Mhz
 *
 * @return Zero on success, negative on failure
 */
int __bdk_qlm_errata_gser_26636(bdk_node_t node, int qlm, int baud_mhz)
{
    if (!CAVIUM_IS_MODEL(CAVIUM_CN88XX_PASS1_X))
        return 0;

    BDK_CSR_MODIFY(c, node, BDK_GSERX_RX_TXDIR_CTRL_1(qlm),
        c.s.rx_precorr_chg_dir = 1;
        c.s.rx_tap1_chg_dir = 1);
    return 0;
}

/**
 * (GSER-27140) SERDES has temperature drift sensitivity in the RX EQ<p>
 * SERDES temperature drift sensitivity in receiver. Issues have
 * been found with the Bit Error Rate (BER) reliability of
 * 10GBASE-KR links over the commercial temperature range (0 to 100C),
 * especially when subjected to rapid thermal ramp stress testing.
 * (See HRM for corresponding case temperature requirements for each speed grade.)<p>
 * Applies to:
 *     CN88XX pass 1.x
 *     CN88XX pass 2.x
 *     CN83XX pass 1.x
 *     CN81XX pass 1.x
 * Fixed in hardware:
 *     TBD<p>
 * Only applies to QLMs running 10G
 *
 * @param node     Note to apply errata fix to
 * @param qlm      QLM to apply errata fix to
 * @param baud_mhz QLM baud rate in Mhz
 * @param channel_loss
 *                 Insertion loss at Nyquist rate (e.g. 5.125Ghz for XFI/XLAUI) in dB
 *
 * @return Zero on success, negative on failure
 */
int __bdk_qlm_errata_gser_27140(bdk_node_t node, int qlm, int baud_mhz, int channel_loss)
{
    if (baud_mhz != 10312)
        return 0;

    /* A channel loss of -1 means the loss is unknown. A short channel is
       considered to have loss between 0 and 10 dB */
    bool short_channel = (channel_loss >= 0) && (channel_loss <= 10);

    /* I. For each GSER QLM: */
    /* Workaround GSER-27140: */
    /* (1) GSER-26150 = Applied by the caller */
    /* (2) Write GSER()_LANE_VMA_FINE_CTRL_0[RX_SDLL_IQ_MAX_FINE] = 0xE */
    /* (3) Write GSER()_LANE_VMA_FINE_CTRL_0[RX_SDLL_IQ_MIN_FINE] = 0x8 */
    /* (4) Write GSER()_LANE_VMA_FINE_CTRL_0[RX_SDLL_IQ_STEP_FINE] = 0x2 */
    /* (5) Write GSER()_LANE_VMA_FINE_CTRL_0[VMA_WINDOW_WAIT_FINE] = 0x5 */
    /* (6) Write GSER()_LANE_VMA_FINE_CTRL_0[LMS_WAIT_TIME_FINE] = 0x5 */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_LANE_VMA_FINE_CTRL_0(qlm),
        c.s.rx_sdll_iq_max_fine = 0xE;
        c.s.rx_sdll_iq_min_fine = 0x8;
        c.s.rx_sdll_iq_step_fine = 0x2;
        c.s.vma_window_wait_fine = 0x5;
        c.s.lms_wait_time_fine = 0x5);
    /* (7) Write GSER()_LANE_VMA_FINE_CTRL_2[RX_PRECTLE_GAIN_MAX_FINE] = 0xB */
    /* (8) Write GSER()_LANE_VMA_FINE_CTRL_2[RX_PRECTLE_GAIN_MIN_FINE] = 0x6(long) or 0x0(short) */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_LANE_VMA_FINE_CTRL_2(qlm),
        c.s.rx_prectle_gain_max_fine = 0xB;
        c.s.rx_prectle_gain_min_fine = short_channel ? 0x0 : 0x6);
    /* (9) Write GSER()_RX_TXDIR_CTRL_0[RX_BOOST_LO_THRES] = 0x4 */
    /* (10) Write GSER()_RX_TXDIR_CTRL_0[RX_BOOST_HI_THRES] = 0xB */
    /* (11) Write GSER()_RX_TXDIR_CTRL_0[RX_BOOST_HI_VAL] = 0xF */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_RX_TXDIR_CTRL_0(qlm),
        c.s.rx_boost_lo_thrs = 0x4;
        c.s.rx_boost_hi_thrs = 0xB;
        c.s.rx_boost_hi_val = 0xF);
    /* (12) Write GSER()_RX_TXDIR_CTRL_1[RX_TAP1_LO_THRS] = 0x8 */
    /* (13) Write GSER()_RX_TXDIR_CTRL_1[RX_TAP1_HI_THRS] = 0x17 */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_RX_TXDIR_CTRL_1(qlm),
        c.s.rx_tap1_lo_thrs = 0x8;
        c.s.rx_tap1_hi_thrs = 0x17);

    /* (14) Write GSER()_EQ_WAIT_TIME[RXEQ_WAIT_CNT] = 0x6 */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_EQ_WAIT_TIME(qlm),
        c.s.rxeq_wait_cnt = 0x6);
    /* (15) Write GSER()_RX_TXDIR_CTRL_2[RX_PRECORR_HI_THRS] = 0xC0 */
    /* (16) Write GSER()_RX_TXDIR_CTRL_2[RX_PRECORR_LO_THRS] = 0x40 */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_RX_TXDIR_CTRL_2(qlm),
        c.s.rx_precorr_hi_thrs = 0xc0;
        c.s.rx_precorr_lo_thrs = 0x40);

    /* We can't call the normal bdk-qlm function as it uses pointers that
       don't work when running in secondary nodes before CCPI is up */
    int num_lanes = 4;
    if (CAVIUM_IS_MODEL(CAVIUM_CN81XX) || (CAVIUM_IS_MODEL(CAVIUM_CN83XX) && (qlm >= 4)))
        num_lanes = 2;

    /* II. For each GSER QLM SerDes lane: */
    /* Establish typical values, which are already reset values in pass 2: */
    for (int lane = 0; lane < num_lanes; lane++)
    {
        /* (17) For each GSER lane in the 10GBASE-KR link: */
        /*    (a) Write GSER()_LANE()_RX_VALBBD_CTRL_0[AGC_GAIN] = 0x3 */
        /*    (b) Write GSER()_LANE()_RX_VALBBD_CTRL_0[DFE_GAIN] = 0x2 */
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_RX_VALBBD_CTRL_0(qlm, lane),
            c.s.agc_gain = 0x3;
            c.s.dfe_gain = 0x2);
    }

    /* III. The GSER QLM SerDes Lanes are now ready. */
    return 0;
}

/**
 * Errata GSER-27882 -GSER 10GBASE-KR Transmit Equalizer
 * Training may not update PHY Tx Taps. This function is not static
 * so we can share it with BGX KR
 * Applies to:
 *     CN88XX pass 1.x, 2.0, 2.1
 * Fixed in hardware:
 *     CN88XX pass 2.2 and higher
 *     CN81XX
 *     CN83XX
 *
 * @param node   Node to apply errata fix for
 * @param qlm    QLM to apply errata fix to
 * @param lane
 *
 * @return Zero on success, negative on failure
 */
int __bdk_qlm_errata_gser_27882(bdk_node_t node, int qlm, int lane)
{
    /* Toggle Toggle Tx Coeff Req override to force an update */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PCS_CTLIFC_0(qlm, lane),
        c.s.cfg_tx_coeff_req_ovrrd_val = 1);
    BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PCS_CTLIFC_2(qlm, lane),
        c.s.cfg_tx_coeff_req_ovrrd_en = 1);
    BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PCS_CTLIFC_2(qlm, lane),
        c.s.ctlifc_ovrrd_req = 1);
    BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PCS_CTLIFC_2(qlm, lane),
        c.s.cfg_tx_coeff_req_ovrrd_en = 0);
    BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PCS_CTLIFC_2(qlm, lane),
        c.s.ctlifc_ovrrd_req = 1);
    return 0;
}

