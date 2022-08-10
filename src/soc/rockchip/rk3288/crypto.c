/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/soc.h>
#include <types.h>
#include <vb2_api.h>

enum rk3288_crypto_interrupt_bits {
	PKA_DONE	= 1 << 5,
	HASH_DONE	= 1 << 4,
	HRDMA_ERR	= 1 << 3,
	HRDMA_DONE	= 1 << 2,
	BCDMA_ERR	= 1 << 1,
	BCDMA_DONE	= 1 << 0,
};

struct rk3288_crypto {
	u32 intsts;
	u32 intena;
	u32 ctrl;
	u32 conf;
	u32 brdmas;
	u32 btdmas;
	u32 btdmal;
	u32 hrdmas;
	u32 hrdmal;
	u8 _res0[0x80 - 0x24];
	u32 aes_ctrl;
	u32 aes_sts;
	u32 aes_din[4];
	u32 aes_dout[4];
	u32 aes_iv[4];
	u32 aes_key[8];
	u32 aes_cnt[4];
	u8 _res1[0x100 - 0xe8];
	u32 tdes_ctrl;
	u32 tdes_sts;
	u32 tdes_din[2];
	u32 tdes_dout[2];
	u32 tdes_iv[2];
	u32 tdes_key[3][2];
	u8 _res2[0x180 - 0x138];
	u32 hash_ctrl;
	u32 hash_sts;
	u32 hash_msg_len;
	u32 hash_dout[8];
	u32 hash_seed[5];
	u8 _res3[0x200 - 0x1c0];
	u32 trng_ctrl;
	u32 trng_dout[8];
} *crypto = (void *)CRYPTO_BASE;
check_member(rk3288_crypto, trng_dout[7], 0x220);

vb2_error_t vb2ex_hwcrypto_digest_init(enum vb2_hash_algorithm hash_alg,
				       uint32_t data_size)
{
	if (hash_alg != VB2_HASH_SHA256 || !data_size)
		return VB2_ERROR_EX_HWCRYPTO_UNSUPPORTED;

	write32(&crypto->ctrl, RK_SETBITS(1 << 6));	/* Assert HASH_FLUSH */
	udelay(1);					/* for 10+ cycles to */
	write32(&crypto->ctrl, RK_CLRBITS(1 << 6));	/* clear out old hash */

	/* Enable DMA byte swapping for little-endian bus (Byteswap_??FIFO) */
	write32(&crypto->conf, 1 << 5 | 1 << 4 | 1 << 3);

	write32(&crypto->intena, HRDMA_ERR | HRDMA_DONE); /* enable interrupt */

	write32(&crypto->hash_msg_len, data_size);	/* program total size */
	write32(&crypto->hash_ctrl, 1 << 3 | 0x2);	/* swap DOUT, SHA256 */

	printk(BIOS_DEBUG, "Initialized RK3288 HW crypto for %u byte SHA256\n",
	       data_size);
	return VB2_SUCCESS;
}

vb2_error_t vb2ex_hwcrypto_digest_extend(const uint8_t *buf, uint32_t size)
{
	uint32_t intsts;

	write32(&crypto->intsts, HRDMA_ERR | HRDMA_DONE); /* clear interrupts */

	/* NOTE: This assumes that the DMA is reading from uncached SRAM. */
	write32(&crypto->hrdmas, (uint32_t)buf);
	write32(&crypto->hrdmal, size / sizeof(uint32_t));
	write32(&crypto->ctrl, RK_SETBITS(1 << 3));	/* Set HASH_START */
	do {
		intsts = read32(&crypto->intsts);
		if (intsts & HRDMA_ERR) {
			printk(BIOS_ERR, "DMA error during HW crypto\n");
			return VB2_ERROR_UNKNOWN;
		}
	} while (!(intsts & HRDMA_DONE));	/* wait for DMA to finish */

	return VB2_SUCCESS;
}

vb2_error_t vb2ex_hwcrypto_digest_finalize(uint8_t *digest,
					   uint32_t digest_size)
{
	uint32_t *dest = (uint32_t *)digest;
	uint32_t *src = crypto->hash_dout;
	assert(digest_size == sizeof(crypto->hash_dout));

	while (!(read32(&crypto->hash_sts) & 0x1))
		/* wait for crypto engine to set HASH_DONE bit */;

	while ((uint8_t *)dest < digest + digest_size)
		*dest++ = read32(src++);

	return VB2_SUCCESS;
}
