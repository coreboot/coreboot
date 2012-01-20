/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Orochi thermal initialization
 *
 * Performs processor thermal initialization.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 55600 $   @e \$Date: 2011-06-23 12:39:18 -0600 (Thu, 23 Jun 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15OrPowerMgmt.h"
#include "OptionFamily15hEarlySample.h"
#include "OptionMultiSocket.h"
#include "cpuF15OrSoftwareThermal.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_OR_CPUF15ORSOFTWARETHERMAL_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern F15_OR_ES_CORE_SUPPORT F15OrEarlySampleCoreSupport;
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 *    Main entry point for initializing the Thermal Control
 *    safety net feature.
 *
 *    This must be run by all Family 15h Orochi core 0s in the system.
 *
 * @param[in]  FamilySpecificServices  The current Family Specific Services.
 * @param[in]  CpuEarlyParamsPtr       Service parameters.
 * @param[in]  StdHeader               Config handle for library and services.
 */
VOID
F15OrPmThermalInit (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CPU_EARLY_PARAMS  *CpuEarlyParamsPtr,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32    Core;
  UINT32    LocalPciRegister;
  PCI_ADDR  PciAddress;

  if (OptionMultiSocketConfiguration.GetCurrPciAddr (&PciAddress, StdHeader)) {
    GetCurrentCore (&Core, StdHeader);
    ASSERT (Core == 0);
    PciAddress.Address.Function = FUNC_3;
    PciAddress.Address.Register = NB_CAPS_REG;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    if (((NB_CAPS_REGISTER *) &LocalPciRegister)->HtcCapable == 1) {
      // Enable HTC
      PciAddress.Address.Register = HTC_REG;
      LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
      ((HTC_REGISTER *) &LocalPciRegister)->HtcSlewSel = 0;
      ((HTC_REGISTER *) &LocalPciRegister)->HtcEn = 1;
      F15OrEarlySampleCoreSupport.F15OrHtcInitHook (&LocalPciRegister, StdHeader);
      IDS_OPTION_HOOK (IDS_HTC_CTRL, &LocalPciRegister, StdHeader);
      LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    }
  }
}
