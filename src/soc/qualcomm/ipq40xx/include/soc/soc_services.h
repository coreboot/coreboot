/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_IPQ40XX_INCLUDE_SOC_SOC_SERVICES_H__
#define __SOC_QUALCOMM_IPQ40XX_INCLUDE_SOC_SOC_SERVICES_H__

#include <types.h>

extern u8 _memlayout_cbmem_top[];

/* Returns zero on success, nonzero on failure. */
int initialize_dram(void);

/* Loads and runs TZBSP, switches into user mode. */
void start_tzbsp(void);

/* A helper function needed to start TZBSP properly. */
int tz_init_wrapper(int, int, void *);

/* Load RPM code into memory and trigger its execution. */
void start_rpm(void);

/* Mark cbmem backing store as ready. */
void ipq_cbmem_backing_store_ready(void);

#endif
