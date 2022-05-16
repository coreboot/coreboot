/* SPDX-License-Identifier: GPL-2.0-only */

#include <AGESA.h>

#define INSTALL_FT3_SOCKET_SUPPORT		TRUE
#define INSTALL_FAMILY_16_MODEL_0x_SUPPORT	TRUE

//#define BLDOPT_REMOVE_UDIMMS_SUPPORT		TRUE
//#define BLDOPT_REMOVE_RDIMMS_SUPPORT		TRUE
#define BLDOPT_REMOVE_LRDIMMS_SUPPORT		TRUE
//#define BLDOPT_REMOVE_ECC_SUPPORT		TRUE
#define BLDOPT_REMOVE_SRAT			FALSE
#define BLDOPT_REMOVE_WHEA			FALSE
#define BLDOPT_REMOVE_CRAT			TRUE
#define BLDOPT_REMOVE_CDIT			TRUE

/* Build configuration values here. */
#define BLDCFG_VRM_NB_LOW_POWER_THRESHOLD	0

#define BLDCFG_AMD_PLATFORM_TYPE		AMD_PLATFORM_MOBILE

#define BLDCFG_MEMORY_RDIMM_CAPABLE		FALSE
#define BLDCFG_MEMORY_UDIMM_CAPABLE		TRUE
#define BLDCFG_MEMORY_SODIMM_CAPABLE		TRUE
#define BLDCFG_MEMORY_CHANNEL_INTERLEAVING	TRUE
#define BLDCFG_ENABLE_ECC_FEATURE		TRUE
#define BLDCFG_ECC_SYNC_FLOOD			TRUE
#define BLDCFG_UMA_ALLOCATION_MODE		UMA_NONE
#define BLDCFG_IOMMU_SUPPORT			FALSE

#define BLDCFG_CFG_GNB_HD_AUDIO			TRUE

/* Include the files that instantiate the configuration definitions. */
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "AdvancedApi.h"
#include "heapManager.h"
#include "CreateStruct.h"
#include "cpuFeatures.h"
#include "Table.h"
#include "cpuEarlyInit.h"
#include "cpuLateInit.h"
#include "GnbInterface.h"

CONST GPIO_CONTROL hp_abm_gpio[] = {
	{ 45, Function2, GpioOutEnB | Sticky },                   // Signal input  APU_SD_LED
	{ 49, Function2, PullUpB | PullDown | Sticky },           // Signal output APU_ABM_LED_UID
	{ 50, Function2, PullUpB | PullDown | Sticky },           // Signal output APU_ABM_LED_HEALTH
	{ 51, Function2, GpioOut | PullUpB | PullDown | Sticky }, // Signal output APU_ABM_LED_FAULT
	{ 57, Function2, GpioOutEnB | Sticky },                   // Signal input  SATA_PRSNT_L
	{ 58, Function2, GpioOutEnB | Sticky },                   // Signal  i/o   APU_HDMI_CEC
	{ 64, Function2, GpioOutEnB | Sticky },                   // Signal input  SWC_APU_INT_L
	{ 68, Function0, GpioOutEnB | Sticky },                   // Signal input  CNTRL1_PRSNT
	{ 69, Function0, GpioOutEnB | Sticky },                   // Signal input  CNTRL2_PRSNT
	{ 71, Function0, GpioOut | PullUpB | PullDown | Sticky }, // Signal output APU_PROCHOT_L_R
	{-1}
};
#define BLDCFG_FCH_GPIO_CONTROL_LIST		(hp_abm_gpio)

#include <PlatformInstall.h>
