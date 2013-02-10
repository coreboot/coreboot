/**
 * @file
 *
 * Southbridge CIMx Function Support Define (All)
 *
 *
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

// Southbridge SBMAIN Routines

/**
 * Southbridge Main Function Public Function
 *
 */

/**
 * sbBeforePciInit - Config Southbridge before PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void sbBeforePciInit (IN AMDSBCFG* pConfig);


/**
 * sbAfterPciInit - Config Southbridge after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void sbAfterPciInit (IN AMDSBCFG* pConfig);

/**
 *  sbMidPostInit - Config Southbridge during middle of POST
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void sbMidPostInit (IN AMDSBCFG* pConfig);

/**
 *  sbLatePost - Prepare Southbridge to boot to OS.
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void sbLatePost (IN AMDSBCFG* pConfig);

/**
 * sbBeforePciRestoreInit - Config Southbridge before ACPI S3 resume PCI config device restore
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void sbBeforePciRestoreInit (IN AMDSBCFG* pConfig);

/**
 * sbAfterPciRestoreInit - Config Southbridge after ACPI S3 resume PCI config device restore
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void sbAfterPciRestoreInit (IN AMDSBCFG* pConfig);

/**
 * sbSmmAcpiOn - Config Southbridge during ACPI_ON
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void sbSmmAcpiOn (IN AMDSBCFG* pConfig);

/**
 * CallBackToOEM - Call Back routine.
 *
 *
 *
 * @param[in] Func    Callback ID.
 * @param[in] Data    Callback specific data.
 * @param[in] pConfig Southbridge configuration structure pointer.
 */
unsigned int CallBackToOEM (IN unsigned int Func, IN unsigned int Data, IN AMDSBCFG* pConfig);


// Southbridge SBPOR Routines

/**
 * Southbridge power-on initial Public Function
 *
 */

/**
 * sbPowerOnInit - Config Southbridge during power on stage.
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void sbPowerOnInit (IN AMDSBCFG* pConfig);

/**
 * XhciEarlyInit - XhciEarlyInit.
 *
 *
 *
 *
 *
 */
void XhciEarlyInit (void);

/**
 * XhciInitIndirectReg - XhciInitIndirectReg.
 *
 *
 *
 *
 *
 */
void XhciInitIndirectReg (void);


// Southbridge Common Routines

/**
 * Southbridge Common Public Function
 *
 */

/**
 * commonInitEarlyBoot - Config Southbridge SMBUS/ACPI/IDE/LPC/PCIB.
 *
 *    This settings should be done during S3 resume also
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void commonInitEarlyBoot (IN AMDSBCFG* pConfig);

/**
 * commonInitEarlyPost - Config Southbridge SMBUS/ACPI/IDE/LPC/PCIB.
 *
 *    This settings might not program during S3 resume
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void commonInitEarlyPost (IN AMDSBCFG* pConfig);

/**
 * commonInitLateBoot - Prepare Southbridge register setting to boot to OS.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void commonInitLateBoot (IN AMDSBCFG* pConfig);

/**
 * abSpecialSetBeforePciEnum - Special setting ABCFG registers before PCI emulation.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void abSpecialSetBeforePciEnum (IN AMDSBCFG* pConfig);

/**
 * hpetInit - Program Southbridge HPET function
 *
 *  ** Eric
 *
 * @param[in] pConfig         Southbridge configuration structure pointer.
 * @param[in] pStaticOptions  Platform build configuration table.
 *
 */
void hpetInit (IN AMDSBCFG* pConfig, IN BUILDPARAM *pStaticOptions);

/**
 * c3PopupSetting - Program Southbridge C state function
 *
 *  ** Eric
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
void c3PopupSetting (IN AMDSBCFG* pConfig);

/**
 * GcpuRelatedSetting - Program GCPU C related function
 *
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
void GcpuRelatedSetting (IN AMDSBCFG* pConfig);

/**
 * MtC1eEnable - Program Mt C1E Enable Function
 *
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
void MtC1eEnable (IN AMDSBCFG* pConfig);

/**
 * Southbridge Common Private Function
 *
 */

/**
 * abLinkInitBeforePciEnum - Set ABCFG registers before PCI emulation.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void abLinkInitBeforePciEnum (IN AMDSBCFG* pConfig);

// Southbridge SATA Routines

/**
 * Southbridge SATA Controller Public Function
 *
 */

/**
 * sataInitMidPost - Config SATA controller in Middle POST.
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void sataInitMidPost (IN AMDSBCFG* pConfig);

/**
 * sataInitAfterPciEnum - Config SATA controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void  sataInitAfterPciEnum (IN AMDSBCFG*   pConfig);

/**
 * sataInitBeforePciEnum - Config SATA controller before PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void  sataInitBeforePciEnum (IN AMDSBCFG*  pConfig);

/**
 * sataInitLatePost - Prepare SATA controller to boot to OS.
 *
 *              - Set class ID to AHCI (if set to AHCI * Mode)
 *              - Enable AHCI interrupt
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void  sataInitLatePost (IN AMDSBCFG* pConfig);

// Southbridge GEC Routines

/**
 * Southbridge GEC Controller Public Function
 *
 */

/**
 * gecInitBeforePciEnum - Config GEC controller before PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void gecInitBeforePciEnum (IN AMDSBCFG*   pConfig);

/**
 * gecInitAfterPciEnum - Config GEC controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void gecInitAfterPciEnum (IN AMDSBCFG* pConfig);

/**
 * gecInitLatePost - Prepare GEC controller to boot to OS.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void gecInitLatePost (IN AMDSBCFG* pConfig);

// Southbridge USB Routines

/**
 * Southbridge USB Controller Public Function
 *
 */

/**
 * Config USB controller before PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void usbInitBeforePciEnum (IN AMDSBCFG* pConfig);

/**
 * Config USB controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void usbInitAfterPciInit (IN AMDSBCFG* pConfig);

/**
 * Config USB controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void usbInitLate (IN AMDSBCFG* pConfig);

/**
 * Config USB1 EHCI controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void usb1EhciInitAfterPciInit (IN AMDSBCFG* pConfig);
void usb2EhciInitAfterPciInit (IN AMDSBCFG* pConfig);
void usb3EhciInitAfterPciInit (IN AMDSBCFG* pConfig);
void usb1OhciInitAfterPciInit (IN AMDSBCFG* pConfig);
void usb2OhciInitAfterPciInit (IN AMDSBCFG* pConfig);
void usb3OhciInitAfterPciInit (IN AMDSBCFG* pConfig);
void usb4OhciInitAfterPciInit (IN AMDSBCFG* pConfig);

// Southbridge SMI Service Routines (SMM.C)

/**
 * Southbridge SMI Service Routines Public Function
 *
 */

/**
 * Southbridge SMI service module
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void sbSmmService (IN AMDSBCFG* pConfig);

/**
 * softwareSMIservice - Software SMI service
 *
 *  ** Eric
 *
 * @param[in] VOID Southbridge software SMI service ID.
 *
 */
void softwareSMIservice (IN void);

// Southbridge GPP Controller Routines

/**
 * Southbridge GPP Controller Routines Public Function
 *
 */

/**
 * GPP early programming and link training. On exit all populated EPs should be fully operational.
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void sbPcieGppEarlyInit (IN AMDSBCFG* pConfig);

/**
 * sbPcieGppLateInit - Late PCIE initialization for Hudson-2 GPP component
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void sbPcieGppLateInit (IN AMDSBCFG* pConfig);

// Southbridge HD Controller Routines (AZALIA.C)

/**
 * Southbridge HD Controller Routines (AZALIA.C) Public Function
 *
 */

/**
 * Config HD Audio Before PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void azaliaInitBeforePciEnum (IN AMDSBCFG* pConfig);

/**
 * Config HD Audio after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void azaliaInitAfterPciEnum (IN AMDSBCFG* pConfig);


// Southbridge EC Routines

#ifndef NO_EC_SUPPORT
/**
 * Southbridge EC Controller Public Function
 *
 */

/**
 * Config EC controller during power-on
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
  void  ecPowerOnInit (IN AMDSBCFG* pConfig);

/**
 * Config EC controller before PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
  void  ecInitBeforePciEnum (IN AMDSBCFG* pConfig);

/**
 * Prepare EC controller to boot to OS.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
  void  ecInitLatePost (IN AMDSBCFG* pConfig);

/**
 * validateImcFirmware - Validate IMC Firmware.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 * @retval        TRUE   Pass
 * @retval        FALSE     Failed
 */
  unsigned char  validateImcFirmware (IN AMDSBCFG* pConfig);

/**
 * validateImcFirmware - Validate IMC Firmware.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
  void  softwareToggleImcStrapping (IN AMDSBCFG* pConfig);
#endif

#ifndef NO_HWM_SUPPORT
/**
 * hwmInit - Init Hardware Monitor.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
  void  hwmInit (IN AMDSBCFG* pConfig);
/**
 * hwmUpdateData - Hardware Monitor Update Data.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
  void  hwmUpdateData (IN AMDSBCFG* pConfig);
/**
 * hwmUpdateData - Hardware Monitor Update Data.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
  void  hwmCopyFanControl (IN AMDSBCFG* pConfig);
/**
 * hwmCopyFanControl - Copy Hardware Monitor Update Data.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
#endif
/**
 * Stress Reset Mode Late
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
void StressResetModeLate (IN AMDSBCFG* pConfig);

/**
 * TurnOffCG2
 *
 *
 * @retval  VOID
 *
 */
void TurnOffCG2 (OUT void);

/**
 * BackUpCG2
 *
 *
 * @retval  VOID
 *
 */
void BackUpCG2 (OUT void);

void XhciA12Fix (OUT void);