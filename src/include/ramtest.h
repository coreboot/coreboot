#ifndef RAMTEST_H
#define RAMTEST_H

void ram_fill(unsigned long start, unsigned long stop);
int ram_verify(unsigned long start, unsigned long stop, int max_errors);
int ramcheck(unsigned long start, unsigned long stop, int max_errors);

#endif /* RAMTEST_H */
