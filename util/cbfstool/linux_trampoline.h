/*
 * This file is part of coreboot..
 *
 * Based on work by Patrick Georgi <patrick@georgi-clan.de>
 * Copyright 2014 Curt Brune <curt@cumulusnetworks.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
