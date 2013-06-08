/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Platform Specific Memory Configuration
 *
 * Contains Definitions and Macros for control of AGESA Memory code on a per platform basis
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  OPTION
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*****************************************************************************
 *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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

#ifndef _PLATFORM_MEMORY_CONFIGURATION_H_
#define _PLATFORM_MEMORY_CONFIGURATION_H_

/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */
#ifndef PSO_ENTRY
  #define PSO_ENTRY UINT8
#endif

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *----------------------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------------------
 *                 PLATFORM SPECIFIC MEMORY DEFINITIONS
 *----------------------------------------------------------------------------------------
 */
///
///  Memory Speed and DIMM Population Masks
///
///<  DDR Speed Masks
///<    Specifies the DDR Speed on a memory channel
///
#define ANY_SPEED 0xFFFFFFFFul
#define DDR400  ((UINT32) 1 << (DDR400_FREQUENCY  / 66))
#define DDR533  ((UINT32) 1 << (DDR533_FREQUENCY  / 66))
#define DDR667  ((UINT32) 1 << (DDR667_FREQUENCY  / 66))
#define DDR800  ((UINT32) 1 << (DDR800_FREQUENCY  / 66))
#define DDR1066 ((UINT32) 1 << (DDR1066_FREQUENCY / 66))
#define DDR1333 ((UINT32) 1 << (DDR1333_FREQUENCY / 66))
#define DDR1600 ((UINT32) 1 << (DDR1600_FREQUENCY / 66))
#define DDR1866 ((UINT32) 1 << (DDR1866_FREQUENCY / 66))
#define DDR2133 ((UINT32) 1 << (DDR2133_FREQUENCY / 66))
#define DDR2400 ((UINT32) 1 << (DDR2400_FREQUENCY / 66))
///
///<  DIMM POPULATION MASKS
///<    Specifies the DIMM Population on a channel (can be added together to specify configuration).
///<    ex. SR_DIMM0 + SR_DIMM1                       : Single Rank Dimm in slot 0 AND Slot 1
///<        SR_DIMM0 + DR_DIMM0 + SR_DIMM1 +DR_DIMM1  : Single OR Dual rank in Slot 0 AND Single OR Dual rank in Slot 1
///
#define ANY_       0xFF       ///< Any dimm configuration the current channel
#define SR_DIMM0   0x0001     ///< Single rank dimm in slot 0 on the current channel
#define SR_DIMM1   0x0010     ///< Single rank dimm in slot 1 on the current channel
#define SR_DIMM2   0x0100     ///< Single rank dimm in slot 2 on the current channel
#define SR_DIMM3   0x1000     ///< Single rank dimm in slot 3 on the current channel
#define DR_DIMM0   0x0002     ///< Dual rank dimm in slot 0 on the current channel
#define DR_DIMM1   0x0020     ///< Dual rank dimm in slot 1 on the current channel
#define DR_DIMM2   0x0200     ///< Dual rank dimm in slot 2 on the current channel
#define DR_DIMM3   0x2000     ///< Dual rank dimm in slot 3 on the current channel
#define QR_DIMM0   0x0004     ///< Quad rank dimm in slot 0 on the current channel
#define QR_DIMM1   0x0040     ///< Quad rank dimm in slot 1 on the current channel
#define QR_DIMM2   0x0400     ///< Quad rank dimm in slot 2 on the current channel
#define QR_DIMM3   0x4000     ///< Quad rank dimm in slot 3 on the current channel
#define LR_DIMM0   0x0001     ///< Lrdimm in slot 0 on the current channel
#define LR_DIMM1   0x0010     ///< Lrdimm in slot 1 on the current channel
#define LR_DIMM2   0x0100     ///< Lrdimm in slot 2 on the current channel
#define LR_DIMM3   0x1000     ///< Lrdimm in slot 3 on the current channel
#define ANY_DIMM0   0x000F    ///< Any Dimm combination in slot 0 on the current channel
#define ANY_DIMM1   0x00F0    ///< Any Dimm combination in slot 1 on the current channel
#define ANY_DIMM2   0x0F00    ///< Any Dimm combination in slot 2 on the current channel
#define ANY_DIMM3   0xF000    ///< Any Dimm combination in slot 3 on the current channel
///
///<  CS POPULATION MASKS
///<    Specifies the CS Population on a channel (can be added together to specify configuration).
///<    ex. CS0 + CS1                       : CS0 and CS1 apply to the setting
///
#define CS_ANY_       0xFF       ///< Any CS configuration
#define CS0_   0x01     ///< CS0 bit map mask
#define CS1_   0x02     ///< CS1 bit map mask
#define CS2_   0x04     ///< CS2 bit map mask
#define CS3_   0x08     ///< CS3 bit map mask
#define CS4_   0x10     ///< CS4 bit map mask
#define CS5_   0x20     ///< CS5 bit map mask
#define CS6_   0x40     ///< CS6 bit map mask
#define CS7_   0x80     ///< CS7 bit map mask
///
///< Number of Dimms on the current channel
///<   This is a mask used to indicate the number of dimms in a channel
///<   They can be added to indicate multiple conditions (i.e 1 OR 2 Dimms)
///
#define ANY_NUM     0xFF      ///< Any number of Dimms
#define NO_DIMM     0x00      ///< No Dimms present
#define ONE_DIMM    0x01      ///< One dimm Poulated on the current channel
#define TWO_DIMM    0x02      ///< Two dimms Poulated on the current channel
#define THREE_DIMM  0x04      ///< Three dimms Poulated on the current channel
#define FOUR_DIMM   0x08      ///< Four dimms Poulated on the current channel

///
///<  DIMM VOLTAGE MASKS
///
#define VOLT_ANY_   0xFF      ///< Any voltage configuration
#define VOLT1_5_    0x01      ///< Voltage 1.5V bit map mask
#define VOLT1_35_   0x02      ///< Voltage 1.35V bit map mask
#define VOLT1_25_   0x04      ///< Voltage 1.25V bit map mask

//
// < Not applicable
//
#define NA_ 0                 ///< Not applicable

/*----------------------------------------------------------------------------------------
 *
 * Platform Specific Override Definitions for Socket, Channel and Dimm
 * This indicates where a platform override will be applied.
 *
 *----------------------------------------------------------------------------------------
 */
///
///< SOCKET MASKS
///<   Indicates associated processor sockets to apply override settings
///
#define ANY_SOCKET     0xFF    ///<  Apply to all sockets
#define SOCKET0        0x01    ///<  Apply to socket 0
#define SOCKET1        0x02    ///<  Apply to socket 1
#define SOCKET2        0x04    ///<  Apply to socket 2
#define SOCKET3        0x08    ///<  Apply to socket 3
#define SOCKET4        0x10    ///<  Apply to socket 4
#define SOCKET5        0x20    ///<  Apply to socket 5
#define SOCKET6        0x40    ///<  Apply to socket 6
#define SOCKET7        0x80    ///<  Apply to socket 7
///
///< CHANNEL MASKS
///<   Indicates Memory channels where override should be applied
///
#define ANY_CHANNEL    0xFF    ///<  Apply to all Memory channels
#define CHANNEL_A      0x01    ///<  Apply to Channel A
#define CHANNEL_B      0x02    ///<  Apply to Channel B
#define CHANNEL_C      0x04    ///<  Apply to Channel C
#define CHANNEL_D      0x08    ///<  Apply to Channel D
///
/// DIMM MASKS
///   Indicates Dimm Slots where override should be applied
///
#define ALL_DIMMS      0xFF    ///<  Apply to all dimm slots
#define DIMM0          0x01    ///<  Apply to Dimm Slot 0
#define DIMM1          0x02    ///<  Apply to Dimm Slot 1
#define DIMM2          0x04    ///<  Apply to Dimm Slot 2
#define DIMM3          0x08    ///<  Apply to Dimm Slot 3
///
/// REGISTER ACCESS MASKS
///   Not supported as an at this time
///
#define ACCESS_NB0     0x0
#define ACCESS_NB1     0x1
#define ACCESS_NB2     0x2
#define ACCESS_NB3     0x3
#define ACCESS_NB4     0x4
#define ACCESS_PHY     0x5
#define ACCESS_DCT_XT  0x6
/*----------------------------------------------------------------------------------------
 *
 * Platform Specific Overriding Table Definitions
 *
 *----------------------------------------------------------------------------------------
 */

#define PSO_END           0     ///< Table End
#define PSO_CKE_TRI       1     ///< CKE Tristate Map
#define PSO_ODT_TRI       2     ///< ODT Tristate Map
#define PSO_CS_TRI        3     ///< CS Tristate Map
#define PSO_MAX_DIMMS     4     ///< Max Dimms per channel
#define PSO_CLK_SPEED     5     ///< Clock Speed
#define PSO_DIMM_TYPE     6     ///< Dimm Type
#define PSO_MEMCLK_DIS    7     ///< MEMCLK Disable Map
#define PSO_MAX_CHNLS     8     ///< Max Channels per Socket
#define PSO_BUS_SPEED     9     ///< Max Memory Bus Speed
#define PSO_MAX_CHIPSELS 10     ///< Max Chipsel per Channel
#define PSO_MEM_TECH     11     ///< Channel Memory Type
#define PSO_WL_SEED      12     ///< DDR3 Write Levelization Seed delay
#define PSO_RXEN_SEED    13     ///< Hardwared based RxEn seed
#define PSO_NO_LRDIMM_CS67_ROUTING 14     ///< CS6 and CS7 are not Routed to all Memoy slots on a channel for LRDIMMs
#define PSO_SOLDERED_DOWN_SODIMM_TYPE 15  ///< Soldered down SODIMM type
#define PSO_LVDIMM_VOLT1_5_SUPPORT 16     ///< Force LvDimm voltage to 1.5V
#define PSO_MIN_RD_WR_DATAEYE_WIDTH 17    ///< Min RD/WR dataeye width
#define PSO_CPU_FAMILY_TO_OVERRIDE 18     ///< CPU family signature to tell following PSO macros are CPU family dependent
#define PSO_MAX_SOLDERED_DOWN_DIMMS 19    ///< Max Soldered-down Dimms per channel
#define PSO_MEMORY_POWER_POLICY 20        ///< Memory power policy override

/*----------------------------------
 * CONDITIONAL PSO SPECIFIC ENTRIES
 *---------------------------------*/
// Condition Types
#define CONDITIONAL_PSO_MIN   100     ///< Start of Conditional Entry Types
#define PSO_CONDITION_AND     100     ///< And Block - Start of Conditional block
#define PSO_CONDITION_LOC     101     ///< Location - Specify Socket, Channel, Dimms to be affected
#define PSO_CONDITION_SPD     102     ///< SPD - Specify a specific SPD value on a Dimm on the channel
#define PSO_CONDITION_REG     103     //   Reserved
#define PSO_CONDITION_MAX     103     ///< End Of Condition Entry Types
// Action Types
#define PSO_ACTION_MIN        120     ///< Start of Action Entry Types
#define PSO_ACTION_ODT        120     ///< ODT values to override
#define PSO_ACTION_ADDRTMG    121     ///< Address/Timing values to override
#define PSO_ACTION_ODCCONTROL 122     ///< ODC Control values to override
#define PSO_ACTION_SLEWRATE   123     ///< Slew Rate value to override
#define PSO_ACTION_REG        124     //   Reserved
#define PSO_ACTION_SPEEDLIMIT 125     ///< Memory Bus speed Limit based on configuration
#define PSO_ACTION_MAX        125     ///< End of Action Entry Types
#define CONDITIONAL_PSO_MAX   139     ///< End of Conditional Entry Types

/*----------------------------------
 * TABLE DRIVEN PSO SPECIFIC ENTRIES
 *---------------------------------*/
// Condition descriptor
#define PSO_TBLDRV_CONFIG        200  ///< Configuration Descriptor

// Overriding entry types
#define PSO_TBLDRV_START         210  ///< Start of Table Driven Overriding Entry Types
#define PSO_TBLDRV_SPEEDLIMIT    210  ///< Speed Limit
#define PSO_TBLDRV_ODT_RTTNOM    211  ///< RttNom
#define PSO_TBLDRV_ODT_RTTWR     212  ///< RttWr
#define PSO_TBLDRV_ODTPATTERN    213  ///< Odt Patterns
#define PSO_TBLDRV_ADDRTMG       214  ///< Address/Timing values
#define PSO_TBLDRV_ODCCTRL       215  ///< ODC Control values
#define PSO_TBLDRV_SLOWACCMODE   216  ///< Slow Access Mode
#define PSO_TBLDRV_MR0_CL        217  ///< MR0[CL]
#define PSO_TBLDRV_MR0_WR        218  ///< MR0[WR]
#define PSO_TBLDRV_RC2_IBT       219  ///< RC2[IBT]
#define PSO_TBLDRV_RC10_OPSPEED  220  ///< RC10[Opearting Speed]
#define PSO_TBLDRV_LRDIMM_IBT    221  ///< LrDIMM IBT
#define PSO_TBLDRV_INVALID_TYPE  223  ///< Invalid Type
#define PSO_TBLDRV_END           223  ///< End of Table Driven Overriding Entry Types

/*----------------------------------------------------------------------------------------
 *                 CONDITIONAL OVERRIDE TABLE MACROS
 *----------------------------------------------------------------------------------------
 */
#define CPU_FAMILY_TO_OVERRIDE(CpuFamilyRevision) \
  PSO_CPU_FAMILY_TO_OVERRIDE, 4, \
  ((CpuFamilyRevision) & 0x0FF), (((CpuFamilyRevision) >> 8)& 0x0FF), (((CpuFamilyRevision) >> 16)& 0x0FF), (((CpuFamilyRevision) >> 24)& 0x0FF)

#define MEMCLK_DIS_MAP(SocketID, ChannelID, Bit0Map, Bit1Map, Bit2Map, Bit3Map, Bit4Map, Bit5Map, Bit6Map, Bit7Map) \
  PSO_MEMCLK_DIS, 11, SocketID, ChannelID, ALL_DIMMS, Bit0Map, Bit1Map, Bit2Map, Bit3Map, Bit4Map, Bit5Map, Bit6Map \
  , Bit7Map

#define CKE_TRI_MAP(SocketID, ChannelID, Bit0Map, Bit1Map) \
  PSO_CKE_TRI, 5, SocketID, ChannelID, ALL_DIMMS, Bit0Map, Bit1Map

#define ODT_TRI_MAP(SocketID, ChannelID, Bit0Map, Bit1Map, Bit2Map, Bit3Map) \
  PSO_ODT_TRI, 7, SocketID, ChannelID, ALL_DIMMS, Bit0Map, Bit1Map, Bit2Map, Bit3Map

#define CS_TRI_MAP(SocketID, ChannelID, Bit0Map, Bit1Map, Bit2Map, Bit3Map, Bit4Map, Bit5Map, Bit6Map, Bit7Map) \
  PSO_CS_TRI, 11, SocketID, ChannelID, ALL_DIMMS, Bit0Map, Bit1Map, Bit2Map, Bit3Map, Bit4Map, Bit5Map, Bit6Map, Bit7Map

#define NUMBER_OF_DIMMS_SUPPORTED(SocketID, ChannelID, NumberOfDimmSlotsPerChannel) \
  PSO_MAX_DIMMS, 4, SocketID, ChannelID, ALL_DIMMS, NumberOfDimmSlotsPerChannel

#define NUMBER_OF_SOLDERED_DOWN_DIMMS_SUPPORTED(SocketID, ChannelID, NumberOfSolderedDownDimmsPerChannel) \
  PSO_MAX_SOLDERED_DOWN_DIMMS, 4, SocketID, ChannelID, ALL_DIMMS, NumberOfSolderedDownDimmsPerChannel

#define NUMBER_OF_CHIP_SELECTS_SUPPORTED(SocketID, ChannelID, NumberOfChipSelectsPerChannel) \
  PSO_MAX_CHIPSELS, 4, SocketID, ChannelID, ALL_DIMMS, NumberOfChipSelectsPerChannel

#define NUMBER_OF_CHANNELS_SUPPORTED(SocketID, NumberOfChannelsPerSocket) \
  PSO_MAX_CHNLS, 4, SocketID, ANY_CHANNEL, ALL_DIMMS, NumberOfChannelsPerSocket

#define OVERRIDE_DDR_BUS_SPEED(SocketID, ChannelID, TimingMode, BusSpeed) \
  PSO_BUS_SPEED, 11, SocketID, ChannelID, ALL_DIMMS, TimingMode, (TimingMode >> 8), (TimingMode >> 16), (TimingMode >> 24), \
  BusSpeed, (BusSpeed >> 8), (BusSpeed >> 16), (BusSpeed >> 24)

#define DRAM_TECHNOLOGY(SocketID, MemTechType) \
  PSO_MEM_TECH, 7, SocketID, ANY_CHANNEL, ALL_DIMMS, MemTechType, (MemTechType >> 8), (MemTechType >> 16), (MemTechType >> 24)

#define WRITE_LEVELING_SEED(SocketID, ChannelID, DimmID, Byte0Seed, Byte1Seed, Byte2Seed, Byte3Seed, Byte4Seed, Byte5Seed, \
  Byte6Seed, Byte7Seed, ByteEccSeed) \
  PSO_WL_SEED, 12, SocketID, ChannelID, DimmID, Byte0Seed, Byte1Seed, Byte2Seed, Byte3Seed, Byte4Seed, Byte5Seed, \
  Byte6Seed, Byte7Seed, ByteEccSeed

#define HW_RXEN_SEED(SocketID, ChannelID, DimmID, Byte0Seed, Byte1Seed, Byte2Seed, Byte3Seed, Byte4Seed, Byte5Seed, \
  Byte6Seed, Byte7Seed, ByteEccSeed) \
  PSO_RXEN_SEED, 21, SocketID, ChannelID, DimmID, Byte0Seed, (Byte0Seed >> 8), Byte1Seed, (Byte1Seed >> 8), Byte2Seed, (Byte2Seed >> 8), \
  Byte3Seed, (Byte3Seed >> 8), Byte4Seed, (Byte4Seed >> 8), Byte5Seed, (Byte5Seed >> 8), Byte6Seed, (Byte6Seed >> 8), \
  Byte7Seed, (Byte7Seed >> 8), ByteEccSeed, (ByteEccSeed >> 8)

#define NO_LRDIMM_CS67_ROUTING(SocketID, ChannelID) \
  PSO_NO_LRDIMM_CS67_ROUTING, 4, SocketID, ChannelID, ALL_DIMMS, TRUE

#define SOLDERED_DOWN_SODIMM_TYPE(SocketID, ChannelID) \
  PSO_SOLDERED_DOWN_SODIMM_TYPE, 4, SocketID, ChannelID, ALL_DIMMS, TRUE

#define LVDIMM_FORCE_VOLT1_5_FOR_D0 \
  PSO_LVDIMM_VOLT1_5_SUPPORT, 4, ANY_SOCKET, ANY_CHANNEL, ALL_DIMMS, TRUE

#define MIN_RD_WR_DATAEYE_WIDTH(SocketID, ChannelID, MinRdDataeyeWidth, MinWrDataeyeWidth) \
  PSO_MIN_RD_WR_DATAEYE_WIDTH, 5, SocketID, ChannelID, ALL_DIMMS, MinRdDataeyeWidth, MinWrDataeyeWidth

#define MEMORY_POWER_POLICY_OVERRIDE(PowerPolicy) \
  PSO_MEMORY_POWER_POLICY, 4, ANY_SOCKET, ANY_CHANNEL, ALL_DIMMS, PowerPolicy

/*----------------------------------------------------------------------------------------
 *                 CONDITIONAL OVERRIDE TABLE MACROS
 *----------------------------------------------------------------------------------------
 */
#define CONDITION_AND \
  PSO_CONDITION_AND, 0

#define COND_LOC(SocketMsk, ChannelMsk, DimmMsk) \
  PSO_CONDITION_LOC, 3, SocketMsk, ChannelMsk, DimmMsk

#define COND_SPD(Byte, Mask, Value) \
  PSO_CONDITION_SPD, 3, Byte, Mask, Value

#define COND_REG(Access, Offset, Mask, Value) \
 PSO_CONDITION_REG, 11, Access, (Offset & 0x0FF), (Offset >> 8), \
   ((Mask) & 0x0FF), (((Mask) >> 8) & 0x0FF), (((Mask) >> 16) & 0x0FF), (((Mask) >> 24) & 0x0FF), \
   ((Value) & 0x0FF), (((Value) >> 8) & 0x0FF), (((Value) >> 16) & 0x0FF), (((Value) >> 24) & 0x0FF)

#define ACTION_ODT(Frequency, Dimms, QrDimms, DramOdt, QrDramOdt, DramDynOdt) \
  PSO_ACTION_ODT, 9, \
  ((Frequency) & 0x0FF), (((Frequency) >> 8)& 0x0FF), (((Frequency) >> 16)& 0x0FF), ((Frequency >> 24)& 0x0FF), \
  Dimms, QrDimms, DramOdt, QrDramOdt, DramDynOdt

#define ACTION_ADDRTMG(Frequency, DimmConfig, AddrTmg) \
  PSO_ACTION_ADDRTMG, 10, \
  ((Frequency) & 0x0FF), (((Frequency) >> 8)& 0x0FF), (((Frequency) >> 16)& 0x0FF), (((Frequency) >> 24)& 0x0FF), \
  ((DimmConfig) & 0x0FF), (((DimmConfig) >> 8) & 0x0FF), \
  (AddrTmg & 0x0FF), ((AddrTmg >> 8)& 0x0FF), ((AddrTmg >> 16)& 0x0FF), ((AddrTmg >> 24)& 0x0FF)

#define ACTION_ODCCTRL(Frequency, DimmConfig, OdcCtrl) \
  PSO_ACTION_ODCCONTROL, 10, \
  ((Frequency) & 0x0FF), (((Frequency) >> 8)& 0x0FF), (((Frequency) >> 16)& 0x0FF), (((Frequency) >> 24)& 0x0FF), \
  ((DimmConfig) & 0x0FF), (((DimmConfig) >> 8) & 0x0FF), \
  (OdcCtrl & 0x0FF), ((OdcCtrl >> 8)& 0x0FF), ((OdcCtrl >> 16)& 0x0FF), ((OdcCtrl >> 24)& 0x0FF)

#define ACTION_SLEWRATE(Frequency, DimmConfig, SlewRate) \
  PSO_ACTION_SLEWRATE, 10, \
  ((Frequency) & 0x0FF), (((Frequency) >> 8)& 0x0FF), (((Frequency) >> 16)& 0x0FF), (((Frequency) >> 24)& 0x0FF), \
  ((DimmConfig) & 0x0FF), (((DimmConfig) >> 8) & 0x0FF), \
  (SlewRate & 0x0FF), ((SlewRate >> 8)& 0x0FF), ((SlewRate >> 16)& 0x0FF), ((SlewRate >> 24)& 0x0FF)

#define ACTION_SPEEDLIMIT(DimmConfig, Dimms, SpeedLimit15, SpeedLimit135, SpeedLimit125) \
  PSO_ACTION_SPEEDLIMIT, 9, \
  ((DimmConfig) & 0x0FF), (((DimmConfig) >> 8) & 0x0FF), Dimms, \
  (SpeedLimit15 & 0x0FF), ((SpeedLimit15 >> 8)& 0x0FF), \
  (SpeedLimit135 & 0x0FF), ((SpeedLimit135 >> 8)& 0x0FF), \
  (SpeedLimit125 & 0x0FF), ((SpeedLimit125 >> 8)& 0x0FF)

/*----------------------------------------------------------------------------------------
 *                 END OF CONDITIONAL OVERRIDE TABLE MACROS
 *----------------------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------------------
 *                 TABLE DRIVEN OVERRIDE  MACROS
 *----------------------------------------------------------------------------------------
 */
/// Configuration sub-descriptors
typedef enum {
  CONFIG_GENERAL,        ///< CONFIG_GENERAL
  CONFIG_SPEEDLIMIT,     ///< CONFIG_SPEEDLIMIT
  CONFIG_RC2IBT,         ///< CONFIG_RC2IBT
  CONFIG_DONT_CARE,      ///< CONFIG_DONT_CARE
} Config_Type;

// ====================
// Configuration Macros
// ====================
#define TBLDRV_CONFIG_TO_OVERRIDE(DimmPerCH, Frequency, DimmVolt, DimmConfig) \
  PSO_TBLDRV_CONFIG, 9, \
  CONFIG_GENERAL, \
  DimmPerCH, DimmVolt, \
  ((Frequency) & 0x0FF), (((Frequency) >> 8)& 0x0FF), (((Frequency) >> 16)& 0x0FF), (((Frequency) >> 24)& 0x0FF), \
  ((DimmConfig) & 0x0FF), (((DimmConfig) >> 8) & 0x0FF)

#define TBLDRV_SPEEDLIMIT_CONFIG_TO_OVERRIDE(DimmPerCH, Dimms, NumOfSR, NumOfDR, NumOfQR, NumOfLRDimm) \
  PSO_TBLDRV_CONFIG, 7, \
  CONFIG_SPEEDLIMIT, \
  DimmPerCH, Dimms, NumOfSR, NumOfDR, NumOfQR, NumOfLRDimm

#define TBLDRV_RC2IBT_CONFIG_TO_OVERRIDE(DimmPerCH, Frequency, DimmVolt, DimmConfig, NumOfReg) \
  PSO_TBLDRV_CONFIG, 10, \
  CONFIG_RC2IBT, \
  DimmPerCH, DimmVolt, \
  ((Frequency) & 0x0FF), (((Frequency) >> 8)& 0x0FF), (((Frequency) >> 16)& 0x0FF), (((Frequency) >> 24)& 0x0FF), \
  ((DimmConfig) & 0x0FF), (((DimmConfig) >> 8) & 0x0FF), \
  NumOfReg

//==================
// Overriding Macros
//==================
#define TBLDRV_CONFIG_ENTRY_SPEEDLIMIT(SpeedLimit1_5, SpeedLimit1_35, SpeedLimit1_25) \
  PSO_TBLDRV_SPEEDLIMIT, 6, \
  (SpeedLimit1_5 & 0x0FF), ((SpeedLimit1_5 >> 8)& 0x0FF), \
  (SpeedLimit1_35 & 0x0FF), ((SpeedLimit1_35 >> 8)& 0x0FF), \
  (SpeedLimit1_25 & 0x0FF), ((SpeedLimit1_25 >> 8)& 0x0FF)

#define TBLDRV_CONFIG_ENTRY_ODT_RTTNOM(TgtCS, RttNom) \
  PSO_TBLDRV_ODT_RTTNOM, 2, \
  TgtCS, RttNom

#define TBLDRV_CONFIG_ENTRY_ODT_RTTWR(TgtCS, RttWr) \
  PSO_TBLDRV_ODT_RTTWR, 2, \
  TgtCS, RttWr

#define TBLDRV_CONFIG_ENTRY_ODTPATTERN(RdODTCSHigh, RdODTCSLow, WrODTCSHigh, WrODTCSLow) \
  PSO_TBLDRV_ODTPATTERN, 16, \
  ((RdODTCSHigh) & 0x0FF), (((RdODTCSHigh) >> 8)& 0x0FF), (((RdODTCSHigh) >> 16)& 0x0FF), (((RdODTCSHigh) >> 24)& 0x0FF), \
  ((RdODTCSLow) & 0x0FF), (((RdODTCSLow) >> 8)& 0x0FF), (((RdODTCSLow) >> 16)& 0x0FF), (((RdODTCSLow) >> 24)& 0x0FF), \
  ((WrODTCSHigh) & 0x0FF), (((WrODTCSHigh) >> 8)& 0x0FF), (((WrODTCSHigh) >> 16)& 0x0FF), (((WrODTCSHigh) >> 24)& 0x0FF), \
  ((WrODTCSLow) & 0x0FF), (((WrODTCSLow) >> 8)& 0x0FF), (((WrODTCSLow) >> 16)& 0x0FF), (((WrODTCSLow) >> 24)& 0x0FF)

#define TBLDRV_CONFIG_ENTRY_ADDRTMG(AddrTmg) \
  PSO_TBLDRV_ADDRTMG, 4, \
  ((AddrTmg) & 0x0FF), (((AddrTmg) >> 8)& 0x0FF), (((AddrTmg) >> 16)& 0x0FF), (((AddrTmg) >> 24)& 0x0FF)

#define TBLDRV_CONFIG_ENTRY_ODCCTRL(OdcCtrl) \
  PSO_TBLDRV_ODCCTRL, 4, \
  ((OdcCtrl) & 0x0FF), (((OdcCtrl) >> 8)& 0x0FF), (((OdcCtrl) >> 16)& 0x0FF), (((OdcCtrl) >> 24)& 0x0FF)

#define TBLDRV_CONFIG_ENTRY_SLOWACCMODE(SlowAccMode) \
  PSO_TBLDRV_SLOWACCMODE, 1, \
  SlowAccMode

#define TBLDRV_CONFIG_ENTRY_RC2_IBT(TgtDimm, IBT) \
  PSO_TBLDRV_RC2_IBT, 2, \
  TgtDimm, IBT

#define TBLDRV_OVERRIDE_MR0_CL(RegValOfTcl, MR0CL13, MR0CL0) \
  PSO_TBLDRV_CONFIG, 1, \
  CONFIG_DONT_CARE, \
  PSO_TBLDRV_MR0_CL, 3, \
  RegValOfTcl, MR0CL13, MR0CL0

#define TBLDRV_OVERRIDE_MR0_WR(RegValOfTwr, MR0WR) \
  PSO_TBLDRV_CONFIG, 1, \
  CONFIG_DONT_CARE, \
  PSO_TBLDRV_MR0_WR, 2, \
  RegValOfTwr, MR0WR

#define TBLDRV_OVERRIDE_RC10_OPSPEED(Frequency, MR10OPSPEED) \
  PSO_TBLDRV_CONFIG, 1, \
  CONFIG_DONT_CARE, \
  PSO_TBLDRV_RC10_OPSPEED, 5, \
  ((Frequency) & 0x0FF), (((Frequency) >> 8)& 0x0FF), (((Frequency) >> 16)& 0x0FF), (((Frequency) >> 24)& 0x0FF), \
  MR10OPSPEED

#define TBLDRV_CONFIG_ENTRY_LRDMM_IBT(F0RC8, F1RC0, F1RC1, F1RC2) \
  PSO_TBLDRV_LRDIMM_IBT, 4, \
  F0RC8, F1RC0, F1RC1, F1RC2


//============================
// Macros for removing entries
//============================
#define INVALID_CONFIG_FLAG   0x8000

#define TBLDRV_INVALID_CONFIG \
  PSO_TBLDRV_INVALID_TYPE, 0

/*----------------------------------------------------------------------------------------
 *                 END OF TABLE DRIVEN OVERRIDE  MACROS
 *----------------------------------------------------------------------------------------
 */

#endif  // _PLATFORM_MEMORY_CONFIGURATION_H_
