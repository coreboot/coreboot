/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __INTEL_MIPI_CAMERA_CHIP_H__
#define __INTEL_MIPI_CAMERA_CHIP_H__

#include <stdint.h>
#include <acpi/acpi_pld.h>
#include <uuid.h>

#define DEFAULT_LINK_FREQ	450000000
#define MAX_PWDB_ENTRIES	12
#define MAX_PORT_ENTRIES	4
#define MAX_LINK_FREQ_ENTRIES	4
#define MAX_CLK_CONFIGS		2
#define MAX_GPIO_CONFIGS	4
#define MAX_PWR_OPS		6
#define MAX_GUARDED_RESOURCES	10
#define IMGCLKOUT_0		0
#define IMGCLKOUT_1		1
#define IMGCLKOUT_2		2
#define IMGCLKOUT_3		3
#define IMGCLKOUT_4		4
#define IMGCLKOUT_5		5
#define FREQ_24_MHZ		0
#define FREQ_19_2_MHZ		1

#define SEQ_OPS_CLK_ENABLE(ind, delay) \
	{ .type = IMGCLK, .index = (ind), .action = ENABLE, .delay_ms = (delay) }
#define SEQ_OPS_CLK_DISABLE(ind, delay) \
	{ .type = IMGCLK, .index = (ind), .action = DISABLE, .delay_ms = (delay) }
#define SEQ_OPS_GPIO_ENABLE(ind, delay) \
	{ .type = GPIO, .index = (ind), .action = ENABLE, .delay_ms = (delay) }
#define SEQ_OPS_GPIO_DISABLE(ind, delay) \
	{ .type = GPIO, .index = (ind), .action = DISABLE, .delay_ms = (delay) }

enum camera_device_type {
	DEV_TYPE_SENSOR = 0,
	DEV_TYPE_VCM,
	DEV_TYPE_ROM
};

enum intel_camera_platform_type {
	PLATFORM_BXT = 8,
	PLATFORM_SKC = 9,
	PLATFORM_CNL = 10,
	PLATFORM_TGL = 12,
	PLATFORM_ADL = 15,
	PLATFORM_MTL = 16,
	PLATFORM_LNL = 17
};

enum intel_camera_flash_type {
	FLASH_DEFAULT = 0,
	FLASH_DISABLE = 2,
	FLASH_ENABLE = 3
};

enum intel_camera_led_type {
	PRIVACY_LED_DEFAULT = 0,
	PRIVACY_LED_A_16mA
};

enum intel_camera_mipi_info {
	MIPI_INFO_SENSOR_DRIVER = 0,
	MIPI_INFO_ACPI_DEFINED
};

#define CLK_FREQ_19_2MHZ	19200000
#define CLK_FREQ_24MHZ		24000000
#define CLK_FREQ_20MHZ		20000000

enum intel_camera_device_type {
	INTEL_ACPI_CAMERA_CIO2,
	INTEL_ACPI_CAMERA_IMGU,
	INTEL_ACPI_CAMERA_SENSOR,
	INTEL_ACPI_CAMERA_VCM,
	INTEL_ACPI_CAMERA_NVM,
	INTEL_ACPI_CAMERA_PMIC = 100,
};

enum intel_power_action_type {
	INTEL_ACPI_CAMERA_REGULATOR,
	INTEL_ACPI_CAMERA_CLK,
	INTEL_ACPI_CAMERA_GPIO,
};

enum ctrl_type {
	UNKNOWN_CTRL,
	IMGCLK,
	GPIO
};

enum action_type {
	UNKNOWN_ACTION,
	ENABLE,
	DISABLE
};

struct camera_resource {
	uint8_t type;
	uint8_t id;
};

struct camera_resource_manager {
	uint8_t cnt;
	struct camera_resource resource[MAX_GUARDED_RESOURCES];
};

struct resource_config {
	enum action_type action;
	enum ctrl_type type;
	union {
		const struct clk_config *clk_conf;
		const struct gpio_config *gpio_conf;
	};
};

struct clk_config {
	/* IMGCLKOUT_x being used for a port */
	uint8_t clknum;
	/* frequency setting: 0:24MHz, 1:19.2 MHz */
	uint8_t freq;
};

struct gpio_config {
	uint16_t gpio_num;
};

struct clock_ctrl_panel {
	struct clk_config clks[MAX_CLK_CONFIGS];
};

struct gpio_ctrl_panel {
	struct gpio_config gpio[MAX_GPIO_CONFIGS];
};

struct operation_type {
	enum ctrl_type type;
	uint8_t index;
	enum action_type action;
	uint32_t delay_ms;
};

struct operation_seq {
	struct operation_type ops[MAX_PWR_OPS];
	uint8_t ops_cnt;
};

struct intel_ssdb {
	uint8_t version;			/* Current version */
	uint8_t sensor_card_sku;		/* CRD Board type */
	guid_t csi2_data_stream_interface;	/* CSI2 data stream GUID */
	uint16_t bdf_value;			/* Bus number of the host
						controller */
	uint32_t dphy_link_en_fuses;		/* Host controller's fuses
						information used to verify if
						link is fused out or not */
	uint32_t lanes_clock_division;		/* Lanes/clock divisions per
						sensor */
	uint8_t link_used;			/* Link used by this sensor
						stream */
	uint8_t lanes_used;			/* Number of lanes connected for
						the sensor */
	uint32_t csi_rx_dly_cnt_termen_clane;	/* MIPI timing information */
	uint32_t csi_rx_dly_cnt_settle_clane;	/* MIPI timing information */
	uint32_t csi_rx_dly_cnt_termen_dlane0;	/* MIPI timing information */
	uint32_t csi_rx_dly_cnt_settle_dlane0;	/* MIPI timing information */
	uint32_t csi_rx_dly_cnt_termen_dlane1;	/* MIPI timing information */
	uint32_t csi_rx_dly_cnt_settle_dlane1;	/* MIPI timing information */
	uint32_t csi_rx_dly_cnt_termen_dlane2;	/* MIPI timing information */
	uint32_t csi_rx_dly_cnt_settle_dlane2;	/* MIPI timing information */
	uint32_t csi_rx_dly_cnt_termen_dlane3;	/* MIPI timing information */
	uint32_t csi_rx_dly_cnt_settle_dlane3;	/* MIPI timing information */
	uint32_t max_lane_speed;		/* Maximum lane speed for
						the sensor */
	uint8_t sensor_cal_file_idx;		/* Legacy field for sensor
						calibration file index */
	uint8_t sensor_cal_file_idx_mbz[3];	/* Legacy field for sensor
						calibration file index */
	uint8_t rom_type;			/* NVM type of the camera
						module */
	uint8_t vcm_type;			/* VCM type of the camera
						module */
	uint8_t platform;			/* Platform information */
	uint8_t platform_sub;			/* Platform sub-categories */
	uint8_t flash_support;			/* Enable/disable flash
						support */
	uint8_t privacy_led;			/* Privacy LED support */
	uint8_t degree;				/* Camera Orientation */
	uint8_t mipi_define;			/* MIPI info defined in ACPI or
						sensor driver */
	uint32_t mclk_speed;			/* Clock info for sensor */
	uint32_t mclk;				/* Clock info for sensor */
	uint8_t control_logic_id;		/* PMIC device node used for
						the camera sensor */
	uint8_t mipi_data_format;		/* MIPI data format */
	uint8_t silicon_version;		/* Silicon version */
	uint8_t customer_id;			/* Customer ID */
	uint8_t mclk_port;
	uint8_t reserved[13];			/* Pads SSDB out so the binary blob in ACPI is
						   the same size as seen on other firmwares.*/
} __packed;

struct intel_pwdb {
	char name[32];		/* Name of the resource required by the power
				action */
	uint32_t value;		/* The value to be set for the power action */
	uint32_t entry_type;	/* The type of the current power action */
	uint32_t delay_usec;	/* The delay time after which power action is
				performed and this is in unit of usec */
} __packed;

struct drivers_intel_mipi_camera_config {
	struct clock_ctrl_panel clk_panel;
	struct gpio_ctrl_panel gpio_panel;
	struct operation_seq on_seq;
	struct operation_seq off_seq;

	struct intel_ssdb ssdb;
	struct intel_pwdb pwdb[MAX_PWDB_ENTRIES];
	enum intel_camera_device_type device_type;
	uint8_t num_pwdb_entries;
	const char *acpi_hid;
	const char *acpi_name;
	const char *chip_name;
	unsigned int acpi_uid;
	const char *pr0;

	/* Settings specific to CIO2 device */
	uint32_t cio2_num_ports;
	uint32_t cio2_lanes_used[MAX_PORT_ENTRIES];
	const char *cio2_lane_endpoint[MAX_PORT_ENTRIES];
	uint32_t cio2_prt[MAX_PORT_ENTRIES];

	/* Settings specific to camera sensor */
	bool disable_ssdb_defaults;

	uint8_t num_freq_entries;	/* # of elements in link_freq */
	uint32_t link_freq[MAX_LINK_FREQ_ENTRIES];
	const char *sensor_name;	/* default "UNKNOWN" */
	const char *remote_name;	/* default "\_SB.PCI0.CIO2" */
	const char *vcm_name;		/* defaults to |vcm_address| device */
	bool use_pld;
	bool disable_pld_defaults;
	struct acpi_pld pld;
	uint16_t rom_address;		/* I2C to use if ssdb.rom_type != 0 */
	uint16_t vcm_address;		/* I2C to use if ssdb.vcm_type != 0 */
	/*
	 * Settings specific to nvram. Many values, if left as zero, will be assigned a default.
	 * Set disable_nvm_defaults to non-zero if you want to disable the defaulting behavior
	 * so you can use zero for a value.
	 */
	bool disable_nvm_defaults;
	uint32_t nvm_size;
	uint32_t nvm_pagesize;
	uint32_t nvm_readonly;
	uint32_t nvm_width;
	const char *nvm_compat;

	/* Settings specific to vcm */
	const char *vcm_compat;
	/* Does the device have a power resource entries */
	bool has_power_resource;
	/* Perform low power probe */
	bool low_power_probe;
	/*
	 * This will create a _DSC method in ACPI which returns an integer, to tell the kernel
	 * the highest allowed D state for a device during probe
	 * Number   State   Description
	 * 0	    D0	    Device fully powered on
	 * 1	    D1
	 * 2	    D2
	 * 3	    D3hot
	 * 4	    D3cold  Off
	 */
	uint8_t max_dstate_for_probe;
};

#endif
