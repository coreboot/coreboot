/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_P2SB_H
#define SOC_INTEL_COMMON_BLOCK_P2SB_H

#include <stddef.h>
#include <stdint.h>

#define PCH_P2SB_E0		0xe0
#define   P2SB_E0_MASKLOCK	(1 << 1)
#define PCH_P2SB_IBDF		0x6c
#define PCH_P2SB_HBDF		0x70

enum {
	P2SB_EP_MASK_0_REG,
	P2SB_EP_MASK_1_REG,
	P2SB_EP_MASK_2_REG,
	P2SB_EP_MASK_3_REG,
	P2SB_EP_MASK_4_REG,
	P2SB_EP_MASK_5_REG,
	P2SB_EP_MASK_6_REG,
	P2SB_EP_MASK_7_REG,
	P2SB_EP_MASK_MAX_REG,
};

void p2sb_unhide(void);
void p2sb_hide(void);
void p2sb_disable_sideband_access(void);
void p2sb_enable_bar(void);
void p2sb_configure_hpet(void);

/*
 * Functions to access IOE P2SB.
 * pid argument: SBI port Id
 */
void ioe_p2sb_enable_bar(void);
uint32_t ioe_p2sb_sbi_read(uint8_t pid, uint16_t reg);
void ioe_p2sb_sbi_write(uint8_t pid, uint16_t reg, uint32_t val);

union p2sb_bdf {
	struct {
		uint16_t fn  : 3;
		uint16_t dev : 5;
		uint16_t bus : 8;
	};
	uint16_t raw;
};

union p2sb_bdf p2sb_get_hpet_bdf(void);
void p2sb_set_hpet_bdf(union p2sb_bdf bdf);
union p2sb_bdf p2sb_get_ioapic_bdf(void);
void p2sb_set_ioapic_bdf(union p2sb_bdf bdf);

/* SOC overrides */
/*
 * Each SoC should implement EP Mask register to disable SB access
 * Input:
 * ep_mask: An array to be filled by SoC code with EP mask register.
 * count: number of element in EP mask array.
 */
void p2sb_soc_get_sb_mask(uint32_t *ep_mask, size_t count);

#endif	/* SOC_INTEL_COMMON_BLOCK_P2SB_H */
