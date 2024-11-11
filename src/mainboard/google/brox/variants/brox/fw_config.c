/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <drivers/intel/ish/chip.h>
#include <fw_config.h>
#include <gpio.h>
#include <static.h>

#define ISH_FIRMWARE_NAME "brox_ish.bin"

static const struct pad_config ish_enable_pads[] = {
	/* GPP_A16 : ISH_GP5, TABLET_MODE_ODL */
	PAD_CFG_NF(GPP_A16, NONE, DEEP, NF4),
	/* GPP_B5 : ISH I2C0_SDA */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B5, NONE, DEEP, NF1),
	/* GPP_B6 : ISH_I2C0_SCL */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B6, NONE, DEEP, NF1),
	/* GPP_B15 : ISH_GP7, LID_OPEN_1V8 */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF5),
	/* GPP_D2 : ISH_GP2, SOC_ISH_ACCEL_INT_L */
	PAD_CFG_NF(GPP_D2, NONE, DEEP, NF1),
	/* GPP_D3 : ISH_GP3, SOC_ISH_IMU_INT_L */
	PAD_CFG_NF(GPP_D3, NONE, DEEP, NF1),
	/* GPP_D13 : [NF1: ISH_UART0_RXD ==> UART0_ISH_RX_DBG_TX */
	PAD_CFG_NF(GPP_D13, NONE, DEEP, NF1),
	/* GPP_D14 : [NF1: ISH_UART0_TXD ==> UART0_ISH_TX_DBG_RX */
	PAD_CFG_NF(GPP_D14, DN_20K, DEEP, NF1),
	/* GPP_E9 : [NF1: USB_OC0# NF2: ISH_GP4 NF6: USB_C_GPP_E9] ==> NOTE_BOOK_MODE */
	PAD_CFG_NF(GPP_E9, NONE, PLTRST, NF2),
};

static void fw_config_handle(void *unused)
{
	struct device *ish_config_device = DEV_PTR(ish_conf);
	struct drivers_intel_ish_config *config = config_of(ish_config_device);

	if (fw_config_probe(FW_CONFIG(ISH, ISH_ENABLE))) {
		printk(BIOS_INFO, "Configure GPIOs, device config for ISH.\n");
		gpio_configure_pads(ish_enable_pads, ARRAY_SIZE(ish_enable_pads));

		config->firmware_name = ISH_FIRMWARE_NAME;
	}

	if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UFS))) {
		printk(BIOS_INFO, "Configure GPIOs, device config for UFS.\n");
		config->add_acpi_dma_property = true;
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
