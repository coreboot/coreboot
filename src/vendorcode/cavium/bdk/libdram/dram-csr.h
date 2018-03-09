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
 * Functions and macros for libdram access to CSR. These build
 * on the normal BDK functions to allow logging of CSRs based on
 * the libdram verbosity level. Internal use only.
 */

/**
 * Write a CSR, possibly logging it based on the verbosity
 * level. You should use DRAM_CSR_WRITE() as a convientent
 * wrapper.
 *
 * @param node
 * @param csr_name
 * @param type
 * @param busnum
 * @param size
 * @param address
 * @param value
 */
#ifdef DRAM_CSR_WRITE_INLINE
static inline void dram_csr_write(bdk_node_t node, const char *csr_name, bdk_csr_type_t type, int busnum, int size, uint64_t address, uint64_t value) __attribute__((always_inline));
static inline void dram_csr_write(bdk_node_t node, const char *csr_name, bdk_csr_type_t type, int busnum, int size, uint64_t address, uint64_t value)
{
    VB_PRT(VBL_CSRS, "N%d: DDR Config %s[%016lx] => %016lx\n", node, csr_name, address, value);
    bdk_csr_write(node, type, busnum, size, address, value);
}
#else
extern void dram_csr_write(bdk_node_t node, const char *csr_name, bdk_csr_type_t type, int busnum, int size, uint64_t address, uint64_t value);
#endif

/**
 * Macro to write a CSR, logging if necessary
 */
#define DRAM_CSR_WRITE(node, csr, value) \
    dram_csr_write(node, basename_##csr, bustype_##csr, busnum_##csr, sizeof(typedef_##csr), csr, value)

/**
 * Macro to make a read, modify, and write sequence easy. The "code_block"
 * should be replaced with a C code block or a comma separated list of
 * "name.s.field = value", without the quotes.
 */
#define DRAM_CSR_MODIFY(name, node, csr, code_block) do { \
        typedef_##csr name = {.u = bdk_csr_read(node, bustype_##csr, busnum_##csr, sizeof(typedef_##csr), csr)}; \
        code_block; \
        dram_csr_write(node, basename_##csr, bustype_##csr, busnum_##csr, sizeof(typedef_##csr), csr, name.u); \
    } while (0)

