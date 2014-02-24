/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#include <stdint.h>
#include <stdlib.h>
#include <console/console.h>
#include <boot/coreboot_tables.h>
#include <payload_loader.h>

extern const struct payload_loader_ops vboot_payload_loader;
extern const struct payload_loader_ops cbfs_payload_loader;

static const struct payload_loader_ops *payload_ops[] = {
#if CONFIG_VBOOT_VERIFY_FIRMWARE
	&vboot_payload_loader,
#endif
	&cbfs_payload_loader,
};

static struct payload global_payload = {
	.name = CONFIG_CBFS_PREFIX "/payload",
};

struct payload *payload_load(void)
{
	int i;
	void *entry;
	struct lb_memory *mem;
	const struct payload_loader_ops *ops;
	struct payload *payload = &global_payload;

	for (i = 0; i < ARRAY_SIZE(payload_ops); i++) {
		ops = payload_ops[i];
		if (ops->locate(payload) < 0) {
			printk(BIOS_DEBUG, "%s: could not locate payload.\n",
				ops->name);
			continue;
		}
		printk(BIOS_DEBUG, "%s: located payload @ %p, %zu bytes.\n",
			ops->name, payload->backing_store.data,
			payload->backing_store.size);
		break;
	}

	if (i == ARRAY_SIZE(payload_ops))
		return NULL;

	mem = get_lb_mem();
	entry = selfload(mem, payload->backing_store.data);

	if (entry == NULL)
		return NULL;

	payload->entry = entry;

	return payload;
}

void payload_run(const struct payload *payload)
{
	if (payload == NULL)
		return;

	selfboot(payload->entry);
}
