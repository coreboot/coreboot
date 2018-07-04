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
#include <string.h>
#include "libbdk-arch/bdk-csrs-gser.h"
#include "libbdk-arch/bdk-csrs-gsern.h"
#include "libbdk-hal/if/bdk-if.h"
#include "libbdk-hal/bdk-qlm.h"
#include "libbdk-hal/qlm/bdk-qlm-common.h"

/* This code is an optional part of the BDK. It is only linked in
    if BDK_REQUIRE() needs it */
BDK_REQUIRE_DEFINE(QLM);

/**
 * Convert a mode into a configuration variable string value
 *
 * @param mode   Mode to convert
 *
 * @return configuration value string
 */
const char *bdk_qlm_mode_to_cfg_str(bdk_qlm_modes_t mode)
{
#define MODE_CASE(m) case m: return #m+13
    switch (mode)
    {
        MODE_CASE(BDK_QLM_MODE_DISABLED);
        MODE_CASE(BDK_QLM_MODE_PCIE_1X1);
        MODE_CASE(BDK_QLM_MODE_PCIE_2X1);
        MODE_CASE(BDK_QLM_MODE_PCIE_1X2);
        MODE_CASE(BDK_QLM_MODE_PCIE_1X4);
        MODE_CASE(BDK_QLM_MODE_PCIE_1X8);
        MODE_CASE(BDK_QLM_MODE_PCIE_1X16);

        MODE_CASE(BDK_QLM_MODE_SATA_4X1);
        MODE_CASE(BDK_QLM_MODE_SATA_2X1);

        MODE_CASE(BDK_QLM_MODE_ILK);
        MODE_CASE(BDK_QLM_MODE_SGMII_4X1);
        MODE_CASE(BDK_QLM_MODE_SGMII_2X1);
        MODE_CASE(BDK_QLM_MODE_SGMII_1X1);
        MODE_CASE(BDK_QLM_MODE_XAUI_1X4);
        MODE_CASE(BDK_QLM_MODE_RXAUI_2X2);
        MODE_CASE(BDK_QLM_MODE_RXAUI_1X2);
        MODE_CASE(BDK_QLM_MODE_OCI);
        MODE_CASE(BDK_QLM_MODE_XFI_4X1);
        MODE_CASE(BDK_QLM_MODE_XFI_2X1);
        MODE_CASE(BDK_QLM_MODE_XFI_1X1);
        MODE_CASE(BDK_QLM_MODE_XLAUI_1X4);
        MODE_CASE(BDK_QLM_MODE_10G_KR_4X1);
        MODE_CASE(BDK_QLM_MODE_10G_KR_2X1);
        MODE_CASE(BDK_QLM_MODE_10G_KR_1X1);
        MODE_CASE(BDK_QLM_MODE_40G_KR4_1X4);
        MODE_CASE(BDK_QLM_MODE_QSGMII_4X1);
        MODE_CASE(BDK_QLM_MODE_25G_4X1);
        MODE_CASE(BDK_QLM_MODE_25G_2X1);
        MODE_CASE(BDK_QLM_MODE_50G_2X2);
        MODE_CASE(BDK_QLM_MODE_50G_1X2);
        MODE_CASE(BDK_QLM_MODE_100G_1X4);
        MODE_CASE(BDK_QLM_MODE_25G_KR_4X1);
        MODE_CASE(BDK_QLM_MODE_25G_KR_2X1);
        MODE_CASE(BDK_QLM_MODE_50G_KR_2X2);
        MODE_CASE(BDK_QLM_MODE_50G_KR_1X2);
        MODE_CASE(BDK_QLM_MODE_100G_KR4_1X4);
        MODE_CASE(BDK_QLM_MODE_USXGMII_4X1);
        MODE_CASE(BDK_QLM_MODE_USXGMII_2X1);

        case BDK_QLM_MODE_LAST: break; /* fall through error */
    }
    return "INVALID_QLM_MODE_VALUE";
}

/**
 * Convert a configuration variable value string into a mode
 *
 * @param val  Configuration variable value
 *
 * @return mode
 */
bdk_qlm_modes_t bdk_qlm_cfg_string_to_mode(const char *val)
{
    bdk_qlm_modes_t mode;

    for (mode = 0; mode < BDK_QLM_MODE_LAST; mode++)
    {
        if (0 == strcmp(val, bdk_qlm_mode_to_cfg_str(mode)))
        {
            return mode;
        }
    }
    return -1;
}

/**
 * Convert a mode into a human understandable string
 *
 * @param mode   Mode to convert
 *
 * @return Easy to read string
 */
const char *bdk_qlm_mode_tostring(bdk_qlm_modes_t mode)
{
    const char *result = "Unknown, update bdk_qlm_mode_tostring()";
    switch (mode)
    {
        case BDK_QLM_MODE_DISABLED:
            result = "Disabled";
            break;
        case BDK_QLM_MODE_PCIE_1X1:
            result = "1 PCIe, 1 lane";
            break;
        case BDK_QLM_MODE_PCIE_2X1:
            result = "2 PCIe, 1 lane each";
            break;
        case BDK_QLM_MODE_PCIE_1X2:
            result = "1 PCIe, 2 lanes";
            break;
        case BDK_QLM_MODE_PCIE_1X4:
            result = "1 PCIe, 4 lanes";
            break;
        case BDK_QLM_MODE_PCIE_1X8:
            result = "1 PCIe, 8 lanes";
            break;
        case BDK_QLM_MODE_PCIE_1X16:
            result = "1 PCIe, 16 lanes";
            break;

        case BDK_QLM_MODE_SATA_4X1:
            result = "4 SATA, one lane each";
            break;
        case BDK_QLM_MODE_SATA_2X1:
            result = "2 SATA, one lane each";
            break;

        case BDK_QLM_MODE_ILK:
            result = "Interlaken";
            break;
        case BDK_QLM_MODE_SGMII_4X1:
            result = "4 SGMII, 1 lane each";
            break;
        case BDK_QLM_MODE_SGMII_2X1:
            result = "2 SGMII, 1 lane each";
            break;
        case BDK_QLM_MODE_SGMII_1X1:
            result = "1 SGMII, 1 lane";
            break;
        case BDK_QLM_MODE_XAUI_1X4:
            result = "1 XAUI, 4 lanes";
            break;
        case BDK_QLM_MODE_RXAUI_2X2:
            result = "2 RXAUI, 2 lanes each";
            break;
        case BDK_QLM_MODE_RXAUI_1X2:
            result = "1 RXAUI, 2 lanes each";
            break;
        case BDK_QLM_MODE_OCI:
            result = "Cavium Coherent Processor Interconnect";
            break;
        case BDK_QLM_MODE_XFI_4X1:
            result = "4 XFI, 1 lane each";
            break;
        case BDK_QLM_MODE_XFI_2X1:
            result = "2 XFI, 1 lane each";
            break;
        case BDK_QLM_MODE_XFI_1X1:
            result = "1 XFI, 1 lane";
            break;
        case BDK_QLM_MODE_XLAUI_1X4:
            result = "1 XLAUI, 4 lanes";
            break;
        case BDK_QLM_MODE_10G_KR_4X1:
            result = "4 10GBASE-KR, 1 lane each";
            break;
        case BDK_QLM_MODE_10G_KR_2X1:
            result = "2 10GBASE-KR, 1 lane each";
            break;
        case BDK_QLM_MODE_10G_KR_1X1:
            result = "1 10GBASE-KR, 1 lane";
            break;
        case BDK_QLM_MODE_40G_KR4_1X4:
            result = "1 40GBASE-KR4, 4 lanes";
            break;
        case BDK_QLM_MODE_QSGMII_4X1:
            result = "4 QSGMII, 1 lane";
            break;
        case BDK_QLM_MODE_25G_4X1:
            result = "4 25G, 1 lane each";
            break;
        case BDK_QLM_MODE_25G_2X1:
            result = "2 25G, 1 lane each";
            break;
        case BDK_QLM_MODE_50G_2X2:
            result = "2 50G, 2 lanes each";
            break;
        case BDK_QLM_MODE_50G_1X2:
            result = "1 50G, 2 lanes";
            break;
        case BDK_QLM_MODE_100G_1X4:
            result = "1 100G, 4 lanes";
            break;
        case BDK_QLM_MODE_25G_KR_4X1:
            result = "4 25G, 1 lane each";
            break;
        case BDK_QLM_MODE_25G_KR_2X1:
            result = "2 25G, 1 lane each";
            break;
        case BDK_QLM_MODE_50G_KR_2X2:
            result = "2 50G, 2 lanes each";
            break;
        case BDK_QLM_MODE_50G_KR_1X2:
            result = "1 50G, 2 lanes";
            break;
        case BDK_QLM_MODE_100G_KR4_1X4:
            result = "1 100G, 4 lanes";
            break;
        case BDK_QLM_MODE_USXGMII_4X1:
            result = "4 USXGMII, 1 lane each";
            break;
        case BDK_QLM_MODE_USXGMII_2X1:
            result = "2 USXGMII, 1 lane each";
            break;

        case BDK_QLM_MODE_LAST:
            break; /* fallthrough error */
    }
    return result;
}

int bdk_qlm_measure_clock(bdk_node_t node, int qlm)
{
    int ref_clock = __bdk_qlm_measure_refclock(node, qlm);
    BDK_TRACE(QLM, "N%d.QLM%d: Ref clock %d Hz\n", node, qlm, ref_clock);

    return ref_clock;
}

/**
 * Set the QLM's clock source.
 *
 * @param node     Node to use in a Numa setup
 * @param qlm      QLM to configure
 * @param clk      Clock source for QLM
 *
 * @return Zero on success, negative on failure
 */
int bdk_qlm_set_clock(bdk_node_t node, int qlm, bdk_qlm_clock_t clk)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
    {
        int sel;
        int com1;
        switch (clk)
        {
            case BDK_QLM_CLK_COMMON_0:
                sel = 1;
                com1 = 0;
                break;
            case BDK_QLM_CLK_COMMON_1:
                sel = 1;
                com1 = 1;
                break;
            case BDK_QLM_CLK_EXTERNAL:
                sel = 0;
                com1 = 0;
                break;
            default:
                bdk_warn("Unrecognized clock mode %d for QLM%d on node %d.\n",
                     clk, qlm, node);
                return -1;
        }

        BDK_CSR_MODIFY(c, node, BDK_GSERX_REFCLK_SEL(qlm),
            c.s.com_clk_sel = sel;
            c.s.use_com1 = com1);
    }
    else
    {
        int cclksel;
        switch (clk)
        {
            case BDK_QLM_CLK_COMMON_0:
                cclksel = 0;
                break;
            case BDK_QLM_CLK_COMMON_1:
                cclksel = 1;
                break;
            case BDK_QLM_CLK_COMMON_2:
                cclksel = 2;
                break;
            case BDK_QLM_CLK_EXTERNAL:
                cclksel = 3;
                break;
            default:
                bdk_warn("Unrecognized clock mode %d for QLM%d on node %d.\n",
                     clk, qlm, node);
                return -1;
        }
        BDK_CSR_MODIFY(c, node, BDK_GSERNX_COMMON_REFCLK_BCFG(qlm),
            c.s.pwdn = (clk == BDK_QLM_CLK_EXTERNAL) ? 0 : 1;
            c.s.cclksel = cclksel);
    }
    return 0;
}

/**
 * Display an eye diagram for the given QLM lane. The eye data can be in "eye", or
 * captured during the call if "eye" is NULL.
 *
 * @param node     Node to use in numa setup
 * @param qlm      QLM to use
 * @param qlm_lane Which lane
 * @param format   Display format. 0 = raw, 1 = Color ASCII
 * @param eye      Eye data to display, or NULL if the data should be captured.
 *
 * @return Zero on success, negative on failure
 */
int bdk_qlm_eye_display(bdk_node_t node, int qlm, int qlm_lane, int format, const bdk_qlm_eye_t *eye)
{
    int result;
    int need_free = 0;
    if (eye == NULL)
    {
        bdk_qlm_eye_t *eye_data = malloc(sizeof(bdk_qlm_eye_t));
        if (eye_data == NULL)
        {
            bdk_error("Failed to allocate space for eye\n");
            return -1;
        }
        if (bdk_qlm_eye_capture(node, qlm, qlm_lane, eye_data))
            return -1;
        eye = eye_data;
    }

    /* Calculate the max eye width */
    int eye_area = 0;
    int eye_width = 0;
    for (int y = 0; y < eye->height; y++)
    {
        int width = 0;
        for (int x = 0; x < eye->width; x++)
        {
            if (eye->data[y][x] == 0)
            {
                width++;
                eye_area++;
            }
        }
        if (width > eye_width)
            eye_width = width;
    }

    /* Calculate the max eye height */
    int eye_height = 0;
    for (int x = 0; x < eye->width; x++)
    {
        int height = 0;
        for (int y = 0; y < eye->height; y++)
        {
            if (eye->data[y][x] == 0)
            {
                height++;
                eye_area++;
            }
        }
        if (height > eye_height)
            eye_height = height;
    }

    printf("\nEye Diagram for Node %d, QLM %d, Lane %d\n", node, qlm, qlm_lane);

    if (format == 0) /* Raw */
    {
        for (int y = 0; y < eye->height; y++)
        {
            for (int x = 0; x < eye->width; x++)
                printf("%u\t", eye->data[y][x]);
            printf("\n");
        }
        result = 0;
    }
    else
        result = -1;

    if (need_free)
        free((void*)eye);
    return result;
}
