
/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_SPI_H
#define SOUTHBRIDGE_INTEL_SPI_H

enum optype {
	READ_NO_ADDR = 0,
	WRITE_NO_ADDR = 1,
	READ_WITH_ADDR = 2,
	WRITE_WITH_ADDR = 3
};

struct intel_spi_op {
	u8 op;
	enum optype type;
};

struct intel_swseq_spi_config {
	u8 opprefixes[2];
	struct intel_spi_op ops[8];
};

void spi_finalize_ops(void);
void intel_southbridge_override_spi(struct intel_swseq_spi_config *spi_config);

#endif
