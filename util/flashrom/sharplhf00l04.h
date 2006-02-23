#ifndef __SHARPLHF00L04_H__
#define __SHARPLHF00L04_H__ 1

extern int probe_lhf00l04(struct flashchip *flash);
extern int erase_lhf00l04(struct flashchip *flash);
extern int write_lhf00l04(struct flashchip *flash, uint8_t *buf);

#endif				/* !__SHARPLHF00L04_H__ */
