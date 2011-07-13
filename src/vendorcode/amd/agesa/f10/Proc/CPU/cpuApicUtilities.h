/**
 * @file
 *
 * AMD CPU APIC related utility functions and structures
 *
 * Contains code that provides mechanism to invoke and control APIC communication.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

#ifndef _CPU_APIC_UTILITIES_H_
#define _CPU_APIC_UTILITIES_H_


/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */
#define APIC_CTRL_DWORD 0xF
#define APIC_CTRL_REG (APIC_CTRL_DWORD << 4)
#define APIC_CTRL_MASK 0xFF
#define APIC_CTRL_SHIFT 0

#define APIC_DATA_DWORD 0x38
#define APIC_DATA_REG (APIC_DATA_DWORD << 4)

#define APIC_REMOTE_READ_REG 0xC0
#define APIC_CMD_LO_REG 0x300
#define APIC_CMD_HI_REG 0x310

// APIC_CMD_LO_REG bits
#define CMD_REG_DELIVERY_STATUS 0x1000
#define CMD_REG_TO_READ 0x300
#define CMD_REG_REMOTE_RD_STS_MSK 0x30000
#define CMD_REG_REMOTE_DELIVERY_PENDING 0x10000
#define CMD_REG_REMOTE_DELIVERY_DONE 0x20000
#define CMD_REG_TO_NMI 0x400

// ExeFlags bits
#define WAIT_FOR_CORE     0x00000001
#define TASK_HAS_OUTPUT   0x00000002
#define RETURN_PARAMS     0x00000004
#define END_AT_HLT        0x00000008
#define PASS_EARLY_PARAMS 0x00000010

// Control Byte Values
// bit 7 indicates the type of message
// 1 - control message
// 0 - launch + APIC ID = message to go
//
#define CORE_UNAVAILABLE            0xFF
#define CORE_IDLE                   0xFE
#define CORE_IDLE_HLT               0xFD
#define CORE_ACTIVE                 0xFC
#define CORE_NEEDS_PTR              0xFB
#define CORE_NEEDS_DATA_SIZE        0xFA
#define CORE_STS_DATA_READY_1       0xF9
#define CORE_STS_DATA_READY_0       0xF8
#define CORE_DATA_FLAGS_READY       0xF7
#define CORE_DATA_FLAGS_ACKNOWLEDGE 0xF6
#define CORE_DATA_PTR_READY         0xF5

// Macro used to determine the number of dwords to transmit to the AP as input
#define SIZE_IN_DWORDS(sInput) ((UINT32) (((sizeof (sInput)) + 3) >> 2))

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
typedef VOID (*PF_AP_TASK) (AMD_CONFIG_PARAMS *StdHeader);
typedef VOID (*PF_AP_TASK_I) (VOID *, AMD_CONFIG_PARAMS *StdHeader);
typedef VOID (*PF_AP_TASK_C) (AMD_CONFIG_PARAMS *StdHeader, AMD_CPU_EARLY_PARAMS *);
typedef VOID (*PF_AP_TASK_IC) (VOID *, AMD_CONFIG_PARAMS *StdHeader, AMD_CPU_EARLY_PARAMS *);
typedef UINT32 (*PF_AP_TASK_O) (AMD_CONFIG_PARAMS *StdHeader);
typedef UINT32 (*PF_AP_TASK_IO) (VOID *, AMD_CONFIG_PARAMS *StdHeader);
typedef UINT32 (*PF_AP_TASK_OC) (AMD_CONFIG_PARAMS *StdHeader, AMD_CPU_EARLY_PARAMS *);
typedef UINT32 (*PF_AP_TASK_IOC) (VOID *, AMD_CONFIG_PARAMS *StdHeader, AMD_CPU_EARLY_PARAMS *);

/// Function pointer union representing the eight different
/// types of functions that an AP can be asked to perform.
typedef union {
  PF_AP_TASK     PfApTask;      ///< AMD_CONFIG_PARAMS *  input with no output
  PF_AP_TASK_I   PfApTaskI;     ///< VOID * + AMD_CONFIG_PARAMS *  input with no output
  PF_AP_TASK_C   PfApTaskC;     ///< AMD_CONFIG_PARAMS * + AMD_CPU_EARLY_PARAMS *  input with no output
  PF_AP_TASK_IC  PfApTaskIC;    ///< VOID * + AMD_CONFIG_PARAMS * + AMD_CPU_EARLY_PARAMS *  input with no output
  PF_AP_TASK_O   PfApTaskO;     ///< AMD_CONFIG_PARAMS * input with UINT32 output
  PF_AP_TASK_IO  PfApTaskIO;    ///< VOID * + AMD_CONFIG_PARAMS * input with UINT32 output
  PF_AP_TASK_OC  PfApTaskOC;    ///< AMD_CONFIG_PARAMS * + AMD_CPU_EARLY_PARAMS * input with UINT32 output
  PF_AP_TASK_IOC PfApTaskIOC;   ///< VOID * + AMD_CONFIG_PARAMS * + AMD_CPU_EARLY_PARAMS *  input with UINT32 output
} AP_FUNCTION_PTR;

/// Input structure for ApUtilTransmitBuffer and ApUtilReceiveBuffer
/// containing information about the data transfer from one core
/// to another.
typedef struct {
  IN OUT   UINT16 DataSizeInDwords;   ///< Size of the data to be transferred rounded up to the nearest dword
  IN OUT   VOID   *DataPtr;           ///< Pointer to the data
  IN       UINT32 DataTransferFlags;  ///< Flags dictating certain aspects of the data transfer
} AP_DATA_TRANSFER;

/// Input structure for ApUtilRunCodeOnSocketCore.
typedef struct _AP_TASK {
  AP_FUNCTION_PTR  FuncAddress;   ///< Pointer to the function that the AP will run
  AP_DATA_TRANSFER DataTransfer;  ///< Data transfer struct for optionally passing data that the AP should use as input to the function
  UINT32           ExeFlags;      ///< Flags dictating certain aspects of the AP tasking sequence
} AP_TASK;

/// Input structure for ApUtilWaitForCoreStatus.
typedef struct {
  IN       UINT8  *Status;            ///< Pointer to the 1st element of an array of values to wait for
  IN       UINT8  NumberOfElements;   ///< Number of elements in the array
  IN       UINT32 RetryCount;         ///< Number of remote read cycles to complete before quitting
  IN       UINT32 WaitForStatusFlags; ///< Flags dictating certain aspects of ApUtilWaitForCoreStatus
} AP_WAIT_FOR_STATUS;

#define WAIT_STATUS_EQUALITY 0x00000001
#define WAIT_INFINITELY 0

// Data Transfer Flags
#define DATA_IN_MEMORY 0x00000001


/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */
//                   These are   P U B L I C   functions, used by AGESA
UINT8
ApUtilReadRemoteControlByte (
  IN       UINT8 Socket,
  IN       UINT8 Core,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
ApUtilWriteControlByte (
  IN       UINT8 Value,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT32
ApUtilReadRemoteDataDword (
  IN       UINT8 Socket,
  IN       UINT8 Core,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
ApUtilWriteDataDword (
  IN       UINT32 Value,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT32
ApUtilRunCodeOnSocketCore (
  IN       UINT8 Socket,
  IN       UINT8 Core,
  IN       AP_TASK *TaskPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT8
ApUtilWaitForCoreStatus (
  IN       UINT8 Socket,
  IN       UINT8 Core,
  IN       AP_WAIT_FOR_STATUS *WaitParamsPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
ApEntry (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams
  );

UINT32
ApUtilTaskOnExecutingCore (
  IN       AP_TASK *TaskPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       VOID    *ConfigParams
  );

VOID
ApUtilTransmitBuffer (
  IN     UINT8   Socket,
  IN     UINT8   Core,
  IN     AP_DATA_TRANSFER *BufferInfo,
  IN     AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
ApUtilReceiveBuffer (
  IN       UINT8   Socket,
  IN       UINT8   Core,
  IN OUT   AP_DATA_TRANSFER  *BufferInfo,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
GetLocalApicIdForCore (
  IN       UINT32 TargetSocket,
  IN       UINT32 TargetCore,
     OUT   UINT32 *LocalApicId,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
ApUtilRunCodeOnAllLocalCoresAtEarly (
  IN       AP_TASK *TaskPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParamsPtr
  );

VOID
RelinquishControlOfAllAPs (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

#endif  /* _CPU_APIC_UTILITIES_H_ */

