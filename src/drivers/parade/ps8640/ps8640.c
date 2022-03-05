/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <device/i2c_simple.h>
#include <edid.h>
#include <console/console.h>
#include <timer.h>
#include <dp_aux.h>
#include "ps8640.h"

int ps8640_get_edid(uint8_t bus, uint8_t chip, struct edid *out)
{
	int ret;
	u8 edid[EDID_LENGTH * 2];
	int edid_size = EDID_LENGTH;

	i2c_writeb(bus, chip + 2, PAGE2_I2C_BYPASS,
		   EDID_I2C_ADDR | I2C_BYPASS_EN);
	ret = i2c_read_bytes(bus, EDID_I2C_ADDR, 0, edid, EDID_LENGTH);

	if (ret != 0) {
		printk(BIOS_INFO, "Failed to read EDID.\n");
		return -1;
	}

	/* check if edid have extension flag, and read additional EDID data */
	if (edid[EDID_EXTENSION_FLAG]) {
		edid_size += EDID_LENGTH;
		ret = i2c_read_bytes(bus, EDID_I2C_ADDR, EDID_LENGTH,
				     &edid[EDID_LENGTH], EDID_LENGTH);
		if (ret != 0) {
			printk(BIOS_INFO, "Failed to read EDID ext block.\n");
			return -1;
		}
	}

	if (decode_edid(edid, edid_size, out) != EDID_CONFORMANT) {
		printk(BIOS_INFO, "Failed to decode EDID.\n");
		return -1;
	}

	return 0;
}

int ps8640_init(uint8_t bus, uint8_t chip)
{
	u8 set_vdo_done;
	struct stopwatch sw;

	mdelay(200);
	stopwatch_init_msecs_expire(&sw, 200);

	while (true) {
		i2c_readb(bus, chip + 2, PAGE2_GPIO_H, &set_vdo_done);
		if ((set_vdo_done & PS_GPIO9) == PS_GPIO9)
			break;
		if (stopwatch_expired(&sw)) {
			printk(BIOS_INFO, "Failed to init ps8640.\n");
			return -1;
		}

		mdelay(20);
	}

	mdelay(50);

	/*
	 * The Manufacturer Command Set (MCS) is a device dependent interface
	 * intended for factory programming of the display module default
	 * parameters. Once the display module is configured, the MCS shall be
	 * disabled by the manufacturer. Once disabled, all MCS commands are
	 * ignored by the display interface.
	 */
	i2c_write_field(bus, chip + 2, PAGE2_MCS_EN, 0x0, MCS_EN_MASK,
			MCS_EN_SHIFT);
	i2c_writeb(bus, chip + 3, PAGE3_SET_ADD, VDO_CTL_ADD);
	i2c_writeb(bus, chip + 3, PAGE3_SET_VAL, VDO_DIS);
	i2c_writeb(bus, chip + 3, PAGE3_SET_ADD, VDO_CTL_ADD);
	i2c_writeb(bus, chip + 3, PAGE3_SET_VAL, VDO_EN);

	return 0;
}

static enum cb_err ps8640_bridge_aux_request(uint8_t bus,
					     uint8_t chip,
					     unsigned int target_reg,
					     unsigned int total_size,
					     enum aux_request request,
					     uint8_t *data)
{
	int i;
	uint32_t length;
	uint8_t buf;
	uint8_t reg;
	int ret;

	if (target_reg & ~SWAUX_ADDR_MASK)
		return CB_ERR;

	while (total_size) {
		length = MIN(total_size, DP_AUX_MAX_PAYLOAD_BYTES);
		total_size -= length;

		ret = i2c_writeb(bus, chip, PAGE0_AUXCH_CFG3, AUXCH_CFG3_RESET);
		if (ret)
			return CB_ERR;

		enum i2c_over_aux cmd = dp_get_aux_cmd(request, total_size);
		if (i2c_writeb(bus, chip, PAGE0_SWAUX_ADDR_23_16,
			(target_reg >> 16) | (cmd << 4)) ||
		    i2c_writeb(bus, chip, PAGE0_SWAUX_ADDR_15_8, target_reg >> 8) ||
		    i2c_writeb(bus, chip, PAGE0_SWAUX_ADDR_7_0, target_reg)) {
			return CB_ERR;
		}

		if (dp_aux_request_is_write(request)) {
			reg = PAGE0_SWAUX_WDATA;
			for (i = 0; i < length; i++) {
				ret = i2c_writeb(bus, chip, reg++, *data++);
				if (ret)
					return CB_ERR;
			}
		} else {
			if (length == 0)
				i2c_writeb(bus, chip, PAGE0_SWAUX_LENGTH, SWAUX_NO_PAYLOAD);
			else
				i2c_writeb(bus, chip, PAGE0_SWAUX_LENGTH, length - 1);
		}

		ret = i2c_writeb(bus, chip, PAGE0_SWAUX_CTRL, SWAUX_SEND);
		if (ret)
			return CB_ERR;

		if (!wait_ms(100, !i2c_readb(bus, chip, PAGE0_SWAUX_CTRL, &buf) &&
				  !(buf & SWAUX_SEND)))
			return CB_ERR;

		if (i2c_readb(bus, chip, PAGE0_SWAUX_STATUS, &buf))
			return CB_ERR;

		switch (buf & SWAUX_STATUS_MASK) {
		case SWAUX_STATUS_NACK:
		case SWAUX_STATUS_I2C_NACK:
		case SWAUX_STATUS_INVALID:
		case SWAUX_STATUS_TIMEOUT:
			return CB_ERR;
		case SWAUX_STATUS_ACKM:
			length = buf & SWAUX_M_MASK;
			break;
		}

		if (length && !dp_aux_request_is_write(request)) {
			reg = PAGE0_SWAUX_RDATA;
			for (i = 0; i < length; i++) {
				if (i2c_readb(bus, chip, reg++, &buf))
					return CB_ERR;
				*data++ = buf;
			}
		}
	}

	return CB_SUCCESS;
}

void ps8640_backlight_enable(uint8_t bus, uint8_t chip)
{
	uint8_t val;

	val = DP_BACKLIGHT_CONTROL_MODE_DPCD;
	ps8640_bridge_aux_request(bus, chip, DP_BACKLIGHT_MODE_SET, 1,
				  DPCD_WRITE, &val);

	val = 0xff;
	ps8640_bridge_aux_request(bus, chip, DP_BACKLIGHT_BRIGHTNESS_MSB, 1,
				  DPCD_WRITE, &val);

	val = DP_BACKLIGHT_ENABLE;
	ps8640_bridge_aux_request(bus, chip, DP_DISPLAY_CONTROL_REGISTER, 1,
				  DPCD_WRITE, &val);
}
