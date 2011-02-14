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
 * @e \$Revision: 35415 $   @e \$Date: 2010-07-22 06:10:32 +0800 (Thu, 22 Jul 2010) $
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
#define ANY_SPEED 0xFFFFFFFF
#define DDR400  ((UINT32) 1 << (DDR400_FREQUENCY  / 66))
#define DDR533  ((UINT32) 1 << (DDR533_FREQUENCY  / 66))
#define DDR667  ((UINT32) 1 << (DDR667_FREQUENCY  / 66))
#define DDR800  ((UINT32) 1 << (DDR800_FREQUENCY  / 66))
#define DDR1066 ((UINT32) 1 << (DDR1066_FREQUENCY / 66))
#define DDR1333 ((UINT32) 1 << (DDR1333_FREQUENCY / 66))
#define DDR1600 ((UINT32) 1 << (DDR1600_FREQUENCY / 66))
#define DDR1866 ((UINT32) 1 << (DDR1866_FREQUENCY / 66))
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
#define ANY_DIMM0   0x000F    ///< Any Dimm combination in slot 0 on the current channel
#define ANY_DIMM1   0x00F0    ///< Any Dimm combination in slot 1 on the current channel
#define ANY_DIMM2   0x0F00    ///< Any Dimm combination in slot 2 on the current channel
#define ANY_DIMM3   0xF000    ///< Any Dimm combination in slot 3 on the current channel
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

/*----------------------------------------------------------------------------------------
 *                 CONDITIONAL OVERRIDE TABLE MACROS
 *----------------------------------------------------------------------------------------
 */
#define MEMCLK_DIS_MAP(SocketID, ChannelID, Bit0Map, Bit1Map, Bit2Map, Bit3Map, Bit4Map, Bit5Map, Bit6Map, Bit7Map) \
  PSO_MEMCLK_DIS, 10, SocketID, ChannelID, Bit0Map, Bit1Map, Bit2Map, Bit3Map, Bit4Map, Bit5Map, Bit6Map, Bit7Map

#define CKE_TRI_MAP(SocketID, ChannelID, Bit0Map, Bit1Map) \
  PSO_CKE_TRI, 4, SocketID, ChannelID, Bit0Map, Bit1Map

#define ODT_TRI_MAP(SocketID, ChannelID, Bit0Map, Bit1Map, Bit2Map, Bit3Map) \
  PSO_ODT_TRI, 6, SocketID, ChannelID, Bit0Map, Bit1Map, Bit2Map, Bit3Map

#define CS_TRI_MAP(SocketID, ChannelID, Bit0Map, Bit1Map, Bit2Map, Bit3Map, Bit4Map, Bit5Map, Bit6Map, Bit7Map) \
  PSO_CS_TRI, 10, SocketID, ChannelID, Bit0Map, Bit1Map, Bit2Map, Bit3Map, Bit4Map, Bit5Map, Bit6Map, Bit7Map

#define NUMBER_OF_DIMMS_SUPPORTED(SocketID, ChannelID, NumberOfDimmSlotsPerChannel) \
  PSO_MAX_DIMMS, 3, SocketID, ChannelID, NumberOfDimmSlotsPerChannel

#define NUMBER_OF_CHIP_SELECTS_SUPPORTED(SocketID, ChannelID, NumberOfChipSelectsPerChannel) \
  PSO_MAX_CHIPSELS, 3, SocketID, ChannelID, NumberOfChipSelectsPerChannel

#define NUMBER_OF_CHANNELS_SUPPORTED(SocketID, NumberOfChannelsPerSocket) \
  PSO_MAX_CHNLS, 3, SocketID, ANY_CHANNEL, NumberOfChannelsPerSocket

#define OVERRIDE_DDR_BUS_SPEED(SocketID, ChannelID, TimingMode, BusSpeed) \
  PSO_BUS_SPEED, 10, SocketID, ChannelID, TimingMode, (TimingMode >> 8), (TimingMode >> 16), (TimingMode >> 24), \
  BusSpeed, (BusSpeed >> 8), (BusSpeed >> 16), (BusSpeed >> 24)

#define DRAM_TECHNOLOGY(SocketID, MemTechType) \
  PSO_MEM_TECH, 6, SocketID, ANY_CHANNEL, MemTechType, (MemTechType >> 8), (MemTechType >> 16), (MemTechType >> 24)

#define WRITE_LEVELING_SEED(SocketID, ChannelID, Byte0Seed, Byte1Seed, Byte2Seed, Byte3Seed, Byte4Seed, Byte5Seed, \
  Byte6Seed, Byte7Seed, ByteEccSeed) \
  PSO_WL_SEED, 11, SocketID, ChannelID, Byte0Seed, Byte1Seed, Byte2Seed, Byte3Seed, Byte4Seed, Byte5Seed, \
  Byte6Seed, Byte7Seed, ByteEccSeed

#define HW_RXEN_SEED(SocketID, ChannelID, Byte0Seed, Byte1Seed, Byte2Seed, Byte3Seed, Byte4Seed, Byte5Seed, \
  Byte6Seed, Byte7Seed, ByteEccSeed) \
  PSO_RXEN_SEED, 20, SocketID, ChannelID, Byte0Seed, (Byte0Seed >> 8), Byte1Seed, (Byte1Seed >> 8), Byte2Seed, (Byte2Seed >> 8), \
  Byte3Seed, (Byte3Seed >> 8), Byte4Seed, (Byte4Seed >> 8), Byte5Seed, (Byte5Seed >> 8), Byte6Seed, (Byte6Seed >> 8), \
  Byte7Seed, (Byte7Seed >> 8), ByteEccSeed, (ByteEccSeed >> 8)

#define NO_LRDIMM_CS67_ROUTING(SocketID, ChannelID) \
  PSO_NO_LRDIMM_CS67_ROUTING, 3, SocketID, ChannelID, TRUE


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

#endif  // _PLATFORM_MEMORY_CONFIGURATION_H_
