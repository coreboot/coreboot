/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __INTEL_MIPI_CAMERA_SSDB_H__
#define __INTEL_MIPI_CAMERA_SSDB_H__

#include <stdint.h>
#include <uuid.h>

#define SSDB_SIZE 108

enum camera_device_type {
	DEV_TYPE_SENSOR = 0,
	DEV_TYPE_VCM,
	DEV_TYPE_ROM
};

enum platform_type {
	PLAT_UNK,
	PLAT_BYT,
	PLAT_CHT,
	PLAT_CHT1,
	PLAT_CHT2,
	PLAT_CHT_CR,
	PLAT_CHT_COPOP,
	PLAT_BSW,
	PLAT_BXT,
	PLAT_SKC,
	PLAT_CNL,
	PLAT_LKF,
	PLAT_TGL,
	PLAT_TGL_H,
	PLAT_JSL,
	PLAT_ADL,
	PLAT_MTL,
	PLAT_LNL,
	PLAT_PTL,
	PLAT_NVL,
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

struct intel_ssdb {
	uint8_t version;			/* Current version */
	uint8_t sensor_card_sku;		/* CRD Board type */
	guid_t csi2_data_stream_interface;	/* CSI2 data stream GUID */
	uint16_t bdf_value;			/* Bus number of the host controller */
	uint32_t dphy_link_en_fuses;		/* Host controller's fuses information used to
						   verify if link is fused out or not */
	uint32_t lanes_clock_division;		/* Lanes/clock divisions per sensor */
	uint8_t link_used;			/* Link used by this sensor stream */
	uint8_t lanes_used;			/* Number of lanes connected for the sensor */
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
	uint32_t max_lane_speed;		/* Maximum lane speed for the sensor */
	uint8_t sensor_cal_file_idx;		/* Legacy field for sensor calibration file index */
	uint8_t sensor_cal_file_idx_mbz[3];	/* Legacy field for sensor calibration file index */
	uint8_t rom_type;			/* NVM type of the camera module */
	uint8_t vcm_type;			/* VCM type of the camera module */
	uint8_t platform;			/* Platform information */
	uint8_t platform_sub;			/* Platform sub-categories */
	uint8_t flash_support;			/* Enable/disable flash support */
	uint8_t privacy_led;			/* Privacy LED support */
	uint8_t degree;				/* Camera Orientation */
	uint8_t mipi_define;			/* MIPI info defined in ACPI or sensor driver */
	uint32_t mclk_speed;			/* Clock info for sensor */
	uint8_t control_logic_id;		/* PMIC device node used for the camera sensor */
	uint8_t mipi_data_format;		/* MIPI data format */
	uint8_t silicon_version;		/* Silicon version */
	uint8_t customer_id;			/* Customer ID */
	uint8_t mclk_port;
	uint8_t reserved[13];			/* Pads SSDB out so the binary blob in ACPI is
						   the same size as seen on other firmwares.*/
} __packed;
_Static_assert(sizeof(struct intel_ssdb) == SSDB_SIZE,
		"SSDB structure size must match SSDB_SIZE");

#endif
