/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef MT6516_MEMORY_H
#define MT6516_MEMORY_H

#include <stdint.h>

/**************************************************************************
*  DEBUG CONTROL
**************************************************************************/
#define MEM_TEST                (1)

// do not change the test size !!!!
#define MEM_TEST_SIZE           (0x2000)

/**************************************************************************
*  DRAM SIZE
**************************************************************************/
#define E1_DRAM_SIZE            (0x10000000)
#define E2_DRAM_SIZE            (0x08000000)

/**************************************************************************
*  EXPOSED API
**************************************************************************/
extern u32 mt6516_get_hardware_ver (void);
extern void mt6516_mem_init (void);

#if MEM_TEST
extern int complex_mem_test (unsigned int start, unsigned int len);
#endif

#endif
