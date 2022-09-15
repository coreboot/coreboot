/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/i2c_bus.h>
#include <types.h>
#include <bootstate.h>

#include "ptn3460.h"

/**
 * \brief  This function selects one of 7 EDID-tables inside PTN3460
 *         which should be emulated on display port and turn emulation ON
 * @param  *dev		Pointer to the relevant I2C controller
 * @param  edid_num	Number of EDID to emulate (0..6)
 * @return PTN_SUCCESS or error code
 */
static int ptn_select_edid(struct device *dev, uint8_t edid_num)
{
	int status = 0;
	u8 val;

	if (edid_num > PTN_MAX_EDID_NUM)
		return PTN_INVALID_EDID;
	val = (edid_num << 1) | PTN_ENABLE_EMULATION;
	status = i2c_dev_writeb_at(dev, PTN_CONFIG_OFF + 4, val);
	return status ? (PTN_BUS_ERROR | status) : PTN_SUCCESS;
}

/**
 * \brief This function writes one EDID data structure to PTN3460
 * @param  *dev      Pointer to the relevant I2C controller
 * @param  edid_num  Number of EDID that must be written (0..6)
 * @param  *data     Pointer to a buffer where data to write is stored in
 * @return           PTN_SUCCESS on success or error code
 */
static int ptn3460_write_edid(struct device *dev, u8 edid_num, u8 *data)
{
	int status;
	int i;

	if (edid_num > PTN_MAX_EDID_NUM)
		return PTN_INVALID_EDID;

	/* First enable access to the desired EDID table */
	status = i2c_dev_writeb_at(dev, PTN_CONFIG_OFF + 5, edid_num);
	if (status)
		return (PTN_BUS_ERROR | status);

	/* Now we can simply write EDID data to ptn3460 */
	for (i = 0; i < PTN_EDID_LEN; i++) {
		status = i2c_dev_writeb_at(dev, PTN_EDID_OFF + i, data[i]);
		if (status)
			return (PTN_BUS_ERROR | status);
	}
	return PTN_SUCCESS;
}

/**
 * \brief This function sets up the DP2LVDS-converter to be used with the
 *         appropriate EDID data
 * @param  *dev	Pointer to the I2C controller where PTN3460 is attached
 */
static void ptn3460_init(struct device *dev)
{
	struct ptn_3460_config cfg;
	uint8_t edid_data[PTN_EDID_LEN], edid_tab, *ptr = (uint8_t *) &cfg;
	int i, val;

	/* Guard against re-initialization of the device */
	static bool init_done = false;

	if (init_done) {
		printk(BIOS_DEBUG, "Skipping PTN3460 init as it's already initialized\n");
		return;
	}

	/* Mainboard provides EDID data. */
	if (mb_get_edid(edid_data) != CB_SUCCESS) {
		printk(BIOS_ERR, "PTN3460 error: Unable to get EDID data from mainboard.\n");
		return;
	}

	/* Mainboard decides which EDID table has to be used. */
	edid_tab = mb_select_edid_table();
	if (edid_tab > PTN_MAX_EDID_NUM) {
		printk(BIOS_ERR, "PTN3460 error: invalid EDID table (%d) selected.\n",
		       edid_tab);
		return;
	}
	/* Write EDID data into PTN. */
	val = ptn3460_write_edid(dev, edid_tab, edid_data);
	if (val != PTN_SUCCESS) {
		printk(BIOS_ERR, "PTN3460 error: writing EDID data into device failed.\n");
		return;
	}
	/* Activate the selected EDID block. */
	ptn_select_edid(dev, edid_tab);
	/* Read out PTN configuration data. */
	for (i = 0; i < sizeof(struct ptn_3460_config); i++) {
		val = i2c_dev_readb_at(dev, PTN_CONFIG_OFF + i);
		if (val < 0) {
			printk(BIOS_ERR,
			       "PTN3460 error: Unable to read config data from device.\n");
			return;
		}
		*ptr++ = (uint8_t)val; /* fill config structure via ptr */
	}
	/* Mainboard can modify the configuration data.
	   Write back configuration data to PTN3460 if modified by mainboard */
	if (mb_adjust_cfg(&cfg) == PTN_CFG_MODIFIED) {
		ptr = (uint8_t *) &cfg;
		for (i = 0; i < sizeof(struct ptn_3460_config); i++) {
			val = i2c_dev_writeb_at(dev, PTN_CONFIG_OFF + i, *ptr++);
			if (val < 0) {
				printk(BIOS_ERR,
				       "PTN3460 error: Unable to write config data.\n");
				return;
			}
		}
	}

	init_done = true;
}

__weak enum cb_err mb_get_edid(uint8_t edid_data[0x80])
{
	return CB_ERR;
}
__weak uint8_t mb_select_edid_table(void)
{
	return 0;
}
__weak int mb_adjust_cfg(struct ptn_3460_config *cfg_ptr)
{
	return 0;
}

static struct device_operations ptn3460_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.init			= ptn3460_init,
};

static void ptn3460_enable(struct device *dev)
{
	dev->ops = &ptn3460_ops;
}

struct chip_operations drivers_i2c_ptn3460_ops = {
	CHIP_NAME("PTN3460")
	.enable_dev = ptn3460_enable
};

#if CONFIG(PTN3460_EARLY_INIT)

/**
 * \brief This function provides a callback for the boot state machine to initialize the
 *        PTN3460 DP-to-LVDS bridge before graphics initialization in order for the bootsplash
 *        logo to be shown.
 * @param  *unused	Unused argument for the callback.
 */

static void ptn3460_early_init(void *unused)
{
	struct device *ptn_dev;

	printk(BIOS_DEBUG, "Attempting PTN3460 early init.\n");
	ptn_dev = dev_find_slot_on_smbus(0, CONFIG_PTN3460_EARLY_ADDR);
	if (!ptn_dev) {
		printk(BIOS_ERR, "Failed to find the PTN3460 device!\n");
		return;
	}

	ptn3460_init(ptn_dev);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, ptn3460_early_init, NULL);

#endif /* CONFIG(PTN3460_EARLY_INIT) */
