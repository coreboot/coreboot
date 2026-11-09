/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_AMD_MICROCODE_H
#define CPU_AMD_MICROCODE_H

#include <types.h>

void amd_update_microcode_from_cbfs(void);
void amd_load_microcode_from_cbfs(void);
void amd_free_microcode(void);
void amd_apply_microcode_patch(void);
void *amd_microcode_find(void);

/**
 * Check whether the given microcode patch is newer than the currently loaded patch.
 *
 * @param ucode_patch Pointer to the CBFS microcode patch block.
 * @return true if ucode_patch is non-NULL and has a newer patch_id than MSR 0x8b.
 */
bool amd_microcode_update_needed(const void *ucode_patch);

#endif /* CPU_AMD_MICROCODE_H */
