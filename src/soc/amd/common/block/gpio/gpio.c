/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_pm.h>
#include <device/mmio.h>
#include <device/device.h>
#include <console/console.h>
#include <elog.h>
#include <gpio.h>
#include <amdblocks/acpi.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/gpio.h>
#include <amdblocks/smi.h>
#include <soc/smi.h>
#include <assert.h>
#include <string.h>
#include <types.h>

/*
 * acpimmio_gpio0, acpimmio_remote_gpio and acpimmio_iomux are defined in
 * soc/amd/common/block/acpimmio/mmio_util.c and declared as extern variables/constants in
 * amdblocks/acpimmio.h which is included in this file.
 */

/* MMIO access of new-style GPIO bank configuration registers */
static inline void *gpio_ctrl_ptr(gpio_t gpio_num)
{
	if (SOC_GPIO_TOTAL_PINS < AMD_GPIO_FIRST_REMOTE_GPIO_NUMBER ||
			/* Verstage on PSP would need to map acpimmio_remote_gpio */
			(CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK) && ENV_SEPARATE_VERSTAGE) ||
			gpio_num < AMD_GPIO_FIRST_REMOTE_GPIO_NUMBER)
		return acpimmio_gpio0 + gpio_num * sizeof(uint32_t);
	else
		return acpimmio_remote_gpio +
			(gpio_num - AMD_GPIO_FIRST_REMOTE_GPIO_NUMBER) * sizeof(uint32_t);
}

static inline uint32_t gpio_read32(gpio_t gpio_num)
{
	return read32(gpio_ctrl_ptr(gpio_num));
}

static inline void gpio_write32(gpio_t gpio_num, uint32_t value)
{
	write32(gpio_ctrl_ptr(gpio_num), value);
}

static inline void *gpio_mux_ptr(gpio_t gpio_num)
{
	if (SOC_GPIO_TOTAL_PINS < AMD_GPIO_FIRST_REMOTE_GPIO_NUMBER ||
			/* Verstage on PSP would need to map acpimmio_remote_gpio */
			(CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK) && ENV_SEPARATE_VERSTAGE) ||
			gpio_num < AMD_GPIO_FIRST_REMOTE_GPIO_NUMBER)
		return acpimmio_iomux + gpio_num;
	else
		return acpimmio_remote_gpio + AMD_GPIO_REMOTE_GPIO_MUX_OFFSET +
			(gpio_num - AMD_GPIO_FIRST_REMOTE_GPIO_NUMBER);
}

static uint8_t get_gpio_mux(gpio_t gpio_num)
{
	return read8(gpio_mux_ptr(gpio_num));
}

static void set_gpio_mux(gpio_t gpio_num, uint8_t function)
{
	write8(gpio_mux_ptr(gpio_num), function & AMD_GPIO_MUX_MASK);
	get_gpio_mux(gpio_num); /* Flush posted write */
}

static int get_gpio_gevent(gpio_t gpio, const struct soc_amd_event *table,
				size_t items)
{
	size_t i;

	for (i = 0; i < items; i++) {
		if ((table + i)->gpio == gpio)
			return (int)(table + i)->event;
	}
	return -1;
}

static void program_smi(uint32_t flags, unsigned int gevent_num)
{
	uint8_t level;

	if (!is_gpio_event_level_triggered(flags)) {
		printk(BIOS_ERR, "%s - Only level trigger allowed for SMI!\n", __func__);
		BUG();
		return;
	}

	if (is_gpio_event_active_high(flags))
		level = SMI_SCI_LVL_HIGH;
	else
		level = SMI_SCI_LVL_LOW;

	configure_gevent_smi(gevent_num, SMI_MODE_SMI, level);
}

/*
 * For each general purpose event, GPE, the choice of edge/level triggered
 * event is represented as a single bit in SMI_SCI_LEVEL register.
 *
 * In a similar fashion, polarity (rising/falling, hi/lo) of each GPE is
 * represented as a single bit in SMI_SCI_TRIG register.
 */
static void program_sci(uint32_t flags, unsigned int gevent_num)
{
	struct sci_source sci;

	sci.scimap = gevent_num;
	sci.gpe = gevent_num;

	if (is_gpio_event_level_triggered(flags))
		sci.level = SMI_SCI_LVL;
	else
		sci.level = SMI_SCI_EDG;

	if (is_gpio_event_active_high(flags))
		sci.direction = SMI_SCI_LVL_HIGH;
	else
		sci.direction = SMI_SCI_LVL_LOW;

	configure_scimap(&sci);
}

static void gpio_update32(gpio_t gpio_num, uint32_t mask, uint32_t or)
{
	uint32_t reg;

	reg = gpio_read32(gpio_num);
	reg &= mask;
	reg |= or;
	gpio_write32(gpio_num, reg);
}

/* Set specified bits of a register to match those of ctrl. */
static void gpio_setbits32(gpio_t gpio_num, uint32_t mask, uint32_t ctrl)
{
	gpio_update32(gpio_num, ~mask, ctrl & mask);
}

static void gpio_and32(gpio_t gpio_num, uint32_t mask)
{
	gpio_update32(gpio_num, mask, 0);
}

static void gpio_or32(gpio_t gpio_num, uint32_t or)
{
	gpio_update32(gpio_num, 0xffffffff, or);
}

static void master_switch_clr(uint32_t mask)
{
	const gpio_t master_reg = GPIO_MASTER_SWITCH / sizeof(uint32_t);
	gpio_and32(master_reg, ~mask);
}

static void master_switch_set(uint32_t or)
{
	const gpio_t master_reg = GPIO_MASTER_SWITCH / sizeof(uint32_t);
	gpio_or32(master_reg, or);
}

int gpio_get(gpio_t gpio_num)
{
	uint32_t reg;

	reg = gpio_read32(gpio_num);
	return !!(reg & GPIO_PIN_STS);
}

void gpio_set(gpio_t gpio_num, int value)
{
	gpio_setbits32(gpio_num, GPIO_OUTPUT_VALUE, value ? GPIO_OUTPUT_VALUE : 0);
}

void gpio_input_pulldown(gpio_t gpio_num)
{
	gpio_setbits32(gpio_num, GPIO_PULL_MASK | GPIO_OUTPUT_ENABLE, GPIO_PULLDOWN_ENABLE);
}

void gpio_input_pullup(gpio_t gpio_num)
{
	gpio_setbits32(gpio_num, GPIO_PULL_MASK | GPIO_OUTPUT_ENABLE, GPIO_PULLUP_ENABLE);
}

void gpio_input(gpio_t gpio_num)
{
	gpio_and32(gpio_num, ~(GPIO_PULL_MASK | GPIO_OUTPUT_ENABLE));
}

void gpio_output(gpio_t gpio_num, int value)
{
	/* set GPIO output value before setting the direction to output to avoid glitches */
	gpio_set(gpio_num, value);
	gpio_or32(gpio_num, GPIO_OUTPUT_ENABLE);
}

const char *gpio_acpi_path(gpio_t gpio)
{
	return "\\_SB.GPIO";
}

uint16_t gpio_acpi_pin(gpio_t gpio)
{
	return gpio;
}

void gpio_save_pin_registers(gpio_t gpio, struct soc_amd_gpio_register_save *save)
{
	save->mux_value = get_gpio_mux(gpio);
	save->control_value = gpio_read32(gpio);
}

void gpio_restore_pin_registers(gpio_t gpio, struct soc_amd_gpio_register_save *save)
{
	set_gpio_mux(gpio, save->mux_value);
	gpio_write32(gpio, save->control_value);
	gpio_read32(gpio); /* Flush posted write */
}

static void set_single_gpio(const struct soc_amd_gpio *g)
{
	static const struct soc_amd_event *gev_tbl;
	static size_t gev_items;
	int gevent_num;
	const bool can_set_smi_flags = !((CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK) &&
			ENV_SEPARATE_VERSTAGE) ||
			CONFIG(SOC_AMD_COMMON_BLOCK_BANKED_GPIOS_NON_SOC_CODEBASE));

	set_gpio_mux(g->gpio, g->function);

	gpio_setbits32(g->gpio, PAD_CFG_MASK, g->control);
	/* Clear interrupt and wake status (write 1-to-clear bits) */
	gpio_or32(g->gpio, GPIO_INT_STATUS | GPIO_WAKE_STATUS);
	if (g->flags == 0)
		return;

	/* Can't set SMI flags from PSP */
	if (!can_set_smi_flags)
		return;

	if (gev_tbl == NULL)
		soc_get_gpio_event_table(&gev_tbl, &gev_items);

	gevent_num = get_gpio_gevent(g->gpio, gev_tbl, gev_items);
	if (gevent_num < 0) {
		printk(BIOS_WARNING, "GPIO pin %d has no associated gevent!\n",
				     g->gpio);
		return;
	}

	if (g->flags & GPIO_FLAG_SMI) {
		program_smi(g->flags, gevent_num);
	} else if (g->flags & GPIO_FLAG_SCI) {
		program_sci(g->flags, gevent_num);
	}
}

void gpio_configure_pads_with_override(const struct soc_amd_gpio *base_cfg,
					size_t base_num_pads,
					const struct soc_amd_gpio *override_cfg,
					size_t override_num_pads)
{
	const struct soc_amd_gpio *c;
	size_t i, j;

	if (!base_cfg || !base_num_pads)
		return;

	/*
	 * Disable blocking wake/interrupt status generation while updating
	 * debounce registers. Otherwise when a debounce register is updated
	 * the whole GPIO controller will zero out all interrupt enable status
	 * bits while the delay happens. This could cause us to drop the bits
	 * due to the read-modify-write that happens on each register.
	 *
	 * Additionally disable interrupt generation so we don't get any
	 * spurious interrupts while updating the registers.
	 */
	master_switch_clr(GPIO_MASK_STS_EN | GPIO_INTERRUPT_EN);

	for (i = 0; i < base_num_pads; i++) {
		c = &base_cfg[i];
		/* Check if override exist for GPIO from the base configuration */
		for (j = 0; override_cfg && j < override_num_pads; j++) {
			if (c->gpio == override_cfg[j].gpio) {
				c = &override_cfg[j];
				break;
			}
		}
		set_single_gpio(c);
	}

	/*
	 * Re-enable interrupt status generation.
	 *
	 * We leave MASK_STATUS disabled because the kernel may reconfigure the
	 * debounce registers while the drivers load. This will cause interrupts
	 * to be missed during boot.
	 */
	master_switch_set(GPIO_INTERRUPT_EN);
}

void gpio_configure_pads(const struct soc_amd_gpio *gpio_list_ptr, size_t size)
{
	gpio_configure_pads_with_override(gpio_list_ptr, size, NULL, 0);
}

int gpio_interrupt_status(gpio_t gpio)
{
	uint32_t reg = gpio_read32(gpio);

	if (reg & GPIO_INT_STATUS) {
		/* Clear interrupt status, preserve wake status */
		reg &= ~GPIO_WAKE_STATUS;
		gpio_write32(gpio, reg);
		return 1;
	}

	return 0;
}

static void check_and_add_wake_gpio(gpio_t begin, gpio_t end, struct gpio_wake_state *state)
{
	gpio_t i;
	uint32_t reg;

	for (i = begin; i < end; i++) {
		reg = gpio_read32(i);
		if (!(reg & GPIO_WAKE_STATUS))
			continue;
		printk(BIOS_INFO, "GPIO %d woke system.\n", i);
		if (state->num_valid_wake_gpios >= ARRAY_SIZE(state->wake_gpios))
			continue;
		state->wake_gpios[state->num_valid_wake_gpios++] = i;
	}
}

static void check_gpios(uint32_t wake_stat, unsigned int bit_limit, gpio_t gpio_base,
			struct gpio_wake_state *state)
{
	unsigned int i;
	gpio_t begin;
	gpio_t end;

	for (i = 0; i < bit_limit; i++) {
		if (!(wake_stat & BIT(i)))
			continue;
		/* Each wake status register bit is for 4 GPIOs that then will be checked */
		begin = gpio_base + i * 4;
		end = begin + 4;
		/* There is no gpio 63. */
		if (begin == 60)
			end = 63;
		check_and_add_wake_gpio(begin, end, state);
	}
}

void gpio_fill_wake_state(struct gpio_wake_state *state)
{
	/* Turn the wake registers into "gpio" index to conform to existing API. */
	const gpio_t stat0 = GPIO_WAKE_STAT_0 / sizeof(uint32_t);
	const gpio_t stat1 = GPIO_WAKE_STAT_1 / sizeof(uint32_t);
	const gpio_t control_switch = GPIO_MASTER_SWITCH / sizeof(uint32_t);

	memset(state, 0, sizeof(*state));

	state->control_switch = gpio_read32(control_switch);
	state->wake_stat[0] = gpio_read32(stat0);
	state->wake_stat[1] = gpio_read32(stat1);

	printk(BIOS_INFO, "GPIO Control Switch: 0x%08x, Wake Stat 0: 0x%08x, Wake Stat 1: 0x%08x\n",
		state->control_switch, state->wake_stat[0], state->wake_stat[1]);

	check_gpios(state->wake_stat[0], 32, 0, state);
	check_gpios(state->wake_stat[1], 14, 128, state);
}

void gpio_add_events(void)
{
	const struct chipset_power_state *ps;
	const struct gpio_wake_state *state;
	unsigned int i;
	unsigned int end;

	if (acpi_fetch_pm_state(&ps, PS_CLAIMER_ELOG) < 0)
		return;
	state = &ps->gpio_state;

	end = MIN(state->num_valid_wake_gpios, ARRAY_SIZE(state->wake_gpios));
	for (i = 0; i < end; i++)
		elog_add_event_wake(ELOG_WAKE_SOURCE_GPIO, state->wake_gpios[i]);
}
