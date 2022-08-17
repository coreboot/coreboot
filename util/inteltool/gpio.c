/* inteltool - dump all registers on an Intel CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

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
	{ 0x44, 4, "GP_IO_SEL3" },
	{ 0x48, 4, "GP_LVL3" },
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
	{ 0x44, 4, "GP_IO_SEL3" },
	{ 0x48, 4, "GP_LVL3" },
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
/* Default values for Ibex Peak desktop chipsets */
static const gpio_default_t ip_pch_desktop_defaults[] = {
	{ 0x00, 0xf96ba1ff }, /* GPIO_USE_SEL  */
	{ 0x04, 0xf6ff6eff }, /* GP_IO_SEL */
	{ 0x0c, 0x02fe0100 }, /* GP_LVL */
	{ 0x18, 0x00040000 }, /* GPO_BLINK */
	{ 0x1c, 0x00000000 }, /* GP_SER_BLINK */
	{ 0x28, 0x00000000 }, /* GP_NMI_EN + GPI_NMI_STS */
	{ 0x2c, 0x00000000 }, /* GP_INV */
	{ 0x30, 0x020300ff }, /* GPIO_USE_SEL2 */
	{ 0x34, 0x1f57fff4 }, /* GP_IO_SEL2 */
	{ 0x38, 0xa4aa0003 }, /* GP_LVL2 */
	{ 0x40, 0x00000100 }, /* GPIO_USE_SEL3 */
	{ 0x44, 0x00000f00 }, /* GP_IO_SEL3 */
	{ 0x48, 0x00000000 }, /* GP_LVL3 */
	{ 0x60, 0x01000000 }, /* GP_RST_SEL1 */
	{ 0x64, 0x00000000 }, /* GP_RST_SEL2 */
	{ 0x68, 0x00000000 }, /* GP_RST_SEL3 */
};
/* Default values for Ibex Peak mobile chipsets */
static const gpio_default_t ip_pch_mobile_defaults[] = {
	{ 0x00, 0xf96ba1ff }, /* GPIO_USE_SEL  */
	{ 0x04, 0xf6ff6eff }, /* GP_IO_SEL */
	{ 0x0c, 0x02fe0100 }, /* GP_LVL */
	{ 0x18, 0x00040000 }, /* GPO_BLINK */
	{ 0x1c, 0x00000000 }, /* GP_SER_BLINK */
	{ 0x28, 0x00000000 }, /* GP_NMI_EN + GPI_NMI_STS */
	{ 0x2c, 0x00000000 }, /* GP_INV */
	{ 0x30, 0x020300fe }, /* GPIO_USE_SEL2 */
	{ 0x34, 0x1f57fff4 }, /* GP_IO_SEL2 */
	{ 0x38, 0xa4aa0003 }, /* GP_LVL2 */
	{ 0x40, 0x00000000 }, /* GPIO_USE_SEL3 */
	{ 0x44, 0x00000f00 }, /* GP_IO_SEL3 */
	{ 0x48, 0x00000000 }, /* GP_LVL3 */
	{ 0x60, 0x01000000 }, /* GP_RST_SEL1 */
	{ 0x64, 0x00000000 }, /* GP_RST_SEL2 */
	{ 0x68, 0x00000000 }, /* GP_RST_SEL3 */
};

static const io_register_t lynxpoint_lp_gpio_registers[] = {
	{ 0x00, 4, "GPIO_OWN1" }, // GPIO Ownership
	{ 0x04, 4, "GPIO_OWN2" }, // GPIO Ownership
	{ 0x08, 4, "GPIO_OWN3" }, // GPIO Ownership
	{ 0x0c, 4, "RESERVED" },  // Reserved
	{ 0x10, 2, "GPIPRIOQ2IOXAPIC" }, // GPI PIRQ[X:I] to IOxAPIC[39:24] Enable
	{ 0x12, 2, "RESERVED" }, // Reserved
	{ 0x14, 4, "RESERVED" }, // Reserved
	{ 0x18, 4, "GPO_BLINK" }, // GPIO Blink Enable
	{ 0x1c, 4, "GP_SER_BLINK" }, // GP Serial Blink
	{ 0x20, 4, "GP_SB_CMDSTS" }, // GP Serial Blink Command Status
	{ 0x24, 4, "GP_SB_DATA" }, // GP Serial Blink Data
	{ 0x28, 2, "GPI_NMI_EN" }, // GPI NMI Enable
	{ 0x2a, 2, "GPI_NMI_STS" }, // GPI NMI Status
	{ 0x2c, 4, "RESERVED" }, // Reserved
	{ 0x30, 4, "GPI_ROUT" }, // GPI Interrupt Input Route
	{ 0x34, 4, "RESERVED" }, // Reserved
	{ 0x38, 4, "RESERVED" }, // Reserved
	{ 0x3C, 4, "RESERVED" }, // Reserved
	{ 0x40, 4, "RESERVED" }, // Reserved
	{ 0x44, 4, "RESERVED" }, // Reserved
	{ 0x48, 4, "RESERVED" }, // Reserved
	{ 0x4C, 4, "RESERVED" }, // Reserved
	{ 0x50, 4, "ALT_GPI_SMI_STS" }, // Alternate GPI SMI Status
	{ 0x54, 4, "ALT_GPI_SMI_EN" }, // Alternate GPI SMI Enable
	{ 0x58, 4, "RESERVED" }, // Reserved
	{ 0x5C, 4, "RESERVED" }, // Reserved
	{ 0x60, 4, "GP_RST_SEL1" }, // GPIO Reset Select 1
	{ 0x64, 4, "GP_RST_SEL2" }, // GPIO Reset Select 2
	{ 0x68, 4, "GP_RST_SEL3" }, // GPIO Reset Select 3
	{ 0x6c, 4, "RESERVED" }, // Reserved
	{ 0x70, 4, "RESERVED" }, // Reserved
	{ 0x74, 4, "RESERVED" }, // Reserved
	{ 0x78, 4, "RESERVED" }, // Reserved
	{ 0x7c, 4, "GPIO_GC" }, // GPIO Global Configuration
	{ 0x80, 4, "GPI_IS[31:0]" }, // GPI Interrupt Status [31:0]
	{ 0x84, 4, "GPI_IS[63:32]" }, // GPI Interrupt Status [63:32]
	{ 0x88, 4, "GPI_IS[94:64]" }, // GPI Interrupt Status [94:64]
	{ 0x8C, 4, "RESERVED" }, // Reserved
	{ 0x90, 4, "GPI_IE[31:0]" }, // GPI Interrupt Enable [31:0]
	{ 0x94, 4, "GPI_IE[63:32]" }, // GPI Interrupt Enable [63:32]
	{ 0x98, 4, "GPI_IE[94:64]" }, // GPI Interrupt Enable [94:64]
	{ 0x9C, 4, "RESERVED" }, // Reserved
/*	{ 0x100, 4, "GPnCONFIGA" }, // GPIO Configuration A Register (n = 0) */
/*	{ 0x104, 4, "GPnCONFIGB" }, // GPIO Configuration B Register (n = 0) */
/*	{ ... } GPIO size = 95 */
/*	{ 0x3f0, 4, "GPnCONFIGA" }, // GPIO Configuration A Register (n = 94) */
/*	{ 0x3f4, 4, "GPnCONFIGB" }, // GPIO Configuration B Register (n = 94) */

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

/* Baytrail */
static const io_register_t baytrail_score_ssus_gpio_registers[] = {
	{ 0x00, 4, "SC_USE_SEL_31_0_" },
	{ 0x04, 4, "SC_IO_SEL_31_0_" },
	{ 0x08, 4, "SC_GP_LVL_31_0_)" },
	{ 0x0C, 4, "SC_TPE_31_0_" },
	{ 0x10, 4, "SC_TNE_31_0_" },
	{ 0x14, 4, "SC_TS_31_0_" },
	{ 0x20, 4, "SC_USE_SEL_63_32_" },
	{ 0x24, 4, "SC_IO_SEL_63_32_" },
	{ 0x28, 4, "SC_GP_LVL_63_32_" },
	{ 0x2C, 4, "SC_TPE_63_32_" },
	{ 0x30, 4, "SC_TNE_63_32_" },
	{ 0x34, 4, "SC_TS_63_32_" },
	{ 0x40, 4, "SC_USE_SEL_95_64_" },
	{ 0x44, 4, "SC_IO_SEL_95_64_" },
	{ 0x48, 4, "SC_GP_LVL_95_64_" },
	{ 0x4C, 4, "SC_TPE_95_64_" },
	{ 0x50, 4, "SC_TNE_95_64_" },
	{ 0x54, 4, "SC_TS_95_64_" },
	{ 0x58, 4, "SC_USE_SEL_127_96_" },
	{ 0x64, 4, "SC_IO_SEL_127_96_" },
	{ 0x68, 4, "SC_GP_LVL_127_96_" },
	{ 0x6C, 4, "SC_TPE_127_96_" },
	{ 0x70, 4, "SC_TNE_127_96_" },
	{ 0x74, 4, "SC_TS_127_96_" },

	{ 0x80 + 0x00, 4, "SUS_USE_SEL_31_0_" },
	{ 0x80 + 0x04, 4, "SUS_IO_SEL_31_0_" },
	{ 0x80 + 0x08, 4, "SUS_GP_LVL_31_0_" },
	{ 0x80 + 0x0c, 4, "SUS_TPE_31_0_" },
	{ 0x80 + 0x10, 4, "SUS_TNE_31_0_" },
	{ 0x80 + 0x14, 4, "SUS_TS_31_0_" },
	{ 0x80 + 0x18, 4, "SUS_WAKE_EN_31_0_" },
	{ 0x80 + 0x20, 4, "SUS_USE_SEL_43_32_" },
	{ 0x80 + 0x24, 4, "SUS_IO_SEL_43_32_" },
	{ 0x80 + 0x28, 4, "SUS_GP_LVL_43_32_" },
	{ 0x80 + 0x2c, 4, "SUS_TPE_43_32_" },
	{ 0x80 + 0x30, 4, "SUS_TNE_43_32_" },
	{ 0x80 + 0x34, 4, "SUS_TS_43_32_" },
	{ 0x80 + 0x38, 4, "SUS_WAKE_EN_43_32_" }
};

/* Description of GPIO 'bank' ex. {ncore, score. ssus} */
struct gpio_bank {
	const uint32_t gpio_count;
	const u8* gpio_to_pad;
	const unsigned long pad_base_offset;
	const char* gpio_name;
	const char ** func_names;
};

/* Number of GPIOs in each bank */
#define BANK_COUNT		3
#define GPNCORE_COUNT		27
#define GPSCORE_COUNT		102
#define GPSSUS_COUNT		44

/* IO Memory offsets */
#define  IO_BASE_OFFSET_GPNCORE		0x1000
#define  IO_BASE_OFFSET_GPSCORE		0x0000
#define  IO_BASE_OFFSET_GPSSUS		0x2000

static const char *ncore_func_names[GPNCORE_COUNT * 8] = {
"GPIO_S0_NC[00]",	"RESERVED",	"DDI0_HPD",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[01]",	"-",		"DDI0_DDCDATA",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[02]",	"-",		"DDI0_DDCCLK",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[03]",	"-",		"DDI0_VDDEN",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[04]",	"-",		"DDI0_BKLTEN",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[05]",	"-",		"DDI0_BKLTCTL",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[06]",	"RESERVED",	"DDI1_HPD",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[07]",	"-",		"DDI1_DDCDATA",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[08]",	"-",		"DDI1_DDCCLK",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[09]",	"RESERVED",	"DDI1_VDDEN",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[10]",	"RESERVED",	"DDI1_BKLTEN",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[11]",	"RESERVED",	"DDI1_BKLTCTL",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[12]",	"RESERVED",	"-",		"-", "-", "-", "-", "-",
"GPIO_S0_NC[13]",	"RESERVED",	"-",		"-", "-", "-", "-", "-",
"GPIO_S0_NC[14]",	"RESERVED",	"-",		"-", "-", "-", "-", "-",
"GPIO_S0_NC[15]",	"RESERVED",	"RESERVED",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[16]",	"RESERVED",	"RESERVED",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[17]",	"RESERVED",	"RESERVED",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[18]",	"RESERVED",	"RESERVED",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[19]",	"RESERVED",	"RESERVED",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[20]",	"RESERVED",	"RESERVED",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[21]",	"RESERVED",	"RESERVED",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[22]",	"RESERVED",	"RESERVED",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[23]",	"RESERVED",	"RESERVED",	"-", "-", "-", "-", "-",
"GPIO_S0_NC[24]",	"RESERVED",	"-",		"-", "-", "-", "-", "-",
"GPIO_S0_NC[25]",	"RESERVED",	"-",		"-", "-", "-", "-", "-",
"GPIO_S0_NC[26]",	"RESERVED",	"-",		"-", "-", "-", "-", "-",
};

static const char *score_func_names[GPSCORE_COUNT * 8] = {
"GPIO_S0_SC[000]",	"SATA_GP[0]",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[001]",	"SATA_GP[1]",		"SATA_DEVSLP[0]",	"-",		"-", "-", "-", "-",
"GPIO_S0_SC[002]",	"SATA_LED#",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[003]",	"PCIE_CLKREQ[0]#",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[004]",	"PCIE_CLKREQ[1]#",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[005]",	"PCIE_CLKREQ[2]#",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[006]",	"PCIE_CLKREQ[3]#",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[007]",	"RESERVED",		"SD3_WP",		"-",		"-", "-", "-", "-",
"GPIO_S0_SC[008]",	"I2S0_CLK",		"HDA_RST#",		"-",		"-", "-", "-", "-",
"GPIO_S0_SC[009]",	"I2S0_FRM",		"HDA_SYNC",		"-",		"-", "-", "-", "-",
"GPIO_S0_SC[010]",	"I2S0_DATAOUT",		"HDA_CLK",		"-",		"-", "-", "-", "-",
"GPIO_S0_SC[011]",	"I2S0_DATAIN",		"HDA_SDO",		"-",		"-", "-", "-", "-",
"GPIO_S0_SC[012]",	"I2S1_CLK",		"HDA_SDI[0]",		"-",		"-", "-", "-", "-",
"GPIO_S0_SC[013]",	"I2S1_FRM",		"HDA_SDI[1]",		"-",		"-", "-", "-", "-",
"GPIO_S0_SC[014]",	"I2S1_DATAOUT",		"RESERVED",		"-",		"-", "-", "-", "-",
"GPIO_S0_SC[015]",	"I2S1_DATAIN",		"RESERVED",		"-",		"-", "-", "-", "-",
"GPIO_S0_SC[016]",	"MMC1_CLK",		"-",			"MMC1_45_CLK",	"-", "-", "-", "-",
"GPIO_S0_SC[017]",	"MMC1_D[0]",		"-",			"MMC1_45_D[0]",	"-", "-", "-", "-",
"GPIO_S0_SC[018]",	"MMC1_D[1]",		"-",			"MMC1_45_D[1]",	"-", "-", "-", "-",
"GPIO_S0_SC[019]",	"MMC1_D[2]",		"-",			"MMC1_45_D[2]",	"-", "-", "-", "-",
"GPIO_S0_SC[020]",	"MMC1_D[3]",		"-",			"MMC1_45_D[3]",	"-", "-", "-", "-",
"GPIO_S0_SC[021]",	"MMC1_D[4]",		"-",			"MMC1_45_D[4]",	"-", "-", "-", "-",
"GPIO_S0_SC[022]",	"MMC1_D[5]",		"-",			"MMC1_45_D[5]",	"-", "-", "-", "-",
"GPIO_S0_SC[023]",	"MMC1_D[6]",		"-",			"MMC1_45_D[6]",	"-", "-", "-", "-",
"GPIO_S0_SC[024]",	"MMC1_D[7]",		"-",			"MMC1_45_D[7]",	"-", "-", "-", "-",
"GPIO_S0_SC[025]",	"MMC1_CMD",		"-",			"MMC1_45_CMD",	"-", "-", "-", "-",
"GPIO_S0_SC[026]",	"MMC1_RST#",		"SATA_DEVSLP[0]",	"MMC1_45_RST#",	"-", "-", "-", "-",
"GPIO_S0_SC[027]",	"SD2_CLK",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[028]",	"SD2_D[0]",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[029]",	"SD2_D[1]",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[030]",	"SD2_D[2]",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[031]",	"SD2_D[3]_CD#",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[032]",	"SD2_CMD",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[033]",	"SD3_CLK",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[034]",	"SD3_D[0]",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[035]",	"SD3_D[1]",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[036]",	"SD3_D[2]",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[037]",	"SD3_D[3]",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[038]",	"SD3_CD#",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[039]",	"SD3_CMD",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[040]",	"SD3_1P8EN",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[041]",	"SD3_PWREN#",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[042]",	"ILB_LPC_AD[0]",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[043]",	"ILB_LPC_AD[1]",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[044]",	"ILB_LPC_AD[2]",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[045]",	"ILB_LPC_AD[3]",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[046]",	"ILB_LPC_FRAME#",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[047]",	"ILB_LPC_CLK[0]",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[048]",	"ILB_LPC_CLK[1]",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[049]",	"ILB_LPC_CLKRUN#",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[050]",	"ILB_LPC_SERIRQ",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[051]",	"PCU_SMB_DATA",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[052]",	"PCU_SMB_CLK",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[053]",	"PCU_SMB_ALERT#",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[054]",	"ILB_8254_SPKR",	"RESERVED",		"-",		"-", "-", "-", "-",
"GPIO_S0_SC[055]",	"RESERVED",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[056]",	"RESERVED",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[057]",	"PCU_UART_TXD",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[058]",	"RESERVED",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[059]",	"RESERVED",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[060]",	"RESERVED",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[061]",	"PCU_UART_RXD",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[062]",	"LPE_I2S2_CLK",		"SATA_DEVSLP[1]",	"RESERVED",	"-", "-", "-", "-",
"GPIO_S0_SC[063]",	"LPE_I2S2_FRM",		"RESERVED",		"-",		"-", "-", "-", "-",
"GPIO_S0_SC[064]",	"LPE_I2S2_DATAIN",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[065]",	"LPE_I2S2_DATAOUT",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[066]",	"SIO_SPI_CS#",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[067]",	"SIO_SPI_MISO",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[068]",	"SIO_SPI_MOSI",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[069]",	"SIO_SPI_CLK",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[070]",	"SIO_UART1_RXD",	"RESERVED",		"-",		"-", "-", "-", "-",
"GPIO_S0_SC[071]",	"SIO_UART1_TXD",	"RESERVED",		"-",		"-", "-", "-", "-",
"GPIO_S0_SC[072]",	"SIO_UART1_RTS#",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[073]",	"SIO_UART1_CTS#",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[074]",	"SIO_UART2_RXD",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[075]",	"SIO_UART2_TXD",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[076]",	"SIO_UART2_RTS#",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[077]",	"SIO_UART2_CTS#",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[078]",	"SIO_I2C0_DATA",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[079]",	"SIO_I2C0_CLK",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[080]",	"SIO_I2C1_DATA",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[081]",	"SIO_I2C1_CLK",		"RESERVED",		"-",		"-", "-", "-", "-",
"GPIO_S0_SC[082]",	"SIO_I2C2_DATA",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[083]",	"SIO_I2C2_CLK",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[084]",	"SIO_I2C3_DATA",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[085]",	"SIO_I2C3_CLK",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[086]",	"SIO_I2C4_DATA",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[087]",	"SIO_I2C4_CLK",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[088]",	"SIO_I2C5_DATA",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[089]",	"SIO_I2C5_CLK",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[090]",	"SIO_I2C6_DATA",	"ILB_NMI",		"-",		"-", "-", "-", "-",
"GPIO_S0_SC[091]",	"SIO_I2C6_CLK",		"SD3_WP",		"-",		"-", "-", "-", "-",
"RESERVED",		"GPIO_S0_SC[092]",	"-",			"-",		"-", "-", "-", "-",
"RESERVED",		"GPIO_S0_SC[093]",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[094]",	"SIO_PWM[0]",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[095]",	"SIO_PWM[1]",		"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[096]",	"PMC_PLT_CLK[0]",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[097]",	"PMC_PLT_CLK[1]",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[098]",	"PMC_PLT_CLK[2]",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[099]",	"PMC_PLT_CLK[3]",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[100]",	"PMC_PLT_CLK[4]",	"-",			"-",		"-", "-", "-", "-",
"GPIO_S0_SC[101]",	"PMC_PLT_CLK[5]",	"-",			"-",		"-", "-", "-", "-",
};

static const char *ssus_func_names[GPSSUS_COUNT * 8] = {
"GPIO_S5[00]",		"RESERVED",		"-",		"-",		"-", "-", "-",			"-",
"GPIO_S5[01]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "PMC_WAKE_PCIE[1]#",	"-",
"GPIO_S5[02]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "PMC_WAKE_PCIE[2]#",	"-",
"GPIO_S5[03]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "PMC_WAKE_PCIE[3]#",	"-",
"GPIO_S5[04]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "RESERVED",		"-",
"GPIO_S5[05]",		"PMC_SUSCLK[1]",	"RESERVED",	"RESERVED",	"-", "-", "RESERVED",		"-",
"GPIO_S5[06]",		"PMC_SUSCLK[2]",	"RESERVED",	"RESERVED",	"-", "-", "RESERVED",		"-",
"GPIO_S5[07]",		"PMC_SUSCLK[3]",	"RESERVED",	"RESERVED",	"-", "-", "RESERVED",		"-",
"GPIO_S5[08]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "RESERVED",		"-",
"GPIO_S5[09]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "RESERVED",		"-",
"GPIO_S5[10]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "-",			"-",
"PMC_SUSPWRDNACK",	"GPIO_S5[11]",		"-",		"-",		"-", "-", "-",			"-",
"PMC_SUSCLK[0]",	"GPIO_S5[12]",		"-",		"-",		"-", "-", "-",			"-",
"RESERVED",		"GPIO_S5[13]",		"-",		"-",		"-", "-", "-",			"-",
"RESERVED",		"GPIO_S5[14]",		"USB_ULPI_RST#","-",		"-", "-", "-",			"-",
"PMC_WAKE_PCIE[0]#",	"GPIO_S5[15]",		"-",		"-",		"-", "-", "-",			"-",
"PMC_PWRBTN#",		"GPIO_S5[16]",		"-",		"-",		"-", "-", "-",			"-",
"RESERVED",		"GPIO_S5[17]",		"-",		"-",		"-", "-", "-",			"-",
"PMC_SUS_STAT#",	"GPIO_S5[18]",		"-",		"-",		"-", "-", "-",			"-",
"USB_OC[0]#",		"GPIO_S5[19]",		"-",		"-",		"-", "-", "-",			"-",
"USB_OC[1]#",		"GPIO_S5[20]",		"-",		"-",		"-", "-", "-",			"-",
"PCU_SPI_CS[1]#",	"GPIO_S5[21]",		"-",		"-",		"-", "-", "-",			"-",
"GPIO_S5[22]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "RESERVED",		"-",
"GPIO_S5[23]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "RESERVED",		"-",
"GPIO_S5[24]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "RESERVED",		"-",
"GPIO_S5[25]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "RESERVED",		"-",
"GPIO_S5[26]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "RESERVED",		"-",
"GPIO_S5[27]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "RESERVED",		"-",
"GPIO_S5[28]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "RESERVED",		"-",
"GPIO_S5[29]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "RESERVED",		"-",
"GPIO_S5[30]",		"RESERVED",		"RESERVED",	"RESERVED",	"-", "-", "RESERVED",		"-",
"GPIO_S5[31]",		"USB_ULPI_CLK",		"RESERVED",	"RESERVED",	"-", "-", "-",			"-",
"GPIO_S5[32]",		"USB_ULPI_DATA[0]",	"RESERVED",	"RESERVED",	"-", "-", "-",			"-",
"GPIO_S5[33]",		"USB_ULPI_DATA[1]",	"RESERVED",	"RESERVED",	"-", "-", "-",			"-",
"GPIO_S5[34]",		"USB_ULPI_DATA[2]",	"RESERVED",	"RESERVED",	"-", "-", "-",			"-",
"GPIO_S5[35]",		"USB_ULPI_DATA[3]",	"RESERVED",	"RESERVED",	"-", "-", "-",			"-",
"GPIO_S5[36]",		"USB_ULPI_DATA[4]",	"RESERVED",	"RESERVED",	"-", "-", "-",			"-",
"GPIO_S5[37]",		"USB_ULPI_DATA[5]",	"RESERVED",	"RESERVED",	"-", "-", "-",			"-",
"GPIO_S5[38]",		"USB_ULPI_DATA[6]",	"RESERVED",	"RESERVED",	"-", "-", "-",			"-",
"GPIO_S5[39]",		"USB_ULPI_DATA[7]",	"RESERVED",	"RESERVED",	"-", "-", "-",			"-",
"GPIO_S5[40]",		"USB_ULPI_DIR",		"RESERVED",	"RESERVED",	"-", "-", "-",			"-",
"GPIO_S5[41]",		"USB_ULPI_NXT",		"RESERVED",	"RESERVED",	"-", "-", "-",			"-",
"GPIO_S5[42]",		"USB_ULPI_STP",		"RESERVED",	"RESERVED",	"-", "-", "-",			"-",
"GPIO_S5[43]",		"USB_ULPI_REFCLK",	"RESERVED",	"RESERVED",	"-", "-", "-",			"-",
};

/* GPIO-to-Pad LUTs - Translate the GPIO number to the pad register */
static const u8 gpncore_gpio_to_pad[GPNCORE_COUNT] =
	{ 19, 18, 17, 20, 21, 22, 24, 25,	/* [ 0: 7] */
	  23, 16, 14, 15, 12, 26, 27,  1,	/* [ 8:15] */
	   4,  8, 11,  0,  3,  6, 10, 13,	/* [16:23] */
	   2,  5,  9 };				/* [24:26] */

static const u8 gpscore_gpio_to_pad[GPSCORE_COUNT] =
	{  85,  89, 93,  96, 99, 102,  98, 101,	/* [ 0:  7] */
	   34,  37, 36,  38, 39,  35,  40,  84,	/* [ 8: 15] */
	   62,  61, 64,  59, 54,  56,  60,  55,	/* [16: 23] */
	   63,  57, 51,  50, 53,  47,  52,  49,	/* [24: 31] */
	   48,  43, 46,  41, 45,  42,  58,  44,	/* [32: 39] */
	   95, 105, 70,  68, 67,  66,  69,  71,	/* [40: 47] */
	   65,  72, 86,  90, 88,  92, 103,  77,	/* [48: 55] */
	   79,  83, 78,  81, 80,  82,  13,  12,	/* [56: 63] */
	   15,  14, 17,  18, 19,  16,   2,   1,	/* [64: 71] */
	    0,   4,  6,   7,  9,   8,  33,  32,	/* [72: 79] */
	   31,  30, 29,  27, 25,  28,  26,  23,	/* [80: 87] */
	   21,  20, 24,  22,  5,   3,  10,  11,	/* [88: 95] */
	  106,  87, 91, 104, 97, 100 };		/* [96:101] */

static const u8 gpssus_gpio_to_pad[GPSSUS_COUNT] =
	{ 29, 33, 30, 31, 32, 34, 36, 35,	/* [ 0: 7] */
	  38, 37, 18,  7, 11, 20, 17,  1,	/* [ 8:15] */
	   8, 10, 19, 12,  0,  2, 23, 39,	/* [16:23] */
	  28, 27, 22, 21, 24, 25, 26, 51,	/* [24:31] */
	  56, 54, 49, 55, 48, 57, 50, 58,	/* [32:39] */
	  52, 53, 59, 40 };			/* [40:43] */

static const struct gpio_bank gpio_banks[] = {
	{
		.gpio_count = GPNCORE_COUNT,
		.gpio_to_pad = gpncore_gpio_to_pad,
		.pad_base_offset = IO_BASE_OFFSET_GPNCORE,
		.gpio_name = "NCORE GPIOs",
		.func_names = ncore_func_names,
	},
	{
		.gpio_count = GPSCORE_COUNT,
		.gpio_to_pad = gpscore_gpio_to_pad,
		.pad_base_offset = IO_BASE_OFFSET_GPSCORE,
		.gpio_name = "SCORE GPIOs (GPIO_S0_SC_XX)",
		.func_names = score_func_names,
	},
	{
		.gpio_count = GPSSUS_COUNT,
		.gpio_to_pad = gpssus_gpio_to_pad,
		.pad_base_offset = IO_BASE_OFFSET_GPSSUS,
		.gpio_name = "SSUS GPIOs (GPIO_S5)",
		.func_names = ssus_func_names,
	},
};

const char *pull_assignment[] = {"None","Up  ","Down","Res "};
const char *pull_strength[] = {"2k", "10k", "20k", "40k"};

static int show_baytrail_pad_reg(struct pci_dev *sb){

	uint64_t iobase = (uint64_t)pci_read_long(sb, 0x4c) & 0xffffc000;
	uint32_t val, bank, gpio, offset, size = 0x3000;
	volatile uint32_t *reg;

	reg = map_physical(iobase, size);

	if (reg == NULL) {
		perror("Error mapping IOBASE");
		return 1;
	}

	printf("\nIOBASE: 0x%08lx\n",(long int)iobase);

	/* Display function values */
	for (bank = 0; bank < BANK_COUNT; bank++) {
		printf("\n========== Bay Trail %s ===========\n\n",
				gpio_banks[bank].gpio_name);

		printf("Address         | GPIO #   | reg value  | "
			"Pull Dir & Str | Func #: Func Name            |"
			" I/O            | Current Val\n");

		for (gpio=0; gpio < gpio_banks[bank].gpio_count; gpio++) {
			offset = gpio_banks[bank].pad_base_offset +
					(16 * gpio_banks[bank].gpio_to_pad[gpio]);

			/* Read Pad Configuration 0 Register */
			val = *(reg + offset / 4);
			printf("iobase + 0x%04x | GPIO %3d | ",offset, gpio);
			printf("0x%08x | ", val);
			printf("Pull: %4s %3s | ",pull_assignment[(val >> 7) & 3],
					((val >> 7) & 3) ?
					pull_strength[(val >> 9) & 3] :
					"");
			printf("Func %d",val & 0x07);
			if (gpio_banks[bank].func_names != NULL)
				printf(": %-20s | ", gpio_banks[bank].func_names[(gpio * 8) + (val & 0x07)] );

			/* Read the Pad Value Register */
			val = *(reg + offset / 4 + 2);
			printf("%6s%3s%5s | %-4s",
					(val & 0x02) ? "" : "Output",
					(val & 0x06) ? "" : " / ",
					(val & 0x04) ? "" : "Input",
					(val & 0x01) ? "High" : "Low");
			printf("\n");
		}
	}

	unmap_physical((void *)reg, size);
	return 0;
}

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

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_FULL:
	case PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_PREM:
	case PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_BASE:
	case PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP_PREM:
	case PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP:
		gpiobase = pci_read_word(sb, 0x48) & 0xfffc;
		gpio_registers = lynxpoint_lp_gpio_registers;
		size = ARRAY_SIZE(lynxpoint_lp_gpio_registers);
		break;
	case PCI_DEVICE_ID_INTEL_C8_MOBILE:
	case PCI_DEVICE_ID_INTEL_C8_DESKTOP:
	case PCI_DEVICE_ID_INTEL_Z87:
	case PCI_DEVICE_ID_INTEL_Z85:
	case PCI_DEVICE_ID_INTEL_HM86:
	case PCI_DEVICE_ID_INTEL_H87:
	case PCI_DEVICE_ID_INTEL_HM87:
	case PCI_DEVICE_ID_INTEL_Q85:
	case PCI_DEVICE_ID_INTEL_Q87:
	case PCI_DEVICE_ID_INTEL_QM87:
	case PCI_DEVICE_ID_INTEL_B85:
	case PCI_DEVICE_ID_INTEL_C222:
	case PCI_DEVICE_ID_INTEL_C224:
	case PCI_DEVICE_ID_INTEL_C226:
	case PCI_DEVICE_ID_INTEL_H81:
		gpiobase = pci_read_word(sb, 0x48) & 0xfffc;
		gpio_registers = pch_gpio_registers;
		size = ARRAY_SIZE(pch_gpio_registers);
		break;
	case PCI_DEVICE_ID_INTEL_3400:
	case PCI_DEVICE_ID_INTEL_3420:
	case PCI_DEVICE_ID_INTEL_3450:
	case PCI_DEVICE_ID_INTEL_3400_DESKTOP:
	case PCI_DEVICE_ID_INTEL_B55_A:
	case PCI_DEVICE_ID_INTEL_B55_B:
	case PCI_DEVICE_ID_INTEL_H55:
	case PCI_DEVICE_ID_INTEL_H57:
	case PCI_DEVICE_ID_INTEL_P55:
	case PCI_DEVICE_ID_INTEL_Q57:
		gpiobase = pci_read_word(sb, 0x48) & 0xff80;
		gpio_registers = pch_gpio_registers;
		size = ARRAY_SIZE(pch_gpio_registers);
		gpio_defaults = ip_pch_desktop_defaults;
		defaults_size = ARRAY_SIZE(ip_pch_desktop_defaults);
		break;
	case PCI_DEVICE_ID_INTEL_3400_MOBILE:
	case PCI_DEVICE_ID_INTEL_3400_MOBILE_SFF:
	case PCI_DEVICE_ID_INTEL_HM55:
	case PCI_DEVICE_ID_INTEL_HM57:
	case PCI_DEVICE_ID_INTEL_PM55:
	case PCI_DEVICE_ID_INTEL_QM57:
	case PCI_DEVICE_ID_INTEL_QS57:
		gpiobase = pci_read_word(sb, 0x48) & 0xff80;
		gpio_registers = pch_gpio_registers;
		size = ARRAY_SIZE(pch_gpio_registers);
		gpio_defaults = ip_pch_mobile_defaults;
		defaults_size = ARRAY_SIZE(ip_pch_mobile_defaults);
		break;
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
	case PCI_DEVICE_ID_INTEL_ICH10:
	case PCI_DEVICE_ID_INTEL_ICH10D:
	case PCI_DEVICE_ID_INTEL_ICH10DO:
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
	case PCI_DEVICE_ID_INTEL_ICH8ME:
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
	case PCI_DEVICE_ID_INTEL_BAYTRAIL_LPC:
		gpiobase = pci_read_word(sb, 0x48) & 0xfffc;
		gpio_registers = baytrail_score_ssus_gpio_registers;
		size = ARRAY_SIZE(baytrail_score_ssus_gpio_registers);
		break;
	case PCI_DEVICE_ID_INTEL_H110:
	case PCI_DEVICE_ID_INTEL_H170:
	case PCI_DEVICE_ID_INTEL_Z170:
	case PCI_DEVICE_ID_INTEL_Q170:
	case PCI_DEVICE_ID_INTEL_Q150:
	case PCI_DEVICE_ID_INTEL_B150:
	case PCI_DEVICE_ID_INTEL_C236:
	case PCI_DEVICE_ID_INTEL_C232:
	case PCI_DEVICE_ID_INTEL_QM170:
	case PCI_DEVICE_ID_INTEL_HM170:
	case PCI_DEVICE_ID_INTEL_CM236:
	case PCI_DEVICE_ID_INTEL_APL_LPC:
	case PCI_DEVICE_ID_INTEL_GLK_LPC:
	case PCI_DEVICE_ID_INTEL_DNV_LPC:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_PRE:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_BASE:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_PREM:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_IHDCP_PREM:
	case PCI_DEVICE_ID_INTEL_CANNONPOINT_LP_U_PREM:
	case PCI_DEVICE_ID_INTEL_COMETPOINT_LP_U_PREM:
	case PCI_DEVICE_ID_INTEL_COMETPOINT_LP_U_BASE:
	case PCI_DEVICE_ID_INTEL_TIGERPOINT_U_SUPER:
	case PCI_DEVICE_ID_INTEL_TIGERPOINT_U_PREM:
	case PCI_DEVICE_ID_INTEL_TIGERPOINT_U_BASE:
	case PCI_DEVICE_ID_INTEL_TIGERPOINT_Y_SUPER:
	case PCI_DEVICE_ID_INTEL_TIGERPOINT_Y_PREM:
	case PCI_DEVICE_ID_INTEL_C621:
	case PCI_DEVICE_ID_INTEL_C622:
	case PCI_DEVICE_ID_INTEL_C624:
	case PCI_DEVICE_ID_INTEL_C625:
	case PCI_DEVICE_ID_INTEL_C626:
	case PCI_DEVICE_ID_INTEL_C627:
	case PCI_DEVICE_ID_INTEL_C628:
	case PCI_DEVICE_ID_INTEL_C629:
	case PCI_DEVICE_ID_INTEL_C621A:
	case PCI_DEVICE_ID_INTEL_C627A:
	case PCI_DEVICE_ID_INTEL_C629A:
	case PCI_DEVICE_ID_INTEL_C624_SUPER:
	case PCI_DEVICE_ID_INTEL_C627_SUPER_1:
	case PCI_DEVICE_ID_INTEL_C621_SUPER:
	case PCI_DEVICE_ID_INTEL_C627_SUPER_2:
	case PCI_DEVICE_ID_INTEL_C628_SUPER:
	case PCI_DEVICE_ID_INTEL_C621A_SUPER:
	case PCI_DEVICE_ID_INTEL_C627A_SUPER:
	case PCI_DEVICE_ID_INTEL_C629A_SUPER:
	case PCI_DEVICE_ID_INTEL_H270:
	case PCI_DEVICE_ID_INTEL_Z270:
	case PCI_DEVICE_ID_INTEL_Q270:
	case PCI_DEVICE_ID_INTEL_Q250:
	case PCI_DEVICE_ID_INTEL_B250:
	case PCI_DEVICE_ID_INTEL_Z370:
	case PCI_DEVICE_ID_INTEL_H310C:
	case PCI_DEVICE_ID_INTEL_X299:
	case PCI_DEVICE_ID_INTEL_H310:
	case PCI_DEVICE_ID_INTEL_H370:
	case PCI_DEVICE_ID_INTEL_Z390:
	case PCI_DEVICE_ID_INTEL_Q370:
	case PCI_DEVICE_ID_INTEL_B360:
	case PCI_DEVICE_ID_INTEL_C246:
	case PCI_DEVICE_ID_INTEL_C242:
	case PCI_DEVICE_ID_INTEL_QM370:
	case PCI_DEVICE_ID_INTEL_HM370:
	case PCI_DEVICE_ID_INTEL_CM246:
	case PCI_DEVICE_ID_INTEL_Q570:
	case PCI_DEVICE_ID_INTEL_Z590:
	case PCI_DEVICE_ID_INTEL_H570:
	case PCI_DEVICE_ID_INTEL_B560:
	case PCI_DEVICE_ID_INTEL_H510:
	case PCI_DEVICE_ID_INTEL_WM590:
	case PCI_DEVICE_ID_INTEL_QM580:
	case PCI_DEVICE_ID_INTEL_HM570:
	case PCI_DEVICE_ID_INTEL_C252:
	case PCI_DEVICE_ID_INTEL_C256:
	case PCI_DEVICE_ID_INTEL_W580:
	case PCI_DEVICE_ID_INTEL_ICELAKE_LP_U:
	case PCI_DEVICE_ID_INTEL_H610:
	case PCI_DEVICE_ID_INTEL_B660:
	case PCI_DEVICE_ID_INTEL_H670:
	case PCI_DEVICE_ID_INTEL_Q670:
	case PCI_DEVICE_ID_INTEL_Z690:
	case PCI_DEVICE_ID_INTEL_W680:
	case PCI_DEVICE_ID_INTEL_W685:
	case PCI_DEVICE_ID_INTEL_WM690:
	case PCI_DEVICE_ID_INTEL_HM670:
	case PCI_DEVICE_ID_INTEL_WM790:
	case PCI_DEVICE_ID_INTEL_HM770:
	case PCI_DEVICE_ID_INTEL_ADL_P:
	case PCI_DEVICE_ID_INTEL_ADL_M:
	case PCI_DEVICE_ID_INTEL_RPL_P:
		print_gpio_groups(sb);
		return 0;
	case PCI_DEVICE_ID_INTEL_82371XX:
		printf("This southbridge has GPIOs in the PM unit.\n");
		return 1;
	default:
		printf("Error: Dumping GPIOs on this southbridge is not (yet) supported.\n");
		return 1;
	}

	if (show_diffs && !show_all)
		printf("\n========== GPIO DIFFS ===========\n\n");
	else
		printf("\n============= GPIOS =============\n\n");

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

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_FULL:
	case PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_PREM:
	case PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_BASE:
	case PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP_PREM:
	case PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP:
		for (i = 0; i < 95; i++) {
			io_register_t tmp_gpio;
			char gpio_name[32];
			uint16_t tmp_addr = 0x100 + (4 * i * 2);

			snprintf(gpio_name, sizeof(gpio_name), "GP%dCONFIGA", i);
			tmp_gpio.addr = tmp_addr;
			tmp_gpio.name = gpio_name;
			tmp_gpio.size = 4;

			if (show_all)
				print_reg(&tmp_gpio);

			snprintf(gpio_name, 32, "GP%dCONFIGB", i);
			tmp_gpio.addr = tmp_addr + 4;
			tmp_gpio.name = gpio_name;
			tmp_gpio.size = 4;

			if (show_all)
				print_reg(&tmp_gpio);
		}
		break;
	case PCI_DEVICE_ID_INTEL_BAYTRAIL_LPC:
		show_baytrail_pad_reg(sb);
		break;
	default:
		break;
	}

	return 0;
}
