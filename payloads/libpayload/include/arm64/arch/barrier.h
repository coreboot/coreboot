/*
 *
 * Copyright 2014 Google Inc.
 * Copyright (C) 2003-2004 Olivier Houchard
 * Copyright (C) 1994-1997 Mark Brinicombe
 * Copyright (C) 1994 Brini
 * All rights reserved.
 *
 * This code is derived from software written for Brini by Mark Brinicombe
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef __ARCH_BARRIER_H__
#define __ARCH_BARRIER_H__

#include <arch/cache.h>

/*
 * Description of different memory barriers introduced:
 *
 * Memory barrier(mb) - Guarantees that all memory accesses specified before the
 * barrier will happen before all memory accesses specified after the barrier
 *
 * Read memory barrier (rmb) - Guarantees that all read memory accesses
 * specified before the barrier will happen before all read memory accesses
 * specified after the barrier
 *
 * Write memory barrier (wmb) - Guarantees that all write memory accesses
 * specified before the barrier will happen before all write memory accesses
 *  specified after the barrier
 */

/*
 * According to ARMv8 Instruction Set Overview:
 * Options specified to dmb instruction have the following meaning:
 * Option      Ordered accesses
 *   sy          any-any
 *   ld          load-load, load-store
 *   st          store-store
 */

#define mb()       dmb_opt(sy)
#define rmb()      dmb_opt(ld)
#define wmb()      dmb_opt(st)

#endif /* __ARCH_BARRIER_H__ */
