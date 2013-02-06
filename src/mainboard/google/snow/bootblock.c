/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <types.h>
#include <arch/io.h>
#include <cbfs.h>
#include <uart.h>
#include <console/console.h>
#include <device/i2c.h>
#include <cpu/samsung/exynos5250/clk.h>
#include <cpu/samsung/exynos5250/dmc.h>
#include <cpu/samsung/exynos5250/periph.h>
#include <cpu/samsung/exynos5250/clock_init.h>
#include <drivers/maxim/max77686/max77686.h>

#define I2C0_BASE	0x12c60000

/*
 * Max77686 parameters values
 * see max77686.h for parameters details
 */
struct max77686_para max77686_param[] = {/*{vol_addr, vol_bitpos,
	vol_bitmask, reg_enaddr, reg_enbitpos, reg_enbitmask, reg_enbiton,
	reg_enbitoff, vol_min, vol_div}*/
	{/* PMIC_BUCK1 */ 0x11, 0x0, 0x3F, 0x10, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_BUCK2 */ 0x14, 0x0, 0xFF, 0x12, 0x4, 0x3, 0x1, 0x0, 600, 12500},
	{/* PMIC_BUCK3 */ 0x1E, 0x0, 0xFF, 0x1C, 0x4, 0x3, 0x1, 0x0, 600, 12500},
	{/* PMIC_BUCK4 */ 0x28, 0x0, 0xFF, 0x26, 0x4, 0x3, 0x1, 0x0, 600, 12500},
	{/* PMIC_BUCK5 */ 0x31, 0x0, 0x3F, 0x30, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_BUCK6 */ 0x33, 0x0, 0x3F, 0x32, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_BUCK7 */ 0x35, 0x0, 0x3F, 0x34, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_BUCK8 */ 0x37, 0x0, 0x3F, 0x36, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_BUCK9 */ 0x39, 0x0, 0x3F, 0x38, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_LDO1 */  0x40, 0x0, 0x3F, 0x40, 0x6, 0x3, 0x3, 0x0, 800, 25000},
	{/* PMIC_LDO2 */  0x41, 0x0, 0x3F, 0x41, 0x6, 0x3, 0x1, 0x0, 800, 25000},
	{/* PMIC_LDO3 */  0x42, 0x0, 0x3F, 0x42, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO4 */  0x43, 0x0, 0x3F, 0x43, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO5 */  0x44, 0x0, 0x3F, 0x44, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO6 */  0x45, 0x0, 0x3F, 0x45, 0x6, 0x3, 0x1, 0x0, 800, 25000},
	{/* PMIC_LDO7 */  0x46, 0x0, 0x3F, 0x46, 0x6, 0x3, 0x1, 0x0, 800, 25000},
	{/* PMIC_LDO8 */  0x47, 0x0, 0x3F, 0x47, 0x6, 0x3, 0x1, 0x0, 800, 25000},
	{/* PMIC_LDO9 */  0x48, 0x0, 0x3F, 0x48, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO10 */ 0x49, 0x0, 0x3F, 0x49, 0x6, 0x3, 0x1, 0x0, 800, 50000},
	{/* PMIC_LDO11 */ 0x4A, 0x0, 0x3F, 0x4A, 0x6, 0x3, 0x1, 0x0, 800, 50000},
	{/* PMIC_LDO12 */ 0x4B, 0x0, 0x3F, 0x4B, 0x6, 0x3, 0x1, 0x0, 800, 50000},
	{/* PMIC_LDO13 */ 0x4C, 0x0, 0x3F, 0x4C, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO14 */ 0x4D, 0x0, 0x3F, 0x4D, 0x6, 0x3, 0x1, 0x0, 800, 50000},
	{/* PMIC_LDO15 */ 0x4E, 0x0, 0x3F, 0x4E, 0x6, 0x3, 0x1, 0x0, 800, 25000},
	{/* PMIC_LDO16 */ 0x4F, 0x0, 0x3F, 0x4F, 0x6, 0x3, 0x1, 0x0, 800, 50000},
	{/* PMIC_LDO17 */ 0x50, 0x0, 0x3F, 0x50, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO18 */ 0x51, 0x0, 0x3F, 0x51, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO19 */ 0x52, 0x0, 0x3F, 0x52, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO20 */ 0x53, 0x0, 0x3F, 0x53, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO21 */ 0x54, 0x0, 0x3F, 0x54, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO22 */ 0x55, 0x0, 0x3F, 0x55, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO23 */ 0x56, 0x0, 0x3F, 0x56, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO24 */ 0x57, 0x0, 0x3F, 0x57, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO25 */ 0x58, 0x0, 0x3F, 0x58, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO26 */ 0x59, 0x0, 0x3F, 0x59, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_EN32KHZ_CP */ 0x0, 0x0, 0x0, 0x7F, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0},
};

/*
 * Write a value to a register
 *
 * @param chip_addr	i2c addr for max77686
 * @param reg		reg number to write
 * @param val		value to be written
 *
 */
static inline int max77686_i2c_write(unsigned char chip_addr,
					unsigned int reg, unsigned char val)
{
	return i2c_write(chip_addr, reg, 1, &val, 1);
}

/*
 * Read a value from a register
 *
 * @param chip_addr	i2c addr for max77686
 * @param reg		reg number to write
 * @param val		value to be written
 *
 */
static inline int max77686_i2c_read(unsigned char chip_addr,
					unsigned int reg, unsigned char *val)
{
	return i2c_read(chip_addr, reg, 1, val, 1);
}

/* Chip register numbers (not exported from this module) */
enum {
	REG_BBAT		= 0x7e,

	/* Bits for BBAT */
	BBAT_BBCHOSTEN_MASK	= 1 << 0,
	BBAT_BBCVS_SHIFT	= 3,
	BBAT_BBCVS_MASK		= 3 << BBAT_BBCVS_SHIFT,
};

int max77686_disable_backup_batt(void)
{
	unsigned char val;
	int ret;

	//i2c_set_bus_num(0);
	ret = max77686_i2c_read(MAX77686_I2C_ADDR, REG_BBAT, &val);
	if (ret) {
//		debug("max77686 i2c read failed\n");
		return ret;
	}

	/* If we already have the correct values, exit */
	if ((val & (BBAT_BBCVS_MASK | BBAT_BBCHOSTEN_MASK)) ==
			BBAT_BBCVS_MASK)
		return 0;

	/* First disable charging */
	val &= ~BBAT_BBCHOSTEN_MASK;
	ret = max77686_i2c_write(MAX77686_I2C_ADDR, REG_BBAT, val);
	if (ret) {
		//debug("max77686 i2c write failed\n");
		return -1;
	}

	/* Finally select 3.5V to minimize power consumption */
	val |= BBAT_BBCVS_MASK;
	ret = max77686_i2c_write(MAX77686_I2C_ADDR, REG_BBAT, val);
	if (ret) {
		//debug("max77686 i2c write failed\n");
		return -1;
	}

	return 0;
}

static int max77686_enablereg(enum max77686_regnum reg, int enable)
{
	struct max77686_para *pmic;
	unsigned char read_data;
	int ret;

	pmic = &max77686_param[reg];

	ret = max77686_i2c_read(MAX77686_I2C_ADDR, pmic->reg_enaddr,
				&read_data);
	if (ret != 0) {
		//debug("max77686 i2c read failed.\n");
		return -1;
	}

	if (enable == REG_DISABLE) {
		clrbits_8(&read_data,
				pmic->reg_enbitmask << pmic->reg_enbitpos);
	} else {
		clrsetbits_8(&read_data,
				pmic->reg_enbitmask << pmic->reg_enbitpos,
				pmic->reg_enbiton << pmic->reg_enbitpos);
	}

	ret = max77686_i2c_write(MAX77686_I2C_ADDR,
				 pmic->reg_enaddr, read_data);
	if (ret != 0) {
		//debug("max77686 i2c write failed.\n");
		return -1;
	}

	return 0;
}

static int max77686_do_volsetting(enum max77686_regnum reg, unsigned int volt,
				  int enable, int volt_units)
{
	struct max77686_para *pmic;
	unsigned char read_data;
	int vol_level = 0;
	int ret;

	pmic = &max77686_param[reg];

	if (pmic->vol_addr == 0) {
		//debug("not a voltage register.\n");
		return -1;
	}

	ret = max77686_i2c_read(MAX77686_I2C_ADDR, pmic->vol_addr, &read_data);
	if (ret != 0) {
		//debug("max77686 i2c read failed.\n");
		return -1;
	}

	if (volt_units == MAX77686_UV)
		vol_level = volt - (u32)pmic->vol_min * 1000;
	else
		vol_level = (volt - (u32)pmic->vol_min) * 1000;

	if (vol_level < 0) {
		//debug("Not a valid voltage level to set\n");
		return -1;
	}
	vol_level /= (u32)pmic->vol_div;

	clrsetbits_8(&read_data, pmic->vol_bitmask << pmic->vol_bitpos,
			vol_level << pmic->vol_bitpos);

	ret = max77686_i2c_write(MAX77686_I2C_ADDR, pmic->vol_addr, read_data);
	if (ret != 0) {
		//debug("max77686 i2c write failed.\n");
		return -1;
	}

	ret = max77686_enablereg(reg, enable);
	if (ret != 0) {
		//debug("Failed to enable buck/ldo.\n");
		return -1;
	}
	return 0;
}

int max77686_volsetting(enum max77686_regnum reg, unsigned int volt,
			int enable, int volt_units)
{
//	int old_bus = i2c_get_bus_num();
	int ret;

//	i2c_set_bus_num(0);
	ret = max77686_do_volsetting(reg, volt, enable, volt_units);
//	i2c_set_bus_num(old_bus);
	return ret;
}

/* power_init() stuff */
static void power_init(void)
{
	max77686_disable_backup_batt();

#if 0
	/* FIXME: for debugging... */
	volatile unsigned long *addr = (unsigned long *)0x1004330c;
	*addr |= 0x100;
	while (1);
#endif

	max77686_volsetting(PMIC_BUCK2, CONFIG_VDD_ARM_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_BUCK3, CONFIG_VDD_INT_UV,
						REG_ENABLE, MAX77686_UV);
	max77686_volsetting(PMIC_BUCK1, CONFIG_VDD_MIF_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_BUCK4, CONFIG_VDD_G3D_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_LDO2, CONFIG_VDD_LDO2_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_LDO3, CONFIG_VDD_LDO3_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_LDO5, CONFIG_VDD_LDO5_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_LDO10, CONFIG_VDD_LDO10_MV,
						REG_ENABLE, MAX77686_MV);
}

#include <console/vtxprintf.h>
#include <string.h>
#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */
/* haha, don't need ctype.c */
#define isdigit(c)	((c) >= '0' && (c) <= '9')
#define is_digit isdigit
#define isxdigit(c)	(((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))

static int skip_atoi(const char **s)
{
	int i=0;

	while (is_digit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

#include <div64.h>
static int number(void (*tx_byte)(unsigned char byte),
	unsigned long long num, int base, int size, int precision, int type)
{
	char c,sign,tmp[66];
	const char *digits="0123456789abcdefghijklmnopqrstuvwxyz";
	int i;
	int count = 0;

	if (type & LARGE)
		digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if ((signed long long)num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0) {
		tmp[i++]='0';
	} else while (num != 0) {
		/* FIXME: do_div was broken */
//		tmp[i++] = digits[do_div(num,base)];
		tmp[i++] = digits[num & 0xf];
		num = num >> 4;
	}
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0)
			tx_byte(' '), count++;
	if (sign)
		tx_byte(sign), count++;
	if (type & SPECIAL) {
		if (base==8)
			tx_byte('0'), count++;
		else if (base==16) {
			tx_byte('0'), count++;
			tx_byte(digits[33]), count++;
		}
	}
	if (!(type & LEFT))
		while (size-- > 0)
			tx_byte(c), count++;
	while (i < precision--)
		tx_byte('0'), count++;
	while (i-- > 0)
		tx_byte(tmp[i]), count++;
	while (size-- > 0)
		tx_byte(' '), count++;
	return count;
}

int vtxprintf(void (*tx_byte)(unsigned char byte), const char *fmt, va_list args)
{
	int len;
	unsigned long long num;
	int i, base;
	const char *s;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'L' for integer fields */

	int count;

	for (count=0; *fmt ; ++fmt) {
		if (*fmt != '%') {
			tx_byte(*fmt), count++;
			continue;
		}

		/* process flags */
		flags = 0;
repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt) {
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
				}

		/* get field width */
		field_width = -1;
		if (is_digit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (is_digit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'z') {
			qualifier = *fmt;
			++fmt;
			if (*fmt == 'l') {
				qualifier = 'L';
				++fmt;
			}
		}

		/* default base */
		base = 10;

		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					tx_byte(' '), count++;
			tx_byte((unsigned char) va_arg(args, int)), count++;
			while (--field_width > 0)
				tx_byte(' '), count++;
			continue;

		case 's':
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";

			len = strnlen(s, precision);

			if (!(flags & LEFT))
				while (len < field_width--)
					tx_byte(' '), count++;
			for (i = 0; i < len; ++i)
				tx_byte(*s++), count++;
			while (len < field_width--)
				tx_byte(' '), count++;
			continue;

		case 'p':
			if (field_width == -1) {
				field_width = 2*sizeof(void *);
				flags |= ZEROPAD;
			}
			count += number(tx_byte,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			continue;

		case 'n':
			if (qualifier == 'L') {
				long long *ip = va_arg(args, long long *);
				*ip = count;
			} else if (qualifier == 'l') {
				long * ip = va_arg(args, long *);
				*ip = count;
			} else {
				int * ip = va_arg(args, int *);
				*ip = count;
			}
			continue;

		case '%':
			tx_byte('%'), count++;
			continue;

		/* integer number formats - set up the flags and "break" */
		case 'o':
			base = 8;
			break;

		case 'X':
			flags |= LARGE;
		case 'x':
			base = 16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;

		default:
			tx_byte('%'), count++;
			if (*fmt)
				tx_byte(*fmt), count++;
			else
				--fmt;
			continue;
		}
		if (qualifier == 'L') {
			num = va_arg(args, unsigned long long);
		} else if (qualifier == 'l') {
			num = va_arg(args, unsigned long);
		} else if (qualifier == 'z') {
			num = va_arg(args, size_t);
		} else if (qualifier == 'h') {
			num = (unsigned short) va_arg(args, int);
			if (flags & SIGN)
				num = (short) num;
		} else if (flags & SIGN) {
			num = va_arg(args, int);
		} else {
			num = va_arg(args, unsigned int);
		}
		count += number(tx_byte, num, base, field_width, precision, flags);
	}
	return count;
}

int do_printk(int msg_level, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vtxprintf(uart_tx_byte, fmt, args);
	va_end(args);

	return i;
}

void bootblock_mainboard_init(void);
void bootblock_mainboard_init(void)
{
	struct mem_timings *mem;
	struct arm_clk_ratios *arm_ratios;

	/* FIXME: we should not need UART in bootblock, this is only
	   done for testing purposes */
	i2c_set_early_reg(I2C0_BASE);
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	power_init();
	mem = get_mem_timings();
	arm_ratios = get_arm_clk_ratios();
	system_clock_init(mem, arm_ratios);
	exynos_pinmux_config(PERIPH_ID_UART3, PINMUX_FLAG_NONE);
	uart_init();
	printk(BIOS_INFO, "%s: UART initialized\n", __func__);
	printk(BIOS_INFO, "%s: finished\n", __func__);
}
