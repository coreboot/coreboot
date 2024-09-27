/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <intelblocks/gpio.h>
#include <intelblocks/pcr.h>
#include <soc/gpio_snr.h>
#include <soc/pci_devs.h>

#define SNR_GPIO_GROUP(GPIO_PAD)        (SNR_GPIO_PAD(GPIO_PAD) / GPIO_MAX_NUM_PER_GROUP)
#define SNR_GPIO_WITHIN_GROUP(GPIO_PAD) (SNR_GPIO_PAD(GPIO_PAD) % GPIO_MAX_NUM_PER_GROUP)

static void gpio_unlock_snr_pad_group(const struct pad_community *comm,
				      const size_t group_index, const uint32_t bit_mask)
{
	struct pcr_sbi_msg msg = {
		.pid = comm->port,
		.offset = comm->pad_cfg_lock_offset + group_index * 2 * sizeof(uint32_t),
		.opcode = GPIO_LOCK_UNLOCK,
		.is_posted = false,
		.fast_byte_enable = 0xf,
		.bar = 0,
		.fid = 0,
	};
	uint32_t data;
	uint8_t response;
	int status;

	if (CONFIG(DEBUG_GPIO))
		printk(BIOS_DEBUG,
		       "Unlock GPIO community %s, group %zu at offset 0x%04x with mask 0x%08x\n",
		       comm->name, group_index, msg.offset, bit_mask);

	data = pcr_read32(msg.pid, msg.offset) & (~bit_mask);
	status = pcr_execute_sideband_msg(PCH_DEV_P2SB, &msg, &data, &response);
	if (status || response)
		die("Failed to unlock GPIO PAD, response = %d\n", response);

	msg.offset += sizeof(uint32_t);
	data = pcr_read32(msg.pid, msg.offset) & (~bit_mask);
	status = pcr_execute_sideband_msg(PCH_DEV_P2SB, &msg, &data, &response);
	if (status || response)
		die("Failed to unlock GPIO PAD Tx state, response = %d\n", response);
}

static uint8_t gpio_get_snr_pad_own(gpio_t pad)
{
	size_t num_communities;
	const struct pad_community *comm =
		soc_gpio_get_community(&num_communities) + SNR_GPIO_COMMUNITY(pad);
	uint16_t pad_own_offset = comm->pad_own_reg_0;
	uint32_t pad_own;

	/**
	 * 4 bits for each pad.
	 */
	pad_own_offset += (pad - comm->first_pad) / 8 * sizeof(uint32_t);
	pad_own = pcr_read32(comm->port, pad_own_offset);

	if (CONFIG(DEBUG_GPIO))
		printk(BIOS_DEBUG,
		       "Pad 0x%08x ownership at 0x%04x value = 0x%08x, mask = 0x%08x\n", pad,
		       pad_own_offset, pad_own, 0x03 << ((pad - comm->first_pad) % 8 * 4));

	return (pad_own >> ((pad - comm->first_pad) % 8 * sizeof(uint32_t))) & 0x03;
}

static uint8_t gpio_get_snr_pad_hostsw_own(gpio_t pad)
{
	size_t num_communities;
	const struct pad_community *comm =
		soc_gpio_get_community(&num_communities) + SNR_GPIO_COMMUNITY(pad);
	uint32_t host_sw_own;

	host_sw_own = pcr_read32(comm->port, comm->host_own_reg_0 + SNR_GPIO_GROUP(pad));

	if (CONFIG(DEBUG_GPIO))
		printk(BIOS_DEBUG,
		       "Pad 0x%08x host software ownership at 0x%04x value = 0x%08x, mask = 0x%08x\n",
		       pad, comm->host_own_reg_0 + SNR_GPIO_GROUP(pad), host_sw_own,
		       0x01 << SNR_GPIO_WITHIN_GROUP(pad));

	return (host_sw_own >> SNR_GPIO_WITHIN_GROUP(pad)) & 0x01;
}

static void gpio_clear_snr_pad_group_interrupt_status(const struct pad_community *comm,
						      const size_t group_index,
						      const uint32_t bit_mask)
{
	if (CONFIG(DEBUG_GPIO))
		printk(BIOS_DEBUG,
		       "Clear GPIO community %s, group %zu interrupt status with mask 0x%08x\n",
		       comm->name, group_index, bit_mask);

	const uint16_t offset = group_index * sizeof(uint32_t);
	if (CONFIG(DEBUG_GPIO))
		printk(BIOS_DEBUG, "GPI interrupt status at offset 0x%04x\n",
		       comm->gpi_int_sts_reg_0 + offset);
	pcr_write32(comm->port, comm->gpi_int_sts_reg_0 + offset, bit_mask);

	if (CONFIG(DEBUG_GPIO))
		printk(BIOS_DEBUG, "GPI GPE status at offset 0x%04x\n",
		       comm->gpi_gpe_sts_reg_0 + offset);
	pcr_write32(comm->port, comm->gpi_gpe_sts_reg_0 + offset, bit_mask);

	if (CONFIG(DEBUG_GPIO))
		printk(BIOS_DEBUG, "SMI status at offset 0x%04x\n",
		       comm->gpi_smi_sts_reg_0 + offset);
	pcr_write32(comm->port, comm->gpi_smi_sts_reg_0 + offset, bit_mask);

	if (CONFIG(DEBUG_GPIO))
		printk(BIOS_DEBUG, "NMI status at offset 0x%04x\n",
		       comm->gpi_nmi_sts_reg_0 + offset);
	pcr_write32(comm->port, comm->gpi_nmi_sts_reg_0 + offset, bit_mask);
}

void gpio_configure_snr_pads(struct snr_pad_config *gpio, size_t num)
{
	size_t i, j, num_communities, comm_index, group_index;
	const struct pad_community *comm = soc_gpio_get_community(&num_communities);
	uint16_t config_offset;
	uint32_t bit_mask, pad_conf;

	if (!gpio || num == 0)
		return;

	/**
	 * Unlock pad configuration and pad tx configuration at group level.
	 */
	for (i = 0; i < num; i++) {
		comm_index = SNR_GPIO_COMMUNITY(gpio[i].cfg.pad);
		if (comm_index >= num_communities) {
			printk(BIOS_WARNING, "Invalid GPIO community index %zu\n", comm_index);
			continue;
		}

		group_index = SNR_GPIO_GROUP(gpio[i].cfg.pad);
		if (group_index >= comm->num_groups) {
			printk(BIOS_WARNING, "Invalid GPIO group index %zu\n", group_index);
			continue;
		}

		bit_mask = 1 << (SNR_GPIO_PAD(gpio[i].cfg.pad) % GPIO_MAX_NUM_PER_GROUP);

		gpio_unlock_snr_pad_group(&comm[comm_index], group_index, bit_mask);
	}

	/**
	 * @note Common block GPIO code doesn't support leaving some bits as default value, thus we
	 * should read the original value and set concerned bits before calling common code.
	 */
	for (i = 0; i < num; i++) {
		if (gpio_get_snr_pad_own(gpio[i].cfg.pad) != GPIO_PAD_OWN_HOST) {
			printk(BIOS_ERR, "GPIO pad 0x%08x is not owned by host\n",
			       gpio[i].cfg.pad);
			continue;
		}

		comm_index = SNR_GPIO_COMMUNITY(gpio[i].cfg.pad);
		config_offset = comm[comm_index].pad_cfg_base +
				(gpio[i].cfg.pad - comm[comm_index].first_pad) *
					GPIO_NUM_PAD_CFG_REGS * sizeof(uint32_t);
		for (j = 0; j < GPIO_NUM_PAD_CFG_REGS; j++) {
			pad_conf = pcr_read32(comm[comm_index].port,
					      config_offset + j * sizeof(uint32_t));
			pad_conf &= ~gpio[i].pad_config_mask[j];
			gpio[i].cfg.pad_config[j] =
				(gpio[i].cfg.pad_config[j] & gpio[i].pad_config_mask[j]) |
				pad_conf;
		}

		/**
		 * Set the fourth bit for host software ownership.
		 */
		switch (gpio[i].hostsw_own) {
		case GPIO_HOSTSW_OWN_ACPI:
			gpio[i].cfg.pad_config[1] |= PAD_CFG_OWN_GPIO(ACPI);
			break;
		case GPIO_HOSTSW_OWN_DRIVER:
			gpio[i].cfg.pad_config[1] |= PAD_CFG_OWN_GPIO(DRIVER);
			break;
		default:
			gpio[i].cfg.pad_config[1] |=
				gpio_get_snr_pad_hostsw_own(gpio[i].cfg.pad) << 4;
			break;
		}

		gpio_configure_pads(&gpio[i].cfg, 1);
	}

	/**
	 * Clear pad interrupt status at group level.
	 */
	for (i = 0; i < num; i++) {
		comm_index = SNR_GPIO_COMMUNITY(gpio[i].cfg.pad);
		group_index = SNR_GPIO_GROUP(gpio[i].cfg.pad);
		bit_mask = 1 << (SNR_GPIO_PAD(gpio[i].cfg.pad) % GPIO_MAX_NUM_PER_GROUP);
		gpio_clear_snr_pad_group_interrupt_status(&comm[comm_index], group_index,
							  bit_mask);
	}
}
