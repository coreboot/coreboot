#include <x86emu.h>
#include "test.h"

_ptr p;
ptr current = 0;
extern int teststart, testend;
void
test()
{
  __asm__ __volatile__(".code16\nteststart:movb $4, %al\n outb %al, $0x80\n.code32\nhlt\ntestend:");
}

static void
do_int(int num) 
{
	setup_int(current);
	//	if(int_handler(current))
	  ; //X86EMU_Exec();
	finish_int(current);
}

unsigned char biosmem[65536];


int
main(int argc, char **argv)
{
	int i;
	unsigned char *cp;
	X86EMU_intrFuncs intFuncs[256];
	void X86EMU_setMemBase(void *base, size_t size);
	void x86emu_dump_xregs (void);

	current = &p;
	X86EMU_setMemBase(biosmem, sizeof(biosmem));

	//	for (i=0;i<256;i++)
	//	intFuncs[i] = do_int;
	//	X86EMU_setupIntrFuncs(intFuncs);

	current->ax = 0xff;
	current->dx = 0x80;
	//	current->ip = 0;
	for(i = 0, cp = (unsigned char *) &teststart; 
	    cp < (unsigned char *) &testend; cp++, i++)
	  wrb(i, *cp);
	//	M.x86.saved_cs = 0;
	//	M.x86.saved_ip = 0;
#if 0
	X86EMU_set_debug(-1);

	x86emu_dump_xregs ();
	x86emu_just_disassemble();
	X86EMU_trace_on();
	x86emu_single_step ();
	x86emu_dump_xregs ();
	x86emu_just_disassemble();
	x86emu_single_step ();
	x86emu_dump_xregs ();
	x86emu_just_disassemble();
	x86emu_single_step ();
	x86emu_dump_xregs ();
	x86emu_just_disassemble();

	//	do_int(0x10);
#endif
	X86EMU_trace_on();
	x86emu_single_step ();
	x86emu_single_step ();
	x86emu_single_step ();
	x86emu_single_step ();
	x86emu_single_step ();
	


	return 0;
}


