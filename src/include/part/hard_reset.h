#ifndef PART_HARD_RESET_H
#define PART_HARD_RESET_H

#if CONFIG_HAVE_HARD_RESET == 1
void hard_reset(void);
#else
#define hard_reset() do {} while(0)
#endif


#endif
