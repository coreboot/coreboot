/***********************license start***********************************
* Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
* reserved.
*
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*
*   * Neither the name of Cavium Inc. nor the names of
*     its contributors may be used to endorse or promote products
*     derived from this software without specific prior written
*     permission.
*
* This Software, including technical data, may be subject to U.S. export
* control laws, including the U.S. Export Administration Act and its
* associated regulations, and may be subject to export or import
* regulations in other countries.
*
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND CAVIUM INC. MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
* TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
* REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
* DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
* PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT,
* QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE  RISK
* ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
***********************license end**************************************/

/**
 * @file
 *
 * This is file defines ASM primitives for the executive.

 * <hr>$Revision: 53373 $<hr>
 *
 * @defgroup asm Assembly support
 * @{
 */

/* This header file can be included from a .S file.  Keep non-preprocessor
   things under !__ASSEMBLER__.  */
#ifndef __ASSEMBLER__

/* turn the variable name into a string */
#define __BDK_TMP_STR(x) __BDK_TMP_STR2(x)
#define __BDK_TMP_STR2(x) #x
#define __BDK_VASTR(...) #__VA_ARGS__

#define BDK_MRS_NV(reg, val) asm ("mrs %x[rd]," #reg : [rd] "=r" (val))
#define BDK_MRS(reg, val) asm volatile ("mrs %x[rd]," #reg : [rd] "=r" (val))
#define BDK_MSR(reg, val) asm volatile ("msr " #reg ",%x[rd]" : : [rd] "r" (val))

/* other useful stuff */
#define BDK_MB          asm volatile ("dmb sy"      : : :"memory") /* Full memory barrier, like MIPS SYNC */
#define BDK_WMB         asm volatile ("dmb st"      : : :"memory") /* Write memory barreir, like MIPS SYNCW */
#define BDK_WFE         asm volatile ("wfe"         : : :"memory") /* Wait for event */
#define BDK_SEV         asm volatile ("sev"         : : :"memory") /* Send global event */
#define BDK_DSB         asm volatile ("dsb sy"      : : :"memory") /* DSB */

// normal prefetches that use the pref instruction
#define BDK_PREFETCH_PREFX(type, address, offset) asm volatile ("PRFUM " type ", [%[rbase],%[off]]" : : [rbase] "r" (address), [off] "I" (offset))
// a normal prefetch
#define BDK_PREFETCH(address, offset) BDK_PREFETCH_PREFX("PLDL1KEEP", address, offset)
#define BDK_ICACHE_INVALIDATE  { asm volatile ("ic iallu" : : ); }    // invalidate entire icache

#define BDK_SYS_CVMCACHE_WBI_L2 "#0,c11,c1,#2"          // L2 Cache Cache Hit Writeback Invalidate
#define BDK_SYS_CVMCACHE_WB_L2 "#0,c11,c1,#3"           // L2 Cache Hit Writeback
#define BDK_SYS_CVMCACHE_LCK_L2 "#0,c11,c1,#4"          // L2 Cache Fetch and Lock
#define BDK_SYS_CVMCACHE_WBI_L2_INDEXED "#0,c11,c0,#5"  // L2 Cache Index Writeback Invalidate
#define BDK_SYS_CVMCACHE_LTG_L2_INDEXED "#0,c11,c0,#7"  // L2 Cache Index Load Tag
#define BDK_SYS_CVMCACHE_INVALL_DC "#0,c11,c0,#2"       // L1 Dcache Invalidate
#define BDK_CACHE_WBI_L2(address) { asm volatile ("sys " BDK_SYS_CVMCACHE_WBI_L2 ", %0" : : "r" (address)); } // Push to memory, invalidate, and unlock
#define BDK_CACHE_WBI_L2_INDEXED(encoded) { asm volatile ("sys " BDK_SYS_CVMCACHE_WBI_L2_INDEXED ", %0" : : "r" (encoded)); } // Push to memory, invalidate, and unlock, index by set/way
#define BDK_CACHE_WB_L2(address) { asm volatile ("sys " BDK_SYS_CVMCACHE_WB_L2 ", %0" : : "r" (address)); } // Push to memory, don't invalidate, don't unlock
#define BDK_CACHE_LCK_L2(address) { asm volatile ("sys " BDK_SYS_CVMCACHE_LCK_L2 ", %0" : : "r" (address)); } // Lock into L2
#define BDK_DCACHE_INVALIDATE { asm volatile ("sys " BDK_SYS_CVMCACHE_INVALL_DC ", xzr"); } // Invalidate the entire Dcache on local core
#define BDK_CACHE_LTG_L2_INDEXED(encoded) { asm volatile ("sys " BDK_SYS_CVMCACHE_LTG_L2_INDEXED ", %0" : : "r" (encoded)); } // Load L2 TAG, index by set/way 

#define BDK_STORE_PAIR(ptr, data1, data2) { asm volatile ("stp %x[d1], %x[d2], [%[b]]" : [mem] "+m" (*(__uint128_t*)ptr) : [b] "r" (ptr), [d1] "r" (data1), [d2] "r" (data2)); }

#endif	/* __ASSEMBLER__ */

/** @} */
