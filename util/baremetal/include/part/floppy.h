#ifndef PART_FLOPPY_H
#define PART_FLOPPY_H

#ifdef MUST_ENABLE_FLOPPY
void enable_floppy(void);
#else
#  define enable_floppy() do {} while(0)
#endif

#endif /* PART_FLOPPY_H */
