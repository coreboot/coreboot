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
#include <libbdk-hal/if/bdk-if.h>
#include <libbdk-hal/bdk-mdio.h>
#include <libbdk-hal/bdk-qlm.h>

#define VSC_PHY_STD_PAGE     (0x0)
#define VSC_PHY_EXT1_PAGE    (0x1)
#define VSC_PHY_EXT2_PAGE    (0x2)
#define VSC_PHY_EXT3_PAGE    (0x3)
#define VSC_PHY_EXT4_PAGE    (0x4)
#define VSC_PHY_GPIO_PAGE    (0x10)
#define VSC_PHY_TEST_PAGE    (0x2A30)
#define VSC_PHY_TR_PAGE      (0x52B5)

const uint16_t init_script_rev_a[] = {
//     Op,   Page,  Reg, Value,  Mask
//      0,      1,    2,     3,     4
//     --, ------, ----, ------, -----
        0, 0x0000, 0x1f, 0x0000, 0xffff,
        1, 0x0000, 0x16, 0x0001, 0x0001,
        0, 0x0001, 0x1f, 0x2A30, 0xffff,
        1, 0x2A30, 0x08, 0x8000, 0x8000,
        0, 0x2A30, 0x1f, 0x52B5, 0xffff,
        0, 0x52B5, 0x12, 0x0068, 0xffff,
        0, 0x52B5, 0x11, 0x8980, 0xffff,
        0, 0x52B5, 0x10, 0x8f90, 0xffff,
        0, 0x52B5, 0x12, 0x0000, 0xffff,
        0, 0x52B5, 0x11, 0x0003, 0xffff,
        0, 0x52B5, 0x10, 0x8796, 0xffff,
        0, 0x52B5, 0x12, 0x0050, 0xffff,
        0, 0x52B5, 0x11, 0x100f, 0xffff,
        0, 0x52B5, 0x10, 0x87fa, 0xffff,
        0, 0x52B5, 0x1f, 0x2A30, 0xffff,
        1, 0x2A30, 0x08, 0x0000, 0x8000,
        0, 0x2A30, 0x1f, 0x0000, 0xffff,
        1, 0x0000, 0x16, 0x0000, 0x0001,
      0xf, 0xffff, 0xff, 0xffff, 0xffff
};

static void wr_masked(bdk_node_t node, int mdio_bus, int phy_addr, int reg, int value, int mask)
{
    int nmask = ~mask;
    int old = bdk_mdio_read(node, mdio_bus, phy_addr, reg);
    int vmask = value & mask;
    int newv = old & nmask;
    newv = newv | vmask;
    bdk_mdio_write(node, mdio_bus, phy_addr, reg, newv);
}
static void vitesse_init_script(bdk_node_t node, int mdio_bus, int phy_addr)
{
    const uint16_t    *ptr;
    uint16_t    reg_addr;
    uint16_t    reg_val;
    uint16_t    mask;
    
    BDK_TRACE(PHY,"In %s\n",__FUNCTION__);
    BDK_TRACE(PHY,"Loading init script for VSC8514\n");

    ptr = init_script_rev_a;
    while (*ptr != 0xf)
    {
        reg_addr = *(ptr+2);
        reg_val = *(ptr+3);
        mask = *(ptr+4);
        ptr+=5;
        if (mask != 0xffff)
        {
            wr_masked(node, mdio_bus, phy_addr, reg_addr,reg_val,mask);
        }
        else
        {
            bdk_mdio_write(node,mdio_bus,phy_addr,reg_addr,reg_val);
        }
    }

    BDK_TRACE(PHY,"loading init script is done\n");

}

static void vitesse_program(bdk_node_t node, int mdio_bus, int phy_addr)
{
    return;
}

/**
 * Setup Vitesse PHYs
 * This function sets up one port in a Vitesse VSC8514 
 */
static void setup_vitesse_phy(bdk_node_t node, int mdio_bus, int phy_addr)
{
    /*setting MAC if*/
    bdk_mdio_write(node, mdio_bus, phy_addr, 31, VSC_PHY_GPIO_PAGE);
    wr_masked(node,mdio_bus,phy_addr, 19, 0x4000, 0xc000); 
    bdk_mdio_write(node, mdio_bus, phy_addr, 18, 0x80e0);

    /*Setting media if*/
    bdk_mdio_write(node, mdio_bus, phy_addr, 31, VSC_PHY_STD_PAGE);
    // Reg23, 10:8 Select copper,  CAT5 copper only
    wr_masked(node,mdio_bus,phy_addr, 23, 0x0000, 0x0700);
    
    // Reg0:15, soft Reset
    wr_masked(node,mdio_bus,phy_addr, 0, 0x8000, 0x8000);
    int time_out = 100;
    while (time_out && bdk_mdio_read(node,mdio_bus,phy_addr, 0) & 0x8000)
    {
        bdk_wait_usec(100000);
        time_out--;
    }

    if (time_out == 0)
    {
        BDK_TRACE(PHY,"setting PHY TIME OUT\n");
        return;
    }
    else
    {
        BDK_TRACE(PHY,"Setting a phy port is done\n");
    }

    bdk_mdio_write(node, mdio_bus, phy_addr, 31, VSC_PHY_EXT3_PAGE);
    bdk_mdio_write(node, mdio_bus, phy_addr, 16, 0x80);
    // Select main registers
    bdk_mdio_write(node, mdio_bus, phy_addr, 31, VSC_PHY_STD_PAGE);
    
  /*
  
    if (LOOP_INTERNAL)
    {
        reg0 = bdk_mdio_read(node, mdio_bus, phy_addr, 0);
        reg0 = bdk_insert(reg0, 1, 14, 1);
        bdk_mdio_write(node, mdio_bus, phy_addr, 0, reg0);
    }

    // Far end loopback (External side)
    if (LOOP_EXTERNAL)
    {
        reg23 = bdk_mdio_read(node, mdio_bus, phy_addr, 23);
        reg23 = bdk_insert(reg23, 1, 3, 1);
        bdk_mdio_write(node, mdio_bus, phy_addr, 23, reg23);
    }
    

    // Dump registers
    if (false)
    {
        printf("\nVitesse PHY register dump, PHY address %d, mode %s\n",
               phy_addr, (qsgmii) ? "QSGMII" : "SGMII");
        int phy_addr = 4;
        for (int reg_set = 0; reg_set <= 0x10; reg_set += 0x10)
        {
            printf("\nDump registers with reg[31]=0x%x\n", reg_set);
            bdk_mdio_write(node, mdio_bus, phy_addr, 31, reg_set);
            for (int reg=0; reg < 32; reg++)
                printf("reg[%02d]=0x%x\n", reg, bdk_mdio_read(node, mdio_bus, phy_addr, reg));
        }
    }
    */
}

//static void vetesse_setup(bdk_node_t node, int qlm, int mdio_bus, int phy_addr)
int bdk_if_phy_vsc8514_setup(bdk_node_t node, int qlm, int mdio_bus, int phy_addr)
{
    /* Check if the PHY is Vetesse PHY we expect */
    int phy_status_1 = bdk_mdio_read(node, mdio_bus, phy_addr, BDK_MDIO_PHY_REG_ID1);
    int phy_status_2 = bdk_mdio_read(node, mdio_bus, phy_addr, BDK_MDIO_PHY_REG_ID2);
    if (phy_status_1 != 0x0007 || phy_status_2 != 0x0670)
    {
         bdk_error("The PHY on this board is NOT VSC8514.\n");
         return -1;
    }

    /* Check that the GSER mode is SGMII or QSGMII */
    bdk_qlm_modes_t qlm_mode = bdk_qlm_get_mode(node, qlm);
    if (qlm_mode != BDK_QLM_MODE_QSGMII_4X1)
        return -1;

    vitesse_init_script(node, mdio_bus, phy_addr);
    vitesse_program(node, mdio_bus, phy_addr);

    /* VSC8514 just support QSGMII */
    for (int port = 0; port < 4; port++)
        setup_vitesse_phy(node, mdio_bus, phy_addr + port);

    return 1;

}
