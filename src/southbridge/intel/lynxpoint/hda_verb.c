/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/azalia_device.h>
#include <device/mmio.h>
#include <types.h>

#include "hda_verb.h"

int hda_codec_detect(u8 *base)
{
	u8 reg8;

	/* Set Bit 0 to 1 to exit reset state (BAR + 0x8)[0] */
	if (azalia_exit_reset(base) != CB_SUCCESS)
		goto no_codec;

	/* Write back the value once reset bit is set. */
	write16(base + HDA_GCAP_REG, read16(base + HDA_GCAP_REG));

	/*
	 * Clear the "State Change Status Register" STATESTS bits
	 * for each of the "SDIN Stat Change Status Flag"
	 */
	write8(base + HDA_STATESTS_REG, 0xf);

	/* Turn off the link and poll RESET# bit until it reads back as 0 */
	if (azalia_enter_reset(base) != CB_SUCCESS)
		goto no_codec;

	/* Turn on the link and poll RESET# bit until it reads back as 1 */
	if (azalia_exit_reset(base) != CB_SUCCESS)
		goto no_codec;

	/* Read in Codec location (BAR + 0xe)[2..0] */
	reg8 = read8(base + HDA_STATESTS_REG);
	reg8 &= 0x0f;
	if (!reg8)
		goto no_codec;

	return reg8;

no_codec:
	/* Codec not found, put HDA back in reset */
	azalia_enter_reset(base);
	printk(BIOS_DEBUG, "HDA: No codec!\n");
	return 0;
}
