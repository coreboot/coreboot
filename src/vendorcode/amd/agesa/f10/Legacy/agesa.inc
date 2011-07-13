; ****************************************************************************
; *
; * @file
; *
; * Agesa structures and definitions
; *
; * Contains AMD AGESA core interface
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  Include
; * @e \$Revision: 47176 $   @e \$Date: 2011-02-15 19:56:01 -0700 (Tue, 15 Feb 2011) $
;
; ****************************************************************************
; *
; * Copyright (c) 2011, Advanced Micro Devices, Inc.
; * All rights reserved.
; * 
; * Redistribution and use in source and binary forms, with or without
; * modification, are permitted provided that the following conditions are met:
; *     * Redistributions of source code must retain the above copyright
; *       notice, this list of conditions and the following disclaimer.
; *     * Redistributions in binary form must reproduce the above copyright
; *       notice, this list of conditions and the following disclaimer in the
; *       documentation and/or other materials provided with the distribution.
; *     * Neither the name of Advanced Micro Devices, Inc. nor the names of 
; *       its contributors may be used to endorse or promote products derived 
; *       from this software without specific prior written permission.
; * 
; * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
; * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; * 
; *
; **************************************************************************

INCLUDE amd.inc
UINT64    TEXTEQU     <QWORD>
UINT32    TEXTEQU     <DWORD>
UINT16    TEXTEQU     <WORD>
UINT8     TEXTEQU     <BYTE>
CHAR8     TEXTEQU     <BYTE>
BOOLEAN   TEXTEQU     <BYTE>
POINTER   TEXTEQU     <DWORD>

 ; AGESA Types and Definitions



 ; AGESA BASIC CALLOUTS
    AGESA_MEM_RELEASE           EQU    00028000h

 ; AGESA ADVANCED CALLOUTS, Processor
    AGESA_CHECK_UMA             EQU    00028100h
    AGESA_DO_RESET              EQU    00028101h
    AGESA_ALLOCATE_BUFFER       EQU    00028102h
    AGESA_DEALLOCATE_BUFFER     EQU    00028103h
    AGESA_LOCATE_BUFFER         EQU    00028104h
    AGESA_RUNFUNC_ONAP          EQU    00028105h

 ; AGESA ADVANCED CALLOUTS, HyperTransport

 ; AGESA ADVANCED CALLOUTS, Memory
    AGESA_READ_SPD              EQU    00028140h
    AGESA_HOOKBEFORE_DRAM_INIT  EQU    00028141h
    AGESA_HOOKBEFORE_DQS_TRAINING       EQU    00028142h
    AGESA_READ_SPD_RECOVERY     EQU    00028143h
    AGESA_HOOKBEFORE_EXIT_SELF_REF      EQU    00028144h

 ; AGESA IDS CALLOUTS
    AGESA_GET_IDS_INIT_DATA     EQU    00028200h

 ; AGESA GNB CALLOUTS
    AGESA_GNB_PCIE_SLOT_RESET   EQU    00028301h
; ------------------------------------------------------------------------

 ; HyperTransport Interface



; -----------------------------------------------------------------------------
                         ; HT DEFINITIONS AND MACROS

; -----------------------------------------------------------------------------


 ; Width equates for call backs
    HT_WIDTH_8_BITS             EQU    8
    HT_WIDTH_16_BITS            EQU    16
    HT_WIDTH_4_BITS             EQU    4
    HT_WIDTH_2_BITS             EQU    2
    HT_WIDTH_NO_LIMIT           EQU    HT_WIDTH_16_BITS

 ; Frequency Limit equates for call backs which take a frequency supported mask.
    HT_FREQUENCY_LIMIT_200M     EQU    1
    HT_FREQUENCY_LIMIT_400M     EQU    7
    HT_FREQUENCY_LIMIT_600M     EQU    1Fh
    HT_FREQUENCY_LIMIT_800M     EQU    3Fh
    HT_FREQUENCY_LIMIT_1000M    EQU    7Fh
    HT_FREQUENCY_LIMIT_HT1_ONLY EQU    7Fh
    HT_FREQUENCY_LIMIT_1200M    EQU    0FFh
    HT_FREQUENCY_LIMIT_1400M    EQU    1FFh
    HT_FREQUENCY_LIMIT_1600M    EQU    3FFh
    HT_FREQUENCY_LIMIT_1800M    EQU    7FFh
    HT_FREQUENCY_LIMIT_2000M    EQU    0FFFh
    HT_FREQUENCY_LIMIT_2200M    EQU    1FFFh
    HT_FREQUENCY_LIMIT_2400M    EQU    3FFFh
    HT_FREQUENCY_LIMIT_2600M    EQU    7FFFh
    HT_FREQUENCY_LIMIT_2800M    EQU    27FFFh
    HT_FREQUENCY_LIMIT_3000M    EQU    67FFFh
    HT_FREQUENCY_LIMIT_3200M    EQU    0E7FFFh
    HT_FREQUENCY_LIMIT_3600M    EQU    1E7FFFh
    HT_FREQUENCY_LIMIT_MAX      EQU    HT_FREQUENCY_LIMIT_3600M
    HT_FREQUENCY_NO_LIMIT       EQU    0FFFFFFFFh

 ; Unit ID Clumping special values
    HT_CLUMPING_DISABLE         EQU    00000000h
    HT_CLUMPING_NO_LIMIT        EQU    0FFFFFFFFh

    HT_LIST_TERMINAL            EQU    0FFh
    HT_LIST_MATCH_ANY           EQU    0FEh
    HT_LIST_MATCH_INTERNAL_LINK EQU    0FDh

 ; Event Notify definitions

 ; Event definitions.

 ; Coherent subfunction events
    HT_EVENT_COH_EVENTS         EQU    10001000h
    HT_EVENT_COH_NO_TOPOLOGY    EQU    10011000h
    HT_EVENT_COH_OBSOLETE000    EQU    10021000h
    HT_EVENT_COH_PROCESSOR_TYPE_MIX     EQU    10031000h
    HT_EVENT_COH_NODE_DISCOVERED        EQU    10041000h
    HT_EVENT_COH_MPCAP_MISMATCH EQU    10051000h

 ; Non-coherent subfunction events
    HT_EVENT_NCOH_EVENTS        EQU    10002000h
    HT_EVENT_NCOH_BUID_EXCEED   EQU    10012000h
    HT_EVENT_NCOH_OBSOLETE000   EQU    10022000h
    HT_EVENT_NCOH_BUS_MAX_EXCEED        EQU    10032000h
    HT_EVENT_NCOH_CFG_MAP_EXCEED        EQU    10042000h
    HT_EVENT_NCOH_DEVICE_FAILED EQU    10052000h
    HT_EVENT_NCOH_AUTO_DEPTH    EQU    10062000h

 ; Optimization subfunction events
    HT_EVENT_OPT_EVENTS         EQU    10003000h
    HT_EVENT_OPT_REQUIRED_CAP_RETRY     EQU    10013000h
    HT_EVENT_OPT_REQUIRED_CAP_GEN3      EQU    10023000h
    HT_EVENT_OPT_UNUSED_LINKS   EQU    10033000h
    HT_EVENT_OPT_LINK_PAIR_EXCEED       EQU    10043000h

 ; HW Fault events
    HT_EVENT_HW_EVENTS          EQU    10004000h
    HT_EVENT_HW_SYNCFLOOD       EQU    10014000h
    HT_EVENT_HW_HTCRC           EQU    10024000h

 ; The Recovery HT component uses 0x10005000 for events.
 ; For consistency, we avoid that range here.

    HT_MAX_NC_BUIDS             EQU    32
; ----------------------------------------------------------------------------
                         ; HT TYPEDEFS, STRUCTURES, ENUMS

; ----------------------------------------------------------------------------
MATCHED                 EQU     0               ; < The link matches the requested customization.
POWERED_OFF             EQU     1               ; < Power the link off.
UNMATCHED               EQU     2               ; < The link should be processed according to normal defaults.
MaxFinalLinkState       EQU     3               ; < Not a final link state, use for limit checking.
FINAL_LINK_STATE        TEXTEQU <DWORD>

 ; Swap a device from its current id to a new one.

BUID_SWAP_ITEM    STRUCT
  FromId                        UINT8 ?         ; < The device responding to FromId,
  ToId                          UINT8 ?         ; < will be moved to ToId.
BUID_SWAP_ITEM    ENDS


 ; Each Non-coherent chain may have a list of device swaps.  After performing the swaps,
 ; the final in order list of device ids is provided. (There can be more swaps than devices.)
 ; The unused entries in both are filled with 0xFF.

BUID_SWAP_LIST    STRUCT
  Swaps                         BUID_SWAP_ITEM (HT_MAX_NC_BUIDS) DUP ({})       ; < The BUID Swaps to perform
  FinalIds                      UINT8 (HT_MAX_NC_BUIDS) DUP (?)         ; < The ordered final BUIDs, resulting from the swaps
BUID_SWAP_LIST    ENDS


 ; Control Manual Initialization of Non-Coherent Chains

 ; This interface is checked every time a non-coherent chain is
 ; processed.  BUID assignment may be controlled explicitly on a
 ; non-coherent chain. Provide a swap list.  Swaps controls the
 ; BUID assignment and FinalIds provides the device to device
 ; Linking.  Device orientation can be detected automatically, or
 ; explicitly.  See interface documentation for more details.

 ; If a manual swap list is not supplied,
 ; automatic non-coherent init assigns BUIDs starting at 1 and incrementing sequentially
 ; based on each device's unit count.

MANUAL_BUID_SWAP_LIST    STRUCT
   ; Match fields
  Socket                        UINT8 ?         ; < The Socket on which this chain is located
  Link                          UINT8 ?         ; < The Link on the host for this chain
   ; Override fields
  SwapList                      BUID_SWAP_LIST {}       ; < The swap list
MANUAL_BUID_SWAP_LIST    ENDS


 ; Override options for DEVICE_CAP_OVERRIDE.

 ; Specify which override actions should be performed.  For Checks, 1 means to check the item
 ; and 0 means to skip the check.  For the override options, 1 means to apply the override and
 ; 0 means to ignore the override.

DEVICE_CAP_OVERRIDE_OPTIONS    STRUCT
  IsCheckDevVenId               UINT32 ?
;   IN       UINT32  IsCheckDevVenId:1;        ; < Check Match on Device/Vendor id
;   IN       UINT32  IsCheckRevision:1;         ; < Check Match on device Revision
;   IN       UINT32  IsOverrideWidthIn:1;       ; < Override Width In
;   IN       UINT32  IsOverrideWidthOut:1;      ; < Override Width Out
;   IN       UINT32  IsOverrideFreq:1;          ; < Override Frequency
;   IN       UINT32  IsOverrideClumping:1;      ; < Override Clumping
;   IN       UINT32  IsDoCallout:1;             ; < Make the optional callout
DEVICE_CAP_OVERRIDE_OPTIONS    ENDS

 ; Override capabilities of a device.

 ; This interface is checked once for every Link on every IO device.
 ; Provide the width and frequency capability if needed for this device.
 ; This is used along with device capabilities, the limit interfaces, and northbridge
 ; limits to compute the default settings.  The components of the device's PCI config
 ; address are provided, so its settings can be consulted if need be.
 ; The optional callout is a catch all.

DEVICE_CAP_OVERRIDE    STRUCT
   ; Match fields
  HostSocket                    UINT8 ?         ; < The Socket on which this chain is located.
  HostLink                      UINT8 ?         ; < The Link on the host for this chain.
  Depth                         UINT8 ?         ; < The Depth in the I/O chain from the Host.
  DevVenId                      UINT32 ?        ; < The Device's PCI Vendor + Device ID (offset 0x00).
  Revision                      UINT8 ?         ; < The Device's PCI Revision field (offset 0x08).
  Link                          UINT8 ?         ; < The Device's Link number (0 or 1).
  Options                       DEVICE_CAP_OVERRIDE_OPTIONS {}          ; < The options for this device override.
   ; Override fields
  LinkWidthIn                   UINT8 ?         ; < modify to change the Link Width In.
  LinkWidthOut                  UINT8 ?         ; < modify to change the Link Width Out.
  FreqCap                       UINT32 ?        ; < modify to change the Link's frequency capability.
  Clumping                      UINT32 ?        ; < modify to change Unit ID clumping support.
  Callout                       CALLOUT_ENTRY ?         ; < optional call for really complex cases, or NULL.
DEVICE_CAP_OVERRIDE    ENDS

 ; Callout param struct for override capabilities of a device.

 ; If the optional callout is implemented this param struct is passed to it.

DEVICE_CAP_CALLOUT_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < The header
   ; Match fields
  HostSocket                    UINT8 ?         ; < The Socket on which this chain is located.
  HostLink                      UINT8 ?         ; < The Link on the host for this chain.
  Depth                         UINT8 ?         ; < The Depth in the I/O chain from the Host.
  DevVenId                      UINT32 ?        ; < The Device's PCI Vendor + Device ID (offset 0x00).
  Revision                      UINT8 ?         ; < The Device's PCI Revision field (offset 0x08).
  Link                          UINT8 ?         ; < The Device's Link number (0 or 1).
  PciAddress                    PCI_ADDR {}      ; < The Device's PCI Address.
   ; Override fields
     LinkWidthIn                POINTER ?       ; < modify to change the Link Width In.
     LinkWidthOut               POINTER ?       ; < modify to change the Link Width Out.
     FreqCap                    POINTER ?       ; < modify to change the Link's frequency capability.
     Clumping                   POINTER ?       ; < modify to change Unit ID clumping support.
DEVICE_CAP_CALLOUT_PARAMS    ENDS

  ; Limits for CPU to CPU Links.

  ; For each coherent connection this interface is checked once.
  ; Provide the frequency and width if needed for this Link (usually based on board
  ; restriction).  This is used with CPU device capabilities and northbridge limits
  ; to compute the default settings.

CPU_TO_CPU_PCB_LIMITS    STRUCT
   ; Match fields
  SocketA                       UINT8 ?         ; < One Socket on which this Link is located
  LinkA                         UINT8 ?         ; < The Link on this Node
  SocketB                       UINT8 ?         ; < The other Socket on which this Link is located
  LinkB                         UINT8 ?         ; < The Link on that Node
   ; Limit fields
  ABLinkWidthLimit              UINT8 ?         ; < modify to change the Link Width A->B
  BALinkWidthLimit              UINT8 ?         ; < modify to change the Link Width B-<A
  PcbFreqCap                    UINT32 ?        ; < modify to change the Link's frequency capability
CPU_TO_CPU_PCB_LIMITS    ENDS

  ; Get limits for non-coherent Links.

 ; For each non-coherent connection this interface is checked once.
 ; Provide the frequency and width if needed for this Link (usually based on board
 ; restriction).  This is used with device capabilities, device overrides, and northbridge limits
 ; to compute the default settings.

IO_PCB_LIMITS    STRUCT
   ; Match fields
  HostSocket                    UINT8 ?         ; < The Socket on which this Link is located
  HostLink                      UINT8 ?         ; < The Link about to be initialized
  Depth                         UINT8 ?         ; < The Depth in the I/O chain from the Host
   ; Limit fields
  DownstreamLinkWidthLimit      UINT8 ?         ; < modify to change the Link Width going away from processor
  UpstreamLinkWidthLimit        UINT8 ?         ; < modify to change the Link Width moving toward processor
  PcbFreqCap                    UINT32 ?        ; < modify to change the Link's frequency capability
IO_PCB_LIMITS    ENDS

  ; Manually control bus number assignment.

 ; This interface is checked every time a non-coherent chain is processed.
 ; If a system can not use the auto Bus numbering feature for non-coherent chain bus
 ; assignments, this interface can provide explicit control.  For each chain, provide
 ; the bus number range to use.

OVERRIDE_BUS_NUMBERS    STRUCT
   ; Match fields
  Socket                        UINT8 ?         ; < The Socket on which this chain is located
  Link                          UINT8 ?         ; < The Link on the host for this chain
   ; Override fields
  SecBus                        UINT8 ?         ; < Secondary Bus number for this non-coherent chain
  SubBus                        UINT8 ?         ; < Subordinate Bus number
OVERRIDE_BUS_NUMBERS    ENDS


  ; Ignore a Link.

  ; This interface is checked every time a coherent Link is found and then every
  ; time a non-coherent Link from a CPU is found.
  ; Any coherent or non-coherent Link from a CPU can be ignored and not used
  ; for discovery or initialization.  Useful for connection based systems.
  ; (Note: not checked for IO device to IO Device Links.)

IGNORE_LINK    STRUCT
   ; Match fields
  Socket                        UINT8 ?         ; < The Socket on which this Link is located
  Link                          UINT8 ?         ; < The Link about to be initialized
   ; Customization fields
  LinkState                     FINAL_LINK_STATE ? ; < The link may be left unitialized, or powered off.
IGNORE_LINK    ENDS


  ; Skip reganging of subLinks.

  ; This interface is checked whenever two subLinks are both connected to the same CPUs.
  ; Normally, unganged sublinks between the same two CPUs are reganged.
  ; Provide a matching structure to leave the Links unganged.

SKIP_REGANG    STRUCT
   ; Match fields
  SocketA                       UINT8 ?         ; < One Socket on which this Link is located
  LinkA                         UINT8 ?         ; < The Link on this Node
  SocketB                       UINT8 ?         ; < The other Socket on which this Link is located
  LinkB                         UINT8 ?         ; < The Link on that Node
   ; Customization fields
  LinkState                     FINAL_LINK_STATE ? ; < The paired sublink may be active, or powered off.
SKIP_REGANG    ENDS

  ; The System Socket layout, which sockets are physically connected.

  ; The hardware method for Socket naming is preferred.  Use this software method only
  ; if required.

SYSTEM_PHYSICAL_SOCKET_MAP    STRUCT
  CurrentSocket                 UINT8 ?         ; < The socket from which this connection originates.
  CurrentLink                   UINT8 ?         ; < The Link from the source socket connects to another socket.
  TargetSocket                  UINT8 ?         ; < The target socket which is connected on that link.
SYSTEM_PHYSICAL_SOCKET_MAP    ENDS

; ----------------------------------------------------------------------------

 ; This is the input structure for AmdHtInitialize.

AMD_HT_INTERFACE    STRUCT
   ; Basic level customization
  AutoBusStart                  UINT8 ?         ; < For automatic bus number assignment, starting bus number usually zero.
  AutoBusMax                    UINT8 ?         ; < For automatic bus number assignment, do not assign above max.
  AutoBusIncrement              UINT8 ?         ; < For automatic bus number assignment, each chain gets this many busses.

   ; Advanced Level Customization
  ManualBuidSwapList            POINTER ?       ; < Provide Manual Swap List, if any.
  DeviceCapOverrideList         POINTER ?       ; < Provide Device Overrides, if any.
  CpuToCpuPcbLimitsList         POINTER ?       ; < Provide CPU PCB Limits, if any.
  IoPcbLimitsList               POINTER ?       ; < Provide IO PCB Limits, if any.
  OverrideBusNumbersList        POINTER ?       ; < Provide manual Bus Number assignment, if any.
                                                 ; < Use either auto bus numbering or override bus
                                                 ; < numbers, not both.

  IgnoreLinkList                POINTER ?       ; < Provide links to ignore, if any.
  SkipRegangList                POINTER ?       ; < Provide links to remain unganged, if any.

   ; Expert Level Customization
  Topolist                      POINTER ?       ; < Use this topology list in addition to the built in, if not NULL.
  SystemPhysicalSocketMap       POINTER ?
                                                     ; < The hardware socket naming method is preferred,
                                                     ; <  If it can't be used, this provides a software method.
AMD_HT_INTERFACE    ENDS

; -----------------------------------------------------------------------------

 ; HT Recovery Interface



; -----------------------------------------------------------------------------
; *              HT Recovery DEFINITIONS AND MACROS
; *
; *-----------------------------------------------------------------------------
;

 ; BBHT subfunction events
    HT_EVENT_BB_EVENTS          EQU    10005000h
    HT_EVENT_BB_BUID_EXCEED     EQU    10015000h
    HT_EVENT_BB_DEVICE_FAILED   EQU    10055000h
    HT_EVENT_BB_AUTO_DEPTH      EQU    10065000h

; ----------------------------------------------------------------------------
; *                      HT Recovery   TYPEDEFS, STRUCTURES, ENUMS
; *
; *----------------------------------------------------------------------------
;


 ; The Interface structure to Recovery HT.

AMD_HT_RESET_INTERFACE    STRUCT
  ManualBuidSwapList            POINTER ?       ; < Option to manually control SB link init
     Depth                      UINT32 ?        ; < If auto init was used this is set to the depth of the chain,
                                   ; < else, for manual init unmodified.
AMD_HT_RESET_INTERFACE    ENDS

; ---------------------------------------------------------------------------
;  CPU Feature related info
; ---------------------------------------------------------------------------
 ; Build Configuration values for BLDCFG_PLATFORM_C1E_MODE
  C1eModeDisabled       EQU    0        ; < Disabled
  C1eModeHardware       EQU    1        ; < Hardware method
  C1eModeMsgBased       EQU    2        ; < Message-based method
  MaxC1eMode            EQU    3        ; < Not a valid value, used for verifying input
PLATFORM_C1E_MODES TEXTEQU  <DWORD>

 ; Build Configuration values for BLDCFG_PLATFORM_CSTATE_MODE
  CStateModeDisabled    EQU    0        ; < Disabled
  CStateModeC6          EQU    1        ; < C6 State
  MaxCStateMode         EQU    2        ; < Not a valid value, used for verifying input
PLATFORM_CSTATE_MODES TEXTEQU  <DWORD>


;----------------------------------------------------------------------------
;   GNB PCIe configuration info
;----------------------------------------------------------------------------

GNB_EVENT_INVALID_CONFIGURATION                 EQU  20010000h   ; User configuration invalid
GNB_EVENT_INVALID_PCIE_TOPOLOGY_CONFIGURATION   EQU  20010001h   ; Requested lane allocation for PCIe port can not be supported
GNB_EVENT_INVALID_PCIE_PORT_CONFIGURATION       EQU  20010002h   ; Requested incorrect PCIe port device address
GNB_EVENT_INVALID_DDI_LINK_CONFIGURATION        EQU  20010003h   ; Incorrect parameter in DDI link configuration
GNB_EVENT_INVALID_LINK_WIDTH_CONFIGURATION      EQU  20010004h   ; Invalid with for PCIe port or DDI link
GNB_EVENT_INVALID_DDI_LANES_CONFIGURATION       EQU  20010005h   ; DDI lane double subscribe lanes
GNB_EVENT_INVALID_DDI_TOPOLOGY_CONFIGURATION    EQU  20010006h   ; Requested lane allocation for DDI link(s) can not be supported
GNB_EVENT_LINK_TRAINING_FAIL                    EQU  20020000h   ; PCIe Link training fail
GNB_EVENT_BROKEN_LANE_RECOVERY                  EQU  20030000h   ; Broken lane workaround applied to recover link training
GNB_EVENT_GEN2_SUPPORT_RECOVERY                 EQU  20040000h   ; Scale back to GEN1 to recover link training

DESCRIPTOR_TERMINATE_LIST                       EQU  80000000h

PCIe_PORT_DATA                  STRUCT
  PortPresent                   UINT8  ?                  ; < Enable PCIe port for initialization.
  ChannelType                   UINT8  ?                  ; < Channel type.
                                                          ;  0 - "lowLoss",
                                                          ;  1 - "highLoss",
                                                          ;  2 - "mob0db",
                                                          ;  3 - "mob3db",
                                                          ;  4 - "extnd6db"
                                                          ;  5 - "extnd8db"
                                                          ;
  DeviceNumber                  UINT8  ?                  ; < Device number for port. Available device numbers may very on different CPUs.
  FunctionNumber                UINT8  ?                  ; < Reserved for future use
  LinkSpeedCapability           UINT8  ?                  ; < Advertised Gen Capability
                                                          ;  0 - Maximum supported by silicon
                                                          ;  1 - Gen1
                                                          ;  2 - Gen2
                                                          ;  3 - Gen3
                                                          ;
  LinkSpeed                     UINT8  ?                  ; < Default boot up speed
                                                          ;  0 - Maximum supported by silicon
                                                          ;  1 - Gen1
                                                          ;  2 - Gen2
                                                          ;  3 - Gen3
                                                          ;
  LinkAspm                      UINT8  ?                  ; < ASPM control. (see OemPcieLinkAspm for additional option to control ASPM)
                                                          ;  0 - Disabled
                                                          ;  1 - L0s only
                                                          ;  2 - L1 only
                                                          ;  2 - L0s and L1
                                                          ;
  LinkHotplug                   UINT8  ?                  ; < Hotplug control.
                                                          ;  0 - Disabled
                                                          ;  1 - Basic
                                                          ;  2 - Server
                                                          ;  3 - Enhanced
                                                          ;
  ResetId                       UINT8  ?                  ; < Arbitrary number greater than 0 assigned by platform firmware for GPIO
                                                          ; identification which control reset for given port.
                                                          ; Each port with unique GPIO should have unique ResetId assigned.
                                                          ; All ports use same GPIO to control reset should have same ResetId assigned.
                                                          ; see AgesaPcieSlotResetControl
                                                          ;
  Compliance                    UINT8  ?                  ; < Force port into compliance mode (device will not be trained, port output compliance pattern)
PCIe_PORT_DATA                  ENDS

; DDI Configuration
PCIe_DDI_DATA                   STRUCT
  ConnectorType                 UINT8  ?                  ; < Display Connector Type
                                                          ;  0 - DP
                                                          ;  1 - eDP
                                                          ;  2 - Single Link DVI
                                                          ;  3 - Dual  Link DVI
                                                          ;  4 - HDMI
                                                          ;  5 - Travis DP-to-VGA
                                                          ;  6 - Travis DP-to-LVDS
                                                          ;  7 - SB900 NutMeg DP-to-VGA
                                                          ;
  AuxIndex                      UINT8  ?                  ; < Indicates which AUX or DDC Line is used
                                                          ;  0 - AUX1
                                                          ;  1 - AUX2
                                                          ;  2 - AUX3
                                                          ;  3 - AUX4
                                                          ;  4 - AUX5
                                                          ;  5 - AUX6
                                                          ;
  HdpIndex                      UINT8  ?                  ; < Indicates which HDP pin is used
                                                          ;  0 - HDP1
                                                          ;  1 - HDP2
                                                          ;  2 - HDP3
                                                          ;  3 - HDP4
                                                          ;  4 - HDP5
                                                          ;  5 - HDP6
PCIe_DDI_DATA                   ENDS


; Engine Configuration
PCIe_ENGINE_DATA                STRUCT
  EngineType                    UINT8   ?                 ; < Engine type
                                                          ;  0 -  Ignore engine configuration
                                                          ;  1 -  PCIe port
                                                          ;  2 -  DDI
  StartLane                     UINT16  ?                 ; < Start lane number (in reversed configuration StartLane > EndLane).
  EndLane                       UINT16  ?                 ; < End lane number (in reversed configuration StartLane > EndLane).
PCIe_ENGINE_DATA                ENDS

; PCIe port descriptor
PCIe_PORT_DESCRIPTOR            STRUCT
  Flags                         UINT32  ?                 ; < Descriptor flags
                                                          ;  Bit31 - last descriptor in complex
  EngineData                    PCIe_ENGINE_DATA {}       ; < Engine data
  Port                          PCIe_PORT_DATA {}         ; < PCIe port specific configuration info
PCIe_PORT_DESCRIPTOR            ENDS

; DDI descriptor
PCIe_DDI_DESCRIPTOR             STRUCT
  Flags                         UINT32  ?                 ; < Descriptor flags
  EngineData                    PCIe_ENGINE_DATA {}       ; < Engine data
  Ddi                           PCIe_DDI_DATA {}          ; < DDI port specific configuration info
PCIe_DDI_DESCRIPTOR             ENDS

; Slot Reset Info
PCIe_SLOT_RESET_INFO            STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}      ; < AGESA Standard Header
  ResetId                       UINT8  ?                  ; < Slot reset ID as specified in PCIe_PORT_DESCRIPTOR
  ResetControl                  UINT8  ?                  ; < Reset control as defined by PCIE_RESET_CONTROL
PCIe_SLOT_RESET_INFO            ENDS


; PCIe Complex descriptor
PCIe_COMPLEX_DESCRIPTOR         STRUCT
  Flags                         UINT32  ?                 ; < Descriptor flags
                                                          ; Bit31 - last descriptor in topology
                                                          ;
                                                          ;
  SocketId                      UINT32  ?                 ; < Socket Id
  PciePortList                  POINTER ?                 ;< Pointer to array of PCIe port descriptors or NULL (Last element of array must be terminated with DESCRIPTOR_TERMINATE_LIST).
  DdiLinkList                   POINTER ?                 ;< Pointer to array DDI link descriptors (Last element of array must be terminated with DESCRIPTOR_TERMINATE_LIST).
  Reserved                      POINTER ?         ;< Reserved for future use
PCIe_COMPLEX_DESCRIPTOR         ENDS

  AssertSlotReset               EQU  0
  DeassertSlotReset             EQU  1
PCIE_RESET_CONTROL TEXTEQU  <DWORD>

  PcieUnusedEngine              EQU  0
  PciePortEngine                EQU  1
  PcieDdiEngine                 EQU  2
  MaxPcieEngine                 EQU  3                    ; < Not a valid value, used for verifying input
PCIE_ENGINE_TYPE   TEXTEQU  <DWORD>

  PcieGenMaxSupported           EQU  0
  PcieGen1                      EQU  1
  PcieGen2                      EQU  2
  MaxPcieGen                    EQU  3                    ; < Not a valid value, used for verifying input
PCIE_LINK_SPEED_CAP      TEXTEQU  <DWORD>

  PsppDisabled                  EQU  0
  PsppPerformance               EQU  1
  PsppBalanceHigh               EQU  2
  PsppBalanceLow                EQU  3
  PsppPowerSaving               EQU  4
  MaxPspp                       EQU  5                     ; < Not a valid value, used for verifying input
PCIE_PSPP_POLICY      TEXTEQU  <DWORD>

  ConnectorTypeDP               EQU  0
  ConnectorTypeEDP              EQU  1
  ConnectorTypeSingleLinkDVI    EQU  2
  ConnectorTypeDualLinkDVI      EQU  3
  ConnectorTypeHDMI             EQU  4
  ConnectorTypeTravisDpToVga    EQU  5
  ConnectorTypeTravisDpToLvds   EQU  6
  ConnectorTypeNutmegDpToVga    EQU  7
  MaxConnectorType              EQU  8                    ; < Not a valid value, used for verifying input
PCIE_CONNECTOR_TYPE      TEXTEQU  <DWORD>

  ChannelTypeLowLoss            EQU  0
  ChannelTypeHighLoss           EQU  1
  ChannelTypeMob0db             EQU  2
  ChannelTypeMob3db             EQU  3
  ChannelTypeExt6db             EQU  4
  ChannelTypeExt8db             EQU  5
  MaxChannelType                EQU  6                    ; < Not a valid value, used for verifying input
PCIE_CHANNEL_TYPE  TEXTEQU  <DWORD>

  AspmDisabled                  EQU  0
  AspmL0s                       EQU  1
  AspmL1                        EQU  2
  AspmL0sL1                     EQU  3
  MaxAspm                       EQU  4                    ; < Not a valid value, used for verifying input
PCIE_ASPM_TYPE  TEXTEQU  <DWORD>

  HotplugDisabled               EQU  0
  HotplugBasic                  EQU  1
  HotplugServer                 EQU  2
  HotplugEnhanced               EQU  3
  MaxHotplug                    EQU  4                    ; < Not a valid value, used for verifying input
PCIE_HOTPLUG_TYPE TEXTEQU  <DWORD>

  PortDisabled                  EQU  0
  PortEnabled                   EQU  1
PCIE_PORT_ENABLE  TEXTEQU  <DWORD>

  Aux1                          EQU  0
  Aux2                          EQU  1
  Aux3                          EQU  2
  Aux4                          EQU  3
  Aux5                          EQU  4
  Aux6                          EQU  5
  MaxAux                        EQU  6                    ; < Not a valid value, used for verifying input
PCIE_AUX_TYPE TEXTEQU  <DWORD>

  Hdp1                          EQU  0
  Hdp2                          EQU  1
  Hdp3                          EQU  2
  Hdp4                          EQU  3
  Hdp5                          EQU  4
  Hdp6                          EQU  5
  MaxHdp                        EQU  6                   ; < Not a valid value, used for verifying input
PCIE_HDP_TYPE TEXTEQU  <DWORD>

;----------------------------------------------------------------------------
;   GNB configuration info
;----------------------------------------------------------------------------
;

; GNB configuration info
GNB_CONFIGURATION              STRUCT
  PcieComplexList              POINTER ?      ; Pointer to array of PCIe_COMPLEX_DESCRIPTOR structures describe PCIe topology on each processor package or NULL.
                                              ;  Last element of array must be terminated with DESCRIPTOR_TERMINATE_LIST
                                              ;
                                              ;
                                              ;
                                              ;  Topology organization definition assume PCIe_COMPLEX_DESCRIPTOR defined first following
                                              ;  PCIe_PORT_DESCRIPTOR and  PCIe_DDI_DESCRIPTOR for given PCIe_COMPLEX_DESCRIPTOR
                                              ;  defined in arbitrary sequence:
                                              ;  Example of topology definition for single socket system:
                                              ;    PlatfromTopology LABEL DWORD
                                              ;
                                              ;    Port0_2  PCIe_PORT_DESCRIPTOR <>;
                                              ;    Port0_3  PCIe_PORT_DESCRIPTOR <DESCRIPTOR_TERMINATE_LIST> ;
                                              ;    ...
                                              ;    Ddi0_A   PCIe_DDI_DESCRIPTOR <>;
                                              ;    Ddi0_B   PCIe_DDI_DESCRIPTOR <DESCRIPTOR_TERMINATE_LIST,>;
                                              ;    ...
                                              ;    Cpu0     PCIe_COMPLEX_DESCRIPTOR <DESCRIPTOR_TERMINATE_LIST, 0, Port0_2, Ddi0_A>
                                              ;
                                              ;
  PsppPolicy                   UINT8  ?       ;< PSPP (PCIe Speed Power Policy)
                                              ;  @li @b 0 - Disabled
                                              ;  @li @b 1 - Performance
                                              ;  @li @b 2 - Balance-High
                                              ;  @li @b 3 - Balance-Low
                                              ;  @li @b 4 - Power Saving
                                              ;
GNB_CONFIGURATION              ENDS


; ---------------------------------------------------------------------------

; MEMORY-SPECIFIC DATA STRUCTURES

; ---------------------------------------------------------------------------


 ; AGESA MAXIMIUM VALUES

   ; These Max values are used to define array sizes and associated loop
   ; counts in the code.  They reflect the maximum values that AGESA
   ; currently supports and does not necessarily reflect the hardware
   ; capabilities of configuration.


    MAX_SOCKETS_SUPPORTED       EQU    8       ; < Max number of sockets in system
    MAX_CHANNELS_PER_SOCKET     EQU    4       ; < Max Channels per sockets
    MAX_DIMMS_PER_CHANNEL       EQU    4       ; < Max DIMMs on a memory channel (independent of platform)
    NUMBER_OF_DELAY_TABLES      EQU    9       ; < Number of tables defined in CH_DEF_STRUCT.
                                               ; < Eg: UINT16  *RcvEnDlys;
                                               ; <     UINT8   *WrDqsDlys;
                                               ; <     UINT8   *RdDqsDlys;
                                               ; <     UINT8   *WrDatDlys;
                                               ; <     UINT8   *RdDqsMinDlys;
                                               ; <     UINT8   *RdDqsMaxDlys;
                                               ; <     UINT8   *WrDatMinDlys;
                                               ; <     UINT8   *WrDatMaxDlys;
    NUMBER_OF_FAILURE_MASK_TABLES EQU 1        ; < Number of failure mask tables
    MAX_PLATFORM_TYPES          EQU    16       ; < Platform types per system

    MCT_TRNG_KEEPOUT_START      EQU    00004000h        ; < base [39:8]
    MCT_TRNG_KEEPOUT_END        EQU    00007FFFh        ; < base [39:8]

    UMA_ATTRIBUTE_INTERLEAVE    EQU    80000000h    ; < Uma Region is interleaved

    PSO_TABLE           TEXTEQU    <UINT8>; < Platform Configuration Table

        ; AGESA DEFINITIONS

        ; Many of these are derived from the platform and hardware specific definitions

 ; EccSymbolSize override value
    ECCSYMBOLSIZE_USE_BKDG      EQU    0       ; < Use BKDG Recommended Value
    ECCSYMBOLSIZE_FORCE_X4      EQU    4       ; < Force to x4
    ECCSYMBOLSIZE_FORCE_X8      EQU    8       ; < Force to x8
 ; CPU Package Type
    PT_L1                       EQU    0                     ; < L1 Package type
    PT_M2                       EQU    1                     ; < AM Package type
    PT_S1                       EQU    2                     ; < S1 Package type

 ; Structures use to pass system Logical CPU-ID
CPU_LOGICAL_ID    STRUCT
  Family                        UINT64 ?        ; < Indicates logical ID Family
  Revision                      UINT64 ?        ; < Indicates logical ID Family
CPU_LOGICAL_ID    ENDS

 ; Build Configuration values for BLDCFG_AMD_PLATFORM_TYPE

  AMD_PLATFORM_SERVER   EQU    8000h    ; < Server
  AMD_PLATFORM_DESKTOP  EQU    10000h   ; < Desktop
  AMD_PLATFORM_MOBILE   EQU    20000h   ; < Mobile
AMD_PLATFORM_TYPE  TEXTEQU  <DWORD>

 ; Dram technology type

  DDR2_TECHNOLOGY       EQU    0        ; < DDR2 technology
  DDR3_TECHNOLOGY       EQU    1        ; < DDR3 technology
TECHNOLOGY_TYPE  TEXTEQU  <DWORD>

 ; Build Configuration values for BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT & BLDCFG_MEMORY_CLOCK_SELECT

  DDR400_FREQUENCY      EQU    200      ; < DDR 400
  DDR533_FREQUENCY      EQU    266      ; < DDR 533
  DDR667_FREQUENCY      EQU    333      ; < DDR 667
  DDR800_FREQUENCY      EQU    400      ; < DDR 800
  DDR1066_FREQUENCY     EQU    533      ; < DDR 1066
  DDR1333_FREQUENCY     EQU    667      ; < DDR 1333
  DDR1600_FREQUENCY     EQU    800      ; < DDR 1600
  DDR1866_FREQUENCY     EQU    933      ; < DDR 1866
  UNSUPPORTED_DDR_FREQUENCY     EQU    934      ; < Highest limit of DDR frequency
MEMORY_BUS_SPEED  TEXTEQU  <DWORD>

 ; Build Configuration values for BLDCFG_MEMORY_QUADRANK_TYPE

  QUADRANK_REGISTERED   EQU    0
  QUADRANK_UNBUFFERED   EQU    1
QUANDRANK_TYPE  TEXTEQU  <DWORD>

 ; Build Configuration values for BLDCFG_TIMING_MODE_SELECT

  TIMING_MODE_AUTO      EQU    0        ; < Use best rate possible
  TIMING_MODE_LIMITED   EQU    1        ; < Set user top limit
  TIMING_MODE_SPECIFIC  EQU    2        ; < Set user specified speed
USER_MEMORY_TIMING_MODE  TEXTEQU  <DWORD>

 ; Build Configuration values for BLDCFG_POWER_DOWN_MODE

  POWER_DOWN_BY_CHANNEL EQU    0
  POWER_DOWN_BY_CHIP_SELECT     EQU    1
POWER_DOWN_MODE  TEXTEQU  <DWORD>

 ; Low voltage support

  VOLT1_5 EQU 0                  ; < 1.5 Volt
  VOLT1_35 EQU 1                 ; < 1.35 Volt
  VOLT1_25 EQU 2                 ; < 1.25 Volt
  VOLT_UNSUPPORTED EQU 0xFF      ; < No common voltage found
DIMM_VOLTAGE   TEXTEQU  <DWORD>

 ; UMA Mode

  UMA_NONE       EQU    0        ; < UMA None
  UMA_SPECIFIED  EQU    1        ; < UMA Specified
  UMA_AUTO       EQU    2        ; < UMA Auto
UMA_MODE  TEXTEQU  <DWORD>

 ; Build Configuration values for BLDCFG_UMA_ALIGNMENT

  NO_UMA_ALIGNED    EQU    00FFFFFFh
  UMA_128MB_ALIGNED EQU    00FFF800h
  UMA_256MB_ALIGNED EQU    00FFF000h
  UMA_512MB_ALIGNED EQU    00FFE000h
UMA_ALIGNMENT  TEXTEQU  <DWORD>
 ; ===============================================================================
   ; Global MCT Configuration Status Word (GStatus)
 ; ===============================================================================

  GsbMTRRshort          EQU    0        ; < Ran out of MTRRs while mapping memory
  GsbAllECCDimms        EQU    1        ; < All banks of all Nodes are ECC capable
  GsbDramECCDis         EQU    2        ; < Dram ECC requested but not enabled.
  GsbSoftHole           EQU    3        ; < A Node Base gap was created
  GsbHWHole             EQU    4        ; < A HW dram remap was created
  GsbNodeIntlv          EQU    5        ; < Node Memory interleaving was enabled
  GsbSpIntRemapHole     EQU    6        ; < Special condition for Node Interleave and HW remapping
  GsbEnDIMMSpareNW      EQU    7        ; < Indicates that DIMM Spare can be used without a warm reset

  GsbEOL                EQU    8        ; < End of list
GLOBAL_STATUS_FIELD  TEXTEQU  <DWORD>

; ===============================================================================
   ; Local Error Status (DIE_STRUCT.ErrStatus[31:0])
; ===============================================================================

  EsbNoDimms            EQU    0        ; < No DIMMs
  EsbSpdChkSum          EQU    1        ; < SPD Checksum fail
  EsbDimmMismatchM      EQU    2        ; < dimm module type(buffer) mismatch
  EsbDimmMismatchT      EQU    3        ; < dimm CL/T mismatch
  EsbDimmMismatchO      EQU    4        ; < dimm organization mismatch (128-bit)
  EsbNoTrcTrfc          EQU    5        ; < SPD missing Trc or Trfc info
  EsbNoCycTime          EQU    6        ; < SPD missing byte 23 or 25
  EsbBkIntDis           EQU    7        ; < Bank interleave requested but not enabled
  EsbDramECCDis         EQU    8        ; < Dram ECC requested but not enabled
  EsbSpareDis           EQU    9        ; < Online spare requested but not enabled
  EsbMinimumMode        EQU    10       ; < Running in Minimum Mode
  EsbNoRcvrEn           EQU    11       ; < No DQS Receiver Enable pass window found
  EsbSmallRcvr          EQU    12       ; < DQS Rcvr En pass window too small (far right of dynamic range)
  EsbNoDqsPos           EQU    13       ; < No DQS-DQ passing positions
  EsbSmallDqs           EQU    14       ; < DQS-DQ passing window too small
  EsbDCBKScrubDis       EQU    15       ; < DCache scrub requested but not enabled

  EsbEMPNotSupported    EQU    16       ; < Processor is not capable for EMP.
  EsbEMPConflict        EQU    17       ; < EMP requested but cannot be enabled since
                               ; < channel interleaving, bank interleaving, or bank swizzle is enabled.
  EsbEMPDis             EQU    18       ; < EMP requested but cannot be enabled since
                               ; < memory size of each DCT is not a power of two.

  EsbEOL                EQU    19       ; < End of list
ERROR_STATUS_FIELD  TEXTEQU  <DWORD>

; ===============================================================================
  ; Local Configuration Status (DIE_STRUCT.Status[31:0])
; ===============================================================================

  SbRegistered          EQU    0        ; < All DIMMs are Registered
  SbEccDimms            EQU    1        ; < All banks ECC capable
  SbParDimms            EQU    2        ; < All banks Addr/CMD Parity capable
  SbDiagClks            EQU    3        ; < Jedec ALL slots clock enable diag mode
  Sb128bitmode          EQU    4        ; < DCT in 128-bit mode operation
  Sb64MuxedMode         EQU    5        ; < DCT in 64-bit mux'ed mode.
  Sb2TMode              EQU    6        ; < 2T CMD timing mode is enabled.
  SbSWNodeHole          EQU    7        ; < Remapping of Node Base on this Node to create a gap.
  SbHWHole              EQU    8        ; < Memory Hole created on this Node using HW remapping.
  SbOver400Mhz          EQU    9        ; < DCT freq greater than or equal to 400MHz flag
  SbDQSPosPass2         EQU    10       ; < Used for TrainDQSPos DIMM0/1, when freq greater than or equal to 400MHz
  SbDQSRcvLimit         EQU    11       ; < Used for DQSRcvEnTrain to know we have reached the upper bound.
  SbExtConfig           EQU    12       ; < Indicate the default setting for extended PCI configuration support

  SbEOL                 EQU    13       ; < End of list
LOCAL_STATUS_FIELD  TEXTEQU  <DWORD>


; < CPU MSR Register definitions ------------------------------------------
    SYS_CFG                     EQU    0C0010010h
    TOP_MEM                     EQU    0C001001Ah
    TOP_MEM2                    EQU    0C001001Dh
    HWCR                        EQU    0C0010015h
    NB_CFG                      EQU    0C001001Fh

    FS_BASE                     EQU    0C0000100h
    IORR0_BASE                  EQU    0C0010016h
    IORR0_MASK                  EQU    0C0010017h
    BU_CFG                      EQU    0C0011023h
    BU_CFG2                     EQU    0C001102Ah
    COFVID_STAT                 EQU    0C0010071h
    TSC                         EQU    10h

; ===============================================================================
 ; SPD Data for each DIMM
; ===============================================================================
SPD_DEF_STRUCT    STRUCT
  DimmPresent                   BOOLEAN ?       ; < Indicates that the DIMM is present and Data is valid
  Data                          UINT8 (256) DUP (?)     ; < Buffer for 256 Bytes of SPD data from DIMM
SPD_DEF_STRUCT    ENDS

; ===============================================================================
 ; Channel Definition Structure
 ; This data structure defines entries that are specific to the channel initialization
; ===============================================================================
CH_DEF_STRUCT    STRUCT
  ChannelID                     UINT8 ?         ; < Physical channel ID of a socket(0 = CH A, 1 = CH B, 2 = CH C, 3 = CH D)
  TechType                      TECHNOLOGY_TYPE ?       ; < Technology type of this channel
  ChDimmPresent                 UINT8 ?         ; < For each bit n 0..7, 1 = DIMM n is present.
                                 ; <  DIMM#  Select Signal
                                 ; <  0      MA0_CS_L[0, 1]
                                 ; <  1      MB0_CS_L[0, 1]
                                 ; <  2      MA1_CS_L[0, 1]
                                 ; <  3      MB1_CS_L[0, 1]
                                 ; <  4      MA2_CS_L[0, 1]
                                 ; <  5      MB2_CS_L[0, 1]
                                 ; <  6      MA3_CS_L[0, 1]
                                 ; <  7      MB3_CS_L[0, 1]

  DCTPtr                        POINTER ?       ; < Pointer to the DCT data of this channel.
  MCTPtr                        POINTER ?       ; < Pointer to the node data of this channel.
  SpdPtr                        POINTER ?       ; < Pointer to the SPD data for this channel. (Setup by NB Constructor)
  DimmSpdPtr                    POINTER (MAX_DIMMS_PER_CHANNEL) DUP (?)         ; < Array of pointers to
                                 ; <   SPD Data for each Dimm. (Setup by Tech Block Constructor)
  ChDimmValid                   UINT8 ?         ; < For each bit n 0..7, 1 = DIMM n is valid and is/will be configured
  RegDimmPresent                UINT8 ?         ; < For each bit n 0..7, 1 = DIMM n is a registered DIMM
  SODimmPresent                 UINT8 ?         ; < For each bit n 0..7, 1 = DIMM n is a SO-DIMM
  Loads                         UINT8 ?         ; < Number of devices loading bus
  Dimms                         UINT8 ?         ; < Number of DIMMs loading Channel
  Ranks                         UINT8 ?         ; < Number of ranks loading Channel DATA
  SlowMode                      BOOLEAN ?       ; < 1T or 2T CMD mode (slow access mode)
                                 ; < FALSE = 1T
                                 ; < TRUE = 2T
  ; < The following pointers will be pointed to dynamically allocated buffers.
  ; < Each buffer is two dimensional (RowCount x ColumnCount) and is lay-outed as in below.
  ; < Example: If DIMM and Byte based training, then
  ; < XX is a value in Hex
  ; <                        BYTE 0, BYTE 1, BYTE 2, BYTE 3, BYTE 4, BYTE 5, BYTE 6, BYTE 7, ECC BYTE
  ; <  Row1 -  Logical DIMM0    XX      XX      XX      XX      XX      XX      XX      XX      XX
  ; <  Row2 -  Logical DIMM1    XX      XX      XX      XX      XX      XX      XX      XX      XX
  RcvEnDlys                     POINTER ?       ; < DQS Receiver Enable Delays
  WrDqsDlys                     POINTER ?       ; < Write DQS delays (only valid for DDR3)
  RdDqsDlys                     POINTER ?       ; < Read Dqs delays
  WrDatDlys                     POINTER ?       ; < Write Data delays
  RdDqsMinDlys                  POINTER ?       ; < Minimum Window for Read DQS
  RdDqsMaxDlys                  POINTER ?       ; < Maximum Window for Read DQS
  WrDatMinDlys                  POINTER ?       ; < Minimum Window for Write data
  WrDatMaxDlys                  POINTER ?       ; < Maximum Window for Write data
  RowCount                      UINT8 ?         ; < Number of rows of the allocated buffer.
  ColumnCount                   UINT8 ?         ; < Number of columns of the allocated buffer.

  FailingBitMask                POINTER ?       ; < Table of masks to Track Failing bits
  DctOdcCtl                     UINT32 ?        ; < Output Driver Strength (see BKDG FN2:Offset 9Ch, index 00h)
  DctAddrTmg                    UINT32 ?        ; < Address Bus Timing (see BKDG FN2:Offset 9Ch, index 04h)
  PhyRODTCSLow                  UINT32 ?        ; < Phy Read ODT Pattern Chip Select low (see BKDG FN2:Offset 9Ch, index 180h)
  PhyRODTCSHigh                 UINT32 ?        ; < Phy Read ODT Pattern Chip Select high (see BKDG FN2:Offset 9Ch, index 181h)
  PhyWODTCSLow                  UINT32 ?        ; < Phy Write ODT Pattern Chip Select low (see BKDG FN2:Offset 9Ch, index 182h)
  PhyWODTCSHigh                 UINT32 ?        ; < Phy Write ODT Pattern Chip Select high (see BKDG FN2:Offset 9Ch, index 183)
  PhyWLODT                      UINT8 (4) DUP (?)       ; < Write Levelization ODT Pattern for Dimm 0-3 (see BKDG FN2:Offset 9Ch, index 0x8[11:8])
  DctEccDqsLike                 UINT16 ?        ; < DCT DQS ECC UINT8 like...
  DctEccDqsScale                UINT8 ?         ; < DCT DQS ECC UINT8 scale
  PtrPatternBufA                UINT16 ?        ; < Ptr on stack to aligned DQS testing pattern
  PtrPatternBufB                UINT16 ?        ; < Ptr on stack to aligned DQS testing pattern
  ByteLane                      UINT8 ?         ; < Current UINT8 Lane (0..7)
  Direction                     UINT8 ?         ; < Current DQS-DQ training write direction (0=read, 1=write)
  Pattern                       UINT8 ?         ; < Current pattern
  DqsDelay                      UINT8 ?         ; < Current DQS delay value
  HostBiosSrvc1                 UINT16 ?        ; < UINT16 sized general purpose field for use by host BIOS.  Scratch space.
  HostBiosSrvc2                 UINT32 ?        ; < UINT32 sized general purpose field for use by host BIOS.  Scratch space.
  DctMaxRdLat                   UINT16 ?        ; < Max Read Latency (ns) for the DCT
  DIMMValidCh                   UINT8 ?         ; < DIMM# in CH
  MaxCh                         UINT8 ?         ; < Max number of CH in system
  Dct                           UINT8 ?         ; < Dct pointer
  WrDatGrossH                   UINT8 ?         ; < Write Data Gross delay high value
  DqsRcvEnGrossL                UINT8 ?         ; < DQS Receive Enable Gross Delay low

  TrwtWB                        UINT8 ?         ; <  Non-SPD timing value for TrwtWB
  CurrRcvrDctADelay             UINT8 ?         ; < for keep current RcvrEnDly
  T1000                         UINT16 ?        ; < get the T1000 figure (cycle time (ns) * 1K)
  DqsRcvEnPass                  UINT8 ?         ; < for TrainRcvrEn UINT8 lane pass flag
  DqsRcvEnSaved                 UINT8 ?         ; < for TrainRcvrEn UINT8 lane saved flag
  SeedPass1Remainder            UINT8 ?         ; < for Phy assisted DQS receiver enable training

  ClToNbFlag                    UINT8 ?         ; < is used to restore ClLinesToNbDis bit after memory
  NodeSysBase                   UINT32 ?        ; < for channel interleave usage
  RefRawCard                    UINT8 (MAX_DIMMS_PER_CHANNEL) DUP (?)           ; < Array of rawcards detected
  CtrlWrd02                     UINT8 (MAX_DIMMS_PER_CHANNEL) DUP (?)           ; < Control Word 2 values per DIMM
  CtrlWrd03                     UINT8 (MAX_DIMMS_PER_CHANNEL) DUP (?)           ; < Control Word 3 values per DIMM
  CtrlWrd04                     UINT8 (MAX_DIMMS_PER_CHANNEL) DUP (?)           ; < Control Word 4 values per DIMM
  CtrlWrd05                     UINT8 (MAX_DIMMS_PER_CHANNEL) DUP (?)           ; < Control Word 5 values per DIMM
  CtrlWrd08                     UINT8 (MAX_DIMMS_PER_CHANNEL) DUP (?)           ; < Control Word 8 values per DIMM

  CsPresentDCT                  UINT16 ?        ; < For each bit n 0..7, 1 = Chip-select n is present
  DimmMirrorPresent             UINT8 ?         ; < For each bit n 0..7, 1 = DIMM n is OnDimmMirror capable
  DimmSpdCse                    UINT8 ?         ; < For each bit n 0..7, 1 = DIMM n SPD checksum error
  DimmExclude               UINT8 ?         ; < For each bit n 0..7, 1 = DIMM n gets excluded
  DimmYr06                      UINT8 ?         ; < Bitmap indicating which Dimms have a manufacturer's year code <= 2006
  DimmWk2406                    UINT8 ?         ; < Bitmap indicating which Dimms have a manufacturer's week code <= 24 of 2006 (June)
  DimmPlPresent                 UINT8 ?         ; < Bitmap indicating that Planar (1) or Stacked (0) Dimms are present.
  DimmQrPresent                 UINT8 ?         ; < QuadRank DIMM present?
  DimmDrPresent                 UINT8 ?         ; < Bitmap indicating that Dual Rank Dimms are present
  DimmSRPresent                 UINT8 ?         ; < Bitmap indicating that Single Rank Dimms are present
  Dimmx4Present                 UINT8 ?         ; < For each bit n 0..7, 1 = DIMM n contains x4 data devices.
  Dimmx8Present                 UINT8 ?         ; < For each bit n 0..7, 1 = DIMM n contains x8 data devices.
  Dimmx16Present                UINT8 ?         ; < For each bit n 0..7, 1 = DIMM n contains x16 data devices.

  MemClkDisMap                  POINTER ?       ; <  This pointer will be set to point to an array that describes
                                  ; <  the routing of M[B,A]_CLK pins to the DIMMs' ranks. AGESA will
                                  ; <  base on this array to disable unused MemClk to save power.
                                  ; <
                                  ; <  The array must have 8 entries. Each entry, which associates with
                                  ; <  one MemClkDis bit, is a bitmap of 8 CS that that MemClk is routed to.
                                  ; <    Example:
                                  ; <    BKDG definition of Fn2x88[MemClkDis] bitmap for AM3 package
                                  ; <    is like below:
                                  ; <         Bit AM3/S1g3 pin name
                                  ; <         0   M[B,A]_CLK_H/L[0]
                                  ; <         1   M[B,A]_CLK_H/L[1]
                                  ; <         2   M[B,A]_CLK_H/L[2]
                                  ; <         3   M[B,A]_CLK_H/L[3]
                                  ; <         4   M[B,A]_CLK_H/L[4]
                                  ; <         5   M[B,A]_CLK_H/L[5]
                                  ; <         6   M[B,A]_CLK_H/L[6]
                                  ; <         7   M[B,A]_CLK_H/L[7]
                                  ; <    And platform has the following routing:
                                  ; <         CS0   M[B,A]_CLK_H/L[4]
                                  ; <         CS1   M[B,A]_CLK_H/L[2]
                                  ; <         CS2   M[B,A]_CLK_H/L[3]
                                  ; <         CS3   M[B,A]_CLK_H/L[5]
                                  ; <    Then MemClkDisMap should be pointed to the following array:
                                  ; <               CLK_2 CLK_3 CLK_4 CLK_5
                                  ; <    0x00, 0x00, 0x02, 0x04, 0x01, 0x08, 0x00, 0x00
                                  ; <  Each entry of the array is the bitmask of 8 chip selects.

  CKETriMap                     POINTER ?       ; <  This pointer will be set to point to an array that describes
                                  ; <  the routing of CKE pins to the DIMMs' ranks.
                                  ; <  The array must have 2 entries. Each entry, which associates with
                                  ; <  one CKE pin, is a bitmap of 8 CS that that CKE is routed to.
                                  ; <  AGESA will base on this array to disable unused CKE pins to save power.

  ODTTriMap                     POINTER ?       ; <  This pointer will be set to point to an array that describes
                                  ; <  the routing of ODT pins to the DIMMs' ranks.
                                  ; <  The array must have 4 entries. Each entry, which associates with
                                  ; <  one ODT pin, is a bitmap of 8 CS that that ODT is routed to.
                                  ; <  AGESA will base on this array to disable unused ODT pins to save power.

  ChipSelTriMap                 POINTER ?       ; <  This pointer will be set to point to an array that describes
                                  ; <  the routing of chip select pins to the DIMMs' ranks.
                                  ; <  The array must have 8 entries. Each entry is a bitmap of 8 CS.
                                  ; <  AGESA will base on this array to disable unused Chip select pins to save power.

  ExtendTmp                     BOOLEAN ?       ; <  If extended temperature is supported on all dimms on a channel.

  Reserved                      UINT8 (100) DUP (?)     ; < Reserved
CH_DEF_STRUCT    ENDS

; ===============================================================================
 ; DCT Channel Timing Parameters
 ; This data structure sets timings that are specific to the channel
; ===============================================================================
CH_TIMING_STRUCT    STRUCT
  DctDimmValid                  UINT16 ?        ; < For each bit n 0..7, 1=DIMM n is valid and is/will be configured
  DimmMirrorPresent             UINT16 ?        ; < For each bit n 0..7, 1=DIMM n is OnDimmMirror capable
  DimmSpdCse                    UINT16 ?        ; < For each bit n 0..7, 1=DIMM n SPD checksum error
  DimmExclude               UINT16 ?        ; < For each bit n 0..7, 1 = DIMM n gets excluded because of no common voltage is found
  CsPresent                     UINT16 ?        ; < For each bit n 0..7, 1=Chip-select n is present
  CsEnabled                     UINT16 ?        ; < For each bit n 0..7, 1=Chip-select n is enabled
  CsTestFail                    UINT16 ?        ; < For each bit n 0..7, 1=Chip-select n is present but disabled
  CsTrainFail                   UINT16 ?        ; < Bitmap showing which chipselects failed training
  DIMM1KPage                    UINT16 ?        ; < For each bit n 0..7, 1=DIMM n contains 1K page devices.
  DimmQrPresent                 UINT16 ?        ; < QuadRank DIMM present?
  DimmDrPresent                 UINT16 ?        ; < Bitmap indicating that Dual Rank Dimms are present
  DimmSRPresent                 UINT8 ?         ; < Bitmap indicating that Single Rank Dimms are present
  Dimmx4Present                 UINT16 ?        ; < For each bit n 0..7, 1=DIMM n contains x4 data devices.
  Dimmx8Present                 UINT16 ?        ; < For each bit n 0..7, 1=DIMM n contains x8 data devices.
  Dimmx16Present                UINT16 ?        ; < For each bit n 0..7, 1=DIMM n contains x16 data devices.

  DIMMTrcd                      UINT16 ?        ; < Minimax Trcd*40 (ns) of DIMMs
  DIMMTrp                       UINT16 ?        ; < Minimax Trp*40 (ns) of DIMMs
  DIMMTrtp                      UINT16 ?        ; < Minimax Trtp*40 (ns) of DIMMs
  DIMMTras                      UINT16 ?        ; < Minimax Tras*40 (ns) of DIMMs
  DIMMTrc                       UINT16 ?        ; < Minimax Trc*40 (ns) of DIMMs
  DIMMTwr                       UINT16 ?        ; < Minimax Twr*40 (ns) of DIMMs
  DIMMTrrd                      UINT16 ?        ; < Minimax Trrd*40 (ns) of DIMMs
  DIMMTwtr                      UINT16 ?        ; < Minimax Twtr*40 (ns) of DIMMs
  DIMMTfaw                      UINT16 ?        ; < Minimax Tfaw*40 (ns) of DIMMs
  TargetSpeed                   UINT16 ?        ; < Target DRAM bus speed in MHz
  Speed                         UINT16 ?        ; < DRAM bus speed in MHz
                                  ; <  400 (MHz)
                                  ; <  533 (MHz)
                                  ; <  667 (MHz)
                                  ; <  800 (MHz)
                                  ; <  and so on...
  CasL                          UINT8 ?         ; < CAS latency DCT setting (busclocks)
  Trcd                          UINT8 ?         ; < DCT Trcd (busclocks)
  Trp                           UINT8 ?         ; < DCT Trp (busclocks)
  Trtp                          UINT8 ?         ; < DCT Trtp (busclocks)
  Tras                          UINT8 ?         ; < DCT Tras (busclocks)
  Trc                           UINT8 ?         ; < DCT Trc (busclocks)
  Twr                           UINT8 ?         ; < DCT Twr (busclocks)
  Trrd                          UINT8 ?         ; < DCT Trrd (busclocks)
  Twtr                          UINT8 ?         ; < DCT Twtr (busclocks)
  Tfaw                          UINT8 ?         ; < DCT Tfaw (busclocks)
  Trfc0                         UINT8 ?         ; < DCT Logical DIMM0 Trfc
                                  ; <  0 = 75ns (for 256Mb devs)
                                  ; <  1 = 105ns (for 512Mb devs)
                                  ; <  2 = 127.5ns (for 1Gb devs)
                                  ; < 3 = 195ns (for 2Gb devs)
                                  ; <  4 = 327.5ns (for 4Gb devs)
  Trfc1                         UINT8 ?         ; < DCT Logical DIMM1 Trfc (see Trfc0 for format)
  Trfc2                         UINT8 ?         ; < DCT Logical DIMM2 Trfc (see Trfc0 for format)
  Trfc3                         UINT8 ?         ; < DCT Logical DIMM3 Trfc (see Trfc0 for format)
  DctMemSize                    UINT32 ?        ; < Base[47:16], total DRAM size controlled by this DCT.
  SlowMode                      BOOLEAN ?       ; < 1T or 2T CMD mode (slow access mode)
                                  ; < FALSE = 1T
                                  ; < TRUE = 2T
  TrwtTO                        UINT8 ?         ; < DCT TrwtTO (busclocks)
  Twrrd                         UINT8 ?         ; < DCT Twrrd (busclocks)
  Twrwr                         UINT8 ?         ; < DCT Twrwr (busclocks)
  Trdrd                         UINT8 ?         ; < DCT Trdrd (busclocks)
  TrwtWB                        UINT8 ?         ; < DCT TrwtWB (busclocks)
  TrdrdSD                       UINT8 ?         ; < DCT TrdrdSD (busclocks)
  TwrwrSD                       UINT8 ?         ; < DCT TwrwrSD (busclocks)
  TwrrdSD                       UINT8 ?         ; < DCT TwrrdSD (busclocks)
  MaxRdLat                      UINT16 ?        ; < Max Read Latency
  WrDatGrossH                   UINT8 ?         ; < Temporary variables must be removed
  DqsRcvEnGrossL                UINT8 ?         ; < Temporary variables must be removed
CH_TIMING_STRUCT    ENDS

; ===============================================================================
 ; Data for each DCT
 ; This data structure defines data used to configure each DRAM controller
; ===============================================================================
DCT_STRUCT    STRUCT
  Dct                           UINT8 ?         ; < Current Dct
  Timings                       CH_TIMING_STRUCT {}     ; < Channel Timing structure
  ChData                        POINTER ?       ; < Pointed to a dynamically allocated array of Channel structures
  ChannelCount                  UINT8 ?         ; < Number of channel per this DCT
DCT_STRUCT    ENDS


; ===============================================================================
 ; Data Structure defining each Die
 ; This data structure contains information that is used to configure each Die
; ===============================================================================
DIE_STRUCT    STRUCT

   ; Advanced:

  NodeId                        UINT8 ?         ; < Node ID of current controller
  SocketId                      UINT8 ?         ; < Socket ID of this Die
  DieId                         UINT8 ?         ; < ID of this die relative to the socket
  PciAddr                       PCI_ADDR {}      ; < Pci bus and device number of this controller.
  ErrCode                       AGESA_STATUS ?          ; < Current error condition of Node
                                   ; <  0x0 = AGESA_SUCCESS
                                   ; <  0x1 = AGESA_UNSUPPORTED
                                   ; <  0x2 = AGESA_BOUNDS_CHK
                                   ; <  0x3 = AGESA_ALERT
                                   ; <  0x4 = AGESA_WARNING
                                   ; <  0x5 = AGESA_ERROR
                                   ; <  0x6 = AGESA_CRITICAL
                                   ; <  0x7 = AGESA_FATAL
                                   ; <
  ErrStatus                     BOOLEAN (EsbEOL) DUP (?)        ; < Error Status bit Field
  Status                        BOOLEAN (SbEOL) DUP (?)         ; < Status bit Field
  NodeMemSize                   UINT32 ?        ; < Base[47:16], total DRAM size controlled by both DCT0 and DCT1 of this Node.
  NodeSysBase                   UINT32 ?        ; < Base[47:16] (system address) DRAM base address of this Node.
  NodeHoleBase                  UINT32 ?        ; < If not zero, Base[47:16] (system address) of dram hole for HW remapping.  Dram hole exists on this Node
  NodeSysLimit                  UINT32 ?        ; < Base[47:16] (system address) DRAM limit address of this Node.
  DimmPresent                   UINT32 ?        ; < For each bit n 0..7, 1 = DIMM n is present.
                                   ; <   DIMM#  Select Signal
                                   ; <   0      MA0_CS_L[0, 1]
                                   ; <   1      MB0_CS_L[0, 1]
                                   ; <   2      MA1_CS_L[0, 1]
                                   ; <   3      MB1_CS_L[0, 1]
                                   ; <   4      MA2_CS_L[0, 1]
                                   ; <   5      MB2_CS_L[0, 1]
                                   ; <   6      MA3_CS_L[0, 1]
                                   ; <   7      MB3_CS_L[0, 1]
  DimmValid                     UINT32 ?        ; < For each bit n 0..7, 1 = DIMM n is valid and is / will be configured
  RegDimmPresent                UINT32 ?        ; < For each bit n 0..7, 1 = DIMM n is registered DIMM
  DimmEccPresent                UINT32 ?        ; < For each bit n 0..7, 1 = DIMM n is ECC capable.
  DimmParPresent                UINT32 ?        ; < For each bit n 0..7, 1 = DIMM n is ADR/CMD Parity capable.
  DimmTrainFail                 UINT16 ?        ; < Bitmap showing which dimms failed training
  ChannelTrainFail              UINT16 ?        ; < Bitmap showing the channel information about failed Chip Selects
                                   ; <  0 in any bit field indicates Channel 0
                                   ; <  1 in any bit field indicates Channel 1
  Dct                           UINT8 ?         ; <  Need to be removed
                                   ; <  DCT pointer
  GangedMode                    BOOLEAN ?       ; < Ganged mode
                                   ; <  0 = disabled
                                   ; <  1 = enabled
  LogicalCpuid                  CPU_LOGICAL_ID {}       ; < The logical CPUID of the node
  HostBiosSrvc1                 UINT16 ?        ; < UINT16 sized general purpose field for use by host BIOS.  Scratch space.
  HostBiosSrvc2                 UINT32 ?        ; < UINT32 sized general purpose field for use by host BIOS.  Scratch space.
  MLoad                         UINT8 ?         ; < Need to be removed
                                   ; < Number of devices loading MAA bus
  MaxAsyncLat                   UINT8 ?         ; < Legacy wrapper
  ChbD3Rcvrdly                  UINT8 ?         ; < Legacy wrapper
  ChaMaxRdLat                   UINT16 ?        ; < Max Read Latency (ns) for DCT 0
  ChbD3BcRcvrdly                UINT8 ?         ; < CHB DIMM 3 Check UINT8 Receiver Enable Delay

  DctData                       POINTER ?       ; < Pointed to a dynamically allocated array of DCT_STRUCTs
  DctCount                      UINT8 ?         ; < Number of DCTs per this Die
  Reserved                      UINT8 (16) DUP (?)      ; < Reserved
DIE_STRUCT    ENDS

; *********************************************************************
; * S3 Support structure
; *********************************************************************
 ; AmdInitResume, AmdS3LateRestore, and AmdS3Save param structure
AMD_S3_PARAMS    STRUCT
     Signature                  UINT32 ?        ; < "ASTR" for AMD Suspend-To-RAM
     Version                    UINT16 ?        ; < S3 Params version number
  Flags                         UINT32 ?        ; < Indicates operation
  NvStorage                     POINTER ?       ; < Pointer to memory critical save state data
  NvStorageSize                 UINT32 ?        ; < Size in bytes of the NvStorage region
  VolatileStorage               POINTER ?       ; < Pointer to remaining AMD save state data
  VolatileStorageSize           UINT32 ?        ; < Size in bytes of the VolatileStorage region
AMD_S3_PARAMS    ENDS

; ===============================================================================
 ; MEM_PARAMETER_STRUCT
 ; This data structure is used to pass wrapper parameters to the memory configuration code
; ===============================================================================
MEM_PARAMETER_STRUCT    STRUCT

   ; Basic (Return parameters)
   ; (This section contains the outbound parameters from the memory init code)

  GStatus                       BOOLEAN (GsbEOL) DUP (?)        ; < Global Status bitfield
  HoleBase                      UINT32 ?        ; < If not zero Base[47:16] (system address) of sub 4GB dram hole for HW remapping.
  Sub4GCacheTop                 UINT32 ?        ; < If not zero, the 32-bit top of cacheable memory.
  Sub1THoleBase                 UINT32 ?        ; < If not zero Base[47:16] (system address) of sub 1TB dram hole.
  SysLimit                      UINT32 ?        ; < Limit[47:16] (system address)
  DDR3Voltage                   DIMM_VOLTAGE ?    ; < Find support voltage and send back to platform BIOS.
                                  ; < 0 = 1.5v
                                  ; < 1 = 1.35v
                                  ; < 2 = 1.2v
                                  ; < 0xFF = Mixed 1.5V and 1.2V in the system. 1.5V dimms get excluded
                                  ; <        from the system.
                                  ; <

  MemData                       POINTER ?       ; < Pointer to MEM_DATA_STRUCT
    ; Advanced (Optional parameters)
    ; Optional (all defaults values will be initialized by the
    ; 'AmdMemInitDataStructDef' based on AMD defaults. It is up
    ; to the IBV/OEM to change the defaults after initialization
    ; but prior to the main entry to the memory code):

   ; Memory Map/Mgt.

  BottomIo                      UINT16 ?        ; < Bottom of 32-bit IO space (8-bits)
                                   ; <   NV_BOTTOM_IO[7:0]=Addr[31:24]
  MemHoleRemapping              BOOLEAN ?       ; < Memory Hole Remapping (1-bit)
                                   ; <  FALSE = disable
                                   ; <  TRUE  = enable
  LimitMemoryToBelow1Tb         BOOLEAN ?       ; < Limit memory address space to below 1 TB
                                                ; <  FALSE = disable
                                                ; <  TRUE  = enable
   ; Dram Timing

  UserTimingMode                USER_MEMORY_TIMING_MODE ?       ; < User Memclock Mode

  MemClockValue                 MEMORY_BUS_SPEED ?      ; < Memory Clock Value

   ; Dram Configuration

  EnableBankIntlv               BOOLEAN ?       ; < Dram Bank (chip-select) Interleaving (1-bit)
                                   ; <   FALSE =disable (AMD default)
                                   ; <   TRUE =enable
  EnableNodeIntlv               BOOLEAN ?       ; < Node Memory Interleaving (1-bit)
                                   ; <   FALSE = disable (AMD default)
                                   ; <   TRUE = enable
  EnableChannelIntlv            BOOLEAN ?       ; < Channel Interleaving (1-bit)
                                   ; <   FALSE = disable (AMD default)
                                   ; <   TRUE = enable
   ; ECC

  EnableEccFeature              BOOLEAN ?       ; < enable ECC error to go into MCE
                                   ; <   FALSE = disable (AMD default)
                                   ; <   TRUE = enable
   ; Dram Power

  EnablePowerDown               BOOLEAN ?       ; < CKE based power down mode (1-bit)
                                   ; <   FALSE =disable (AMD default)
                                   ; <   TRUE =enable
   ; Online Spare

  EnableOnLineSpareCtl          BOOLEAN ?       ; < Chip Select Spare Control bit 0:
                                   ; <  FALSE = disable Spare (AMD default)
                                   ; <  TRUE = enable Spare
  TableBasedAlterations         POINTER ?       ; < Point to an array of data bytes describing desired modifications to register settings.

  PlatformMemoryConfiguration           POINTER ?
                                   ; < Points to a table that contains platform specific settings
                                   ; < (i.e. MemClk routing, the number of DIMM slots per channel,...)
                                   ; < AGESA initializes this pointer with DefaultPlatformMemoryConfiguration that
                                   ; < contains default conservative settings. Platform BIOS can either tweak
                                   ; < DefaultPlatformMemoryConfiguration or reassign this pointer to its own table.
                                   ; <
  EnableParity                  BOOLEAN ?       ; < Parity control
                                   ; <  TRUE = enable
                                   ; <  FALSE = disable (AMD default)
  EnableBankSwizzle             BOOLEAN ?       ; < BankSwizzle control
                                   ; <  FALSE = disable
                                   ; <  TRUE = enable  (AMD default)
  EnableMemClr                  BOOLEAN ?       ; < Memory Clear functionality control
                                   ; <  FALSE = disable
                                   ; <  TRUE = enable  (AMD default)
  ; Uma Configuration

  UmaMode                       UMA_MODE ?       ; < Uma Mode
                                   ; <  0 = None
                                   ; <  1 = Specified
                                   ; <  2 = Auto
  UmaSize                       UINT32 ?       ; < The size of shared graphics dram (16-bits)
                                   ; <  NV_UMA_Size[31:0]=Addr[47:16]
                                   ; <
  UmaBase                       UINT32 ?       ; < The allocated Uma base address (32-bits)
                                   ; <  NV_UMA_Base[31:0]=Addr[47:16]
                                   ; <

   ; Memory Restore Feature

  MemRestoreCtl                 BOOLEAN ?       ; < Memory context restore control
                                   ; <   FALSE = perform memory init as normal (AMD default)
                                   ; <   TRUE = restore memory context and skip training. This requires
                                   ; <          MemContext is valid before AmdInitPost
  SaveMemContextCtl             BOOLEAN ?       ; < Control switch to save memory context at the end of MemAuto
                                   ; <   TRUE = AGESA will setup MemContext block before exit AmdInitPost
                                   ; <   FALSE = AGESA will not setup MemContext block. Platform is
                                   ; <           expected to call S3Save later in POST if it wants to
                                   ; <           use memory context restore feature.
  MemContext                    AMD_S3_PARAMS {}        ; < Memory context block describes the data that platform needs to
                                   ; < save and restore for memory context restore feature to work.
                                   ; < It uses the subset of S3Save block to save/restore. Hence platform
                                   ; < may save only S3 block and uses it for both S3 resume and
                                   ; < memory context restore.
                                   ; <  - If MemRestoreCtl is TRUE, platform needs to pass in MemContext
                                   ; <    before AmdInitPost.
                                   ; <  - If SaveMemContextCtl is TRUE, platform needs to save MemContext
                                   ; <    right after AmdInitPost.
MEM_PARAMETER_STRUCT    ENDS


; ===============================================================================
 ; Function definition
 ; This data structure passes function pointers to the memory configuration code.
 ; The wrapper can use this structure with customized versions
; ================================================================================
MEM_FUNCTION_STRUCT    STRUCT

   ; PUBLIC required Internal functions

  amdMemGetPsCfgU       POINTER ?       ; < Proc for Unbuffered DIMMs, platform specific
  amdMemGetPsCfgR       POINTER ?       ; < Proc for Registered DIMMs, platform specific

   ; PUBLIC optional functions

  amdMemEccInit         POINTER ?       ; < NB proc for ECC feature
  amdMemChipSelectInterleaveInit        POINTER ?       ; < NB proc for CS interleave feature
  amdMemDctInterleavingInit     POINTER ?       ; < NB proc for Channel interleave feature
  amdMemMctInterleavingInit     POINTER ?       ; < NB proc for Node interleave feature
  amdMemParallelTraining        POINTER ?       ; < NB proc for parallel training feature
  amdMemEarlySampleSupport      POINTER ?       ; < NB code for early sample support feature
  amdMemMultiPartInitSupport    POINTER ?       ; < NB code for 'multi-part'
  amdMemOnlineSpareSupport      POINTER ?       ; < NB code for On-Line Spare feature
  amdMemUDimmInit       POINTER ?       ; < NB code for UDIMMs
  amdMemRDimmInit       POINTER ?       ; < NB code for RDIMMs

  Reserved                      UINT32 (100) DUP (?)    ; < Reserved for later function definition
MEM_FUNCTION_STRUCT    ENDS

; ===============================================================================
 ; Socket Structure

; ===============================================================================
MEM_SOCKET_STRUCT    STRUCT
  ChannelPtr                    POINTER (MAX_CHANNELS_PER_SOCKET) DUP (?)       ; < Pointers to each channels training data

  TimingsPtr                    POINTER (MAX_CHANNELS_PER_SOCKET) DUP (?)       ; < Pointers to each channels timing data

MEM_SOCKET_STRUCT    ENDS

; ===============================================================================
 ; MEM_DATA_STRUCT
; ===============================================================================
MEM_DATA_STRUCT    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < AGESA Standard Header

  ParameterListPtr              POINTER ?       ; < List of input Parameters

  FunctionList                  MEM_FUNCTION_STRUCT {}          ; < List of function Pointers

  GetPlatformCfg        POINTER (MAX_PLATFORM_TYPES) DUP (?)    ; < look-up platform info

  ErrorHandling         POINTER ?     ; < Error Handling

   ; SocketList is a shortcut for IBVs to retrieve training
   ; and timing data for each channel indexed by socket/channel,
   ; eliminating their need to parse die/dct/channel etc.
   ; It contains pointers to the populated data structures for
   ; each channel and skips the channel structures that are
   ; unpopulated. In the case of channels sharing the same DCT,
   ; the pTimings pointers will point to the same DCT Timing data.

  SocketList                    MEM_SOCKET_STRUCT (MAX_SOCKETS_SUPPORTED) DUP ({})      ; < Socket list for memory code

  DiesPerSystem                 POINTER ?       ; < Pointed to an array of DIE_STRUCTs
  DieCount                      UINT8 ?         ; < Number of MCTs in the system.

  SpdDataStructure              POINTER ?       ; < Pointer to SPD Data structure

  PlatFormConfig              POINTER ?       ; < Pointer to Platform profile/build option config structure
MEM_DATA_STRUCT    ENDS

; ===============================================================================
; UMA_INFO_STRUCT
; ===============================================================================
UMA_INFO      STRUCT
  UmaBase         UINT64 ?        ; < UmaBase[63:0] = Addr[63:0]
  UmaSize         UINT32 ?        ; < UmaSize[31:0] = Addr[31:0]
  UmaAttributes   UINT32 ?        ; < Indicate the attribute of Uma
  UmaMode         UINT8 ?         ; < Indicate the mode of Uma
  MemClock        UINT16 ?        ; < Indicate memory running speed in MHz
  Reserved        UINT8 (3) DUP (?)         ; < Reserved for future usage
UMA_INFO      ENDS

  ; AGESA MEMORY ERRORS

 ; AGESA_ALERT Memory Errors
MEM_ALERT_USER_TMG_MODE_OVERRULED   EQU    04010000h     ; < TIMING_MODE_SPECIFIC is requested but
                                                       ; < cannot be applied to current configurations.
MEM_ALERT_ORG_MISMATCH_DIMM         EQU    04010100h     ; < DIMM organization miss-match
MEM_ALERT_BK_INT_DIS                EQU    04010200h     ; < Bank interleaving disable for internal issue

 ; AGESA_ERROR Memory Errors
MEM_ERROR_NO_DQS_POS_RD_WINDOW            EQU   04010300h     ; < No DQS Position window for RD DQS
MEM_ERROR_SMALL_DQS_POS_RD_WINDOW         EQU   04020300h     ; < Small DQS Position window for RD DQS
MEM_ERROR_NO_DQS_POS_WR_WINDOW            EQU   04030300h     ; < No DQS Position window for WR DQS
MEM_ERROR_SMALL_DQS_POS_WR_WINDOW         EQU   04040300h     ; < Small DQS Position window for WR DQS
MEM_ERROR_ECC_DIS                         EQU   04010400h     ; < ECC has been disabled as a result of an internal issue
MEM_ERROR_DIMM_SPARING_NOT_ENABLED        EQU   04010500h     ; < DIMM sparing has not been enabled for an internal issues
MEM_ERROR_RCVR_EN_VALUE_TOO_LARGE         EQU   04050300h     ; < Receive Enable value is too large
MEM_ERROR_RCVR_EN_NO_PASSING_WINDOW       EQU   04060300h     ; < There is no DQS receiver enable window
MEM_ERROR_DRAM_ENABLED_TIME_OUT           EQU   04010600h     ; < Time out when polling DramEnabled bit
MEM_ERROR_DCT_ACCESS_DONE_TIME_OUT        EQU   04010700h     ; < Time out when polling DctAccessDone bit
MEM_ERROR_SEND_CTRL_WORD_TIME_OUT         EQU   04010800h     ; < Time out when polling SendCtrlWord bit
MEM_ERROR_PREF_DRAM_TRAIN_MODE_TIME_OUT   EQU   04010900h     ; < Time out when polling PrefDramTrainMode bit
MEM_ERROR_ENTER_SELF_REF_TIME_OUT         EQU   04010A00h     ; < Time out when polling EnterSelfRef bit
MEM_ERROR_FREQ_CHG_IN_PROG_TIME_OUT       EQU   04010B00h     ; < Time out when polling FreqChgInProg bit
MEM_ERROR_EXIT_SELF_REF_TIME_OUT          EQU   04020A00h     ; < Time out when polling ExitSelfRef bit
MEM_ERROR_SEND_MRS_CMD_TIME_OUT           EQU   04010C00h     ; < Time out when polling SendMrsCmd bit
MEM_ERROR_SEND_ZQ_CMD_TIME_OUT            EQU   04010D00h     ; < Time out when polling SendZQCmd bit
MEM_ERROR_DCT_EXTRA_ACCESS_DONE_TIME_OUT  EQU   04010E00h     ; < Time out when polling DctExtraAccessDone bit
MEM_ERROR_MEM_CLR_BUSY_TIME_OUT           EQU   04010F00h     ; < Time out when polling MemClrBusy bit
MEM_ERROR_MEM_CLEARED_TIME_OUT            EQU   04020F00h     ; < Time out when polling MemCleared bit
MEM_ERROR_FLUSH_WR_TIME_OUT               EQU   04011000h     ; < Time out when polling FlushWr bit
MEM_ERROR_MAX_LAT_NO_WINDOW               EQU   04070300h     ; < Fail to find pass during Max Rd Latency training
MEM_ERROR_PARALLEL_TRAINING_LAUNCH_FAIL   EQU   04080300h     ; < Fail to launch training code on an AP
MEM_ERROR_PARALLEL_TRAINING_TIME_OUT      EQU   04090300h     ; < Fail to finish parallel training
MEM_ERROR_NO_ADDRESS_MAPPING              EQU   04011100h     ; < No address mapping found for a dimm
MEM_ERROR_RCVR_EN_NO_PASSING_WINDOW_EQUAL_LIMIT  EQU   040A0300h ; < There is no DQS receiver enable window and the value is equal to the largest value
MEM_ERROR_RCVR_EN_VALUE_TOO_LARGE_LIMIT_LESS_ONE EQU   040B0300h ; < Receive Enable value is too large and is 1 less than limit
MEM_ERROR_CHECKSUM_NV_SPDCHK_RESTRT_ERROR     EQU    04011200h    ; < SPD Checksum error for NV_SPDCHK_RESTRT
MEM_ERROR_NO_CHIPSELECT     EQU    04011300h                  ; < No chipselects found
MEM_ERROR_UNSUPPORTED_333MHZ_UDIMM   EQU   04011500h          ; < Unbuffered dimm is not supported at 333MHz

 ; AGESA_WARNING Memory Errors
    MEM_WARNING_UNSUPPORTED_QRDIMM      EQU    04011600h     ; < QR DIMMs detected but not supported
    MEM_WARNING_UNSUPPORTED_UDIMM       EQU    04021600h     ; < U DIMMs detected but not supported
    MEM_WARNING_UNSUPPORTED_SODIMM      EQU    04031600h     ; < SO-DIMMs detected but not supported
    MEM_WARNING_UNSUPPORTED_X4DIMM      EQU    04041600h     ; < x4 DIMMs detected but not supported
    MEM_WARNING_UNSUPPORTED_RDIMM       EQU    04051600h     ; < R DIMMs detected but not supported
    MEM_WARNING_EMP_NOT_SUPPORTED       EQU    04011700h     ; < Processor is not capable for EMP
    MEM_WARNING_EMP_CONFLICT    EQU    04021700h      ; < EMP cannot be enabled if channel interleaving,
                                                        ; < bank interleaving, or bank swizzle is enabled.
    MEM_WARNING_EMP_NOT_ENABLED EQU    04031700h      ; < Memory size is not power of two.
    MEM_WARNING_PERFORMANCE_ENABLED_BATTERY_LIFE_PREFERRED EQU 04011800h  ; < Performance has been enabled, but battery life is preferred.
    MEM_WARNING_NO_SPDTRC_FOUND EQU    04011900h      ; < No Trc timing value found in SPD of a dimm.
    MEM_WARNING_NODE_INTERLEAVING_NOT_ENABLED     EQU     04012000h    ; < Node Interleaveing Requested, but could not be enabled
    MEM_WARNING_CHANNEL_INTERLEAVING_NOT_ENABLED  EQU     04012100h    ; < Channel Interleaveing Requested, but could not be enabled
    MEM_WARNING_BANK_INTERLEAVING_NOT_ENABLED     EQU     04012200h    ; < Bank Interleaveing Requested, but could not be enabled
    MEM_WARNING_VOLTAGE_1_35_NOT_SUPPORTED     EQU     04012300h    ; < Voltage 1.35 determined, but could not be supported

 ; AGESA_FATAL Memory Errors
    MEM_ERROR_MINIMUM_MODE      EQU    04011A00h                   ; < Running in minimum mode
    MEM_ERROR_MODULE_TYPE_MISMATCH_DIMM EQU    04011B00h      ; < DIMM modules are miss-matched
    MEM_ERROR_NO_DIMM_FOUND_ON_SYSTEM    EQU    04011C00h     ; < No DIMMs have been found on system
    MEM_ERROR_MISMATCH_DIMM_CLOCKS      EQU    04011D00h          ; < DIMM clocks miss-matched
    MEM_ERROR_NO_CYC_TIME       EQU    04011E00h                    ; < No cycle time found
    MEM_ERROR_HEAP_ALLOCATE_DYN_STORING_OF_TRAINED_TIMINGS   EQU    04011F00h ; < Heap allocation error with dynamic storing of trained timings
    MEM_ERROR_HEAP_ALLOCATE_FOR_DCT_STRUCT_AND_CH_DEF_STRUCTs   EQU    04021F00h ; < Heap allocation error for DCT_STRUCT and CH_DEF_STRUCT
    MEM_ERROR_HEAP_ALLOCATE_FOR_REMOTE_TRAINING_ENV   EQU    04031F00h ; < Heap allocation error with REMOTE_TRAINING_ENV
    MEM_ERROR_HEAP_ALLOCATE_FOR_SPD     EQU    04041F00h    ; < Heap allocation error for SPD data
    MEM_ERROR_HEAP_ALLOCATE_FOR_RECEIVED_DATA     EQU    04051F00h    ; < Heap allocation error for RECEIVED_DATA during parallel training
    MEM_ERROR_HEAP_ALLOCATE_FOR_S3_SPECIAL_CASE_REGISTERS     EQU    04061F00h    ; < Heap allocation error for S3 "SPECIAL_CASE_REGISTER"
    MEM_ERROR_HEAP_ALLOCATE_FOR_TRAINING_DATA     EQU    04071F00h    ; < Heap allocation error for Training Data
    MEM_ERROR_HEAP_ALLOCATE_FOR_IDENTIFY_DIMM_MEM_NB_BLOCK     EQU    04081F00h   ; < Heap allocation error for  DIMM Identify "MEM_NB_BLOCK
    MEM_ERROR_NO_CONSTRUCTOR_FOR_IDENTIFY_DIMM    EQU    04022300h    ; < No Constructor for DIMM Identify
    MEM_ERROR_VDDIO_UNSUPPORTED   EQU   04022500h     ; < VDDIO of the dimms on the board is not supported


 ; AGESA_CRITICAL Memory Errors
    MEM_ERROR_HEAP_ALLOCATE_FOR_DMI_TABLE_DDR3     EQU    04091F00h    ; < Heap allocation error for DMI table for DDR3
    MEM_ERROR_HEAP_ALLOCATE_FOR_DMI_TABLE_DDR2     EQU    040A1F00h    ; < Heap allocation error for DMI table for DDR2
    MEM_ERROR_UNSUPPORTED_DIMM_CONFIG    EQU   04011400h          ; < Dimm population is not supported


; ----------------------------------------------------------------------------
; *
; *                END OF MEMORY-SPECIFIC DATA STRUCTURES
; *
; *----------------------------------------------------------------------------
;


; ----------------------------------------------------------------------------
; *
; *                    CPU RELATED DEFINITIONS
; *
; *----------------------------------------------------------------------------
;

; CPU Event definitions.

; Defines used to filter CPU events based on functional blocks
CPU_EVENT_PM_EVENT_MASK                         EQU   0FF00FF00h
CPU_EVENT_PM_EVENT_CLASS                        EQU   008000400h

;================================================================
; CPU General events
;    Heap allocation                    (AppFunction =     01h)
CPU_ERROR_HEAP_BUFFER_IS_NOT_PRESENT            EQU   008000100h
CPU_ERROR_HEAP_IS_ALREADY_INITIALIZED           EQU   008010100h
CPU_ERROR_HEAP_IS_FULL                          EQU   008020100h
CPU_ERROR_HEAP_BUFFER_HANDLE_IS_ALREADY_USED    EQU   008030100h
CPU_ERROR_HEAP_BUFFER_HANDLE_IS_NOT_PRESENT     EQU   008040100h
;    BrandId                            (AppFunction =     02h)
CPU_ERROR_BRANDID_HEAP_NOT_AVAILABLE            EQU   008000200h
;    Micro code patch                   (AppFunction =     03h)
CPU_ERROR_MICRO_CODE_PATCH_IS_NOT_LOADED        EQU   008000300h
;    Power management                   (AppFunction =     04h)
CPU_EVENT_PM_PSTATE_OVERCURRENT                 EQU   008000400h
CPU_EVENT_PM_ALL_PSTATE_OVERCURRENT             EQU   008010400h
CPU_ERROR_PSTATE_HEAP_NOT_AVAILABLE             EQU   008020400h
;   BIST                                (AppFunction =     05h)
CPU_EVENT_BIST_ERROR                            EQU   008000500h
;=================================================================
; CPU Feature events
;    Execution cache                    (AppFunction =     21h)
;        AGESA_CACHE_SIZE_REDUCED                          2101
;        AGESA_CACHE_REGIONS_ACROSS_1MB                    2102
;        AGESA_CACHE_REGIONS_ACROSS_4GB                    2103
;        AGESA_REGION_NOT_ALIGNED_ON_BOUNDARY              2104
;        AGESA_CACHE_START_ADDRESS_LESS_D0000              2105
;        AGESA_THREE_CACHE_REGIONS_ABOVE_1MB               2106
;        AGESA_DEALLOCATE_CACHE_REGIONS                    2107
CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR      EQU   008002100h
;    Core Leveling                      (AppFunction =     22h)
CPU_WARNING_ADJUSTED_LEVELING_MODE              EQU   008002200h
;    HT Assist                          (AppFunction =     23h)
CPU_WARNING_NONOPTIMAL_HT_ASSIST_CFG            EQU   008002300h

; CPU Build Configuration structures and definitions

; Build Configuration values for BLDGCFG_AP_MTRR_SETTINGS
AP_MTRR_SETTINGS  STRUCT
  MsrAddr       UINT32 ?  ; < Fixed-Sized MTRR address
  MsrData       UINT64 ?  ; < MTRR Settings
AP_MTRR_SETTINGS  ENDS

AMD_AP_MTRR_FIX64k_00000    EQU 000000250h
AMD_AP_MTRR_FIX16k_80000    EQU 000000258h
AMD_AP_MTRR_FIX16k_A0000    EQU 000000259h
AMD_AP_MTRR_FIX4k_C0000     EQU 000000268h
AMD_AP_MTRR_FIX4k_C8000     EQU 000000269h
AMD_AP_MTRR_FIX4k_D0000     EQU 00000026Ah
AMD_AP_MTRR_FIX4k_D8000     EQU 00000026Bh
AMD_AP_MTRR_FIX4k_E0000     EQU 00000026Ch
AMD_AP_MTRR_FIX4k_E8000     EQU 00000026Dh
AMD_AP_MTRR_FIX4k_F0000     EQU 00000026Eh
AMD_AP_MTRR_FIX4k_F8000     EQU 00000026Fh
CPU_LIST_TERMINAL           EQU 0FFFFFFFFh

; ***********************************************************************
; *
; *  AGESA interface Call-Out function parameter structures
; *
; **********************************************************************

 ; Parameters structure for interface call-out AgesaAllocateBuffer
AGESA_BUFFER_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Standard header
  BufferLength                  UINT32 ?        ; < Size of buffer to allocate
  BufferHandle                  UINT32 ?        ; < Identifier or name for the buffer
  BufferPointer                 POINTER ?       ; < location of the created buffer
AGESA_BUFFER_PARAMS    ENDS

 ; Parameters structure for interface call-out AgesaRunCodeOnAp
AP_EXE_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Standard header
  FunctionNumber                UINT32 ?        ; < Index of the procedure to execute
  RelatedDataBlock              POINTER ?       ; < Location of data structure the procedure will use
  RelatedBlockLength            UINT32 ?        ; < Size of the related data block
AP_EXE_PARAMS    ENDS

 ; Parameters structure for the interface call-out AgesaReadSpd & AgesaReadSpdRecovery
AGESA_READ_SPD_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < standard header
  SocketId                      UINT8 ?         ; < Address of SPD - socket ID
  MemChannelId                  UINT8 ?         ; < Address of SPD - memory channel ID
  DimmId                        UINT8 ?         ; < Address of SPD - DIMM ID
  Buffer                        POINTER ?       ; < Location where to place the SPD content
  MemData                       POINTER ?       ; < Location of the MemData structure, for reference
AGESA_READ_SPD_PARAMS    ENDS

 ; Buffer Handles
  AMD_DMI_INFO_BUFFER_HANDLE    EQU    000D000h ; < Assign 0x000D000 buffer handle to DMI function
  AMD_PSTATE_DATA_BUFFER_HANDLE EQU    000D001h ; < Assign 0x000D001 buffer handle to Pstate data
  AMD_PSTATE_ACPI_BUFFER_HANDLE EQU    000D002h ; < Assign 0x000D002 buffer handle to Pstate table
  AMD_BRAND_ID_BUFFER_HANDLE    EQU    000D003h ; < Assign 0x000D003 buffer handle to Brand ID
  AMD_ACPI_SLIT_BUFFER_HANDLE   EQU    000D004h ; < Assign 0x000D004 buffer handle to SLIT function
  AMD_SRAT_INFO_BUFFER_HANDLE   EQU    000D005h ; < Assign 0x000D005 buffer handle to SRAT function
  AMD_WHEA_BUFFER_HANDLE        EQU    000D006h ; < Assign 0x000D006 buffer handle to WHEA function
  AMD_S3_INFO_BUFFER_HANDLE     EQU    000D007h ; < Assign 0x000D007 buffer handle to S3 function
  AMD_S3_NB_INFO_BUFFER_HANDLE  EQU    000D008h ; < Assign 0x000D008 buffer handle to S3 NB device info
AMD_BUFFER_HANDLE  TEXTEQU  <DWORD>
; ***********************************************************************
; *
; *  AGESA interface Call-Out function prototypes
; *
; **********************************************************************

; ***********************************************************************
; *
; *  AGESA interface structure definition and function prototypes
; *
; **********************************************************************

; *********************************************************************
; * Platform Configuration:  The parameters in boot branch function
; *********************************************************************

;   The possible platform control flow settings.
  Nfcm                          EQU  0            ; < Normal Flow Control Mode.
  UmaDr                         EQU  1            ; < UMA using Display Refresh flow control.
  UmaIfcm                       EQU  2            ; < UMA using Isochronous Flow Control.
  Ifcm                          EQU  3            ; < Isochronous Flow Control Mode (other than for UMA).
  Iommu                         EQU  4            ; < An IOMMU is in use in the system.
  MaxControlFlow                EQU  5            ; < Not a control flow mode, use for limit checking.
PLATFORM_CONTROL_FLOW  TEXTEQU  <DWORD>

;   Platform Deemphasis Levels.
  DeemphasisLevelNone            EQU  0           ; < No Deemphasis.
  DeemphasisLevelMinus3          EQU  1           ; < Minus 3 db deemphasis.
  DeemphasisLevelMinus6          EQU  2           ; < Minus 6 db deemphasis.
  DeemphasisLevelMinus8          EQU  3           ; < Minus 8 db deemphasis.
  DeemphasisLevelMinus11         EQU  4           ; < Minus 11 db deemphasis.
  DeemphasisLevelMinus11pre8     EQU  5           ; < Minus 11, Minus 8 precursor db deemphasis.
  DcvLevelNone                   EQU  16          ; < No DCV Deemphasis.
  DcvLevelMinus2                 EQU  17          ; < Minus 2 db DCV deemphasis.
  DcvLevelMinus3                 EQU  18          ; < Minus 3 db DCV deemphasis.
  DcvLevelMinus5                 EQU  19          ; < Minus 5 db DCV deemphasis.
  DcvLevelMinus6                 EQU  20          ; < Minus 6 db DCV deemphasis.
  DcvLevelMinus7                 EQU  21          ; < Minus 7 db DCV deemphasis.
  DcvLevelMinus8                 EQU  22          ; < Minus 8 db DCV deemphasis.
  DcvLevelMinus9                 EQU  23          ; < Minus 9 db DCV deemphasis.
  DcvLevelMinus11                EQU  24          ; < Minus 11 db DCV deemphasis.
  MaxPlatformDeemphasisLevel     EQU  25          ; < Not a deemphasis level, use for limit checking.
PLATFORM_DEEMPHASIS_LEVEL  TEXTEQU  <DWORD>

;   Provide Deemphasis Levels for HT Links.
;
;   For each CPU to CPU or CPU to IO device HT link, the list of Deemphasis Levels will
;   be checked for a match. The item matches for a Socket, Link if the link frequency is
;   is in the inclusive range HighFreq:LoFreq.
;   AGESA does not set deemphasis in IO devices, only in processors.

CPU_HT_DEEMPHASIS_LEVEL STRUCT
  ; Match fields
  Socket                   UINT8  ?               ; < One Socket on which this Link is located
  Link                     UINT8  ?               ; < The Link on this Processor.
  LoFreq                   UINT8  ?               ; < If the link is set to this frequency or greater, apply these levels, and
  HighFreq                 UINT8  ?               ; < If the link is set to this frequency or less, apply these levels.
  ; Value fields
  ReceiverDeemphasis       PLATFORM_DEEMPHASIS_LEVEL ? ; < The deemphasis level for this link
  DcvDeemphasis            PLATFORM_DEEMPHASIS_LEVEL ? ; < The DCV, or far transmitter deemphasis level.
CPU_HT_DEEMPHASIS_LEVEL ENDS

;   The possible platform power policy settings.
  Performance                    EQU  0           ; < Optimize for performance.
  BatteryLife                    EQU  1           ; < Optimize for battery life.
  MaxPowerPolicy                 EQU  2           ; < Not a power policy mode, use for limit checking.
PLATFORM_POWER_POLICY  TEXTEQU  <DWORD>

; Platform performance settings for optimized settings.
; Several configuration settings for the processor depend upon other parts and
; general designer choices for the system. The determination of these data points
; is not standard for all platforms, so the host environment needs to provide these
; to specify how the system is to be configured.
PERFORMANCE_PROFILE    STRUCT
  PlatformControlFlowMode       PLATFORM_CONTROL_FLOW ?  ; < The platform's control flow mode for optimum platform performance.
  UseHtAssist                   BOOLEAN ?       ; < HyperTransport link traffic optimization.
  Use32ByteRefresh              BOOLEAN ?       ; < Display Refresh traffic generates 32 byte requests.
  UseVariableMctIsocPriority    BOOLEAN ?       ; < The Memory controller will be set to Variable Isoc Priority.
  PlatformPowerPolicy           PLATFORM_POWER_POLICY ?  ; < The platform's desired power policy
PERFORMANCE_PROFILE    ENDS

; Platform settings that describe the voltage regulator modules of the system.
; Many power management settings are dependent upon the characteristics of the
; on-board voltage regulator module (VRM).  The host environment needs to provide
; these to specify how the system is to be configured.
PLATFORM_VRM_CONFIGURATION   STRUCT
  CurrentLimit                  UINT32 ?         ; < Vrm Current Limit.
  LowPowerThreshold             UINT32 ?         ; < Vrm Low Power Threshold.
  SlewRate                      UINT32 ?         ; < Vrm Slew Rate.
  AdditionalDelay               UINT32 ?         ; < Vrm Additional Delay.
  HiSpeedEnable                 BOOLEAN ?        ; < Select high speed VRM.
  InrushCurrentLimit            UINT32 ?         ; < Vrm Inrush Current Limit.
PLATFORM_VRM_CONFIGURATION   ENDS

; Build Option/Configuration Boolean Structure
BUILD_OPT_CFG    STRUCT
  ; Build Option Area
  VersionString                 AMD_CODE_HEADER {} ; < AMD embedded code version string
  OptionUDimms                  BOOLEAN ?       ; < UDIMMS
  OptionRDimms                  BOOLEAN ?       ; < RDIMMS
  OptionEcc                     BOOLEAN ?       ; < ECC
  OptionBankInterleave          BOOLEAN ?       ; < BANK_INTERLEAVE
  OptionDctInterleave           BOOLEAN ?       ; < DCT_INTERLEAVE
  OptionNodeInterleave          BOOLEAN ?       ; < NODE_INTERLEAVE
  OptionParallelTraining        BOOLEAN ?       ; < PARALLEL_TRAINING
  OptionOnlineSpare             BOOLEAN ?       ; < ONLINE_SPARE
  OptionMemRestore              BOOLEAN ?       ; < MEM CONTEXT RESTORE
  OptionMultisocket             BOOLEAN ?       ; < MULTISOCKET
  OptionAcpiPstates             BOOLEAN ?       ; < ACPI_PSTATES
  OptionSrat                    BOOLEAN ?       ; < SRAT
  OptionSlit                    BOOLEAN ?       ; < SLIT
  OptionWhea                    BOOLEAN ?       ; < WHEA
  OptionDmi                     BOOLEAN ?       ; < DMI
  OptionEarlySamples            BOOLEAN ?       ; < EARLY_SAMPLES
  OptionAddrToCsTranslator      BOOLEAN ?       ; < ADDR_TO_CS_TRANSLATOR

  ; Build Configuration Area
  CfgPciMmioAddress             UINT64 ?        ; < PciMmioBase
  CfgPciMmioSize                UINT32 ?        ; < PciMmioSize
  CfgPlatVrmCfg                 PLATFORM_VRM_CONFIGURATION {}  ; < Several configuration settings for the voltage regulator module.
  CfgPlatNumIoApics             UINT32 ?        ; < PlatformApicIoNumber
  CfgMemInitPstate              UINT32 ?        ; < MemoryInitPstate
  CfgPlatformC1eMode            PLATFORM_C1E_MODES ?  ; < PlatformC1eMode
  CfgPlatformC1eOpData          UINT32 ?        ; < PlatformC1eOpData
  CfgPlatformCStateMode         PLATFORM_CSTATE_MODES ?  ; < PlatformCStateMode
  CfgPlatformCStateOpData       UINT32 ?        ; < PlatformCStateOpData
  CfgCoreLevelingMode           UINT32 ?        ; < CoreLevelingCofig
  CfgPerformanceProfile         PERFORMANCE_PROFILE  {}  ; < The platform's control flow mode and platform performance settings.
  CfgPlatformDeemphasisList     POINTER  ?      ; < HT Deemphasis
  CfgAmdPlatformType            UINT32 ?        ; < AmdPlatformType
  CfgAmdPstateCapValue          UINT32 ?        ; < Amd pstate ceiling enabling deck

  CfgMemoryBusFrequencyLimit    MEMORY_BUS_SPEED ?      ; < Memory Bus Frequency Limit
  CfgMemoryModeUnganged         BOOLEAN ?       ; < Memory Mode Unganged
  CfgMemoryQuadRankCapable      BOOLEAN ?       ; < Memory Quad Rank Capable
  CfgMemoryQuadrankType         QUANDRANK_TYPE ?        ; < Memory Quadrank Type
  CfgMemoryRDimmCapable         BOOLEAN ?       ; < Memory RDIMM Capable
  CfgMemoryUDimmCapable         BOOLEAN ?       ; < Memory UDIMM Capable
  CfgMemorySODimmCapable        BOOLEAN ?       ; < Memory SODimm Capable
  CfgLimitMemoryToBelow1Tb      BOOLEAN ?       ; < Limit memory address space to below 1TB
  CfgMemoryEnableBankInterleaving       BOOLEAN ?       ; < Memory Enable Bank Interleaving
  CfgMemoryEnableNodeInterleaving       BOOLEAN ?       ; < Memory Enable Node Interleaving
  CfgMemoryChannelInterleaving  BOOLEAN ?       ; < Memory Channel Interleaving
  CfgMemoryPowerDown            BOOLEAN ?       ; < Memory Power Down
  CfgPowerDownMode              POWER_DOWN_MODE ?       ; < Power Down Mode
  CfgOnlineSpare                BOOLEAN ?       ; < Online Spare
  CfgMemoryParityEnable         BOOLEAN ?       ; < Memory Parity Enable
  CfgBankSwizzle                BOOLEAN ?       ; < Bank Swizzle
  CfgTimingModeSelect           USER_MEMORY_TIMING_MODE ?       ; < Timing Mode Select
  CfgMemoryClockSelect          MEMORY_BUS_SPEED ?      ; < Memory Clock Select
  CfgDqsTrainingControl         BOOLEAN ?       ; < Dqs Training Control
  CfgIgnoreSpdChecksum          BOOLEAN ?       ; < Ignore Spd Checksum
  CfgUseBurstMode               BOOLEAN ?       ; < Use Burst Mode
  CfgMemoryAllClocksOn          BOOLEAN ?       ; < Memory All Clocks On
  CfgEnableEccFeature           BOOLEAN ?       ; < Enable ECC Feature
  CfgEccRedirection             BOOLEAN ?       ; < ECC Redirection
  CfgScrubDramRate              UINT16 ?        ; < Scrub Dram Rate
  CfgScrubL2Rate                UINT16 ?        ; < Scrub L2Rate
  CfgScrubL3Rate                UINT16 ?        ; < Scrub L3Rate
  CfgScrubIcRate                UINT16 ?        ; < Scrub Ic Rate
  CfgScrubDcRate                UINT16 ?        ; < Scrub Dc Rate
  CfgEccSyncFlood               BOOLEAN ?       ; < ECC Sync Flood
  CfgEccSymbolSize              UINT16 ?        ; < ECC Symbol Size
  CfgHeapDramAddress            UINT64 ?        ; < Heap contents will be temporarily stored in this address during the transition
  CfgNodeMem1GBAlign            BOOLEAN ?       ; < Node Mem 1GB boundary Alignment
  CfgS3LateRestore              BOOLEAN ?       ; < S3 Late Restore
  CfgAcpiPstateIndependent      BOOLEAN ?       ; < PSD method dependent/Independent
  CfgApMtrrSettingsList         POINTER ?       ; < The AP's MTRR settings before final halt
  CfgUmaMode                    UMA_MODE ?      ; < Uma Mode
  CfgUmaSize                    UINT32 ?        ; < Uma Size [31:0]=Addr[47:16]
  CfgUmaAbove4G                 BOOLEAN ?       ; < Uma Above 4G Support
  CfgUmaAlignment               UMA_ALIGNMENT ? ; < Uma alignment
  CfgProcessorScopeInSb         BOOLEAN ?       ; < ACPI Processor Object in \_SB scope
  CfgProcessorScopeName0        CHAR8 ?         ; < OEM specific 1st character of processor scope name.
  CfgProcessorScopeName1        CHAR8 ?         ; < OEM specific 2nd character of processor scope name.
  CfgGnbHdAudio                 UINT8 ?         ; < Gnb HD Audio Enable
  CfgAbmSupport                 UINT8 ?         ; < ABM support
  CfgDynamicRefreshRate         UINT8 ?         ; < Dynamic refresh rate
  CfgLcdBackLightControl        UINT8 ?         ; < Lcd back light control
  CfgTempPcieMmioBaseAddress    UINT32 ?        ; < Temp pcie MMIO base address

  Reserved                      BOOLEAN ?       ; < reserved...
BUILD_OPT_CFG    ENDS

  ; A structure containing platform specific operational characteristics. This
  ; structure is initially populated by the initializer with a copy of the same
  ; structure that was created at build time using the build configuration controls.
PLATFORM_CONFIGURATION    STRUCT
  PlatformProfile               PERFORMANCE_PROFILE {}          ; < Several configuration settings for the processor.
  PlatformDeemphasisList        POINTER ?       ; < Deemphasis levels for the platform's HT links.
  CoreLevelingMode              UINT8 ?         ; < Indicates how to balance the number of cores per processor.
  C1eMode                       PLATFORM_C1E_MODES ? ; < Specifies the method of C1e enablement - Disabled, HW, or message based.
  C1ePlatformData               UINT32 ?        ; < If C1eMode is HW, specifies the P_LVL3 I/O port of the platform.
  CStateMode                    PLATFORM_CSTATE_MODES ? ; < Specifies the method of C-State enablement - Disabled, or C6.
  CStatePlatformData            UINT32 ?        ; < If CStateMode is C6, specifies a block of 8 free I/O ports to be used for
                                                ; < placing the CPU in C6.
  UserOptionDmi                 BOOLEAN ?       ; < When set to TRUE, the DMI data table is generated.
  UserOptionPState              BOOLEAN ?       ; < When set to TRUE, the PState data tables are generated.
  UserOptionSrat                BOOLEAN ?       ; < When set to TRUE, the SRAT data table is generated.
  UserOptionSlit                BOOLEAN ?       ; < When set to TRUE, the SLIT data table is generated.
  UserOptionWhea                BOOLEAN ?       ; < When set to TRUE, the WHEA data table is generated.
  PowerCeiling                  UINT32 ?        ; <  P-State Ceiling Enabling Deck - Max power milli-watts.
  ForcePstateIndependent        BOOLEAN ?       ; < P-State _PSD independence or dependence.
  NumberOfIoApics               UINT32 ?        ; < Number of I/O APICs in the system
  VrmProperties                 PLATFORM_VRM_CONFIGURATION {}  ; < Several configuration settings for the voltage regulator module.
  ProcessorScopeInSb            BOOLEAN ?       ; < ACPI Processor Object in \_SB scope
  ProcessorScopeName0           CHAR8 ?         ; < OEM specific 1st character of processor scope name.
  ProcessorScopeName1           CHAR8 ?         ; < OEM specific 2nd character of processor scope name.
PLATFORM_CONFIGURATION    ENDS

 ; P-state structure for each state
S_PSTATE_VALUES    STRUCT
  PStateEnable                  UINT32 ?        ; < Pstate enable
  CoreFreq                      UINT32 ?        ; < MHz
  Power                         UINT32 ?        ; < milliWatts
  IddValue                      UINT32 ?     ; < Current value field
  IddDiv                        UINT32 ?       ; < Current divisor field
S_PSTATE_VALUES    ENDS

 ; P-state structure for each core
S_PSTATE    STRUCT
  PStateMaxValue                UINT8 ?         ; < Max p-state number in this core
  HtcPstateLimit                UINT8 ?         ; < Htc limit
  HtcCapable                    UINT8 ?         ; < Htc capable
  Reserved                      UINT8 ?         ; < Reserved
  PStateStruct                  S_PSTATE_VALUES (1) DUP ({})    ; < P state struc
S_PSTATE    ENDS

 ; P-state structure for each node
PSTATE_LEVELING    STRUCT
  SetPState0                    UINT8 ?         ; < If value = 0x55 (Don't set PState0)
  TotalCoresInNode              UINT8 ?         ; < core number per node
  PStateLevelingSizeOfBytes     UINT16 ?        ; < Size
  OnlyOneEnabledPState          BOOLEAN ?       ; < Only P0
  InitStruct                    UINT8 ?         ; < Init struc
  AllCpusHaveIdenticalPStates   BOOLEAN ?       ; < Have Identical p state
  CreateAcpiTables              UINT8 ?         ; < Create table flag
  SocketNumber                  UINT8 ?         ; < Physical socket number of this socket
  Reserved                      UINT8 (3) DUP (?)       ; < Reserved.
  PStateCoreStruct              S_PSTATE (1) DUP ({})           ; < P state core struc
PSTATE_LEVELING    ENDS

 ; P-state structure for whole system
S_CPU_AMD_PSTATE    STRUCT
  TotalSocketInSystem           UINT32 ?        ; < Total node number in system
  SizeOfBytes                   UINT32 ?        ; < Structure size
  PStateLevelingStruc           PSTATE_LEVELING (1) DUP ({})    ; < P state level structure
S_CPU_AMD_PSTATE    ENDS

; *********************************************************************
; * Structures for: AmdInitLate
; *********************************************************************
    PROC_VERSION_LENGTH         EQU    3h
    MAX_DIMMS_PER_SOCKET        EQU    16


  ; Interface Parameter Structures
 ; DMI Type4 - Processor ID
TYPE4_PROC_ID    STRUCT
  ProcIdLsd                     UINT32 ?        ; < Lower half of 64b ID
  ProcIdMsd                     UINT32 ?        ; < Upper half of 64b ID
TYPE4_PROC_ID    ENDS

 ; DMI Type 4 - Processor information
TYPE4_DMI_INFO    STRUCT
  T4ProcType                    UINT8 ?         ; < CPU Type
  T4ProcFamily                  UINT8 ?         ; < Family 1
  T4ProcId                      TYPE4_PROC_ID {}        ; < Id
  T4Voltage                     UINT8 ?         ; < Voltage
  T4ExternalClock               UINT16 ?        ; < External clock
  T4MaxSpeed                    UINT16 ?        ; < Max speed
  T4CurrentSpeed                UINT16 ?        ; < Current speed
  T4Status                      UINT8 ?         ; < Status
  T4ProcUpgrade                 UINT8 ?         ; < Up grade
  T4CoreCount                   UINT8 ?         ; < Core count
  T4CoreEnabled                 UINT8 ?         ; < Core Enable
  T4ThreadCount                 UINT8 ?         ; < Thread count
  T4ProcCharacteristics         UINT16 ?        ; < Characteristics
  T4ProcFamily2                 UINT16 ?        ; < Family 2
  T4ProcVersion                 CHAR8 (PROC_VERSION_LENGTH) DUP (?)     ; < Cpu version
TYPE4_DMI_INFO    ENDS

 ; DMI Type 7 - Cache information
TYPE7_DMI_INFO    STRUCT
  T7CacheCfg                    UINT16 ?        ; < Cache cfg
  T7MaxCacheSize                UINT16 ?        ; < Max size
  T7InstallSize                 UINT16 ?        ; < Install size
  T7SupportedSramType           UINT16 ?        ; < Supported Sram Type
  T7CurrentSramType             UINT16 ?        ; < Current type
  T7CacheSpeed                  UINT8 ?         ; < Speed
  T7ErrorCorrectionType         UINT8 ?         ; < ECC type
  T7SystemCacheType             UINT8 ?         ; < Cache type
  T7Associativity               UINT8 ?         ; < Associativity
TYPE7_DMI_INFO    ENDS

 ; DMI Type 16 offset 04h - Location

  OtherLocation         EQU    01h      ; < Assign 01 to Other
  UnknownLocation       EQU    2        ; < Assign 02 to Unknown
  SystemboardOrMotherboard      EQU    3        ; < Assign 03 to systemboard or motherboard
  IsaAddonCard          EQU    4        ; < Assign 04 to ISA add-on card
  EisaAddonCard         EQU    5        ; < Assign 05 to EISA add-on card
  PciAddonCard          EQU    6        ; < Assign 06 to PCI add-on card
  McaAddonCard          EQU    7        ; < Assign 07 to MCA add-on card
  PcmciaAddonCard       EQU    8        ; < Assign 08 to PCMCIA add-on card
  ProprietaryAddonCard  EQU    9        ; < Assign 09 to proprietary add-on card
  NuBus                 EQU    10       ; < Assign 0A to NuBus
  Pc98C20AddonCard      EQU    11       ; < Assign 0A0 to PC-98/C20 add-on card
  Pc98C24AddonCard      EQU    12       ; < Assign 0A1 to PC-98/C24 add-on card
  Pc98EAddoncard        EQU    13       ; < Assign 0A2 to PC-98/E add-on card
  Pc98LocalBusAddonCard EQU    14       ; < Assign 0A3 to PC-98/Local bus add-on card
DMI_T16_LOCATION  TEXTEQU  <DWORD> ;} DMI_T16_LOCATION;

 ; DMI Type 16 offset 05h - Memory Error Correction

  OtherUse              EQU    01h      ; < Assign 01 to Other
  UnknownUse            EQU    2        ; < Assign 02 to Unknown
  SystemMemory          EQU    3        ; < Assign 03 to system memory
  VideoMemory           EQU    4        ; < Assign 04 to video memory
  FlashMemory           EQU    5        ; < Assign 05 to flash memory
  NonvolatileRam        EQU    6        ; < Assign 06 to non-volatile RAM
  CacheMemory           EQU    7        ; < Assign 07 to cache memory
DMI_T16_USE  TEXTEQU  <DWORD> ;} DMI_T16_USE;

 ; DMI Type 16 offset 07h - Maximum Capacity

  Dmi16OtherErrCorrection    EQU    01h ; < Assign 01 to Other
  Dmi16UnknownErrCorrection  EQU    2   ; < Assign 02 to Unknown
  Dmi16NoneErrCorrection     EQU    3   ; < Assign 03 to None
  Dmi16Parity                EQU    4   ; < Assign 04 to parity
  Dmi16SingleBitEcc          EQU    5   ; < Assign 05 to Single-bit ECC
  Dmi16MultiBitEcc           EQU    6   ; < Assign 06 to Multi-bit ECC
  Dmi16Crc                   EQU    7   ; < Assign 07 to CRC
DMI_T16_ERROR_CORRECTION  TEXTEQU  <DWORD> ;} DMI_T16_ERROR_CORRECTION;

 ; DMI Type 16 - Physical Memory Array
TYPE16_DMI_INFO    STRUCT
  Location                      DMI_T16_LOCATION ?      ; < The physical location of the Memory Array,
                                                        ; < whether on the system board or an add-in board.
  Use                           DMI_T16_USE ?           ; < Identifies the function for which the array
                                                        ; < is used.
  MemoryErrorCorrection         DMI_T16_ERROR_CORRECTION ?      ; < The primary hardware error correction or
                                                        ; < detection method supported by this memory array.
  MaximumCapacity               UINT32 ?        ; < The maximum memory capacity, in kilobytes,
                                                        ; < for the array.
  NumberOfMemoryDevices         UINT16 ?        ; < The number of slots or sockets available
                                                        ; < for memory devices in this array.
TYPE16_DMI_INFO    ENDS

 ; DMI Type 17 offset 0Eh - Form Factor
  OtherFormFactor       EQU    01h      ; < Assign 01 to Other
  UnknowFormFactor      EQU    2        ; < Assign 02 to Unknown
  SimmFormFactor        EQU    3        ; < Assign 03 to SIMM
  SipFormFactor         EQU    4        ; < Assign 04 to SIP
  ChipFormFactor        EQU    5        ; < Assign 05 to Chip
  DipFormFactor         EQU    6        ; < Assign 06 to DIP
  ZipFormFactor         EQU    7        ; < Assign 07 to ZIP
  ProprietaryCardFormFactor     EQU    8        ; < Assign 08 to Proprietary Card
  DimmFormFactorFormFactor      EQU    9        ; < Assign 09 to DIMM
  TsopFormFactor        EQU    10       ; < Assign 10 to TSOP
  RowOfChipsFormFactor  EQU    11       ; < Assign 11 to Row of chips
  RimmFormFactor        EQU    12       ; < Assign 12 to RIMM
  SodimmFormFactor      EQU    13       ; < Assign 13 to SODIMM
  SrimmFormFactor       EQU    14       ; < Assign 14 to SRIMM
  FbDimmFormFactor      EQU    15       ; < Assign 15 to FB-DIMM
DMI_T17_FORM_FACTOR  TEXTEQU  <DWORD>

 ; DMI Type 17 offset 12h - Memory Type
  OtherMemType          EQU    01h      ; < Assign 01 to Other
  UnknownMemType        EQU    2        ; < Assign 02 to Unknown
  DramMemType           EQU    3        ; < Assign 03 to DRAM
  EdramMemType          EQU    4        ; < Assign 04 to EDRAM
  VramMemType           EQU    5        ; < Assign 05 to VRAM
  SramMemType           EQU    6        ; < Assign 06 to SRAM
  RamMemType            EQU    7        ; < Assign 07 to RAM
  RomMemType            EQU    8        ; < Assign 08 to ROM
  FlashMemType          EQU    9        ; < Assign 09 to Flash
  EepromMemType         EQU    10       ; < Assign 10 to EEPROM
  FepromMemType         EQU    11       ; < Assign 11 to FEPROM
  EpromMemType          EQU    12       ; < Assign 12 to EPROM
  CdramMemType          EQU    13       ; < Assign 13 to CDRAM
  ThreeDramMemType      EQU    14       ; < Assign 14 to 3DRAM
  SdramMemType          EQU    15       ; < Assign 15 to SDRAM
  SgramMemType          EQU    16       ; < Assign 16 to SGRAM
  RdramMemType          EQU    17       ; < Assign 17 to RDRAM
  DdrMemType            EQU    18       ; < Assign 18 to DDR
  Ddr2MemType           EQU    19       ; < Assign 19 to DDR2
  Ddr2FbdimmMemType     EQU    20       ; < Assign 20 to DDR2 FB-DIMM
  Ddr3MemType           EQU    24       ; < Assign 24 to DDR3
  Fbd2MemType           EQU    25       ; < Assign 25 to FBD2
DMI_T17_MEMORY_TYPE  TEXTEQU  <DWORD>

 ; DMI Type 17 offset 13h - Type Detail
DMI_T17_TYPE_DETAIL    STRUCT
  Reserved1                     UINT16 ?
;   OUT UINT16                    Reserved1:1;                  ; < Reserved
;   OUT UINT16                    Other:1;                      ; < Other
;   OUT UINT16                    Unknown:1;                    ; < Unknown
;   OUT UINT16                    FastPaged:1;                  ; < Fast-Paged
;   OUT UINT16                    StaticColumn:1;               ; < Static column
;   OUT UINT16                    PseudoStatic:1;               ; < Pseudo-static
;   OUT UINT16                    Rambus:1;                     ; < RAMBUS
;   OUT UINT16                    Synchronous:1;                ; < Synchronous
;   OUT UINT16                    Cmos:1;                       ; < CMOS
;   OUT UINT16                    Edo:1;                        ; < EDO
;   OUT UINT16                    WindowDram:1;                 ; < Window DRAM
;   OUT UINT16                    CacheDram:1;                  ; < Cache Dram
;   OUT UINT16                    NonVolatile:1;                ; < Non-volatile
;   OUT UINT16                    Reserved2:3;                  ; < Reserved
DMI_T17_TYPE_DETAIL    ENDS

 ; DMI Type 17 - Memory Device
TYPE17_DMI_INFO    STRUCT
  TotalWidth                    UINT16 ?        ; < Total Width, in bits, of this memory device, including any check or error-correction bits.
  DataWidth                     UINT16 ?        ; < Data Width, in bits, of this memory device.
  MemorySize                    UINT16 ?        ; < The size of the memory device.
  FormFactor                    DMI_T17_FORM_FACTOR ?           ; < The implementation form factor for this memory device.
  DeviceSet                     UINT8 ?         ; < Identifies when the Memory Device is one of a set of
                                                        ; < Memory Devices that must be populated with all devices of
                                                        ; < the same type and size, and the set to which this device belongs.
  DeviceLocator                 CHAR8 (8) DUP (?)       ; < The string number of the string that identifies the physically labeled socket or board position where the memory device is located.
  BankLocator                   CHAR8 (10) DUP (?)      ; < The string number of the string that identifies the physically labeled bank where the memory device is located.
  MemoryType                    DMI_T17_MEMORY_TYPE ?           ; < The type of memory used in this device.
  TypeDetail                    DMI_T17_TYPE_DETAIL {}          ; < Additional detail on the memory device type
  Speed                         UINT16 ?        ; < Identifies the speed of the device, in megahertz (MHz).
  ManufacturerIdCode            UINT64 ?        ; < Manufacturer ID code.
  SerialNumber                  CHAR8 (9) DUP (?)       ; < Serial Number.
  PartNumber                    CHAR8 (19) DUP (?)      ; < Part Number.
  Attributes                    UINT8 ?         ; < Bits 7-4: Reserved, Bits 3-0: rank.
TYPE17_DMI_INFO    ENDS

 ; Memory DMI Type 17 and 20 - for memory use
MEM_DMI_INFO    STRUCT
  TotalWidth                    UINT16 ?              ; ///< Total Width, in bits, of this memory device, including any check or error-correction bits.
  DataWidth                     UINT16 ?              ; ///< Data Width, in bits, of this memory device.
  MemorySize                    UINT16 ?              ; ///< The size of the memory device.
  FormFactor                    DMI_T17_FORM_FACTOR ? ; ///< The implementation form factor for this memory device.
  DeviceLocator                 UINT8 ?               ; ///< The string number of the string that identifies the physically labeled socket or board position where the memory device is located.
  BankLocator                   UINT8 ?               ; ///< The string number of the string that identifies the physically labeled bank where the memory device is located.
  Speed                         UINT16 ?              ; ///< Identifies the speed of the device, in megahertz (MHz).
  ManufacturerIdCode            UINT64 ?              ; ///< Manufacturer ID code.
  SerialNumber                  UINT8 (4) DUP (?)     ; ///< Serial Number.
  PartNumber                    UINT8 (18) DUP (?)    ; ///< Part Number.
  Attributes                    UINT8 ?               ; ///< Bits 7-4: Reserved, Bits 3-0: rank.
  Socket                        UINT8 ?
; OUT UINT8                     Socket:3              ; ///< Socket ID
; OUT UINT8                     Channel:2             ; ///< Channel ID
; OUT UINT8                     Dimm:2                ; ///< DIMM ID
; OUT UINT8                     DimmPresent:1         ; ///< Dimm Present
  StartingAddr                  UINT32 ?              ; ///< The physical address, in kilobytes, of a range
                                                      ; ///< of memory mapped to the referenced Memory Device.
  EndingAddr                    UINT32 ?              ; ///< The handle, or instance number, associated with
                                                      ; ///< the Memory Device structure to which this address
                                                      ; ///< range is mapped.
MEM_DMI_INFO    ENDS

 ; DMI Type 19 - Memory Array Mapped Address
TYPE19_DMI_INFO    STRUCT
  StartingAddr                  UINT32 ?        ; < The physical address, in kilobytes,
                                                        ; < of a range of memory mapped to the
                                                        ; < specified physical memory array.
  EndingAddr                    UINT32 ?        ; < The physical ending address of the
                                                        ; < last kilobyte of a range of addresses
                                                        ; < mapped to the specified physical memory array.
  MemoryArrayHandle             UINT16 ?        ; < The handle, or instance number, associated
                                                        ; < with the physical memory array to which this
                                                        ; < address range is mapped.
  PartitionWidth                UINT8 ?         ; < Identifies the number of memory devices that
                                                        ; < form a single row of memory for the address
                                                        ; < partition defined by this structure.
TYPE19_DMI_INFO    ENDS

; DMI Type 20 - Memory Device Mapped Address
TYPE20_DMI_INFO    STRUCT
  StartingAddr                  UINT32 ?        ; < The physical address, in kilobytes, of a range
                                                        ; < of memory mapped to the referenced Memory Device.
  EndingAddr                    UINT32 ?        ; < The handle, or instance number, associated with
                                                        ; < the Memory Device structure to which this address
                                                        ; < range is mapped.
  MemoryDeviceHandle            UINT16 ?        ; < The handle, or instance number, associated with
                                                        ; < the Memory Device structure to which this address
                                                        ; < range is mapped.
  MemoryArrayMappedAddressHandle        UINT16 ?        ; < The handle, or instance number, associated
                                                        ; < with the Memory Array Mapped Address structure to
                                                        ; < which this device address range is mapped.
  PartitionRowPosition          UINT8 ?         ; < Identifies the position of the referenced Memory
                                                        ; < Device in a row of the address partition.
  InterleavePosition            UINT8 ?         ; < The position of the referenced Memory Device in
                                                        ; < an interleave.
  InterleavedDataDepth          UINT8 ?         ; < The maximum number of consecutive rows from the
                                                        ; < referenced Memory Device that are accessed in a
                                                        ; < single interleaved transfer.
TYPE20_DMI_INFO    ENDS

 ; Collection of pointers to the DMI records
DMI_INFO    STRUCT
  T4                            TYPE4_DMI_INFO (MAX_SOCKETS_SUPPORTED) DUP ({})         ; < Type 4 struc
  T7L1                          TYPE7_DMI_INFO (MAX_SOCKETS_SUPPORTED) DUP ({})         ; < Type 7 struc 1
  T7L2                          TYPE7_DMI_INFO (MAX_SOCKETS_SUPPORTED) DUP ({})         ; < Type 7 struc 2
  T7L3                          TYPE7_DMI_INFO (MAX_SOCKETS_SUPPORTED) DUP ({})         ; < Type 7 struc 3
  T16                           TYPE16_DMI_INFO {}      ; < Type 16 struc
  T17                           TYPE17_DMI_INFO (MAX_SOCKETS_SUPPORTED * MAX_CHANNELS_PER_SOCKET * MAX_DIMMS_PER_CHANNEL) DUP ({})          ; < Type 17 struc
  T19                           TYPE19_DMI_INFO {}      ; < Type 19 struc
  T20                           TYPE20_DMI_INFO (MAX_SOCKETS_SUPPORTED * MAX_CHANNELS_PER_SOCKET * MAX_DIMMS_PER_CHANNEL) DUP ({})          ; < Type 20 struc
DMI_INFO    ENDS



; *********************************************************************
; * Interface call:  AllocateExecutionCache
; *********************************************************************
    MAX_CACHE_REGIONS           EQU    3

 ; AllocateExecutionCache sub param structure for cached memory region
EXECUTION_CACHE_REGION    STRUCT
  ExeCacheStartAddr             UINT32 ?        ; < Start address
  ExeCacheSize                  UINT32 ?        ; < Size
EXECUTION_CACHE_REGION    ENDS

; *********************************************************************
; * Interface call:  AmdGetAvailableExeCacheSize
; *********************************************************************
 ; Get available Cache remain
AMD_GET_EXE_SIZE_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Standard header
     AvailableExeCacheSize      UINT32 ?        ; < Remain size
AMD_GET_EXE_SIZE_PARAMS    ENDS






 ; Selection type for core leveling
  CORE_LEVEL_LOWEST     EQU    0        ; < Level to lowest common denominator
  CORE_LEVEL_TWO        EQU    1        ; < Level to 2 cores
  CORE_LEVEL_POWER_OF_TWO       EQU    2        ; < Level to 1,2,4 or 8
  CORE_LEVEL_NONE       EQU    3        ; < Do no leveling
  CORE_LEVEL_ONE        EQU    4        ; < Level to 1 core
  CORE_LEVEL_THREE      EQU    5        ; < Level to 3 cores
  CORE_LEVEL_FOUR       EQU    6        ; < Level to 4 cores
  CORE_LEVEL_FIVE       EQU    7        ; < Level to 5 cores
  CORE_LEVEL_SIX        EQU    8        ; < Level to 6 cores
  CORE_LEVEL_SEVEN      EQU    9        ; < Level to 7 cores
  CORE_LEVEL_EIGHT      EQU    10       ; < Level to 8 cores
  CORE_LEVEL_NINE       EQU    11       ; < Level to 9 cores
  CORE_LEVEL_TEN        EQU    12       ; < Level to 10 cores
  CORE_LEVEL_ELEVEN     EQU    13       ; < Level to 11 cores
  CORE_LEVEL_TWELVE     EQU    14       ; < Level to 12 cores
  CORE_LEVEL_THIRTEEN   EQU    15       ; < Level to 13 cores
  CORE_LEVEL_FOURTEEN   EQU    16       ; < Level to 14 cores
  CORE_LEVEL_FIFTEEN    EQU    17       ; < Level to 15 cores
  CoreLevelModeMax      EQU    18       ; < Used for bounds checking
CORE_LEVELING_TYPE  TEXTEQU  <DWORD>


; ***********************************************************************
; *
; *  AGESA Basic Level interface structure definition and function prototypes
; *
; **********************************************************************

; *********************************************************************
; * Interface call:  AmdCreateStruct
; *********************************************************************

; *********************************************************************
; * Interface call:  AmdReleaseStruct
; *********************************************************************

; *********************************************************************
; * Interface call:  AmdInitReset
; *********************************************************************
 ; AmdInitReset param structure
AMD_RESET_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Standard header
  CacheRegion                   EXECUTION_CACHE_REGION (3) DUP ({})     ; < The cached memory region
  HtConfig                      AMD_HT_RESET_INTERFACE {}       ; < The interface for Ht Recovery
AMD_RESET_PARAMS    ENDS

; *********************************************************************
; * Interface call:  AmdInitEarly
; *********************************************************************
 ; InitEarly param structure

 ; Provide defaults or customizations to each service performed in AmdInitEarly.

AMD_EARLY_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < The standard header
  CacheRegion                   EXECUTION_CACHE_REGION (3) DUP ({})     ; < Execution Map Interface
  PlatformConfig                PLATFORM_CONFIGURATION {}       ; < platform operational characteristics.
  HtConfig                      AMD_HT_INTERFACE {}     ; < HyperTransport Interface
  GnbConfig                     GNB_CONFIGURATION {}    ; < GNB configuration
AMD_EARLY_PARAMS    ENDS

; *********************************************************************
; * Interface call:  AmdInitPost
; *********************************************************************
 ; AmdInitPost param structure
AMD_POST_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Standard header
  PlatformConfig                PLATFORM_CONFIGURATION {}       ; < platform operational characteristics.
  MemConfig                     MEM_PARAMETER_STRUCT {}         ; < Memory post param
AMD_POST_PARAMS    ENDS

; *********************************************************************
; * Interface call:  AmdInitEnv
; *********************************************************************
 ; AmdInitEnv param structure
AMD_ENV_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Standard header
  PlatformConfig                PLATFORM_CONFIGURATION {}       ; < platform operational characteristics.
AMD_ENV_PARAMS    ENDS

; *********************************************************************
; * Interface call:  AmdInitMid
; *********************************************************************
 ; AmdInitMid param structure
AMD_MID_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Standard header
  PlatformConfig                PLATFORM_CONFIGURATION {}       ; < platform operational characteristics.
AMD_MID_PARAMS    ENDS

; *********************************************************************
; * Interface call:  AmdInitLate
; *********************************************************************
 ; AmdInitLate param structure
AMD_LATE_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}    ; < Standard header
  PlatformConfig                PLATFORM_CONFIGURATION {} ; < platform operational characteristics.
     DmiTable                   POINTER ?       ; < DMI Interface
     AcpiPState                 POINTER ?       ; < Acpi Pstate SSDT Table
     AcpiSrat                   POINTER ?       ; < SRAT Table
     AcpiSlit                   POINTER ?       ; < SLIT Table
     AcpiWheaMce                POINTER ?       ; < WHEA MCE Table
     AcpiWheaCmc                POINTER ?       ; < WHEA CMC Table
AMD_LATE_PARAMS    ENDS

; *********************************************************************
; * Interface call:  AmdInitRecovery
; *********************************************************************
 ; CPU Recovery Parameters
AMD_CPU_RECOVERY_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Standard Header
  PlatformConfig                PLATFORM_CONFIGURATION {} ; < platform operational characteristics
AMD_CPU_RECOVERY_PARAMS    ENDS

 ; AmdInitRecovery param structure
AMD_RECOVERY_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Standard header
  MemConfig                     MEM_PARAMETER_STRUCT {}         ; < Memory post param
  CacheRegion                   EXECUTION_CACHE_REGION (3) DUP ({})     ; < The cached memory region. And the max cache region is 3
  CpuRecoveryParams             AMD_CPU_RECOVERY_PARAMS {}      ; < Params for CPU related recovery init.
AMD_RECOVERY_PARAMS    ENDS


; *********************************************************************
; * Interface call:  AmdInitResume
; *********************************************************************
 ; AmdInitResume param structure
AMD_RESUME_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}      ; < Standard header
  PlatformConfig                PLATFORM_CONFIGURATION {} ; < Platform operational characteristics
  S3DataBlock                   AMD_S3_PARAMS {}          ; < Save state data
AMD_RESUME_PARAMS    ENDS

; *********************************************************************
; * Interface call:  AmdS3LateRestore
; *********************************************************************
 ; AmdS3LateRestore param structure
AMD_S3LATE_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Standard header
  PlatformConfig                PLATFORM_CONFIGURATION {}       ; < platform operational characteristics.
  S3DataBlock                   AMD_S3_PARAMS {}        ; < Save state data
AMD_S3LATE_PARAMS    ENDS

; *********************************************************************
; * Interface call:  AmdS3Save
; *********************************************************************
 ; AmdS3Save param structure
AMD_S3SAVE_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Standard header
  PlatformConfig                PLATFORM_CONFIGURATION {}       ; < platform operational characteristics.
  S3DataBlock                   AMD_S3_PARAMS {}        ; < Standard header
AMD_S3SAVE_PARAMS    ENDS

 ; General Services API


; *********************************************************************
; * Interface service call:  AmdGetApicId
; *********************************************************************
 ; Request the APIC ID of a particular core.

AMD_APIC_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Header for library and services.
  Socket                        UINT8 ?         ; < The Core's Socket.
  Core                          UINT8 ?         ; < The Core id.
     IsPresent                  BOOLEAN ?       ; < The Core is present, and  ApicAddress is valid.
     ApicAddress                UINT8 ?         ; < The Core's APIC ID.
AMD_APIC_PARAMS    ENDS

; *********************************************************************
; * Interface service call:  AmdGetPciAddress
; *********************************************************************
 ; Request the PCI Address of a Processor Module (that is, its Northbridge)

AMD_GET_PCI_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Header for library and services.
  Socket                        UINT8 ?         ; < The Processor's socket
  Module                        UINT8 ?         ; < The Module in that Processor
     IsPresent                  BOOLEAN ?       ; < The Core is present, and  PciAddress is valid.
     PciAddress                 PCI_ADDR {}      ; < The Processor's PCI Config Space address (Function 0, Register 0)
AMD_GET_PCI_PARAMS    ENDS

; *********************************************************************
; * Interface service call:  AmdIdentifyCore
; *********************************************************************
 ; Request the identity (Socket, Module, Core) of the current Processor Core

AMD_IDENTIFY_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Header for library and services.
     Socket                     UINT8 ?         ; < The current Core's Socket
     Module                     UINT8 ?         ; < The current Core's Processor Module
     Core                       UINT8 ?         ; < The current Core's core id.
AMD_IDENTIFY_PARAMS    ENDS

; *********************************************************************
; * Interface service call:  AmdReadEventLog
; *********************************************************************
 ; An Event Log Entry.
EVENT_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Header for library and services.
     EventClass                 UINT32 ?        ; < The severity of this event, matches AGESA_STATUS.
     EventInfo                  UINT32 ?        ; < The unique event identifier, zero means "no event".
     DataParam1                 UINT32 ?        ; < Data specific to the Event.
     DataParam2                 UINT32 ?        ; < Data specific to the Event.
     DataParam3                 UINT32 ?        ; < Data specific to the Event.
     DataParam4                 UINT32 ?        ; < Data specific to the Event.
EVENT_PARAMS    ENDS

; *********************************************************************
; * Interface service call:  AmdIdentifyDimm
; *********************************************************************
 ; Request the identity of dimm from system address

AMD_IDENTIFY_DIMM    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < Header for library and services.
  MemoryAddress                 UINT64 ?        ; < System Address that needs to be translated to dimm identification.
  SocketId                      UINT8 ?         ; < The socket on which the targeted address locates.
  MemChannelId                  UINT8 ?         ; < The channel on which the targeted address locates.
  DimmId                        UINT8 ?         ; < The dimm on which the targeted address locates.
AMD_IDENTIFY_DIMM    ENDS

 ; Data structure for the Mapping Item between Unified ID for IDS Setup Option
 ; and the option value.

IDS_NV_ITEM    STRUCT
  IdsNvId                       UINT16 ?        ; < Unified ID for IDS Setup Option.
  IdsNvValue                    UINT16 ?        ; < The value of IDS Setup Option.
IDS_NV_ITEM    ENDS

 ; Data Structure for IDS CallOut Function
IDS_CALLOUT_STRUCT    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}     ; < The Standard Header for AGESA Service
  IdsNvPtr                      POINTER ?       ; < Memory Pointer of IDS NV Table
  Reserved                      UINT32 ?         ; < reserved
IDS_CALLOUT_STRUCT    ENDS


 ; AGESA IDS NV ID Definitions
  AGESA_IDS_DFT_VAL                            EQU    0FFFFh; <  Default value of every uninitlized NV item, the action for it will be ignored
  AGESA_IDS_NV_END                             EQU    0FFFFh; <  Flag specify end of option structure

  AGESA_IDS_EXT_ID_START                            EQU    0000h; <                                   specify the start of external NV id

  AGESA_IDS_NV_UCODE                                EQU    0001h; <                                  Enable or disable microcode patching

  AGESA_IDS_NV_TARGET_PSTATE                        EQU    0002h; <                              Set the P-state required to be activated
  AGESA_IDS_NV_POSTPSTATE                           EQU    0003h; <                 Set the P-state required to be activated through POST

  AGESA_IDS_NV_CHANNEL_INTERLEAVE                   EQU    0004h; <                                  Enable or disable Channel Interleave

  AGESA_IDS_NV_SCRUB_REDIRECTION                    EQU    0005h; <                 Enable or disable a write to dram with corrected data
  AGESA_IDS_NV_DRAM_SCRUB                           EQU    0006h; <                         Set the rate of background scrubbing for DRAM
  AGESA_IDS_NV_DCACHE_SCRUB                         EQU    0007h; <                  Set the rate of background scrubbing for the DCache.
  AGESA_IDS_NV_L2_SCRUB                             EQU    0008h; <                 Set the rate of background scrubbing for the L2 cache
  AGESA_IDS_NV_L3_SCRUB                             EQU    0009h; <                 Set the rate of background scrubbing for the L3 cache
  AGESA_IDS_NV_SYNC_ON_ECC_ERROR                    EQU    000Ah; <          Enable or disable the sync flood on un-correctable ECC error
  AGESA_IDS_NV_ECC_SYMBOL_SIZE                      EQU    000Bh; <                                                   Set ECC symbol size

  AGESA_IDS_NV_ALL_MEMCLKS                          EQU    000Ch; <                            Enable or disable all memory clocks enable
  AGESA_IDS_NV_DCT_GANGING_MODE                     EQU    000Dh; <                                                   Set the Ganged mode
  AGESA_IDS_NV_DRAM_BURST_LENGTH32                  EQU    000Eh; <                                          Set the DRAM Burst Length 32
  AGESA_IDS_NV_MEMORY_POWER_DOWN                    EQU    000Fh; <                              Enable or disable Memory power down mode
  AGESA_IDS_NV_MEMORY_POWER_DOWN_MODE               EQU    0010h; <                                        Set the Memory power down mode
  AGESA_IDS_NV_DLL_SHUT_DOWN                        EQU    0011h; <                                         Enable or disable DLLShutdown

  AGESA_IDS_NV_PROBEFILTER                          EQU    0012h; <                                         Enable or Disable ProbeFilter

  AGESA_IDS_NV_HDTOUT                               EQU    0013h; <                                      Enable or disable HDTOUT feature

  AGESA_IDS_NV_HTLINKSOCKET                         EQU    0014h; <                                                        HT Link Socket
  AGESA_IDS_NV_HTLINKPORT                           EQU    0015h; <                                                          HT Link Port
  AGESA_IDS_NV_HTLINKFREQ                           EQU    0016h; <                                                     HT Link Frequency
  AGESA_IDS_NV_HTLINKWIDTHIN                        EQU    0017h; <                                                      HT Link In Width
  AGESA_IDS_NV_HTLINKWIDTHOUT                       EQU    0018h; <                                                     HT Link Out Width

  AGESA_IDS_NV_GNBHDAUDIOEN                         EQU    0019h; <                                        Enable or disable GNB HD Audio
  AGESA_IDS_EXT_ID_END                              EQU    001Ah; <                                     specify the end of external NV ID

  IDS_EX_NV_ID  TEXTEQU  <DWORD>
