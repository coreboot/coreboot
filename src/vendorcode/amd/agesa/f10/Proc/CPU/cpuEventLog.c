/**
 * @file
 *
 * AMD Event (Error) Log APIs, and related functions.
 *
 * Contains code that records and returns the events and errors.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "amdlib.h"
#include "heapManager.h"
#include "GeneralServices.h"
#include "Ids.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_CPUEVENTLOG_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define TOTAL_EVENT_LOG_BUFFERS  16

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/**
 * A wrapper for each Event Log entry.
 */
typedef struct {
  UINT16      Count;         ///< Entry number
  AGESA_EVENT AgesaEvent;    ///< The entry itself.
} AGESA_EVENT_STRUCT;

/**
 * The Event Log.
 */
typedef struct {
  UINT16        ReadWriteFlag;   ///< Read Write flag.
  UINT16        Count;           ///< The total number of active entries.
  UINT16        ReadRecordPtr;   ///< The next entry to read.
  UINT16        WriteRecordPtr;  ///< The next entry to write.
  AGESA_EVENT_STRUCT AgesaEventStruct[TOTAL_EVENT_LOG_BUFFERS];   ///< The entries.
} AGESA_STRUCT_BUFFER;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
GetEventLogHeapPointer (
     OUT  AGESA_STRUCT_BUFFER **EventLog,
  IN      AMD_CONFIG_PARAMS *StdHeader
  );

/*---------------------------------------------------------------------------------------*/
/**
 * External AGESA interface to read an Event from the Event Log.
 *
 * This is the implementation of the external AGESA interface entry, as a thin wrapper
 * around the internal log services.
 *
 * @param[in]  Event           The event class, id, and any associated data.
 *
 * @retval     AGESA_SUCCESS   Always Succeeds.
 */
AGESA_STATUS
AmdReadEventLog (
  IN       EVENT_PARAMS *Event
  )
{
  AGESA_EVENT  LogEvent;
  AGESA_STATUS Status;

  AGESA_TESTPOINT (TpIfAmdReadEventLogEntry, &Event->StdHeader);

  ASSERT (Event != NULL);
  Status = GetEventLog (&LogEvent, &Event->StdHeader);

  Event->EventClass = LogEvent.EventClass;
  Event->EventInfo = LogEvent.EventInfo;
  Event->DataParam1 = LogEvent.DataParam1;
  Event->DataParam2 = LogEvent.DataParam2;
  Event->DataParam3 = LogEvent.DataParam3;
  Event->DataParam4 = LogEvent.DataParam4;

  AGESA_TESTPOINT (TpIfAmdReadEventLogExit, &Event->StdHeader);
  return Status;
}


/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function prepares the Event Log for use.
 *
 * Allocate the memory for an event log on the heap.  Set the read pointer, write pointer,
 * and count to reflect the log is empty.
 *
 * @param[in]  StdHeader      Our configuration, for passing to services.
 *
 * @retval      AGESA_SUCCESS     The event log is initialized.
 * @retval      AGESA_ERROR       Allocate Heap Buffer returned an error.
 *
 */
AGESA_STATUS
EventLogInitialization (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  ALLOCATE_HEAP_PARAMS  AllocateHeapParams;
  AGESA_STRUCT_BUFFER   *AgesaEventAlloc;
  AGESA_STATUS          Status;

  AllocateHeapParams.BufferHandle = EVENT_LOG_BUFFER_HANDLE;
  AllocateHeapParams.RequestedBufferSize = sizeof (AGESA_STRUCT_BUFFER);
  AllocateHeapParams.Persist = HEAP_SYSTEM_MEM;
  Status = HeapAllocateBuffer (&AllocateHeapParams, StdHeader);
  AgesaEventAlloc = (AGESA_STRUCT_BUFFER *) AllocateHeapParams.BufferPtr;
  AgesaEventAlloc->Count = 0;
  AgesaEventAlloc->ReadRecordPtr = 0;
  AgesaEventAlloc->WriteRecordPtr = 0;
  AgesaEventAlloc->ReadWriteFlag = 1;

  return Status;
}


/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function logs AGESA events into the event log.
 *
 * It will put the information in a circular buffer consisting of 16 such log
 * entries. If the buffer gets full, then the next event log entry will be written
 * over the oldest event log entry.
 *
 * @param[in]   EventClass   The severity of the event, its associated AGESA_STATUS.
 * @param[in]   EventInfo    Uniquely identifies the event.
 * @param[in]   DataParam1   Event specific additional data
 * @param[in]   DataParam2   Event specific additional data
 * @param[in]   DataParam3   Event specific additional data
 * @param[in]   DataParam4   Event specific additional data
 * @param[in]   StdHeader    Header for library and services
 *
 */
VOID
PutEventLog (
  IN       AGESA_STATUS EventClass,
  IN       UINT32 EventInfo,
  IN       UINT32 DataParam1,
  IN       UINT32 DataParam2,
  IN       UINT32 DataParam3,
  IN       UINT32 DataParam4,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT16 Index;
  AGESA_STRUCT_BUFFER *AgesaEventAlloc;

  AgesaEventAlloc = NULL;
  GetEventLogHeapPointer (&AgesaEventAlloc, StdHeader);
  ASSERT (AgesaEventAlloc != NULL);
  Index = AgesaEventAlloc->WriteRecordPtr;

  // Add the new event log data into a circular buffer
  AgesaEventAlloc->AgesaEventStruct[Index].AgesaEvent.EventClass = EventClass;
  AgesaEventAlloc->AgesaEventStruct[Index].AgesaEvent.EventInfo  = EventInfo;
  AgesaEventAlloc->AgesaEventStruct[Index].AgesaEvent.DataParam1 = DataParam1;
  AgesaEventAlloc->AgesaEventStruct[Index].AgesaEvent.DataParam2 = DataParam2;
  AgesaEventAlloc->AgesaEventStruct[Index].AgesaEvent.DataParam3 = DataParam3;
  AgesaEventAlloc->AgesaEventStruct[Index].AgesaEvent.DataParam4 = DataParam4;

 if ((AgesaEventAlloc->WriteRecordPtr == AgesaEventAlloc->ReadRecordPtr) &&
             (AgesaEventAlloc->ReadWriteFlag == 0)) {
    AgesaEventAlloc->WriteRecordPtr += 1;
    AgesaEventAlloc->ReadRecordPtr += 1;
    if (AgesaEventAlloc->WriteRecordPtr == TOTAL_EVENT_LOG_BUFFERS) {
      AgesaEventAlloc->WriteRecordPtr = 0;
      AgesaEventAlloc->ReadRecordPtr  = 0;
    }
  } else {
    AgesaEventAlloc->WriteRecordPtr += 1;
    if (AgesaEventAlloc->WriteRecordPtr == TOTAL_EVENT_LOG_BUFFERS) {
      AgesaEventAlloc->WriteRecordPtr = 0;
    }
    AgesaEventAlloc->ReadWriteFlag = 0;
  }
  AgesaEventAlloc->Count = AgesaEventAlloc->Count + 1;

  if (AgesaEventAlloc->Count <= TOTAL_EVENT_LOG_BUFFERS) {
    AgesaEventAlloc->AgesaEventStruct[Index].Count = Index;
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function gets event logs from the circular buffer.
 *
 * It will read the oldest entry from the circular buffer and place that information to the structure
 * pointed to by the parameter. The read pointers will be incremented to remove the entry from buffer
 * so that a subsequent call will return the next entry from the buffer. If the buffer is empty the
 * returned log event will have EventInfo zero, which is not a valid event id.
 *
 * @param[out]  EventRecord    The next log event.
 * @param[in]   StdHeader    Header for library and services
 *
 * @retval      AGESA_SUCCESS     Always succeeds.
 *
 */
AGESA_STATUS
GetEventLog (
     OUT   AGESA_EVENT *EventRecord,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT16 Index;
  AGESA_STRUCT_BUFFER *AgesaEventAlloc;

  AgesaEventAlloc = NULL;

  GetEventLogHeapPointer (&AgesaEventAlloc, StdHeader);
  ASSERT (AgesaEventAlloc != NULL);

  if ((AgesaEventAlloc->ReadRecordPtr == AgesaEventAlloc->WriteRecordPtr) &&
      (AgesaEventAlloc->ReadWriteFlag == 1)) {
    // EventInfo == zero, means no more data.
    LibAmdMemFill (EventRecord, 0, sizeof (AGESA_EVENT), StdHeader);
  } else {
    Index = AgesaEventAlloc->ReadRecordPtr;
    EventRecord->EventClass = AgesaEventAlloc->AgesaEventStruct[Index].AgesaEvent.EventClass;
    EventRecord->EventInfo  = AgesaEventAlloc->AgesaEventStruct[Index].AgesaEvent.EventInfo;
    EventRecord->DataParam1 = AgesaEventAlloc->AgesaEventStruct[Index].AgesaEvent.DataParam1;
    EventRecord->DataParam2 = AgesaEventAlloc->AgesaEventStruct[Index].AgesaEvent.DataParam2;
    EventRecord->DataParam3 = AgesaEventAlloc->AgesaEventStruct[Index].AgesaEvent.DataParam3;
    EventRecord->DataParam4 = AgesaEventAlloc->AgesaEventStruct[Index].AgesaEvent.DataParam4;
    if (AgesaEventAlloc->ReadRecordPtr == (TOTAL_EVENT_LOG_BUFFERS - 1)) {
      AgesaEventAlloc->ReadRecordPtr = 0;
    } else {
      AgesaEventAlloc->ReadRecordPtr = AgesaEventAlloc->ReadRecordPtr + 1;
    }
    if (AgesaEventAlloc->ReadRecordPtr == AgesaEventAlloc->WriteRecordPtr) {
      AgesaEventAlloc->ReadWriteFlag = 1;
    }
  }
  return (AGESA_SUCCESS);
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function gets event logs from the circular buffer without flushing the entry.
 *
 * It will read the desired entry from the circular buffer and place that information to the structure
 * pointed to by the parameter. The read pointers will not be incremented to remove the entry from the
 * buffer. If the buffer is empty, or the desired entry does not exist, FALSE will be returned.
 *
 * @param[out]  EventRecord  The next log event.
 * @param[in]   Index        Zero-based unread entry index
 * @param[in]   StdHeader    Header for library and services
 *
 * @retval      TRUE         Entry exists
 * @retval      FALSE        Entry does not exist
 *
 */
BOOLEAN
PeekEventLog (
     OUT   AGESA_EVENT *EventRecord,
  IN       UINT16 Index,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT16 ActualIndex;
  UINT16 UnreadEntries;
  AGESA_STRUCT_BUFFER *AgesaEventAlloc;

  AgesaEventAlloc = NULL;

  GetEventLogHeapPointer (&AgesaEventAlloc, StdHeader);
  ASSERT (AgesaEventAlloc != NULL);

  if ((AgesaEventAlloc->ReadRecordPtr == AgesaEventAlloc->WriteRecordPtr) &&
      (AgesaEventAlloc->ReadWriteFlag == 1)) {
    // EventInfo == zero, means no more data.
    return FALSE;
  }
  if (AgesaEventAlloc->ReadRecordPtr < AgesaEventAlloc->WriteRecordPtr) {
    UnreadEntries = AgesaEventAlloc->WriteRecordPtr - AgesaEventAlloc->ReadRecordPtr;
  } else {
    UnreadEntries = TOTAL_EVENT_LOG_BUFFERS - (AgesaEventAlloc->ReadRecordPtr - AgesaEventAlloc->WriteRecordPtr);
  }
  if (Index >= UnreadEntries) {
    return FALSE;
  }
  ActualIndex = Index + AgesaEventAlloc->ReadRecordPtr;
  if (ActualIndex >= TOTAL_EVENT_LOG_BUFFERS) {
    ActualIndex -= TOTAL_EVENT_LOG_BUFFERS;
  }

  EventRecord->EventClass = AgesaEventAlloc->AgesaEventStruct[ActualIndex].AgesaEvent.EventClass;
  EventRecord->EventInfo  = AgesaEventAlloc->AgesaEventStruct[ActualIndex].AgesaEvent.EventInfo;
  EventRecord->DataParam1 = AgesaEventAlloc->AgesaEventStruct[ActualIndex].AgesaEvent.DataParam1;
  EventRecord->DataParam2 = AgesaEventAlloc->AgesaEventStruct[ActualIndex].AgesaEvent.DataParam2;
  EventRecord->DataParam3 = AgesaEventAlloc->AgesaEventStruct[ActualIndex].AgesaEvent.DataParam3;
  EventRecord->DataParam4 = AgesaEventAlloc->AgesaEventStruct[ActualIndex].AgesaEvent.DataParam4;

  return TRUE;
}


/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function gets the Event Log pointer.
 *
 * It will locate the Event Log on the heap using the heap locate service.  If the Event
 * Log is not located, NULL is returned.
 *
 * @param[out]  EventLog  Pointer to the Event Log, or NULL.
 * @param[in]   StdHeader Our Configuration, for passing to services.
 *
 */
VOID
STATIC
GetEventLogHeapPointer (
     OUT   AGESA_STRUCT_BUFFER **EventLog,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  LOCATE_HEAP_PTR LocateHeapStruct;

  LocateHeapStruct.BufferHandle = EVENT_LOG_BUFFER_HANDLE;
  LocateHeapStruct.BufferPtr = NULL;
  HeapLocateBuffer (&LocateHeapStruct, StdHeader);
  *EventLog = (AGESA_STRUCT_BUFFER *)LocateHeapStruct.BufferPtr;
}
