#include <string.h>
#include <printk.h>

// we had hoped to avoid this. 
// this is a stub IDT only. It's main purpose is to ignore calls 
// to the BIOS. 
// no longer. Dammit. We have to respond to these.
struct realidt {
	unsigned short offset, cs;
}; 

// from a handy writeup that andrey found.

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
	"	pushal\n"
	"	movb $0, %al\n"
	"	ljmp $0, $callbiosint16\n"
	"end_idthandle:\n"
	".code32\n"
	);

}

// Calling conventions. The first C function is called with this stuff
// on the stack. They look like value parameters, but note that if you
// modify them they will go back to the INTx function modified. 
// the C function will call the biosint function with these as
// REFERENCE parameters. In this way, we can easily get 
// returns back to the INTx caller (i.e. vgabios)
void callbiosint(void) {
__asm__ __volatile__ (
	".code16\n"
	"callbiosint16:\n"
	// clean up the int #. To save space we put it in the lower
	// byte. But the top 24 bits are junk. 
	"andl $0xffffff00, %eax\n"
	// this push does two things:
	// - put the INT # on the stack as a parameter
	// - provides us with a temp for the %cr0 mods.
	"pushl	%eax\n"
 	"movl    %cr0, %eax\n"
       //"andl    $0x7FFAFFD1, %eax\n" /* PG,AM,WP,NE,TS,EM,MP = 0 */
        //"orl    $0x60000001, %eax\n" /* CD, NW, PE = 1 */
        "orl    $0x00000001, %eax\n" /* PE = 1 */
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
	// pop the INT # that you pushed earlier
	"   popl	%eax\n"
	" 	popal\n"
	"	iret\n"
	".code32\n"
	);
}


enum {
	PCIBIOS = 0x1a
	};
#ifdef CONFIG_PCIBIOS
int
pcibios(
        unsigned long *pedi,
        unsigned long *pesi,
        unsigned long *pebp,
        unsigned long *pesp,
        unsigned long *pebx,
        unsigned long *pedx,
        unsigned long *pecx,
        unsigned long *peax,
        unsigned long *pflags
        );
#endif
int
biosint(
	unsigned long intnumber,
	unsigned long edi, 
	unsigned long esi,
	unsigned long ebp, 
	unsigned long esp, 
	unsigned long ebx, 
	unsigned long edx, 
	unsigned long ecx, 
	unsigned long eax, 
	// these came in from 16-bit land
	// but gcc does something weird we have to undo.
	unsigned short stackip,
	unsigned short stackflags,
	unsigned short stackcs
	) {
	unsigned long ip; 
	unsigned long cs; 
	unsigned long flags;
	int ret = -1;

	ip = stackip;
	cs = stackcs;
	flags = stackflags;

	printk_debug("biosint: # 0x%lx, eax 0x%lx ebx 0x%lx ecx 0x%lx edx 0x%lx\n", 
		intnumber, eax, ebx, ecx, edx);
	printk_debug("biosint: ebp 0x%lx esp 0x%lx edi 0x%lx esi 0x%lx\n", ebp, esp, edi, esi);
	printk_debug("biosint: ip 0x%x cs 0x%x flags 0x%x\n", ip, cs, flags);
#ifdef CONFIG_PCIBIOS
	if (intnumber == PCIBIOS)
		ret = pcibios( &edi, &esi, &ebp, &esp, &ebx, &edx, &ecx, &eax, &flags);
	if (ret)
		flags |= 1; // carry flags
	else
		flags &= ~1;
#endif
	stackflags = flags;
	return ret;
} 


void
setup_realmode_idt(void) {
	extern unsigned char idthandle, end_idthandle;
	int i;
	struct realidt *idts = (struct realidt *) 0;
	int codesize = &end_idthandle - &idthandle;
	unsigned char *intbyte, *codeptr;

	// for each int, we create a customized little handler
	// that just pushes %ax, puts the int # in %al, 
	// then calls the common interrupt handler. 
	// this necessitated because intel didn't know much about 
	// architecture when they did the 8086 (it shows)
	// (hmm do they know anymore even now :-)
	// obviously you can see I don't really care about memory 
	// efficiency. If I did I would probe back through the stack
	// and get it that way. But that's really disgusting.
	for (i = 0; i < 256; i++) {
		idts[i].cs = 0;
		codeptr = 1024 + i * codesize;
		idts[i].offset = codeptr;
		memcpy((void *) codeptr, &idthandle, codesize);
		intbyte = codeptr + 3;
		*intbyte = i;
	}

}
