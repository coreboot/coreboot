/* SPDX-License-Identifier: GPL-2.0-only */
#include "gpio.h"

#include <amdblocks/acpi.h>
#include <amdblocks/amd_pci_util.h>
#include <amdblocks/i2c.h>
#include <device/device.h>
#include <ec/acpi/ec.h>
#include <soc/amd/glinda/chip.h>
#include <static.h>
#include <types.h>

#include "ec.h"

/* The IRQ mapping in fch_irq_map ends up getting written to the indirect address space that is
   accessed via I/O ports 0xc00/0xc01. */

/*
 * This controls the device -> IRQ routing.
 *
 * Hardcoded IRQs:
 *  0: timer < soc/amd/common/acpi/lpc.asl
 *  1: i8042 - Keyboard
 *  2: cascade
 *  8: rtc0 <- soc/amd/common/acpi/lpc.asl
 *  9: acpi <- soc/amd/common/acpi/lpc.asl
 */
static const struct fch_irq_routing fch_irq_map[] = {
	{ PIRQ_A,	12,		PIRQ_NC },
	{ PIRQ_B,	14,		PIRQ_NC },
	{ PIRQ_C,	15,		PIRQ_NC },
	{ PIRQ_D,	12,		PIRQ_NC },
	{ PIRQ_E,	14,		PIRQ_NC },
	{ PIRQ_F,	15,		PIRQ_NC },
	{ PIRQ_G,	12,		PIRQ_NC },
	{ PIRQ_H,	14,		PIRQ_NC },

	{ PIRQ_SCI,	ACPI_SCI_IRQ,	ACPI_SCI_IRQ },
	{ PIRQ_SDIO,	PIRQ_NC,	PIRQ_NC },
	{ PIRQ_GPIO,	11,		11 },
	{ PIRQ_I2C0,	0xA,	0xA },
	{ PIRQ_I2C1,	0xB,	0xB },
	{ PIRQ_I2C2,	0x4,    0x4 },
	{ PIRQ_I2C3,	0x6,	0x6 },
	{ PIRQ_UART0,	0x3,	0x3 },
#if CONFIG(UART_0_1_2_3_TWO_WIRE)
	{ PIRQ_UART1,	0xE,	0xE },
	{ PIRQ_UART3,   0xF,    0xF },
#endif
	{ PIRQ_UART2,   0x5,    0x5 },
	{ PIRQ_UART4,   0x10,   0x10 },
	/* The MISC registers are not interrupt numbers */
	{ PIRQ_MISC,	0xfa,		0x00 },
	{ PIRQ_MISC0,	0x91,		0x00 },
	{ PIRQ_HPET_L,	0x00,		0x00 },
	{ PIRQ_HPET_H,	0x00,		0x00 },
};

const struct fch_irq_routing *mb_get_fch_irq_mapping(size_t *length)
{
	*length = ARRAY_SIZE(fch_irq_map);
	return fch_irq_map;
}

static void mainboard_configure_uarts(void)
{
	if (CONFIG(XGBE_LED_TURN_ON)) {
		DEV_PTR(uart_2)->enabled = 0;
		DEV_PTR(uart_4)->enabled = 0;
	}
	if (CONFIG(UART_0_2_4_FOUR_WIRE)) {
		DEV_PTR(uart_1)->enabled = 0;
		DEV_PTR(uart_3)->enabled = 0;
	}
}

static void mainboard_configure_i2c(void)
{
	if (!CONFIG(I2C_ENABLE)) {
		DEV_PTR(i2c_0)->enabled = 0;
		DEV_PTR(i2c_1)->enabled = 0;
		DEV_PTR(i2c_2)->enabled = 0;
		DEV_PTR(i2c_3)->enabled = 0;
	}
}

static void mainboard_configure_i3c(void)
{
	struct soc_amd_glinda_config *cfg = config_of_soc();

	if (CONFIG(I2C_ENABLE)) {
		/* I2C mode - disable I3C devices, use devicetree defaults (I2C_PAD_RX_1_8V) */
		DEV_PTR(i3c_0)->enabled = 0;
		DEV_PTR(i3c_1)->enabled = 0;
		DEV_PTR(i3c_2)->enabled = 0;
		DEV_PTR(i3c_3)->enabled = 0;
	} else {
		/* I3C mode - configure i2c_pad for I3C voltage levels */
		cfg->i2c_pad[0].rx_level = I3C_PAD_RX_1_8V;
		cfg->i2c_pad[1].rx_level = I3C_PAD_RX_1_8V;
		cfg->i2c_pad[2].rx_level = I3C_PAD_RX_1_8V;
		cfg->i2c_pad[3].rx_level = I3C_PAD_RX_1_8V;
	}
}

static void mainboard_configure_ec(void)
{
	ec_set_ports(JAGUAR_EC_CMD, JAGUAR_EC_DATA);

	/* Modern Standby enable, D3 cold enable */
	const u8 mask = EC_MODERN_STANDBY_SSD0_D3_EN |
			EC_MODERN_STANDBY_SSD1_D3_EN |
			EC_MODERN_STANDBY_S0ix;

	u8 tmp = ec_read(EC_MODERN_STANDBY);
	if ((tmp & mask) != mask)
		ec_write(EC_MODERN_STANDBY, tmp | mask);
}

static void mainboard_init(void *chip_info)
{
	mainboard_program_gpios();
	mainboard_configure_uarts();
	mainboard_configure_i2c();
	mainboard_configure_i3c();
	mainboard_configure_ec();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
