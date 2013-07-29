/* $NoKeywords:$ */
/**
 * @file
 *
 * Procedure to parse PCIe input configuration data
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "PcieConfigLib.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIECONFIG_PCIEINPUTPARSER_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
PCIe_COMPLEX_DESCRIPTOR*
PcieInputParserGetComplexDescriptor (
  IN      PCIe_COMPLEX_DESCRIPTOR         *ComplexList,
  IN      UINTN                           Index
  );

PCIe_COMPLEX_DESCRIPTOR*
PcieInputParserGetComplexDescriptorOfSocket (
  IN      PCIe_COMPLEX_DESCRIPTOR         *ComplexList,
  IN      UINT32                          SocketId
  );

PCIe_ENGINE_DESCRIPTOR*
PcieInputParserGetEngineDescriptor (
  IN      PCIe_COMPLEX_DESCRIPTOR         *Complex,
  IN      UINTN                           Index
  );

UINTN
PcieInputParserGetNumberOfEngines (
  IN      CONST PCIe_COMPLEX_DESCRIPTOR   *Complex
  );

UINTN
PcieInputParserGetNumberOfComplexes (
  IN      CONST PCIe_COMPLEX_DESCRIPTOR   *ComplexList
  );

UINTN
PcieInputParserGetLengthOfPcieEnginesList (
  IN      CONST PCIe_COMPLEX_DESCRIPTOR   *Complex
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Get number of complexes in platform topology configuration
 *
 *
 *
 * @param[in] ComplexList  First complex configuration in complex configuration array
 * @retval                 Number of Complexes
 *
 */
UINTN
PcieInputParserGetNumberOfComplexes (
  IN      CONST PCIe_COMPLEX_DESCRIPTOR       *ComplexList
  )
{
  UINTN                Result;
  Result = 0;
  while (ComplexList != NULL) {
    Result++;
    ComplexList = PcieInputParsetGetNextDescriptor (ComplexList);
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get number of PCIe engines in given complex
 *
 *
 *
 * @param[in] Complex     Complex configuration
 * @retval                Number of Engines
 */
UINTN
PcieInputParserGetLengthOfPcieEnginesList (
  IN      CONST PCIe_COMPLEX_DESCRIPTOR       *Complex
  )
{
  UINTN                 Result;
  CONST PCIe_PORT_DESCRIPTOR  *PciePortList;
  Result = 0;
  PciePortList = Complex->PciePortList;
  while (PciePortList != NULL) {
    Result++;
    PciePortList = PcieInputParsetGetNextDescriptor (PciePortList);
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get number of DDI engines in given complex
 *
 *
 *
 * @param[in] Complex     Complex configuration
 * @retval                Number of Engines
 */
STATIC UINTN
PcieInputParserGetLengthOfDdiEnginesList (
  IN      CONST PCIe_COMPLEX_DESCRIPTOR       *Complex
  )
{
  UINTN                 Result;
  CONST PCIe_DDI_DESCRIPTOR  *DdiLinkList;
  Result = 0;
  DdiLinkList = Complex->DdiLinkList;
  while (DdiLinkList != NULL) {
    Result++;
    DdiLinkList = PcieInputParsetGetNextDescriptor (DdiLinkList);
  }
  return Result;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get number of engines in given complex
 *
 *
 *
 * @param[in] Complex     Complex configuration header
 * @retval                Number of Engines
 */
UINTN
PcieInputParserGetNumberOfEngines (
  IN      CONST PCIe_COMPLEX_DESCRIPTOR         *Complex
  )
{
  UINTN                     Result;

  Result = PcieInputParserGetLengthOfDdiEnginesList (Complex) +
           PcieInputParserGetLengthOfPcieEnginesList (Complex);
  return Result;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get Complex descriptor by index from given Platform configuration
 *
 *
 *
 * @param[in] ComplexList Platform topology configuration
 * @param[in] Index       Complex descriptor Index
 * @retval                Pointer to Complex Descriptor
 */
PCIe_COMPLEX_DESCRIPTOR*
PcieInputParserGetComplexDescriptor (
  IN      PCIe_COMPLEX_DESCRIPTOR         *ComplexList,
  IN      UINTN                           Index
  )
{
  ASSERT (Index < (PcieInputParserGetNumberOfComplexes (ComplexList)));
  return &ComplexList[Index];
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get Complex descriptor by index from given Platform configuration
 *
 *
 *
 * @param[in] ComplexList  Platform topology configuration
 * @param[in] SocketId     Socket Id
 * @retval                Pointer to Complex Descriptor
 */
PCIe_COMPLEX_DESCRIPTOR*
PcieInputParserGetComplexDescriptorOfSocket (
  IN      PCIe_COMPLEX_DESCRIPTOR         *ComplexList,
  IN      UINT32                          SocketId
  )
{
  PCIe_COMPLEX_DESCRIPTOR *Result;
  Result = NULL;
  while (ComplexList != NULL) {
    if (ComplexList->SocketId == SocketId) {
      Result = ComplexList;
      break;
    }
    ComplexList = PcieInputParsetGetNextDescriptor (ComplexList);
  }
  return Result;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get Engine descriptor from given complex by index
 *
 *
 *
 * @param[in] Complex     Complex descriptor
 * @param[in] Index       Engine descriptor index
 * @retval                Pointer to Engine Descriptor
 */
PCIe_ENGINE_DESCRIPTOR*
PcieInputParserGetEngineDescriptor (
  IN      PCIe_COMPLEX_DESCRIPTOR         *Complex,
  IN      UINTN                           Index
  )
{
  UINTN PcieListlength;
  ASSERT (Index < (PcieInputParserGetNumberOfEngines (Complex)));
  PcieListlength = PcieInputParserGetLengthOfPcieEnginesList (Complex);
  if (Index < PcieListlength) {
    return (PCIe_ENGINE_DESCRIPTOR*) &((Complex->PciePortList)[Index]);
  } else {
    return (PCIe_ENGINE_DESCRIPTOR*) &((Complex->DdiLinkList)[Index - PcieListlength]);
  }
}

