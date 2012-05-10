/**
 * @file
 *
 * Link optimization support specific to family 10h processors.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*
*****************************************************************************
*
* Copyright (c) 2011, Advanced Micro Devices, Inc.
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Advanced Micro Devices, Inc. nor the names of 
*       its contributors may be used to endorse or promote products derived 
*       from this software without specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* ***************************************************************************
*
*/

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "Ids.h"
#include "Topology.h"
#include "htFeat.h"
#include "htInterface.h"
#include "htNb.h"
#include "htNbOptimizationFam10.h"
#include "Filecode.h"
#define FILECODE PROC_HT_FAM10_HTNBOPTIMIZATIONFAM10_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Northbridge specific Frequency limit.
 *
 * @HtNbMethod{::F_NORTH_BRIDGE_FREQ_MASK}
 *
 * Return a mask that eliminates HT frequencies that cannot be used due to a slow
 * northbridge frequency.
 *
 * @param[in]     Node           Result could (later) be for a specific Node
 * @param[in]     Interface      Access to non-HT support functions.
 * @param[in]     Nb             this northbridge
 *
 * @return Frequency mask
 */
UINT32
Fam10NorthBridgeFreqMask (
  IN       UINT8        Node,
  IN       HT_INTERFACE *Interface,
  IN       NORTHBRIDGE  *Nb
  )
{
  UINT32 NbCoreFreq;
  UINT32 Supported;

  ASSERT (Node < MAX_NODES);
  ASSERT (Interface != NULL);
  // The interface to power management will return a system based result.
  // So we only need to call it once, not on every link.  Save the answer,
  // and check to see if we can use a saved answer on subsequent calls.
  //
  if (Nb->CoreFrequency == 0) {
    NbCoreFreq = Interface->GetMinNbCoreFreq (Nb->ConfigHandle);
    NbCoreFreq = (NbCoreFreq / 100);
    ASSERT (NbCoreFreq != 0);
    Nb->CoreFrequency = NbCoreFreq;
  } else {
    NbCoreFreq = Nb->CoreFrequency;
  }

  //
  // NbCoreFreq is minimum northbridge speed in hundreds of MHz.
  // HT can not go faster than the minimum speed of the northbridge.
  //
  if ((NbCoreFreq >= 6) && (NbCoreFreq <= 26)) {
    //  Convert frequency to bit and all less significant bits,
    // by setting next power of 2 and subtracting 1.
    //
    Supported = ((UINT32)1 << ((NbCoreFreq >> 1) + 2)) - 1;
  } else if ((NbCoreFreq > 26) && (NbCoreFreq <= 32)) {
    // Convert frequency to bit and all less significant bits,
    // by setting next power of 2 and subtracting 1, noting that
    // next power of two is two greater than non-extended frequencies
    // (because of the register break).
    //
    Supported = ((UINT32)1 << ((NbCoreFreq >> 1) + 4)) - 1;
  } else if (NbCoreFreq > 32) {
    Supported = HT_FREQUENCY_LIMIT_MAX;
  } else if (NbCoreFreq == 4) {
    // unlikely cases, but include as a defensive measure, also avoid trick above
    Supported = HT_FREQUENCY_LIMIT_400M;
  } else if (NbCoreFreq == 2) {
    Supported = HT_FREQUENCY_LIMIT_200M;
  } else {
    ASSERT (FALSE);
    Supported = HT_FREQUENCY_LIMIT_200M;
  }

  return (Supported);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Northbridge specific Frequency limit.
 *
 * @HtNbMethod{::F_NORTH_BRIDGE_FREQ_MASK}
 *
 * Return a mask that eliminates HT frequencies that cannot be used due to a slow
 * northbridge frequency.
 *
 * @param[in]     Node           Result could (later) be for a specific Node
 * @param[in]     Interface      Access to non-HT support functions.
 * @param[in]     Nb             this northbridge
 *
 * @return Frequency mask
 */
UINT32
Fam10RevDNorthBridgeFreqMask (
  IN       UINT8        Node,
  IN       HT_INTERFACE *Interface,
  IN       NORTHBRIDGE  *Nb
  )
{
  UINT32 NbCoreFreq;
  UINT32 Supported;

  ASSERT (Node < MAX_NODES);
  ASSERT (Interface != NULL);
  // The interface to power management will return a system based result.
  // So we only need to call it once, not on every link.  Save the answer,
  // and check to see if we can use a saved answer on subsequent calls.
  //
  if (Nb->CoreFrequency == 0) {
    NbCoreFreq = Interface->GetMinNbCoreFreq (Nb->ConfigHandle);
    NbCoreFreq = (NbCoreFreq / 100);
    ASSERT (NbCoreFreq != 0);
    Nb->CoreFrequency = NbCoreFreq;
  } else {
    NbCoreFreq = Nb->CoreFrequency;
  }

  // For Rev D, the Ht frequency can go twice the Nb COF, as long as it's HT3.
  // (side note: we are not speculatively upgrading HT1 at 6 .. 10 to HT3,
  //  to avoid complicated recovery if the final speed is HT1.)
  if (NbCoreFreq > 10) {
    NbCoreFreq = NbCoreFreq * 2;
  }
  //
  // NbCoreFreq is minimum northbridge speed in hundreds of MHz.
  // HT can not go faster than the minimum speed of the northbridge.
  //
  if ((NbCoreFreq >= 6) && (NbCoreFreq <= 26)) {
    //  Convert frequency to bit and all less significant bits,
    // by setting next power of 2 and subtracting 1.
    //
    Supported = ((UINT32)1 << ((NbCoreFreq >> 1) + 2)) - 1;
  } else if ((NbCoreFreq > 26) && (NbCoreFreq <= 32)) {
    // Convert frequency to bit and all less significant bits,
    // by setting next power of 2 and subtracting 1, noting that
    // next power of two is two greater than non-extended frequencies
    // (because of the register break).
    //
    Supported = ((UINT32)1 << ((NbCoreFreq >> 1) + 4)) - 1;
  } else if (NbCoreFreq > 32) {
    Supported = HT_FREQUENCY_LIMIT_MAX;
  } else if (NbCoreFreq == 4) {
    // unlikely cases, but include as a defensive measure, also avoid trick above
    Supported = HT_FREQUENCY_LIMIT_400M;
  } else if (NbCoreFreq == 2) {
    Supported = HT_FREQUENCY_LIMIT_200M;
  } else {
    ASSERT (FALSE);
    Supported = HT_FREQUENCY_LIMIT_200M;
  }

  return (Supported);
}
