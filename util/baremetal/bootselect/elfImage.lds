




OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)

HEAPSIZE = DEFINED(HEAPSIZE) ? HEAPSIZE : 0x8000;

ENTRY(main)
SECTIONS
{
/*  . = 0x10000 - (elf_note - elf_header); */
  . = 0x1000;
  _text = .;			/* Text and read-only data */
  .text . : {
	*(.text)
	*(.fixup)
	*(.gnu.warning)
	}  = 0x9090
  .rodata (.): { *(.rodata) }
  .kstrtab (.): { *(.kstrtab) }


  . = ALIGN(16);		/* Exception table */
  _etext = .;			/* End of text section */

  .data (.): {			/* Data */
	*(.data)
	CONSTRUCTORS
	}

  _edata = .;			/* End of data section */
  __bss_start = .;		/* BSS */
  .bss (.): {
	*(.bss)
	}
  _ebss = .;
  _heap = .;
  .heap (.): {
	. = HEAPSIZE;
  }
  _eheap = .;
  _end = . ;

  _ram_seg = _text;
  _eram_seg = _end;

  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
}
