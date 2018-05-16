/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#include <memlayout.h>

/* This file is included inside a SECTIONS block */

/* First we place the code and read only data (typically const declared).
 * This could theoretically be placed in rom.
 * The '.' in '.text . : {' is actually significant to prevent missing some
 * SoC's entry points due to artificial alignment restrictions, see
 * https://sourceware.org/binutils/docs/ld/Output-Section-Address.html
 */
.text . : {
	_program = .;
	_text = .;
	/*
	 * The .rom.* sections are to acommodate x86 romstage. romcc as well
	 * as the assembly files put their text and data in these sections.
	 */
	*(.rom.text);
	*(.rom.data);
	*(.text._start);
	*(.text.stage_entry);
#if (ENV_DECOMPRESSOR || ENV_BOOTBLOCK && \
		!IS_ENABLED(CONFIG_COMPRESS_BOOTBLOCK)) && \
    !(IS_ENABLED(CONFIG_ARCH_BOOTBLOCK_X86_32) || \
      IS_ENABLED(CONFIG_ARCH_BOOTBLOCK_X86_64))
	KEEP(*(.id));
#endif
	*(.text);
	*(.text.*);

#if ENV_RAMSTAGE || ENV_ROMSTAGE || ENV_POSTCAR
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_cbmem_init_hooks = .;
	KEEP(*(.rodata.cbmem_init_hooks));
	_ecbmem_init_hooks = .;
#endif

	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_rsbe_init_begin = .;
	KEEP(*(.rsbe_init));
	_ersbe_init_begin = .;

#if ENV_RAMSTAGE
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_pci_drivers = .;
	KEEP(*(.rodata.pci_driver));
	_epci_drivers = .;
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_cpu_drivers = .;
	KEEP(*(.rodata.cpu_driver));
	_ecpu_drivers = .;
#endif

	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	*(.rodata);
	*(.rodata.*);
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_etext = .;
} : to_load

#if ENV_RAMSTAGE && IS_ENABLED(CONFIG_COVERAGE)
.ctors . : {
	. = ALIGN(0x100);
	__CTOR_LIST__ = .;
	KEEP(*(.ctors));
	LONG(0);
	LONG(0);
	__CTOR_END__ = .;
}
#endif

/* Include data, bss, and heap in that order. Not defined for all stages. */
#if ARCH_STAGE_HAS_DATA_SECTION
.data . : {
	. = ALIGN(ARCH_CACHELINE_ALIGN_SIZE);
	_data = .;

/*
 * The postcar phase uses a stack value that is located in the relocatable
 * module section. While the postcar stage could be linked like smm and
 * other rmodules the postcar stage needs similar semantics of the more
 * traditional stages in the coreboot infrastructure. Therefore it's easier
 * to specialize this case.
 */
#if ENV_RMODULE || ENV_POSTCAR
	_rmodule_params = .;
	KEEP(*(.module_parameters));
	_ermodule_params = .;
#endif

	*(.data);
	*(.data.*);
	*(.sdata);
	*(.sdata.*);

#ifdef __PRE_RAM__
	PROVIDE(_preram_cbmem_console = .);
	PROVIDE(_epreram_cbmem_console = _preram_cbmem_console);
#elif ENV_RAMSTAGE
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_bs_init_begin = .;
	KEEP(*(.bs_init));
	LONG(0);
	LONG(0);
	_ebs_init_begin = .;
#endif

	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_edata = .;
}
#endif

#if ARCH_STAGE_HAS_BSS_SECTION
.bss . : {
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_bss = .;
	*(.bss)
	*(.bss.*)
	*(.sbss)
	*(.sbss.*)
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_ebss = .;
}
#endif

#if ARCH_STAGE_HAS_HEAP_SECTION
.heap . : {
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_heap = .;
	. += (ENV_RMODULE ? __heap_size : CONFIG_HEAP_SIZE);
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_eheap = .;
}
#endif

_eprogram = .;

/* Discard the sections we don't need/want */

zeroptr = 0;

/DISCARD/ : {
	*(.comment)
	*(.comment.*)
	*(.note)
	*(.note.*)
	*(.eh_frame);
}
