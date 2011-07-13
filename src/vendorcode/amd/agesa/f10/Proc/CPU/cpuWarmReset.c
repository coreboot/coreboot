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
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*
 ******************************************************************************
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

  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
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

  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetWarmResetFlag (FamilySpecificServices, StdHeader, Request);
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

  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetWarmResetFlag (FamilySpecificServices, StdHeader, &Request);

  if (Request.StateBits >= PostStage) {
    WarmReset = TRUE;
  } else {
    WarmReset = FALSE;
  }

  return WarmReset;
}

/*----------------------------------------------------------------------------------------
 *                            L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

