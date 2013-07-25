/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_16 MMIO map manager
 *
 * manage MMIO base/limit registers.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F16
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "mmioMapManager.h"
#include "cpuF16MmioMap.h"
#include "S3SaveState.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X16_CPUF16MMIOMAP_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
STATIC CONST UINT16 ROMDATA MmioBaseLowRegOffset[MMIO_REG_PAIR_NUM] = {0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0x1A0, 0x1A8, 0x1B0, 0x1B8};
STATIC CONST UINT16 ROMDATA MmioLimitLowRegOffset[MMIO_REG_PAIR_NUM] = {0x84, 0x8C, 0x94, 0x9C, 0xA4, 0xAC, 0xB4, 0xBC, 0x1A4, 0x1AC, 0x1B4, 0x1BC};
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
#define F16_MMIO_ALIGN 0x10000l

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------*/
/**
 * BSC entry point for for adding MMIO map
 *
 * program MMIO base/limit registers
 *
 * @param[in]    MmioMapServices    MMIO map manager services.
 * @param[in]    AmdAddMmioParams   Pointer to a data structure containing the parameter information.
 *
 * @retval       AGESA_STATUS       AGESA_ERROR       - The requested range could not be added because there are not
 *                                                      enough mapping resources.
 *                                  AGESA_BOUNDS_CHK  - One or more input parameters are invalid. For example, the
 *                                                      TargetAddress does not correspond to any device in the system.
 *                                  AGESA_SUCCESS     - Adding MMIO map succeeds
 */
AGESA_STATUS
STATIC
cpuF16AddingMmioMap (
  IN       MMIO_MAP_FAMILY_SERVICES *MmioMapServices,
  IN       AMD_ADD_MMIO_PARAMS AmdAddMmioParams
  )
{
  UINT8               i;
  UINT8               j;
  UINT8               UnusedMmioPair;
  UINT8               Socket;
  UINT8               Module;
  UINT8               MmioPair;
  AGESA_STATUS        IgnoredSts;
  PCI_ADDR            PciAddress;
  MMIO_BASE_LOW       MmioBaseLow;
  MMIO_LIMIT_LOW      MmioLimitLow;
  MMIO_RANGE          MmioRange[MMIO_REG_PAIR_NUM];
  MMIO_RANGE          MmioRangeTemp;
  MMIO_RANGE          NewMmioRange;
  MMIO_RANGE          FragmentMmioRange;
  BOOLEAN             Overlap;
  BOOLEAN             NewMmioIncluded;

  UnusedMmioPair = 0;
  // FragmentMMioRange is used for record the MMIO range which is splitted by overriding.
  FragmentMmioRange.Attribute.MmioReadableRange = 0;
  FragmentMmioRange.Attribute.MmioWritableRange = 0;
  FragmentMmioRange.Base = 0;
  FragmentMmioRange.Limit = F16_MMIO_ALIGN;

  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 0x18, FUNC_1, MmioBaseLowRegOffset[0]);
  IDS_HDT_CONSOLE (MAIN_FLOW, "MMIO map configuration before merging:\n");
  IDS_HDT_CONSOLE (MAIN_FLOW, "    Base                 Limit               NP RE WE Lock DstNode DstLink DstSubLink\n");
  for (MmioPair = 0; MmioPair < MMIO_REG_PAIR_NUM; MmioPair++) {
    // MMIO base low
    PciAddress.Address.Register = MmioBaseLowRegOffset[MmioPair];
    LibAmdPciRead (AccessWidth32, PciAddress, &MmioBaseLow, &(AmdAddMmioParams.StdHeader));
    // MMIO limit low
    PciAddress.Address.Register = MmioLimitLowRegOffset[MmioPair];
    LibAmdPciRead (AccessWidth32, PciAddress, &MmioLimitLow, &(AmdAddMmioParams.StdHeader));
    // get MMIO info
    MmioRange[MmioPair].Base = (MmioBaseLow.MmioBase << 16);
    MmioRange[MmioPair].Limit = (MmioLimitLow.MmioLimit << 16) + F16_MMIO_ALIGN;
    MmioRange[MmioPair].Attribute.MmioPostedRange =  (UINT8) MmioLimitLow.NP;
    MmioRange[MmioPair].Attribute.MmioReadableRange = (UINT8) MmioBaseLow.RE;
    MmioRange[MmioPair].Attribute.MmioWritableRange =  (UINT8) MmioBaseLow.WE;
    MmioRange[MmioPair].Attribute.MmioSecuredRange =  (UINT8) MmioBaseLow.Lock;
    MmioRange[MmioPair].Destination.DstNode = MmioLimitLow.DstNode;
    MmioRange[MmioPair].Destination.DstLink = MmioLimitLow.DstLink;
    MmioRange[MmioPair].Destination.DstSubLink = MmioLimitLow.DstSubLink;
    MmioRange[MmioPair].RangeNum = MmioPair;
    MmioRange[MmioPair].Modified = FALSE;
    if ((MmioRange[MmioPair].Attribute.MmioReadableRange == 0) && (MmioRange[MmioPair].Attribute.MmioWritableRange == 0)) {
      UnusedMmioPair++;
    }
    IDS_HDT_CONSOLE (MAIN_FLOW, " %02d ", MmioPair);
    IDS_HDT_CONSOLE (MAIN_FLOW, "%08x%08x    %08x%08x", (MmioRange[MmioPair].Base >> 32) & 0xFFFFFFFF,
                                                         MmioRange[MmioPair].Base & 0xFFFFFFFF,
                                                        (MmioRange[MmioPair].Limit >> 32) & 0xFFFFFFFF,
                                                         MmioRange[MmioPair].Limit & 0xFFFFFFFF);
    IDS_HDT_CONSOLE (MAIN_FLOW, "    %s  %s  %s  %s", MmioRange[MmioPair].Attribute.MmioPostedRange ? "Y" : "N",
                                                      MmioRange[MmioPair].Attribute.MmioReadableRange ? "Y" : "N",
                                                      MmioRange[MmioPair].Attribute.MmioWritableRange ? "Y" : "N",
                                                      MmioRange[MmioPair].Attribute.MmioSecuredRange ? "Y" : "N");
    IDS_HDT_CONSOLE (MAIN_FLOW, "    %02d      %02d      %02d\n", MmioRange[MmioPair].Destination.DstNode,
                                                                  MmioRange[MmioPair].Destination.DstLink,
                                                                  MmioRange[MmioPair].Destination.DstSubLink);
  }

  // parse requirement
  NewMmioRange.Base = AmdAddMmioParams.BaseAddress;
  NewMmioRange.Limit = AmdAddMmioParams.BaseAddress + AmdAddMmioParams.Length;
  NewMmioRange.Attribute = AmdAddMmioParams.Attributes;
  IDS_HDT_CONSOLE (MAIN_FLOW, "req %08x%08x    %08x%08x\n", (NewMmioRange.Base >> 32) & 0xFFFFFFFF,
                                                         NewMmioRange.Base & 0xFFFFFFFF,
                                                        (NewMmioRange.Limit >> 32) & 0xFFFFFFFF,
                                                         NewMmioRange.Limit & 0xFFFFFFFF);


  // sort by base address
  //    range0, range1, range2, non used, non used...
  for (i = 0; i < (MMIO_REG_PAIR_NUM - 1); i++) {
    for (j = 0; j < (MMIO_REG_PAIR_NUM - i - 1); j++) {
      if (((MmioRange[j].Base > MmioRange[j + 1].Base) && ((MmioRange[j + 1].Attribute.MmioReadableRange != 0) || (MmioRange[j + 1].Attribute.MmioWritableRange != 0))) ||
          (((MmioRange[j].Attribute.MmioReadableRange == 0) && (MmioRange[j].Attribute.MmioWritableRange == 0)) &&
          ((MmioRange[j + 1].Attribute.MmioReadableRange != 0) || (MmioRange[j + 1].Attribute.MmioWritableRange != 0)))) {
        MmioRangeTemp = MmioRange[j];
        MmioRange[j] = MmioRange[j + 1];
        MmioRange[j + 1] = MmioRangeTemp;
      }
    }
  }

  // merge the request to current setting
  Overlap = FALSE;
  NewMmioIncluded = FALSE;
  for (MmioPair = 0; MmioPair < MMIO_REG_PAIR_NUM; MmioPair++) {
    if (MmioRange[MmioPair].Attribute.MmioReadableRange != 0 || MmioRange[MmioPair].Attribute.MmioWritableRange != 0) {
      if (((NewMmioRange.Base <= MmioRange[MmioPair].Base) && (NewMmioRange.Limit >= MmioRange[MmioPair].Base)) ||
          ((MmioRange[MmioPair].Base <= NewMmioRange.Base) && (MmioRange[MmioPair].Limit > NewMmioRange.Base))) {
        if ((NewMmioRange.Attribute.MmioPostedRange == MmioRange[MmioPair].Attribute.MmioPostedRange) &&
            (NewMmioRange.Attribute.MmioReadableRange == MmioRange[MmioPair].Attribute.MmioReadableRange) &&
            (NewMmioRange.Attribute.MmioWritableRange == MmioRange[MmioPair].Attribute.MmioWritableRange) &&
            (NewMmioRange.Attribute.MmioSecuredRange == MmioRange[MmioPair].Attribute.MmioSecuredRange)) {

//  Original sorted MMIO register pair defined ranges:
//                                          ____________            ________          ____________
//                                          |          |            |      |          |          |
//                                        base0      limit0       base1  limit1     base2      limit2
//  Requested MMIO range:
//    case 1:
//     ((NewMmioRange.Base <= MmioRange[MmioPair].Base) && (NewMmioRange.Limit >= MmioRange[MmioPair].Base))
//                                                              __________
//                                                              |        |
//                                                         new base   new limit
//                                                              ____________________
//                                                              |                  |
//                                                         new base             new limit
//    case 2:
//    ((MmioRange[MmioPair].Base <= NewMmioRange.Base) && (MmioRange[MmioPair].Limit >= NewMmioRange.Base))
//                                                                                       ____________
//                                                                                       |          |
//                                                                                   new base   new limit

          MmioRange[MmioPair].Base = (MmioRange[MmioPair].Base <= NewMmioRange.Base) ? MmioRange[MmioPair].Base : NewMmioRange.Base;
          MmioRange[MmioPair].Modified = TRUE;
          for (i = 1; NewMmioRange.Limit >= MmioRange[MmioPair + i].Base; i++) {
            if ((NewMmioRange.Attribute.MmioPostedRange == MmioRange[MmioPair + i].Attribute.MmioPostedRange) &&
                (NewMmioRange.Attribute.MmioReadableRange == MmioRange[MmioPair + i].Attribute.MmioReadableRange) &&
                (NewMmioRange.Attribute.MmioWritableRange == MmioRange[MmioPair + i].Attribute.MmioWritableRange) &&
                (NewMmioRange.Attribute.MmioSecuredRange == MmioRange[MmioPair + i].Attribute.MmioSecuredRange)) {
              MmioRange[MmioPair].Limit = MmioRange[MmioPair + i].Limit;
              MmioRange[MmioPair + i].Base = 0;
              MmioRange[MmioPair + i].Limit = F16_MMIO_ALIGN;
              MmioRange[MmioPair + i].Attribute.MmioReadableRange = 0;
              MmioRange[MmioPair + i].Attribute.MmioWritableRange = 0;
              MmioRange[MmioPair + i].Modified = TRUE;
              UnusedMmioPair++;
            } else if (MmioRange[MmioPair + i].Attribute.MmioReadableRange != 0 || MmioRange[MmioPair + i].Attribute.MmioWritableRange != 0) {
              // Overlapped MMIO regions with different attributes
              MmioRange[MmioPair].Limit = (MmioRange[MmioPair].Limit >= NewMmioRange.Limit) ? MmioRange[MmioPair].Limit : NewMmioRange.Limit;
              NewMmioIncluded = TRUE;
              Overlap = TRUE;
              break;
            }
          }
          MmioRange[MmioPair].Limit = (MmioRange[MmioPair + i - 1].Limit >= NewMmioRange.Limit) ? MmioRange[MmioPair + i - 1].Limit : NewMmioRange.Limit;
          break;
        } else {
          // Overlapped MMIO regions with different attributes
          Overlap = TRUE;
          break;
        }
      }
    } else {

//  Original sorted MMIO register pair defined ranges:
//                                          ____________            ________          ____________
//                                          |          |            |      |          |          |
//                                        base0      limit0       base1  limit1     base2      limit2
//  Requested MMIO range:
//    case 3:
//     No overlapping area with the original ranges
//                    ____________
//                    |          |
//                new base   new limit
//                                                                                                        ______________
//                                                                                                        |            |
//                                                                                                    new base     new limit

      MmioRange[MmioPair].Base = NewMmioRange.Base;
      MmioRange[MmioPair].Limit = NewMmioRange.Limit;
      MmioRange[MmioPair].Attribute = NewMmioRange.Attribute;
      MmioRange[MmioPair].Modified = TRUE;

      break;
    }
  }

  if (MmioPair == MMIO_REG_PAIR_NUM) {
    IDS_HDT_CONSOLE (MAIN_FLOW, " [ERROR] Not enough MMIO register pairs to hold the request.\n");
    return AGESA_ERROR;
  }

  if (Overlap) {
    if (NewMmioRange.Attribute.OverrideExisting) {
      // First loop, to see which existing MMIO range should be overrided
      for (MmioPair = 0; MmioPair < MMIO_REG_PAIR_NUM; MmioPair++) {
        if ((MmioRange[MmioPair].Modified == TRUE) || (NewMmioRange.Limit < MmioRange[MmioPair].Base) || (NewMmioRange.Base > MmioRange[MmioPair].Limit)) {
          continue;
        } else {
          // There's an overlap between NewMmio and MmioRange[MmioPair]

          //                  ____________________
          //                  |                  |
          //                base0              limit0
          //                       __________
          //                       |        |
          //                   new base   new limit
          if ((NewMmioRange.Base >= MmioRange[MmioPair].Base) && (NewMmioRange.Limit <= MmioRange[MmioPair].Limit)) {
            if ((MmioRange[MmioPair].Limit - NewMmioRange.Limit) >= F16_MMIO_ALIGN) {
              FragmentMmioRange.Base = NewMmioRange.Limit;
              FragmentMmioRange.Limit = MmioRange[MmioPair].Limit;
              FragmentMmioRange.Attribute = MmioRange[MmioPair].Attribute;
              FragmentMmioRange.Destination = MmioRange[MmioPair].Destination;
            }
            if ((NewMmioRange.Base -  MmioRange[MmioPair].Base) < F16_MMIO_ALIGN) {
              MmioRange[MmioPair].Attribute.MmioReadableRange = 0;
              MmioRange[MmioPair].Attribute.MmioWritableRange = 0;
              UnusedMmioPair++;
            }
            MmioRange[MmioPair].Limit = NewMmioRange.Base;
            MmioRange[MmioPair].Modified = TRUE;
          }
          //                  ____________________
          //                  |                  |
          //                base0              limit0
          //            ______________
          //            |            |
          //         new base    new limit
          if ((NewMmioRange.Base < MmioRange[MmioPair].Base) && (NewMmioRange.Limit <= MmioRange[MmioPair].Limit) && (NewMmioRange.Limit > MmioRange[MmioPair].Base)) {
            MmioRange[MmioPair].Base = NewMmioRange.Limit;
            MmioRange[MmioPair].Modified = TRUE;
            if ((MmioRange[MmioPair].Limit - NewMmioRange.Limit) < F16_MMIO_ALIGN) {
              MmioRange[MmioPair].Attribute.MmioReadableRange = 0;
              MmioRange[MmioPair].Attribute.MmioWritableRange = 0;
              UnusedMmioPair++;
            }
          }
          //                  ____________________
          //                  |                  |
          //                base0              limit0
          //                                 ______________
          //                                 |            |
          //                              new base    new limit
          if ((NewMmioRange.Base >= MmioRange[MmioPair].Base) && (NewMmioRange.Base < MmioRange[MmioPair].Limit) && (NewMmioRange.Limit > MmioRange[MmioPair].Limit)) {
            MmioRange[MmioPair].Limit = NewMmioRange.Base;
            MmioRange[MmioPair].Modified = TRUE;
            if ((NewMmioRange.Base - MmioRange[MmioPair].Base) < F16_MMIO_ALIGN) {
              MmioRange[MmioPair].Attribute.MmioReadableRange = 0;
              MmioRange[MmioPair].Attribute.MmioWritableRange = 0;
              UnusedMmioPair++;
            }
          }
          //                  _________
          //                  |       |
          //                base0   limit0
          //             ___________________
          //             |                 |
          //          new base         new limit
          if ((NewMmioRange.Base <= MmioRange[MmioPair].Base) && (NewMmioRange.Limit >= MmioRange[MmioPair].Limit)) {
            MmioRange[MmioPair].Base = 0;
            MmioRange[MmioPair].Limit = F16_MMIO_ALIGN;
            MmioRange[MmioPair].Attribute.MmioReadableRange = 0;
            MmioRange[MmioPair].Attribute.MmioWritableRange = 0;
            MmioRange[MmioPair].Attribute.MmioPostedRange = 0;
            MmioRange[MmioPair].Attribute.MmioSecuredRange = 0;
            MmioRange[MmioPair].Modified = TRUE;
            UnusedMmioPair++;
          }
        }
      }
      // Let's see if there's enough MMIO registers for NewMmioRange and FragmentMmioRange
      if (!NewMmioIncluded) {
        UnusedMmioPair--;
      }
      if ((FragmentMmioRange.Attribute.MmioReadableRange != 0) && (FragmentMmioRange.Attribute.MmioWritableRange != 0)) {
        UnusedMmioPair--;
      }

      if ((UnusedMmioPair != 0) && (UnusedMmioPair <= MMIO_REG_PAIR_NUM)) {
        // Set MMIO for NewMmioRange and FragmentMmioRange
        for (MmioPair = 0; MmioPair < MMIO_REG_PAIR_NUM; MmioPair++) {
          if ((MmioRange[MmioPair].Attribute.MmioReadableRange == 0) && (MmioRange[MmioPair].Attribute.MmioWritableRange == 0)) {
            if (!NewMmioIncluded) {
              MmioRange[MmioPair].Base = NewMmioRange.Base;
              MmioRange[MmioPair].Limit = NewMmioRange.Limit;
              MmioRange[MmioPair].Attribute = NewMmioRange.Attribute;
              MmioRange[MmioPair].Modified = TRUE;
              NewMmioIncluded = TRUE;
            } else if ((FragmentMmioRange.Attribute.MmioReadableRange != 0) && (FragmentMmioRange.Attribute.MmioWritableRange != 0)) {
              MmioRange[MmioPair].Base = FragmentMmioRange.Base;
              MmioRange[MmioPair].Limit = FragmentMmioRange.Limit;
              MmioRange[MmioPair].Attribute = FragmentMmioRange.Attribute;
              MmioRange[MmioPair].Destination = FragmentMmioRange.Destination;
              MmioRange[MmioPair].Modified = TRUE;
              FragmentMmioRange.Attribute.MmioReadableRange = 0;
              FragmentMmioRange.Attribute.MmioWritableRange = 0;
            } else {
              break;
            }
          }
        }
      } else {
        // We don't have enough MMIO registers.
        IDS_HDT_CONSOLE (MAIN_FLOW, " [ERROR] Not enough MMIO register pairs to hold the request.\n");
        return AGESA_ERROR;
      }
    } else {
      // Overlapped MMIO regions with different attributes
      // and the caller doesn't want to override existing MMIO setting.
      IDS_HDT_CONSOLE (MAIN_FLOW, " [ERROR] Overlapped MMIO regions with different attributes.\n");
      return AGESA_ERROR;
    }
  }
  // write back MMIO base/limit
  IDS_HDT_CONSOLE (MAIN_FLOW, "MMIO map configuration after merging:\n");
  IDS_HDT_CONSOLE (MAIN_FLOW, "    Base                 Limit               NP RE WE Lock DstNode DstLink DstSubLink\n");
  for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
    if (IsProcessorPresent (Socket, &(AmdAddMmioParams.StdHeader))) {
      for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
        if (GetPciAddress (&(AmdAddMmioParams.StdHeader), Socket, Module, &PciAddress, &IgnoredSts)) {
          PciAddress.Address.Function = FUNC_1;
          for (MmioPair = 0; MmioPair < MMIO_REG_PAIR_NUM; MmioPair++) {
            IDS_HDT_CONSOLE (MAIN_FLOW, " %02d ", MmioPair);
            IDS_HDT_CONSOLE (MAIN_FLOW, "%08x%08x    %08x%08x", (MmioRange[MmioPair].Base >> 32) & 0xFFFFFFFF,
                                                                 MmioRange[MmioPair].Base & 0xFFFFFFFF,
                                                                (MmioRange[MmioPair].Limit >> 32) & 0xFFFFFFFF,
                                                                 MmioRange[MmioPair].Limit & 0xFFFFFFFF);
            IDS_HDT_CONSOLE (MAIN_FLOW, "    %s  %s  %s  %s", MmioRange[MmioPair].Attribute.MmioPostedRange ? "Y" : "N",
                                                              MmioRange[MmioPair].Attribute.MmioReadableRange ? "Y" : "N",
                                                              MmioRange[MmioPair].Attribute.MmioWritableRange ? "Y" : "N",
                                                              MmioRange[MmioPair].Attribute.MmioSecuredRange ? "Y" : "N");
            IDS_HDT_CONSOLE (MAIN_FLOW, "    %02d      %02d      %02d\n", MmioRange[MmioPair].Destination.DstNode,
                                                                          MmioRange[MmioPair].Destination.DstLink,
                                                                          MmioRange[MmioPair].Destination.DstSubLink);
            if (MmioRange[MmioPair].Modified) {
              // MMIO base low
              PciAddress.Address.Register = MmioBaseLowRegOffset[MmioRange[MmioPair].RangeNum];
              LibAmdPciRead (AccessWidth32, PciAddress, &MmioBaseLow, &(AmdAddMmioParams.StdHeader));
              if (MmioBaseLow.Lock == 1) {
                IDS_HDT_CONSOLE (MAIN_FLOW, " [ERROR] MMIO register pair locked.\n");
                return AGESA_ERROR;
              }
              // Disable RE/WE before changing the address range
              MmioBaseLow.RE = 0;
              MmioBaseLow.WE = 0;
              S3_SAVE_PCI_WRITE (&(AmdAddMmioParams.StdHeader), PciAddress, AccessWidth32, &MmioBaseLow);
              LibAmdPciWrite (AccessWidth32, PciAddress, &MmioBaseLow, &(AmdAddMmioParams.StdHeader));

              IDS_HDT_CONSOLE (MAIN_FLOW, " Reconfiguring offset %X\n", MmioBaseLowRegOffset[MmioRange[MmioPair].RangeNum]);
              MmioBaseLow.MmioBase = (UINT32) (MmioRange[MmioPair].Base >> 16) & 0xFFFFFFul;
              MmioBaseLow.RE = MmioRange[MmioPair].Attribute.MmioReadableRange;
              MmioBaseLow.WE = MmioRange[MmioPair].Attribute.MmioWritableRange;
              S3_SAVE_PCI_WRITE (&(AmdAddMmioParams.StdHeader), PciAddress, AccessWidth32, &MmioBaseLow);
              LibAmdPciWrite (AccessWidth32, PciAddress, &MmioBaseLow, &(AmdAddMmioParams.StdHeader));

              // MMIO limit low
              IDS_HDT_CONSOLE (MAIN_FLOW, " Reconfiguring offset %X\n", MmioLimitLowRegOffset[MmioRange[MmioPair].RangeNum]);
              PciAddress.Address.Register = MmioLimitLowRegOffset[MmioRange[MmioPair].RangeNum];
              LibAmdPciRead (AccessWidth32, PciAddress, &MmioLimitLow, &(AmdAddMmioParams.StdHeader));
              MmioLimitLow.MmioLimit = (UINT32) ((MmioRange[MmioPair].Limit - 1) >> 16) & 0xFFFFFFul;
              MmioLimitLow.NP = MmioRange[MmioPair].Attribute.MmioPostedRange;
              MmioLimitLow.DstNode = MmioRange[MmioPair].Destination.DstNode;
              MmioLimitLow.DstLink = MmioRange[MmioPair].Destination.DstLink;
              MmioLimitLow.DstSubLink = MmioRange[MmioPair].Destination.DstSubLink;
              S3_SAVE_PCI_WRITE (&(AmdAddMmioParams.StdHeader), PciAddress, AccessWidth32, &MmioLimitLow);
              LibAmdPciWrite (AccessWidth32, PciAddress, &MmioLimitLow, &(AmdAddMmioParams.StdHeader));
            }
          }
        }
      }
    }
  }
  return AGESA_SUCCESS;
}

CONST MMIO_MAP_FAMILY_SERVICES ROMDATA F16MmioMapSupport =
{
  0,
  cpuF16AddingMmioMap
};
