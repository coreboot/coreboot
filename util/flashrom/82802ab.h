#ifndef __82802AB_H__
#define __82802AB_H__ 1

extern int probe_82802ab(struct flashchip *flash);
extern int erase_82802ab(struct flashchip *flash);
extern int write_82802ab(struct flashchip *flash, uint8_t *buf);

#endif				/* !__82802AB_H__ */
