/*
 *   UBL, The Universal Talkware Boot Loader 
 *    Copyright (C) 2000 Universal Talkware Inc.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version. 
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details. 
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * Note: Parts of this code grew out of the Openbios effort that was 
 *       abandoned. Without the info that we were able to learn from
 *       OpenBios this program would have never worked. We are forever
 *       grateful for those who came before us.
 *
 *   This code can be retrieved in a machine/human readable form at:
 *
 *   		http://www.talkware.net/GPL/UBL
 *  
 *   Anyone making changes to this code please send them to us so we 
 *   can include them in the standard release.
 *
 *  $Id$
 *
 */
#if !defined(IDE_H_INCLUDE)
#define IDE_H_INCLUDE

#define IDE_WRITE

#include <types.h>

typedef struct {
    unsigned short controller_port;
    unsigned short num_heads;
    unsigned short num_cylinders;
    unsigned short num_sectors_per_track;
    unsigned long num_sectors; /* total */
    unsigned char address_mode;	/* am i lba (0x40) or chs (0x00) */
    unsigned char model_number[40];
    unsigned char drive_exists;
} harddisk_info_t;

#define NUM_HD (2)

extern harddisk_info_t harddisk_info[NUM_HD];

extern int ide_init(void);
extern int ide_read_sector(int driveno, void * buf, unsigned int sector,
			   int byte_offset, int n_bytes);
#ifdef IDE_WRITE
extern int ide_write_sector(int driveno, void * buf, unsigned int sector);
#endif

extern int ide_read_data(unsigned base, void * buf, size_t size);
extern int ide_write_data(unsigned base, void * buf, size_t size);
extern int ide_shutdown(void);



#if 1
#define PORT80(x) (outb_p(x, 0x80))
#else
#define PORT80(x) (0)
#endif

#define IDE_SECTOR_SIZE 0x200

#define IDE_BASE1             (0x1F0u) /* primary controller */
#define IDE_BASE2             (0x170u) /* secondary */
#define IDE_BASE3             (0x0F0u) /* third */
#define IDE_BASE4             (0x070u) /* fourth */

#define IDE_REG_EXTENDED_OFFSET   (0x200u)

#define IDE_REG_DATA(base)          ((base) + 0u) /* word register */
#define IDE_REG_ERROR(base)         ((base) + 1u)
#define IDE_REG_PRECOMP(base)       ((base) + 1u)
#define IDE_REG_SECTOR_COUNT(base)  ((base) + 2u)
#define IDE_REG_SECTOR_NUMBER(base) ((base) + 3u)
#define IDE_REG_CYLINDER_LSB(base)  ((base) + 4u)
#define IDE_REG_CYLINDER_MSB(base)  ((base) + 5u)
#define IDE_REG_DRIVEHEAD(base)     ((base) + 6u)
#define IDE_REG_STATUS(base)        ((base) + 7u)
#define IDE_REG_COMMAND(base)       ((base) + 7u)
#define IDE_REG_ALTSTATUS(base)     ((base) + IDE_REG_EXTENDED_OFFSET + 6u)
#define IDE_REG_CONTROL(base)       ((base) + IDE_REG_EXTENDED_OFFSET + 6u)
#define IDE_REG_ADDRESS(base)       ((base) + IDE_REG_EXTENDED_OFFSET + 7u)

typedef struct {
    unsigned char precomp;
    unsigned char sector_count;
    unsigned char sector_number;
    unsigned short cylinder;
    unsigned char drivehead;
#       define IDE_DH_DEFAULT (0xA0)
#       define IDE_DH_HEAD(x) ((x) & 0x0F)
#       define IDE_DH_MASTER  (0x00)
#       define IDE_DH_SLAVE   (0x10)
#       define IDE_DH_DRIVE(x) ((((x) & 1) != 0)?IDE_DH_SLAVE:IDE_DH_MASTER)
#       define IDE_DH_LBA     (0x40)
#       define IDE_DH_CHS     (0x00)

} ide_cmd_param_t;

#define IDE_DEFAULT_COMMAND { 0xFFu, 0x01, 0x00, 0x0000, IDE_DH_DEFAULT }

typedef enum {
    IDE_CMD_NOOP = 0,
    IDE_CMD_RECALIBRATE = 0x10,
    IDE_CMD_READ_MULTI_RETRY = 0x20,
    IDE_CMD_WRITE_MULTI_RETRY = 0x30,
    IDE_CMD_READ_MULTI = IDE_CMD_READ_MULTI_RETRY,
    IDE_CMD_READ_MULTI_NORETRY = 0x21,

    IDE_CMD_DRIVE_DIAG = 0x90,
    IDE_CMD_SET_PARAMS = 0x91,
    IDE_CMD_STANDBY_IMMEDIATE = 0x94, /* 2 byte command- also send 
                                         IDE_CMD_STANDBY_IMMEDIATE2 */
    IDE_CMD_SET_MULTIMODE = 0xC6,
    IDE_CMD_STANDBY_IMMEDIATE2 = 0xE0,
    IDE_CMD_GET_INFO = 0xEC
} ide_command_t;


#endif /* IDE_H_INCLUDE */










