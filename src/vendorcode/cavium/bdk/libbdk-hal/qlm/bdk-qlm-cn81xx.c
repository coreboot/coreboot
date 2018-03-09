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
#include "libbdk-hal/bdk-qlm.h"
#include "libbdk-hal/qlm/bdk-qlm-common.h"
#include "libbdk-arch/bdk-csrs-bgx.h"
#include "libbdk-arch/bdk-csrs-gser.h"
#include "libbdk-arch/bdk-csrs-pem.h"
#include "libbdk-arch/bdk-csrs-sata.h"
#include "libbdk-arch/bdk-csrs-rst.h"
#include "libbdk-hal/bdk-config.h"
#include "libbdk-hal/qlm/bdk-qlm-errata-cn8xxx.h"
#include "libbdk-hal/bdk-gpio.h"

/**
 * Return the number of QLMs supported for the chip
 *
 * @return Number of QLMs
 */
int bdk_qlm_get_num(bdk_node_t node)
{
    return 4; /* 4 DLM */
}

/**
 * Return the number of lanes in a QLM. QLMs normally contain
 * 4 lanes, except for chips which only have half of a QLM.
 *
 * @param qlm    QLM to get lanes number for
 *
 * @return Number of lanes on the QLM
 */
int bdk_qlm_get_lanes(bdk_node_t node, int qlm)
{

    if ((qlm < 2) && cavium_is_altpkg(CAVIUM_CN81XX))
        return 1; /* DLM0 and DLM1 are a single lane on CN80XX */
    else
        return 2; /* DLMs */
}

/**
 * Lookup the hardware QLM number for a given interface type and index. This
 * function will fail with a fatal error if called on invalid interfaces for
 * a chip. It returns the QLM number for an interface without checking to
 * see if the QLM is in the correct mode.
 *
 * @param iftype    Interface type
 * @param interface Interface index number
 *
 * @return QLM number. Dies on a fatal error on failure.
 */
int bdk_qlm_get_qlm_num(bdk_node_t node, bdk_if_t iftype, int interface, int index)
{
    switch (iftype)
    {
        case BDK_IF_BGX:
        {
            int qlm;
            switch (interface)
            {
                case 0:
                {
                    /* This BGX spans two DLMs. The index must be used to
                       figure out which DLM we are using */
                    BDK_CSR_INIT(gserx_cfg, node, BDK_GSERX_CFG(0));
                    if (gserx_cfg.s.bgx)
                    {
                        if (gserx_cfg.s.bgx_quad) /* 4 lanes together */
                            qlm = 0;
                        else if (gserx_cfg.s.bgx_dual) /* 2 lanes together */
                            qlm = (index >= 1) ? 1 : 0;
                        else /* All lanes independent */
                        {
                            bdk_qlm_modes_t mode = bdk_qlm_get_mode(node, 0);
                            if (mode == BDK_QLM_MODE_QSGMII_4X1)
                                qlm = 0;
                            else if (mode <= BDK_QLM_MODE_PCIE_1X8)
                                qlm = 1;
                            else if (cavium_is_altpkg(CAVIUM_CN81XX))
                            {
                                bdk_qlm_modes_t mode1 = bdk_qlm_get_mode(node, 1);
                                if ((mode1 != BDK_QLM_MODE_QSGMII_4X1) && (index >= 2))
                                    return -1;
                                qlm = (index >= 1) ? 1 : 0;
                            }
                            else
                                qlm = (index >= 2) ? 1 : 0;
                        }
                    }
                    else
                        qlm = 1;
                    break;
                }
                case 1:
                {
                    /* This BGX spans two DLMs. The index must be used to
                       figure out which DLM we are using */
                    BDK_CSR_INIT(gserx_cfg, node, BDK_GSERX_CFG(2));
                    if (gserx_cfg.s.bgx)
                    {
                        if (gserx_cfg.s.bgx_quad) /* 4 lanes together */
                            qlm = 2;
                        else if (gserx_cfg.s.bgx_dual) /* 2 lanes together */
                            qlm = (index >= 1) ? 3 : 2;
                        else /* All lanes independent */
                        {
                            bdk_qlm_modes_t mode = bdk_qlm_get_mode(node, 2);
                            if (mode == BDK_QLM_MODE_QSGMII_4X1)
                                qlm = 2;
                            else if (mode <= BDK_QLM_MODE_PCIE_1X8)
                                qlm = 1;
                            else
                                qlm = (index >= 2) ? 3 : 2;
                        }
                    }
                    else
                        qlm = 3;
                    break;
                }
                default:
                    return -1;
            }
            /* Make sure the QLM is powered up and out of reset */
            BDK_CSR_INIT(phy_ctl, node, BDK_GSERX_PHY_CTL(qlm));
            if (phy_ctl.s.phy_pd || phy_ctl.s.phy_reset)
                return -1;
            /* Make sure the QLM is in BGX mode */
            BDK_CSR_INIT(gserx_cfg, node, BDK_GSERX_CFG(qlm));
            if (gserx_cfg.s.bgx)
                return qlm;
            else
                return -1;
        }
        case BDK_IF_PCIE: /* PCIe */
        {
            switch (interface)
            {
                case 0: /* PEM0 */
                {
                    BDK_CSR_INIT(gserx_cfg, node, BDK_GSERX_CFG(0));
                    if (gserx_cfg.s.pcie)
                        return 0; /* PEM0 is on DLM0 */
                    else
                        return -1; /* PEM0 is disabled */
                }
                case 1: /* PEM1 */
                {
                    BDK_CSR_INIT(gserx_cfg, node, BDK_GSERX_CFG(2));
                    if (gserx_cfg.s.pcie)
                        return 2; /* PEM1 is on DLM2 */
                    else
                        return -1; /* PEM1 is disabled */
                }
                case 2: /* PEM2 */
                {
                    BDK_CSR_INIT(pem1_cfg, node, BDK_PEMX_CFG(1));
                    BDK_CSR_INIT(gserx_cfg, node, BDK_GSERX_CFG(3));
                    if (!pem1_cfg.cn81xx.lanes4 && gserx_cfg.s.pcie)
                        return 3; /* PEM2 is on DLM3 */
                    else
                        return -1; /* PEM2 is disabled */
                }
                default: /* Max of 3 PEMs, 0-2 */
                    return -1;
            }
        }
        default: /* Not supported by CN81XX */
            return -1;
    }
}

/**
 * Get the mode of a QLM as a human readable string
 *
 * @param qlm    QLM to examine
 *
 * @return String mode
 */
bdk_qlm_modes_t bdk_qlm_get_mode(bdk_node_t node, int qlm)
{
    BDK_CSR_INIT(gserx_cfg, node, BDK_GSERX_CFG(qlm));
    if (gserx_cfg.s.pcie)
    {
        switch (qlm)
        {
            case 0: /* PEM0 */
            {
                BDK_CSR_INIT(pemx_cfg, node, BDK_PEMX_CFG(0));
                if (cavium_is_altpkg(CAVIUM_CN81XX))
                    return BDK_QLM_MODE_PCIE_1X1; /* PEM0 x1 */
                else if (pemx_cfg.cn81xx.lanes4)
                    return BDK_QLM_MODE_PCIE_1X4; /* PEM0 x4 */
                else
                    return BDK_QLM_MODE_PCIE_1X2; /* PEM0 x2 */
            }
            case 1: /* PEM0 second two lanes */
                return BDK_QLM_MODE_PCIE_1X4; /* PEM0 x4 */
            case 2: /* Either PEM1 x4 or PEM1 x2 */
            {
                BDK_CSR_INIT(pemx_cfg, node, BDK_PEMX_CFG(1));
                if (pemx_cfg.cn81xx.lanes4)
                    return BDK_QLM_MODE_PCIE_1X4; /* PEM1 x4 */
                else
                    return BDK_QLM_MODE_PCIE_1X2; /* PEM1 x2 */
            }
            case 3: /* Either PEM1 x4 or PEM2 x2 */
            {
                /* Can be last 2 lanes of PEM1 */
                BDK_CSR_INIT(pem1_cfg, node, BDK_PEMX_CFG(1));
                if (pem1_cfg.cn81xx.lanes4)
                    return BDK_QLM_MODE_PCIE_1X4; /* PEM1 x4 */
                /* Can be 2 lanes of PEM2 */
                return BDK_QLM_MODE_PCIE_1X2; /* PEM2 x2 */
            }
            default:
                return BDK_QLM_MODE_DISABLED;
        }
    }
    else if (gserx_cfg.s.bgx)
    {
        int bgx;
        int bgx_index;
        switch (qlm)
        {
            case 0:
            {
                bgx = 0;
                bgx_index = 0;
                break;
            }
            case 1:
                bgx = 0;
                bgx_index = 2;
                break;
            case 2:
            {
                bgx = 1;
                bgx_index = 0;
                break;
            }
            case 3:
                bgx = 1;
                bgx_index = 2;
                break;
            default:
                return BDK_QLM_MODE_DISABLED;
        }
        BDK_CSR_INIT(cmrx_config, node, BDK_BGXX_CMRX_CONFIG(bgx, bgx_index));
        bool is_kr = __bdk_qlm_is_lane_kr(node, qlm, 0);
        switch (cmrx_config.s.lmac_type)
        {
            case BDK_BGX_LMAC_TYPES_E_SGMII:
                if (cavium_is_altpkg(CAVIUM_CN81XX) && (qlm < 2))
                    return BDK_QLM_MODE_SGMII_1X1;
                else
                    return BDK_QLM_MODE_SGMII_2X1;
            case BDK_BGX_LMAC_TYPES_E_XAUI: return BDK_QLM_MODE_XAUI_1X4; /* Doesn't differntiate between XAUI and DXAUI */
            case BDK_BGX_LMAC_TYPES_E_RXAUI: return BDK_QLM_MODE_RXAUI_1X2;
            case BDK_BGX_LMAC_TYPES_E_TENG_R:
                if (is_kr)
                    return (cavium_is_altpkg(CAVIUM_CN81XX) && (qlm < 2)) ? BDK_QLM_MODE_10G_KR_1X1 : BDK_QLM_MODE_10G_KR_2X1;
                else
                    return (cavium_is_altpkg(CAVIUM_CN81XX) && (qlm < 2)) ? BDK_QLM_MODE_XFI_1X1 : BDK_QLM_MODE_XFI_2X1;
            case BDK_BGX_LMAC_TYPES_E_FORTYG_R:
                if (is_kr)
                    return BDK_QLM_MODE_40G_KR4_1X4;
                else
                    return BDK_QLM_MODE_XLAUI_1X4;
            case BDK_BGX_LMAC_TYPES_E_QSGMII: return BDK_QLM_MODE_QSGMII_4X1;
            default:  return BDK_QLM_MODE_DISABLED;
        }
    }
    else if (gserx_cfg.s.sata)
        return BDK_QLM_MODE_SATA_2X1;
    else
        return BDK_QLM_MODE_DISABLED;
}

static int qlm_set_sata(bdk_node_t node, int qlm, bdk_qlm_modes_t mode, int baud_mhz, bdk_qlm_mode_flags_t flags)
{
    /* SATA has a fixed mapping for ports on CN81XX */
    int sata_port;
    switch (qlm)
    {
        case 3: /* SATA 0-1 = DLM3 lanes 0-1 */
            sata_port = 0;
            break;
        default:
            bdk_error("Attempted to configure SATA on QLM that doesn't support it\n");
            return -1;
    }
    return __bdk_qlm_set_sata_cn8xxx(node, qlm, baud_mhz, sata_port, sata_port + 1);
}

/**
 * For chips that don't use pin strapping, this function programs
 * the QLM to the specified mode
 *
 * @param node     Node to use in a Numa setup
 * @param qlm      QLM to configure
 * @param mode     Desired mode
 * @param baud_mhz Desired speed
 * @param flags    Flags to specify mode specific options
 *
 * @return Zero on success, negative on failure
 */
int bdk_qlm_set_mode(bdk_node_t node, int qlm, bdk_qlm_modes_t mode, int baud_mhz, bdk_qlm_mode_flags_t flags)
{
    int lane_mode = 0xf;
    int lmac_type = -1;
    int is_pcie = 0;
    int is_sata = 0;
    int is_ilk = 0;
    int is_bgx = 0;
    int bgx_block;
    int bgx_index;

    switch (qlm)
    {
        case 0:
            bgx_block = 0;
            bgx_index = 0;
            break;
        case 1:
            bgx_block = 0;
            bgx_index = 2;
            break;
        case 2:
            bgx_block = 1;
            bgx_index = 0;
            break;
        case 3:
            bgx_block = 1;
            bgx_index = 2;
            break;
        default:
            bgx_block = -1;
            bgx_index = -1;
            break;
    }

    int measured_ref = bdk_qlm_measure_clock(node, qlm);
    int ref_clk = (mode == BDK_QLM_MODE_DISABLED) ? 0 : __bdk_qlm_round_refclock(node, qlm, measured_ref);
    int kr_mode = 0;

    switch (mode)
    {
        case BDK_QLM_MODE_PCIE_1X1:
        case BDK_QLM_MODE_PCIE_1X2:
        case BDK_QLM_MODE_PCIE_1X4:
        {
            /* Note: PCIe ignores baud_mhz. Use the GEN 1/2/3 flags
               to control speed */
            is_pcie = 1;
            if (ref_clk == REF_100MHZ)
            {
                BDK_CSR_MODIFY(c, node, BDK_GSERX_REFCLK_SEL(qlm),
                    c.s.pcie_refclk125 = 0);
                if (baud_mhz == 2500)
                    lane_mode = BDK_GSER_LMODE_E_R_25G_REFCLK100;
                else if (baud_mhz == 5000)
                    lane_mode = BDK_GSER_LMODE_E_R_5G_REFCLK100;
                else
                    lane_mode = BDK_GSER_LMODE_E_R_8G_REFCLK100;
            }
            else if (ref_clk == REF_125MHZ)
            {
                BDK_CSR_MODIFY(c, node, BDK_GSERX_REFCLK_SEL(qlm),
                    c.s.pcie_refclk125 = 1);
                if (baud_mhz == 2500)
                    lane_mode = BDK_GSER_LMODE_E_R_25G_REFCLK125;
                else if (baud_mhz == 5000)
                    lane_mode = BDK_GSER_LMODE_E_R_5G_REFCLK125;
                else
                    lane_mode = BDK_GSER_LMODE_E_R_8G_REFCLK125;
            }
            else
            {
                bdk_error("Invalid reference clock for PCIe on QLM%d\n", qlm);
                return -1;
            }
            int cfg_md;
            if (baud_mhz == 2500)
                cfg_md = 0; /* Gen1 Speed */
            else if (baud_mhz == 5000)
                cfg_md = 1; /* Gen2 Speed */
            else
                cfg_md = 2; /* Gen3 Speed */
            switch (qlm)
            {
                case 0: /* Either PEM0 x4 or PEM0 x2 or PEM0 x1 */
                    BDK_CSR_MODIFY(c, node, BDK_RST_SOFT_PRSTX(0),
                        c.s.soft_prst = !(flags & BDK_QLM_MODE_FLAG_ENDPOINT));
                    __bdk_qlm_setup_pem_reset(node, 0, flags & BDK_QLM_MODE_FLAG_ENDPOINT);
                    BDK_CSR_MODIFY(c, node, BDK_PEMX_CFG(0),
                        c.cn81xx.lanes4 = (mode == BDK_QLM_MODE_PCIE_1X4);
                        //c.cn81xx.hostmd = !(flags & BDK_QLM_MODE_FLAG_ENDPOINT);
                        c.cn81xx.md = cfg_md);
                    break;
                case 1: /* Second two lanes for PEM0 x4 */
                    /* PEMX_CFG already setup */
                    break;
                case 2: /* Either PEM1 x4 or PEM1 x2 */
                    BDK_CSR_MODIFY(c, node, BDK_RST_SOFT_PRSTX(1),
                        c.s.soft_prst = !(flags & BDK_QLM_MODE_FLAG_ENDPOINT));
                    __bdk_qlm_setup_pem_reset(node, 1, flags & BDK_QLM_MODE_FLAG_ENDPOINT);
                    BDK_CSR_MODIFY(c, node, BDK_PEMX_CFG(1),
                        c.cn81xx.lanes4 = (mode == BDK_QLM_MODE_PCIE_1X4);
                        //c.cn81xx.hostmd = !(flags & BDK_QLM_MODE_FLAG_ENDPOINT);
                        c.cn81xx.md = cfg_md);
                    break;
                case 3: /* Either PEM1 x4 or PEM2 x2 */
                    if (mode == BDK_QLM_MODE_PCIE_1X4)
                    {
                        /* Last 2 lanes of PEM1 */
                        /* PEMX_CFG already setup */
                    }
                    else
                    {
                        /* Two lanes for PEM2 */
                        BDK_CSR_MODIFY(c, node, BDK_RST_SOFT_PRSTX(2),
                            c.s.soft_prst = !(flags & BDK_QLM_MODE_FLAG_ENDPOINT));
                        __bdk_qlm_setup_pem_reset(node, 2, flags & BDK_QLM_MODE_FLAG_ENDPOINT);
                        BDK_CSR_MODIFY(c, node, BDK_PEMX_CFG(2),
                            c.cn81xx.lanes4 = 0;
                            //c.cn81xx.hostmd = !(flags & BDK_QLM_MODE_FLAG_ENDPOINT);
                            c.cn81xx.md = cfg_md);
                    }
                    break;
                default:
                    return -1;
            }
            break;
        }
        case BDK_QLM_MODE_SGMII_4X1:
        case BDK_QLM_MODE_SGMII_2X1:
        case BDK_QLM_MODE_SGMII_1X1:
            /* Disable port BGX ports 2-3 on CN80XX */
            if ((qlm < 2) && cavium_is_altpkg(CAVIUM_CN81XX))
            {
                BDK_CSR_WRITE(node, BDK_BGXX_CMRX_RX_DMAC_CTL(0, 2), 0);
                BDK_CSR_WRITE(node, BDK_BGXX_CMRX_RX_DMAC_CTL(0, 3), 0);
            }
            lmac_type = BDK_BGX_LMAC_TYPES_E_SGMII; /* SGMII */
            is_bgx = 1;
            lane_mode = __bdk_qlm_get_lane_mode_for_speed_and_ref_clk("SGMII", qlm, ref_clk, baud_mhz);
            if (lane_mode == -1)
                return -1;
            break;
        case BDK_QLM_MODE_XAUI_1X4:
            lmac_type = BDK_BGX_LMAC_TYPES_E_XAUI; /* XAUI */
            is_bgx = 5;
            lane_mode = __bdk_qlm_get_lane_mode_for_speed_and_ref_clk("XAUI", qlm, ref_clk, baud_mhz);
            if (lane_mode == -1)
                return -1;
            break;
        case BDK_QLM_MODE_RXAUI_2X2:
        case BDK_QLM_MODE_RXAUI_1X2:
            lmac_type = BDK_BGX_LMAC_TYPES_E_RXAUI; /* RXAUI */
            is_bgx = 3;
            lane_mode = __bdk_qlm_get_lane_mode_for_speed_and_ref_clk("RXAUI", qlm, ref_clk, baud_mhz);
            if (lane_mode == -1)
                return -1;
            break;
        case BDK_QLM_MODE_XFI_4X1:
        case BDK_QLM_MODE_XFI_2X1:
        case BDK_QLM_MODE_XFI_1X1:
            /* Disable port BGX ports 2-3 on CN80XX */
            if ((qlm < 2) && cavium_is_altpkg(CAVIUM_CN81XX))
            {
                BDK_CSR_WRITE(node, BDK_BGXX_CMRX_RX_DMAC_CTL(0, 2), 0);
                BDK_CSR_WRITE(node, BDK_BGXX_CMRX_RX_DMAC_CTL(0, 3), 0);
            }
            lmac_type = BDK_BGX_LMAC_TYPES_E_TENG_R; /* 10G_R */
            is_bgx = 1;
            lane_mode = __bdk_qlm_get_lane_mode_for_speed_and_ref_clk("XFI", qlm, ref_clk, baud_mhz);
            if (lane_mode == -1)
                return -1;
            break;
        case BDK_QLM_MODE_XLAUI_1X4:
            lmac_type = BDK_BGX_LMAC_TYPES_E_FORTYG_R; /* 40G_R */
            is_bgx = 5;
            lane_mode = __bdk_qlm_get_lane_mode_for_speed_and_ref_clk("XLAUI", qlm, ref_clk, baud_mhz);
            if (lane_mode == -1)
                return -1;
            break;
        case BDK_QLM_MODE_10G_KR_4X1:
        case BDK_QLM_MODE_10G_KR_2X1:
        case BDK_QLM_MODE_10G_KR_1X1:
            /* Disable port BGX ports 2-3 on CN80XX */
            if ((qlm < 2) && cavium_is_altpkg(CAVIUM_CN81XX))
            {
                BDK_CSR_WRITE(node, BDK_BGXX_CMRX_RX_DMAC_CTL(0, 2), 0);
                BDK_CSR_WRITE(node, BDK_BGXX_CMRX_RX_DMAC_CTL(0, 3), 0);
            }
            lmac_type = BDK_BGX_LMAC_TYPES_E_TENG_R; /* 10G_R */
            is_bgx = 1;
            lane_mode = __bdk_qlm_get_lane_mode_for_speed_and_ref_clk("10G-KR", qlm, ref_clk, baud_mhz);
            if (lane_mode == -1)
                return -1;
            kr_mode = 1;
            break;
        case BDK_QLM_MODE_40G_KR4_1X4:
            lmac_type = BDK_BGX_LMAC_TYPES_E_FORTYG_R; /* 40G_R */
            is_bgx = 5;
            lane_mode = __bdk_qlm_get_lane_mode_for_speed_and_ref_clk("40G-KR", qlm, ref_clk, baud_mhz);
            if (lane_mode == -1)
                return -1;
            kr_mode = 1;
            break;
        case BDK_QLM_MODE_QSGMII_4X1:
            lmac_type = BDK_BGX_LMAC_TYPES_E_QSGMII; /* QSGMII */
            is_bgx = 1;
            lane_mode = BDK_GSER_LMODE_E_R_5G_REFCLK15625_QSGMII;
            break;
        case BDK_QLM_MODE_SATA_2X1:
            BDK_CSR_MODIFY(c, node, BDK_GSERX_LANE_MODE(qlm), c.s.lmode = BDK_GSER_LMODE_E_R_8G_REFCLK100);
            /* SATA initialization is different than BGX. Call its init function
               and skip the rest of this routine */
            return qlm_set_sata(node, qlm, mode, baud_mhz, flags);
        case BDK_QLM_MODE_DISABLED:
            /* Set gser for the interface mode */
            BDK_CSR_MODIFY(c, node, BDK_GSERX_CFG(qlm),
                c.u = 0);
            /* Put the PHY in reset */
            BDK_CSR_MODIFY(c, node, BDK_GSERX_PHY_CTL(qlm),
                c.s.phy_reset = 1);
            return 0;
        default:
            bdk_error("Unsupported QLM mode %d\n", mode);
            return -1;
    }

    BDK_TRACE(QLM, "N%u.QLM%u: Power up...\n", node, qlm);

    /* Power up phy, but keep it in reset */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_PHY_CTL(qlm),
        c.s.phy_pd = 0;
        c.s.phy_reset = 1);

    /* Set gser for the interface mode */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_CFG(qlm),
        c.s.sata = is_sata;
        c.s.ila = is_ilk;
        c.s.bgx = is_bgx & 1;
        c.s.bgx_quad = (is_bgx >> 2) & 1;
        c.s.bgx_dual = (is_bgx >> 1) & 1;
        c.s.pcie = is_pcie);

    /* Lane mode */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_LANE_MODE(qlm),
        c.s.lmode = lane_mode);

    /* LMAC type. We only program one port as the full setup is done in BGX */
    if (lmac_type != -1)
    {
        BDK_CSR_MODIFY(c, node, BDK_BGXX_CMRX_CONFIG(bgx_block, bgx_index),
            c.s.enable = 0;
            c.s.lmac_type = lmac_type);
    }

    BDK_TRACE(QLM, "N%u.QLM%u: Deassert reset...\n", node, qlm);

    /* Bring phy out of reset */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_PHY_CTL(qlm),
        c.s.phy_reset = 0);

    /* Wait 1us until the management interface is ready to accept
       read/write commands.*/
    bdk_wait_usec(1);

    /* Configure the gser pll */
    __bdk_qlm_init_mode_table(node, qlm, ref_clk);

    /* Remember which lanes are using KR over BGX */
    if (is_bgx)
    {
        int num_lanes = bdk_qlm_get_lanes(node, qlm);
        for (int lane = 0; lane < num_lanes; lane++)
            __bdk_qlm_set_lane_kr(node, qlm, lane, kr_mode);
    }

    /* Wait for reset to complete and the PLL to lock */
    if (BDK_CSR_WAIT_FOR_FIELD(node, BDK_GSERX_PLL_STAT(qlm), pll_lock, ==, 1, 10000))
    {
        bdk_error("QLM%d: Timeout waiting for GSERX_PLL_STAT[pll_lock]\n", qlm);
        return -1;
    }

    /* PCIe mode doesn't become ready until the PEM block attempts to bring
       the interface up. Skip this check for PCIe */
    if (!is_pcie && BDK_CSR_WAIT_FOR_FIELD(node, BDK_GSERX_QLM_STAT(qlm), rst_rdy, ==, 1, 10000))
    {
        bdk_error("QLM%d: Timeout waiting for GSERX_QLM_STAT[rst_rdy]\n", qlm);
        return -1;
    }

    /* cdrlock will be checked in the BGX */

    /* Errata (GSER-27140) SERDES temperature drift sensitivity in receiver */
    int channel_loss = bdk_config_get_int(BDK_CONFIG_QLM_CHANNEL_LOSS, node, qlm);
    __bdk_qlm_errata_gser_27140(node, qlm, baud_mhz, channel_loss);

    /* Apply any custom tuning */
    __bdk_qlm_tune(node, qlm, mode, baud_mhz);

    /* Some modes require 4 lanes, which spans DLMs. For these modes, we need
       to setup the second DLM at the same time we setup the first. The second
       DLM also must use the same reference clock as the first */
    bool paired_dlm = ((qlm & 1) == 0) && /* We're on the first (even) DLM */
        ((mode == BDK_QLM_MODE_PCIE_1X4) || /* We're using a 4 lane mode */
         (mode == BDK_QLM_MODE_XAUI_1X4) ||
         (mode == BDK_QLM_MODE_XLAUI_1X4) ||
         (mode == BDK_QLM_MODE_40G_KR4_1X4));
    if (paired_dlm)
    {
        /* Use the same reference clock for the second QLM */
        BDK_CSR_WRITE(node, BDK_GSERX_REFCLK_SEL(qlm + 1),
            BDK_CSR_READ(node, BDK_GSERX_REFCLK_SEL(qlm)));
        return bdk_qlm_set_mode(node, qlm + 1, mode, baud_mhz, flags);
    }

    return 0;
}

/**
 * Get the speed (Gbaud) of the QLM in Mhz.
 *
 * @param qlm    QLM to examine
 *
 * @return Speed in Mhz
 */
int bdk_qlm_get_gbaud_mhz(bdk_node_t node, int qlm)
{
    BDK_CSR_INIT(gserx_cfg, node, BDK_GSERX_CFG(qlm));
    if (gserx_cfg.u == 0)
        return 0;
    if (gserx_cfg.s.pcie)
    {
        /* QLMs in PCIe mode ignore LMODE and get their speed from
           the PEM block that controls them */
        int pem;
        switch (qlm)
        {
            case 0: /* PEM0 */
            case 1: /* PEM0 */
                pem = 0;
                break;
            case 2: /* PEM1 */
                pem = 1;
                break;
            case 3: /* PEM1 or PEM2 */
            {
                BDK_CSR_INIT(pemx_cfg, node, BDK_PEMX_CFG(1));
                if (pemx_cfg.cn81xx.lanes4)
                    pem = 1;
                else
                    pem = 2;
                break;
            }
            default:
                bdk_fatal("QLM%d: In PCIe mode, which shouldn't happen\n", qlm);
        }
        return __bdk_qlm_get_gbaud_mhz_pem(node, pem);
    }
    else if (gserx_cfg.s.sata)
    {
        int sata;
        switch (qlm)
        {
            case 3:
                sata = 0;
                break;
            default:
                return 0;
        }
        BDK_CSR_INIT(sata_uctl_ctl, node, BDK_SATAX_UCTL_CTL(sata));
        if (!sata_uctl_ctl.s.a_clk_en)
            return 0;
        BDK_CSR_INIT(sctl, node, BDK_SATAX_UAHC_P0_SCTL(sata));
        switch (sctl.s.spd)
        {
            case 1: return 1500;
            case 2: return 3000;
            case 3: return 6000;
            default: return 6000; /* No limit, assume 6G */
        }
    }
    else
        return __bdk_qlm_get_gbaud_mhz_lmode(node, qlm);
}

/**
 * Initialize the QLM layer
 */
void bdk_qlm_init(bdk_node_t node)
{
    /* Setup how each PEM drives the PERST lines */
    for (int pem = 0; pem < 3; pem++)
    {
        BDK_CSR_INIT(rst_ctlx, node, BDK_RST_CTLX(pem));
        __bdk_qlm_setup_pem_reset(node, pem, !rst_ctlx.s.host_mode);
    }
}

static void __bdk_qlm_sff81xx_set_reference(bdk_node_t node, int qlm, int ref_clk)
{
    int use_clock;

    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) || CAVIUM_IS_MODEL(CAVIUM_CN83XX) || CAVIUM_IS_MODEL(CAVIUM_CN81XX))
    {
        // Common clock 0 is 156MHz
        // Common clock 1 is 100MHz
        switch (qlm)
        {
        case 0:
             use_clock = 1; /* DLMC_REF_CLK1 of 100MHz */
             break;
        case 1:
             if (ref_clk == REF_100MHZ)
                 use_clock = 1; /* DLMC_REF_CLK1 of 100MHz */
             else
                 use_clock = 2; /* DLM1_REF_CLK of 156MHz */
             break;
        case 2:
        case 3:
        default:
            if (ref_clk == REF_100MHZ)
                use_clock = 1; /* DLMC_REF_CLK1 of 100MHz */
            else
                use_clock = 2; /* DLM1_REF_CLK of 156MHz */
            break;
        }

          BDK_TRACE(QLM, "Setting N%d.QLM%d to use ref clock %d\n", node, qlm, use_clock);
    }
    else
    {
        bdk_error("Update %s for qlm auto config of this chip\n",__FUNCTION__);
        return;
    }
    BDK_CSR_MODIFY(c, node, BDK_GSERX_REFCLK_SEL(qlm),
        c.s.com_clk_sel = (use_clock != 2);
        c.s.use_com1 = (use_clock == 1));
}

int bdk_qlm_auto_config(bdk_node_t node)
{
    return -1;
}

/**
 * For Cavium SFF board, query the DIP switches in GPIO o determine the QLM setup.
 * Configure the GPIOs to read the DLM settings
 * SW1.1 -> DLM0_SEL -> GPIO_26
 * SW1.2 -> DLM1_SEL -> GPIO_25
 * SW1.3 -> DLM2_SEL -> GPIO_31
 * SW1.4 -> DLM3_SEL -> GPIO_4
  *V1.x boards SW3.8 -> QSGMII/XFI SEL ->GPIO_9
  *V2.x boards SW3.7 -> QSGMII/XFI SEL ->GPIO_36
*/
int bdk_qlm_dip_auto_config(bdk_node_t node)
{
    bdk_qlm_modes_t dlm_mode[4];
    int dlm_speed = 0;
    int use_ref = 0;
    bdk_qlm_mode_flags_t dlm_flags = 0;

    unsigned int dlm_config, dlm3, dlm2, dlm1, dlm0;
    uint64_t gpio = 0;

    /* Configure the GPIOs to read the DLM settings */
    /* SW1.1 -> DLM0_SEL -> GPIO_26 */
    /* SW1.2 -> DLM1_SEL -> GPIO_25 */
    /* SW1.3 -> DLM2_SEL -> GPIO_31 */
    /* SW1.4 -> DLM3_SEL -> GPIO_4  */
    //V1.x boards /* SW3.8 -> QSGMII/XFI SEL ->GPIO_9 */
    //V2.x boards /* SW3.7 -> QSGMII/XFI SEL ->GPIO_36 */
    /* Configure the GPIOs are input */
    bdk_gpio_initialize(node, 26, 0, 0);
    bdk_gpio_initialize(node, 25, 0, 0);
    bdk_gpio_initialize(node, 31, 0, 0);
    bdk_gpio_initialize(node, 4, 0, 0);
    bdk_gpio_initialize(node, 36, 0, 0);


    /* Read the GPIOs */
    gpio = bdk_gpio_read(node, 0);

    dlm3 = !!(gpio & (1ULL<<4));
    dlm2 = !!(gpio & (1ULL<<31));
    dlm1 = !!(gpio & (1ULL<<25));
    dlm0 = !!(gpio & (1ULL<<26));


    dlm_config = (dlm0<<3)| (dlm1<<2) | (dlm2<<1) | (dlm3);

    BDK_TRACE(QLM, "DLM CONFIG:%d gpio36: %d\n", dlm_config, !!(gpio & (1ULL<<36)));

    switch(dlm_config)
    {
    case 0:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[1] = BDK_QLM_MODE_DISABLED;
        dlm_mode[2] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[3] = BDK_QLM_MODE_PCIE_1X2;
        break;
    case 1:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[1] = BDK_QLM_MODE_DISABLED;
        dlm_mode[2] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[3] = BDK_QLM_MODE_SATA_2X1;
        break;
    case 2:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[1] = BDK_QLM_MODE_DISABLED;
        dlm_mode[2] = BDK_QLM_MODE_XFI_2X1;
        dlm_mode[3] = BDK_QLM_MODE_PCIE_1X2;
        break;
    case 3:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[1] = BDK_QLM_MODE_DISABLED;
        dlm_mode[2] = BDK_QLM_MODE_XFI_2X1;
        dlm_mode[3] = BDK_QLM_MODE_SATA_2X1;
        break;
    case 4:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[1] = (!!(gpio & (1ULL<<36)) ? BDK_QLM_MODE_XFI_1X1 : BDK_QLM_MODE_QSGMII_4X1);
        dlm_mode[2] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[3] = BDK_QLM_MODE_PCIE_1X2;
        break;
    case 5:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[1] = (!!(gpio & (1ULL<<36)) ? BDK_QLM_MODE_XFI_1X1 : BDK_QLM_MODE_QSGMII_4X1);
        dlm_mode[2] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[3] = BDK_QLM_MODE_SATA_2X1;
        break;
    case 6:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[1] = (!!(gpio & (1ULL<<36)) ? BDK_QLM_MODE_XFI_1X1 : BDK_QLM_MODE_QSGMII_4X1);
        dlm_mode[2] = BDK_QLM_MODE_XFI_2X1;
        dlm_mode[3] = BDK_QLM_MODE_PCIE_1X2;
        break;
    case 7:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[1] = (!!(gpio & (1ULL<<36)) ? BDK_QLM_MODE_XFI_1X1 : BDK_QLM_MODE_QSGMII_4X1);
        dlm_mode[2] = BDK_QLM_MODE_XFI_2X1;
        dlm_mode[3] = BDK_QLM_MODE_SATA_2X1;
        break;
    case 8:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X4;
        dlm_mode[1] = BDK_QLM_MODE_PCIE_1X4;
        dlm_mode[2] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[3] = BDK_QLM_MODE_PCIE_1X2;
        break;
    case 9:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X4;
        dlm_mode[1] = BDK_QLM_MODE_PCIE_1X4;
        dlm_mode[2] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[3] = BDK_QLM_MODE_SATA_2X1;
        break;
    case 10:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X4;
        dlm_mode[1] = BDK_QLM_MODE_PCIE_1X4;
        dlm_mode[2] = BDK_QLM_MODE_XFI_2X1;
        dlm_mode[3] = BDK_QLM_MODE_PCIE_1X2;
        break;
    case 11:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X4;
        dlm_mode[1] = BDK_QLM_MODE_PCIE_1X4;
        dlm_mode[2] = BDK_QLM_MODE_XFI_2X1;
        dlm_mode[3] = BDK_QLM_MODE_SATA_2X1;
        break;
    case 12:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[1] = (!!(gpio & (1ULL<<36)) ? BDK_QLM_MODE_XFI_1X1 : BDK_QLM_MODE_QSGMII_4X1);
        dlm_mode[2] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[3] = BDK_QLM_MODE_PCIE_1X2;
        break;
    case 13:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[1] = (!!(gpio & (1ULL<<36)) ? BDK_QLM_MODE_XFI_1X1 : BDK_QLM_MODE_QSGMII_4X1);
        dlm_mode[2] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[3] = BDK_QLM_MODE_SATA_2X1;
        break;
    case 14:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[1] = (!!(gpio & (1ULL<<36)) ? BDK_QLM_MODE_XFI_1X1 : BDK_QLM_MODE_QSGMII_4X1);
        dlm_mode[2] = BDK_QLM_MODE_XFI_2X1;
        dlm_mode[3] = BDK_QLM_MODE_PCIE_1X2;
        break;
    case 15:
        dlm_mode[0] = BDK_QLM_MODE_PCIE_1X2;
        dlm_mode[1] = (!!(gpio & (1ULL<<36)) ? BDK_QLM_MODE_XFI_1X1 : BDK_QLM_MODE_QSGMII_4X1);
        dlm_mode[2] = BDK_QLM_MODE_XFI_2X1;
        dlm_mode[3] = BDK_QLM_MODE_SATA_2X1;
        break;
    default:
        return -1;
    }

    for(int dlm = 0; dlm < 4; dlm++)
    {
        const char *dlm_mode_str = bdk_qlm_mode_tostring(dlm_mode[dlm]);
        switch(dlm_mode[dlm])
        {
            case  BDK_QLM_MODE_DISABLED:
                break;
            case BDK_QLM_MODE_XFI_2X1:
            case BDK_QLM_MODE_XFI_1X1:
                use_ref = REF_156MHZ;
                dlm_speed = 10312;
                break;
            case BDK_QLM_MODE_SATA_2X1:
                dlm_speed = 6000;
                use_ref = REF_100MHZ;
                break;
            case BDK_QLM_MODE_PCIE_1X2:
            case BDK_QLM_MODE_PCIE_1X4:
                dlm_speed = 8000;
                use_ref =REF_100MHZ;
                break;
            case BDK_QLM_MODE_QSGMII_4X1:
                use_ref = REF_100MHZ;
                dlm_speed = 5000;
                break;
            default:
                bdk_error("Unsupported N%d.QLM%d mode: %s(%d)",
                          node, dlm,
                          dlm_mode_str ? dlm_mode_str : "???",
                          dlm_mode[dlm]);
                return -1;
        }
        if ((1 == dlm) && (dlm_mode[dlm] != BDK_QLM_MODE_QSGMII_4X1) && (dlm_mode[dlm] != BDK_QLM_MODE_DISABLED))
        {
        /* This code is specific to sff8104 board
        ** QSGMII phy is wired to dlm1-gser lane 2
        ** AQR-107 phy is wired to dlm1-gser lane 3
        ** bdk always uses bgx0.port0 on that board
                */
                // If dlm1 is in XFI mode, change PHY address to mdio of aquantia phy
                unsigned mdio_bus = 1;
                unsigned mdio_addr = 0;
                int phy_cfg = 0xff<<24 | ((mdio_bus& 0xf)<<8) | (mdio_addr & 0xff);
                bdk_config_set_int((uint32_t) phy_cfg,BDK_CONFIG_PHY_ADDRESS, node, 0, 0);
        /* Indicate serdes lane 3 , aquantia phy active */
            int aq_phy = (0x3<<8) | 1;
                bdk_config_set_int(aq_phy, BDK_CONFIG_AQUANTIA_PHY,node,0,0);
                BDK_TRACE(QLM,"Disabling phys 0.1,0.2,0.3\n");
                for (int i = 1; i<4; i++) {
                    bdk_config_set_int(-1,BDK_CONFIG_PHY_ADDRESS, node, 0, i);
                    bdk_config_set_int(0,BDK_CONFIG_BGX_ENABLE,node,0,i);
                }
        }

        BDK_TRACE(QLM, "Setting N%d.QLM%d mode %s(%d), speed %d, flags 0x%x\n",
                  node, dlm, dlm_mode_str, dlm_mode[dlm], dlm_speed, dlm_flags);

        /* Set the reference clock for this QLM */
        __bdk_qlm_sff81xx_set_reference(node, dlm, use_ref);

        if (bdk_qlm_set_mode(node, dlm, dlm_mode[dlm], dlm_speed, dlm_flags))
            return -1;
    }
    return 0;
}
