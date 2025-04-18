/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/i2c.h>
#include <soc/i2c_common.h>
#include <device/i2c_simple.h>

void mtk_i2c_bus_init(uint8_t bus, uint32_t speed)
{
	mtk_i2c_speed_init(bus, speed);
	mtk_i2c_set_gpio_pinmux(bus);
}

void mtk_i2c_dump_more_info(struct mt_i2c_regs *regs)
{
	printk(BIOS_DEBUG, "LTIMING %x\nCLK_DIV %x\n",
	       read32(&regs->ltiming),
	       read32(&regs->clock_div));
}

void mtk_i2c_config_timing(struct mt_i2c_regs *regs, struct mtk_i2c *bus_ctrl)
{
	write32(&regs->clock_div, bus_ctrl->ac_timing.inter_clk_div);
	write32(&regs->timing, bus_ctrl->ac_timing.htiming);
	write32(&regs->ltiming, bus_ctrl->ac_timing.ltiming);
	write32(&regs->hs, bus_ctrl->ac_timing.hs);
	write32(&regs->ext_conf, bus_ctrl->ac_timing.ext);
}
