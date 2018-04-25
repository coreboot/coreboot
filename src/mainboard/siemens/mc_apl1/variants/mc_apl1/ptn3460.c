/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014-2017 Siemens AG
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
#include <lib.h>
#include <hwilib.h>
#include <device/i2c_simple.h>
#include <variant/ptn3460.h>

/**
 * This function sets up the DP2LVDS-converter to be used with the appropriate
 * lcd panel.
 *
 * @param *hwi_block Filename in CBFS of the block to use as HW-Info.
 * @return 0 on success or HWI-Data/PTN error code.
 */
int ptn3460_init(const char *hwi_block)
{
	struct ptn_3460_config cfg;
	int status;
	uint8_t disp_con = 0, color_depth = 0;
	uint8_t edid_data[PTN_EDID_LEN];
	int i;

	if (!hwi_block || hwilib_find_blocks(hwi_block) != CB_SUCCESS) {
		printk(BIOS_ERR, "LCD: Info block \"%s\" not found!\n",
				hwi_block);
		return 1;
	}
	/* Get all needed information from hwinfo block. */
	if (hwilib_get_field(Edid, edid_data, sizeof(edid_data)) !=
			sizeof(edid_data)) {
		printk(BIOS_ERR, "LCD: No EDID data available in %s\n",
				hwi_block);
		return 1;
	}
	if ((hwilib_get_field(PF_DisplCon, &disp_con, sizeof(disp_con)) !=
			sizeof(disp_con))) {
		printk(BIOS_ERR, "LCD: Missing panel features from %s\n",
				hwi_block);
		return 1;
	}
	if (hwilib_get_field(PF_Color_Depth, &color_depth, sizeof(color_depth))
			!= sizeof(color_depth)) {
		printk(BIOS_ERR, "LCD: Missing panel features from %s\n",
				hwi_block);
		return 1;
	}
	/*
	 * Here, all the desired information for setting up DP2LVDS converter
	 * is present. Inside the converter, table 6 will be used for the
	 * timings.
	 */
	status = ptn3460_write_edid(6, edid_data);
	if (status)
		return status;
	/* Select this table to be emulated. */
	ptn_select_edid(6);
	/* Read PTN configuration data. */
	status = i2c_read_bytes(PTN_I2C_CONTROLLER, PTN_SLAVE_ADR,
			PTN_CONFIG_OFF, (uint8_t *)&cfg,
			sizeof(cfg));
	if (status)
		return (PTN_BUS_ERROR | status);
	/* Set up configuration data according to the hwinfo block we get. */
	cfg.dp_interface_ctrl = 0;
	cfg.lvds_interface_ctrl1 = 0x00;
	if (disp_con == PF_DISPLCON_LVDS_DUAL)
		/* Turn on dual LVDS lane and clock. */
		cfg.lvds_interface_ctrl1 |= 0x0b;
	if (color_depth == PF_COLOR_DEPTH_6BIT)
		/* Use 18 bits per pixel. */
		cfg.lvds_interface_ctrl1 |= 0x20;

	/* No clock spreading, 300 mV LVDS swing. */
	cfg.lvds_interface_ctrl2 = 0x03;
	/* No LVDS signal swap. */
	cfg.lvds_interface_ctrl3 = 0x00;
	/* Delay T2 (VDD to LVDS active) by 16 ms. */
	cfg.t2_delay = 1;
	/* 250 ms from LVDS to backlight active. */
	cfg.t3_timing = 10;
	/* 1 second re-power delay. */
	cfg.t12_timing = 20;
	/* 150 ms backlight off to LVDS inactive. */
	cfg.t4_timing = 3;
	/* Delay T5 (LVDS to VDD inactive) by 16 ms. */
	cfg.t5_delay = 1;
	/* Enable backlight control. */
	cfg.backlight_ctrl = 0;

	/* Write back configuration data to PTN3460. */
	for (i = 0; i < sizeof(struct ptn_3460_config); i++) {
		status = i2c_writeb(PTN_I2C_CONTROLLER, PTN_SLAVE_ADR,
				PTN_CONFIG_OFF+i,
				*(((uint8_t *)&cfg)+i));
		if (status)
			return (PTN_BUS_ERROR | status);
	}

	/* Read PTN configuration data. */
	status = i2c_read_bytes(PTN_I2C_CONTROLLER, PTN_SLAVE_ADR,
			PTN_CONFIG_OFF, (uint8_t *)&cfg, sizeof(cfg));
	if (status)
		return (PTN_BUS_ERROR | status);

	return PTN_NO_ERROR;
}

/**
 * This function writes one Extended Display Identification Data (EDID)
 * structure to PTN3460.
 *
 * @param edid_num Number of EDID that must be written (0..6).
 * @param *data Pointer to a buffer where data to write is stored in.
 * @return 0 on success or error code.
 */
int ptn3460_write_edid(uint8_t edid_num, const uint8_t data[PTN_EDID_LEN])
{
	int status;
	int i;

	if (edid_num > PTN_MAX_EDID_NUM)
		return PTN_INVALID_EDID;

	/* First enable access to the desired EDID table. */
	status = i2c_writeb(PTN_I2C_CONTROLLER, PTN_SLAVE_ADR,
			PTN_CONFIG_OFF + 5, edid_num);
	if (status)
		return (PTN_BUS_ERROR | status);

	/* Now we can simply write EDID-data to ptn3460. */
	for (i = 0; i < PTN_EDID_LEN; i++) {
		status = i2c_writeb(PTN_I2C_CONTROLLER, PTN_SLAVE_ADR,
				PTN_EDID_OFF + i, data[i]);
		if (status)
			return (PTN_BUS_ERROR | status);
	}

	return PTN_NO_ERROR;
}

/**
 * This function selects one of 7 EDID-tables inside PTN3460 which should be
 * emulated on DisplayPort and turn emulation ON.
 *
 * @param edid_num Number of EDID to emulate (0..6).
 * @return 0 on success or error code.
 */
int ptn_select_edid(uint8_t edid_num)
{
	int status;
	uint8_t val;

	if (edid_num > PTN_MAX_EDID_NUM)
		return PTN_INVALID_EDID;
	/* Enable emulation of the desired EDID table. */
	val = (edid_num << 1) | 1;
	status = i2c_writeb(PTN_I2C_CONTROLLER, PTN_SLAVE_ADR,
			PTN_CONFIG_OFF + 4, val);
	if (status)
		return (PTN_BUS_ERROR | status);
	else
		return PTN_NO_ERROR;
}
