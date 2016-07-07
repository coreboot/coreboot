/*
 * Copyright (c) 2013, The Regents of the University of California (Regents).
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Regents nor the
 * names of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
 * SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
 * OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
 * HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
 * MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#include <spike_util.h>
#include <arch/errno.h>
#include <atomic.h>
#include <string.h>
#include <console/console.h>

uintptr_t translate_address(uintptr_t vAddr) {
	// TODO: implement the page table translation algorithm
	//uintptr_t pageTableRoot = read_csr(sptbr);
	uintptr_t physAddrMask = 0xfffffff;
	uintptr_t translationResult = vAddr & physAddrMask;
	printk(BIOS_DEBUG, "Translated virtual address 0x%llx to physical address 0x%llx\n", vAddr, translationResult);
	return translationResult;
}

uintptr_t mcall_query_memory(uintptr_t id, memory_block_info *p)
{
	uintptr_t physicalAddr = translate_address((uintptr_t) p);
	memory_block_info *info = (memory_block_info*) physicalAddr;
	if (id == 0) {
		info->base = 0x1000000; // hard coded for now, but we can put these values somewhere later
		info->size = 0x7F000000 - info->base;
		return 0;
	}

	return -1;
}

uintptr_t mcall_send_ipi(uintptr_t recipient)
{
	die("mcall_send_ipi is currently not implemented");
	return 0;
}

uintptr_t mcall_clear_ipi(void)
{
	// only clear SSIP if no other events are pending
	if (HLS()->device_response_queue_head == NULL) {
		clear_csr(mip, MIP_SSIP);
		mb();
	}

	return atomic_swap(&HLS()->ipi_pending, 0);
}

uintptr_t mcall_shutdown(void)
{
	die("mcall_shutdown is currently not implemented");
	return 0;
}

uintptr_t mcall_set_timer(unsigned long long when)
{
	die("mcall_set_timer is currently not implemented");
	return 0;
}

uintptr_t mcall_dev_req(sbi_device_message *m)
{
	die("mcall_dev_req is currently not implemented");
	return 0;
}

uintptr_t mcall_dev_resp(void)
{
	die("mcall_dev_resp is currently not implemented");
	return 0;
}

uintptr_t mcall_hart_id(void)
{
	return HLS()->hart_id;
}

void hls_init(uint32_t hart_id)
{
	memset(HLS(), 0, sizeof(*HLS()));
	HLS()->hart_id = hart_id;
}

uintptr_t mcall_console_putchar(uint8_t ch)
{
	do_putchar(ch);
	return 0;
}

void testPrint(void) {
	/* Print a test command to check Spike console output */
	mcall_console_putchar('h');
	mcall_console_putchar('e');
	mcall_console_putchar('l');
	mcall_console_putchar('l');
	mcall_console_putchar('o');
	mcall_console_putchar('\n');
}
