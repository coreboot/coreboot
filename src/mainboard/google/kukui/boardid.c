/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * The boardid.c should provide board_id, sku_id, and ram_code.
 * board_id is provided by ec/google/chromeec/ec_boardid.c.
 * sku_id and ram_code are defined in this file.
 */

#include <assert.h>
#include <boardid.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <drivers/camera/cros_camera.h>
#include <ec/google/chromeec/ec.h>
#include <soc/auxadc_common.h>
#include <soc/i2c.h>
#include <soc/pmic_wrap_common.h>
#include <string.h>

/* For CBI un-provisioned/corrupted Flapjack board. */
#define FLAPJACK_UNDEF_SKU_ID 0

#define ADC_LEVELS 12

enum {
	LCM_ID_CHANNEL = 2,  /* ID of LCD Module on schematics. */
	RAM_ID_CHANNEL = 3,
	SKU_ID_CHANNEL = 4,
};

static const int ram_voltages[ADC_LEVELS] = {
	/* ID : Voltage (unit: uV) */
	/*  0 : */   74000,
	/*  1 : */  212000,
	/*  2 : */  319000,
	/*  3 : */  429000,
	/*  4 : */  542000,
	/*  5 : */  666000,
	/*  6 : */  781000,
	/*  7 : */  900000,
	/*  8 : */ 1023000,
	/*  9 : */ 1137000,
	/* 10 : */ 1240000,
	/* 11 : */ 1343000,
};

static const int lcm_voltages[ADC_LEVELS] = {
	/* ID : Voltage (unit: uV) */
	/*  0 : */       0,
	/*  1 : */  283000,
	/*  2 : */  394000,
	/*  3 : */  503000,
	/*  4 : */  608000,
	/*  5 : */  712000,
	/*  6 : */  823000,
	/*  7 : */  937000,
	/*  8 : */ 1046000,
	/*  9 : */ 1155000,
	/* 10 : */ 1277000,
	/* 11 : */ 1434000,
};

static const int *adc_voltages[] = {
	[LCM_ID_CHANNEL] = lcm_voltages,
	[RAM_ID_CHANNEL] = ram_voltages,
	[SKU_ID_CHANNEL] = ram_voltages, /* SKU ID is sharing RAM voltages. */
};

static uint32_t get_adc_index(unsigned int channel)
{
	int value = auxadc_get_voltage_uv(channel);

	assert(channel < ARRAY_SIZE(adc_voltages));
	const int *voltages = adc_voltages[channel];
	assert(voltages);

	/* Find the closest voltage */
	uint32_t id;
	for (id = 0; id < ADC_LEVELS - 1; id++)
		if (value < (voltages[id] + voltages[id + 1]) / 2)
			break;
	printk(BIOS_DEBUG, "ADC[%d]: Raw value=%d ID=%d\n", channel, value, id);
	return id;
}

static uint8_t eeprom_random_read(uint8_t bus, uint8_t slave, uint16_t offset,
				  uint8_t *data, uint16_t len)
{
	struct i2c_msg seg[2];
	uint8_t address[2];

	address[0] = offset >> 8;
	address[1] = offset & 0xff;

	seg[0].flags = 0;
	seg[0].slave = slave;
	seg[0].buf   = address;
	seg[0].len   = sizeof(address);
	seg[1].flags = I2C_M_RD;
	seg[1].slave = slave;
	seg[1].buf   = data;
	seg[1].len   = len;

	return i2c_transfer(bus, seg, ARRAY_SIZE(seg));
}

/* Regulator for world facing camera. */
#define PMIC_LDO_VCAMIO_CON0 0x1cb0

#define CROS_CAMERA_INFO_OFFSET 0x1f80
#define MT8183_FORMAT 0x8183
#define KODAMA_PID 0x00c7

/* Returns the ID for world facing camera. */
static uint8_t wfc_id(void)
{
	if (!CONFIG(BOARD_GOOGLE_KODAMA))
		return 0;

	int i, ret;
	uint8_t bus = 2;
	uint8_t dev_addr = 0x50;  /* at24c32/64 device address */

	struct cros_camera_info data = {0};

	const uint16_t sensor_pids[] = {
		[0] = 0x5965,  /* OV5965 */
		[1] = 0x5035,  /* GC5035 */
	};

	mtk_i2c_bus_init(bus);

	/* Turn on camera sensor EEPROM */
	pwrap_write(PMIC_LDO_VCAMIO_CON0, 0x1);
	udelay(270);

	ret = eeprom_random_read(bus, dev_addr, CROS_CAMERA_INFO_OFFSET,
				 (uint8_t *)&data, sizeof(data));
	pwrap_write(PMIC_LDO_VCAMIO_CON0, 0x0);

	if (ret) {
		printk(BIOS_ERR,
		       "Failed to read from EEPROM; using default WFC id 0\n");
		return 0;
	}

	if (check_cros_camera_info(&data)) {
		printk(BIOS_ERR,
		       "Failed to check camera info; using default WFC id 0\n");
		return 0;
	}

	if (data.data_format != MT8183_FORMAT) {
		printk(BIOS_ERR, "Incompatible camera format: %#04x\n",
		       data.data_format);
		return 0;
	}
	if (data.module_pid != KODAMA_PID) {
		printk(BIOS_ERR, "Incompatible module pid: %#04x\n",
		       data.module_pid);
		return 0;
	}

	printk(BIOS_DEBUG, "Camera sensor pid: %#04x\n", data.sensor_pid);

	for (i = 0; i < ARRAY_SIZE(sensor_pids); i++) {
		if (data.sensor_pid == sensor_pids[i]) {
			printk(BIOS_INFO, "Detected WFC id: %d\n", i);
			return i;
		}
	}

	printk(BIOS_WARNING, "Unknown WFC id; using default id 0\n");
	return 0;
}

/* Returns the ID for LCD module (type of panel). */
static uint8_t lcm_id(void)
{
	/* LCM is unused on Jacuzzi followers. */
	if (CONFIG(BOARD_GOOGLE_JACUZZI_COMMON))
		return CONFIG_BOARD_OVERRIDE_LCM_ID;

	return get_adc_index(LCM_ID_CHANNEL);
}

uint32_t sku_id(void)
{
	static uint32_t cached_sku_id = BOARD_ID_INIT;

	if (cached_sku_id != BOARD_ID_INIT)
		return cached_sku_id;

	/* On Flapjack, getting the SKU via CBI. */
	if (CONFIG(BOARD_GOOGLE_FLAPJACK)) {
		if (google_chromeec_cbi_get_sku_id(&cached_sku_id))
			cached_sku_id = FLAPJACK_UNDEF_SKU_ID;
		return cached_sku_id;
	}

	/* Quirk for Kukui: All Rev1/Sku0 had incorrectly set SKU_ID=1. */
	if (CONFIG(BOARD_GOOGLE_KUKUI)) {
		if (board_id() == 1) {
			cached_sku_id = 0;
			return cached_sku_id;
		}
	}

	/*
	 * The SKU (later used for device tree matching) is combined from:
	 * World facing camera (WFC) ID.
	 * ADC2[4bit/H] = straps on LCD module (type of panel).
	 * ADC4[4bit/L] = SKU ID from board straps.
	 */
	cached_sku_id = (wfc_id() << 8 |
			 lcm_id() << 4 |
			 get_adc_index(SKU_ID_CHANNEL));

	return cached_sku_id;
}

uint32_t ram_code(void)
{
	static uint32_t cached_ram_code = BOARD_ID_INIT;

	if (cached_ram_code == BOARD_ID_INIT) {
		cached_ram_code = get_adc_index(RAM_ID_CHANNEL);
		/* Model-specific offset - see sdram_configs.c for details. */
		cached_ram_code += CONFIG_BOARD_SDRAM_TABLE_OFFSET;
	}
	return cached_ram_code;
}
