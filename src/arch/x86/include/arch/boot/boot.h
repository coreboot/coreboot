/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ASM_I386_BOOT_H
#define ASM_I386_BOOT_H

#define ELF_CLASS	ELFCLASS32
#define ELF_DATA	ELFDATA2LSB
#define ELF_ARCH	EM_386

#include <types.h>
/*
 * Jump to function in protected mode.
 * @arg func_ptr           Function to jump to in protected mode
 * @arg                    Argument to pass to called function
 *
 * @noreturn
 */
void protected_mode_jump(uint32_t func_ptr,
			 uint32_t argument);

#endif /* ASM_I386_BOOT_H */
