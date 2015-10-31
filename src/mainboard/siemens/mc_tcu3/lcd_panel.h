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

#ifndef _LCD_PANEL_H_
#define _LCD_PANEL_H_

/* This GPIOs are used for LCD panel type encoding */
#define LCD_TYPE_GPIO_BIT0	40
#define LCD_TYPE_GPIO_BIT1	41
#define LCD_TYPE_GPIO_BIT2	42
#define LCD_TYPE_GPIO_BIT3	43

#define LCD_PANEL_TYPE_10_INCH	4
#define LCD_PANEL_TYPE_12_INCH	7
#define LCD_PANEL_TYPE_15_INCH	6
#define LCD_PANEL_TYPE_19_INCH	1
#define LCD_PANEL_TYPE_EDID	15

u8 get_lcd_panel_type(void);
int setup_lcd_panel(void);


#endif /* _LCD_PANEL_H_ */
