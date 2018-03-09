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
#include "libbdk-hal/if/bdk-if.h"
#include "libbdk-hal/bdk-qlm.h"
#include "libbdk-hal/qlm/bdk-qlm-common.h"
#include "libbdk-arch/bdk-csrs-gser.h"
#include "libbdk-arch/bdk-csrs-sata.h"

/**
 * Initialize a DLM/QLM for use with SATA controllers
 *
 * @param node       Node to intialize
 * @param qlm        Which DLM/QLM to init
 * @param baud_mhz   QLM speed in Gbaud
 * @param sata_first First SATA controller connected to this DLM/QLM
 * @param sata_last  Last SATA controller connected to this DLM/QLM (inclusive)
 *
 * @return Zero on success, negative on failure
 */
int __bdk_qlm_set_sata_cn8xxx(bdk_node_t node, int qlm, int baud_mhz, int sata_first, int sata_last)
{
    const int NUM_LANES = sata_last - sata_first + 1;
    const int MAX_A_CLK = 333000000; /* Max of 333Mhz */

    /* 26.4.1 Cold Reset */
    /* 1.  Ensure that the SerDes reference clock is up and stable. */
    /* Already done */

    /* 2.  Optionally program the GPIO CSRs for SATA features.
        a.  For cold-presence detect, select a GPIO for the input and program GPI-
            O_BIT_CFG(0..50)[PIN_SEL] = GPIO_PIN_SEL_E::SATA(0..15)_CP_DET.
        b. For mechanical-presence detect, select a GPIO for the input and program
            GPIO_BIT_CFG(0..50)[PIN_SEL] = GPI-
            O_PIN_SEL_E::SATA(0..15)_MP_SWITCH.
        c. For BIST board-test loopback, select a GPIO for the input and program GPI-
            O_BIT_CFG(0..50)[PIN_SEL] = GPIO_PIN_SEL_E:::SATA_LAB_LB.
        d. For LED activity, select a GPIO for the output and program GPI-
            O_BIT_CFG(0..50)[PIN_SEL] = GPIO_PIN_SEL_E:::SATA(0..15)_ACT_LED.
        e. For cold-presence power-on-device, select a GPIO for the output and program
            GPIO_BIT_CFG(0..50)[PIN_SEL] = GPIO_PIN_SEL_E:::SATA(0..15)_CP_-
            POD. */
    /* Skipping */

    /* 3.  Optionally program the SGPIO unit. */
    /* Skipping */

    /* 4.  Assert all resets:
        a.  UAHC reset: SATA(0..15)_UCTL_CTL[SATA_UAHC_RST] = 1
        b. UCTL reset: SATA(0..15)_UCTL_CTL[SATA_UCTL_RST] = 1 */
    for (int p = sata_first; p <= sata_last; p++)
    {
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.sata_uahc_rst = 1;
            c.s.sata_uctl_rst = 1);
    }

    /* 5.  Configure the ACLK:
        a.  Reset the clock dividers: SATA(0..15)_UCTL_CTL[A_CLKDIV_RST] = 1.
        b. Select the ACLK frequency (refer to maximum values in Table 26 1).
            i. SATA(0..15)_UCTL_CTL[A_CLKDIV_SEL] = desired value,
            ii. SATA(0..15)_UCTL_CTL[A_CLK_EN] = 1 to enable the ACLK.
        c.  Deassert the ACLK clock divider reset:
            SATA(0..15)_UCTL_CTL[A_CLKDIV_RST] = 0. */
    int divisor = (bdk_clock_get_rate(node, BDK_CLOCK_SCLK) + MAX_A_CLK - 1) / MAX_A_CLK;
    int a_clkdiv;
    /* This screwy if logic is from the description of
       SATAX_UCTL_CTL[a_clkdiv_sel] in the CSR */
    if (divisor <= 4)
    {
        a_clkdiv = divisor - 1;
        /* Divisor matches calculated value */
    }
    else if (divisor <= 6)
    {
        a_clkdiv = 4;
        divisor = 6;
    }
    else if (divisor <= 8)
    {
        a_clkdiv = 5;
        divisor = 8;
    }
    else if (divisor <= 16)
    {
        a_clkdiv = 6;
        divisor = 16;
    }
    else if (divisor <= 24)
    {
        a_clkdiv = 7;
        divisor = 24;
    }
    else
    {
        bdk_error("Unable to determine SATA clock divisor\n");
        return -1;
    }
    /* Calculate the final clock rate */
    int a_clk = bdk_clock_get_rate(node, BDK_CLOCK_SCLK) / divisor;

    for (int p = sata_first; p <= sata_last; p++)
    {
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.a_clkdiv_rst = 1);
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.a_clk_byp_sel = 0;
            c.s.a_clkdiv_sel = a_clkdiv;
            c.s.a_clk_en = 1);
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.a_clkdiv_rst = 0);
    }
    bdk_wait_usec(1);

    /* 8.  Configure PHY for SATA. Refer to Section 21.1.2. */
    /* Done below, section 24.1.2.3 */

    /* 9.  TBD: Poll QLM2_MPLL_STATUS for MPLL lock */
    /* Not needed */

    /* 10. Initialize UAHC as described in the AHCI specification
        (UAHC_* registers). */
    /* Done when a SATA driver is initialized */

    /* 24.1.2.3 SATA Configuration
    Software must perform the following steps to configure the GSER_WEST
    for a SATA application. Note that the GSERW steps are on a QLM basis. */

    /* 1. Configure the SATA controller (refer to Chapter 26). */
    /* This is the code above */

    /* 2. Configure the QLM Reference clock.
        Set GSER(0..13)_REFCLK_SEL[COM_CLK_SEL] = 1 to source the reference
            clock from the external clock multiplexer.
        Configure GSER(0..13)_REFCLK_SEL[USE_COM1]:
            0 = use QLMC_REF_CLK0_P/N
            1 = use QLMC_REF_CLK1_P/N */
    /* Already done */

    /* Make sure the PHY is in reset before we reconfig */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_PHY_CTL(qlm),
        c.s.phy_reset = 1);
    bdk_wait_usec(1);

    /* 3. Configure the QLM for SATA mode: set GSER(0..13)_CFG[SATA] = 1. */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_CFG(qlm),
        c.u = 0;
        c.s.sata = 1);

   /* 9. Clear the appropriate lane resets:
       GSER(0..13)_SATA_LANE_RST[Ln_RST] = 0, where n is the lane number 0-3. */
    BDK_CSR_WRITE(node, BDK_GSERX_SATA_LANE_RST(qlm), 0);
    BDK_CSR_READ(node, BDK_GSERX_SATA_LANE_RST(qlm));

    /* We'll check for the SATA_PCS Ready in step 8a below */
    /* Short 1 usec wait */
    bdk_wait_usec(1);

    /* 4. Take the PHY out of reset: write GSER(0..13)_PHY_CTL[PHY_RESET] = 0. */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_PHY_CTL(qlm),
        c.s.phy_reset = 0);

    /* 4a. Poll for PHY RST_RDY indicating the PHY has initialized before
           trying to access internal registers to reconfigure for SATA */
    /* 8. Wait for GSER(0..13)_QLM_STAT[RST_RDY] = 1, indicating that the PHY
       has been reconfigured and PLLs are locked. */
    if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_GSERX_QLM_STAT(qlm), rst_rdy, ==, 1, 10000))
    {
        bdk_error("QLM%d: Timeout waiting for GSERX_QLM_STAT[rst_rdy]\n", qlm);
        return -1;
    }

    /* Workaround for errata GSER-30310: SATA HDD Not Ready due to
       PHY SDLL/LDLL lockup at 3GHz */
    for (int slice = 0; slice < NUM_LANES / 2; slice++)
    {
        BDK_CSR_MODIFY(c, node, BDK_GSERX_SLICEX_PCIE1_MODE(qlm, slice),
            c.s.rx_pi_bwsel = 1;
            c.s.rx_ldll_bwsel = 1;
            c.s.rx_sdll_bwsel = 1);
        BDK_CSR_MODIFY(c, node, BDK_GSERX_SLICEX_PCIE2_MODE(qlm, slice),
            c.s.rx_pi_bwsel = 1;
            c.s.rx_ldll_bwsel = 1;
            c.s.rx_sdll_bwsel = 1);
        BDK_CSR_MODIFY(c, node, BDK_GSERX_SLICEX_PCIE3_MODE(qlm, slice),
            c.s.rx_pi_bwsel = 1;
            c.s.rx_ldll_bwsel = 1;
            c.s.rx_sdll_bwsel = 1);
    }

    /* 5. Change the P2 termination
       GSERn_RX_PWR_CTRL_P2[P2_RX_SUBBLK_PD<0>] = 0 (termination) */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_RX_PWR_CTRL_P2(qlm),
        c.s.p2_rx_subblk_pd &= 0x1e);

    /* 6. Modify the electrical IDLE detect on delay: set
       GSER(0..13)_LANE(0..3)_MISC_CFG_0[EIE_DET_STL_ON_TIME] = 0x4 */
    for (int lane = 0; lane < NUM_LANES; lane++)
    {
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_MISC_CFG_0(qlm, lane),
            c.s.eie_det_stl_on_time = 4);
    }

    /* 7. Modify the PLL and lane-protocol-mode registers to configure the
       PHY for SATA */
    /* Errata (GSER-26724) SATA never indicates GSER QLM_STAT[RST_RDY]
       We program PLL_PX_MODE_0 last due to this errata */
    for (int p=0; p<3; p++)
    {
        BDK_CSR_MODIFY(c, node, BDK_GSERX_PLL_PX_MODE_1(qlm, p),
            c.s.pll_16p5en = 0x0;
            c.s.pll_cpadj = 0x2;
            c.s.pll_pcie3en = 0;
            c.s.pll_opr = 0x0;
            c.s.pll_div = 0x1e);
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANE_PX_MODE_0(qlm, p),
            c.s.ctle = 0x0;
            c.s.pcie = 0;
            c.s.tx_ldiv = 0x0;
            c.s.rx_ldiv = 2 - p;
            c.s.srate = 0;
            c.s.tx_mode = 3;
            c.s.rx_mode = 3);
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANE_PX_MODE_1(qlm, p),
            c.s.vma_fine_cfg_sel = 0;
            c.s.vma_mm = 1;
            c.s.cdr_fgain = 0xa;
            c.s.ph_acc_adj = 0x15);
    }
    for (int p=0; p<3; p++)
    {
        BDK_CSR_MODIFY(c, node, BDK_GSERX_PLL_PX_MODE_0(qlm, p),
            c.s.pll_icp = 0x1;
            c.s.pll_rloop = 0x3;
            c.s.pll_pcs_div = 0x5);
    }

    for (int s = 0; s < NUM_LANES / 2; s++)
    {
        BDK_CSR_MODIFY(c, node, BDK_GSERX_SLICEX_RX_SDLL_CTRL(qlm, s),
            c.s.pcs_sds_oob_clk_ctrl  = 2;
            c.s.pcs_sds_rx_sdll_tune  = 0;
            c.s.pcs_sds_rx_sdll_swsel = 0);
    }

    for (int lane = 0; lane < NUM_LANES; lane++)
    {
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_MISC_CFG_0(qlm, lane),
            c.s.use_pma_polarity     = 0;
            c.s.cfg_pcs_loopback     = 0;
            c.s.pcs_tx_mode_ovrrd_en = 0;
            c.s.pcs_rx_mode_ovrrd_en = 0;
            c.s.cfg_eie_det_cnt      = 0;
            c.s.eie_det_stl_on_time  = 4;
            c.s.eie_det_stl_off_time = 0;
            c.s.tx_bit_order         = 1;
            c.s.rx_bit_order         = 1);
    }

    /* 8. Wait for GSER(0..13)_QLM_STAT[RST_RDY] = 1, indicating that the PHY
       has been reconfigured and PLLs are locked. */
    if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_GSERX_QLM_STAT(qlm), rst_rdy, ==, 1, 10000))
    {
        bdk_error("QLM%d: Timeout waiting for GSERX_QLM_STAT[rst_rdy]\n", qlm);
        return -1;
    }
    /* 8a. Check that the SATA_PCS is "Ready" here, should be but check it */
    /* Poll GSERX_SATA_STATUS for PX_RDY = 1 */
    if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_GSERX_SATA_STATUS(qlm), p0_rdy, ==, 1, 10000))
    {
        bdk_error("QLM%d: Timeout waiting for GSERX_SATA_STATUS[p0_rdy]\n", qlm);
        return -1;
    }

    /* Add 1ms delay for everything to stabilize*/
    bdk_wait_usec(1000);

    /* Apply any custom tuning */
    __bdk_qlm_tune(node, qlm, BDK_QLM_MODE_SATA_4X1, baud_mhz);
    bdk_wait_usec(1000);


    /* 6.  Deassert UCTL and UAHC resets:
        a.  SATA(0..15)_UCTL_CTL[SATA_UAHC_RST] = 0
        b. SATA(0..15)_UCTL_CTL[SATA_UCTL_RST] = 0
        c. Wait 10 ACLK cycles before accessing any ACLK-only registers. */
    for (int p = sata_first; p <= sata_last; p++)
    {
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.sata_uahc_rst = 0;
            c.s.sata_uctl_rst = 0);
    }
    bdk_wait_usec(1);

    /* 7.  Enable conditional SCLK of UCTL by writing
        SATA(0..15)_UCTL_CTL[CSCLK_EN] = 1. */
    for (int p = sata_first; p <= sata_last; p++)
    {
        if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        {
            /* CN9XXX make coprocessor clock automatic */
            BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
                c.cn8.csclk_en = 1);
        }
    }

    /* Check BIST on the SATA controller. Start BIST in parallel on the
       controllers */

    for (int p = sata_first; p <= sata_last; p++)
    {
        /* Make sure BIST is configured properly before we start it. We
           want full BIST, not just CLEAR */
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.clear_bist = 0;
            c.s.start_bist = 0);
        /* Start BIST */
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.start_bist = 1);
    }
    bdk_wait_usec(1000);
    for (int p = sata_first; p <= sata_last; p++)
    {
        BDK_CSR_INIT(bist, node, BDK_SATAX_UCTL_BIST_STATUS(p));
        if (bist.u)
            bdk_error("N%d.SATA%d: Controller failed BIST (0x%llx)\n", node, p, bist.u);
        else
            BDK_TRACE(SATA, "N%d.SATA%d: Passed BIST\n", node, p);
    }
    /* Clear start_bist so it is ready for the next run */
    for (int p = sata_first; p <= sata_last; p++)
    {
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.start_bist = 0);
    }

    int spd;
    if (baud_mhz < 3000)
        spd = 1;
    else if (baud_mhz < 6000)
        spd = 2;
    else
        spd = 3;

    for (int p = sata_first; p <= sata_last; p++)
    {
        /* From the synopsis data book, SATAX_UAHC_GBL_TIMER1MS is the
           AMBA clock in MHz * 1000, which is a_clk(Hz) / 1000 */
        BDK_TRACE(QLM, "QLM%d: SATA%d set to %d Hz\n", qlm, p, a_clk);
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UAHC_GBL_TIMER1MS(p),
            c.s.timv = a_clk / 1000);
        /* Set speed */
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UAHC_P0_SCTL(p),
            c.s.ipm = 3; /* Disable parial and slumber power management */
            c.s.spd = spd);
        /* The following SATA setup is from the AHCI 1.3 spec, section
           10.1.1, Firmware Specific Initialization. */
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UAHC_GBL_CAP(p),
            c.s.sss = 1;    /* Support staggered spin-up */
            c.s.smps = 1);  /* Support mechanical presence switch */
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UAHC_GBL_PI(p),
            c.s.pi = 1);    /* One port per controller */
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UAHC_P0_CMD(p),
            c.s.hpcp = 1;   /* Hot-plug-capable support */
            c.s.mpsp = 1;   /* Mechanical presence switch attached to port */
            c.s.cpd = 1);   /* Cold-presence detection */
    }
    return 0;
}

/**
 * Initialize a DLM/QLM for use with SATA controllers
 *
 * @param node       Node to intialize
 * @param qlm        Which DLM/QLM to init
 * @param baud_mhz   QLM speed in Gbaud
 * @param sata_first First SATA controller connected to this DLM/QLM
 * @param sata_last  Last SATA controller connected to this DLM/QLM (inclusive)
 *
 * @return Zero on success, negative on failure
 */
int __bdk_qlm_set_sata_cn9xxx(bdk_node_t node, int qlm, int baud_mhz, int sata_first, int sata_last)
{
    //const int NUM_LANES = sata_last - sata_first + 1;
    const int MAX_A_CLK = 333000000; /* Max of 333Mhz */

    /* 26.4.1 Cold Reset */
    /* 1.  Ensure that the SerDes reference clock is up and stable. */
    /* Already done */

    /* 2.  Optionally program the GPIO CSRs for SATA features.
        a.  For cold-presence detect, select a GPIO for the input and program GPI-
            O_BIT_CFG(0..50)[PIN_SEL] = GPIO_PIN_SEL_E::SATA(0..15)_CP_DET.
        b. For mechanical-presence detect, select a GPIO for the input and program
            GPIO_BIT_CFG(0..50)[PIN_SEL] = GPI-
            O_PIN_SEL_E::SATA(0..15)_MP_SWITCH.
        c. For BIST board-test loopback, select a GPIO for the input and program GPI-
            O_BIT_CFG(0..50)[PIN_SEL] = GPIO_PIN_SEL_E:::SATA_LAB_LB.
        d. For LED activity, select a GPIO for the output and program GPI-
            O_BIT_CFG(0..50)[PIN_SEL] = GPIO_PIN_SEL_E:::SATA(0..15)_ACT_LED.
        e. For cold-presence power-on-device, select a GPIO for the output and program
            GPIO_BIT_CFG(0..50)[PIN_SEL] = GPIO_PIN_SEL_E:::SATA(0..15)_CP_-
            POD. */
    /* Skipping */

    /* 3.  Optionally program the SGPIO unit. */
    /* Skipping */

    /* 4.  Assert all resets:
        a.  UAHC reset: SATA(0..15)_UCTL_CTL[SATA_UAHC_RST] = 1
        b. UCTL reset: SATA(0..15)_UCTL_CTL[SATA_UCTL_RST] = 1 */
    for (int p = sata_first; p <= sata_last; p++)
    {
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.sata_uahc_rst = 1;
            c.s.sata_uctl_rst = 1);
    }

    /* 5.  Configure the ACLK:
        a.  Reset the clock dividers: SATA(0..15)_UCTL_CTL[A_CLKDIV_RST] = 1.
        b. Select the ACLK frequency (refer to maximum values in Table 26 1).
            i. SATA(0..15)_UCTL_CTL[A_CLKDIV_SEL] = desired value,
            ii. SATA(0..15)_UCTL_CTL[A_CLK_EN] = 1 to enable the ACLK.
        c.  Deassert the ACLK clock divider reset:
            SATA(0..15)_UCTL_CTL[A_CLKDIV_RST] = 0. */
    int divisor = (bdk_clock_get_rate(node, BDK_CLOCK_SCLK) + MAX_A_CLK - 1) / MAX_A_CLK;
    int a_clkdiv;
    /* This screwy if logic is from the description of
       SATAX_UCTL_CTL[a_clkdiv_sel] in the CSR */
    if (divisor <= 4)
    {
        a_clkdiv = divisor - 1;
        /* Divisor matches calculated value */
    }
    else if (divisor <= 6)
    {
        a_clkdiv = 4;
        divisor = 6;
    }
    else if (divisor <= 8)
    {
        a_clkdiv = 5;
        divisor = 8;
    }
    else if (divisor <= 16)
    {
        a_clkdiv = 6;
        divisor = 16;
    }
    else if (divisor <= 24)
    {
        a_clkdiv = 7;
        divisor = 24;
    }
    else
    {
        bdk_error("Unable to determine SATA clock divisor\n");
        return -1;
    }
    /* Calculate the final clock rate */
    int a_clk = bdk_clock_get_rate(node, BDK_CLOCK_SCLK) / divisor;

    for (int p = sata_first; p <= sata_last; p++)
    {
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.a_clkdiv_rst = 1);
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.a_clk_byp_sel = 0;
            c.s.a_clkdiv_sel = a_clkdiv;
            c.s.a_clk_en = 1);
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.a_clkdiv_rst = 0);
    }
    bdk_wait_usec(1);

    /* 8.  Configure PHY for SATA. Refer to Section 21.1.2. */
    /* Done below, section 24.1.2.3 */

    /* 9.  TBD: Poll QLM2_MPLL_STATUS for MPLL lock */
    /* Not needed */

    /* 10. Initialize UAHC as described in the AHCI specification
        (UAHC_* registers). */
    /* Done when a SATA driver is initialized */

    /* 24.1.2.3 SATA Configuration
    Software must perform the following steps to configure the GSER_WEST
    for a SATA application. Note that the GSERW steps are on a QLM basis. */

    /* 1. Configure the SATA controller (refer to Chapter 26). */
    /* This is the code above */

    /* 2. Configure the QLM Reference clock.
        Set GSER(0..13)_REFCLK_SEL[COM_CLK_SEL] = 1 to source the reference
            clock from the external clock multiplexer.
        Configure GSER(0..13)_REFCLK_SEL[USE_COM1]:
            0 = use QLMC_REF_CLK0_P/N
            1 = use QLMC_REF_CLK1_P/N */
    /* Already done */

    // FIXME: GSERN setup

    /* 6.  Deassert UCTL and UAHC resets:
        a.  SATA(0..15)_UCTL_CTL[SATA_UAHC_RST] = 0
        b. SATA(0..15)_UCTL_CTL[SATA_UCTL_RST] = 0
        c. Wait 10 ACLK cycles before accessing any ACLK-only registers. */
    for (int p = sata_first; p <= sata_last; p++)
    {
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.sata_uahc_rst = 0;
            c.s.sata_uctl_rst = 0);
    }
    bdk_wait_usec(1);

    /* 7.  Enable conditional SCLK of UCTL by writing
        SATA(0..15)_UCTL_CTL[CSCLK_EN] = 1. */
    for (int p = sata_first; p <= sata_last; p++)
    {
        if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        {
            /* CN9XXX make coprocessor clock automatic */
            BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
                c.cn8.csclk_en = 1);
        }
    }

    /* Check BIST on the SATA controller. Start BIST in parallel on the
       controllers */
    for (int p = sata_first; p <= sata_last; p++)
    {
       /* Make sure BIST is configured properly before we start it. We
           want full BIST, not just CLEAR */
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.clear_bist = 0;
            c.s.start_bist = 0);
        /* Start BIST */
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.start_bist = 1);
    }
    bdk_wait_usec(1000);
    for (int p = sata_first; p <= sata_last; p++)
    {
        BDK_CSR_INIT(bist, node, BDK_SATAX_UCTL_BIST_STATUS(p));
        if (bist.u)
            bdk_error("N%d.SATA%d: Controller failed BIST (0x%llx)\n", node, p, bist.u);
        else
            BDK_TRACE(SATA, "N%d.SATA%d: Passed BIST\n", node, p);
    }
    /* Clear start_bist so it is ready for the next run */
    for (int p = sata_first; p <= sata_last; p++)
    {
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UCTL_CTL(p),
            c.s.start_bist = 0);
    }

    int spd;
    if (baud_mhz < 3000)
        spd = 1;
    else if (baud_mhz < 6000)
        spd = 2;
    else
        spd = 3;

    for (int p = sata_first; p <= sata_last; p++)
    {
        /* From the synopsis data book, SATAX_UAHC_GBL_TIMER1MS is the
           AMBA clock in MHz * 1000, which is a_clk(Hz) / 1000 */
        BDK_TRACE(QLM, "QLM%d: SATA%d set to %d Hz\n", qlm, p, a_clk);
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UAHC_GBL_TIMER1MS(p),
            c.s.timv = a_clk / 1000);
        /* Set speed */
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UAHC_P0_SCTL(p),
            c.s.ipm = 3; /* Disable parial and slumber power management */
            c.s.spd = spd);
        /* The following SATA setup is from the AHCI 1.3 spec, section
           10.1.1, Firmware Specific Initialization. */
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UAHC_GBL_CAP(p),
            c.s.sss = 1;    /* Support staggered spin-up */
            c.s.smps = 1);  /* Support mechanical presence switch */
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UAHC_GBL_PI(p),
            c.s.pi = 1);    /* One port per controller */
        BDK_CSR_MODIFY(c, node, BDK_SATAX_UAHC_P0_CMD(p),
            c.s.hpcp = 1;   /* Hot-plug-capable support */
            c.s.mpsp = 1;   /* Mechanical presence switch attached to port */
            c.s.cpd = 1);   /* Cold-presence detection */
    }
    return 0;
}

