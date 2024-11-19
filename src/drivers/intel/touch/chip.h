/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_INTEL_TOUCH_CHIP_H__
#define __DRIVERS_INTEL_TOUCH_CHIP_H__

#include <acpi/acpi_device.h>

#define INTEL_THC0_NAME "THC0"
#define INTEL_THC1_NAME "THC1"

struct intel_thc_hidi2c_dev_info {
	/* Device I2C address */
	uint64_t addr;
	/* Used in THC_HID_I2C mode */
	uint32_t descriptor_address;
};

struct intel_thc_hidspi_dev_info {
	/* Touch Host Controller HID Over SPI Input Report Header Address */
	uint32_t input_report_header_address;

	/* Touch Host Controller HID Over SPI Input Report Body Address */
	uint32_t input_report_body_address;

	/* Touch Host Controller HID Over SPI Output Report Address */
	uint32_t output_report_address;

	/* Touch Host Controller HID Over SPI Read Opcode */
	uint32_t read_opcode;

	/* Touch Host Controller HID Over SPI Write Opcode */
	uint32_t write_opcode;

};

union intel_thc_dev_intf_info {
	struct intel_thc_hidi2c_dev_info hidi2c;
	struct intel_thc_hidspi_dev_info hidspi;
};

struct intel_thc_dev_info {
	/* Device ACPI _HID */
	const char *hid;
	/* Device ACPI _CID */
	const char *cid;
	union intel_thc_dev_intf_info intf;
};

struct intel_thc_hidi2c_info {
	/* Device connection speed in Hz */
	uint64_t connection_speed;
	/* Device address mode */
	uint64_t addr_mode;
	/* Standard Mode (100 kbit/s) Serial Clock Line HIGH Period */
	/* NOTE: unit for period: # of reference clock count */
	uint64_t sm_scl_high_period;
	/* Standard Mode (100 kbit/s) Serial Clock Line LOW Period */
	uint64_t sm_scl_low_period;
	/* Standard Mode (100 kbit/s) Serial Data Line Transmit Hold Period */
	uint64_t sm_sda_hold_tx_period;
	/* Standard Mode (100 kbit/s) Serial Data Receive Hold Period */
	uint64_t sm_sda_hold_rx_period;
	/* Fast Mode (400 kbit/s) Serial Clock Line HIGH Period */
	uint64_t fm_scl_high_period;
	/* Fast Mode (400 kbit/s) Serial Clock Line LOW Period */
	uint64_t fm_scl_low_period;
	/* Fast Mode (400 kbit/s) Serial Data Line Transmit Hold Period */
	uint64_t fm_sda_hold_tx_period;
	/* Fast Mode (400 kbit/s) Serial Data Line Receive Hold Period */
	uint64_t fm_sda_hold_rx_period;
	/*
	 * Maximum length (in ic_clk_cycles) of suppressed spikes in Std Mode, Fast Mode, and
	 * Fast Mode Plus.
	 */
	uint64_t suppressed_spikes_s_f_fp;
	/* Fast Mode Plus (1Mbit/sec) Serial Clock Line HIGH Period */
	uint64_t fmp_scl_high_period;
	/* Fast Mode Plus (1Mbit/sec) Serial Clock Line LOW Period */
	uint64_t fmp_scl_low_period;
	/* Fast Mode Plus (1Mbit/sec) Serial Data Line Transmit HOLD Period */
	uint64_t fmp_sda_hold_tx_period;
	/* Fast Mode Plus (1Mbit/sec) Serial Data Line Receive HOLD Period */
	uint64_t fmp_sda_hold_rx_period;
	/* High Speed Mode Plus (3.4Mbits/sec) Serial Clock Line HIGH Period */
	uint64_t hm_scl_high_period;
	/* High Speed Mode Plus (3.4Mbits/sec) Serial Clock Line LOW Period */
	uint64_t hm_scl_low_period;
	/* High Speed Mode Plus (3.4Mbits/sec) Serial Data Line Transmit HOLD Period */
	uint64_t hm_sda_hold_tx_period;
	/* High Speed Mode Plus (3.4Mbits/sec) Serial Data Line Receive HOLD Period */
	uint64_t hm_sda_hold_rx_period;
	/* Maximum length (in ic_clk_cycles) of suppressed spikes in High Speed Mode */
	uint64_t suppressed_spikes_h_fp;
};

struct intel_thc_hidspi_info {
	/*
	 * Touch Host Controller HID Over SPI Connection Speed
	 *
	 * HID Over SPI Connection Speed - SPI Frequency
	 */
	uint32_t connection_speed;

	/*
	 * Touch Host Controller HID Over SPI Limit PacketSize
	 *
	 * When set, limits SPI read & write packet size to 64B. Otherwise, THC uses Max Soc
	 * packet size for SPI Read and Write 0x0- Max Soc Packet Size, 0x11 - 64 Bytes
	 */
	uint32_t limit_packet_size;

	/*
	 * Touch Host Controller HID Over SPI Flags
	 *
	 * HID Over SPI Flags 0x0:Single SPI Mode, 0x4000:Dual SPI Mode, 0x8000:Quad SPI Mode
	 *  Bit 0-12: Reserved
	 *  Bit 13: SPI Write Mode.
	 *    0b0 - Writes are carried in single SPI mode
	 *    0b1 - Writes are carried out in Multi-SPI mode as specified by bit 14-15
	 *  Bit 14-15: Multi-SPI Mode
	 *    0b00 - Single SPI Mode
	 *    0b01 - Dual SPI Mode
	 *    0b10 - Quad SPI Mode
	 *    0b11 - Reserved
	 */
	enum {
		HIDSPI_WRITE_MODE_SINGLE = 0x0,
		HIDSPI_WRITE_MODE_MULTI_SINGLE_SPI = 0x2000,
		HIDSPI_WRITE_MODE_MULTI_DUAL_SPI = 0x6000,
		HIDSPI_WRITE_MODE_MULTI_QUAD_SPI = 0xa000
	} write_mode;

	/* Touch Host Controller HID Over SPI ResetPad */
	/* NOTE: This could be the same pad as reset_gpio */
	struct acpi_gpio reset_gpio;
	/* Delay to be inserted after device is taken out of reset */
	unsigned int reset_delay_ms;
	/* Delay to be inserted after device is put into reset */
	unsigned int reset_off_delay_ms;

	/*
	 * Touch Host Controller HID Over SPI Limit PacketSize
	 *
	 * Minimum amount of delay the THC/QUICKSPI driver must wait between end of write
	 * operation and begin of read operation. This value shall be in 10us multiples 0x0:
	 * Disabled, 1-65535 (0xFFFF) - up to 655350 us
	 */
	uint32_t performance_limit;

	/*
	 * Touch Host Controller HID Over SPI Reset Sequencing Delay [ms]
	 * Policy control for reset sequencing delay (ACPI _INI, _RST) default 300ms
	 */
	uint16_t reset_sequencing_delay;
};

enum intel_touch_device {
	TH_SENSOR_NONE,
	TH_SENSOR_WACOM,    /* BOM22 for SPI only */
	TH_SENSOR_ELAN,     /* BOM36 for SPI and BOM37 for I2C */
	TH_SENSOR_HYNITRON, /* NYITRON for I2C only  */
	TH_SENSOR_GENERIC,  /* for device properity thru devicetree */
	TH_SENSOR_MAX
};

/*
 * Intel Touch Controller (THC) & sensor device
 * Reference document: Meteor Lake UH type4 EDS vol1 640228
 */
struct drivers_intel_touch_config {
	const char *name;
	const char *desc;

	/*
	 * Touch Host Controller's (THC) 3 protocol modes:
	 *     0x0: THC IPTS (not supported at this time)
	 *     0x1: THC HID SPI (for MTL or later)
	 *     0x2: THC HID I2C (for PTL or later)
	 */
	enum {
		THC_IPTS_MODE,
		THC_HID_SPI_MODE,
		THC_HID_I2C_MODE,
	} mode;

	/* GPIO used to enable device. */
	struct acpi_gpio enable_gpio;
	/* Delay to be inserted after device is enabled. */
	unsigned int enable_delay_ms;
	/* Delay to be inserted after device is disabled. */
	unsigned int enable_off_delay_ms;

	/* GPIO used to take device out of reset or to put it into reset. */
	struct acpi_gpio reset_gpio;
	/* Delay to be inserted after device is taken out of reset. */
	unsigned int reset_delay_ms;
	/* Delay to be inserted after device is put into reset. */
	unsigned int reset_off_delay_ms;

	/* GPIO used for report enabling */
	struct acpi_gpio report_gpio;
	/* Delay to be inserted after device is taken out of reset. */
	unsigned int report_delay_ms;
	/* Delay to be inserted after device is put into reset. */
	unsigned int report_off_delay_ms;

	/*
	 * Touch Host Controller Wake On Touch
	 *
	 * Based on this setting vGPIO for given THC will be in native mode, and additional _CRS
	 * for wake will be exposed in ACPI
	*/
	uint8_t wake_on_touch;

	/*
	 * Either acpi_irq wake_irq or wake_gpio is needed for wake but not both use acpi_irq
	 * for Interrupt() and wake_gpio for GpioInt().
	 */
	struct acpi_irq wake_irq;
	struct acpi_gpio wake_gpio;
	unsigned int wake_gpe; /* mapped GPE number for wake */
	/* sensor-specific */
	const char *sensor_dev_name;
	const char *sensor_dev_desc;

	enum intel_touch_device connected_device;

	/*
	 * Touch Host Controller HID Over SPI Limit PacketSize
	 *
	 * When set, limits SPI read & write packet size to 64B. Otherwise, THC uses Max Soc
	 * packet size for SPI Read and Write 0x0- Max Soc Packet Size, 0x11 - 64 Bytes
	 */
	uint32_t limit_packet_size;

	struct intel_thc_dev_info dev_hidi2c;
	struct intel_thc_dev_info dev_hidspi;

	struct intel_thc_hidspi_info soc_hidspi;
	struct intel_thc_hidi2c_info soc_hidi2c;

	/* Touch Host Controller Active Ltr */
	uint32_t active_ltr;

	/* Touch Host Controller Idle Ltr */
	uint32_t idle_ltr;

	/* Add `DmaProperty` in _DSD */
	bool add_acpi_dma_property;
};

/* These functions are weak and need to be overridden by a SoC */
const struct intel_thc_hidi2c_info *soc_get_thc_hidi2c_info(void);
const struct intel_thc_hidspi_info *soc_get_thc_hidspi_info(void);

#endif /* __DRIVERS_INTEL_TOUCH_CHIP_H__ */
