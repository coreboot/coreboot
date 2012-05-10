/**
 * @file
 *
 * SB Initialization.
 *
 * Init IOAPIC/IOMMU/Misc NB features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-SB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*;********************************************************************************
;
; Copyright (c) 2011, Advanced Micro Devices, Inc.
; All rights reserved.
; 
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * Neither the name of Advanced Micro Devices, Inc. nor the names of 
;       its contributors may be used to endorse or promote products derived 
;       from this software without specific prior written permission.
; 
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; 
;*********************************************************************************/

#include "SbPlatform.h"
#include "cbtypes.h"
#include "AmdSbLib.h"

#ifndef B1_IMAGE

/*----------------------------------------------------------------------------------------*/
/**
 * sbBeforePciInit - Config Southbridge before PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */

VOID
sbBeforePciInit (
  IN       AMDSBCFG* pConfig
  )
{
  TRACE ((DMSG_SB_TRACE, "CIMx - Entering sbBeforePciInit \n"));
  RecordSbConfigPtr ( (UINT32) ((UINTN) (pConfig)));
  CheckEfuse (pConfig);
  ValidateFchVariant (pConfig);
  imcEnableSurebootTimer (pConfig);
  commonInitEarlyPost (pConfig);
  commonInitEarlyBoot (pConfig);
#ifndef NO_EC_SUPPORT
  ecInitBeforePciEnum (pConfig);
#endif
  usbInitBeforePciEnum (pConfig);                     // USB POST TIME Only
  sataInitBeforePciEnum (pConfig);                    // Init SATA class code and PHY
  gecInitBeforePciEnum (pConfig);                     // Init GEC
  azaliaInitBeforePciEnum (pConfig);                  // Detect and configure High Definition Audio
  sbPcieGppEarlyInit (pConfig);                       // Gpp port init
  abSpecialSetBeforePciEnum (pConfig);
  hwmInit (pConfig);
  TRACE ((DMSG_SB_TRACE, "CIMx - Exiting sbBeforePciInit \n"));
}

/**
 *  sbAfterPciInit - Config Southbridge after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
sbAfterPciInit (
  IN       AMDSBCFG* pConfig
  )
{
  TRACE ((DMSG_SB_TRACE, "CIMx - Entering sbAfterPciInit \n"));

  imcEnableSurebootTimer (pConfig);
  usbInitAfterPciInit (pConfig);                      // Init USB MMIO
  sataInitAfterPciEnum (pConfig);                     // SATA port enumeration
  gecInitAfterPciEnum (pConfig);
  azaliaInitAfterPciEnum (pConfig);                   // Detect and configure High Definition Audio
  hwmUpdateData (pConfig);
  TRACE ((DMSG_SB_TRACE, "CIMx - Exiting sbAfterPciInit \n"));
}

/**
 *  sbMidPostInit - Config Southbridge during middle of POST
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
sbMidPostInit (
  IN       AMDSBCFG* pConfig
  )
{
  TRACE ((DMSG_SB_TRACE, "CIMx - Entering sbMidPostInit \n"));
  imcEnableSurebootTimer (pConfig);
  sataInitMidPost (pConfig);
  TRACE ((DMSG_SB_TRACE, "CIMx - Exiting sbMidPostInit \n"));
}

/*----------------------------------------------------------------------------------------*/
/**
 *  sbLatePost - Prepare Southbridge to boot to OS.
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
sbLatePost (
  IN       AMDSBCFG* pConfig
  )
{
// UINT16 dwVar;
  BUILDPARAM  *pStaticOptions;
  pStaticOptions = &(pConfig->BuildParameters);
  TRACE ((DMSG_SB_TRACE, "CIMx - Entering sbLatePost \n"));
  commonInitLateBoot (pConfig);
  sataInitLatePost (pConfig);
  gecInitLatePost (pConfig);
  hpetInit (pConfig, pStaticOptions);                 // SB Configure HPET base and enable bit
#ifndef NO_EC_SUPPORT
  ecInitLatePost (pConfig);
#endif
  sbPcieGppLateInit (pConfig);
  hwmImcInit (pConfig);
//  hwmSbtsiAutoPollingOff (pConfig);
  imcDisarmSurebootTimer (pConfig);
  usbInitLate (pConfig);                      // Init USB
  StressResetModeLate (pConfig);                      //
}

/*----------------------------------------------------------------------------------------*/
/**
 * sbBeforePciRestoreInit - Config Southbridge before ACPI S3 resume PCI config device restore
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */

VOID
sbBeforePciRestoreInit (
  IN       AMDSBCFG* pConfig
  )
{
  TRACE ((DMSG_SB_TRACE, "CIMx - Entering sbBeforePciRestoreInit \n"));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG00, AccWidthUint8, 0xFF, 0x1E);
  pConfig->S3Resume = 1;
  ValidateFchVariant (pConfig);
  commonInitEarlyBoot (pConfig);                      // set /SMBUS/ACPI/IDE/LPC/PCIB
  abLinkInitBeforePciEnum (pConfig);                  // Set ABCFG registers
  usbInitBeforePciEnum (pConfig);                     // USB POST TIME Only
  sataInitBeforePciEnum (pConfig);
  gecInitBeforePciEnum (pConfig);                     // Init GEC
  azaliaInitBeforePciEnum (pConfig);                  // Detect and configure High Definition Audio
  sbPcieGppEarlyInit (pConfig);                       // Gpp port init
  abSpecialSetBeforePciEnum (pConfig);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG00, AccWidthUint8, 0xFF, 0x1E);
}

/*----------------------------------------------------------------------------------------*/
/**
 * sbAfterPciRestoreInit - Config Southbridge after ACPI S3 resume PCI config device restore
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */

VOID
sbAfterPciRestoreInit (
  IN       AMDSBCFG* pConfig
  )
{
  BUILDPARAM  *pStaticOptions;

  pConfig->S3Resume = 1;

  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG00, AccWidthUint8, 0xFF, 0x1E);
  pStaticOptions = &(pConfig->BuildParameters);
  TRACE ((DMSG_SB_TRACE, "CIMx - Entering sbAfterPciRestoreInit \n"));
  commonInitLateBoot (pConfig);
  sataInitAfterPciEnum (pConfig);
  gecInitAfterPciEnum (pConfig);
  azaliaInitAfterPciEnum (pConfig);          // Detect and configure High Definition Audio
  hpetInit (pConfig, pStaticOptions);        // SB Configure HPET base and enable bit
  sataInitLatePost (pConfig);
  c3PopupSetting (pConfig);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG00, AccWidthUint8, 0xFF, 0x1E);
  hwmInit (pConfig);
  hwmImcInit (pConfig);
}

/*----------------------------------------------------------------------------------------*/
/**
 * sbSmmAcpiOn - Config Southbridge during ACPI_ON
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
sbSmmAcpiOn (
  IN       AMDSBCFG* pConfig
  )
{
  // Commented the following code since we need to leave the IRQ1/12 filtering enabled always as per latest
  // recommendation in RPR. This is required to fix the keyboard stuck issue when playing games under Windows
  AMDSBCFG*   pTmp;      //lx-dummy for /W4 build
  pTmp = pConfig;

  // Disable Power Button SMI
  RWMEM (ACPI_MMIO_BASE + SMI_BASE + SB_SMI_REGAC, AccWidthUint8, ~(BIT6), 0);
  // USB workaroud
  // x00[0] = 1;
  // x04[31] = 0, x04[0] = 1;
  // x08[31] = 0, x08[7] = 0;
  // x0C[31] = 0, x0C[7] = 0,
  // x10[31] = 0, x10[0] = 1;
  // x14[31] = 0, x14[0] = 1;
  // x18[31] = 0, x18[7] = 0;
  // x1C[31] = 0,
  // x20[31] = 0,
  //RWMEM (0x00, AccWidthUint32, 0, BIT0);
  //RWMEM (0x04, AccWidthUint32, 0, BIT0);
  //RWMEM (0x08, AccWidthUint32, 0, 0);
  //RWMEM (0x0C, AccWidthUint32, 0, 0);
  //RWMEM (0x10, AccWidthUint32, 0, BIT0);
  //RWMEM (0x14, AccWidthUint32, 0, BIT0);
  //RWMEM (0x18, AccWidthUint32, 0, 0);
  //RWMEM (0x1C, AccWidthUint32, 0, 0);
  //RWMEM (0x20, AccWidthUint32, 0, 0);
}

#endif

/*----------------------------------------------------------------------------------------*/
/**
 * Call Back routine.
 *
 *
 *
 * @param[in] Func    Callback ID.
 * @param[in] Data    Callback specific data.
 * @param[in] pConfig Southbridge configuration structure pointer.
 */
UINTN
CallBackToOEM (
  IN       UINT32 Func,
  IN       UINT32 Data,
  IN       AMDSBCFG* pConfig
  )
{
  UINT32 Result;
  Result = 0;
  if ( pConfig->StdHeader.CALLBACK.CalloutPtr == NULL ) return Result;
  TRACE ((DMSG_SB_TRACE, "Calling %lx ( % x, % x, % x)\n", pConfig->StdHeader.CalloutPtr, Func, Data, pConfig));
  Result = (*(SBCIM_HOOK_ENTRY*) (UINTN)&pConfig->StdHeader.CALLBACK.CalloutPtr) ( Func, Data, pConfig);

  TRACE ((DMSG_SB_TRACE, "SB Hook Status [ % x]\n", Result));
  return Result;
}


