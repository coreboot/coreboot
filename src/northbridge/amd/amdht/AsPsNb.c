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

#undef FILECODE
#define FILECODE 0xCCCC

#include "comlib.h"
#include "AsPsDefs.h"
#include "AsPsNb.h"

u8 getNumOfNodeNb(void);
u8 translateNodeIdToDeviceIdNb(u8 nodeId);

/**
 * Return the minimum possible NbCOF (in 100MHz) for the system.
 *
 * This function can be run on any core and is used by the HT & Memory init
 * code in Phase 1.
 *
 * @return minNbCOF (in multiple of half of CLKIN, 100MHz).
 */
u8 getMinNbCOF(void)
{
	u8 numOfNode, i, j, deviceId, nbDid, nbFid, nextNbFid;
	u32 dtemp;

	nbDid = 0;
	nbFid = 0;

	/* get number of node in the system */
	numOfNode = getNumOfNodeNb();

	/* go through each node for the minimum NbCOF (in multiple of CLKIN/2) */
	for(i=0; i < numOfNode; i++)
	{
		/* stub function for APIC ID virtualization for large MP system later */
		deviceId = translateNodeIdToDeviceIdNb(i);

		/* read all P-state spec registers for NbDid=1 */
		for(j=0; j < 5; j++)
		{
			AmdPCIRead(MAKE_SBDFO(0,0,deviceId,FN_4,PS_SPEC_REG+(j*PCI_REG_LEN)), &dtemp); /*F4x1E0 + j*4 */
			/* get NbDid */
			if(dtemp & NB_DID_MASK)
				nbDid = 1;
		}
		/* if F3x1FC[NbCofVidUpdate]=0, NbFid =  default value */
		AmdPCIRead(MAKE_SBDFO(0,0,deviceId,FN_3,PRCT_INFO), &dtemp); /*F3x1FC*/
		if(!(dtemp & NB_CV_UPDATE)) /* F3x1FC[NbCofVidUpdated]=0, use default VID */
		{
			AmdPCIRead(MAKE_SBDFO(0,0,deviceId,FN_3,CPTC0), &dtemp); /*F3xD4*/
			nextNbFid = (u8) (dtemp & BIT_MASK_5);
			if(nbDid)
				nextNbFid = (u8) (nextNbFid >> 1);
		}
		else
		{
			/* check PVI/SPI */
			AmdPCIRead(MAKE_SBDFO(0,0,deviceId,FN_3,PW_CTL_MISC), &dtemp); /*F3xA0*/
			if(dtemp & PVI_MODE) /* PVI */
			{
				AmdPCIRead(MAKE_SBDFO(0,0,deviceId,FN_3,PRCT_INFO), &dtemp); /*F3x1FC*/
				nextNbFid = (u8) (dtemp >> UNI_NB_FID_BIT);
				nextNbFid &= BIT_MASK_5;
				/* if(nbDid)
					nextNbFid = nextNbFid  >> 1; */
			}
			else /* SVI */
			{
				AmdPCIRead(MAKE_SBDFO(0,0,deviceId,FN_3,PRCT_INFO), &dtemp); /*F3x1FC*/
				nextNbFid = (u8) ((dtemp >> UNI_NB_FID_BIT) & BIT_MASK_5);
				nextNbFid = (u8) (nextNbFid + ((dtemp >> SPLT_NB_FID_OFFSET) & BIT_MASK_3));
				/* if(nbDid)
					nextNbFid = nextNbFid >> 1; */
			}
		}
		if( i == 0)
			nbFid = nextNbFid;
		else if( nbFid > nextNbFid )
		nbFid = nextNbFid;
	}

	/* add the base and convert to 100MHz divide by 2 if DID=1 */
	if(nbDid)
		nbFid = (u8) (nbFid + 4);
	else
		nbFid = (u8) ((nbFid + 4) << 1);
	return nbFid;
}

u8 getNumOfNodeNb(void)
{
	u32 dtemp;

	AmdPCIRead(MAKE_SBDFO(0,0,24,0,0x60), &dtemp);
	dtemp = (dtemp >> 4) & BIT_MASK_3;
	dtemp++;
	return (u8)dtemp;
}

/**
 * Return the PCI device ID for PCI access using node ID.
 *
 * This function may need to change node ID to device ID in big MP systems.
 *
 * @param nodeId Node ID of the node.
 * @return PCI device ID of the node.
 */
u8 translateNodeIdToDeviceIdNb(u8 nodeId)
{
	return (u8) (nodeId+PCI_DEV_BASE);
}
