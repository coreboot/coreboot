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
#include <libbdk-hal/if/bdk-if.h>
#include <libbdk-hal/bdk-config.h>
#include <libbdk-hal/bdk-mdio.h>
#include <libbdk-hal/bdk-qlm.h>
#include <libbdk-hal/bdk-twsi.h>

/**
 * Called when the PHY is connected through TWSI
 *
 * @param dev_node Node the ethernet device is on
 * @param phy_addr Encoded address, see bdk-if.h for format
 *
 * @return Link status
 */
static bdk_if_link_t __bdk_if_phy_get_twsi(bdk_node_t dev_node, int phy_addr)
{
    /* For TWSI:
        Bits[31:24]: Node ID, 0xff for device node
        Bits[23:16]: TWSI internal address width in bytes (0-2)
        Bits[15:12]: 2=TWSI
        Bits[11:8]: TWSI bus number
        Bits[7:0]: TWSI address */
    int node = (phy_addr >> 24) & 0xff;
    int twsi_ia_width = (phy_addr >> 16) & 0xff;
    int twsi_bus = (phy_addr >> 8) & 0xf;
    int twsi_addr = phy_addr & 0xff;
    if (node == 0xff)
        node = dev_node;

    bdk_if_link_t result;
    result.u64 = 0;

    /* This is from the Avago SFP 1G Module data sheet
       Register 17 (Extended Status 1) */
    int64_t phy_status = bdk_twsix_read_ia(node, twsi_bus, twsi_addr, 17, 2, twsi_ia_width);
    if (phy_status != -1)
    {
        int speed = (phy_status >> 14)& 3;
        int duplex = (phy_status >> 13)& 1;
        int resolved = (phy_status >> 11)& 1;
        int link = (phy_status >> 10)& 1;
        if (resolved)
        {
            result.s.up = link;
            result.s.full_duplex = duplex;
            switch (speed)
            {
                case 0: /* 10 Mbps */
                    result.s.speed = 10;
                    break;
                case 1: /* 100 Mbps */
                    result.s.speed = 100;
                    break;
                case 2: /* 1 Gbps */
                    result.s.speed = 1000;
                    break;
                case 3: /* Illegal */
                    result.u64 = 0;
                    break;
            }
        }
    }

    return result;
}

/**
 * Read the status of a PHY
 *
 * @param dev_node Node the ethernet device is on
 * @param phy_addr Encoded PHY address, see bdk-if.h for format
 *
 * @return Link status
 */
bdk_if_link_t __bdk_if_phy_get(bdk_node_t dev_node, int phy_addr)
{
    int node = (phy_addr >> 24) & 0xff;
    int mdio_bus = (phy_addr >> 8) & 0xff;
    int mdio_addr = phy_addr & 0xff;
    if (node == 0xff)
        node = dev_node;
    int phy_status;
    bdk_if_link_t result;
    result.u64 = 0;

    /* PHY address of -1 menas there is no PHY and we should have never
        gotten here */
    if (phy_addr == -1)
        return result;

    /* A PHY address with the special value 0x1000 represents a PHY we can't
        connect to through MDIO which is assumed to be at 1Gbps */
    if (phy_addr == BDK_IF_PHY_FIXED_1GB)
    {
        result.s.up = 1;
        result.s.full_duplex = 1;
        result.s.speed = 1000;
        return result;
    }

    /* A PHY address with the special value 0x1001 represents a PHY we can't
        connect to through MDIO which is assumed to be at 100Mbps */
    if (phy_addr == BDK_IF_PHY_FIXED_100MB)
    {
        result.s.up = 1;
        result.s.full_duplex = 1;
        result.s.speed = 100;
        return result;
    }

    /* Check for a PHY connected through TWSI */
    if ((phy_addr & BDK_IF_PHY_TYPE_MASK) == BDK_IF_PHY_TWSI)
        return __bdk_if_phy_get_twsi(dev_node, phy_addr);

    phy_status = bdk_mdio_read(node, mdio_bus, mdio_addr, BDK_MDIO_PHY_REG_ID1);
    if ((phy_status <= 0) || (phy_status == 0xffff))
        return result;

    switch (phy_status)
    {
        case 0x0141: /* Marvell */
        {

            /* This code assumes we are using a Marvell Gigabit PHY. All the
                speed information can be read from register 17 in one go. Somebody
                using a different PHY will need to handle it above in the board
                specific area */
            phy_status = bdk_mdio_read(node, mdio_bus, mdio_addr, 17);
            if (phy_status < 0)
                return result;

            /* If the resolve bit 11 isn't set, see if autoneg is turned off
                (bit 12, reg 0). The resolve bit doesn't get set properly when
                autoneg is off, so force it */
            if ((phy_status & (1<<11)) == 0)
            {
                bdk_mdio_phy_reg_control_t control;
                int phy_c = bdk_mdio_read(node, mdio_bus, mdio_addr, BDK_MDIO_PHY_REG_CONTROL);
                if (phy_c < 0)
                    return result;
                control.u16 = phy_c;
                if (control.s.autoneg_enable == 0)
                    phy_status |= 1<<11;
            }

            /* Only return a link if the PHY has finished auto negotiation
                and set the resolved bit (bit 11) */
            if (phy_status & (1<<11))
            {
                result.s.up = 1;
                result.s.full_duplex = ((phy_status>>13)&1);
                switch ((phy_status>>14)&3)
                {
                    case 0: /* 10 Mbps */
                        result.s.speed = 10;
                        break;
                    case 1: /* 100 Mbps */
                        result.s.speed = 100;
                        break;
                    case 2: /* 1 Gbps */
                        result.s.speed = 1000;
                        break;
                    case 3: /* Illegal */
                        result.u64 = 0;
                        break;
                }
            }
            break;
        }
        case 0x0022: /* Kendin */
        {
            /* Register 1Fh - PHY Control */
            /* Micrel KSZ9031RNX, EBB8104 RGMII transceiver */
            /* Reports as "Kendin" in BDK_MDIO_PHY_REG_ID1 */
            phy_status = bdk_mdio_read(node, mdio_bus, mdio_addr, 0x1F);
            if (phy_status & (1 << 6)) // Speed Status - 1000Base-T
            {
                result.s.up = 1;
                result.s.speed = 1000;
            }
            else if (phy_status & (1 << 5)) // Speed Status - 100Base-TX
            {
                result.s.up = 1;
                result.s.speed = 100;
            }
            else if (phy_status & (1 << 4)) // Speed Status - 10Base-T
            {
                result.s.up = 1;
                result.s.speed = 10;
            }
            if (phy_status & (1 << 3)) // Duplex Status
            {
                result.s.full_duplex = 1;
            }
            break;
        }
        case 0x0007: /* Vitesse */
        {
            /* Auxiliary Control and Status, Address 28 (0x1C) */
            phy_status = bdk_mdio_read(node, mdio_bus, mdio_addr, 0x1c);
            result.s.full_duplex = (phy_status>>5)&1;
            switch ((phy_status>>3) & 3)
            {
                case 0:
                    result.s.speed = 10;
                    result.s.up = 1;
                    break;
                case 1:
                    result.s.speed = 100;
                    result.s.up = 1;
                    break;
                default:
                    result.s.speed = 1000;
                    break;
            }
            phy_status = bdk_mdio_read(node, mdio_bus, mdio_addr, 0x01);
            result.s.up = (phy_status>>2)&1;
            break;
        }
        default: /* Treat like Broadcom */
        {
            /* Below we are going to read SMI/MDIO register 0x19 which works
                on Broadcom parts */
            phy_status = bdk_mdio_read(node, mdio_bus, mdio_addr, 0x19);
            if (phy_status < 0)
                return result;

            switch ((phy_status>>8) & 0x7)
            {
                case 0:
                    result.u64 = 0;
                    break;
                case 1:
                    result.s.up = 1;
                    result.s.full_duplex = 0;
                    result.s.speed = 10;
                    break;
                case 2:
                    result.s.up = 1;
                    result.s.full_duplex = 1;
                    result.s.speed = 10;
                    break;
                case 3:
                    result.s.up = 1;
                    result.s.full_duplex = 0;
                    result.s.speed = 100;
                    break;
                case 4:
                    result.s.up = 1;
                    result.s.full_duplex = 1;
                    result.s.speed = 100;
                    break;
                case 5:
                    result.s.up = 1;
                    result.s.full_duplex = 1;
                    result.s.speed = 100;
                    break;
                case 6:
                    result.s.up = 1;
                    result.s.full_duplex = 0;
                    result.s.speed = 1000;
                    break;
                case 7:
                    result.s.up = 1;
                    result.s.full_duplex = 1;
                    result.s.speed = 1000;
                    break;
            }
            break;
        }
    }

    /* If link is down, return all fields as zero. */
    if (!result.s.up)
        result.u64 = 0;

    return result;
}

/**
 * PHY XS initialization, primarily for RXAUI
 *
 * @param dev_node Node the ethernet device is on
 * @param phy_addr Encoded PHY address, see bdk-if.h for format
 *
 * @return none
 */
void __bdk_if_phy_xs_init(bdk_node_t dev_node, int phy_addr)
{
    /* This code only supports PHYs connected through MDIO */
    if ((phy_addr & BDK_IF_PHY_TYPE_MASK) != BDK_IF_PHY_MDIO)
        return;

    int node = (phy_addr >> 24) & 0xff;
    int mdio_bus = (phy_addr >> 8) & 0xff;
    int mdio_addr = phy_addr & 0xff;
    if (node == 0xff)
        node = dev_node;

    /* Read the PMA/PMD Device Identifier (1.2, 1.3)
       OUI is spread across both registers */
    int dev_addr = 1;
    int reg_addr = 2;
    int phy_id1 = bdk_mdio_45_read(node, mdio_bus, mdio_addr, dev_addr, reg_addr);
    if (phy_id1 == -1)
        return;
    reg_addr = 3;
    int phy_id2 = bdk_mdio_45_read(node, mdio_bus, mdio_addr, dev_addr, reg_addr);
    if (phy_id2 == -1)
        return;
    int model_number = (phy_id2 >> 4) & 0x3F;
    int oui = phy_id1;
    oui <<= 6;
    oui |= (phy_id2 >> 10) & 0x3F;
    switch (oui)
    {
       case 0x5016:  /* Marvell */
           if (model_number == 9) /* 88X3140/3120 */
           {
               BDK_TRACE(BGX, "N%d.MDIO%d.%d: Performing PHY reset on Marvell RXAUI PHY\n",
                   node, mdio_bus, mdio_addr);
               dev_addr = 4;
               reg_addr = 0;
               /* Write bit 15, Software Reset, in PHY XS Control 1 (4.0).  On CN78xx,
                  sometimes the PHY/BGX gets stuck in local fault mode, link never comes up,
                  and this appears to clear it up.  Haven't seen this on CN81xx or T88,
                  but the reset seems like cheap insurance. */
               if (bdk_mdio_45_write(node, mdio_bus, mdio_addr, dev_addr, reg_addr, (1 << 15)))
               {
                   bdk_error("PHY XS: MDIO write to (%d.%d) failed\n", dev_addr, reg_addr);
                   return;
               }

               int reset_pending = 1;
               while (reset_pending)
               {
                   reset_pending = bdk_mdio_45_read(node, mdio_bus, mdio_addr, dev_addr, reg_addr);
                   reset_pending &= (1 << 15);
               }

               /* Adjust the RXAUI TX Level for Marvell PHY, per Brendan Metzner
                  write 5 to register 4.49155 */
               reg_addr = 49155;
               if (bdk_mdio_45_write(node, mdio_bus, mdio_addr, dev_addr, reg_addr, 5))
               {
                   bdk_error("PHY XS: MDIO write to (%d.%d) failed\n", dev_addr, reg_addr);
                   return;
               }
           }
           break;

       default:  /* Unknown PHY, or no PHY present */
           break;
    }
}

int bdk_if_phy_setup(bdk_node_t dev_node)
{
    /* 81xx has only 2 BGX (BGX0-BGX1); BGX2 is RGMII */
    for (int bgx = 0; bgx < 2; bgx++)
    {
        int port = 0;
        int phy_addr = bdk_config_get_int(BDK_CONFIG_PHY_ADDRESS, dev_node, bgx, port);
        if (phy_addr != -1)
        {
            int node = (phy_addr >> 24) & 0xff;
            int mdio_bus = (phy_addr >> 8) & 0xff;
            int mdio_addr = phy_addr & 0xff;
            if (node == 0xff)
                node = bdk_numa_local();
            if ((phy_addr & BDK_IF_PHY_TYPE_MASK) == BDK_IF_PHY_MDIO)
            {
                int qlm = bdk_qlm_get_qlm_num(node, BDK_IF_BGX, bgx, port);
                if (qlm == -1)
                    continue;

                BDK_TRACE(PHY, "N%d.BGX%d.%d: Configuring ...\n", node, bgx, port);

                 /* Check PHY id */
                int phy_status_1 = bdk_mdio_read(node, mdio_bus, phy_addr, BDK_MDIO_PHY_REG_ID1);
                int phy_status_2 = bdk_mdio_read(node, mdio_bus, phy_addr, BDK_MDIO_PHY_REG_ID2);

                /* Vitesse */
                if (phy_status_1 == 0x0007)
                {
                    if (phy_status_2 == 0x0670)
                    {
                        bdk_if_phy_vsc8514_setup(node, qlm, mdio_bus, mdio_addr);
                    }
                    else
                    {
                        bdk_if_phy_vetesse_setup(node, qlm, mdio_bus, mdio_addr);
                    }
                }

                /* Marvell */
                else if (phy_status_1 == 0x0141)
                    bdk_if_phy_marvell_setup(node, qlm, mdio_bus, mdio_addr);
                else
                    BDK_TRACE(PHY, "N%d.BGX%d.%d: Unknown PHY %x\n", node, bgx, port, phy_status_1);
            }
        }
    }
    return 0;
}

