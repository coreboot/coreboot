package main

import (
	"fmt"
	"os"
	"strings"
)

const (
	PIRQI = 0
	PIRQJ = 1
	PIRQK = 2
	PIRQL = 3
	PIRQM = 4
	PIRQN = 5
	PIRQO = 6
	PIRQP = 7
	PIRQQ = 8
	PIRQR = 9
	PIRQS = 10
	PIRQT = 11
	PIRQU = 12
	PIRQV = 13
	PIRQW = 14
	PIRQX = 15
)

/* from sb/intel/lynxpoint/lp_gpio.c */
func lp_gpio_to_pirq(gpioNum uint16) int {
	var pirqmap = map[uint16] int {
		8: PIRQI,
		9: PIRQJ,
		10: PIRQK,
		13: PIRQL,
		14: PIRQM,
		45: PIRQN,
		46: PIRQO,
		47: PIRQP,
		48: PIRQQ,
		49: PIRQR,
		50: PIRQS,
		51: PIRQT,
		52: PIRQU,
		53: PIRQV,
		54: PIRQW,
		55: PIRQX,
	}
	pirq, valid := pirqmap[gpioNum]
	if (valid) {
		return pirq
	} else {
		return -1
	}
}

func conf0str(conf0 uint32) string {
	if (conf0 & 1) == 0 {
		return "GPIO_MODE_NATIVE"
	} else {
		s := []string{"GPIO_MODE_GPIO"}
		var gpio_output bool
		if ((conf0 >> 2) & 1) == 1 {
			s = append(s, "GPIO_DIR_INPUT")
			gpio_output = false
		} else {
			s = append(s, "GPIO_DIR_OUTPUT")
			gpio_output = true
		}
		if ((conf0 >> 3) & 1) == 1 {
			s = append(s, "GPIO_INVERT")
		}
		if ((conf0 >> 4) & 1) == 1 {
			s = append(s, "GPIO_IRQ_LEVEL")
		}
		if gpio_output {
			if ((conf0 >> 31) & 1) == 1 {
				s = append(s, "GPO_LEVEL_HIGH")
			} else {
				s = append(s, "GPO_LEVEL_LOW")
			}
		}
		return strings.Join(s, " | ")
	}
}

func lpgpio_preset(conf0 uint32, owner uint32, route uint32, irqen uint32, pirq uint32) string {
	if conf0 == 0xd { /* 0b1101: MODE_GPIO | INPUT | INVERT */
		if owner == 0 { /* OWNER_ACPI */
			if irqen == 0 && pirq == 0 {
				if route == 0 { /* SCI */
					return "GPIO_ACPI_SCI"
				} else {
					return "GPIO_ACPI_SMI"
				}
			}
			return ""
		} else { /* OWNER_GPIO */
			if route == 0 && irqen == 0 && pirq != 0 {
				return "GPIO_INPUT_INVERT"
			}
			return ""
		}
	}

	if conf0 == 0x5 && owner == 1 { /* 0b101: MODE_GPIO | INPUT, OWNER_GPIO */
		if route == 0 && irqen == 0 {
			if pirq == 1 {
				return "GPIO_PIRQ"
			} else {
				return "GPIO_INPUT"
			}
		}
		return ""
	}

	if owner == 1 && irqen == 1 {
		if route == 0 && pirq == 0 {
			if conf0 == 0x5 { /* 0b00101 */
				return "GPIO_IRQ_EDGE"
			}
			if conf0 == 0x15 { /* 0b10101 */
				return "GPIO_IRQ_LEVEL"
			}
		}
		return ""
	}
	return ""
}

func gpio_str(conf0 uint32, conf1 uint32, owner uint32, route uint32, irqen uint32, reset uint32, blink uint32, pirq uint32) string {
	s := []string{}
	s = append(s, fmt.Sprintf(".conf0 = %s", conf0str(conf0)))
	if conf1 != 0 {
		s = append(s, fmt.Sprintf(".conf1 = 0x%x", conf1))
	}
	if owner != 0 {
		s = append(s, ".owner = GPIO_OWNER_GPIO")
	}
	if route != 0 {
		s = append(s, ".route = GPIO_ROUTE_SMI")
	}
	if irqen != 0 {
		s = append(s, ".irqen = GPIO_IRQ_ENABLE")
	}
	if reset != 0 {
		s = append(s, ".reset = GPIO_RESET_RSMRST")
	}
	if blink != 0 {
		s = append(s, ".blink = GPO_BLINK")
	}
	if pirq != 0 {
		s = append(s, ".pirq = GPIO_PIRQ_APIC_ROUTE")
	}
	return strings.Join(s, ", ")
}

/* start addresses of GPIO registers */
const (
	GPIO_OWN        = 0x0
	GPIPIRQ2IOXAPIC = 0x10
	GPO_BLINK       = 0x18
	GPI_ROUT        = 0x30
	GP_RST_SEL      = 0x60
	GPI_IE          = 0x90
	GPnCONFIGA      = 0x100
	GPnCONFIGB      = 0x104
)

func PrintLPGPIO(gpio *os.File, inteltool InteltoolData) {
	for gpioNum := uint16(0); gpioNum <= 94; gpioNum++ {
		if gpioNum < 10 {
			fmt.Fprintf(gpio, "\t[%d]  = ", gpioNum)
		} else {
			fmt.Fprintf(gpio, "\t[%d] = ", gpioNum)
		}
		conf0 := inteltool.GPIO[GPnCONFIGA+gpioNum*8]
		conf1 := inteltool.GPIO[GPnCONFIGB+gpioNum*8]
		set := gpioNum / 32
		bit := gpioNum % 32
		/* owner only effective in GPIO mode */
		owner := (inteltool.GPIO[GPIO_OWN+set*4] >> bit) & 1
		route := (inteltool.GPIO[GPI_ROUT+set*4] >> bit) & 1
		irqen := (inteltool.GPIO[GPI_IE+set*4] >> bit) & 1
		reset := (inteltool.GPIO[GP_RST_SEL+set*4] >> bit) & 1
		var blink, pirq uint32
		/* blink only effective in GPIO output mode */
		if set == 0 {
			blink = (inteltool.GPIO[GPO_BLINK] >> bit) & 1
		} else {
			blink = 0
		}
		irqset := lp_gpio_to_pirq(gpioNum)
		if irqset >= 0 {
			pirq = (inteltool.GPIO[GPIPIRQ2IOXAPIC] >> uint(irqset)) & 1
		} else {
			pirq = 0
		}

		if (conf0 & 1) == 0 {
			fmt.Fprintf(gpio, "LP_GPIO_NATIVE,\n")
		} else if (conf0 & 4) == 0 {
			/* configured as output */
			if ((conf0 >> 31) & 1) == 0 {
				fmt.Fprintf(gpio, "LP_GPIO_OUT_LOW,\n")
			} else {
				fmt.Fprintf(gpio, "LP_GPIO_OUT_HIGH,\n")
			}
		} else if (conf1 & 4) != 0 {
			/* configured as input and sensing disabled */
			fmt.Fprintf(gpio, "LP_GPIO_UNUSED,\n")
		} else {
			is_preset := false
			if conf1 == 0 && reset == 0 && blink == 0 {
				preset := lpgpio_preset(conf0, owner, route, irqen, pirq)
				if preset != "" {
					fmt.Fprintf(gpio, "LP_%s,\n", preset)
					is_preset = true
				}
			}
			if !is_preset {
				fmt.Fprintf(gpio, "{ %s },\n", gpio_str(conf0, conf1, owner, route, irqen, reset, blink, pirq))
			}
		}
	}
}

func Lynxpoint_LP_GPIO(ctx Context, inteltool InteltoolData) {
	gpio := Create(ctx, "gpio.c")
	defer gpio.Close()

	AddROMStageFile("gpio.c", "")

	Add_SPDX(gpio, C, GPL2_only)
	gpio.WriteString(`#include <southbridge/intel/lynxpoint/lp_gpio.h>

const struct pch_lp_gpio_map mainboard_lp_gpio_map[] = {
`)
	PrintLPGPIO(gpio, inteltool)
	gpio.WriteString("\tLP_GPIO_END\n};\n")
}
