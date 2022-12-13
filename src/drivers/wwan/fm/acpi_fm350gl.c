/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include "chip.h"
#include "soc/intel/common/block/pcie/rtd3/chip.h"

/* FCPO# to RESET# delay time during WWAN ON */
#define FM350GL_TN2B 20
/* RESET# to PERST# delay time during WWAN ON */
#define FM350GL_TB2R 80
/* The delay between de-assertion of PERST# to change of PDS state from 0 to 1 during WWAN ON */
#define FM350GL_TR2P 0
/* RESET# to FCPO# delay time during WWAN OFF */
#define FM350GL_TB2F 10
/* Time to allow the WWAN module to fully discharge any residual voltages before FCPO# could be
   de-asserted again. */
#define FM350GL_TFDI 500
/* The delay between assertion and de-assertion RESET# during FLDR */
#define FM350GL_TBTG 10
/* The delay between de-assertion of RESET# and change of PDS state from 0 to 1 after FLDR */
#define FM350GL_TBTP 170
/* PERST# to RESET# delay time during WWAN OFF */
#define FM350GL_TR2B 10
/* 20s HW initialization needed after de-assertion of PERST#
   However, it is not required and is not proper place to ensure HW initialization in ACPI. The
   delay here is to ensure the following reset or RTD3 _OFF method won't be called immediately.
 */
#define FM350GL_TIME_HW_INIT 100

enum reset_type {
	RESET_TYPE_WARM = 0,
	RESET_TYPE_COLD = 1
};

/*
 *  Returns the RTD3 PM methods requested and available to the device.
 */
static enum acpi_pcie_rp_pm_emit
wwan_fm350gl_get_rtd3_method_support(const struct drivers_wwan_fm_config *config)
{
	const struct soc_intel_common_block_pcie_rtd3_config *rtd3_config;

	rtd3_config = config_of(config->rtd3dev);

	return rtd3_config->ext_pm_support;
}

/*
 *  Generate first half reset flow (FHRF) method.
 *  Arg0 = 0; RESET_TYPE_WARM: warm reset
 *  Arg0 = 1; RESET_TYPE_COLD: cold reset
 */
static void wwan_fm350gl_acpi_method_fhrf(const struct device *parent_dev,
	const struct drivers_wwan_fm_config *config)
{
	acpigen_write_method_serialized("FHRF", 1);
	{
		/* LOCAL0 = PERST# */
		acpigen_get_tx_gpio(&config->perst_gpio);
		acpigen_write_if_lequal_op_int(LOCAL0_OP, 0);
		{
			if (wwan_fm350gl_get_rtd3_method_support(config) &
				ACPI_PCIE_RP_EMIT_L23) {
				acpigen_emit_namestring(acpi_device_path_join(parent_dev,
					"DL23"));
			}
			/* assert PERST# pin */
			acpigen_enable_tx_gpio(&config->perst_gpio);
		}
		acpigen_write_if_end(); /* If */
		acpigen_write_sleep(FM350GL_TR2B);
		/* assert RESET# pin */
		acpigen_enable_tx_gpio(&config->reset_gpio);
		/* warm reset */
		acpigen_write_if_lequal_op_int(ARG0_OP, RESET_TYPE_WARM);
		{
			acpigen_write_sleep(FM350GL_TBTG);
		}
		/* cold reset */
		acpigen_write_else();
		{
			acpigen_write_if_lequal_op_int(ARG0_OP, RESET_TYPE_COLD);
			{
				/* disable source clock */
				if (wwan_fm350gl_get_rtd3_method_support(config) &
					ACPI_PCIE_RP_EMIT_SRCK) {
					acpigen_emit_namestring(acpi_device_path_join(
						parent_dev, "SRCK"));
					acpigen_emit_byte(ZERO_OP);
				}
				acpigen_write_sleep(FM350GL_TB2F);
				/* assert FCPO# pin */
				acpigen_enable_tx_gpio(&config->fcpo_gpio);
				acpigen_write_sleep(FM350GL_TFDI);
			}
			acpigen_write_if_end(); /* If */
		}
		acpigen_pop_len(); /* Else */
	}
	acpigen_write_method_end(); /* Method */
}

/*
 *  Generate second half reset flow (SHRF) method.
 */
static void wwan_fm350gl_acpi_method_shrf(const struct device *parent_dev,
		const struct drivers_wwan_fm_config *config)
{
	acpigen_write_method_serialized("SHRF", 0);
	{
		/* call rtd3 method to Disable ModPHY Power Gating. */
		if (wwan_fm350gl_get_rtd3_method_support(config) &
			ACPI_PCIE_RP_EMIT_PSD0) {
			acpigen_emit_namestring(acpi_device_path_join(parent_dev,
				"PSD0"));
		}
		/* call rtd3 method to Enable SRC Clock. */
		if (wwan_fm350gl_get_rtd3_method_support(config) &
			ACPI_PCIE_RP_EMIT_SRCK) {
			acpigen_emit_namestring(acpi_device_path_join(parent_dev,
				"SRCK"));
			acpigen_emit_byte(ONE_OP);
		}
		/* De-assert FCPO# GPIO. */
		acpigen_disable_tx_gpio(&config->fcpo_gpio);
		acpigen_write_sleep(FM350GL_TN2B);
		/* De-assert RESET# GPIO. */
		acpigen_disable_tx_gpio(&config->reset_gpio);
		acpigen_write_sleep(FM350GL_TB2R);
		/* De-assert PERST# GPIO. */
		acpigen_disable_tx_gpio(&config->perst_gpio);
		/* Call rtd3 method to trigger L2/L3 ready exit flow in root port */
		if (wwan_fm350gl_get_rtd3_method_support(config) &
			ACPI_PCIE_RP_EMIT_L23) {
			acpigen_emit_namestring(acpi_device_path_join(parent_dev,
				"L23D"));
		}
		acpigen_write_sleep(FM350GL_TIME_HW_INIT);
	}
	acpigen_write_method_end(); /* Method */
}

/*
 * Generate _RST method. This is to perform a soft reset. It is added under
 * PXSX. This is called during device driver removal.
 */
static void wwan_fm350gl_acpi_method_rst(const struct device *parent_dev,
			 const struct drivers_wwan_fm_config *config)
{
	acpigen_write_method_serialized("_RST", 0);
	{
		/* Perform 1st Half of FLDR Flow for soft reset: FHRF(0) */
		acpigen_emit_namestring("FHRF");
		acpigen_emit_byte(RESET_TYPE_WARM);
		/* Perform 2nd Half of FLDR Flow: SHRF() */
		acpigen_emit_namestring("SHRF");
		/* Indicates that the following _Off will be skipped. */
		acpigen_emit_byte(INCREMENT_OP);
		acpigen_emit_namestring(acpi_device_path_join(parent_dev, "RTD3.OFSK"));
	}
	acpigen_write_method_end(); /* Method */
}

/*
 * Generate _RST method. This is to perform a cold reset. This reset will be
 * included under PXSX.MRST. This method is used during device firmware update.
 */
static void wwan_fm350gl_acpi_method_mrst_rst(const struct device *parent_dev,
			 const struct drivers_wwan_fm_config *config)
{
	acpigen_write_method_serialized("_RST", 0);
	{
		/* Perform 1st Half of FLDR Flow for cold reset: FHRF (1) */
		acpigen_emit_namestring("FHRF");
		acpigen_emit_byte(RESET_TYPE_COLD);
		/* Perform 2nd Half of FLDR Flow: SHRF () */
		acpigen_emit_namestring("SHRF");
		/* Indicate kernel ACPI PM to skip _off RTD3 after reset at the end of
		   driver removal */
		acpigen_emit_byte(INCREMENT_OP);
		acpigen_emit_namestring(acpi_device_path_join(parent_dev, "RTD3.OFSK"));
	}
	acpigen_write_method_end(); /* Method */
}

/*
 * Generate DPTS (Device Prepare To Seep) Method. This is called in
 *  \.SB.MPTS Method.
 */
static void wwan_fm350gl_acpi_method_dpts(const struct device *parent_dev,
			 const struct drivers_wwan_fm_config *config)
{
	acpigen_write_method_serialized("DPTS", 1);
	{
		/* Perform 1st Half of FLDR Flow for cold reset: FHRF (1) */
		acpigen_emit_namestring("FHRF");
		acpigen_emit_byte(RESET_TYPE_COLD);
	}
	acpigen_write_method_end(); /* Method */
}

static const char *wwan_fm350gl_acpi_name(const struct device *dev)
{
	/* Attached device name must be "PXSX" for the Linux Kernel to recognize it. */
	return "PXSX";
}

static void wwan_fm350gl_acpi_fill_ssdt(const struct device *dev)
{
	const struct drivers_wwan_fm_config *config = config_of(dev);
	const struct device *parent = dev->bus->dev;
	const char *scope = acpi_device_path(parent);

	if (!is_dev_enabled(parent)) {
		printk(BIOS_ERR, "%s: root port not enabled\n", __func__);
		return;
	}
	if (!scope) {
		printk(BIOS_ERR, "%s: root port scope not found\n", __func__);
		return;
	}
	if (!config->fcpo_gpio.pin_count && !config->reset_gpio.pin_count &&
		!config->perst_gpio.pin_count) {
		printk(BIOS_ERR, "%s: FCPO, RESET, PERST GPIO required for %s.\n",
			 __func__, scope);
		return;
	}
	printk(BIOS_INFO, "%s: Enable WWAN for %s (%s)\n", scope, dev_path(parent),
		config->desc ?: dev->chip_ops->name);
	acpigen_write_scope(scope);
	{
		acpigen_write_device(wwan_fm350gl_acpi_name(dev));
		{
			acpigen_write_ADR(0);
			if (config->name)
				acpigen_write_name_string("_DDN", config->name);
			if (config->desc)
				acpigen_write_name_unicode("_STR", config->desc);
			wwan_fm350gl_acpi_method_fhrf(parent, config);
			wwan_fm350gl_acpi_method_shrf(parent, config);
			wwan_fm350gl_acpi_method_rst(parent, config);
			wwan_fm350gl_acpi_method_dpts(parent, config);

			if (config->add_acpi_dma_property)
				acpi_device_add_dma_property(NULL);

			/* NOTE: the 5G driver will call MRST._RST to trigger a cold reset
			 * during firmware update.
			 */
			acpigen_write_device("MRST");
			{
				acpigen_write_ADR(0);
				wwan_fm350gl_acpi_method_mrst_rst(parent, config);
			}

			acpigen_write_device_end(); /* Device */
		}
		acpigen_write_device_end(); /* Device */
	}
	acpigen_write_scope_end(); /* Scope */
}

static struct device_operations wwan_fm350gl_ops = {
	.read_resources  = noop_read_resources,
	.set_resources   = noop_set_resources,
	.acpi_fill_ssdt  = wwan_fm350gl_acpi_fill_ssdt,
	.acpi_name       = wwan_fm350gl_acpi_name
};

static void wwan_fm350gl_acpi_enable(struct device *dev)
{
	dev->ops = &wwan_fm350gl_ops;
}

struct chip_operations drivers_wwan_fm_ops = {
	 CHIP_NAME("Fibocom FM-350-GL")
	.enable_dev = wwan_fm350gl_acpi_enable
};
