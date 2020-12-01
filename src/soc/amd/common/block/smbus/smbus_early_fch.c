/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/smbus.h>
#include <soc/southbridge.h>

void fch_smbus_init(void)
{
	/* 400 kHz smbus speed. */
	const uint8_t smbus_speed = (66000000 / (400000 * 4));

	pm_write8(SMB_ASF_IO_BASE, SMB_BASE_ADDR >> 8);
	smbus_write8(SMBTIMING, smbus_speed);
	/* Clear all SMBUS status bits */
	smbus_write8(SMBHSTSTAT, SMBHST_STAT_CLEAR);
	smbus_write8(SMBSLVSTAT, SMBSLV_STAT_CLEAR);
	asf_write8(SMBHSTSTAT, SMBHST_STAT_CLEAR);
	asf_write8(SMBSLVSTAT, SMBSLV_STAT_CLEAR);
}
