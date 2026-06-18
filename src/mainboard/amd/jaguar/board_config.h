/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <option.h>
#include "ec.h"

/* I2C */

#define OPTION_NAME_I2C "i2c_i3c_select"
static inline bool mb_cfg_i2c_enabled(void)
{
	return get_uint_option(OPTION_NAME_I2C, CONFIG(I2C_ENABLE));
}

/* UART */

#define OPTION_NAME_UART "uart_four_wire_mode"
static inline bool mb_cfg_uart1_disabled(void)
{
	return get_uint_option(OPTION_NAME_UART, CONFIG(UART_0_2_4_FOUR_WIRE));
}

static inline bool mb_cfg_uart3_disabled(void)
{
	return get_uint_option(OPTION_NAME_UART, CONFIG(UART_0_2_4_FOUR_WIRE));
}

static inline bool mb_cfg_uart024_4_wire(void)
{
	return get_uint_option(OPTION_NAME_UART, CONFIG(UART_0_2_4_FOUR_WIRE));
}

/* PCIe */

#define OPTION_NAME_PCIE_SLOT0_FORCE_PWR "pcie_slot0_force_pwr"
static inline bool mb_cfg_pcie_slot0_force_pwr(void)
{
	if (!CONFIG(ENABLE_EVAL_CARD))
		return false;
	return get_uint_option(OPTION_NAME_PCIE_SLOT0_FORCE_PWR,
		CONFIG(ENABLE_FORCE_POWER_GPP0) && CONFIG(PCIE_SLOT0_2X4));
}

#define OPTION_NAME_PCIE_BIF "pcie_ssd0_slot1_bifurcation"
static inline int mb_cfg_pcie_bifurcation(void)
{
	u8 bif;
	if (CONFIG(ENABLE_NVME_4LANES))
		bif = EC_PCIE_MUX_NVMEX4;
	else if (CONFIG(ENABLE_PCIE_4LANES))
		bif = EC_PCIE_MUX_SLOT1X4;
	else if (CONFIG(ENABLE_NVME_PCIE_2LANES))
		bif = EC_PCIE_MUX_M2_SLOT_2X2X;
	else
		bif = EC_PCIE_MUX_M2_WLAN_2X2X;

	return get_uint_option(OPTION_NAME_PCIE_BIF, bif);
}

/* xGBE */

#define OPTION_NAME_XGBE_LEDS "xgbe_leds_enable"
static inline bool mb_cfg_xgbe_leds(void)
{
	if (!CONFIG(XGBE_EN))
		return false;
	return get_uint_option(OPTION_NAME_XGBE_LEDS, CONFIG(XGBE_LED_TURN_ON));
}

static inline bool mb_cfg_uart2_disabled(void)
{
	return mb_cfg_xgbe_leds();
}

static inline bool mb_cfg_uart4_disabled(void)
{
	return mb_cfg_xgbe_leds();
}

#define OPTION_NAME_XGBE_P0_LINK_LEDS "xgbe_p0_link_speed_leds_enable"
static inline bool mb_cfg_xgbe_p0_link_speed_leds(void)
{
	if (!CONFIG(XGBE_EN))
		return false;
	return get_uint_option(OPTION_NAME_XGBE_P0_LINK_LEDS, CONFIG(TURN_ON_PORT_0_LINK_SPEED_LED));
}

#define OPTION_NAME_XGBE_P0_STATUS_LEDS "xgbe_p0_link_status_leds_enable"
static inline bool mb_cfg_xgbe_p0_link_status_leds(void)
{
	if (!CONFIG(XGBE_EN))
		return false;
	return get_uint_option(OPTION_NAME_XGBE_P0_STATUS_LEDS, CONFIG(TURN_ON_PORT_0_LINK_STATUS_LED));
}

#define OPTION_NAME_XGBE_P0_BLINK_RATE "xgbe_p0_link_leds_blink_rate"
static inline int mb_cfg_xgbe_p0_led_blink_rate(void)
{
	if (!CONFIG(XGBE_EN))
		return false;
	return get_uint_option(OPTION_NAME_XGBE_P0_BLINK_RATE, CONFIG_PORT_0_TX_RX_LED_BLINK_RATE);
}

#define OPTION_NAME_XGBE_P1_LINK_LEDS "xgbe_p1_link_speed_leds_enable"
static inline bool mb_cfg_xgbe_p1_link_speed_leds(void)
{
	if (!CONFIG(XGBE_EN))
		return false;
	return get_uint_option(OPTION_NAME_XGBE_P1_LINK_LEDS, CONFIG(TURN_ON_PORT_1_LINK_SPEED_LED));
}

#define OPTION_NAME_XGBE_P1_STATUS_LEDS "xgbe_p1_link_status_leds_enable"
static inline bool mb_cfg_xgbe_p1_link_status_leds(void)
{
	if (!CONFIG(XGBE_EN))
		return false;
	return get_uint_option(OPTION_NAME_XGBE_P1_STATUS_LEDS, CONFIG(TURN_ON_PORT_1_LINK_STATUS_LED));
}

#define OPTION_NAME_XGBE_P1_BLINK_RATE "xgbe_p1_link_leds_blink_rate"
static inline int mb_cfg_xgbe_p1_led_blink_rate(void)
{
	if (!CONFIG(XGBE_EN))
		return false;
	return get_uint_option(OPTION_NAME_XGBE_P1_BLINK_RATE, CONFIG_PORT_1_TX_RX_LED_BLINK_RATE);
}
