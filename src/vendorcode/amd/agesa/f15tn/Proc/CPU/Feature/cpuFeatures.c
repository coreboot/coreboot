/* $NoKeywords:$ */
/**
 * @file
 *
 * Implement general feature dispatcher.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
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

/*----------------------------------------------------------------------------------------
 *                            M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "Ids.h"
#include "GeneralServices.h"
#include "cpuFeatures.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)
#define FILECODE PROC_CPU_FEATURE_CPUFEATURES_FILECODE
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

/*----------------------------------------------------------------------------------------
 *            E X P O R T E D    F U N C T I O N S - External General Services API
 *----------------------------------------------------------------------------------------
 */
extern CONST CPU_FEATURE_DESCRIPTOR* ROMDATA SupportedCpuFeatureList[];

/**
 * Determines if a specific feature is or will be enabled.
 *
 * This code traverses the feature list until a match is
 * found, then invokes the 'IsEnabled' function of the
 * feature.
 *
 * @param[in]       Feature        Indicates the desired feature.
 * @param[in]       PlatformConfig Contains the runtime modifiable feature input data.
 * @param[in]       StdHeader      Standard AMD configuration parameters.
 *
 * @retval         TRUE      Feature is or will be enabled
 * @retval         FALSE     Feature is not enabled
 */
BOOLEAN
IsFeatureEnabled (
  IN       DISPATCHABLE_CPU_FEATURES Feature,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINTN i;

  ASSERT (Feature < MaxCpuFeature);

  for (i = 0; SupportedCpuFeatureList[i] != NULL; i++) {
    if (SupportedCpuFeatureList[i]->Feature == Feature) {
      return (SupportedCpuFeatureList[i]->IsEnabled (PlatformConfig, StdHeader));
    }
  }
  return FALSE;
}

/**
 * Dispatches all features needing to perform some initialization at
 * this time point.
 *
 * This routine searches the feature table for features needing to
 * run at this time point, and invokes them.
 *
 * @param[in]      EntryPoint     Timepoint designator
 * @param[in]      PlatformConfig Contains the runtime modifiable feature input data.
 * @param[in]      StdHeader      Standard AMD configuration parameters.
 *
 * @return         The most severe status of any called service.
 */
AGESA_STATUS
DispatchCpuFeatures (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINTN i;
  AGESA_STATUS AgesaStatus;
  AGESA_STATUS CalledStatus;
  AGESA_STATUS IgnoredStatus;

  AgesaStatus = AGESA_SUCCESS;

  if (IsBsp (StdHeader, &IgnoredStatus)) {
    for (i = 0; SupportedCpuFeatureList[i] != NULL; i++) {
      if ((SupportedCpuFeatureList[i]->EntryPoint & EntryPoint) != 0) {
        IDS_SKIP_HOOK (IDS_CPU_FEAT, (CPU_FEATURE_DESCRIPTOR *) SupportedCpuFeatureList[i], StdHeader) {
          if (SupportedCpuFeatureList[i]->IsEnabled (PlatformConfig, StdHeader)) {
            CalledStatus = SupportedCpuFeatureList[i]->InitializeFeature (EntryPoint, PlatformConfig, StdHeader);
            if (CalledStatus > AgesaStatus) {
              AgesaStatus = CalledStatus;
            }
          }
        }
      }
    }
  }
  return AgesaStatus;
}

/**
 * This routine checks whether any non-coherent links in the system
 * runs in HT1 mode; used to determine whether certain features
 * should be disabled when this routine returns TRUE.
 *
 * @param[in]      StdHeader  Standard AMD configuration parameters.
 *
 * @retval         TRUE       One of the non-coherent links in the
 *                            system runs in HT1 mode
 * @retval         FALSE      None of the non-coherent links in the
 *                            system is running in HT1 mode
 */
BOOLEAN
IsNonCoherentHt1 (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINTN                 Link;
  UINT32                Socket;
  UINT32                Module;
  PCI_ADDR              PciAddress;
  AGESA_STATUS          AgesaStatus;
  HT_HOST_FEATS         HtHostFeats;
  CPU_SPECIFIC_SERVICES *CpuServices;

  for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {
      GetCpuServicesOfSocket (Socket, (CONST CPU_SPECIFIC_SERVICES **)&CpuServices, StdHeader);
      for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
        if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &AgesaStatus)) {
          HtHostFeats.HtHostValue = 0;
          Link = 0;
          while (CpuServices->GetNextHtLinkFeatures (CpuServices, &Link, &PciAddress, &HtHostFeats, StdHeader)) {
            // Return TRUE and exit routine once we find a non-coherent link in HT1
            if ((HtHostFeats.HtHostFeatures.NonCoherent == 1) && (HtHostFeats.HtHostFeatures.Ht1 == 1)) {
              return TRUE;
            }
          }
        }
      }
    }
  }

  return FALSE;
}
