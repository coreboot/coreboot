#ifndef SMP_START_STOP_H
#define SMP_START_STOP_H

#ifdef SMP
#include <smp/atomic.h>
int this_processors_id(void);
void stop_cpu(int processor_id);
void start_cpu(int processor_id);
void startup_other_cpus(void);
#else
#define this_processors_id()	0
#define startup_other_cpus()	do {} while(0)
#endif

#endif /* SMP_START_STOP_H */
