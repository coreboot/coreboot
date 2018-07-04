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
#include "libbdk-hal/if/bdk-if.h"
#include "libbdk-hal/bdk-qlm.h"
#include "libbdk-hal/bdk-utils.h"

/* This code is an optional part of the BDK. It is only linked in
    if BDK_REQUIRE() needs it */
BDK_REQUIRE_DEFINE(QLM_MARGIN);

typedef union
{
    struct
    {
        uint64_t rx_os_mvalbbd_2 :16;
        uint64_t rx_os_mvalbbd_1 :16;
        uint64_t reserved_63_32 :32;

    } s;
    struct
    {
        uint64_t Qb :6;
        uint64_t Q :6;
        uint64_t Lb :6; // Spans the two registers
        uint64_t L :6;
        uint64_t qerr0 :6;
        int64_t reserved_63_30 :34;
    } f;
    uint64_t u;
} rx_os_mvalbbd_t;

int __bdk_disable_ccpi_error_report = 0;

static int convert_to_signed_mag(int source)
{
    /* Synopsis encoded sign in an unexpected way. 0=negative and 1=positive
       So bit 5 should be 0 for negative numbers, 1 for positive numbers */
    if (source < 0)
        source = -source;
    else
        source |= 0x20;
    return source;
}

static rx_os_mvalbbd_t get_current_settings(bdk_node_t node, int qlm, int qlm_lane)
{
    rx_os_mvalbbd_t mvalbbd;
    mvalbbd.u = 0;

    BDK_CSR_INIT(rx_cfg_1, node, BDK_GSERX_LANEX_RX_CFG_1(qlm, qlm_lane));
    if (!rx_cfg_1.s.pcs_sds_rx_os_men)
    {
        /* Get the current settings */
        BDK_CSR_INIT(rx_os_out_1, node, BDK_GSERX_LANEX_RX_OS_OUT_1(qlm, qlm_lane));
        BDK_CSR_INIT(rx_os_out_2, node, BDK_GSERX_LANEX_RX_OS_OUT_2(qlm, qlm_lane));
        BDK_CSR_INIT(rx_os_out_3, node, BDK_GSERX_LANEX_RX_OS_OUT_3(qlm, qlm_lane));
        int qerr0 = bdk_extracts(rx_os_out_1.u, 0, 6);
        int lb = bdk_extracts(rx_os_out_2.u, 0, 6);
        int l = bdk_extracts(rx_os_out_2.u, 6, 6);
        int qb = bdk_extracts(rx_os_out_3.u, 0, 6);
        int q = bdk_extracts(rx_os_out_3.u, 6, 6);
        /* Enable the override with the current values */
        mvalbbd.f.Qb = convert_to_signed_mag(qb);
        mvalbbd.f.Q = convert_to_signed_mag(q);
        mvalbbd.f.Lb = convert_to_signed_mag(lb);
        mvalbbd.f.L = convert_to_signed_mag(l);
        mvalbbd.f.qerr0 = convert_to_signed_mag(qerr0);
    }
    else
    {
        BDK_CSR_INIT(mvalbbd_1, node, BDK_GSERX_LANEX_RX_OS_MVALBBD_1(qlm, qlm_lane));
        mvalbbd.s.rx_os_mvalbbd_1 = mvalbbd_1.s.pcs_sds_rx_os_mval;
        BDK_CSR_INIT(mvalbbd_2, node, BDK_GSERX_LANEX_RX_OS_MVALBBD_2(qlm, qlm_lane));
        mvalbbd.s.rx_os_mvalbbd_2 = mvalbbd_2.s.pcs_sds_rx_os_mval;
    }
    //printf("qerr0=%d, lb=%d, l=%d, qb=%d, q=%d\n",
    //    mvalbbd.f.qerr0, mvalbbd.f.Lb, mvalbbd.f.L, mvalbbd.f.Qb, mvalbbd.f.Q);
    return mvalbbd;
}

/**
 * Get the current RX margining parameter
 *
 * @param node     Node to read margin value from
 * @param qlm      QLM to read from
 * @param qlm_lane Lane to read
 * @param margin_type
 *                 Type of margining parameter to read
 *
 * @return Current margining parameter value
 */
int64_t bdk_qlm_margin_rx_get(bdk_node_t node, int qlm, int qlm_lane, bdk_qlm_margin_t margin_type)
{
    rx_os_mvalbbd_t mvalbbd = get_current_settings(node, qlm, qlm_lane);

    switch (margin_type)
    {
        case BDK_QLM_MARGIN_VERTICAL:
            if (mvalbbd.f.Q & 0x20) /* Check if sign bit says positive */
                return mvalbbd.f.Q & 0x1f; /* positive, strip off sign */
            else
                return -mvalbbd.f.Q; /* negative */
        case BDK_QLM_MARGIN_HORIZONTAL:
            return 0;
    }
    return 0;
}

/**
 * Get the current RX margining parameter minimum value
 *
 * @param node     Node to read margin value from
 * @param qlm      QLM to read from
 * @param qlm_lane Lane to read
 * @param margin_type
 *                 Type of margining parameter to read
 *
 * @return Current margining parameter minimum value
 */
int64_t bdk_qlm_margin_rx_get_min(bdk_node_t node, int qlm, int qlm_lane, bdk_qlm_margin_t margin_type)
{
    switch (margin_type)
    {
        case BDK_QLM_MARGIN_VERTICAL:
            return -31;
        case BDK_QLM_MARGIN_HORIZONTAL:
            return 0;
    }
    return 0;
}

/**
 * Get the current RX margining parameter maximum value
 *
 * @param node     Node to read margin value from
 * @param qlm      QLM to read from
 * @param qlm_lane Lane to read
 * @param margin_type
 *                 Type of margining parameter to read
 *
 * @return Current margining parameter maximum value
 */
int64_t bdk_qlm_margin_rx_get_max(bdk_node_t node, int qlm, int qlm_lane, bdk_qlm_margin_t margin_type)
{
    switch (margin_type)
    {
        case BDK_QLM_MARGIN_VERTICAL:
            return 31;
        case BDK_QLM_MARGIN_HORIZONTAL:
            return 0;
    }
    return 0;
}

/**
 * Set the current RX margining parameter value
 *
 * @param node     Node to set margin value on
 * @param qlm      QLM to set
 * @param qlm_lane Lane to set
 * @param margin_type
 *                 Type of margining parameter to set
 * @param value    Value of margining parameter
 *
 * @return Zero on success, negative on failure
 */
int bdk_qlm_margin_rx_set(bdk_node_t node, int qlm, int qlm_lane, bdk_qlm_margin_t margin_type, int value)
{
    rx_os_mvalbbd_t mvalbbd = get_current_settings(node, qlm, qlm_lane);

    switch (margin_type)
    {
        case BDK_QLM_MARGIN_VERTICAL:
            if (value < 0)
                mvalbbd.f.Q = -value; /* Sign bit is zero, weird Synopsys */
            else
                mvalbbd.f.Q = value | 0x20; /* Sign bit is one, weird Synopsys */
            break;
        case BDK_QLM_MARGIN_HORIZONTAL:
            return -1;
    }

    BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_RX_OS_MVALBBD_1(qlm, qlm_lane),
        c.s.pcs_sds_rx_os_mval = mvalbbd.s.rx_os_mvalbbd_1);
    BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_RX_OS_MVALBBD_2(qlm, qlm_lane),
        c.s.pcs_sds_rx_os_mval = mvalbbd.s.rx_os_mvalbbd_2);
    BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_RX_CFG_1(qlm, qlm_lane),
        c.s.pcs_sds_rx_os_men = 1);

    /* Disable the DFE(s), gives a better eye measurement */
    BDK_CSR_INIT(pwr_ctrl, node, BDK_GSERX_LANEX_PWR_CTRL(qlm, qlm_lane));
    if (!pwr_ctrl.s.rx_lctrl_ovrrd_en)
    {
        BDK_CSR_WRITE(node, BDK_GSERX_LANEX_RX_LOOP_CTRL(qlm, qlm_lane), 0xF1);
        BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PWR_CTRL(qlm, qlm_lane),
             c.s.rx_lctrl_ovrrd_en =  1);
    }

    if (qlm >= 8)
        __bdk_disable_ccpi_error_report = 1;

    return 0;
}

/**
 * Restore the supplied RX margining parameter value as if it was never set. This
 * disables any overrides in the SERDES need to perform margining
 *
 * @param node     Node to restore margin value on
 * @param qlm      QLM to restore
 * @param qlm_lane Lane to restore
 * @param margin_type
 *                 Type of margining parameter to restore
 * @param value    Value of margining parameter
 *
 * @return Zero on success, negative on failure
 */
int bdk_qlm_margin_rx_restore(bdk_node_t node, int qlm, int qlm_lane, bdk_qlm_margin_t margin_type, int value)
{
    BDK_CSR_INIT(rx_cfg_1, node, BDK_GSERX_LANEX_RX_CFG_1(qlm, qlm_lane));
    /* Return if no overrides have been applied */
    if (!rx_cfg_1.s.pcs_sds_rx_os_men)
        return 0;
    bdk_qlm_margin_rx_set(node, qlm, qlm_lane, margin_type, value);
    BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_RX_CFG_1(qlm, qlm_lane),
        c.s.pcs_sds_rx_os_men = 0);
    /* Enable the DFE(s) */
    BDK_CSR_MODIFY(c, node, BDK_GSERX_LANEX_PWR_CTRL(qlm, qlm_lane),
         c.s.rx_lctrl_ovrrd_en =  0);
    __bdk_disable_ccpi_error_report = 0;
    return 0;
}

