#include <arch/cache_ram.h>
	
#define MINI_DATA_SIZE  0x00008000
#define MINI_STACK_SIZE 0x00000400

#define MINI_DATA_BASE  (_edata)
#define MINI_DATA_END   (MINI_DATA_BASE + MINI_DATA_SIZE)
#define MINI_STACK_BASE MINI_DATA_END
#define MINI_STACK_END  (MINI_STACK_BASE + MINI_STACK_SIZE)

debug_loop:
	CONSOLE_DEBUG_INLINE_TX_CHAR($'a')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\r')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\n')

        /* Load my normal data segments */
        movw    $0x18, %ax
        movw    %ax, %ds
        movw    %ax, %es
        movw    %ax, %ss

	CONSOLE_DEBUG_INLINE_TX_CHAR($'b')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\r')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\n')

	movl	$0x20, %ecx
	movl	$(CACHE_RAM_BASE), %eax
	movl	$(CACHE_RAM_BASE + CACHE_RAM_SIZE), %ebx
	CALLSP(ramtest)

	CONSOLE_DEBUG_INLINE_TX_CHAR($'c')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\r')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\n')

        /* Load a different set of data segments */
        movw    $0x08, %ax
        movw    %ax, %ds
        movw    %ax, %es
        movw    %ax, %ss

	movl	$(_stack + STACK_SIZE), %esp

	CONSOLE_DEBUG_INLINE_TX_CHAR($'d')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\r')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\n')

	call debug2

	CONSOLE_DEBUG_INLINE_TX_CHAR($'e')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\r')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\n')

	call debug3

	CONSOLE_DEBUG_INLINE_TX_CHAR($'f')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\r')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\n')
#if 0
	jmp debug_loop
#else
	jmp debug_loop_out
#endif

debug2:
	CONSOLE_DEBUG_INLINE_TX_CHAR($'g')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\r')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\n')

	ret


debug3:
	CONSOLE_DEBUG_INLINE_TX_CHAR($'h')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\r')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\n')
#if 1
	movl	$(MINI_DATA_BASE + (MINI_DATA_SIZE/2)), %esi
	movl	$MINI_DATA_BASE, %edi
	movl	$((MINI_DATA_SIZE)/8), %ecx
	rep	movsl
#endif
#if 1

push   %ebp
mov    %esp,%ebp
push   %esi
push   %ebx
mov    $0x3f8,%ecx
mov    $0x3f9,%ebx
xor    %eax,%eax
mov    %ebx,%edx
out    %al,(%dx)
mov    $0x1,%al
mov    $0x3fa,%edx
out    %al,(%dx)
mov    $0x3fb,%esi
mov    $0x83,%al
mov    %esi,%edx
out    %al,(%dx)
mov    $0xc,%al
mov    %ecx,%edx
out    %al,(%dx)
xor    %eax,%eax
mov    %ebx,%edx
out    %al,(%dx)
mov    $0x3,%al
mov    %esi,%edx
out    %al,(%dx)
lea    0xfffffff8(%ebp),%esp
pop    %ebx
pop    %esi
leave  
#endif

	ret


debug_loop_out:
	CONSOLE_DEBUG_INLINE_TX_CHAR($'i')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\r')
	CONSOLE_DEBUG_INLINE_TX_CHAR($'\n')


