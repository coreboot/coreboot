// This is very early MTRR code so that DRAM runs at reasonable speeds. 
// we do a better job of exact MTRR setup later
#if 1
	/* enable caching for all using variable mtrr */

	mov $0x200, %ecx
	rdmsr
	and $0xfffffff0, %edx
	or  $0x00000000, %edx
	and $0x00000f00, %eax
	or  $0x00000006, %eax
	wrmsr
	
	mov $0x201, %ecx
	rdmsr
	and $0xfffffff0, %edx
	or  $0x0000000f, %edx
	and $0x000007ff, %eax
	or  $0xf0000800, %eax
	wrmsr
#endif
#if 0
	/* enable read only caching for top 64K of flash using variable mtrr */

	mov $0x200, %ecx
	rdmsr
	and $0xfffffff0, %edx
	or  $0x00000000, %edx
	and $0x00000f00, %eax
	or  $0x000f0006, %eax
	wrmsr
	
	mov $0x201, %ecx
	rdmsr
	and $0xfffffff0, %edx
	or  $0x0000000f, %edx
	and $0x000007ff, %eax
	or  $0xffff0800, %eax
	wrmsr
#endif
#if 0
	/* enable read only caching for top 64K of flash using fixed mtrr */
	/*
	 * This seems to enable caching for entire 1st meg.  Maybe
	 * all the other fixed ones don't have the default I was
	 * assuming.
	 */
	mov $0x26e, %ecx
	rdmsr
	mov $0x05050505, %edx
	mov $0x05050505, %eax
	wrmsr
	
	mov $0x26f, %ecx
	rdmsr
	mov $0x05050505, %edx
	mov $0x05050505, %eax
	wrmsr
#endif
	/*
	 * Set the default memory type and enable
	 * fixed and variable MTRRs.
	 */

	mov $0x2ff, %ecx
	rdmsr
	and $0xfffff300, %eax
#if 1	/* Enable/Disable MTRRs */
	or  $0x00000800, %eax
#endif
#if 0	/* Enable fixed MTRRs if MTRRs are enabled */
	or  $0x00000400, %eax
#endif
	wrmsr

	/* enable cache */

	mov %cr0, %eax
	and $0x9fffffff,%eax
	mov %eax, %cr0
