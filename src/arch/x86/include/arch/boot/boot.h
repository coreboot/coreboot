/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef X86_BOOT_H
#define X86_BOOT_H

#include <types.h>
/*
 * Jump to function in protected mode.
 * @arg func_ptr           Function to jump to in protected mode
 * @arg                    Argument to pass to called function
 *
 * @noreturn
 */
void protected_mode_jump(uint32_t func_ptr, uint32_t argument);

#endif /* X86_BOOT_H */
