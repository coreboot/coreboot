#define BSY_SET_DURING_SPINUP 1
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
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 *
 */

#include <arch/io.h>
#include <printk.h>
#include <string.h>
#include <delay.h>
#include <pci.h>
#include <pc80/ide.h>
#include <arch/io.h>

struct controller    controller;
struct harddisk_info harddisk_info[NUM_HD];

static int await_ide(int (*done)(struct controller *ctrl), 
	struct controller *ctrl, unsigned long timeout)
{
	int result;
	for(;;) {
		result = done(ctrl);
		if (result) {
			return 0;
		}
		if (timeout-- == 0) {
			break;
		}
		udelay(1000); /* Added to avoid spinning GRW */
	}
	return -1;
}

/* The maximum time any IDE command can last 31 seconds,
 * So if any IDE commands takes this long we know we have problems.
 */
#define IDE_TIMEOUT (32*1000)

static int not_bsy(struct controller *ctrl)
{
	return !(inb(IDE_REG_STATUS(ctrl)) & IDE_STATUS_BSY);
}
#if  !BSY_SET_DURING_SPINUP
static int timeout(struct controller *ctrl)
{
	return 0;
}
#endif

static int ide_software_reset(struct controller *ctrl)
{
	/* Wait a little bit in case this is immediately after
	 * hardware reset.
	 */
	udelay(2000);
	/* A software reset should not be delivered while the bsy bit
	 * is set.  If the bsy bit does not clear in a reasonable
	 * amount of time give up.
	 */
	if (await_ide(not_bsy, ctrl, IDE_TIMEOUT) < 0) {
		return -1;
	}

	/* Disable Interrupts and reset the ide bus */
	outb(IDE_CTRL_HD15 | IDE_CTRL_SRST | IDE_CTRL_NIEN, 
		IDE_REG_DEVICE_CONTROL(ctrl));
	udelay(5);
	outb(IDE_CTRL_HD15 | IDE_CTRL_NIEN, IDE_REG_DEVICE_CONTROL(ctrl));
	udelay(2000);
	if (await_ide(not_bsy, ctrl, IDE_TIMEOUT) < 0) {
		return -1;
	}
	return 0;
}

static void pio_set_registers(
	struct controller *ctrl, const struct ide_pio_command *cmd)
{
	uint8_t device;
	/* Disable Interrupts */
	outb(IDE_CTRL_HD15 | IDE_CTRL_NIEN, IDE_REG_DEVICE_CONTROL(ctrl));

	/* Possibly switch selected device */
	device = inb(IDE_REG_DEVICE(ctrl));
	outb(cmd->device,          IDE_REG_DEVICE(ctrl));
	if ((device & (1UL << 4)) != (cmd->device & (1UL << 4))) {
		/* Allow time for the selected drive to switch,
		 * The linux ide code suggests 50ms is the right
		 * amount of time to use here.
		 */
		udelay(50000); 
	}
	outb(cmd->feature,         IDE_REG_FEATURE(ctrl));
	outb(cmd->sector_count2,   IDE_REG_SECTOR_COUNT(ctrl));
	outb(cmd->sector_count,    IDE_REG_SECTOR_COUNT(ctrl));
	outb(cmd->lba_low2,        IDE_REG_LBA_LOW(ctrl));
	outb(cmd->lba_low,         IDE_REG_LBA_LOW(ctrl));
	outb(cmd->lba_mid2,        IDE_REG_LBA_MID(ctrl));
	outb(cmd->lba_mid,         IDE_REG_LBA_MID(ctrl));
	outb(cmd->lba_high2,       IDE_REG_LBA_HIGH(ctrl));
	outb(cmd->lba_high,        IDE_REG_LBA_HIGH(ctrl));
	outb(cmd->command,         IDE_REG_COMMAND(ctrl));
}


static int pio_non_data(struct controller *ctrl, const struct ide_pio_command *cmd)
{
	/* Wait until the busy bit is clear */
	if (await_ide(not_bsy, ctrl, IDE_TIMEOUT) < 0) {
		return -1;
	}

	pio_set_registers(ctrl, cmd);
	if (await_ide(not_bsy, ctrl, IDE_TIMEOUT) < 0) {
		return -1;
	}
	/* FIXME is there more error checking I could do here? */
	return 0;
}

static int pio_data_in(struct controller *ctrl, const struct ide_pio_command *cmd,
	void *buffer, size_t bytes)
{
	unsigned int status;

	/* FIXME handle commands with multiple blocks */
	/* Wait until the busy bit is clear */
	if (await_ide(not_bsy, ctrl, IDE_TIMEOUT) < 0) {
		return -1;
	}

	/* How do I tell if INTRQ is asserted? */
	pio_set_registers(ctrl, cmd);
	if (await_ide(not_bsy, ctrl, IDE_TIMEOUT) < 0) {
		return -1;
	}
	status = inb(IDE_REG_STATUS(ctrl));
	if (!(status & IDE_STATUS_DRQ)) {
		return -1;
	}
	insw(IDE_REG_DATA(ctrl), buffer, bytes/2);
	status = inb(IDE_REG_STATUS(ctrl));
	if (status & IDE_STATUS_ERR) {
		return -1;
	}
	return 0;
}

static inline int ide_read_sector_chs(
	struct harddisk_info *info, void *buffer, unsigned long sector)
{
	struct ide_pio_command cmd;
	unsigned int track;
	unsigned int offset;
	unsigned int cylinder;
		
	memset(&cmd, 0, sizeof(cmd));
	cmd.sector_count = 1;

	track = sector / info->sectors_per_track;
	/* Sector number */
	offset = 1 + (sector % info->sectors_per_track);
	cylinder = track / info->heads;
	cmd.lba_low = offset;
	cmd.lba_mid = cylinder & 0xff;
	cmd.lba_high = (cylinder >> 8) & 0xff;
	cmd.device = IDE_DH_DEFAULT |
		IDE_DH_HEAD(track % info->heads) |
		info->slave |
		IDE_DH_CHS;
	cmd.command = IDE_CMD_READ_SECTORS;
	return pio_data_in(info->ctrl, &cmd, buffer, IDE_SECTOR_SIZE);
}

static inline int ide_read_sector_lba(
	struct harddisk_info *info, void *buffer, unsigned long sector)
{
	struct ide_pio_command cmd;
	memset(&cmd, 0, sizeof(cmd));

	cmd.sector_count = 1;
	cmd.lba_low = sector & 0xff;
	cmd.lba_mid = (sector >> 8) & 0xff;
	cmd.lba_high = (sector >> 16) & 0xff;
	cmd.device = IDE_DH_DEFAULT |
		((sector >> 24) & 0x0f) |
		info->slave | 
		IDE_DH_LBA;
	cmd.command = IDE_CMD_READ_SECTORS;
	return pio_data_in(info->ctrl, &cmd, buffer, IDE_SECTOR_SIZE);
}

static inline int ide_read_sector_lba48(
	struct harddisk_info *info, void *buffer, sector_t sector)
{
	struct ide_pio_command cmd;
	memset(&cmd, 0, sizeof(cmd));

	cmd.sector_count = 1;
	cmd.lba_low = sector & 0xff;
	cmd.lba_mid = (sector >> 8) & 0xff;
	cmd.lba_high = (sector >> 16) & 0xff;
	cmd.lba_low2 = (sector >> 24) & 0xff;
	cmd.lba_mid2 = (sector >> 32) & 0xff;
	cmd.lba_high2 = (sector >> 40) & 0xff;
	cmd.device =  info->slave | IDE_DH_LBA;
	cmd.command = IDE_CMD_READ_SECTORS_EXT;
	return pio_data_in(info->ctrl, &cmd, buffer, IDE_SECTOR_SIZE);
}

int ide_read_sector(int driveno, void * buf, unsigned int sector,
                           int byte_offset, int n_bytes)
{
	struct harddisk_info *info = &harddisk_info[driveno];
	int result;

	/* Report the buffer is empty */
	if (sector > info->sectors) {
		return -1;
	}
	if (info->address_mode == ADDRESS_MODE_CHS) {
		result = ide_read_sector_chs(info, buf, sector);
	}
	else if (info->address_mode == ADDRESS_MODE_LBA) {
		result = ide_read_sector_lba(info, buf, sector);
	}
	else if (info->address_mode == ADDRESS_MODE_LBA48) {
		result = ide_read_sector_lba48(info, buf, sector);
	}
	else {
		result = -1;
	}
	return result;
}

static int init_drive(struct harddisk_info *info, struct controller *ctrl, int slave, int basedrive)
{
	uint16_t* drive_info;
	struct ide_pio_command cmd;
	unsigned char disk_buffer[DISK_BUFFER_SIZE];
	int i;

	info->ctrl = ctrl;
	info->heads = 0u;
	info->cylinders = 0u;
	info->sectors_per_track = 0u;
	info->address_mode = IDE_DH_CHS;
	info->sectors = 0ul;
	info->drive_exists = 0;
	info->slave_absent = 0;
	info->slave = slave?IDE_DH_SLAVE: IDE_DH_MASTER;
	info->basedrive = basedrive;

	printk_info("Testing for disk %d\n", info->basedrive);

	/* Select the drive that we are testing */
	outb(IDE_DH_DEFAULT | IDE_DH_HEAD(0) | IDE_DH_CHS | info->slave, 
		IDE_REG_DEVICE(ctrl));
	udelay(50000);

	/* Test to see if the drive registers exist,
	 * In many cases this quickly rules out a missing drive.
	 */
	for(i = 0; i < 4; i++) {
		outb(0xaa + i, (ctrl->cmd_base) + 2 + i);
	}
	for(i = 0; i < 4; i++) {
		if (inb((ctrl->cmd_base) + 2 + i) != 0xaa + i) {
			return 1;
		}
	}
	for(i = 0; i < 4; i++) {
		outb(0x55 + i, (ctrl->cmd_base) + 2 + i);
	}
	for(i = 0; i < 4; i++) {
		if (inb((ctrl->cmd_base) + 2 + i) != 0x55 + i) {
			return 1;
		}
	}

	printk_info("Probing for disk %d\n", info->basedrive);
	
	memset(&cmd, 0, sizeof(cmd));
	cmd.device = IDE_DH_DEFAULT | IDE_DH_HEAD(0) | IDE_DH_CHS | info->slave;
	cmd.command = IDE_CMD_IDENTIFY_DEVICE;

	
	if (pio_data_in(ctrl, &cmd, disk_buffer, IDE_SECTOR_SIZE) < 0) {
		/* Well, if that command didn't work, we probably don't have drive. */
		return 1;
	}


	/* Now suck the data out */
	drive_info = (uint16_t *)disk_buffer;
	if (drive_info[2] == 0x37C8) {
		/* If the response is incomplete spin up the drive... */
		memset(&cmd, 0, sizeof(cmd));
		cmd.device = IDE_DH_DEFAULT | IDE_DH_HEAD(0) | IDE_DH_CHS |
			info->slave;
		cmd.feature = IDE_FEATURE_STANDBY_SPINUP_DRIVE;
		if (pio_non_data(ctrl, &cmd) < 0) {
			/* If the command doesn't work give up on the drive */
			return 1;
		}
		
	}
	if ((drive_info[2] == 0x37C8) || (drive_info[2] == 0x8C73)) {
		/* The response is incomplete retry the drive info command */
		memset(&cmd, 0, sizeof(cmd));
		cmd.device = IDE_DH_DEFAULT | IDE_DH_HEAD(0) | IDE_DH_CHS |
			info->slave;
		cmd.command = IDE_CMD_IDENTIFY_DEVICE;
		if(pio_data_in(ctrl, &cmd, disk_buffer, IDE_SECTOR_SIZE) < 0) {
			/* If the command didn't work give up on the drive. */
			return 1;
		}
	}
	if ((drive_info[2] != 0x37C8) &&
		(drive_info[2] != 0x738C) &&
		(drive_info[2] != 0x8C73) &&
		(drive_info[2] != 0xC837) &&
		(drive_info[2] != 0x0000)) {
		printk_info("Invalid IDE Configuration: %hx\n", drive_info[2]);
		return 1;
	}
	for(i = 27; i < 47; i++) {
		info->model_number[((i-27)<< 1)] = (drive_info[i] >> 8) & 0xff;
		info->model_number[((i-27)<< 1)+1] = drive_info[i] & 0xff;
	}
	info->model_number[40] = '\0';
	info->drive_exists = 1;

	/* See if LBA is supported */
	if (drive_info[49] & (1 << 9)) {
		info->address_mode = ADDRESS_MODE_LBA;
		info->sectors = (drive_info[61] << 16) | (drive_info[60]);
		/* Enable LBA48 mode if it is present */
		if (drive_info[83] & (1 <<10)) {
			/* Should LBA48 depend on LBA? */
			printk_info("LBA48 mode\n");
			info->address_mode = ADDRESS_MODE_LBA48;
			info->sectors = 
				(((sector_t)drive_info[103]) << 48) |
				(((sector_t)drive_info[102]) << 32) |
				(((sector_t)drive_info[101]) << 16) |
				(((sector_t)drive_info[100]) <<  0);
		}
	} else {
		info->address_mode = ADDRESS_MODE_CHS;
		info->heads = drive_info[3];
		info->cylinders = drive_info[1];
		info->sectors_per_track = drive_info[6];
		info->sectors = 
			info->sectors_per_track *
			info->heads *
			info->cylinders;
		printk_info("%s sectors_per_track=[%d], heads=[%d], cylinders=[%d]\n",
			__FUNCTION__,
			info->sectors_per_track,
			info->heads,
			info->cylinders);
	}
	/* See if we have a slave */
	if (!info->slave && (((drive_info[93] >> 14) & 3) == 1)) {
		info->slave_absent = !(drive_info[93] & (1 << 5));
	}
	/* See if we need to put the device in CFA power mode 1 */
	if ((drive_info[160] & ((1 << 15) | (1 << 13)| (1 << 12))) ==
		((1 << 15) | (1 << 13)| (1 << 12))) {
		memset(&cmd, 0, sizeof(cmd));
		cmd.device = IDE_DH_DEFAULT | IDE_DH_HEAD(0) | IDE_DH_CHS | info->slave;
		cmd.feature = IDE_FEATURE_CFA_ENABLE_POWER_MODE1;
		if (pio_non_data(ctrl, &cmd) < 0) {
			/* If I need to power up the drive, and I can't
			 * give up.
			 */
			printk_info("Cannot power up CFA device\n");
			return 1;
		}
	}
	printk_info("disk%d %dk cap: %hx\n",
		info->basedrive,
		(unsigned long)(info->sectors >> 1),
		drive_info[49]);
	return 0;
}

static int init_controller(struct controller *ctrl, int basedrive) 
{
	struct harddisk_info *info;

	/* Put the drives ide channel in a know state and wait
	 * for the drives to spinup.  
	 *
	 * In practice IDE disks tend not to respond to commands until
	 * they have spun up.  This makes IDE hard to deal with
	 * immediately after power up, as the delays can be quite
	 * long, so we must be very careful here.
	 *
	 * There are two pathological cases that must be dealt with:
	 *
	 * - The BSY bit not being set while the IDE drives spin up.
	 *   In this cases only a hard coded delay will work.  As
	 *   I have not reproduced it, and this is out of spec for
	 *   IDE drives the work around can be enabled by setting
	 *   BSY_SET_DURING_SPINUP to 0.
	 *
	 * - The BSY bit floats high when no drives are plugged in.
	 *   This case will not be detected except by timing out but
	 *   we avoid the problems by only probing devices we are
	 *   supposed to boot from.  If we don't do the probe we
	 *   will not experience the problem.
	 *
	 * So speed wise I am only slow if the BSY bit is not set
	 * or not reported by the IDE controller during spinup, which
	 * is quite rare.
	 * 
	 */
#if !BSY_SET_DURING_SPINUP
	if (await_ide(timeout, ctrl, IDE_TIMEOUT) < 0) {
		return -1;
	}
#endif
	if (ide_software_reset(ctrl) < 0) {
		return -1;
	}

	/* Note: I have just done a software reset.  It may be
	 * reasonable to just read the boot time signatures 
	 * off of the drives to see if they are present.
	 *
	 * For now I will go with just sending commands to the drives
	 * and assuming filtering out missing drives by detecting registers
	 * that won't set and commands that fail to execute properly.
	 */

	/* Now initialize the individual drives */
	info = &harddisk_info[basedrive];
	init_drive(info, ctrl, 0, basedrive & 1);

	/* at the moment this only works for the first drive */
#if 0
	if (info->drive_exists && !info->slave_absent) {
		basedrive++;
		info++;
		init_drive(info, ctrl, 1, basedrive & 1);
	}
#endif

	return 0;
}

int ide_init(void)
{
	int index;
	int drives = 0;

	/* Intialize the harddisk_info structures */
	memset(harddisk_info, 0, sizeof(harddisk_info));

	for(index = 0; index < 1; index++) {
#if 0
		/* IDE normal pci mode */
		unsigned cmd_reg, ctrl_reg;
		uint32_t cmd_base, ctrl_base;
		if (index < 2) {
			cmd_reg  = PCI_BASE_ADDRESS_0;
			ctrl_reg = PCI_BASE_ADDRESS_1;
		} else {
			cmd_reg  = PCI_BASE_ADDRESS_2;
			ctrl_reg = PCI_BASE_ADDRESS_3;
		}
		pcibios_read_config_dword(0, 0, cmd_reg, &cmd_base);
		pcibios_read_config_dword(0, 0, ctrl_reg, &ctrl_base);
		controller.cmd_base  = cmd_base  & ~3;
		controller.ctrl_base = ctrl_base & ~3;

#endif
		uint16_t base;
		base = (index < 1)?IDE_BASE0:IDE_BASE1;
		controller.cmd_base  = base;
		controller.ctrl_base = base + IDE_REG_EXTENDED_OFFSET;

		printk_info("init_controller %d at (%x, %x)\n", index, controller.cmd_base, controller.ctrl_base);

		if (init_controller(&controller, index << 1) < 0) {
			/* nothing behind the controller */
			continue;
		}
		drives++;
	}

	return drives > 0 ? 0 : -1;
}
