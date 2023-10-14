/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _AMDBLOCKS_SMM_H_
#define _AMDBLOCKS_SMM_H_

#include <cpu/x86/msr.h>
#include <types.h>

void *get_smi_source_handler(int source);
void handle_smi_gsmi(void);
void handle_smi_store(void);
void fch_apmc_smi_handler(void);
void clear_tvalid(void);
void tseg_valid(void);
bool is_smm_locked(void);
void lock_smm(void);
/* See SMITYPE_* for list possible of events. GEVENTS are handled with mainboard_smi_gpi. */
void mainboard_handle_smi(int event);

#if CONFIG_SMM_TSEG_SIZE != 0
#if (CONFIG_SMM_TSEG_SIZE <= CONFIG_SMM_RESERVED_SIZE)
# error "CONFIG_SMM_TSEG_SIZE <= CONFIG_SMM_RESERVED_SIZE"
#endif
#if (CONFIG_SMM_TSEG_SIZE < 0x20000)
# error "CONFIG_SMM_TSEG_SIZE must at least be 128KiB"
#endif
#if ((CONFIG_SMM_TSEG_SIZE & (CONFIG_SMM_TSEG_SIZE - 1)) != 0)
# error "CONFIG_SMM_TSEG_SIZE is not a power of 2"
#endif
#endif

#endif
