/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_CPU_H
#define AMD_BLOCK_CPU_H

void early_cache_setup(void);
int get_cpu_count(void);
void set_cstate_io_addr(void);
void write_resume_eip(void);

#endif /* AMD_BLOCK_CPU_H */
