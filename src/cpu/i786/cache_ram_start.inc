#include <arch/cache_ram.h>

  
	/* copy data segment from FLASH ROM to CACHE */
	movl	$_clrodata, %esi
	movl	$_rodata, %edi
	movl	$_celdata, %ecx
	subl	%esi, %ecx
	jz	1f			/* should not happen */
	rep
	movsb
1:

	/** clear bss */
	movl	$EXT(_bss), %edi
	movl	$EXT(_ebss), %ecx
	subl	%edi, %ecx
	jz	1f
	xorl	%eax, %eax
	rep
	stosb
1:

	/* set new stack */
	movl	$(_stack + STACK_SIZE), %esp

	/* The next bit is tricky.  
	 * - I change code segments to handle the cache ram case.
	 * - I force the rom_code code segment again
	 *   when I call cache_ram_start to avoid strange processor
	 *   behavoir.  A simple call instruction does not work at
	 *   this point.  This is due either to virutally mapped
	 *   instruction caches, or address wrap around.
	 * - I change teh code segments back to my normal segment with
	 *   a 4GB limit and a base address of 0.
	 */
	ljmp	$0x20, $___cache_ram_code_start
.globl __cache_ram_code_start
__cache_ram_code_start:
	lcall	$0x20, $cache_ram_start
	ljmp	$0x10, $__cache_ram_code_done
.globl __cache_ram_code_done
__cache_ram_code_done:

