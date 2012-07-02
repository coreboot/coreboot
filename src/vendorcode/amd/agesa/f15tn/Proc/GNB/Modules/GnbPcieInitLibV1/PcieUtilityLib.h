/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe utility. Various supporting functions.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
* ***************************************************************************
*
*/

#ifndef _PCIEUTILLIB_H_
#define _PCIEUTILLIB_H_

/// Core lanes
typedef enum {
  AllCoreLanes,           ///< All core lanes
  AllocatedCoreLanes,     ///< Allocated core lanes
  ActiveCoreLanes,        ///< Active core lanes
  HotplugCoreLanes,       ///< Hot plug core lanes
  SbCoreLanes,            ///< South bridge core lanes
} CORE_LANES;

/// DDI lanes
typedef enum {
  DdiAllLanes,            ///< All DDI Lanes
  DdiActiveLanes          ///< Active DDI Lanes
} DDI_LANES;

BOOLEAN
PcieUtilSearchArray (
  IN      UINT8                         *Buf1,
  IN      UINTN                         Buf1Length,
  IN      UINT8                         *Buf2,
  IN      UINTN                         Buf2Length
  );

VOID
PcieUtilGetLinkHwStateHistory (
  IN       PCIe_ENGINE_CONFIG            *Engine,
     OUT   UINT8                         *History,
  IN       UINT8                         Length,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  );


BOOLEAN
PcieUtilIsLinkReversed (
  IN      BOOLEAN                        HwLinkState,
  IN      PCIe_ENGINE_CONFIG             *Engine,
  IN      PCIe_PLATFORM_CONFIG           *Pcie
  );


UINT8
PcieUtilGetLinkWidth (
  IN      PCIe_ENGINE_CONFIG            *Engine,
  IN      PCIe_PLATFORM_CONFIG          *Pcie
  );


UINT32
PcieUtilGetEngineLaneBitMap (
  IN      UINT32                         IncludeLaneType,
  IN      UINT32                         ExcludeLaneType,
  IN      PCIe_ENGINE_CONFIG             *Engine
  );

UINT32
PcieUtilGetWrapperLaneBitMap (
  IN      UINT32                         IncludeLaneType,
  IN      UINT32                         ExcludeLaneType,
  IN      PCIe_WRAPPER_CONFIG            *Wrapper
  );

VOID
PciePortProgramRegisterTable (
  IN      PCIE_PORT_REGISTER_ENTRY       *Table,
  IN      UINTN                          Length,
  IN      PCIe_ENGINE_CONFIG             *Engine,
  IN      BOOLEAN                        S3Save,
  IN      PCIe_PLATFORM_CONFIG           *Pcie
  );

VOID
PcieLockRegisters (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  );

PCIE_LINK_SPEED_CAP
PcieUtilGlobalGenCapability (
  IN      UINT32                         Flags,
  IN      PCIe_PLATFORM_CONFIG           *Pcie
  );

#endif
