/*****************************************************************************
 *
 * Copyright (c) 2013 - 2014, Sage Electronic Engineering, LLC
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
 ***************************************************************************/
#ifndef _FIELDACCESSORS_H_
#define _FIELDACCESSORS_H_

/// AGESA value name
typedef enum {
	AMD_GLOBAL_USER_OPTIONS    = 0x00020000,
	AMD_GLOBAL_NUM_SOCKETS     = 0x00020001,
	AMD_GLOBAL_NUM_MODULES     = 0x00020002,
} AGESA_FIELD_NAME;

typedef AGESA_STATUS (*SETTER_ENTRY) (
  IN OUT   VOID* value,
  IN       UINT32 size
  );

typedef AGESA_STATUS (*GETTER_ENTRY) (
  IN OUT   VOID** value,
  IN       UINT32 size
  );

/// Accessor Interface.
typedef struct {
  IN       AMD_CONFIG_PARAMS   StdHeader;          ///< Standard configuration header
  IN       AGESA_FIELD_NAME    FieldName;          ///< The service to init
  IN       ALLOCATION_METHOD   AllocationMethod;   ///< For pointers, how to allocate space for copied data
  IN OUT   VOID                *Struct;            ///< The struct for the service.
  IN OUT   UINT32              FieldSize;          ///< The size of the data value.
  IN OUT   VOID                *FieldValue;        ///< The value retrieved or set into the target structure.
} AMD_ACCESSOR_PARAMS;

/**********************************************************************
 * Interface call:  AmdSetValue
 **********************************************************************/
AGESA_STATUS
AmdSetValueDispatch (
  IN OUT   AMD_ACCESSOR_PARAMS *AccessorParams
  );

AGESA_STATUS
AmdSetValue (
  IN       CONST AGESA_FIELD_NAME name,
  IN OUT   VOID* value,
  IN       UINT32 size
  );

/**********************************************************************
 * Interface call:  AmdGetValue
 **********************************************************************/
AGESA_STATUS
AmdGetValueDispatch (
  IN OUT   AMD_ACCESSOR_PARAMS *AccessorParams
  );

AGESA_STATUS
AmdGetValue (
  IN       CONST AGESA_FIELD_NAME name,
  IN OUT   VOID** value,
  IN       UINT32 size
  );

/**
 * Dispatch Table.
 *
 * The push high dispatcher uses this table to find what entries are currently in the build image.
 */
typedef struct {
  UINT32              FunctionId;
  SETTER_ENTRY        SetValueEntryPoint;
  GETTER_ENTRY        GetValueEntryPoint;
} ACCESSOR_DISPATCH_TABLE;

AGESA_STATUS
GetUserOptions(
  IN OUT   VOID** value,
  IN       UINT32 size
  );

AGESA_STATUS
GetNumSockets(
  IN OUT   VOID** value,
  IN       UINT32 size
  );

AGESA_STATUS
GetNumModules(
  IN OUT   VOID** value,
  IN       UINT32 size
  );

#endif /* _FIELDACCESSORS_H_ */
