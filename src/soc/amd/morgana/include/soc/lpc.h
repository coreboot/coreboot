/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Update for Morgana */

#ifndef AMD_MORGANA_LPC_H
#define AMD_MORGANA_LPC_H

/* LPC_MISC_CONTROL_BITS at D14F3x078 */

#define   LPC_LDRQ0_PU_EN		BIT(10)
#define   LPC_LDRQ0_PD_EN		BIT(9)

#define SPI_BASE_ADDRESS_REGISTER	0xa0
#define   SPI_BASE_ALIGNMENT		BIT(8)
#define   SPI_BASE_RESERVED		(BIT(5) | BIT(6) | BIT(7))
#define   PSP_SPI_MMIO_SEL		BIT(4)
#define   ROUTE_TPM_2_SPI		BIT(3)
#define   SPI_ABORT_ENABLE		BIT(2)
#define   SPI_ROM_ENABLE		BIT(1)
#define   SPI_ROM_ALT_ENABLE		BIT(0)
#define   SPI_PRESERVE_BITS		(BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4))

#endif /* AMD_MORGANA_LPC_H */
