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
#include <hwilib.h>
#include <string.h>
#include "soc/i2c.h"
#include "ptn3460.h"

/** \brief This functions sets up the DP2LVDS-converter to be used with the
 *         appropriate lcd panel
 * @param  *hwi_block	Filename in CBFS of the block to use as HW-Info
 * @return		0 on success or error code
 */
int ptn3460_init(char *hwi_block)
{
	struct ptn_3460_config cfg;
	int status;
	uint8_t disp_con = 0, color_depth = 0;
	uint8_t edid_data[0x80];
	uint8_t hwid[4], tcu31_hwid[4] = {7, 9, 2, 0};

	if (!hwi_block || hwilib_find_blocks(hwi_block) != CB_SUCCESS) {
		printk(BIOS_ERR, "LCD: Info block \"%s\" not found!\n",
			hwi_block);
		return 1;
	}

	status = i2c_init(PTN_I2C_CONTROLLER);
	if (status)
		return (PTN_BUS_ERROR | status);

	/* Get all needed information from hwinfo block */
	if (hwilib_get_field(Edid, edid_data, 0x80) != sizeof(edid_data)) {
		printk(BIOS_ERR, "LCD: No EDID data available in %s\n",
			hwi_block);
		return 1;
	}
	if ((hwilib_get_field(PF_DisplCon, &disp_con, 1) != 1)) {
		printk(BIOS_ERR, "LCD: Missing panel features from %s\n",
			hwi_block);
		return 1;
	}
	if (hwilib_get_field(PF_Color_Depth ,&color_depth, 1) != 1) {
		printk(BIOS_ERR, "LCD: Missing panel features from %s\n",
			hwi_block);
		return 1;
	}
	/* Here, all the desired information for setting up DP2LVDS converter*/
	/* are present. Inside the converter, table 6 will be used for */
	/* the timings. */
	if ((status = ptn3460_write_edid(6, edid_data)) != 0)
		return status;
	/* Select this table to be emulated */
	ptn_select_edid(6);
	/* Read PTN configuration data */
	status = i2c_read(PTN_I2C_CONTROLLER, PTN_SLAVE_ADR, PTN_CONFIG_OFF,
			  (u8*)&cfg, PTN_CONFIG_LEN);
	if (status)
		return (PTN_BUS_ERROR | status);

	/* Set up configuration data according to the hwinfo blocks we get */
	cfg.dp_interface_ctrl = 0;
	cfg.lvds_interface_ctrl1 = 0x00;
	if (disp_con == PF_DISPLCON_LVDS_DUAL)
		cfg.lvds_interface_ctrl1 |= 0x0b; /* Turn on dual LVDS lane and clock */
	if (color_depth == PF_COLOR_DEPTH_6BIT)
		cfg.lvds_interface_ctrl1 |= 0x20; /* Use 18 bits per pixel */

	cfg.lvds_interface_ctrl2 = 0x03;  /* no clock spreading, 300 mV LVDS swing */
	/* Swap LVDS even and odd lanes for HW-ID 7.9.2.0 only. */
	if (hwilib_get_field(HWID, hwid, sizeof(hwid)) == sizeof(hwid) &&
	    !(memcmp(hwid, tcu31_hwid, sizeof(hwid)))) {
		cfg.lvds_interface_ctrl3 = 0x01;  /* swap LVDS even and odd */
	} else
		cfg.lvds_interface_ctrl3 = 0x00;  /* no LVDS signal swap */
	cfg.t2_delay = 1;		  /* Delay T2 (VDD to LVDS active) by 16 ms */
	cfg.t3_timing = 10;		  /* 500 ms from LVDS to backlight active */
	cfg.t12_timing = 20;		  /* 1 second re-power delay */
	cfg.t4_timing = 3;		  /* 150 ms backlight off to LVDS inactive */
	cfg.t5_delay = 1;		  /* Delay T5 (LVDS to VDD inactive) by 16 ms */
	cfg.backlight_ctrl = 0;		  /* Enable backlight control */

	/* Write back configuration data to PTN3460 */
	status = i2c_write(PTN_I2C_CONTROLLER, PTN_SLAVE_ADR, PTN_CONFIG_OFF,
			   (u8*)&cfg, PTN_CONFIG_LEN);
	if (status)
		return (PTN_BUS_ERROR | status);
	else
		return PTN_NO_ERROR;
}

/** \brief This functions reads one desired EDID data structure from PTN3460
 * @param  edid_num Number of EDID that must be read (0..6)
 * @param  *data    Pointer to a buffer where to store read data
 * @return          0 on success or error code
 */
int ptn3460_read_edid(u8 edid_num, u8 *data)
{
	int status;

	if (edid_num > PTN_MAX_EDID_NUM)
		return PTN_INVALID_EDID;
	/* First enable access to the desired EDID table */
	status = i2c_write(PTN_I2C_CONTROLLER, PTN_SLAVE_ADR, PTN_CONFIG_OFF + 5,
			   &edid_num, 1);
	if (status)
		return (PTN_BUS_ERROR | status);

	/* Now we can simply read back EDID-data */
	status = i2c_read(PTN_I2C_CONTROLLER, PTN_SLAVE_ADR, PTN_EDID_OFF,
			  data, PTN_EDID_LEN);
	if (status)
		return (PTN_BUS_ERROR | status);
	else
		return PTN_NO_ERROR;
}

/** \brief This functions writes one EDID data structure to PTN3460
 * @param  edid_num  Number of EDID that must be written (0..6)
 * @param  *data     Pointer to a buffer where data to write is stored in
 * @return           0 on success or error code
 */
int ptn3460_write_edid(u8 edid_num, u8 *data)
{
	int status;

	if (edid_num > PTN_MAX_EDID_NUM)
		return PTN_INVALID_EDID;
	/* First enable access to the desired EDID table */
	status = i2c_write(PTN_I2C_CONTROLLER, PTN_SLAVE_ADR, PTN_CONFIG_OFF + 5,
			   &edid_num, 1);
	if (status)
		return (PTN_BUS_ERROR | status);

	/* Now we can simply write EDID-data to ptn3460 */
	status = i2c_write(PTN_I2C_CONTROLLER, PTN_SLAVE_ADR, PTN_EDID_OFF,
			   data, PTN_EDID_LEN);
	if (status)
		return (PTN_BUS_ERROR | status);
	else
		return PTN_NO_ERROR;
}

/** \brief  This functions selects one of 7 EDID-tables inside PTN3460
 *          which should be emulated on display port and turn emulation ON
 * @param   edid_num  Number of EDID to emulate (0..6)
 * @return            0 on success or error code
 */
int ptn_select_edid (u8 edid_num)
{
	int status;
	u8 val;

	if (edid_num > PTN_MAX_EDID_NUM)
		return PTN_INVALID_EDID;
	/* Enable emulation of the desired EDID table */
	val = (edid_num << 1) | 1;
	status = i2c_write(PTN_I2C_CONTROLLER, PTN_SLAVE_ADR, PTN_CONFIG_OFF + 4,
			   &val, 1);
	if (status)
		return (PTN_BUS_ERROR | status);
	else
		return PTN_NO_ERROR;
}

/** \brief This functions performs a flash operation which will write
 *         current configuration table (all the EDID-tables and the
 *         configuration space with a total amount of 1 KByte)
 *         to the internal flash of PTN3460
 * @param  none
 * @return 0 on success or error code
 */
int ptn3460_flash_config(void)
{
	int status;
	struct ptn_3460_flash flash;

	flash.cmd = 0x01;	/* perform erase and flash cycle */
	flash.magic = 0x7845;	/* Magic number to protect flash operation */
	flash.trigger = 0x56;	/* This value starts flash operation */
	status = i2c_write(PTN_I2C_CONTROLLER, PTN_SLAVE_ADR, PTN_FLASH_CFG_OFF,
			   (u8*)&flash, PTN_FLASH_CFG_LEN);
	if (status) {
		return (PTN_BUS_ERROR | status);
	} else {
		/* To ensure the flash operation is finished, we have to wait 300 ms */
		mdelay(300);
		return PTN_NO_ERROR;
	}
}
