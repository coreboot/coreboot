/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CPER_H_
#define _CPER_H_

#include <commonlib/bsd/bcd.h>
#include <rtc.h>
#include <types.h>
#include <uuid.h>

/* This file contains some definitions and helpers for implementing structures
 * in the UEFI specification, Appendix "Common Platform Error Record".  This
 * is not a complete definition, but contains enough to generate a BERT ACPI
 * table.
 *
 * All table numbers and references correspond with UEFI spec version 2.7,
 * errata A, available at uefi.org.
 */

/* Error Record Header, Timestamp field (Table 251) */
typedef struct cper_timestamp { /* BCD values */
	u8 sec;
	u8 min;
	u8 hour;
	u8 precise; /* b[0] precise - timestamp of error,  b[7:1] rsvd */
	u8 day;
	u8 month;
	u8 year;
	u8 century;
} __packed cper_timestamp_t;

#define CPER_TIMESTAMP_PRECISE			BIT(0)

/* Section Descriptor, Flags field (Table 253) */
#define CPER_SEC_PRIMARY			BIT(0)
#define CPER_SEC_CONTAINMENT_WARNING		BIT(1)
#define CPER_SEC_RESET				BIT(2)
#define CPER_SEC_ERROR_THRESHOLD_EXCEEDED	BIT(3)
#define CPER_SEC_RESOURCE_NOT_ACCESSIBLE	BIT(4)
#define CPER_SEC_LATENT_ERROR			BIT(5)
#define CPER_SEC_PROPAGATED			BIT(6)
#define CPER_SEC_OVERFLOW			BIT(7)

/* Section Descriptor, Section Types values (Table 253) */
#define CPER_SEC_PROC_GENERIC_GUID			\
		GUID_INIT(0x9876ccad, 0x47b4, 0x4bdb,	\
		0xb6, 0x5e, 0x16, 0xf1, 0x93, 0xc4, 0xf3, 0xdb)
#define CPER_SEC_PROC_IA32X64_GUID			\
		GUID_INIT(0xdc3ea0b0, 0xa144, 0x4797,	\
		0xb9, 0x5b, 0x53, 0xfa, 0x24, 0x2b, 0x6e, 0x1d)
#define CPER_SEC_PROC_ARM_GUID				\
		GUID_INIT(0xe19e3d16, 0xbc11, 0x11e4,	\
		0x9c, 0xaa, 0xc2, 0x05, 0x1d, 0x5d, 0x46, 0xb0)
#define CPER_SEC_PLATFORM_MEM_GUID			\
		GUID_INIT(0xa5bc1114, 0x6f64, 0x4ede,	\
		0xb8, 0x63, 0x3e, 0x83, 0xed, 0x7c, 0x83, 0xb1)
#define CPER_SEC_PLATFORM_MEM2_GUID			\
		GUID_INIT(0x61ec04fc, 0x48e6, 0xd813,	\
		0x25, 0xc9, 0x8d, 0xaa, 0x44, 0x75, 0x0b, 0x12)
#define CPER_SEC_PCIE_GUID				\
		GUID_INIT(0xd995e954, 0xbbc1, 0x430f,	\
		0xad, 0x91, 0xb4, 0x4d, 0xcb, 0x3c, 0x6f, 0x35)
#define CPER_SEC_FW_ERR_REC_REF_GUID			\
		GUID_INIT(0x81212a96, 0x09ed, 0x4996,	\
		0x94, 0x71, 0x8d, 0x72, 0x9c, 0x8e, 0x69, 0xed)
#define CPER_SEC_PCI_X_BUS_GUID				\
		GUID_INIT(0xc5753963, 0x3b84, 0x4095,	\
		0xbf, 0x78, 0xed, 0xda, 0xd3, 0xf9, 0xc9, 0xdd)
#define CPER_SEC_PCI_DEV_GUID				\
		GUID_INIT(0xeb5e4685, 0xca66, 0x4769,	\
		0xb6, 0xa2, 0x26, 0x06, 0x8b, 0x00, 0x13, 0x26)
#define CPER_SEC_DMAR_GENERIC_GUID			\
		GUID_INIT(0x5b51fef7, 0xc79d, 0x4434,	\
		0x8f, 0x1b, 0xaa, 0x62, 0xde, 0x3e, 0x2c, 0x64)
#define CPER_SEC_DMAR_VT_GUID				\
		GUID_INIT(0x71761d37, 0x32b2, 0x45cd,	\
		0xa7, 0xd0, 0xb0, 0xfe, 0xdd, 0x93, 0xe8, 0xcf)
#define CPER_SEC_DMAR_IOMMU_GUID			\
		GUID_INIT(0x036f84e1, 0x7f37, 0x428c,	\
		0xa7, 0x9e, 0x57, 0x5f, 0xdf, 0xaa, 0x84, 0xec)

/*
 * Processor Generic Error Section (Table 254)
 */
typedef struct cper_proc_generic_error_section {
	u64 validation;
	u8 proc_type;
	u8 proc_isa;
	u8 error_type;
	u8 operation;
	u8 flags;
	u8 level;
	u16 reserved;
	u64 cpu_version;
	char cpu_brand_string[128];
	u64 proc_id;
	u64 target_addr;
	u64 requestor_id;
	u64 responder_id;
	u64 instruction_ip;
} __packed cper_proc_generic_error_section_t;

/* Processor Generic Error Section, Validation Bits field (Table 254) */
#define GENPROC_VALID_PROC_TYPE			BIT(0)
#define GENPROC_VALID_PROC_ISA			BIT(1)
#define GENPROC_VALID_PROC_ERR_TYPE		BIT(2)
#define GENPROC_VALID_OPERATION			BIT(3)
#define GENPROC_VALID_FLAGS			BIT(4)
#define GENPROC_VALID_LEVEL			BIT(5)
#define GENPROC_VALID_CPU_VERSION		BIT(6)
#define GENPROC_VALID_CPU_BRAND			BIT(7)
#define GENPROC_VALID_CPU_ID			BIT(8) /* LAPIC ID, not CPUID */
#define GENPROC_VALID_TGT_ADDR			BIT(9)
#define GENPROC_VALID_REQR_ID			BIT(10)
#define GENPROC_VALID_RSPR_ID			BIT(11)
#define GENPROC_VALID_INSTR_IP			BIT(12)

/* Processor Generic Error Section, Processor Type field (Table 254) */
#define GENPROC_PROCTYPE_IA32X64		0x0
#define GENPROC_PROCTYPE_IA64			0x1
#define GENPROC_PROCTYPE_ARM			0x2

/* Processor Generic Error Section, Processor ISA (@time of fail) (Table 254) */
#define GENPROC_ISA_IA32			0x0
#define GENPROC_ISA_IA64			0x1
#define GENPROC_ISA_X64				0x2
#define GENPROC_ISA_ARM32			0x3
#define GENPROC_ISA_ARM64			0x4

/* error_type definitions */
/* Processor Generic Error Section, Processor Error Type field (Table 254) */
#define GENPROC_ERRTYPE_UNKNOWN			0x0
#define GENPROC_ERRTYPE_CACHE			0x1
#define GENPROC_ERRTYPE_TLB			0x2
#define GENPROC_ERRTYPE_BUS			0x4
#define GENPROC_ERRTYPE_UARCH			0x8

/* Processor Generic Error Section, Operation field (Table 254) */
#define GENPROC_ERROP_UNKNOWN			0x0
#define GENPROC_ERROP_READ			0x1
#define GENPROC_ERROP_WRITE			0x2
#define GENPROC_ERROP_EXECUTION			0x3

/* Processor Generic Error Section, Flags field (Table 254) */
#define GENPROC_FLAG_RESTARTABLE		0x0
#define GENPROC_FLAG_PRECISE_IP			0x1
#define GENPROC_FLAG_OVERFLOW			0x2
#define GENPROC_FLAG_CORRECTED			0x3

/*
 * IA32/X64 Processor Error Section (Table 255)
 */
typedef struct cper_ia32x64_proc_error_section {
	u64 validation;
	u64 apicid;
	u64 cpuid[6];
	/* PROC_ERR_INFO_NUM x 64-byte structures */
	/* PROC_CONTEXT_INFO_NUM x context structures */
} __packed cper_ia32x64_proc_error_section_t;

/* IA32/X64 Processor Error, Validation Bits field (Table 255) */
#define I32X64SEC_VALID_LAPIC			BIT(0)
#define I32X64SEC_VALID_CPUID			BIT(1)
#define I32X64SEC_VALID_ERRNUM_SH		2
#define I32X64SEC_VALID_ERRNUM_MAX		0x3f
#define I32X64SEC_VALID_ERRNUM_MASK		\
					(I32X64SEC_VALID_ERRNUM_MAX \
					<< I32X64SEC_VALID_ERRNUM_SH)
#define I32X64SEC_VALID_CTXNUM_SH		8
#define I32X64SEC_VALID_CTXNUM_MAX		0x3f
#define I32X64SEC_VALID_CTXNUM_MASK		\
					(I32X64SEC_VALID_CTXNUM_MAX \
					<< I32X64SEC_VALID_CTXNUM_SH)

/* IA32/X64 Processor Error Information Structure (Table 256) */
typedef struct cper_ia32x64_proc_error_info {
	guid_t type; /* cache, tlb, bus, micro-architecture specific */
	u64 validation;
	u64 check_info;
	u64 target_id;
	u64 requestor_id;
	u64 responder_id;
	u64 instruction_ip;
} cper_ia32x64_proc_error_info_t;

/* IA32/X64 Processor Error Information Structs, Err Struct Types (Table 256) */
#define X86_PROCESSOR_CACHE_CHK_ERROR_GUID		\
		GUID_INIT(0xa55701f5, 0xe3ef, 0x43de,	\
		0xac, 0x72, 0x24, 0x9b, 0x57, 0x3f, 0xad, 0x2c)
#define X86_PROCESSOR_TLB_CHK_ERROR_GUID		\
		GUID_INIT(0xfc06b535, 0x5e1f, 0x4562,	\
		0x9f, 0x25, 0x0a, 0x3b, 0x9a, 0xdb, 0x63, 0xc3)
#define X86_PROCESSOR_BUS_CHK_ERROR_GUID		\
		GUID_INIT(0x1cf3f8b3, 0xc5b1, 0x49a2,	\
		0xaa, 0x59, 0x5e, 0xef, 0x92, 0xff, 0xa6, 0x3c)
#define X86_PROCESSOR_MS_CHK_ERROR_GUID			\
		GUID_INIT(0x48ab7f57, 0xdc34, 0x4f6c,	\
		0xa7, 0xd3, 0xb0, 0xb5, 0xb0, 0xa7, 0x43, 0x14)

enum cper_x86_check_type {
	X86_PROCESSOR_CACHE_CHK,	/* X86_PROCESSOR_CACHE_CHK_ERROR_GUID */
	X86_PROCESSOR_TLB_CHK,		/* X86_PROCESSOR_TLB_CHK_ERROR_GUID */
	X86_PROCESSOR_BUS_CHK,		/* X86_PROCESSOR_BUS_CHK_ERROR_GUID */
	X86_PROCESSOR_MS_CHK		/* X86_PROCESSOR_MS_CHK_ERROR_GUID */
};
#define X86_PROCESSOR_CHK_MAX X86_PROCESSOR_MS_CHK

/* IA32/X64 Processor Error Information Structure, Validation Bits (Tbl 256) */
#define I32X64ERRINFO_VALID_CHECK		BIT(0)
#define I32X64ERRINFO_VALID_TGT_ADDR		BIT(1)
#define I32X64ERRINFO_VALID_RQST_ID		BIT(2)
#define I32X64ERRINFO_VALID_RSPD_ID		BIT(3)
#define I32X64ERRINFO_VALID_IPPTR		BIT(4)

/* IA32/X64 Proc. Error Info: Cache/TLB/Check defs (Tables 257, 258, 259) */
#define X86_PROC_CHK_XACT_TYPE_VALID		BIT(0)  /* CACHE|TLB|BUS */
#define X86_PROC_CHK_OPERATION_VALID		BIT(1)  /* CACHE|TLB|BUS */
#define X86_PROC_CHK_LEVEL_VALID		BIT(2)  /* CACHE|TLB|BUS */
#define X86_PROC_CHK_CONTEXT_CORPT_VALID	BIT(3)  /* CACHE|TLB|BUS */
#define X86_PROC_CHK_UNCORRECTED_VALID		BIT(4)  /* CACHE|TLB|BUS */
#define X86_PROC_CHK_PRECISE_IP_VALID		BIT(5)  /* CACHE|TLB|BUS */
#define X86_PROC_CHK_RESTARTABLE_VALID		BIT(6)  /* CACHE|TLB|BUS */
#define X86_PROC_CHK_OVERFLOW_VALID		BIT(7)  /* CACHE|TLB|BUS */
#define X86_PROC_CHK_PART_TYPE_VALID		BIT(8)  /*      |   |BUS */
#define X86_PROC_CHK_TIMEOUT_VALID		BIT(9)  /*      |   |BUS */
#define X86_PROC_CHK_ADDR_SPACE_VALID		BIT(10) /*      |   |BUS */
#define X86_PROC_CHK_XACT_SH			16	   /* CA|TLB|BUS */
#define X86_PROC_CHK_XACT_MASK			(0x3 << X86_PROC_CHK_XACT_SH)
#define X86_PROC_CHK_XACT_INSTRUCTION		(0 << X86_PROC_CHK_XACT_SH)
#define X86_PROC_CHK_XACT_DATA			(1 << X86_PROC_CHK_XACT_SH)
#define X86_PROC_CHK_XACT_GENERIC		(2 << X86_PROC_CHK_XACT_SH)
#define X86_PROC_CHK_OPER_SH			18         /* CA|TLB|BUS */
#define X86_PROC_CHK_OPER_MASK			(0xf << X86_PROC_CHK_OPER_SH)
#define X86_PROC_CHK_OPER_GENERIC		(0 << X86_PROC_CHK_OPER_SH)
#define X86_PROC_CHK_OPER_GENREAD		(1 << X86_PROC_CHK_OPER_SH)
#define X86_PROC_CHK_OPER_GENWRITE		(2 << X86_PROC_CHK_OPER_SH)
#define X86_PROC_CHK_OPER_DATAREAD		(3 << X86_PROC_CHK_OPER_SH)
#define X86_PROC_CHK_OPER_DATAWRITE		(4 << X86_PROC_CHK_OPER_SH)
#define X86_PROC_CHK_OPER_FETCH			(5 << X86_PROC_CHK_OPER_SH)
#define X86_PROC_CHK_OPER_PREFETCH		(6 << X86_PROC_CHK_OPER_SH)
							   /* CA|   |    */
#define X86_PROC_CHK_OPER_EVICTION		(7 << X86_PROC_CHK_OPER_SH)
#define X86_PROC_CHK_OPER_SNOOP			(8 << X86_PROC_CHK_OPER_SH)
#define X86_PROC_CHK_LEVEL_SH			22	   /* CA|TLB|BUS */
#define X86_PROC_CHK_LEVEL_MASK			(0x7 << X86_PROC_CHK_LEVEL_SH)
#define X86_PROC_CHK_LEVEL_1			(1 << X86_PROC_CHK_LEVEL_SH)
#define X86_PROC_CHK_LEVEL_2			(2 << X86_PROC_CHK_LEVEL_SH)
#define X86_PROC_CHK_LEVEL_3			(3 << X86_PROC_CHK_LEVEL_SH)
#define X86_PROC_CHK_CTX_CORRUPT		(1 << 25)  /* CA|TLB|BUS */
#define X86_PROC_CHK_UNCORRECTED		(1 << 26)  /* CA|TLB|BUS */
#define X86_PROC_CHK_PRECISE_IP			(1 << 27)  /* CA|TLB|BUS */
#define X86_PROC_CHK_RESTARTABLE_IP		(1 << 28)  /* CA|TLB|BUS */
#define X86_PROC_CHK_OVERFLOW			(1 << 29)  /* CA|TLB|BUS */
#define X86_PROC_CHK_PARTIC_SH			30         /*   |   |BUS */
#define X86_PROC_CHK_PARTIC_MASK		(3 << X86_PROC_CHK_PARTIC_SH)
#define X86_PROC_CHK_ORIGINATED			(0 << X86_PROC_CHK_PARTIC_SH)
#define X86_PROC_CHK_RESPONDED			(1 << X86_PROC_CHK_PARTIC_SH)
#define X86_PROC_CHK_OBSERVED			(2 << X86_PROC_CHK_PARTIC_SH)
#define X86_PROC_CHK_TIMEOUT			0x100000000 /* BIT(32) */
#define X86_PROC_CHK_SPACE_SH			33         /*   |   |BUS */
#define X86_PROC_CHK_SPACE_MASK			(0x3 << X86_PROC_CHK_SPACE_SH)
#define X86_PROC_CHK_SPACE_MEM			(0 << X86_PROC_CHK_SPACE_SH)
#define X86_PROC_CHK_SPACE_IO			(2 << X86_PROC_CHK_SPACE_SH)
#define X86_PROC_CHK_SPACE_OTHER		(3 << X86_PROC_CHK_SPACE_SH)
/* MS check defines & aligns (Table 260 */
#define X86_PROC_MS_ERROR_TYPE_VALID		BIT(0)
#define X86_PROC_MS_CONTEXT_CORPT_VALID		BIT(1)
#define X86_PROC_MS_UNCORRECTED_VALID		BIT(2)
#define X86_PROC_MS_PRECISE_IP_VALID		BIT(3)
#define X86_PROC_MS_RESTARTABLE_IP_VALID	BIT(4)
#define X86_PROC_MS_OVERFLOW_VALID		BIT(5)
#define X86_PROC_MS_CHK_XACT_SH			16
#define X86_PROC_MS_CHK_XACT_MASK		(0x7 << X86_PROC_MS_CHK_XACT_SH)
#define X86_PROC_MS_CHK_XACT_TYPE_NOERR		(0 << X86_PROC_MS_CHK_XACT_SH)
#define X86_PROC_MS_CHK_XACT_TYPE_UNCL		(1 << X86_PROC_MS_CHK_XACT_SH)
#define X86_PROC_MS_CHK_XACT_TYPE_UCODE_ROM	(2 << X86_PROC_MS_CHK_XACT_SH)
#define X86_PROC_MS_CHK_XACT_TYPE_EXT		(3 << X86_PROC_MS_CHK_XACT_SH)
#define X86_PROC_MS_CHK_XACT_TYPE_FRC		(4 << X86_PROC_MS_CHK_XACT_SH)
#define X86_PROC_MS_CHK_XACT_TYPE_INT_UNCL	(5 << X86_PROC_MS_CHK_XACT_SH)
#define X86_PROC_MS_CHK_CTX_CORRUPT		(1 << 19)
#define X86_PROC_MS_CHK_UNCORRECTED		(1 << 20)
#define X86_PROC_MS_CHK_PRECISE_IP		(1 << 21)
#define X86_PROC_MS_CHK_RESTARTABLE_IP		(1 << 22)
#define X86_PROC_MS_CHK_OVERFLOW		(1 << 23)

/* IA32/X64 Processor Context Information (Table 261) */
typedef struct cper_ia32x64_context {
	u16 type;
	u16 array_size;
	u32 msr_addr;
	u64 mmap_addr;
	/* N bytes of register array */
} cper_ia32x64_context_t;

/* IA32/X64 Processor Context Information, Types field (Table 261) */
#define CPER_IA32X64_CTX_UNCL			0
#define CPER_IA32X64_CTX_MSR			1
#define CPER_IA32X64_CTX_32BIT_EX		2
#define CPER_IA32X64_CTX_64BIT_EX		3
#define CPER_IA32X64_CTX_FXSAVE			4
#define CPER_IA32X64_CTX_32BIT_DBG		5
#define CPER_IA32X64_CTX_64BIT_DBG		6
#define CPER_IA32X64_CTX_MEMMAPPED		7

/* IA32/X64 Processor Context IA32 Register State (Table 262) */
typedef struct cper_ia32x64_ctx_ia32state {
	u32 eax;
	u32 ebx;
	u32 ecx;
	u32 edx;
	u32 esi;
	u32 edi;
	u32 ebp;
	u32 esp;
	u16 cs;
	u16 ds;
	u16 ss;
	u16 es;
	u16 fs;
	u16 gs;
	u32 eflags;
	u32 eip;
	u32 cr0;
	u32 cr1;
	u32 cr2;
	u32 cr3;
	u32 cr4;
	u64 gdtr;
	u64 idtr;
	u16 ldtr;
	u16 tr;
} cper_ia32x64_ctx_ia32state_t;

/* IA32/X64 Processor Context X64 Register state (Table 263) */
typedef struct cper_ia32x64_ctx_x64state {
	u64 rax;
	u64 rbx;
	u64 rcx;
	u64 rdx;
	u64 rsi;
	u64 rdi;
	u64 rbp;
	u64 rsp;
	u64 r8;
	u64 r9;
	u64 r10;
	u64 r11;
	u64 r12;
	u64 r13;
	u64 r14;
	u64 r15;
	u16 cs;
	u16 ds;
	u16 ss;
	u16 es;
	u16 fs;
	u16 gs;
	u32 reserved;
	u64 rflags;
	u64 eip;
	u64 cr0;
	u64 cr1;
	u64 cr2;
	u64 cr3;
	u64 cr4;
	u64 cr8;
	u8 gdtr[16];
	u8 idtr[16];
	u16 ldtr;
	u16 tr;
} cper_ia32x64_ctx_x64state_t;

/* UEFI Spec 2.10, Appendix N.2.5 Memory Error Types (Table N.31) */
#define CPER_UNDEFINED			0
#define CPER_ERR_SINGLE_BIT_ECC		2
#define CPER_ERR_MULTI_BIT_ECC		3
#define CPER_ERR_MEM_PARITY_ERR		8
#define CPER_ERR_MEM_SCRUB_CE_ERR	13
#define CPER_ERR_MEM_SCRUB_UCE_ERR	14

/* UEFI Spec 2.10, Appendix N.2.5 Memory Error Section (Table N.31) */
struct cper_memory_section {
	u64	valid_bits;
	u64	err_sts;
	u64	phys_addr;
	u64	phys_addr_mask;
	u16	node;
	u16	card;
	u16	module;
	u16	bank;
	u16	device;
	u16	row;
	u16	column;
	u16	bit_position;
	u64	requestor_id;
	u64	responder_id;
	u64	target_id;
	u8	mem_err_type;
	u8	extended;
	u16	rank_number;
	u16	card_handle;
	u16	module_handle;
};

#define FW_ERR_RECORD_ID_CRASHLOG_GUID				\
	GUID_INIT(0x8f87f311, 0xc998, 0x4d9e,			\
		0xa0, 0xc4, 0x60, 0x65, 0x51, 0x8c, 0x4f, 0x6d)

/* Firmware Error Record Reference, UEFI v2.8 sec N.2.10  */
typedef struct cper_fw_err_rec_section {
	u8 record_type;
	u8 revision;
	u8 reserved[6];
	u64 record_id;
	guid_t record_guid;
} cper_fw_err_rec_section_t;

static inline cper_timestamp_t cper_timestamp(int precise)
{
	cper_timestamp_t ts;
#if CONFIG(RTC)
	struct rtc_time time;

	rtc_get(&time);
	ts.sec = bin2bcd(time.sec);
	ts.min = bin2bcd(time.min);
	ts.hour = bin2bcd(time.hour);
	ts.day = bin2bcd(time.mday);
	ts.month = bin2bcd(time.mon);
	ts.year = bin2bcd(time.year % 100);
	ts.century = bin2bcd(time.year / 100);
	ts.precise = precise;
#else
	ts.sec = 0;
	ts.min = 0;
	ts.hour = 0;
	ts.day = 0;
	ts.month = 0;
	ts.year = 0;
	ts.century = 0;
	ts.precise = 0;
#endif
	return ts;
}

/* Calculate the size of an IA32/X64 context by its type.  Some types have a
 * predetermined size, and others are variable size.  All sizes are rounded up
 * to the nearest multiple of 16 bytes (See Processor Context field of
 * Table 255).
 *
 * type is one of:
 *   CPER_IA32X64_CTX_UNCL
 *   CPER_IA32X64_CTX_MSR
 *   CPER_IA32X64_CTX_32BIT_EX
 *   CPER_IA32X64_CTX_64BIT_EX
 *   CPER_IA32X64_CTX_FXSAVE
 *   CPER_IA32X64_CTX_32BIT_DBG
 *   CPER_IA32X64_CTX_64BIT_DBG
 *   CPER_IA32X64_CTX_MEMMAPPED
 * num is the number of items in the context's register array
 */
static inline size_t cper_ia32x64_ctx_sz_bytype(int type, int arr_num)
{
	size_t sz = sizeof(cper_ia32x64_context_t);

	switch (type) {
	case CPER_IA32X64_CTX_32BIT_EX:
		return ALIGN_UP(sz + sizeof(cper_ia32x64_ctx_ia32state_t), 16);
	case CPER_IA32X64_CTX_64BIT_EX:
		return ALIGN_UP(sz + sizeof(cper_ia32x64_ctx_x64state_t), 16);
	case CPER_IA32X64_CTX_UNCL:
	case CPER_IA32X64_CTX_MSR:
	case CPER_IA32X64_CTX_FXSAVE:
	case CPER_IA32X64_CTX_32BIT_DBG:
	case CPER_IA32X64_CTX_64BIT_DBG:
	case CPER_IA32X64_CTX_MEMMAPPED:
	default:
		/* Table 261: "size ... is determined by (Array Size / 8)" */
		return ALIGN_UP(sz + arr_num * 8, 16);
	}
}

static inline size_t cper_ia32x64_check_sz(void)
{
	return sizeof(cper_ia32x64_proc_error_info_t); /* all the same size */
}

/* Return PROC_ERR_INFO_NUM for an IA32/X64 Processor Error Record */
static inline int cper_ia32x64_proc_num_chks(
				cper_ia32x64_proc_error_section_t *x86err)
{
	int mask;
	int shift;

	mask = I32X64SEC_VALID_ERRNUM_MASK;
	shift = I32X64SEC_VALID_ERRNUM_SH;

	return (x86err->validation & mask) >> shift;
}

/* Return PROC_CONTEXT_INFO_NUM for an IA32/X64 Processor Error Record */
static inline int cper_ia32x64_proc_num_ctxs(
				cper_ia32x64_proc_error_section_t *x86err)
{
	int mask;
	int shift;

	mask = I32X64SEC_VALID_CTXNUM_MASK;
	shift = I32X64SEC_VALID_CTXNUM_SH;

	return (x86err->validation & mask) >> shift;
}

/* Do PROC_ERR_INFO_NUM++ of an IA32/X64 error section.  Caller should ensure
 * the max is not being exceeded.
 */
static inline void cper_bump_ia32x64_chk_count(
				cper_ia32x64_proc_error_section_t *x86err)
{
	int count;

	count = cper_ia32x64_proc_num_chks(x86err) + 1;
	x86err->validation &= ~I32X64SEC_VALID_ERRNUM_MASK;
	x86err->validation |= count << I32X64SEC_VALID_ERRNUM_SH;
}

/* Do PROC_CONTEXT_INFO_NUM++ of an IA32/X64 error section.  Caller should
 * ensure the max is not being exceeded.
 */
static inline void cper_bump_ia32x64_ctx_count(
				cper_ia32x64_proc_error_section_t *x86err)
{
	int count;

	count = cper_ia32x64_proc_num_ctxs(x86err) + 1;
	x86err->validation &= ~I32X64SEC_VALID_CTXNUM_MASK;
	x86err->validation |= count << I32X64SEC_VALID_CTXNUM_SH;
}

#endif /* _CPER_H_ */
