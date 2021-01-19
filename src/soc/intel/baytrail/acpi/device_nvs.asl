/* SPDX-License-Identifier: GPL-2.0-only */

Field (DNVS, ByteAcc, NoLock, Preserve)
{
	/* Device Enabled in ACPI Mode */

	S0EN,	8,	/* SDMA Enable */
	S1EN,	8,	/* I2C1 Enable */
	S2EN,	8,	/* I2C2 Enable */
	S3EN,	8,	/* I2C3 Enable */
	S4EN,	8,	/* I2C4 Enable */
	S5EN,	8,	/* I2C5 Enable */
	S6EN,	8,	/* I2C6 Enable */
	S7EN,	8,	/* I2C7 Enable */
	S8EN,	8,	/* SDMA2 Enable */
	S9EN,	8,	/* SPI Enable */
	SAEN,	8,	/* PWM1 Enable */
	SBEN,	8,	/* PWM2 Enable */
	SCEN,	8,	/* UART2 Enable */
	SDEN,	8,	/* UART2 Enable */
	C0EN,	8,	/* MMC Enable */
	C1EN,	8,	/* SDIO Enable */
	C2EN,	8,	/* SD Card Enable */
	LPEN,	8,	/* LPE Enable */

	/* BAR 0 */

	S0B0,	32,	/* SDMA BAR0 */
	S1B0,	32,	/* I2C1 BAR0 */
	S2B0,	32,	/* I2C2 BAR0 */
	S3B0,	32,	/* I2C3 BAR0 */
	S4B0,	32,	/* I2C4 BAR0 */
	S5B0,	32,	/* I2C5 BAR0 */
	S6B0,	32,	/* I2C6 BAR0 */
	S7B0,	32,	/* I2C7 BAR0 */
	S8B0,	32,	/* SDMA2 BAR0 */
	S9B0,	32,	/* SPI BAR0 */
	SAB0,	32,	/* PWM1 BAR0 */
	SBB0,	32,	/* PWM2 BAR0 */
	SCB0,	32,	/* UART1 BAR0 */
	SDB0,	32,	/* UART2 BAR0 */
	C0B0,	32,	/* MMC BAR0 */
	C1B0,	32,	/* SDIO BAR0 */
	C2B0,	32,	/* SD Card BAR0 */
	LPB0,	32,	/* LPE BAR0 */

	/* BAR 1 */

	S0B1,	32,	/* SDMA BAR1 */
	S1B1,	32,	/* I2C1 BAR1 */
	S2B1,	32,	/* I2C2 BAR1 */
	S3B1,	32,	/* I2C3 BAR1 */
	S4B1,	32,	/* I2C4 BAR1 */
	S5B1,	32,	/* I2C5 BAR1 */
	S6B1,	32,	/* I2C6 BAR1 */
	S7B1,	32,	/* I2C7 BAR1 */
	S8B1,	32,	/* SDMA2 BAR1 */
	S9B1,	32,	/* SPI BAR1 */
	SAB1,	32,	/* PWM1 BAR1 */
	SBB1,	32,	/* PWM2 BAR1 */
	SCB1,	32,	/* UART1 BAR1 */
	SDB1,	32,	/* UART2 BAR1 */
	C0B1,	32,	/* MMC BAR1 */
	C1B1,	32,	/* SDIO BAR1 */
	C2B1,	32,	/* SD Card BAR1 */
	LPB1,	32,	/* LPE BAR1 */

	/* Extra */

	LPFW,	32,	/* LPE BAR2 Firmware */
}
