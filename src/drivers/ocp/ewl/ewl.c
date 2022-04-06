/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/soc_util.h>
#include <lib.h>
#include "ocp_ewl.h"

static void process_ewl_type3(EWL_ENTRY_HEADER *header, EWL_ENTRY_MEMORY_LOCATION memory_location)
{
	/* Treat warning as type 3, collect basic information and print to serial log */
	EWL_ENTRY_TYPE3 *basic_warning;
	basic_warning = (EWL_ENTRY_TYPE3 *)header;
	printk(BIOS_ERR, "Major Warning Code = 0x%02x, Minor Warning Code = 0x%02x,\n",
			basic_warning->Context.MajorWarningCode,
			basic_warning->Context.MinorWarningCode);
	printk(BIOS_ERR, "Major Checkpoint: 0x%02x\n", basic_warning->Context.MajorCheckpoint);
	printk(BIOS_ERR, "Minor Checkpoint: 0x%02x\n", basic_warning->Context.MinorCheckpoint);

	if (memory_location.Socket != 0xff)
		printk(BIOS_ERR, "Socket %d\n", memory_location.Socket);
	if (memory_location.Channel != 0xff)
		printk(BIOS_ERR, "Channel %d\n", memory_location.Channel);
	if (memory_location.Dimm != 0xff)
		printk(BIOS_ERR, "Dimm %d\n", memory_location.Dimm);
	if (memory_location.Rank != 0xff)
		printk(BIOS_ERR, "Rank %d\n", memory_location.Rank);
}

void get_ewl(void)
{
	const EWL_PRIVATE_DATA *hob = get_ewl_hob();
	int offset = 0;
	bool type3_flag = 0;
	EWL_ENTRY_HEADER *warning_header;
	printk(BIOS_DEBUG, "Number of EWL entries %d\n", hob->numEntries);
	while (offset < hob->status.Header.FreeOffset) {
		warning_header = (EWL_ENTRY_HEADER *) &(hob->status.Buffer[offset]);
		if (warning_header->Type == EwlType3) {
			printk(BIOS_ERR, "EWL type: %d\n", warning_header->Type);
			if (warning_header->Size != sizeof(EWL_ENTRY_TYPE3)) {
				printk(BIOS_ERR, "EWL type3 size mismatch!\n");
				return;
			}
			EWL_ENTRY_TYPE3 *type3;
			type3 = (EWL_ENTRY_TYPE3 *)warning_header;
			process_ewl_type3(warning_header, type3->MemoryLocation);
			type3_flag = 1;
		} else {
			printk(BIOS_DEBUG, "EWL type: %d size:%d severity level:%d\n",
					warning_header->Type,
					warning_header->Size,
					warning_header->Severity);
			hexdump(&(hob->status.Buffer[offset]), warning_header->Size);
		}
		offset += warning_header->Size;
	}
	if (type3_flag)
		die("Memory Training Error!\n");
}
