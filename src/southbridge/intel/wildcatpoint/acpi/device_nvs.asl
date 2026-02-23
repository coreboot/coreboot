/* SPDX-License-Identifier: GPL-2.0-only */

Field (DNVS, ByteAcc, NoLock, Preserve)
{
	/* Device enables in ACPI mode */

	S0EN,	8,	// DMA Enable
	S1EN,	8,	// I2C0 Enable
	S2EN,	8,	// I2C1 Enable
	S3EN,	8,	// SPI0 Enable
	S4EN,	8,	// SPI1 Enable
	S5EN,	8,	// UART0 Enable
	S6EN,	8,	// UART1 Enable
	S7EN,	8,	// SDIO Enable
	S8EN,	8,	// ADSP Enable

	/* BAR 0 */

	S0B0,	32,	// DMA BAR0
	S1B0,	32,	// I2C0 BAR0
	S2B0,	32,	// I2C1 BAR0
	S3B0,	32,	// SPI0 BAR0
	S4B0,	32,	// SPI1 BAR0
	S5B0,	32,	// UART0 BAR0
	S6B0,	32,	// UART1 BAR0
	S7B0,	32,	// SDIO BAR0
	S8B0,	32,	// ADSP BAR0

	/* BAR 1 */

	S0B1,	32,	// DMA BAR1
	S1B1,	32,	// I2C0 BAR1
	S2B1,	32,	// I2C1 BAR1
	S3B1,	32,	// SPI0 BAR1
	S4B1,	32,	// SPI1 BAR1
	S5B1,	32,	// UART0 BAR1
	S6B1,	32,	// UART1 BAR1
	S7B1,	32,	// SDIO BAR1
	S8B1,	32,	// ADSP BAR1
}
