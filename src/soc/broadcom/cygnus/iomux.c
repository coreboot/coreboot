/*
* Copyright (C) 2015 Broadcom Corporation
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation version 2.
*
* This program is distributed "as is" WITHOUT ANY WARRANTY of any
* kind, whether express or implied; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include <arch/io.h>
#include <console/console.h>
#include <gpio.h>
#include <stdlib.h>
#include <string.h>
#include <soc/addressmap.h>

#define CYGNUS_NUM_IOMUX_REGS     8
#define CYGNUS_NUM_MUX_PER_REG    8
#define CYGNUS_NUM_IOMUX          (CYGNUS_NUM_IOMUX_REGS * \
				   CYGNUS_NUM_MUX_PER_REG)

/*
 * Cygnus IOMUX pinctrl core
 *
 * @base1: second I/O register base
 */
struct cygnus_pinctrl {
	void *base1;
};

static struct cygnus_pinctrl pinctrl_data = {
	.base1 = (void *)IPROC_IOMUX_OVERRIDE_BASE
};

/*
 * Certain pins can be individually muxed to GPIO function
 *
 * @is_supported: flag to indicate GPIO mux is supported for this pin
 * @offset: register offset for GPIO mux override of a pin
 * @shift: bit shift for GPIO mux override of a pin
 */
struct cygnus_gpio_mux {
	int is_supported;
	unsigned int offset;
	unsigned int shift;
};

/*
 * Description of a pin in Cygnus
 *
 * @pin: pin number
 * @name: pin name
 * @gpio_mux: GPIO override related information
 */
struct cygnus_pin {
	unsigned pin;
	const char *name;
	struct cygnus_gpio_mux gpio_mux;
};

#define CYGNUS_PIN_DESC(p, n, i, o, s)	\
{					\
	.pin = p,			\
	.name = n,			\
	.gpio_mux = {			\
		.is_supported = i,	\
		.offset = o,		\
		.shift = s,		\
	},				\
}

/*
 * List of pins in Cygnus
 */
static struct cygnus_pin cygnus_pins[] = {
	CYGNUS_PIN_DESC(0, "ext_device_reset_n", 0, 0, 0),
	CYGNUS_PIN_DESC(1, "chip_mode0", 0, 0, 0),
	CYGNUS_PIN_DESC(2, "chip_mode1", 0, 0, 0),
	CYGNUS_PIN_DESC(3, "chip_mode2", 0, 0, 0),
	CYGNUS_PIN_DESC(4, "chip_mode3", 0, 0, 0),
	CYGNUS_PIN_DESC(5, "chip_mode4", 0, 0, 0),
	CYGNUS_PIN_DESC(6, "bsc0_scl", 0, 0, 0),
	CYGNUS_PIN_DESC(7, "bsc0_sda", 0, 0, 0),
	CYGNUS_PIN_DESC(8, "bsc1_scl", 0, 0, 0),
	CYGNUS_PIN_DESC(9, "bsc1_sda", 0, 0, 0),
	CYGNUS_PIN_DESC(10, "d1w_dq", 1, 0x28, 0),
	CYGNUS_PIN_DESC(11, "d1wowstz_l", 1, 0x4, 28),
	CYGNUS_PIN_DESC(12, "gpio0", 0, 0, 0),
	CYGNUS_PIN_DESC(13, "gpio1", 0, 0, 0),
	CYGNUS_PIN_DESC(14, "gpio2", 0, 0, 0),
	CYGNUS_PIN_DESC(15, "gpio3", 0, 0, 0),
	CYGNUS_PIN_DESC(16, "gpio4", 0, 0, 0),
	CYGNUS_PIN_DESC(17, "gpio5", 0, 0, 0),
	CYGNUS_PIN_DESC(18, "gpio6", 0, 0, 0),
	CYGNUS_PIN_DESC(19, "gpio7", 0, 0, 0),
	CYGNUS_PIN_DESC(20, "gpio8", 0, 0, 0),
	CYGNUS_PIN_DESC(21, "gpio9", 0, 0, 0),
	CYGNUS_PIN_DESC(22, "gpio10", 0, 0, 0),
	CYGNUS_PIN_DESC(23, "gpio11", 0, 0, 0),
	CYGNUS_PIN_DESC(24, "gpio12", 0, 0, 0),
	CYGNUS_PIN_DESC(25, "gpio13", 0, 0, 0),
	CYGNUS_PIN_DESC(26, "gpio14", 0, 0, 0),
	CYGNUS_PIN_DESC(27, "gpio15", 0, 0, 0),
	CYGNUS_PIN_DESC(28, "gpio16", 0, 0, 0),
	CYGNUS_PIN_DESC(29, "gpio17", 0, 0, 0),
	CYGNUS_PIN_DESC(30, "gpio18", 0, 0, 0),
	CYGNUS_PIN_DESC(31, "gpio19", 0, 0, 0),
	CYGNUS_PIN_DESC(32, "gpio20", 0, 0, 0),
	CYGNUS_PIN_DESC(33, "gpio21", 0, 0, 0),
	CYGNUS_PIN_DESC(34, "gpio22", 0, 0, 0),
	CYGNUS_PIN_DESC(35, "gpio23", 0, 0, 0),
	CYGNUS_PIN_DESC(36, "mdc", 0, 0, 0),
	CYGNUS_PIN_DESC(37, "mdio", 0, 0, 0),
	CYGNUS_PIN_DESC(38, "pwm0", 1, 0x10, 30),
	CYGNUS_PIN_DESC(39, "pwm1", 1, 0x10, 28),
	CYGNUS_PIN_DESC(40, "pwm2", 1, 0x10, 26),
	CYGNUS_PIN_DESC(41, "pwm3", 1, 0x10, 24),
	CYGNUS_PIN_DESC(42, "sc0_clk", 1, 0x10, 22),
	CYGNUS_PIN_DESC(43, "sc0_cmdvcc_l", 1, 0x10, 20),
	CYGNUS_PIN_DESC(44, "sc0_detect", 1, 0x10, 18),
	CYGNUS_PIN_DESC(45, "sc0_fcb", 1, 0x10, 16),
	CYGNUS_PIN_DESC(46, "sc0_io", 1, 0x10, 14),
	CYGNUS_PIN_DESC(47, "sc0_rst_l", 1, 0x10, 12),
	CYGNUS_PIN_DESC(48, "sc1_clk", 1, 0x10, 10),
	CYGNUS_PIN_DESC(49, "sc1_cmdvcc_l", 1, 0x10, 8),
	CYGNUS_PIN_DESC(50, "sc1_detect", 1, 0x10, 6),
	CYGNUS_PIN_DESC(51, "sc1_fcb", 1, 0x10, 4),
	CYGNUS_PIN_DESC(52, "sc1_io", 1, 0x10, 2),
	CYGNUS_PIN_DESC(53, "sc1_rst_l", 1, 0x10, 0),
	CYGNUS_PIN_DESC(54, "spi0_clk", 1, 0x18, 10),
	CYGNUS_PIN_DESC(55, "spi0_mosi", 1, 0x18, 6),
	CYGNUS_PIN_DESC(56, "spi0_miso", 1, 0x18, 8),
	CYGNUS_PIN_DESC(57, "spi0_ss", 1, 0x18, 4),
	CYGNUS_PIN_DESC(58, "spi1_clk", 1, 0x18, 2),
	CYGNUS_PIN_DESC(59, "spi1_mosi", 1, 0x1c, 30),
	CYGNUS_PIN_DESC(60, "spi1_miso", 1, 0x18, 0),
	CYGNUS_PIN_DESC(61, "spi1_ss", 1, 0x1c, 28),
	CYGNUS_PIN_DESC(62, "spi2_clk", 1, 0x1c, 26),
	CYGNUS_PIN_DESC(63, "spi2_mosi", 1, 0x1c, 22),
	CYGNUS_PIN_DESC(64, "spi2_miso", 1, 0x1c, 24),
	CYGNUS_PIN_DESC(65, "spi2_ss", 1, 0x1c, 20),
	CYGNUS_PIN_DESC(66, "spi3_clk", 1, 0x1c, 18),
	CYGNUS_PIN_DESC(67, "spi3_mosi", 1, 0x1c, 14),
	CYGNUS_PIN_DESC(68, "spi3_miso", 1, 0x1c, 16),
	CYGNUS_PIN_DESC(69, "spi3_ss", 1, 0x1c, 12),
	CYGNUS_PIN_DESC(70, "uart0_cts", 1, 0x1c, 10),
	CYGNUS_PIN_DESC(71, "uart0_rts", 1, 0x1c, 8),
	CYGNUS_PIN_DESC(72, "uart0_rx", 1, 0x1c, 6),
	CYGNUS_PIN_DESC(73, "uart0_tx", 1, 0x1c, 4),
	CYGNUS_PIN_DESC(74, "uart1_cts", 1, 0x1c, 2),
	CYGNUS_PIN_DESC(75, "uart1_dcd", 1, 0x1c, 0),
	CYGNUS_PIN_DESC(76, "uart1_dsr", 1, 0x20, 14),
	CYGNUS_PIN_DESC(77, "uart1_dtr", 1, 0x20, 12),
	CYGNUS_PIN_DESC(78, "uart1_ri", 1, 0x20, 10),
	CYGNUS_PIN_DESC(79, "uart1_rts", 1, 0x20, 8),
	CYGNUS_PIN_DESC(80, "uart1_rx", 1, 0x20, 6),
	CYGNUS_PIN_DESC(81, "uart1_tx", 1, 0x20, 4),
	CYGNUS_PIN_DESC(82, "uart3_rx", 1, 0x20, 2),
	CYGNUS_PIN_DESC(83, "uart3_tx", 1, 0x20, 0),
	CYGNUS_PIN_DESC(84, "sdio1_clk_sdcard", 1, 0x14, 6),
	CYGNUS_PIN_DESC(85, "sdio1_cmd", 1, 0x14, 4),
	CYGNUS_PIN_DESC(86, "sdio1_data0", 1, 0x14, 2),
	CYGNUS_PIN_DESC(87, "sdio1_data1", 1, 0x14, 0),
	CYGNUS_PIN_DESC(88, "sdio1_data2", 1, 0x18, 30),
	CYGNUS_PIN_DESC(89, "sdio1_data3", 1, 0x18, 28),
	CYGNUS_PIN_DESC(90, "sdio1_wp_n", 1, 0x18, 24),
	CYGNUS_PIN_DESC(91, "sdio1_card_rst", 1, 0x14, 10),
	CYGNUS_PIN_DESC(92, "sdio1_led_on", 1, 0x18, 26),
	CYGNUS_PIN_DESC(93, "sdio1_cd", 1, 0x14, 8),
	CYGNUS_PIN_DESC(94, "sdio0_clk_sdcard", 1, 0x14, 26),
	CYGNUS_PIN_DESC(95, "sdio0_cmd", 1, 0x14, 24),
	CYGNUS_PIN_DESC(96, "sdio0_data0", 1, 0x14, 22),
	CYGNUS_PIN_DESC(97, "sdio0_data1", 1, 0x14, 20),
	CYGNUS_PIN_DESC(98, "sdio0_data2", 1, 0x14, 18),
	CYGNUS_PIN_DESC(99, "sdio0_data3", 1, 0x14, 16),
	CYGNUS_PIN_DESC(100, "sdio0_wp_n", 1, 0x14, 12),
	CYGNUS_PIN_DESC(101, "sdio0_card_rst", 1, 0x14, 30),
	CYGNUS_PIN_DESC(102, "sdio0_led_on", 1, 0x14, 14),
	CYGNUS_PIN_DESC(103, "sdio0_cd", 1, 0x14, 28),
	CYGNUS_PIN_DESC(104, "sflash_clk", 1, 0x18, 22),
	CYGNUS_PIN_DESC(105, "sflash_cs_l", 1, 0x18, 20),
	CYGNUS_PIN_DESC(106, "sflash_mosi", 1, 0x18, 14),
	CYGNUS_PIN_DESC(107, "sflash_miso", 1, 0x18, 16),
	CYGNUS_PIN_DESC(108, "sflash_wp_n", 1, 0x18, 12),
	CYGNUS_PIN_DESC(109, "sflash_hold_n", 1, 0x18, 18),
	CYGNUS_PIN_DESC(110, "nand_ale", 1, 0xc, 30),
	CYGNUS_PIN_DESC(111, "nand_ce0_l", 1, 0xc, 28),
	CYGNUS_PIN_DESC(112, "nand_ce1_l", 1, 0xc, 26),
	CYGNUS_PIN_DESC(113, "nand_cle", 1, 0xc, 24),
	CYGNUS_PIN_DESC(114, "nand_dq0", 1, 0xc, 22),
	CYGNUS_PIN_DESC(115, "nand_dq1", 1, 0xc, 20),
	CYGNUS_PIN_DESC(116, "nand_dq2", 1, 0xc, 18),
	CYGNUS_PIN_DESC(117, "nand_dq3", 1, 0xc, 16),
	CYGNUS_PIN_DESC(118, "nand_dq4", 1, 0xc, 14),
	CYGNUS_PIN_DESC(119, "nand_dq5", 1, 0xc, 12),
	CYGNUS_PIN_DESC(120, "nand_dq6", 1, 0xc, 10),
	CYGNUS_PIN_DESC(121, "nand_dq7", 1, 0xc, 8),
	CYGNUS_PIN_DESC(122, "nand_rb_l", 1, 0xc, 6),
	CYGNUS_PIN_DESC(123, "nand_re_l", 1, 0xc, 4),
	CYGNUS_PIN_DESC(124, "nand_we_l", 1, 0xc, 2),
	CYGNUS_PIN_DESC(125, "nand_wp_l", 1, 0xc, 0),
	CYGNUS_PIN_DESC(126, "lcd_clac", 1, 0x4, 26),
	CYGNUS_PIN_DESC(127, "lcd_clcp", 1, 0x4, 24),
	CYGNUS_PIN_DESC(128, "lcd_cld0", 1, 0x4, 22),
	CYGNUS_PIN_DESC(129, "lcd_cld1", 1, 0x4, 0),
	CYGNUS_PIN_DESC(130, "lcd_cld10", 1, 0x4, 20),
	CYGNUS_PIN_DESC(131, "lcd_cld11", 1, 0x4, 18),
	CYGNUS_PIN_DESC(132, "lcd_cld12", 1, 0x4, 16),
	CYGNUS_PIN_DESC(133, "lcd_cld13", 1, 0x4, 14),
	CYGNUS_PIN_DESC(134, "lcd_cld14", 1, 0x4, 12),
	CYGNUS_PIN_DESC(135, "lcd_cld15", 1, 0x4, 10),
	CYGNUS_PIN_DESC(136, "lcd_cld16", 1, 0x4, 8),
	CYGNUS_PIN_DESC(137, "lcd_cld17", 1, 0x4, 6),
	CYGNUS_PIN_DESC(138, "lcd_cld18", 1, 0x4, 4),
	CYGNUS_PIN_DESC(139, "lcd_cld19", 1, 0x4, 2),
	CYGNUS_PIN_DESC(140, "lcd_cld2", 1, 0x8, 22),
	CYGNUS_PIN_DESC(141, "lcd_cld20", 1, 0x8, 30),
	CYGNUS_PIN_DESC(142, "lcd_cld21", 1, 0x8, 28),
	CYGNUS_PIN_DESC(143, "lcd_cld22", 1, 0x8, 26),
	CYGNUS_PIN_DESC(144, "lcd_cld23", 1, 0x8, 24),
	CYGNUS_PIN_DESC(145, "lcd_cld3", 1, 0x8, 20),
	CYGNUS_PIN_DESC(146, "lcd_cld4", 1, 0x8, 18),
	CYGNUS_PIN_DESC(147, "lcd_cld5", 1, 0x8, 16),
	CYGNUS_PIN_DESC(148, "lcd_cld6", 1, 0x8, 14),
	CYGNUS_PIN_DESC(149, "lcd_cld7", 1, 0x8, 12),
	CYGNUS_PIN_DESC(150, "lcd_cld8", 1, 0x8, 10),
	CYGNUS_PIN_DESC(151, "lcd_cld9", 1, 0x8, 8),
	CYGNUS_PIN_DESC(152, "lcd_clfp", 1, 0x8, 6),
	CYGNUS_PIN_DESC(153, "lcd_clle", 1, 0x8, 4),
	CYGNUS_PIN_DESC(154, "lcd_cllp", 1, 0x8, 2),
	CYGNUS_PIN_DESC(155, "lcd_clpower", 1, 0x8, 0),
	CYGNUS_PIN_DESC(156, "camera_vsync", 1, 0x4, 30),
	CYGNUS_PIN_DESC(157, "camera_trigger", 1, 0x0, 0),
	CYGNUS_PIN_DESC(158, "camera_strobe", 1, 0x0, 2),
	CYGNUS_PIN_DESC(159, "camera_standby", 1, 0x0, 4),
	CYGNUS_PIN_DESC(160, "camera_reset_n", 1, 0x0, 6),
	CYGNUS_PIN_DESC(161, "camera_pixdata9", 1, 0x0, 8),
	CYGNUS_PIN_DESC(162, "camera_pixdata8", 1, 0x0, 10),
	CYGNUS_PIN_DESC(163, "camera_pixdata7", 1, 0x0, 12),
	CYGNUS_PIN_DESC(164, "camera_pixdata6", 1, 0x0, 14),
	CYGNUS_PIN_DESC(165, "camera_pixdata5", 1, 0x0, 16),
	CYGNUS_PIN_DESC(166, "camera_pixdata4", 1, 0x0, 18),
	CYGNUS_PIN_DESC(167, "camera_pixdata3", 1, 0x0, 20),
	CYGNUS_PIN_DESC(168, "camera_pixdata2", 1, 0x0, 22),
	CYGNUS_PIN_DESC(169, "camera_pixdata1", 1, 0x0, 24),
	CYGNUS_PIN_DESC(170, "camera_pixdata0", 1, 0x0, 26),
	CYGNUS_PIN_DESC(171, "camera_pixclk", 1, 0x0, 28),
	CYGNUS_PIN_DESC(172, "camera_hsync", 1, 0x0, 30),
	CYGNUS_PIN_DESC(173, "camera_pll_ref_clk", 0, 0, 0),
	CYGNUS_PIN_DESC(174, "usb_id_indication", 0, 0, 0),
	CYGNUS_PIN_DESC(175, "usb_vbus_indication", 0, 0, 0),
	CYGNUS_PIN_DESC(176, "gpio0_3p3", 0, 0, 0),
	CYGNUS_PIN_DESC(177, "gpio1_3p3", 0, 0, 0),
	CYGNUS_PIN_DESC(178, "gpio2_3p3", 0, 0, 0),
	CYGNUS_PIN_DESC(179, "gpio3_3p3", 0, 0, 0),
};

int cygnus_gpio_request_enable(void *priv, unsigned pin)
{
	struct cygnus_pinctrl *pinctrl = (struct cygnus_pinctrl *)priv;
	struct cygnus_gpio_mux *mux = &cygnus_pins[pin].gpio_mux;
	u32 val;

	/* not all pins support GPIO pinmux override */
	if (!mux->is_supported)
		return -ENOTSUPP;

	val = read32(pinctrl->base1 + mux->offset);
	val |= 0x3 << mux->shift;
	write32(pinctrl->base1 + mux->offset, val);

	printk(BIOS_INFO, "gpio request enable pin=%u offset=0x%x shift=%u\n",
		pin, mux->offset, mux->shift);

	return 0;
}

void cygnus_gpio_disable_free(void *priv, unsigned pin)
{
	struct cygnus_pinctrl *pinctrl = (struct cygnus_pinctrl *)priv;
	struct cygnus_gpio_mux *mux = &cygnus_pins[pin].gpio_mux;
	u32 val;

	if (!mux->is_supported)
		return;

	val = read32(pinctrl->base1 + mux->offset);
	val &= ~(0x3 << mux->shift);
	write32(pinctrl->base1 + mux->offset, val);

	printk(BIOS_INFO, "gpio disable free pin=%u offset=0x%x shift=%u\n",
		pin, mux->offset, mux->shift);
}

void *cygnus_pinmux_init()
{
	return &pinctrl_data;
}
