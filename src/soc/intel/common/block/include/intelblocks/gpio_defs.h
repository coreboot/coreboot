/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_BLOCK_GPIO_DEFS_H_
#define _SOC_BLOCK_GPIO_DEFS_H_

#include <intelblocks/gpio.h>

#define PAD_CFG0_TX_STATE_BIT		0
#define PAD_CFG0_TX_STATE		(1 << PAD_CFG0_TX_STATE_BIT)
#define PAD_CFG0_RX_STATE_BIT		1
#define PAD_CFG0_RX_STATE		(1 << PAD_CFG0_RX_STATE_BIT)
#define PAD_CFG0_TX_DISABLE		(1 << 8)
#define PAD_CFG0_RX_DISABLE		(1 << 9)
#define PAD_CFG0_MODE_SHIFT		10
#define PAD_CFG0_MODE_MASK		(7 << 10)
#define  PAD_CFG0_MODE_GPIO		(0 << 10)
#define  PAD_CFG0_MODE_FUNC(x)		((x) << 10)
#define  PAD_CFG0_MODE_NF1		(1 << 10)
#define  PAD_CFG0_MODE_NF2		(2 << 10)
#define  PAD_CFG0_MODE_NF3		(3 << 10)
#define  PAD_CFG0_MODE_NF4		(4 << 10)
#define  PAD_CFG0_MODE_NF5		(5 << 10)
#define  PAD_CFG0_MODE_NF6		(6 << 10)
#define  PAD_CFG0_MODE_NF7		(7 << 10)
#define PAD_CFG0_ROUTE_MASK		(0xF << 17)
#define  PAD_CFG0_ROUTE_NMI		(1 << 17)
#define  PAD_CFG0_ROUTE_SMI		(1 << 18)
#define  PAD_CFG0_ROUTE_SCI		(1 << 19)
#define  PAD_CFG0_ROUTE_IOAPIC		(1 << 20)
#define PAD_CFG0_RXTENCFG_MASK		(3 << 21)
#define PAD_CFG0_RXINV_MASK		(1 << 23)
#define  PAD_CFG0_RX_POL_INVERT		(1 << 23)
#define  PAD_CFG0_RX_POL_NONE		(0 << 23)
#define  PAD_CFG0_PREGFRXSEL		(1 << 24)
#define PAD_CFG0_TRIG_MASK		(3 << 25)
#define  PAD_CFG0_TRIG_LEVEL		(0 << 25)
#define  PAD_CFG0_TRIG_EDGE_SINGLE	(1 << 25) /* controlled by RX_INVERT*/
#define  PAD_CFG0_TRIG_OFF		(2 << 25)
#define  PAD_CFG0_TRIG_EDGE_BOTH	(3 << 25)
#define PAD_CFG0_NAFVWE_ENABLE		(1 << 27)
#define PAD_CFG0_RXRAW1_MASK		(1 << 28)
#define PAD_CFG0_RXPADSTSEL_MASK	(1 << 29)
#define PAD_CFG0_RESET_MASK		(3 << 30)
#define  PAD_CFG0_LOGICAL_RESET_PWROK		(0U << 30)
#define  PAD_CFG0_LOGICAL_RESET_DEEP		(1U << 30)
#define  PAD_CFG0_LOGICAL_RESET_PLTRST		(2U << 30)
#define  PAD_CFG0_LOGICAL_RESET_RSMRST		(3U << 30)

/*
 * Use the fourth bit in IntSel field to indicate gpio
 * ownership. This field is RO and hence not used during
 * gpio configuration.
 */
#define PAD_CFG_OWN_GPIO_DRIVER		(1 << 4)
#define PAD_CFG_OWN_GPIO_ACPI		(0 << 4)
#define PAD_CFG_OWN_GPIO(own)		PAD_CFG_OWN_GPIO_##own

#define PAD_CFG1_IRQ_MASK		(0xff << 0)
#define PAD_CFG1_IOSTERM_MASK		(0x3 << 8)
#define PAD_CFG1_IOSTERM_SAME		(0x0 << 8)
#define PAD_CFG1_IOSTERM_DISPUPD	(0x1 << 8)
#define PAD_CFG1_IOSTERM_ENPD		(0x2 << 8)
#define PAD_CFG1_IOSTERM_ENPU		(0x3 << 8)
#define PAD_CFG1_PULL_MASK		(0xf << 10)
#define  PAD_CFG1_PULL_NONE		(0x0 << 10)
#define  PAD_CFG1_PULL_DN_5K		(0x2 << 10)
#define  PAD_CFG1_PULL_DN_20K		(0x4 << 10)
#define  PAD_CFG1_PULL_UP_1K		(0x9 << 10)
#define  PAD_CFG1_PULL_UP_5K		(0xa << 10)
#define  PAD_CFG1_PULL_UP_2K		(0xb << 10)
#define  PAD_CFG1_PULL_UP_20K		(0xc << 10)
#define  PAD_CFG1_PULL_UP_667		(0xd << 10)
#define  PAD_CFG1_PULL_NATIVE		(0xf << 10)
#if CONFIG(SOC_INTEL_COMMON_BLOCK_GPIO_IOSTANDBY)
/* Tx enabled driving last value driven, Rx enabled */
#define PAD_CFG1_IOSSTATE_TxLASTRxE	(0x0 << 14)
/* Tx enabled driving 0, Rx disabled and Rx driving 0 back to its controller
 * internally */
#define PAD_CFG1_IOSSTATE_Tx0RxDCRx0	(0x1 << 14)
/* Tx enabled driving 0, Rx disabled and Rx driving 1 back to its controller
 * internally */
#define PAD_CFG1_IOSSTATE_Tx0RxDCRx1	(0x2 << 14)
/* Tx enabled driving 1, Rx disabled and Rx driving 0 back to its controller
 * internally */
#define PAD_CFG1_IOSSTATE_Tx1RxDCRx0	(0x3 << 14)
/* Tx enabled driving 1, Rx disabled and Rx driving 1 back to its controller
 * internally */
#define PAD_CFG1_IOSSTATE_Tx1RxDCRx1	(0x4 << 14)
/* Tx enabled driving 0, Rx enabled */
#define PAD_CFG1_IOSSTATE_Tx0RxE	(0x5 << 14)
/* Tx enabled driving 1, Rx enabled */
#define PAD_CFG1_IOSSTATE_Tx1RxE	(0x6 << 14)
/* Hi-Z, Rx driving 0 back to its controller internally */
#define PAD_CFG1_IOSSTATE_HIZCRx0	(0x7 << 14)
/* Hi-Z, Rx driving 1 back to its controller internally */
#define PAD_CFG1_IOSSTATE_HIZCRx1	(0x8 << 14)
#define PAD_CFG1_IOSSTATE_TxDRxE	(0x9 << 14) /* Tx disabled, Rx enabled */
#define PAD_CFG1_IOSSTATE_IGNORE	(0xf << 14) /* Ignore Iostandby */
#define PAD_CFG1_IOSSTATE_MASK		(0xf << 14) /* mask to extract Iostandby bits */
#define PAD_CFG1_IOSSTATE_SHIFT	14 /* set Iostandby bits [17:14] */
#else /* CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO_IOSTANDBY */
#define PAD_CFG1_IOSSTATE_MASK		0
#endif /* CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO_IOSTANDBY */

#define PAD_CFG2_DEBEN			1
/* Debounce Duration = (2 ^ PAD_CFG2_DEBOUNCE_x_RTC) * RTC clock duration */
#define PAD_CFG2_DEBOUNCE_8_RTC		(0x3 << 1)
#define PAD_CFG2_DEBOUNCE_16_RTC	(0x4 << 1)
#define PAD_CFG2_DEBOUNCE_32_RTC	(0x5 << 1)
#define PAD_CFG2_DEBOUNCE_64_RTC	(0x6 << 1)
#define PAD_CFG2_DEBOUNCE_128_RTC	(0x7 << 1)
#define PAD_CFG2_DEBOUNCE_256_RTC	(0x8 << 1)
#define PAD_CFG2_DEBOUNCE_512_RTC	(0x9 << 1)
#define PAD_CFG2_DEBOUNCE_1K_RTC	(0xa << 1)
#define PAD_CFG2_DEBOUNCE_2K_RTC	(0xb << 1)
#define PAD_CFG2_DEBOUNCE_4K_RTC	(0xc << 1)
#define PAD_CFG2_DEBOUNCE_8K_RTC	(0xd << 1)
#define PAD_CFG2_DEBOUNCE_16K_RTC	(0xe << 1)
#define PAD_CFG2_DEBOUNCE_32K_RTC	(0xf << 1)
#define PAD_CFG2_DEBOUNCE_MASK		0x1f

/* voltage tolerance  0=3.3V default 1=1.8V tolerant */
#if CONFIG(SOC_INTEL_COMMON_BLOCK_GPIO_PADCFG_PADTOL)
#define PAD_CFG1_TOL_MASK		(0x1 << 25)
#define  PAD_CFG1_TOL_1V8		(0x1 << 25)
#endif /* CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO_PADCFG_PADTOL */

#define PAD_FUNC(value)		PAD_CFG0_MODE_##value
#define PAD_RESET(value)	PAD_CFG0_LOGICAL_RESET_##value
#define PAD_RX_POL(value)	PAD_CFG0_RX_POL_##value
#define PAD_IRQ_ROUTE(value)	PAD_CFG0_ROUTE_##value
#define PAD_TRIG(value)		PAD_CFG0_TRIG_##value
#define PAD_PULL(value)		PAD_CFG1_PULL_##value
#define PAD_LOCK(value)		GPIO_##value

/* Disable the input/output buffer of the pad */
#define PAD_CFG0_BUF_NO_DISABLE		(0)
#define PAD_CFG0_BUF_TX_DISABLE		PAD_CFG0_TX_DISABLE
#define PAD_CFG0_BUF_RX_DISABLE		PAD_CFG0_RX_DISABLE
#define PAD_CFG0_BUF_TX_RX_DISABLE	\
	(PAD_CFG0_TX_DISABLE | PAD_CFG0_RX_DISABLE)

#define PAD_BUF(value)		PAD_CFG0_BUF_##value

#if CONFIG(SOC_INTEL_COMMON_BLOCK_GPIO_IOSTANDBY)
#define PAD_IOSSTATE(value)	PAD_CFG1_IOSSTATE_##value
#define PAD_IOSTERM(value)	PAD_CFG1_IOSTERM_##value
#else
#define PAD_IOSSTATE(value)	0
#define PAD_IOSTERM(value)	0
#endif

#define PAD_IRQ_CFG(route, trig, inv)		\
		(PAD_IRQ_ROUTE(route) |		\
		PAD_TRIG(trig) |		\
		PAD_RX_POL(inv))

#if CONFIG(SOC_INTEL_COMMON_BLOCK_GPIO_DUAL_ROUTE_SUPPORT)
#define PAD_IRQ_CFG_DUAL_ROUTE(route1, route2, trig, inv)	\
		(PAD_IRQ_ROUTE(route1) |	\
		PAD_IRQ_ROUTE(route2) |		\
		PAD_TRIG(trig) |		\
		PAD_RX_POL(inv))
#endif /* CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO_DUAL_ROUTE_SUPPORT */

#define _PAD_CFG_STRUCT(__pad, __config0, __config1)	\
	{					        \
		.pad = __pad,				\
		.pad_config[0] = __config0,		\
		.pad_config[1] = __config1,		\
		.lock_action = PAD_LOCK(UNLOCK),	\
	}

#define _PAD_CFG_STRUCT_LOCK(__pad, __config0, __config1, __action)	\
	{					\
		.pad = __pad,			\
		.pad_config[0] = __config0,	\
		.pad_config[1] = __config1,	\
		.lock_action = __action,	\
	}

#if GPIO_NUM_PAD_CFG_REGS > 2
#define _PAD_CFG_STRUCT_3(__pad, __config0, __config1, __config2)	\
	{					\
		.pad = __pad,			        \
		.pad_config[0] = __config0,	        \
		.pad_config[1] = __config1,	        \
		.pad_config[2] = __config2,	        \
		.lock_action = PAD_LOCK(UNLOCK),	\
	}
#else
#define _PAD_CFG_STRUCT_3(__pad, __config0, __config1, __config2)	\
	_PAD_CFG_STRUCT(__pad, __config0, __config1)
#endif

/* Native function configuration */
#define PAD_CFG_NF(pad, pull, rst, func)			\
	_PAD_CFG_STRUCT(pad,					\
		PAD_RESET(rst) | PAD_FUNC(func),		\
		PAD_PULL(pull) | PAD_IOSSTATE(TxLASTRxE))

/* Native function configuration with lock */
#define PAD_CFG_NF_LOCK(pad, pull, func, lock_action)		\
	_PAD_CFG_STRUCT_LOCK(pad,				\
		PAD_RESET(PWROK) | PAD_FUNC(func),		\
		PAD_PULL(pull) | PAD_IOSSTATE(TxLASTRxE),	\
		PAD_LOCK(lock_action))

#if CONFIG(SOC_INTEL_COMMON_BLOCK_GPIO_PADCFG_PADTOL)
/* Native 1.8V tolerant pad, only applies to some pads like I2C/I2S
    Not applicable to all SOCs. Refer EDS
 */
#define PAD_CFG_NF_1V8(pad, pull, rst, func)			\
	_PAD_CFG_STRUCT(pad,					\
		PAD_RESET(rst) | PAD_FUNC(func),		\
		PAD_PULL(pull) | PAD_IOSSTATE(TxLASTRxE) | PAD_CFG1_TOL_1V8)
#endif

/* Native function configuration for standby state */
#define PAD_CFG_NF_IOSSTATE(pad, pull, rst, func, iosstate)	\
	_PAD_CFG_STRUCT(pad,					\
		PAD_RESET(rst) | PAD_FUNC(func),		\
		PAD_PULL(pull) | PAD_IOSSTATE(iosstate))

/* Native function configuration for standby state, also configuring
   iostandby as masked */
#define PAD_CFG_NF_IOSTANDBY_IGNORE(pad, pull, rst, func)	\
	_PAD_CFG_STRUCT(pad,					\
		PAD_RESET(rst) | PAD_FUNC(func),		\
		PAD_PULL(pull) | PAD_IOSSTATE(IGNORE))

/* Native function configuration for standby state, also configuring
   iosstate and iosterm */
#define PAD_CFG_NF_IOSSTATE_IOSTERM(pad, pull, rst, func, iosstate, iosterm)	\
	_PAD_CFG_STRUCT(pad,							\
		PAD_RESET(rst) | PAD_FUNC(func), PAD_PULL(pull) |		\
		PAD_IOSSTATE(iosstate) | PAD_IOSTERM(iosterm))

/* Native function configuration with "native function virtual wire
   messaging enable (NAFVWE_ENABLE)" */
#define PAD_CFG_NF_VWEN(pad, pull, rst, func)			\
	_PAD_CFG_STRUCT(pad,					\
		PAD_RESET(rst) | PAD_FUNC(func) | PAD_CFG0_NAFVWE_ENABLE,\
		PAD_PULL(pull) | PAD_IOSSTATE(TxLASTRxE))

/* General purpose output, no pullup/down. */
#define PAD_CFG_GPO(pad, val, rst)				\
	_PAD_CFG_STRUCT(pad,					\
		PAD_FUNC(GPIO) | PAD_RESET(rst) |		\
		PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE) | !!val,	\
		PAD_PULL(NONE) | PAD_IOSSTATE(TxLASTRxE))

/* General purpose output with lock, no pullup/down. */
#define PAD_CFG_GPO_LOCK(pad, val, lock_action)		\
	_PAD_CFG_STRUCT_LOCK(pad,				\
	PAD_FUNC(GPIO) | PAD_RESET(PWROK) |			\
	PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE) | !!val,		\
	PAD_PULL(NONE) | PAD_IOSSTATE(TxLASTRxE),		\
	PAD_LOCK(lock_action))

/* General purpose output, with termination specified */
#define PAD_CFG_TERM_GPO(pad, val, pull, rst)			\
	_PAD_CFG_STRUCT(pad,					\
		PAD_FUNC(GPIO) | PAD_RESET(rst) |		\
		PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE) | !!val,	\
		PAD_PULL(pull) | PAD_IOSSTATE(TxLASTRxE))

/* General purpose output, no pullup/down. */
#define PAD_CFG_GPO_GPIO_DRIVER(pad, val, rst, pull)		\
	_PAD_CFG_STRUCT(pad,					\
		PAD_FUNC(GPIO) | PAD_RESET(rst) |		\
		PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE) | !!val,	\
		PAD_PULL(pull) | PAD_IOSSTATE(TxLASTRxE) |	\
		PAD_CFG_OWN_GPIO(DRIVER))

/* General purpose output. */
#define PAD_CFG_GPO_IOSSTATE_IOSTERM(pad, val, rst, pull, iosstate, ioterm)	\
	_PAD_CFG_STRUCT(pad,							\
		PAD_FUNC(GPIO) | PAD_RESET(rst) |				\
		PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE) | !!val,			\
		PAD_PULL(pull) | PAD_IOSSTATE(iosstate) | PAD_IOSTERM(ioterm))

/* General purpose input */
#define PAD_CFG_GPI(pad, pull, rst)					\
	_PAD_CFG_STRUCT(pad,						\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_BUF(TX_DISABLE),	\
		PAD_PULL(pull) | PAD_IOSSTATE(TxDRxE))

/* General purpose input with lock */
#define PAD_CFG_GPI_LOCK(pad, pull, lock_action)				\
	_PAD_CFG_STRUCT_LOCK(pad,						\
		PAD_FUNC(GPIO) | PAD_RESET(PWROK) | PAD_BUF(TX_DISABLE),	\
		PAD_PULL(pull) | PAD_IOSSTATE(TxDRxE),				\
		PAD_LOCK(lock_action))

#define PAD_CFG_GPI_TRIG_IOSSTATE_OWN(pad, pull, rst, trig, iosstate, own)		\
	_PAD_CFG_STRUCT(pad,								\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_TRIG(trig) | PAD_BUF(TX_DISABLE),	\
		PAD_PULL(pull) | PAD_CFG_OWN_GPIO(own) | PAD_IOSSTATE(iosstate))

#define PAD_CFG_GPI_TRIG_IOS_OWN(pad, pull, rst, trig, iosstate, iosterm, own)		\
	_PAD_CFG_STRUCT(pad,								\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_TRIG(trig) | PAD_BUF(TX_DISABLE),	\
		PAD_PULL(pull) | PAD_CFG_OWN_GPIO(own) |				\
		PAD_IOSSTATE(iosstate) | PAD_IOSTERM(iosterm))

/*
 * General purpose input. The following macro sets the
 * Host Software Pad Ownership to GPIO Driver mode.
 */
#define PAD_CFG_GPI_TRIG_OWN(pad, pull, rst, trig, own)				\
	_PAD_CFG_STRUCT(pad,							\
		PAD_FUNC(GPIO) | PAD_RESET(rst) |				\
		PAD_TRIG(trig) | PAD_RX_POL(NONE) | PAD_BUF(TX_DISABLE),	\
		PAD_PULL(pull) | PAD_CFG_OWN_GPIO(own))

#define PAD_CFG_GPI_TRIG_OWN_LOCK(pad, pull, rst, trig, own, lock_action)	\
	_PAD_CFG_STRUCT_LOCK(pad,						\
		PAD_FUNC(GPIO) | PAD_RESET(rst) |				\
		PAD_TRIG(trig) | PAD_RX_POL(NONE) | PAD_BUF(TX_DISABLE),	\
		PAD_PULL(pull) | PAD_CFG_OWN_GPIO(own), PAD_LOCK(lock_action))

#define PAD_CFG_GPI_GPIO_DRIVER(pad, pull, rst)					\
	_PAD_CFG_STRUCT(pad,							\
		PAD_FUNC(GPIO) | PAD_RESET(rst) |				\
		PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE),				\
		PAD_PULL(pull) | PAD_CFG_OWN_GPIO(DRIVER) | PAD_IOSSTATE(TxDRxE))

#define PAD_CFG_GPI_GPIO_DRIVER_LOCK(pad, pull, lock_action)				\
	_PAD_CFG_STRUCT_LOCK(pad,							\
		PAD_FUNC(GPIO) | PAD_RESET(PWROK) |					\
		PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE),					\
		PAD_PULL(pull) | PAD_CFG_OWN_GPIO(DRIVER) | PAD_IOSSTATE(TxDRxE),	\
		PAD_LOCK(lock_action))

#define PAD_CFG_GPIO_DRIVER_HI_Z(pad, pull, rst, iosstate, iosterm)		\
	_PAD_CFG_STRUCT(pad,							\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_BUF(TX_RX_DISABLE),	\
		PAD_PULL(pull) | PAD_CFG_OWN_GPIO(DRIVER) |			\
		PAD_IOSSTATE(iosstate) | PAD_IOSTERM(iosterm))

#define PAD_CFG_GPIO_HI_Z(pad, pull, rst, iosstate, iosterm)			\
	_PAD_CFG_STRUCT(pad,							\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_BUF(TX_RX_DISABLE),	\
		PAD_PULL(pull) | PAD_IOSSTATE(iosstate) | PAD_IOSTERM(iosterm))

/* GPIO Interrupt */
#define PAD_CFG_GPI_INT(pad, pull, rst, trig)	\
		PAD_CFG_GPI_TRIG_OWN(pad, pull, rst, trig, DRIVER)

/* GPIO Interrupt with lock */
#define PAD_CFG_GPI_INT_LOCK(pad, pull, trig, lock_action)	\
		PAD_CFG_GPI_TRIG_OWN_LOCK(pad, pull, PWROK, trig, DRIVER, lock_action)

/*
 * No Connect configuration for unconnected or unused pad.
 * Both TX and RX are disabled. RX disabling is done to avoid unnecessary
 * setting of GPI_STS and to prevent triggering the internal logic by floating
 * pads.
 */
#define PAD_NC(pad, pull)					\
	_PAD_CFG_STRUCT(pad,					\
		PAD_FUNC(GPIO) | PAD_RESET(DEEP) |		\
		PAD_TRIG(OFF) | PAD_BUF(TX_RX_DISABLE),		\
		PAD_PULL(pull) | PAD_IOSSTATE(TxDRxE))

/* No Connect configuration with lock */
#define PAD_NC_LOCK(pad, pull, lock_action)			\
	_PAD_CFG_STRUCT_LOCK(pad,				\
		PAD_RESET(PWROK) | PAD_FUNC(GPIO) |		\
		PAD_TRIG(OFF) | PAD_BUF(TX_RX_DISABLE),		\
		PAD_PULL(pull) | PAD_IOSSTATE(TxDRxE),		\
		PAD_LOCK(lock_action))

/* General purpose input, routed to APIC */
#define PAD_CFG_GPI_APIC(pad, pull, rst, trig, inv)				\
	_PAD_CFG_STRUCT(pad,							\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_BUF(TX_DISABLE) |		\
		PAD_IRQ_CFG(IOAPIC, trig, inv), PAD_PULL(pull) |		\
		PAD_IOSSTATE(TxDRxE))

/* General purpose input with lock, routed to APIC */
#define PAD_CFG_GPI_APIC_LOCK(pad, pull, trig, inv, lock_action)		\
	_PAD_CFG_STRUCT_LOCK(pad,						\
		PAD_FUNC(GPIO) | PAD_RESET(PWROK) | PAD_BUF(TX_DISABLE) |	\
		PAD_IRQ_CFG(IOAPIC, trig, inv), PAD_PULL(pull) |		\
		PAD_IOSSTATE(TxDRxE),						\
		PAD_LOCK(lock_action))

/* General purpose input, routed to APIC - with IOStandby Config*/
#define PAD_CFG_GPI_APIC_IOS(pad, pull, rst, trig, inv, iosstate, iosterm)	\
	_PAD_CFG_STRUCT(pad,							\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_BUF(TX_DISABLE) |		\
		PAD_IRQ_CFG(IOAPIC, trig, inv), PAD_PULL(pull) |		\
		PAD_IOSSTATE(iosstate) | PAD_IOSTERM(iosterm))

/*
 * The following APIC macros assume the APIC will handle the filtering
 * on its own end. One just needs to pass an active high message into the
 * ITSS.
 */
#define PAD_CFG_GPI_APIC_LOW(pad, pull, rst)	\
	PAD_CFG_GPI_APIC(pad, pull, rst, LEVEL, INVERT)

#define PAD_CFG_GPI_APIC_HIGH(pad, pull, rst)	\
	PAD_CFG_GPI_APIC(pad, pull, rst, LEVEL, NONE)

#define PAD_CFG_GPI_APIC_EDGE_LOW(pad, pull, rst)	\
	PAD_CFG_GPI_APIC(pad, pull, rst, EDGE_SINGLE, INVERT)

/* General purpose input, routed to SMI */
#define PAD_CFG_GPI_SMI(pad, pull, rst, trig, inv)				\
	_PAD_CFG_STRUCT(pad,							\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_BUF(TX_DISABLE) |		\
		PAD_IRQ_CFG(SMI, trig, inv), PAD_PULL(pull) |			\
		PAD_IOSSTATE(TxDRxE))

/* General purpose input, routed to SMI */
#define PAD_CFG_GPI_SMI_IOS(pad, pull, rst, trig, inv, iosstate, iosterm)	\
	_PAD_CFG_STRUCT(pad,							\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_BUF(TX_DISABLE) |		\
		PAD_IRQ_CFG(SMI, trig, inv), PAD_PULL(pull) |			\
		PAD_IOSSTATE(iosstate) | PAD_IOSTERM(iosterm))

#define PAD_CFG_GPI_SMI_LOW(pad, pull, rst, trig)	\
	PAD_CFG_GPI_SMI(pad, pull, rst, trig, INVERT)

#define PAD_CFG_GPI_SMI_HIGH(pad, pull, rst, trig)	\
	PAD_CFG_GPI_SMI(pad, pull, rst, trig, NONE)

/* General purpose input, routed to SCI */
#define PAD_CFG_GPI_SCI(pad, pull, rst, trig, inv)				\
	_PAD_CFG_STRUCT(pad,							\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_BUF(TX_DISABLE) |		\
		PAD_IRQ_CFG(SCI, trig, inv), PAD_PULL(pull) |			\
		PAD_IOSSTATE(TxDRxE))

/* General purpose input with lock, routed to SCI */
#define PAD_CFG_GPI_SCI_LOCK(pad, pull, trig, inv, lock_action)			\
	_PAD_CFG_STRUCT_LOCK(pad,						\
		PAD_FUNC(GPIO) | PAD_RESET(PWROK) | PAD_BUF(TX_DISABLE) |	\
		PAD_IRQ_CFG(SCI, trig, inv), PAD_PULL(pull) |			\
		PAD_IOSSTATE(TxDRxE), PAD_LOCK(lock_action))

/* General purpose input, routed to SCI */
#define PAD_CFG_GPI_SCI_IOS(pad, pull, rst, trig, inv, iosstate, iosterm)	\
	_PAD_CFG_STRUCT(pad,							\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_BUF(TX_DISABLE) |		\
		PAD_IRQ_CFG(SCI, trig, inv), PAD_PULL(pull) |			\
		PAD_IOSSTATE(iosstate) | PAD_IOSTERM(iosterm))

#define PAD_CFG_GPI_SCI_LOW(pad, pull, rst, trig) \
	PAD_CFG_GPI_SCI(pad, pull, rst, trig, INVERT)

#define PAD_CFG_GPI_SCI_LOW_LOCK(pad, pull, trig, lock_action) \
	PAD_CFG_GPI_SCI_LOCK(pad, pull, trig, INVERT, lock_action)

#define PAD_CFG_GPI_SCI_HIGH(pad, pull, rst, trig) \
	PAD_CFG_GPI_SCI(pad, pull, rst, trig, NONE)

#define PAD_CFG_GPI_SCI_HIGH_LOCK(pad, pull, trig, lock_action) \
	PAD_CFG_GPI_SCI_LOCK(pad, pull, trig, NONE, lock_action)

#define PAD_CFG_GPI_SCI_DEBEN(pad, pull, rst, trig, inv, dur)			\
	_PAD_CFG_STRUCT_3(pad,							\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_BUF(TX_DISABLE) |		\
		PAD_IRQ_CFG(SCI, trig, inv), PAD_PULL(pull) |			\
		PAD_IOSSTATE(TxDRxE), PAD_CFG2_DEBEN | PAD_CFG2_##dur)

#define PAD_CFG_GPI_SCI_LOW_DEBEN(pad, pull, rst, trig, dur)	\
	PAD_CFG_GPI_SCI_DEBEN(pad, pull, rst, trig, INVERT, dur)

#define PAD_CFG_GPI_SCI_HIGH_DEBEN(pad, pull, rst, trig, dur)	\
	PAD_CFG_GPI_SCI_DEBEN(pad, pull, rst, trig, NONE, dur)

/* General purpose input, routed to NMI */
#define PAD_CFG_GPI_NMI(pad, pull, rst, trig, inv)				\
	_PAD_CFG_STRUCT(pad,							\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_BUF(TX_DISABLE) |		\
		PAD_IRQ_CFG(NMI, trig, inv), PAD_PULL(pull) |			\
		PAD_IOSSTATE(TxDRxE))

#if CONFIG(SOC_INTEL_COMMON_BLOCK_GPIO_DUAL_ROUTE_SUPPORT)
#define PAD_CFG_GPI_DUAL_ROUTE(pad, pull, rst, trig, inv, route1, route2)	\
	_PAD_CFG_STRUCT(pad,							\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_BUF(TX_DISABLE) |		\
		PAD_IRQ_CFG_DUAL_ROUTE(route1, route2,  trig, inv),		\
		PAD_PULL(pull) | PAD_IOSSTATE(TxDRxE))

#define PAD_CFG_GPI_DUAL_ROUTE_LOCK(pad, pull, rst, trig, inv, route1, route2, lock_action)   \
	_PAD_CFG_STRUCT_LOCK(pad,						\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_BUF(TX_DISABLE) |		\
		PAD_IRQ_CFG_DUAL_ROUTE(route1, route2,  trig, inv),		\
		PAD_PULL(pull) | PAD_IOSSTATE(TxDRxE),		                \
		PAD_LOCK(lock_action))

#define PAD_CFG_GPI_IRQ_WAKE(pad, pull, rst, trig, inv)		\
	PAD_CFG_GPI_DUAL_ROUTE(pad, pull, rst, trig, inv, IOAPIC, SCI)

#define PAD_CFG_GPI_IRQ_WAKE_LOCK(pad, pull, trig, inv, lock_action)		\
	PAD_CFG_GPI_DUAL_ROUTE_LOCK(pad, pull, PWROK, trig, inv, IOAPIC, SCI, lock_action)

#endif /* CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO_DUAL_ROUTE_SUPPORT */

#endif /* _SOC_BLOCK_GPIO_DEFS_H_ */
