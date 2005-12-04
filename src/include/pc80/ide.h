/*
 *   UBL, The Universal Talkware Boot Loader 
 *    Copyright (C) 2000 Universal Talkware Inc.
 *    Copyright (C) 2002 Eric Biederman
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
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 *
 */

typedef uint64_t sector_t;

struct controller {
	uint16_t cmd_base;
	uint16_t ctrl_base;
};

struct harddisk_info {
	struct controller *ctrl;
	uint16_t heads;
	uint16_t cylinders;
	uint16_t sectors_per_track;
	uint8_t  model_number[41];
	uint8_t  slave;
	sector_t sectors;
	int  address_mode;	/* am i lba (0x40) or chs (0x00) */
#define ADDRESS_MODE_CHS    0
#define ADDRESS_MODE_LBA    1
#define ADDRESS_MODE_LBA48  2
#define ADDRESS_MODE_PACKET 3
	uint32_t hw_sector_size;
	unsigned drive_exists : 1;
	unsigned slave_absent : 1;
	unsigned removable : 1;
};


#define IDE_SECTOR_SIZE 0x200
#define CDROM_SECTOR_SIZE 0x800

#define IDE_BASE0             (0x1F0u) /* primary controller */
#define IDE_BASE1             (0x170u) /* secondary */
#define IDE_BASE2             (0x0F0u) /* third */
#define IDE_BASE3             (0x070u) /* fourth */

#define IDE_REG_EXTENDED_OFFSET   (0x204u)

#define IDE_REG_DATA(ctrl)           ((ctrl)->cmd_base + 0u) /* word register */
#define IDE_REG_ERROR(ctrl)          ((ctrl)->cmd_base + 1u)
#define IDE_REG_PRECOMP(ctrl)        ((ctrl)->cmd_base + 1u)
#define IDE_REG_FEATURE(ctrl)        ((ctrl)->cmd_base + 1u)
#define IDE_REG_SECTOR_COUNT(ctrl)   ((ctrl)->cmd_base + 2u)
#define IDE_REG_SECTOR_NUMBER(ctrl)  ((ctrl)->cmd_base + 3u)
#define IDE_REG_LBA_LOW(ctrl)        ((ctrl)->cmd_base + 3u)
#define IDE_REG_CYLINDER_LSB(ctrl)   ((ctrl)->cmd_base + 4u)
#define IDE_REG_LBA_MID(ctrl)	     ((ctrl)->cmd_base + 4u)
#define IDE_REG_CYLINDER_MSB(ctrl)   ((ctrl)->cmd_base + 5u)
#define IDE_REG_LBA_HIGH(ctrl)	     ((ctrl)->cmd_base + 5u)
#define IDE_REG_DRIVEHEAD(ctrl)      ((ctrl)->cmd_base + 6u)
#define IDE_REG_DEVICE(ctrl)	     ((ctrl)->cmd_base + 6u)
#define IDE_REG_STATUS(ctrl)         ((ctrl)->cmd_base + 7u)
#define IDE_REG_COMMAND(ctrl)        ((ctrl)->cmd_base + 7u)
#define IDE_REG_ALTSTATUS(ctrl)      ((ctrl)->ctrl_base + 2u)
#define IDE_REG_DEVICE_CONTROL(ctrl) ((ctrl)->ctrl_base + 2u)

struct ide_pio_command
{
	uint8_t feature;
	uint8_t sector_count;
	uint8_t lba_low;
	uint8_t lba_mid;
	uint8_t lba_high;
	uint8_t device;
#       define IDE_DH_DEFAULT (0xA0)
#       define IDE_DH_HEAD(x) ((x) & 0x0F)
#       define IDE_DH_MASTER  (0x00)
#       define IDE_DH_SLAVE   (0x10)
#       define IDE_DH_LBA     (0x40)
#       define IDE_DH_CHS     (0x00)
	uint8_t command;
	uint8_t sector_count2;
	uint8_t lba_low2;
	uint8_t lba_mid2;
	uint8_t lba_high2;
};

#define IDE_DEFAULT_COMMAND { 0xFFu, 0x01, 0x00, 0x0000, IDE_DH_DEFAULT }

#define IDE_ERR_ICRC	0x80	/* ATA Ultra DMA bad CRC */
#define IDE_ERR_BBK	0x80	/* ATA bad block */
#define IDE_ERR_UNC	0x40	/* ATA uncorrected error */
#define IDE_ERR_MC	0x20	/* ATA media change */
#define IDE_ERR_IDNF	0x10	/* ATA id not found */
#define IDE_ERR_MCR	0x08	/* ATA media change request */
#define IDE_ERR_ABRT	0x04	/* ATA command aborted */
#define IDE_ERR_NTK0	0x02	/* ATA track 0 not found */
#define IDE_ERR_NDAM	0x01	/* ATA address mark not found */

#define IDE_STATUS_BSY	0x80	/* busy */
#define IDE_STATUS_RDY	0x40	/* ready */
#define IDE_STATUS_DF	0x20	/* device fault */
#define IDE_STATUS_WFT	0x20	/* write fault (old name) */
#define IDE_STATUS_SKC	0x10	/* seek complete */
#define IDE_STATUS_DRQ	0x08	/* data request */
#define IDE_STATUS_CORR	0x04	/* corrected */
#define IDE_STATUS_IDX	0x02	/* index */
#define IDE_STATUS_ERR	0x01	/* error (ATA) */
#define IDE_STATUS_CHK	0x01	/* check (ATAPI) */

#define IDE_CTRL_HD15	0x08	/* bit should always be set to one */
#define IDE_CTRL_SRST	0x04	/* soft reset */
#define IDE_CTRL_NIEN	0x02	/* disable interrupts */


/* Most mandtory and optional ATA commands (from ATA-3), */

#define IDE_CMD_CFA_ERASE_SECTORS            0xC0
#define IDE_CMD_CFA_REQUEST_EXT_ERR_CODE     0x03
#define IDE_CMD_CFA_TRANSLATE_SECTOR         0x87
#define IDE_CMD_CFA_WRITE_MULTIPLE_WO_ERASE  0xCD
#define IDE_CMD_CFA_WRITE_SECTORS_WO_ERASE   0x38
#define IDE_CMD_CHECK_POWER_MODE1            0xE5
#define IDE_CMD_CHECK_POWER_MODE2            0x98
#define IDE_CMD_DEVICE_RESET                 0x08
#define IDE_CMD_EXECUTE_DEVICE_DIAGNOSTIC    0x90
#define IDE_CMD_FLUSH_CACHE                  0xE7
#define IDE_CMD_FORMAT_TRACK                 0x50
#define IDE_CMD_IDENTIFY_DEVICE              0xEC
#define IDE_CMD_IDENTIFY_DEVICE_PACKET       0xA1
#define IDE_CMD_IDENTIFY_PACKET_DEVICE       0xA1
#define IDE_CMD_IDLE1                        0xE3
#define IDE_CMD_IDLE2                        0x97
#define IDE_CMD_IDLE_IMMEDIATE1              0xE1
#define IDE_CMD_IDLE_IMMEDIATE2              0x95
#define IDE_CMD_INITIALIZE_DRIVE_PARAMETERS  0x91
#define IDE_CMD_INITIALIZE_DEVICE_PARAMETERS 0x91
#define IDE_CMD_NOP                          0x00
#define IDE_CMD_PACKET                       0xA0
#define IDE_CMD_READ_BUFFER                  0xE4
#define IDE_CMD_READ_DMA                     0xC8
#define IDE_CMD_READ_DMA_QUEUED              0xC7
#define IDE_CMD_READ_MULTIPLE                0xC4
#define IDE_CMD_READ_SECTORS                 0x20
#define IDE_CMD_READ_SECTORS_EXT             0x24
#define IDE_CMD_READ_VERIFY_SECTORS          0x40
#define IDE_CMD_RECALIBRATE                  0x10
#define IDE_CMD_SEEK                         0x70
#define IDE_CMD_SET_FEATURES                 0xEF
#define IDE_CMD_SET_MAX_ADDR_EXT             0x24
#define IDE_CMD_SET_MULTIPLE_MODE            0xC6
#define IDE_CMD_SLEEP1                       0xE6
#define IDE_CMD_SLEEP2                       0x99
#define IDE_CMD_STANDBY1                     0xE2
#define IDE_CMD_STANDBY2                     0x96
#define IDE_CMD_STANDBY_IMMEDIATE1           0xE0
#define IDE_CMD_STANDBY_IMMEDIATE2           0x94
#define IDE_CMD_WRITE_BUFFER                 0xE8
#define IDE_CMD_WRITE_DMA                    0xCA
#define IDE_CMD_WRITE_DMA_QUEUED             0xCC
#define IDE_CMD_WRITE_MULTIPLE               0xC5
#define IDE_CMD_WRITE_SECTORS                0x30
#define IDE_CMD_WRITE_VERIFY                 0x3C

/* IDE_CMD_SET_FEATURE sub commands */
#define IDE_FEATURE_CFA_ENABLE_8BIT_PIO                     0x01
#define IDE_FEATURE_ENABLE_WRITE_CACHE                      0x02
#define IDE_FEATURE_SET_TRANSFER_MODE                       0x03
#define IDE_FEATURE_ENABLE_POWER_MANAGEMENT                 0x05
#define IDE_FEATURE_ENABLE_POWERUP_IN_STANDBY               0x06
#define IDE_FEATURE_STANDBY_SPINUP_DRIVE                    0x07
#define IDE_FEATURE_CFA_ENABLE_POWER_MODE1                  0x0A
#define IDE_FEATURE_DISABLE_MEDIA_STATUS_NOTIFICATION       0x31
#define IDE_FEATURE_ENABLE_AUTOMATIC_ACOUSTIC_MANAGEMENT    0x42
#define IDE_FEATURE_SET_MAXIMUM_HOST_INTERFACE_SECTOR_TIMES 0x43
#define IDE_FEATURE_DISABLE_READ_LOOKAHEAD                  0x55
#define IDE_FEATURE_ENABLE_RELEASE_INTERRUPT                0x5D
#define IDE_FEATURE_ENABLE_SERVICE_INTERRUPT                0x5E
#define IDE_FEATURE_DISABLE_REVERTING_TO_POWERON_DEFAULTS   0x66
#define IDE_FEATURE_CFA_DISABLE_8BIT_PIO                    0x81
#define IDE_FEATURE_DISABLE_WRITE_CACHE                     0x82
#define IDE_FEATURE_DISABLE_POWER_MANAGEMENT                0x85
#define IDE_FEATURE_DISABLE_POWERUP_IN_STANDBY              0x86
#define IDE_FEATURE_CFA_DISABLE_POWER_MODE1                 0x8A
#define IDE_FEATURE_ENABLE_MEDIA_STATUS_NOTIFICATION        0x95
#define IDE_FEATURE_ENABLE_READ_LOOKAHEAD                   0xAA
#define IDE_FEATURE_DISABLE_AUTOMATIC_ACOUSTIC_MANAGEMENT   0xC2
#define IDE_FEATURE_ENABLE_REVERTING_TO_POWERON_DEFAULTS    0xCC
#define IDE_FEATURE_DISABLE_SERVICE_INTERRUPT               0xDE

#define IDE_MAX_CONTROLLERS 2
#define IDE_MAX_DRIVES (IDE_MAX_CONTROLLERS*2)
#define SECTOR_SIZE 512
#define SECTOR_SHIFT 9

/* Maximum block_size that may be set. */
#define DISK_BUFFER_SIZE (18 * SECTOR_SIZE)

extern int ide_probe(int drive);
extern int ide_read(int drive, sector_t sector, void *buffer);
