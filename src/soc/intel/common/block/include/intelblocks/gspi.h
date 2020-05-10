/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_GSPI_H
#define SOC_INTEL_COMMON_BLOCK_GSPI_H

#include <spi-generic.h>
#include <stdint.h>

/* GSPI controller structure to allow SoCs to define bus-controller mapping. */
extern const struct spi_ctrlr gspi_ctrlr;

struct gspi_cfg {
	/* Bus speed in MHz. */
	uint32_t speed_mhz;
	/* Bus should be enabled prior to ramstage with temporary base. */
	uint8_t early_init;
};

/* GSPI controller APIs. */
void gspi_early_bar_init(void);

/* SoC-callbacks */
/*
 * Map given GSPI bus number to devfn.
 * Return value:
 * -1 = error
 * otherwise, devfn(>=0) corresponding to GSPI bus number.
 */
int gspi_soc_bus_to_devfn(unsigned int gspi_bus);

/*
 * SoC-provided callback for getting configuration of SPI bus. Driver provides
 * weak implementation with default SPI-bus configuration.
 *
 * Return value:
 * 0 = Success
 * -1 = Error
 */
int gspi_get_soc_spi_cfg(unsigned int bus, struct spi_cfg *cfg);

#endif /* SOC_INTEL_COMMON_BLOCK_GSPI_H */
