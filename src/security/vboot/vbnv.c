/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <string.h>
#include <types.h>
#include <security/vboot/misc.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/vbnv_layout.h>
#include <vb2_api.h>

static bool vbnv_initialized;

/* Return CRC-8 of the data, using x^8 + x^2 + x + 1 polynomial. */
static uint8_t crc8_vbnv(const uint8_t *data, int len)
{
	unsigned int crc = 0;
	int i, j;

	for (j = len; j; j--, data++) {
		crc ^= (*data << 8);
		for (i = 8; i; i--) {
			if (crc & 0x8000)
				crc ^= (0x1070 << 3);
			crc <<= 1;
		}
	}

	return (uint8_t)(crc >> 8);
}

void vbnv_reset(uint8_t *vbnv_copy)
{
	memset(vbnv_copy, 0, VBOOT_VBNV_BLOCK_SIZE);
}

/* Verify VBNV header and checksum. */
int verify_vbnv(uint8_t *vbnv_copy)
{
	return (HEADER_SIGNATURE == (vbnv_copy[HEADER_OFFSET] & HEADER_MASK)) &&
		(crc8_vbnv(vbnv_copy, CRC_OFFSET) == vbnv_copy[CRC_OFFSET]);
}

/* Re-generate VBNV checksum. */
void regen_vbnv_crc(uint8_t *vbnv_copy)
{
	vbnv_copy[CRC_OFFSET] = crc8_vbnv(vbnv_copy, CRC_OFFSET);
}

/*
 * Read VBNV data from configured storage backend.
 * If VBNV verification fails, reset the vbnv copy.
 */
void read_vbnv(uint8_t *vbnv_copy)
{
	if (CONFIG(VBOOT_VBNV_CMOS))
		read_vbnv_cmos(vbnv_copy);
	else if (CONFIG(VBOOT_VBNV_FLASH))
		read_vbnv_flash(vbnv_copy);
	else
		dead_code();

	/* Check data for consistency */
	if (!verify_vbnv(vbnv_copy))
		vbnv_reset(vbnv_copy);
}

/*
 * Write VBNV data to configured storage backend.
 * This assumes that the caller has updated the CRC already.
 */
void save_vbnv(const uint8_t *vbnv_copy)
{
	if (CONFIG(VBOOT_VBNV_CMOS))
		save_vbnv_cmos(vbnv_copy);
	else if (CONFIG(VBOOT_VBNV_FLASH))
		save_vbnv_flash(vbnv_copy);
	else
		dead_code();
}

/* Read the USB Device Controller(UDC) enable flag from VBNV. */
int vbnv_udc_enable_flag(void)
{
	struct vb2_context *ctx = vboot_get_context();

	/* This function is expected to be called after temporary nvdata storage in vboot
	   context is initialized. */
	assert(vbnv_initialized);

	return (ctx->nvdata[DEV_FLAGS_OFFSET] & DEV_ENABLE_UDC) ? 1 : 0;
}

void vbnv_init(void)
{
	struct vb2_context *ctx;

	/* NV data already initialized and read */
	if (vbnv_initialized)
		return;

	ctx = vboot_get_context();
	if (CONFIG(VBOOT_VBNV_CMOS))
		vbnv_init_cmos(ctx->nvdata);
	read_vbnv(ctx->nvdata);
	vbnv_initialized = true;
}
