/* SPDX-License-Identifier: GPL-2.0-or-later */

#define __SIMPLE_DEVICE__

#include <assert.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <fsp/debug.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/gpio.h>
#include <gpio.h>
#include <intelblocks/itss.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/pcr.h>
#include <security/vboot/vboot_common.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <stdlib.h>
#include <types.h>

#define GPIO_DWx_SIZE(x)	(sizeof(uint32_t) * (x))
#define PAD_CFG_OFFSET(x, dw_num)	((x) + GPIO_DWx_SIZE(dw_num))
#define PAD_CFG0_OFFSET(x)	PAD_CFG_OFFSET(x, 0)
#define PAD_CFG1_OFFSET(x)	PAD_CFG_OFFSET(x, 1)
#define PAD_CFG2_OFFSET(x)	PAD_CFG_OFFSET(x, 2)
#define PAD_CFG3_OFFSET(x)	PAD_CFG_OFFSET(x, 3)

#define PAD_DW0_MASK	(PAD_CFG0_TX_STATE |				\
	PAD_CFG0_TX_DISABLE | PAD_CFG0_RX_DISABLE | PAD_CFG0_MODE_MASK |\
	PAD_CFG0_ROUTE_MASK | PAD_CFG0_RXTENCFG_MASK |			\
	PAD_CFG0_RXINV_MASK | PAD_CFG0_PREGFRXSEL |			\
	PAD_CFG0_TRIG_MASK | PAD_CFG0_RXRAW1_MASK | PAD_CFG0_NAFVWE_ENABLE |\
	PAD_CFG0_RXPADSTSEL_MASK | PAD_CFG0_RESET_MASK)

#if CONFIG(SOC_INTEL_COMMON_BLOCK_GPIO_PADCFG_PADTOL)
#define PAD_DW1_MASK	(PAD_CFG1_IOSTERM_MASK |			\
			PAD_CFG1_PULL_MASK |				\
			PAD_CFG1_TOL_MASK |				\
			PAD_CFG1_IOSSTATE_MASK)
#else
#define PAD_DW1_MASK	(PAD_CFG1_IOSTERM_MASK |			\
			PAD_CFG1_PULL_MASK |				\
			PAD_CFG1_IOSSTATE_MASK)
#endif

#define PAD_DW2_MASK	(PAD_CFG2_DEBOUNCE_MASK)
#define PAD_DW3_MASK	(0)

#define MISCCFG_GPE0_DW0_SHIFT 8
#define MISCCFG_GPE0_DW0_MASK (0xf << MISCCFG_GPE0_DW0_SHIFT)
#define MISCCFG_GPE0_DW1_SHIFT 12
#define MISCCFG_GPE0_DW1_MASK (0xf << MISCCFG_GPE0_DW1_SHIFT)
#define MISCCFG_GPE0_DW2_SHIFT 16
#define MISCCFG_GPE0_DW2_MASK (0xf << MISCCFG_GPE0_DW2_SHIFT)

#define GPI_SMI_STS_OFFSET(comm, group) ((comm)->gpi_smi_sts_reg_0 +	\
				((group) * sizeof(uint32_t)))
#define GPI_SMI_EN_OFFSET(comm, group) ((comm)->gpi_smi_en_reg_0 +	\
				((group) * sizeof(uint32_t)))
#define GPI_NMI_STS_OFFSET(comm, group) ((comm)->gpi_nmi_sts_reg_0 +	\
				((group) * sizeof(uint32_t)))
#define GPI_NMI_EN_OFFSET(comm, group) ((comm)->gpi_nmi_en_reg_0 +	\
				((group) * sizeof(uint32_t)))
#define GPI_IS_OFFSET(comm, group) ((comm)->gpi_int_sts_reg_0 +	\
				((group) * sizeof(uint32_t)))
#define GPI_IE_OFFSET(comm, group) ((comm)->gpi_int_en_reg_0 +	\
				((group) * sizeof(uint32_t)))
#define GPI_GPE_STS_OFFSET(comm, group) ((comm)->gpi_gpe_sts_reg_0 +	\
				((group) * sizeof(uint32_t)))
#define GPI_GPE_EN_OFFSET(comm, group) ((comm)->gpi_gpe_en_reg_0 +	\
				((group) * sizeof(uint32_t)))

static inline size_t relative_pad_in_comm(const struct pad_community *comm,
						gpio_t gpio)
{
	return gpio - comm->first_pad;
}

/* find the group within the community that the pad is a part of */
static inline size_t gpio_group_index(const struct pad_community *comm,
					unsigned int relative_pad)
{
	size_t i;

	if (!comm->groups)
		die("Failed to get comm->groups.");

	/* find the base pad number for this pad's group */
	for (i = 0; i < comm->num_groups; i++) {
		if (relative_pad >= comm->groups[i].first_pad &&
			relative_pad < comm->groups[i].first_pad +
			comm->groups[i].size) {
			return i;
		}
	}
	printk(BIOS_ERR, "%s: pad %d is not found in community %s!\n",
			__func__, relative_pad, comm->name);
	BUG();

	return i;
}

static inline size_t gpio_group_index_scaled(const struct pad_community *comm,
					unsigned int relative_pad, size_t scale)
{
	return gpio_group_index(comm, relative_pad) * scale;
}

static inline size_t gpio_within_group(const struct pad_community *comm,
						unsigned int relative_pad)
{
	size_t i;

	i = gpio_group_index(comm, relative_pad);

	return relative_pad - comm->groups[i].first_pad;
}

static inline uint32_t gpio_bitmask_within_group(
					const struct pad_community *comm,
					unsigned int relative_pad)
{
	return 1U << gpio_within_group(comm, relative_pad);
}

static const struct pad_community *gpio_get_community(gpio_t pad)
{
	size_t gpio_communities;
	size_t i;
	const struct pad_community *comm;
	comm = soc_gpio_get_community(&gpio_communities);
	for (i = 0; i < gpio_communities; i++, comm++) {
		if (pad >= comm->first_pad && pad <= comm->last_pad)
			return comm;
	}
	printk(BIOS_ERR, "%s pad %d not found\n", __func__, pad);
	die("Invalid GPIO pad number\n");
	return NULL;
}

static void gpio_configure_owner(const struct pad_config *cfg,
				const struct pad_community *comm)
{
	uint32_t hostsw_own;
	uint16_t hostsw_own_offset;
	int pin;

	pin = relative_pad_in_comm(comm, cfg->pad);

	/* Based on the gpio pin number configure the corresponding bit in
	 * HOSTSW_OWN register. Value of 0x1 indicates GPIO Driver onwership.
	 */
	hostsw_own_offset = comm->host_own_reg_0;
	hostsw_own_offset += gpio_group_index_scaled(comm, pin,
		sizeof(uint32_t));

	hostsw_own = pcr_read32(comm->port, hostsw_own_offset);

	/* The 4th bit in pad_config 1 (RO) is used to indicate if the pad
	 * needs GPIO driver ownership.  Set the bit if GPIO driver ownership
	 * requested, otherwise clear the bit.
	 */
	if (cfg->pad_config[1] & PAD_CFG_OWN_GPIO_DRIVER)
		hostsw_own |= gpio_bitmask_within_group(comm, pin);
	else
		hostsw_own &= ~gpio_bitmask_within_group(comm, pin);

	pcr_write32(comm->port, hostsw_own_offset, hostsw_own);
}

static void gpi_enable_gpe(const struct pad_config *cfg,
			const struct pad_community *comm, int group, int pin)
{
	uint16_t en_reg;
	uint32_t en_value;

	/* Do not configure GPE_EN if PAD is not configured for SCI/wake */
	if (((cfg->pad_config[0]) & PAD_CFG0_ROUTE_SCI) != PAD_CFG0_ROUTE_SCI)
		return;

	/* Get comm offset and bit mask to be set as per pin */
	en_reg = GPI_GPE_EN_OFFSET(comm, group);
	en_value = gpio_bitmask_within_group(comm, pin);

	/* Set enable bits */
	pcr_or32(comm->port, en_reg, en_value);

	if (CONFIG(DEBUG_GPIO)) {
		printk(BIOS_DEBUG, "GPE_EN[0x%02x, %02zd]: Reg: 0x%x, Value = 0x%x \n",\
			comm->port, relative_pad_in_comm(comm, cfg->pad), en_reg,\
			pcr_read32(comm->port, en_reg));
	}
}

static void gpi_enable_smi(const struct pad_config *cfg,
			const struct pad_community *comm, int group, int pin)
{
	uint16_t sts_reg;
	uint16_t en_reg;
	uint32_t en_value;

	if (((cfg->pad_config[0]) & PAD_CFG0_ROUTE_SMI) != PAD_CFG0_ROUTE_SMI)
		return;

	sts_reg = GPI_SMI_STS_OFFSET(comm, group);
	en_reg = GPI_SMI_EN_OFFSET(comm, group);
	en_value = gpio_bitmask_within_group(comm, pin);

	/* Write back 1 to reset the sts bit */
	pcr_rmw32(comm->port, sts_reg, en_value, 0);

	/* Set enable bits */
	pcr_or32(comm->port, en_reg, en_value);
}

static void gpi_enable_nmi(const struct pad_config *cfg,
			const struct pad_community *comm, int group, int pin)
{
	uint16_t sts_reg;
	uint16_t en_reg;
	uint32_t en_value;

	if (((cfg->pad_config[0]) & PAD_CFG0_ROUTE_NMI) != PAD_CFG0_ROUTE_NMI)
		return;

	/* Do not configure NMI if the platform doesn't support it */
	if (!comm->gpi_nmi_sts_reg_0 || !comm->gpi_nmi_en_reg_0)
		return;

	sts_reg = GPI_NMI_STS_OFFSET(comm, group);
	en_reg = GPI_NMI_EN_OFFSET(comm, group);
	en_value = gpio_bitmask_within_group(comm, pin);

	/* Write back 1 to reset the sts bit */
	pcr_rmw32(comm->port, sts_reg, en_value, 0);

	/* Set enable bits */
	pcr_or32(comm->port, en_reg, en_value);
}

/* 120 GSIs is the default for IOxAPIC */
static uint32_t gpio_ioapic_irqs_used[120 / (sizeof(uint32_t) * BITS_PER_BYTE) + 1];
static void set_ioapic_used(uint32_t irq)
{
	size_t word_offset = irq / 32;
	size_t bit_offset = irq % 32;
	assert (word_offset < ARRAY_SIZE(gpio_ioapic_irqs_used));
	gpio_ioapic_irqs_used[word_offset] |= BIT(bit_offset);
}

bool gpio_routes_ioapic_irq(uint32_t irq)
{
	size_t word_offset = irq / 32;
	size_t bit_offset = irq % 32;
	assert (word_offset < ARRAY_SIZE(gpio_ioapic_irqs_used));
	return (gpio_ioapic_irqs_used[word_offset] & BIT(bit_offset)) != 0;
}

static void gpio_configure_itss(const struct pad_config *cfg, uint16_t port,
	uint16_t  pad_cfg_offset)
{
	/* No ITSS configuration in SMM. */
	if (ENV_SMM)
		return;

	int irq;

	/* Set up ITSS polarity if pad is routed to APIC.
	 *
	 * The ITSS takes only active high interrupt signals. Therefore,
	 * if the pad configuration indicates an inversion assume the
	 * intent is for the ITSS polarity. Before forwarding on the
	 * request to the APIC there's an inversion setting for how the
	 * signal is forwarded to the APIC. Honor the inversion setting
	 * in the GPIO pad configuration so that a hardware active low
	 * signal looks that way to the APIC (double inversion).
	 */
	if (!(cfg->pad_config[0] & PAD_CFG0_ROUTE_IOAPIC))
		return;

	irq = pcr_read32(port, PAD_CFG1_OFFSET(pad_cfg_offset));
	irq &= PAD_CFG1_IRQ_MASK;
	if (!irq) {
		printk(BIOS_ERR, "GPIO %u doesn't support APIC routing,\n",
			cfg->pad);
		return;
	}

	if (CONFIG(SOC_INTEL_COMMON_BLOCK_GPIO_ITSS_POL_CFG))
		itss_set_irq_polarity(irq, !!(cfg->pad_config[0] &
					      PAD_CFG0_RX_POL_INVERT));

	set_ioapic_used(irq);
}

/* Number of DWx config registers can be different for different SOCs */
static uint16_t pad_config_offset(const struct pad_community *comm, gpio_t pad)
{
	size_t offset;

	offset = relative_pad_in_comm(comm, pad);
	offset *= GPIO_DWx_SIZE(GPIO_NUM_PAD_CFG_REGS);
	return offset + comm->pad_cfg_base;
}

static uint32_t gpio_pad_reset_config_override(const struct pad_community *comm,
	uint32_t config_value)
{
	const struct reset_mapping *rst_map = comm->reset_map;
	int i;

	if (rst_map == NULL || comm->num_reset_vals == 0)
		return config_value;/* Logical reset values equal chipset
					values */
	for (i = 0; i < comm->num_reset_vals; i++, rst_map++) {
		if ((config_value & PAD_CFG0_RESET_MASK) == rst_map->logical) {
			config_value &= ~PAD_CFG0_RESET_MASK;
			config_value |= rst_map->chipset;
			return config_value;
		}
	}
	printk(BIOS_ERR, "%s: Logical to Chipset mapping not found\n",
			__func__);
	return config_value;
}

static const int mask[4] = {
	PAD_DW0_MASK, PAD_DW1_MASK, PAD_DW2_MASK, PAD_DW3_MASK
};

static void gpio_configure_pad(const struct pad_config *cfg)
{
	const struct pad_community *comm;
	uint16_t config_offset;
	uint32_t pad_conf, soc_pad_conf;
	int i, pin, group;

	if (!cfg) {
		printk(BIOS_ERR, "%s: cfg value is NULL\n", __func__);
		return;
	}

	comm = gpio_get_community(cfg->pad);
	if (!comm) {
		printk(BIOS_ERR, "%s: Could not find community for pad: 0x%x\n",
				__func__, cfg->pad);
		return;
	}

	config_offset = pad_config_offset(comm, cfg->pad);
	pin = relative_pad_in_comm(comm, cfg->pad);
	group = gpio_group_index(comm, pin);

	for (i = 0; i < GPIO_NUM_PAD_CFG_REGS; i++) {
		pad_conf = pcr_read32(comm->port,
			PAD_CFG_OFFSET(config_offset, i));

		soc_pad_conf = cfg->pad_config[i];
		if (i == 0)
			soc_pad_conf = gpio_pad_reset_config_override(comm,
				soc_pad_conf);
		soc_pad_conf &= mask[i];
		soc_pad_conf |= pad_conf & ~mask[i];

		/* Patch GPIO settings for SoC specifically */
		soc_pad_conf = soc_gpio_pad_config_fixup(cfg, i, soc_pad_conf);

		if (CONFIG(DEBUG_GPIO))
			printk(BIOS_DEBUG,
			"gpio_padcfg [0x%02x, %02d] DW%d [0x%08x : 0x%08x"
			" : 0x%08x]\n",
			comm->port, pin, i,
			pad_conf,/* old value */
			cfg->pad_config[i],/* value passed from gpio table */
			soc_pad_conf);/*new value*/
		pcr_write32(comm->port, PAD_CFG_OFFSET(config_offset, i),
			soc_pad_conf);
	}

	gpio_configure_itss(cfg, comm->port, config_offset);
	gpio_configure_owner(cfg, comm);
	gpi_enable_smi(cfg, comm, group, pin);
	gpi_enable_nmi(cfg, comm, group, pin);
	gpi_enable_gpe(cfg, comm, group, pin);
	if (cfg->lock_action)
		gpio_lock_pad(cfg->pad, cfg->lock_action);
}

void gpio_configure_pads(const struct pad_config *cfg, size_t num_pads)
{
	size_t i;

	for (i = 0; i < num_pads; i++)
		gpio_configure_pad(cfg + i);
}

/*
 * This functions checks to see if there is an override config present for the
 * provided pad_config. If no override config is present, then the input config
 * is returned. Else, it returns the override config.
 */
static const struct pad_config *gpio_get_config(const struct pad_config *c,
				const struct pad_config *override_cfg_table,
				size_t num)
{
	size_t i;

	if (override_cfg_table == NULL)
		return c;

	for (i = 0; i < num; i++) {
		if (c->pad == override_cfg_table[i].pad)
			return override_cfg_table + i;
	}

	return c;
}

void gpio_configure_pads_with_override(const struct pad_config *base_cfg,
					size_t base_num_pads,
					const struct pad_config *override_cfg,
					size_t override_num_pads)
{
	size_t i;
	const struct pad_config *c;

	for (i = 0; i < base_num_pads; i++) {
		c = gpio_get_config(base_cfg + i, override_cfg,
				override_num_pads);
		gpio_configure_pad(c);
	}
}

struct pad_config *new_padbased_table(void)
{
	struct pad_config *padbased_table;
	padbased_table = malloc(sizeof(struct pad_config) * TOTAL_PADS);
	memset(padbased_table, 0, sizeof(struct pad_config) * TOTAL_PADS);

	return padbased_table;
}

void gpio_padbased_override(struct pad_config *padbased_table,
				const struct pad_config *override_cfg,
				size_t override_num_pads)
{
	for (size_t i = 0; i < override_num_pads; i++) {
		/* Prevent overflow hack */
		ASSERT(override_cfg[i].pad < TOTAL_PADS);
		padbased_table[override_cfg[i].pad] = override_cfg[i];
	}
}

void gpio_configure_pads_with_padbased(struct pad_config *padbased_table)
{
	size_t i;
	const struct pad_config *cfg = padbased_table;
	for (i = 0; i < TOTAL_PADS; i++) {
		/* Consider unmapped pin as default setting, skip */
		if (cfg[i].pad == 0 && cfg[i].pad_config[0] == 0)
			continue;
		gpio_configure_pad(&cfg[i]);
	}
}

void *gpio_dwx_address(const gpio_t pad)
{
	/* Calculate Address of DW0 register for given GPIO
	 * pad - GPIO number
	 * returns - address of GPIO
	 */
	const struct pad_community *comm = gpio_get_community(pad);
	uint16_t config_offset;

	config_offset = pad_config_offset(comm, pad);
	return pcr_reg_address(comm->port, config_offset);
}

uint8_t gpio_get_pad_portid(const gpio_t pad)
{
	/* Get the port id of given pad
	 * pad - GPIO number
	 * returns - given pad port id
	 */
	const struct pad_community *comm = gpio_get_community(pad);
	return comm->port;
}

void gpio_input_pulldown(gpio_t gpio)
{
	struct pad_config cfg = PAD_CFG_GPI(gpio, DN_20K, DEEP);
	gpio_configure_pad(&cfg);
}

void gpio_input_pullup(gpio_t gpio)
{
	struct pad_config cfg = PAD_CFG_GPI(gpio, UP_20K, DEEP);
	gpio_configure_pad(&cfg);
}

void gpio_input(gpio_t gpio)
{
	struct pad_config cfg = PAD_CFG_GPI(gpio, NONE, DEEP);
	gpio_configure_pad(&cfg);
}

void gpio_output(gpio_t gpio, int value)
{
	struct pad_config cfg = PAD_CFG_GPO(gpio, value, DEEP);
	gpio_configure_pad(&cfg);
}

int gpio_get(gpio_t gpio_num)
{
	const struct pad_community *comm = gpio_get_community(gpio_num);
	uint16_t config_offset;
	uint32_t reg;

	config_offset = pad_config_offset(comm, gpio_num);
	reg = pcr_read32(comm->port, config_offset);

	return !!(reg & PAD_CFG0_RX_STATE);
}

int gpio_tx_get(gpio_t gpio_num)
{
	const struct pad_community *comm = gpio_get_community(gpio_num);
	uint16_t config_offset;
	uint32_t reg;

	config_offset = pad_config_offset(comm, gpio_num);
	reg = pcr_read32(comm->port, config_offset);

	return !!(reg & PAD_CFG0_TX_STATE);
}

static void
gpio_pad_config_lock_using_sbi(const struct gpio_lock_config *pad_info,
	uint8_t pid, uint16_t offset, const uint32_t bit_mask)
{
	int status;
	uint8_t response;
	uint32_t data;
	struct pcr_sbi_msg msg = {
		.pid = pid,
		.offset = offset,
		.opcode = GPIO_LOCK_UNLOCK,
		.is_posted = false,
		.fast_byte_enable = 0xf,
		.bar = 0,
		.fid = 0,
	};

	if (!(pad_info->lock_action & GPIO_LOCK_FULL)) {
		printk(BIOS_ERR, "%s: Error: no lock_action specified for pad %d!\n",
				__func__, pad_info->pad);
		return;
	}

	if ((pad_info->lock_action & GPIO_LOCK_CONFIG) == GPIO_LOCK_CONFIG) {
		if (CONFIG(DEBUG_GPIO))
			printk(BIOS_INFO, "%s: Locking pad %d configuration\n",
						__func__, pad_info->pad);
		data = pcr_read32(pid, offset) | bit_mask;
		status = pcr_execute_sideband_msg(PCH_DEV_P2SB, &msg, &data, &response);
		if (status || response)
			printk(BIOS_ERR, "Failed to lock GPIO PAD, response = %d\n", response);
	}

	if ((pad_info->lock_action & GPIO_LOCK_TX) == GPIO_LOCK_TX) {
		if (CONFIG(DEBUG_GPIO))
			printk(BIOS_INFO, "%s: Locking pad %d Tx state\n",
						__func__, pad_info->pad);
		offset += sizeof(uint32_t);
		data = pcr_read32(pid, offset) | bit_mask;
		msg.offset = offset;
		status = pcr_execute_sideband_msg(PCH_DEV_P2SB, &msg, &data, &response);
		if (status || response)
			printk(BIOS_ERR, "Failed to lock GPIO PAD Tx state, response = %d\n",
					response);
	}
}

int gpio_lock_pads(const struct gpio_lock_config *pad_list, const size_t count)
{
	const struct pad_community *comm;
	uint16_t offset;
	size_t rel_pad;
	gpio_t pad;

	if (!CONFIG(SOC_INTEL_COMMON_BLOCK_SMM_LOCK_GPIO_PADS))
		return -1;

	/*
	 * FSP-S will unlock all the GPIO pads and hide the P2SB device.  With
	 * the device hidden, we will not be able to send the sideband interface
	 * message to lock the GPIO configuration. Therefore, we need to unhide
	 * the P2SB device which can only be done in SMM requiring that this
	 * function is called from SMM.
	 */
	if (!ENV_SMM) {
		printk(BIOS_ERR, "%s: Error: must be called from SMM!\n", __func__);
		return -1;
	}

	if ((pad_list == NULL) || (count == 0)) {
		printk(BIOS_ERR, "%s: Error: pad_list null or count = 0!\n", __func__);
		return -1;
	}

	p2sb_unhide();

	for (int x = 0; x < count; x++) {
		pad = pad_list[x].pad;
		comm = gpio_get_community(pad);
		rel_pad = relative_pad_in_comm(comm, pad);
		offset = comm->pad_cfg_lock_offset;
		if (!offset) {
			printk(BIOS_ERR, "%s: Error: offset not defined for pad %d!\n",
					__func__, pad);
			continue;
		}
		/* PADCFGLOCK and PADCFGLOCKTX registers for each community are contiguous */
		offset += gpio_group_index_scaled(comm, rel_pad, 2 * sizeof(uint32_t));

		const uint32_t bit_mask = gpio_bitmask_within_group(comm, rel_pad);

		gpio_pad_config_lock_using_sbi(&pad_list[x], comm->port, offset, bit_mask);
	}

	p2sb_hide();
}

static void
gpio_pad_config_lock_using_pcr(const struct gpio_lock_config *pad_info,
	uint8_t pid, uint16_t offset, const uint32_t bit_mask)
{
	if ((pad_info->lock_action & GPIO_LOCK_CONFIG) == GPIO_LOCK_CONFIG) {
		if (CONFIG(DEBUG_GPIO))
			printk(BIOS_INFO, "%s: Locking pad %d configuration\n",
						__func__, pad_info->pad);
		pcr_or32(pid, offset, bit_mask);
	}

	if ((pad_info->lock_action & GPIO_LOCK_TX) == GPIO_LOCK_TX) {
		if (CONFIG(DEBUG_GPIO))
			printk(BIOS_INFO, "%s: Locking pad %d TX state\n",
				__func__, pad_info->pad);
		pcr_or32(pid, offset + sizeof(uint32_t), bit_mask);
	}
}

static int gpio_non_smm_lock_pad(const struct gpio_lock_config *pad_info)
{
	const struct pad_community *comm = gpio_get_community(pad_info->pad);
	uint16_t offset;
	size_t rel_pad;

	if (!pad_info) {
		printk(BIOS_ERR, "%s: Error: pad_info is null!\n", __func__);
		return -1;
	}

	if (cpu_soc_is_in_untrusted_mode()) {
		printk(BIOS_ERR, "%s: Error: IA Untrusted Mode enabled, can't lock pad!\n",
					__func__);
		return -1;
	}

	rel_pad = relative_pad_in_comm(comm, pad_info->pad);
	offset = comm->pad_cfg_lock_offset;
	if (!offset) {
		printk(BIOS_ERR, "%s: Error: offset not defined for pad %d!\n",
						__func__, pad_info->pad);
		return -1;
	}

	/* PADCFGLOCK and PADCFGLOCKTX registers for each community are contiguous */
	offset += gpio_group_index_scaled(comm, rel_pad, 2 * sizeof(uint32_t));
	const uint32_t bit_mask = gpio_bitmask_within_group(comm, rel_pad);

	if (CONFIG(SOC_INTEL_COMMON_BLOCK_GPIO_LOCK_USING_PCR)) {
		if (CONFIG(DEBUG_GPIO))
			printk(BIOS_INFO, "Locking pad configuration using PCR\n");
		gpio_pad_config_lock_using_pcr(pad_info, comm->port, offset, bit_mask);
	} else if (CONFIG(SOC_INTEL_COMMON_BLOCK_GPIO_LOCK_USING_SBI)) {
		if (CONFIG(DEBUG_GPIO))
			printk(BIOS_INFO, "Locking pad configuration using SBI\n");
		gpio_pad_config_lock_using_sbi(pad_info, comm->port, offset, bit_mask);
	} else {
		printk(BIOS_ERR, "%s: Error: No pad configuration lock method is selected!\n",
						__func__);
	}

	return 0;
}

int gpio_lock_pad(const gpio_t pad, enum gpio_lock_action lock_action)
{
	/* Skip locking GPIO PAD in early stages or in recovery mode */
	if (ENV_ROMSTAGE_OR_BEFORE || vboot_recovery_mode_enabled())
		return -1;

	const struct gpio_lock_config pads = {
		.pad = pad,
		.lock_action = lock_action
	};

	if (!ENV_SMM && !CONFIG(SOC_INTEL_COMMON_BLOCK_SMM_LOCK_GPIO_PADS))
		return gpio_non_smm_lock_pad(&pads);

	return gpio_lock_pads(&pads, 1);
}

void gpio_set(gpio_t gpio_num, int value)
{
	const struct pad_community *comm = gpio_get_community(gpio_num);
	uint16_t config_offset;

	config_offset = pad_config_offset(comm, gpio_num);
	pcr_rmw32(comm->port, config_offset,
		~PAD_CFG0_TX_STATE, (!!value & PAD_CFG0_TX_STATE));
}

uint16_t gpio_acpi_pin(gpio_t gpio_num)
{
	const struct pad_community *comm;
	size_t group, pin;

	if (CONFIG(SOC_INTEL_COMMON_BLOCK_GPIO_MULTI_ACPI_DEVICES))
		return relative_pad_in_comm(gpio_get_community(gpio_num),
					    gpio_num);

	comm = gpio_get_community(gpio_num);
	pin = relative_pad_in_comm(comm, gpio_num);
	group = gpio_group_index(comm, pin);

	/* If pad base is not set then use GPIO number as ACPI pin number. */
	if (comm->groups[group].acpi_pad_base == PAD_BASE_NONE)
		return gpio_num;

	/*
	 * If this group has a non-zero pad base then compute the ACPI pin
	 * number from the pad base and the relative pad in the group.
	 */
	return comm->groups[group].acpi_pad_base + gpio_within_group(comm, pin);
}

static void print_gpi_status(const struct gpi_status *sts)
{
	int i;
	int group;
	int index;
	int bit_set;
	int num_groups;
	int abs_bit;
	size_t gpio_communities;
	const struct pad_community *comm;

	comm = soc_gpio_get_community(&gpio_communities);
	for (i = 0; i < gpio_communities; i++) {
		num_groups = comm->num_gpi_regs;
		index = comm->gpi_status_offset;
		for (group = 0; group < num_groups; group++, index++) {
			for (bit_set = comm->max_pads_per_group - 1;
				bit_set >= 0; bit_set--) {
				if (!(sts->grp[index] & (1 << bit_set)))
					continue;

				abs_bit = bit_set;
				abs_bit += group * comm->max_pads_per_group;
				printk(BIOS_DEBUG, "%s %d\n", comm->name,
								abs_bit);
			}
		}
		comm++;
	}
}

void gpi_clear_get_smi_status(struct gpi_status *sts)
{
	int i;
	int group;
	int index;
	uint32_t sts_value;
	uint32_t en_value;
	size_t gpio_communities;
	int num_groups;
	const struct pad_community *comm;

	comm = soc_gpio_get_community(&gpio_communities);
	for (i = 0; i < gpio_communities; i++) {
		num_groups = comm->num_gpi_regs;
		index = comm->gpi_status_offset;
		for (group = 0; group < num_groups; group++, index++) {
			sts_value = pcr_read32(comm->port,
					GPI_SMI_STS_OFFSET(comm, group));
			en_value = pcr_read32(comm->port,
					GPI_SMI_EN_OFFSET(comm, group));
			sts->grp[index] = sts_value & en_value;
			/* Clear the set status bits. */
			pcr_write32(comm->port, GPI_SMI_STS_OFFSET(comm,
				group), sts->grp[index]);
		}
		comm++;
	}

	if (CONFIG(DEBUG_SMI))
		print_gpi_status(sts);

}

int gpi_status_get(const struct gpi_status *sts, gpio_t pad)
{
	uint8_t sts_index;
	const struct pad_community *comm = gpio_get_community(pad);

	pad = relative_pad_in_comm(comm, pad);
	sts_index = comm->gpi_status_offset;
	sts_index += gpio_group_index(comm, pad);

	return !!(sts->grp[sts_index] & gpio_bitmask_within_group(comm, pad));
}

static int gpio_route_pmc_gpio_gpe(int pmc_gpe_num)
{
	size_t num_routes;
	const struct pmc_to_gpio_route *routes;
	int i;

	routes = soc_pmc_gpio_routes(&num_routes);
	assert (routes != NULL);
	for (i = 0; i < num_routes; i++, routes++) {
		if (pmc_gpe_num == routes->pmc)
			return routes->gpio;
	}
	return -1;
}

void gpio_route_gpe(uint8_t gpe0b, uint8_t gpe0c, uint8_t gpe0d)
{
	int i;
	uint32_t misccfg_mask;
	uint32_t misccfg_value;
	int ret;
	size_t gpio_communities;
	const struct pad_community *comm;

	/* Get the group here for community specific MISCCFG register.
	 * If any of these returns -1 then there is some error in devicetree
	 * where the group is probably hardcoded and does not comply with the
	 * PMC group defines. So we return from here and MISCFG is set to
	 * default.
	 */
	ret = gpio_route_pmc_gpio_gpe(gpe0b);
	if (ret == -1)
		return;
	gpe0b = ret;

	ret = gpio_route_pmc_gpio_gpe(gpe0c);
	if (ret == -1)
		return;
	gpe0c = ret;

	ret = gpio_route_pmc_gpio_gpe(gpe0d);
	if (ret == -1)
		return;
	gpe0d = ret;

	misccfg_value = gpe0b << MISCCFG_GPE0_DW0_SHIFT;
	misccfg_value |= gpe0c << MISCCFG_GPE0_DW1_SHIFT;
	misccfg_value |= gpe0d << MISCCFG_GPE0_DW2_SHIFT;

	/* Program GPIO_MISCCFG */
	misccfg_mask = ~(MISCCFG_GPE0_DW2_MASK |
			MISCCFG_GPE0_DW1_MASK |
			MISCCFG_GPE0_DW0_MASK);

	if (CONFIG(DEBUG_GPIO))
		printk(BIOS_DEBUG, "misccfg_mask:%x misccfg_value:%x\n",
			misccfg_mask, misccfg_value);
	comm = soc_gpio_get_community(&gpio_communities);
	for (i = 0; i < gpio_communities; i++, comm++)
		pcr_rmw32(comm->port, GPIO_MISCCFG,
				misccfg_mask, misccfg_value);
}

const char *gpio_acpi_path(gpio_t gpio_num)
{
	const struct pad_community *comm = gpio_get_community(gpio_num);
	return comm->acpi_path;
}

uint32_t __weak soc_gpio_pad_config_fixup(const struct pad_config *cfg,
						int dw_reg, uint32_t reg_val)
{
	return reg_val;
}

void gpi_clear_int_cfg(void)
{
	int i, group, num_groups;
	uint32_t sts_value;
	size_t gpio_communities;
	const struct pad_community *comm;

	comm = soc_gpio_get_community(&gpio_communities);
	for (i = 0; i < gpio_communities; i++, comm++) {
		num_groups = comm->num_gpi_regs;
		for (group = 0; group < num_groups; group++) {
			/* Clear the enable register */
			pcr_write32(comm->port, GPI_IE_OFFSET(comm, group), 0);

			/* Read and clear the set status register bits*/
			sts_value = pcr_read32(comm->port,
					GPI_IS_OFFSET(comm, group));
			pcr_write32(comm->port,
					GPI_IS_OFFSET(comm, group), sts_value);
		}
	}
}

/* The function performs GPIO Power Management programming. */
void gpio_pm_configure(const uint8_t *misccfg_pm_values, size_t num)
{
	int i;
	size_t gpio_communities;
	const uint8_t misccfg_pm_mask = (uint8_t)~MISCCFG_GPIO_PM_CONFIG_BITS;
	const struct pad_community *comm;

	comm = soc_gpio_get_community(&gpio_communities);
	if (gpio_communities != num)
		die("Incorrect GPIO community count!\n");

	/* Program GPIO_MISCCFG */
	for (i = 0; i < num; i++, comm++)
		pcr_rmw8(comm->port, GPIO_MISCCFG,
				misccfg_pm_mask, misccfg_pm_values[i]);
}

size_t gpio_get_index_in_group(gpio_t pad)
{
	const struct pad_community *comm;
	size_t pin;

	comm = gpio_get_community(pad);
	pin = relative_pad_in_comm(comm, pad);
	return gpio_within_group(comm, pin);
}

static uint32_t *snapshot;

static void *allocate_snapshot_space(void)
{
	size_t gpio_communities, total = 0, i;
	const struct pad_community *comm;

	comm = soc_gpio_get_community(&gpio_communities);
	for (i = 0; i < gpio_communities; i++, comm++)
		total += comm->last_pad - comm->first_pad + 1;

	if (total == 0)
		return NULL;

	return malloc(total * GPIO_NUM_PAD_CFG_REGS * sizeof(uint32_t));
}

void gpio_snapshot(void)
{
	size_t gpio_communities, index, i, pad, reg;
	const struct pad_community *comm;
	uint16_t config_offset;

	if (snapshot == NULL) {
		snapshot = allocate_snapshot_space();
		if (snapshot == NULL)
			return;
	}

	comm = soc_gpio_get_community(&gpio_communities);
	for (i = 0, index = 0; i < gpio_communities; i++, comm++) {
		for (pad = comm->first_pad; pad <= comm->last_pad; pad++) {
			config_offset = pad_config_offset(comm, pad);
			for (reg = 0; reg < GPIO_NUM_PAD_CFG_REGS; reg++) {
				snapshot[index] = pcr_read32(comm->port,
							PAD_CFG_OFFSET(config_offset, reg));
				index++;
			}
		}
	}
}

size_t gpio_verify_snapshot(void)
{
	size_t gpio_communities, index, i, pad, reg;
	const struct pad_community *comm;
	uint32_t curr_val;
	uint16_t config_offset;
	size_t changes = 0;

	if (snapshot == NULL)
		return 0;

	comm = soc_gpio_get_community(&gpio_communities);
	for (i = 0, index = 0; i < gpio_communities; i++, comm++) {
		for (pad = comm->first_pad; pad <= comm->last_pad; pad++) {
			config_offset = pad_config_offset(comm, pad);
			for (reg = 0; reg < GPIO_NUM_PAD_CFG_REGS; reg++) {
				curr_val = pcr_read32(comm->port,
						      PAD_CFG_OFFSET(config_offset, reg));
				if (curr_val != snapshot[index]) {
					printk(BIOS_SPEW,
					       "%zd(DW%zd): Changed from 0x%x to 0x%x\n",
					       pad, reg, snapshot[index], curr_val);
					changes++;
				}
				index++;
			}
		}
	}

	return changes;
}

static void snapshot_cleanup(void *unused)
{
	free(snapshot);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, snapshot_cleanup, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, snapshot_cleanup, NULL);

bool gpio_get_vw_info(gpio_t pad, unsigned int *vw_index, unsigned int *vw_bit)
{
	const struct pad_community *comm;
	unsigned int offset = 0;
	size_t i;

	comm = gpio_get_community(pad);
	for (i = 0; i < comm->num_vw_entries; i++) {
		if (pad >= comm->vw_entries[i].first_pad && pad <= comm->vw_entries[i].last_pad)
			break;

		offset += 1 + comm->vw_entries[i].last_pad - comm->vw_entries[i].first_pad;
	}

	if (i == comm->num_vw_entries)
		return false;

	offset += pad - comm->vw_entries[i].first_pad;
	*vw_index = comm->vw_base + offset / 8;
	*vw_bit = offset % 8;

	return true;
}

unsigned int gpio_get_pad_cpu_portid(gpio_t pad)
{
	const struct pad_community *comm = gpio_get_community(pad);
	return comm->cpu_port;
}
