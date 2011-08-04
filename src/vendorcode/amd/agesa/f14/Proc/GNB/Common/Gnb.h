/* $NoKeywords:$ */
/**
 * @file
 *
 * Misc common definition
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 44325 $   @e \$Date: 2010-12-22 03:29:53 -0700 (Wed, 22 Dec 2010) $
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
#ifndef _GNB_H_
#define _GNB_H_


#define GNB_DEADLOOP() \
{ \
  VOLATILE BOOLEAN k; \
  k = TRUE; \
  while (k) { \
  } \
}
#ifdef IDSOPT_TRACING_ENABLED
  #if (IDSOPT_TRACING_ENABLED == TRUE)
    #define GNB_TRACE_ENABLE
  #endif
#endif


#ifndef GNB_DEBUG_CODE
  #ifdef GNB_TRACE_ENABLE
    #define  GNB_DEBUG_CODE(Code) Code
  #else
    #define  GNB_DEBUG_CODE(Code)
  #endif
#endif

#ifndef MIN
#define MIN(x, y) (((x) > (y))? (y):(x))
#endif

#ifndef MAX
#define MAX(x, y) (((x) > (y))? (x):(y))
#endif

#define OFF 0

#define GnbLibGetHeader(x)  ((AMD_CONFIG_PARAMS*) (x)->StdHeader)

#define AGESA_STATUS_UPDATE(Current, Aggregated) \
if (Current > Aggregated) { \
  Aggregated = Current; \
}

#ifndef offsetof
  #define offsetof(s, m) (UINTN)&(((s *)0)->m)
#endif

/// Power gaiter data setting (do not change this structure definition)
typedef struct {
  UINT16  MothPsoPwrup;                   ///< Mother Timer Powerup
  UINT16  MothPsoPwrdn;                   ///< Mother Timer Powerdown
  UINT16  DaugPsoPwrup;                   ///< Daughter Timer Powerup
  UINT16  DaugPsoPwrdn;                   ///< Daughter Timer Powerdown
  UINT16  ResetTimer;                     ///< Reset Timer
  UINT16  IsoTimer;                       ///< Isolation Timer
} POWER_GATE_DATA;

#define GNB_STRINGIZE(x)                  #x
#define GNB_SERVICE_DEFINITIONS(x)        GNB_STRINGIZE (Services/x/x.h)
#define GNB_MODULE_DEFINITIONS(x)         GNB_STRINGIZE (Modules/x/x.h)

#endif
