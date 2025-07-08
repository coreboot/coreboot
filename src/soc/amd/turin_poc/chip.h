/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __TURIN_POC_CHIP_H__
#define __TURIN_POC_CHIP_H__

#include <amdblocks/chip.h>
#include <amdblocks/i2c.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <soc/iomap.h>
#include <types.h>

struct usb31_phy_settings {
	uint8_t rx_ana_iq_phase_adjust;
	uint8_t rx_eq_delta_iq_ovrd_en;
	uint8_t rx_eq_delta_iq_ovrd_val;
	uint8_t rx_iq_phase_adjust;
	uint8_t tx_vboost_lvl_en;
	uint8_t tx_vboost_lvl;
	uint8_t rx_vref_ctrl_en;
	uint8_t rx_vref_ctrl;
	uint8_t tx_vboost_lvl_en_x;
	uint8_t tx_vboost_lvl_x;
	uint8_t rx_vref_ctrl_en_x;
	uint8_t rx_vref_ctrl_x;
};

struct soc_usb_config {
	uint8_t xhci0_enable : 1;
	uint8_t xhci1_enable : 1;
	struct {
		uint8_t port0 : 4;
		uint8_t port1 : 4;
		uint8_t port2 : 4;
		uint8_t port3 : 4;
		uint8_t port4 : 4;
		uint8_t port5 : 4;
		uint8_t port6 : 4;
		uint8_t port7 : 4;
	} usb2_oc_pins[2];
	struct {
		uint8_t port0 : 4;
		uint8_t port1 : 4;
		uint8_t port2 : 4;
		uint8_t port3 : 4;
	}  usb3_oc_pins[2];
	bool polarity_cfg_low;
	union {
		struct {
			uint8_t port0 : 2;
			uint8_t port1 : 2;
			uint8_t port2 : 2; /* Broken in OpenSIL */
			uint8_t port3 : 2; /* Broken in OpenSIL */
		};
		uint8_t raw;
	} usb3_force_gen1;
	bool usb31_phy_enable;
	struct usb31_phy_settings usb31_phy[8];
	bool s1_usb31_phy_enable;
	struct usb31_phy_settings s1_usb31_phy[8];
};

struct soc_amd_turin_poc_config {
	struct soc_amd_common_config common_config;

	u8 i2c_scl_reset;
	struct dw_i2c_bus_config i2c[I2C_CTRLR_COUNT];

	struct soc_usb_config usb;
};

#endif /* __TURIN_POC_CHIP_H__ */
