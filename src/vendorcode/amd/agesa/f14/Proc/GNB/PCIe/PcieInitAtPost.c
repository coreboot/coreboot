/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe late post initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 44325 $   @e \$Date: 2010-12-22 03:29:53 -0700 (Wed, 22 Dec 2010) $
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
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "PcieInit.h"
#include  "PciePortInit.h"
#include  GNB_MODULE_DEFINITIONS (GnbPcieInitLibV1)
#include  GNB_MODULE_DEFINITIONS (GnbPcieConfig)
#include  GNB_MODULE_DEFINITIONS (GnbPcieTrainingV1)
#include  "Filecode.h"
#define FILECODE PROC_GNB_PCIE_PCIEINITATPOST_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
PcieInitAtPostEarly (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

AGESA_STATUS
PcieInitAtPost (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

AGESA_STATUS
PcieInitAtPostS3 (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

VOID
PcieLateRestoreS3Script (
  IN      AMD_CONFIG_PARAMS               *StdHeader,
  IN      UINT16                          ContextLength,
  IN      VOID*                           Context
  );

/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Post Init prior DRAM init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */
AGESA_STATUS
PcieInitAtPostEarly (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieInitAtPostEarly Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    PciePortsVisibilityControl (UnhidePorts, Pcie);

    Status = PciePortPostEarlyInit (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PcieTraining (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    PciePortsVisibilityControl (HidePorts, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieInitAtPostEarly Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Post Init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */
AGESA_STATUS
PcieInitAtPost (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieInitAtPost Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    PciePortsVisibilityControl (UnhidePorts, Pcie);

    Status = PciePostInit (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PciePortPostInit (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PcieTraining (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    PciePortsVisibilityControl (HidePorts, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieInitAtPost Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}


/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Post Init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */
AGESA_STATUS
PcieInitAtPostS3 (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieInitAtPostS3 Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    PciePortsVisibilityControl (UnhidePorts, Pcie);

    Status = PciePostInit (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    if (Pcie->TrainingAlgorithm == PcieTrainingDistributed) {
      Status = PciePortPostS3Init (Pcie);
    } else {
      Status = PciePortPostInit (Pcie);
    }
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    Status = PcieTraining (Pcie);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);

    PciePortsVisibilityControl (HidePorts, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieInitAtPostS3 Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * PCIe S3 restore
 *
 *
 *
 * @param[in]  StdHeader      Standard configuration header
 * @param[in]  ContextLength  Context Length (not used)
 * @param[in]  Context        Context pointer (not used)
 */
VOID
PcieLateRestoreS3Script (
  IN      AMD_CONFIG_PARAMS               *StdHeader,
  IN      UINT16                          ContextLength,
  IN      VOID*                           Context
  )
{
 PcieInitAtPostS3 (StdHeader);
}
