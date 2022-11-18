/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <northbridge/intel/ironlake/ironlake.h>
#include <southbridge/intel/ibexpeak/me.h>
#include <southbridge/intel/ibexpeak/pch.h>
#include <types.h>

#define HECIDEV PCI_DEV(0, 0x16, 0)

/* FIXME: add timeout.  */
static void wait_heci_ready(void)
{
	while (!(read32(DEFAULT_HECIBAR + 0xc) & 8))	// = 0x8000000c
		;

	write32((DEFAULT_HECIBAR + 0x4), (read32(DEFAULT_HECIBAR + 0x4) & ~0x10) | 0xc);
}

/* FIXME: add timeout.  */
static void wait_heci_cb_avail(int len)
{
	union {
		struct mei_csr csr;
		u32 raw;
	} csr;

	while (!(read32(DEFAULT_HECIBAR + 0xc) & 8))
		;

	do {
		csr.raw = read32(DEFAULT_HECIBAR + 0x4);
	} while (len > csr.csr.buffer_depth - (csr.csr.buffer_write_ptr -
					       csr.csr.buffer_read_ptr));
}

static void send_heci_packet_dword(u8 *payload, size_t length)
{
	int i;
	for (i = 0; i < length; i += sizeof(uint32_t)) {
		uint32_t dword = 0;
		size_t bytes = MIN(length - i, sizeof(dword));
		memcpy(&dword, payload + i, bytes);
		write32(DEFAULT_HECIBAR + 0, dword);
	}
}

static void send_heci_packet(struct mei_header *head, u8 *payload)
{
	wait_heci_cb_avail(DIV_ROUND_UP(sizeof(*head) + head->length, sizeof(u32)));

	send_heci_packet_dword((u8 *)head, sizeof(*head));
	send_heci_packet_dword(payload, head->length);

	write32(DEFAULT_HECIBAR + 0x4, read32(DEFAULT_HECIBAR + 0x4) | 0x4);
}

static void send_heci_message(u8 *msg, int len, u8 hostaddress, u8 clientaddress)
{
	struct mei_header head;
	int maxlen;

	wait_heci_ready();
	maxlen = (read32(DEFAULT_HECIBAR + 0x4) >> 24) * 4 - 4;

	while (len) {
		int cur = len;
		if (cur > maxlen) {
			cur = maxlen;
			head.is_complete = 0;
		} else
			head.is_complete = 1;
		head.length = cur;
		head.reserved = 0;
		head.client_address = clientaddress;
		head.host_address = hostaddress;
		send_heci_packet(&head, msg);
		len -= cur;
		msg += cur;
	}
}

/* FIXME: Add timeout.  */
static int recv_heci_packet(struct mei_header *head, u32 *packet, u32 *packet_size)
{
	union {
		struct mei_csr csr;
		u32 raw;
	} csr;
	int i = 0;

	write32(DEFAULT_HECIBAR + 0x4, read32(DEFAULT_HECIBAR + 0x4) | 2);
	do {
		csr.raw = read32(DEFAULT_HECIBAR + 0xc);
	} while (csr.csr.buffer_write_ptr == csr.csr.buffer_read_ptr);

	*(u32 *)head = read32(DEFAULT_HECIBAR + 0x8);
	if (!head->length) {
		write32(DEFAULT_HECIBAR + 0x4, read32(DEFAULT_HECIBAR + 0x4) | 2);
		*packet_size = 0;
		return 0;
	}
	if (head->length + 4 > 4 * csr.csr.buffer_depth || head->length > *packet_size) {
		*packet_size = 0;
		return -1;
	}

	do {
		csr.raw = read32(DEFAULT_HECIBAR + 0xc);
	} while (((head->length + 3) >> 2) >
		(csr.csr.buffer_write_ptr - csr.csr.buffer_read_ptr));

	for (i = 0; i < (head->length + 3) >> 2; i++)
		packet[i++] = read32(DEFAULT_HECIBAR + 0x8);
	*packet_size = head->length;
	if (!csr.csr.ready)
		*packet_size = 0;
	write32(DEFAULT_HECIBAR + 0x4, read32(DEFAULT_HECIBAR + 0x4) | 4);
	return 0;
}

union uma_reply {
	struct {
		u8 group_id;
		u8 command;
		u8 reserved;
		u8 result;
		u8 field2;
		u8 unk3[0x48 - 4 - 1];
	};
	u32 dwords[0x48 / sizeof(u32)];
} __packed;

/* FIXME: Add timeout.  */
static int recv_heci_message(union uma_reply *message, u32 *message_size)
{
	struct mei_header head;
	int current_position;

	current_position = 0;
	while (1) {
		u32 current_size;
		current_size = *message_size - current_position;
		if (recv_heci_packet
		    (&head, &message->dwords[current_position / sizeof(u32)],
		     &current_size) == -1)
			break;
		if (!current_size)
			break;
		current_position += current_size;
		if (head.is_complete) {
			*message_size = current_position;
			return 0;
		}

		if (current_position >= *message_size)
			break;
	}
	*message_size = 0;
	return -1;
}

static void send_heci_uma_message(const u64 heci_uma_addr, const unsigned int heci_uma_size)
{
	union uma_reply reply;

	struct uma_message {
		u8 group_id;
		u8 cmd;
		u8 reserved;
		u8 result;
		u32 c2;
		u64 heci_uma_addr;
		u32 heci_uma_size;
		u16 c3;
	} __packed msg = {
		.group_id      = 0,
		.cmd           = MKHI_SET_UMA,
		.reserved      = 0,
		.result        = 0,
		.c2            = 0x82,
		.heci_uma_addr = heci_uma_addr,
		.heci_uma_size = heci_uma_size,
		.c3            = 0,
	};
	u32 reply_size;

	send_heci_message((u8 *)&msg, sizeof(msg), 0, 7);

	reply_size = sizeof(reply);
	if (recv_heci_message(&reply, &reply_size) == -1)
		return;

	if (reply.command != (MKHI_SET_UMA | (1 << 7)))
		die("HECI init failed\n");
}

void setup_heci_uma(u64 heci_uma_addr, unsigned int heci_uma_size)
{
	if (!heci_uma_size && !(pci_read_config32(HECIDEV, 0x40) & 0x20))
		return;

	if (heci_uma_size) {
		dmibar_clrbits32(DMIVC0RCTL, 1 << 7);
		RCBA32(0x14) &= ~0x80;
		dmibar_clrbits32(DMIVC1RCTL, 1 << 7);
		RCBA32(0x20) &= ~0x80;
		dmibar_clrbits32(DMIVCPRCTL, 1 << 7);
		RCBA32(0x30) &= ~0x80;
		dmibar_clrbits32(DMIVCMRCTL, 1 << 7);
		RCBA32(0x40) &= ~0x80;

		RCBA32(0x40) = 0x87000080;	// OK
		dmibar_write32(DMIVCMRCTL, 0x87000080);	// OK

		while ((RCBA16(0x46) & 2) && dmibar_read16(DMIVCMRSTS) & VCMNP)
			;
	}

	mchbar_write32(0x24, 0x10000 + heci_uma_size);

	send_heci_uma_message(heci_uma_addr, heci_uma_size);

	pci_write_config32(HECIDEV, 0x10, 0x0);
	pci_write_config8(HECIDEV, 0x4, 0x0);
}
