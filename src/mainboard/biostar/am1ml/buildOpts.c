/* SPDX-License-Identifier: GPL-2.0-only */

#include <AGESA.h>

#define INSTALL_FT3_SOCKET_SUPPORT		TRUE
#define INSTALL_FAMILY_16_MODEL_0x_SUPPORT	TRUE

//#define BLDOPT_REMOVE_UDIMMS_SUPPORT		TRUE
//#define BLDOPT_REMOVE_RDIMMS_SUPPORT		TRUE
//#define BLDOPT_REMOVE_LRDIMMS_SUPPORT		TRUE
#define BLDOPT_REMOVE_ECC_SUPPORT		TRUE
//#define BLDOPT_REMOVE_SRAT			FALSE
#define BLDOPT_REMOVE_WHEA			FALSE
#define BLDOPT_REMOVE_CRAT			TRUE
#define BLDOPT_REMOVE_CDIT			TRUE

/* Build configuration values here. */
#define BLDCFG_VRM_NB_LOW_POWER_THRESHOLD	0

#define BLDCFG_AMD_PLATFORM_TYPE		AMD_PLATFORM_MOBILE

#define BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT	DDR1600_FREQUENCY
#define BLDCFG_MEMORY_RDIMM_CAPABLE		TRUE
#define BLDCFG_MEMORY_UDIMM_CAPABLE		TRUE
#define BLDCFG_MEMORY_SODIMM_CAPABLE		FALSE
#define BLDCFG_MEMORY_ENABLE_BANK_INTERLEAVING	FALSE
#define BLDCFG_MEMORY_CHANNEL_INTERLEAVING	FALSE
#define BLDCFG_MEMORY_CLOCK_SELECT		DDR1333_FREQUENCY /* FIXME: Turtle RAM? */
#define BLDCFG_IGNORE_SPD_CHECKSUM		TRUE
#define BLDCFG_ENABLE_ECC_FEATURE		FALSE
#define BLDCFG_ECC_SYNC_FLOOD			FALSE
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

#include <PlatformInstall.h>
