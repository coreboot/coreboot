/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <drivers/i2c/ptn3460/ptn3460.h>
#include <hwilib.h>
#include <types.h>

/** \brief This function provides EDID data to the driver for DP2LVDS Bridge (PTN3460).
 * @param  edid_data  pointer to EDID data in driver
 * @return CB_SUCCESS on successful EDID data retrieval, CB_ERR otherwise
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
 * @return Index of the EDID slot selected for EDID emulation
 */
uint8_t mb_select_edid_table(void)
{
	return 6; /* With this mainboard we use EDID block 6 for emulation in PTN3460. */
}

/** \brief Function to enable mainboard to adjust the config data of PTN3460. For reference,
 * see NXP document AN11128 - PTN3460 Programming guide.
 * @param   *cfg_ptr  Pointer to the PTN config structure to modify
 * @return  -1 on error; PTN_CFG_MODIFIED if data was modified and needs to be updated.
 */
int mb_adjust_cfg(struct ptn_3460_config *cfg)
{
	const char *hwi_block = "hwinfo.hex";
	uint8_t disp_con = 0, color_depth = 0;

	/* Get display-specific configuration from hwinfo. */
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

	/* Set up PTN3460 registers based on hwinfo and fixed board-specific parameters: */
	/* Use 2 lanes for eDP, no P/N swapping, no ASSR, allow both HBR and RBR modes. */
	cfg->dp_interface_ctrl = 0x00;
	/* Use odd bus for LVDS clock distribution only. */
	cfg->lvds_interface_ctrl1 = 0x01;
	if (disp_con == PF_DISPLCON_LVDS_DUAL) {
		/* Turn on dual LVDS lane and clock. */
		cfg->lvds_interface_ctrl1 |= 0x0b;
	}
	if (color_depth == PF_COLOR_DEPTH_6BIT) {
		/* Use 18 bits per pixel. */
		cfg->lvds_interface_ctrl1 |= 0x20;
	}
	/* No LVDS clock spreading, 300 mV LVDS swing */
	cfg->lvds_interface_ctrl2 = 0x03;
	/* No LVDS lane/channel swapping */
	cfg->lvds_interface_ctrl3 = 0x00;
	/* Enable VDD to LVDS active delay. */
	cfg->t2_delay = 0x01;
	/* LVDS to backlight active delay: 200 ms */
	cfg->t3_timing = 0x04;
	/* Minimum re-power delay: 500 ms */
	cfg->t12_timing = 0x0a;
	/* Backlight off to LVDS inactive delay: 200 ms */
	cfg->t4_timing = 0x04;
	/* Enable LVDS to VDD inactive delay. */
	cfg->t5_delay = 0x01;
	/* Enable backlight control. */
	cfg->backlight_ctrl = 0x00;

	return PTN_CFG_MODIFIED;
}
