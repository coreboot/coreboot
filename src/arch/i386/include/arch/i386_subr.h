#ifndef I386_SUBR_H
#define I386_SUBR_H

struct mem_range;
void cache_on(struct mem_range *mem);
void interrupts_on(void);
 
#endif /* I386_SUBR_H */
