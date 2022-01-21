/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <fsp/util.h>

void display_fsp_error_info_hob(const void *hob)
{
	const FSP_ERROR_INFO_HOB *fsp_error_info_hob = (FSP_ERROR_INFO_HOB *)hob;
	printk(BIOS_ERR, "FspErrorInfoHob->CallerId  = ");
	fsp_print_guid(BIOS_CRIT, &fsp_error_info_hob->CallerId);
	printk(BIOS_ERR, "\nFspErrorInfoHob->ErrorType  = ");
	fsp_print_guid(BIOS_CRIT, &fsp_error_info_hob->ErrorType);
	printk(BIOS_ERR, "\nFspErrorInfoHob->Status  = %x\n", fsp_error_info_hob->Status);
}
