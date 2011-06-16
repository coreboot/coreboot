/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: GNB
 *
 * Contains AMD AGESA install macros and test conditions. Output is the
 * defaults tables reflecting the User's build options selection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 49877 $   @e \$Date: 2011-03-30 13:15:18 +0800 (Wed, 30 Mar 2011) $
 */
/*****************************************************************************
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
 ***************************************************************************/

#ifndef _OPTION_GNB_INSTALL_H_
#define _OPTION_GNB_INSTALL_H_

#include "S3SaveState.h"
/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */

//---------------------------------------------------------------------------------------------------
// Family installation
//---------------------------------------------------------------------------------------------------


#define GNB_TYPE_KR   FALSE
#define GNB_TYPE_TN   FALSE
#define GNB_TYPE_LN   FALSE
#define GNB_TYPE_ON   FALSE

#if (OPTION_FAMILY14H_ON == TRUE)
  #undef  GNB_TYPE_ON
  #define GNB_TYPE_ON  TRUE
#endif

#if (OPTION_FAMILY12H_LN == TRUE)
  #undef  GNB_TYPE_LN
  #define GNB_TYPE_LN  TRUE
#endif

#if (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
//---------------------------------------------------------------------------------------------------
// Service installation
//---------------------------------------------------------------------------------------------------

  #include "Gnb.h"
  #include "GnbPcie.h"

  #define SERVICES_POINTER  NULL
  GNB_SERVICE   *ServiceTable = SERVICES_POINTER;

//---------------------------------------------------------------------------------------------------
// BUILD options
//---------------------------------------------------------------------------------------------------

  #ifndef CFG_IGFX_AS_PCIE_EP
    #define CFG_IGFX_AS_PCIE_EP                   TRUE
  #endif

  #ifndef CFG_LCLK_DEEP_SLEEP_EN
    #define  CFG_LCLK_DEEP_SLEEP_EN               TRUE
  #endif

  #ifndef CFG_LCLK_DPM_EN
    #define CFG_LCLK_DPM_EN                       TRUE
  #endif

  #ifndef CFG_GMC_POWER_GATE_STUTTER_ONLY
    #define CFG_GMC_POWER_GATE_STUTTER_ONLY       FALSE
  #endif

  #ifndef CFG_SMU_SCLK_CLOCK_GATING_ENABLE
    #if (GNB_TYPE_ON == TRUE)
      #define  CFG_SMU_SCLK_CLOCK_GATING_ENABLE   TRUE
    #else
      #define CFG_SMU_SCLK_CLOCK_GATING_ENABLE    FALSE
    #endif
  #endif

  #ifndef CFG_PCIE_ASPM_BLACK_LIST_ENABLE
    #define CFG_PCIE_ASPM_BLACK_LIST_ENABLE       TRUE
  #endif

  #ifndef CFG_GNB_IVRS_RELATIVE_ADDR_NAMES_SUPPORT
    #define CFG_GNB_IVRS_RELATIVE_ADDR_NAMES_SUPPORT     FALSE
  #endif

  #ifndef CFG_GNB_LOAD_REAL_FUSE
    #define  CFG_GNB_LOAD_REAL_FUSE   TRUE
  #endif

  #ifndef CFG_GNB_PCIE_LINK_RECEIVER_DETECTION_POOLING
    #define CFG_GNB_PCIE_LINK_RECEIVER_DETECTION_POOLING  (60 * 1000)
  #endif

  #ifndef CFG_GNB_PCIE_LINK_L0_POOLING
    #define CFG_GNB_PCIE_LINK_L0_POOLING                  (60 * 1000)
  #endif

  #ifndef CFG_GNB_PCIE_LINK_GPIO_RESET_ASSERT_TIME
    #define CFG_GNB_PCIE_LINK_GPIO_RESET_ASSERT_TIME      (2 * 1000)
  #endif

  #ifndef CFG_GNB_PCIE_LINK_RESET_TO_TRAINING_TIME
    #define CFG_GNB_PCIE_LINK_RESET_TO_TRAINING_TIME      (2 * 1000)
  #endif

  #ifdef BLDCFG_PCIE_TRAINING_ALGORITHM
    #define CFG_GNB_PCIE_TRAINING_ALGORITHM               BLDCFG_PCIE_TRAINING_ALGORITHM
  #else
    #define CFG_GNB_PCIE_TRAINING_ALGORITHM               PcieTrainingStandard
  #endif

  #ifndef CFG_GNB_FORCE_CABLESAFE_OFF
    #define CFG_GNB_FORCE_CABLESAFE_OFF                   FALSE
  #endif

  #ifndef CFG_ORB_CLOCK_GATING_ENABLE
      #define CFG_ORB_CLOCK_GATING_ENABLE                 TRUE
  #endif

  #ifndef CFG_GNB_PCIE_POWERGATING_FLAGS
    #define CFG_GNB_PCIE_POWERGATING_FLAGS                0x0
  #endif

  #ifndef CFG_IOC_LCLK_CLOCK_GATING_ENABLE
      #define CFG_IOC_LCLK_CLOCK_GATING_ENABLE            FALSE
  #endif

  #ifndef CFG_IOC_SCLK_CLOCK_GATING_ENABLE
      #define CFG_IOC_SCLK_CLOCK_GATING_ENABLE            FALSE
  #endif

  #ifndef CFG_IOMMU_L1_CLOCK_GATING_ENABLE
      #define CFG_IOMMU_L1_CLOCK_GATING_ENABLE            FALSE
  #endif

  #ifndef CFG_IOMMU_L2_CLOCK_GATING_ENABLE
      #define CFG_IOMMU_L2_CLOCK_GATING_ENABLE            FALSE
  #endif

  #ifndef CFG_GNB_ALTVDDNB_SUPPORT
    #define CFG_GNB_ALTVDDNB_SUPPORT                      TRUE
  #endif

  GNB_BUILD_OPTIONS ROMDATA GnbBuildOptions = {
    CFG_IGFX_AS_PCIE_EP,
    CFG_LCLK_DEEP_SLEEP_EN,
    CFG_LCLK_DPM_EN,
    CFG_GMC_POWER_GATE_STUTTER_ONLY,
    CFG_SMU_SCLK_CLOCK_GATING_ENABLE,
    CFG_PCIE_ASPM_BLACK_LIST_ENABLE,
    CFG_GNB_IVRS_RELATIVE_ADDR_NAMES_SUPPORT,
    CFG_GNB_LOAD_REAL_FUSE,
    CFG_GNB_PCIE_LINK_RECEIVER_DETECTION_POOLING,
    CFG_GNB_PCIE_LINK_L0_POOLING,
    CFG_GNB_PCIE_LINK_GPIO_RESET_ASSERT_TIME,
    CFG_GNB_PCIE_LINK_RESET_TO_TRAINING_TIME,
    CFG_GNB_PCIE_TRAINING_ALGORITHM,
    CFG_GNB_FORCE_CABLESAFE_OFF,
    CFG_ORB_CLOCK_GATING_ENABLE,
    CFG_GNB_PCIE_POWERGATING_FLAGS,
    CFG_IOC_LCLK_CLOCK_GATING_ENABLE,
    CFG_IOC_SCLK_CLOCK_GATING_ENABLE,
    CFG_IOMMU_L1_CLOCK_GATING_ENABLE,
    CFG_IOMMU_L2_CLOCK_GATING_ENABLE,
    CFG_GNB_ALTVDDNB_SUPPORT
  };

  //---------------------------------------------------------------------------------------------------
  // Module entries
  //---------------------------------------------------------------------------------------------------

  #if (AGESA_ENTRY_INIT_EARLY == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_EARLY_INIT
      #define OPTION_NB_EARLY_INIT TRUE
    #endif
    #if (OPTION_NB_EARLY_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  NbInitAtEarly;
      #define OPTION_NBINITATEARLY_ENTRY                  {AMD_FAMILY_LN | AMD_FAMILY_ON, NbInitAtEarly},
    #else
      #define OPTION_NBINITATEARLY_ENTRY
    #endif
    #define OPTION_GNBEARLYINTERFACETN_ENTRY
  //---------------------------------------------------------------------------------------------------
  // SMU init
    #ifndef OPTION_SMU
      #define OPTION_SMU  TRUE
    #endif
    #if (OPTION_SMU == TRUE) && (GNB_TYPE_LN == TRUE)
      OPTION_GNB_FEATURE                                  F12NbSmuInitFeature;
      #define OPTION_F12NBSMUINITFEATURE_ENTRY            {AMD_FAMILY_LN, F12NbSmuInitFeature},
    #else
      #define OPTION_F12NBSMUINITFEATURE_ENTRY
    #endif
    #if (OPTION_SMU == TRUE) && (GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  F14NbSmuInitFeature;
      #define OPTION_F14NBSMUINITFEATURE_ENTRY            {AMD_FAMILY_ON, F14NbSmuInitFeature},
    #else
      #define OPTION_F14NBSMUINITFEATURE_ENTRY
    #endif
    #define OPTION_KRNBSMUINITFEATURE_ENTRY
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_CONFIG_INIT
      #define OPTION_PCIE_CONFIG_INIT TRUE
    #endif
    #if (OPTION_PCIE_CONFIG_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  PcieConfigurationInit;
      #define OPTION_PCIECONFIGURATIONINIT_ENTRY          {AMD_FAMILY_LN | AMD_FAMILY_ON, PcieConfigurationInit},
    #else
      #define OPTION_PCIECONFIGURATIONINIT_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_EARLY_INIT
      #define OPTION_PCIE_EARLY_INIT TRUE
    #endif
    #if (OPTION_PCIE_EARLY_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  PcieInitAtEarly;
      #define OPTION_PCIEINITATEARLY_ENTRY                {AMD_FAMILY_LN | AMD_FAMILY_ON, PcieInitAtEarly},
    #else
      #define  OPTION_PCIEINITATEARLY_ENTRY
    #endif
    #define  OPTION_PCIEEARLYINTERFACETN_ENTRY
  //---------------------------------------------------------------------------------------------------
    OPTION_GNB_CONFIGURATION  GnbEarlyFeatureTable[] = {
      OPTION_NBINITATEARLY_ENTRY
      OPTION_GNBEARLYINTERFACETN_ENTRY
      OPTION_F12NBSMUINITFEATURE_ENTRY
      OPTION_F14NBSMUINITFEATURE_ENTRY
      OPTION_KRNBSMUINITFEATURE_ENTRY
      OPTION_PCIECONFIGURATIONINIT_ENTRY
      OPTION_PCIEINITATEARLY_ENTRY
      OPTION_PCIEEARLYINTERFACETN_ENTRY
      {0, NULL}
    };

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_EARLIER_INIT
      #define OPTION_NB_EARLIER_INIT TRUE
    #endif
    #define OPTION_GNBEARLIERINTERFACETN_ENTRY

    OPTION_GNB_CONFIGURATION  GnbEarlierFeatureTable[] = {
      OPTION_GNBEARLIERINTERFACETN_ENTRY
      {0, NULL}
    };
  #endif

  #if (AGESA_ENTRY_INIT_POST == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_CONFIG_POST_INIT
      #define OPTION_GFX_CONFIG_POST_INIT TRUE
    #endif
    #if (OPTION_GFX_CONFIG_POST_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  GfxConfigPostInterface;
      #define OPTION_GFXCONFIGPOSTINTERFACE_ENTRY         {AMD_FAMILY_LN | AMD_FAMILY_ON, GfxConfigPostInterface},
    #else
      #define  OPTION_GFXCONFIGPOSTINTERFACE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_POST_INIT
      #define OPTION_GFX_POST_INIT TRUE
    #endif
    #if (OPTION_GFX_POST_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  GfxInitAtPost;
      #define OPTION_GFXINITATPOST_ENTRY                  {AMD_FAMILY_LN | AMD_FAMILY_ON, GfxInitAtPost},
    #else
      #define  OPTION_GFXINITATPOST_ENTRY
    #endif
    #define  OPTION_GFXPOSTINTERFACETN_ENTRY
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_POST_INIT
      #define OPTION_NB_POST_INIT TRUE
    #endif
    #if (OPTION_NB_POST_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  NbInitAtPost;
      #define OPTION_NBINITATPOST_ENTRY                   {AMD_FAMILY_LN | AMD_FAMILY_ON, NbInitAtPost},
    #else
      #define OPTION_NBINITATPOST_ENTRY
    #endif
    #define OPTION_GNBPOSTINTERFACETN_ENTRY
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_POST_EALRY_INIT
      #define OPTION_PCIE_POST_EALRY_INIT TRUE
    #endif
    #if (OPTION_PCIE_POST_EALRY_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  PcieInitAtPostEarly;
      #define OPTION_PCIEINITATPOSTEARLY_ENTRY            {AMD_FAMILY_LN | AMD_FAMILY_ON, PcieInitAtPostEarly},
    #else
      #define OPTION_PCIEINITATPOSTEARLY_ENTRY
    #endif
    #define OPTION_PCIEPOSTEARLYINTERFACETN_ENTRY
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_POST_INIT
      #define OPTION_PCIE_POST_INIT TRUE
    #endif
    #if (OPTION_PCIE_POST_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  PcieInitAtPost;
      #define OPTION_PCIEINITATPOST_ENTRY                 {AMD_FAMILY_LN | AMD_FAMILY_ON, PcieInitAtPost},
    #else
      #define OPTION_PCIEINITATPOST_ENTRY
    #endif
    #define OPTION_PCIEPOSTINTERFACETN_ENTRY

  //---------------------------------------------------------------------------------------------------
    OPTION_GNB_CONFIGURATION  GnbPostFeatureTable[] = {
      OPTION_PCIEINITATPOSTEARLY_ENTRY
      OPTION_PCIEPOSTEARLYINTERFACETN_ENTRY
      OPTION_GFXCONFIGPOSTINTERFACE_ENTRY
      OPTION_GFXINITATPOST_ENTRY
      OPTION_GFXPOSTINTERFACETN_ENTRY
      {0, NULL}
    };

    OPTION_GNB_CONFIGURATION  GnbPostAfterDramFeatureTable[] = {
      OPTION_NBINITATPOST_ENTRY
      OPTION_GNBPOSTINTERFACETN_ENTRY
      OPTION_PCIEINITATPOST_ENTRY
      OPTION_PCIEPOSTINTERFACETN_ENTRY
      {0, NULL}
    };
  #endif

  #if (AGESA_ENTRY_INIT_ENV == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_FUSE_TABLE_INIT
      #define OPTION_FUSE_TABLE_INIT TRUE
    #endif
    #if (OPTION_FUSE_TABLE_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  NbFuseTableFeature;
      #define OPTION_NBFUSETABLEFEATURE_ENTRY             {AMD_FAMILY_LN | AMD_FAMILY_ON, NbFuseTableFeature},
    #else
      #define OPTION_NBFUSETABLEFEATURE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_ENV_INIT
      #define OPTION_NB_ENV_INIT TRUE
    #endif
    #if (OPTION_NB_ENV_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  NbInitAtEnv;
      #define OPTION_NBINITATENVT_ENTRY                   {AMD_FAMILY_LN | AMD_FAMILY_ON, NbInitAtEnv},
    #else
      #define OPTION_NBINITATENVT_ENTRY
    #endif
    #define OPTION_GNBENVINTERFACETN_ENTRY
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_CONFIG_ENV_INIT
      #define OPTION_GFX_CONFIG_ENV_INIT TRUE
    #endif
    #if (OPTION_GFX_CONFIG_ENV_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                   GfxConfigEnvInterface;
      #define OPTION_GFXCONFIGENVINTERFACE_ENTRY           {AMD_FAMILY_LN | AMD_FAMILY_ON, GfxConfigEnvInterface},
    #else
      #define  OPTION_GFXCONFIGENVINTERFACE_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_ENV_INIT
      #define OPTION_GFX_ENV_INIT TRUE
    #endif
    #if (OPTION_GFX_ENV_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  GfxInitAtEnvPost;
      #define OPTION_GFXINITATENVPOST_ENTRY               {AMD_FAMILY_LN | AMD_FAMILY_ON, GfxInitAtEnvPost},
    #else
      #define OPTION_GFXINITATENVPOST_ENTRY
    #endif
    #define OPTION_GFXENVINTERFACETN_ENTRY

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_POWER_GATE
      #define OPTION_POWER_GATE TRUE
    #endif
    #if (OPTION_POWER_GATE == TRUE) && (GNB_TYPE_LN == TRUE)
      OPTION_GNB_FEATURE                                  F12NbPowerGateFeature;
      #define OPTION_F12NBPOWERGATEFEATURE_ENTRY          {AMD_FAMILY_LN, F12NbPowerGateFeature},
    #else
      #define OPTION_F12NBPOWERGATEFEATURE_ENTRY
    #endif
    #if (OPTION_POWER_GATE == TRUE) && (GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  F14NbPowerGateFeature;
      #define OPTION_F14NBPOWERGATEFEATURE_ENTRY          {AMD_FAMILY_ON, F14NbPowerGateFeature},
    #else
      #define OPTION_F14NBPOWERGATEFEATURE_ENTRY
    #endif
    #define OPTION_KRNBPOWERGATEFEATURE_ENTRY
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_ENV_INIT
      #define OPTION_PCIE_ENV_INIT TRUE
    #endif
    #if (OPTION_PCIE_ENV_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  PcieInitAtEnv;
      #define OPTION_PCIEINITATENV_ENTRY                  {AMD_FAMILY_LN | AMD_FAMILY_ON, PcieInitAtEnv},
    #else
      #define OPTION_PCIEINITATENV_ENTRY
    #endif
    #define OPTION_PCIEENVINTERFACETN_ENTRY

  //---------------------------------------------------------------------------------------------------

    OPTION_GNB_CONFIGURATION  GnbEnvFeatureTable[] = {
      OPTION_NBFUSETABLEFEATURE_ENTRY
      OPTION_NBINITATENVT_ENTRY
      OPTION_GNBENVINTERFACETN_ENTRY
      OPTION_PCIEINITATENV_ENTRY
      OPTION_PCIEENVINTERFACETN_ENTRY
      OPTION_GFXCONFIGENVINTERFACE_ENTRY
      OPTION_GFXINITATENVPOST_ENTRY
      OPTION_GFXENVINTERFACETN_ENTRY
      OPTION_F12NBPOWERGATEFEATURE_ENTRY
      OPTION_F14NBPOWERGATEFEATURE_ENTRY
      OPTION_KRNBPOWERGATEFEATURE_ENTRY
      {0, NULL}
    };
  #endif

  #if (AGESA_ENTRY_INIT_MID == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTOIN_GNB_CABLESAFE
      #define OPTOIN_GNB_CABLESAFE TRUE
    #endif
    #if (OPTOIN_GNB_CABLESAFE == TRUE) && (GNB_TYPE_LN == TRUE)
      OPTION_GNB_FEATURE                                  GnbCableSafeEntry;
      #define OPTION_GNBCABLESAFEENTRY_ENTRY              {AMD_FAMILY_LN, GnbCableSafeEntry},
    #else
      #define OPTION_GNBCABLESAFEENTRY_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTOIN_NB_LCLK_NCLK_RATIO
      #define OPTOIN_NB_LCLK_NCLK_RATIO TRUE
    #endif
    #if (OPTOIN_NB_LCLK_NCLK_RATIO == TRUE) && (GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  F14NbLclkNclkRatioFeature;
      #define OPTION_F14NBLCLKNCLKRATIOFEATURE_ENTRY      {AMD_FAMILY_ON, F14NbLclkNclkRatioFeature},
    #else
      #define OPTION_F14NBLCLKNCLKRATIOFEATURE_ENTRY
    #endif
    #define OPTION_KRNBLCLKNCLKRATIOFEATURE_ENTRY
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_LCLK_DPM_INIT
      #define OPTION_NB_LCLK_DPM_INIT TRUE
    #endif
    #if (OPTION_NB_LCLK_DPM_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  NbLclkDpmFeature;
      #define OPTION_NBLCLKDPMFEATURE_ENTRY               {AMD_FAMILY_LN | AMD_FAMILY_ON, NbLclkDpmFeature},
    #else
      #define OPTION_NBLCLKDPMFEATURE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_POWER_GATE
      #define OPTION_PCIE_POWER_GATE TRUE
    #endif
    #if (OPTION_PCIE_POWER_GATE == TRUE) && (GNB_TYPE_LN == TRUE)
      OPTION_GNB_FEATURE                                  PciePowerGateFeature;
      #define OPTION_PCIEPOWERGATEFEATURE_ENTRY           {AMD_FAMILY_LN, PciePowerGateFeature},
    #else
      #define OPTION_PCIEPOWERGATEFEATURE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_MID_INIT
      #define OPTION_GFX_MID_INIT TRUE
    #endif
    #if (OPTION_GFX_MID_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  GfxInitAtMidPost;
      #define OPTION_GFXINITATMIDPOST_ENTRY               {AMD_FAMILY_LN | AMD_FAMILY_ON, GfxInitAtMidPost},
    #else
      #define OPTION_GFXINITATMIDPOST_ENTRY
    #endif
    #define OPTION_GFXMIDINTERFACETN_ENTRY
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_INTEGRATED_TABLE_INIT
      #define OPTION_GFX_INTEGRATED_TABLE_INIT TRUE
    #endif
    #if (OPTION_GFX_INTEGRATED_TABLE_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                 GfxIntegratedInfoTableEntry;
      #define OPTION_GFXINTEGRATEDINFOTABLE_ENTRY        {AMD_FAMILY_LN | AMD_FAMILY_ON, GfxIntegratedInfoTableEntry},
    #else
      #define OPTION_GFXINTEGRATEDINFOTABLE_ENTRY
    #endif
    #define OPTION_GFXINTINFOTABLEINTERFACETN_ENTRY
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIe_MID_INIT
      #define OPTION_PCIe_MID_INIT TRUE
    #endif
    #if (OPTION_PCIe_MID_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  PcieInitAtMid;
      #define OPTION_PCIEINITATMID_ENTRY                  {AMD_FAMILY_LN | AMD_FAMILY_ON, PcieInitAtMid},
    #else
      #define OPTION_PCIEINITATMID_ENTRY
    #endif
    #define OPTION_PCIEMIDINTERFACETN_ENTRY

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_MID_INIT
      #define OPTION_NB_MID_INIT TRUE
    #endif
    #if (OPTION_NB_MID_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  NbInitAtLatePost;
      #define OPTION_NBINITATLATEPOST_ENTRY               {AMD_FAMILY_LN | AMD_FAMILY_ON, NbInitAtLatePost},
    #else
      #define OPTION_NBINITATLATEPOST_ENTRY
    #endif
    #define OPTION_GNBMIDINTERFACETN_ENTRY

  //---------------------------------------------------------------------------------------------------
    OPTION_GNB_CONFIGURATION  GnbMidFeatureTable[] = {
      OPTION_GFXINITATMIDPOST_ENTRY
      OPTION_GFXMIDINTERFACETN_ENTRY
      OPTION_GFXINTEGRATEDINFOTABLE_ENTRY
      OPTION_GFXINTINFOTABLEINTERFACETN_ENTRY
      OPTION_GNBCABLESAFEENTRY_ENTRY
      OPTION_PCIEINITATMID_ENTRY
      OPTION_PCIEMIDINTERFACETN_ENTRY
      OPTION_NBINITATLATEPOST_ENTRY
      OPTION_GNBMIDINTERFACETN_ENTRY
      OPTION_F14NBLCLKNCLKRATIOFEATURE_ENTRY
      OPTION_KRNBLCLKNCLKRATIOFEATURE_ENTRY
      OPTION_NBLCLKDPMFEATURE_ENTRY
      OPTION_PCIEPOWERGATEFEATURE_ENTRY
      {0, NULL}
    };
  #endif

  #if (AGESA_ENTRY_INIT_LATE == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_ALIB
      #define OPTION_ALIB FALSE
    #endif
    #if (OPTION_ALIB == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  PcieAlibFeature;
      #define OPTION_PCIEALIBFEATURE_ENTRY                {AMD_FAMILY_LN | AMD_FAMILY_ON, PcieAlibFeature},
    #else
      #define OPTION_PCIEALIBFEATURE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_IOMMU_ACPI_IVRS
      #define OPTION_IOMMU_ACPI_IVRS TRUE
    #endif
    #define OPTIONIOMMUACPIIVRSLATE_ENTRY
  //---------------------------------------------------------------------------------------------------
    OPTION_GNB_CONFIGURATION  GnbLateFeatureTable[] = {
      OPTION_PCIEALIBFEATURE_ENTRY
      OPTIONIOMMUACPIIVRSLATE_ENTRY
      {0, NULL}
    };
  #endif

  #if  (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE)
    S3_DISPATCH_FUNCTION  NbSmuServiceRequestS3Script;
    S3_DISPATCH_FUNCTION  PcieLateRestoreS3Script;
    S3_DISPATCH_FUNCTION  NbSmuIndirectWriteS3Script;
    #define GNB_S3_DISPATCH_FUNCTION_TABLE \
    {NbSmuIndirectWriteS3Script_ID, NbSmuIndirectWriteS3Script}, \
    {NbSmuServiceRequestS3Script_ID, NbSmuServiceRequestS3Script}, \
    {PcieLateRestoreS3Script_ID, PcieLateRestoreS3Script},
  #endif


#endif
#endif  // _OPTION_GNB_INSTALL_H_
