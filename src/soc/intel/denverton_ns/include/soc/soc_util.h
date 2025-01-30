/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DENVERTON_NS_SOC_UTIL_H_
#define _DENVERTON_NS_SOC_UTIL_H_

#ifndef __ACPI__
#include <device/device.h>
#include <string.h>

/* Silicon revisions */
typedef enum {
	SILICON_REV_DENVERTON_A0 = 0x00,
	SILICON_REV_DENVERTON_A1 = 0x01,
	SILICON_REV_DENVERTON_B0 = 0x02,
} silicon_revision;

/* soc_util.c */
#ifdef __SIMPLE_DEVICE__
pci_devfn_t get_hostbridge_dev(void);
pci_devfn_t get_lpc_dev(void);
pci_devfn_t get_pmc_dev(void);
pci_devfn_t get_smbus_dev(void);
#else
struct device *get_hostbridge_dev(void);
struct device *get_lpc_dev(void);
struct device *get_pmc_dev(void);
struct device *get_smbus_dev(void);
#endif

uint32_t get_pciebase(void);
uint32_t get_pcielength(void);
uint32_t get_tseg_memory(void);
uint32_t get_top_of_low_memory(void);
uint64_t get_top_of_upper_memory(void);
uint16_t get_pmbase(void);
uint16_t get_tcobase(void);

void mmio_andthenor32(void *addr, uint32_t val2and, uint32_t val2or);
uint8_t silicon_stepping(void);

/*
* MMIO Read/Write
*/
#define MMIO8(x) (*((volatile u8 *)(x)))
#define MMIO16(x) (*((volatile u16 *)(x)))
#define MMIO32(x) (*((volatile u32 *)(x)))

#define MMIO_AND_OR(bits, x, and, or) \
	(MMIO##bits(x) = ((MMIO##bits(x) & (and)) | (or)))

#define MMIO8_AND_OR(x, and, or) MMIO_AND_OR(8, x, and, or)
#define MMIO16_AND_OR(x, and, or) MMIO_AND_OR(16, x, and, or)
#define MMIO32_AND_OR(x, and, or) MMIO_AND_OR(32, x, and, or)
#define MMIO32_OR(x, or) MMIO_AND_OR(32, x, ~0UL, or)
#define MMIO32_AND(x, and) MMIO_AND_OR(32, x, and, 0UL)

#endif //__ACPI__

#endif /* _DENVERTON_NS_SOC_UTIL_H_ */
