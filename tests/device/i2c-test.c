/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/i2c_simple.h>
#include <limits.h>
#include <tests/test.h>

/* Simulate two i2c devices, both on bus 0, each with three uint8_t regs
   implemented. */
typedef struct {
	uint8_t reg;
	uint8_t data;
} i2c_ex_regs_t;

typedef struct {
	unsigned int bus;
	uint8_t slave;
	i2c_ex_regs_t regs[3];
} i2c_ex_devs_t;

i2c_ex_devs_t i2c_ex_devs[] = {
	{
		.bus = 0,
		.slave = 0xA,
		.regs = {
			{.reg = 0x0, .data = 0xB},
			{.reg = 0x1, .data = 0x6},
			{.reg = 0x2, .data = 0xF},
		}
	},
	{
		.bus = 0,
		.slave = 0x3,
		.regs = {
			{.reg = 0x0, .data = 0xDE},
			{.reg = 0x1, .data = 0xAD},
			{.reg = 0x2, .data = 0xBE},
		}
	},
};

int platform_i2c_transfer(unsigned int bus, struct i2c_msg *segments, int count)
{
	int i;
	int reg;
	struct i2c_msg *tmp = segments;
	i2c_ex_devs_t *i2c_dev = NULL;

	check_expected(count);

	for (i = 0; i < count; i++, segments++) {
		check_expected_ptr(segments->buf);
		check_expected(segments->flags);
	}

	reg = tmp->buf[0];

	/* Find object for requested device */
	for (i = 0; i < ARRAY_SIZE(i2c_ex_devs); i++)
		if (i2c_ex_devs[i].slave == tmp->slave) {
			i2c_dev = &i2c_ex_devs[i];
			break;
		}

	if (i2c_dev == NULL)
		return -1;

	/* Write commands */
	if (tmp->len > 1) {
		i2c_dev->regs[reg].data = tmp->buf[1];
	};

	/* Read commands */
	for (i = 0; i < count; i++, tmp++)
		if (tmp->flags & I2C_M_RD) {
			*(tmp->buf) = i2c_dev->regs[reg].data;
		};

	return 0;
}

static void mock_expect_params_platform_i2c_transfer(void)
{
	unsigned long expected_flags[] = {0, I2C_M_RD, I2C_M_TEN, I2C_M_RECV_LEN,
					      I2C_M_NOSTART};

	/* Flags should always be only within supported range */
	expect_in_set_count(platform_i2c_transfer, segments->flags, expected_flags, -1);

	expect_not_value_count(platform_i2c_transfer, segments->buf, NULL, -1);

	expect_in_range_count(platform_i2c_transfer, count, 1, INT_MAX, -1);
}

#define MASK 0x3
#define SHIFT 0x1

static void i2c_read_field_test(void **state)
{
	int i, j;
	uint8_t buf;

	mock_expect_params_platform_i2c_transfer();

	/* Read particular bits in all registers in all devices, then compare
	   with expected value. */
	for (i = 0; i < ARRAY_SIZE(i2c_ex_devs); i++)
		for (j = 0; j < ARRAY_SIZE(i2c_ex_devs[0].regs); j++) {
			i2c_read_field(i2c_ex_devs[i].bus, i2c_ex_devs[i].slave,
				       i2c_ex_devs[i].regs[j].reg, &buf, MASK, SHIFT);
			assert_int_equal(
				(i2c_ex_devs[i].regs[j].data & (MASK << SHIFT)) >> SHIFT, buf);
		};

	/* Read whole registers */
	for (i = 0; i < ARRAY_SIZE(i2c_ex_devs); i++)
		for (j = 0; j < ARRAY_SIZE(i2c_ex_devs[0].regs); j++) {
			i2c_read_field(i2c_ex_devs[i].bus, i2c_ex_devs[i].slave,
				       i2c_ex_devs[i].regs[j].reg, &buf, 0xFF, 0);
			assert_int_equal(i2c_ex_devs[i].regs[j].data, buf);
		};
}

static void i2c_write_field_test(void **state)
{
	int i, j;
	uint8_t buf, tmp;

	mock_expect_params_platform_i2c_transfer();

	/* Clear particular bits in all registers in all devices, then compare
	   with expected value. */
	for (i = 0; i < ARRAY_SIZE(i2c_ex_devs); i++)
		for (j = 0; j < ARRAY_SIZE(i2c_ex_devs[0].regs); j++) {
			buf = 0x0;
			tmp = i2c_ex_devs[i].regs[j].data;
			i2c_write_field(i2c_ex_devs[i].bus, i2c_ex_devs[i].slave,
					i2c_ex_devs[i].regs[j].reg, buf, MASK, SHIFT);
			assert_int_equal(i2c_ex_devs[i].regs[j].data,
					 (tmp & ~(MASK << SHIFT)) | (buf << SHIFT));
		};

	/* Set all bits in all registers, this time verify using
	   i2c_read_field() accessor. */
	for (i = 0; i < ARRAY_SIZE(i2c_ex_devs); i++)
		for (j = 0; j < ARRAY_SIZE(i2c_ex_devs[0].regs); j++) {
			i2c_write_field(i2c_ex_devs[i].bus, i2c_ex_devs[i].slave,
					i2c_ex_devs[i].regs[j].reg, 0xFF, 0xFF, 0);
			i2c_read_field(i2c_ex_devs[i].bus, i2c_ex_devs[i].slave,
				       i2c_ex_devs[i].regs[j].reg, &buf, 0xFF, 0);
			assert_int_equal(buf, 0xFF);
		};
}

int main(void)
{
	const struct CMUnitTest tests[] = {cmocka_unit_test(i2c_read_field_test),
					   cmocka_unit_test(i2c_write_field_test)};

	return cb_run_group_tests(tests, NULL, NULL);
}
