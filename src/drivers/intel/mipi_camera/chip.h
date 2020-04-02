/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __INTEL_MIPI_CAMERA_CHIP_H__
#define __INTEL_MIPI_CAMERA_CHIP_H__

#include <stdint.h>

#define MAX_PWDB_ENTRIES 12

enum intel_camera_device_type {
	INTEL_ACPI_CAMERA_CIO2,
	INTEL_ACPI_CAMERA_IMGU,
	INTEL_ACPI_CAMERA_SENSOR,
	INTEL_ACPI_CAMERA_VCM,
	INTEL_ACPI_CAMERA_PMIC = 100,
};

enum intel_power_action_type {
	INTEL_ACPI_CAMERA_REGULATOR,
	INTEL_ACPI_CAMERA_CLK,
	INTEL_ACPI_CAMERA_GPIO,
};

struct intel_ssdb {
	uint8_t version;			/* Current version */
	uint8_t sensor_card_sku;		/* CRD Board type */
	uint8_t csi2_data_stream_interface[16];	/* CSI2 data stream GUID */
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
	uint32_t mclk;				/* Clock info for sensor */
	uint8_t control_logic_id;		/* PMIC device node used for
						the camera sensor */
	uint8_t mipi_data_format;		/* MIPI data format */
	uint8_t silicon_version;		/* Silicon version */
	uint8_t customer_id;			/* Customer ID */
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
	struct intel_ssdb ssdb;
	struct intel_pwdb pwdb[MAX_PWDB_ENTRIES];
	enum intel_camera_device_type device_type;
	uint8_t num_pwdb_entries;
	const char *acpi_hid;
	const char *acpi_name;
	const char *chip_name;
	unsigned int acpi_uid;
};

#endif
