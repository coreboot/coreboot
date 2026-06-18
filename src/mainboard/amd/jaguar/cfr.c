/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include "ec.h"
#include "board_config.h"

static const struct sm_object i2c_mode = SM_DECLARE_ENUM({
	.opt_name	= OPTION_NAME_I2C,
	.ui_name	= "I2C/I3C select",
	.ui_helptext	= "Select if I2C or I3C is used. They share the same pads.",
	.default_value	= CONFIG(I2C_ENABLE),
	.values		= (const struct sm_enum_value[]) {
		{ "I3C", 0},
		{ "I2C", 1},
		SM_ENUM_VALUE_END
	},
});

static struct sm_obj_form i2c = {
	.ui_name = "I2C",
	.obj_list = (const struct sm_object *[]) {
		&i2c_mode,
		NULL
	},
};

static const struct sm_object uart_four_wire = SM_DECLARE_ENUM({
	.opt_name	= OPTION_NAME_UART,
	.ui_name	= "UART 4-wire Mode",
	.ui_helptext	= "Select how many wires are used on the UARTs.",
	.default_value	= CONFIG(UART_0_2_4_FOUR_WIRE),
	.values		= (const struct sm_enum_value[]) {
		{ "All UARTs in 2-wire mode", 0},
		{ "UART0, UART2, UART4 in 4-wire mode. UART1/UART3 off.", 1},
		SM_ENUM_VALUE_END
	},
});

static struct sm_obj_form uart = {
	.ui_name = "UART",
	.obj_list = (const struct sm_object *[]) {
		&uart_four_wire,
		NULL
	},
};

static const struct sm_object slot0_enable = SM_DECLARE_BOOL({
	.opt_name	= OPTION_NAME_SLOT_PCIE_ENABLE,
	.ui_name	= "Enable PCIe Slot0",
	.default_value	= CONFIG(ENABLE_EVAL_CARD),
});

static const struct sm_object slot0_force_pwr = SM_DECLARE_BOOL({
	.opt_name	= OPTION_NAME_PCIE_SLOT0_FORCE_PWR,
	.ui_name	= "PCIe Slot0 Force Power",
	.ui_helptext	= "Force power on PCIe Slot0",
	.default_value	= CONFIG(ENABLE_FORCE_POWER_GPP0) && CONFIG(PCIE_SLOT0_2X4),
}, WITH_DEP_VALUES(&slot0_enable, true));

static struct sm_obj_form pcie_force_pwr = {
	.ui_name = "PCIe Force Power",
	.obj_list = (const struct sm_object *[]) {
		&slot0_force_pwr,
		NULL
	},
};

static const struct sm_object slot1_pcie_mux = SM_DECLARE_ENUM({
	.opt_name	= OPTION_NAME_PCIE_BIF,
	.ui_name	= "[SSD0 : Slot_1 : WLAN] bifurcation",
	.ui_helptext	= "Select how PCIe is bifurcated",
#if CONFIG(ENABLE_NVME_4LANES)
	.default_value	= EC_PCIE_MUX_NVMEX4,
#elif CONFIG(ENABLE_PCIE_4LANES)
	.default_value	= EC_PCIE_MUX_SLOT1X4,
#elif CONFIG(ENABLE_NVME_PCIE_2LANES)
	.default_value	= EC_PCIE_MUX_M2_SLOT_2X2X,
#else
	.default_value	= EC_PCIE_MUX_M2_WLAN_2X2X,
#endif
	.values		= (const struct sm_enum_value[]) {
		{ "x4:x0:x0", EC_PCIE_MUX_NVMEX4},
		{ "x0:x4:x0", EC_PCIE_MUX_SLOT1X4},
		{ "x2:x0:x2", EC_PCIE_MUX_M2_WLAN_2X2X},
		{ "x2:x2:x0", EC_PCIE_MUX_M2_SLOT_2X2X},
		SM_ENUM_VALUE_END
	},
}, WITH_DEP_VALUES(&slot0_enable, true));

static const struct sm_object slot0_pcie_4x4x = SM_DECLARE_ENUM({
	.opt_name	= OPTION_NAME_SLOT0_PCIE_BIF,
	.ui_name	= "Slot0 (Eval slot) bifurcation",
	.ui_helptext	= "Select how PCIe is bifurcated",
#if CONFIG(PCIE_SLOT0_1X8)
	.default_value	= 0,
#else
	.default_value	= 1,
#endif
	.values		= (const struct sm_enum_value[]) {
		{ "x8", 0},
		{ "x4:x4", 1},
		SM_ENUM_VALUE_END
	},
}, WITH_DEP_VALUES(&slot0_enable, true));

static struct sm_obj_form pcie_enable = {
	.ui_name = "PCIe enablement",
	.obj_list = (const struct sm_object *[]) {
		&slot0_enable,
		NULL
	},
};

static struct sm_obj_form pcie_bif = {
	.ui_name = "PCIe bifurcation",
	.obj_list = (const struct sm_object *[]) {
		&slot0_pcie_4x4x,
		&slot1_pcie_mux,
		NULL
	},
};

static const struct sm_object xgbe_leds = SM_DECLARE_BOOL({
	.opt_name	= OPTION_NAME_XGBE_LEDS,
	.ui_name	= "Enable xGBE LEDs",
	.ui_helptext	= "When enabled UART2 and UART4 are deactivated.",
	.default_value	= CONFIG(XGBE_LED_TURN_ON),
#if !CONFIG(XGBE_EN)
	.flags		= CFR_OPTFLAG_SUPPRESS,
#endif
});

static const struct sm_object xgbe_p0_link_speed_led = SM_DECLARE_BOOL({
	.opt_name	= OPTION_NAME_XGBE_P0_LINK_LEDS,
	.ui_name	= "XGBE PORT-0 Link Speed LED",
	.default_value	= CONFIG(TURN_ON_PORT_0_LINK_SPEED_LED),
#if !CONFIG(XGBE_EN)
	.flags		= CFR_OPTFLAG_SUPPRESS,
#endif
}, WITH_DEP_VALUES(&xgbe_leds, true));

static const struct sm_object xgbe_p0_link_status_led = SM_DECLARE_BOOL({
	.opt_name	= OPTION_NAME_XGBE_P0_STATUS_LEDS,
	.ui_name	= "XGBE PORT-0 Link Status LED",
	.default_value	= CONFIG(TURN_ON_PORT_0_LINK_STATUS_LED),
#if !CONFIG(XGBE_EN)
	.flags		= CFR_OPTFLAG_SUPPRESS,
#endif
}, WITH_DEP_VALUES(&xgbe_leds, true));

static const struct sm_object xgbe_p1_link_speed_led = SM_DECLARE_BOOL({
	.opt_name	= OPTION_NAME_XGBE_P1_LINK_LEDS,
	.ui_name	= "XGBE PORT-1 Link Speed LED",
	.default_value	= CONFIG(TURN_ON_PORT_1_LINK_SPEED_LED),
#if !CONFIG(XGBE_EN)
	.flags		= CFR_OPTFLAG_SUPPRESS,
#endif
}, WITH_DEP_VALUES(&xgbe_leds, true));

static const struct sm_object xgbe_p1_link_status_led = SM_DECLARE_BOOL({
	.opt_name	= OPTION_NAME_XGBE_P1_STATUS_LEDS,
	.ui_name	= "XGBE PORT-1 Link Status LED",
	.default_value	= CONFIG(TURN_ON_PORT_1_LINK_STATUS_LED),
#if !CONFIG(XGBE_EN)
	.flags		= CFR_OPTFLAG_SUPPRESS,
#endif
}, WITH_DEP_VALUES(&xgbe_leds, true));

static const struct sm_object xgbe_p0_led_blink_rate = SM_DECLARE_ENUM({
	.opt_name	= OPTION_NAME_XGBE_P0_BLINK_RATE,
	.ui_name	= "PORT-0 TX/RX LED blink rate",
	.ui_helptext	= "Select how xGBE LEDs blink.",
#if CONFIG(PORT_0_TX_RX_LED_BLINK_RATE_42)
	.default_value	= 0,
#elif CONFIG(PORT_0_TX_RX_LED_BLINK_RATE_84)
	.default_value	= 1,
#elif CONFIG(PORT_0_TX_RX_LED_BLINK_RATE_170)
	.default_value	= 2,
#else
	.default_value	= 3,
#endif
	.values		= (const struct sm_enum_value[]) {
		{ "42ms", 0},
		{ "85ms", 1},
		{ "170ms", 2},
		{ "340ms", 3},
		SM_ENUM_VALUE_END
	},
#if !CONFIG(XGBE_EN)
	.flags		= CFR_OPTFLAG_SUPPRESS,
#endif
}, WITH_DEP_VALUES(&xgbe_leds, true));

static const struct sm_object xgbe_p1_led_blink_rate = SM_DECLARE_ENUM({
	.opt_name	= OPTION_NAME_XGBE_P1_BLINK_RATE,
	.ui_name	= "PORT-1 TX/RX LED blink rate",
	.ui_helptext	= "Select how xGBE LEDs blink.",
#if CONFIG(PORT_1_TX_RX_LED_BLINK_RATE_42)
	.default_value	= 0,
#elif CONFIG(PORT_1_TX_RX_LED_BLINK_RATE_84)
	.default_value	= 1,
#elif CONFIG(PORT_1_TX_RX_LED_BLINK_RATE_170)
	.default_value	= 2,
#else
	.default_value	= 3,
#endif
	.values		= (const struct sm_enum_value[]) {
		{ "42ms", 0},
		{ "85ms", 1},
		{ "170ms", 2},
		{ "340ms", 3},
		SM_ENUM_VALUE_END
	},
#if !CONFIG(XGBE_EN)
	.flags		= CFR_OPTFLAG_SUPPRESS,
#endif
}, WITH_DEP_VALUES(&xgbe_leds, true));

static struct sm_obj_form xgbe = {
	.ui_name = "xGBE",
	.obj_list = (const struct sm_object *[]) {
		&xgbe_leds,
		&xgbe_p0_link_speed_led,
		&xgbe_p1_link_speed_led,
		&xgbe_p0_link_status_led,
		&xgbe_p1_link_status_led,
		&xgbe_p0_led_blink_rate,
		&xgbe_p1_led_blink_rate,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&uart,
	&pcie_enable,
	&pcie_bif,
	&pcie_force_pwr,
	&xgbe,
	&i2c,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
