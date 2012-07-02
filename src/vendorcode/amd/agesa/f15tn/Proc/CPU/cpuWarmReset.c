/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Warm Reset Implementation.
 *
 * Implement Warm Reset Interface.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                            M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_CPUWARMRESET_FILECODE
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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *    This function will set the CPU register warm reset bits.
 *
 *    Note: This function will be called by UEFI BIOS's
 *    The UEFI wrapper code should register this function, to be called back later point
 *    in time, before the wrapper code does warm reset.
 *
 *    @param[in] StdHeader Config handle for library and services
 *    @param[in] Request   Indicate warm reset status
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
SetWarmResetFlag (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       WARM_RESET_REQUEST *Request
  )
{
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  FamilySpecificServices = NULL;

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->SetWarmResetFlag (FamilySpecificServices, StdHeader, Request);
}

/*---------------------------------------------------------------------------------------*/
/**
 *    This function will get the CPU register warm reset bits.
 *
 *    Note: This function will be called by UEFI BIOS's
 *    The UEFI wrapper code should register this function, to be called back later point
 *    in time, before the wrapper code does warm reset.
 *
 *    @param[in] StdHeader Config handle for library and services
 *    @param[out] Request   Indicate warm reset status
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
GetWarmResetFlag (
  IN       AMD_CONFIG_PARAMS *StdHeader,
     OUT   WARM_RESET_REQUEST *Request
  )
{
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  FamilySpecificServices = NULL;

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetWarmResetFlag (FamilySpecificServices, StdHeader, Request);

  switch (StdHeader->Func) {
  case AMD_INIT_RESET:
    Request->PostStage = (UINT8) WR_STATE_RESET;
    break;
  case AMD_INIT_EARLY:
    Request->PostStage = (UINT8) WR_STATE_EARLY;
    break;
  case AMD_INIT_POST:
    // Fall through to default case
  default:
    Request->PostStage = (UINT8) WR_STATE_POST;
    break;
  }
}
/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S - (AGESA ONLY)
 *----------------------------------------------------------------------------------------
 */



/*---------------------------------------------------------------------------------------*/
/**
 * Is this boot a warm reset?
 *
 * This function reads the CPU register warm reset bit that is preserved after a warm reset.
 * Which in fact gets set before issuing warm reset.  We just use the BSP's register always.
 *
 * @param[in]       StdHeader       Config handle for library and services
 *
 * @retval      TRUE            Warm Reset
 * @retval      FALSE           Not Warm Reset
 *
 */
BOOLEAN
IsWarmReset (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 PostStage;
  WARM_RESET_REQUEST Request;
  BOOLEAN  WarmReset;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  FamilySpecificServices = NULL;

  switch (StdHeader->Func) {
  case AMD_INIT_RESET:
    PostStage = WR_STATE_RESET;
    break;
  case AMD_INIT_EARLY:
    PostStage = WR_STATE_EARLY;
    break;
  case AMD_INIT_POST:
  default:
    PostStage = WR_STATE_POST;
    break;
  }

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetWarmResetFlag (FamilySpecificServices, StdHeader, &Request);

  if (Request.StateBits >= PostStage) {
    WarmReset = TRUE;
  } else {
    WarmReset = FALSE;
  }

  return WarmReset;
}

/*---------------------------------------------------------------------------------------*/
/**
 *    This function will set the CPU register warm reset bits at AmdInitEarly if it is
 *    currently in cold boot. To request for a warm reset, set the RequestBit to TRUE
 *    and the StateBits to (current poststage - 1)
 *
 *    @param[in] Data      The table data value (unused in this routine)
 *    @param[in] StdHeader Config handle for library and services
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
SetWarmResetAtEarly (
  IN       UINT32            Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  WARM_RESET_REQUEST Request;

  if (!IsWarmReset (StdHeader)) {
    GetWarmResetFlag (StdHeader, &Request);

    Request.RequestBit = TRUE;
    Request.StateBits = (Request.PostStage - 1);

    SetWarmResetFlag (StdHeader, &Request);
  }
}

/*----------------------------------------------------------------------------------------
 *                            L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

