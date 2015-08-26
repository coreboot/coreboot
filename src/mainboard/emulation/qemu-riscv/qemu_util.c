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
	//if (recipient >= num_harts)
	//return -1;

	if (atomic_swap(&OTHER_HLS(recipient)->ipi_pending, 1) == 0) {
		mb();
		write_csr(send_ipi, recipient);
	}

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
	while (1) write_csr(mtohost, 1);
	return 0;
}

uintptr_t mcall_set_timer(unsigned long long when)
{
	write_csr(mtimecmp, when);
	clear_csr(mip, MIP_STIP);
	set_csr(mie, MIP_MTIP);
	return 0;
}

uintptr_t mcall_dev_req(sbi_device_message *m)
{
	if ((m->dev > 0xFFU) | (m->cmd > 0xFFU) | (m->data > 0x0000FFFFFFFFFFFFU)) return -EINVAL;

	while (swap_csr(mtohost, TOHOST_CMD(m->dev, m->cmd, m->data)) != 0);

	m->sbi_private_data = (uintptr_t)HLS()->device_request_queue_head;
	HLS()->device_request_queue_head = m;
	HLS()->device_request_queue_size++;

	return 0;
}

uintptr_t mcall_dev_resp(void)
{
	htif_interrupt(0, 0);

	sbi_device_message* m = HLS()->device_response_queue_head;
	if (m) {
		//printm("resp %p\n", m);
		sbi_device_message* next = (void*)atomic_read(&m->sbi_private_data);
		HLS()->device_response_queue_head = next;
		if (!next) {
			HLS()->device_response_queue_tail = 0;

			// only clear SSIP if no other events are pending
			clear_csr(mip, MIP_SSIP);
			mb();
			if (HLS()->ipi_pending) set_csr(mip, MIP_SSIP);
		}
	}
	return (uintptr_t)m;
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

uintptr_t htif_interrupt(uintptr_t mcause, uintptr_t* regs) {
	uintptr_t fromhost = swap_csr(mfromhost, 0);
	if (!fromhost)
	return 0;

	uintptr_t dev = FROMHOST_DEV(fromhost);
	uintptr_t cmd = FROMHOST_CMD(fromhost);
	uintptr_t data = FROMHOST_DATA(fromhost);

	sbi_device_message* m = HLS()->device_request_queue_head;
	sbi_device_message* prev = 0x0;
	unsigned long i, n;
	for (i = 0, n = HLS()->device_request_queue_size; i < n; i++) {
		/*
		if (!supervisor_paddr_valid(m, sizeof(*m))
		&& EXTRACT_FIELD(read_csr(mstatus), MSTATUS_PRV1) != PRV_M)
		panic("htif: page fault");
		*/

		sbi_device_message* next = (void*)m->sbi_private_data;
		if (m->dev == dev && m->cmd == cmd) {
			m->data = data;

			// dequeue from request queue
			if (prev)
			prev->sbi_private_data = (uintptr_t)next;
			else
			HLS()->device_request_queue_head = next;
			HLS()->device_request_queue_size = n-1;
			m->sbi_private_data = 0;

			// enqueue to response queue
			if (HLS()->device_response_queue_tail)
			{
				HLS()->device_response_queue_tail->sbi_private_data = (uintptr_t)m;
			}
			else
			{
				HLS()->device_response_queue_head = m;
			}
			HLS()->device_response_queue_tail = m;

			// signal software interrupt
			set_csr(mip, MIP_SSIP);
			return 0;
		}

		prev = m;
		m = (void*)atomic_read(&m->sbi_private_data);
	}
	//HLT();
	return 0;
	//panic("htif: no record");
}

uintptr_t mcall_console_putchar(uint8_t ch)
{
	while (swap_csr(mtohost, TOHOST_CMD(1, 1, ch)) != 0);
	while (1) {
		uintptr_t fromhost = read_csr(mfromhost);
		if (FROMHOST_DEV(fromhost) != 1 || FROMHOST_CMD(fromhost) != 1) {
		if (fromhost)
		htif_interrupt(0, 0);
		continue;
	}
	write_csr(mfromhost, 0);
	break;
	}
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
