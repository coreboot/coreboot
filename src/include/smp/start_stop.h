#ifndef SMP_START_STOP_H
#define SMP_START_STOP_H

#ifdef SMP
#include <smp/atomic.h>
unsigned long this_processors_id(void);
int processor_index(unsigned long processor_id);
void stop_cpu(unsigned long processor_id);
int start_cpu(unsigned long processor_id);
void startup_other_cpus(unsigned long *processor_map);
#else
#define this_processors_id()	0
#define startup_other_cpus(p)	do {} while(0)
#define processor_index(p) 0
#endif

#endif /* SMP_START_STOP_H */
