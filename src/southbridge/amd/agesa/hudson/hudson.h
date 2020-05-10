/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef HUDSON_H
#define HUDSON_H

#include <device/device.h>

/* Power management index/data registers */
#define BIOSRAM_INDEX	0xcd4
#define BIOSRAM_DATA	0xcd5
#define PM_INDEX	0xcd6
#define PM_DATA		0xcd7
#define PM2_INDEX	0xcd0
#define PM2_DATA	0xcd1

#define HUDSON_ACPI_IO_BASE 0x800

#define ACPI_PM_EVT_BLK		(HUDSON_ACPI_IO_BASE + 0x00) /* 4 bytes */
#define ACPI_PM1_CNT_BLK	(HUDSON_ACPI_IO_BASE + 0x04) /* 2 bytes */
#define ACPI_PM_TMR_BLK		(HUDSON_ACPI_IO_BASE + 0x18) /* 4 bytes */
#define ACPI_GPE0_BLK		(HUDSON_ACPI_IO_BASE + 0x10) /* 8 bytes */
#define ACPI_CPU_CONTROL	(HUDSON_ACPI_IO_BASE + 0x08) /* 6 bytes */

#define ACPI_SMI_CTL_PORT		0xb2
#define ACPI_SMI_CMD_CST_CONTROL	0xde
#define ACPI_SMI_CMD_PST_CONTROL	0xad
#define ACPI_SMI_CMD_DISABLE		0xbe
#define ACPI_SMI_CMD_ENABLE		0xef
#define ACPI_SMI_CMD_S4_REQ		0xc0

#define REV_HUDSON_A11	0x11
#define REV_HUDSON_A12	0x12

#define LPC_IO_PORT_DECODE_ENABLE	0x44
#define   DECODE_ENABLE_PARALLEL_PORT0	BIT(0)
#define   DECODE_ENABLE_PARALLEL_PORT1	BIT(1)
#define   DECODE_ENABLE_PARALLEL_PORT2	BIT(2)
#define   DECODE_ENABLE_PARALLEL_PORT3	BIT(3)
#define   DECODE_ENABLE_PARALLEL_PORT4	BIT(4)
#define   DECODE_ENABLE_PARALLEL_PORT5	BIT(5)
#define   DECODE_ENABLE_SERIAL_PORT0	BIT(6)
#define   DECODE_ENABLE_SERIAL_PORT1	BIT(7)
#define   DECODE_ENABLE_SERIAL_PORT2	BIT(8)
#define   DECODE_ENABLE_SERIAL_PORT3	BIT(9)
#define   DECODE_ENABLE_SERIAL_PORT4	BIT(10)
#define   DECODE_ENABLE_SERIAL_PORT5	BIT(11)
#define   DECODE_ENABLE_SERIAL_PORT6	BIT(12)
#define   DECODE_ENABLE_SERIAL_PORT7	BIT(13)
#define   DECODE_ENABLE_AUDIO_PORT0	BIT(14)
#define   DECODE_ENABLE_AUDIO_PORT1	BIT(15)
#define   DECODE_ENABLE_AUDIO_PORT2	BIT(16)
#define   DECODE_ENABLE_AUDIO_PORT3	BIT(17)
#define   DECODE_ENABLE_MIDI_PORT0	BIT(18)
#define   DECODE_ENABLE_MIDI_PORT1	BIT(19)
#define   DECODE_ENABLE_MIDI_PORT2	BIT(20)
#define   DECODE_ENABLE_MIDI_PORT3	BIT(21)
#define   DECODE_ENABLE_MSS_PORT0	BIT(22)
#define   DECODE_ENABLE_MSS_PORT1	BIT(23)
#define   DECODE_ENABLE_MSS_PORT2	BIT(24)
#define   DECODE_ENABLE_MSS_PORT3	BIT(25)
#define   DECODE_ENABLE_FDC_PORT0	BIT(26)
#define   DECODE_ENABLE_FDC_PORT1	BIT(27)
#define   DECODE_ENABLE_GAME_PORT	BIT(28)
#define   DECODE_ENABLE_KBC_PORT	BIT(29)
#define   DECODE_ENABLE_ACPIUC_PORT	BIT(30)
#define   DECODE_ENABLE_ADLIB_PORT	BIT(31)

#define LPC_IO_OR_MEM_DECODE_ENABLE	0x48

#define LPC_TRUSTED_PLATFORM_MODULE	0x7c
#define   TPM_12_EN			BIT(0)
#define   TPM_LEGACY_EN			BIT(2)

#define SPIROM_BASE_ADDRESS_REGISTER  0xA0
#define SPI_ROM_ENABLE                0x02
#define SPI_BASE_ADDRESS              0xFEC10000

static inline int hudson_sata_enable(void)
{
	/* True if IDE or AHCI. */
	return (CONFIG_HUDSON_SATA_MODE == 0) || (CONFIG_HUDSON_SATA_MODE == 2);
}

static inline int hudson_ide_enable(void)
{
	/* True if IDE or LEGACY IDE. */
	return (CONFIG_HUDSON_SATA_MODE == 0) || (CONFIG_HUDSON_SATA_MODE == 3);
}

void hudson_lpc_port80(void);
void hudson_pci_port80(void);
void hudson_lpc_decode(void);
void hudson_clk_output_48Mhz(void);

void hudson_enable(struct device *dev);

#endif /* HUDSON_H */
