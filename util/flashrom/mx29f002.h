#ifndef __MX29F002_H__
#define __MX29F002_H__ 1

extern int probe_29f002(struct flashchip *flash);
extern int erase_29f002(struct flashchip *flash);
extern int write_29f002(struct flashchip *flash, uint8_t *buf);

#endif				/* !__MX29F002_H__ */
