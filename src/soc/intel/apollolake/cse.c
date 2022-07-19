/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <bootstate.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <fmap.h>
#include <intelblocks/cse.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/pcr.h>
#include <soc/cse.h>
#include <soc/heci.h>
#include <soc/iomap.h>
#include <soc/pcr_ids.h>
#include <soc/pci_devs.h>
#include <device/pci_ops.h>
#include <stdint.h>

#define MKHI_GROUP_ID_MCA			0x0a
#define READ_FILE				0x02
#define   READ_FILE_FLAG_DEFAULT		(1 << 0)
#define   READ_FILE_FLAG_HASH			(1 << 1)
#define   READ_FILE_FLAG_EMULATED		(1 << 2)
#define   READ_FILE_FLAG_HW			(1 << 3)

#define MCA_MAX_FILE_PATH_SIZE			64

#define FUSE_LOCK_FILE				"/fpf/intel/SocCfgLock"

/* Status values are made in such a way erase is not needed */
static enum fuse_flash_state {
	FUSE_FLASH_FUSED = 0xfc,
	FUSE_FLASH_UNFUSED = 0xfe,
	FUSE_FLASH_UNKNOWN = 0xff,
} g_fuse_state;

#define FPF_STATUS_FMAP				"FPF_STATUS"

/*
 * Read file from CSE internal filesystem.
 * size is maximum length of provided buffer buff, which is updated with actual
 * size of the file read. flags indicate whether real file or fuse is used.
 * Returns 1 on success and 0 otherwise.
 */
static int read_cse_file(const char *path, void *buff, size_t *size,
						size_t offset, uint32_t flags)
{
	size_t reply_size;

	struct mca_command {
		struct mkhi_hdr hdr;
		char file_name[MCA_MAX_FILE_PATH_SIZE];
		uint32_t offset;
		uint32_t data_size;
		uint8_t flags;
	} __packed msg;

	struct mca_response {
		struct mkhi_hdr hdr;
		uint32_t data_size;
		uint8_t buffer[128];
	} __packed rmsg;

	if (sizeof(rmsg.buffer) < *size) {
		printk(BIOS_ERR, "internal buffer is too small\n");
		return 0;
	}

	if (strnlen(path, sizeof(msg.file_name)) >= sizeof(msg.file_name)) {
		printk(BIOS_ERR, "path too big for msg.file_name buffer\n");
		return 0;
	}
	strncpy(msg.file_name, path, sizeof(msg.file_name));
	msg.hdr.group_id = MKHI_GROUP_ID_MCA;
	msg.hdr.command = READ_FILE;
	msg.flags = flags;
	msg.data_size = *size;
	msg.offset = offset;

	reply_size = sizeof(rmsg);

	if (heci_send_receive(&msg, sizeof(msg), &rmsg, &reply_size, HECI_MKHI_ADDR)) {
		printk(BIOS_ERR, "HECI: Failed to read file\n");
		return 0;
	}

	if (rmsg.data_size > *size) {
		printk(BIOS_ERR, "reply is too large\n");
		return 0;
	}

	memcpy(buff, rmsg.buffer, rmsg.data_size);
	*size = rmsg.data_size;

	return 1;
}

static enum fuse_flash_state load_cached_fpf(struct region_device *rdev)
{
	enum fuse_flash_state state;
	uint8_t buff;

	state = FUSE_FLASH_UNKNOWN;

	if (rdev_readat(rdev, &buff, 0, sizeof(buff)) >= 0) {
		state = read8(&buff);
		return state;
	}

	printk(BIOS_WARNING, "failed to load cached FPF value\n");

	return state;
}

static
int save_fpf_state(enum fuse_flash_state state, struct region_device *rdev)
{
	uint8_t buff;

	write8(&buff, (uint8_t) state);
	return rdev_writeat(rdev, &buff, 0, sizeof(buff));
}

static void fpf_blown(void *unused)
{
	uint8_t fuse;
	struct region_device rdev;
	size_t sz = sizeof(fuse);
	bool rdev_valid = false;

	if (fmap_locate_area_as_rdev_rw(FPF_STATUS_FMAP, &rdev) == 0) {
		rdev_valid = true;
		g_fuse_state = load_cached_fpf(&rdev);
		if (g_fuse_state != FUSE_FLASH_UNKNOWN)
			return;
	}

	if (!read_cse_file(FUSE_LOCK_FILE, &fuse, &sz, 0, READ_FILE_FLAG_HW))
		return;

	g_fuse_state = fuse == 1 ? FUSE_FLASH_FUSED : FUSE_FLASH_UNFUSED;

	if (rdev_valid && (save_fpf_state(g_fuse_state, &rdev) < 0))
		printk(BIOS_CRIT, "failed to save FPF state\n");
}

static uint32_t dump_status(int index, int reg_addr)
{
	uint32_t reg;

	reg = me_read_config32(reg_addr);

	printk(BIOS_DEBUG, "CSE FWSTS%d: 0x%08x\n", index, reg);

	return reg;
}

static void dump_cse_state(void)
{
	union cse_fwsts1 fwsts1;
	union cse_fwsts2 fwsts2;
	union cse_fwsts3 fwsts3;
	union cse_fwsts4 fwsts4;
	union cse_fwsts5 fwsts5;
	union cse_fwsts6 fwsts6;

	if (!is_cse_enabled())
		return;

	fwsts1.data = dump_status(1, PCI_ME_HFSTS1);
	fwsts2.data = dump_status(2, PCI_ME_HFSTS2);
	fwsts3.data = dump_status(3, PCI_ME_HFSTS3);
	fwsts4.data = dump_status(4, PCI_ME_HFSTS4);
	fwsts5.data = dump_status(5, PCI_ME_HFSTS5);
	fwsts6.data = dump_status(6, PCI_ME_HFSTS6);

	printk(BIOS_DEBUG, "CSE: Working State          : %u\n",
		fwsts1.fields.working_state);
	printk(BIOS_DEBUG, "CSE: Manufacturing Mode     : %s\n",
		fwsts1.fields.mfg_mode ? "YES" : "NO");
	printk(BIOS_DEBUG, "CSE: Operation State        : %u\n",
		fwsts1.fields.operation_state);
	printk(BIOS_DEBUG, "CSE: FW Init Complete       : %s\n",
		fwsts1.fields.fw_init_complete ? "YES" : "NO");
	printk(BIOS_DEBUG, "CSE: Error Code             : %u\n",
		fwsts1.fields.error_code);
	printk(BIOS_DEBUG, "CSE: Operation Mode         : %u\n",
		fwsts1.fields.operation_mode);
	printk(BIOS_DEBUG, "CSE: IBB Verification Result: %s\n",
		fwsts3.fields.ibb_verif_result ? "PASS" : "FAIL");
	printk(BIOS_DEBUG, "CSE: IBB Verification Done  : %s\n",
		fwsts3.fields.ibb_verif_done ? "YES" : "NO");
	printk(BIOS_DEBUG, "CSE: Actual IBB Size        : %u\n",
		fwsts3.fields.ibb_size);
	printk(BIOS_DEBUG, "CSE: Verified Boot Valid    : %s\n",
		fwsts4.fields.txe_veri_boot_valid ? "PASS" : "FAIL");
	printk(BIOS_DEBUG, "CSE: Verified Boot Test     : %s\n",
		fwsts4.fields.txe_veri_boot_test ? "YES" : "NO");
	printk(BIOS_DEBUG, "CSE: FPF status             : %s\n",
		fwsts6.fields.fpf_commited ? "FUSED" : "UNFUSED");
	printk(BIOS_DEBUG, "CSE: Error Status Code      : %u\n",
		fwsts5.fields.error_status_code);
}

#define PCR_PSFX_T0_SHDW_PCIEN		0x1C
#define PCR_PSFX_T0_SHDW_PCIEN_FUNDIS	(1 << 8)

void soc_disable_heci1_using_pcr(void)
{
	pcr_or32(PID_PSF3, PSF3_BASE_ADDRESS + PCR_PSFX_T0_SHDW_PCIEN,
		 PCR_PSFX_T0_SHDW_PCIEN_FUNDIS);
}

void heci_cse_lockdown(void)
{
	dump_cse_state();

	/*
	 * It is safe to disable HECI1 now since we won't be talking to the ME
	 * anymore.
	 */
	if (CONFIG(DISABLE_HECI1_AT_PRE_BOOT))
		heci1_disable();
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, fpf_blown, NULL);
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT, print_me_fw_version, NULL);
