/* FIXME(dhendrix): This is split out from asm/system.h. */
#ifndef SYSTEM_H_
#define SYSTEM_H_

/*
 * This is used to ensure the compiler did actually allocate the register we
 * asked it for some inline assembly sequences.  Apparently we can't trust
 * the compiler from one version to another so a bit of paranoia won't hurt.
 * This string is meant to be concatenated with the inline asm string and
 * will cause compilation to stop on mismatch.
 * (for details, see gcc PR 15089)
 */
#define __asmeq(x, y)  ".ifnc " x "," y " ; .err ; .endif\n\t"

#define nop() __asm__ __volatile__("mov\tr0,r0\t@ nop\n\t");

#define arch_align_stack(x) (x)

#ifndef __ASSEMBLER__
 /*
  * FIXME: sdelay originally came from arch/arm/cpu/armv7/exynos5/setup.h in
  * u-boot but does not seem specific to exynos5...
  */
void sdelay(unsigned long loops);
#endif // __ASSEMBLY__
#endif	/* SYSTEM_H_ */
