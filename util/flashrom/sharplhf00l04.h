#ifndef __SHARPLHF00L04_H__
#define __SHARPLHF00L04_H__ 1

extern int probe_lhf00l04(struct flashchip *flash);
extern int erase_lhf00l04(struct flashchip *flash);
extern int write_lhf00l04(struct flashchip *flash, uint8_t *buf);
extern void toggle_ready_lhf00l04(volatile uint8_t *dst);
extern void data_polling_lhf00l04(volatile uint8_t *dst, uint8_t data);
extern void protect_lhf00l04(volatile uint8_t *bios);

#endif				/* !__SHARPLHF00L04_H__ */
