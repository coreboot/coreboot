/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008 by coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include "inteltool.h"

typedef struct { uint16_t addr; uint32_t def; } gpio_default_t;

static const io_register_t ich0_gpio_registers[] = {
	{ 0x00, 4, "GPIO_USE_SEL" },
	{ 0x04, 4, "GP_IO_SEL" },
	{ 0x08, 4, "RESERVED" },
	{ 0x0c, 4, "GP_LVL" },
	{ 0x10, 4, "RESERVED" },
	{ 0x14, 4, "GPO_TTL" },
	{ 0x18, 4, "GPO_BLINK" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 4, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "RESERVED" },
	{ 0x2c, 4, "GPI_INV" },
	{ 0x30, 4, "RESERVED" },
	{ 0x34, 4, "RESERVED" },
	{ 0x38, 4, "RESERVED" },
	{ 0x3C, 4, "RESERVED" }
};

static const io_register_t ich2_gpio_registers[] = {
	{ 0x00, 4, "GPIO_USE_SEL" },
	{ 0x04, 4, "GP_IO_SEL" },
	{ 0x08, 4, "RESERVED" },
	{ 0x0c, 4, "GP_LVL" },
	{ 0x10, 4, "RESERVED" },
	{ 0x14, 4, "GPO_TTL" },
	{ 0x18, 4, "GPO_BLINK" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 4, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "RESERVED" },
	{ 0x2c, 4, "GPI_INV" },
	{ 0x30, 4, "RESERVED" },
	{ 0x34, 4, "RESERVED" },
	{ 0x38, 4, "RESERVED" },
	{ 0x3C, 4, "RESERVED" }
};

static const io_register_t ich4_gpio_registers[] = {
	{ 0x00, 4, "GPIO_USE_SEL" },
	{ 0x04, 4, "GP_IO_SEL" },
	{ 0x08, 4, "RESERVED" },
	{ 0x0c, 4, "GP_LVL" },
	{ 0x10, 4, "RESERVED" },
	{ 0x14, 4, "GPO_TTL" },
	{ 0x18, 4, "GPO_BLINK" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 4, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "RESERVED" },
	{ 0x2c, 4, "GPI_INV" },
	{ 0x30, 4, "GPIO_USE_SEL2" },
	{ 0x34, 4, "GP_IO_SEL2" },
	{ 0x38, 4, "GP_LVL2" },
	{ 0x3C, 4, "RESERVED" }
};

static const io_register_t ich5_gpio_registers[] = {
	{ 0x00, 4, "GPIO_USE_SEL" },
	{ 0x04, 4, "GP_IO_SEL" },
	{ 0x08, 4, "RESERVED" },
	{ 0x0c, 4, "GP_LVL" },
	{ 0x10, 4, "RESERVED" },
	{ 0x14, 4, "GPO_TTL"},
	{ 0x18, 4, "GPO_BLINK"},
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 4, "RESERVED" },
	{ 0x2c, 4, "GPI_INV" },
	{ 0x30, 4, "GPIO_USE_SEL2" },
	{ 0x34, 4, "GP_IO_SEL2" },
	{ 0x38, 4, "GP_LVL2" },
};

static const io_register_t ich6_gpio_registers[] = {
	{ 0x00, 4, "GPIO_USE_SEL" },
	{ 0x08, 4, "RESERVED" },
	{ 0x0c, 4, "GP_LVL" },
	{ 0x10, 4, "RESERVED" },
	{ 0x14, 4, "RESERVED" },
	{ 0x18, 4, "GPO_BLINK" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 4, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "RESERVED" },
	{ 0x2c, 4, "GPI_INV" },
	{ 0x30, 4, "GPIO_USE_SEL2" },
	{ 0x34, 4, "GP_IO_SEL2" },
	{ 0x38, 4, "GP_LVL2" },
	{ 0x04, 4, "GP_IO_SEL" },
};

static const io_register_t ich7_gpio_registers[] = {
	{ 0x00, 4, "GPIO_USE_SEL" },
	{ 0x04, 4, "GP_IO_SEL" },
	{ 0x08, 4, "RESERVED" },
	{ 0x0c, 4, "GP_LVL" },
	{ 0x10, 4, "RESERVED" },
	{ 0x14, 4, "RESERVED" },
	{ 0x18, 4, "GPO_BLINK" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 4, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "RESERVED" },
	{ 0x2c, 4, "GPI_INV" },
	{ 0x30, 4, "GPIO_USE_SEL2" },
	{ 0x34, 4, "GP_IO_SEL2" },
	{ 0x38, 4, "GP_LVL2" },
	{ 0x3C, 4, "RESERVED" }
};

static const io_register_t ich8_gpio_registers[] = {
	{ 0x00, 4, "GPIO_USE_SEL" },
	{ 0x04, 4, "GP_IO_SEL" },
	{ 0x08, 4, "RESERVED" },
	{ 0x0c, 4, "GP_LVL" },
	{ 0x10, 4, "GPIO_USE_SEL Override (LOW)" },
	{ 0x14, 4, "RESERVED" },
	{ 0x18, 4, "GPO_BLINK" },
	{ 0x1c, 4, "GP_SER_BLINK" },
	{ 0x20, 4, "GP_SB_CMDSTS" },
	{ 0x24, 4, "GP_SB_DATA" },
	{ 0x28, 4, "RESERVED" },
	{ 0x2c, 4, "GPI_INV" },
	{ 0x30, 4, "GPIO_USE_SEL2" },
	{ 0x34, 4, "GP_IO_SEL2" },
	{ 0x38, 4, "GP_LVL2" },
	{ 0x3C, 4, "GPIO_USE_SEL Override (HIGH)" }
};

static const io_register_t ich9_gpio_registers[] = {
	{ 0x00, 4, "GPIO_USE_SEL" },
	{ 0x04, 4, "GP_IO_SEL" },
	{ 0x08, 4, "RESERVED" },
	{ 0x0c, 4, "GP_LVL" },
	{ 0x10, 4, "RESERVED" },
	{ 0x14, 4, "RESERVED" },
	{ 0x18, 4, "GPO_BLINK" },
	{ 0x1c, 4, "GP_SER_BLINK" },
	{ 0x20, 4, "GP_SB_CMDSTS" },
	{ 0x24, 4, "GP_SB_DATA" },
	{ 0x28, 4, "RESERVED" },
	{ 0x2c, 4, "GPI_INV" },
	{ 0x30, 4, "GPIO_USE_SEL2" },
	{ 0x34, 4, "GP_IO_SEL2" },
	{ 0x38, 4, "GP_LVL2" },
	{ 0x3C, 4, "RESERVED" }
};

static const io_register_t ich10_gpio_registers[] = {
	{ 0x00, 4, "GPIO_USE_SEL" },
	{ 0x04, 4, "GP_IO_SEL" },
	{ 0x08, 4, "RESERVED" },
	{ 0x0c, 4, "GP_LVL" },
	{ 0x10, 4, "RESERVED" },
	{ 0x14, 4, "RESERVED" },
	{ 0x18, 4, "GPO_BLINK" },
	{ 0x1c, 4, "GP_SER_BLINK" },
	{ 0x20, 4, "GP_SB_CMDSTS" },
	{ 0x24, 4, "GP_SB_DATA" },
	{ 0x28, 4, "RESERVED" },
	{ 0x2c, 4, "GPI_INV" },
	{ 0x30, 4, "GPIO_USE_SEL2" },
	{ 0x34, 4, "GP_IO_SEL2" },
	{ 0x38, 4, "GP_LVL2" },
	{ 0x3C, 4, "RESERVED" },
	{ 0x40, 4, "GPIO_USE_SEL3" },
	{ 0x44, 4, "GPIO_SEL3" },
	{ 0x48, 4, "GPIO_LVL3" },
	{ 0x4c, 4, "RESERVED" },
	{ 0x50, 4, "RESERVED" },
	{ 0x54, 4, "RESERVED" },
	{ 0x58, 4, "RESERVED" },
	{ 0x5c, 4, "RESERVED" },
	{ 0x60, 4, "GP_RST_SEL" },
	{ 0x64, 4, "RESERVED" },
	{ 0x68, 4, "RESERVED" },
	{ 0x6c, 4, "RESERVED" },
	{ 0x70, 4, "RESERVED" },
	{ 0x74, 4, "RESERVED" },
	{ 0x78, 4, "RESERVED" },
	{ 0x7c, 4, "RESERVED" },
};

static const io_register_t i631x_gpio_registers[] = {
	{ 0x00, 4, "GPIO_USE_SEL" },
	{ 0x04, 4, "GP_IO_SEL" },
	{ 0x08, 4, "RESERVED" },
	{ 0x0c, 4, "GP_LVL" },
	{ 0x10, 4, "RESERVED" },
	{ 0x14, 4, "RESERVED" },
	{ 0x18, 4, "GPO_BLINK" },
	{ 0x1c, 4, "RESERVED" },
	{ 0x20, 4, "RESERVED" },
	{ 0x24, 4, "RESERVED" },
	{ 0x28, 4, "RESERVED" },
	{ 0x2c, 4, "GPI_INV" },
	{ 0x30, 4, "GPIO_USE_SEL2" },
	{ 0x34, 4, "GP_IO_SEL2" },
	{ 0x38, 4, "GP_LVL2" },
};

static const io_register_t pch_gpio_registers[] = {
	{ 0x00, 4, "GPIO_USE_SEL" },
	{ 0x04, 4, "GP_IO_SEL" },
	{ 0x08, 4, "RESERVED" },
	{ 0x0c, 4, "GP_LVL" },
	{ 0x10, 4, "RESERVED" },
	{ 0x14, 4, "RESERVED" },
	{ 0x18, 4, "GPO_BLINK" },
	{ 0x1c, 4, "GP_SER_BLINK" },
	{ 0x20, 4, "GP_SB_CMDSTS" },
	{ 0x24, 4, "GP_SB_DATA" },
	{ 0x28, 2, "GPI_NMI_EN" },
	{ 0x2a, 2, "GPI_NMI_STS" },
	{ 0x2c, 4, "GPI_INV" },
	{ 0x30, 4, "GPIO_USE_SEL2" },
	{ 0x34, 4, "GP_IO_SEL2" },
	{ 0x38, 4, "GP_LVL2" },
	{ 0x3c, 4, "RESERVED" },
	{ 0x40, 4, "GPIO_USE_SEL3" },
	{ 0x44, 4, "GPIO_SEL3" },
	{ 0x48, 4, "GPIO_LVL3" },
	{ 0x4c, 4, "RESERVED" },
	{ 0x50, 4, "RESERVED" },
	{ 0x54, 4, "RESERVED" },
	{ 0x58, 4, "RESERVED" },
	{ 0x5c, 4, "RESERVED" },
	{ 0x60, 4, "GP_RST_SEL1" },
	{ 0x64, 4, "GP_RST_SEL2" },
	{ 0x68, 4, "GP_RST_SEL3" },
	{ 0x6c, 4, "RESERVED" },
	{ 0x70, 4, "RESERVED" },
	{ 0x74, 4, "RESERVED" },
	{ 0x78, 4, "RESERVED" },
	{ 0x7c, 4, "RESERVED" },
};
/* Default values for Cougar Point desktop chipsets */
static const gpio_default_t cp_pch_desktop_defaults[] = {
	{ 0x00, 0xb96ba1ff },
	{ 0x04, 0xf6ff6eff },
	{ 0x0c, 0x02fe0100 },
	{ 0x18, 0x00040000 },
	{ 0x28, 0x00000000 },
	{ 0x2c, 0x00000000 },
	{ 0x30, 0x020300ff },
	{ 0x34, 0x1f57fff4 },
	{ 0x38, 0xa4aa0007 },
	{ 0x40, 0x00000130 },
	{ 0x44, 0x00000ff0 },
	{ 0x48, 0x000000c0 },
	{ 0x60, 0x01000000 },
	{ 0x64, 0x00000000 },
	{ 0x68, 0x00000000 },
};
/* Default values for Cougar Point mobile chipsets */
static const gpio_default_t cp_pch_mobile_defaults[] = {
	{ 0x00, 0xb96ba1ff },
	{ 0x04, 0xf6ff6eff },
	{ 0x0c, 0x02fe0100 },
	{ 0x18, 0x00040000 },
	{ 0x28, 0x00000000 },
	{ 0x2c, 0x00000000 },
	{ 0x30, 0x020300fe },
	{ 0x34, 0x1f57fff4 },
	{ 0x38, 0xa4aa0007 },
	{ 0x40, 0x00000030 },
	{ 0x44, 0x00000ff0 },
	{ 0x48, 0x000000c0 },
	{ 0x60, 0x01000000 },
	{ 0x64, 0x00000000 },
	{ 0x68, 0x00000000 },
};
/* Default values for Panther Point desktop chipsets */
static const gpio_default_t pp_pch_desktop_defaults[] = {
	{ 0x00, 0xb96ba1ff },
	{ 0x04, 0xeeff6eff },
	{ 0x0c, 0x02fe0100 },
	{ 0x18, 0x00040000 },
	{ 0x28, 0x00000000 },
	{ 0x2c, 0x00000000 },
	{ 0x30, 0x020300ff },
	{ 0x34, 0x1f57fff4 },
	{ 0x38, 0xa4aa0007 },
	{ 0x40, 0x00000130 },
	{ 0x44, 0x00000ff0 },
	{ 0x48, 0x000000c0 },
	{ 0x60, 0x01000000 },
	{ 0x64, 0x00000000 },
	{ 0x68, 0x00000000 },
};
/* Default values for Panther Point mobile chipsets */
static const gpio_default_t pp_pch_mobile_defaults[] = {
	{ 0x00, 0xb96ba1ff },
	{ 0x04, 0xeeff6eff },
	{ 0x0c, 0x02fe0100 },
	{ 0x18, 0x00040000 },
	{ 0x28, 0x00000000 },
	{ 0x2c, 0x00000000 },
	{ 0x30, 0x020300fe },
	{ 0x34, 0x1f57fff4 },
	{ 0x38, 0xa4aa0007 },
	{ 0x40, 0x00000030 },
	{ 0x44, 0x00000ff0 },
	{ 0x48, 0x000000c0 },
	{ 0x60, 0x01000000 },
	{ 0x64, 0x00000000 },
	{ 0x68, 0x00000000 },
};

static uint16_t gpiobase;

static void print_reg(const io_register_t *const reg)
{
	switch (reg->size) {
	case 4:
		printf("gpiobase+0x%04x: 0x%08x (%s)\n",
			reg->addr, inl(gpiobase+reg->addr), reg->name);
		break;
	case 2:
		printf("gpiobase+0x%04x: 0x%04x     (%s)\n",
			reg->addr, inw(gpiobase+reg->addr), reg->name);
		break;
	case 1:
		printf("gpiobase+0x%04x: 0x%02x       (%s)\n",
			reg->addr, inb(gpiobase+reg->addr), reg->name);
		break;
	}
}

static uint32_t get_diff(const io_register_t *const reg, const uint32_t def)
{
	uint32_t gpio_diff = 0;
	switch (reg->size) {
	case 4:
		gpio_diff = def ^ inl(gpiobase+reg->addr);
		break;
	case 2:
		gpio_diff = (uint16_t)def ^ inw(gpiobase+reg->addr);
		break;
	case 1:
		gpio_diff = (uint8_t)def ^ inb(gpiobase+reg->addr);
		break;
	}
	return gpio_diff;
}

static void print_diff(const io_register_t *const reg,
		       const uint32_t def, const uint32_t diff)
{
	switch (reg->size) {
	case 4:
		printf("gpiobase+0x%04x: 0x%08x (%s) DEFAULT\n",
			reg->addr, def, reg->name);
		printf("gpiobase+0x%04x: 0x%08x (%s) DIFF\n",
			reg->addr, diff, reg->name);
		break;
	case 2:
		printf("gpiobase+0x%04x: 0x%04x     (%s) DEFAULT\n",
			reg->addr, def, reg->name);
		printf("gpiobase+0x%04x: 0x%04x     (%s) DIFF\n",
			reg->addr, diff, reg->name);
		break;
	case 1:
		printf("gpiobase+0x%04x: 0x%02x       (%s) DEFAULT\n",
			reg->addr, def, reg->name);
		printf("gpiobase+0x%04x: 0x%02x       (%s) DIFF\n",
			reg->addr, diff, reg->name);
		break;
	}
}

int print_gpios(struct pci_dev *sb, int show_all, int show_diffs)
{
	int i, j, size, defaults_size = 0;
	const io_register_t *gpio_registers;
	const gpio_default_t *gpio_defaults = NULL;
	uint32_t gpio_diff;

	if (show_diffs && !show_all)
		printf("\n========== GPIO DIFFS ===========\n\n");
	else
		printf("\n============= GPIOS =============\n\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_Z68:
	case PCI_DEVICE_ID_INTEL_P67:
	case PCI_DEVICE_ID_INTEL_H67:
	case PCI_DEVICE_ID_INTEL_Q65:
	case PCI_DEVICE_ID_INTEL_QS67:
	case PCI_DEVICE_ID_INTEL_Q67:
	case PCI_DEVICE_ID_INTEL_B65:
	case PCI_DEVICE_ID_INTEL_C202:
	case PCI_DEVICE_ID_INTEL_C204:
	case PCI_DEVICE_ID_INTEL_C206:
	case PCI_DEVICE_ID_INTEL_H61:
		gpiobase = pci_read_word(sb, 0x48) & 0xfffc;
		gpio_registers = pch_gpio_registers;
		size = ARRAY_SIZE(pch_gpio_registers);
		gpio_defaults = cp_pch_desktop_defaults;
		defaults_size = ARRAY_SIZE(cp_pch_desktop_defaults);
		break;
	case PCI_DEVICE_ID_INTEL_UM67:
	case PCI_DEVICE_ID_INTEL_HM65:
	case PCI_DEVICE_ID_INTEL_HM67:
	case PCI_DEVICE_ID_INTEL_QM67:
		gpiobase = pci_read_word(sb, 0x48) & 0xfffc;
		gpio_registers = pch_gpio_registers;
		size = ARRAY_SIZE(pch_gpio_registers);
		gpio_defaults = cp_pch_mobile_defaults;
		defaults_size = ARRAY_SIZE(cp_pch_mobile_defaults);
		break;
	case PCI_DEVICE_ID_INTEL_Z77:
	case PCI_DEVICE_ID_INTEL_Z75:
	case PCI_DEVICE_ID_INTEL_Q77:
	case PCI_DEVICE_ID_INTEL_Q75:
	case PCI_DEVICE_ID_INTEL_B75:
	case PCI_DEVICE_ID_INTEL_H77:
	case PCI_DEVICE_ID_INTEL_C216:
		gpiobase = pci_read_word(sb, 0x48) & 0xfffc;
		gpio_registers = pch_gpio_registers;
		size = ARRAY_SIZE(pch_gpio_registers);
		gpio_defaults = pp_pch_desktop_defaults;
		defaults_size = ARRAY_SIZE(pp_pch_desktop_defaults);
		break;
	case PCI_DEVICE_ID_INTEL_QM77:
	case PCI_DEVICE_ID_INTEL_QS77:
	case PCI_DEVICE_ID_INTEL_HM77:
	case PCI_DEVICE_ID_INTEL_UM77:
	case PCI_DEVICE_ID_INTEL_HM76:
	case PCI_DEVICE_ID_INTEL_HM75:
	case PCI_DEVICE_ID_INTEL_HM70:
		gpiobase = pci_read_word(sb, 0x48) & 0xfffc;
		gpio_registers = pch_gpio_registers;
		size = ARRAY_SIZE(pch_gpio_registers);
		gpio_defaults = pp_pch_mobile_defaults;
		defaults_size = ARRAY_SIZE(pp_pch_mobile_defaults);
		break;
	case PCI_DEVICE_ID_INTEL_ICH10R:
		gpiobase = pci_read_word(sb, 0x48) & 0xfffc;
		gpio_registers = ich10_gpio_registers;
		size = ARRAY_SIZE(ich10_gpio_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH9DH:
	case PCI_DEVICE_ID_INTEL_ICH9DO:
	case PCI_DEVICE_ID_INTEL_ICH9R:
	case PCI_DEVICE_ID_INTEL_ICH9:
	case PCI_DEVICE_ID_INTEL_ICH9M:
	case PCI_DEVICE_ID_INTEL_ICH9ME:
		gpiobase = pci_read_word(sb, 0x48) & 0xfffc;
		gpio_registers = ich9_gpio_registers;
		size = ARRAY_SIZE(ich9_gpio_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH8:
	case PCI_DEVICE_ID_INTEL_ICH8M:
		gpiobase = pci_read_word(sb, 0x48) & 0xfffc;
		gpio_registers = ich8_gpio_registers;
		size = ARRAY_SIZE(ich8_gpio_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH7:
	case PCI_DEVICE_ID_INTEL_ICH7M:
	case PCI_DEVICE_ID_INTEL_ICH7DH:
	case PCI_DEVICE_ID_INTEL_ICH7MDH:
	case PCI_DEVICE_ID_INTEL_NM10:
		gpiobase = pci_read_word(sb, 0x48) & 0xfffc;
		gpio_registers = ich7_gpio_registers;
		size = ARRAY_SIZE(ich7_gpio_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH6:
		gpiobase = pci_read_word(sb, 0x48) & 0xfffc;
		gpio_registers = ich6_gpio_registers;
		size = ARRAY_SIZE(ich6_gpio_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH5:
		gpiobase = pci_read_word(sb, 0x58) & 0xfffc;
		gpio_registers = ich5_gpio_registers;
		size = ARRAY_SIZE(ich5_gpio_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH4:
	case PCI_DEVICE_ID_INTEL_ICH4M:
		gpiobase = pci_read_word(sb, 0x58) & 0xfffc;
		gpio_registers = ich4_gpio_registers;
		size = ARRAY_SIZE(ich4_gpio_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH2:
		gpiobase = pci_read_word(sb, 0x58) & 0xfffc;
		gpio_registers = ich2_gpio_registers;
		size = ARRAY_SIZE(ich2_gpio_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH:
	case PCI_DEVICE_ID_INTEL_ICH0:
		gpiobase = pci_read_word(sb, 0x58) & 0xfffc;
		gpio_registers = ich0_gpio_registers;
		size = ARRAY_SIZE(ich0_gpio_registers);
		break;

	case PCI_DEVICE_ID_INTEL_I63XX:
		gpiobase = pci_read_word(sb, 0x48) & 0xfffc;
		gpio_registers = i631x_gpio_registers;
		size = ARRAY_SIZE(i631x_gpio_registers);
		break;

	case PCI_DEVICE_ID_INTEL_3400_DESKTOP:
	case PCI_DEVICE_ID_INTEL_3400_MOBILE:
	case PCI_DEVICE_ID_INTEL_P55:
	case PCI_DEVICE_ID_INTEL_PM55:
	case PCI_DEVICE_ID_INTEL_H55:
	case PCI_DEVICE_ID_INTEL_QM57:
	case PCI_DEVICE_ID_INTEL_H57:
	case PCI_DEVICE_ID_INTEL_HM55:
	case PCI_DEVICE_ID_INTEL_Q57:
	case PCI_DEVICE_ID_INTEL_HM57:
	case PCI_DEVICE_ID_INTEL_3400_MOBILE_SFF:
	case PCI_DEVICE_ID_INTEL_B55_A:
	case PCI_DEVICE_ID_INTEL_QS57:
	case PCI_DEVICE_ID_INTEL_3400:
	case PCI_DEVICE_ID_INTEL_3420:
	case PCI_DEVICE_ID_INTEL_3450:
	case PCI_DEVICE_ID_INTEL_B55_B:
		gpiobase = pci_read_word(sb, 0x48) & 0xfffc;
		gpio_registers = i631x_gpio_registers;
		size = ARRAY_SIZE(i631x_gpio_registers);
		break;

	case PCI_DEVICE_ID_INTEL_82371XX:
		printf("This southbridge has GPIOs in the PM unit.\n");
		return 1;
	case 0x1234: // Dummy for non-existent functionality
		printf("This southbridge does not have GPIOBASE.\n");
		return 1;
	default:
		printf("Error: Dumping GPIOs on this southbridge is not (yet) supported.\n");
		return 1;
	}

	printf("GPIOBASE = 0x%04x (IO)\n\n", gpiobase);

	j = 0;
	for (i = 0; i < size; i++) {
		if (show_all)
			print_reg(&gpio_registers[i]);

		if (show_diffs &&
		    (j < defaults_size) &&
		    (gpio_defaults[j].addr == gpio_registers[i].addr)) {
			gpio_diff = get_diff(&gpio_registers[i],
					     gpio_defaults[j].def);
			if (gpio_diff) {
				if (!show_all)
					print_reg(&gpio_registers[i]);
				print_diff(&gpio_registers[i],
					   gpio_defaults[j].def, gpio_diff);
				if (!show_all)
					printf("\n");
			}
			j++;
		}
	}

	return 0;
}
