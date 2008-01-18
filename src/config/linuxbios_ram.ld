/*
 *	Memory map:
 *
 *	_RAMBASE		
 *				: data segment
 *				: bss segment
 *				: heap
 *				: stack
 */
/*
 * Bootstrap code for the STPC Consumer
 * Copyright (c) 1999 by Net Insight AB. All Rights Reserved.
 */

/*
 *	Written by Johan Rydberg, based on work by Daniel Kahlin.
 *      Rewritten by Eric Biederman
 *  2005.12 yhlu add coreboot_ram cross the vga font buffer handling
 */
/*
 *	We use ELF as output format. So that we can
 *	debug the code in some form. 
 */
INCLUDE ldoptions

ENTRY(_start)

SECTIONS
{
	. = _RAMBASE;
	/*
	 * First we place the code and read only data (typically const declared).
	 * This get placed in rom.
	 */
	.text : {
		_text = .;
		*(.text);
		*(.text.*);
		. = ALIGN(16);
		_etext = .;
	}
	.rodata : {
		_rodata = .;
		. = ALIGN(4);
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
		*(.rodata)
		*(.rodata.*)
		/*
		 * kevinh/Ispiri - Added an align, because the objcopy tool
		 * incorrectly converts sections that are not long word aligned.
		 * This breaks the coreboot.strip target.
		 */
		 . = ALIGN(4);

		_erodata = .;
	}	
	/*
	 * After the code we place initialized data (typically initialized
	 * global variables). This gets copied into ram by startup code.
	 * __data_start and __data_end shows where in ram this should be placed,
	 * whereas __data_loadstart and __data_loadend shows where in rom to
	 * copy from.
	 */
	.data : {
		_data = .;
		*(.data)
		_edata = .;
	}
	/*
	 * bss does not contain data, it is just a space that should be zero
	 * initialized on startup. (typically uninitialized global variables)
	 * crt0.S fills between _bss and _ebss with zeroes.
	 */
	_bss = .;
	.bss . : {
		*(.bss)
		*(.sbss)
		*(COMMON)
	}
	_ebss = .;
	_end = .;
	. = ALIGN(STACK_SIZE);
	_stack = .;
	.stack . : {
		/* Reserve a stack for each possible cpu */
		/* the stack for ap will be put after pgtbl in 1M to CONFIG_LB_MEM_TOPK range when VGA and ROM_RUN and CONFIG_LB_MEM_TOPK>1024*/
		. = ((CONFIG_CONSOLE_VGA || CONFIG_PCI_ROM_RUN)&&(_RAMBASE<0x100000)&&(CONFIG_LB_MEM_TOPK>(0x100000>>10)) ) ? STACK_SIZE : (CONFIG_MAX_CPUS*STACK_SIZE);
	}
	_estack = .;
        _heap = .;
        .heap . : {
                /* Reserve 256K for the heap */
                . = HEAP_SIZE ;
                . = ALIGN(4);
        }
        _eheap = .;
	/* The ram segment
 	 * This is all address of the memory resident copy of coreboot.
	 */
	_ram_seg = _text; 
	_eram_seg = _eheap;

	_bogus = ASSERT( ( (_eram_seg>>10) < (CONFIG_LB_MEM_TOPK)) , "please increase CONFIG_LB_MEM_TOPK");

        _bogus = ASSERT( !((CONFIG_CONSOLE_VGA || CONFIG_PCI_ROM_RUN) && ((_ram_seg<0xa0000) && (_eram_seg>0xa0000))) , "please increase CONFIG_LB_MEM_TOPK and if still fail, try to set _RAMBASE more than 1M");

	/DISCARD/ : {
		*(.comment)
		*(.note)
		*(.note.*)
	}
}
