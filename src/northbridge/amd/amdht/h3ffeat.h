/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Advanced Micro Devices, Inc.
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


#ifndef H3FFEAT_H
#define H3FFEAT_H

#include "h3finit.h"

/*----------------------------------------------------------------------------
 *	Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *			DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

#define MAX_NODES 8
#define MAX_LINKS 8
#define MAX_PLATFORM_LINKS 64 /* 8x8 fully connected (28) + 4 chains with two HT devices */

/* These following are internal definitions */
#define ROUTETOSELF 0x0F
#define INVALID_LINK 0xCC     /* Used in port list data structure to mark unused data entries.
				  Can also be used for no link found in a port list search */

/* definitions for working with the port list structure */
#define PORTLIST_TYPE_CPU 0
#define PORTLIST_TYPE_IO  1

/*
 * Hypertransport Capability definitions and macros
 *
 */

/* HT Host Capability */
/* bool isHTHostCapability(u32 reg) */
#define IS_HT_HOST_CAPABILITY(reg) \
	((reg & (u32)0xE00000FF) == (u32)0x20000008)

#define HT_HOST_CAP_SIZE			0x20

/* Host CapabilityRegisters */
#define HTHOST_LINK_CAPABILITY_REG		0x00
#define HTHOST_LINK_CONTROL_REG		0x04
#define HTHOST_FREQ_REV_REG			0x08
#define HTHOST_FREQ_REV_REG_2			0x1c
	#define HT_HOST_REV_REV3		0x60
#define HTHOST_FEATURE_CAP_REG			0x0C
#define HTHOST_BUFFER_COUNT_REG		0x10
#define HTHOST_ISOC_REG			0x14
#define HTHOST_LINK_TYPE_REG			0x18
	#define HTHOST_TYPE_COHERENT		3
	#define HTHOST_TYPE_NONCOHERENT	7
	#define HTHOST_TYPE_MASK		0x1F

/* HT Slave Capability (HT1 compat) */
#define IS_HT_SLAVE_CAPABILITY(reg) \
	((reg & (u32)0xE00000FF) == (u32)0x00000008)
#define HTSLAVE_LINK01_OFFSET			4
#define HTSLAVE_LINK_CONTROL_0_REG		4
#define HTSLAVE_FREQ_REV_0_REG			0xC
#define HTSLAVE_FEATURE_CAP_REG		0x10

/* HT3 gen Capability */
#define IS_HT_GEN3_CAPABILITY(reg) \
	((reg & (u32)0xF80000FF) == (u32)0xD0000008)
#define HTGEN3_LINK01_OFFSET			0x10
#define HTGEN3_LINK_TRAINING_0_REG		0x10

/* HT3 Retry Capability */
#define IS_HT_RETRY_CAPABILITY(reg) \
	((reg & (u32)0xF80000FF) == (u32)0xC0000008)

#define HTRETRY_CONTROL_REG			4

/* Unit ID Clumping Capability */
#define IS_HT_UNITID_CAPABILITY(reg) \
	((reg & (u32)0xF80000FF) == (u32)0x90000008)

#define HTUNIT_SUPPORT_REG			4
#define HTUNIT_ENABLE_REG			8

/*----------------------------------------------------------------------------
 *			    TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

typedef struct cNorthBridge cNorthBridge;

/* A pair consists of a source node, a link to the destination node, the
 * destination node, and its link back to source node.	 The even indices are
 * the source nodes and links, and the odd indices are for the destination
 * nodes and links.
 */
typedef struct
{
	/* This section is where the link is in the system and how to find it */
	u8 Type; /* 0 = CPU, 1 = Device, all others reserved */
	u8 Link; /* 0-1 for devices, 0-7 for CPUs */
	u8 NodeID; /* The node, or a pointer to the devices parent node */
	u8 HostLink, HostDepth; /* Link of parent node + depth in chain.  Only used by devices */
	SBDFO Pointer; /* A pointer to the device's slave HT capability, so we don't have to keep searching */

	/* This section is for the final settings, which are written to hardware */
	BOOL SelRegang; /* Only used for CPU->CPU links */
	u8 SelWidthIn;
	u8 SelWidthOut;
	u8 SelFrequency;
	uint8_t enable_isochronous_mode;

	/* This section is for keeping track of capabilities and possible configurations */
	BOOL RegangCap;
	uint32_t PrvFrequencyCap;
	uint32_t PrvFeatureCap;
	u8 PrvWidthInCap;
	u8 PrvWidthOutCap;
	uint32_t CompositeFrequencyCap;

} sPortDescriptor;


/*
 * Our global state data structure
 */
typedef struct {
	AMD_HTBLOCK *HtBlock;

	u8 NodesDiscovered;	 /* One less than the number of nodes found in the system */
	u8 TotalLinks;
	u8 sysMpCap;		 /* The maximum number of nodes that all processors are capable of */

	/* Two ports for each link
	 * Note: The Port pair 2*N and 2*N+1 are connected together to form a link
	 * (e.g. 0,1 and 8,9 are ports on either end of an HT link) The lower number
	 * port (2*N) is the source port.	The device that owns the source port is
	 * always the device closer to the BSP. (i.e. nearer the CPU in a
	 * non-coherent chain, or the CPU with the lower NodeID).
	 */
	sPortDescriptor PortList[MAX_PLATFORM_LINKS*2];

	/* The number of coherent links coming off of each node (i.e. the 'Degree' of the node) */
	u8 sysDegree[MAX_NODES];
	/* The systems adjency (sysMatrix[i][j] is true if Node_i has a link to Node_j) */
	BOOL sysMatrix[MAX_NODES][MAX_NODES];

	/* Same as above, but for the currently selected database entry */
	u8 dbDegree[MAX_NODES];
	BOOL dbMatrix[MAX_NODES][MAX_NODES];

	u8 Perm[MAX_NODES];	 /* The node mapping from the database to the system */
	u8 ReversePerm[MAX_NODES];	 /* The node mapping from the system to the database */

	/* Data for non-coherent initialization */
	u8 AutoBusCurrent;
	u8 UsedCfgMapEntires;

	/* 'This' pointer for northbridge */
	cNorthBridge *nb;
} sMainData;

#endif	 /* H3FFEAT_H */
