#ifndef __CB_BDK_CSR_H__
#define __CB_BDK_CSR_H__
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

//#include <libbdk-arch/bdk-csrs-rst.h>	/* FIXME: circular dependency with this header */
#include <libbdk-hal/bdk-clock.h>	/* FIXME(dhendrix): added */

/**
 * @file
 *
 * Functions and macros for accessing Cavium CSRs.
 *
 * <hr>$Revision: 49448 $<hr>
 *
 * @defgroup csr CSR support
 * @{
 */

/**
 * Possible CSR bus types
 */
typedef enum {
   BDK_CSR_TYPE_DAB,            /**< External debug 64bit CSR */
   BDK_CSR_TYPE_DAB32b,         /**< External debug 32bit CSR */
   BDK_CSR_TYPE_MDSB,           /**< CN93XX: Memory Diagnostic Serial Bus?, not memory mapped */
   BDK_CSR_TYPE_NCB,            /**< Fast 64bit CSR */
   BDK_CSR_TYPE_NCB32b,         /**< Fast 32bit CSR */
   BDK_CSR_TYPE_PCCBR,
   BDK_CSR_TYPE_PCCPF,
   BDK_CSR_TYPE_PCCVF,
   BDK_CSR_TYPE_PCICONFIGRC,    /**< PCIe config address (RC mode) */
   BDK_CSR_TYPE_PCICONFIGEP,    /**< PCIe config address (EP mode) */
   BDK_CSR_TYPE_PCICONFIGEP_SHADOW, /**< CN93XX: PCIEP register invisible to host, not memory mapped */
   BDK_CSR_TYPE_PCICONFIGEPVF,  /**< CN93XX: PCIEP registers only on vertain PEMs, not memory mapped */
   BDK_CSR_TYPE_PEXP,           /**< PCIe BAR 0 address only */
   BDK_CSR_TYPE_PEXP_NCB,       /**< NCB-direct and PCIe BAR0 address */
   BDK_CSR_TYPE_RSL,            /**< Slow 64bit CSR */
   BDK_CSR_TYPE_RSL32b,         /**< Slow 32bit CSR */
   BDK_CSR_TYPE_RVU_PF_BAR0,    /**< Index into RVU PF BAR0 */
   BDK_CSR_TYPE_RVU_PF_BAR2,    /**< Index into RVU PF BAR2 */
   BDK_CSR_TYPE_RVU_PFVF_BAR2,  /**< Index into RVU PF or VF BAR2 */
   BDK_CSR_TYPE_RVU_VF_BAR2,    /**< Index into RVU VF BAR2 */
   BDK_CSR_TYPE_SYSREG,         /**< Core system register */
} bdk_csr_type_t;

#define BDK_CSR_DB_MAX_PARAM 4
typedef struct __attribute__ ((packed)) {
    uint32_t        name_index : 20;/**< Index into __bdk_csr_db_string where the name is */
    uint32_t        base_index : 14;/**< Index into __bdk_csr_db_number where the base address is */
    uint8_t         unused : 5;
    bdk_csr_type_t  type : 5;       /**< Enum type from above */
    uint8_t         width : 4;      /**< CSR width in bytes */
    uint16_t        field_index;    /**< Index into __bdk_csr_db_fieldList where the fields start */
    uint16_t        range[BDK_CSR_DB_MAX_PARAM]; /**< Index into __bdk_csr_db_range where the range is */
    uint16_t        param_inc[BDK_CSR_DB_MAX_PARAM]; /**< Index into __bdk_csr_db_number where the param multiplier is */
} __bdk_csr_db_type_t;

typedef struct __attribute__ ((packed)) {
    uint32_t        name_index : 20;/**< Index into __bdk_csr_db_string where the name is */
    uint32_t        start_bit : 6;  /**< LSB of the field */
    uint32_t        stop_bit : 6;   /**< MSB of the field */
} __bdk_csr_db_field_t;

typedef struct {
    uint32_t model;
    const int16_t *data;            /**< Array of integers indexing __bdk_csr_db_csr */
} __bdk_csr_db_map_t;

extern void __bdk_csr_fatal(const char *name, int num_args, unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4) __attribute__ ((noreturn));
extern int bdk_csr_decode(const char *name, uint64_t value);
extern int bdk_csr_field(const char *csr_name, int field_start_bit, const char **field_name);
extern uint64_t bdk_csr_read_by_name(bdk_node_t node, const char *name);
extern int bdk_csr_write_by_name(bdk_node_t node, const char *name, uint64_t value);
extern int __bdk_csr_lookup_index(const char *name, int params[]);
extern int bdk_csr_get_name(const char *last_name, char *buffer);
struct bdk_readline_tab;
extern struct bdk_readline_tab *__bdk_csr_get_tab_complete(void) BDK_WEAK;
extern uint64_t bdk_sysreg_read(int node, int core, uint64_t regnum);
extern void bdk_sysreg_write(int node, int core, uint64_t regnum, uint64_t value);

#ifndef BDK_BUILD_HOST

/**
 * Read a value from a CSR. Normally this function should not be
 * used directly. Instead use the macro BDK_CSR_READ that fills
 * in the parameters to this function for you.
 *
 * @param node    Node to use in a Numa setup. Can be an exact ID or a special value.
 * @param type    Bus type the CSR is on
 * @param busnum  Bus number the CSR is on
 * @param size    Width of the CSR in bytes
 * @param address The address of the CSR
 *
 * @return The value of the CSR
 */
/* FIXME(dhendrix): Moved __bdk_csr_read_slow out of the function body... */
extern uint64_t __bdk_csr_read_slow(bdk_node_t node, bdk_csr_type_t type, int busnum, int size, uint64_t address);
static inline uint64_t bdk_csr_read(bdk_node_t node, bdk_csr_type_t type, int busnum, int size, uint64_t address) __attribute__ ((always_inline));
static inline uint64_t bdk_csr_read(bdk_node_t node, bdk_csr_type_t type, int busnum, int size, uint64_t address)
{
    switch (type)
    {
        case BDK_CSR_TYPE_DAB:
        case BDK_CSR_TYPE_DAB32b:
        case BDK_CSR_TYPE_NCB:
        case BDK_CSR_TYPE_NCB32b:
        case BDK_CSR_TYPE_PEXP_NCB:
        case BDK_CSR_TYPE_RSL:
        case BDK_CSR_TYPE_RSL32b:
        case BDK_CSR_TYPE_RVU_PF_BAR0:
        case BDK_CSR_TYPE_RVU_PF_BAR2:
        case BDK_CSR_TYPE_RVU_PFVF_BAR2:
        case BDK_CSR_TYPE_RVU_VF_BAR2:
            address |= (uint64_t)(node&3) << 44;
            /* Note: This code assume a 1:1 mapping of all of address space.
               It is designed to run with the MMU disabled */
            switch (size)
            {
                case 1:
                    return *(volatile uint8_t *)address;
                case 2:
                    return bdk_le16_to_cpu(*(volatile uint16_t *)address);
                case 4:
                    return bdk_le32_to_cpu(*(volatile uint32_t *)address);
                default:
                    return bdk_le64_to_cpu(*(volatile uint64_t *)address);
            }
        default:
            return __bdk_csr_read_slow(node, type, busnum, size, address);
    }
}


/**
 * Wrate a value to a CSR. Normally this function should not be
 * used directly. Instead use the macro BDK_CSR_WRITE that fills
 * in the parameters to this function for you.
 *
 * @param node    Node to use in a Numa setup. Can be an exact ID or a special value.
 * @param type    Bus type the CSR is on
 * @param busnum  Bus number the CSR is on
 * @param size    Width of the CSR in bytes
 * @param address The address of the CSR
 * @param value   Value to write to the CSR
 */
/* FIXME(dhendrix): Moved __bdk_csr_write_slow out of the function body... */
extern void __bdk_csr_write_slow(bdk_node_t node, bdk_csr_type_t type, int busnum, int size, uint64_t address, uint64_t value);
static inline void bdk_csr_write(bdk_node_t node, bdk_csr_type_t type, int busnum, int size, uint64_t address, uint64_t value) __attribute__ ((always_inline));
static inline void bdk_csr_write(bdk_node_t node, bdk_csr_type_t type, int busnum, int size, uint64_t address, uint64_t value)
{
    switch (type)
    {
        case BDK_CSR_TYPE_DAB:
        case BDK_CSR_TYPE_DAB32b:
        case BDK_CSR_TYPE_NCB:
        case BDK_CSR_TYPE_NCB32b:
        case BDK_CSR_TYPE_PEXP_NCB:
        case BDK_CSR_TYPE_RSL:
        case BDK_CSR_TYPE_RSL32b:
        case BDK_CSR_TYPE_RVU_PF_BAR0:
        case BDK_CSR_TYPE_RVU_PF_BAR2:
        case BDK_CSR_TYPE_RVU_PFVF_BAR2:
        case BDK_CSR_TYPE_RVU_VF_BAR2:
            address |= (uint64_t)(node&3) << 44;
            /* Note: This code assume a 1:1 mapping of all of address space.
               It is designed to run with the MMU disabled */
            switch (size)
            {
                case 1:
                    *(volatile uint8_t *)address = value;
                    break;
                case 2:
                    *(volatile uint16_t *)address = bdk_cpu_to_le16(value);
                    break;
                case 4:
                    *(volatile uint32_t *)address = bdk_cpu_to_le32(value);
                    break;
                default:
                    *(volatile uint64_t *)address = bdk_cpu_to_le64(value);
                    break;
            }
            break;

        default:
            __bdk_csr_write_slow(node, type, busnum, size, address, value);
    }
}

#else
#define bdk_csr_read thunder_remote_read_csr
#define bdk_csr_write thunder_remote_write_csr
#endif

/**
 * This macro makes it easy to define a variable of the correct
 * type for a CSR.
 */
#define BDK_CSR_DEFINE(name, csr) typedef_##csr name

/**
 * This macro makes it easy to define a variable and initialize it
 * with a CSR.
 */
#define BDK_CSR_INIT(name, node, csr) typedef_##csr name = {.u = bdk_csr_read(node, bustype_##csr, busnum_##csr, sizeof(typedef_##csr), csr)}

/**
 * Macro to read a CSR
 */
#define BDK_CSR_READ(node, csr) bdk_csr_read(node, bustype_##csr, busnum_##csr, sizeof(typedef_##csr), csr)

/**
 * Macro to write a CSR
 */
#define BDK_CSR_WRITE(node, csr, value) bdk_csr_write(node, bustype_##csr, busnum_##csr, sizeof(typedef_##csr), csr, value)

/**
 * Macro to make a read, modify, and write sequence easy. The "code_block"
 * should be replaced with a C code block or a comma separated list of
 * "name.s.field = value", without the quotes.
 */
#define BDK_CSR_MODIFY(name, node, csr, code_block) do { \
        uint64_t _tmp_address = csr; \
        typedef_##csr name = {.u = bdk_csr_read(node, bustype_##csr, busnum_##csr, sizeof(typedef_##csr), _tmp_address)}; \
        code_block; \
        bdk_csr_write(node, bustype_##csr, busnum_##csr, sizeof(typedef_##csr), _tmp_address, name.u); \
    } while (0)

/**
 * This macro spins on a field waiting for it to reach a value. It
 * is common in code to need to wait for a specific field in a CSR
 * to match a specific value. Conceptually this macro expands to:
 *
 * 1) read csr at "address" with a csr typedef of "type"
 * 2) Check if ("type".s."field" "op" "value")
 * 3) If #2 isn't true loop to #1 unless too much time has passed.
 */
/* FIXME(dhendrix): removed bdk_thread_yield() */
#if 0
#define BDK_CSR_WAIT_FOR_FIELD(node, csr, field, op, value, timeout_usec) \
    ({int result;                                                       \
    do {                                                                \
        uint64_t done = bdk_clock_get_count(BDK_CLOCK_TIME) + (uint64_t)timeout_usec * \
                        bdk_clock_get_rate(bdk_numa_local(), BDK_CLOCK_TIME) / 1000000;   \
        typedef_##csr c;                                                \
        uint64_t _tmp_address = csr;                                    \
        while (1)                                                       \
        {                                                               \
            c.u = bdk_csr_read(node, bustype_##csr, busnum_##csr, sizeof(typedef_##csr), _tmp_address); \
            if ((c.s.field) op (value)) {                               \
                result = 0;                                             \
                break;                                                  \
            } else if (bdk_clock_get_count(BDK_CLOCK_TIME) > done) {    \
                result = -1;                                            \
                break;                                                  \
            } else                                                      \
                bdk_thread_yield();                                     \
        }                                                               \
    } while (0);                                                        \
    result;})
#endif
#define BDK_CSR_WAIT_FOR_FIELD(node, csr, field, op, value, timeout_usec) \
    ({int result;                                                       \
    do {                                                                \
        uint64_t done = bdk_clock_get_count(BDK_CLOCK_TIME) + (uint64_t)timeout_usec * \
                        bdk_clock_get_rate(bdk_numa_local(), BDK_CLOCK_TIME) / 1000000;   \
        typedef_##csr c;                                                \
        uint64_t _tmp_address = csr;                                    \
        while (1)                                                       \
        {                                                               \
            c.u = bdk_csr_read(node, bustype_##csr, busnum_##csr, sizeof(typedef_##csr), _tmp_address); \
            if ((c.s.field) op (value)) {                               \
                result = 0;                                             \
                break;                                                  \
            } else if (bdk_clock_get_count(BDK_CLOCK_TIME) > done) {    \
                result = -1;                                            \
                break;                                                  \
            }                                                           \
        }                                                               \
    } while (0);                                                        \
    result;})

/**
 * This macro spins on a field waiting for it to reach a value. It
 * is common in code to need to wait for a specific field in a CSR
 * to match a specific value. Conceptually this macro expands to:
 *
 * 1) read csr at "address" with a csr typedef of "type"
 * 2) Check if ("type"."chip"."field" "op" "value")
 * 3) If #2 isn't true loop to #1 unless too much time has passed.
 *
 * Note that usage of this macro should be avoided. When future chips
 * change bit locations, the compiler will not catch those changes
 * with this macro. Changes silently do the wrong thing at runtime.
 */
/* FIXME(dhendrix): removed bdk_thread_yield() */
#if 0
#define BDK_CSR_WAIT_FOR_CHIP_FIELD(node, csr, chip, field, op, value, timeout_usec) \
    ({int result;                                                       \
    do {                                                                \
        uint64_t done = bdk_clock_get_count(BDK_CLOCK_TIME) + (uint64_t)timeout_usec * \
                        bdk_clock_get_rate(bdk_numa_local(), BDK_CLOCK_TIME) / 1000000;   \
        typedef_##csr c;                                                \
        uint64_t _tmp_address = csr;                                    \
        while (1)                                                       \
        {                                                               \
            c.u = bdk_csr_read(node, bustype_##csr, busnum_##csr, sizeof(typedef_##csr), _tmp_address); \
            if ((c.chip.field) op (value)) {                            \
                result = 0;                                             \
                break;                                                  \
            } else if (bdk_clock_get_count(BDK_CLOCK_TIME) > done) {    \
                result = -1;                                            \
                break;                                                  \
            } else                                                      \
                bdk_thread_yield();                                     \
        }                                                               \
    } while (0);                                                        \
    result;})
#endif
#define BDK_CSR_WAIT_FOR_CHIP_FIELD(node, csr, chip, field, op, value, timeout_usec) \
    ({int result;                                                       \
    do {                                                                \
        uint64_t done = bdk_clock_get_count(BDK_CLOCK_TIME) + (uint64_t)timeout_usec * \
                        bdk_clock_get_rate(bdk_numa_local(), BDK_CLOCK_TIME) / 1000000;   \
        typedef_##csr c;                                                \
        uint64_t _tmp_address = csr;                                    \
        while (1)                                                       \
        {                                                               \
            c.u = bdk_csr_read(node, bustype_##csr, busnum_##csr, sizeof(typedef_##csr), _tmp_address); \
            if ((c.chip.field) op (value)) {                            \
                result = 0;                                             \
                break;                                                  \
            } else if (bdk_clock_get_count(BDK_CLOCK_TIME) > done) {    \
                result = -1;                                            \
                break;                                                  \
            }                                                           \
        }                                                               \
    } while (0);                                                        \
    result;})

/** @} */
#endif
