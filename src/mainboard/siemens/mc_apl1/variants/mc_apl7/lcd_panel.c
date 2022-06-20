/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <drivers/i2c/ptn3460/ptn3460.h>
#include <hwilib.h>
#include <types.h>

/** \brief This function provides EDID data to the driver for DP2LVDS Bridge (PTN3460)
 * @param  edid_data	pointer to EDID data in driver
*/
enum cb_err mb_get_edid(uint8_t edid_data[0x80])
{
	const char *hwi_block = "hwinfo.hex";

	if (hwilib_find_blocks(hwi_block) != CB_SUCCESS) {
		printk(BIOS_ERR, "LCD: Info block \"%s\" not found!\n", hwi_block);
		return CB_ERR;
	}

	/* Get EDID data from hwinfo block */
	if (hwilib_get_field(Edid, edid_data, PTN_EDID_LEN) != PTN_EDID_LEN) {
		printk(BIOS_ERR, "LCD: No EDID data available in %s\n", hwi_block);
		return CB_ERR;
	}
	return CB_SUCCESS;
}

/** \brief This function provides EDID block [0..6] to the driver for DP2LVDS Bridge (PTN3460)
 * which has to be used.
*/
uint8_t mb_select_edid_table(void)
{
	return 6; /* With this mainboard we use EDID block 6 for emulation in PTN3460. */
}

/** \brief Function to enable mainboard to adjust the config data of PTN3460.
 * @param   *cfg_ptr  Pointer to the PTN config structure to modify.
 * @return  -1 on error; PTN_CFG_MODIFIED if data was modified and needs to be updated.
*/
int mb_adjust_cfg(struct ptn_3460_config *cfg)
{
	const char *hwi_block = "hwinfo.hex";
	uint8_t disp_con = 0, color_depth = 0;

	if (hwilib_find_blocks(hwi_block) != CB_SUCCESS) {
		printk(BIOS_ERR, "LCD: Info block \"%s\" not found!\n", hwi_block);
		return -1;
	}

	if (hwilib_get_field(PF_DisplCon, &disp_con, sizeof(disp_con)) != sizeof(disp_con)) {
		printk(BIOS_ERR, "LCD: Missing panel features from %s\n", hwi_block);
		return -1;
	}
	if (hwilib_get_field(PF_Color_Depth, &color_depth,
			     sizeof(color_depth)) != sizeof(color_depth)) {
		printk(BIOS_ERR, "LCD: Missing panel features from %s\n", hwi_block);
		return -1;
	}
	/* Set up configuration data according to the hwinfo block we got. */
	cfg->dp_interface_ctrl = 0x00;
	/* Use odd-bus for clock distribution only. */
	cfg->lvds_interface_ctrl1 = 0x01;
	if (disp_con == PF_DISPLCON_LVDS_DUAL) {
		/* Turn on dual LVDS lane and clock. */
		cfg->lvds_interface_ctrl1 |= 0x0b;
	}
	if (color_depth == PF_COLOR_DEPTH_6BIT) {
		/* Use 18 bits per pixel. */
		cfg->lvds_interface_ctrl1 |= 0x20;
	}
	/* No clock spreading, 300 mV LVDS swing. */
	cfg->lvds_interface_ctrl2 = 0x03;
	/* Swap LVDS lanes (N vs. P). */
	cfg->lvds_interface_ctrl3 = 0x04;
	/* Delay T2 (VDD to LVDS active) by 16 ms. */
	cfg->t2_delay = 1;
	/* 500 ms from LVDS to backlight active. */
	cfg->t3_timing = 10;
	/* 1 second re-power delay. */
	cfg->t12_timing = 20;
	/* 150 ms backlight off to LVDS inactive. */
	cfg->t4_timing = 3;
	/* Delay T5 (LVDS to VDD inactive) by 16 ms. */
	cfg->t5_delay = 1;
	/* Enable backlight control. */
	cfg->backlight_ctrl = 0;

	return PTN_CFG_MODIFIED;
}
