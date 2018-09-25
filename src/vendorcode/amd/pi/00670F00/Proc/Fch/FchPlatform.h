/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH platform definition
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision$   @e \$Date$
 *
 */
 /*****************************************************************************
 *
 * Copyright (c) 2008 - 2017, Advanced Micro Devices, Inc.
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

#include <check_for_wrapper.h>

#ifndef  _FCH_PLATFORM_H_
#define  _FCH_PLATFORM_H_

#define MAX_SATA_PORTS 8

#include "AGESA.h"

#ifndef FCHOEM_ACPI_RESTORE_SWSMI
  #define FCHOEM_BEFORE_PCI_RESTORE_SWSMI    0xD3
  #define FCHOEM_AFTER_PCI_RESTORE_SWSMI     0xD4
  #define FCHOEM_ENABLE_ACPI_SWSMI           0xA0
  #define FCHOEM_DISABLE_ACPI_SWSMI          0xA1
  #define FCHOEM_START_TIMER_SMI             0xBC
  #define FCHOEM_STOP_TIMER_SMI              0xBD
#endif

#ifndef FCHOEM_SPI_UNLOCK_SWSMI
  #define FCHOEM_SPI_UNLOCK_SWSMI            0xAA
#endif
#ifndef FCHOEM_SPI_LOCK_SWSMI
  #define FCHOEM_SPI_LOCK_SWSMI              0xAB
#endif

#ifndef FCHOEM_ACPI_TABLE_RANGE_LOW
  #define FCHOEM_ACPI_TABLE_RANGE_LOW        0xE0000ul
#endif

#ifndef FCHOEM_ACPI_TABLE_RANGE_HIGH
  #define FCHOEM_ACPI_TABLE_RANGE_HIGH       0xFFFF0ul
#endif

#ifndef FCHOEM_ACPI_BYTE_CHECHSUM
  #define FCHOEM_ACPI_BYTE_CHECHSUM          0x100
#endif

#ifndef FCHOEM_IO_DELAY_PORT
  #define FCHOEM_IO_DELAY_PORT               0x80
#endif

#ifndef FCHOEM_OUTPUT_DEBUG_PORT
  #define FCHOEM_OUTPUT_DEBUG_PORT           0x80
#endif

#define FCH_PCIRST_BASE_IO                   0xCF9
#define FCH_PCI_RESET_COMMAND06              0x06
#define FCH_PCI_RESET_COMMAND0E              0x0E
#define FCH_KBDRST_BASE_IO                   0x64
#define FCH_KBC_RESET_COMMAND                0xFE
#define FCH_ROMSIG_BASE_IO                   0x20000l
#define FCH_ROMSIG_SIGNATURE                 0x55AA55AAul
#define FCH_MAX_TIMER                        0xFFFFFFFFul
#define FCH_GEC_INTERNAL_REG                 0x6804
#define FCH_HPET_REG_MASK                    0xFFFFF800ul
#define FCH_FAKE_USB_BAR_ADDRESS             0x58830000ul


#ifndef FCHOEM_ELAPSED_TIME_UNIT
  #define FCHOEM_ELAPSED_TIME_UNIT           28
#endif

#ifndef FCHOEM_ELAPSED_TIME_DIVIDER
  #define FCHOEM_ELAPSED_TIME_DIVIDER        100
#endif

#include "Fch.h"
#include "FchCommonCfg.h"
#include "AmdFch.h"

extern CONST BUILD_OPT_CFG    UserOptions;

#endif // _FCH_PLATFORM_H_
