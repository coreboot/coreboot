/* $NoKeywords:$ */
/**
 * @file
 *
 * Link Optimization Feature.
 *
 * Contains interface for Link Optimization.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
 * ***************************************************************************
 *
 */

#ifndef _HT_FEAT_OPTIMIZATION_H_
#define _HT_FEAT_OPTIMIZATION_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

/**
 * Translate a desired width setting to the bits to set in the register field.
 */
UINT8
ConvertWidthToBits (
  IN       UINT8       Value
  );

/**
 * Access HT Link Control Register.
 *
 */
VOID
SetHtControlRegisterBits (
  IN       PCI_ADDR    Reg,
  IN       UINT8       HiBit,
  IN       UINT8       LoBit,
  IN       UINT32      *Value,
  IN       STATE_DATA  *State
  );

/**
 * Get Link features into system data structure.
 *
 */
VOID
GatherLinkData (
  IN       STATE_DATA   *State
  );

/**
 * Optimize Links.
 *
 */
VOID
SelectOptimalWidthAndFrequency (
  IN OUT   STATE_DATA   *State
  );

/**
 * Change the hardware state for all Links according to the now optimized data in the
 * port list data structure.
 *
 */
VOID
SetLinkData (
  IN       STATE_DATA   *State
  );

/**
 * Retry must be enabled on all coherent links if it is enabled on any coherent links.
 *
 */
BOOLEAN
IsCoherentRetryFixup (
  IN       STATE_DATA       *State
  );

/**
 * Find a specific HT capability type.
 *
 * @retval FALSE    the capability was not found
 */
BOOLEAN
DoesDeviceHaveHtSubtypeCap (
  IN       PCI_ADDR         DevicePointer,
  IN       UINT8            CapSubType,
     OUT   PCI_ADDR         *CapabilityBase,
  IN       STATE_DATA       *State
  );

#endif  /* _HT_FEAT_OPTIMIZATION_H_ */
