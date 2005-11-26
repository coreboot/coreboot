#ifndef __SST28SF040_H__
#define __SST28SF040_H__

extern int probe_28sf040(struct flashchip *flash);
extern int erase_28sf040(struct flashchip *flash);
extern int write_28sf040(struct flashchip *flash, uint8_t *buf);

#endif				/* !__SST28SF040_H__ */
