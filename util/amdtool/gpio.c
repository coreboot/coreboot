/* amdtool - dump all registers on an AMD CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdbool.h>
#include <stdio.h>
#include "acpimmio.h"
#include "amdtool.h"
#include "smn.h"

#define AMD_IOMUX_MAX_FUNC_COUNT	4
#define AMD_IOMUX_SIZE			0x100
#define AMD_GPIO_BANK_SIZE		(0x100 / 4)

#define AMD_BRH_IOMUX_SMN_BASE		0x02D01000

static uint8_t *iomux_base;
static uint32_t *gpio_base;

struct gpio_group {
	const uint8_t *iomux_defaults;
	const char *const *gpio_names;
	const unsigned int gpio_bank_count;
	const uint32_t *gpio_defaults;
	const uint16_t *special_gpio_regs;
	const uint16_t special_gpio_regs_size;
	const uint16_t acpimmio_gpio_offset;
	const uint16_t acpimmio_iomux_offset;
};

/* For better readiability and less SLOC, we override the initialized values.
 * Hide the warnings, as they will overflow the screen and make it harder to
 * focus on real compielr errors and warnings.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverride-init"

const char *const kunlun_iomux_gpio_names[] = {
	[0 ... AMD_IOMUX_MAX_FUNC_COUNT * AMD_IOMUX_SIZE - 1] = "",
	[0x00 * 4] = "PWR_BTN_L",		"GPIO0",	"GPIO0",	"GPIO0",
	[0x01 * 4] = "SYS_RESET_L",		"GPIO1",	"GPIO1",	"GPIO1",
	[0x02 * 4] = "WAKE_L",			"GPIO2",	"GPIO2",	"GPIO2",
	[0x03 * 4] = "GPIO3",			"GPIO3",	"GPIO3",	"GPIO3",
	[0x04 * 4] = "GPIO4",			"SATA_ACT_L",	"GPIO4",	"GPIO4",
	[0x05 * 4] = "GPIO5",			"DEVSLP0",	"GPIO5",	"GPIO5",
	[0x06 * 4] = "GPIO6",			"DEVSLP1",	"GPIO6",	"GPIO6",
	[0x07 * 4] = "GPIO7",			"GPIO7",	"GPIO7",	"GPIO7",
	[0x0C * 4] = "PWRGD_OUT",		"GPIO12",	"GPIO12",	"GPIO12",
	[0x0D * 4] = "I2C4_SCL",		"GPIO13",	"GPIO13",	"GPIO13",
	[0x0E * 4] = "I2C4_SDA",		"GPIO14",	"GPIO14",	"GPIO14",
	[0x10 * 4] = "USB10_OC0_L",		"GPIO16",	"GPIO16",	"GPIO16",
	[0x11 * 4] = "USB11_OC1_L",		"GPIO17",	"GPIO17",	"GPIO17",
	[0x13 * 4] = "I2C5_SCL",		"SMBUS1_SCL",	"GPIO19",	"GPIO19",
	[0x14 * 4] = "I2C5_SDA",		"SMBUS1_SDA",	"GPIO20",	"GPIO20",
	[0x15 * 4] = "GPIO21",			"GPIO21",	"GPIO21",	"n/a",
	[0x16 * 4] = "GPIO22",			"n/a",		"GPIO22",	"GPIO22",
	[0x17 * 4] = "ESPI_RSTOUT_L",		"GPIO23",	"GPIO23",	"GPIO23",
	[0x18 * 4] = "SMERR_L",			"GPIO24",	"GPIO24",	"GPIO24",
	[0x1A * 4] = "PCIE_RST_L",		"GPIO26",	"GPIO26",	"GPIO26",
	[0x1C * 4] = "X48M_OUT",		"GPIO28",	"GPIO28",	"GPIO28",
	[0x4A * 4] = "ESPI_CLK2",		"GPIO74",	"GPIO74"	"GPIO74",
	[0x4B * 4] = "ESPI_CLK1",		"GPIO75",	"GPIO75",	"n/a",
	[0x4C * 4] = "GPIO76",			"SPI_TPM_CS_L",	"GPIO76",	"GPIO76",
	[0x56 * 4] = "GPIO86",			"GPIO86",	"LPC_SMI_L",	"GPIO86",
	[0x57 * 4] = "GPIO87",			"n/a",		"GPIO87",	"GPIO87",
	[0x58 * 4] = "GPIO88",			"n/a",		"GPIO88",	"GPIO88",
	[0x59 * 4] = "GENINT1_L",		"PM_INTR_L",	"GPIO89",	"GPIO89",
	[0x68 * 4] = "GPIO104",			"GPIO104",	"n/a",		"GPIO104",
	[0x69 * 4] = "GPIO105",			"GPIO105",	"n/a",		"GPIO105",
	[0x6A * 4] = "GPIO106",			"GPIO106",	"n/a",		"GPIO106",
	[0x6B * 4] = "GPIO107",			"GPIO107",	"n/a",		"GPIO107",
	[0x6C * 4] = "ESPI0_ALERT_D1",		"GPIO108",	"n/a",		"GPIO108",
	[0x6D * 4] = "GPIO109",			"GPIO109",	"n/a",		"GPIO109",
	[0x6E * 4] = "ESPI1_ALERT_D1",		"GPIO110",	"GPIO110",	"GPIO110",
	[0x73 * 4] = "GPIO115",			"CLK_REQ1_L",	"GPIO115",	"GPIO115",
	[0x74 * 4] = "GPIO116",			"CLK_REQ2_L",	"GPIO116",	"GPIO116",
	[0x75 * 4] = "ESPI_CLK0",		"GPIO117",	"GPIO117",	"GPIO117",
	[0x76 * 4] = "SPI_CS0_L",		"GPIO118",	"GPIO118",	"GPIO118",
	[0x77 * 4] = "SPI_CS1_L",		"GPIO119",	"GPIO119",	"GPIO119",
	[0x78 * 4] = "ESPI0_D0/SPI0_D0",	"GPIO120",	"GPIO120",	"GPIO120",
	[0x79 * 4] = "ESPI0_D1/SPI0_D1",	"GPIO121",	"GPIO121",	"GPIO121",
	[0x7A * 4] = "ESPI0_D2/SPI0_D2",	"GPIO122",	"GPIO122",	"GPIO122",
	[0x7B * 4] = "ESPI0_D3/SPI0_D3",	"GPIO123",	"GPIO123",	"GPIO123",
	[0x7C * 4] = "ESPI_CS0_L",		"GPIO124",	"GPIO124",	"GPIO124",
	[0x7D * 4] = "ESPI_CS1_L",		"GPIO125",	"GPIO125",	"GPIO125",
	[0x7E * 4] = "SPI_CS2_L",		"GPIO126",	"GPIO126",	"GPIO126",
	[0x81 * 4] = "ESPI_RSTIN_L",		"KBRST_L",	"GPIO129",	"GPIO129",
	[0x83 * 4] = "ESPI1_D0/SPI1_D0",	"GPIO131",	"GPIO131",	"GPIO131",
	[0x84 * 4] = "ESPI1_D1/SPI1_D1",	"GPIO132",	"GPIO132",	"GPIO132",
	[0x85 * 4] = "ESPI1_D2/SPI1_D2",	"GPIO133",	"GPIO133",	"GPIO133",
	[0x86 * 4] = "ESPI1_D3/SPI1_D3",	"GPIO134",	"GPIO134",	"GPIO134",
	[0x87 * 4] = "UART0_CTS_L",		"UART2_RXD",	"GPIO135",	"GPIO135",
	[0x88 * 4] = "UART0_RXD",		"GPIO136",	"GPIO136",	"GPIO136",
	[0x89 * 4] = "UART0_RTS_L",		"UART2_TXD",	"GPIO137",	"GPIO137",
	[0x8A * 4] = "UART0_TXD",		"GPIO138",	"GPIO138",	"GPIO138",
	[0x8B * 4] = "UART0_INTR",		"GPIO139",	"GPIO139",	"GPIO139",
	[0x8D * 4] = "UART1_RXD",		"GPIO141",	"GPIO141",	"GPIO141",
	[0x8E * 4] = "UART1_TXD",		"GPIO142",	"GPIO142",	"GPIO142",
	[0x91 * 4] = "I3C0_SCL",		"I2C0_SCL",	"SMBUS0_SCL",	"GPIO145",
	[0x92 * 4] = "I3C0_SDA",		"I2C0_SDA",	"SMBUS0_SDA",	"GPIO146",
	[0x93 * 4] = "I3C1_SCL",		"I2C1_SCL",	"GPIO147",	"GPIO147",
	[0x94 * 4] = "I3C1_SDA",		"I2C1_SDA",	"GPIO148",	"GPIO148",
	[0x95 * 4] = "I3C2_SCL",		"I2C2_SCL",	"GPIO149",	"GPIO149",
	[0x96 * 4] = "I3C2_SDA",		"I2C2_SDA",	"GPIO150",	"GPIO150",
	[0x97 * 4] = "I3C3_SCL",		"I2C3_SCL",	"GPIO151",	"GPIO151",
	[0x98 * 4] = "I3C3_SDA",		"I2C3_SDA",	"GPIO152",	"GPIO152",
};

const uint8_t kunlun_iomux_group_defaults[] = {
	[0 ...  AMD_IOMUX_SIZE - 1] = 0x00,
	[0x13] = 0x01,
	[0x14] = 0x01,
	[0x87] = 0x02,
	[0x89] = 0x02,
	[0x8A] = 0x01,
	[0x8E] = 0x01,
};

const uint32_t kunlun_gpio_group_defaults[] = {
	[0 ...  4 * AMD_GPIO_BANK_SIZE - 1] = 0,
	[0x0000]     = 0x00140000, 0x00140000, 0x00140000, 0x00140000,
	[0x0010 / 4] = 0x00140000, 0x00240000, 0x00240000, 0x00240000,
	[0x0020 / 4] = 0x00240000, 0x00240000, 0x00140000, 0x00140000,
	[0x0030 / 4] = 0x00040000, 0x00040000, 0x00040000, 0x00000000,
	[0x0040 / 4] = 0x00140000, 0x00140000, 0x00140000, 0x00040000,
	[0x0050 / 4] = 0x00040000, 0x00240000, 0x00240000, 0x00140000,
	[0x0060 / 4] = 0x00140000, 0x00000000, 0x00040000, 0x00240000,
	[0x0070 / 4] = 0x00240000, 0x00140000, 0x00140000, 0x00140000,
	[0x0080 / 4] = 0x00240000, 0x00000000, 0x00000000, 0x00000000,
	[0x00A0 / 4] = 0x00240000, 0x00000000, 0x00140000, 0x00000000,
	[0x0120 / 4] = 0x00000000, 0x00000000, 0x00240000, 0x00240000,
	[0x0130 / 4] = 0x00140000, 0x00000000, 0x00000000, 0x00000000,
	[0x0150 / 4] = 0x00000000, 0x00000000, 0x00240000, 0x00240000,
	[0x0160 / 4] = 0x00240000, 0x00140000, 0x00000001, 0x00000000,
	[0x01A0 / 4] = 0x00240000, 0x00240000, 0x00240000, 0x00240000,
	[0x01B0 / 4] = 0x00140000, 0x00240000, 0x00000000, 0x00000000,
	[0x01C0 / 4] = 0x00000000, 0x00000000, 0x00000000, 0x00140000,
	[0x01D0 / 4] = 0x00140000, 0x00240000, 0x00140000, 0x00140000,
	[0x01E0 / 4] = 0x00140000, 0x00140000, 0x00140000, 0x00140000,
	[0x01F0 / 4] = 0x00140000, 0x00140000, 0x00140000, 0x00000000,
	[0x0200 / 4] = 0x00000000, 0x00140000, 0x00000000, 0x00140000,
	[0x0210 / 4] = 0x00140000, 0x00140000, 0x00140000, 0x00240000,
	[0x0220 / 4] = 0x00240000, 0x00140000, 0x00140000, 0x00240000,
	[0x0230 / 4] = 0x00000000, 0x00240000, 0x00140000, 0x00000000,
	[0x0240 / 4] = 0x00040000, 0x00040000, 0x00040000, 0x00040000,
	[0x0250 / 4] = 0x00040000, 0x00040000, 0x00040000, 0x00040000,
	[0x0260 / 4] = 0x00040000, 0x00000000, 0x00000000, 0x00000000,
};


#pragma GCC diagnostic pop

const uint16_t kunlun_special_gpio_regs[] = {
	0x0fc, 0x1fc, 0x2f0, 0x02f4, 0x2f8, 0x2fc
};

const struct gpio_group kunlun_gpio_group = {
	.iomux_defaults		= kunlun_iomux_group_defaults,
	.gpio_names		= kunlun_iomux_gpio_names,
	.gpio_bank_count	= 4,
	.gpio_defaults		= kunlun_gpio_group_defaults,
	.special_gpio_regs	= kunlun_special_gpio_regs,
	.special_gpio_regs_size = ARRAY_SIZE(kunlun_special_gpio_regs),
	.acpimmio_gpio_offset	= 0x1500,
	.acpimmio_iomux_offset	= 0x0d00,
};

static const io_register_t fch_gpio_reg_fields[] = {
	{  0, 4, "DebounceTmrOut" },
	{  4, 1, "DebounceTmrOutUnit" },
	{  5, 2, "DebounceCntrl." },
	{  7, 1, "DebounceTmrLarge." },
	{  9, 3, "Trigger Type" },
	{ 11, 1, "Enable interrupt status" },
	{ 12, 1, "Enable interrupt delivery" },
	{ 13, 3, "Wake Control" },
	{ 16, 1, "Pin Status" },
	{ 17, 2, "DrvStrengthSel" },
	{ 19, 1, "Reserved" },
	{ 20, 1, "Pull Up Enable" },
	{ 21, 1, "Pull Down Enable" },
	{ 22, 1, "Output Value" },
	{ 23, 1, "Output Enable" },
	{ 24, 1, "SW Control In" },
	{ 25, 1, "SW Control Enable" },
	{ 25, 1, "RX Disable" },
	{ 27, 1, "Reserved" },
	{ 28, 1, "Interrupt Status" },
	{ 29, 1, "Wake Status" },
	{ 30, 1, "Less2SecSts" },
	{ 31, 1, "Less10SecSts" },
};

const char * const drive_strength[] = {
	"Unsupported",
	"60 Ohms",
	"40 Ohms",
	"80 Ohms"
};

const char * const wake_cntrl[] = {
	"S0i3",
	"S3",
	"S4/S5"
};

const char * const debounce_cntrl[] = {
	"No debounce",
	"Preserve low glitch",
	"Preserve high glitch",
	"Remove glitch"
};

const char * const trigger_type[] = {
	[0] = "High edge",
	[1] = "High level",
	[2] = "Low edge",
	[3] = "Low level",
	[4] = "Both edges",
	[5 ... 8] = "Reserved",
};

static void print_iomux_reg(uint16_t addr, uint8_t reg, const char *const *gpio_names)
{
	printf("IOMUXx%02x: 0x%02x       (%s)\n",
		addr, reg, gpio_names[addr * 4 + reg]);
}

static void print_iomux_diff(const uint8_t reg, const uint8_t def, const uint8_t diff,
			     const char *const *gpio_names)
{
	printf("IOMUXx%02x: 0x%02x       (%s) DEFAULT\n",
		reg, def, gpio_names[reg * 4 + def]);
	printf("IOMUXx%02x: 0x%02x       DIFF\n", reg, diff);
}

static void print_gpio_reg(uint16_t addr, uint32_t reg, bool verbose)
{
	size_t i;
	const char *attr;

	printf("GPIOx%04x: 0x%08"PRIx32"\n", addr * 4, reg);

	if (!verbose)
		return;

	for (i = 0; i < ARRAY_SIZE(fch_gpio_reg_fields); i++) {
		uint32_t val = reg >> fch_gpio_reg_fields[i].addr;
		val &= ((1 << fch_gpio_reg_fields[i].size) - 1);
		switch(i) {
		case 2: attr = debounce_cntrl[val]; break;
		case 4: attr = trigger_type[val]; break;
		case 7: attr = wake_cntrl[val]; break;
		case 9: attr = drive_strength[val]; break;
		default: attr = NULL; break;
		}
		if (attr)
			printf("0x%04x = %s (%s)\n", val, fch_gpio_reg_fields[i].name, attr);
		else
			printf("0x%04x = %s\n", val, fch_gpio_reg_fields[i].name);
	}
}

static void print_gpio_diff(const uint16_t reg, const uint32_t def, const uint32_t diff)
{
	printf("GPIOx%04x: 0x%08x       DEFAULT\n", reg * 4, def);
	printf("GPIOx%04x: 0x%08x       DIFF\n", reg * 4, diff);
}

static bool is_special_gpio_register(uint16_t reg, const struct gpio_group *sb_gpio_group)
{
	size_t i;

	for (i = 0; i < sb_gpio_group->special_gpio_regs_size; i++) {
		if (reg == sb_gpio_group->special_gpio_regs[i])
			return true;
	}

	return false;
}

int print_gpios(struct pci_dev *sb, int show_all, int show_diffs)
{
	size_t i;
	const struct gpio_group *sb_gpio_group = NULL;
	const uint8_t *acpi_mmio_bar;
	uint32_t acpi_mmio_smn_bar;
	uint32_t gpio_reg, gpio_diff;
	uint8_t iomux_reg, iomux_diff;
	int smbus_rev;
	bool use_smn = false;

	switch (sb->device_id) {
	case PCI_DEVICE_ID_AMD_FCH_LPC_2:
		smbus_rev = find_smbus_dev_rev(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FCH_SMB_2);
		if (smbus_rev == -1)
			return 1;

		switch (smbus_rev) {
		case 0x71:
			sb_gpio_group = &kunlun_gpio_group;
			acpi_mmio_smn_bar = AMD_BRH_IOMUX_SMN_BASE;
			use_smn = true;
			break;
		default:
			printf("Error: Dumping GPIOs on this southbridge is not (yet) supported.\n");
			return 1;
		}

		break;
	default:
		printf("Error: Dumping GPIOs on this southbridge is not (yet) supported.\n");
		return 1;
	}

	if (show_diffs && !show_all)
		printf("\n========== GPIO DIFFS ===========\n\n");
	else
		printf("\n============= GPIOS =============\n\n");

	acpi_mmio_bar = get_acpi_mmio_bar(sb);

	if (!acpi_mmio_bar && !use_smn)
		return 1;

	if (use_smn) {
		iomux_base = (uint8_t *)(uintptr_t)(acpi_mmio_smn_bar + sb_gpio_group->acpimmio_iomux_offset);
		gpio_base = (uint32_t *)(uintptr_t)(acpi_mmio_smn_bar + sb_gpio_group->acpimmio_gpio_offset);

		printf("ACPI MMIO IOMUX 0x%08x (SMN)\n\n", acpi_mmio_smn_bar + sb_gpio_group->acpimmio_iomux_offset);
		printf("ACPI MMIO GPIO 0x%08x (SMN)\n\n", acpi_mmio_smn_bar + sb_gpio_group->acpimmio_gpio_offset);
	} else {
		iomux_base = (uint8_t *)(acpi_mmio_bar + sb_gpio_group->acpimmio_iomux_offset);
		gpio_base = (uint32_t *)(acpi_mmio_bar + sb_gpio_group->acpimmio_gpio_offset);

		printf("ACPI MMIO IOMUX 0x%08lx (MEM)\n\n", (uintptr_t)acpi_mmio_bar + sb_gpio_group->acpimmio_iomux_offset);
		printf("ACPI MMIO GPIO 0x%08lx (MEM)\n\n", (uintptr_t)acpi_mmio_bar + sb_gpio_group->acpimmio_gpio_offset);
	}


	for (i = 0; i < AMD_IOMUX_SIZE; i++) {
		if (use_smn)
			iomux_reg = smn_read8((uint32_t)(uintptr_t)(iomux_base + i)) & 3;
		else
			iomux_reg = read8(iomux_base + i) & 3;

		if (show_all)
			print_iomux_reg(i, iomux_reg, sb_gpio_group->gpio_names);

		if (show_diffs) {
			iomux_diff = iomux_reg ^ sb_gpio_group->iomux_defaults[i];
			if (iomux_diff) {
				if (!show_all)
					print_iomux_reg(i, iomux_reg, sb_gpio_group->gpio_names);
				print_iomux_diff(i, sb_gpio_group->iomux_defaults[i],
						 iomux_diff, sb_gpio_group->gpio_names);
				if (!show_all)
					printf("\n");
			}
		}
	}

	for (i = 0; i < AMD_GPIO_BANK_SIZE * sb_gpio_group->gpio_bank_count; i++) {
		if (use_smn)
			gpio_reg = smn_read32((uint32_t)(uintptr_t)(gpio_base + i)) ;
		else
			gpio_reg = read32(gpio_base + i);

		print_gpio_reg(i, gpio_reg, false);

		if (show_diffs) {
			gpio_diff = gpio_reg ^ sb_gpio_group->gpio_defaults[i];
			if (gpio_diff)
				print_gpio_diff(i, sb_gpio_group->gpio_defaults[i], gpio_diff);
		}
	}

	if (show_all) {
		printf("\n========== GPIO CONFIG ===========\n\n");

		for (i = 0; i < AMD_GPIO_BANK_SIZE * sb_gpio_group->gpio_bank_count; i++) {
			if (is_special_gpio_register(i * 4, sb_gpio_group))
				continue;

			if (use_smn)
				gpio_reg = smn_read32((uint32_t)(uintptr_t)(gpio_base + i)) ;
			else
				gpio_reg = read32(gpio_base + i);

			print_gpio_reg(i, gpio_reg, true);
		}
	}

	return 0;
}
