/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_pm.h>
#include <device/mmio.h>
#include <device/device.h>
#include <console/console.h>
#include <elog.h>
#include <gpio.h>
#include <amdblocks/acpi.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/gpio_banks.h>
#include <amdblocks/smi.h>
#include <soc/gpio.h>
#include <soc/smi.h>
#include <assert.h>
#include <string.h>

static int get_gpio_gevent(uint8_t gpio, const struct soc_amd_event *table,
				size_t items)
{
	int i;

	for (i = 0; i < items; i++) {
		if ((table + i)->gpio == gpio)
			return (int)(table + i)->event;
	}
	return -1;
}

static void program_smi(uint32_t flags, int gevent_num)
{
	uint8_t level;

	if (!is_gpio_event_level_triggered(flags)) {
		printk(BIOS_ERR, "ERROR: %s - Only level trigger allowed for SMI!\n", __func__);
		BUG();
		return;
	}

	if (is_gpio_event_active_high(flags))
		level = SMI_SCI_LVL_HIGH;
	else
		level = SMI_SCI_LVL_LOW;

	configure_gevent_smi(gevent_num, SMI_MODE_SMI, level);
}

struct sci_trigger_regs {
	uint32_t mask;
	uint32_t polarity;
	uint32_t level;
};

/*
 * For each general purpose event, GPE, the choice of edge/level triggered
 * event is represented as a single bit in SMI_SCI_LEVEL register.
 *
 * In a similar fashion, polarity (rising/falling, hi/lo) of each GPE is
 * represented as a single bit in SMI_SCI_TRIG register.
 */
static void fill_sci_trigger(uint32_t flags, int gpe, struct sci_trigger_regs *regs)
{
	uint32_t mask = 1 << gpe;

	regs->mask |= mask;

	if (is_gpio_event_level_triggered(flags))
		regs->level |= mask;
	else
		regs->level &= ~mask;

	if (is_gpio_event_active_high(flags))
		regs->polarity |= mask;
	else
		regs->polarity &= ~mask;
}

/* TODO: See configure_scimap() implementations. */
static void set_sci_trigger(const struct sci_trigger_regs *regs)
{
	uint32_t value;

	value = smi_read32(SMI_SCI_TRIG);
	value &= ~regs->mask;
	value |= regs->polarity;
	smi_write32(SMI_SCI_TRIG, value);

	value = smi_read32(SMI_SCI_LEVEL);
	value &= ~regs->mask;
	value |= regs->level;
	smi_write32(SMI_SCI_LEVEL, value);
}

uintptr_t gpio_get_address(gpio_t gpio_num)
{
	return (uintptr_t)gpio_ctrl_ptr(gpio_num);
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
	gpio_update32(gpio_num, -1UL, or);
}

static void master_switch_clr(uint32_t mask)
{
	const uint8_t master_reg = GPIO_MASTER_SWITCH / sizeof(uint32_t);
	gpio_and32(master_reg, ~mask);
}

static void master_switch_set(uint32_t or)
{
	const uint8_t master_reg = GPIO_MASTER_SWITCH / sizeof(uint32_t);
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

__weak void soc_gpio_hook(uint8_t gpio, uint8_t mux) {}

void program_gpios(const struct soc_amd_gpio *gpio_list_ptr, size_t size)
{
	uint32_t control, control_flags;
	uint8_t mux, index, gpio;
	int gevent_num;
	const struct soc_amd_event *gev_tbl;
	struct sci_trigger_regs sci_trigger_cfg = { 0 };
	size_t gev_items;
	const bool can_set_smi_flags = !(CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK) &&
			ENV_SEPARATE_VERSTAGE);
	if (!gpio_list_ptr || !size)
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

	if (can_set_smi_flags)
		soc_get_gpio_event_table(&gev_tbl, &gev_items);

	for (index = 0; index < size; index++) {
		gpio = gpio_list_ptr[index].gpio;
		mux = gpio_list_ptr[index].function;
		control = gpio_list_ptr[index].control;
		control_flags = gpio_list_ptr[index].flags;

		iomux_write8(gpio, mux & AMD_GPIO_MUX_MASK);
		iomux_read8(gpio); /* Flush posted write */

		soc_gpio_hook(gpio, mux);

		gpio_setbits32(gpio, PAD_CFG_MASK, control);
		/* Clear interrupt and wake status (write 1-to-clear bits) */
		gpio_or32(gpio, GPIO_INT_STATUS | GPIO_WAKE_STATUS);
		if (control_flags == 0)
			continue;

		/* Can't set SMI flags from PSP */
		if (!can_set_smi_flags)
			continue;

		gevent_num = get_gpio_gevent(gpio, gev_tbl, gev_items);
		if (gevent_num < 0) {
			printk(BIOS_WARNING, "Warning: GPIO pin %d has no associated gevent!\n",
			       gpio);
			continue;
		}

		if (control_flags & GPIO_FLAG_SMI) {
			program_smi(control_flags, gevent_num);
		} else if (control_flags & GPIO_FLAG_SCI) {
			fill_sci_trigger(control_flags, gevent_num, &sci_trigger_cfg);
			soc_route_sci(gevent_num);
		}
	}

	/*
	 * Re-enable interrupt status generation.
	 *
	 * We leave MASK_STATUS disabled because the kernel may reconfigure the
	 * debounce registers while the drivers load. This will cause interrupts
	 * to be missed during boot.
	 */
	master_switch_set(GPIO_INTERRUPT_EN);

	/* Set all SCI trigger polarity (high/low) and level (edge/level). */
	if (can_set_smi_flags)
		set_sci_trigger(&sci_trigger_cfg);
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

/*
 * This function checks to see if there is an override config present for the
 * provided pad_config. If no override config is present, then the input config
 * is returned. Else, it returns the override config.
 */
static const struct soc_amd_gpio *gpio_get_config(const struct soc_amd_gpio *c,
				const struct soc_amd_gpio *override_cfg_table,
				size_t num)
{
	size_t i;
	if (override_cfg_table == NULL)
		return c;
	for (i = 0; i < num; i++) {
		if (c->gpio == override_cfg_table[i].gpio)
			return override_cfg_table + i;
	}
	return c;
}
void gpio_configure_pads_with_override(const struct soc_amd_gpio *base_cfg,
					size_t base_num_pads,
					const struct soc_amd_gpio *override_cfg,
					size_t override_num_pads)
{
	size_t i;
	const struct soc_amd_gpio *c;

	for (i = 0; i < base_num_pads; i++) {
		c = gpio_get_config(base_cfg + i, override_cfg,
				override_num_pads);
		program_gpios(c, 1);
	}
}

static void check_and_add_wake_gpio(int begin, int end, struct gpio_wake_state *state)
{
	int i;
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

static void check_gpios(uint32_t wake_stat, int bit_limit, int gpio_base,
			struct gpio_wake_state *state)
{
	int i;
	int begin;
	int end;

	for (i = 0; i < bit_limit; i++) {
		if (!(wake_stat & BIT(i)))
			continue;
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
	const uint8_t stat0 = GPIO_WAKE_STAT_0 / sizeof(uint32_t);
	const uint8_t stat1 = GPIO_WAKE_STAT_1 / sizeof(uint32_t);
	const uint8_t control_switch = GPIO_MASTER_SWITCH / sizeof(uint32_t);

	/* Register fields and gpio availability need to be confirmed on other chipsets. */
	if (!CONFIG(SOC_AMD_PICASSO))
		dead_code();

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
	int i;
	int end;

	if (acpi_pm_state_for_elog(&ps) < 0)
		return;
	state = &ps->gpio_state;

	end = MIN(state->num_valid_wake_gpios, ARRAY_SIZE(state->wake_gpios));
	for (i = 0; i < end; i++)
		elog_add_event_wake(ELOG_WAKE_SOURCE_GPIO, state->wake_gpios[i]);
}
