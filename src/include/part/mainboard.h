#ifndef PART_MAINBOARD_H
#define PART_MAINBOARD_H

void mainboard_fixup(void);

#ifdef FINAL_MAINBOARD_FIXUP
void final_mainboard_fixup(void);
#else
#  define final_mainboard_fixup() do {} while(0)
#endif /* FINAL_MAINBOARD_FIXUP */

#endif /* PART_MAINBOARD_H */
