/***********************license start***********************************
* Copyright (c) 2003-2016  Cavium Inc. (support@cavium.com). All rights
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
#include <libbdk-hal/bdk-mdio.h>
#include <libbdk-hal/bdk-qlm.h>
#include <libbdk-hal/if/bdk-if.h>

/**
 * Setup marvell PHYs
 * This function sets up one port in a marvell 88E1512 in SGMII mode
 */
static void setup_marvell_phy(bdk_node_t node, int mdio_bus, int mdio_addr)
{
      int phy_status = 0;

           BDK_TRACE(PHY, "%s In SGMII mode for Marvell PHY 88E1512\n", __FUNCTION__);
          /* Switch to Page 18 */
            phy_status = bdk_mdio_write(node, mdio_bus, mdio_addr, 22, 18);
          if (phy_status < 0)
                       return;

          phy_status = bdk_mdio_read(node, mdio_bus, mdio_addr, 22);
          if (phy_status < 0)
                       return;

          /* Change the Phy System mode from RGMII(default hw reset mode) to SGMII */
          phy_status = bdk_mdio_write(node, mdio_bus, mdio_addr, 20, 1);
          if (phy_status < 0)
                return;

          /* Requires a Software reset */
          phy_status = bdk_mdio_write(node, mdio_bus, mdio_addr, 20, 0x8001);
          if (phy_status < 0)
                return;

          phy_status = bdk_mdio_read(node, mdio_bus, mdio_addr, 20);
          if (phy_status < 0)
                       return;

          /* Change the Page back to 0 */
            phy_status = bdk_mdio_write(node, mdio_bus, mdio_addr, 22, 0);
          if (phy_status < 0)
                       return;

          phy_status = bdk_mdio_read(node, mdio_bus, mdio_addr, 22);
          if (phy_status < 0)
                       return;
      
      phy_status = bdk_mdio_read(node, mdio_bus, mdio_addr, 17);
      if (phy_status < 0)
           return;
}

int bdk_if_phy_marvell_setup(bdk_node_t node, int qlm, int mdio_bus, int phy_addr)
{
    BDK_TRACE(PHY,"In %s\n",__FUNCTION__);

    /* Check if the PHY is marvell PHY we expect */
    int phy_status = bdk_mdio_read(node, mdio_bus, phy_addr, BDK_MDIO_PHY_REG_ID1);
    if (phy_status != 0x0141)
        return 0;

    /* Check that the GSER mode is SGMII */
    /* Switch the marvell PHY to the correct mode */
    bdk_qlm_modes_t qlm_mode = bdk_qlm_get_mode(node, qlm);
    
    BDK_TRACE(PHY,"%s: QLM:%d QLM_MODE:%d\n",__FUNCTION__, qlm, qlm_mode);
    
    if ((qlm_mode != BDK_QLM_MODE_SGMII_1X1) &&
        (qlm_mode != BDK_QLM_MODE_SGMII_2X1))
        return 0;

    BDK_TRACE(PHY,"%s: Detected Marvell Phy in SGMII mode\n", __FUNCTION__);    
    for (int port = 0; port < 2; port++)
    {
        setup_marvell_phy(node, mdio_bus, phy_addr + port);
    }
    return 0;
}
