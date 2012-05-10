/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH oem definition
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 44854 $   @e \$Date: 2011-01-07 16:48:51 +0800 (Fri, 07 Jan 2011) $
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
****************************************************************************
*/
#define ACPI_SLEEP_TRAP                0x01

/**
 * Module Specific Defines for platform BIOS
 *
 */

/**
 * PCIEX_BASE_ADDRESS - Define PCIE base address
 *
 * Option MOVE_PCIEBAR_TO_F0000000 Set PCIe base address to 0xF7000000
 */
#ifdef  MOVE_PCIEBAR_TO_F0000000
  #define PCIEX_BASE_ADDRESS           0xF7000000
#else
  #define PCIEX_BASE_ADDRESS           0xE0000000
#endif

#define SATA_IDE_MODE_SSID           0x78001022
/**
 * SATA_RAID_MODE_SSID - Sata controller RAID mode SSID.
 *    Define value for SSID while SATA controller set to RAID mode.
 */
#define SATA_RAID_MODE_SSID          0x78021022

/**
 * SATA_RAID5_MODE_SSID - Sata controller RAID5 mode SSID.
 *    Define value for SSID while SATA controller set to RAID5 mode.
 */
#define SATA_RAID5_MODE_SSID         0x78031022

/**
 * SATA_AHCI_MODE_SSID - Sata controller AHCI mode SSID.
 *    Define value for SSID while SATA controller set to AHCI mode.
 */
#define SATA_AHCI_SSID               0x78011022

/**
 * OHCI_SSID - All FCH OHCI controllers SSID value.
 *
 */
#define OHCI_SSID                    0x78071022

/**
 * EHCI_SSID - All FCH EHCI controllers SSID value.
 *
 */
#define EHCI_SSID                    0x78081022

/**
 * OHCI4_SSID - OHCI (USB 1.1 mode *HW force) controllers SSID value.
 *
 */
#define OHCI4_SSID                   0x78091022

/**
 * SMBUS_SSID - Smbus controller (South Bridge device 0x14 function 0) SSID value.
 *
 */
#define SMBUS_SSID                   0x780B1022

/**
 * IDE_SSID - SATA IDE controller (South Bridge device 0x14 function 1) SSID value.
 *
 */
#define IDE_SSID                     0x780C1022

/**
 * AZALIA_SSID - AZALIA controller (South Bridge device 0x14 function 2) SSID value.
 *
 */
#define AZALIA_SSID                  0x780D1022

/**
 * LPC_SSID - LPC controller (South Bridge device 0x14 function 3) SSID value.
 *
 */
#define LPC_SSID                     0x780E1022

/**
 * PCIB_SSID - PCIB controller (South Bridge device 0x14 function 4) SSID value.
 *
 */
#define PCIB_SSID                    0x780F1022


#ifndef XHCI_SUPPORT
  #define FCH_NO_XHCI_SUPPORT         TRUE
#endif

#ifdef NO_EC_SUPPORT
  #define FCH_NO_IMC_SUPPORT          TRUE
#endif

#ifdef NO_EC_SUPPORT
  #define FCH_NO_HWM_SUPPORT          TRUE
#endif

