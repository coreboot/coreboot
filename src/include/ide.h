#ifndef IDE_H
#define IDE_H

/* read a sector or a partial sector */
extern int ide_read_sector(int drive, void * buffer, unsigned int block, int byte_offset, 
	int n_bytes);
extern int ide_init(void);

#define DRIVE_ABSENT  0
#if 1
#define DRIVE_PRESENT 1
#else
#define DRIVE_ATA     1
#define DRIVE_ATAPI   2
#endif
#define DRIVE_SHIFT 16
extern unsigned ide_probe(unsigned base, unsigned basedrive);
extern unsigned ide_spinup(unsigned base, unsigned basedrive);

#endif /* IDE_H */
