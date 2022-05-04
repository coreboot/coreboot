/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/chip.h>
#include <amdblocks/espi.h>
#include <amdblocks/lpc.h>
#include <arch/mmio.h>
#include <console/console.h>
#include <espi.h>
#include <soc/pci_devs.h>
#include <timer.h>
#include <types.h>

#include "espi_def.h"

static uintptr_t espi_bar;

void espi_update_static_bar(uintptr_t bar)
{
	espi_bar = bar;
}

__weak void mb_set_up_early_espi(void)
{
}

static uintptr_t espi_get_bar(void)
{
	if (ENV_X86 && !espi_bar)
		espi_update_static_bar(lpc_get_spibase() + ESPI_OFFSET_FROM_BAR);
	return espi_bar;
}

static uint32_t espi_read32(unsigned int reg)
{
	return read32((void *)(espi_get_bar() + reg));
}

static void espi_write32(unsigned int reg, uint32_t val)
{
	write32((void *)(espi_get_bar() + reg), val);
}

static uint16_t espi_read16(unsigned int reg)
{
	return read16((void *)(espi_get_bar() + reg));
}

static void espi_write16(unsigned int reg, uint16_t val)
{
	write16((void *)(espi_get_bar() + reg), val);
}

static uint8_t espi_read8(unsigned int reg)
{
	return read8((void *)(espi_get_bar() + reg));
}

static void espi_write8(unsigned int reg, uint8_t val)
{
	write8((void *)(espi_get_bar() + reg), val);
}

static inline uint32_t espi_decode_io_range_en_bit(unsigned int idx)
{
	return ESPI_DECODE_IO_RANGE_EN(idx);
}

static inline uint32_t espi_decode_mmio_range_en_bit(unsigned int idx)
{
	return ESPI_DECODE_MMIO_RANGE_EN(idx);
}

static inline unsigned int espi_io_range_base_reg(unsigned int idx)
{
	return ESPI_IO_RANGE_BASE_REG(ESPI_IO_BASE_REG0, idx);
}

static inline unsigned int espi_io_range_size_reg(unsigned int idx)
{
	return ESPI_IO_RANGE_SIZE_REG(ESPI_IO_SIZE0, idx);
}

static inline unsigned int espi_mmio_range_base_reg(unsigned int idx)
{
	return ESPI_MMIO_RANGE_BASE_REG(ESPI_MMIO_BASE_REG0, idx);
}

static inline unsigned int espi_mmio_range_size_reg(unsigned int idx)
{
	return ESPI_MMIO_RANGE_SIZE_REG(ESPI_MMIO_SIZE_REG0, idx);
}

static void espi_enable_decode(uint32_t decode_en)
{
	uint32_t val;

	val = espi_read32(ESPI_DECODE);
	val |= decode_en;
	espi_write32(ESPI_DECODE, val);
}

static bool espi_is_decode_enabled(uint32_t decode)
{
	uint32_t val;

	val = espi_read32(ESPI_DECODE);
	return !!(val & decode);
}

static int espi_find_io_window(uint16_t win_base)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_IO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(espi_decode_io_range_en_bit(i)))
			continue;

		if (espi_read16(espi_io_range_base_reg(i)) == win_base)
			return i;
	}

	return -1;
}

static int espi_get_unused_io_window(void)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_IO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(espi_decode_io_range_en_bit(i)))
			return i;
	}

	return -1;
}

static void espi_clear_decodes(void)
{
	unsigned int idx;

	/* First turn off all enable bits, then zero base, range, and size registers */
	if (CONFIG(SOC_AMD_COMMON_BLOCK_ESPI_RETAIN_PORT80_EN))
		espi_write16(ESPI_DECODE, (espi_read16(ESPI_DECODE) & ESPI_DECODE_IO_0x80_EN));
	else
		espi_write16(ESPI_DECODE, 0);

	for (idx = 0; idx < ESPI_GENERIC_IO_WIN_COUNT; idx++) {
		espi_write16(espi_io_range_base_reg(idx), 0);
		espi_write8(espi_io_range_size_reg(idx), 0);
	}
	for (idx = 0; idx < ESPI_GENERIC_MMIO_WIN_COUNT; idx++) {
		espi_write32(espi_mmio_range_base_reg(idx), 0);
		espi_write16(espi_mmio_range_size_reg(idx), 0);
	}
}

/*
 * Returns decode enable bits for standard IO port addresses. If port address is not supported
 * by standard decode or if the size of window is not 1, then it returns -1.
 */
static int espi_std_io_decode(uint16_t base, size_t size)
{
	if (size == 2 && base == 0x2e)
		return ESPI_DECODE_IO_0X2E_0X2F_EN;

	if (size != 1)
		return -1;

	switch (base) {
	case 0x80:
		return ESPI_DECODE_IO_0x80_EN;
	case 0x60:
	case 0x64:
		return ESPI_DECODE_IO_0X60_0X64_EN;
	case 0x2e:
	case 0x2f:
		return ESPI_DECODE_IO_0X2E_0X2F_EN;
	default:
		return -1;
	}
}

static size_t espi_get_io_window_size(int idx)
{
	return espi_read8(espi_io_range_size_reg(idx)) + 1;
}

static void espi_write_io_window(int idx, uint16_t base, size_t size)
{
	espi_write16(espi_io_range_base_reg(idx), base);
	espi_write8(espi_io_range_size_reg(idx), size - 1);
}

static enum cb_err espi_open_generic_io_window(uint16_t base, size_t size)
{
	size_t win_size;
	int idx;

	for (; size; size -= win_size, base += win_size) {
		win_size = MIN(size, ESPI_GENERIC_IO_MAX_WIN_SIZE);

		idx = espi_find_io_window(base);
		if (idx != -1) {
			size_t curr_size = espi_get_io_window_size(idx);

			if (curr_size > win_size) {
				printk(BIOS_INFO, "eSPI window already configured to be larger than requested! ");
				printk(BIOS_INFO, "Base: 0x%x, Requested size: 0x%zx, Actual size: 0x%zx\n",
				       base, win_size, curr_size);
			} else if (curr_size < win_size) {
				espi_write_io_window(idx, base, win_size);
				printk(BIOS_INFO, "eSPI window at base: 0x%x resized from 0x%zx to 0x%zx\n",
				       base, curr_size, win_size);
			}

			continue;
		}

		idx = espi_get_unused_io_window();
		if (idx == -1) {
			printk(BIOS_ERR, "Cannot open IO window base %x size %zx\n", base,
			       size);
			printk(BIOS_ERR, "No more available IO windows!\n");
			return CB_ERR;
		}

		espi_write_io_window(idx, base, win_size);
		espi_enable_decode(espi_decode_io_range_en_bit(idx));
	}

	return CB_SUCCESS;
}

enum cb_err espi_open_io_window(uint16_t base, size_t size)
{
	int std_io;

	std_io = espi_std_io_decode(base, size);
	if (std_io != -1) {
		espi_enable_decode(std_io);
		return CB_SUCCESS;
	} else {
		return espi_open_generic_io_window(base, size);
	}
}

static int espi_find_mmio_window(uint32_t win_base)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_MMIO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(espi_decode_mmio_range_en_bit(i)))
			continue;

		if (espi_read32(espi_mmio_range_base_reg(i)) == win_base)
			return i;
	}

	return -1;
}

static int espi_get_unused_mmio_window(void)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_MMIO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(espi_decode_mmio_range_en_bit(i)))
			return i;
	}

	return -1;

}

static size_t espi_get_mmio_window_size(int idx)
{
	return espi_read16(espi_mmio_range_size_reg(idx)) + 1;
}

static void espi_write_mmio_window(int idx, uint32_t base, size_t size)
{
	espi_write32(espi_mmio_range_base_reg(idx), base);
	espi_write16(espi_mmio_range_size_reg(idx), size - 1);
}

enum cb_err espi_open_mmio_window(uint32_t base, size_t size)
{
	size_t win_size;
	int idx;

	for (; size; size -= win_size, base += win_size) {
		win_size = MIN(size, ESPI_GENERIC_MMIO_MAX_WIN_SIZE);

		idx = espi_find_mmio_window(base);
		if (idx != -1) {
			size_t curr_size = espi_get_mmio_window_size(idx);

			if (curr_size > win_size) {
				printk(BIOS_INFO, "eSPI window already configured to be larger than requested! ");
				printk(BIOS_INFO, "Base: 0x%x, Requested size: 0x%zx, Actual size: 0x%zx\n",
				       base, win_size, curr_size);
			} else if (curr_size < win_size) {
				espi_write_mmio_window(idx, base, win_size);
				printk(BIOS_INFO, "eSPI window at base: 0x%x resized from 0x%zx to 0x%zx\n",
				       base, curr_size, win_size);
			}

			continue;
		}

		idx = espi_get_unused_mmio_window();
		if (idx == -1) {
			printk(BIOS_ERR, "Cannot open IO window base %x size %zx\n", base,
			       size);
			printk(BIOS_ERR, "No more available MMIO windows!\n");
			return CB_ERR;
		}

		espi_write_mmio_window(idx, base, win_size);
		espi_enable_decode(espi_decode_mmio_range_en_bit(idx));
	}

	return CB_SUCCESS;
}

static const struct espi_config *espi_get_config(void)
{
	const struct soc_amd_common_config *soc_cfg = soc_get_common_config();

	if (!soc_cfg)
		die("Common config structure is NULL!\n");

	return &soc_cfg->espi_config;
}

static enum cb_err espi_configure_decodes(const struct espi_config *cfg)
{
	int i;

	espi_enable_decode(cfg->std_io_decode_bitmap);

	for (i = 0; i < ESPI_GENERIC_IO_WIN_COUNT; i++) {
		if (cfg->generic_io_range[i].size == 0)
			continue;
		if (espi_open_generic_io_window(cfg->generic_io_range[i].base,
						cfg->generic_io_range[i].size) != CB_SUCCESS)
			return CB_ERR;
	}

	return CB_SUCCESS;
}

enum espi_cmd_type {
	CMD_TYPE_SET_CONFIGURATION = 0,
	CMD_TYPE_GET_CONFIGURATION = 1,
	CMD_TYPE_IN_BAND_RESET = 2,
	CMD_TYPE_PERIPHERAL = 4,
	CMD_TYPE_VW = 5,
	CMD_TYPE_OOB = 6,
	CMD_TYPE_FLASH = 7,
};

#define ESPI_CMD_TIMEOUT_US			100
#define ESPI_CH_READY_TIMEOUT_US		10000

union espi_txhdr0 {
	uint32_t val;
	struct  {
		uint32_t cmd_type:3;
		uint32_t cmd_sts:1;
		uint32_t slave_sel:2;
		uint32_t rsvd:2;
		uint32_t hdata0:8;
		uint32_t hdata1:8;
		uint32_t hdata2:8;
	};
} __packed;

union espi_txhdr1 {
	uint32_t val;
	struct {
		uint32_t hdata3:8;
		uint32_t hdata4:8;
		uint32_t hdata5:8;
		uint32_t hdata6:8;
	};
} __packed;

union espi_txhdr2 {
	uint32_t val;
	struct {
		uint32_t hdata7:8;
		uint32_t rsvd:24;
	};
} __packed;

union espi_txdata {
	uint32_t val;
	struct {
		uint32_t byte0:8;
		uint32_t byte1:8;
		uint32_t byte2:8;
		uint32_t byte3:8;
	};
} __packed;

struct espi_cmd {
	union espi_txhdr0 hdr0;
	union espi_txhdr1 hdr1;
	union espi_txhdr2 hdr2;
	union espi_txdata data;
	uint32_t expected_status_codes;
} __packed;

/* Wait up to ESPI_CMD_TIMEOUT_US for hardware to clear DNCMD_STATUS bit. */
static enum cb_err espi_wait_ready(void)
{
	struct stopwatch sw;
	union espi_txhdr0 hdr0;

	stopwatch_init_usecs_expire(&sw, ESPI_CMD_TIMEOUT_US);
	do {
		hdr0.val = espi_read32(ESPI_DN_TX_HDR0);
		if (!hdr0.cmd_sts)
			return CB_SUCCESS;
	} while (!stopwatch_expired(&sw));

	return CB_ERR;
}

/* Clear interrupt status register */
static void espi_clear_status(void)
{
	uint32_t status = espi_read32(ESPI_SLAVE0_INT_STS);
	if (status)
		espi_write32(ESPI_SLAVE0_INT_STS, status);
}

/*
 * Wait up to ESPI_CMD_TIMEOUT_US for interrupt status register to update after sending a
 * command.
 */
static enum cb_err espi_poll_status(uint32_t *status)
{
	struct stopwatch sw;

	stopwatch_init_usecs_expire(&sw, ESPI_CMD_TIMEOUT_US);
	do {
		*status = espi_read32(ESPI_SLAVE0_INT_STS);
		if (*status)
			return CB_SUCCESS;
	} while (!stopwatch_expired(&sw));

	printk(BIOS_ERR, "eSPI timed out waiting for status update.\n");

	return CB_ERR;
}

static void espi_show_failure(const struct espi_cmd *cmd, const char *str, uint32_t status)
{
	printk(BIOS_ERR, "eSPI cmd0-cmd2: %08x %08x %08x data: %08x.\n",
	       cmd->hdr0.val, cmd->hdr1.val, cmd->hdr2.val, cmd->data.val);
	printk(BIOS_ERR, "%s (Status = 0x%x)\n", str, status);
}

static enum cb_err espi_send_command(const struct espi_cmd *cmd)
{
	uint32_t status;

	if (CONFIG(ESPI_DEBUG))
		printk(BIOS_DEBUG, "eSPI cmd0-cmd2: %08x %08x %08x data: %08x.\n",
		       cmd->hdr0.val, cmd->hdr1.val, cmd->hdr2.val, cmd->data.val);

	if (espi_wait_ready() != CB_SUCCESS) {
		espi_show_failure(cmd, "Error: eSPI was not ready to accept a command", 0);
		return CB_ERR;
	}

	espi_clear_status();

	espi_write32(ESPI_DN_TX_HDR1, cmd->hdr1.val);
	espi_write32(ESPI_DN_TX_HDR2, cmd->hdr2.val);
	espi_write32(ESPI_DN_TX_DATA, cmd->data.val);

	/* Dword 0 must be last as this write triggers the transaction */
	espi_write32(ESPI_DN_TX_HDR0, cmd->hdr0.val);

	if (espi_wait_ready() != CB_SUCCESS) {
		espi_show_failure(cmd,
				  "Error: eSPI timed out waiting for command to complete", 0);
		return CB_ERR;
	}

	if (espi_poll_status(&status) != CB_SUCCESS) {
		espi_show_failure(cmd, "Error: eSPI poll status failed", 0);
		return CB_ERR;
	}

	/* If command did not complete downstream, return error. */
	if (!(status & ESPI_STATUS_DNCMD_COMPLETE)) {
		espi_show_failure(cmd, "Error: eSPI downstream command completion failure",
				  status);
		return CB_ERR;
	}

	if (status & ~(ESPI_STATUS_DNCMD_COMPLETE | cmd->expected_status_codes)) {
		espi_show_failure(cmd, "Error: unexpected eSPI status register bits set",
				  status);
		return CB_ERR;
	}

	espi_write32(ESPI_SLAVE0_INT_STS, status);

	return CB_SUCCESS;
}

static enum cb_err espi_send_reset(void)
{
	struct espi_cmd cmd = {
		.hdr0 = {
			.cmd_type = CMD_TYPE_IN_BAND_RESET,
			.cmd_sts = 1,
		},

		/*
		 * When performing an in-band reset the host controller and the
		 * peripheral can have mismatched IO configs.
		 *
		 * i.e., The eSPI peripheral can be in IO-4 mode while, the
		 * eSPI host will be in IO-1. This results in the peripheral
		 * getting invalid packets and thus not responding.
		 *
		 * If the peripheral is alerting when we perform an in-band
		 * reset, there is a race condition in espi_send_command.
		 * 1) espi_send_command clears the interrupt status.
		 * 2) eSPI host controller hardware notices the alert and sends
		 *    a GET_STATUS.
		 * 3) espi_send_command writes the in-band reset command.
		 * 4) eSPI hardware enqueues the in-band reset until GET_STATUS
		 *    is complete.
		 * 5) GET_STATUS fails with NO_RESPONSE and sets the interrupt
		 *    status.
		 * 6) eSPI hardware performs in-band reset.
		 * 7) espi_send_command checks the status and sees a
		 *    NO_RESPONSE bit.
		 *
		 * As a workaround we allow the NO_RESPONSE status code when
		 * we perform an in-band reset.
		 */
		.expected_status_codes = ESPI_STATUS_NO_RESPONSE,
	};

	return espi_send_command(&cmd);
}

static enum cb_err espi_send_pltrst(const struct espi_config *mb_cfg, bool assert)
{
	struct espi_cmd cmd = {
		.hdr0 = {
			.cmd_type = CMD_TYPE_VW,
			.cmd_sts = 1,
			.hdata0 = 0, /* 1 VW group */
		},
		.data = {
			.byte0 = ESPI_VW_INDEX_SYSTEM_EVENT_3,
			.byte1 = assert ? ESPI_VW_SIGNAL_LOW(ESPI_VW_PLTRST)
					: ESPI_VW_SIGNAL_HIGH(ESPI_VW_PLTRST),
		},
	};

	if (!mb_cfg->vw_ch_en)
		return CB_SUCCESS;

	return espi_send_command(&cmd);
}

/*
 * In case of get configuration command, hdata0 contains bits 15:8 of the slave register address
 * and hdata1 contains bits 7:0 of the slave register address.
 */
#define ESPI_CONFIGURATION_HDATA0(a)		(((a) >> 8) & 0xff)
#define ESPI_CONFIGURATION_HDATA1(a)		((a) & 0xff)

static enum cb_err espi_get_configuration(uint16_t slave_reg_addr, uint32_t *config)
{
	struct espi_cmd cmd = {
		.hdr0 = {
			.cmd_type = CMD_TYPE_GET_CONFIGURATION,
			.cmd_sts = 1,
			.hdata0 = ESPI_CONFIGURATION_HDATA0(slave_reg_addr),
			.hdata1 = ESPI_CONFIGURATION_HDATA1(slave_reg_addr),
		},
	};

	*config = 0;

	if (espi_send_command(&cmd) != CB_SUCCESS)
		return CB_ERR;

	*config = espi_read32(ESPI_DN_TX_HDR1);

	if (CONFIG(ESPI_DEBUG))
		printk(BIOS_DEBUG, "Get configuration for slave register(0x%x): 0x%x\n",
		       slave_reg_addr, *config);

	return CB_SUCCESS;
}

static enum cb_err espi_set_configuration(uint16_t slave_reg_addr, uint32_t config)
{
	struct espi_cmd cmd = {
		.hdr0 = {
			.cmd_type = CMD_TYPE_SET_CONFIGURATION,
			.cmd_sts = 1,
			.hdata0 = ESPI_CONFIGURATION_HDATA0(slave_reg_addr),
			.hdata1 = ESPI_CONFIGURATION_HDATA1(slave_reg_addr),
		},
		.hdr1 = {
			 .val = config,
		},
	};

	return espi_send_command(&cmd);
}

static enum cb_err espi_get_general_configuration(uint32_t *config)
{
	if (espi_get_configuration(ESPI_SLAVE_GENERAL_CFG, config) != CB_SUCCESS)
		return CB_ERR;

	espi_show_slave_general_configuration(*config);
	return CB_SUCCESS;
}

static void espi_set_io_mode_config(enum espi_io_mode mb_io_mode, uint32_t slave_caps,
				    uint32_t *slave_config, uint32_t *ctrlr_config)
{
	switch (mb_io_mode) {
	case ESPI_IO_MODE_QUAD:
		if (espi_slave_supports_quad_io(slave_caps)) {
			*slave_config |= ESPI_SLAVE_IO_MODE_SEL_QUAD;
			*ctrlr_config |= ESPI_IO_MODE_QUAD;
			break;
		}
		printk(BIOS_ERR, "eSPI Quad I/O not supported. Dropping to dual mode.\n");
		__fallthrough;
	case ESPI_IO_MODE_DUAL:
		if (espi_slave_supports_dual_io(slave_caps)) {
			*slave_config |= ESPI_SLAVE_IO_MODE_SEL_DUAL;
			*ctrlr_config |= ESPI_IO_MODE_DUAL;
			break;
		}
		printk(BIOS_ERR, "eSPI Dual I/O not supported. Dropping to single mode.\n");
		__fallthrough;
	case ESPI_IO_MODE_SINGLE:
		/* Single I/O mode is always supported. */
		*slave_config |= ESPI_SLAVE_IO_MODE_SEL_SINGLE;
		*ctrlr_config |= ESPI_IO_MODE_SINGLE;
		break;
	default:
		die("No supported eSPI I/O modes!\n");
	}
}

static void espi_set_op_freq_config(enum espi_op_freq mb_op_freq, uint32_t slave_caps,
				    uint32_t *slave_config, uint32_t *ctrlr_config)
{
	int slave_max_speed_mhz = espi_slave_max_speed_mhz_supported(slave_caps);

	switch (mb_op_freq) {
	case ESPI_OP_FREQ_66_MHZ:
		if (slave_max_speed_mhz >= 66) {
			*slave_config |= ESPI_SLAVE_OP_FREQ_SEL_66_MHZ;
			*ctrlr_config |= ESPI_OP_FREQ_66_MHZ;
			break;
		}
		printk(BIOS_ERR, "eSPI 66MHz not supported. Dropping to 33MHz.\n");
		__fallthrough;
	case ESPI_OP_FREQ_33_MHZ:
		if (slave_max_speed_mhz >= 33) {
			*slave_config |= ESPI_SLAVE_OP_FREQ_SEL_33_MHZ;
			*ctrlr_config |= ESPI_OP_FREQ_33_MHZ;
			break;
		}
		printk(BIOS_ERR, "eSPI 33MHz not supported. Dropping to 16MHz.\n");
		__fallthrough;
	case ESPI_OP_FREQ_16_MHZ:
		/*
		 * eSPI spec says the minimum frequency is 20MHz, but AMD datasheets support
		 * 16.7 Mhz.
		 */
		if (slave_max_speed_mhz > 0) {
			*slave_config |= ESPI_SLAVE_OP_FREQ_SEL_20_MHZ;
			*ctrlr_config |= ESPI_OP_FREQ_16_MHZ;
			break;
		}
		__fallthrough;
	default:
		die("No supported eSPI Operating Frequency!\n");
	}
}

static void espi_set_alert_pin_config(enum espi_alert_pin alert_pin, uint32_t slave_caps,
				    uint32_t *slave_config, uint32_t *ctrlr_config)
{
	switch (alert_pin) {
	case ESPI_ALERT_PIN_IN_BAND:
		*slave_config |= ESPI_SLAVE_ALERT_MODE_IO1;
		return;
	case ESPI_ALERT_PIN_PUSH_PULL:
		*slave_config |= ESPI_SLAVE_ALERT_MODE_PIN | ESPI_SLAVE_PUSH_PULL_ALERT_SEL;
		*ctrlr_config |= ESPI_ALERT_MODE;
		return;
	case ESPI_ALERT_PIN_OPEN_DRAIN:
		if (!(slave_caps & ESPI_SLAVE_OPEN_DRAIN_ALERT_SUPP))
			die("eSPI peripheral does not support open drain alert!");

		*slave_config |= ESPI_SLAVE_ALERT_MODE_PIN | ESPI_SLAVE_OPEN_DRAIN_ALERT_SEL;
		*ctrlr_config |= ESPI_ALERT_MODE;
		return;
	default:
		die("Unknown espi alert config: %u!\n", alert_pin);
	}
}

static enum cb_err espi_set_general_configuration(const struct espi_config *mb_cfg,
						  uint32_t slave_caps)
{
	uint32_t slave_config = 0;
	uint32_t ctrlr_config = 0;

	if (mb_cfg->crc_check_enable) {
		slave_config |= ESPI_SLAVE_CRC_ENABLE;
		ctrlr_config |= ESPI_CRC_CHECKING_EN;
	}

	espi_set_alert_pin_config(mb_cfg->alert_pin, slave_caps, &slave_config, &ctrlr_config);
	espi_set_io_mode_config(mb_cfg->io_mode, slave_caps, &slave_config, &ctrlr_config);
	espi_set_op_freq_config(mb_cfg->op_freq_mhz, slave_caps, &slave_config, &ctrlr_config);

	if (CONFIG(ESPI_DEBUG))
		printk(BIOS_INFO, "Setting general configuration: slave: 0x%x controller: 0x%x\n",
		       slave_config, ctrlr_config);

	espi_show_slave_general_configuration(slave_config);

	if (espi_set_configuration(ESPI_SLAVE_GENERAL_CFG, slave_config) != CB_SUCCESS)
		return CB_ERR;

	espi_write32(ESPI_SLAVE0_CONFIG, ctrlr_config);
	return CB_SUCCESS;
}

static enum cb_err espi_wait_channel_ready(uint16_t slave_reg_addr)
{
	struct stopwatch sw;
	uint32_t config;

	stopwatch_init_usecs_expire(&sw, ESPI_CH_READY_TIMEOUT_US);
	do {
		if (espi_get_configuration(slave_reg_addr, &config) != CB_SUCCESS)
			return CB_ERR;
		if (espi_slave_is_channel_ready(config))
			return CB_SUCCESS;
	} while (!stopwatch_expired(&sw));

	printk(BIOS_ERR, "Channel is not ready after %d usec (slave addr: 0x%x)\n",
	       ESPI_CH_READY_TIMEOUT_US, slave_reg_addr);
	return CB_ERR;

}

static void espi_enable_ctrlr_channel(uint32_t channel_en)
{
	uint32_t reg = espi_read32(ESPI_SLAVE0_CONFIG);

	reg |= channel_en;

	espi_write32(ESPI_SLAVE0_CONFIG, reg);
}

static enum cb_err espi_set_channel_configuration(uint32_t slave_config,
						  uint32_t slave_reg_addr,
						  uint32_t ctrlr_enable)
{
	if (espi_set_configuration(slave_reg_addr, slave_config) != CB_SUCCESS)
		return CB_ERR;

	if (!(slave_config & ESPI_SLAVE_CHANNEL_ENABLE))
		return CB_SUCCESS;

	if (espi_wait_channel_ready(slave_reg_addr) != CB_SUCCESS)
		return CB_ERR;

	espi_enable_ctrlr_channel(ctrlr_enable);
	return CB_SUCCESS;
}

static enum cb_err espi_setup_vw_channel(const struct espi_config *mb_cfg, uint32_t slave_caps)
{
	uint32_t slave_vw_caps;
	uint32_t ctrlr_vw_caps;
	uint32_t slave_vw_count_supp;
	uint32_t ctrlr_vw_count_supp;
	uint32_t use_vw_count;
	uint32_t slave_config;

	if (!mb_cfg->vw_ch_en)
		return CB_SUCCESS;

	if (!espi_slave_supports_vw_channel(slave_caps)) {
		printk(BIOS_ERR, "eSPI slave doesn't support VW channel!\n");
		return CB_ERR;
	}

	if (espi_get_configuration(ESPI_SLAVE_VW_CFG, &slave_vw_caps) != CB_SUCCESS)
		return CB_ERR;

	ctrlr_vw_caps = espi_read32(ESPI_MASTER_CAP);
	ctrlr_vw_count_supp = (ctrlr_vw_caps & ESPI_VW_MAX_SIZE_MASK) >> ESPI_VW_MAX_SIZE_SHIFT;

	slave_vw_count_supp = espi_slave_get_vw_count_supp(slave_vw_caps);
	use_vw_count = MIN(ctrlr_vw_count_supp, slave_vw_count_supp);

	slave_config = ESPI_SLAVE_CHANNEL_ENABLE | ESPI_SLAVE_VW_COUNT_SEL_VAL(use_vw_count);
	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_VW_CFG, ESPI_VW_CH_EN);
}

static enum cb_err espi_setup_periph_channel(const struct espi_config *mb_cfg,
					     uint32_t slave_caps)
{
	uint32_t slave_config;
	/* Peripheral channel requires BME bit to be set when enabling the channel. */
	const uint32_t slave_en_mask =
		ESPI_SLAVE_CHANNEL_ENABLE | ESPI_SLAVE_PERIPH_BUS_MASTER_ENABLE;

	if (espi_get_configuration(ESPI_SLAVE_PERIPH_CFG, &slave_config) != CB_SUCCESS)
		return CB_ERR;

	/*
	 * Peripheral channel is the only one which is enabled on reset. So, if the mainboard
	 * wants to disable it, set configuration to disable peripheral channel. It also
	 * requires that BME bit be cleared.
	 */
	if (mb_cfg->periph_ch_en) {
		if (!espi_slave_supports_periph_channel(slave_caps)) {
			printk(BIOS_ERR, "eSPI slave doesn't support periph channel!\n");
			return CB_ERR;
		}
		slave_config |= slave_en_mask;
	} else {
		slave_config &= ~slave_en_mask;
	}

	espi_show_slave_peripheral_channel_configuration(slave_config);

	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_PERIPH_CFG,
					      ESPI_PERIPH_CH_EN);
}

static enum cb_err espi_setup_oob_channel(const struct espi_config *mb_cfg, uint32_t slave_caps)
{
	uint32_t slave_config;

	if (!mb_cfg->oob_ch_en)
		return CB_SUCCESS;

	if (!espi_slave_supports_oob_channel(slave_caps)) {
		printk(BIOS_ERR, "eSPI slave doesn't support OOB channel!\n");
		return CB_ERR;
	}

	if (espi_get_configuration(ESPI_SLAVE_OOB_CFG, &slave_config) != CB_SUCCESS)
		return CB_ERR;

	slave_config |= ESPI_SLAVE_CHANNEL_ENABLE;

	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_OOB_CFG,
					      ESPI_OOB_CH_EN);
}

static enum cb_err espi_setup_flash_channel(const struct espi_config *mb_cfg,
					    uint32_t slave_caps)
{
	uint32_t slave_config;

	if (!mb_cfg->flash_ch_en)
		return CB_SUCCESS;

	if (!espi_slave_supports_flash_channel(slave_caps)) {
		printk(BIOS_ERR, "eSPI slave doesn't support flash channel!\n");
		return CB_ERR;
	}

	if (espi_get_configuration(ESPI_SLAVE_FLASH_CFG, &slave_config) != CB_SUCCESS)
		return CB_ERR;

	slave_config |= ESPI_SLAVE_CHANNEL_ENABLE;

	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_FLASH_CFG,
					      ESPI_FLASH_CH_EN);
}

static void espi_set_initial_config(const struct espi_config *mb_cfg)
{
	uint32_t espi_initial_mode = ESPI_OP_FREQ_16_MHZ | ESPI_IO_MODE_SINGLE;

	switch (mb_cfg->alert_pin) {
	case ESPI_ALERT_PIN_IN_BAND:
		break;
	case ESPI_ALERT_PIN_PUSH_PULL:
	case ESPI_ALERT_PIN_OPEN_DRAIN:
		espi_initial_mode |= ESPI_ALERT_MODE;
		break;
	default:
		die("Unknown espi alert config: %u!\n", mb_cfg->alert_pin);
	}

	espi_write32(ESPI_SLAVE0_CONFIG, espi_initial_mode);
}

static void espi_setup_subtractive_decode(const struct espi_config *mb_cfg)
{
	uint32_t global_ctrl_reg;
	global_ctrl_reg = espi_read32(ESPI_GLOBAL_CONTROL_1);

	if (mb_cfg->subtractive_decode) {
		global_ctrl_reg &= ~ESPI_SUB_DECODE_SLV_MASK;
		global_ctrl_reg |= ESPI_SUB_DECODE_EN;

	} else {
		global_ctrl_reg &= ~ESPI_SUB_DECODE_EN;
	}
	espi_write32(ESPI_GLOBAL_CONTROL_1, global_ctrl_reg);
}

enum cb_err espi_setup(void)
{
	uint32_t slave_caps, ctrl;
	const struct espi_config *cfg = espi_get_config();

	printk(BIOS_SPEW, "Initializing ESPI.\n");

	espi_write32(ESPI_GLOBAL_CONTROL_0, ESPI_AL_STOP_EN);
	espi_write32(ESPI_GLOBAL_CONTROL_1, ESPI_RGCMD_INT(23) | ESPI_ERR_INT_SMI);
	espi_write32(ESPI_SLAVE0_INT_EN, 0);
	espi_clear_status();
	espi_clear_decodes();

	/*
	 * Boot sequence: Step 1
	 * Set correct initial configuration to talk to the slave:
	 * Set clock frequency to 16.7MHz and single IO mode.
	 */
	espi_set_initial_config(cfg);

	/*
	 * Boot sequence: Step 2
	 * Send in-band reset
	 * The resets affects both host and slave devices, so set initial config again.
	 */
	if (espi_send_reset() != CB_SUCCESS) {
		printk(BIOS_ERR, "In-band reset failed!\n");
		return CB_ERR;
	}
	espi_set_initial_config(cfg);

	/*
	 * Boot sequence: Step 3
	 * Get configuration of slave device.
	 */
	if (espi_get_general_configuration(&slave_caps) != CB_SUCCESS) {
		printk(BIOS_ERR, "Slave GET_CONFIGURATION failed!\n");
		return CB_ERR;
	}

	/*
	 * Boot sequence:
	 * Step 4: Write slave device general config
	 * Step 5: Set host slave config
	 */
	if (espi_set_general_configuration(cfg, slave_caps) != CB_SUCCESS) {
		printk(BIOS_ERR, "Slave SET_CONFIGURATION failed!\n");
		return CB_ERR;
	}

	/*
	 * Setup polarity before enabling the VW channel so any interrupts
	 * received will have the correct polarity.
	 */
	espi_write32(ESPI_RXVW_POLARITY, cfg->vw_irq_polarity);

	/*
	 * Boot Sequences: Steps 6 - 9
	 * Channel setup
	 */
	/* Set up VW first so we can deassert PLTRST#. */
	if (espi_setup_vw_channel(cfg, slave_caps) != CB_SUCCESS) {
		printk(BIOS_ERR, "Setup VW channel failed!\n");
		return CB_ERR;
	}

	/* Assert PLTRST# if VW channel is enabled by mainboard. */
	if (espi_send_pltrst(cfg, true) != CB_SUCCESS) {
		printk(BIOS_ERR, "PLTRST# assertion failed!\n");
		return CB_ERR;
	}

	/* De-assert PLTRST# if VW channel is enabled by mainboard. */
	if (espi_send_pltrst(cfg, false) != CB_SUCCESS) {
		printk(BIOS_ERR, "PLTRST# deassertion failed!\n");
		return CB_ERR;
	}

	if (espi_setup_periph_channel(cfg, slave_caps) != CB_SUCCESS) {
		printk(BIOS_ERR, "Setup Periph channel failed!\n");
		return CB_ERR;
	}

	if (espi_setup_oob_channel(cfg, slave_caps) != CB_SUCCESS) {
		printk(BIOS_ERR, "Setup OOB channel failed!\n");
		return CB_ERR;
	}

	if (espi_setup_flash_channel(cfg, slave_caps) != CB_SUCCESS) {
		printk(BIOS_ERR, "Setup Flash channel failed!\n");
		return CB_ERR;
	}

	if (espi_configure_decodes(cfg) != CB_SUCCESS) {
		printk(BIOS_ERR, "Configuring decodes failed!\n");
		return CB_ERR;
	}

	/* Enable subtractive decode if configured */
	espi_setup_subtractive_decode(cfg);

	ctrl = espi_read32(ESPI_GLOBAL_CONTROL_1);
	ctrl |= ESPI_BUS_MASTER_EN;

	if (CONFIG(SOC_AMD_COMMON_BLOCK_HAS_ESPI_ALERT_ENABLE))
		ctrl |= ESPI_ALERT_ENABLE;

	espi_write32(ESPI_GLOBAL_CONTROL_1, ctrl);

	printk(BIOS_SPEW, "Finished initializing ESPI.\n");

	return CB_SUCCESS;
}

/* Setup eSPI with any mainboard specific initialization. */
void configure_espi_with_mb_hook(void)
{
	mb_set_up_early_espi();
	espi_setup();
}
