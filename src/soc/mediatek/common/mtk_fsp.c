/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <console/console.h>
#include <soc/mtk_fsp.h>

#define MAX_PARAM_ENTRIES 32
#define FSP_INTF_SIZE (sizeof(struct mtk_fsp_intf) + \
		       sizeof(struct mtk_fsp_param) * MAX_PARAM_ENTRIES)

static struct mtk_fsp_intf *intf;
static uint8_t fsp_intf_buf[FSP_INTF_SIZE] __aligned(8);

static int vprintf_wrapper(const char *fmt, va_list args)
{
	return vprintk(BIOS_INFO, fmt, args);
}

void mtk_fsp_init(enum fsp_phase phase)
{
	intf = (struct mtk_fsp_intf *)fsp_intf_buf;
	intf->major_version = INTF_MAJOR_VER;
	intf->minor_version = INTF_MINOR_VER;
	intf->header_size = sizeof(struct mtk_fsp_intf);
	intf->entry_size = sizeof(struct mtk_fsp_param);
	intf->num_entries = 0;
	intf->phase = phase;
	intf->do_vprintf = vprintf_wrapper;
}

enum cb_err mtk_fsp_add_param(enum fsp_param_type type, size_t param_size,
			      void *param)
{
	struct mtk_fsp_param *entry;

	if (!intf) {
		printk(BIOS_ERR, "%s: intf is not initialized\n", __func__);
		return CB_ERR;
	}

	if (intf->num_entries == MAX_PARAM_ENTRIES) {
		printk(BIOS_ERR, "%s: run out all entries\n", __func__);
		return CB_ERR;
	}

	entry = &intf->entries[intf->num_entries];

	entry->param_type = type;
	entry->param_size = param_size;
	entry->param = param;
	intf->num_entries++;

	return CB_SUCCESS;
}

static void mtk_fsp_dump_intf(void)
{
	struct mtk_fsp_param *entry;

	if (!intf) {
		printk(BIOS_ERR, "%s: intf is not initialized\n", __func__);
		return;
	}

	printk(BIOS_DEBUG, "%s: major version: %u, minor version: %u\n",
	       __func__, intf->major_version, intf->minor_version);
	printk(BIOS_DEBUG, "%s: FSP phase: %u, status: %d\n",
	       __func__, intf->phase, intf->status);
	printk(BIOS_DEBUG, "%-5s %-10s %-10s %s\n", "Param", "type", "size", "address");
	for (int i = 0; i < intf->num_entries; i++) {
		entry = &intf->entries[i];
		printk(BIOS_DEBUG, "%-5u %-10u %-10u %p\n",
		       i, entry->param_type, entry->param_size, entry->param);
	}
}

static const char *mtk_fsp_file(void)
{
	return CONFIG_CBFS_PREFIX "/mtk_fsp_" ENV_STRING;
}

enum cb_err mtk_fsp_load_and_run(void)
{
	struct prog fsp = PROG_INIT(PROG_REFCODE, mtk_fsp_file());

	if (cbfs_prog_stage_load(&fsp)) {
		printk(BIOS_ERR, "%s: CBFS load program failed\n", __func__);
		return CB_ERR;
	}

	if (!intf) {
		printk(BIOS_ERR, "%s: intf is not initialized\n", __func__);
		return CB_ERR;
	}

	prog_set_arg(&fsp, intf);
	prog_run(&fsp);

	if (intf->status != FSP_STATUS_SUCCESS) {
		mtk_fsp_dump_intf();
		return CB_ERR;
	}

	printk(BIOS_INFO, "%s: run %s at phase %#x done\n",
	       __func__, mtk_fsp_file(), intf->phase);

	return CB_SUCCESS;
}
