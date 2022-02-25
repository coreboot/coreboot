/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_X86_SMM_H
#define CPU_X86_SMM_H

#include <arch/cpu.h>
#include <commonlib/region.h>
#include <types.h>

#define SMM_DEFAULT_BASE 0x30000
#define SMM_DEFAULT_SIZE 0x10000

/* used only by C programs so far */
#define SMM_BASE 0xa0000

#define SMM_ENTRY_OFFSET 0x8000
#define SMM_SAVE_STATE_BEGIN(x) (SMM_ENTRY_OFFSET + (x))

#define APM_CNT		0xb2
#define APM_CNT_NOOP_SMI	0x00
#define APM_CNT_ACPI_DISABLE	0x1e
#define APM_CNT_ACPI_ENABLE	0xe1
#define APM_CNT_ROUTE_ALL_XHCI	0xca
#define APM_CNT_FINALIZE	0xcb
#define APM_CNT_LEGACY		0xcc
#define APM_CNT_MBI_UPDATE	0xeb
#define APM_CNT_SMMINFO		0xec
#define APM_CNT_SMMSTORE	0xed
#define APM_CNT_ELOG_GSMI	0xef
#define APM_STS		0xb3

/* Send cmd to APM_CNT with HAVE_SMI_HANDLER checking. */
int apm_control(u8 cmd);
u8 apm_get_apmc(void);

void io_trap_handler(int smif);
int southbridge_io_trap_handler(int smif);
int mainboard_io_trap_handler(int smif);

void southbridge_smi_set_eos(void);

void global_smi_enable(void);
void global_smi_enable_no_pwrbtn(void);

void cpu_smi_handler(void);
void northbridge_smi_handler(void);
void southbridge_smi_handler(void);

void mainboard_smi_gpi(u32 gpi_sts);
int  mainboard_smi_apmc(u8 data);
void mainboard_smi_sleep(u8 slp_typ);
void mainboard_smi_finalize(void);

void smm_soc_early_init(void);
void smm_soc_exit(void);

/* This is the SMM handler. */
extern unsigned char _binary_smm_start[];
extern unsigned char _binary_smm_end[];

struct smm_runtime {
	u32 smbase;
	u32 smm_size;
	u32 save_state_size;
	u32 num_cpus;
	u32 gnvs_ptr;
	u32 cbmemc_size;
	void *cbmemc;
	uintptr_t save_state_top[CONFIG_MAX_CPUS];
} __packed;

struct smm_module_params {
	size_t cpu;
	/* A canary value that has been placed at the end of the stack.
	 * If (uintptr_t)canary != *canary then a stack overflow has occurred.
	 */
	const uintptr_t *canary;
};

/* These parameters are used by the SMM stub code. A pointer to the params
 * is also passed to the C-base handler. */
struct smm_stub_params {
	u32 stack_size;
	u32 stack_top;
	u32 c_handler;
	u32 fxsave_area;
	u32 fxsave_area_size;
	/* The apic_id_to_cpu provides a mapping from APIC id to CPU number.
	 * The CPU number is indicated by the index into the array by matching
	 * the default APIC id and value at the index. The stub loader
	 * initializes this array with a 1:1 mapping. If the APIC ids are not
	 * contiguous like the 1:1 mapping it is up to the caller of the stub
	 * loader to adjust this mapping. */
	u16 apic_id_to_cpu[CONFIG_MAX_CPUS];
	/* STM's 32bit entry into SMI handler */
	u32 start32_offset;
} __packed;

/* smm_handler_t is called with arg of smm_module_params pointer. */
typedef asmlinkage void (*smm_handler_t)(void *);

/* SMM Runtime helpers. */
#if ENV_SMM
extern struct global_nvs *gnvs;
#endif

/* Entry point for SMM modules. */
asmlinkage void smm_handler_start(void *params);

/* Retrieve SMM save state for a given CPU. WARNING: This does not take into
 * account CPUs which are configured to not save their state to RAM. */
void *smm_get_save_state(int cpu);

/* Returns true if the region overlaps with the SMM */
bool smm_region_overlaps_handler(const struct region *r);

/* Returns true if the memory pointed to overlaps with SMM reserved memory. */
static inline bool smm_points_to_smram(const void *ptr, const size_t len)
{
	const struct region r = {(uintptr_t)ptr, len};

	return smm_region_overlaps_handler(&r);
}

/* SMM Module Loading API */

/* The smm_loader_params structure provides direction to the SMM loader:
 * - num_cpus - number of concurrent cpus in handler needing stack
 *                           optional for setting up relocation handler.
 * - per_cpu_save_state_size - the SMM save state size per cpu
 * - num_concurrent_save_states - number of concurrent cpus needing save state
 *                                space
 * - handler - optional handler to call. Only used during SMM relocation setup.
 * - runtime - this field is a result only. The SMM runtime location is filled
 *             into this field so the code doing the loading can manipulate the
 *             runtime's assumptions. e.g. updating the APIC id to CPU map to
 *             handle sparse APIC id space.
 */
struct smm_loader_params {
	size_t num_cpus;

	size_t real_cpu_save_state_size;
	size_t per_cpu_save_state_size;
	size_t num_concurrent_save_states;

	smm_handler_t handler;

	struct smm_stub_params *stub_params;
};

/* All of these return 0 on success, < 0 on failure. */
int smm_setup_stack(const uintptr_t perm_smbase, const size_t perm_smram_size,
		    const unsigned int total_cpus, const size_t stack_size);
int smm_setup_relocation_handler(struct smm_loader_params *params);
int smm_load_module(uintptr_t smram_base, size_t smram_size, struct smm_loader_params *params);

u32 smm_get_cpu_smbase(unsigned int cpu_num);

/* Backup and restore default SMM region. */
void *backup_default_smm_area(void);
void restore_default_smm_area(void *smm_save_area);

/*
 * Fills in the arguments for the entire SMM region covered by chipset
 * protections. e.g. TSEG.
 */
void smm_region(uintptr_t *start, size_t *size);

enum {
	/* SMM handler area. */
	SMM_SUBREGION_HANDLER,
	/* SMM cache region. */
	SMM_SUBREGION_CACHE,
	/* Chipset specific area. */
	SMM_SUBREGION_CHIPSET,
	/* Total sub regions supported. */
	SMM_SUBREGION_NUM,
};

/* Fills in the start and size for the requested SMM subregion. Returns
 * 0 on success, < 0 on failure. */
int smm_subregion(int sub, uintptr_t *start, size_t *size);

/* Print the SMM memory layout on console. */
void smm_list_regions(void);

#define SMM_REVISION_OFFSET_FROM_TOP (0x8000 - 0x7efc)
/* Return the SMM save state revision. The revision can be fetched from the smm savestate
   which is always at the same offset downward from the top of the save state. */
uint32_t smm_revision(void);
/* Returns the PM ACPI SMI port. On Intel systems this typically not configurable (APM_CNT, 0xb2).
   On AMD systems it is sometimes configurable. */
uint16_t pm_acpi_smi_cmd_port(void);

#endif /* CPU_X86_SMM_H */
