/*
 * This linker script is used to link rmodules (relocatable modules).  It
 * links at zero so that relocation fixups are easy when placing the binaries
 * anywhere in the address space.
 *
 * NOTE:  The program's loadable sections (text, module_params, and data) are
 * packed into the flat blob. The rmodule loader assumes the entire program
 * resides in one contiguous address space. Therefore, alignment for a given
 * section (if required) needs to be done at the end of the preceeding section.
 * e.g. if the data section should be aligned to an 8 byte address the text
 * section should have ALIGN(8) at the end of its section.  Otherwise there
 * won't be a consistent mapping between the flat blob and the loaded program.
 */

BASE_ADDRESS = 0x00000;

ENTRY(__rmodule_entry);

SECTIONS
{
	. = BASE_ADDRESS;

	.payload :  {
		/* C code of the module. */
		_program = .;
		*(.text._start);
		*(.text.stage_entry);
		*(.text);
		*(.text.*);
		/* C read-only data. */
		. = ALIGN(16);

#if IS_ENABLED(CONFIG_COVERAGE)
		__CTOR_LIST__ = .;
		*(.ctors);
		LONG(0);
		LONG(0);
		__CTOR_END__ = .;
#endif

		/* The driver sections are to allow linking coreboot's
		 * ramstage with the rmodule linker. Any changes made in
		 * ramstage.ld should be made here as well. */
		. = ALIGN(8);
		_pci_drivers = . ;
		KEEP(*(.rodata.pci_driver));
		_epci_drivers = . ;
		. = ALIGN(8);
		_cpu_drivers = . ;
		KEEP(*(.rodata.cpu_driver));
		_ecpu_drivers = . ;
		. = ALIGN(8);
		_bs_init_begin = .;
		KEEP(*(.bs_init));
		LONG(0);
		LONG(0);
		_bs_init_end = .;
		_cbmem_init_hooks = .;
		KEEP(*(.rodata.cbmem_init_hooks));
		_ecbmem_init_hooks = .;

		. = ALIGN(8);

		*(.rodata);
		*(.rodata.*);
		. = ALIGN(8);

		/* The parameters section can be used to pass parameters
		 * to a module, however there has to be an prior agreement
		 * on how to interpret the parameters. */
		_module_params_begin = .;
		KEEP(*(.module_parameters));
		_module_params_end = .;
		. = ALIGN(8);

		/* Data section. */
		. = ALIGN(64);	/* Mirror cache line alignment from ramstage. */
		_sdata = .;
		*(.data);
		*(.data.*);
		. = ALIGN(8);
		_edata = .;

		. = ALIGN(8);
	}

	.bss (NOLOAD) : {
		/* C uninitialized data of the module. */
		_bss = .;
		*(.bss);
		*(.bss.*)
		*(.sbss)
		*(.sbss.*)
		*(COMMON);
		. = ALIGN(8);
		_ebss = .;

		/*
		 * Place the heap after BSS. The heap size is passed in by
		 * by way of ld --defsym=__heap_size=<>
		 */
		_heap = .;
		. = . + __heap_size;
		_eheap = .;
		_eprogram = .;
	}

	/DISCARD/ : {
		/* Drop unnecessary sections. */
		*(.eh_frame);
		*(.note);
		*(.note.*);
	}
}
