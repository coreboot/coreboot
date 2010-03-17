#ifndef RESET_H
#define RESET_H

#if !defined( __ROMCC__ )
/* ROMCC can't do function prototypes... */

#if CONFIG_HAVE_HARD_RESET == 1
void hard_reset(void);
#else
#define hard_reset() do {} while(0)
#endif
void soft_reset(void);

#endif
#endif
