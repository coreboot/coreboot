/* SPDX-License-Identifier: GPL-2.0-only */

#include <libpayload.h>
#include <endian.h>
#include <commonlib/device_tree.h>

uint64_t boot_hartid;
/**
 * This is our C entry function - set up the system and jump into the payload entry point.
 * Unlike other archs, we do not save a0~a2 from previous stage as global variables in head.S,
 * but pass them to start_main directly as arg0, arg1, arg2 instead.
 *
 * arg0 is the preferred boot hart id.
 * arg1 is a pointer to the DTB.
 * This interface is static no matter whether previous stage is OpenSBI or coreboot Ramstage.
 *
 * arg2 is reserved for possible future usage.
 */
void start_main(uint64_t arg0, uint64_t arg1, uint64_t arg2);
void start_main(uint64_t arg0, uint64_t arg1, uint64_t arg2)
{
	extern int main(int argc, char **argv);
	extern void *cb_header_ptr;

	/* cbtable pointer is stored in the dtb passed by previous stage */
	boot_hartid = arg0;
	const void *dtb = (const void *)arg1;
	if (dtb && fdt_is_valid(dtb)) {
		/* Look for the "coreboot-table" node in the DTB */
		u32 node_offset = fdt_find_node_by_path(dtb, "/chosen", NULL, NULL);
		if (node_offset) {
			struct fdt_property prop;
			if (fdt_read_prop(dtb, node_offset, "coreboot-table", &prop)) {
				/* Extract the coreboot table pointer from the property */
				cb_header_ptr = (void *)be64dec(prop.data);
			}
		}
	}

	/* Gather system information. */
	lib_get_sysinfo();

#if !CONFIG(LP_SKIP_CONSOLE_INIT)
	console_init();
#endif

	/*
	 * Go to the entry point.
	 * In the future we may care about the return value.
	 */
	main(0, NULL);
	printf("Unexpected return from payload\n");
}
