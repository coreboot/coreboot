#ifndef __AM29F040B_H__
#define __AM29F040B_H__ 1

extern int probe_29f040b(struct flashchip *flash);
extern int erase_29f040b(struct flashchip *flash);
extern int write_29f040b(struct flashchip *flash, uint8_t *buf);

#endif				/* !__AM29F040B_H__ */
