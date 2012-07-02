/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 specific utility functions.
 *
 * Provides numerous utility functions specific to family 15h.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

#ifndef _CPU_F15_UTILITES_H_
#define _CPU_F15_UTILITES_H_


/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
/// The structure for Software Initiated NB Voltage Transitions
typedef struct {
  UINT32  VidCode;          ///< VID code to transition to
  BOOLEAN SlamMode;         ///< Whether voltage is to be slammed, or stepped
} SW_VOLT_TRANS_NB;

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */

AGESA_STATUS
F15DisablePstate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8 StateNumber,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
F15TransitionPstate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT8              StateNumber,
  IN       BOOLEAN            WaitForTransition,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
F15GetTscRate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
F15LaunchApCore (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       UINT32 SocketNum,
  IN       UINT32 ModuleNum,
  IN       UINT32 CoreNum,
  IN       UINT32 PrimaryCoreNum,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
F15HtPhyOverrideDllCompensation (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

BOOLEAN
F15GetNextHtLinkFeatures (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN OUT   UINTN                  *Link,
  IN OUT   PCI_ADDR               *LinkBase,
     OUT   HT_HOST_FEATS          *HtHostFeats,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

BOOLEAN
F15NextLinkHasHtPhyFeats (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN OUT   PCI_ADDR           *HtHostCapability,
  IN OUT   UINT32             *Link,
  IN       HT_PHY_LINK_FEATS  *HtPhyLinkType,
     OUT   BOOLEAN            *MatchedSublink1,
     OUT   HT_FREQUENCIES     *Frequency0,
     OUT   HT_FREQUENCIES     *Frequency1,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
F15SetHtPhyRegister (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       HT_PHY_TYPE_ENTRY_DATA  *HtPhyEntry,
  IN       PCI_ADDR                 CapabilitySet,
  IN       UINT32                   Link,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );

BOOLEAN
F15CommonGetNbCofVidUpdate (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       PCI_ADDR *PciAddress,
     OUT   BOOLEAN *NbVidUpdateAll,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
F15IsNbPstateEnabled (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

#endif  // _CPU_F15_UTILITES_H_
