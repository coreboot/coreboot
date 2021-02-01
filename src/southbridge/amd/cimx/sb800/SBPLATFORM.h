/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _AMD_SBPLATFORM_H_
#define _AMD_SBPLATFORM_H_

#include <stddef.h>

typedef unsigned long long PLACEHOLDER;

#ifndef SBOEM_ACPI_RESTORE_SWSMI
  #define SBOEM_BEFORE_PCI_RESTORE_SWSMI    0xD3
  #define SBOEM_AFTER_PCI_RESTORE_SWSMI     0xD4
#endif

#ifndef _AMD_NB_CIM_X_PROTOCOL_H_

/*
/// Extended PCI Address
typedef struct _EXT_PCI_ADDR {
  UINT32                  Reg :16; ///< / PCI Register
  UINT32                  Func:3;  ///< / PCI Function
  UINT32                  Dev :5;  ///< / PCI Device
  UINT32                  Bus :8;  ///< / PCI Address
} EXT_PCI_ADDR;

/// PCI Address
typedef union _PCI_ADDR {
  UINT32                  ADDR; ///< / 32 bit Address
  EXT_PCI_ADDR            Addr; ///< / Extended PCI Address
} PCI_ADDR;
*/
#endif
#define FIXUP_PTR(ptr)  ptr

#if CONFIG(SB800_IMC_FWM)
	#define IMC_ENABLE_OVER_WRITE        0x01
#endif

#include "AmdSbLib.h"
#include "Amd.h"
#include <SB800.h>
#include <SBTYPE.h>
#include <ACPILIB.h>
#include <SBDEF.h>
#include <AMDSBLIB.h>
#include <SBSUBFUN.h>
#include "platform_cfg.h"
#include <OEM.h>
#include <AMD.h>

//------------------------------------------------------------------------------------------------------------------------//
/**
 * SB_CIMx_PARAMETER                0                1                   2            Default Value When CIMx Take over
 *  SpreadSpectrum         CIMx take over   User (Setup Option) User (Setup Option)               Enable
 * SpreadSpectrumType      CIMx take over   User (Setup Option) User (Setup Option)               Normal
 *     HpetTimer           CIMx take over   User (Setup Option) User (Setup Option)               Enable
 *     HpetMsiDis          CIMx take over   User (Setup Option) User (Setup Option)               Enable (0x00)
 *      IrConfig           CIMx take over   User (Setup Option) User (Setup Option)               Disable (0x00)
 * SpiFastReadEnable       CIMx take over   User (Setup Option) User (Setup Option)               Disable
 * SpiFastReadSpeed        CIMx take over   User (Setup Option) User (Setup Option)               Disable (NULL)
 *     NbSbGen2            CIMx take over   User (Setup Option) User (Setup Option)               Enable
 * AlinkPhyPllPowerDown    CIMx take over   User (Setup Option) User (Setup Option)               Enable
 * ResetCpuOnSyncFlood     CIMx take over   User (Setup Option) User (Setup Option)               Enable
 *     GppGen2             CIMx take over   User (Setup Option) User (Setup Option)               Disable
 *  GppMemWrImprove        CIMx take over   User (Setup Option) User (Setup Option)               Enable
 *    GppPortAspm          CIMx take over   User (Setup Option) User (Setup Option)               Disable
 *  GppLaneReversal        CIMx take over   User (Setup Option) User (Setup Option)               Disable
 *  GppPhyPllPowerDown     CIMx take over   User (Setup Option) User (Setup Option)               Enable
 *  UsbPhyPowerDown        CIMx take over   User (Setup Option) User (Setup Option)               Disable
 *  SBGecDebugBus          CIMx take over   User (Setup Option) User (Setup Option)               Disable
 *     SBGecPwr            CIMx take over   User (Setup Option) User (Setup Option)               Never Power down (0x11)
 *   SataSetMaxGen2        CIMx take over   User (Setup Option) User (Setup Option)               Max Gen3 (0x00)
 *   SataClkMode           CIMx take over   User (Setup Option) User (Setup Option)               0x90   int. 100Mhz
 *  SataAggrLinkPmCap      CIMx take over   User (Setup Option) User (Setup Option)               Enable
 *  SataPortMultCap        CIMx take over   User (Setup Option) User (Setup Option)               Enable
 *   SataPscCap            CIMx take over   User (Setup Option) User (Setup Option)               Enable (0x00)
 *   SataSscCap            CIMx take over   User (Setup Option) User (Setup Option)               Enable (0x00)
 * SataFisBasedSwitching   CIMx take over   User (Setup Option) User (Setup Option)               Disable
 *   SataCccSupport        CIMx take over   User (Setup Option) User (Setup Option)               Disable
 *   SataMsiCapability     CIMx take over   User (Setup Option) User (Setup Option)               Enable
 *   SataClkAutoOff        CIMx take over   User (Setup Option) User (Setup Option)               Disable
 *    AcDcMsg              CIMx take over   User (Setup Option) User (Setup Option)               Disable
 *   TimerTickTrack        CIMx take over   User (Setup Option) User (Setup Option)               Disable
 *  ClockInterruptTag      CIMx take over   User (Setup Option) User (Setup Option)               Disable
 *  OhciTrafficHanding     CIMx take over   User (Setup Option) User (Setup Option)               Disable
 *  EhciTrafficHanding     CIMx take over   User (Setup Option) User (Setup Option)               Disable
 *  FusionMsgCMultiCore    CIMx take over   User (Setup Option) User (Setup Option)               Disable
 *   FusionMsgCStage       CIMx take over   User (Setup Option) User (Setup Option)               Disable
 */
#define SB_CIMx_PARAMETER  0x02

// Generic
#define cimSpreadSpectrumDefault        TRUE
#define cimSpreadSpectrumTypeDefault	0x00      // Normal
#define cimHpetTimerDefault             TRUE
#define cimHpetMsiDisDefault            FALSE     // Enable
#define cimIrConfigDefault              0x00      // Disable
#define cimSpiFastReadEnableDefault     0x01      // Enable
#define cimSpiFastReadSpeedDefault      0x01      // 33 MHz
#define cimSioHwmPortEnableDefault      FALSE
// GPP/AB Controller
#define cimNbSbGen2Default              TRUE
#define cimAlinkPhyPllPowerDownDefault  TRUE
#define cimResetCpuOnSyncFloodDefault   TRUE
#define cimGppGen2Default               FALSE
#define cimGppMemWrImproveDefault       TRUE
#define cimGppPortAspmDefault           FALSE
#define cimGppLaneReversalDefault       FALSE
#define cimGppPhyPllPowerDownDefault    TRUE
// USB Controller
#define cimUsbPhyPowerDownDefault       FALSE
// GEC Controller
#define cimSBGecDebugBusDefault         FALSE
#define cimSBGecPwrDefault              0x03
// Sata Controller
#define cimSataSetMaxGen2Default        0x00
#define cimSATARefClkSelDefault         0x10
#define cimSATARefDivSelDefault         0x80
#define cimSataAggrLinkPmCapDefault     TRUE
#define cimSataPortMultCapDefault       TRUE
#define cimSataPscCapDefault            0x00      // Enable
#define cimSataSscCapDefault            0x00      // Enable
#define cimSataFisBasedSwitchingDefault FALSE
#define cimSataCccSupportDefault        FALSE
#define cimSataClkAutoOffDefault        FALSE
#define cimNativepciesupportDefault     FALSE
// Fusion Related
#define cimAcDcMsgDefault               FALSE
#define cimTimerTickTrackDefault        FALSE
#define cimClockInterruptTagDefault     FALSE
#define cimOhciTrafficHandingDefault    FALSE
#define cimEhciTrafficHandingDefault    FALSE
#define cimFusionMsgCMultiCoreDefault   FALSE
#define cimFusionMsgCStageDefault       FALSE

#include "vendorcode/amd/cimx/sb800/AMDSBLIB.h"
#include <spi-generic.h>

#define BIOSRAM_INDEX	0xcd4
#define BIOSRAM_DATA	0xcd5

#endif // _AMD_SBPLATFORM_H_
