/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_SMI_H
#define AMD_BLOCK_SMI_H

#include <stdint.h>

enum smi_mode {
	SMI_MODE_DISABLE = 0,
	SMI_MODE_SMI = 1,
	SMI_MODE_NMI = 2,
	SMI_MODE_IRQ13 = 3,
};

enum smi_sci_type {
	INTERRUPT_NONE,
	INTERRUPT_SCI,
	INTERRUPT_SMI,
	INTERRUPT_BOTH,
};

enum smi_sci_lvl {
	SMI_SCI_LVL_LOW,
	SMI_SCI_LVL_HIGH,
};

enum smi_sci_dir {
	SMI_SCI_EDG,
	SMI_SCI_LVL,
};

struct smi_sources_t {
	int type;
	void (*handler)(void);
};

struct sci_source {
	uint8_t scimap;		/* SCI source number */
	uint8_t gpe;		/* 32 GPEs */
	uint8_t direction;	/* Active High or Low,  smi_sci_lvl */
	uint8_t level;		/* Edge or Level,  smi_sci_dir */
};

void configure_smi(uint8_t smi_num, uint8_t mode);
void configure_gevent_smi(uint8_t gevent, uint8_t mode, uint8_t level);
void configure_scimap(const struct sci_source *sci);
void disable_gevent_smi(uint8_t gevent);
void gpe_configure_sci(const struct sci_source *scis, size_t num_gpes);
void soc_route_sci(uint8_t event);
void clear_all_smi_status(void);
void clear_smi_sci_status(void);

#endif /* AMD_BLOCK_SMI_H */
