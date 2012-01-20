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
 */

// helper.c - these functions are compiled separately because they redefine
//            functions invoked directly by the compiler code generator.
//            The Microsoft tools do not allow such functions to be compiled
//            with the "Enable link-time code generation (/GL)" option. Compile
//            this module without /GL to avoid a build failure LNK1237.
//

#if defined (_MSC_VER)

#include "Porting.h"

//---------------------------------------------------------------------------
void *memcpy (void *dest, const void *src, size_t bytes)
   {
   // Rep movsb is faster than a byte loop, but still quite slow
   // for large operations. However, it is a good choice here because
   // this function is intended for use by the compiler only. For
   // large copy operations, call LibAmdMemCopy.
   __movsb (dest, src, bytes);
   return dest;
   }

//---------------------------------------------------------------------------

void *memset (void *dest, int value, size_t bytes)
   {
   // Rep stosb is faster than a byte loop, but still quite slow
   // for large operations. However, it is a good choice here because
   // this function is intended for use by the compiler only. For
   // large fill operations, call LibAmdMemFill.
   __stosb (dest, value, bytes);
   return dest;
   }
//---------------------------------------------------------------------------

#endif