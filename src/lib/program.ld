/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>

/* This file is included inside a SECTIONS block */

/* First we place the code and read only data (typically const declared).
 * This could theoretically be placed in rom.
 * The '.' in '.text . : {' is actually significant to prevent missing some
 * SoC's entry points due to artificial alignment restrictions, see
 * https://sourceware.org/binutils/docs/ld/Output-Section-Address.html
 */

/* Starting with version 18 LLVM the combination -ffunction-section -mcmodel=large
 * puts code and data in '.ltext, '.lrodata', '.ldata' and '.lbss'
 */

.text . : {
	_program = .;
	_text = .;
#if !(ENV_X86 && ENV_BOOTBLOCK)
	*(.init._start);
	*(.init);
	*(.init.*);
#endif
	*(.text._start);
	*(.text.stage_entry);
	KEEP(*(.metadata_hash_anchor));
	*(.text);
	*(.text.*);
	*(.ltext);
	*(.ltext.*);

#if ENV_HAS_CBMEM
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_cbmem_init_hooks = .;
	KEEP(*(.rodata.cbmem_init_hooks_early));
	KEEP(*(.rodata.cbmem_init_hooks));
	_ecbmem_init_hooks = .;
	RECORD_SIZE(cbmem_init_hooks)
#endif

	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_rsbe_init_begin = .;
	KEEP(*(.rsbe_init));
	_ersbe_init_begin = .;
	RECORD_SIZE(rsbe_init_begin)

#if ENV_RAMSTAGE
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_pci_drivers = .;
	KEEP(*(.rodata.pci_driver));
	_epci_drivers = .;
	RECORD_SIZE(pci_drivers)
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_cpu_drivers = .;
	KEEP(*(.rodata.cpu_driver));
	_ecpu_drivers = .;
	RECORD_SIZE(cpu_drivers)
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_cfr_forms = .;
	KEEP(*(.rodata.cfr_forms));
	_ecfr_forms = .;
	RECORD_SIZE(cfr_forms)
#endif

	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	*(.rodata);
	*(.rodata.*);
	*(.lrodata);
	*(.lrodata.*);
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_etext = .;
	RECORD_SIZE(text)
} : to_load

#if ENV_RAMSTAGE && (CONFIG(COVERAGE) || CONFIG(ASAN_IN_RAMSTAGE))
.ctors . : {
	. = ALIGN(0x100);
	__CTOR_LIST__ = .;
	KEEP(*(.ctors));
	LONG(0);
	LONG(0);
	__CTOR_END__ = .;
} : to_load
#endif

/* Include data, bss, and heap in that order. Not defined for all stages. */
#if !ENV_SEPARATE_DATA_AND_BSS
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
	RECORD_SIZE(rmodule_params)
#endif

	*(.data);
	*(.data.*);
	*(.ldata);
	*(.ldata.*);
	*(.sdata);
	*(.sdata.*);

#if ENV_ROMSTAGE_OR_BEFORE
	PROVIDE(_preram_cbmem_console = .);
	PROVIDE(_epreram_cbmem_console = _preram_cbmem_console);
	PROVIDE(_preram_cbmem_console_size = ABSOLUTE(0));
#elif ENV_RAMSTAGE
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_bs_init_begin = .;
	KEEP(*(.bs_init));
	LONG(0);
	LONG(0);
	_ebs_init_begin = .;
	RECORD_SIZE(bs_init_begin)
#endif

	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_edata = .;
	RECORD_SIZE(data)
} : to_load
#endif

#if !ENV_SEPARATE_DATA_AND_BSS
.bss . (NOLOAD) : {
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_bss = .;
	*(.bss)
	*(.bss.*)
	*(.lbss)
	*(.lbss.*)
	*(.sbss)
	*(.sbss.*)
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_ebss = .;
	RECORD_SIZE(bss)
} : to_load
#endif

#if ENV_HAS_HEAP_SECTION
.heap . (NOLOAD) : {
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_heap = .;
	. += CONFIG_HEAP_SIZE;
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_eheap = .;
	RECORD_SIZE(heap)
} : to_load
#endif

#if ENV_RAMSTAGE && CONFIG(ASAN_IN_RAMSTAGE)
	_shadow_size = (_eheap - _data) >> 3;
	REGION(asan_shadow, ., _shadow_size, ARCH_POINTER_ALIGN_SIZE)
#endif

_eprogram = .;
RECORD_SIZE(program)

/* The stage cache drops CONFIG_HEAP_SIZE bytes from the end of the in-memory
   image of the ramstage, so ensure that when moving that many bytes backwards
   from the program end, we're in the heap (or later), in some region that
   doesn't contain initialized code or data. */
#if ENV_RAMSTAGE
_bogus = ASSERT(_eprogram - CONFIG_HEAP_SIZE >= _heap,
	"HEAP_SIZE and heap misaligned");
#endif

/* Discard the sections we don't need/want */

zeroptr = 0;

/DISCARD/ : {
	*(.comment)
	*(.comment.*)
	*(.note)
	*(.note.*)
	*(.eh_frame);
}
