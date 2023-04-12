/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/endian.h>
#include <console/console.h>
#include <device/device.h>
#include <device/smbus.h>

#include "pi608gp.h"
#include "chip.h"

#define PI608GP_CMD_BLK_RD_INIT	0xba
#define PI608GP_CMD_BLK_RD	0xbd
#define PI608GP_CMD_BLK_WR	0xbe
#define PI608GP_SUBCMD_RD	0x04
#define PI608GP_SUBCMD_WR	0x03

/*
 * Only some of the available registers are implemented.
 * For a full list, see the PI7C9X2G608GP datasheet.
 */
#define PI608GP_REG_SW_OPMODE	0x74
#define PI608GP_REG_PHY_PAR1	0x78
#define PI608GP_REG_TL_CSR	0x8c

#define PI608GP_EN_4B		0x0f	/* Enable all 4 data bytes in SMBus messages. */
#define PI608GP_ENCODE_ERR	0xff

static uint8_t pi608gp_encode_amp_lvl(uint32_t level_mv)
{
	/* Allowed drive amplitude levels are in units of mV in range 0 to 475 mV with 25 mV
	   steps, based on Table 6-6 from the PI7C9X2G608GP datasheet. */
	if (level_mv > 475) {
		printk(BIOS_ERR, "PI608GP: Drive level %d mV out of range 0 to 475 mV!",
		       level_mv);
		return PI608GP_ENCODE_ERR;
	}
	if (level_mv % 25 != 0) {
		printk(BIOS_ERR, "PI608GP: Drive level %d mV not a multiple of 25!\n",
		       level_mv);
		return PI608GP_ENCODE_ERR;
	}

	/* The encoded value is a 5-bit number representing 25 mV steps. */
	return (level_mv / 25) & 0x1f;
}

static uint8_t pi608gp_encode_deemph_lvl(struct deemph_lvl level_mv)
{
	/* Table of allowed fixed-point millivolt values, based on Table 6-8 from the
	   PI7C9X2G608GP datasheet. */
	struct deemph_lvl allowed[] = {
		{  0, 0}, {  6, 0}, { 12, 5}, { 19, 0}, { 25, 0}, { 31, 0}, { 37, 5}, { 44, 0},
		{ 50, 0}, { 56, 0}, { 62, 5}, { 69, 0}, { 75, 0}, { 81, 0}, { 87, 0}, { 94, 0},
		{100, 0}, {106, 0}, {112, 5}, {119, 0}, {125, 0}, {131, 0}, {137, 5}, {144, 0},
		{150, 0}, {156, 0}, {162, 5}, {169, 0}, {175, 0}, {181, 0}, {187, 5}, {194, 0},
	};

	for (int i = 0; i < ARRAY_SIZE(allowed); i++) {
		if (allowed[i].lvl == level_mv.lvl && allowed[i].lvl_10 == level_mv.lvl_10)
			/* When found, the encoded value is a 5-bit number that corresponds to
			   the index in the table of allowed values above. */
			return (uint8_t) (i & 0x1f);
	}

	printk(BIOS_ERR, "PI608GP: Requested unsupported de-emphasis level value: %d.%d mV!\n",
			level_mv.lvl, level_mv.lvl_10);
	return PI608GP_ENCODE_ERR;
}

static enum cb_err
pi608gp_reg_read(struct device *dev, uint8_t port, uint32_t reg_addr, uint32_t *val)
{
	int ret;

	/*
	 * Compose the SMBus message for register read init operation (from MSB to LSB):
	 * Byte 1: 7:3 = Rsvd., 2:0 = Command,
	 * Byte 2: 7:4 = Rsvd., 3:0 = Port Select[4:1],
	 * Byte 3: 7 = Port Select[0], 6 = Rsvd., 5:2 = Byte Enable, 1:0 = Reg. Addr. [11:10],
	 * Byte 4: 7:0 = Reg. Addr.[9:2] (Reg. Addr. [1:0] is fixed to 0).
	 */
	uint8_t buf[4] = {
		PI608GP_SUBCMD_RD,
		(port >> 1) & 0xf,
		((port & 0x1) << 7) | (PI608GP_EN_4B << 2) | ((reg_addr >> 10) & 0x3),
		(reg_addr >> 2) & 0xff,
	};

	/* Initialize register read operation */
	ret = smbus_block_write(dev, PI608GP_CMD_BLK_RD_INIT, sizeof(buf), buf);
	if (ret != sizeof(buf)) {
		printk(BIOS_ERR, "PI608GP: Unable to initiate register read!\n");
		return CB_ERR;
	}

	/* Perform the register read */
	ret = smbus_block_read(dev, PI608GP_CMD_BLK_RD, sizeof(buf), buf);
	if (ret != sizeof(buf)) {
		printk(BIOS_ERR, "PI608GP: Error reading register 0x%x (port %d)\n",
				reg_addr, port);
		return CB_ERR;
	}

	/* Retrieve back the value from the received SMBus packet in big endian order. */
	*val = read_be32((void *) buf);

	return CB_SUCCESS;
}

static enum cb_err
pi608gp_reg_write(struct device *dev, uint8_t port, uint32_t reg_addr, uint32_t val)
{
	int ret;

	/* Assemble register write command header, the same way as with read but add extra 4
	   bytes for the value. */
	uint8_t buf[8] = {
		PI608GP_SUBCMD_WR,
		(port >> 1) & 0xf,
		((port & 0x1) << 7) | (PI608GP_EN_4B << 2) | ((reg_addr >> 10) & 0x3),
		(reg_addr >> 2) & 0xff,
	};

	/* Insert register value to write in BE order after the header. */
	write_be32((void *) &buf[4], val);

	/* Perform the register write */
	ret = smbus_block_write(dev, PI608GP_CMD_BLK_WR, sizeof(buf), buf);
	if (ret != sizeof(buf)) {
		printk(BIOS_ERR, "PI608GP: Unable to write register 0x%x\n", reg_addr);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

static enum cb_err pi608gp_reg_update(struct device *dev, uint8_t port, uint32_t reg_addr,
		uint32_t and_mask, uint32_t or_mask)
{
	uint32_t val;

	if (pi608gp_reg_read(dev, port, reg_addr, &val))
		return CB_ERR;

	val &= and_mask;
	val |= or_mask;

	if (pi608gp_reg_write(dev, port, reg_addr, val))
		return CB_ERR;

	return CB_SUCCESS;
}

static void pi608gp_init(struct device *dev)
{
	const uint8_t port = 0; /* Only port 0 is being configured */
	struct drivers_i2c_pi608gp_config *config = dev->chip_info;
	uint8_t amp_lvl, deemph_lvl;

	/* The register values need to be encoded in a more complex way for the hardware. */
	amp_lvl = pi608gp_encode_amp_lvl(config->gen2_3p5_amp);
	deemph_lvl = pi608gp_encode_deemph_lvl(config->gen2_3p5_deemph);

	/* When the de-emphasis option isn't enabled or the values incorrectly encoded,
	   don't do anything. */
	if (!config->gen2_3p5_enable || amp_lvl == PI608GP_ENCODE_ERR ||
	    deemph_lvl == PI608GP_ENCODE_ERR)
		return;

	/* Enable -3,5 dB de-emphasis option (P35_GEN2_MODE). */
	if (pi608gp_reg_update(dev, port, PI608GP_REG_TL_CSR, ~0, 1 << 31))
		return;

	/* Set drive amplitude level for -3,5 dB de-emphasis (bits 20:16). */
	if (pi608gp_reg_update(dev, port, PI608GP_REG_SW_OPMODE, ~(0x1f << 16), amp_lvl << 16))
		return;

	/* Set drive de-emphasis for -3,5 dB on Gen 2 (bits 25:21). */
	if (pi608gp_reg_update(dev, port, PI608GP_REG_PHY_PAR1, ~(0x1f << 21), deemph_lvl << 21))
		return;
}

struct device_operations pi608gp_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.init			= pi608gp_init,
};

struct chip_operations drivers_i2c_pi608gp_ops = {
	CHIP_NAME("PI7C9X2G608GP")
};
