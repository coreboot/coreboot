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
/*
 *****************************************************************************
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 * ***************************************************************************
 *
 */

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
VOID sbBeforePciInit (IN AMDSBCFG* pConfig);


/**
 * sbAfterPciInit - Config Southbridge after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID sbAfterPciInit (IN AMDSBCFG* pConfig);

/**
 *  sbMidPostInit - Config Southbridge during middle of POST
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID sbMidPostInit (IN AMDSBCFG* pConfig);

/**
 *  sbLatePost - Prepare Southbridge to boot to OS.
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID sbLatePost (IN AMDSBCFG* pConfig);

/**
 * sbBeforePciRestoreInit - Config Southbridge before ACPI S3 resume PCI config device restore
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID sbBeforePciRestoreInit (IN AMDSBCFG* pConfig);

/**
 * sbAfterPciRestoreInit - Config Southbridge after ACPI S3 resume PCI config device restore
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID sbAfterPciRestoreInit (IN AMDSBCFG* pConfig);

/**
 * sbSmmAcpiOn - Config Southbridge during ACPI_ON
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID sbSmmAcpiOn (IN AMDSBCFG* pConfig);

/**
 * CallBackToOEM - Call Back routine.
 *
 *
 *
 * @param[in] Func    Callback ID.
 * @param[in] Data    Callback specific data.
 * @param[in] pConfig Southbridge configuration structure pointer.
 */
UINTN CallBackToOEM (IN UINT32 Func, IN UINT32 Data, IN AMDSBCFG* pConfig);


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
VOID sbPowerOnInit (IN AMDSBCFG* pConfig);


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
VOID commonInitEarlyBoot (IN AMDSBCFG* pConfig);

/**
 * commonInitEarlyPost - Config Southbridge SMBUS/ACPI/IDE/LPC/PCIB.
 *
 *    This settings might not program during S3 resume
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID commonInitEarlyPost (IN AMDSBCFG* pConfig);

/**
 * commonInitLateBoot - Prepare Southbridge register setting to boot to OS.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID commonInitLateBoot (IN AMDSBCFG* pConfig);

/**
 * abSpecialSetBeforePciEnum - Special setting ABCFG registers before PCI emulation.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID abSpecialSetBeforePciEnum (IN AMDSBCFG* pConfig);

VOID usbSetPllDuringS3 (IN AMDSBCFG* pConfig);
VOID usbDesertPll (IN AMDSBCFG* pConfig);

/**
 * hpetInit - Program Southbridge HPET function
 *
 *  ** Eric
 *
 * @param[in] pConfig         Southbridge configuration structure pointer.
 * @param[in] pStaticOptions  Platform build configuration table.
 *
 */
VOID hpetInit (IN AMDSBCFG* pConfig, IN BUILDPARAM *pStaticOptions);

/**
 * c3PopupSetting - Program Southbridge C state function
 *
 *  ** Eric
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
VOID c3PopupSetting (IN AMDSBCFG* pConfig);

/**
 * FusionRelatedSetting - Program Fusion C related function
 *
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
VOID FusionRelatedSetting (IN AMDSBCFG* pConfig);

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
VOID abLinkInitBeforePciEnum (IN AMDSBCFG* pConfig);

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
VOID sataInitMidPost (IN AMDSBCFG* pConfig);

/**
 * sataInitAfterPciEnum - Config SATA controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID  sataInitAfterPciEnum (IN AMDSBCFG*   pConfig);

/**
 * sataInitBeforePciEnum - Config SATA controller before PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID  sataInitBeforePciEnum (IN AMDSBCFG*  pConfig);

/**
 * sataInitLatePost - Prepare SATA controller to boot to OS.
 *
 *              - Set class ID to AHCI (if set to AHCI * Mode)
 *              - Enable AHCI interrupt
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID  sataInitLatePost (IN AMDSBCFG* pConfig);

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
VOID gecInitBeforePciEnum (IN AMDSBCFG*   pConfig);

/**
 * gecInitAfterPciEnum - Config GEC controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID gecInitAfterPciEnum (IN AMDSBCFG* pConfig);

/**
 * gecInitLatePost - Prepare GEC controller to boot to OS.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID gecInitLatePost (IN AMDSBCFG* pConfig);

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
VOID usbInitBeforePciEnum (IN AMDSBCFG* pConfig);

/**
 * Config USB controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID usbInitAfterPciInit (IN AMDSBCFG* pConfig);

/**
 * Config USB1 EHCI controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID usb1EhciInitAfterPciInit (IN AMDSBCFG* pConfig);
VOID usb2EhciInitAfterPciInit (IN AMDSBCFG* pConfig);
VOID usb3EhciInitAfterPciInit (IN AMDSBCFG* pConfig);
VOID usb1OhciInitAfterPciInit (IN AMDSBCFG* pConfig);
VOID usb2OhciInitAfterPciInit (IN AMDSBCFG* pConfig);
VOID usb3OhciInitAfterPciInit (IN AMDSBCFG* pConfig);
VOID usb4OhciInitAfterPciInit (IN AMDSBCFG* pConfig);

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
VOID sbSmmService (IN AMDSBCFG* pConfig);

/**
 * softwareSMIservice - Software SMI service
 *
 *  ** Eric
 *
 * @param[in] VOID Southbridge software SMI service ID.
 *
 */
VOID softwareSMIservice (IN VOID);

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
VOID sbPcieGppEarlyInit (IN AMDSBCFG* pConfig);

/**
 * sbPcieGppLateInit - Late PCIE initialization for SB800 GPP component
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID sbPcieGppLateInit (IN AMDSBCFG* pConfig);

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
VOID azaliaInitBeforePciEnum (IN AMDSBCFG* pConfig);

/**
 * Config HD Audio after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID azaliaInitAfterPciEnum (IN AMDSBCFG* pConfig);


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
  VOID  ecPowerOnInit (IN AMDSBCFG* pConfig);

/**
 * Config EC controller before PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
  VOID  ecInitBeforePciEnum (IN AMDSBCFG* pConfig);

/**
 * Prepare EC controller to boot to OS.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
  VOID  ecInitLatePost (IN AMDSBCFG* pConfig);

/**
 * validateImcFirmware - Validate IMC Firmware.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 * @retval        TRUE   Pass
 * @retval        FALSE     Failed
 */
  BOOLEAN  validateImcFirmware (IN AMDSBCFG* pConfig);

/**
 * validateImcFirmware - Validate IMC Firmware.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
  VOID  softwareToggleImcStrapping (IN AMDSBCFG* pConfig);
#endif

#ifndef NO_HWM_SUPPORT
/**
 * validateImcFirmware - Validate IMC Firmware.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
  VOID  hwmInit (IN AMDSBCFG* pConfig);
#endif

