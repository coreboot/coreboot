#ifndef PART_MAINBOARD_H
#define PART_MAINBOARD_H

void mainboard_fixup(void);

#ifdef FINAL_MAINBOARD_FIXUP
void final_mainboard_fixup(void);
#else
#  define final_mainboard_fixup() do {} while(0)
#endif /* FINAL_MAINBOARD_FIXUP */

#ifdef HAVE_MAINBOARD_CPU_FIXUP
void mainboard_cpu_fixup(int);
#else
#  define mainboard_cpu_fixup(x) do {} while(0)
#endif /* HAVE_MAINBOARD_CPU_FIXUP */

#endif /* PART_MAINBOARD_H */
