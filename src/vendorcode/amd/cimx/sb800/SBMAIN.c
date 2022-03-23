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

#include "SBPLATFORM.h"
#include "cbtypes.h"

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
  commonInitEarlyBoot (pConfig);
  commonInitEarlyPost (pConfig);
#ifndef NO_EC_SUPPORT
  ecInitBeforePciEnum (pConfig);
#endif
  usbInitBeforePciEnum (pConfig);                     // USB POST TIME Only
  sataInitBeforePciEnum (pConfig);                    // Init SATA class code and PHY
  gecInitBeforePciEnum (pConfig);                     // Init GEC
  azaliaInitBeforePciEnum (pConfig);                  // Detect and configure High Definition Audio
  sbPcieGppEarlyInit (pConfig);                       // Gpp port init
  abSpecialSetBeforePciEnum (pConfig);
  usbDesertPll (pConfig);
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
  usbInitAfterPciInit (pConfig);                      // Init USB MMIO
  sataInitAfterPciEnum (pConfig);                     // SATA port enumeration
  gecInitAfterPciEnum (pConfig);
  azaliaInitAfterPciEnum (pConfig);                   // Detect and configure High Definition Audio

#ifndef NO_HWM_SUPPORT
  hwmInit (pConfig);
#endif
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
  sataInitMidPost (pConfig);
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
  commonInitLateBoot (pConfig);
  sataInitLatePost (pConfig);
  gecInitLatePost (pConfig);
  hpetInit (pConfig, pStaticOptions);                 // SB Configure HPET base and enable bit
#ifndef NO_EC_SUPPORT
  ecInitLatePost (pConfig);
#endif
  sbPcieGppLateInit (pConfig);

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
  pConfig->S3Resume = 1;
  commonInitEarlyBoot (pConfig);                      // set /SMBUS/ACPI/IDE/LPC/PCIB
  abLinkInitBeforePciEnum (pConfig);                  // Set ABCFG registers
  usbInitBeforePciEnum (pConfig);                     // USB POST TIME Only
  sataInitBeforePciEnum (pConfig);
  gecInitBeforePciEnum (pConfig);                     // Init GEC
  azaliaInitBeforePciEnum (pConfig);                  // Detect and configure High Definition Audio
  sbPcieGppEarlyInit (pConfig);                       // Gpp port init
  abSpecialSetBeforePciEnum (pConfig);
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

  usbSetPllDuringS3 (pConfig);
  pStaticOptions = &(pConfig->BuildParameters);
  commonInitLateBoot (pConfig);
  sataInitAfterPciEnum (pConfig);
  gecInitAfterPciEnum (pConfig);
  azaliaInitAfterPciEnum (pConfig);          // Detect and configure High Definition Audio
  hpetInit (pConfig, pStaticOptions);        // SB Configure HPET base and enable bit
  sataInitLatePost (pConfig);
  c3PopupSetting (pConfig);

#ifndef NO_HWM_SUPPORT
  SBIMCFanInitializeS3 ();
#endif
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

  // Disable Power Button SMI
  RWMEM (ACPI_MMIO_BASE + SMI_BASE + SB_SMI_REGB2, AccWidthUint8, ~(BIT4 + BIT5), 0);
  RWMEM (ACPI_MMIO_BASE + SMI_BASE + SB_SMI_REGAC, AccWidthUint8, ~(BIT6 + BIT7), 0);
}

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
UINT32
CallBackToOEM (
  IN       UINT32 Func,
  IN       UINT32 Data,
  IN       AMDSBCFG* pConfig
  )
{
  UINT32 Result;
  Result = 0;
  if ( pConfig->StdHeader.CALLBACK.CalloutPtr == NULL ) return Result;
  Result = (pConfig->StdHeader.CALLBACK.CalloutPtr) ( Func, Data, pConfig);

  return Result;
}
