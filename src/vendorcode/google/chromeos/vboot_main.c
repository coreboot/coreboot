#include <2api.h>
#include <2struct.h>
#include <antirollback.h>
#include <arch/exception.h>
#include <arch/stages.h>
#include <cbfs.h>
#include <console/console.h>
#include <console/vtxprintf.h>
#include <reset.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <string.h>

#include "chromeos.h"
#include "fmap.h"

#define VBDEBUG(format, args...) \
	printk(BIOS_INFO, "%s():%d: " format,  __func__, __LINE__, ## args)
#define TODO_BLOCK_SIZE 8192
#define MAX_PARSED_FW_COMPONENTS 5
#define ROMSTAGE_INDEX 2

struct component_entry {
	uint32_t offset;
	uint32_t size;
} __attribute__((packed));

struct components {
	uint32_t num_components;
	struct component_entry entries[MAX_PARSED_FW_COMPONENTS];
} __attribute__((packed));

struct vboot_region {
	uintptr_t offset_addr;
	int32_t size;
};

static void locate_region(const char *name, struct vboot_region *region)
{
	region->size = find_fmap_entry(name, (void **)&region->offset_addr);
	VBDEBUG("Located %s @%x\n", name, region->offset_addr);
}

static int is_slot_a(struct vb2_context *ctx)
{
	return !(ctx->flags & VB2_CONTEXT_FW_SLOT_B);
}

static int in_ro(void)
{
	/* TODO: Implement */
	return 1;
}

/* exports */

void vb2ex_printf(const char *func, const char *fmt, ...)
{
	va_list args;

	printk(BIOS_INFO, "VB2:%s() ", func);
	va_start(args, fmt);
	printk(BIOS_INFO, fmt, args);
	va_end(args);

	return;
}

int vb2ex_tpm_clear_owner(struct vb2_context *ctx)
{
	uint32_t rv;
	VBDEBUG("Clearing TPM owner\n");
	rv = tpm_clear_and_reenable();
	if (rv)
		return VB2_ERROR_EX_TPM_CLEAR_OWNER;
	return VB2_SUCCESS;
}

int vb2ex_read_resource(struct vb2_context *ctx,
			enum vb2_resource_index index,
			uint32_t offset,
			void *buf,
			uint32_t size)
{
	struct vboot_region region;

	switch (index) {
	case VB2_RES_GBB:
		locate_region("GBB", &region);
		break;
	case VB2_RES_FW_VBLOCK:
		if (is_slot_a(ctx))
			locate_region("VBLOCK_A", &region);
		else
			locate_region("VBLOCK_B", &region);
		break;
	default:
		return VB2_ERROR_EX_READ_RESOURCE_INDEX;
	}

	if (offset + size > region.size)
		return VB2_ERROR_EX_READ_RESOURCE_SIZE;

	if (vboot_get_region(region.offset_addr + offset, size, buf) == NULL)
		return VB2_ERROR_UNKNOWN;

	return VB2_SUCCESS;
}

static void reboot(void)
{
	cpu_reset();
}

static void recovery(void)
{
	void *entry;

	if (!in_ro())
		reboot();

	entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA, "fallback/romstage");
	if (entry != (void *)-1)
		stage_exit(entry);

	for(;;);
}

static int hash_body(struct vb2_context *ctx, struct vboot_region *fw_main)
{
	uint32_t expected_size;
	uint8_t block[TODO_BLOCK_SIZE];
	size_t block_size = sizeof(block);
	uintptr_t offset;
	int rv;

	expected_size = fw_main->size;
	offset= fw_main->offset_addr;

	/* Start the body hash */
	rv = vb2api_init_hash(ctx, VB2_HASH_TAG_FW_BODY, &expected_size);
	if (rv) {
		return rv;
	}

	/* Extend over the body */
	while (expected_size) {
		void *b;
		if (block_size > expected_size)
			block_size = expected_size;

		b = vboot_get_region(offset, block_size, block);
		if (b == NULL)
			return VB2_ERROR_UNKNOWN;
		rv = vb2api_extend_hash(ctx, b, block_size);
		if (rv)
			return rv;

		expected_size -= block_size;
		offset+= block_size;
	}

	/* Check the result */
	rv = vb2api_check_hash(ctx);
	if (rv) {
		return rv;
	}

	return VB2_SUCCESS;
}

static int locate_fw_components(struct vb2_context *ctx,
                                 struct vboot_region *fw_main,
                                 struct components *fw_info)
{
	if (is_slot_a(ctx))
		locate_region("FW_MAIN_A", fw_main);
	else
		locate_region("FW_MAIN_B", fw_main);
	if (fw_main->size < 0)
		return 1;

	if (vboot_get_region(fw_main->offset_addr,
	                     sizeof(*fw_info), fw_info) == NULL)
		return 1;
	return 0;
}

static struct cbfs_stage *load_stage(struct vb2_context *ctx,
                                     int stage_index,
                                     struct vboot_region *fw_main,
                                     struct components *fw_info)
{
	struct cbfs_stage *stage;
	uint32_t fc_addr;
	uint32_t fc_size;

	/* Check for invalid address. */
	fc_addr = fw_main->offset_addr + fw_info->entries[stage_index].offset;
	fc_size = fw_info->entries[stage_index].size;
	if (fc_addr == 0 || fc_size == 0) {
		VBDEBUG("romstage address invalid.\n");
		return NULL;
	}

	/* Loading to cbfs cache. This stage data must be retained until it's
	 * decompressed. */
	stage = vboot_get_region(fc_addr, fc_size, NULL);

	if (stage == NULL) {
		VBDEBUG("Unable to load a stage.\n");
		return NULL;
	}

	return stage;
}

static void enter_stage(struct cbfs_stage *stage)
{
	/* Stages rely the below clearing so that the bss is initialized. */
	memset((void *) (uintptr_t)stage->load, 0, stage->memlen);

	if (cbfs_decompress(stage->compression,
	                    (unsigned char *)stage + sizeof(*stage),
	                    (void *) (uintptr_t) stage->load,
	                    stage->len))
		return;

	VBDEBUG("Jumping to entry @%llx.\n", stage->entry);
	stage_exit((void *)(uintptr_t)stage->entry);
}

enum {
	L2CTLR_ECC_PARITY = 0x1 << 21,
	L2CTLR_TAG_RAM_LATENCY_MASK = 0x7 << 6,
	L2CTLR_TAG_RAM_LATENCY_CYCLES_3 = 2 << 6,
	L2CTLR_DATA_RAM_LATENCY_MASK = 0x7 << 0,
	L2CTLR_DATA_RAM_LATENCY_CYCLES_3  = 2 << 0
};

enum {
	L2ACTLR_FORCE_L2_LOGIC_CLOCK_ENABLE_ACTIVE = 0x1 << 27,
	L2ACTLR_ENABLE_HAZARD_DETECT_TIMEOUT = 0x1 << 7,
	L2ACTLR_DISABLE_CLEAN_EVICT_PUSH_EXTERNAL = 0x1 << 3
};

/* Configures L2 Control Register to use 3 cycles for DATA/TAG RAM latency. */
static void configure_l2ctlr(void)
{
   uint32_t val;

   val = read_l2ctlr();
   val &= ~(L2CTLR_DATA_RAM_LATENCY_MASK | L2CTLR_TAG_RAM_LATENCY_MASK);
   val |= (L2CTLR_DATA_RAM_LATENCY_CYCLES_3 | L2CTLR_TAG_RAM_LATENCY_CYCLES_3 |
	   L2CTLR_ECC_PARITY);
   write_l2ctlr(val);
}

/* Configures L2 Auxiliary Control Register for Cortex A15. */
static void configure_l2actlr(void)
{
   uint32_t val;

   val = read_l2actlr();
   val |= (L2ACTLR_DISABLE_CLEAN_EVICT_PUSH_EXTERNAL |
	   L2ACTLR_ENABLE_HAZARD_DETECT_TIMEOUT |
	   L2ACTLR_FORCE_L2_LOGIC_CLOCK_ENABLE_ACTIVE);
   write_l2actlr(val);
}

static void enable_cache(void)
{
	mmu_init();
	mmu_config_range(0, CONFIG_SYS_SDRAM_BASE >> 20, DCACHE_OFF);
	mmu_config_range(0x40000000 >> 20, 2, DCACHE_WRITEBACK);
	mmu_disable_range(0, 1);
	VBDEBUG("Enabling cache\n");
	dcache_mmu_enable();
}

/**
 * Save non-volatile and/or secure data if needed.
 */
static void save_if_needed(struct vb2_context *ctx)
{
	if (ctx->flags & VB2_CONTEXT_NVDATA_CHANGED) {
		VBDEBUG("Saving nvdata\n");
		save_vbnv(ctx->nvdata);
		ctx->flags &= ~VB2_CONTEXT_NVDATA_CHANGED;
	}
	if (ctx->flags & VB2_CONTEXT_SECDATA_CHANGED) {
		VBDEBUG("Saving secdata\n");
		antirollback_write_space_firmware(ctx);
		ctx->flags &= ~VB2_CONTEXT_SECDATA_CHANGED;
	}
}

/**
 * Load and verify the next stage from RW image and jump to it
 *
 * If validation fails, it exits to romstage for recovery or reboots.
 *
 * TODO: Avoid loading a stage twice (once in hash_body & again in load_stage).
 * when per-stage verification is ready.
 */
void __attribute__((noinline)) select_firmware(void)
{
	struct vb2_context ctx;
	uint8_t *workbuf = (uint8_t *)CONFIG_VBOOT_WORK_BUFFER_ADDRESS;
	struct vboot_region fw_main;
	struct components fw_info;
	struct cbfs_stage *stage;
	int rv;

	/* Do minimum to enable cache and run vboot at full speed */
	configure_l2ctlr();
	configure_l2actlr();
	console_init();
	exception_init();
	enable_cache();

	/* Set up context */
	memset(&ctx, 0, sizeof(ctx));
	ctx.workbuf = workbuf;
	ctx.workbuf_size = CONFIG_VBOOT_WORK_BUFFER_SIZE;
	memset(ctx.workbuf, 0, ctx.workbuf_size);

	/* Read nvdata from a non-volatile storage */
	read_vbnv(ctx.nvdata);

	/* Read secdata from TPM. Initialize TPM if secdata not found. We don't
	 * check the return value here because vb2api_fw_phase1 will catch
	 * invalid secdata and tell us what to do (=reboot). */
	antirollback_read_space_firmware(&ctx);

	if (get_developer_mode_switch())
		ctx.flags |= VB2_CONTEXT_FORCE_DEVELOPER_MODE;
	if (get_recovery_mode_switch()) {
		clear_recovery_mode_switch();
		ctx.flags |= VB2_CONTEXT_FORCE_RECOVERY_MODE;
	}

	/* Do early init */
	VBDEBUG("Phase 1\n");
	rv = vb2api_fw_phase1(&ctx);
	if (rv) {
		VBDEBUG("Recovery requested (%x)\n", rv);
		/* If we need recovery mode, leave firmware selection now */
		save_if_needed(&ctx);
		recovery();
	}

	/* Determine which firmware slot to boot */
	VBDEBUG("Phase 2\n");
	rv = vb2api_fw_phase2(&ctx);
	if (rv) {
		VBDEBUG("Reboot requested (%x)\n", rv);
		save_if_needed(&ctx);
		reboot();
	}

	/* Try that slot */
	VBDEBUG("Phase 3\n");
	rv = vb2api_fw_phase3(&ctx);
	if (rv) {
		VBDEBUG("Reboot requested (%x)\n", rv);
		save_if_needed(&ctx);
		reboot();
	}

	VBDEBUG("Phase 4\n");
	rv = locate_fw_components(&ctx, &fw_main, &fw_info);
	if (rv) {
		VBDEBUG("Failed to locate firmware components\n");
		reboot();
	}
	rv = hash_body(&ctx, &fw_main);
	stage = load_stage(&ctx, ROMSTAGE_INDEX, &fw_main, &fw_info);
	if (stage == NULL) {
		VBDEBUG("Failed to load stage\n");
		reboot();
	}
	save_if_needed(&ctx);
	if (rv) {
		VBDEBUG("Reboot requested (%x)\n", rv);
		reboot();
	}

	/* TODO: Do we need to lock secdata? */
	VBDEBUG("Locking TPM\n");

	/* Load next stage and jump to it */
	VBDEBUG("Jumping to rw-romstage @%llx\n", stage->entry);
	enter_stage(stage);

	/* Shouldn't reach here */
	VBDEBUG("Halting\n");
	for(;;);
}
