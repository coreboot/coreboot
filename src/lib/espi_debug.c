/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <espi.h>
#include <stdint.h>

void espi_show_slave_general_configuration(uint32_t config)
{
	uint32_t io_mode;
	uint32_t op_freq;

	printk(BIOS_DEBUG, "eSPI Slave configuration:\n");

	if (config & ESPI_SLAVE_CRC_ENABLE)
		printk(BIOS_DEBUG, "    CRC checking enabled\n");

	if (config & ESPI_SLAVE_RESP_MOD_ENABLE)
		printk(BIOS_DEBUG, "    Response modifier enabled\n");

	if (config & ESPI_SLAVE_ALERT_MODE_PIN)
		printk(BIOS_DEBUG, "    Dedicated Alert# used to signal alert event\n");
	else
		printk(BIOS_DEBUG, "    IO bit1 pin used to signal alert event\n");

	io_mode = config & ESPI_SLAVE_IO_MODE_SEL_MASK;
	switch (io_mode) {
	case ESPI_SLAVE_IO_MODE_SEL_SINGLE:
		printk(BIOS_DEBUG, "    eSPI single IO mode selected\n");
		break;
	case ESPI_SLAVE_IO_MODE_SEL_DUAL:
		printk(BIOS_DEBUG, "    eSPI dual IO mode selected\n");
		break;
	case ESPI_SLAVE_IO_MODE_SEL_QUAD:
		printk(BIOS_DEBUG, "    eSPI quad IO mode selected\n");
		break;
	default:
		printk(BIOS_DEBUG, "    Error: Invalid eSPI IO mode selected\n");
	}

	io_mode = config & ESPI_SLAVE_IO_MODE_SUPP_MASK;
	switch (io_mode) {
	case ESPI_SLAVE_IO_MODE_SUPP_SINGLE_QUAD:
		printk(BIOS_DEBUG, "    eSPI quad and single IO modes supported\n");
		break;
	case ESPI_SLAVE_IO_MODE_SUPP_SINGLE_DUAL:
		printk(BIOS_DEBUG, "    eSPI dual and single IO modes supported\n");
		break;
	case ESPI_SLAVE_IO_MODE_SUPP_SINGLE_DUAL_QUAD:
		printk(BIOS_DEBUG, "    eSPI quad, dual and single IO modes supported\n");
		break;
	default:
		printk(BIOS_DEBUG, "    Only eSPI single IO mode supported\n");
	}

	if (config & ESPI_SLAVE_OPEN_DRAIN_ALERT_SEL)
		printk(BIOS_DEBUG, "    Alert# pin is open-drain\n");
	else
		printk(BIOS_DEBUG, "    Alert# pin is driven\n");

	op_freq = config & ESPI_SLAVE_OP_FREQ_SEL_MASK;
	switch (op_freq) {
	case ESPI_SLAVE_OP_FREQ_SEL_20_MHZ:
		printk(BIOS_DEBUG, "    eSPI 20MHz selected\n");
		break;
	case ESPI_SLAVE_OP_FREQ_SEL_25_MHZ:
		printk(BIOS_DEBUG, "    eSPI 25MHz selected\n");
		break;
	case ESPI_SLAVE_OP_FREQ_SEL_33_MHZ:
		printk(BIOS_DEBUG, "    eSPI 33MHz selected\n");
		break;
	case ESPI_SLAVE_OP_FREQ_SEL_50_MHZ:
		printk(BIOS_DEBUG, "    eSPI 50MHz selected\n");
		break;
	case ESPI_SLAVE_OP_FREQ_SEL_66_MHZ:
		printk(BIOS_DEBUG, "    eSPI 66MHz selected\n");
		break;
	default:
		printk(BIOS_DEBUG, "    Error: Invalid eSPI frequency\n");
	}

	if (config & ESPI_SLAVE_OPEN_DRAIN_ALERT_SUPP)
		printk(BIOS_DEBUG, "    Open-drain Alert# pin supported\n");

	op_freq = config & ESPI_SLAVE_OP_FREQ_SUPP_MASK;
	switch (op_freq) {
	case ESPI_SLAVE_OP_FREQ_SUPP_20_MHZ:
		printk(BIOS_DEBUG, "    eSPI up to 20MHz supported\n");
		break;
	case ESPI_SLAVE_OP_FREQ_SUPP_25_MHZ:
		printk(BIOS_DEBUG, "    eSPI up to 25MHz supported\n");
		break;
	case ESPI_SLAVE_OP_FREQ_SUPP_33_MHZ:
		printk(BIOS_DEBUG, "    eSPI up to 33MHz supported\n");
		break;
	case ESPI_SLAVE_OP_FREQ_SUPP_50_MHZ:
		printk(BIOS_DEBUG, "    eSPI up to 50MHz supported\n");
		break;
	case ESPI_SLAVE_OP_FREQ_SUPP_66_MHZ:
		printk(BIOS_DEBUG, "    eSPI up to 66MHz supported\n");
		break;
	default:
		printk(BIOS_DEBUG, "    Error: Invalid eSPI frequency\n");
	}

	printk(BIOS_DEBUG, "    Maximum Wait state: %d\n",
	       (config & ESPI_SLAVE_MAX_WAIT_MASK) >> ESPI_SLAVE_MAX_WAIT_SHIFT);

	if (config & ESPI_SLAVE_PERIPH_CH_SUPP)
		printk(BIOS_DEBUG, "    Peripheral Channel supported\n");
	if (config & ESPI_SLAVE_VW_CH_SUPP)
		printk(BIOS_DEBUG, "    Virtual Wire Channel supported\n");
	if (config & ESPI_SLAVE_OOB_CH_SUPP)
		printk(BIOS_DEBUG, "    OOB Channel supported\n");
	if (config & ESPI_SLAVE_FLASH_CH_SUPP)
		printk(BIOS_DEBUG, "    Flash Access Channel supported\n");
	printk(BIOS_DEBUG, "\n");
}

void espi_show_slave_peripheral_channel_configuration(uint32_t config)
{
	uint32_t request_size;
	uint32_t payload_size;

	printk(BIOS_DEBUG, "eSPI Slave Peripheral configuration:\n");

	printk(BIOS_DEBUG, "    Peripheral Channel Maximum Read Request Size: ");
	request_size = config & ESPI_SLAVE_PERIPH_MAX_READ_SIZE_MASK;
	switch (request_size) {
	case ESPI_SLAVE_PERIPH_MAX_READ_64B:
		printk(BIOS_DEBUG, "64 bytes\n");
		break;
	case ESPI_SLAVE_PERIPH_MAX_READ_128B:
		printk(BIOS_DEBUG, "128 bytes\n");
		break;
	case ESPI_SLAVE_PERIPH_MAX_READ_256B:
		printk(BIOS_DEBUG, "256 bytes\n");
		break;
	case ESPI_SLAVE_PERIPH_MAX_READ_512B:
		printk(BIOS_DEBUG, "512 bytes\n");
		break;
	case ESPI_SLAVE_PERIPH_MAX_READ_1024B:
		printk(BIOS_DEBUG, "1024 bytes\n");
		break;
	case ESPI_SLAVE_PERIPH_MAX_READ_2048B:
		printk(BIOS_DEBUG, "2048 bytes\n");
		break;
	case ESPI_SLAVE_PERIPH_MAX_READ_4096B:
		printk(BIOS_DEBUG, "4096 bytes\n");
		break;
	default:
		printk(BIOS_DEBUG, "Unknown\n");
	}

	printk(BIOS_DEBUG, "    Peripheral Channel Maximum Payload Size Selected: ");
	payload_size = config & ESPI_SLAVE_PERIPH_MAX_PAYLOAD_SIZE_SEL_MASK;
	switch (payload_size) {
	case ESPI_SLAVE_PERIPH_MAX_PAYLOAD_SIZE_SEL_64B:
		printk(BIOS_DEBUG, "64 bytes\n");
		break;
	case ESPI_SLAVE_PERIPH_MAX_PAYLOAD_SIZE_SEL_128B:
		printk(BIOS_DEBUG, "128 bytes\n");
		break;
	case ESPI_SLAVE_PERIPH_MAX_PAYLOAD_SIZE_SEL_256B:
		printk(BIOS_DEBUG, "256 bytes\n");
		break;
	default:
		printk(BIOS_DEBUG, "Unknown\n");
	}

	printk(BIOS_DEBUG, "    Peripheral Channel Maximum Payload Size Supported: ");
	payload_size = config & ESPI_SLAVE_PERIPH_MAX_PAYLOAD_SIZE_SUPP_MASK;
	switch (payload_size) {
	case ESPI_SLAVE_PERIPH_MAX_PAYLOAD_SIZE_SUPP_64B:
		printk(BIOS_DEBUG, "64 bytes\n");
		break;
	case ESPI_SLAVE_PERIPH_MAX_PAYLOAD_SIZE_SUPP_128B:
		printk(BIOS_DEBUG, "128 bytes\n");
		break;
	case ESPI_SLAVE_PERIPH_MAX_PAYLOAD_SIZE_SUPP_256B:
		printk(BIOS_DEBUG, "256 bytes\n");
		break;
	default:
		printk(BIOS_DEBUG, "Unknown\n");
	}

	printk(BIOS_DEBUG, "    Bus master: ");
	if (config & ESPI_SLAVE_PERIPH_BUS_MASTER_ENABLE)
		printk(BIOS_DEBUG, "enabled\n");
	else
		printk(BIOS_DEBUG, "disabled\n");

	printk(BIOS_DEBUG, "    Peripheral Channel: ");
	if (config & ESPI_SLAVE_CHANNEL_READY)
		printk(BIOS_DEBUG, "ready\n");
	else
		printk(BIOS_DEBUG, "not ready\n");

	printk(BIOS_DEBUG, "    Peripheral Channel: ");
	if (config & ESPI_SLAVE_CHANNEL_ENABLE)
		printk(BIOS_DEBUG, "enabled\n");
	else
		printk(BIOS_DEBUG, "disabled\n");
}
