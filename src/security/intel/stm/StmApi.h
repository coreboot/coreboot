/* @file
 * STM API definition
 *
 * Copyright (c) 2015, Intel Corporation. All rights reserved.
 * This program and the accompanying materials are licensed and made available
 * under the terms and conditions of the BSD License which accompanies this
 * distribution.  The full text of the license may be found at
 * http://opensource.org/licenses/bsd-license.php.
 *
 * THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 * WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED.
 *
 */

#ifndef _STM_API_H_
#define _STM_API_H_

#include <stdint.h>

// definition in STM spec

#define STM_SPEC_VERSION_MAJOR 1
#define STM_SPEC_VERSION_MINOR 0

#pragma pack(push, 1)

#define STM_HARDWARE_FIELD_FILL_TO_2K (2048 - sizeof(uint32_t) * 8)
typedef struct {
	uint32_t stm_header_revision;
	uint32_t monitor_features;
	uint32_t gdtr_limit;
	uint32_t gdtr_base_offset;
	uint32_t cs_selector;
	uint32_t eip_offset;
	uint32_t esp_offset;
	uint32_t cr3_offset;
	uint8_t reserved[STM_HARDWARE_FIELD_FILL_TO_2K];
} HARDWARE_STM_HEADER;

#define STM_FEATURES_IA32E 0x1

typedef struct {
	uint32_t intel_64mode_supported : 1;
	uint32_t ept_supported : 1;
	uint32_t mbz : 30;
} STM_FEAT;

typedef struct {
	uint8_t stm_spec_ver_major;
	uint8_t stm_pec_ver_minor;
	uint16_t mbz;
	uint32_t static_image_size;
	uint32_t per_proc_dynamic_memory_size;
	uint32_t additional_dynamic_memory_size;
	STM_FEAT stm_features;
	uint32_t number_of_rev_ids;
	uint32_t stm_smm_rev_id[1];

	// The total STM_HEADER should be 4K.
} SOFTWARE_STM_HEADER;

typedef struct {
	HARDWARE_STM_HEADER hw_stm_hdr;
	SOFTWARE_STM_HEADER sw_stm_hdr;
} STM_HEADER;

#define SHA1 1
#define SHA256 2
typedef struct {
	uint64_t bios_component_base;
	uint32_t image_size;
	uint32_t hash_algorithm; // SHA1 or SHA256
	uint8_t hash[32];
} TXT_BIOS_COMPONENT_STATUS;

#define PAGE_SIZE 4096
typedef struct {
	uint32_t image_size;
	uint32_t reserved;
	uint64_t image_page_base[1]; //[NumberOfPages];
} TXT_BIOS_COMPONENT_UPDATE;

typedef struct {
	uint64_t spe_rip;
	uint64_t spe_rsp;
	uint16_t spe_ss;
	uint16_t page_violation_exception : 1;
	uint16_t msr_violation_exception : 1;
	uint16_t register_violation_exception : 1;
	uint16_t io_violation_exception : 1;
	uint16_t pci_violation_exception : 1;
	uint16_t reserved1 : 11;
	uint32_t reserved2;
} STM_PROTECTION_EXCEPTION_HANDLER;

typedef struct {
	uint8_t execution_disable_outside_smrr : 1;
	uint8_t intel_64mode : 1;
	uint8_t cr4_pae : 1;
	uint8_t cr4_pse : 1;
	uint8_t reserved1 : 4;
} STM_SMM_ENTRY_STATE;

typedef struct {
	uint8_t smram_to_vmcs_restore_required : 1; // BIOS restore hint
	uint8_t reinitialize_vmcs_required : 1;     // BIOS request
	uint8_t reserved2 : 6;
} STM_SMM_RESUME_STATE;

typedef struct {
	uint8_t domain_type : 4;    // STM input to BIOS on each SM
	uint8_t x_state_policy : 2; // STM input to BIOS on each SMI
	uint8_t ept_enabled : 1;
	uint8_t reserved3 : 1;
} STM_SMM_STATE;

typedef struct {
	uint64_t signature;
	uint16_t size;
	uint8_t smm_descriptor_ver_major;
	uint8_t smm_descriptor_ver_minor;
	uint32_t local_apic_id;
	STM_SMM_ENTRY_STATE smm_entry_state;
	STM_SMM_RESUME_STATE smm_resume_state;
	STM_SMM_STATE stm_smm_state;
	uint8_t reserved4;
	uint16_t smm_cs;
	uint16_t smm_ds;
	uint16_t smm_ss;
	uint16_t smm_other_segment;
	uint16_t smm_tr;
	uint16_t reserved5;
	uint64_t smm_cr3;
	uint64_t smm_stm_setup_rip;
	uint64_t smm_stm_teardown_rip;
	uint64_t smm_smi_handler_rip;
	uint64_t smm_smi_handler_rsp;
	uint64_t smm_gdt_ptr;
	uint32_t smm_gdt_size;
	uint32_t required_stm_smm_rev_id;
	STM_PROTECTION_EXCEPTION_HANDLER stm_protection_exception_handler;
	uint64_t reserved6;
	uint64_t bios_hw_resource_requirements_ptr;
	// extend area
	uint64_t acpi_rsdp;
	uint8_t physical_address_bits;
} TXT_PROCESSOR_SMM_DESCRIPTOR;

#define TXT_PROCESSOR_SMM_DESCRIPTOR_SIGNATURE "TXTPSSIG"
#define TXT_PROCESSOR_SMM_DESCRIPTOR_VERSION_MAJOR 1
#define TXT_PROCESSOR_SMM_DESCRIPTOR_VERSION_MINOR 0

#define SMM_PSD_OFFSET 0xfb00

typedef enum {
	TxtSmmPageViolation = 1,
	TxtSmmMsrViolation,
	TxtSmmRegisterViolation,
	TxtSmmIoViolation,
	TxtSmmPciViolation
} TXT_SMM_PROTECTION_EXCEPTION_TYPE;

typedef struct {
	uint32_t rdi;
	uint32_t rsi;
	uint32_t rbp;
	uint32_t rdx;
	uint32_t rcx;
	uint32_t rbx;
	uint32_t rax;
	uint32_t cr3;
	uint32_t cr2;
	uint32_t cr0;
	uint32_t vmcs_exit_instruction_info;
	uint32_t vmcs_exit_instruction_length;
	uint64_t vmcs_exit_qualification;
	uint32_t error_code; // TXT_SMM_PROTECTION_EXCEPTION_TYPE
	uint32_t rip;
	uint32_t cs;
	uint32_t rflags;
	uint32_t rsp;
	uint32_t ss;
} STM_PROTECTION_EXCEPTION_STACK_FRAME_IA32;

typedef struct {
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rbp;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
	uint64_t cr8;
	uint64_t cr3;
	uint64_t cr2;
	uint64_t cr0;
	uint64_t vmcs_exit_instruction_info;
	uint64_t vmcs_exit_instruction_length;
	uint64_t vmcs_exit_qualification;
	uint64_t error_code; // TXT_SMM_PROTECTION_EXCEPTION_TYPE
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
} STM_PROTECTION_EXCEPTION_STACK_FRAME_X64;

typedef union {
	STM_PROTECTION_EXCEPTION_STACK_FRAME_IA32 *ia32_stack_frame;
	STM_PROTECTION_EXCEPTION_STACK_FRAME_X64 *x64_stack_frame;
} STM_PROTECTION_EXCEPTION_STACK_FRAME;

#define STM_SMM_REV_ID 0x80010100

typedef struct _STM_SMM_CPU_STATE { //       Writable?
	uint8_t reserved1[0x1d0];   // fc00h
	uint32_t gdt_base_hi_dword; // fdd0h : NO
	uint32_t ldt_base_hi_dword; // fdd4h : NO
	uint32_t idt_base_hi_dword; // fdd8h : NO
	uint8_t reserved2[0x4];     // fddch
	uint64_t io_rdi;	    // fde0h : NO
				    // - restricted
	uint64_t io_eip;	    // fde8h : YES
	uint64_t io_rcx;	    // fdf0h : NO
				    // - restricted
	uint64_t io_rsi;	    // fdf8h : NO
				    // - restricted
	uint8_t reserved3[0x40];    // fe00h
	uint32_t cr4;		    // fe40h : NO
	uint8_t reserved4[0x48];    // fe44h
	uint32_t gdt_base_lo_dword; // fe8ch : NO
	uint32_t gdt_limit;	 // fe90h : NO
				    // - RESTRICTED
	uint32_t idt_base_lo_dword; // fe94h : NO
	uint32_t idt_limit;	 // fe98h : NO
				    // - RESTRICTED
	uint32_t ldt_base_lo_dword; // fe9ch : NO
	uint32_t ldt_limit;	 // fea0h : NO
				    // - RESTRICTED
	uint32_t ldt_info;	  // fea4h : NO
				    // - RESTRICTED
	uint8_t reserved5[0x30];    // fea8h
	uint64_t eptp;		    // fed8h : NO
	uint32_t enabled_ept;       // fee0h : NO
	uint8_t reserved6[0x14];    // fee4h
	uint32_t smbase;	    // fef8h : YES
				    // - NO for STM
	uint32_t smm_rev_id;	// fefch : NO
	uint16_t io_restart;	// ff00h : YES
	uint16_t auto_halt_restart; // ff02h : YES
	uint8_t reserved7[0x18];    // ff04h
	uint64_t r15;		    // ff1ch : YES
	uint64_t r14;		    // ff24h : YES
	uint64_t r13;		    // ff2ch : YES
	uint64_t r12;		    // ff34h : YES
	uint64_t r11;		    // ff3ch : YES
	uint64_t r10;		    // ff44h : YES
	uint64_t r9;		    // ff4ch : YES
	uint64_t r8;		    // ff54h : YES
	uint64_t rax;		    // ff5ch : YES
	uint64_t rcx;		    // ff64h : YES
	uint64_t rdx;		    // ff6ch : YES
	uint64_t rbx;		    // ff74h : YES
	uint64_t rsp;		    // ff7ch : YES
	uint64_t rbp;		    // ff84h : YES
	uint64_t rsi;		    // ff8ch : YES
	uint64_t rdi;		    // ff94h : YES
	uint64_t io_mem_addr;       // ff9ch : NO
	uint32_t io_misc;	   // ffa4h : NO
	uint32_t es;		    // ffa8h : NO
	uint32_t cs;		    // ffach : NO
	uint32_t ss;		    // ffb0h : NO
	uint32_t ds;		    // ffb4h : NO
	uint32_t fs;		    // ffb8h : NO
	uint32_t gs;		    // ffbch : NO
	uint32_t ldtr;		    // ffc0h : NO
	uint32_t tr;		    // ffc4h : NO
	uint64_t dr7;		    // ffc8h : NO
	uint64_t dr6;		    // ffd0h : NO
	uint64_t rip;		    // ffd8h : YES
	uint64_t ia32_efer;	 // ffe0h : YES
				    // - NO for STM
	uint64_t rflags;	    // ffe8h : YES
	uint64_t cr3;		    // fff0h : NO
	uint64_t cr0;		    // fff8h : NO
} STM_SMM_CPU_STATE;

// STM Mapping
typedef struct {
	uint64_t physical_address;
	uint64_t virtual_ddress;
	uint32_t Page_count;
	uint32_t Pat_cache_type;
} STM_MAP_ADDRESS_RANGE_DESCRIPTOR;

#define ST_UC 0x00
#define WC 0x01
#define WT 0x04
#define WP 0x05
#define WB 0x06
#define UC 0x07
#define FOLLOW_MTRR 0xFFFFFFFF

typedef struct {
	uint64_t virtual_address;
	uint32_t length;
} STM_UNMAP_ADDRESS_RANGE_DESCRIPTOR;

typedef struct {
	uint64_t interrupted_guest_virtual_address;
	uint32_t length;
	uint64_t interrupted_cr3;
	uint64_t interrupted_eptp;
	uint32_t map_to_smm_guest : 2;
	uint32_t interrupted_cr4_pae : 1;
	uint32_t interrupted_cr4_pse : 1;
	uint32_t interrupted_ia32e_mode : 1;
	uint32_t reserved1 : 27;
	uint32_t reserved2;
	uint64_t physical_address;
	uint64_t smm_guest_virtual_address;
} STM_ADDRESS_LOOKUP_DESCRIPTOR;

#define DO_NOT_MAP 0
#define ONE_TO_ONE 1
#define VIRTUAL_ADDRESS_SPECIFIED 3

// STM_RESOURCE_LIST
#define END_OF_RESOURCES 0
#define MEM_RANGE 1
#define IO_RANGE 2
#define MMIO_RANGE 3
#define MACHINE_SPECIFIC_REG 4
#define PCI_CFG_RANGE 5
#define TRAPPED_IO_RANGE 6
#define ALL_RESOURCES 7
#define REGISTER_VIOLATION 8
#define MAX_DESC_TYPE 8

typedef struct {
	uint32_t rsc_type;
	uint16_t length;
	uint16_t return_status : 1;
	uint16_t reserved : 14;
	uint16_t ignore_resource : 1;
} STM_RSC_DESC_HEADER;

typedef struct {
	STM_RSC_DESC_HEADER Hdr;
	uint64_t resource_list_continuation;
} STM_RSC_END;

// byte granular Memory range support
#define STM_RSC_BGM 0x4

typedef struct {
	STM_RSC_DESC_HEADER hdr;
	uint64_t base;
	uint64_t length;
	uint32_t rwx_attributes : 3;
	uint32_t reserved : 29;
	uint32_t reserved_2;
} STM_RSC_MEM_DESC;

#define STM_RSC_MEM_R 0x1
#define STM_RSC_MEM_W 0x2
#define STM_RSC_MEM_X 0x4

typedef struct {
	STM_RSC_DESC_HEADER hdr;
	uint16_t base;
	uint16_t length;
	uint32_t reserved;
} STM_RSC_IO_DESC;

// byte granular MMIO range support
#define STM_RSC_BGI 0x2

typedef struct {
	STM_RSC_DESC_HEADER hdr;
	uint64_t base;
	uint64_t length;
	uint32_t rwx_attributes : 3;
	uint32_t reserved : 29;
	uint32_t reserved_2;
} STM_RSC_MMIO_DESC;

#define STM_RSC_MMIO_R 0x1
#define STM_RSC_MMIO_W 0x2
#define STM_RSC_MMIO_X 0x4

typedef struct {
	STM_RSC_DESC_HEADER hdr;
	uint32_t msr_index;
	uint32_t kernel_mode_processing : 1;
	uint32_t reserved : 31;
	uint64_t read_mask;
	uint64_t write_mask;
} STM_RSC_MSR_DESC;

// bit granular MSR resource support
#define STM_RSC_MSR 0x8

typedef struct {
	uint8_t type;    // must be 1, indicating Hardware Device Path
	uint8_t subtype; // must be 1, indicating PCI
	uint16_t length; // sizeof(STM_PCI_DEVICE_PATH_NODE) which is 6
	uint8_t pci_function;
	uint8_t pci_device;
} STM_PCI_DEVICE_PATH_NODE;

typedef struct {
	STM_RSC_DESC_HEADER hdr;
	uint16_t rw_attributes : 2;
	uint16_t reserved : 14;
	uint16_t base;
	uint16_t length;
	uint8_t originating_bus_number;
	uint8_t last_node_index;
	STM_PCI_DEVICE_PATH_NODE pci_device_path[1];
	// STM_PCI_DEVICE_PATH_NODE  PciDevicePath[LastNodeIndex + 1];
} STM_RSC_PCI_CFG_DESC;

#define STM_RSC_PCI_CFG_R 0x1
#define STM_RSC_PCI_CFG_W 0x2

typedef struct {
	STM_RSC_DESC_HEADER hdr;
	uint16_t base;
	uint16_t length;
	uint16_t in : 1;
	uint16_t out : 1;
	uint16_t api : 1;
	uint16_t reserved1 : 13;
	uint16_t reserved2;
} STM_RSC_TRAPPED_IO_DESC;

typedef struct {
	STM_RSC_DESC_HEADER hdr;
} STM_RSC_ALL_RESOURCES_DESC;

typedef struct {
	STM_RSC_DESC_HEADER hdr;
	uint32_t register_type;
	uint32_t reserved;
	uint64_t readMask;
	uint64_t write_mask;
} STM_REGISTER_VIOLATION_DESC;

typedef enum {
	stm_register_cr0,
	stm_register_cr2,
	stm_register_cr3,
	stm_register_cr4,
	stm_register_cr8,
	stm_register_max,
} STM_REGISTER_VIOLATION_TYPE;

typedef union {
	STM_RSC_DESC_HEADER header;
	STM_RSC_END end;
	STM_RSC_MEM_DESC mem;
	STM_RSC_IO_DESC io;
	STM_RSC_MMIO_DESC mmio;
	STM_RSC_MSR_DESC msr;
	STM_RSC_PCI_CFG_DESC pci_cfg;
	STM_RSC_TRAPPED_IO_DESC trapped_io;
	STM_RSC_ALL_RESOURCES_DESC all;
	STM_REGISTER_VIOLATION_DESC register_violation;
} STM_RSC;

// VMCS database
#define STM_VMCS_DATABASE_REQUEST_ADD 1
#define STM_VMCS_DATABASE_REQUEST_REMOVE 0

// Values for DomainType
// Interpreter of DomainType
#define DOMAIN_DISALLOWED_IO_OUT (1u << 0)
#define DOMAIN_DISALLOWED_IO_IN (1u << 1)
#define DOMAIN_INTEGRITY (1u << 2)
#define DOMAIN_CONFIDENTIALITY (1u << 3)

#define DOMAIN_UNPROTECTED 0x00
#define DOMAIN_INTEGRITY_PROT_OUT_IN (DOMAIN_INTEGRITY)
#define DOMAIN_FULLY_PROT_OUT_IN (DOMAIN_CONFIDENTIALITY | DOMAIN_INTEGRITY)
#define DOMAIN_FULLY_PROT                                                      \
	(DOMAIN_CONFIDENTIALITY | DOMAIN_INTEGRITY | DOMAIN_DISALLOWED_IO_IN   \
	 | DOMAIN_DISALLOWED_IO_OUT)

// Values for XStatePolicy
#define XSTATE_READWRITE 0x00
#define XSTATE_READONLY 0x01
#define XSTATE_SCRUB 0x03

typedef struct {
	uint64_t vmcs_phys_pointer; // bits 11:0 are reserved and must be 0
	uint32_t domain_type : 4;
	uint32_t x_state_policy : 2;
	uint32_t degradation_policy : 4;
	uint32_t reserved1 : 22; // Must be 0
	uint32_t add_or_remove;
} STM_VMCS_DATABASE_REQUEST;

// Event log
#define NEW_LOG 1
#define CONFIGURE_LOG 2
#define START_LOG 3
#define STOP_LOG 4
#define CLEAR_LOG 5
#define DELETE_LOG 6
typedef enum {
	evt_log_started,
	evt_log_stopped,
	evt_log_invalid_parameter_detected,
	evt_handled_protection_exception,
	// unhandled protection exceptions result in reset & cannot be logged
	evt_bios_access_to_unclaimed_resource,
	evt_mle_resource_protection_granted,
	evt_mle_resource_protection_denied,
	evt_mle_resource_unprotect,
	evt_mle_resource_unprotect_error,
	evt_mle_domain_type_degraded,
	// add more here
	evt_mle_max,
	// Not used
	evt_invalid = 0xFFFFFFFF,
} EVENT_TYPE;

typedef struct {
	uint32_t page_count;
	uint64_t pages[1]; // number of elements is PageCount
} STM_EVENT_LOG_MANAGEMENT_REQUEST_DATA_LOG_BUFFER;

typedef union {
	STM_EVENT_LOG_MANAGEMENT_REQUEST_DATA_LOG_BUFFER log_buffer;
	uint32_t event_enable_bitmap; // bitmap of EVENT_TYPE
} STM_EVENT_LOG_MANAGEMENT_REQUEST_DATA;

typedef struct {
	uint32_t sub_functionindex;
	STM_EVENT_LOG_MANAGEMENT_REQUEST_DATA data;
} STM_EVENT_LOG_MANAGEMENT_REQUEST;

// VMCALL API Numbers
//
// API number convention: BIOS facing VMCALL interfaces have bit 16 clear
#define STM_API_MAP_ADDRESS_RANGE 0x00000001
#define STM_API_UNMAP_ADDRESS_RANGE 0x00000002
#define STM_API_ADDRESS_LOOKUP 0x00000003
#define STM_API_RETURN_FROM_PROTECTION_EXCEPTION 0x00000004

// API number convention: MLE facing VMCALL interfaces have bit 16 set
//
// The STM configuration lifecycle is as follows:
// 1. SENTER->SINIT->MLE: MLE begins execution with SMI disabled (masked).
// 2. MLE invokes InitializeProtectionVMCALL() to prepare STM for setup of
//    initial protection profile. This is done on a single CPU and has global
//    effect.
// 3. MLE invokes ProtectResourceVMCALL() to define the initial protection
//    profile. The protection profile is global across all CPUs.
// 4. MLE invokes StartStmVMCALL() to enable the STM to begin receiving SMI
//    events. This must be done on every logical CPU.
// 5. MLE may invoke ProtectResourceVMCALL() or UnProtectResourceVMCALL()
//    during runtime as many times as necessary.
// 6. MLE invokes StopStmVMCALL() to disable the STM. SMI is again masked
//    following StopStmVMCALL().
//
#define STM_API_START 0x00010001
#define STM_API_STOP 0x00010002
#define STM_API_PROTECT_RESOURCE 0x00010003
#define STM_API_UNPROTECT_RESOURCE 0x00010004
#define STM_API_GET_BIOS_RESOURCES 0x00010005
#define STM_API_MANAGE_VMCS_DATABASE 0x00010006
#define STM_API_INITIALIZE_PROTECTION 0x00010007
#define STM_API_MANAGE_EVENT_LOG 0x00010008

// Return codes
typedef uint32_t STM_STATUS;

#define STM_SUCCESS 0x00000000
#define SMM_SUCCESS 0x00000000
// all error codes have bit 31 set
// STM errors have bit 16 set
#define ERROR_STM_SECURITY_VIOLATION 0x80010001
#define ERROR_STM_CACHE_TYPE_NOT_SUPPORTED 0x80010002
#define ERROR_STM_PAGE_NOT_FOUND 0x80010003
#define ERROR_STM_BAD_CR3 0x80010004
#define ERROR_STM_PHYSICAL_OVER_4G 0x80010005
#define ERROR_STM_VIRTUAL_SPACE_TOO_SMALL 0x80010006
#define ERROR_STM_UNPROTECTABLE_RESOURCE 0x80010007
#define ERROR_STM_ALREADY_STARTED 0x80010008
#define ERROR_STM_WITHOUT_SMX_UNSUPPORTED 0x80010009
#define ERROR_STM_STOPPED 0x8001000A
#define ERROR_STM_BUFFER_TOO_SMALL 0x8001000B
#define ERROR_STM_INVALID_VMCS_DATABASE 0x8001000C
#define ERROR_STM_MALFORMED_RESOURCE_LIST 0x8001000D
#define ERROR_STM_INVALID_PAGECOUNT 0x8001000E
#define ERROR_STM_LOG_ALLOCATED 0x8001000F
#define ERROR_STM_LOG_NOT_ALLOCATED 0x80010010
#define ERROR_STM_LOG_NOT_STOPPED 0x80010011
#define ERROR_STM_LOG_NOT_STARTED 0x80010012
#define ERROR_STM_RESERVED_BIT_SET 0x80010013
#define ERROR_STM_NO_EVENTS_ENABLED 0x80010014
#define ERROR_STM_OUT_OF_RESOURCES 0x80010015
#define ERROR_STM_FUNCTION_NOT_SUPPORTED 0x80010016
#define ERROR_STM_UNPROTECTABLE 0x80010017
#define ERROR_STM_UNSUPPORTED_MSR_BIT 0x80010018
#define ERROR_STM_UNSPECIFIED 0x8001FFFF

// SMM errors have bit 17 set
#define ERROR_SMM_BAD_BUFFER 0x80020001
#define ERROR_SMM_INVALID_RSC 0x80020004
#define ERROR_SMM_INVALID_BUFFER_SIZE 0x80020005
#define ERROR_SMM_BUFFER_TOO_SHORT 0x80020006
#define ERROR_SMM_INVALID_LIST 0x80020007
#define ERROR_SMM_OUT_OF_MEMORY 0x80020008
#define ERROR_SMM_AFTER_INIT 0x80020009
#define ERROR_SMM_UNSPECIFIED 0x8002FFFF

// Errors that apply to both have bits 15, 16, and 17 set
#define ERROR_INVALID_API 0x80038001
#define ERROR_INVALID_PARAMETER 0x80038002

// STM TXT.ERRORCODE codes
#define STM_CRASH_PROTECTION_EXCEPTION 0xC000F001
#define STM_CRASH_PROTECTION_EXCEPTION_FAILURE 0xC000F002
#define STM_CRASH_DOMAIN_DEGRADATION_FAILURE 0xC000F003
#define STM_CRASH_BIOS_PANIC 0xC000E000

typedef struct {
	uint32_t event_serial_number;
	uint16_t type;
	uint16_t lock : 1;
	uint16_t valid : 1;
	uint16_t read_by_mle : 1;
	uint16_t wrapped : 1;
	uint16_t reserved : 12;
} LOG_ENTRY_HEADER;

typedef struct {
	uint32_t reserved;
} ENTRY_EVT_LOG_STARTED;

typedef struct {
	uint32_t reserved;
} ENTRY_EVT_LOG_STOPPED;

typedef struct {
	uint32_t vmcall_api_number;
} ENTRY_EVT_LOG_INVALID_PARAM;

typedef struct {
	STM_RSC resource;
} ENTRY_EVT_LOG_HANDLED_PROTECTION_EXCEPTION;

typedef struct {
	STM_RSC resource;
} ENTRY_EVT_BIOS_ACCESS_UNCLAIMED_RSC;

typedef struct {
	STM_RSC resource;
} ENTRY_EVT_MLE_RSC_PROT_GRANTED;

typedef struct {
	STM_RSC resource;
} ENTRY_EVT_MLE_RSC_PROT_DENIED;

typedef struct {
	STM_RSC resource;
} ENTRY_EVT_MLE_RSC_UNPROT;

typedef struct {
	STM_RSC resource;
} ENTRY_EVT_MLE_RSC_UNPROT_ERROR;

typedef struct {
	uint64_t vmcs_phys_pointer;
	uint8_t expected_domain_type;
	uint8_t degraded_domain_type;
} ENTRY_EVT_MLE_DOMAIN_TYPE_DEGRADED;

typedef union {
	ENTRY_EVT_LOG_STARTED started;
	ENTRY_EVT_LOG_STOPPED stopped;
	ENTRY_EVT_LOG_INVALID_PARAM invalid_param;
	ENTRY_EVT_LOG_HANDLED_PROTECTION_EXCEPTION
	handled_protection_exception;
	ENTRY_EVT_BIOS_ACCESS_UNCLAIMED_RSC bios_unclaimed_rsc;
	ENTRY_EVT_MLE_RSC_PROT_GRANTED mle_rsc_prot_granted;
	ENTRY_EVT_MLE_RSC_PROT_DENIED mle_rsc_prot_denied;
	ENTRY_EVT_MLE_RSC_UNPROT mle_rsc_unprot;
	ENTRY_EVT_MLE_RSC_UNPROT_ERROR mle_rsc_unprot_error;
	ENTRY_EVT_MLE_DOMAIN_TYPE_DEGRADED mle_domain_type_degraded;
} LOG_ENTRY_DATA;

typedef struct {
	LOG_ENTRY_HEADER hdr;
	LOG_ENTRY_DATA data;
} STM_LOG_ENTRY;

#define STM_LOG_ENTRY_SIZE 256
#define STM_CONFIG_SMI_UNBLOCKING_BY_VMX_OFF 0x1

// TXT debug
#define SW_SMI_STM_ADD_RUNTIME_RESOURCES_SUB_FUNC 0
#define SW_SMI_STM_READ_BIOS_RESOURCES_SUB_FUNC 1
#define SW_SMI_STM_REPLACE_BIOS_RESOURCES_SUB_FUNC 2

typedef struct {
	uint32_t buffer_size;
	uint32_t reserved;
	// uint8_t   Data[];
} TXT_BIOS_DEBUG;

#pragma pack(pop)

#endif
