/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This is a driver for the Whirlwind LED ring, which is equipped with two LED
 * microcontrollers TI LP55231 (http://www.ti.com/product/lp55231), each of
 * them driving three multicolor LEDs.
 *
 * The only connection between the ring and the main board is an i2c bus.
 *
 * This driver imitates a depthcharge display device. On initialization the
 * driver sets up the controllers to prepare them to accept programs to run.
 *
 * When a certain vboot state needs to be indicated, the program for that
 * state is loaded into the controllers, resulting in the state appropriate
 * LED behavior.
 */

#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <string.h>

#include "drivers/i2c/ww_ring/ww_ring_programs.h"

/* I2c address of the first of the controllers, the rest are contiguous. */
#define WW_RING_BASE_ADDR	0x32

/* Key lp55231 registers. */
#define LP55231_ENGCTRL1_REG	0x00
#define LP55231_ENGCTRL2_REG	0x01
#define LP55231_D1_CRT_CTRL_REG	0x26
#define LP55231_MISC_REG	0x36
#define LP55231_VARIABLE_REG	0x3c
#define LP55231_RESET_REG	0x3d
#define LP55231_ENG1_PROG_START	0x4c
#define LP55231_PROG_PAGE_REG	0x4f
#define LP55231_PROG_BASE_REG	0x50

/* LP55231_D1_CRT_CTRL_REG, default value, applies to all nine of them */
#define LP55231_CRT_CTRL_DEFAULT 0xaf

/* LP55231_ENGCTRL1_REG	fields */
#define LP55231_ENGCTRL1_CHIP_EN     0x40
#define LP55231_ENGCTRL1_ALL_ENG_GO  0x2a

/* LP55231_ENGCTRL2_REG	fields. */
#define LP55231_ENGCTRL2_ALL_DISABLE 0
#define LP55231_ENGCTRL2_ALL_LOAD    0x15
#define LP55231_ENGCTRL2_ALL_RUN     0x2a

/* LP55231_MISC_REG fields. */
#define LP55231_MISC_AUTOINCR  (1 << 6)
#define LP55231_MISC_PUMP_1X   (1 << 3)
#define LP55231_MISC_INT_CLK   (3 << 0)

/*
 * LP55231_VARIABLE_REG cookie value. It indicates to depthcharge that the
 * ring has been initialized by coreboot.
 */
#define LP55231_VARIABLE_COOKIE	0xb4

/* Goes into LP55231_RESET_REG to reset the chip. */
#define LP55231_RESET_VALUE	0xff

/*
 * The controller has 192 bytes of SRAM for code/data, available as six 32 byte
 * pages.
 */
#define LP55231_PROG_PAGE_SIZE  32
#define LP55231_PROG_PAGES      6
#define LP55231_MAX_PROG_SIZE  (LP55231_PROG_PAGE_SIZE * LP55231_PROG_PAGES)

/*
 * Structure to cache data relevant to accessing one controller. I2c interface
 * to use, device address on the i2c bus and a data buffer for write
 * transactions. The most bytes sent at a time is the register address plus
 * the program page size.
 */
typedef struct {
	unsigned int i2c_bus;
	uint8_t  dev_addr;
	uint8_t  data_buffer[LP55231_PROG_PAGE_SIZE + 1];
} TiLp55231;

static void ww_ring_init(unsigned int i2c_bus);

/* Controller descriptors. */
static TiLp55231 lp55231s[WW_RING_NUM_LED_CONTROLLERS];

/*
 * i2c transfer function for the driver. To keep things simple, the function
 * repeats the transfer, if the first attempt fails. This is OK with the
 * controller and makes it easier to handle errors.
 *
 * Note that the reset register accesses are expected to fail on writes, but
 * due to a bug in the ipq806x i2c controller, the error is reported on the
 * following read attempt.
 *
 * To work around this the driver writes and then reads the reset register,
 * the transfer function ignores errors when accessing the reset register.
 */

static int ledc_transfer(TiLp55231 *ledc, struct i2c_msg *segs,
			 int seg_count, int reset)
{
	int rv, max_attempts = 2;

	while (max_attempts--) {
		rv = i2c_transfer(ledc->i2c_bus, segs, seg_count);

		/* Accessing reset register is expected to fail. */
		if (!rv || reset)
			break;
	}

	if (rv) {
		if (!reset)
			printk(BIOS_WARNING,
			       "%s: dev %#x, reg %#x, %s transaction error.\n",
			       __func__, segs->slave, segs->buf[0],
			       seg_count == 1 ? "write" : "read");
		else
			rv = 0;
	}

	return rv;
}

/*
 * The controller is programmed to autoincrement on writes, so up to page size
 * bytes can be transmitted in one write transaction.
 */
static int ledc_write(TiLp55231 *ledc, uint8_t start_addr,
		      const uint8_t *data, unsigned int count)
{
	struct i2c_msg seg;

	if (count > (sizeof(ledc->data_buffer) - 1)) {
		printk(BIOS_WARNING, "%s: transfer size too large (%d bytes)\n",
		       __func__, count);
		return -1;
	}

	memcpy(ledc->data_buffer + 1, data, count);
	ledc->data_buffer[0] = start_addr;

	seg.flags = 0;
	seg.slave = ledc->dev_addr;
	seg.buf = ledc->data_buffer;
	seg.len = count + 1;

	return ledc_transfer(ledc, &seg, 1, start_addr == LP55231_RESET_REG);
}

/* To keep things simple, read is limited to one byte at a time. */
static int ledc_read(TiLp55231 *ledc, uint8_t addr, uint8_t *data)
{
	struct i2c_msg seg[2];

	seg[0].flags = 0;
	seg[0].slave = ledc->dev_addr;
	seg[0].buf = &addr;
	seg[0].len = 1;

	seg[1].flags = I2C_M_RD;
	seg[1].slave = ledc->dev_addr;
	seg[1].buf = data;
	seg[1].len = 1;

	return ledc_transfer(ledc, seg, ARRAY_SIZE(seg),
			     addr == LP55231_RESET_REG);
}

/*
 * Reset transaction is expected to result in a failing i2c command. But even
 * before trying it, read the reset register, which is supposed to always
 * return 0. If this fails - there is no lp55231 at this address.
 *
 * Return 0 on success, -1 on failure to detect controller.
 */
static int ledc_reset(TiLp55231 *ledc)
{
	uint8_t data;

	data = ~0;
	ledc_read(ledc, LP55231_RESET_REG, &data);
	if (data) {
		printk(BIOS_WARNING,
		       "WW_RING: no controller found at address %#2.2x\n",
		       ledc->dev_addr);
		return -1;
	}

	data = LP55231_RESET_VALUE;
	ledc_write(ledc, LP55231_RESET_REG, &data, 1);

	/*
	 * This read is not necessary for the chip reset, but is required to
	 * work around the i2c driver bug where the missing ACK on the last
	 * byte of the write transaction is ignored, but the next transaction
	 * fails.
	 */
	ledc_read(ledc, LP55231_RESET_REG, &data);
	return 0;
}

/*
 * Write a program into the internal lp55231 memory. Split write transactions
 * into sections fitting into memory pages.
 */
static void ledc_write_program(TiLp55231 *ledc, uint8_t load_addr,
			       const uint8_t *program, unsigned int count)
{
	uint8_t page_num = load_addr / LP55231_PROG_PAGE_SIZE;
	unsigned int page_offs = load_addr % LP55231_PROG_PAGE_SIZE;

	if ((load_addr + count) > LP55231_MAX_PROG_SIZE) {
		printk(BIOS_WARNING,
		       "%s: program of size %#x does not fit at addr %#x\n",
		       __func__, count, load_addr);
		return;
	}

	while (count) {
		unsigned int segment_size = LP55231_PROG_PAGE_SIZE - page_offs;

		if (segment_size > count)
			segment_size = count;

		ledc_write(ledc, LP55231_PROG_PAGE_REG, &page_num, 1);
		ledc_write(ledc, LP55231_PROG_BASE_REG + page_offs,
			   program, segment_size);

		count -= segment_size;
		program += segment_size;
		page_offs = 0;
		page_num++;
	}
}

static void ledc_write_engctrl2(TiLp55231 *ledc, uint8_t value)
{
	ledc_write(ledc, LP55231_ENGCTRL2_REG, &value, 1);
	udelay(1500);
}

/* Run an lp55231 program on a controller. */
static void ledc_run_program(TiLp55231 *ledc,
			     const TiLp55231Program *program_desc)
{
	int i;
	uint8_t data;

	/* All engines on hold. */
	data = LP55231_ENGCTRL1_CHIP_EN;
	ledc_write(ledc, LP55231_ENGCTRL1_REG, &data, 1);

	ledc_write_engctrl2(ledc, LP55231_ENGCTRL2_ALL_DISABLE);
	ledc_write_engctrl2(ledc, LP55231_ENGCTRL2_ALL_LOAD);

	ledc_write_program(ledc, program_desc->load_addr,
			   program_desc->program_text,
			   program_desc->program_size);

	for (i = 0; i < sizeof(program_desc->engine_start_addr); i++)
		ledc_write(ledc, LP55231_ENG1_PROG_START + i,
			   program_desc->engine_start_addr + i, 1);

	data = LP55231_ENGCTRL1_CHIP_EN | LP55231_ENGCTRL1_ALL_ENG_GO;
	ledc_write(ledc, LP55231_ENGCTRL1_REG, &data, 1);
	ledc_write_engctrl2(ledc, LP55231_ENGCTRL2_ALL_RUN);
}

/*
 * Initialize a controller to a state were it is ready to accept programs, and
 * try to confirm that we are in fact talking to a lp55231
 */
static int ledc_init_validate(TiLp55231 *ledc)
{
	uint8_t data;
	int i;

	if (ledc_reset(ledc))
		return -1;

	/* Enable the chip, keep engines in hold state. */
	data = LP55231_ENGCTRL1_CHIP_EN;
	ledc_write(ledc, LP55231_ENGCTRL1_REG, &data, 1);

	/*
	 * Internal clock, 3.3V output (pump 1x), autoincrement on multibyte
	 * writes.
	 */
	data = LP55231_MISC_AUTOINCR |
		LP55231_MISC_PUMP_1X | LP55231_MISC_INT_CLK;
	ledc_write(ledc, LP55231_MISC_REG, &data, 1);

	/*
	 * All nine current control registers are supposed to return the same
	 * value at reset.
	 */
	for (i = 0; i < 9; i++) {
		ledc_read(ledc, LP55231_D1_CRT_CTRL_REG + i, &data);
		if (data != LP55231_CRT_CTRL_DEFAULT) {
			printk(BIOS_WARNING,
			       "%s: read %#2.2x from register %#x\n", __func__,
			       data, LP55231_D1_CRT_CTRL_REG + i);
			return -1;
		}
	}

	/*
	 * Signal Depthcharge that the controller has been initiazed by
	 * coreboot.
	 */
	data = LP55231_VARIABLE_COOKIE;
	ledc_write(ledc, LP55231_VARIABLE_REG, &data, 1);

	return 0;
}

/*
 * Find a program matching screen type, and run it on both controllers, if
 * found.
 */
int ww_ring_display_pattern(unsigned int i2c_bus, enum display_pattern pattern)
{
	static int initted;
	const WwRingStateProg *wwr_prog;

	if (!initted) {
		ww_ring_init(i2c_bus);
		initted = 1;
	}

	/* Last entry does not have any actual programs defined. */
	for (wwr_prog = wwr_state_programs; wwr_prog->programs[0]; wwr_prog++)
		if (wwr_prog->led_pattern == pattern) {
			int j;

			/*
			 * First stop all running programs to avoid
			 * inerference between the controllers.
			 */
			for (j = 0; j < WW_RING_NUM_LED_CONTROLLERS; j++) {
				if (!lp55231s[j].dev_addr)
					continue;
				ledc_write_engctrl2
					(lp55231s + j,
					 LP55231_ENGCTRL2_ALL_DISABLE);
			}

			for (j = 0; j < WW_RING_NUM_LED_CONTROLLERS; j++) {
				if (!lp55231s[j].dev_addr)
					continue;
				ledc_run_program(lp55231s + j,
						 wwr_prog->programs[j]);
			}
			return 0;
		}

	printk(BIOS_WARNING, "%s: did not find program for pattern %d\n",
	       __func__, pattern);

	return -1;
}

#define LP55231_I2C_BASE_ADDR 0x32

static void ww_ring_init(unsigned int i2c_bus)
{
	TiLp55231 *ledc;
	int i, count = 0;

	for (i = 0, ledc = lp55231s;
	     i < WW_RING_NUM_LED_CONTROLLERS;
	     i++, ledc++) {
		ledc->i2c_bus = i2c_bus;
		ledc->dev_addr = LP55231_I2C_BASE_ADDR + i;

		if (!ledc_init_validate(ledc))
			count++;
		else
			ledc->dev_addr = 0; /* Mark disabled. */
	}

	printk(BIOS_INFO, "WW_RING: initialized %d out of %d\n", count, i);
	if (count != i) {
		if (count)
			printk(BIOS_WARNING,
			       "WW_RING: will keep going anyway\n");
		else
			printk(BIOS_WARNING,
			       "WW_RING: LED ring not present\n");
	}
}
