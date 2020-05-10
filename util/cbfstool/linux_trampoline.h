/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file contains #define constants used by both the Linux
 * trampoline C-code and assembly language code.  As such it can only
 * contain preprocessor macros.  Do not inlucde C language
 * declarations in this file.
 */

#ifndef LINUX_TRAMPOLINE_H__
#define LINUX_TRAMPOLINE_H__

/*
 * Trampoline entry point
 * TODO: any better place?
 */
#define TRAMPOLINE_ENTRY_LOC 0x40000

#define LINUX_PARAM_LOC 0x90000
#define COMMAND_LINE_LOC 0x91000

#endif /* LINUX_TRAMPOLINE_H__ */
