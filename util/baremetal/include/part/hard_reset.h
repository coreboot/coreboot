#ifndef PART_HARD_RESET_H
#define PART_HARD_RESET_H

#ifdef HAVE_HARD_RESET
void hard_reset(void);
#else
#define hard_reset() do {} while(0)
#endif


#endif
