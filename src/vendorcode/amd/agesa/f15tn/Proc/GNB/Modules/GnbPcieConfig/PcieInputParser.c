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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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
#include  "PcieInputParser.h"
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
  IN      PCIe_COMPLEX_DESCRIPTOR       *ComplexList
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
  IN      PCIe_COMPLEX_DESCRIPTOR       *Complex
  )
{
  UINTN                 Result;
  PCIe_PORT_DESCRIPTOR  *PciePortList;
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
  IN      PCIe_COMPLEX_DESCRIPTOR       *Complex
  )
{
  UINTN                 Result;
  PCIe_DDI_DESCRIPTOR  *DdiLinkList;
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
  IN      PCIe_COMPLEX_DESCRIPTOR         *Complex
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

