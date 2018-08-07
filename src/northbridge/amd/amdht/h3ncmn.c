/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


/*----------------------------------------------------------------------------
 *				MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#undef FILECODE
#define FILECODE 0xF002
#include "h3ncmn.h"
#include "h3finit.h"
#include "h3ffeat.h"
#include "AsPsNb.h"

#include <device/pci.h>
#include <console/console.h>
#include <cpu/amd/msr.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <northbridge/amd/amdfam10/raminit.h>
#include <northbridge/amd/amdfam10/amdfam10.h>


/*----------------------------------------------------------------------------
 *			DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/* CPU Northbridge Functions */
#define CPU_HTNB_FUNC_00		0
#define CPU_HTNB_FUNC_04		4
#define CPU_ADDR_FUNC_01		1
#define CPU_NB_FUNC_03			3
#define CPU_NB_FUNC_05			5

/* Function 0 registers */
#define REG_ROUTE0_0X40		0x40
#define REG_ROUTE1_0X44		0x44
#define REG_NODE_ID_0X60		0x60
#define REG_UNIT_ID_0X64		0x64
#define REG_LINK_TRANS_CONTROL_0X68	0x68
#define REG_LINK_INIT_CONTROL_0X6C	0x6c
#define REG_HT_CAP_BASE_0X80		0x80
#define REG_NORTHBRIDGE_CFG_3X8C	0x8c
#define REG_HT_LINK_RETRY0_0X130	0x130
#define REG_HT_TRAFFIC_DIST_0X164	0x164
#define REG_HT_LINK_EXT_CONTROL0_0X170	0x170

#define HT_CONTROL_CLEAR_CRC		(~(3 << 8))

/* Function 1 registers */
#define REG_ADDR_CONFIG_MAP0_1XE0	0xE0
#define CPU_ADDR_NUM_CONFIG_MAPS	4

/* Function 3 registers */
#define REG_NB_SRI_XBAR_BUF_3X70	0x70
#define REG_NB_MCT_XBAR_BUF_3X78	0x78
#define REG_NB_FIFOPTR_3XDC		0xDC
#define REG_NB_CAPABILITY_3XE8		0xE8
#define REG_NB_CPUID_3XFC		0xFC
#define REG_NB_LINK_XCS_TOKEN0_3X148	0x148
#define REG_NB_DOWNCORE_3X190		0x190
#define REG_NB_CAPABILITY_5X84		0x84

/* Function 4 registers */


/*----------------------------------------------------------------------------
 *			TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------
 *			PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/***************************************************************************
 ***			FAMILY/NORTHBRIDGE SPECIFIC FUNCTIONS		***
 ***************************************************************************/

inline uint8_t is_gt_rev_d(void)
{
	uint8_t fam15h = 0;
	uint8_t rev_gte_d = 0;
	uint32_t family;
	uint32_t model;

	family = model = cpuid_eax(0x80000001);
	model = ((model & 0xf0000) >> 12) | ((model & 0xf0) >> 4);
	family = ((family & 0xf00000) >> 16) | ((family & 0xf00) >> 8);

	if (family >= 0x6f)
		/* Family 15h or later */
		fam15h = 1;

	if ((model >= 0x8) || fam15h)
		/* Revision D or later */
		rev_gte_d = 1;

	return rev_gte_d;
}

/***************************************************************************//**
 *
 * SBDFO
 * makeLinkBase(u8 currentNode, u8 currentLink)
 *
 *  Description:
 *	Private to northbridge implementation. Return the HT Host capability base
 *	PCI config address for a link.
 *
 *  Parameters:
 *	@param[in]  node    = the node this link is on
 *	@param[in]  link    = the link
 *
 *****************************************************************************/
static SBDFO makeLinkBase(u8 node, u8 link)
{
	SBDFO linkBase;

	/* With rev F can not be called with a 4th link or with the sublinks */
	if (link < 4)
		linkBase = MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_HTNB_FUNC_00,
				REG_HT_CAP_BASE_0X80 + link*HT_HOST_CAP_SIZE);
	else
		linkBase = MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_HTNB_FUNC_04,
				REG_HT_CAP_BASE_0X80 + (link-4)*HT_HOST_CAP_SIZE);
	return linkBase;
}

/***************************************************************************//**
 *
 * void
 * setHtControlRegisterBits(SBDFO reg, u8 hiBit, u8 loBit, u32 *pValue)
 *
 *  Description:
 *	Private to northbridge implementation. Provide a common routine for accessing the
 *	HT Link Control registers (84, a4, c4, e4), to enforce not clearing the
 *	HT CRC error bits.  Replaces direct use of AmdPCIWriteBits().
 *	NOTE: This routine is called for IO Devices as well as CPUs!
 *
 *  Parameters:
 *	@param[in]  reg    = the PCI config address the control register
 *	@param[in]  hiBit  = the high bit number
 *	@param[in]  loBit  = the low bit number
 *	@param[in]  pValue = the value to write to that bit range. Bit 0 => loBit.
 *
 *****************************************************************************/
static void setHtControlRegisterBits(SBDFO reg, u8 hiBit, u8 loBit, u32 *pValue)
{
	u32 temp, mask;

	ASSERT((hiBit < 32) && (loBit < 32) && (hiBit >= loBit) && ((reg & 0x3) == 0));
	ASSERT((hiBit < 8) || (loBit > 9));

	/* A 1<<32 == 1<<0 due to x86 SHL instruction, so skip if that is the case */
	if ((hiBit-loBit) != 31)
		mask = (((u32)1 << (hiBit-loBit+1))-1);
	else
		mask = (u32)0xFFFFFFFF;

	AmdPCIRead(reg, &temp);
	temp &= ~(mask << loBit);
	temp |= (*pValue & mask) << loBit;
	temp &= (u32)HT_CONTROL_CLEAR_CRC;
	AmdPCIWrite(reg, &temp);
}

/***************************************************************************//**
 *
 * static void
 * writeRoutingTable(u8 node, u8 target, u8 Link, cNorthBridge *nb)
 *
 *  Description:
 *	 This routine will modify the routing tables on the
 *	 SourceNode to cause it to route both request and response traffic to the
 *	 targetNode through the specified Link.
 *
 *	 NOTE: This routine is to be used for early discovery and initialization.  The
 *	 final routing tables must be loaded some other way because this
 *	 routine does not address the issue of probes, or independent request
 *	 response paths.
 *
 *  Parameters:
 *	@param[in]  node    = the node that will have it's routing tables modified.
 *	@param[in]  target  = For routing to node target
 *	@param[in]  link    =  Link from node to target
 *	@param[in]  *nb   = this northbridge
 *
 *****************************************************************************/

static void writeRoutingTable(u8 node, u8 target, u8 link, cNorthBridge *nb)
{
#ifndef HT_BUILD_NC_ONLY
	u32 temp = (nb->selfRouteResponseMask | nb->selfRouteRequestMask) << (link + 1);
	ASSERT((node < nb->maxNodes) && (target < nb->maxNodes) && (link < nb->maxLinks));
	AmdPCIWrite(MAKE_SBDFO(makePCISegmentFromNode(node),
			makePCIBusFromNode(node),
			makePCIDeviceFromNode(node),
			CPU_HTNB_FUNC_00,
			REG_ROUTE0_0X40 + target*4),
			&temp);
#else
	STOP_HERE;
#endif
}

/***************************************************************************//**
 *
 * static void
 * writeNodeID(u8 node, u8 nodeID, cNorthBridge *nb)
 *
 *  Description:
 *	Modifies the NodeID register on the target node
 *
 *  Parameters:
 *	@param[in] node    = the node that will have its NodeID altered.
 *	@param[in] nodeID  = the new value for NodeID
 *	@param[in] *nb     = this northbridge
 *
 *****************************************************************************/

static void writeNodeID(u8 node, u8 nodeID, cNorthBridge *nb)
{
	u32 temp;
	ASSERT((node < nb->maxNodes) && (nodeID < nb->maxNodes));
	if (is_fam15h()) {
		temp = 1;
		AmdPCIWriteBits(MAKE_SBDFO(makePCISegmentFromNode(node),
					makePCIBusFromNode(node),
					makePCIDeviceFromNode(node),
					CPU_NB_FUNC_03,
					REG_NORTHBRIDGE_CFG_3X8C),
					22, 22, &temp);
	}
	temp = nodeID;
	AmdPCIWriteBits(MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_HTNB_FUNC_00,
				REG_NODE_ID_0X60),
				2, 0, &temp);
}

/***************************************************************************//**
 *
 * static void
 * readDefLnk(u8 node, cNorthBridge *nb)
 *
 *  Description:
 *	 Read the DefLnk (the source link of the current packet)
 *	 from node
 *
 *  Parameters:
 *	@param[in] node    = the node that will have its NodeID altered.
 *	@param[in] *nb     = this northbridge
 *	@return                 The HyperTransport link where the request to
 *				read the default link came from.  Since this
 *				code is running on the BSP, this should be the link
 *				pointing back towards the BSP.
 *
 *****************************************************************************/

static u8 readDefLnk(u8 node, cNorthBridge *nb)
{
	u32 deflink = 0;
	SBDFO licr;
	u32 temp;

	licr = MAKE_SBDFO(makePCISegmentFromNode(node),
			makePCIBusFromNode(node),
			makePCIDeviceFromNode(node),
			CPU_HTNB_FUNC_00,
			REG_LINK_INIT_CONTROL_0X6C);

	ASSERT((node < nb->maxNodes));
	AmdPCIReadBits(licr, 3, 2, &deflink);
	AmdPCIReadBits(licr, 8, 8, &temp);	/* on rev F, this bit is reserved == 0 */
	deflink |= temp << 2;
	return (u8)deflink;
}

/***************************************************************************//**
 *
 * static void
 * enableRoutingTables(u8 node, cNorthBridge *nb)
 *
 *  Description:
 *	Turns routing tables on for a given node
 *
 *  Parameters:
 *	@param[in]  node = the node that will have it's routing tables enabled
 *	@param[in]  *nb  = this northbridge
 *
 *****************************************************************************/

static void enableRoutingTables(u8 node, cNorthBridge *nb)
{
	u32 temp = 0;
	ASSERT((node < nb->maxNodes));
	AmdPCIWriteBits(MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_HTNB_FUNC_00,
				REG_LINK_INIT_CONTROL_0X6C),
				0, 0, &temp);
}


/***************************************************************************//**
 *
 * static BOOL
 * verifyLinkIsCoherent(u8 node, u8 Link, cNorthBridge *nbk)
 *
 *  Description:
 *	Verify that the link is coherent, connected, and ready
 *
 *  Parameters:
 *	@param[in]   node      = the node that will be examined
 *	@param[in]   link      = the link on that Node to examine
 *	@param[in]   *nb       = this northbridge
 *	@return            true - The link has the following status
 *				  linkCon = 1,		Link is connected
 *				  InitComplete = 1,	Link initialization is complete
 *				  NC = 0,		Link is coherent
 *				  UniP-cLDT = 0,	Link is not Uniprocessor cLDT
 *				  LinkConPend = 0	Link connection is not pending
 *				  false- The link has some other status
 *
 *****************************************************************************/

static BOOL verifyLinkIsCoherent(u8 node, u8 link, cNorthBridge *nb)
{
#ifndef HT_BUILD_NC_ONLY

	u32 linkType;
	SBDFO linkBase;

	ASSERT((node < nb->maxNodes) && (link < nb->maxLinks));

	linkBase = makeLinkBase(node, link);

	/*  FN0_98/A4/C4 = LDT Type Register */
	AmdPCIRead(linkBase + HTHOST_LINK_TYPE_REG, &linkType);

	/*  Verify LinkCon = 1, InitComplete = 1, NC = 0, UniP-cLDT = 0, LinkConPend = 0 */
	return (linkType & HTHOST_TYPE_MASK) ==  HTHOST_TYPE_COHERENT;
#else
	return 0;
#endif /* HT_BUILD_NC_ONLY */
}

/***************************************************************************//**
 *
 * static bool
 * readTrueLinkFailStatus(u8 node, u8 link, sMainData *pDat, cNorthBridge *nb)
 *
 *  Description:
 *	Return the LinkFailed status AFTER an attempt is made to clear the bit.
 *	Also, call event notify if a Hardware Fault caused a synch flood on a previous boot.
 *
 *	The table below summarizes correct responses of this routine.
 *	Family	  before    after    unconnected    Notify?    return
 *	  0F         0       0          0             No         0
 *	  0F         1       0          0             Yes        0
 *	  0F         1       1          X             No         1
 *	  10         0       0          0             No         0
 *	  10         1       0          0             Yes        0
 *	  10         1       0          3             No         1
 *
 *  Parameters:
 *	@param[in]    node      = the node that will be examined
 *	@param[in]    link      = the link on that node to examine
 *	@param[in]    *pDat = access to call back routine
 *	@param[in]    *nb       = this northbridge
 *	@return                   true - the link is not connected or has hard error
 *	                          false- if the link is connected
 *
 *****************************************************************************/

static BOOL readTrueLinkFailStatus(u8 node, u8 link, sMainData *pDat, cNorthBridge *nb)
{
	u32 before, after, unconnected, crc;
	SBDFO linkBase;

	ASSERT((node < nb->maxNodes) && (link < nb->maxLinks));

	linkBase = makeLinkBase(node, link);

	/* Save the CRC status before doing anything else.
	 * Read, Clear, the Re-read the error bits in the Link Control Register
	 * FN0_84/A4/C4[4] = LinkFail bit
	 * and the connection status, TransOff and EndOfChain
	 */
	AmdPCIReadBits(linkBase + HTHOST_LINK_CONTROL_REG, 9, 8, &crc);
	AmdPCIReadBits(linkBase + HTHOST_LINK_CONTROL_REG, 4, 4, &before);
	setHtControlRegisterBits(linkBase + HTHOST_LINK_CONTROL_REG, 4, 4, &before);
	AmdPCIReadBits(linkBase + HTHOST_LINK_CONTROL_REG, 4, 4, &after);
	AmdPCIReadBits(linkBase + HTHOST_LINK_CONTROL_REG, 7, 6, &unconnected);

	if (before != after)
	{
		if (!unconnected)
		{
			if (crc != 0)
			{
				/* A synch flood occurred due to HT CRC */
				if (pDat->HtBlock->AMD_CB_EventNotify)
				{
					/* Pass the node and link on which the generic synch flood event occurred. */
					sHtEventHWHtCrc evt;
					evt.eSize = sizeof(sHtEventHWHtCrc);
					evt.node = node;
					evt.link = link;
					evt.laneMask = (uint8)crc;

					pDat->HtBlock->AMD_CB_EventNotify(HT_EVENT_CLASS_HW_FAULT,
									HT_EVENT_HW_HTCRC,
									(u8 *)&evt);
				}
			}
			else
			{
				/* Some synch flood occurred */
				if (pDat->HtBlock->AMD_CB_EventNotify)
				{
					/* Pass the node and link on which the generic synch flood event occurred. */
					sHtEventHWSynchFlood evt;
					evt.eSize = sizeof(sHtEventHWSynchFlood);
					evt.node = node;
					evt.link = link;

					pDat->HtBlock->AMD_CB_EventNotify(HT_EVENT_CLASS_HW_FAULT,
									HT_EVENT_HW_SYNCHFLOOD,
									(u8 *)&evt);
				}
			}
		}
	}
	return ((after != 0) || unconnected);
}


/***************************************************************************//**
 *
 * static u8
 * readToken(u8 node, cNorthBridge *nb)
 *
 *  Description:
 *	Read the token stored in the scratchpad register
 *	NOTE: The location used to store the token is arbitrary.  The only
 *	requirement is that the location warm resets to zero, and that
 *	using it will have no ill-effects during HyperTransport initialization.
 *
 *  Parameters:
 *	@param[in]  node      = the node that will be examined
 *	@param[in]  *nb       = this northbridge
 *	@return                the Token read from the node
 *
 *****************************************************************************/
static u8 readToken(u8 node, cNorthBridge *nb)
{
	u32 temp;

	ASSERT((node < nb->maxNodes));
	/* Use CpuCnt as a scratch register */
	/* Limiting use to 4 bits makes code GH to rev F compatible. */
	AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_HTNB_FUNC_00,
				REG_NODE_ID_0X60),
				19, 16, &temp);

	return (u8)temp;
}


/***************************************************************************//**
 *
 * static void
 * writeToken(u8 node, u8 Value, cNorthBridge *nb)
 *
 *  Description:
 *	Write the token stored in the scratchpad register
 *	NOTE: The location used to store the token is arbitrary.  The only
 *	requirement is that the location warm resets to zero, and that
 *	using it will have no ill-effects during HyperTransport initialization.
 *	Limiting use to 4 bits makes code GH to rev F compatible.
 *
 *  Parameters:
 *	@param[in]  node  = the node that will be examined
 *	@param      value
 *	@param[in] *nb  = this northbridge
 *
 *****************************************************************************/
static void writeToken(u8 node, u8 value, cNorthBridge *nb)
{
	u32 temp = value;
	ASSERT((node < nb->maxNodes));
	/* Use CpuCnt as a scratch register */
	/* Limiting use to 4 bits makes code GH to rev F compatible. */
	AmdPCIWriteBits(MAKE_SBDFO(makePCISegmentFromNode(node),
					makePCIBusFromNode(node),
					makePCIDeviceFromNode(node),
					CPU_HTNB_FUNC_00,
					REG_NODE_ID_0X60),
					19, 16, &temp);
}

/***************************************************************************//**
 *
 * static u8
 * fam0FGetNumCoresOnNode(u8 node, cNorthBridge *nb)
 *
 *  Description:
 *	Return the number of cores (1 based count) on node.
 *
 *  Parameters:
 *	@param[in]  node      = the node that will be examined
 *	@param[in] *nb = this northbridge
 *	@return    = the number of cores
 *
 * ---------------------------------------------------------------------------------------
 */
static u8 fam0FGetNumCoresOnNode(u8 node, cNorthBridge *nb)
{
	u32 temp;

	ASSERT((node < nb->maxNodes));
	/* Read CmpCap */
	AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(node),
			makePCIBusFromNode(node),
			makePCIDeviceFromNode(node),
			CPU_NB_FUNC_03,
			REG_NB_CAPABILITY_3XE8),
			13, 12, &temp);

	/* and add one */
	return (u8)(temp+1);
}

/***************************************************************************//**
 *
 * static u8
 * fam10GetNumCoresOnNode(u8 node, cNorthBridge *nb)
 *
 *  Description:
 *	Return the number of cores (1 based count) on node.
 *
 *  Parameters:
 *	@param[in]  node      = the node that will be examined
 *	@param[in] *nb = this northbridge
 *	@return    = the number of cores
 *
 *
 */
static u8 fam10GetNumCoresOnNode(u8 node, cNorthBridge *nb)
{
	u32 temp, leveling, cores;
	u8 i;

	ASSERT((node < nb->maxNodes));
	/* Read CmpCap [2][1:0] */
	AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_NB_FUNC_03,
				REG_NB_CAPABILITY_3XE8),
				15, 12, &temp);

	/* bits[15,13,12] specify the cores */
	temp = ((temp & 8) >> 1) + (temp & 3);
	cores = temp + 1;

	/* Support Downcoring */
	AmdPCIReadBits (MAKE_SBDFO(makePCISegmentFromNode(node),
					makePCIBusFromNode(node),
					makePCIDeviceFromNode(node),
					CPU_NB_FUNC_03,
					REG_NB_DOWNCORE_3X190),
					3, 0, &leveling);
	for (i = 0; i < cores; i++)
	{
		if (leveling & ((u32) 1 << i))
		{
			temp--;
		}
	}
	return (u8)(temp+1);
}

/***************************************************************************//**
 *
 * static u8
 * fam15GetNumCoresOnNode(u8 node, cNorthBridge *nb)
 *
 *  Description:
 *	Return the number of cores (1 based count) on node.
 *
 *  Parameters:
 *	@param[in]  node      = the node that will be examined
 *	@param[in] *nb = this northbridge
 *	@return    = the number of cores
 *
 *
 */
static u8 fam15GetNumCoresOnNode(u8 node, cNorthBridge *nb)
{
	u32 temp, leveling, cores;
	u8 i;

	ASSERT((node < nb->maxNodes));
	/* Read CmpCap [7:0] */
	AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_NB_FUNC_05,
				REG_NB_CAPABILITY_5X84),
				7, 0, &temp);

	/* bits[7:0] specify the cores */
	temp = temp & 0xff;
	cores = temp + 1;

	/* Support Downcoring */
	AmdPCIReadBits (MAKE_SBDFO(makePCISegmentFromNode(node),
					makePCIBusFromNode(node),
					makePCIDeviceFromNode(node),
					CPU_NB_FUNC_03,
					REG_NB_DOWNCORE_3X190),
					31, 0, &leveling);
	for (i = 0; i < cores; i++)
	{
		if (leveling & ((u32) 1 << i))
		{
			temp--;
		}
	}
	return (u8)(temp+1);
}

/***************************************************************************//**
 *
 * static void
 * setTotalNodesAndCores(u8 node, u8 totalNodes, u8 totalCores, cNorthBridge *nb)
 *
 *  Description:
 *	Write the total number of cores and nodes to the node
 *
 *  Parameters:
 *	@param[in]  node   = the node that will be examined
 *	@param[in]  totalNodes  = the total number of nodes
 *	@param[in]  totalCores  = the total number of cores
 *	@param[in] *nb   = this northbridge
 *
 * ---------------------------------------------------------------------------------------
 */
static void setTotalNodesAndCores(u8 node, u8 totalNodes, u8 totalCores, cNorthBridge *nb)
{
	SBDFO nodeIDReg;
	u32 temp;

	ASSERT((node < nb->maxNodes) && (totalNodes <= nb->maxNodes));
	nodeIDReg = MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_HTNB_FUNC_00,
				REG_NODE_ID_0X60);

	temp = totalCores-1;
	/* Rely on max number of nodes:cores for rev F and GH to make
	 * this code work, even though we write reserved bit 20 on rev F it will be
	 * zero in that case.
	 */
	AmdPCIWriteBits(nodeIDReg, 20, 16, &temp);
	temp = totalNodes-1;
	AmdPCIWriteBits(nodeIDReg, 6,  4, &temp);
}

/***************************************************************************//**
 *
 * static void
 * limitNodes(u8 node, cNorthBridge *nb)
 *
 *  Description:
 *	Limit coherent config accesses to cpus as indicated by nodecnt.
 *
 *  Parameters:
 *	@param[in]  node  = the node that will be examined
 *	@param[in] *nb  = this northbridge
 *
 * ---------------------------------------------------------------------------------------
 */
static void limitNodes(u8 node, cNorthBridge *nb)
{
	u32 temp = 1;
	ASSERT((node < nb->maxNodes));
	AmdPCIWriteBits(MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_HTNB_FUNC_00,
				REG_LINK_TRANS_CONTROL_0X68),
				15, 15, &temp);
}

/***************************************************************************//**
 *
 * static void
 * writeFullRoutingTable(u8 node, u8 target, u8 reqLink, u8 rspLink, u32 BClinks, cNorthBridge *nb)
 *
 *  Description:
 *	Write the routing table entry for node to target, using the request link, response
 *	link, and broadcast links provided.
 *
 *  Parameters:
 *	@param[in]  node   = the node that will be examined
 *	@param[in]  target   = the target node for these routes
 *	@param[in]  reqLink  = the link for requests to target
 *	@param[in]  rspLink  = the link for responses to target
 *	@param[in]  bClinks  = the broadcast links
 *	@param[in] *nb  = this northbridge
 *
 * ---------------------------------------------------------------------------------------
 */
static void writeFullRoutingTable(u8 node, u8 target, u8 reqLink, u8 rspLink, u32 bClinks, cNorthBridge *nb)
{
#ifndef HT_BUILD_NC_ONLY
	u32 value = 0;

	ASSERT((node < nb->maxNodes) && (target < nb->maxNodes));
	if (reqLink == ROUTETOSELF)
		value |= nb->selfRouteRequestMask;
	else
		value |= nb->selfRouteRequestMask << (reqLink+1);

	if (rspLink == ROUTETOSELF)
		value |= nb->selfRouteResponseMask;
	else
		value |= nb->selfRouteResponseMask << (rspLink+1);

	/* Allow us to accept a Broadcast ourselves, then set broadcasts for routes */
	value |= (u32)1 << nb->broadcastSelfBit;
	value |= (u32)bClinks << (nb->broadcastSelfBit + 1);

	AmdPCIWrite(MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_HTNB_FUNC_00,
				REG_ROUTE0_0X40 + target*4), &value);
#else
	STOP_HERE;
#endif /* HT_BUILD_NC_ONLY */
}

/***************************************************************************//**
 *
 * static u32
 * makeKey(u8 currentNode)
 *
 *  Description:
 *	Private routine to northbridge code.
 *	Determine whether a node is compatible with the discovered configuration so
 *	far.  Currently, that means the family, extended family of the new node are the
 *	same as the BSP's.
 *
 *  Parameters:
 *	@param[in]   node   = the node
 *	@return = the key value
 *
 * ---------------------------------------------------------------------------------------
 */
static u32 makeKey(u8 node)
{
	u32 extFam, baseFam;
	AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_NB_FUNC_03,
				REG_NB_CPUID_3XFC),
				27, 20, &extFam);
	AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_NB_FUNC_03,
				REG_NB_CPUID_3XFC),
				11, 8, &baseFam);
	return ((u32)(baseFam << 8) | extFam);
}


/***************************************************************************//**
 *
 * static BOOL
 * isCompatible(u8 currentNode, cNorthBridge *nb)
 *
 *  Description:
 *	Determine whether a node is compatible with the discovered configuration so
 *	far.  Currently, that means the family, extended family of the new node are the
 *	same as the BSP's.
 *
 *  Parameters:
 *	@param[in]  node   = the node
 *	@param[in] *nb  = this northbridge
 *	@return = true: the new is compatible, false: it is not
 *
 * ---------------------------------------------------------------------------------------
 */
static BOOL isCompatible(u8 node, cNorthBridge *nb)
{
	return (makeKey(node) == nb->compatibleKey);
}

/***************************************************************************//**
 *
 * static BOOL
 * fam0fIsCapable(u8 node, sMainData *pDat, cNorthBridge *nb)
 *
 *  Description:
 *	Get node capability and update the minimum supported system capability.
 *	Return whether the current configuration exceeds the capability.
 *
 *  Parameters:
 *	@param[in]       node = the node
 *	@param[in,out]  *pDat = sysMpCap (updated) and NodesDiscovered
 *	@param[in]        *nb = this northbridge
 *	@return               true:  system is capable of current config.
 *			      false: system is not capable of current config.
 *
 * ---------------------------------------------------------------------------------------
 */
static BOOL fam0fIsCapable(u8 node, sMainData *pDat, cNorthBridge *nb)
{
#ifndef HT_BUILD_NC_ONLY
	u32 temp;
	u8 maxNodes;

	ASSERT(node < nb->maxNodes);

	AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_NB_FUNC_03,
				REG_NB_CAPABILITY_3XE8),
				2, 1, &temp);
	if (temp > 1)
	{
		maxNodes = 8;
	} else {
		if (temp == 1)
		{
			maxNodes = 2;
		} else {
			maxNodes = 1;
		}
	}
	if (pDat->sysMpCap > maxNodes)
	{
		 pDat->sysMpCap = maxNodes;
	}
	/* Note since sysMpCap is one based and NodesDiscovered is zero based, equal is false */
	return (pDat->sysMpCap > pDat->NodesDiscovered);
#else
	return 1;
#endif
}

/***************************************************************************//**
 *
 * static BOOL
 * fam10IsCapable(u8 node, sMainData *pDat, cNorthBridge *nb)
 *
 *  Description:
 *	Get node capability and update the minimum supported system capability.
 *	Return whether the current configuration exceeds the capability.
 *
 *  Parameters:
 *	@param[in]  node   = the node
 *	@param[in,out] *pDat = sysMpCap (updated) and NodesDiscovered
 *	@param[in] *nb   = this northbridge
 *	@return             true: system is capable of current config.
 *			   false: system is not capable of current config.
 *
 * ---------------------------------------------------------------------------------------
 */
static BOOL fam10IsCapable(u8 node, sMainData *pDat, cNorthBridge *nb)
{
#ifndef HT_BUILD_NC_ONLY
	u32 temp;
	u8 maxNodes;

	ASSERT(node < nb->maxNodes);

	AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_NB_FUNC_03,
				REG_NB_CAPABILITY_3XE8),
				18, 16, &temp);

	if (temp != 0)
	{
		maxNodes = (1 << (~temp & 0x3));  /* That is, 1, 2, 4, or 8 */
	}
	else
	{
		maxNodes = 8;
	}

	if (pDat->sysMpCap > maxNodes)
	{
		pDat->sysMpCap = maxNodes;
	}
	/* Note since sysMpCap is one based and NodesDiscovered is zero based, equal is false */
	return (pDat->sysMpCap > pDat->NodesDiscovered);
#else
	return 1;
#endif
}

/***************************************************************************//**
 *
 * static BOOL
 * fam15IsCapable(u8 node, sMainData *pDat, cNorthBridge *nb)
 *
 *  Description:
 *	Get node capability and update the minimum supported system capability.
 *	Return whether the current configuration exceeds the capability.
 *
 *  Parameters:
 *	@param[in]  node   = the node
 *	@param[in,out] *pDat = sysMpCap (updated) and NodesDiscovered
 *	@param[in] *nb   = this northbridge
 *	@return             true: system is capable of current config.
 *			   false: system is not capable of current config.
 *
 * ---------------------------------------------------------------------------------------
 */
static BOOL fam15IsCapable(u8 node, sMainData *pDat, cNorthBridge *nb)
{
#ifndef HT_BUILD_NC_ONLY
	u32 temp;
	u8 maxNodes;

	ASSERT(node < nb->maxNodes);

	AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_NB_FUNC_03,
				REG_NB_CAPABILITY_3XE8),
				18, 16, &temp);

	if (temp != 0)
	{
		maxNodes = (1 << (~temp & 0x3));  /* That is, 1, 2, 4, or 8 */
	}
	else
	{
		/* Check if CPU package is dual node */
		AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(node),
					makePCIBusFromNode(node),
					makePCIDeviceFromNode(node),
					CPU_NB_FUNC_03,
					REG_NB_CAPABILITY_3XE8),
					29, 29, &temp);
		if (temp)
			maxNodes = 4;
		else
			maxNodes = 8;
	}

	if (pDat->sysMpCap > maxNodes)
	{
		pDat->sysMpCap = maxNodes;
	}
	/* Note since sysMpCap is one based and NodesDiscovered is zero based, equal is false */
	return (pDat->sysMpCap > pDat->NodesDiscovered);
#else
	return 1;
#endif
}

/***************************************************************************//**
 *
 * static void
 * fam0fStopLink(u8 currentNode, u8 currentLink, cNorthBridge *nb)
 *
 *  Description:
 *	Disable a cHT link on node by setting F0x[E4, C4, A4, 84][TransOff, EndOfChain]=1
 *
 *  Parameters:
 *	@param[in]  node      = the node this link is on
 *	@param[in]  link      = the link to stop
 *	@param[in] *nb = this northbridge
 *
 * ---------------------------------------------------------------------------------------
 */
static void fam0fStopLink(u8 node, u8 link, cNorthBridge *nb)
{
#ifndef HT_BUILD_NC_ONLY
	u32 temp;
	SBDFO linkBase;

	ASSERT((node < nb->maxNodes) && (link < nb->maxLinks));

	linkBase = makeLinkBase(node, link);

	/* Set TransOff, EndOfChain */
	temp = 3;
	setHtControlRegisterBits(linkBase + HTHOST_LINK_CONTROL_REG, 7, 6, &temp);
#endif
}

/***************************************************************************//**
 *
 * static void
 * commonVoid()
 *
 *  Description:
 *	Nothing.
 *
 *  Parameters:
 *		None.
 *
 * ---------------------------------------------------------------------------------------
 */
static void commonVoid(void)
{
}

/***************************************************************************//**
 *
 * static BOOL
 * commonReturnFalse()
 *
 *  Description:
 *	Return False.
 *
 *  Parameters:
 *	     @return	   = false
 *
 */
static BOOL commonReturnFalse(void)
{
	return 0;
}

/***************************************************************************
 ***			Non-coherent init code				  ***
 ***			Northbridge access routines			  ***
 ***************************************************************************/

/***************************************************************************//**
 *
 * static u8
 * readSbLink(cNorthBridge *nb)
 *
 *  Description:
 *	 Return the link to the Southbridge
 *
 *  Parameters:
 *	@param[in] *nb = this northbridge
 *	@return          the link to the southbridge
 *
 * ---------------------------------------------------------------------------------------
 */
static u8 readSbLink(cNorthBridge *nb)
{
	u32 temp;
	AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(0),
				makePCIBusFromNode(0),
				makePCIDeviceFromNode(0),
				CPU_HTNB_FUNC_00,
				REG_UNIT_ID_0X64),
				10, 8, &temp);
	return (u8)temp;
}

/***************************************************************************//**
 *
 * static BOOL
 * verifyLinkIsNonCoherent(u8 node, u8 link, cNorthBridge *nb)
 *
 *  Description:
 *	 Verify that the link is non-coherent, connected, and ready
 *
 *  Parameters:
 *	@param[in]  node   = the node that will be examined
 *	@param[in]  link   = the Link on that node to examine
 *	@param[in] *nb = this northbridge
 *	@return   = true - The link has the following status
 *					LinkCon = 1,     Link is connected
 *					InitComplete = 1,Link initialization is complete
 *					NC = 1,          Link is coherent
 *					UniP-cLDT = 0,   Link is not Uniprocessor cLDT
 *					LinkConPend = 0  Link connection is not pending
 *					false- The link has some other status
 *
 * ---------------------------------------------------------------------------------------
 */
static BOOL verifyLinkIsNonCoherent(u8 node, u8 link, cNorthBridge *nb)
{
	u32 linkType;
	SBDFO linkBase;

	ASSERT((node < nb->maxNodes) && (link < nb->maxLinks));

	linkBase = makeLinkBase(node, link);

	/* FN0_98/A4/C4 = LDT Type Register */
	AmdPCIRead(linkBase + HTHOST_LINK_TYPE_REG, &linkType);

	/* Verify linkCon = 1, InitComplete = 1, NC = 0, UniP-cLDT = 0, LinkConPend = 0 */
	return (linkType & HTHOST_TYPE_MASK) ==  HTHOST_TYPE_NONCOHERENT;
}

/***************************************************************************//**
 *
 * static void
 * ht3SetCFGAddrMap(u8 cfgMapIndex, u8 secBus, u8 subBus, u8 targetNode, u8 targetLink, sMainData *pDat, cNorthBridge *nb)
 *
 *  Description:
 *	 Configure and enable config access to a non-coherent chain for the given bus range.
 *
 *  Parameters:
 *	@param[in] cfgMapIndex = the map entry to set
 *	@param[in] secBus      = The secondary bus number to use
 *	@param[in] subBus      = The subordinate bus number to use
 *	@param[in] targetNode  = The node  that shall be the recipient of the traffic
 *	@param[in] targetLink  = The link that shall be the recipient of the traffic
 *	@param[in] pDat   = our global state
 *	@param[in] *nb  = this northbridge
 *
 * ---------------------------------------------------------------------------------------
 */
static void  ht3SetCFGAddrMap(u8 cfgMapIndex, u8 secBus, u8 subBus, u8 targetNode, u8 targetLink, sMainData *pDat, cNorthBridge *nb)
{
	u8 curNode;
	SBDFO linkBase;
	u32 temp;

	linkBase = makeLinkBase(targetNode, targetLink);

	ASSERT(secBus <= subBus);
	temp = secBus;
	AmdPCIWriteBits(linkBase + HTHOST_ISOC_REG, 15, 8, &temp);

	/* For target link, note that rev F uses bits 9:8 and only with GH is bit 10
	 * set to indicate a sublink.  For node, we are currently not supporting Extended
	 * routing tables.
	 */
	temp = ((u32)subBus << 24) + ((u32)secBus << 16) + ((u32)targetLink << 8)
		+ ((u32)targetNode << 4) + (u32)3;
	for (curNode = 0; curNode < pDat->NodesDiscovered+1; curNode++)
		AmdPCIWrite(MAKE_SBDFO(makePCISegmentFromNode(curNode),
					makePCIBusFromNode(curNode),
					makePCIDeviceFromNode(curNode),
					CPU_ADDR_FUNC_01,
					REG_ADDR_CONFIG_MAP0_1XE0 + 4*cfgMapIndex),
					&temp);
}

/***************************************************************************//**
 *
 * static void
 * ht1SetCFGAddrMap(u8 cfgMapIndex, u8 secBus, u8 subBus, u8 targetNode, u8 targetLink, sMainData *pDat, cNorthBridge *nb)
 *
 *  Description:
 *	 Configure and enable config access to a non-coherent chain for the given bus range.
 *
 *  Parameters:
 *	@param[in]  cfgMapIndex = the map entry to set
 *	@param[in]  secBus      = The secondary bus number to use
 *	@param[in]  subBus      = The subordinate bus number to use
 *	@param[in]  targetNode  = The node  that shall be the recipient of the traffic
 *	@param[in]  targetLink  = The link that shall be the recipient of the traffic
 *	@param[in] pDat   = our global state
 *	@param[in] *nb   = this northbridge
 *
 ******************************************************************************/
static void ht1SetCFGAddrMap(u8 cfgMapIndex, u8 secBus, u8 subBus, u8 targetNode, u8 targetLink, sMainData *pDat, cNorthBridge *nb)
{
	u8 curNode;
	SBDFO linkBase;
	u32 temp;

	linkBase = makeLinkBase(targetNode, targetLink);

	ASSERT(secBus <= subBus);
	temp = secBus;
	AmdPCIWriteBits(linkBase + HTHOST_ISOC_REG, 15, 8, &temp);

	temp = subBus;
	AmdPCIWriteBits(linkBase + HTHOST_ISOC_REG, 23, 16, &temp);

	/* For target link, note that rev F uses bits 9:8 and only with GH is bit 10
	 * set to indicate a sublink.  For node, we are currently not supporting Extended
	 * routing tables.
	 */
	temp = ((u32)subBus << 24) + ((u32)secBus << 16) + ((u32)targetLink << 8)
		+ ((u32)targetNode << 4) + (u32)3;
	for (curNode = 0; curNode < pDat->NodesDiscovered+1; curNode++)
		 AmdPCIWrite(MAKE_SBDFO(makePCISegmentFromNode(curNode),
					makePCIBusFromNode(curNode),
					makePCIDeviceFromNode(curNode),
					CPU_ADDR_FUNC_01,
					REG_ADDR_CONFIG_MAP0_1XE0 + 4*cfgMapIndex),
					&temp);
}

/***************************************************************************
 ***				 Link Optimization			  ***
 ***************************************************************************/

/**
 * static u8
 * convertBitsToWidth(u8 value, cNorthBridge *nb)
 *
 *  Description:
 *	 Given the bits set in the register field, return the width it represents
 *
 *  Parameters:
 *	@param[in]  value   = The bits for the register
 *	@param[in] *nb = this northbridge
 *	@return  The width
 *
 ******************************************************************************/
static u8 convertBitsToWidth(u8 value, cNorthBridge *nb)
{
	switch(value) {
	case 1: return 16;
	case 0: return 8;
	case 5: return 4;
	case 4: return 2;
	default: STOP_HERE; /*  This is an error internal condition */
	}
	return 0; // shut up GCC.
}

/***************************************************************************//**
 *
 * static u8
 * convertWidthToBits(u8 value, cNorthBridge *nb)
 *
 *  Description:
 *	Translate a desired width setting to the bits to set in the register field
 *
 *  Parameters:
 *	@param[in]  value     = The width
 *	@param[in] *nb = this northbridge
 *	@return The bits for the register
 *
 ******************************************************************************/
static u8 convertWidthToBits(u8 value, cNorthBridge *nb)
{
	switch (value) {
	case 16: return 1;
	case  8: return 0;
	case  4: return 5;
	case  2: return 4;
	default: STOP_HERE; /*  This is an internal error condition */
	}
	return 0; // shut up GCC
}

/***************************************************************************//**
 *
 * static u16
 * ht1NorthBridgeFreqMask(u8 NodeID, cNorthBridge *nb)
 *
 *  Description:
 *	Return a mask that eliminates HT frequencies that cannot be used due to a slow
 *	northbridge frequency.
 *
 *  Parameters:
 *	@param[in]  node      = Result could (later) be for a specific node
 *	@param[in] *nb = this northbridge
 *	@return  Frequency mask
 *
 ******************************************************************************/
static uint32_t ht1NorthBridgeFreqMask(u8 node, cNorthBridge *nb)
{
	/* only up to HT1 speeds */
	return (HT_FREQUENCY_LIMIT_HT1_ONLY);
}

/***************************************************************************//**
 *
 * static u16
 * fam10NorthBridgeFreqMask(u8 NodeID, cNorthBridge *nb)
 *
 *  Description:
 *	Return a mask that eliminates HT frequencies that cannot be used due to a slow
 *	northbridge frequency.
 *
 *  Parameters:
 *	@param[in]  node     = Result could (later) be for a specific node
 *	@param[in]  *nb      = this northbridge
 *	@return  = Frequency mask
 *
 ******************************************************************************/
static uint32_t fam10NorthBridgeFreqMask(u8 node, cNorthBridge *nb)
{
	u8 nbCOF;
	uint32_t supported;

	nbCOF = getMinNbCOF();
	/*
	 * nbCOF is minimum northbridge speed in hundreds of MHz.
	 * HT can not go faster than the minimum speed of the northbridge.
	 */
	if ((nbCOF >= 6) && (nbCOF < 10))
	{
		/* Generation 1 HT link frequency */
		/* Convert frequency to bit and all less significant bits,
		 * by setting next power of 2 and subtracting 1.
		 */
		supported = ((uint32_t)1 << ((nbCOF >> 1) + 2)) - 1;
	}
	else if ((nbCOF >= 10) && (nbCOF <= 32))
	{
		/* Generation 3 HT link frequency
		 * Assume error retry is enabled on all Gen 3 links
		 */
		if (is_gt_rev_d()) {
			nbCOF *= 2;
			if (nbCOF > 32)
				nbCOF = 32;
		}

		/* Convert frequency to bit and all less significant bits,
		 * by setting next power of 2 and subtracting 1.
		 */
		supported = ((uint32_t)1 << ((nbCOF >> 1) + 2)) - 1;
	}
	else if (nbCOF > 32)
	{
		supported = HT_FREQUENCY_LIMIT_3200M;
	}
	/* unlikely cases, but include as a defensive measure, also avoid trick above */
	else if (nbCOF == 4)
	{
		supported = HT_FREQUENCY_LIMIT_400M;
	}
	else if (nbCOF == 2)
	{
		supported = HT_FREQUENCY_LIMIT_200M;
	}
	else
	{
		STOP_HERE;
		supported = HT_FREQUENCY_LIMIT_200M;
	}

	return (fixEarlySampleFreqCapability(supported));
}

/***************************************************************************//**
 *
 * static u16
 * fam15NorthBridgeFreqMask(u8 NodeID, cNorthBridge *nb)
 *
 *  Description:
 *	Return a mask that eliminates HT frequencies that cannot be used due to a slow
 *	northbridge frequency.
 *
 *  Parameters:
 *	@param[in]  node     = Result could (later) be for a specific node
 *	@param[in]  *nb      = this northbridge
 *	@return  = Frequency mask
 *
 ******************************************************************************/
static uint32_t fam15NorthBridgeFreqMask(u8 node, cNorthBridge *nb)
{
	u8 nbCOF;
	uint32_t supported;

	nbCOF = getMinNbCOF();
	/*
	 * nbCOF is minimum northbridge speed in hundreds of MHz.
	 * HT can not go faster than the minimum speed of the northbridge.
	 */
	if ((nbCOF >= 6) && (nbCOF < 10))
	{
		/* Generation 1 HT link frequency */
		/* Convert frequency to bit and all less significant bits,
		 * by setting next power of 2 and subtracting 1.
		 */
		supported = ((uint32_t)1 << ((nbCOF >> 1) + 2)) - 1;
	}
	else if ((nbCOF >= 10) && (nbCOF <= 32))
	{
		/* Generation 3 HT link frequency
		 * Assume error retry is enabled on all Gen 3 links
		 */
		nbCOF *= 2;
		if (nbCOF > 32)
			nbCOF = 32;

		/* Convert frequency to bit and all less significant bits,
		 * by setting next power of 2 and subtracting 1.
		 */
		supported = ((uint32_t)1 << ((nbCOF >> 1) + 2)) - 1;
	}
	else if (nbCOF > 32)
	{
		supported = HT_FREQUENCY_LIMIT_3200M;
	}
	/* unlikely cases, but include as a defensive measure, also avoid trick above */
	else if (nbCOF == 4)
	{
		supported = HT_FREQUENCY_LIMIT_400M;
	}
	else if (nbCOF == 2)
	{
		supported = HT_FREQUENCY_LIMIT_200M;
	}
	else
	{
		STOP_HERE;
		supported = HT_FREQUENCY_LIMIT_200M;
	}

	return (fixEarlySampleFreqCapability(supported));
}

/***************************************************************************//**
 *
 * static void
 * gatherLinkData(sMainData *pDat, cNorthBridge *nb)
 *
 *  Description:
 *	 For all discovered links, populate the port list with the frequency and width
 *	 capabilities.
 *
 *  Parameters:
 *	@param[in,out] pDat = our global state, port list
 *	@param[in]     *nb = this northbridge
 *
 ******************************************************************************/
static void gatherLinkData(sMainData *pDat, cNorthBridge *nb)
{
	u8 i;
	SBDFO linkBase;
	u32 temp;

	for (i = 0; i < pDat->TotalLinks*2; i++)
	{
		if (pDat->PortList[i].Type == PORTLIST_TYPE_CPU)
		{
			linkBase = makeLinkBase(pDat->PortList[i].NodeID, pDat->PortList[i].Link);

			pDat->PortList[i].Pointer = linkBase;

			AmdPCIReadBits(linkBase + HTHOST_LINK_CONTROL_REG, 22, 20, &temp);
			pDat->PortList[i].PrvWidthOutCap = convertBitsToWidth((u8)temp, pDat->nb);

			AmdPCIReadBits(linkBase + HTHOST_LINK_CONTROL_REG, 18, 16, &temp);
			pDat->PortList[i].PrvWidthInCap = convertBitsToWidth((u8)temp, pDat->nb);

			AmdPCIReadBits(linkBase + HTHOST_FREQ_REV_REG, 31, 16, &temp);
			pDat->PortList[i].PrvFrequencyCap = temp & 0x7FFF	/*  Mask off bit 15, reserved value */
				& nb->northBridgeFreqMask(pDat->PortList[i].NodeID, pDat->nb);
			if (is_gt_rev_d()) {
				AmdPCIReadBits(linkBase + HTHOST_FREQ_REV_REG_2, 15, 1, &temp);
				temp &= 0x7;	/* Mask off reserved values */
				pDat->PortList[i].PrvFrequencyCap |= (temp << 17);
			}

			AmdPCIReadBits(linkBase + HTHOST_FEATURE_CAP_REG, 9, 0, &temp);
			pDat->PortList[i].PrvFeatureCap = (u16)temp;
		}
		else
		{
			linkBase = pDat->PortList[i].Pointer;
			if (pDat->PortList[i].Link == 1)
				linkBase += HTSLAVE_LINK01_OFFSET;

			AmdPCIReadBits(linkBase + HTSLAVE_LINK_CONTROL_0_REG, 22, 20, &temp);
			pDat->PortList[i].PrvWidthOutCap = convertBitsToWidth((u8)temp, pDat->nb);

			AmdPCIReadBits(linkBase + HTSLAVE_LINK_CONTROL_0_REG, 18, 16, &temp);
			pDat->PortList[i].PrvWidthInCap = convertBitsToWidth((u8)temp, pDat->nb);

			AmdPCIReadBits(linkBase + HTSLAVE_FREQ_REV_0_REG, 31, 16, &temp);
			pDat->PortList[i].PrvFrequencyCap = (u16)temp;

			AmdPCIReadBits(linkBase + HTSLAVE_FEATURE_CAP_REG, 7, 0, &temp);
			pDat->PortList[i].PrvFeatureCap = (u16)temp;

			if (pDat->HtBlock->AMD_CB_DeviceCapOverride)
			{
				linkBase &= 0xFFFFF000;
				AmdPCIRead(linkBase, &temp);

				pDat->HtBlock->AMD_CB_DeviceCapOverride(
					pDat->PortList[i].NodeID,
					pDat->PortList[i].HostLink,
					pDat->PortList[i].HostDepth,
					(u8)SBDFO_SEG(pDat->PortList[i].Pointer),
					(u8)SBDFO_BUS(pDat->PortList[i].Pointer),
					(u8)SBDFO_DEV(pDat->PortList[i].Pointer),
					temp,
					pDat->PortList[i].Link,
					&(pDat->PortList[i].PrvWidthInCap),
					&(pDat->PortList[i].PrvWidthOutCap),
					&(pDat->PortList[i].PrvFrequencyCap),
					&(pDat->PortList[i].PrvFeatureCap));
			}
		}
	}
}

/***************************************************************************//**
 *
 * static void
 * setLinkData(sMainData *pDat, cNorthBridge *nb)
 *
 *  Description:
 *	 Change the hardware state for all links according to the now optimized data in the
 *	 port list data structure.
 *
 *  Parameters:
 *	  @param[in]  pDat = our global state, port list
 *	  @param[in]  *nb   = this northbridge
 *
 ******************************************************************************/
static void setLinkData(sMainData *pDat, cNorthBridge *nb)
{
	u8 i;
	SBDFO linkBase;
	u32 temp, temp2, frequency_index, widthin, widthout, bits;

	for (i = 0; i < pDat->TotalLinks*2; i++)
	{

		ASSERT(pDat->PortList[i&0xFE].SelWidthOut == pDat->PortList[(i&0xFE)+1].SelWidthIn);
		ASSERT(pDat->PortList[i&0xFE].SelWidthIn == pDat->PortList[(i&0xFE)+1].SelWidthOut);
		ASSERT(pDat->PortList[i&0xFE].SelFrequency == pDat->PortList[(i&0xFE)+1].SelFrequency);

		if (pDat->PortList[i].SelRegang)
		{
			ASSERT(pDat->PortList[i].Type == PORTLIST_TYPE_CPU);
			ASSERT(pDat->PortList[i].Link < 4);
			temp = 1;
			AmdPCIWriteBits(MAKE_SBDFO(makePCISegmentFromNode(pDat->PortList[i].NodeID),
					makePCIBusFromNode(pDat->PortList[i].NodeID),
					makePCIDeviceFromNode(pDat->PortList[i].NodeID),
					CPU_HTNB_FUNC_00,
					REG_HT_LINK_EXT_CONTROL0_0X170 + 4*pDat->PortList[i].Link),
					0, 0, &temp);
		}

		if (pDat->PortList[i].Type == PORTLIST_TYPE_CPU)
		{
			if (pDat->HtBlock->AMD_CB_OverrideCpuPort)
				pDat->HtBlock->AMD_CB_OverrideCpuPort(pDat->PortList[i].NodeID,
						pDat->PortList[i].Link,
						&(pDat->PortList[i].SelWidthIn),
						&(pDat->PortList[i].SelWidthOut),
						&(pDat->PortList[i].SelFrequency));
		}
		else
		{
			if (pDat->HtBlock->AMD_CB_OverrideDevicePort)
				pDat->HtBlock->AMD_CB_OverrideDevicePort(pDat->PortList[i].NodeID,
							pDat->PortList[i].HostLink,
							pDat->PortList[i].HostDepth,
							pDat->PortList[i].Link,
							&(pDat->PortList[i].SelWidthIn),
							&(pDat->PortList[i].SelWidthOut),
							&(pDat->PortList[i].SelFrequency));
		}

		linkBase = pDat->PortList[i].Pointer;
		if ((pDat->PortList[i].Type == PORTLIST_TYPE_IO) && (pDat->PortList[i].Link == 1))
			linkBase += HTSLAVE_LINK01_OFFSET;

		/* Some IO devices don't work properly when setting widths, so write them in a single operation,
		 * rather than individually.
		 */
		widthout = convertWidthToBits(pDat->PortList[i].SelWidthOut, pDat->nb);
		ASSERT(widthout == 1 || widthout == 0 || widthout == 5 || widthout == 4);
		widthin = convertWidthToBits(pDat->PortList[i].SelWidthIn, pDat->nb);
		ASSERT(widthin == 1 || widthin == 0 || widthin == 5 || widthin == 4);

		temp = (widthin & 7) | ((widthout & 7) << 4);
		setHtControlRegisterBits(linkBase + HTHOST_LINK_CONTROL_REG, 31, 24, &temp);

		temp = pDat->PortList[i].SelFrequency;
		if (pDat->PortList[i].Type == PORTLIST_TYPE_CPU)
		{
			ASSERT((temp >= HT_FREQUENCY_600M && temp <= HT_FREQUENCY_3200M)
				|| (temp == HT_FREQUENCY_200M) || (temp == HT_FREQUENCY_400M));
			frequency_index = temp;
			if (temp > 0xf) {
				temp2 = (temp >> 4) & 0x1;
				temp &= 0xf;
			} else {
				temp2 = 0x0;
			}
			/* NOTE
			 * The Family 15h BKDG Rev. 3.14 is wrong
			 * Freq[4] must be set before Freq[3:0], otherwise the register writes will be ignored!
			 */
			if (is_gt_rev_d())
				AmdPCIWriteBits(linkBase + HTHOST_FREQ_REV_REG_2, 0, 0, &temp2);
			AmdPCIWriteBits(linkBase + HTHOST_FREQ_REV_REG, 11, 8, &temp);

			/* Enable isochronous flow control mode if supported by chipset */
			if (is_fam15h()) {
				if (pDat->PortList[i].enable_isochronous_mode)
					temp = 1;
				else
					temp = 0;
				setHtControlRegisterBits(linkBase + HTHOST_LINK_CONTROL_REG, 12, 12, &temp);
			}

			if (frequency_index > HT_FREQUENCY_1000M) /*  Gen1 = 200MHz -> 1000MHz, Gen3 = 1200MHz -> 3200MHz */
			{
				/* Enable  for Gen3 frequencies */
				temp = 1;
			}
			else
			{
				/* Disable  for Gen1 frequencies */
				temp = 0;
			}
			/* HT3 retry mode enable / disable */
			AmdPCIWriteBits(MAKE_SBDFO(makePCISegmentFromNode(pDat->PortList[i].NodeID),
						makePCIBusFromNode(pDat->PortList[i].NodeID),
						makePCIDeviceFromNode(pDat->PortList[i].NodeID),
						CPU_HTNB_FUNC_00,
						REG_HT_LINK_RETRY0_0X130 + 4*pDat->PortList[i].Link),
						0, 0, &temp);

			/* and Scrambling enable / disable */
			AmdPCIWriteBits(MAKE_SBDFO(makePCISegmentFromNode(pDat->PortList[i].NodeID),
					makePCIBusFromNode(pDat->PortList[i].NodeID),
					makePCIDeviceFromNode(pDat->PortList[i].NodeID),
					CPU_HTNB_FUNC_00,
					REG_HT_LINK_EXT_CONTROL0_0X170 + 4*pDat->PortList[i].Link),
					3, 3, &temp);
		}
		else
		{
			SBDFO currentPtr;
			BOOL isFound;

			ASSERT(temp <= HT_FREQUENCY_3200M);
			/* Write the frequency setting */
			AmdPCIWriteBits(linkBase + HTSLAVE_FREQ_REV_0_REG, 11, 8, &temp);

			/* Handle additional HT3 frequency requirements, if needed,
			 * or clear them if switching down to ht1 on a warm reset.
			 * Gen1 = 200MHz -> 1000MHz, Gen3 = 1200MHz -> 2600MHz
			 *
			 * Even though we assert if debugging, we need to check that the capability was found
			 * always, since this is an unknown hardware device, also we are taking
			 * unqualified frequency from the call backs
			 * (could be trying to do ht3 on an ht1 IO device).
			 */

			if (temp > HT_FREQUENCY_1000M)
			{
				/* Enabling features if gen 3 */
				bits = 1;
			}
			else
			{
				/* Disabling features if gen 1 */
				bits = 0;
			}

			/* Enable isochronous flow control mode if supported by chipset */
			if (is_fam15h()) {
				if (pDat->PortList[i].enable_isochronous_mode)
					temp = 1;
				else
					temp = 0;
			}

			/* Retry Enable */
			isFound = FALSE;
			currentPtr = linkBase & (u32)0xFFFFF000; /* Set PCI Offset to 0 */
			do
			{
				AmdPCIFindNextCap(&currentPtr);
				if (currentPtr != ILLEGAL_SBDFO)
				{
					AmdPCIRead(currentPtr, &temp);
					/* HyperTransport Retry Capability? */
					if (IS_HT_RETRY_CAPABILITY(temp))
					{
						ASSERT(pDat->PortList[i].Link < 2);
						AmdPCIWriteBits(currentPtr + HTRETRY_CONTROL_REG,
								pDat->PortList[i].Link*16,
								pDat->PortList[i].Link*16,
								&bits);
						isFound = TRUE;
					}
				/* Some other capability, keep looking */
				}
				else
				{
					/* If we are turning it off, that may mean the device was only ht1 capable,
					 * so don't complain that we can't do it.
					 */
					if (bits != 0)
					{
						if (pDat->HtBlock->AMD_CB_EventNotify)
						{
							sHtEventOptRequiredCap evt;
							evt.eSize = sizeof(sHtEventOptRequiredCap);
							evt.node = pDat->PortList[i].NodeID;
							evt.link = pDat->PortList[i].HostLink;
							evt.depth = pDat->PortList[i].HostDepth;

							pDat->HtBlock->AMD_CB_EventNotify(HT_EVENT_CLASS_WARNING,
										HT_EVENT_OPT_REQUIRED_CAP_RETRY,
										(u8 *)&evt);
						}
						STOP_HERE;
					}
					isFound = TRUE;
				}
			} while (!isFound);

			/* Scrambling enable */
			isFound = FALSE;
			currentPtr = linkBase & (u32)0xFFFFF000; /* Set PCI Offset to 0 */
			do
			{
				AmdPCIFindNextCap(&currentPtr);
				if (currentPtr != ILLEGAL_SBDFO)
				{
					AmdPCIRead(currentPtr, &temp);
					/* HyperTransport Gen3 Capability? */
					if (IS_HT_GEN3_CAPABILITY(temp))
					{
						ASSERT(pDat->PortList[i].Link < 2);
						AmdPCIWriteBits((currentPtr +
							HTGEN3_LINK_TRAINING_0_REG +
							pDat->PortList[i].Link*HTGEN3_LINK01_OFFSET),
							3, 3, &bits);
						isFound = TRUE;
					}
					/* Some other capability, keep looking */
					}
					else
					{
					/* If we are turning it off, that may mean the device was only ht1 capable,
					 * so don't complain that we can't do it.
					 */
					if (bits != 0)
					{
						if (pDat->HtBlock->AMD_CB_EventNotify)
						{
							sHtEventOptRequiredCap evt;
							evt.eSize = sizeof(sHtEventOptRequiredCap);
							evt.node = pDat->PortList[i].NodeID;
							evt.link = pDat->PortList[i].HostLink;
							evt.depth = pDat->PortList[i].HostDepth;

							pDat->HtBlock->AMD_CB_EventNotify(HT_EVENT_CLASS_WARNING,
										HT_EVENT_OPT_REQUIRED_CAP_GEN3,
										(u8 *)&evt);
						}
						STOP_HERE;
					}
					isFound = TRUE;
				}
			} while (!isFound);
		}
	}
}

/***************************************************************************//**
 *
 * void
 * fam0fWriteHTLinkCmdBufferAlloc(u8 node, u8 link, u8 req, u8 preq, u8 rsp, u8 prb)
 *
 *  Description:
 *	Set the command buffer allocations in the buffer count register for the node and link.
 *	The command buffer settings in the low 16 bits are the same on both
 *	family 10h and family 0fh northbridges.
 *
 *  Parameters:
 *	@param[in] node = The node to set allocations on
 *	@param[in] link = the link to set allocations on
 *	@param[in] req  = non-posted Request Command Buffers
 *	@param[in] preq = Posted Request Command Buffers
 *	@param[in] rsp  = Response Command Buffers
 *	@param[in] prb  = Probe Command Buffers
 *
 ******************************************************************************/
#ifndef HT_BUILD_NC_ONLY

static void fam0fWriteHTLinkCmdBufferAlloc(u8 node, u8 link, u8 req, u8 preq, u8 rsp, u8 prb)
{
	u32 temp;
	SBDFO currentPtr;

	currentPtr = makeLinkBase(node, link);
	currentPtr += HTHOST_BUFFER_COUNT_REG;

	/* non-posted Request Command Buffers */
	temp = req;
	AmdPCIWriteBits(currentPtr, 3, 0, &temp);
	/* Posted Request Command Buffers */
	temp = preq;
	AmdPCIWriteBits(currentPtr, 7, 4, &temp);
	/* Response Command Buffers */
	temp = rsp;
	AmdPCIWriteBits(currentPtr, 11, 8, &temp);
	/* Probe Command Buffers */
	temp = prb;
	AmdPCIWriteBits(currentPtr, 15, 12, &temp);
	/* LockBc */
	temp = 1;
	AmdPCIWriteBits(currentPtr, 31, 31, &temp);
}
#endif /* HT_BUILD_NC_ONLY */

/***************************************************************************//**
 *
 * void
 * fam0fWriteHTLinkDatBufferAlloc(u8 node, u8 link, u8 reqD, u8 preqD, u8 rspD)
 *
 *  Description:
 *	 Set the data buffer allocations in the buffer count register for the node and link.
 *	 The command buffer settings in the high 16 bits are not the same on both
 *	 family 10h and family 0fh northbridges.
 *
 *  Parameters:
 *	@param[in] node  = The node to set allocations on
 *	@param[in] link  = the link to set allocations on
 *	@param[in] reqD  = non-posted Request Data Buffers
 *	@param[in] preqD = Posted Request Data Buffers
 *	@param[in] rspD  = Response Data Buffers
 *
 ******************************************************************************/
#ifndef HT_BUILD_NC_ONLY

static void fam0fWriteHTLinkDatBufferAlloc(u8 node, u8 link, u8 reqD, u8 preqD, u8 rspD)
{
	u32 temp;
	SBDFO currentPtr;

	currentPtr = makeLinkBase(node, link);
	currentPtr += HTHOST_BUFFER_COUNT_REG;

	/* Request Data Buffers */
	temp = reqD;
	AmdPCIWriteBits(currentPtr, 18, 16, &temp);
	/* Posted Request Data Buffers */
	temp = preqD;
	AmdPCIWriteBits(currentPtr, 22, 20, &temp);
	/* Response Data Buffers */
	temp = rspD;
	AmdPCIWriteBits(currentPtr, 26, 24, &temp);
}
#endif /* HT_BUILD_NC_ONLY */

/***************************************************************************//**
 *
 * static void
 * ht3WriteTrafficDistribution(u32 links01, u32 links10, cNorthBridge *nb)
 *
 *  Description:
 *	 Set the traffic distribution register for the links provided.
 *
 *  Parameters:
 *	@param[in]  links01   = coherent links from node 0 to 1
 *	@param[in]  links10   = coherent links from node 1 to 0
 *	@param[in]  nb = this northbridge
 *
 ******************************************************************************/
static void ht3WriteTrafficDistribution(u32 links01, u32 links10, cNorthBridge *nb)
{
#ifndef HT_BUILD_NC_ONLY
	u32 temp;

	/* Node 0 */
	/* DstLnk */
	AmdPCIWriteBits(MAKE_SBDFO(makePCISegmentFromNode(0),
			makePCIBusFromNode(0),
			makePCIDeviceFromNode(0),
			CPU_HTNB_FUNC_00,
			REG_HT_TRAFFIC_DIST_0X164),
			23, 16, &links01);
	/* DstNode = 1, cHTPrbDistEn = 1, cHTRspDistEn = 1, cHTReqDistEn = 1 */
	temp = 0x0107;
	AmdPCIWriteBits(MAKE_SBDFO(makePCISegmentFromNode(0),
			makePCIBusFromNode(0),
			makePCIDeviceFromNode(0),
			CPU_HTNB_FUNC_00,
			REG_HT_TRAFFIC_DIST_0X164),
			15, 0, &temp);

	/* Node 1 */
	/* DstLnk */
	AmdPCIWriteBits(MAKE_SBDFO(makePCISegmentFromNode(1),
			makePCIBusFromNode(1),
			makePCIDeviceFromNode(1),
			CPU_HTNB_FUNC_00,
			REG_HT_TRAFFIC_DIST_0X164),
			23, 16, &links10);
	/* DstNode = 0, cHTPrbDistEn = 1, cHTRspDistEn = 1, cHTReqDistEn = 1 */
	temp = 0x0007;
	AmdPCIWriteBits(MAKE_SBDFO(makePCISegmentFromNode(1),
			makePCIBusFromNode(1),
			makePCIDeviceFromNode(1),
			CPU_HTNB_FUNC_00,
			REG_HT_TRAFFIC_DIST_0X164),
			15, 0, &temp);
#endif /* HT_BUILD_NC_ONLY */
}

/***************************************************************************//**
 *
 * static void
 * ht1WriteTrafficDistribution(u32 links01, u32 links10, cNorthBridge *nb)
 *
 *  Description:
 *	 Traffic distribution is more complex in this case as the routing table must be
 *	 adjusted to use one link for requests and the other for responses.  Also,
 *	 perform the buffer tunings on the links required for this config.
 *
 *  Parameters:
 *	@param[in]  links01  = coherent links from node 0 to 1
 *	@param[in]  links10  = coherent links from node 1 to 0
 *	@param[in]  nb = this northbridge
 *
 ******************************************************************************/
static void ht1WriteTrafficDistribution(u32 links01, u32 links10, cNorthBridge *nb)
{
#ifndef HT_BUILD_NC_ONLY
	u32 route01, route10;
	u8 req0, req1, rsp0, rsp1, nclink;

	/*
	 * Get the current request route for 0->1 and 1->0.  This will indicate which of the links
	 * in links01 are connected to which links in links10.  Since we have to route to distribute
	 * traffic, we need to know that.	The link used by htinit will become the request, probe link.
	 * the other link will be used for responses.
	 */

	/* Get the routes, and hang on to them, we will write them back updated. */
	AmdPCIRead(MAKE_SBDFO(makePCISegmentFromNode(0),
				makePCIBusFromNode(0),
				makePCIDeviceFromNode(0),
				CPU_HTNB_FUNC_00,
				REG_ROUTE1_0X44),
				&route01);
	AmdPCIRead(MAKE_SBDFO(makePCISegmentFromNode(1),
				makePCIBusFromNode(1),
				makePCIDeviceFromNode(1),
				CPU_HTNB_FUNC_00,
				REG_ROUTE0_0X40),
				&route10);

	/* Convert the request routes to a link number.  Note "0xE" is ht1 nb specific.
	 * Find the response link numbers.
	*/
	ASSERT((route01 & 0xE) && (route10 & 0xE));    /* no route! error! */
	req0 = (u8)AmdBitScanReverse((route01 & 0xE)) - 1;
	req1 = (u8)AmdBitScanReverse((route10 & 0xE)) - 1;
	/* Now, find the other link for the responses */
	rsp0 = (u8)AmdBitScanReverse((links01 & ~((u32)1 << req0)));
	rsp1 = (u8)AmdBitScanReverse((links10 & ~((u32)1 << req1)));

	/* ht1 nb restriction, must have exactly two links */
	ASSERT(((((links01 & ~((u32)1 << req0)) & ~((u32)1 << rsp0))) == 0)
		&& ((((links10 & ~((u32)1 << req1)) & ~((u32)1 << rsp1))) == 0));

	route01 = (route01 & ~0x0E00) | ((u32)0x0100<<(rsp0 + 1));
	route10 = (route10 & ~0x0E00) | ((u32)0x0100<<(rsp1 + 1));

	AmdPCIWrite(MAKE_SBDFO(makePCISegmentFromNode(0),
				makePCIBusFromNode(0),
				makePCIDeviceFromNode(0),
				CPU_HTNB_FUNC_00,
				REG_ROUTE1_0X44),
				&route01);

	AmdPCIWrite(MAKE_SBDFO(makePCISegmentFromNode(1),
				makePCIBusFromNode(1),
				makePCIDeviceFromNode(1),
				CPU_HTNB_FUNC_00,
				REG_ROUTE0_0X40),
				&route10);

	/* While we otherwise do buffer tunings elsewhere, for the dual cHT DP case with
	 * ht1 northbridges like family 0Fh, do the tunings here where we have all the
	 * link and route info at hand and don't need to recalculate it.
	 */

	/* Node 0, Request / Probe Link (note family F only has links < 4) */
	fam0fWriteHTLinkCmdBufferAlloc(0, req0, 6, 3, 1, 6);
	fam0fWriteHTLinkDatBufferAlloc(0, req0, 4, 3, 1);
	/* Node 0, Response Link (note family F only has links < 4) */
	fam0fWriteHTLinkCmdBufferAlloc(0, rsp0, 1, 0, 15, 0);
	fam0fWriteHTLinkDatBufferAlloc(0, rsp0, 1, 1, 6);
	/* Node 1, Request / Probe Link (note family F only has links < 4) */
	fam0fWriteHTLinkCmdBufferAlloc(1, req1, 6, 3, 1, 6);
	fam0fWriteHTLinkDatBufferAlloc(1, req1, 4, 3, 1);
	/* Node 1, Response Link (note family F only has links < 4) */
	fam0fWriteHTLinkCmdBufferAlloc(1, rsp1, 1, 0, 15, 0);
	fam0fWriteHTLinkDatBufferAlloc(1, rsp1, 1, 1, 6);

	/* Node 0, is the third link non-coherent? */
	nclink = (u8)AmdBitScanReverse(((u8)0x07 & ~((u32)1 << req0) & ~((u32)1 << rsp0)));
	if (nb->verifyLinkIsNonCoherent(0, nclink, nb))
	{
		fam0fWriteHTLinkCmdBufferAlloc(0, nclink, 6, 5, 2, 0);
	}

	/* Node 1, is the third link non-coherent? */
	nclink = (u8)AmdBitScanReverse(((u8)0x07 & ~((u32)1 << req1) & ~((u32)1 << rsp1)));
	if (nb->verifyLinkIsNonCoherent(1, nclink, nb))
	{
		fam0fWriteHTLinkCmdBufferAlloc(1, nclink, 6, 5, 2, 0);
	}
#endif /* HT_BUILD_NC_ONLY */
}

/***************************************************************************//**
 *
 * static void
 * fam0fBufferOptimizations(u8 node, sMainData *pDat, cNorthBridge *nb)
 *
 *  Description:
 *	 Buffer tunings are inherently northbridge specific. Check for specific configs
 *	 which require adjustments and apply any standard workarounds to this node.
 *
 *  Parameters:
 *	@param[in]  node      = the node to
 *	@param[in] *pDat  = coherent links from node 0 to 1
 *	@param[in]  nb = this northbridge
 *
 ******************************************************************************/
static void fam0fBufferOptimizations(u8 node, sMainData *pDat, cNorthBridge *nb)
{
#ifndef HT_BUILD_NC_ONLY
	u8 i;
	u32 temp;
	SBDFO currentPtr;

	ASSERT(node < nb->maxNodes);

	/* Fix the FIFO pointer register before changing speeds */
	currentPtr = MAKE_SBDFO(makePCISegmentFromNode(node),
				makePCIBusFromNode(node),
				makePCIDeviceFromNode(node),
				CPU_NB_FUNC_03,
				REG_NB_FIFOPTR_3XDC);
	for (i = 0; i < nb->maxLinks; i++)
	{
		temp = 0;
		if (nb->verifyLinkIsCoherent(node, i, nb))
		{
			temp = 0x26;
			ASSERT(i < 3);
			AmdPCIWriteBits(currentPtr, 8*i + 5, 8*i, &temp);
		}
		else
		{
			if (nb->verifyLinkIsNonCoherent(node, i, nb))
			{
				temp = 0x25;
				ASSERT(i < 3);
				AmdPCIWriteBits(currentPtr, 8*i + 5, 8*i, &temp);
			}
		}
	}
	/*
	 * 8P Buffer tuning.
	 * Either apply the BKDG tunings or, if applicable, apply the more restrictive errata 153
	 * workaround.
	 * If 8 nodes, Check this node for 'inner' or 'outer'.
	 * Tune each link based on coherent or non-coherent
	 */
	if (pDat->NodesDiscovered >= 6)
	{
		u8 j;
		BOOL isOuter;
		BOOL isErrata153;

		/* This is for family 0Fh, so assuming dual core max then 7 or 8 nodes are required
		 * to be in the situation of 14 or more cores.	 We checked nodes above, cross check
		 * that the number of cores is 14 or more. We want both 14 cores with at least 7 or 8 nodes
		 * not one condition alone, to apply the errata 153 workaround.  Otherwise, 7 or 8 rev F
		 * nodes use the BKDG tuning.
		 */

		isErrata153 = 0;

		AmdPCIReadBits (MAKE_SBDFO(makePCISegmentFromNode(0),
					makePCIBusFromNode(0),
					makePCIDeviceFromNode(0),
					CPU_HTNB_FUNC_00,
					REG_NODE_ID_0X60),
					19, 16, &temp);

		if (temp >= 14)
		{
			/* Check whether we need to do errata 153 tuning or BKDG tuning.
			 * Errata 153 applies to JH-1, JH-2 and older.  It is fixed in JH-3
			 * (and, one assumes, from there on).
			 */
			for (i = 0; i < (pDat->NodesDiscovered +1); i++)
			{
				AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(i),
						makePCIBusFromNode(i),
						makePCIDeviceFromNode(i),
						CPU_NB_FUNC_03,
						REG_NB_CPUID_3XFC),
						7, 0, &temp);
				if (((u8)temp & ~0x40) < 0x13)
				{
					isErrata153 = 1;
					break;
				}
			}
		}

		for (i = 0; i < CPU_ADDR_NUM_CONFIG_MAPS; i++)
		{
			isOuter = FALSE;
			/* Check for outer node by scanning the config maps on node 0 for one
			 * which is assigned to this node.
			 */
			currentPtr = MAKE_SBDFO(makePCISegmentFromNode(0),
						makePCIBusFromNode(0),
						makePCIDeviceFromNode(0),
						CPU_ADDR_FUNC_01,
						REG_ADDR_CONFIG_MAP0_1XE0 + (4 * i));
			AmdPCIReadBits (currentPtr, 1, 0, &temp);
			/* Make sure this config map is valid, if it is it will be enabled for read/write */
			if (temp == 3)
			{
				/* It's valid, get the node (that node is an outer node) */
				AmdPCIReadBits (currentPtr, 6, 4, &temp);
				/* Is the node we're working on now? */
				if (node == (u8)temp)
				{
					/* This is an outer node.	Tune it appropriately. */
					for (j = 0; j < nb->maxLinks; j++)
					{
						if (isErrata153)
						{
							if (nb->verifyLinkIsCoherent(node, j, nb))
							{
								fam0fWriteHTLinkCmdBufferAlloc(node, j, 1, 1, 6, 4);
							}
							else
							{
								if (nb->verifyLinkIsNonCoherent(node, j, nb))
								{
									fam0fWriteHTLinkCmdBufferAlloc(node, j, 5, 4, 1, 0);
								}
							}
						}
						else
						{
							if (nb->verifyLinkIsCoherent(node, j, nb))
							{
								fam0fWriteHTLinkCmdBufferAlloc(node, j, 1, 1, 8, 5);
							}
						}
					}
					/*
					 * SRI to XBAR Buffer Counts are correct for outer node at power on defaults.
					 */
					isOuter = TRUE;
					break;
				}
			}
			/* We fill config maps in ascending order, so if we didn't use this one, we're done. */
			else break;
		}
		if (!isOuter)
		{
			if (isErrata153)
			{
				/* Tuning for inner node coherent links */
				for (j = 0; j < nb->maxLinks; j++)
				{
					if (nb->verifyLinkIsCoherent(node, j, nb))
					{
						fam0fWriteHTLinkCmdBufferAlloc(node, j, 2, 1, 5, 4);
					}

				}
				/* SRI to XBAR Buffer Count for inner nodes, zero DReq and DPReq */
				temp = 0;
				AmdPCIWriteBits (MAKE_SBDFO(makePCISegmentFromNode(node),
							makePCIBusFromNode(node),
							makePCIDeviceFromNode(node),
							CPU_NB_FUNC_03,
							REG_NB_SRI_XBAR_BUF_3X70),
							31, 28, &temp);
			}
		}

		/*
		 * Tune MCT to XBAR Buffer Count the same an all nodes, 2 Probes, 5 Response
		 */
		if (isErrata153)
		{
			temp = 0x25;
			AmdPCIWriteBits (MAKE_SBDFO(makePCISegmentFromNode(node),
						makePCIBusFromNode(node),
						makePCIDeviceFromNode(node),
						CPU_NB_FUNC_03,
						REG_NB_MCT_XBAR_BUF_3X78),
						14, 8, &temp);
		}
	}
#endif /* HT_BUILD_NC_ONLY */
}

/***************************************************************************//**
 *
 * static void
 * fam10BufferOptimizations(u8 node, sMainData *pDat, cNorthBridge *nb)
 *
 *  Description:
 *	 Buffer tunings are inherently northbridge specific. Check for specific configs
 *	 which require adjustments and apply any standard workarounds to this node.
 *
 *  Parameters:
 *	@param[in] node       = the node to tune
 *	@param[in] *pDat  = global state
 *	@param[in] nb = this northbridge
 *
 ******************************************************************************/
static void fam10BufferOptimizations(u8 node, sMainData *pDat, cNorthBridge *nb)
{
	u32 temp;
	SBDFO currentPtr;
	u8 i;

	ASSERT(node < nb->maxNodes);

	/*
	 * Link to XCS Token Count Tuning
	 *
	 * For each active link that we reganged (so this unfortunately can't go into the PCI reg
	 * table), we have to switch the Link to XCS Token Counts to the ganged state.
	 * We do this here for the non-uma case, which is to write the values that would have
	 * been power on defaults if the link was ganged at cold reset.
	 */
	for (i = 0; i < pDat->TotalLinks*2; i++)
	{
		if ((pDat->PortList[i].NodeID == node) && (pDat->PortList[i].Type == PORTLIST_TYPE_CPU))
		{
			/* If the link is greater than 4, this is a sublink 1, so it is not reganged. */
			if (pDat->PortList[i].Link < 4)
			{
				currentPtr = MAKE_SBDFO(makePCISegmentFromNode(node),
						makePCIBusFromNode(node),
						makePCIDeviceFromNode(node),
						CPU_NB_FUNC_03,
						REG_NB_LINK_XCS_TOKEN0_3X148 + 4*pDat->PortList[i].Link);
				if (pDat->PortList[i].SelRegang)
				{
					/* Handle all the regang Token count adjustments */

					/* Sublink 0: [Probe0tok] = 2 [Rsp0tok] = 2 [PReq0tok] = 2 [Req0tok] = 2 */
					temp = 0xAA;
					AmdPCIWriteBits(currentPtr, 7, 0, &temp);
					/* Sublink 1: [Probe1tok] = 0 [Rsp1tok] = 0 [PReq1tok] = 0 [Req1tok] = 0 */
					temp = 0;
					AmdPCIWriteBits(currentPtr, 23, 16, &temp);
					/* [FreeTok] = 3 */
					temp = 3;
					AmdPCIWriteBits(currentPtr, 15, 14, &temp);
				}
				else
				{
					/* Read the regang bit in hardware */
					AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(pDat->PortList[i].NodeID),
							makePCIBusFromNode(pDat->PortList[i].NodeID),
							makePCIDeviceFromNode(pDat->PortList[i].NodeID),
							CPU_HTNB_FUNC_00,
							REG_HT_LINK_EXT_CONTROL0_0X170 + 4*pDat->PortList[i].Link),
							0, 0, &temp);
					if (temp == 1)
					{
						/* handle a minor adjustment for stapped ganged links.	 If SelRegang is false we
						 * didn't do the regang, so if the bit is on then it's hardware strapped.
						 */

						/* [FreeTok] = 3 */
						temp = 3;
						AmdPCIWriteBits(currentPtr, 15, 14, &temp);
					}
				}
			}
		}
	}
}

/***************************************************************************//**
 *
 * static void
 * fam15BufferOptimizations(u8 node, sMainData *pDat, cNorthBridge *nb)
 *
 *  Description:
 *	 Buffer tunings are inherently northbridge specific. Check for specific configs
 *	 which require adjustments and apply any standard workarounds to this node.
 *
 *  Parameters:
 *	@param[in] node       = the node to tune
 *	@param[in] *pDat  = global state
 *	@param[in] nb = this northbridge
 *
 ******************************************************************************/
static void fam15BufferOptimizations(u8 node, sMainData *pDat, cNorthBridge *nb)
{
	/* Buffer count setup on Family 15h is currently handled in cpuSetAMDPCI */
}

/*
 * North Bridge 'constructor'.
 *
 */

/***************************************************************************//**
 *
 * void
 * newNorthBridge(u8 node, cNorthBridge *nb)
 *
 *  Description:
 *	 Construct a new northbridge.  This routine encapsulates knowledge of how to tell
 *	 significant differences between families of supported northbridges and what routines
 *	 can be used in common and which are unique.  A fully populated northbridge interface
 *	 is provided by nb.
 *
 *  Parameters:
 *	  @param            node
 *	  @param[out]	    nb		 = the caller's northbridge structure to initialize.
 *
 ******************************************************************************/
void newNorthBridge(u8 node, cNorthBridge *nb)
{
	u32 match;
	u32 extFam, baseFam, model;

	cNorthBridge fam15 =
	{
#ifdef HT_BUILD_NC_ONLY
		8,
		1,
		12,
#else
		8,
		8,
		64,
#endif /* HT_BUILD_NC_ONLY*/
		writeRoutingTable,
		writeNodeID,
		readDefLnk,
		enableRoutingTables,
		verifyLinkIsCoherent,
		readTrueLinkFailStatus,
		readToken,
		writeToken,
		fam15GetNumCoresOnNode,
		setTotalNodesAndCores,
		limitNodes,
		writeFullRoutingTable,
		isCompatible,
		fam15IsCapable,
		(void (*)(u8, u8, cNorthBridge*))commonVoid,
		(BOOL (*)(u8, u8, sMainData*, cNorthBridge*))commonReturnFalse,
		readSbLink,
		verifyLinkIsNonCoherent,
		ht3SetCFGAddrMap,
		convertBitsToWidth,
		convertWidthToBits,
		fam15NorthBridgeFreqMask,
		gatherLinkData,
		setLinkData,
		ht3WriteTrafficDistribution,
		fam15BufferOptimizations,
		0x00000001,
		0x00000200,
		18,
		0x00000f06
	};

	cNorthBridge fam10 =
	{
#ifdef HT_BUILD_NC_ONLY
		8,
		1,
		12,
#else
		8,
		8,
		64,
#endif /* HT_BUILD_NC_ONLY*/
		writeRoutingTable,
		writeNodeID,
		readDefLnk,
		enableRoutingTables,
		verifyLinkIsCoherent,
		readTrueLinkFailStatus,
		readToken,
		writeToken,
		fam10GetNumCoresOnNode,
		setTotalNodesAndCores,
		limitNodes,
		writeFullRoutingTable,
		isCompatible,
		fam10IsCapable,
		(void (*)(u8, u8, cNorthBridge*))commonVoid,
		(BOOL (*)(u8, u8, sMainData*, cNorthBridge*))commonReturnFalse,
		readSbLink,
		verifyLinkIsNonCoherent,
		ht3SetCFGAddrMap,
		convertBitsToWidth,
		convertWidthToBits,
		fam10NorthBridgeFreqMask,
		gatherLinkData,
		setLinkData,
		ht3WriteTrafficDistribution,
		fam10BufferOptimizations,
		0x00000001,
		0x00000200,
		18,
		0x00000f01
	};

	cNorthBridge fam0f =
	{
#ifdef HT_BUILD_NC_ONLY
		3,
		1,
		12,
#else
		3,
		8,
		32,
#endif /* HT_BUILD_NC_ONLY*/
		writeRoutingTable,
		writeNodeID,
		readDefLnk,
		enableRoutingTables,
		verifyLinkIsCoherent,
		readTrueLinkFailStatus,
		readToken,
		writeToken,
		fam0FGetNumCoresOnNode,
		setTotalNodesAndCores,
		limitNodes,
		writeFullRoutingTable,
		isCompatible,
		fam0fIsCapable,
		fam0fStopLink,
		(BOOL (*)(u8, u8, sMainData*, cNorthBridge*))commonReturnFalse,
		readSbLink,
		verifyLinkIsNonCoherent,
		ht1SetCFGAddrMap,
		convertBitsToWidth,
		convertWidthToBits,
		ht1NorthBridgeFreqMask,
		gatherLinkData,
		setLinkData,
		ht1WriteTrafficDistribution,
		fam0fBufferOptimizations,
		0x00000001,
		0x00000100,
		16,
		0x00000f00
	};

	/* Start with enough of the key to identify the northbridge interface */
	AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(node),
			makePCIBusFromNode(node),
			makePCIDeviceFromNode(node),
			CPU_NB_FUNC_03,
			REG_NB_CPUID_3XFC),
			27, 20, &extFam);
	AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(node),
			makePCIBusFromNode(node),
			makePCIDeviceFromNode(node),
			CPU_NB_FUNC_03,
			REG_NB_CPUID_3XFC),
			11, 8, &baseFam);
	AmdPCIReadBits(MAKE_SBDFO(makePCISegmentFromNode(node),
			makePCIBusFromNode(node),
			makePCIDeviceFromNode(node),
			CPU_NB_FUNC_03,
			REG_NB_CPUID_3XFC),
			7, 4, &model);
	match = (u32)((baseFam << 8) | extFam);

	/* Test each in turn looking for a match.
	 * Initialize the struct if found.
	 */
	if (match == fam15.compatibleKey)
	{
		Amdmemcpy((void *)nb, (const void *)&fam15, (u32) sizeof(cNorthBridge));
	}
	else if (match == fam10.compatibleKey)
	{
		Amdmemcpy((void *)nb, (const void *)&fam10, (u32) sizeof(cNorthBridge));
	}
	else
	{
		if (match == fam0f.compatibleKey)
		{
			Amdmemcpy((void *)nb, (const void *)&fam0f, (u32) sizeof(cNorthBridge));
		}
		else
		{
		STOP_HERE;
		}
	}

	/* Update the initial limited key to the real one, which may include other matching info */
	nb->compatibleKey = makeKey(node);
}
