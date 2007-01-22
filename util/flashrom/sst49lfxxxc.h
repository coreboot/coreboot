#ifndef __SST49LFXXXC_H__
#define __SST49LFXXXC_H__

extern int probe_49lfxxxc(struct flashchip *flash);
extern int erase_49lfxxxc(struct flashchip *flash);
extern int write_49lfxxxc(struct flashchip *flash, uint8_t *buf);

#endif				/* !__SST49LFXXXC_H__ */
