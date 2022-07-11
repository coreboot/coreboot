/* SPDX-License-Identifier: BSD-2-Clause */

#include <console/console.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/cache.h>
#include <security/intel/stm/SmmStm.h>
#include <stdbool.h>
#include <string.h>

#define TXT_EVTYPE_BASE 0x400
#define TXT_EVTYPE_STM_HASH (TXT_EVTYPE_BASE + 14)

#define RDWR_ACCS 3
#define FULL_ACCS 7

#define SIZE_4KB 0x00001000
#define SIZE_4MB 0x00400000

#define PTP_SIZE SIZE_4KB

#define IA32_PG_P (1 << 0)
#define IA32_PG_RW (1 << 1)
#define IA32_PG_PS (1 << 7)

#define STM_PAGE_SHIFT 12
#define STM_PAGE_MASK 0xFFF
#define STM_SIZE_TO_PAGES(a)                                                   \
	(((a) >> STM_PAGE_SHIFT) + (((a)&STM_PAGE_MASK) ? 1 : 0))
#define STM_PAGES_TO_SIZE(a) ((a) << STM_PAGE_SHIFT)

#define STM_ACCESS_DENIED 15
#define STM_UNSUPPORTED 3

#define STM_BUFFER_TOO_SMALL 1

#define STM_SM_MONITOR_STATE_ENABLED 1

typedef struct {

	uint64_t vmcs_revision_id : 31;
	uint64_t always_zero : 1;
	uint64_t vmcs_size : 13;
	uint64_t reserved1 : 3;
	uint64_t vmxon_add_width : 1;
	uint64_t stm_supported : 1;
	uint64_t vmcs_memory_type : 4;
	uint64_t in_out_reporting : 1;
	uint64_t may_clear_defaults : 1;
	uint64_t reserved2 : 8;
} VMX_BASIC_MSR_BITS;

typedef union {
	VMX_BASIC_MSR_BITS bits;
	uint64_t uint64;
	msr_t msr;
} VMX_BASIC_MSR;

typedef struct {
	uint64_t valid : 1;
	uint64_t reserved1 : 1;
	uint64_t vmx_off_blockSmi : 1;
	uint64_t reserved2 : 9;
	uint64_t mseg_address : 20;
	uint64_t reserved3 : 32;
} SMM_MONITOR_CTL_MSR_BITS;

extern struct mp_state {
	struct mp_ops ops;
	int cpu_count;
	uintptr_t perm_smbase;
	size_t perm_smsize;
	size_t smm_save_state_size;
	bool do_smm;
} mp_state;

typedef union {
	SMM_MONITOR_CTL_MSR_BITS bits;
	uint64_t uint64;
	msr_t msr;
} SMM_MONITOR_CTL_MSR;

// Template of STM_RSC_END structure for copying.

STM_RSC_END m_rsc_end_node = {
	{END_OF_RESOURCES, sizeof(STM_RSC_END)},
};

uint8_t *m_stm_resources_ptr = NULL;
uint32_t m_stm_resource_total_size = 0x0;
uint32_t m_stm_resource_size_used = 0x0;
uint32_t m_stm_resource_size_available = 0x0;

uint8_t *stm_resource_heap = NULL;

uint32_t m_stm_state = 0;

/*
 * Handle single Resource to see if it can be merged into Record.
 *
 *  @param resource  A pointer to resource node to be added
 *  @param record    A pointer to record node to be merged
 *
 *  @retval true  resource handled
 *  @retval false resource is not handled
 */

static bool handle_single_resource(STM_RSC *resource, STM_RSC *record)
{
	uint64_t resource_lo = 0;
	uint64_t resource_hi = 0;
	uint64_t record_lo = 0;
	uint64_t record_hi = 0;

	// Calling code is responsible for making sure that
	// Resource->Header.RscType == (*Record)->Header.RscType
	// thus we use just one of them as switch variable.

	switch (resource->header.rsc_type) {
	case MEM_RANGE:
	case MMIO_RANGE:
		resource_lo = resource->mem.base;
		resource_hi = resource->mem.base + resource->mem.length;
		record_lo = record->mem.base;
		record_hi = record->mem.base + record->mem.length;
		if (resource->mem.rwx_attributes
		    != record->mem.rwx_attributes) {
			if ((resource_lo == record_lo)
			    && (resource_hi == record_hi)) {
				record->mem.rwx_attributes =
					resource->mem.rwx_attributes
					| record->mem.rwx_attributes;
				return true;
			} else {
				return false;
			}
		}
		break;
	case IO_RANGE:
	case TRAPPED_IO_RANGE:
		resource_lo = (uint64_t)resource->io.base;
		resource_hi = (uint64_t)resource->io.base
			      + (uint64_t)resource->io.length;
		record_lo = (uint64_t)record->io.base;
		record_hi =
			(uint64_t)record->io.base + (uint64_t)record->io.length;
		break;
	case PCI_CFG_RANGE:
		if ((resource->pci_cfg.originating_bus_number
		     != record->pci_cfg.originating_bus_number)
		    || (resource->pci_cfg.last_node_index
			!= record->pci_cfg.last_node_index))
			return false;

		if (memcmp(resource->pci_cfg.pci_device_path,
			   record->pci_cfg.pci_device_path,
			   sizeof(STM_PCI_DEVICE_PATH_NODE)
				   * (resource->pci_cfg.last_node_index + 1))
		    != 0) {
			return false;
		}
		resource_lo = (uint64_t)resource->pci_cfg.base;
		resource_hi = (uint64_t)resource->pci_cfg.base
			      + (uint64_t)resource->pci_cfg.length;
		record_lo = (uint64_t)record->pci_cfg.base;
		record_hi = (uint64_t)record->pci_cfg.base
			    + (uint64_t)record->pci_cfg.length;
		if (resource->pci_cfg.rw_attributes
		    != record->pci_cfg.rw_attributes) {
			if ((resource_lo == record_lo)
			    && (resource_hi == record_hi)) {
				record->pci_cfg.rw_attributes =
					resource->pci_cfg.rw_attributes
					| record->pci_cfg.rw_attributes;
				return true;
			} else {
				return false;
			}
		}
		break;
	case MACHINE_SPECIFIC_REG:

		// Special case - merge MSR masks in place.
		if (resource->msr.msr_index != record->msr.msr_index)
			return false;
		record->msr.read_mask |= resource->msr.read_mask;
		record->msr.write_mask |= resource->msr.write_mask;
		return true;
	default:
		return false;
	}

	// If resources are disjoint
	if ((resource_hi < record_lo) || (resource_lo > record_hi))
		return false;

	// If resource is consumed by record.
	if ((resource_lo >= record_lo) && (resource_hi <= record_hi))
		return true;

	// Resources are overlapping.
	// Resource and record are merged.
	resource_lo = (resource_lo < record_lo) ? resource_lo : record_lo;
	resource_hi = (resource_hi > record_hi) ? resource_hi : record_hi;

	switch (resource->header.rsc_type) {
	case MEM_RANGE:
	case MMIO_RANGE:
		record->mem.base = resource_lo;
		record->mem.length = resource_hi - resource_lo;
		break;
	case IO_RANGE:
	case TRAPPED_IO_RANGE:
		record->io.base = (uint64_t)resource_lo;
		record->io.length = (uint64_t)(resource_hi - resource_lo);
		break;
	case PCI_CFG_RANGE:
		record->pci_cfg.base = (uint64_t)resource_lo;
		record->pci_cfg.length = (uint64_t)(resource_hi - resource_lo);
		break;
	default:
		return false;
	}

	return true;
}

/*
 * Add resource node.
 *
 * @param Resource  A pointer to resource node to be added
 */
static void add_single_resource(STM_RSC *resource)
{
	STM_RSC *record;

	record = (STM_RSC *)m_stm_resources_ptr;

	while (true) {
		if (record->header.rsc_type == END_OF_RESOURCES)
			break;

		// Go to next record if resource and record types don't match.
		if (resource->header.rsc_type != record->header.rsc_type) {
			record = (STM_RSC *)((void *)record
					     + record->header.length);
			continue;
		}

		// Record is handled inside of procedure - don't adjust.
		if (handle_single_resource(resource, record))
			return;
		record = (STM_RSC *)((void *)record + record->header.length);
	}

	// Add resource to the end of area.
	memcpy(m_stm_resources_ptr + m_stm_resource_size_used
		       - sizeof(m_rsc_end_node),
	       resource, resource->header.length);
	memcpy(m_stm_resources_ptr + m_stm_resource_size_used
		       - sizeof(m_rsc_end_node) + resource->header.length,
	       &m_rsc_end_node, sizeof(m_rsc_end_node));
	m_stm_resource_size_used += resource->header.length;
	m_stm_resource_size_available =
		m_stm_resource_total_size - m_stm_resource_size_used;
}

/*
 *  Add resource list.
 *
 *  @param resource_list   A pointer to resource list to be added
 *  @param num_entries     Optional number of entries.
 *		            If 0, list must be terminated by END_OF_RESOURCES.
 */
static void add_resource(STM_RSC *resource_list, uint32_t num_entries)
{
	uint32_t count;
	uint32_t index;
	STM_RSC *resource;

	if (num_entries == 0)
		count = 0xFFFFFFFF;
	else
		count = num_entries;

	resource = resource_list;

	for (index = 0; index < count; index++) {
		if (resource->header.rsc_type == END_OF_RESOURCES)
			return;
		add_single_resource(resource);
		resource =
			(STM_RSC *)((void *)resource + resource->header.length);
	}
}

/*
 *  Validate resource list.
 *
 *  @param resource_list  A pointer to resource list to be added
 *  @param num_entries    Optional number of entries.
 *			If 0, list must be terminated by END_OF_RESOURCES.
 *
 *  @retval true  resource valid
 *  @retval false resource invalid
 */
static bool validate_resource(STM_RSC *resource_list, uint32_t num_entries)
{
	uint32_t count;
	uint32_t index;
	STM_RSC *resource;
	uint32_t sub_index;

	// If NumEntries == 0 make it very big. Scan will be terminated by
	// END_OF_RESOURCES.
	if (num_entries == 0)
		count = 0xFFFFFFFF;
	else
		count = num_entries;

	// Start from beginning of resource list.
	resource = resource_list;

	for (index = 0; index < count; index++) {
		printk(BIOS_DEBUG, "STM: %s (%u) - RscType(%x) length(0x%x)\n",
			__func__,
			index,
			resource->header.rsc_type,
			resource->header.length);
		// Validate resource.
		switch (resource->header.rsc_type) {
		case END_OF_RESOURCES:
			if (resource->header.length != sizeof(STM_RSC_END))
				return false;

			// If we are passed actual number of resources to add,
			// END_OF_RESOURCES structure between them is considered
			// an error. If NumEntries == 0 END_OF_RESOURCES is a
			// termination.
			if (num_entries != 0)
				return false;

			// If NumEntries == 0 and list reached end - return
			// success.
			return true;

		case MEM_RANGE:
		case MMIO_RANGE:
			printk(BIOS_DEBUG,
				"STM: %s - MEM (0x%0llx, 0x%0llx)\n",
				__func__,
				resource->mem.base,
				resource->mem.length);

			if (resource->header.length != sizeof(STM_RSC_MEM_DESC))
				return false;

			if (resource->mem.rwx_attributes > FULL_ACCS)
				return false;
			break;

		case IO_RANGE:
		case TRAPPED_IO_RANGE:
			if (resource->header.length != sizeof(STM_RSC_IO_DESC))
				return false;

			if ((resource->io.base + resource->io.length) > 0xFFFF)
				return false;
			break;

		case PCI_CFG_RANGE:
			printk(BIOS_DEBUG,
			       "STM: %s - PCI (0x%02x, 0x%08x, 0x%02x, 0x%02x)\n",
			       __func__,
			       resource->pci_cfg.originating_bus_number,
			       resource->pci_cfg.last_node_index,
			       resource->pci_cfg.pci_device_path[0].pci_device,
			       resource->pci_cfg.pci_device_path[0]
				       .pci_function);
			if (resource->header.length
			    != sizeof(STM_RSC_PCI_CFG_DESC)
				       + (sizeof(STM_PCI_DEVICE_PATH_NODE)
					  * resource->pci_cfg.last_node_index))
				return false;
			for (sub_index = 0;
			     sub_index <= resource->pci_cfg.last_node_index;
			     sub_index++) {
				if ((resource->pci_cfg
					     .pci_device_path[sub_index]
					     .pci_device
				     > 0x1F)
				    || (resource->pci_cfg
						.pci_device_path[sub_index]
						.pci_function
					> 7))
					return false;
			}
			if ((resource->pci_cfg.base + resource->pci_cfg.length)
			    > 0x1000)
				return false;
			break;

		case MACHINE_SPECIFIC_REG:
			if (resource->header.length != sizeof(STM_RSC_MSR_DESC))
				return false;
			break;

		default:
			printk(BIOS_DEBUG, "STM: %s - Unknown RscType(%x)\n",
			       __func__, resource->header.rsc_type);
			return false;
		}
		resource =
			(STM_RSC *)((void *)resource + resource->header.length);
	}
	return true;
}

/*
 * Get resource list.
 * EndResource is excluded.
 *
 *  @param resou rce_list  A pointer to resource list to be added
 *  @param num_entries    Optional number of entries.
 *			If 0, list must be terminated by END_OF_RESOURCES.
 *
 *  @retval true  resource valid
 *  @retval false resource invalid
 */
static uint32_t get_resource_size(STM_RSC *resource_list, uint32_t num_entries)
{
	uint32_t count;
	uint32_t index;
	STM_RSC *resource;

	resource = resource_list;

	// If NumEntries == 0 make it very big. Scan will be terminated by
	// END_OF_RESOURCES.
	if (num_entries == 0)
		count = 0xFFFFFFFF;
	else
		count = num_entries;

	// Start from beginning of resource list.
	resource = resource_list;

	for (index = 0; index < count; index++) {
		if (resource->header.rsc_type == END_OF_RESOURCES)
			break;
		resource =
			(STM_RSC *)((void *)resource + resource->header.length);
	}
	return (uint32_t)((uint32_t)resource - (uint32_t)resource_list);
}

/*
 * Add resources in list to database. Allocate new memory areas as needed.
 *
 *  @param resource_list  A pointer to resource list to be added
 *  @param num_entries    Optional number of entries.
 *			If 0, list must be terminated by END_OF_RESOURCES.
 *
 *  @retval SUCCESS       If resources are added
 *  @retval INVALID_PARAMETER  If nested procedure detected resource failure
 *  @retval OUT_OF_RESOURCES   If nested procedure returned it and we cannot
 *					allocate more areas.
 */
int add_pi_resource(STM_RSC *resource_list, uint32_t num_entries)
{
	size_t resource_size;

	printk(BIOS_DEBUG, "STM: %s - Enter\n", __func__);

	if (!validate_resource(resource_list, num_entries))
		return -1; // INVALID_PARAMETER;

	resource_size = get_resource_size(resource_list, num_entries);
	printk(BIOS_DEBUG, "STM: ResourceSize - 0x%08zx\n", resource_size);
	if (resource_size == 0)
		return -1; // INVALID_PARAMETER;

	if (m_stm_resources_ptr == NULL) {

		// Copy EndResource for initialization
		m_stm_resources_ptr = stm_resource_heap;
		m_stm_resource_total_size = CONFIG_BIOS_RESOURCE_LIST_SIZE;
		memset(m_stm_resources_ptr, 0, CONFIG_BIOS_RESOURCE_LIST_SIZE);

		memcpy(m_stm_resources_ptr, &m_rsc_end_node,
		       sizeof(m_rsc_end_node));
		m_stm_resource_size_used = sizeof(m_rsc_end_node);
		m_stm_resource_size_available =
			m_stm_resource_total_size - sizeof(m_rsc_end_node);
		wbinvd(); // force to memory

	} else {
		if (m_stm_resource_size_available < resource_size) {
			printk(BIOS_DEBUG,
			"STM: ERROR - not enough space for SMM resource list\n");
			return -1; // OUT_OF_RESOURCES
		}
	}

	// Check duplication
	add_resource(resource_list, num_entries);

	return 0; // SUCCESS;
}

/*
 * Delete resources in list to database.
 *
 *  @param resource_list  A pointer to resource list to be deleted
 *			 NULL means delete all resources.
 *  @param num_entries    Optional number of entries.
 *			 If 0, list must be terminated by END_OF_RESOURCES.
 *
 *  @retval SUCCESS            If resources are deleted
 *  @retval INVALID_PARAMETER  If nested procedure detected resource failure
 */
int32_t delete_pi_resource(STM_RSC *resource_list, uint32_t num_entries)
{
	if (resource_list != NULL) {
		// ASSERT (false);
		return -1; // UNSUPPORTED;
	}

	// Delete all
	memcpy(m_stm_resources_ptr, &m_rsc_end_node, sizeof(m_rsc_end_node));
	m_stm_resource_size_used = sizeof(m_rsc_end_node);
	m_stm_resource_size_available =
		m_stm_resource_total_size - sizeof(m_rsc_end_node);
	return 0; // SUCCESS;
}

/*
 * Get BIOS resources.
 *
 *  @param resource_list  A pointer to resource list to be filled
 *  @param resource_size  On input it means size of resource list input.
 *			  On output it means size of resource list filled,
 *			  or the size of resource list to be filled if size is
 *			  too small.
 *
 *  @retval SUCCESS            If resources are returned.
 *  @retval BUFFER_TOO_SMALL   If resource list buffer is too small to hold
 *				the whole resource list.
 */
int32_t get_pi_resource(STM_RSC *resource_list, uint32_t *resource_size)
{
	if (*resource_size < m_stm_resource_size_used) {
		*resource_size = (uint32_t)m_stm_resource_size_used;
		return -1; // BUFFER_TOO_SMALL;
	}

	memcpy(resource_list, m_stm_resources_ptr, m_stm_resource_size_used);
	*resource_size = (uint32_t)m_stm_resource_size_used;
	return 0; // SUCCESS;
}

/*
 *  Get 4K page aligned VMCS size.
 *  @return 4K page aligned VMCS size
 */
static uint32_t get_vmcs_size(void)
{
	uint32_t this_vmcs_size;
	VMX_BASIC_MSR msr_data64;
	int stm_support;

	msr_data64.msr = rdmsr(IA32_VMX_BASIC_MSR);

	this_vmcs_size = msr_data64.bits.vmcs_size;
	stm_support = msr_data64.bits.stm_supported;
	printk(BIOS_DEBUG, "STM: %s: Size %d StmSupport %d\n", __func__,
	       this_vmcs_size, stm_support);

	// VMCS require 0x1000 alignment
	this_vmcs_size = STM_PAGES_TO_SIZE(STM_SIZE_TO_PAGES(this_vmcs_size));

	return this_vmcs_size;
}

/*
 *  Create 4G page table for STM.
 *  2M PTEs for x86_64 or 2M PTEs for x86_32.
 *
 *  @param pageable_base        The page table base in MSEG
 */
void stm_gen_4g_pagetable_x64(uint32_t pagetable_base)
{
	uint32_t index;
	uint32_t sub_index;
	uint64_t *pde;
	uint64_t *pte;
	uint64_t *pml4;

	pml4 = (uint64_t *)(uint32_t)pagetable_base;
	pagetable_base += PTP_SIZE;
	*pml4 = pagetable_base | IA32_PG_RW | IA32_PG_P;

	pde = (uint64_t *)(uint32_t)pagetable_base;
	pagetable_base += PTP_SIZE;
	pte = (uint64_t *)(uint32_t)pagetable_base;

	for (index = 0; index < 4; index++) {
		*pde = pagetable_base | IA32_PG_RW | IA32_PG_P;
		pde++;
		pagetable_base += PTP_SIZE;

		for (sub_index = 0; sub_index < SIZE_4KB / sizeof(*pte);
		     sub_index++) {
			*pte = (((index << 9) + sub_index) << 21) | IA32_PG_PS
			       | IA32_PG_RW | IA32_PG_P;
			pte++;
		}
	}
}

/*
 * Check STM image size.
 *
 *  @param stm_image      STM image
 *  @param stm_imageSize  STM image size
 *
 *  @retval true  check pass
 *  @retval false check fail
 */

bool stm_check_stm_image(void *stm_image, uint32_t stm_imagesize)
{
	uint32_t min_mseg_size;
	STM_HEADER *stm_header;

	stm_header = (STM_HEADER *)stm_image;

	// Get Minimal required Mseg size
	min_mseg_size = (STM_PAGES_TO_SIZE(STM_SIZE_TO_PAGES(
				 stm_header->sw_stm_hdr.static_image_size))
			 + stm_header->sw_stm_hdr.additional_dynamic_memory_size
			 + (stm_header->sw_stm_hdr.per_proc_dynamic_memory_size
			    + get_vmcs_size() * 2)
				   * mp_state.cpu_count);
	if (min_mseg_size < stm_imagesize)
		min_mseg_size = stm_imagesize;

	if (stm_header->hw_stm_hdr.cr3_offset
	    >= stm_header->sw_stm_hdr.static_image_size) {

		// We will create page table, just in case that SINIT does not
		// create it.
		if (min_mseg_size < stm_header->hw_stm_hdr.cr3_offset
					    + STM_PAGES_TO_SIZE(6)) {
			min_mseg_size = stm_header->hw_stm_hdr.cr3_offset
					+ STM_PAGES_TO_SIZE(6);
		}
	}

	// Check if it exceeds MSEG size
	if (min_mseg_size > CONFIG_MSEG_SIZE)
		return false;

	return true;
}

/*
 *  This function return BIOS STM resource.
 *  Produced by SmmStm.
 *  Consumed by SmmMpService when Init.
 *
 *  @return BIOS STM resource
 */
void *get_stm_resource(void)
{
	return m_stm_resources_ptr;
}
