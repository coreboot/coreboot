#ifndef FALLBACK_H
#define FALLBACK_H

#if !defined(__ASSEMBLER__) && !defined(__PRE_RAM__)

void boot_successful(void);
void set_boot_successful(void);

#if !CONFIG_PC80_SYSTEM
void set_boot_successful(void) { }
#endif

#endif /* __ASSEMBLER__ */
#endif /* FALLBACK_H */
