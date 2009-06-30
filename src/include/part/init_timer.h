#ifndef PART_INIT_TIMER_H
#define PART_DELAY_H

#if CONFIG_HAVE_INIT_TIMER == 1
void init_timer(void);
#else
#define init_timer() do{} while(0)
#endif

#endif /* PART_INIT_TIMER_H */
