/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This linker script is used to link rmodules (relocatable modules).  It
 * links at zero so that relocation fixups are easy when placing the binaries
 * anywhere in the address space.
 *
 * NOTE:  The program's loadable sections (text, module_params, and data) are
 * packed into the flat blob. The rmodule loader assumes the entire program
 * resides in one contiguous address space. Therefore, alignment for a given
 * section (if required) needs to be done at the end of the preceding section.
 * e.g. if the data section should be aligned to an 8 byte address the text
 * section should have ALIGN(8) at the end of its section.  Otherwise there
 * won't be a consistent mapping between the flat blob and the loaded program.
 */

#include <memlayout.h>
#include <arch/header.ld>

SECTIONS
{
	SET_COUNTER(rmodule, 0x00000000)

	/* program.ld is directly included because there's no one particular
	 * class that rmodule is used on. */
	#include <lib/program.ld>
}
