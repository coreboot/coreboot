/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <console/streams.h>
#include <fsp/util.h>
#include <timestamp.h>

/* Locate the FSP binary in the coreboot filesystem */
FSP_INFO_HEADER *find_fsp(uintptr_t fsp_base_address)
{
	union {
		EFI_FFS_FILE_HEADER *ffh;
		FSP_INFO_HEADER *fih;
		EFI_FIRMWARE_VOLUME_EXT_HEADER *fveh;
		EFI_FIRMWARE_VOLUME_HEADER *fvh;
		EFI_RAW_SECTION *rs;
		u32 u32;
	} fsp_ptr;

	u64 *image_id;

	/* Get the FSP binary base address in CBFS */
	fsp_ptr.u32 = fsp_base_address;

	/* Check the FV signature, _FVH */
	if (fsp_ptr.fvh->Signature != 0x4856465F)
		return (FSP_INFO_HEADER *)ERROR_NO_FV_SIG;

	/* Locate the file header which follows the FV header. */
	fsp_ptr.u32 += fsp_ptr.fvh->ExtHeaderOffset;
	fsp_ptr.u32 += fsp_ptr.fveh->ExtHeaderSize;
	fsp_ptr.u32 = ALIGN_UP(fsp_ptr.u32, 8);

	/* Check the FFS GUID */
	if ((((u32 *)&fsp_ptr.ffh->Name)[0] != 0x912740BE)
		|| (((u32 *)&fsp_ptr.ffh->Name)[1] != 0x47342284)
		|| (((u32 *)&fsp_ptr.ffh->Name)[2] != 0xB08471B9)
		|| (((u32 *)&fsp_ptr.ffh->Name)[3] != 0x0C3F3527)) {
		return (FSP_INFO_HEADER *)ERROR_NO_FFS_GUID;
	}

	/* Locate the Raw Section Header */
	fsp_ptr.u32 += sizeof(EFI_FFS_FILE_HEADER);

	if (fsp_ptr.rs->Type != EFI_SECTION_RAW)
		return (FSP_INFO_HEADER *)ERROR_NO_INFO_HEADER;

	/* Locate the FSP INFO Header which follows the Raw Header. */
	fsp_ptr.u32 += sizeof(EFI_RAW_SECTION);

	/* Verify that the FSP base address.*/
	if (fsp_ptr.fih->ImageBase != fsp_base_address)
		return (FSP_INFO_HEADER *)ERROR_IMAGEBASE_MISMATCH;

	/* Verify the FSP Signature */
	if (fsp_ptr.fih->Signature != FSP_SIG)
		return (FSP_INFO_HEADER *)ERROR_INFO_HEAD_SIG_MISMATCH;

	/* Verify the FSP ID */
	image_id = (u64 *)&fsp_ptr.fih->ImageId[0];
	if (*image_id != FSP_IMAGE_ID)
		return (FSP_INFO_HEADER *)ERROR_FSP_SIG_MISMATCH;

	/* Verify the FSP Revision */
	if (fsp_ptr.fih->ImageRevision > FSP_IMAGE_REV)
		return (FSP_INFO_HEADER *)ERROR_FSP_REV_MISMATCH;

	return fsp_ptr.fih;
}

void print_fsp_info(FSP_INFO_HEADER *fsp_header)
{
	u8 *fsp_base;

	fsp_base = (u8 *)fsp_header->ImageBase;
	printk(BIOS_SPEW, "FSP_INFO_HEADER: %p\n", fsp_header);
	printk(BIOS_INFO, "FSP Signature: %c%c%c%c%c%c%c%c\n",
			fsp_header->ImageId[0], fsp_header->ImageId[1],
			fsp_header->ImageId[2], fsp_header->ImageId[3],
			fsp_header->ImageId[4], fsp_header->ImageId[5],
			fsp_header->ImageId[6], fsp_header->ImageId[7]);
	printk(BIOS_INFO, "FSP Header Version: %d\n",
			fsp_header->HeaderRevision);
	printk(BIOS_INFO, "FSP Revision: %d.%d.%d.%d\n",
			(u8)((fsp_header->ImageRevision >> 24) & 0xff),
			(u8)((fsp_header->ImageRevision >> 16) & 0xff),
			(u8)((fsp_header->ImageRevision >> 8) & 0xff),
			(u8)(fsp_header->ImageRevision  & 0xff));
#if CONFIG(DISPLAY_FSP_ENTRY_POINTS)
	printk(BIOS_SPEW, "FSP Entry Points:\n");
	printk(BIOS_SPEW, "    %p: Image Base\n", fsp_base);
	printk(BIOS_SPEW, "    %p: TempRamInit\n",
		&fsp_base[fsp_header->TempRamInitEntryOffset]);
	printk(BIOS_SPEW, "    %p: FspInit\n",
		&fsp_base[fsp_header->FspInitEntryOffset]);
	if (fsp_header->HeaderRevision >= FSP_HEADER_REVISION_2) {
		printk(BIOS_SPEW, "    %p: MemoryInit\n",
			&fsp_base[fsp_header->FspMemoryInitEntryOffset]);
		printk(BIOS_SPEW, "    %p: TempRamExit\n",
			&fsp_base[fsp_header->TempRamExitEntryOffset]);
		printk(BIOS_SPEW, "    %p: SiliconInit\n",
			&fsp_base[fsp_header->FspSiliconInitEntryOffset]);
	}
	printk(BIOS_SPEW, "    %p: NotifyPhase\n",
		&fsp_base[fsp_header->NotifyPhaseEntryOffset]);
	printk(BIOS_SPEW, "    %p: Image End\n",
			&fsp_base[fsp_header->ImageSize]);
#endif
}

void fsp_notify(u32 phase)
{
	FSP_NOTIFY_PHASE notify_phase_proc;
	NOTIFY_PHASE_PARAMS notify_phase_params;
	EFI_STATUS status;
	FSP_INFO_HEADER *fsp_header_ptr;

	fsp_header_ptr = fsp_get_fih();
	if (fsp_header_ptr == NULL) {
		fsp_header_ptr = (void *)find_fsp(CONFIG_FSP_LOC);
		if ((u32)fsp_header_ptr < 0xff) {
			/* output something in case there is no serial */
			post_code(0x4F);
			die("Can't find the FSP!\n");
		}
	}

	/* call FSP PEI to Notify PostPciEnumeration */
	notify_phase_proc = (FSP_NOTIFY_PHASE)(fsp_header_ptr->ImageBase +
		fsp_header_ptr->NotifyPhaseEntryOffset);
	notify_phase_params.Phase = phase;

	if (phase == EnumInitPhaseReadyToBoot) {
		timestamp_add_now(TS_FSP_FINALIZE_START);
		post_code(POST_FSP_NOTIFY_BEFORE_FINALIZE);
	} else {
		timestamp_add_now(TS_FSP_ENUMERATE_START);
		post_code(POST_FSP_NOTIFY_BEFORE_ENUMERATE);
	}

	status = notify_phase_proc(&notify_phase_params);

	timestamp_add_now(phase == EnumInitPhaseReadyToBoot ?
		TS_FSP_FINALIZE_END : TS_FSP_ENUMERATE_END);

	if (status != 0)
		printk(BIOS_ERR, "FSP API NotifyPhase failed for phase 0x%x with status: 0x%x\n",
			phase, status);
}

static void fsp_notify_boot_state_callback(void *arg)
{
	u32 phase = (u32)arg;

	printk(BIOS_SPEW, "Calling FspNotify(0x%08x)\n", phase);
	fsp_notify(phase);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_RESOURCES, BS_ON_EXIT,
	fsp_notify_boot_state_callback,
	(void *)EnumInitPhaseAfterPciEnumeration);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT,
	fsp_notify_boot_state_callback,
	(void *)EnumInitPhaseReadyToBoot);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY,
	fsp_notify_boot_state_callback,
	(void *)EnumInitPhaseReadyToBoot);

struct fsp_runtime {
	uint32_t fih;
	uint32_t hob_list;
} __packed;

void fsp_set_runtime(FSP_INFO_HEADER *fih, void *hob_list)
{
	struct fsp_runtime *fspr;

	fspr = cbmem_add(CBMEM_ID_FSP_RUNTIME, sizeof(*fspr));

	if (fspr == NULL)
		die("Can't save FSP runtime information.\n");

	fspr->fih = (uintptr_t)fih;
	fspr->hob_list = (uintptr_t)hob_list;
}

FSP_INFO_HEADER *fsp_get_fih(void)
{
	struct fsp_runtime *fspr;

	fspr = cbmem_find(CBMEM_ID_FSP_RUNTIME);

	if (fspr == NULL)
		return NULL;

	return (void *)(uintptr_t)fspr->fih;
}

void *fsp_get_hob_list(void)
{
	struct fsp_runtime *fspr;

	fspr = cbmem_find(CBMEM_ID_FSP_RUNTIME);

	if (fspr == NULL)
		return NULL;

	return (void *)(uintptr_t)fspr->hob_list;
}

void fsp_update_fih(FSP_INFO_HEADER *fih)
{
	struct fsp_runtime *fspr;

	fspr = cbmem_find(CBMEM_ID_FSP_RUNTIME);

	if (fspr == NULL)
		die("Can't update FSP runtime information.\n");

	fspr->fih = (uintptr_t)fih;
}

void fsp_display_upd_value(const char *name, uint32_t size, uint64_t old,
	uint64_t new)
{
	if (old == new) {
		switch (size) {
		case 1:
			printk(BIOS_SPEW, "  0x%02llx: %s\n", new, name);
			break;

		case 2:
			printk(BIOS_SPEW, "  0x%04llx: %s\n", new, name);
			break;

		case 4:
			printk(BIOS_SPEW, "  0x%08llx: %s\n", new, name);
			break;

		case 8:
			printk(BIOS_SPEW, "  0x%016llx: %s\n", new, name);
			break;
		}
	} else {
		switch (size) {
		case 1:
			printk(BIOS_SPEW, "  0x%02llx --> 0x%02llx: %s\n", old,
				new, name);
			break;

		case 2:
			printk(BIOS_SPEW, "  0x%04llx --> 0x%04llx: %s\n", old,
				new, name);
			break;

		case 4:
			printk(BIOS_SPEW, "  0x%08llx --> 0x%08llx: %s\n", old,
				new, name);
			break;

		case 8:
			printk(BIOS_SPEW, "  0x%016llx --> 0x%016llx: %s\n",
				old, new, name);
			break;
		}
	}
}
