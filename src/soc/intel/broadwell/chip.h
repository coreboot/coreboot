/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_INTEL_BROADWELL_CHIP_H_
#define _SOC_INTEL_BROADWELL_CHIP_H_

#include <drivers/intel/gma/i915.h>
#include <stdint.h>

struct soc_intel_broadwell_config {
	/* GPE configuration */
	uint32_t gpe0_en_1;
	uint32_t gpe0_en_2;
	uint32_t gpe0_en_3;
	uint32_t gpe0_en_4;

	/* GPIO SMI configuration */
	uint32_t alt_gp_smi_en;

	/* IDE configuration */
	uint8_t sata_port_map;
	uint32_t sata_port0_gen3_tx;
	uint32_t sata_port1_gen3_tx;
	uint32_t sata_port2_gen3_tx;
	uint32_t sata_port3_gen3_tx;
	uint32_t sata_port0_gen3_dtle;
	uint32_t sata_port1_gen3_dtle;
	uint32_t sata_port2_gen3_dtle;
	uint32_t sata_port3_gen3_dtle;

	/*
	 * SATA DEVSLP Mux
	 * 0 = port 0 DEVSLP on DEVSLP0/GPIO33
	 * 1 = port 3 DEVSLP on DEVSLP0/GPIO33
	 */
	uint8_t sata_devslp_mux;

	/*
	 * DEVSLP Disable
	 * 0: DEVSLP is enabled
	 * 1: DEVSLP is disabled
	 */
	uint8_t sata_devslp_disable;

	/* Generic IO decode ranges */
	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;

	/* Enable linear PCIe Root Port function numbers starting at zero */
	uint8_t pcie_port_coalesce;

	/* Force root port ASPM configuration with port bitmap */
	uint8_t pcie_port_force_aspm;

	/* Put SerialIO devices into ACPI mode instead of a PCI device */
	uint8_t sio_acpi_mode;

	/* I2C voltage select: 0=3.3V 1=1.8V */
	uint8_t sio_i2c0_voltage;
	uint8_t sio_i2c1_voltage;

	/* Enable ADSP power gating features */
	uint8_t adsp_d3_pg_enable;
	uint8_t adsp_sram_pg_enable;

	/*
	 * Clock Disable Map:
	 * [21:16] = CLKOUT_PCIE# 5-0
	 *    [24] = CLKOUT_ITPXDP
	 */
	uint32_t icc_clock_disable;

	/*
	 * Digital Port Hotplug Enable:
	 *  0x04 = Enabled, 2ms short pulse
	 *  0x05 = Enabled, 4.5ms short pulse
	 *  0x06 = Enabled, 6ms short pulse
	 *  0x07 = Enabled, 100ms short pulse
	 */
	u8 gpu_dp_b_hotplug;
	u8 gpu_dp_c_hotplug;
	u8 gpu_dp_d_hotplug;

	/* Panel power sequence timings */
	u8 gpu_panel_power_cycle_delay;
	u16 gpu_panel_power_up_delay;
	u16 gpu_panel_power_down_delay;
	u16 gpu_panel_power_backlight_on_delay;
	u16 gpu_panel_power_backlight_off_delay;

	/* Panel backlight settings */
	unsigned int gpu_pch_backlight_pwm_hz;
	enum {
		GPU_BACKLIGHT_POLARITY_HIGH = 0,
		GPU_BACKLIGHT_POLARITY_LOW,
	} gpu_pch_backlight_polarity;

	/*
	 * Graphics CD Clock Frequency
	 * 0 = 337.5MHz
	 * 1 = 450MHz
	 * 2 = 540MHz
	 * 3 = 675MHz
	 */
	int cdclk;

	struct i915_gpu_controller_info gfx;

	/* Enable S0iX support */
	int s0ix_enable;

	/*
	 * Minimum voltage for C6/C7 state:
	 * 0x67 = 1.6V (full swing)
	 *  ...
	 * 0x79 = 1.7V
	 *  ...
	 * 0x83 = 1.8V (no swing)
	 */
	int vr_cpu_min_vid;

	/*
	 * Set slow VR ramp rate on C-state exit:
	 * 0 = Fast VR ramp rate / 2
	 * 1 = Fast VR ramp rate / 4
	 * 2 = Fast VR ramp rate / 8
	 * 3 = Fast VR ramp rate / 16
	 */
	int vr_slow_ramp_rate_set;

	/* Enable slow VR ramp rate */
	int vr_slow_ramp_rate_enable;

	/* Deep SX enable */
	int deep_sx_enable_ac;
	int deep_sx_enable_dc;

	/* TCC activation offset */
	uint32_t tcc_offset;
};

typedef struct soc_intel_broadwell_config config_t;

#endif
