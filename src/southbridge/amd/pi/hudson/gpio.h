/*
 * This file is part of the coreboot project.
 *
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

#ifndef _HUDSON_GPIO_H_
#define _HUDSON_GPIO_H_

#include <southbridge/amd/common/amd_defs.h>
#include <types.h>

#define CROS_GPIO_DEVICE_NAME "AmdKern"

#define GPIO_PIN_STS		(1 << 16)
#define GPIO_OUTPUT_VALUE	(1 << 22)
#define GPIO_OUTPUT_ENABLE	(1 << 23)

#if CONFIG(SOUTHBRIDGE_AMD_PI_KERN)
/* GPIO_0 - GPIO_62 */
#define GPIO_BANK0_CONTROL (AMD_SB_ACPI_MMIO_ADDR + 0x1500)
#define   GPIO_0	(GPIO_BANK0_CONTROL + 0x00)
#define   GPIO_1	(GPIO_BANK0_CONTROL + 0x04)
#define   GPIO_2	(GPIO_BANK0_CONTROL + 0x08)
#define   GPIO_3	(GPIO_BANK0_CONTROL + 0x0C)
#define   GPIO_4	(GPIO_BANK0_CONTROL + 0x10)
#define   GPIO_5	(GPIO_BANK0_CONTROL + 0x14)
#define   GPIO_6	(GPIO_BANK0_CONTROL + 0x18)
#define   GPIO_7	(GPIO_BANK0_CONTROL + 0x1C)
#define   GPIO_8	(GPIO_BANK0_CONTROL + 0x20)
#define   GPIO_9	(GPIO_BANK0_CONTROL + 0x24)
#define   GPIO_10	(GPIO_BANK0_CONTROL + 0x28)
#define   GPIO_11	(GPIO_BANK0_CONTROL + 0x2C)
#define   GPIO_12	(GPIO_BANK0_CONTROL + 0x30)
#define   GPIO_13	(GPIO_BANK0_CONTROL + 0x34)
#define   GPIO_14	(GPIO_BANK0_CONTROL + 0x38)
#define   GPIO_15	(GPIO_BANK0_CONTROL + 0x3C)
#define   GPIO_16	(GPIO_BANK0_CONTROL + 0x40)
#define   GPIO_17	(GPIO_BANK0_CONTROL + 0x44)
#define   GPIO_18	(GPIO_BANK0_CONTROL + 0x48)
#define   GPIO_19	(GPIO_BANK0_CONTROL + 0x4C)
#define   GPIO_20	(GPIO_BANK0_CONTROL + 0x50)
#define   GPIO_21	(GPIO_BANK0_CONTROL + 0x54)
#define   GPIO_22	(GPIO_BANK0_CONTROL + 0x58)
#define   GPIO_23	(GPIO_BANK0_CONTROL + 0x5C)
#define   GPIO_24	(GPIO_BANK0_CONTROL + 0x60)
#define   GPIO_25	(GPIO_BANK0_CONTROL + 0x64)
#define   GPIO_26	(GPIO_BANK0_CONTROL + 0x68)
#define   GPIO_39	(GPIO_BANK0_CONTROL + 0x9C)
#define   GPIO_42	(GPIO_BANK0_CONTROL + 0xA8)

/* GPIO_64 - GPIO_127 */
#define GPIO_BANK1_CONTROL (AMD_SB_ACPI_MMIO_ADDR + 0x1600)
#define   GPIO_64	(GPIO_BANK1_CONTROL + 0x00)
#define   GPIO_65	(GPIO_BANK1_CONTROL + 0x04)
#define   GPIO_66	(GPIO_BANK1_CONTROL + 0x08)
#define   GPIO_67	(GPIO_BANK1_CONTROL + 0x0C)
#define   GPIO_68	(GPIO_BANK1_CONTROL + 0x10)
#define   GPIO_69	(GPIO_BANK1_CONTROL + 0x14)
#define   GPIO_70	(GPIO_BANK1_CONTROL + 0x18)
#define   GPIO_71	(GPIO_BANK1_CONTROL + 0x1C)
#define   GPIO_72	(GPIO_BANK1_CONTROL + 0x20)
#define   GPIO_74	(GPIO_BANK1_CONTROL + 0x28)
#define   GPIO_75	(GPIO_BANK1_CONTROL + 0x2C)
#define   GPIO_76	(GPIO_BANK1_CONTROL + 0x30)
#define   GPIO_84	(GPIO_BANK1_CONTROL + 0x50)
#define   GPIO_85	(GPIO_BANK1_CONTROL + 0x54)
#define   GPIO_86	(GPIO_BANK1_CONTROL + 0x58)
#define   GPIO_87	(GPIO_BANK1_CONTROL + 0x5C)
#define   GPIO_88	(GPIO_BANK1_CONTROL + 0x60)
#define   GPIO_89	(GPIO_BANK1_CONTROL + 0x64)
#define   GPIO_90	(GPIO_BANK1_CONTROL + 0x68)
#define   GPIO_91	(GPIO_BANK1_CONTROL + 0x6C)
#define   GPIO_92	(GPIO_BANK1_CONTROL + 0x70)
#define   GPIO_93	(GPIO_BANK1_CONTROL + 0x74)
#define   GPIO_95	(GPIO_BANK1_CONTROL + 0x7C)
#define   GPIO_96	(GPIO_BANK1_CONTROL + 0x80)
#define   GPIO_97	(GPIO_BANK1_CONTROL + 0x84)
#define   GPIO_98	(GPIO_BANK1_CONTROL + 0x88)
#define   GPIO_99	(GPIO_BANK1_CONTROL + 0x8C)
#define   GPIO_100	(GPIO_BANK1_CONTROL + 0x90)
#define   GPIO_101	(GPIO_BANK1_CONTROL + 0x94)
#define   GPIO_102	(GPIO_BANK1_CONTROL + 0x98)
#define   GPIO_113	(GPIO_BANK1_CONTROL + 0xC4)
#define   GPIO_114	(GPIO_BANK1_CONTROL + 0xC8)
#define   GPIO_115	(GPIO_BANK1_CONTROL + 0xCC)
#define   GPIO_116	(GPIO_BANK1_CONTROL + 0xD0)
#define   GPIO_117	(GPIO_BANK1_CONTROL + 0xD4)
#define   GPIO_118	(GPIO_BANK1_CONTROL + 0xD8)
#define   GPIO_119	(GPIO_BANK1_CONTROL + 0xDC)
#define   GPIO_120	(GPIO_BANK1_CONTROL + 0xE0)
#define   GPIO_121	(GPIO_BANK1_CONTROL + 0xE4)
#define   GPIO_122	(GPIO_BANK1_CONTROL + 0xE8)
#define   GPIO_126	(GPIO_BANK1_CONTROL + 0xF8)

/* GPIO_128 - GPIO_183 */
#define GPIO_BANK2_CONTROL (AMD_SB_ACPI_MMIO_ADDR + 0x1700)
#define   GPIO_129	(GPIO_BANK2_CONTROL + 0x04)
#define   GPIO_130	(GPIO_BANK2_CONTROL + 0x08)
#define   GPIO_131	(GPIO_BANK2_CONTROL + 0x0C)
#define   GPIO_132	(GPIO_BANK2_CONTROL + 0x10)
#define   GPIO_133	(GPIO_BANK2_CONTROL + 0x14)
#define   GPIO_134	(GPIO_BANK2_CONTROL + 0x18)
#define   GPIO_135	(GPIO_BANK2_CONTROL + 0x1C)
#define   GPIO_136	(GPIO_BANK2_CONTROL + 0x20)
#define   GPIO_137	(GPIO_BANK2_CONTROL + 0x24)
#define   GPIO_138	(GPIO_BANK2_CONTROL + 0x28)
#define   GPIO_139	(GPIO_BANK2_CONTROL + 0x2C)
#define   GPIO_140	(GPIO_BANK2_CONTROL + 0x30)
#define   GPIO_141	(GPIO_BANK2_CONTROL + 0x34)
#define   GPIO_142	(GPIO_BANK2_CONTROL + 0x38)
#define   GPIO_143	(GPIO_BANK2_CONTROL + 0x3C)
#define   GPIO_144	(GPIO_BANK2_CONTROL + 0x40)
#define   GPIO_145	(GPIO_BANK2_CONTROL + 0x44)
#define   GPIO_146	(GPIO_BANK2_CONTROL + 0x48)
#define   GPIO_147	(GPIO_BANK2_CONTROL + 0x4C)
#define   GPIO_148	(GPIO_BANK2_CONTROL + 0x50)
#endif	/* CONFIG(SOUTHBRIDGE_AMD_PI_KERN) */

typedef uint32_t gpio_t;

int gpio_get(gpio_t gpio_num);

#endif	/* _HUDSON_GPIO_H_ */
