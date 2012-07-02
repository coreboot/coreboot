/* $NoKeywords:$ */
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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*****************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

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
  Event->StdHeader.HeapBasePtr = HeapGetBaseAddress (&Event->StdHeader);
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

  IDS_HDT_CONSOLE (MAIN_FLOW, "\n * %s Event: %08x Data: %x, %x, %x, %x\n\n",
                    (EventClass == AGESA_FATAL)       ? "FATAL"       :
                    (EventClass == AGESA_CRITICAL)    ? "CRITICAL"    :
                    (EventClass == AGESA_ERROR)       ? "ERROR"       :
                    (EventClass == AGESA_WARNING)     ? "WARNING"     :
                    (EventClass == AGESA_ALERT)       ? "ALERT"       :
                    (EventClass == AGESA_BOUNDS_CHK)  ? "BOUNDS_CHK"  :
                    (EventClass == AGESA_UNSUPPORTED) ? "UNSUPPORTED" :
                    "SUCCESS", EventInfo, DataParam1, DataParam2, DataParam3, DataParam4);

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
  if ((HeapLocateBuffer (&LocateHeapStruct, StdHeader)) == AGESA_SUCCESS) {
    *EventLog = (AGESA_STRUCT_BUFFER *)LocateHeapStruct.BufferPtr;
  } else {
    *EventLog = NULL;
  }
}
