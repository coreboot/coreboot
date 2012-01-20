/* $NoKeywords:$ */
/**
 * @file
 *
 * mport.h
 *
 * API's to support different OS
 *
 * A detailed description, giving important information about this file.
 * Omit the detailed description if none is needed.  For other than the
 * simplest files, there should be one.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Memory
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 02:16:51 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*
 *****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *===========================================================================
 *  AMD Revision History
 *  Initial Version
 *
 */
#ifndef _MPORT_H_
#define _MPORT_H_

///< 64 bit data structure
///< lo - Lower 32 bits
///< hi - Upper 32 bits
typedef struct _S_UINT64 {
  UINT32 lo; ///< Lower 32 bits
  UINT32 hi; ///< Upper 32 bits
} S_UINT64;
/*
 *   SBDFO - Segment Bus Device Function Offset
 *   31:28   Segment (4-bits)
 *   27:20   Bus     (8-bits)
 *   19:15   Device  (5-bits)
 *   14:12   Function(3-bits)
 *   11:00   Offset  (12-bits)
 */
typedef UINT32 SBDFO;

//#define MAKE_SBDFO(seg,bus,dev,fun,off) ((((UINT32)(seg))<<28) | (((UINT32)(bus))<<20) | \
//                   (((UINT32)(dev))<<15) | (((UINT32)(fun))<<12) | ((UINT32)(off)))
//#define SBDFO_SEG(x) (((UINT32)(x)>>28) & 0x0F)
//#define SBDFO_BUS(x) (((UINT32)(x)>>20) & 0xFF)
//#define SBDFO_DEV(x) (((UINT32)(x)>>15) & 0x1F)
//#define SBDFO_FUN(x) (((UINT32)(x)>>12) & 0x07)
//#define SBDFO_OFF(x) (((UINT32)(x)) & 0xFFF)
//#define ILLEGAL_SBDFO 0xFFFFFFFF


#define GET_SIZE_OF(x)   (sizeof (x) / sizeof (x[0]))

#endif /* _MPORT_H_ */
