/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_MCA_H
#define AMD_BLOCK_MCA_H

#include <cpu/x86/msr.h>

struct mca_bank_status {
	unsigned int bank;
	msr_t sts;
};

void check_mca(void);
void mca_check_all_banks(void);
void build_bert_mca_error(struct mca_bank_status *mci);

#endif /* AMD_BLOCK_MCA_H */
