/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fsp/util.h>
#include <types.h>

struct amd_image_revision {
	uint8_t build;
	uint8_t revision;
	uint8_t minor;
	uint8_t major;
} __packed;

/* Validate the FSP-M header in romstage */
void soc_validate_fspm_header(const struct fsp_header *hdr)
{
	struct amd_image_revision *rev;

	rev = (struct amd_image_revision *)&(hdr->image_revision);

	/* Check if the image fits into the reserved memory region */
	if (hdr->image_size > CONFIG_FSP_M_SIZE)
		die("The FSP-M binary is %u bytes larger than the memory region"
		" allocated for it. Increase FSP_M_SIZE to make it fit.\n",
			hdr->image_size - CONFIG_FSP_M_SIZE);

	/* a coding bug on the AMD FSP side makes this value 1 in
	older versions of the FSP.*/
	if (hdr->image_revision == 1) {
		printk(BIOS_WARNING, "No AMD FSP image revision information available\n");
		return;
	}

	printk(BIOS_INFO, "FSP major    = %d\n", rev->major);
	printk(BIOS_INFO, "FSP minor    = %d\n", rev->minor);
	printk(BIOS_INFO, "FSP revision = %d\n", rev->revision);
	printk(BIOS_INFO, "FSP build    = %d\n", rev->build);

	if ((rev->major != IMAGE_REVISION_MAJOR_VERSION) ||
	(rev->minor != IMAGE_REVISION_MINOR_VERSION)) {
		printk(BIOS_WARNING, "FSP binary and SOC FSP header file don't match.\n");
		printk(BIOS_WARNING, "include file ImageRevisionMajorVersion=%d\n",
			IMAGE_REVISION_MAJOR_VERSION);
		printk(BIOS_WARNING, "include file ImageRevisionMinorVersion=%d\n",
			IMAGE_REVISION_MINOR_VERSION);
		printk(BIOS_WARNING, "Please update FspmUpd.h based on the corresponding FSP"
			" build's FspmUpd.h\n");
	}

	if (rev->major != IMAGE_REVISION_MAJOR_VERSION)
		die("IMAGE_REVISION_MAJOR_VERSION mismatch, halting\nGoodbye now\n");
}
