/*
 *   UBL, The Universal Talkware Boot Loader 
 *    Copyright (C) 2000 Universal Talkware Inc.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
 *
 * $Id$
 */

#include <arch/io.h>
#include <printk.h>
#include <string.h>
#include <pc80/ide.h>

static __inline__ int wait_for_notbusy(unsigned base)
{
	unsigned i = 0;

	do {
		if (((inb_p(IDE_REG_ERROR(base)) & 0x80) != 0x80) &&
		    ((inb_p(IDE_REG_STATUS(base)) & 0x80) != 0x80))
		{
			return 0;
		}
		i++;
	} while (i != 0);

	return 1;
}

static __inline__ int wait_for_dataready(unsigned base)
{
	unsigned i = 0;

	do {
		if (((inb_p(IDE_REG_ERROR(base)) & 0x80) != 0x80) &&
		    ((inb_p(IDE_REG_STATUS(base)) & 0x88) != 0x88))
		{
			return 0;
		}
		i++;
	} while (i != 0);

	return 1;
}

static __inline__ int write_command(
	unsigned base,
	ide_command_t command, 
	ide_cmd_param_t * params) 
{
	if (wait_for_notbusy(base) != 0)
		return 1;

	outb_p(params->precomp, IDE_REG_PRECOMP(base));
	outb_p(params->sector_count, IDE_REG_SECTOR_COUNT(base));
	outb_p(params->sector_number, IDE_REG_SECTOR_NUMBER(base));
	outb_p(params->cylinder & 0xFF, IDE_REG_CYLINDER_LSB(base));
	outb_p((params->cylinder >> 8) & 0x03, IDE_REG_CYLINDER_MSB(base));
	outb_p(params->drivehead, IDE_REG_DRIVEHEAD(base));

	if (wait_for_notbusy(base) != 0)
		return 1;

	outb_p(command, IDE_REG_COMMAND(base));

	return 0;
}

int ide_shutdown(void)
{
	outb_p(IDE_CMD_STANDBY_IMMEDIATE, IDE_REG_COMMAND(IDE_BASE1));
	outb_p(IDE_CMD_STANDBY_IMMEDIATE2, IDE_REG_COMMAND(IDE_BASE1));
	return 0;
}

int ide_read_data(unsigned base, void * buf, size_t size)
{
	register unsigned short * ptr = (unsigned short *) buf;

	if (wait_for_dataready(base)) {
		printk_info("data not ready...\n");
		return 1;
	}

	while (size > 1) {
		*ptr++ = inw_p(IDE_REG_DATA(base));
		size -= sizeof(unsigned short);
	}

	return 0;
}

int ide_write_data(unsigned base, void * buf, size_t size)
{
	register unsigned short * ptr = (unsigned short *) buf;

	if (wait_for_dataready(base))
		return 1;
	while (size > 1) {
		outw_p(*ptr, IDE_REG_DATA(base));
		ptr++;
		size -= sizeof(unsigned short);
	}

	return 0;
}

harddisk_info_t harddisk_info[NUM_HD];

static char buffer[512];

static int init_drive(unsigned base, int driveno)
{
	volatile int delay;
	ide_cmd_param_t cmd = IDE_DEFAULT_COMMAND;
	unsigned char command_val;
	unsigned short* drive_info;

	harddisk_info[driveno].controller_port = base;
	harddisk_info[driveno].num_heads = 0u;
	harddisk_info[driveno].num_cylinders = 0u;
	harddisk_info[driveno].num_sectors_per_track = 0u;
	harddisk_info[driveno].num_sectors = 0ul;
	harddisk_info[driveno].address_mode = IDE_DH_CHS;
	harddisk_info[driveno].drive_exists = 0;

	cmd.drivehead = IDE_DH_DEFAULT | IDE_DH_HEAD(0) | IDE_DH_CHS |
		IDE_DH_DRIVE(driveno);
	write_command(base, IDE_CMD_GET_INFO, &cmd);

	if ((inb_p(IDE_REG_STATUS(base)) & 1) != 0) {
		/* Well, if that command didn't work, we probably don't have drive. */
		printk_info("Drive %d: detect FAILED\n", driveno);
		return 1;
	}
	ide_read_data(base, buffer, IDE_SECTOR_SIZE);

	/* Now suck the data out */
	drive_info = (unsigned short*)buffer;
	harddisk_info[driveno].num_heads = drive_info[3];
	harddisk_info[driveno].num_cylinders = drive_info[1];
	harddisk_info[driveno].num_sectors_per_track = drive_info[6];
	harddisk_info[driveno].num_sectors = *((unsigned int*)&(drive_info[60]));
	memcpy(harddisk_info[driveno].model_number, ((unsigned short*)&(drive_info[27])), 40);
	harddisk_info[driveno].drive_exists = 1;

	printk_info("%s \n", harddisk_info[driveno].model_number);

	printk_info(__FUNCTION__ " sectors_per_track=[%d], num_heads=[%d], num_cylinders=[%d]\n",
		     harddisk_info[driveno].num_sectors_per_track,
		     harddisk_info[driveno].num_heads,
		     harddisk_info[driveno].num_cylinders);

#define HD harddisk_info[driveno]
	if(drive_info[49] != 0) {
		printk_info("IDE%d %d/%d/%d cap: %04x\n",
			     (int)driveno,
			     (int)HD.num_heads, (int)HD.num_cylinders,
			     (int)HD.num_sectors_per_track, 
			     (int) drive_info[49]);
	}

	if (drive_info[49] & 0x200) { /* bit 9 of capability word is lba supported bit */
		harddisk_info[driveno].address_mode = IDE_DH_LBA;
	} else {
		harddisk_info[driveno].address_mode = IDE_DH_CHS;
	}
	//  harddisk_info[driveno].address_mode = IDE_DH_CHS;

	/* Set up the Extended control register */
	if (harddisk_info[driveno].num_heads > 8) {
		command_val = 0x0A;
	} else {
		command_val = 0x02;
	}

	outb_p(command_val, IDE_REG_CONTROL(base));

	/* Execute the drive diagnostics command */
	write_command(base, IDE_CMD_DRIVE_DIAG, &cmd);
	if ((inb_p(IDE_REG_STATUS(base)) & 1) != 0) {
		return 1;
	}

	/* Reset the bus (again) */
	outb_p(cmd.drivehead, IDE_REG_DRIVEHEAD(base));
	outb_p(0x04, IDE_REG_CONTROL(base));
	for (delay = 0x100; delay > 0; --delay);
	outb_p(command_val, IDE_REG_CONTROL(base));

	/* Now do a drive recalibrate */
	write_command(base, IDE_CMD_RECALIBRATE, &cmd);
	if ((inb_p(IDE_REG_STATUS(base)) & 1) != 0) {
		return 1;
	}
	(void)wait_for_notbusy(base);

	/* Set device parameters */
	cmd.sector_count = harddisk_info[driveno].num_sectors_per_track;
	cmd.drivehead = IDE_DH_DEFAULT | 
		IDE_DH_HEAD(harddisk_info[driveno].num_heads) |
		IDE_DH_DRIVE(driveno) |
		harddisk_info[driveno].address_mode;
	write_command(base, IDE_CMD_SET_PARAMS, &cmd);

	/* Set multiple mode */
	cmd.sector_count = 0x10; /* Single-word DMA, mode 0 */
	cmd.drivehead = IDE_DH_DEFAULT | 
		IDE_DH_HEAD(0) |
		IDE_DH_DRIVE(driveno) |
		harddisk_info[driveno].address_mode;
	write_command(base, IDE_CMD_SET_MULTIMODE, &cmd);

	/* Make sure command is still OK */
	outb_p(command_val, IDE_REG_CONTROL(base));

	/* Set parameters _again_ */
	cmd.sector_count = harddisk_info[driveno].num_sectors_per_track;
	cmd.drivehead = IDE_DH_DEFAULT | 
		IDE_DH_HEAD(harddisk_info[driveno].num_heads) |
		IDE_DH_DRIVE(driveno) |
		harddisk_info[driveno].address_mode;
	write_command(base, IDE_CMD_SET_PARAMS, &cmd);

	/* Make sure command is still OK */
	outb_p(command_val, IDE_REG_CONTROL(base));

#if 0
	/* Exercise the drive to see if it works OK */
	printk_info("Exercising HardDisk- buffer=0x%08lX\n", (unsigned long) buffer);
	outb_p(0x42, 0xeb);
	while (1) {
		for (idx = 0; idx < harddisk_info[driveno].num_sectors; ++idx) {
			outb_p(idx & 0xFF, 0x80);
			retval = ide_read_sector(driveno, buffer, idx);
			if (retval != 0) {
				printk_info("readsector(driveno=%d, sector=%lu) returned %d!\n",
					     driveno, (unsigned long) idx, retval);
			}
		}
		printk_info("Exercise complete!\n");
		outb(0x42, 0xeb);
	}
#endif /* 0 */

	return 0;
}


static int init_controller(unsigned base, int basedrive) {
	volatile int delay;

	/* First, check to see if the controller even exists */
	outb_p(0x5, IDE_REG_SECTOR_COUNT(base));
	if (inb_p(IDE_REG_SECTOR_COUNT(base)) != 0x5) {
		printk_info("Controller %d: detect FAILED (1)\n", basedrive / 2);
		return -1;
	}
	outb_p(0xA, IDE_REG_SECTOR_COUNT(base));
	if (inb_p(IDE_REG_SECTOR_COUNT(base)) != 0xA) {
		printk_info("Controller %d: detect FAILED (2)\n", basedrive / 2);
		return -2;
	}

	/* Reset the system */
	outb_p(0x4, IDE_REG_CONTROL(base));
	for (delay = 0x100; delay > 0; --delay);
	outb_p(0x0, IDE_REG_CONTROL(base));

	/* Now initialize the individual drives */
	init_drive(base, basedrive);
	init_drive(base, basedrive+1);

	return 0;
}


int ide_init(void)
{
	outb_p(0x42, 0xEB);
	printk_info ("I am now initializing the ide system\n");

	if (init_controller(IDE_BASE1, 0) < 0) {
		printk_info ("Initializing the main controller failed!\n");
		/* error return error */
		return -1;
	};

#if (NUM_HD > 3)
	init_controller(IDE_BASE2, 2);
#endif

#if (NUM_HD > 5)
	init_controller(IDE_BASE3, 4);
#endif

#if (NUM_HD > 7)
	init_controller(IDE_BASE4, 6);
#endif
	return 0;
}

/* read a sector or a partial sector */
int ide_read_sector(int drive, void * buffer, unsigned int block, int byte_offset, 
		    int n_bytes) {
	ide_cmd_param_t cmd = IDE_DEFAULT_COMMAND;
	unsigned base;
	unsigned char sect_buffer[IDE_SECTOR_SIZE];
	unsigned int track;
	int status;
	int address_mode = harddisk_info[drive].address_mode;
	//int i;

	printk_info(__FUNCTION__ " drive[%d], buffer[%08x], block[%08x], offset[%d], n_bytes[%d]\n",
	     drive, buffer, block, byte_offset, n_bytes);
	    printk_info(__FUNCTION__ " block(%08x) to addr(%08x)\r", block, (int)buffer);
	if ((drive < 0) || (drive >= NUM_HD) ||
	    (harddisk_info[drive].drive_exists == 0))
	{
		printk_info("unknown drive\n");
		return 1;
	}
	base = harddisk_info[drive].controller_port;

	if (harddisk_info[drive].num_heads > 8) {
		outb_p(0xA, IDE_REG_CONTROL(base));
	} else {
		outb_p(0x2, IDE_REG_CONTROL(base));
	}

	cmd.sector_count = 1;

	if (address_mode == IDE_DH_CHS) {
		track = block / harddisk_info[drive].num_sectors_per_track;

		cmd.sector_number = 1+(block % harddisk_info[drive].num_sectors_per_track);
		cmd.cylinder = track / harddisk_info[drive].num_heads;
		cmd.drivehead = IDE_DH_DEFAULT | 
			IDE_DH_HEAD(track % harddisk_info[drive].num_heads) |
			IDE_DH_DRIVE(drive) |
			IDE_DH_CHS;
		printk_info(__FUNCTION__ " CHS: track=[%d], sector_number=[%d], cylinder=[%d]\n", track, cmd.sector_number, cmd.cylinder);
		/*
		*/
	} else {
#if 1
		cmd.sector_number = block & 0xff; /* lower byte of block (lba) */
		cmd.cylinder = (block >> 8) & 0xffff; /* middle 2 bytes of block (lba) */
		cmd.drivehead = IDE_DH_DEFAULT | /* set bits that must be on */
			((block >> 24) & 0x0f) | /* lower nibble of byte 3 of block */
			IDE_DH_DRIVE(drive) |
			IDE_DH_LBA;
#else
		cmd.sector_number = (block >> 24) & 0xff; /* byte 0 of block (lba) */
		cmd.cylinder = (block >> 8) & 0xffff; /* bytes 1 & 2 of block (lba) */
		cmd.drivehead = IDE_DH_DEFAULT | /* set bits that must be on */
			((block >> 4) & 0x0f) | /* upper nibble of byte 3 of block */
			IDE_DH_DRIVE(drive) |
			IDE_DH_LBA;
#endif
		printk_info(__FUNCTION__ " LBA: drivehead[%0x], cylinder[%04x], sector[%0x], block[%8x]\n",
		  cmd.drivehead, cmd.cylinder, cmd.sector_number, block & 0x0fffffff);
		/*
		*/
	}
	
	write_command(base, IDE_CMD_READ_MULTI_RETRY, &cmd);
	if ((inb_p(IDE_REG_STATUS(base)) & 1) != 0) {
		printk_info("ide not ready...\n");
		return 1;
	}
	if (n_bytes != IDE_SECTOR_SIZE) {
		status = ide_read_data(base, sect_buffer, IDE_SECTOR_SIZE);
		if (status == 0) {
			memcpy(buffer, sect_buffer+byte_offset, n_bytes);
		}
	} else {
		status = ide_read_data(base, buffer, IDE_SECTOR_SIZE);
	}
	return status;
}

#if 0
/* read a sector or a partial sector */
int ide_read_sector(int drive, void * buffer, unsigned int block, int byte_offset, 
		    int n_bytes) {
	ide_cmd_param_t cmd = IDE_DEFAULT_COMMAND;
	unsigned base;
	unsigned char sect_buffer[IDE_SECTOR_SIZE];
	unsigned int track;
	int status;

	if ((drive < 0) || (drive >= NUM_HD) ||
	    (harddisk_info[drive].drive_exists == 0))
	{
		return 1;
	}
	base = harddisk_info[drive].controller_port;

	if (harddisk_info[drive].num_heads > 8) {
		outb_p(0xA, IDE_REG_CONTROL(base));
	} else {
		outb_p(0x2, IDE_REG_CONTROL(base));
	}

	track = block / harddisk_info[drive].num_sectors_per_track;

	cmd.sector_count = 1;
	cmd.sector_number = 1+(block % harddisk_info[drive].num_sectors_per_track);
	cmd.cylinder = track % harddisk_info[drive].num_heads;
	cmd.drivehead = IDE_DH_DEFAULT | 
		IDE_DH_HEAD(track / harddisk_info[drive].num_heads) |
		IDE_DH_DRIVE(drive) |
		IDE_DH_CHS;

    printk_info(__FUNCTION__ " track=[%d], sector_number=[%d], cylinder=[%d]\n",
	   track, cmd.sector_number, cmd.cylinder);
	write_command(base, IDE_CMD_READ_MULTI_RETRY, &cmd);
	if ((inb_p(IDE_REG_STATUS(base)) & 1) != 0) {
		return 1;
	}
	if (n_bytes != IDE_SECTOR_SIZE) {
		status = ide_read_data(base, sect_buffer, IDE_SECTOR_SIZE);
		if (status == 0) {
			memcpy(buffer, sect_buffer+byte_offset, n_bytes);
		}
	} else {
		status = ide_read_data(base, buffer, IDE_SECTOR_SIZE);
	}
    printk_info(__FUNCTION__ " status = [%d]\n", status);
	return status;
}
#endif
