/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <cbfs.h>
#include <device/i2c_simple.h>
#include <gpio.h>
#include <soc/ddp.h>
#include <soc/dsi.h>
#include <soc/gpio_common.h>
#include <soc/mtcmos.h>

#include "gpio.h"
#include "panel.h"

void aw37503_init(unsigned int bus)
{
	i2c_write_field(bus, PMIC_AW37503_SLAVE, 0x00, 0x14, 0x1F, 0);
	i2c_write_field(bus, PMIC_AW37503_SLAVE, 0x01, 0x14, 0x1F, 0);
	i2c_write_field(bus, PMIC_AW37503_SLAVE, 0x21, 0x4C, 0xFF, 0);
	i2c_write_field(bus, PMIC_AW37503_SLAVE, 0x03, 0x43, 0xFF, 0);
	i2c_write_field(bus, PMIC_AW37503_SLAVE, 0x21, 0x00, 0xFF, 0);
}

bool is_pmic_aw37503(unsigned int bus)
{
	u8 vendor_id;
	return (!i2c_read_field(bus, PMIC_AW37503_SLAVE,
				0x04, &vendor_id, 0x0F, 0) && vendor_id == 0x01);
}

void backlight_control(bool enable)
{
	gpio_output(GPIO_AP_EDP_BKLTEN, enable);
	gpio_output(GPIO_BL_PWM_1V8, enable);
}

struct panel_description *get_active_panel(void)
{
	/* Board-specific exceptions */
	if (CONFIG(BOARD_GOOGLE_STEELIX) && board_id() < 2) /* Early builds use PS8640 */
		return get_ps8640_description();

	if (CONFIG(DRIVER_ANALOGIX_ANX7625))
		return get_anx7625_description();

	if (CONFIG(DRIVER_PARADE_PS8640))
		return get_ps8640_description();

	/* MIPI panels */
	return get_panel_description();
}
