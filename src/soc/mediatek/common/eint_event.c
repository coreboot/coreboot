/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>
#include <device/mmio.h>
#include <soc/eint_event.h>
#include <types.h>

/* EINT reg_base + 0x880 is eint_event_mask_clr register with access type W1C. */
#define EINT_EVENT_MASK_CLR_OFFSET 0x880

#define EINT_VALUE 0xFFFFFFFF

static void enable_eint_event(const struct eint_event_info *event)
{
	uint32_t *reg = (uint32_t *)(event->reg_base + EINT_EVENT_MASK_CLR_OFFSET);
	size_t port = DIV_ROUND_UP(event->eint_num, sizeof(*reg) * BITS_PER_BYTE);
	int i;

	for (i = 0; i < port; i++)
		write32(&reg[i], EINT_VALUE);
}

void unmask_eint_event_mask(void)
{
	const struct eint_event_info *event = &eint_event[0];
	while (event->reg_base && event->eint_num) {
		enable_eint_event(event);
		event++;
	}
}
