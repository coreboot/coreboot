OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)

/*
 * This linker script is used to link rmodules (relocatable modules).  It
 * links at zero so that relocation fixups are easy when placing the binaries
 * anywhere in the address space.
 *
 * NOTE:  The program's loadable sections (text, module_params, and data) are
 * packed into the flat blob using the AT directive. The rmodule loader assumes
 * the entire program resides in one contiguous address space. Therefore,
 * alignment for a given section (if required) needs to be done at the end of
 * the preceeding section. e.g. if the data section should be aligned to an 8
 * byte address the text section should have ALIGN(8) at the end of its section.
 * Otherwise there won't be a consistent mapping between the flat blob and the
 * loaded program.
 */

BASE_ADDRESS = 0x00000;

SECTIONS
{
	. = BASE_ADDRESS;

	.header : AT (0) {
		*(.module_header);
		. = ALIGN(8);
	}

	/* Align the start of the module program to a large enough alignment
	 * so that any data in the program with an alignement property is met.
	 * Essentially, this alignment is the maximum possible data alignment
	 * property a program can have. */
	. = ALIGN(4096);
	_module_link_start_addr = .;
	_payload_begin_offset  = LOADADDR(.header) + SIZEOF(.header);

	.payload : AT (_payload_begin_offset) {
		/* C code of the module. */
		*(.textfirst);
		*(.text);
		*(.text.*);
		/* C read-only data. */
		. = ALIGN(16);

		__CTOR_LIST__ = .;
		*(.ctors);
		LONG(0);
		__CTOR_END__ = .;

		/* The driver sections are to allow linking coreboot's
		 * ramstage with the rmodule linker. Any changes made in
		 * coreboot_ram.ld should be made here as well. */
		console_drivers = .;
		*(.rodata.console_drivers)
		econsole_drivers = . ;
		. = ALIGN(4);
		pci_drivers = . ;
		*(.rodata.pci_driver)
		epci_drivers = . ;
		cpu_drivers = . ;
		*(.rodata.cpu_driver)
		ecpu_drivers = . ;
		_bs_init_begin = .;
		*(.bs_init)
		_bs_init_end = .;

		. = ALIGN(4);

		*(.rodata);
		*(.rodata.*);
		. = ALIGN(4);

		/* The parameters section can be used to pass parameters
		 * to a module, however there has to be an prior agreement
		 * on how to interpret the parameters. */
		_module_params_begin = .;
		*(.module_parameters);
		_module_params_end = .;
		. = ALIGN(8);

		/* Data section. */
		_sdata = .;
		*(.data);
		. = ALIGN(4);
		_edata = .;

		. = ALIGN(8);
	}

	/* _payload_end marks the end of the module's code and data. */
	_payload_end_offset = LOADADDR(.payload) + SIZEOF(.payload);

	.bss (NOLOAD) : {
		/* C uninitialized data of the module. */
		_bss = .;
		*(.bss);
		*(.sbss);
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
	}

	/* _module_program_size is the total memory used by the program. */
	_module_program_size = _eheap - _module_link_start_addr;

	/* coreboot's ramstage uses the _ram_seg and _eram_seg symbols
	 * for determining its load location. Provide those to help it out.
	 * It's a nop for any non-ramstage rmodule. */
	_ram_seg = _module_link_start_addr;
	_eram_seg = _module_link_start_addr + _module_program_size;

	/* The relocation information is linked on top of the BSS section
	 * because the BSS section takes no space on disk. The relocation data
	 * resides directly after the data section in the flat binary. */
	.relocations ADDR(.bss) : AT (_payload_end_offset) {
		*(.rel.*);
	}
	_relocations_begin_offset = LOADADDR(.relocations);
	_relocations_end_offset  = _relocations_begin_offset +
	                           SIZEOF(.relocations);

	/DISCARD/ : {
		/* Drop unnecessary sections. Since these modules are linked
		 * as shared objects there are dynamic sections. These sections
		 * aren't needed so drop them. */
		*(.comment);
		*(.note);
		*(.note.*);
		*(.dynamic);
		*(.dynsym);
		*(.dynstr);
		*(.gnu.hash);
		*(.eh_frame);
	}
}
