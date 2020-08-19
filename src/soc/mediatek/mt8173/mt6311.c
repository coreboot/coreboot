/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/i2c_simple.h>
#include <soc/mt6311.h>

enum {
	MT6311_SLAVE_ADDR = 0x6B,
};

static u32 get_mt6311_chip_id(uint8_t i2c_num)
{
	unsigned char id[2] = {0};

	i2c_read_field(i2c_num, MT6311_SLAVE_ADDR, MT6311_CID,
		       &id[0], 0xFF, 0);
	i2c_read_field(i2c_num, MT6311_SLAVE_ADDR, MT6311_SWCID,
		       &id[1], 0xFF, 0);

	return (u32)(id[0] << 8 | id[1]);
}

static void mt6311_hw_init(uint8_t i2c_num)
{
	int ret = 0;
	unsigned char var[3] = {0};

	/*
	 * Phase Shedding Trim Software Setting
	 * The phase 2 of MT6311 will enter PWM mode if the threshold is
	 * reached.
	 * The threshold is set according to EFUSE value.
	 */
	ret |= i2c_read_field(i2c_num, MT6311_SLAVE_ADDR,
			      MT6311_EFUSE_DOUT_56_63, &var[0],
			      0x3, 1);
	ret |= i2c_read_field(i2c_num, MT6311_SLAVE_ADDR,
			      MT6311_EFUSE_DOUT_56_63, &var[1],
			      0x1, 7);
	ret |= i2c_read_field(i2c_num, MT6311_SLAVE_ADDR,
			      MT6311_EFUSE_DOUT_64_71, &var[2],
			      0x1, 0);

	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR,
			       MT6311_VDVFS1_ANA_CON10,
			       var[0] | var[1] << 2 | var[2] << 3, 0xf, 0);

	/* I2C_CONFIG; pushpull setting, Opendrain is '0' */
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR, MT6311_TOP_INT_CON,
			       0x1, 0x1, 2);
	/* RG_WDTRSTB_EN; CC, initial WDRSTB setting. */
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR, MT6311_TOP_RST_CON,
			       0x1, 0x1, 5);
	/* initial INT function */
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR, MT6311_GPIO_MODE,
			       0x1, 0x7, 3);
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR, MT6311_STRUP_CON5,
			       0, 1 << 2 | 1 << 1 | 1 << 0, 0);

	/* Vo max is 1.15V */
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR,
			       MT6311_STRUP_ANA_CON1, 0x3, 0x3, 5);
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR,
			       MT6311_BUCK_ALL_CON23, 0x1, 0x1, 0);
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR,
			       MT6311_STRUP_ANA_CON2, 0x3, 0x3, 0);
	/* Suspend HW control from SPM */
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR,
			       MT6311_TOP_CON, 0x1, 0x1, 0);
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR,
			       MT6311_VDVFS11_CON7, 0x1, 0x1, 0);
	/* default VDVFS power on */
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR,
			       MT6311_VDVFS11_CON9, 0x1, 0x1, 0);
	/* for DVFS slew rate rising=0.67us */
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR,
			       MT6311_VDVFS11_CON10, 0x1, 0x7f, 0);
	/* for DVFS slew rate, falling 2.0us */
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR,
			       MT6311_VDVFS11_CON11, 0x5, 0x7f, 0);
	/* default VDVFS11_VOSEL 1.0V, SW control */
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR,
			       MT6311_VDVFS11_CON12, 0x40, 0x7f, 0);
	/* default VDVFS11_VOSEL_ON 1.0V, HW control */
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR,
			       MT6311_VDVFS11_CON13, 0x40, 0x7f, 0);
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR,
			       MT6311_VDVFS11_CON14, 0x40, 0x7f, 0);
	/* for DVFS sof change, falling 50us */
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR,
			       MT6311_VDVFS11_CON19, 0x3, 0x3, 0);
	/* for DVFS sof change, falling only */
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR,
			       MT6311_VDVFS11_CON19, 0x1, 0x3, 4);
	/* OFF LDO */
	ret |= i2c_write_field(i2c_num, MT6311_SLAVE_ADDR,
			       MT6311_LDO_CON3, 0, 0x1, 0);

	if (ret)
		printk(BIOS_ERR, "ERROR: %s failed\n", __func__);
}

void mt6311_probe(uint8_t i2c_num)
{
	u32 val = 0;

	/* Check device ID is MT6311 */
	val = get_mt6311_chip_id(i2c_num);
	printk(BIOS_INFO, "%s: device ID = %#x\n", __func__, val);

	if (val < MT6311_E1_CID_CODE) {
		printk(BIOS_ERR, "ERROR: unknown MT6311 device_id\n");
		return;
	}

	mt6311_hw_init(i2c_num);
}
