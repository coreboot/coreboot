/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build options for a Kabini platform solution
 *
 * This file generates the defaults tables for the "Kabini" platform solution
 * set of processors. The documented build options are imported from a user
 * controlled file for processing.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 69377 $   @e \$Date: 2012-05-08 03:52:23 -0500 (Tue, 08 May 2012) $
 */
/*****************************************************************************
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
 *
 ***************************************************************************/

#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "AdvancedApi.h"
#include "heapManager.h"
#include "CreateStruct.h"
#include "cpuFeatures.h"
#include "Table.h"
#include "CommonReturns.h"
#include "cpuEarlyInit.h"
#include "cpuLateInit.h"
#include "GnbInterface.h"

/*****************************************************************************
 *   Define the RELEASE VERSION string
 *
 * The Release Version string should identify the next planned release.
 * When a branch is made in preparation for a release, the release manager
 * should change/confirm that the branch version of this file contains the
 * string matching the desired version for the release. The trunk version of
 * the file should always contain a trailing 'X'. This will make sure that a
 * development build from trunk will not be confused for a released version.
 * The release manager will need to remove the trailing 'X' and update the
 * version string as appropriate for the release. The trunk copy of this file
 * should also be updated/incremented for the next expected version, + trailing 'X'
 ****************************************************************************/
                  // This is the delivery package title, "KabiniPI        "
                  // This string MUST be exactly 16 characters long
#define AGESA_PACKAGE_STRING  {'K', 'a', 'b', 'i', 'n', 'i', 'P', 'I', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}

                  // This is the release version number of the AGESA component
                  // This string MUST be exactly 12 characters long
#define AGESA_VERSION_STRING  {'V', '1', '.', '0', '.', '0', '.', '7', ' ', ' ', ' ', ' '}


// The Kabini FT3 solution is defined to be family 0x16 models 0x00 - 0x0F in the FT3 sockets.
#define INSTALL_FT3_SOCKET_SUPPORT           TRUE
#define INSTALL_FAMILY_16_MODEL_0x_SUPPORT   TRUE

#ifdef BLDOPT_REMOVE_FT3_SOCKET_SUPPORT
  #if BLDOPT_REMOVE_FT3_SOCKET_SUPPORT == TRUE
    #undef INSTALL_FT3_SOCKET_SUPPORT
    #define INSTALL_FT3_SOCKET_SUPPORT     FALSE
  #endif
#endif


// The following definitions specify the default values for various parameters in which there are
// no clearly defined defaults to be used in the common file.  The values below are based on product
// and BKDG content, please consult the AGESA Memory team for consultation.
#define DFLT_SCRUB_DRAM_RATE            (0)
#define DFLT_SCRUB_L2_RATE              (0)
#define DFLT_SCRUB_L3_RATE              (0)
#define DFLT_SCRUB_IC_RATE              (0)
#define DFLT_SCRUB_DC_RATE              (0)
#define DFLT_MEMORY_QUADRANK_TYPE       QUADRANK_UNBUFFERED
#define DFLT_VRM_SLEW_RATE              (5000)


#define DFLT_SMBUS0_BASE_ADDRESS            0xB00
#define DFLT_SMBUS1_BASE_ADDRESS            0xB20
#define DFLT_SIO_PME_BASE_ADDRESS           0xE00
#define DFLT_ACPI_PM1_EVT_BLOCK_ADDRESS     0x400
#define DFLT_ACPI_PM1_CNT_BLOCK_ADDRESS     0x404
#define DFLT_ACPI_PM_TMR_BLOCK_ADDRESS      0x408
#define DFLT_ACPI_CPU_CNT_BLOCK_ADDRESS     0x410
#define DFLT_ACPI_GPE0_BLOCK_ADDRESS        0x420
#define DFLT_SPI_BASE_ADDRESS               0xFEC10000ul
#define DFLT_WATCHDOG_TIMER_BASE_ADDRESS    0xFEC000F0ul
#define DFLT_HPET_BASE_ADDRESS              0xFED00000ul
#define DFLT_SMI_CMD_PORT                   0xB0
#define DFLT_ACPI_PMA_CNT_BLK_ADDRESS       0xFE00
#define DFLT_GEC_BASE_ADDRESS               0xFED61000ul
#define DFLT_AZALIA_SSID                    0x780D1022ul
#define DFLT_SMBUS_SSID                     0x780B1022ul
#define DFLT_IDE_SSID                       0x780C1022ul
#define DFLT_SATA_AHCI_SSID                 0x78011022ul
#define DFLT_SATA_IDE_SSID                  0x78001022ul
#define DFLT_SATA_RAID5_SSID                0x78031022ul
#define DFLT_SATA_RAID_SSID                 0x78021022ul
#define DFLT_EHCI_SSID                      0x78081022ul
#define DFLT_OHCI_SSID                      0x78071022ul
#define DFLT_LPC_SSID                       0x780E1022ul
#define DFLT_SD_SSID                        0x78061022ul
#define DFLT_XHCI_SSID                      0x78121022ul
#define DFLT_FCH_PORT80_BEHIND_PCIB         FALSE
#define DFLT_FCH_ENABLE_ACPI_SLEEP_TRAP     TRUE
#define DFLT_FCH_GPP_LINK_CONFIG            PortA4
#define DFLT_FCH_GPP_PORT0_PRESENT          FALSE
#define DFLT_FCH_GPP_PORT1_PRESENT          FALSE
#define DFLT_FCH_GPP_PORT2_PRESENT          FALSE
#define DFLT_FCH_GPP_PORT3_PRESENT          FALSE
#define DFLT_FCH_GPP_PORT0_HOTPLUG          FALSE
#define DFLT_FCH_GPP_PORT1_HOTPLUG          FALSE
#define DFLT_FCH_GPP_PORT2_HOTPLUG          FALSE
#define DFLT_FCH_GPP_PORT3_HOTPLUG          FALSE
#define OPTION_MICROSERVER                  TRUE
// Instantiate all solution relevant data.
#include "PlatformInstall.h"

