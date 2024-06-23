/* code to generate common GPIO code for Intel 6/7/8 Series Chipset */

package main

import (
	"fmt"
	"os"
)

func writeGPIOSet(ctx Context, sb *os.File,
	val uint32, set uint, partno int, constraint uint32) {

	max := uint(32)
	if set == 3 {
		max = 12
	}

	bits := [6][2]string{
		{"GPIO_MODE_NATIVE", "GPIO_MODE_GPIO"},
		{"GPIO_DIR_OUTPUT", "GPIO_DIR_INPUT"},
		{"GPIO_LEVEL_LOW", "GPIO_LEVEL_HIGH"},
		{"GPIO_RESET_PWROK", "GPIO_RESET_RSMRST"},
		{"GPIO_NO_INVERT", "GPIO_INVERT"},
		{"GPIO_NO_BLINK", "GPIO_BLINK"},
	}

	for i := uint(0); i < max; i++ {
		if (constraint>>i)&1 == 1 {
			fmt.Fprintf(sb, "	.gpio%d = %s,\n",
				(set-1)*32+i,
				bits[partno][(val>>i)&1])
		}
	}
}

func GPIO(ctx Context, inteltool InteltoolData) {
	var constraint uint32
	gpio := Create(ctx, "gpio.c")
	defer gpio.Close()

	AddBootBlockFile("gpio.c", "")
	AddROMStageFile("gpio.c", "")

	Add_SPDX(gpio, C, GPL2_only)
	gpio.WriteString("#include <southbridge/intel/common/gpio.h>\n\n")

	addresses := [3][6]int{
		{0x00, 0x04, 0x0c, 0x60, 0x2c, 0x18},
		{0x30, 0x34, 0x38, 0x64, -1, -1},
		{0x40, 0x44, 0x48, 0x68, -1, -1},
	}

	for set := 1; set <= 3; set++ {
		for partno, part := range []string{"mode", "direction", "level", "reset", "invert", "blink"} {
			addr := addresses[set-1][partno]
			if addr < 0 {
				continue
			}
			fmt.Fprintf(gpio, "static const struct pch_gpio_set%d pch_gpio_set%d_%s = {\n",
				set, set, part)

			constraint = 0xffffffff
			switch part {
			case "direction":
				/* Ignored on native mode */
				constraint = inteltool.GPIO[uint16(addresses[set-1][0])]
			case "level":
				/* Level doesn't matter for input */
				constraint = inteltool.GPIO[uint16(addresses[set-1][0])]
				constraint &^= inteltool.GPIO[uint16(addresses[set-1][1])]
			case "reset":
				/* Only show reset */
				constraint = inteltool.GPIO[uint16(addresses[set-1][3])]
			case "invert":
				/* Only on input and only show inverted GPIO */
				constraint = inteltool.GPIO[uint16(addresses[set-1][0])]
				constraint &= inteltool.GPIO[uint16(addresses[set-1][1])]
				constraint &= inteltool.GPIO[uint16(addresses[set-1][4])]
			case "blink":
				/* Only on output and only show blinking GPIO */
				constraint = inteltool.GPIO[uint16(addresses[set-1][0])]
				constraint &^= inteltool.GPIO[uint16(addresses[set-1][1])]
				constraint &= inteltool.GPIO[uint16(addresses[set-1][5])]
			}
			writeGPIOSet(ctx, gpio, inteltool.GPIO[uint16(addr)], uint(set), partno, constraint)
			gpio.WriteString("};\n\n")
		}
	}

	gpio.WriteString(`const struct pch_gpio_map mainboard_gpio_map = {
	.set1 = {
		.mode		= &pch_gpio_set1_mode,
		.direction	= &pch_gpio_set1_direction,
		.level		= &pch_gpio_set1_level,
		.blink		= &pch_gpio_set1_blink,
		.invert		= &pch_gpio_set1_invert,
		.reset		= &pch_gpio_set1_reset,
	},
	.set2 = {
		.mode		= &pch_gpio_set2_mode,
		.direction	= &pch_gpio_set2_direction,
		.level		= &pch_gpio_set2_level,
		.reset		= &pch_gpio_set2_reset,
	},
	.set3 = {
		.mode		= &pch_gpio_set3_mode,
		.direction	= &pch_gpio_set3_direction,
		.level		= &pch_gpio_set3_level,
		.reset		= &pch_gpio_set3_reset,
	},
};
`)
}
