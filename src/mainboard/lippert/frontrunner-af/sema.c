/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <delay.h>
#include <OEM.h>

#include <Porting.h>
#include <AGESA.h>
#include <northbridge/amd/agesa/dimmSpd.h>
#include "sema.h"

/* Write data block to slave on SMBUS0. */
#define SMB0_STATUS	((SMBUS0_BASE_ADDRESS) + 0)
#define SMB0_CONTROL	((SMBUS0_BASE_ADDRESS) + 2)
#define SMB0_HOSTCMD	((SMBUS0_BASE_ADDRESS) + 3)
#define SMB0_ADDRESS	((SMBUS0_BASE_ADDRESS) + 4)
#define SMB0_DATA0	((SMBUS0_BASE_ADDRESS) + 5)
#define SMB0_BLOCKDATA	((SMBUS0_BASE_ADDRESS) + 7)

static int smb_write_blk(u8 slave, u8 command, u8 length, const u8 *data)
{
	__outbyte(SMB0_STATUS, 0x1E);		// clear error status
	__outbyte(SMB0_ADDRESS, slave & ~1);	// slave addr + direction = out
	__outbyte(SMB0_HOSTCMD, command);	// or destination offset
	__outbyte(SMB0_DATA0, length);		// sent before data
	__inbyte(SMB0_CONTROL);			// reset block data array
	while (length--)
		__outbyte(SMB0_BLOCKDATA, *(data++));
	__outbyte(SMB0_CONTROL, 0x54);		// execute block write, no IRQ

	while (__inbyte(SMB0_STATUS) == 0x01);	// busy, no errors
	return __inbyte(SMB0_STATUS) ^ 0x02;	// 0x02 = completed, no errors
}

#define RETRY_COUNT 100

/* Use of mdelay() here would fail in romstage. */
static void early_mdelay(int msecs)
{
	while (msecs--) {
		int i;
		for (i = 0; i < 1000; i++)
			inb(0x80);
	}
}

int sema_send_alive(void)
{
	const u8 i_am_alive[] = { 0x03 };
	int i, j = 0;
	char one_spd_byte;

	/* Fake read just to setup SMBUS controller. */
	if (ENV_RAMINIT)
		smbus_readSpd(0xa0, &one_spd_byte, 1);

	/* Notify the SMC we're alive and kicking, or after a while it will
	 * effect a power cycle and switch to the alternate BIOS chip.
	 * Should be done as late as possible. */

	printk(BIOS_CRIT, "Sending BIOS alive message... ");

	do {
		i = smb_write_blk(0x50, 0x25, sizeof(i_am_alive), i_am_alive);
		early_mdelay(25);
	} while ((++j < RETRY_COUNT) && i);

	if (j == RETRY_COUNT) {
		printk(BIOS_INFO, "failed\n");
		return -1;
	}
	printk(BIOS_CRIT, "took %d tries\n", j);

	return 0;
}
