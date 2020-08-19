/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/i2c_simple.h>
#include <soc/da9212.h>

enum {
	DA9212_SLAVE_ADDR = 0x68,
};

static void da9212_hw_init(uint8_t i2c_num, unsigned char variant_id)
{
	int ret = 0;
	int buck_mode = DA9212_BUCK_MODE_AUTO;

	if (variant_id == DA9212_VARIANT_ID_AB)
		buck_mode = DA9212_BUCK_MODE_PWM;

	/* page select to 0 */
	ret |= i2c_write_field(i2c_num, DA9212_SLAVE_ADDR,
			       DA9212_REG_PAGE_CON, DA9212_REG_PAGE0,
			       DA9212_REG_PAGE_MASK, DA9212_REG_PAGE_SHIFT);

	ret |= i2c_write_field(i2c_num, DA9212_SLAVE_ADDR,
			       DA9212_REG_BUCKA_CONT, DA9212_BUCK_GPI_GPIO1,
			       DA9212_BUCK_GPI_MASK, DA9212_BUCK_GPI_SHIFT);

	ret |= i2c_write_field(i2c_num, DA9212_SLAVE_ADDR,
			       DA9212_REG_BUCKB_CONT, DA9212_BUCK_GPI_OFF,
			       DA9212_BUCK_GPI_OFF, DA9212_BUCK_GPI_SHIFT);

	ret |= i2c_write_field(i2c_num, DA9212_SLAVE_ADDR,
			       DA9212_REG_BUCKA_CONT, DA9212_VBUCK_SEL_A,
			       DA9212_VBUCK_SEL_MASK, DA9212_VBUCK_SEL_SHIFT);

	ret |= i2c_write_field(i2c_num, DA9212_SLAVE_ADDR,
			       DA9212_REG_BUCKB_CONT, DA9212_VBUCK_SEL_A,
			       DA9212_VBUCK_SEL_MASK, DA9212_VBUCK_SEL_SHIFT);

	ret |= i2c_write_field(i2c_num, DA9212_SLAVE_ADDR,
			       DA9212_REG_BUCKA_CONF, buck_mode,
			       DA9212_BUCK_MODE_MASK, DA9212_BUCK_MODE_SHIFT);

	ret |= i2c_write_field(i2c_num, DA9212_SLAVE_ADDR,
			       DA9212_REG_BUCKB_CONF, buck_mode,
			       DA9212_BUCK_MODE_MASK, DA9212_BUCK_MODE_SHIFT);

	if (ret)
		printk(BIOS_ERR, "ERROR: %s failed\n", __func__);

}

void da9212_probe(uint8_t i2c_num)
{
	int ret = 0;
	unsigned char device_id = 0;
	unsigned char variant_id = 0;

	/* select to page 4, clear REVERT at first time */
	ret |= i2c_write_field(i2c_num, DA9212_SLAVE_ADDR,
			       DA9212_REG_PAGE_CON, DA9212_REG_PAGE4,
			       0xF, DA9212_REG_PAGE_SHIFT);

	ret |= i2c_read_field(i2c_num, DA9212_SLAVE_ADDR,
			      DA9212_REG_DEVICE_ID, &device_id,
			      0xFF, 0);

	ret |= i2c_read_field(i2c_num, DA9212_SLAVE_ADDR,
			      DA9212_REG_VARIANT_ID, &variant_id,
			      0xFF, 0);

	printk(BIOS_INFO, "%s: device ID = %#x, variant ID = %#x\n", __func__,
	       device_id, variant_id);

	/* Check device ID is DA9212 */
	if (device_id != DA9212_ID || ret) {
		printk(BIOS_ERR, "ERROR: unknown DA9212 device_id\n");
		return;
	}

	da9212_hw_init(i2c_num, variant_id);
}
