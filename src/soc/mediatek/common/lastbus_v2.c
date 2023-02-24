/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/lastbus_v2.h>
#include <timer.h>

#define SYS_TIMER_0_OFFSET 0x400
#define SYS_TIMER_1_OFFSET 0x404
#define DEBUG_RESULT_OFFSET 0x408

static bool lastbus_is_timeout(const struct lastbus_monitor *m)
{
	return read32p(m->base) & LASTBUS_TIMEOUT;
}

static uint64_t gray_code_to_binary(uint64_t gray_code)
{
	uint64_t value = 0;
	while (gray_code) {
		value ^= gray_code;
		gray_code >>= 1;
	}
	return value;
}

static void lastbus_dump_monitor(const struct lastbus_monitor *m)
{
	int i;
	uint64_t gray_code, bin_code;

	printk(BIOS_INFO, "--- %s %#lx %ld ---\n", m->name, m->base, m->num_ports);

	for (i = 0; i < m->num_ports; i++)
		printk(BIOS_INFO, "%08x\n",
		       read32p(m->base + DEBUG_RESULT_OFFSET + (i * 4)));

	gray_code = (uint64_t)read32p(m->base + SYS_TIMER_1_OFFSET) << 32 |
		    read32p(m->base + SYS_TIMER_0_OFFSET);
	bin_code = gray_code_to_binary(gray_code);
	printk(BIOS_INFO, "\ntimestamp: %#llx\n", bin_code);
}

static void lastbus_dump(void)
{
	const struct lastbus_monitor *m;
	bool found  = false;
	int i;

	for (i = 0; i < lastbus_cfg.num_used_monitors; i++) {
		m = &lastbus_cfg.monitors[i];
		if (!lastbus_is_timeout(m))
			continue;

		if (!found)
			printk(BIOS_INFO,
			       "\n******************* %s lastbus ******************\n",
			       lastbus_cfg.latch_platform);
		found = true;
		lastbus_dump_monitor(m);
	}
}

static u16 calculate_timeout_thres(u16 bus_freq_mhz, u32 timeout_ms)
{
	u64 value;
	value = ((u64)timeout_ms * USECS_PER_MSEC * bus_freq_mhz) >> 10;
	if (value >= UINT16_MAX)
		return UINT16_MAX - 1;
	return value >= 1 ? value - 1 : 0;
}

static void lastbus_init_monitor(const struct lastbus_monitor *m,
				 u32 timeout_ms, u32 timeout_type)
{
	u16 timeout_thres;
	int i;

	for (i = 0; i < m->num_idle_mask; i++)
		write32p(m->base + m->idle_masks[i].reg_offset,
			 m->idle_masks[i].reg_value);

	/* clear timeout status with DBG_CKEN */
	write32p(m->base, LASTBUS_TIMEOUT_CLR | LASTBUS_DEBUG_CKEN);
	/* de-assert clear bit */
	clrbits32p(m->base, LASTBUS_TIMEOUT_CLR);

	if (timeout_ms == UINT32_MAX)
		timeout_thres = 0xFFFF;
	else
		timeout_thres = calculate_timeout_thres(m->bus_freq_mhz, timeout_ms);

	setbits32p(m->base, (timeout_thres << TIMEOUT_THRES_SHIFT) |
			    (timeout_type << TIMEOUT_TYPE_SHIFT));
	setbits32p(m->base, LASTBUS_DEBUG_EN);
}

static void lastbus_setup(void)
{
	int i;

	for (i = 0; i < lastbus_cfg.num_used_monitors; i++)
		lastbus_init_monitor(&lastbus_cfg.monitors[i], lastbus_cfg.timeout_ms,
				     lastbus_cfg.timeout_type);
}

void lastbus_init(void)
{
	lastbus_dump();
	lastbus_setup();
}
