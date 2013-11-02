#ifndef RESET_H
#define RESET_H

#if CONFIG_HAVE_HARD_RESET
void hard_reset(void);
void reset_system(void);
#else
#define hard_reset() do {} while(0)
#define reset_system() do {} while(0)
#endif
void soft_reset(void);

#endif
