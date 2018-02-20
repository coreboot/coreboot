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
#include <types.h>

#define GPIO_EDGEL_TRIG		(0 << 8)
#define GPIO_LEVEL_TRIG		(1 << 8)
#define GPIO_TRIGGER_MASK	(1 << 8)

#define GPIO_ACTIVE_HIGH	(0 << 9)
#define GPIO_ACTIVE_LOW		(1 << 9)
#define GPIO_ACTIVE_BOTH	(2 << 9)
#define GPIO_ACTIVE_MASK	(3 << 9)

#define GPIO_INT_STATUS_EN	(1 << 11)
#define GPIO_INT_DELIVERY_EN	(1 << 12)
#define GPIO_INTERRUPT_MASK	(3 << 11)

#define GPIO_PIN_STS		(1 << 16)
#define GPIO_PULLUP_ENABLE	(1 << 20)
#define GPIO_PULLDOWN_ENABLE	(1 << 21)
#define GPIO_OUTPUT_SHIFT	22
#define GPIO_OUTPUT_MASK	(1 << GPIO_OUTPUT_SHIFT)
#define GPIO_OUTPUT_ENABLE	(1 << 23)

#define GPIO_INT_STATUS		(1 << 28)
#define GPIO_WAKE_STATUS	(1 << 29)

/*
 * The definitions below should be used to make GPIO arrays compact and
 * easy to understand.
 */
#define INPUT		0
#define OUTPUT_H	(FCH_GPIO_OUTPUT_ENABLE | FCH_GPIO_OUTPUT_VALUE)
#define OUTPUT_L	FCH_GPIO_OUTPUT_ENABLE

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
#define GPIO_67_IOMUX_GPIOxx 0
#define GPIO_67_IOMUX_DEVSLP0 1
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

#define GPIO_OUTPUT_OUT_HIGH (FCH_GPIO_OUTPUT_ENABLE | FCH_GPIO_OUTPUT_VALUE)
#define GPIO_OUTPUT_OUT_LOW FCH_GPIO_OUTPUT_ENABLE

#define GPIO_PULL_PULL_UP FCH_GPIO_PULL_UP_ENABLE
#define GPIO_PULL_PULL_DOWN FCH_GPIO_PULL_DOWN_ENABLE
#define GPIO_PULL_PULL_NONE 0

/* Native function pad configuration */
#define PAD_NF(pin, func, pull) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## func, \
		.control = GPIO_PULL ## _ ## pull }
/* General purpose input pad configuration */
#define PAD_GPI(pin, pull) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = GPIO_PULL ## _ ## pull }
/* General purpose output pad configuration */
#define PAD_GPO(pin, direction) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = GPIO_OUTPUT ## _OUT_ ## direction }

typedef uint32_t gpio_t;

/* Update interrupt settings for given GPIO */
void gpio_set_interrupt(gpio_t gpio, uint32_t flags);

/* Return the interrupt status and clear if set. */
int gpio_interrupt_status(gpio_t gpio);

#endif /* __ACPI__ */
#endif /* __STONEYRIDGE_GPIO_H__ */
