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
#include "libbdk-hal/if/bdk-if.h"
#include "libbdk-hal/bdk-qlm.h"
#include "libbdk-hal/bdk-utils.h"
#include "libbdk-boot/bdk-boot-qlm.h"
#include "libbdk-hal/bdk-config.h"
#include "libbdk-hal/bdk-twsi.h"

static void boot_init_qlm_clk(void)
{
    /* Setup reference clocks */
    for (bdk_node_t n = BDK_NODE_0; n < BDK_NUMA_MAX_NODES; n++)
    {
        if (!bdk_numa_exists(n))
            continue;

        int num_qlms = bdk_qlm_get_num(n);

        BDK_TRACE(INIT, "Initializing QLM clocks on Node %d\n", n);
        for (int qlm = 0; qlm < num_qlms; qlm++)
        {
            bdk_qlm_clock_t clk = bdk_config_get_int(BDK_CONFIG_QLM_CLK, n, qlm);
            if (BDK_QLM_CLK_LAST == clk) /* no entry */
                continue;

            if (clk > BDK_QLM_CLK_LAST)
            {
                bdk_warn("Invalid clock source %d for QLM%d on node %d. Not configuring.\n",
                         clk, qlm, n);
                continue;
            }

            if (0 != bdk_qlm_set_clock(n, qlm, clk))
            {
                bdk_error("Error setting clock source %d for QLM%d on node %d. Ignoring.\n",
                          clk, qlm, n);
            }
        }
    }
}

/**
 * Given a node and DLM/QLM, return the possible BGX lanes connected to it. This
 * is needed to determine which PHY address to use for SFP/SFP+ detection.
 *
 * @param node   Node the DLM/QLM is on
 * @param qlm    DLM/QLM to find the BGX for
 * @param bgx    Output: The BGX instance number, or -1 on failure
 * @param bgx_lane_mask
 *               Output: Which BGX indexes may be connected to this port
 */
static void find_bgx(int node, int qlm, int *bgx, int *bgx_lane_mask)
{
    *bgx = -1;
    *bgx_lane_mask = 0;

    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
    {
        switch (qlm)
        {
            case 0: /* BGX0 -> QLM0 */
            case 1: /* BGX1 -> QLM1 */
                *bgx = qlm;
                *bgx_lane_mask = 0xf;
                return;
            default:
                BDK_TRACE(INIT, "N%d.QLM%d: No BGX for this QLM, illegal config\n", node, qlm);
                return;
        }
    }
    else if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
    {
        switch (qlm)
        {
            case 2: /* BGX0 -> QLM2 */
                *bgx = 0;
                *bgx_lane_mask = 0xf;
                return;
            case 3: /* BGX1 -> QLM3 */
                *bgx = 1;
                *bgx_lane_mask = 0xf;
                return;
            case 4: /* BGX3 -> DLM4 */
                *bgx = 3;
                *bgx_lane_mask = 0x3;
                return;
            case 5: /* BGX2 -> DLM5 */
                *bgx = 2;
                *bgx_lane_mask = 0x3;
                return;
            case 6: /* BGX2 -> DLM6 */
                *bgx = 2;
                *bgx_lane_mask = 0xc;
                return;
            default:
                BDK_TRACE(INIT, "N%d.QLM%d: No BGX for this QLM, illegal config\n", node, qlm);
                return;
        }
    }
    else if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
    {
        switch (qlm)
        {
            case 0: /* BGX0 -> DLM0 */
                *bgx = 0;
                *bgx_lane_mask = 0x3;
                return;
            case 1: /* BGX0 -> DLM1 */
                *bgx = 0;
                *bgx_lane_mask = 0xc;
                return;
            case 2: /* BGX1 -> DLM2 */
                *bgx = 1;
                *bgx_lane_mask = 0x3;
                return;
            case 3: /* BGX1 -> DLM3 */
                *bgx = 1;
                *bgx_lane_mask = 0xc;
                return;
            default:
                BDK_TRACE(INIT, "N%d.QLM%d: No BGX for this QLM, illegal config\n", node, qlm);
                return;
        }
    }
    else
        bdk_error("N%d.QLM%d: Unsupported chip, update %s()\n", node, qlm, __FUNCTION__);
}

/**
 * Determine the DLM/QLM mode based on a SFP/SFP+ connected to the port. Note that
 * the CN8XXX parts can't control mode per lane, so all SFP/SFP+ on a DLM/QLM must
 * be the same mode. This code is sloppy about finding the BGX PHY for the DLM/QLM
 * because not all lanes may be used.
 *
 * @param node   Node to determine mode for
 * @param qlm    DLM/QLM the SFP/SFP+ is connected to
 *
 * @return QLM mode or -1 on failure
 */
static int init_sfp(int node, int qlm)
{
    int mode = BDK_QLM_MODE_XFI_4X1; /* Default to XFI if detection fails */
    int bgx = -1;
    int bgx_lane_mask = 0;

    find_bgx(node, qlm, &bgx, &bgx_lane_mask);
    if (bgx == -1)
        return mode;

    BDK_TRACE(INIT, "N%d.QLM%d: Checking for SFP/SFP+\n", node, qlm);

    for (int index = 0; index < 4; index++)
    {
        /* Skip BGX indexes that aren't applicable */
        if ((bgx_lane_mask & (1 << index)) == 0)
            continue;
        /* Lookup the PHY address for this BGX port */
        int phy_addr = bdk_config_get_int(BDK_CONFIG_PHY_ADDRESS, node, bgx, index);
        /* SFP/SFP+ are connected with TWSI, so only check ports with
           PHYs connected with TWSI */
        if ((phy_addr & BDK_IF_PHY_TYPE_MASK) != BDK_IF_PHY_TWSI)
            continue;

        /* For TWSI:
            Bits[31:24]: Node ID, 0xff for device node
            Bits[23:16]: TWSI internal address width in bytes (0-2)
            Bits[15:12]: 2=TWSI
            Bits[11:8]: TWSI bus number
            Bits[7:0]: TWSI address */
        int n = (phy_addr >> 24) & 0xff;
        int twsi_ia_width = (phy_addr >> 16) & 0xff;
        int twsi_bus = (phy_addr >> 8) & 0xf;
        int twsi_addr = 0x50; /* From SFP spec */
        if (n == 0xff)
            n = node;

        /* Read bytes 0-3 from eeprom. Note read is big endian, so byte 0 is
           bits 31:24 in the result */
        int64_t eeprom_00_03 = bdk_twsix_read_ia(n, twsi_bus, twsi_addr, 0, 4, twsi_ia_width);
        if (eeprom_00_03 == -1)
        {
            BDK_TRACE(INIT, "N%d.QLM%d: BGX%d.%d SFP/SFP+ eeprom access failed\n", node, qlm, bgx, index);
            continue;
        }
        int64_t eeprom_04_07 = bdk_twsix_read_ia(n, twsi_bus, twsi_addr, 4, 4, twsi_ia_width);
        if (eeprom_04_07 == -1)
        {
            BDK_TRACE(INIT, "N%d.QLM%d: BGX%d.%d SFP/SFP+ eeprom access failed\n", node, qlm, bgx, index);
            continue;
        }
        int64_t eeprom_08_11 = bdk_twsix_read_ia(n, twsi_bus, twsi_addr, 8, 4, twsi_ia_width);
        if (eeprom_08_11 == -1)
        {
            BDK_TRACE(INIT, "N%d.QLM%d: BGX%d.%d SFP/SFP+ eeprom access failed\n", node, qlm, bgx, index);
            continue;
        }
        int64_t eeprom_12 = bdk_twsix_read_ia(n, twsi_bus, twsi_addr, 12, 1, twsi_ia_width);
        if (eeprom_12 == -1)
        {
            BDK_TRACE(INIT, "N%d.QLM%d: BGX%d.%d SFP/SFP+ eeprom access failed\n", node, qlm, bgx, index);
            continue;
        }

        /* Byte 0: Identifier, should be 0x03 for SFP/SFP+
                    0x03 = SFP of SFP+
                    0x0c = QSFP
                    0x0d = QSFP+ */
        if (bdk_extract(eeprom_00_03, 24, 8) != 0x03)
        {
            /* Byte 0 of eeprom should be 0x03 for SFP/SFP+ */
            BDK_TRACE(INIT, "N%d.QLM%d: BGX%d.%d SFP/SFP+ not detected\n", node, qlm, bgx, index);
            continue;
        }
        /* Byte 1: Extended Identifier, should be 0x04 */
        if (bdk_extract(eeprom_00_03, 16, 8) != 0x04)
        {
            BDK_TRACE(INIT, "N%d.QLM%d: BGX%d.%d SFP/SFP+ incorrect extended identifier\n", node, qlm, bgx, index);
            continue;
        }
        /* Byte 2: Connector
                    Value   Description of connector
                    00h     Unknown or unspecified
                    01h     SC
                    02h     Fibre Channel Style 1 copper connector
                    03h     Fibre Channel Style 2 copper connector
                    04h     BNC/TNC
                    05h     Fibre Channel coaxial headers
                    06h     FiberJack
                    07h     LC
                    08h     MT-RJ
                    09h     MU
                    0Ah     SG
                    0Bh     Optical pigtail
                    0Ch     MPO Parallel Optic
                    0Dh-1Fh Reserved, Unallocated
                    20h     HSSDC II
                    21h     Copper Pigtail
                    22h     RJ45
                    23h-7Fh Reserved, Unallocated
                    80-FFh Vendor specific */
        bool isOptical = false;
        switch (bdk_extract(eeprom_00_03, 8, 8))
        {
            case 0x01: /* SC - Short channel */
            case 0x07: /* LC - Long channel */
            case 0x0B: /* Optical pigtail */
                isOptical = true;
                break;
        }
        BDK_TRACE(INIT, "N%d.QLM%d: SFP/SFP+ eeprom Bytes[0:3] 0x%0llx, Bytes[4:7] 0x%08llx, [8:11] 0x%08llx [12] 0x%02llx\n",
            node, qlm, eeprom_00_03, eeprom_04_07, eeprom_08_11, eeprom_12);
        /* Byte 3: Transceiver info first byte. See comments below */
        /* Byte 3, bits 4-7 correspond to 10G Ethernet speeds */
        /* 10G Ethernet Compliance Codes
                Byte 3[7] 10G BASE-ER (Fiber - Extended Reach)
                Byte 3[6] 10G BASE-LRM (Fiber - Long reach multi-mode)
                Byte 3[5] 10G BASE-LR (Fiber - Long reach)
                Byte 3[4] 10G BASE-SR (Fiber - Short reach) */
        bool isXFI = bdk_extract(eeprom_00_03, 0, 8) != 0;
        /* Byte 6, bits 0-7 correspond to Gigabit Ethernet speeds */
        /* Gigabit Ethernet Compliance Codes
                Byte 6[7] BASE-PX
                Byte 6[6] BASE-BX10
                Byte 6[5] 100BASE-FX
                Byte 6[4] 100BASE-LX/LX10 (Fiber)
                Byte 6[3] 1000BASE-T (Twisted pair)
                Byte 6[2] 1000BASE-CX (Shielded balanced copper)
                Byte 6[1] 1000BASE-LX (Fiber)
                Byte 6[0] 1000BASE-SX (Fiber) */
        bool isSGMII = bdk_extract(eeprom_04_07, 8, 8) != 0;
        /* Byte 12 is the nominal bit rate, units of 100 MBits/sec. */
        int bit_rate = eeprom_12 * 100;
        if (bit_rate)
        {
            BDK_TRACE(INIT, "N%d.QLM%d: Nominal bit rate %d  MBits/sec\n",
                node, qlm, bit_rate);
            isXFI = (bit_rate >= 10000);
            isSGMII = (bit_rate <= 2500);
        }

        if (isXFI)
        {
            mode = BDK_QLM_MODE_XFI_4X1;
            if (isOptical)
                BDK_TRACE(INIT, "N%d.QLM%d: SFP+ selecting XFI Optical\n", node, qlm);
            else
                BDK_TRACE(INIT, "N%d.QLM%d: SFP+ selecting XFI Copper\n", node, qlm);
        }
        else if (isSGMII)
        {
            mode = BDK_QLM_MODE_SGMII_4X1;
            if (isOptical)
            {
                /* This should be 1000BASE-X, gigabit over fiber */
                BDK_TRACE(INIT, "N%d.QLM%d: SFP selecting SGMII Optical\n", node, qlm);
            }
            else /* This should be SGMII, gigabit over copper */
                BDK_TRACE(INIT, "N%d.QLM%d: SFP selecting SGMII Copper\n", node, qlm);
        }
    }
    return mode;
}

/**
 * Determine the DLM/QLM mode based on a QSFP/QSFP+ connected to
 * the port. This code is sloppy about finding the BGX PHY for
 * the DLM/QLM because not all lanes may be used.
 *
 * @param node   Node to determine mode for
 * @param qlm    DLM/QLM the SFP/SFP+ is connected to
 *
 * @return QLM mode or -1 on failure
 */
static int init_qsfp(int node, int qlm)
{
    int mode = BDK_QLM_MODE_XLAUI_1X4; /* Default to XLAUI if detection fails */
    int bgx = -1;
    int bgx_lane_mask = 0;

    find_bgx(node, qlm, &bgx, &bgx_lane_mask);
    if (bgx == -1)
        return mode;

    BDK_TRACE(INIT, "N%d.QLM%d: Checking for QSFP/QSFP+\n", node, qlm);
    int index = 0;

    /* Lookup the PHY address for this BGX port */
    int phy_addr = bdk_config_get_int(BDK_CONFIG_PHY_ADDRESS, node, bgx, index);
    /* QSFP/QSFP+ are connected with TWSI, so only check ports with
       PHYs connected with TWSI */
    if ((phy_addr & BDK_IF_PHY_TYPE_MASK) != BDK_IF_PHY_TWSI)
        return mode;

    /* For TWSI:
        Bits[31:24]: Node ID, 0xff for device node
        Bits[23:16]: TWSI internal address width in bytes (0-2)
        Bits[15:12]: 2=TWSI
        Bits[11:8]: TWSI bus number
        Bits[7:0]: TWSI address */
    int n = (phy_addr >> 24) & 0xff;
    int twsi_ia_width = (phy_addr >> 16) & 0xff;
    int twsi_bus = (phy_addr >> 8) & 0xf;
    int twsi_addr = 0x50; /* From SFP spec */
    if (n == 0xff)
        n = node;

    /* Byte 0: Identifier, should be 0x03 for SFP/SFP+
                0x03 = SFP of SFP+
                0x0c = QSFP
                0x0d = QSFP+ */
    int64_t eeprom_00 = bdk_twsix_read_ia(n, twsi_bus, twsi_addr, 0, 1, twsi_ia_width);
    switch (eeprom_00)
    {
        case 0x03:
            BDK_TRACE(INIT, "N%d.QLM%d: BGX%d QSFP/QSFP+ contains a SFP+\n", node, qlm, bgx);
            mode = init_sfp(node, qlm);
            break;
        case 0x0c:
        case 0x0d:
            BDK_TRACE(INIT, "N%d.QLM%d: BGX%d Found a QSFP/QSFP+, assuming 40G\n", node, qlm, bgx);
            mode = BDK_QLM_MODE_XLAUI_1X4;
            break;
        default:
            BDK_TRACE(INIT, "N%d.QLM%d: BGX%d QSFP/QSFP+ not detected\n", node, qlm, bgx);
            break;
    }
    return mode;
}

static void boot_init_qlm_mode(void)
{
    /* Check if QLM autoconfig is requested */
    int qlm_auto = bdk_config_get_int(BDK_CONFIG_QLM_AUTO_CONFIG);
    if (qlm_auto)
    {
        /* Auto configuration of QLMs
         */
        for (bdk_node_t n = BDK_NODE_0; n < BDK_NUMA_MAX_NODES; n++)
        {
            if (bdk_numa_exists(n))
            {
                BDK_TRACE(INIT, "Initializing QLMs on Node %d\n", n);
                bdk_qlm_auto_config(n);
            }
        }
    }
   /* 
    * Check if QLM autoconfig from DIP switch settings is requested
    */
    else if (bdk_config_get_int(BDK_CONFIG_QLM_DIP_AUTO_CONFIG))
    {
        BDK_TRACE(INIT, "Reading DIP Switch settings for QLM Auto configuration\n");

        /* Auto configuration of QLMs
         */
        for (bdk_node_t n = BDK_NODE_0; n < BDK_NUMA_MAX_NODES; n++)
        {
            if (bdk_numa_exists(n))
            {
                BDK_TRACE(INIT, "Initializing QLMs on Node %d\n", n);
                if (bdk_qlm_dip_auto_config(n))
                    bdk_error("QLM Auto configuration failed!\n");
            }
        }

    }
    else
    {
        /* Initialize the QLMs based on configuration file settings
         */

        boot_init_qlm_clk();

        for (bdk_node_t n = BDK_NODE_0; n < BDK_NUMA_MAX_NODES; n++)
        {
            if (!bdk_numa_exists(n))
                continue;

            int num_qlms = bdk_qlm_get_num(n);

            BDK_TRACE(INIT, "Initializing QLMs on Node %d\n", n);
            for (int qlm = 0; qlm < num_qlms; qlm++)
            {
                const char *cfg_val;

                cfg_val = bdk_config_get_str(BDK_CONFIG_QLM_MODE, n, qlm);
                if (!cfg_val)
                    continue;

                int mode;
                int freq;
                /* Check for special token telling us to configure the QLM
                   based on the SFP/SFP+/QSFP/QSFP+ plugged into the system. */
                if ((strcmp(cfg_val, "SFP+") == 0) || (strcmp(cfg_val, "QSFP+") == 0))
                {
                    if (strcmp(cfg_val, "SFP+") == 0)
                        mode = init_sfp(n, qlm);
                    else
                        mode = init_qsfp(n, qlm);

                    if (mode == BDK_QLM_MODE_SGMII_4X1)
                        freq = 1250;
                    else
                        freq = 10321;
                }
                else
                {
                    mode = bdk_qlm_cfg_string_to_mode(cfg_val);
                    freq = bdk_config_get_int(BDK_CONFIG_QLM_FREQ, n, qlm);
                }
                if (-1 == mode)
                {
                    bdk_error("Invalid QLM mode string '%s' for QLM%d on node %d. "
                                "Not configuring.\n", cfg_val, qlm, n);
                    continue;
                }
                if (-1 == freq)
                {
                    bdk_error("No frequency setting for QLM%d on node %d. "
                                "Not configuring.\n", qlm, n);
                    continue;
                }

                bdk_qlm_set_mode(n, qlm, mode, freq, 0);
            }
        }
    }
}

/**
 * Configure QLM on all nodes as part of booting
 */
void bdk_boot_qlm(void)
{
    boot_init_qlm_mode();
}
