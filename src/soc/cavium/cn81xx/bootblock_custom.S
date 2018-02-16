/*
 * Early initialization code for aarch64 (a.k.a. armv8)
 *
 * Copyright 2016          Cavium, Inc. <support@cavium.com>
 * Copyright 2018-present  Facebook, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/asm.h>
#include <soc/addressmap.h>

ENTRY(_start)
    .org 0
     /**
     * According to the reference manual the first instruction is fetched from
     * offset 0x100, but at offset 0 a branch instruction is always placed.
     * Support two entry points for now.
     * To save memory put the cavium specific init code between those to entry
     * points.
     */
    ic      ialluis
    fmov    d30, x0     /* Save X0 in FPR for use later */
     /**
      * The BDK stores X1 for later use, but it turns out that we don't need
      * this "feature". The idea is to hide the devicetree somewhere in
      * flash, that only the ROM will find it and point to it using X1.
      */
    adr     x1, _start  /* x1 = _start location based on PC */
    fmov    d29, x1     /* Save PC in FPR for use later */

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    /* Change the core to big endian mode for EL3 */
    mrs     x0, SCTLR_EL3
    mov     x1, 1<<25       /* Set SCTLR_EL3[ee]=1 */
    orr     x0, x0, x1
    msr     SCTLR_EL3, x0
    #define ENDIAN_CONVERT64(reg) rev reg, reg
    #define ENDIAN_CONVERT32(reg) rev reg, reg
    #define ENDIAN_CONVERT16(reg) rev16 reg, reg
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    /* Nothing needed, default is little endian */
    #define ENDIAN_CONVERT64(reg)
    #define ENDIAN_CONVERT32(reg)
    #define ENDIAN_CONVERT16(reg)
#else
    #error Unknown endianness
#endif

    mov     x0, (LMC0_PF_BAR0 >> 32)
    lsl     x0, x0, 32
    mov     x1, (LMC0_PF_BAR0 & 0xffffffff)
    orr     x0, x0, x1

    /* Test if DRAM PLL is running */
    ldr     x1, [x0, LMC0_DDR_PLL_CTL0]

    tst     x1, 0x80

    b.ne     cache_setup_done

    bl      _setup_car

cache_setup_done:

    /* Check that we're running on the node we're linked for */
    mrs     x0, MPIDR_EL1
    ubfx    x0, x0, 16, 8       /* Bits 23:16 are the physical node ID */
    mov     x1, 0x0
    cmp     x0, x1

    b.ne    _wfi

node_check_done:
    /* Get code position */
    mov     x1, 0x020000
    mov     x0, BOOTROM_OFFSET
    add     x1, x0, x1

    adr     x0, _start

    /**
     * Check if IROM has loaded the code to BOOTROM_OFFSET.
     * In case the offset is wrong, try to relocate.
     * Ideally the following code is never executed.
     * FIXME: Add region overlap check.
     */
    cmp x0, x1
    b.eq    after_relocate

relocate:
    /* Get bootblock length */
    ldr     x2, =_program
    ldr     x3, =_eprogram
    sub     x2, x2, x3
    b       copy_code

.align 7
copy_code:
    ldp     q0, q1, [x1], 32    /* Load 32 bytes */
    subs    w2, w2, 32          /* Subtract 32 from length, setting flags */
    stp     q0, q1, [x0], 32    /* Store 32 bytes */
    b.gt    copy_code           /* Repeat if length is still positive */
    dmb     sy

    /* Load the actual location we're suppose to be at */
    adr     x0, after_relocate  /* Relative address */
    adr     x1, _start          /* Relative address */
    sub     x0, x0, x1     /* This only works if _start is suppose to be zero */
    mov     x1, BOOTROM_OFFSET
    add     x0, x0, x1
    br      x0             /* Branch to relocated code */

    ic      ialluis        /* Clear the icache now that all code is correct */

after_relocate:
    /* Allow unaligned memory access as long as MMU is disabled */
    mrs     x22, s3_0_c11_c0_4
    orr     x22, x22, # (1 << 37) /* Set DCVA47 */
    msr     s3_0_c11_c0_4, x22

    bl      start

    /* Real entry point */
    .org 0x100
    b      _start
ENDPROC(_start)


ENTRY(_setup_car)
    mrs     x0, MIDR_EL1
    ubfx    x0, x0, 4, 12       /* Bits 15:4 are the part number */
    cmp     x0, 0xb0
    b.ge    _wfi

thunder1_cache_setup:
   /**
    * Setup L2 cache to allow secure access to all of the address space
    * thunder1 compability list:
    * - CN81XX
    * - CN83XX
    * - CN88XX
    */
    #define REGIONX_START   0x1000
    #define REGIONX_END     0x1008
    #define REGIONX_ATTR    0x1010
    mov     x0, L2C_PF_BAR0 >> 32
    lsl     x0, x0, 32
    mov     x1, (L2C_PF_BAR0 & 0xffffffff)
    orr     x0, x0, x1
    str     xzr, [x0, REGIONX_START]    /* Start of zero */
    mov     x1, 0x3fffff00000           /* End of max address */
    ENDIAN_CONVERT64(x1)
    str     x1, [x0, REGIONX_END]
    mov     x1, 2                       /* Secure only access */
    ENDIAN_CONVERT64(x1)
    str     x1, [x0, REGIONX_ATTR]
    /* Update way partition to allow core 0 to write to L2 */
    #define L2C_WPAR_PP0_OFFSET 0x40000
    mov     x1, L2C_WPAR_PP0_OFFSET
    str     xzr, [x0, x1]
    ldr     xzr, [x0, x1]       /* Read back to make sure done */
    #undef REGIONX_START
    #undef REGIONX_END
    #undef REGIONX_ATTR
    #undef L2C_WPAR_PP0_OFFSET

    /**
     * At this point the whole CAR is readable and writeable, but if
     * we touch to many cache-lines our code might get flushed out.
     * We have to lock all cache-lines that are to be used as RAM, which are
     * the ones marked as SRAM in memlayout.
     */
    mrs     x0, CTR_EL0            /* Get cache-line size */
    /* [19:16] - Indicates (Log2(number of words in cache line) */
    ubfx    x0, x0, 16, 4
    mov     x1, 4                  /* Bytes in a word (32-bit) */
    lsl     x0, x1, x0             /* Number of Bytes in x0 */

    sub     x1, x0, 1
    mvn     x1, x1                 /* Place mask in x1 */

    ldr     x3, =_sram
    and     x3, x3, x1             /* Align addresses with cache-lines */
    ldr     x4, =_esram
    add     x4, x4, x0
    sub     x4, x4, 1
    and     x4, x4, x1             /* Align addresses with cache-lines */
    sub     x2, x4, x3             /* Store sram length in x2 */

lock_cache_lines:
    sys     #0, c11, c1, #4, x3
    add     x3, x3, x0             /* Increment address by cache-line bytes */
    subs    w2, w2, w0             /* Subtract cache-line bytes from length */
    b.gt    lock_cache_lines       /* Repeat if length is still positive */

    /**
     * The locked region isn't considered dirty by L2. Do read/write of
     * each cache line to force each to be dirty. This is needed across the
     * whole line to make sure the L2 dirty bits are all up to date.
     * NOTE: If we'd relocate we could memset the whole memory !
     */
    ldr     x3, =_sram
    and     x3, x3, x1             /* Align addresses with cache-lines */
    ldr     x4, =_esram
    add     x4, x4, x0
    sub     x4, x4, 1
    and     x4, x4, x1             /* Align addresses with cache-lines */
    sub     x2, x4, x3             /* Store sram length in x2 */
    mov     x4, x3
    b       dirty_cache_line

.align 7
dirty_cache_line:
    ldp     q0, q1, [x3], 32    /* Load 32 bytes */
    subs    w2, w2, 32          /* Subtract 32 from length, setting flags */
    stp     q0, q1, [x4], 32    /* Store 32 bytes */
    b.gt    dirty_cache_line    /* Repeat if length is still positive */
    dmb     sy

clear_interrupts:
    /**
     * As the memory controller isn't running, but we access the DRAM's
     * address space, some interrupt flags had been set.
     * Tidy up our mess now on (valid for CN81XX only).
     */
    mov     x0, (L2C_TAD0_INT_W1C >> 32)
    lsl     x0, x0, 32
    mov     x1, (L2C_TAD0_INT_W1C & 0xffffffff)
    orr     x0, x0, x1

    ldr x1, [x0]
    orr x1, x1, 0x1c00 /* Clear WRDISLMC, RDDISLMC, RDNXM */
    str x1, [x0]

    ret
ENDPROC(_setup_car)

ENTRY(_wfi)
    wfi
ENDPROC(_wfi)

ENTRY(start)
    bl      arm64_init_cpu

    fmov    x0, d30 /* The original X0, info from previous image */
    fmov    x1, d29 /* The original PC we were loaded at */

    /* Call C entry */
    bl      bootblock_main

ENDPROC(start)
