/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH BIOS Ram usage
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
*****************************************************************************
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
****************************************************************************
*/
#ifndef _FCH_BIOS_RAM_USAGE_H_
#define _FCH_BIOS_RAM_USAGE_H_

#define RESTORE_MEMORY_CONTROLLER_START       0
#define XHCI_REGISTER_BAR00                   0xD0
#define XHCI_REGISTER_BAR01                   0xD1
#define XHCI_REGISTER_BAR02                   0xD2
#define XHCI_REGISTER_BAR03                   0xD3
#define XHCI_REGISTER_04H                     0xD4
#define XHCI_REGISTER_0CH                     0xD5
#define XHCI_REGISTER_3CH                     0xD6
#define XHCI1_REGISTER_BAR00                  0xE0
#define XHCI1_REGISTER_BAR01                  0xE1
#define XHCI1_REGISTER_BAR02                  0xE2
#define XHCI1_REGISTER_BAR03                  0xE3
#define XHCI1_REGISTER_04H                    0xE4
#define XHCI1_REGISTER_0CH                    0xE5
#define XHCI1_REGISTER_3CH                    0xE6
#define RTC_WORKAROUND_DATA_START             0xF0
#define BOOT_TIME_FLAG_SEC                    0xF8
#define BOOT_TIME_FLAG_INT19                  0xFC

#endif

