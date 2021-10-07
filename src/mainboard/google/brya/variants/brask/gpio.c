/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B7  : ISH_12C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_B7, NONE, DEEP, NF2),
	/* B8  : ISH_12C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF2),
	/*
	 * D1  : ISH_GP1 ==> FP_RST_ODL
	 * FP_RST_ODL comes out of reset as hi-z and does not have an external pull-down.
	 * To ensure proper power sequencing for the FPMCU device, reset signal is driven low
	 * early on in bootblock, followed by enabling of power. Reset signal is deasserted
	 * later on in ramstage. Since reset signal is asserted in bootblock, it results in
	 * FPMCU not working after a S3 resume. This is a known issue.
	 */
	PAD_CFG_GPO(GPP_D1, 0, DEEP),
	/* D2  : ISH_GP2 ==> EN_FP_PWR */
	PAD_CFG_GPO(GPP_D2, 1, DEEP),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* H13 : I2C7_SCL ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_H13, 1, DEEP),

	/* CPU PCIe VGPIO for PEG60 */
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_48, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_49, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_50, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_51, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_52, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_53, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_54, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_55, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_56, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_57, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_58, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_59, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_60, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_61, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_62, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_63, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_76, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_77, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_78, NONE, DEEP, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_79, NONE, DEEP, NF1),
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}
