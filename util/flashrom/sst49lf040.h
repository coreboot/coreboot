#ifndef __SST49LF040_H__
#define __SST49LF040_H__ 1

extern int erase_49lf040(struct flashchip *flash);
extern int write_49lf040(struct flashchip *flash, uint8_t *buf);

#endif				/* !__SST49LF040_H__ */
