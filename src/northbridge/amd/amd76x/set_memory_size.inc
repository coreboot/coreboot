jmp set_memory_size_out

#include <cpu/k7/mtrr.h>
set_memory_size:
	/* Read the top of memory */
	movl	$0x9c, %eax
	PCI_READ_CONFIG_DWORD

#if USE_AMD_NDA_CODE
#endif /* USE_AMD_NDA_CODE */

set_memory_size_end:
	RETSP
set_memory_size_out:
