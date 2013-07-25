/* $NoKeywords:$ */
/**
 * @file
 *
 * Initialize GFX configuration data structure.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 86709 $   @e \$Date: 2013-01-24 17:39:09 -0600 (Thu, 24 Jan 2013) $
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
* ***************************************************************************
*
*/

#ifndef _GNBGFX_H_
#define _GNBGFX_H_

#define DEVICE_DFP                                0x1
#define DEVICE_CRT                                0x2
#define DEVICE_LCD                                0x3


#define CONNECTOR_DISPLAYPORT_ENUM                0x3013
#define CONNECTOR_HDMI_TYPE_A_ENUM                0x300c
#define CONNECTOR_SINGLE_LINK_DVI_D_ENUM          0x3003
#define CONNECTOR_DUAL_LINK_DVI_D_ENUM            0x3004
#define CONNECTOR_SINGLE_LINK_DVI_I_ENUM          0x3001
#define CONNECTOR_DUAL_LINK_DVI_I_ENUM            0x3002
#define CONNECTOR_VGA_ENUM                        0x3005
#define CONNECTOR_LVDS_ENUM                       0x300E
#define CONNECTOR_eDP_ENUM                        0x3014
#define CONNECTOR_LVDS_eDP_ENUM                   0x3016
//DP to VGA:
#define ENCODER_DP2VGA_ENUM_ID1                   0x2123
//DP to LVDS:
#define ENCODER_DP2LVDS_ENUM_ID2                  0x2223
#define ENCODER_ALMOND_ENUM_ID1                   0x2122
#define ENCODER_NOT_PRESENT                       0x0000

// no eDP->LVDS translator chip
#define eDP_TO_LVDS_RX_DISABLE                    0x00
// common eDP->LVDS translator chip without AMD SW init
#define eDP_TO_LVDS_COMMON_ID                     0x01
// Third party translator which requires AMD SW init
#define eDP_TO_LVDS_SWINIT_ID                     0x02


#define ATOM_DEVICE_CRT1_SUPPORT                  0x0001
#define ATOM_DEVICE_DFP1_SUPPORT                  0x0008
#define ATOM_DEVICE_DFP6_SUPPORT                  0x0040
#define ATOM_DEVICE_DFP2_SUPPORT                  0x0080
#define ATOM_DEVICE_DFP3_SUPPORT                  0x0200
#define ATOM_DEVICE_DFP4_SUPPORT                  0x0400
#define ATOM_DEVICE_DFP5_SUPPORT                  0x0800
#define ATOM_DEVICE_LCD1_SUPPORT                  0x0002

/// Graphics card information structure
typedef struct {
  UINT32   AmdPcieGfxCardBitmap;                  ///< AMD PCIE graphics card information
  UINT32   PcieGfxCardBitmap;                     ///< All PCIE graphics card information
  UINT32   PciGfxCardBitmap;                      ///< All PCI graphics card information
} GFX_CARD_CARD_INFO;

typedef enum {
  iGpuVgaAdapter,                                       ///< Configure iGPU as VGA adapter
  iGpuVgaNonAdapter                                     ///< Configure iGPU as non VGA adapter
} GFX_IGPU_VGA_MODE;

/// UMA Steering Mode
typedef enum {
  UMA_STEERING_ENUM0,
  SystemTrafficOnion,                                   ///< System traffic to onion
  Onion,                                                ///< Onion
  UMA_STEERING_ENUM3,
} UMA_STEERING;

/// User Options
typedef enum {
  OptionDisabled,                                       ///< Disabled
  OptionEnabled                                         ///< Enabled
} CONTROL_OPTION;

/// GFX enable Policy
typedef enum {
  GmcPowerGatingDisabled,                               ///< Disable Power gating
  GmcPowerGatingStutterOnly,                            ///< GMC Stutter Only mode
  GmcPowerGatingWithStutter                            ///< GMC Power gating with Stutter mode
} GMC_POWER_GATING;

/// Internal GFX mode
typedef enum {
  GfxControllerLegacyBridgeMode,                        ///< APC bridge Legacy mode
  GfxControllerPcieEndpointMode,                        ///< IGFX PCIE Bus 0, Device 1
} GFX_CONTROLLER_MODE;

/// Graphics Platform Configuration
typedef struct {
  PVOID                       StdHeader;                ///< Standard Header
  PCI_ADDR                    GfxPciAddress;            ///< Graphics PCI Address
  UMA_INFO                    UmaInfo;                  ///< UMA Information
  UINT64                      GmmBase;                  ///< GMM Base
  UINT8                       GnbHdAudio;               ///< Control GFX HD Audio controller(Used for HDMI and DP display output),
                                                        ///< essentially it enables function 1 of graphics device.
                                                        ///< @li 0 = HD Audio disable
                                                        ///< @li 1 = HD Audio enable
  UINT8                       AbmSupport;               ///< Automatic adjust LVDS/eDP Back light level support.It is
                                                        ///< characteristic specific to display panel which used by platform design.
                                                        ///< @li 0 = ABM support disabled
                                                        ///< @li 1 = ABM support enabled
  UINT8                       DynamicRefreshRate;       ///< Adjust refresh rate on LVDS/eDP.
  UINT16                      LcdBackLightControl;      ///< The PWM frequency to LCD backlight control.
                                                        ///< If equal to 0 backlight not controlled by iGPU.
  UINT32                      AmdPlatformType;          ///< Platform type
  UMA_STEERING                UmaSteering;              ///< UMA Steering
  GFX_IGPU_VGA_MODE           iGpuVgaMode;              ///< iGPU VGA mode
  BOOLEAN                     GmcClockGating;           ///< Clock gating
  BOOLEAN                     GmcLockRegisters;         ///< GmcLock Registers
  BOOLEAN                     GfxFusedOff;              ///< Record if GFX is fused off.
  GMC_POWER_GATING            GmcPowerGating;           ///< Gmc Power Gating.
  UINT8                       Gnb3dStereoPinIndex;      ///< 3D Stereo Pin ID
  GFX_CONTROLLER_MODE         GfxControllerMode;        ///< Gfx controller mode
  UINT16                      LvdsSpreadSpectrum;       ///< Spread spectrum value in 0.01 %
  UINT16                      LvdsSpreadSpectrumRate;   ///< Spread spectrum frequency used by SS hardware logic in unit of 10Hz, 0 - default frequency 40kHz
  UINT8                       LvdsPowerOnSeqDigonToDe;    ///< Panel initialization timing.
  UINT8                       LvdsPowerOnSeqDeToVaryBl;   ///< Panel initialization timing.
  UINT8                       LvdsPowerOnSeqDeToDigon;    ///< Panel initialization timing.
  UINT8                       LvdsPowerOnSeqVaryBlToDe;   ///< Panel initialization timing.
  UINT8                       LvdsPowerOnSeqOnToOffDelay; ///< Panel initialization timing.
  UINT8                       LvdsPowerOnSeqVaryBlToBlon; ///< Panel initialization timing.
  UINT8                       LvdsPowerOnSeqBlonToVaryBl; ///< Panel initialization timing.
  UINT16                      LvdsMaxPixelClockFreq;      ///< The maximum pixel clock frequency supported.
  UINT32                      LcdBitDepthControlValue;    ///< The LCD bit depth control settings.
  UINT8                       Lvds24bbpPanelMode;         ///< The LVDS 24 BBP mode.
  LVDS_MISC_CONTROL           LvdsMiscControl;            ///< THe LVDS swap/Hsync/Vsync/BLON/Volt-overwrite control
  GFX_CARD_CARD_INFO          GfxDiscreteCardInfo;        ///< Discrete GFX card info
  UINT16                      PcieRefClkSpreadSpectrum;   ///< Spread spectrum value in 0.01 %
  BOOLEAN                     GnbRemoteDisplaySupport;    ///< Wireless Display Enable
  UINT8                       LVDSVoltAdjust;             ///< when ucLVDSMisc[5]=1, then this value will be programmed to register LVDS_CTRL_4 to adjust LVDS output voltage
  DISPLAY_MISC_CONTROL        DisplayMiscControl;         ///< The Display misc control
  DP_FIXED_VOLT_SWING_TYPE    DpFixedVoltSwingType;       ///< To indicate fixed voltage swing value
  UINT8                       MinAllowedBLLevel;          ///< Minimum allowed LCD backlight level
} GFX_PLATFORM_CONFIG;


typedef UINT32 ULONG;
typedef UINT16 USHORT;
typedef UINT8  UCHAR;

/// Driver interface header structure
typedef struct _ATOM_COMMON_TABLE_HEADER {
  USHORT                      usStructureSize;             ///< Structure size
  UCHAR                       ucTableFormatRevision;       ///< Format revision number
  UCHAR                       ucTableContentRevision;      ///< Contents revision number
} ATOM_COMMON_TABLE_HEADER;

/// Link ping mapping for DP/eDP/LVDS
typedef struct _ATOM_DP_CONN_CHANNEL_MAPPING {
  UCHAR ucDP_Lane0_Source  :2;                      ///< Define which pin connect to DP connector DP_Lane0, =0: source from GPU pin TX0, =1: from GPU pin TX1, =2: from GPU pin TX2, =3 from GPU pin TX3
  UCHAR ucDP_Lane1_Source  :2;                      ///< Define which pin connect to DP connector DP_Lane1, =0: source from GPU pin TX0, =1: from GPU pin TX1, =2: from GPU pin TX2, =3 from GPU pin TX3
  UCHAR ucDP_Lane2_Source  :2;                      ///< Define which pin connect to DP connector DP_Lane2, =0: source from GPU pin TX0, =1: from GPU pin TX1, =2: from GPU pin TX2, =3 from GPU pin TX3
  UCHAR ucDP_Lane3_Source  :2;                      ///< Define which pin connect to DP connector DP_Lane3, =0: source from GPU pin TX0, =1: from GPU pin TX1, =2: from GPU pin TX2, =3 from GPU pin TX3
} ATOM_DP_CONN_CHANNEL_MAPPING;

/// Link ping mapping for DVI/HDMI
typedef struct _ATOM_DVI_CONN_CHANNEL_MAPPING {
  UCHAR ucDVI_DATA2_Source  :2;                     ///< Define which pin connect to DVI connector data Lane2, =0: source from GPU pin TX0, =1: from GPU pin TX1, =2: from GPU pin TX2, =3 from GPU pin TX3
  UCHAR ucDVI_DATA1_Source  :2;                     ///< Define which pin connect to DVI connector data Lane1, =0: source from GPU pin TX0, =1: from GPU pin TX1, =2: from GPU pin TX2, =3 from GPU pin TX3
  UCHAR ucDVI_DATA0_Source  :2;                     ///< Define which pin connect to DVI connector data Lane0, =0: source from GPU pin TX0, =1: from GPU pin TX1, =2: from GPU pin TX2, =3 from GPU pin TX3
  UCHAR ucDVI_CLK_Source    :2;                     ///< Define which pin connect to DVI connector clock lane, =0: source from GPU pin TX0, =1: from GPU pin TX1, =2: from GPU pin TX2, =3 from GPU pin TX3
} ATOM_DVI_CONN_CHANNEL_MAPPING;


/// External Display Path
typedef struct _EXT_DISPLAY_PATH {
  USHORT                      usDeviceTag;          ///< A bit vector to show what devices are supported
  USHORT                      usDeviceACPIEnum;     ///< 16bit device ACPI id.
  USHORT                      usDeviceConnector;    ///< A physical connector for displays to plug in, using object connector definitions
  UCHAR                       ucExtAUXDDCLutIndex;  ///< An index into external AUX/DDC channel LUT
  UCHAR                       ucExtHPDPINLutIndex;  ///< An index into external HPD pin LUT
  USHORT                      usExtEncoderObjId;    ///< external encoder object id
  union {                                           ///< Lane mapping
    UCHAR                     ucChannelMapping;     ///< lane mapping on connector (ucChannelMapping=0 use default)
    ATOM_DP_CONN_CHANNEL_MAPPING  asDPMapping;      ///< lane mapping on connector (ucChannelMapping=0 use default)
    ATOM_DVI_CONN_CHANNEL_MAPPING asDVIMapping;     ///< lane mapping on connector (ucChannelMapping=0 use default)
  } ChannelMapping;
  UCHAR                       ucChPNInvert;         ///< Bit vector for up to 8 lanes. 0: P and N is not invert, 1: P and N is inverted
  USHORT                      usCaps;               ///< Capabilities IF BIT[0] == 1, downgrade phy link to DP1.1
  USHORT                      usReserved;           ///< Reserved
} EXT_DISPLAY_PATH;

/// External Display Connection Information
typedef  struct _ATOM_EXTERNAL_DISPLAY_CONNECTION_INFO {
  ATOM_COMMON_TABLE_HEADER    sHeader;              ///< Standard Header
  UCHAR                       ucGuid [16];          ///< Guid
  EXT_DISPLAY_PATH            sPath[7];             ///< External Display Path
  UCHAR                       ucChecksum;           ///< Checksum
  UCHAR                       uc3DStereoPinId;      ///< 3D Stereo Pin ID
  UCHAR                       ucRemoteDisplayConfig;   ///< Bit0=1:Enable Wireless Display through APU VCE HW function
  UCHAR                       uceDPToLVDSRxId;      ///< 3rd party eDP to LVDS translator chip presented. 0:no, 1:chip without AMD SW init, 2:Third party translator which require AMD SW init
  UCHAR                       ucFixDPVoltageSwing;  ///< The value match DPCD register DPx_LANE_SET defined in DP spec
  UCHAR                       Reserved [3];         ///< Reserved
} ATOM_EXTERNAL_DISPLAY_CONNECTION_INFO;

/// DispClk to VID relation table
typedef struct _ATOM_CLK_VOLT_CAPABILITY {
  ULONG                       ulVoltageIndex;       ///< The Voltage Index indicated by FUSE, same voltage index shared with SCLK DPM fuse table
  ULONG                       ulMaximumSupportedCLK;///< Maximum clock supported with specified voltage index, unit in 10kHz
} ATOM_CLK_VOLT_CAPABILITY;

typedef struct _GnbGfx275_STRUCT {
  ULONG                        GnbGfx275_STRUCT_fld0;
  USHORT                       GnbGfx275_STRUCT_fld1;
  USHORT                       GnbGfx275_STRUCT_fld2;
} GnbGfx275_STRUCT;

/// TDP Configuration Bitfields
typedef struct _ATOM_TDP_CONFIG_BITS {
  UINT32                       uCTDP_Enable:2;      ///< = (uCTDP_Value > uTDP_Value? 2: (uCTDP_Value < uTDP_Value))
  UINT32                       uCTDP_Value:14;      ///< Override value in tens of milli watts
  UINT32                       uTDP_Value:14;       ///< Original TDP value in tens of milli watts
  UINT32                       uReserved:2;         ///< Reserved
} ATOM_TDP_CONFIG_BITS;

/// TDP Configuration Union
typedef union _ATOM_TDP_CONFIG {
  ATOM_TDP_CONFIG_BITS         TDP_config;          ///< Field-wise access
  ULONG                        TDP_config_all;      ///< Access to all
} ATOM_TDP_CONFIG;


/// IntegrateSystemInfoTable is used for Kaveri & Kabini APU
typedef struct _ATOM_INTEGRATED_SYSTEM_INFO_V1_8 {
  ATOM_COMMON_TABLE_HEADER    sHeader;                            ///<
  ULONG                       ulBootUpEngineClock;                ///<
  ULONG                       field2;                   ///<
  ULONG                       ulBootUpUMAClock;                   ///<
  ATOM_CLK_VOLT_CAPABILITY    ATOM_INTEGRATED_SYSTEM_INFO_V1_8_fld4[4];                ///<
  ULONG                       ulBootUpReqDisplayVector;           ///<
  ULONG                       ulVBIOSMisc;                        ///<
  ULONG                       ulGPUCapInfo;                       ///<
  ULONG                       ulReserved1;                        ///<
  USHORT                      usRequestedPWMFreqInHz;             ///<
  UCHAR                       ucHtcTmpLmt;                        ///<
  UCHAR                       ATOM_INTEGRATED_SYSTEM_INFO_V1_8_fld11;                       ///<
  ULONG                       ulReserved2;                        ///<
  ULONG                       ulSystemConfig;                     ///<
  ULONG                       ulCPUCapInfo;                       ///<
  ULONG                       ulReserved3;                        ///<
  USHORT                      usReserved1;                        ///<
  USHORT                      usExtDispConnInfoOffset;            ///<
  USHORT                      usPanelRefreshRateRange;            ///<
  UCHAR                       ucMemoryType;                       ///<
  UCHAR                       ucUMAChannelNumber;                 ///<
  UCHAR                       strVBIOSMsg[40];                    ///<
  ULONG                       ulReserved[20];                     ///<
  GnbGfx275_STRUCT            ATOM_INTEGRATED_SYSTEM_INFO_V1_8[5];                     ///<
  ULONG                       ulGMCRestoreResetTime;              ///<
  ULONG                       ulReserved4;                        ///<
  ULONG                       ulIdleNClk;                         ///<
  ULONG                       ulDDR_DLL_PowerUpTime;              ///<
  ULONG                       ulDDR_PLL_PowerUpTime;              ///<
  USHORT                      usPCIEClkSSPercentage;              ///<
  USHORT                      usPCIEClkSSType;                    ///<
  USHORT                      usLvdsSSPercentage;                 ///<
  USHORT                      usLvdsSSpreadRateIn10Hz;            ///<
  USHORT                      usHDMISSPercentage;                 ///<
  USHORT                      usHDMISSpreadRateIn10Hz;            ///<
  USHORT                      usDVISSPercentage;                  ///<
  USHORT                      usDVISSpreadRateIn10Hz;             ///<
  ULONG                       ulReserved5[5];                     ///<
  USHORT                      usMaxLVDSPclkFreqInSingleLink;      ///<
  UCHAR                       ucLvdsMisc;                         ///<
  UCHAR                       ucLVDSVoltAdjust;                   ///<
  UCHAR                       ucLVDSPwrOnSeqDIGONtoDE_in4Ms;      ///<
  UCHAR                       ucLVDSPwrOnSeqDEtoVARY_BL_in4Ms;    ///<
  UCHAR                       ucLVDSPwrOffSeqVARY_BLtoDE_in4Ms;   ///<
  UCHAR                       ucLVDSPwrOffSeqDEtoDIGON_in4Ms;     ///<
  UCHAR                       ucLVDSOffToOnDelay_in4Ms;           ///<
  UCHAR                       ucLVDSPwrOnSeqVARY_BLtoBLON_in4Ms;  ///<
  UCHAR                       ucLVDSPwrOffSeqBLONtoVARY_BL_in4Ms; ///<
  UCHAR                       ucMinAllowedBL_Level;               ///<
  ULONG                       ulLCDBitDepthControlVal;            ///<
  ULONG                       ulNbpStateMemclkFreq[4];            ///<
  ULONG                       ulReserved6;                        ///<
  ULONG                       ulNbpStateNClkFreq[4];              ///<
  USHORT                      usNBPStateVoltage[4];               ///<
  USHORT                      usBootUpNBVoltage;                  ///<
  USHORT                      usReserved2;                        ///<
  ATOM_EXTERNAL_DISPLAY_CONNECTION_INFO sExtDispConnInfo;         ///<
} ATOM_INTEGRATED_SYSTEM_INFO_V1_8;

/// this Table is used for Kaveri/Kabini APU
typedef struct _ATOM_FUSION_SYSTEM_INFO_V3 {
  ATOM_INTEGRATED_SYSTEM_INFO_V1_8  sIntegratedSysInfo;      ///< Refer to ATOM_INTEGRATED_SYSTEM_INFO_V1_8 definition.
  ULONG                             ulPowerplayTable[128];   ///< This 512 bytes memory is used to save ATOM_PPLIB_POWERPLAYTABLE3, starting form ulPowerplayTable[0]
} ATOM_FUSION_SYSTEM_INFO_V3;

#define GNB_SBDFO MAKE_SBDFO(0, 0, 0, 0, 0)

/// Define configuration values for ulGPUCapInfo
// BIT[0] - TMDS/HDMI Coherent Mode 0: use cascade PLL mode, 1: use single PLL mode.
#define GPUCAPINFO_TMDS_HDMI_USE_CASCADE_PLL_MODE      0x00ul
#define GPUCAPINFO_TMDS_HDMI_USE_SINGLE_PLL_MODE       0x01ul

// BIT[1] - DP mode 0: use cascade PLL mode, 1: use single PLL mode
#define GPUCAPINFO_DP_MODE_USE_CASCADE_PLL_MODE        0x00ul
#define GPUCAPINFO_DP_USE_SINGLE_PLL_MODE              0x02ul

// BIT[3] - AUX HW mode detection logic 0: Enable, 1: Disable
#define GPUCAPINFO_AUX_HW_MODE_DETECTION_ENABLE        0x00ul
#define GPUCAPINFO_AUX_HW_MODE_DETECTION_DISABLE       0x08ul

// BIT[4] - DFS bypass 0: Disable, 1: Enable
#define GPUCAPINFO_DFS_BYPASS_DISABLE       0x00ul
#define GPUCAPINFO_DFS_BYPASS_ENABLE        0x10ul

#endif
