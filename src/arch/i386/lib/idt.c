#include <string.h>

// we had hoped to avoid this. 
// this is a stub IDT only. It's main purpose is to ignore calls 
// to the BIOS. 
struct realidt {
	unsigned short offset, cs;
}; 


void handler(void) {
 __asm__ __volatile__ ( 
	".code16\n"
	"idthandle:\n"
	"	movb $0x55, %al\n"
	"	outb %al, $0x80\n"
	"	iret\n"
	"end_idthandle:\n"
	".code32\n"
	);
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
