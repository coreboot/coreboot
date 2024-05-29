/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <device/mmio.h>
#include <gpio.h>

enum {
	GPIO_DIRECTION_IN = 0,
	GPIO_DIRECTION_OUT = 1,
};

enum {
	GPIO_MODE = 0,
};

static void pos_bit_calc(gpio_t gpio, u32 *pos, u32 *bit)
{
	*pos = gpio.id / MAX_GPIO_REG_BITS;
	*bit = gpio.id % MAX_GPIO_REG_BITS;
}

static void pos_bit_calc_for_mode(gpio_t gpio, u32 *pos, u32 *bit)
{
	*pos = gpio.id / MAX_GPIO_MODE_PER_REG;
	*bit = (gpio.id % MAX_GPIO_MODE_PER_REG) * GPIO_MODE_BITS;
}

static s32 gpio_set_dir(gpio_t gpio, u32 dir)
{
	u32 pos;
	u32 bit;
	u32 *reg;

	pos_bit_calc(gpio, &pos, &bit);

	if (dir == GPIO_DIRECTION_IN)
		reg = &mtk_gpio->dir[pos].rst;
	else
		reg = &mtk_gpio->dir[pos].set;

	write32(reg, 1L << bit);

	return 0;
}

void gpio_set_mode(gpio_t gpio, int mode)
{
	u32 pos;
	u32 bit;
	u32 mask = (1L << GPIO_MODE_BITS) - 1;

	pos_bit_calc_for_mode(gpio, &pos, &bit);

	clrsetbits32(&mtk_gpio->mode[pos].val, mask << bit, mode << bit);
}

int gpio_get(gpio_t gpio)
{
	u32 pos;
	u32 bit;
	u32 *reg;
	u32 data;

	pos_bit_calc(gpio, &pos, &bit);

	reg = &mtk_gpio->din[pos].val;
	data = read32(reg);

	return (data & (1L << bit)) ? 1 : 0;
}

void gpio_set(gpio_t gpio, int output)
{
	u32 pos;
	u32 bit;
	u32 *reg;

	pos_bit_calc(gpio, &pos, &bit);

	if (output == 0)
		reg = &mtk_gpio->dout[pos].rst;
	else
		reg = &mtk_gpio->dout[pos].set;

	write32(reg, 1L << bit);
}

void gpio_input_pulldown(gpio_t gpio)
{
	gpio_set_pull(gpio, GPIO_PULL_ENABLE, GPIO_PULL_DOWN);
	gpio_set_dir(gpio, GPIO_DIRECTION_IN);
	gpio_set_mode(gpio, GPIO_MODE);
}

void gpio_input_pullup(gpio_t gpio)
{
	gpio_set_pull(gpio, GPIO_PULL_ENABLE, GPIO_PULL_UP);
	gpio_set_dir(gpio, GPIO_DIRECTION_IN);
	gpio_set_mode(gpio, GPIO_MODE);
}

void gpio_input(gpio_t gpio)
{
	gpio_set_pull(gpio, GPIO_PULL_DISABLE, GPIO_PULL_DOWN);
	gpio_set_dir(gpio, GPIO_DIRECTION_IN);
	gpio_set_mode(gpio, GPIO_MODE);
}

void gpio_output(gpio_t gpio, int value)
{
	gpio_set_pull(gpio, GPIO_PULL_DISABLE, GPIO_PULL_DOWN);
	gpio_set(gpio, value);
	gpio_set_dir(gpio, GPIO_DIRECTION_OUT);
	gpio_set_mode(gpio, GPIO_MODE);
}

int gpio_eint_poll(gpio_t gpio)
{
	u32 pos;
	u32 bit;
	u32 status;
	struct eint_regs *mtk_eint;

	gpio_calc_eint_pos_bit(gpio, &pos, &bit);
	mtk_eint = gpio_get_eint_reg(gpio);
	assert(mtk_eint);

	status = (read32(&mtk_eint->sta.regs[pos]) >> bit) & 0x1;

	if (status)
		write32(&mtk_eint->ack.regs[pos], 1 << bit);

	return status;
}

void gpio_eint_configure(gpio_t gpio, enum gpio_irq_type type)
{
	u32 pos;
	u32 bit, mask;
	struct eint_regs *mtk_eint;

	gpio_calc_eint_pos_bit(gpio, &pos, &bit);
	mtk_eint = gpio_get_eint_reg(gpio);
	assert(mtk_eint);

	mask = 1 << bit;

	/* Make it an input first. */
	gpio_input_pullup(gpio);

	write32(&mtk_eint->d0en[pos], mask);

	switch (type) {
	case IRQ_TYPE_EDGE_FALLING:
		write32(&mtk_eint->sens_clr.regs[pos], mask);
		write32(&mtk_eint->pol_clr.regs[pos], mask);
		break;
	case IRQ_TYPE_EDGE_RISING:
		write32(&mtk_eint->sens_clr.regs[pos], mask);
		write32(&mtk_eint->pol_set.regs[pos], mask);
		break;
	case IRQ_TYPE_LEVEL_LOW:
		write32(&mtk_eint->sens_set.regs[pos], mask);
		write32(&mtk_eint->pol_clr.regs[pos], mask);
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		write32(&mtk_eint->sens_set.regs[pos], mask);
		write32(&mtk_eint->pol_set.regs[pos], mask);
		break;
	}

	write32(&mtk_eint->mask_clr.regs[pos], mask);
}

static inline bool is_valid_drv(uint8_t drv)
{
	return drv <= GPIO_DRV_16_MA;
}

static inline bool is_valid_drv_adv(enum gpio_drv_adv drv)
{
	return drv <= GPIO_DRV_ADV_1_MA && drv >= GPIO_DRV_ADV_125_UA;
}

int gpio_set_driving(gpio_t gpio, uint8_t drv)
{
	uint32_t mask;
	const struct gpio_drv_info *info = get_gpio_driving_info(gpio.id);
	const struct gpio_drv_info *adv_info = get_gpio_driving_adv_info(gpio.id);
	void *reg, *reg_adv, *reg_addr;

	if (!info)
		return -1;

	if (!is_valid_drv(drv))
		return -1;

	if (info->width == 0)
		return -1;

	mask = BIT(info->width) - 1;
	/* Check setting value is not beyond width */
	if ((uint32_t)drv > mask)
		return -1;

	reg_addr = gpio_find_reg_addr(gpio);
	reg = reg_addr + info->offset;
	clrsetbits32(reg, mask << info->shift, drv << info->shift);

	/* Disable EH if supported */
	if (adv_info && adv_info->width != 0) {
		reg_adv = reg_addr + adv_info->offset;
		clrbits32(reg_adv, BIT(adv_info->shift));
	}

	return 0;
}

int gpio_get_driving(gpio_t gpio)
{
	const struct gpio_drv_info *info = get_gpio_driving_info(gpio.id);
	void *reg;

	if (!info)
		return -1;

	if (info->width == 0)
		return -1;

	reg = gpio_find_reg_addr(gpio) + info->offset;
	return (read32(reg) >> info->shift) & (BIT(info->width) - 1);
}

int gpio_set_driving_adv(gpio_t gpio, enum gpio_drv_adv drv)
{
	uint32_t mask;
	const struct gpio_drv_info *adv_info = get_gpio_driving_adv_info(gpio.id);
	void *reg_adv;

	if (!adv_info)
		return -1;

	if (!is_valid_drv_adv(drv))
		return -1;

	if (adv_info->width == 0)
		return -1;

	/* Not include EH bit (the lowest bit) */
	if ((uint32_t)drv > (BIT(adv_info->width - 1) - 1))
		return -1;

	reg_adv = gpio_find_reg_addr(gpio) + adv_info->offset;
	mask = BIT(adv_info->width) - 1;
	/* EH enable */
	drv = (drv << 1) | BIT(0);

	clrsetbits32(reg_adv, mask << adv_info->shift, drv << adv_info->shift);

	return 0;
}

int gpio_get_driving_adv(gpio_t gpio)
{
	const struct gpio_drv_info *adv_info = get_gpio_driving_adv_info(gpio.id);
	void *reg_adv;
	uint32_t drv;

	if (!adv_info)
		return -1;

	if (adv_info->width == 0)
		return -1;

	reg_adv = gpio_find_reg_addr(gpio) + adv_info->offset;
	drv = (read32(reg_adv) >> adv_info->shift) & (BIT(adv_info->width) - 1);

	/* Drop EH bit */
	return drv >> 1;
}
