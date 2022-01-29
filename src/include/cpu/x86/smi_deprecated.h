/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __X86_SMI_DEPRECATED_H__
#define __X86_SMI_DEPRECATED_H__

void smm_init(void);
void smm_init_completion(void);

/* Entry from smmhandler.S. */
void smi_handler(void);

#endif
