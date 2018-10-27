/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Siemens AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <string.h>
#include "soc/gpio.h"
#include "lcd_panel.h"
#include "ptn3460.h"


/** \brief Reads GPIOs used for LCD panel encoding and returns the 4 bit value
 * @param  no parameters
 * @return LCD panel type encoded in 4 bits
 */
u8 get_lcd_panel_type(void)
{
	u8 lcd_type_gpio;

	lcd_type_gpio =  ((read_ssus_gpio(LCD_TYPE_GPIO_BIT3) << 3) |
			  (read_ssus_gpio(LCD_TYPE_GPIO_BIT2) << 2) |
			  (read_ssus_gpio(LCD_TYPE_GPIO_BIT1) << 1) |
			  (read_ssus_gpio(LCD_TYPE_GPIO_BIT0)));
	/* There is an inverter in this signals so we need to invert them as well */
	return ((~lcd_type_gpio) & 0x0f);
}

/** \brief Set up LCD panel
 * @param  no parameters
 * @return 0 on success otherwise error value
 */
int setup_lcd_panel(void)
{
	u8 lcd_type;
	int status;
	char blockname[33];

	lcd_type = get_lcd_panel_type();
	printk(BIOS_INFO, "LCD: Found panel type %d\n", lcd_type);

	switch (lcd_type) {
	case LCD_PANEL_TYPE_10_INCH:
		strcpy(blockname, "hwinfo10.hex");
		break;
	case LCD_PANEL_TYPE_12_INCH:
		strcpy(blockname, "hwinfo12.hex");
		break;
	case LCD_PANEL_TYPE_15_INCH:
		strcpy(blockname, "hwinfo15.hex");
		break;
	case LCD_PANEL_TYPE_19_INCH:
		strcpy(blockname, "hwinfo19.hex");
		break;
	case LCD_PANEL_TYPE_EDID:
		strcpy(blockname, "hwinfo.hex");
		break;
	default:
		printk(BIOS_ERR, "LCD: No supported panel found.\n");
		return 1;
		break;
	}

	/* Now that we have the panel type, set up the DP2LVDS converter */
	status = ptn3460_init(blockname);
	if (status)
		printk(BIOS_ERR, "LCD: Setup PTN with status 0x%x\n", status);
	else
		printk(BIOS_INFO, "LCD: Setup PTN with status 0x%x\n", status);

	return status;
}
