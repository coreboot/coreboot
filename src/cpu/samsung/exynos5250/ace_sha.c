/*
 * Advanced Crypto Engine - SHA Firmware
 *
 * Copyright (c) 2012  Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <common.h>
#include <asm/arch/ace_sha.h>
#include <asm/arch/ace_sfr.h>

/* Maximum input data size is 8 MB. Timeout observed for data size above 8MB */
#define TIMEOUT_MS		100

#define SHA1_DIGEST_LEN		20
#define SHA256_DIGEST_LEN	32

/* SHA1 value for the message of zero length */
static const unsigned char sha1_digest_emptymsg[SHA1_DIGEST_LEN] = {
	0xDA, 0x39, 0xA3, 0xEE, 0x5E, 0x6B, 0x4B, 0x0D,
	0x32, 0x55, 0xBF, 0xFF, 0x95, 0x60, 0x18, 0x90,
	0xAF, 0xD8, 0x07, 0x09};

/* SHA256 value for the message of zero length */
static const unsigned char sha256_digest_emptymsg[SHA256_DIGEST_LEN] = {
	0xE3, 0xB0, 0xC4, 0x42, 0x98, 0xFC, 0x1C, 0x14,
	0x9A, 0xFB, 0xF4, 0xC8, 0x99, 0x6F, 0xB9, 0x24,
	0x27, 0xAE, 0x41, 0xE4, 0x64, 0x9B, 0x93, 0x4C,
	0xA4, 0x95, 0x99, 0x1B, 0x78, 0x52, 0xB8, 0x55};

int ace_sha_hash_digest(
	unsigned char *pout, unsigned char *pbuf,
	unsigned int buf_len, unsigned int hash_type)
{
	unsigned int i, reg, len;
	unsigned int *pdigest;
	ulong start;
	struct exynos_ace_sfr *ace_sha_reg =
		(struct exynos_ace_sfr *) samsung_get_base_ace_sfr();

	if (buf_len == 0) {
		/* ACE H/W cannot compute hash value for empty string */
		if (hash_type == ACE_SHA_TYPE_SHA1)
			memcpy(pout, sha1_digest_emptymsg, SHA1_DIGEST_LEN);
		else
			memcpy(pout, sha256_digest_emptymsg, SHA256_DIGEST_LEN);
		return 0;
	}

	/* Flush HRDMA */
	writel(ACE_FC_HRDMACFLUSH_ON, &ace_sha_reg->fc_hrdmac);
	writel(ACE_FC_HRDMACFLUSH_OFF, &ace_sha_reg->fc_hrdmac);

	/* Set byte swap of data in */
	writel(ACE_HASH_SWAPDI_ON | ACE_HASH_SWAPDO_ON | ACE_HASH_SWAPIV_ON,
			&ace_sha_reg->hash_byteswap);

	/* Select Hash input mux as external source */
	reg = readl(&ace_sha_reg->fc_fifoctrl);
	reg = (reg & ~ACE_FC_SELHASH_MASK) | ACE_FC_SELHASH_EXOUT;
	writel(reg, &ace_sha_reg->fc_fifoctrl);

	/* Set Hash as SHA1 or SHA256 and start Hash engine */
	reg = (hash_type == ACE_SHA_TYPE_SHA1) ?
		ACE_HASH_ENGSEL_SHA1HASH : ACE_HASH_ENGSEL_SHA256HASH;
	reg |= ACE_HASH_STARTBIT_ON;
	writel(reg, &ace_sha_reg->hash_control);

	/* Enable FIFO mode */
	writel(ACE_HASH_FIFO_ON, &ace_sha_reg->hash_fifo_mode);

	/* Set message length */
	writel(buf_len, &ace_sha_reg->hash_msgsize_low);
	writel(0, &ace_sha_reg->hash_msgsize_high);

	/* Set HRDMA */
	writel((unsigned int)pbuf, &ace_sha_reg->fc_hrdmas);
	writel(buf_len, &ace_sha_reg->fc_hrdmal);

	start = get_timer(0);

	while ((readl(&ace_sha_reg->hash_status) & ACE_HASH_MSGDONE_MASK) ==
		ACE_HASH_MSGDONE_OFF) {

		if (get_timer(start) > TIMEOUT_MS) {
			debug("%s: Timeout waiting for ACE\n", __func__);
			return -1;
		}
	}

	/* Clear MSG_DONE bit */
	writel(ACE_HASH_MSGDONE_ON, &ace_sha_reg->hash_status);

	/* Read hash result */
	pdigest = (unsigned int *)pout;
	len = (hash_type == ACE_SHA_TYPE_SHA1) ? 5 : 8;

	for (i = 0; i < len ; i++)
		pdigest[i] = readl(&ace_sha_reg->hash_result[i]);

	/* Clear HRDMA pending bit */
	writel(ACE_FC_HRDMA, &ace_sha_reg->fc_intpend);

	return 0;
}
