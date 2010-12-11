#ifndef __ARCH_LLSHELL__
#define __ARCH_LLSHELL__


#if CONFIG_LLSHELL
#define llshell() asm("jmp low_level_shell");
#else
#define llshell() print_debug("LLSHELL not active.\n");
#endif

#endif
