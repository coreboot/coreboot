#include <string.h>
#include <printk.h>

// we had hoped to avoid this. 
// this is a stub IDT only. It's main purpose is to ignore calls 
// to the BIOS. 
// no longer. Dammit. We have to respond to these.
struct realidt {
	unsigned short offset, cs;
}; 

// handler. 
// There are some assumptions we can make here. 
// First, the Top Of Stack (TOS) is located on the top of page zero. 
// we can share this stack between real and protected mode. 
// that simplifies a lot of things ...
// we'll just push all the registers on the stack as longwords, 
// and pop to protected mode. 
// second, since this only ever runs as part of linuxbios, 
// we know all the segment register values -- so we don't save any.
// keep the handler that calls things small. It can do a call to 
// more complex code in linuxbios itself. This helps a lot as we don't
// have to do address fixup in this little stub, and calls are absolute
// so the handler is relocatable.
void handler(void) {
 __asm__ __volatile__ ( 
	".code16\n"
	"idthandle:\n"
	"	movb $0x55, %al\n"
	"	outb %al, $0x80\n"
	"	ljmp $0, $callbiosint16\n"
	"end_idthandle:\n"
	".code32\n"
	);
}

void callbiosint(void) {
__asm__ __volatile__ (
	".code16\n"
	"callbiosint16:\n"
	"	pushl	%eax\n"
	"	pushl	%ebx\n"
	"	pushl	%ecx\n"
	"	pushl	%edx\n"
	"	pushl	%edi\n"
	"	pushl	%esi\n"
 	"movl    %cr0, %eax\n"
       "andl    $0x7FFAFFD1, %eax\n" /* PG,AM,WP,NE,TS,EM,MP = 0 */
        "orl    $0x60000001, %eax\n" /* CD, NW, PE = 1 */
       "movl    %eax, %cr0\n"
        /* Now that we are in protected mode jump to a 32 bit code segment. */
       "data32  ljmp    $0x10, $biosprotect\n"
       "biosprotect:\n"
       ".code32\n"
       "    movw $0x18, %ax          \n"
       "    mov  %ax, %ds          \n"
       "    mov  %ax, %es          \n"
       "    mov  %ax, %fs          \n"
       "    mov  %ax, %gs          \n"
       "    mov  %ax, %ss          \n"
	"	call	biosint	\n"
	// back to real mode ...
       "    ljmp $0x28, $__rms_16bit\n"
       "__rms_16bit:                 \n"
       ".code16                      \n" /* 16 bit code from here on... */

       /* Load the segment registers w/ properly configured segment
        * descriptors.  They will retain these configurations (limits,
        * writability, etc.) once protected mode is turned off. */
       "    mov  $0x30, %ax         \n"
       "    mov  %ax, %ds          \n"
       "    mov  %ax, %es          \n"
       "    mov  %ax, %fs          \n"
       "    mov  %ax, %gs          \n"
       "    mov  %ax, %ss          \n"

       /* Turn off protection (bit 0 in CR0) */
       "    movl %cr0, %eax        \n"
       "    andl $0xFFFFFFFE, %eax  \n"
       "    movl %eax, %cr0        \n"

       /* Now really going into real mode */
       "    ljmp $0,  $__rms_real \n"
       "__rms_real:                  \n"

       /* Setup a stack */
       "    mov  $0x0, %ax       \n"
       "    mov  %ax, %ss          \n"
        /* ebugging for RGM */
       "    mov $0x11, %al      \n"
        " outb  %al, $0x80\n"
       "    xor  %ax, %ax          \n"
       "    mov  %ax, %ds          \n"
       "    mov  %ax, %es          \n"
       "    mov  %ax, %fs          \n"
       "    mov  %ax, %gs          \n"
	"	popl	%eax\n"
	"	popl	%ebx\n"
	"	popl	%ecx\n"
	"	popl	%edx\n"
	"	popl	%edi\n"
	"	popl	%esi\n"
	"	iret\n"
	".code32\n"
	);
}

int
biosint(unsigned long eax, 
	unsigned long ebx, 
	unsigned long ecx, 
	unsigned long edx, 
	unsigned long edi, 
	unsigned long esi) {
	printk_debug("biosint: eax 0x%lx ebx 0x%lx ecx 0x%lx edx 0x%lx\n", 
		eax, ebx, ecx, edx);
	printk_debug("biosint: edi 0x%lx esi 0x%lx\n", edi, esi);
	return 0;
} 
void
setup_realmode_idt(void) {
	extern unsigned char idthandle, end_idthandle;
	int i;
	struct realidt *idts = (struct realidt *) 0;

	for (i = 0; i < 256; i++) {
		idts[i].cs = 0;
		idts[i].offset = 1024;
	}

	memcpy((void *) 1024, &idthandle, &end_idthandle - &idthandle);

}
