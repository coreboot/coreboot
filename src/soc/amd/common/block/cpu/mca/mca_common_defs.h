/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_MCA_COMMON_DEF_H
#define AMD_BLOCK_MCA_COMMON_DEF_H

#include <amdblocks/mca.h>
#include <cper.h>
#include <types.h>

bool mca_skip_check(void);
void mca_print_error(unsigned int bank);
void build_bert_mca_error(struct mca_bank_status *mci);
enum cper_x86_check_type error_to_chktype(struct mca_bank_status *mci);
void fill_generic_section(cper_proc_generic_error_section_t *sec, struct mca_bank_status *mci);

#endif /* AMD_BLOCK_MCA_COMMON_DEF_H */
