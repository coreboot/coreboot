/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

#ifndef __CHROMEOS_H__
#define __CHROMEOS_H__

#include <stddef.h>
#include <stdint.h>
#include <bootmode.h>

/*for mainboard use only*/
void setup_chromeos_gpios(void);

/* functions implemented in vbnv.c: */
int get_recovery_mode_from_vbnv(void);
int vboot_wants_oprom(void);

void read_vbnv(uint8_t *vbnv_copy);
void save_vbnv(const uint8_t *vbnv_copy);

#if CONFIG_CHROMEOS
/* functions implemented in vboot.c */
void init_chromeos(int bootmode);

/* functions implemented in elog.c */
void elog_add_boot_reason(void);
#else
static inline void init_chromeos(int bootmode) { }
static inline void elog_add_boot_reason(void) { return; }
#endif /* CONFIG_CHROMEOS */

struct romstage_handoff;

/* TODO(shawnn): Remove these CONFIGs and define default weak functions
 * that can be overridden in the platform / MB code. */
#if CONFIG_VBOOT_VERIFY_FIRMWARE || CONFIG_VBOOT2_VERIFY_FIRMWARE
struct vboot_region {
	uintptr_t offset_addr;
	int32_t size;
};

/*
 * The vboot handoff structure keeps track of a maximum number of firmware
 * components in the verfieid RW area of flash.  This is not a restriction on
 * the number of components packed in a firmware block. It's only the maximum
 * number of parsed firmware components (address and size) included in the
 * handoff structure.
 */
#define MAX_PARSED_FW_COMPONENTS 5

/* The FW areas consist of multiple components. At the beginning of
 * each area is the number of total compoments as well as the size and
 * offset for each component. One needs to caculate the total size of the
 * signed firmware region based off of the embedded metadata. */
struct vboot_component_entry {
	uint32_t offset;
	uint32_t size;
} __attribute__((packed));

struct vboot_components {
	uint32_t num_components;
	struct vboot_component_entry entries[0];
} __attribute__((packed));

void vboot_locate_region(const char *name, struct vboot_region *region);

struct vboot_components *vboot_locate_components(struct vboot_region *region);

/*
 * This is a dual purpose routine. If dest is non-NULL the region at
 * offset_addr will be read into the area pointed to by dest.  If dest
 * is NULL,the region will be mapped to a memory location. NULL is
 * returned on error else the location of the requested region.
 */
void *vboot_get_region(uintptr_t offset_addr, size_t size, void *dest);
/* Returns 0 on success < 0 on error. */
int vboot_get_handoff_info(void **addr, uint32_t *size);
int vboot_enable_developer(void);
int vboot_enable_recovery(void);
int vboot_skip_display_init(void);
#else
static inline void vboot_verify_firmware(struct romstage_handoff *h) {}
static inline void *vboot_get_payload(int *len) { return NULL; }
static inline int vboot_get_handoff_info(void **addr, uint32_t *size)
{
	return -1;
}
static inline int vboot_skip_display_init(void) { return 0; }
#endif /* CONFIG_VBOOT_VERIFY_FIRMWARE || CONFIG_VBOOT2_VERIFY_FIRMWARE */

int vboot_get_sw_write_protect(void);

#include "gnvs.h"
struct device;

#if CONFIG_CHROMEOS_RAMOOPS
void chromeos_ram_oops_init(chromeos_acpi_t *chromeos);
#if CONFIG_CHROMEOS_RAMOOPS_DYNAMIC
static inline void chromeos_reserve_ram_oops(struct device *dev, int idx) {}
#else /* CONFIG_CHROMEOS_RAMOOPS_DYNAMIC */
void chromeos_reserve_ram_oops(struct device *dev, int idx);
#endif /* CONFIG_CHROMEOS_RAMOOPS_DYNAMIC */
#else  /* !CONFIG_CHROMEOS_RAMOOPS */
static inline void chromeos_ram_oops_init(chromeos_acpi_t *chromeos) {}
static inline void chromeos_reserve_ram_oops(struct device *dev, int idx) {}
#endif /* CONFIG_CHROMEOS_RAMOOPS */

void vboot2_verify_firmware(void);

#if CONFIG_VBOOT2_VERIFY_FIRMWARE
void *vboot_load_ramstage(void);
void verstage_main(void);
void *vboot_load_stage(int stage_index,
		       struct vboot_region *fw_main,
		       struct vboot_components *fw_info);
void vboot_reboot(void);

/*
 * this is placed at the start of the vboot work buffer. selected_region is used
 * for the verstage to return the location of the selected slot. buffer is used
 * by the vboot2 core. Keep the struct cpu architecture agnostic as it crosses
 * stage boundaries.
 */
struct vb2_working_data {
	uint32_t selected_region_offset;
	uint32_t selected_region_size;
	uint64_t buffer_size;
	uint64_t buffer;
};

struct vb2_working_data * const vboot_get_working_data(void);

static inline void vb2_get_selected_region(struct vb2_working_data *wd,
			     struct vboot_region *region)
{
	region->offset_addr = wd->selected_region_offset;
	region->size = wd->selected_region_size;
}

static inline void vb2_set_selected_region(struct vb2_working_data *wd,
			     struct vboot_region *region)
{
	wd->selected_region_offset = region->offset_addr;
	wd->selected_region_size = region->size;
}

static inline int vboot_is_slot_selected(struct vb2_working_data *wd)
{
	return wd->selected_region_size > 0;
}

static inline int vboot_is_readonly_path(struct vb2_working_data *wd)
{
	return wd->selected_region_size == 0;
}

#endif /* CONFIG_VBOOT2_VERIFY_FIRMWARE */

#endif
