#ifndef MEM_H
#define MEM_H

struct mem_range {
	unsigned long basek;
	unsigned long sizek;
};

/* mem_range arrays are non-overlapping, in ascending order and null terminated */

struct mem_range *sizeram(void);

#endif /* MEM_H */
