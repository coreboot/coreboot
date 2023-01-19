/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/spi.h>
#include <soc/espi.h>
#include <types.h>

#define ESPI_CNTRL_REGISTER	0x10 /* SPI register, not eSPI register! */
#define  LOCK_SPIX10_BIT2	BIT(3)
#define  ESPI_MUX_SPI1		BIT(2)
#define  ROM_ADDR_WR_PROT	BIT(1)

void espi_switch_to_spi1_pads(void)
{
	uint8_t reg = spi_read8(ESPI_CNTRL_REGISTER);

	reg |= ESPI_MUX_SPI1;

	spi_write8(ESPI_CNTRL_REGISTER, reg);
}
