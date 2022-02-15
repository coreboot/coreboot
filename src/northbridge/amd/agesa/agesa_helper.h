/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _AGESA_HELPER_H_
#define _AGESA_HELPER_H_

#include <stddef.h>
#include <arch/romstage.h>

enum {
	PICK_DMI,       /* DMI Interface */
	PICK_PSTATE,    /* Acpi Pstate SSDT Table */
	PICK_SRAT,      /* SRAT Table */
	PICK_SLIT,      /* SLIT Table */
	PICK_WHEA_MCE,  /* WHEA MCE table */
	PICK_WHEA_CMC,  /* WHEA CMV table */
	PICK_ALIB,      /* SACPI SSDT table with ALIB implementation */
	PICK_IVRS,      /* IOMMU ACPI IVRS(I/O Virtualization Reporting Structure) table */
	PICK_CRAT,      /* Component Resource Affinity Table table */
	PICK_CDIT,      /* Component Locality Distance Information table */
};

void agesawrapper_setlateinitptr (void *Late);
void *agesawrapper_getlateinitptr (int pick);

void amd_initcpuio(void);
void amd_initenv(void);

void *GetHeapBase(void);
void EmptyHeap(void);

#define BSP_STACK_BASE_ADDR		0x30000

/* This covers node 0 only. */
#define HIGH_ROMSTAGE_STACK_SIZE	(0x48000 - BSP_STACK_BASE_ADDR)

#define HIGH_MEMORY_SCRATCH		0x30000

void fixup_cbmem_to_UC(int s3resume);

void restore_mtrr(void);
void backup_mtrr(void *mtrr_store, u32 *mtrr_store_size);
const void *OemS3Saved_MTRR_Storage(void);

#endif /* _AGESA_HELPER_H_ */
