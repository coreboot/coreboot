/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <console/console.h>
#include <device/pnp_ops.h>
#include <gpio.h>
#include <superio/ite/common/ite.h>
#include <types.h>

static const struct pad_config gpio_table[] = {
	PAD_CFG_NF(GPP_B14, DN_20K, PLTRST, NF1),			/* SPKR */
	PAD_CFG_NF(GPP_C20, UP_20K, PLTRST, NF1),			/* PCH_UART2_RXD */
	PAD_CFG_NF(GPP_C21, UP_20K, PLTRST, NF1),			/* PCH_UART2_TXD */
	PAD_NC(GPP_C22, NONE),
	PAD_CFG_GPI(GPP_C23, NONE, PLTRST),				/* TODO: SIO PME# */
	PAD_CFG_NF(GPP_E8, NONE, DEEP, NF1),				/* SATA_LED# */
	PAD_CFG_NF(GPP_E9, NONE, DEEP, NF1),				/* USB_OC_LAN# */
	PAD_CFG_NF(GPP_E10, NONE, DEEP, NF1),				/* USB3.0_OC_BACK# */
	PAD_CFG_NF(GPP_E11, NONE, DEEP, NF1),				/* USB_OC_REAR2# */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF1),				/* USB_OC_FRONT1# */
	PAD_CFG_NF(GPP_F15, NONE, DEEP, NF1),				/* USB_OC_FRONT2# */
	PAD_CFG_GPI(GPP_G1, NONE, PLTRST),				/* LPT_DET# */
	PAD_CFG_GPO(GPP_G2, 0, PLTRST),					/* AUD_AMP_ON# */
	PAD_CFG_GPO(GPP_G3, 0, PLTRST),					/* W_DISABLE2# */
	PAD_CFG_GPI(GPP_G4, NONE, PLTRST),				/* CLR_CMOS# */
	PAD_CFG_GPI(GPP_G5, NONE, PLTRST),				/* CLR_PSWD# */
	PAD_CFG_GPI(GPP_G6, NONE, PLTRST),				/* BOOT_BLOCK_EN# */
	PAD_CFG_GPI(GPP_G9, NONE, PLTRST),				/* HOOD_SW_DET# */
	PAD_CFG_GPI(GPP_G12, NONE, PLTRST),				/* FRONT_USB_DET1# */
	PAD_CFG_GPI(GPP_G13, NONE, PLTRST),				/* FRONT_USB_DET2# */
	PAD_CFG_GPI(GPP_G14, NONE, PLTRST),				/* FRONT_USB_DET3# */
	PAD_CFG_GPI(GPP_G16, NONE, PLTRST),				/* F_AUDIO_DET# */
	PAD_CFG_GPI(GPP_G17, NONE, PLTRST),				/* COMM_B_DET# */
	PAD_CFG_GPI_SCI(GPP_G21, NONE, DEEP, EDGE_SINGLE, INVERT),	/* SPI_TPM_PIRQ# */
	PAD_CFG_GPI(GPP_H10, NONE, PLTRST),				/* S_GPI_SKU0 */
	PAD_CFG_GPI(GPP_H15, NONE, PLTRST),				/* BRD_REV0 */
	PAD_CFG_GPI(GPP_H16, NONE, PLTRST),				/* BRD_REV1 */
	PAD_CFG_GPI(GPP_H17, NONE, PLTRST),				/* BRD_REV2 */
	PAD_CFG_GPI(GPP_H18, NONE, PLTRST),				/* S_GPI_SKU1 */
	PAD_CFG_NF(GPP_I2, NONE, PLTRST, NF1),				/* DPD_HPD_R */
	PAD_CFG_NF(GPP_I3, NONE, PLTRST, NF1),				/* DPE_HPD_R */
	PAD_CFG_NF(GPP_I9, NONE, PLTRST, NF1),				/* DDPD_CTRLCLK */
	PAD_CFG_NF(GPP_I10, DN_20K, PLTRST, NF1),			/* DDPD_CTRLDATA */
};

static void mainboard_configure_super_io(void)
{
	const pnp_devfn_t dev = PNP_DEV(0x2e, 7);

	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);

	pnp_write_config(dev, 0x23, 0x59);
	pnp_write_config(dev, 0x25, 0x10);
	pnp_write_config(dev, 0x26, 0x04);
	pnp_write_config(dev, 0x28, 0x08);
	pnp_write_config(dev, 0x2a, 0x81);
	pnp_write_config(dev, 0x71, 0x08);
	pnp_write_config(dev, 0xc0, 0x00);
	pnp_write_config(dev, 0xc1, 0x04);
	pnp_write_config(dev, 0xc8, 0x00);
	pnp_write_config(dev, 0xc9, 0x04);
	pnp_write_config(dev, 0xcb, 0x08);
	pnp_write_config(dev, 0xd5, 0x07);
	pnp_write_config(dev, 0xf8, 0x12);
	pnp_write_config(dev, 0xf9, 0x01);

	pnp_exit_conf_state(dev);
}

void bootblock_mainboard_early_init(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));

	mainboard_configure_super_io();
}

void bootblock_mainboard_init(void)
{
	const gpio_t rev_gpios[] = {
		GPP_H15,
		GPP_H16,
		GPP_H17,
	};

	const char *const rev_table[8] = {
		[0] = "DB",
		[1] = "Pre-SI",
		[2] = "SI",
		[3] = "PV",
		[4] = "1.00 (SMVB)",
		[5] = "1.10 (ECN1)",
		[6] = "1.20 (ECN1)",
		[7] = "1.30 (ECN1)",
	};

	const char *const brd_str = gpio_get(GPP_H10) ? "Sid" : "Manny";

	const uint32_t brd_rev = gpio_base2_value(rev_gpios, ARRAY_SIZE(rev_gpios));

	printk(BIOS_DEBUG, "Mainboard: %s rev %s\n", brd_str, rev_table[brd_rev]);
}
