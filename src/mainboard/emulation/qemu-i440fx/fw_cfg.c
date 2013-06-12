/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <string.h>
#include <swab.h>
#include <console/console.h>
#include <arch/io.h>

#include "fw_cfg.h"
#include "fw_cfg_if.h"

#define FW_CFG_PORT_CTL       0x0510
#define FW_CFG_PORT_DATA      0x0511

static unsigned char fw_cfg_detected = 0xff;
static FWCfgFiles *fw_files;

static int fw_cfg_present(void)
{
	static const char qsig[] = "QEMU";
	unsigned char sig[4];

	if (fw_cfg_detected == 0xff) {
		fw_cfg_get(FW_CFG_SIGNATURE, sig, sizeof(sig));
		fw_cfg_detected = (memcmp(sig, qsig, 4) == 0) ? 1 : 0;
		printk(BIOS_INFO, "QEMU: firmware config interface %s\n",
		       fw_cfg_detected ? "detected" : "not found");
	}
	return fw_cfg_detected;
}

void fw_cfg_get(int entry, void *dst, int dstlen)
{
	outw(entry, FW_CFG_PORT_CTL);
#if 0
	insb(FW_CFG_PORT_DATA, dst, dstlen);
#else
	u8 *d = dst;
	while (dstlen) {
		*d = inb(FW_CFG_PORT_DATA);
		d++; dstlen--;
	}
#endif
}

static void fw_cfg_init_file(void)
{
	u32 i, size, count = 0;

	if (fw_files != NULL)
		return;

	fw_cfg_get(FW_CFG_FILE_DIR, &count, sizeof(count));
	count = swab32(count);
	size = count * sizeof(FWCfgFile) + sizeof(count);
	printk(BIOS_DEBUG, "QEMU: %d files in fw_cfg\n", count);
	fw_files = malloc(size);
	fw_cfg_get(FW_CFG_FILE_DIR, fw_files, size);
	fw_files->count = swab32(fw_files->count);
	for (i = 0; i < count; i++) {
		fw_files->f[i].size   = swab32(fw_files->f[i].size);
		fw_files->f[i].select = swab16(fw_files->f[i].select);
		printk(BIOS_DEBUG, "QEMU:     %s [size=%d]\n",
		       fw_files->f[i].name, fw_files->f[i].size);
	}
}

static FWCfgFile *fw_cfg_find_file(const char *name)
{
	int i;

	fw_cfg_init_file();
	for (i = 0; i < fw_files->count; i++)
		if (strcmp(fw_files->f[i].name, name) == 0)
			return fw_files->f + i;
	return NULL;
}

int fw_cfg_check_file(const char *name)
{
	FWCfgFile *file;

	if (!fw_cfg_present())
		return -1;
	file = fw_cfg_find_file(name);
	if (!file)
		return -1;
	return file->size;
}

void fw_cfg_load_file(const char *name, void *dst)
{
	FWCfgFile *file;

	if (!fw_cfg_present())
		return;
	file = fw_cfg_find_file(name);
	if (!file)
		return;
	fw_cfg_get(file->select, dst, file->size);
}

int fw_cfg_max_cpus(void)
{
	unsigned short max_cpus;

	if (!fw_cfg_present())
		return -1;

	fw_cfg_get(FW_CFG_MAX_CPUS, &max_cpus, sizeof(max_cpus));
	return max_cpus;
}
