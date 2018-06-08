/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __STONEYRIDGE_GPIO_H__
#define __STONEYRIDGE_GPIO_H__

#define GPIO_DEVICE_NAME	"AMD0030"
#define GPIO_DEVICE_DESC	"GPIO Controller"

#ifndef __ACPI__
#include <soc/iomap.h>
#include <soc/smi.h>
#include <types.h>

struct soc_amd_gpio {
	uint8_t gpio;
	uint8_t function;
	uint32_t control;
	uint32_t flags;
};

struct soc_amd_event {
	uint8_t gpio;
	uint8_t event;
};

#define GPIO_EDGE_TRIG		(0 << 8)
#define GPIO_LEVEL_TRIG		(1 << 8)
#define GPIO_TRIGGER_MASK	(1 << 8)

#define GPIO_ACTIVE_HIGH	(0 << 9)
#define GPIO_ACTIVE_LOW		(1 << 9)
#define GPIO_ACTIVE_BOTH	(2 << 9)
#define GPIO_ACTIVE_MASK	(3 << 9)

#define GPIO_INT_STATUS_EN	(1 << 11)
#define GPIO_INT_DELIVERY_EN	(1 << 12)
#define GPIO_INTERRUPT_MASK	(3 << 11)
#define GPIO_S0I3_WAKE_EN	(1 << 13)
#define GPIO_S3_WAKE_EN		(1 << 14)
#define GPIO_S4_S5_WAKE_EN	(1 << 15)

#define GPIO_PIN_STS		(1 << 16)
#define GPIO_PULLUP_ENABLE	(1 << 20)
#define GPIO_PULLDOWN_ENABLE	(1 << 21)
#define GPIO_OUTPUT_SHIFT	22
#define GPIO_OUTPUT_MASK	(1 << GPIO_OUTPUT_SHIFT)
#define GPIO_OUTPUT_VALUE	(1 << GPIO_OUTPUT_SHIFT)
#define GPIO_OUTPUT_ENABLE	(1 << 23)

#define GPIO_INT_STATUS		(1 << 28)
#define GPIO_WAKE_STATUS	(1 << 29)

/* GPIO_0 - GPIO_62 */
#define GPIO_BANK0_CONTROL(gpio) \
	(AMD_SB_ACPI_MMIO_ADDR + 0x1500 + ((gpio) * 4))
#define GPIO_0				0
#define GPIO_1				1
#define GPIO_2				2
#define GPIO_3				3
#define GPIO_4				4
#define GPIO_5				5
#define GPIO_6				6
#define GPIO_7				7
#define GPIO_8				8
#define GPIO_9				9
#define GPIO_10				10
#define GPIO_11				11
#define GPIO_12				12
#define GPIO_13				13
#define GPIO_14				14
#define GPIO_15				15
#define GPIO_16				16
#define GPIO_17				17
#define GPIO_18				18
#define GPIO_19				19
#define GPIO_20				20
#define GPIO_21				21
#define GPIO_22				22
#define GPIO_23				23
#define GPIO_24				24
#define GPIO_25				25
#define GPIO_26				26
#define GPIO_39				39
#define GPIO_40				40
#define GPIO_42				42

/* GPIO_64 - GPIO_127 */
#define GPIO_BANK1_CONTROL(gpio) \
	(AMD_SB_ACPI_MMIO_ADDR + 0x1600 + (((gpio) - 64) * 4))
#define GPIO_64				64
#define GPIO_65				65
#define GPIO_66				66
#define GPIO_67				67
#define GPIO_68				68
#define GPIO_69				69
#define GPIO_70				70
#define GPIO_71				71
#define GPIO_72				72
#define GPIO_74				74
#define GPIO_75				75
#define GPIO_76				76
#define GPIO_84				84
#define GPIO_85				85
#define GPIO_86				86
#define GPIO_87				87
#define GPIO_88				88
#define GPIO_89				89
#define GPIO_90				90
#define GPIO_91				91
#define GPIO_92				92
#define GPIO_93				93
#define GPIO_95				95
#define GPIO_96				96
#define GPIO_97				97
#define GPIO_98				98
#define GPIO_99				99
#define GPIO_100			100
#define GPIO_101			101
#define GPIO_102			102
#define GPIO_113			113
#define GPIO_114			114
#define GPIO_115			115
#define GPIO_116			116
#define GPIO_117			117
#define GPIO_118			118
#define GPIO_119			119
#define GPIO_120			120
#define GPIO_121			121
#define GPIO_122			122
#define GPIO_126			126

/* GPIO_128 - GPIO_183 */
#define GPIO_BANK2_CONTROL(gpio) \
	(AMD_SB_ACPI_MMIO_ADDR + 0x1700 + (((gpio) - 128) * 4))
/* GPIO_128 Reserved */
#define GPIO_129			129
#define GPIO_130			130
#define GPIO_131			131
#define GPIO_132			132
#define GPIO_133			133
#define GPIO_134			134
#define GPIO_135			135
#define GPIO_136			136
#define GPIO_137			137
#define GPIO_138			138
#define GPIO_139			139
#define GPIO_140			140
#define GPIO_141			141
#define GPIO_142			142
#define GPIO_143			143
#define GPIO_144			144
#define GPIO_145			145
#define GPIO_146			146
#define GPIO_147			147
#define GPIO_148			148

/* IOMUX function names and values generated from BKDG. */
#define GPIO_0_IOMUX_PWR_BTN_L 0
#define GPIO_0_IOMUX_GPIOxx 1
#define GPIO_1_IOMUX_SYS_RESET_L 0
#define GPIO_1_IOMUX_GPIOxx 1
#define GPIO_2_IOMUX_WAKE_L 0
#define GPIO_2_IOMUX_GPIOxx 1
#define GPIO_3_IOMUX_GPIOxx 0
#define GPIO_4_IOMUX_GPIOxx 0
#define GPIO_5_IOMUX_GPIOxx 0
#define GPIO_5_IOMUX_DEVSLP0_S5 1
#define GPIO_6_IOMUX_GPIOxx 0
#define GPIO_6_IOMUX_LDT_RST_L 1
#define GPIO_7_IOMUX_GPIOxx 0
#define GPIO_7_IOMUX_LDT_PWROK 1
#define GPIO_8_IOMUX_GPIOxx 0
#define GPIO_8_IOMUX_SerPortTX_OUT 1
#define GPIO_9_IOMUX_GPIOxx 0
#define GPIO_9_IOMUX_SerPortRX_OUT 1
#define GPIO_10_IOMUX_S0A3_GPIO 0
#define GPIO_10_IOMUX_GPIOxx 1
#define GPIO_11_IOMUX_GPIOxx 0
#define GPIO_11_IOMUX_USB_OC7_L 1
#define GPIO_12_IOMUX_IR_LED_L 0
#define GPIO_12_IOMUX_LLB_L 1
#define GPIO_12_IOMUX_GPIOxx 2
#define GPIO_13_IOMUX_USB_OC5_L 0
#define GPIO_13_IOMUX_GPIOxx 1
#define GPIO_14_IOMUX_USB_OC6_L 0
#define GPIO_14_IOMUX_GPIOxx 1
#define GPIO_15_IOMUX_IR_RX1 0
#define GPIO_15_IOMUX_GPIOxx 1
#define GPIO_16_IOMUX_USB_OC0_L 0
#define GPIO_16_IOMUX_TRST_L 1
#define GPIO_16_IOMUX_GPIOxx 2
#define GPIO_17_IOMUX_USB_OC1_L 0
#define GPIO_17_IOMUX_TDI 1
#define GPIO_17_IOMUX_GPIOxx 2
#define GPIO_18_IOMUX_USB_OC2_L 0
#define GPIO_18_IOMUX_TCK 1
#define GPIO_18_IOMUX_GPIOxx 2
#define GPIO_19_IOMUX_SCL1 0
#define GPIO_19_IOMUX_I2C3_SCL 1
#define GPIO_19_IOMUX_GPIOxx 2
#define GPIO_20_IOMUX_SDA1 0
#define GPIO_20_IOMUX_I2C3_SDA 1
#define GPIO_20_IOMUX_GPIOxx 2
#define GPIO_21_IOMUX_LPC_PD_L 0
#define GPIO_21_IOMUX_GPIOxx 1
#define GPIO_22_IOMUX_LPC_PME_L 0
#define GPIO_22_IOMUX_GPIOxx 1
#define GPIO_23_IOMUX_USB_OC4_L 0
#define GPIO_23_IOMUX_IR_RX0 1
#define GPIO_23_IOMUX_GPIOxx 2
#define GPIO_24_IOMUX_USB_OC3_L 0
#define GPIO_24_IOMUX_GPIOxx 1
#define GPIO_25_IOMUX_SD0_CD 0
#define GPIO_25_IOMUX_GPIOxx 1
#define GPIO_26_IOMUX_PCIE_RST_L 0
#define GPIO_26_IOMUX_GPIOxx 1
#define GPIO_39_IOMUX_VDDGFX_PD 0
#define GPIO_39_IOMUX_GPIOxx 1
#define GPIO_40_IOMUX_GPIOxx 0
#define GPIO_42_IOMUX_S5_MUX_CTRL 0
#define GPIO_42_IOMUX_GPIOxx 1
#define GPIO_64_IOMUX_GPIOxx 0
#define GPIO_65_IOMUX_GPIOxx 0
#define GPIO_66_IOMUX_GPIOxx 0
#define GPIO_67_IOMUX_GPIOxx 0
#define GPIO_67_IOMUX_DEVSLP0 1
#define GPIO_69_IOMUX_GPIOxx 0
#define GPIO_69_IOMUX_SGPIO_LOAD 1
#define GPIO_70_IOMUX_GPIOxx 0
#define GPIO_70_IOMUX_DEVSLP1 1
#define GPIO_74_IOMUX_LPCCLK0 0
#define GPIO_74_IOMUX_GPIOxx 1
#define GPIO_75_IOMUX_LPCCLK1 0
#define GPIO_75_IOMUX_GPIOxx 1
#define GPIO_76_IOMUX_GPIOxx 0
#define GPIO_76_IOMUX_SPI_TPM_CS_L 1
#define GPIO_84_IOMUX_FANIN0 0
#define GPIO_84_IOMUX_GPIOxx 1
#define GPIO_85_IOMUX_FANOUT0 0
#define GPIO_85_IOMUX_GPIOxx 1
#define GPIO_86_IOMUX_GPIOxx 1
#define GPIO_87_IOMUX_SERIRQ 0
#define GPIO_87_IOMUX_GPIOxx 1
#define GPIO_88_IOMUX_LPC_CLKRUN_L 0
#define GPIO_88_IOMUX_GPIOxx 1
#define GPIO_90_IOMUX_GPIOxx 0
#define GPIO_91_IOMUX_SPKR 0
#define GPIO_91_IOMUX_GPIOxx 1
#define GPIO_92_IOMUX_CLK_REQ0_L 0
#define GPIO_92_IOMUX_SATA_IS0_L 1
#define GPIO_92_IOMUX_SATA_ZP0_L 2
#define GPIO_92_IOMUX_GPIOxx 3
#define GPIO_93_IOMUX_SD0_LED 0
#define GPIO_93_IOMUX_GPIOxx 1
#define GPIO_95_IOMUX_GPIOxx 0
#define GPIO_96_IOMUX_GPIOxx 0
#define GPIO_97_IOMUX_GPIOxx 0
#define GPIO_98_IOMUX_GPIOxx 0
#define GPIO_99_IOMUX_GPIOxx 0
#define GPIO_100_IOMUX_GPIOxx 0
#define GPIO_101_IOMUX_SD0_WP 0
#define GPIO_101_IOMUX_GPIOxx 1
#define GPIO_102_IOMUX_SD0_PWR_CTRL 0
#define GPIO_102_IOMUX_GPIOxx 1
#define GPIO_113_IOMUX_SCL0 0
#define GPIO_113_IOMUX_I2C2_SCL 1
#define GPIO_113_IOMUX_GPIOxx 2
#define GPIO_114_IOMUX_SDA0 0
#define GPIO_114_IOMUX_I2C2_SDA 1
#define GPIO_114_IOMUX_GPIOxx 2
#define GPIO_115_IOMUX_CLK_REQ1_L 0
#define GPIO_115_IOMUX_GPIOxx 1
#define GPIO_116_IOMUX_CLK_REQ2_L 0
#define GPIO_116_IOMUX_GPIOxx 1
#define GPIO_117_IOMUX_ESPI_CLK 0
#define GPIO_117_IOMUX_GPIOxx 1
#define GPIO_118_IOMUX_SPI_CS1_L 0
#define GPIO_118_IOMUX_GPIOxx 1
#define GPIO_119_IOMUX_SPI_CS2_L 0
#define GPIO_119_IOMUX_ESPI_CS_L 1
#define GPIO_119_IOMUX_GPIOxx 2
#define GPIO_120_IOMUX_ESPI_DAT1 0
#define GPIO_120_IOMUX_GPIOxx 1
#define GPIO_121_IOMUX_ESPI_DAT0 0
#define GPIO_121_IOMUX_GPIOxx 1
#define GPIO_122_IOMUX_ESPI_DAT2 0
#define GPIO_122_IOMUX_GPIOxx 1
#define GPIO_126_IOMUX_GA20IN 0
#define GPIO_126_IOMUX_GPIOxx 1
#define GPIO_129_IOMUX_KBRST_L 0
#define GPIO_129_IOMUX_GPIOxx 1
#define GPIO_130_IOMUX_SATA_ACT_L 0
#define GPIO_130_IOMUX_GPIOxx 1
#define GPIO_131_IOMUX_CLK_REQ3_L 0
#define GPIO_131_IOMUX_SATA_IS1_L 1
#define GPIO_131_IOMUX_SATA_ZP1_L 2
#define GPIO_131_IOMUX_GPIOxx 3
#define GPIO_132_IOMUX_CLK_REQG_L 0
#define GPIO_132_IOMUX_OSCIN 1
#define GPIO_132_IOMUX_GPIOxx 2
#define GPIO_133_IOMUX_ESPI_DAT3 0
#define GPIO_133_IOMUX_GPIOxx 1
#define GPIO_135_IOMUX_UART0_CTS_L 0
#define GPIO_135_IOMUX_GPIOxx 1
#define GPIO_136_IOMUX_UART0_RXD 0
#define GPIO_136_IOMUX_GPIOxx 1
#define GPIO_137_IOMUX_UART0_RTS_L 0
#define GPIO_137_IOMUX_GPIOxx 1
#define GPIO_138_IOMUX_UART0_TXD 0
#define GPIO_138_IOMUX_GPIOxx 1
#define GPIO_139_IOMUX_UART0_INTR 0
#define GPIO_139_IOMUX_GPIOxx 1
#define GPIO_140_IOMUX_UART1_CTS_L 0
#define GPIO_140_IOMUX_GPIOxx 1
#define GPIO_141_IOMUX_UART1_RXD 0
#define GPIO_141_IOMUX_GPIOxx 1
#define GPIO_142_IOMUX_UART1_RTS_L 0
#define GPIO_142_IOMUX_GPIOxx 1
#define GPIO_143_IOMUX_UART1_TXD 0
#define GPIO_143_IOMUX_GPIOxx 1
#define GPIO_144_IOMUX_UART1_INTR 0
#define GPIO_144_IOMUX_GPIOxx 1
#define GPIO_145_IOMUX_I2C0_SCL 0
#define GPIO_145_IOMUX_GPIOxx 1
#define GPIO_146_IOMUX_I2C0_SDA 0
#define GPIO_146_IOMUX_GPIOxx 1
#define GPIO_147_IOMUX_I2C1_SCL 0
#define GPIO_147_IOMUX_GPIOxx 1
#define GPIO_148_IOMUX_I2C1_SDA 0
#define GPIO_148_IOMUX_GPIOxx 1

enum {
	GEVENT_0,
	GEVENT_1,
	GEVENT_2,
	GEVENT_3,
	GEVENT_4,
	GEVENT_5,
	GEVENT_6,
	GEVENT_7,
	GEVENT_8,
	GEVENT_9,
	GEVENT_10,
	GEVENT_11,
	GEVENT_12,
	GEVENT_13,
	GEVENT_14,
	GEVENT_15,
	GEVENT_16,
	GEVENT_17,
	GEVENT_18,
	GEVENT_19,
	GEVENT_20,
	GEVENT_21,
	GEVENT_22,
	GEVENT_23,
};

#define GPIO_OUTPUT_OUT_HIGH (GPIO_OUTPUT_ENABLE | GPIO_OUTPUT_VALUE)
#define GPIO_OUTPUT_OUT_LOW GPIO_OUTPUT_ENABLE

#define GPIO_PULL_PULL_UP GPIO_PULLUP_ENABLE
#define GPIO_PULL_PULL_DOWN GPIO_PULLDOWN_ENABLE
#define GPIO_PULL_PULL_NONE 0

#define AMD_GPIO_CONTROL_MASK			0x00f4ff00
#define AMD_GPIO_MUX_MASK			0x03

/* Definitions for PAD_INT. */
#define GPIO_INT_EDGE_HIGH		(GPIO_ACTIVE_HIGH | GPIO_EDGE_TRIG)
#define GPIO_INT_EDGE_LOW		(GPIO_ACTIVE_LOW | GPIO_EDGE_TRIG)
#define GPIO_INT_BOTH_EDGES		(GPIO_ACTIVE_BOTH | GPIO_EDGE_TRIG)
#define GPIO_INT_LEVEL_HIGH		(GPIO_ACTIVE_HIGH | GPIO_LEVEL_TRIG)
#define GPIO_INT_LEVEL_LOW		(GPIO_ACTIVE_LOW | GPIO_LEVEL_TRIG)

enum {
	GPIO_TRIGGER_LEVEL_LOW,
	GPIO_TRIGGER_LEVEL_HIGH,
	GPIO_TRIGGER_EDGE_LOW,
	GPIO_TRIGGER_EDGE_HIGH,
};

#define GPIO_TRIGGER_INVALID		-1
#define SCI_TRIGGER_EDGE		0
#define SCI_TRIGGER_LEVEL		1

#define GPIO_SPECIAL_FLAG		(1 << 31)
#define GPIO_DEBOUNCE_FLAG		(1 << 30)
#define GPIO_WAKE_FLAG			(1 << 29)
#define GPIO_INT_FLAG			(1 << 28)
#define GPIO_SMI_FLAG			(1 << 27)
#define GPIO_SCI_FLAG			(1 << 26)
#define GPIO_FLAG_DEBOUNCE		(GPIO_SPECIAL_FLAG | GPIO_DEBOUNCE_FLAG)
#define GPIO_FLAG_WAKE			(GPIO_SPECIAL_FLAG | GPIO_WAKE_FLAG)
#define GPIO_FLAG_INT			(GPIO_SPECIAL_FLAG | GPIO_INT_FLAG)
#define GPIO_FLAG_SCI			(GPIO_SPECIAL_FLAG | GPIO_SCI_FLAG)
#define GPIO_FLAG_SMI			(GPIO_SPECIAL_FLAG | GPIO_SMI_FLAG)

#define FLAGS_TRIGGER_MASK		0x00000003
#define GPIO_SPECIAL_MASK		0x7c000000
#define GPIO_DEBOUNCE_MASK		0x000000ff
#define INT_TRIGGER_MASK		0x00000700
#define INT_WAKE_MASK			0x0000e700
#define INT_SCI_SMI_MASK		0x00f40000

#define IN_GLITCH_SHIFT			5
#define GLITCH_LOW			1
#define GLITCH_HIGH			2
#define GLITCH_NONE			3
#define GPIO_IN_PRESERVE_LOW_GLITCH	(GLITCH_LOW << IN_GLITCH_SHIFT)
#define GPIO_IN_PRESERVE_HIGH_GLITCH	(GLITCH_HIGH << IN_GLITCH_SHIFT)
#define GPIO_IN_REMOVE_GLITCH		(GLITCH_NONE << IN_GLITCH_SHIFT)

#define GPIO_TIMEBASE_61uS		0
#define GPIO_TIMEBASE_183uS		(1 << 4)
#define GPIO_TIMEBASE_15560uS		(1 << 7)
#define GPIO_TIMEBASE_62440uS		(GPIO_TIMEBASE_183uS | \
					GPIO_TIMEBASE_15560uS)
#define GPIO_IN_60uS			(1 | GPIO_TIMEBASE_61uS)
#define GPIO_IN_120uS			(2 | GPIO_TIMEBASE_61uS)
#define GPIO_IN_200uS			(3 | GPIO_TIMEBASE_61uS)
#define GPIO_IN_500uS			(8 | GPIO_TIMEBASE_61uS)
#define GPIO_IN_1mS			(5 | GPIO_TIMEBASE_183uS)
#define GPIO_IN_2mS			(11 | GPIO_TIMEBASE_183uS)
#define GPIO_IN_15mS			(1 | GPIO_TIMEBASE_15560uS)
#define GPIO_IN_50mS			(3 | GPIO_TIMEBASE_15560uS)
#define GPIO_IN_100mS			(6 | GPIO_TIMEBASE_15560uS)
#define GPIO_IN_200mS			(13 | GPIO_TIMEBASE_15560uS)
#define GPIO_IN_500mS			(8 | GPIO_TIMEBASE_62440uS)

#define GPIO_IN_NO_DEBOUNCE		(DEBOUNCE_NONE << IN_GLITCH_SHIFT)
#define GPIO_IN_PRESERVE_LOW_GLITCH	(GLITCH_LOW << IN_GLITCH_SHIFT)
#define GPIO_IN_PRESERVE_HIGH_GLITCH	(GLITCH_HIGH << IN_GLITCH_SHIFT)
#define GPIO_IN_REMOVE_GLITCH		(GLITCH_NONE << IN_GLITCH_SHIFT)

#define GPIO_EVENT_INT_STATUS		GPIO_INT_STATUS_EN
#define GPIO_EVENT_INT_DELIVER		GPIO_INT_DELIVERY_EN
#define GPIO_EVENT_INT_STATUS_DELIVER	(GPIO_INT_STATUS_EN | \
					GPIO_INT_DELIVERY_EN)
#define GPIO_WAKE_S0i3			(1 << 13)
#define GPIO_WAKE_S3			(1 << 14)
#define GPIO_WAKE_S4_S5			(1 << 15)
#define GPIO_WAKE_S0i3_S4_S5		(GPIO_WAKE_S0i3 | GPIO_WAKE_S4_S5)
#define GPIO_WAKE_S3_S4_S5		(GPIO_WAKE_S3 | GPIO_WAKE_S4_S5)

/*
 * Several macros are available to declare programming of GPIO pins, and if
 * needed more than 1 macro can be used for any pin. However, some macros
 * will have no effect if combined. For example debounce only affects input
 * or one of the interrupts. Some macros should not be combined, such as SMI
 * and regular interrupt. The defined macros and their parameters are:
 * PAD_NF		Define native alternate function for the pin.
 *	pin		the pin to be programmed
 *	function	the native function
 *	pull		pull up, pull down or no pull
 * PAD_GPI		The pin is a GPIO input
 *	pin		the pin to be programmed
 *	pull		pull up, pull down or no pull
 * PAD_GPO		The pin is a GPIO output
 *	pin		the pin to be programmed
 *	direction	high or low
 * PAD_INT		The pin is regular interrupt that works while booting
 *	pin		the pin to be programmed
 *	pull		pull up, pull down or no pull
 *	trigger		LEVEL_LOW, LEVEL_HIGH, EDGE_LOW, EDGE_HIGH, BOTH_EDGES
 *	action		STATUS, DELIVER, STATUS_DELIVER
 * PAD_SCI		The pin is a SCI source
 *	pin		the pin to be programmed
 *	pull		pull up, pull down or no pull
 *	trigger		LEVEL_LOW, LEVEL_HIGH, EDGE_LOW, EDGE_HIGH
 * PAD_SMI		The pin is a SMI source
 *	pin		the pin to be programmed
 *	pull		pull up, pull down or no pull
 *	trigger		LEVEL_LOW, LEVEL_HIGH
 * PAD_WAKE		The pin can wake, use after PAD_INT or PAD_SCI
 *	pin		the pin to be programmed
 *	pull		pull up, pull down or no pull
 *	trigger		LEVEL_LOW, LEVEL_HIGH, EDGE_LOW, EDGE_HIGH, BOTH_EDGES
 *	type		S0i3, S3, S4_S5 or S4_S5 combinations (S0i3_S3 invalid)
 * PAD_DEBOUNCE		The input or interrupt will be debounced, invalid after
 *			PAD_NF
 *	pin		the pin to be programmed
 *	debounce_type	preserve low glitch, preserve high glitch, no glitch
 *	debounce_time	the debounce time
 */

/* Native function pad configuration */
#define PAD_NF(pin, func, pull) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## func, \
		.control = GPIO_PULL ## _ ## pull, \
		.flags = 0 }
/* General purpose input pad configuration */
#define PAD_GPI(pin, pull) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = GPIO_PULL ## _ ## pull, \
		.flags = 0 }
/* General purpose output pad configuration */
#define PAD_GPO(pin, direction) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = GPIO_OUTPUT ## _OUT_ ## direction, \
		.flags = 0 }
/* Auxiliary macro for legacy interrupt and wake */
#define PAD_AUX1(pull, trigger) (GPIO_PULL ## _ ## pull | \
				 GPIO_INT ## _ ## trigger)
/* Legacy interrupt pad configuration */
#define PAD_INT(pin, pull, trigger, action) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = (PAD_AUX1(pull, trigger) | \
			    GPIO_EVENT_INT ## _ ## action), \
		.flags = GPIO_FLAG_INT }
/* Auxiliary macro for SCI and SMI */
#define PAD_AUX2(trigger, flag) (GPIO_TRIGGER ## _ ## trigger | flag)
/* SCI pad configuration */
#define PAD_SCI(pin, pull, trigger) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = GPIO_PULL ## _ ## pull, \
		.flags = PAD_AUX2(trigger, GPIO_FLAG_SCI) }
/* SMI pad configuration */
#define PAD_SMI(pin, pull, trigger) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = GPIO_PULL ## _ ## pull, \
		.flags = PAD_AUX2(trigger, GPIO_FLAG_SMI) }
/* WAKE pad configuration */
#define PAD_WAKE(pin, pull, trigger, type) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = (PAD_AUX1(pull, trigger) | \
			    GPIO_WAKE ## _ ## type), \
		.flags = GPIO_FLAG_WAKE }
/* pin debounce configuration */
#define PAD_DEBOUNCE(pin, type, time) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = (GPIO_IN  ## _ ## type | GPIO_IN  ## _ ## time), \
		.flags = GPIO_FLAG_DEBOUNCE }

typedef uint32_t gpio_t;

/**
 * @brief program a particular set of GPIO
 *
 * @param gpio_list_ptr = pointer to array of gpio configurations
 * @param size = number of entries in array
 *
 * @return none
 */
void sb_program_gpios(const struct soc_amd_gpio *gpio_list_ptr, size_t size);

/* Return the interrupt status and clear if set. */
int gpio_interrupt_status(gpio_t gpio);

#endif /* __ACPI__ */
#endif /* __STONEYRIDGE_GPIO_H__ */
