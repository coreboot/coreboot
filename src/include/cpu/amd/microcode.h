/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_AMD_MICROCODE_H
#define CPU_AMD_MICROCODE_H

void amd_update_microcode_from_cbfs(void);
void amd_load_microcode_from_cbfs(void);
void amd_free_microcode(void);
void amd_apply_microcode_patch(void);

#endif /* CPU_AMD_MICROCODE_H */
