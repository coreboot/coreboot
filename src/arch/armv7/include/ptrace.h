#ifndef __ASM_ARM_PTRACE_H
#define __ASM_ARM_PTRACE_H

#define PTRACE_GETREGS		12
#define PTRACE_SETREGS		13
#define PTRACE_GETFPREGS	14
#define PTRACE_SETFPREGS	15

#define PTRACE_SETOPTIONS	21

/* options set using PTRACE_SETOPTIONS */
#define PTRACE_O_TRACESYSGOOD	0x00000001

#include <asm/proc/ptrace.h>

#ifndef __ASSEMBLER__
#define pc_pointer(v) \
	((v) & ~PCMASK)

#define instruction_pointer(regs) \
	(pc_pointer((regs)->ARM_pc))

extern void show_regs(struct pt_regs *);

#define predicate(x)	(x & 0xf0000000)
#define PREDICATE_ALWAYS	0xe0000000

#endif /* __ASSEMBLER__ */

#endif
