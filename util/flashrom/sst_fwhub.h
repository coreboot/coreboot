#ifndef __SST_FWHUB_H__
#define __SST_FWHUB_H__ 1

extern int probe_sst_fwhub(struct flashchip *flash);
extern int erase_sst_fwhub(struct flashchip *flash);
extern int write_sst_fwhub(struct flashchip *flash, uint8_t *buf);

#endif				/* !__SST_FWHUB_H__ */
