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

typedef uint32_t gpio_t;

/* Update interrupt settings for given GPIO */
void gpio_set_interrupt(gpio_t gpio, uint32_t flags);

/* Return the interrupt status and clear if set. */
int gpio_interrupt_status(gpio_t gpio);

#endif /* __ACPI__ */
#endif /* __STONEYRIDGE_GPIO_H__ */
