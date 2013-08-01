/* $NoKeywords:$ */
/**
 * @file
 *
 *  Procedure to parse PCIe input configuration data
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#ifndef _PCIEINPUTPARSER_H_
#define _PCIEINPUTPARSER_H_


UINTN
PcieInputParserGetNumberOfComplexes (
  IN      CONST PCIe_COMPLEX_DESCRIPTOR       *ComplexList
 );

UINTN
PcieInputParserGetNumberOfEngines (
  IN      CONST PCIe_COMPLEX_DESCRIPTOR       *Complex
 );


PCIe_COMPLEX_DESCRIPTOR*
PcieInputParserGetComplexDescriptor (
  IN      PCIe_COMPLEX_DESCRIPTOR       *ComplexList,
  IN      UINTN                         Index
 );

PCIe_ENGINE_DESCRIPTOR*
PcieInputParserGetEngineDescriptor (
  IN      PCIe_COMPLEX_DESCRIPTOR       *Complex,
  IN      UINTN                         Index
 );

PCIe_COMPLEX_DESCRIPTOR*
PcieInputParserGetComplexDescriptorOfSocket (
  IN      PCIe_COMPLEX_DESCRIPTOR       *ComplexList,
  IN      UINT32                        SocketId
  );

UINTN
PcieInputParserGetLengthOfPcieEnginesList (
  IN      CONST PCIe_COMPLEX_DESCRIPTOR       *Complex
  );
#endif

