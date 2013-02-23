/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef H3NCMN_H
#define H3NCMN_H

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *			DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/* Use a macro to convert a node number to a PCI device.  If some future port of
 * this code needs to, this can easily be replaced by a function call:
 * u8 makePCIDeviceFromNode(u8 node);
 */
#define makePCIDeviceFromNode(node) \
	((u8)(24 + node))

/* Use a macro to convert a node number to a PCI bus.	If some future port of
 * this code needs to, this can easily be replaced by a function call:
 * u8 makePCIBusFromNode(u8 node);
 */
#define makePCIBusFromNode(node) \
	((u8)(0))

/* Use a macro to convert a node number to a PCI Segment.  If some future port of
 * this code needs to, this can easily be replaced by a function call:
 * u8 makePCISegmentFromNode(u8 node);
 */
#define makePCISegmentFromNode(node) \
	((u8)(0))

/* Macros to fix support issues that come up with early sample processors, which
 * sometimes do things like report capabilities that are actually unsupported.
 * Use the build flag, HT_BUILD_EARLY_SAMPLE_CPU_SUPPORT, to enable this support.
 *
 * It's not envisioned this would be replaced by an external function, but the prototype is
 * u16 fixEarlySampleFreqCapability(u16 fc);
 */
#ifndef HT_BUILD_EARLY_SAMPLE_CPU_SUPPORT
#define fixEarlySampleFreqCapability(fc) \
	((u16)fc)
#else
#define fixEarlySampleFreqCapability(fc) \
	((u16)fc & HT_FREQUENCY_LIMIT_HT1_ONLY)
#endif

/*----------------------------------------------------------------------------
 *			    TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

struct cNorthBridge
{
	/* Public data, clients of northbridge can access */
	u8 maxLinks;
	u8 maxNodes;
	u8 maxPlatformLinks;

	/* Public Interfaces for northbridge clients, coherent init*/
	void (*writeRoutingTable)(u8 node, u8 target, u8 link, cNorthBridge *nb);
	void (*writeNodeID)(u8 node, u8 nodeID, cNorthBridge *nb);
	u8 (*readDefLnk)(u8 node, cNorthBridge *nb);
	void (*enableRoutingTables)(u8 node, cNorthBridge *nb);
	BOOL (*verifyLinkIsCoherent)(u8 node, u8 link, cNorthBridge *nb);
	BOOL (*readTrueLinkFailStatus)(u8 node, u8 link, sMainData *pDat, cNorthBridge *nb);
	u8 (*readToken)(u8 node, cNorthBridge *nb);
	void (*writeToken)(u8 node, u8 value, cNorthBridge *nb);
	u8 (*getNumCoresOnNode)(u8 node, cNorthBridge *nb);
	void (*setTotalNodesAndCores)(u8 node, u8 totalNodes, u8 totalCores, cNorthBridge *nb);
	void (*limitNodes)(u8 node, cNorthBridge *nb);
	void (*writeFullRoutingTable)(u8 node, u8 target, u8 reqLink, u8 rspLink, u32 bClinks, cNorthBridge *nb);
	BOOL (*isCompatible)(u8 node, cNorthBridge *nb);
	BOOL (*isCapable)(u8 node, sMainData *pDat, cNorthBridge *nb);
	void (*stopLink)(u8 node, u8 link, cNorthBridge *nb);
	BOOL (*handleSpecialLinkCase)(u8 node, u8 link, sMainData *pDat, cNorthBridge *nb);

	/* Public Interfaces for northbridge clients, noncoherent init */
	u8 (*readSbLink)(cNorthBridge *nb);
	BOOL (*verifyLinkIsNonCoherent)(u8 node, u8 link, cNorthBridge *nb);
	void (*setCFGAddrMap)(u8 cfgMapIndex, u8 secBus, u8 subBus, u8 targetNode, u8 targetLink, sMainData *pDat, cNorthBridge *nb);

	/* Public Interfaces for northbridge clients, Optimization */
	u8 (*convertBitsToWidth)(u8 value, cNorthBridge *nb);
	u8 (*convertWidthToBits)(u8 value, cNorthBridge *nb);
	u16 (*northBridgeFreqMask)(u8 node, cNorthBridge *nb);
	void (*gatherLinkData)(sMainData *pDat, cNorthBridge *nb);
	void (*setLinkData)(sMainData *pDat, cNorthBridge *nb);

	/* Public Interfaces for northbridge clients, System and performance Tuning. */
	void (*writeTrafficDistribution)(u32 links01, u32 links10, cNorthBridge *nb);
	void (*bufferOptimizations)(u8 node, sMainData *pDat, cNorthBridge *nb);

	/* Private Data for northbridge implementation use only */
	u32 selfRouteRequestMask;
	u32 selfRouteResponseMask;
	u8 broadcastSelfBit;
	u32 compatibleKey;
} ;

/*----------------------------------------------------------------------------
 *			FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */
void newNorthBridge(u8 node, cNorthBridge *nb);

#endif	 /* H3NCMN_H */
