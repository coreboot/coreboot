/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <fsp/util.h>

void fsp_print_header_info(const struct fsp_header *hdr)
{
	union fsp_revision revision;
	union extended_fsp_revision ext_revision;
	ext_revision.val = 0;
	int i;

	/* For FSP 2.3 and later use extended image revision field present in header
	 * for build number and revision calculation */
	if (CONFIG(PLATFORM_USES_FSP2_3))
		ext_revision.val = hdr->extended_image_revision;

	revision.val = hdr->image_revision;
	printk(BIOS_SPEW, "Spec version: v%u.%u\n", (hdr->spec_version >> 4),
						     hdr->spec_version & 0xf);
	printk(BIOS_SPEW, "Revision: %u.%u.%u, Build Number %u\n",
			revision.rev.major,
			revision.rev.minor,
			((ext_revision.rev.revision << 8) | revision.rev.revision),
			((ext_revision.rev.bld_num << 8) | revision.rev.bld_num));
	printk(BIOS_SPEW, "Type: %s/%s\n",
			(hdr->component_attribute & 1) ? "release" : "debug",
			(hdr->component_attribute & 2) ? "official" : "test");

	printk(BIOS_SPEW, "image ID: ");
	for (i = 0; i < FSP_IMAGE_ID_LENGTH; i++)
		printk(BIOS_SPEW, "%c", hdr->image_id[i]);
	printk(BIOS_SPEW, "\n");

	printk(BIOS_SPEW, "	base 0x%zx + 0x%zx\n",
		(size_t)hdr->image_base, (size_t)hdr->image_size);
	printk(BIOS_SPEW, "\tConfig region        0x%zx + 0x%zx\n",
		(size_t)hdr->cfg_region_offset, (size_t)hdr->cfg_region_size);

	if ((hdr->component_attribute >> 12) == FSP_HDR_ATTRIB_FSPM) {
		printk(BIOS_SPEW, "\tMemory init offset   0x%zx\n",
		       (size_t)hdr->fsp_memory_init_entry_offset);
	}

	if ((hdr->component_attribute >> 12) == FSP_HDR_ATTRIB_FSPS) {
		printk(BIOS_SPEW, "\tSilicon init offset  0x%zx\n",
		       (size_t)hdr->fsp_silicon_init_entry_offset);
		if (CONFIG(PLATFORM_USES_FSP2_2))
			printk(BIOS_SPEW, "\tMultiPhaseSiInit offset  0x%zx\n",
			       (size_t)hdr->fsp_multi_phase_si_init_entry_offset);
		printk(BIOS_SPEW, "\tNotify phase offset  0x%zx\n",
		       (size_t)hdr->notify_phase_entry_offset);
	}
}
