/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/smbus.h>
#include <soc/iomap.h>

static void fch_smbus_enable_decode(uint16_t base)
{
	uint32_t val = pm_read32(PM_DECODE_EN);
	/* Configure upper byte of the I/O address; lower byte is always 0 */
	val = (val & ~SMBUS_ASF_IO_BASE_MASK) | (base & SMBUS_ASF_IO_BASE_MASK);
	/* Set enable decode bit even though it should already be set */
	val |= SMBUS_ASF_IO_EN;
	pm_write32(PM_DECODE_EN, val);
}

void fch_smbus_init(void)
{
	/* 400 kHz smbus speed. */
	const uint8_t smbus_speed = (66000000 / (400000 * 4));

	fch_smbus_enable_decode(SMB_BASE_ADDR);
	smbus_write8(SMBTIMING, smbus_speed);
	/* Clear all SMBUS status bits */
	smbus_write8(SMBHSTSTAT, SMBHST_STAT_CLEAR);
	smbus_write8(SMBSLVSTAT, SMBSLV_STAT_CLEAR);
	asf_write8(SMBHSTSTAT, SMBHST_STAT_CLEAR);
	asf_write8(SMBSLVSTAT, SMBSLV_STAT_CLEAR);
}
