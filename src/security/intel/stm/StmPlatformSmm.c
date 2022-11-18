/* SPDX-License-Identifier: BSD-2-Clause */

#include <security/intel/stm/StmApi.h>
#include <security/intel/stm/SmmStm.h>
#include <security/intel/stm/StmPlatformResource.h>
#include <security/tpm/tspi.h>
#include <cpu/x86/smm.h>
#include <cpu/x86/msr.h>

#include <cbfs.h>
#include <console/console.h>
#include <stdbool.h>
#include <stdint.h>
#include <arch/rom_segs.h>

/*
 * Load STM image to MSEG
 *
 * @retval SUCCESS           STM is loaded to MSEG
 */
int load_stm_image(uintptr_t mseg)
{
	int status;
	void *mseg_base;
	uint32_t stm_buffer_size;
	uint32_t stm_image_size;
	bool stm_status;

	STM_HEADER *stm_header;

	// Extract STM image from FV
	mseg_base = (void *)mseg;
	stm_buffer_size = CONFIG_MSEG_SIZE;
	stm_image_size = 0;

	memset((void *)mseg_base, 0, CONFIG_MSEG_SIZE); // clear the mseg

	stm_image_size = cbfs_load("stm.bin", mseg_base, stm_buffer_size);
	printk(BIOS_DEBUG, "STM:loaded into mseg: 0x%p size: %u\n", mseg_base,
	       stm_image_size);
	/* status is number of bytes loaded */
	stm_status = stm_check_stm_image(mseg_base, stm_image_size);

	if (!stm_status) {
		printk(BIOS_DEBUG, "STM: Error in STM image\n");
		return -1;
	}

	stm_header = mseg_base;

	stm_gen_4g_pagetable_x64((uint32_t)mseg_base
				 + stm_header->hw_stm_hdr.cr3_offset);

	// Debug stuff
	printk(BIOS_DEBUG,
	       "STM: Header-Revision %d Features 0x%08x Cr3Offset 0x%08x\n",
	       stm_header->hw_stm_hdr.stm_header_revision,
	       stm_header->hw_stm_hdr.monitor_features,
	       stm_header->hw_stm_hdr.cr3_offset);
	printk(BIOS_DEBUG,
	       "STM: Header-StaticImageSize: %d  Cr3Location: 0x%08x\n",
	       stm_header->sw_stm_hdr.static_image_size,
	       ((uint32_t)mseg_base + stm_header->hw_stm_hdr.cr3_offset));

	status = 0; // always return good for now

	return status;
}

struct descriptor {
	uint16_t limit;
	uintptr_t base;
} __attribute__((packed));

static void read_gdtr(struct descriptor *gdtr)
{
	__asm__ __volatile__("sgdt %0" : "=m"(*gdtr));
}

void setup_smm_descriptor(void *smbase, int32_t apic_id, int32_t entry32_off)
{
	struct descriptor gdtr;
	void *smbase_processor;
	//msr_t smbase_msr;

	TXT_PROCESSOR_SMM_DESCRIPTOR *psd;

	smbase_processor =  (void *)SMM_DEFAULT_BASE;//we are here
	psd = smbase + SMM_PSD_OFFSET;

	printk(BIOS_DEBUG,
	"STM: Smm Descriptor setup: Smbase: %p Smbase_processor: %p Psd: %p\n",
		smbase,
		smbase_processor,
		psd);

	memset(psd, 0, sizeof(TXT_PROCESSOR_SMM_DESCRIPTOR));

	memcpy(&psd->signature, TXT_PROCESSOR_SMM_DESCRIPTOR_SIGNATURE, 8);
	psd->smm_descriptor_ver_major =
		TXT_PROCESSOR_SMM_DESCRIPTOR_VERSION_MAJOR;
	psd->smm_descriptor_ver_minor =
		TXT_PROCESSOR_SMM_DESCRIPTOR_VERSION_MINOR;
	psd->smm_smi_handler_rip =
		(uint64_t)((uintptr_t)smbase + SMM_ENTRY_OFFSET +
		entry32_off);
	psd->local_apic_id = apic_id;
	psd->size = sizeof(TXT_PROCESSOR_SMM_DESCRIPTOR);
	psd->acpi_rsdp = 0;
	psd->bios_hw_resource_requirements_ptr =
		(uint64_t)((uintptr_t)get_stm_resource());
	psd->smm_cs = ROM_CODE_SEG;
	psd->smm_ds = ROM_DATA_SEG;
	psd->smm_ss = ROM_DATA_SEG;
	psd->smm_other_segment = ROM_DATA_SEG;
	psd->smm_tr = SMM_TASK_STATE_SEG;

	// At this point the coreboot smm_stub is relative to the default
	// smbase and not the one for the smi handler in tseg.  So we have
	// to adjust the gdtr.base

	read_gdtr(&gdtr);

	gdtr.base -= (uintptr_t)smbase_processor;
	gdtr.base += (uintptr_t)smbase;

	psd->smm_gdt_ptr = gdtr.base;
	psd->smm_gdt_size = gdtr.limit + 1; // the stm will subtract, so add
	printk(BIOS_DEBUG, "STM: Smm Descriptor setup complete - Smbase: %p Psd: %p\n",
		smbase, psd);
}

extern uint8_t *stm_resource_heap;

#define FXSAVE_SIZE 512

static int stm_load_status = 0;

void stm_setup(uintptr_t mseg, int cpu, uintptr_t smbase,
			uintptr_t base_smbase, uint32_t offset32)
{
	msr_t InitMseg;
	msr_t MsegChk;
	msr_t vmx_basic;

	uintptr_t addr_calc;  // used to calculate the stm resource heap area

	printk(BIOS_DEBUG, "STM: set up for cpu %d\n", cpu);

	vmx_basic = rdmsr(IA32_VMX_BASIC_MSR);

	// Does this processor support an STM?
	if ((vmx_basic.hi & VMX_BASIC_HI_DUAL_MONITOR) != VMX_BASIC_HI_DUAL_MONITOR) {
		printk(BIOS_WARNING, "STM: not supported on CPU %d\n", cpu);
		return;
	}

	// This code moved here because paralled SMM set can cause
	// some processor to receive a bad value
	// calculate the location in SMRAM
	addr_calc = mseg - CONFIG_BIOS_RESOURCE_LIST_SIZE;
	stm_resource_heap = (uint8_t *)addr_calc;

	if (cpu == 0) {

		// need to create the BIOS resource list once
		printk(BIOS_DEBUG, "STM: stm_resource_heap located at %p\n",
				stm_resource_heap);
		//setup the list
		add_resources_cmd();

		stm_load_status = load_stm_image(mseg);
	}

	if (stm_load_status == 0) {
		// enable STM for this cpu
		InitMseg.lo = mseg | IA32_SMM_MONITOR_VALID;
		InitMseg.hi = 0;

		wrmsr(IA32_SMM_MONITOR_CTL_MSR, InitMseg);

		MsegChk = rdmsr(IA32_SMM_MONITOR_CTL_MSR);

		printk(BIOS_DEBUG, "STM: MSEG Initialized (%d) 0x%08x 0x%08x\n",
			cpu, MsegChk.hi, MsegChk.lo);

		// setup the descriptor for this cpu
		setup_smm_descriptor((void *)smbase, cpu, offset32);

	} else {
		printk(BIOS_DEBUG,
			"STM: Error in STM load, STM not enabled: %d\n",
			cpu);
	}
}
