#ifndef __PM49FL004_H__
#define __PM49FL004_H__ 1

extern int probe_49fl004 (struct flashchip * flash);
extern int erase_49fl004 (struct flashchip * flash);
extern int write_49fl004 (struct flashchip * flash, unsigned char * buf);

#endif 
