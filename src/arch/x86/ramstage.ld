/*
 *	Memory map:
 *
 *	CONFIG_RAMBASE		: text segment
 *				: rodata segment
 *				: data segment
 *				: bss segment
 *				: stack
 *				: heap
 */
ENTRY(_start)

PHDRS
{
	to_load PT_LOAD;
}

SECTIONS
{
	. = CONFIG_RAMBASE;

	INCLUDE "lib/program.ramstage.ld"

	_ = ASSERT( ( _eprogram < (CONFIG_RAMTOP)) , "Please increase CONFIG_RAMTOP");
}
