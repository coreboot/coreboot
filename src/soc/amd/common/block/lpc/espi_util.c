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

static uintptr_t espi_bar;

void espi_update_static_bar(uintptr_t bar)
{
	espi_bar = bar;
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
		if (!espi_is_decode_enabled(ESPI_DECODE_IO_RANGE_EN(i)))
			continue;

		if (espi_read16(ESPI_IO_RANGE_BASE(i)) == win_base)
			return i;
	}

	return -1;
}

static int espi_get_unused_io_window(void)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_IO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(ESPI_DECODE_IO_RANGE_EN(i)))
			return i;
	}

	return -1;
}

void espi_clear_decodes(void)
{
	unsigned int idx;

	/* First turn off all enable bits, then zero base, range, and size registers */
	/*
	 * There is currently a bug where the SMU will lock up at times if the port80h enable
	 * bit is cleared.  See b/183974365
	 */
	espi_write16(ESPI_DECODE, (espi_read16(ESPI_DECODE) & ESPI_DECODE_IO_0x80_EN));

	for (idx = 0; idx < ESPI_GENERIC_IO_WIN_COUNT; idx++) {
		espi_write16(ESPI_IO_RANGE_BASE(idx), 0);
		espi_write8(ESPI_IO_RANGE_SIZE(idx), 0);
	}
	for (idx = 0; idx < ESPI_GENERIC_MMIO_WIN_COUNT; idx++) {
		espi_write32(ESPI_MMIO_RANGE_BASE(idx), 0);
		espi_write16(ESPI_MMIO_RANGE_SIZE(idx), 0);
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
	return espi_read8(ESPI_IO_RANGE_SIZE(idx)) + 1;
}

static void espi_write_io_window(int idx, uint16_t base, size_t size)
{
	espi_write16(ESPI_IO_RANGE_BASE(idx), base);
	espi_write8(ESPI_IO_RANGE_SIZE(idx), size - 1);
}

static int espi_open_generic_io_window(uint16_t base, size_t size)
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
			printk(BIOS_ERR, "ERROR: No more available IO windows!\n");
			return -1;
		}

		espi_write_io_window(idx, base, win_size);
		espi_enable_decode(ESPI_DECODE_IO_RANGE_EN(idx));
	}

	return 0;
}

int espi_open_io_window(uint16_t base, size_t size)
{
	int std_io;

	std_io = espi_std_io_decode(base, size);
	if (std_io != -1) {
		espi_enable_decode(std_io);
		return 0;
	} else {
		return espi_open_generic_io_window(base, size);
	}
}

static int espi_find_mmio_window(uint32_t win_base)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_MMIO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(ESPI_DECODE_MMIO_RANGE_EN(i)))
			continue;

		if (espi_read32(ESPI_MMIO_RANGE_BASE(i)) == win_base)
			return i;
	}

	return -1;
}

static int espi_get_unused_mmio_window(void)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_MMIO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(ESPI_DECODE_MMIO_RANGE_EN(i)))
			return i;
	}

	return -1;

}

static size_t espi_get_mmio_window_size(int idx)
{
	return espi_read16(ESPI_MMIO_RANGE_SIZE(idx)) + 1;
}

static void espi_write_mmio_window(int idx, uint32_t base, size_t size)
{
	espi_write32(ESPI_MMIO_RANGE_BASE(idx), base);
	espi_write16(ESPI_MMIO_RANGE_SIZE(idx), size - 1);
}

int espi_open_mmio_window(uint32_t base, size_t size)
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
			printk(BIOS_ERR, "ERROR: No more available MMIO windows!\n");
			return -1;
		}

		espi_write_mmio_window(idx, base, win_size);
		espi_enable_decode(ESPI_DECODE_MMIO_RANGE_EN(idx));
	}

	return 0;
}

static const struct espi_config *espi_get_config(void)
{
	const struct soc_amd_common_config *soc_cfg = soc_get_common_config();

	if (!soc_cfg)
		die("Common config structure is NULL!\n");

	return &soc_cfg->espi_config;
}

static int espi_configure_decodes(const struct espi_config *cfg)
{
	int i, ret;

	espi_enable_decode(cfg->std_io_decode_bitmap);

	for (i = 0; i < ESPI_GENERIC_IO_WIN_COUNT; i++) {
		if (cfg->generic_io_range[i].size == 0)
			continue;
		ret = espi_open_generic_io_window(cfg->generic_io_range[i].base,
						  cfg->generic_io_range[i].size);
		if (ret)
			return ret;
	}

	return 0;
}

#define ESPI_DN_TX_HDR0			0x00
enum espi_cmd_type {
	CMD_TYPE_SET_CONFIGURATION = 0,
	CMD_TYPE_GET_CONFIGURATION = 1,
	CMD_TYPE_IN_BAND_RESET = 2,
	CMD_TYPE_PERIPHERAL = 4,
	CMD_TYPE_VW = 5,
	CMD_TYPE_OOB = 6,
	CMD_TYPE_FLASH = 7,
};

#define ESPI_DN_TX_HDR1				0x04
#define ESPI_DN_TX_HDR2				0x08
#define ESPI_DN_TX_DATA				0x0c

#define ESPI_MASTER_CAP				0x2c
#define  ESPI_VW_MAX_SIZE_SHIFT			13
#define  ESPI_VW_MAX_SIZE_MASK			(0x3f << ESPI_VW_MAX_SIZE_SHIFT)

#define ESPI_GLOBAL_CONTROL_0			0x30
#define  ESPI_WAIT_CNT_SHIFT			24
#define  ESPI_WAIT_CNT_MASK			(0x3F << ESPI_WAIT_CNT_SHIFT)
#define  ESPI_WDG_CNT_SHIFT			8
#define  ESPI_WDG_CNT_MASK			(0xFFFF << ESPI_WDG_CNT_SHIFT)
#define  ESPI_AL_IDLE_TIMER_SHIFT		4
#define  ESPI_AL_IDLE_TIMER_MASK		(0x7 << ESPI_AL_IDLE_TIMER_SHIFT)
#define  ESPI_AL_STOP_EN			(1 << 3)
#define  ESPI_PR_CLKGAT_EN			(1 << 2)
#define  ESPI_WAIT_CHKEN			(1 << 1)
#define  ESPI_WDG_EN				(1 << 0)

#define ESPI_GLOBAL_CONTROL_1			0x34
#define  ESPI_RGCMD_INT_MAP_SHIFT		13
#define  ESPI_RGCMD_INT_MAP_MASK		(0x1F << ESPI_RGCMD_INT_MAP_SHIFT)
#define    ESPI_RGCMD_INT(irq)			((irq) << ESPI_RGCMD_INT_MAP_SHIFT)
#define    ESPI_RGCMD_INT_SMI			(0x1F << ESPI_RGCMD_INT_MAP_SHIFT)
#define  ESPI_ERR_INT_MAP_SHIFT			8
#define  ESPI_ERR_INT_MAP_MASK			(0x1F << ESPI_ERR_INT_MAP_SHIFT)
#define    ESPI_ERR_INT(irq)			((irq) << ESPI_ERR_INT_MAP_SHIFT)
#define    ESPI_ERR_INT_SMI			(0x1F << ESPI_ERR_INT_MAP_SHIFT)
#define  ESPI_SUB_DECODE_SLV_SHIFT		3
#define  ESPI_SUB_DECODE_SLV_MASK		(0x3 << ESPI_SUB_DECODE_SLV_SHIFT)
#define  ESPI_SUB_DECODE_EN			(1 << 2)
#define  ESPI_BUS_MASTER_EN			(1 << 1)
#define  ESPI_SW_RST				(1 << 0)

#define ESPI_SLAVE0_INT_EN			0x6C
#define ESPI_SLAVE0_INT_STS			0x70
#define  ESPI_STATUS_DNCMD_COMPLETE		(1 << 28)
#define  ESPI_STATUS_NON_FATAL_ERROR		(1 << 6)
#define  ESPI_STATUS_FATAL_ERROR		(1 << 5)
#define  ESPI_STATUS_NO_RESPONSE		(1 << 4)
#define  ESPI_STATUS_CRC_ERR			(1 << 2)
#define  ESPI_STATUS_WAIT_TIMEOUT		(1 << 1)
#define  ESPI_STATUS_BUS_ERROR			(1 << 0)

#define ESPI_RXVW_POLARITY			0xac

#define ESPI_CMD_TIMEOUT_US			100
#define ESPI_CH_READY_TIMEOUT_US		1000

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
} __packed;

/* Wait up to ESPI_CMD_TIMEOUT_US for hardware to clear DNCMD_STATUS bit. */
static int espi_wait_ready(void)
{
	struct stopwatch sw;
	union espi_txhdr0 hdr0;

	stopwatch_init_usecs_expire(&sw, ESPI_CMD_TIMEOUT_US);
	do {
		hdr0.val = espi_read32(ESPI_DN_TX_HDR0);
		if (!hdr0.cmd_sts)
			return 0;
	} while (!stopwatch_expired(&sw));

	return -1;
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
static int espi_poll_status(uint32_t *status)
{
	struct stopwatch sw;

	stopwatch_init_usecs_expire(&sw, ESPI_CMD_TIMEOUT_US);
	do {
		*status = espi_read32(ESPI_SLAVE0_INT_STS);
		if (*status)
			return 0;
	} while (!stopwatch_expired(&sw));

	printk(BIOS_ERR, "Error: eSPI timed out waiting for status update.\n");

	return -1;
}

static void espi_show_failure(const struct espi_cmd *cmd, const char *str, uint32_t status)
{
	printk(BIOS_ERR, "eSPI cmd0-cmd2: %08x %08x %08x data: %08x.\n",
	       cmd->hdr0.val, cmd->hdr1.val, cmd->hdr2.val, cmd->data.val);
	printk(BIOS_ERR, "%s (Status = 0x%x)\n", str, status);
}

static int espi_send_command(const struct espi_cmd *cmd)
{
	uint32_t status;

	if (CONFIG(ESPI_DEBUG))
		printk(BIOS_ERR, "eSPI cmd0-cmd2: %08x %08x %08x data: %08x.\n",
		       cmd->hdr0.val, cmd->hdr1.val, cmd->hdr2.val, cmd->data.val);

	if (espi_wait_ready() == -1) {
		espi_show_failure(cmd, "Error: eSPI was not ready to accept a command", 0);
		return -1;
	}

	espi_clear_status();

	espi_write32(ESPI_DN_TX_HDR1, cmd->hdr1.val);
	espi_write32(ESPI_DN_TX_HDR2, cmd->hdr2.val);
	espi_write32(ESPI_DN_TX_DATA, cmd->data.val);

	/* Dword 0 must be last as this write triggers the transaction */
	espi_write32(ESPI_DN_TX_HDR0, cmd->hdr0.val);

	if (espi_wait_ready() == -1) {
		espi_show_failure(cmd,
				  "Error: eSPI timed out waiting for command to complete", 0);
		return -1;
	}

	if (espi_poll_status(&status) == -1) {
		espi_show_failure(cmd, "Error: eSPI poll status failed", 0);
		return -1;
	}

	/* If command did not complete downstream, return error. */
	if (!(status & ESPI_STATUS_DNCMD_COMPLETE)) {
		espi_show_failure(cmd, "Error: eSPI downstream command completion failure",
				  status);
		return -1;
	}

	if (status & ~ESPI_STATUS_DNCMD_COMPLETE) {
		espi_show_failure(cmd, "Error: unexpected eSPI status register bits set",
				  status);
		return -1;
	}

	espi_write32(ESPI_SLAVE0_INT_STS, ESPI_STATUS_DNCMD_COMPLETE);

	return 0;
}

static int espi_send_reset(void)
{
	struct espi_cmd cmd = {
		.hdr0 = {
			.cmd_type = CMD_TYPE_IN_BAND_RESET,
			.cmd_sts = 1,
		},
	};

	return espi_send_command(&cmd);
}

static int espi_send_pltrst_deassert(const struct espi_config *mb_cfg)
{
	struct espi_cmd cmd = {
		.hdr0 = {
			.cmd_type = CMD_TYPE_VW,
			.cmd_sts = 1,
			.hdata0 = 0, /* 1 VW group */
		},
		.data = {
			.byte0 = ESPI_VW_INDEX_SYSTEM_EVENT_3,
			.byte1 = ESPI_VW_SIGNAL_HIGH(ESPI_VW_PLTRST),
		},
	};

	if (!mb_cfg->vw_ch_en)
		return 0;

	return espi_send_command(&cmd);
}

/*
 * In case of get configuration command, hdata0 contains bits 15:8 of the slave register address
 * and hdata1 contains bits 7:0 of the slave register address.
 */
#define ESPI_CONFIGURATION_HDATA0(a)		(((a) >> 8) & 0xff)
#define ESPI_CONFIGURATION_HDATA1(a)		((a) & 0xff)

static int espi_get_configuration(uint16_t slave_reg_addr, uint32_t *config)
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

	if (espi_send_command(&cmd))
		return -1;

	*config = espi_read32(ESPI_DN_TX_HDR1);

	if (CONFIG(ESPI_DEBUG))
		printk(BIOS_DEBUG, "Get configuration for slave register(0x%x): 0x%x\n",
		       slave_reg_addr, *config);

	return 0;
}

static int espi_set_configuration(uint16_t slave_reg_addr, uint32_t config)
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

static int espi_get_general_configuration(uint32_t *config)
{
	int ret = espi_get_configuration(ESPI_SLAVE_GENERAL_CFG, config);
	if (ret == -1)
		return -1;

	espi_show_slave_general_configuration(*config);
	return 0;
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
		printk(BIOS_ERR, "Error: eSPI Quad I/O not supported. Dropping to dual mode.\n");
		/* Intentional fall-through */
	case ESPI_IO_MODE_DUAL:
		if (espi_slave_supports_dual_io(slave_caps)) {
			*slave_config |= ESPI_SLAVE_IO_MODE_SEL_DUAL;
			*ctrlr_config |= ESPI_IO_MODE_DUAL;
			break;
		}
		printk(BIOS_ERR,
		       "Error: eSPI Dual I/O not supported. Dropping to single mode.\n");
		/* Intentional fall-through */
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
		printk(BIOS_ERR, "Error: eSPI 66MHz not supported. Dropping to 33MHz.\n");
		/* Intentional fall-through */
	case ESPI_OP_FREQ_33_MHZ:
		if (slave_max_speed_mhz >= 33) {
			*slave_config |= ESPI_SLAVE_OP_FREQ_SEL_33_MHZ;
			*ctrlr_config |= ESPI_OP_FREQ_33_MHZ;
			break;
		}
		printk(BIOS_ERR, "Error: eSPI 33MHz not supported. Dropping to 16MHz.\n");
		/* Intentional fall-through */
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
		/* Intentional fall-through */
	default:
		die("No supported eSPI Operating Frequency!\n");
	}
}

static int espi_set_general_configuration(const struct espi_config *mb_cfg, uint32_t slave_caps)
{
	uint32_t slave_config = 0;
	uint32_t ctrlr_config = 0;

	if (mb_cfg->crc_check_enable) {
		slave_config |= ESPI_SLAVE_CRC_ENABLE;
		ctrlr_config |= ESPI_CRC_CHECKING_EN;
	}

	if (mb_cfg->dedicated_alert_pin) {
		slave_config |= ESPI_SLAVE_ALERT_MODE_PIN;
		ctrlr_config |= ESPI_ALERT_MODE;
	}

	espi_set_io_mode_config(mb_cfg->io_mode, slave_caps, &slave_config, &ctrlr_config);
	espi_set_op_freq_config(mb_cfg->op_freq_mhz, slave_caps, &slave_config, &ctrlr_config);

	if (CONFIG(ESPI_DEBUG))
		printk(BIOS_INFO, "Setting general configuration: slave: 0x%x controller: 0x%x\n",
		       slave_config, ctrlr_config);

	if (espi_set_configuration(ESPI_SLAVE_GENERAL_CFG, slave_config) == -1)
		return -1;

	espi_write32(ESPI_SLAVE0_CONFIG, ctrlr_config);
	return 0;
}

static int espi_wait_channel_ready(uint16_t slave_reg_addr)
{
	struct stopwatch sw;
	uint32_t config;

	stopwatch_init_usecs_expire(&sw, ESPI_CH_READY_TIMEOUT_US);
	do {
		espi_get_configuration(slave_reg_addr, &config);
		if (espi_slave_is_channel_ready(config))
			return 0;
	} while (!stopwatch_expired(&sw));

	printk(BIOS_ERR, "Error: Channel is not ready after %d usec (slave addr: 0x%x)\n",
	       ESPI_CH_READY_TIMEOUT_US, slave_reg_addr);
	return -1;

}

static void espi_enable_ctrlr_channel(uint32_t channel_en)
{
	uint32_t reg = espi_read32(ESPI_SLAVE0_CONFIG);

	reg |= channel_en;

	espi_write32(ESPI_SLAVE0_CONFIG, reg);
}

static int espi_set_channel_configuration(uint32_t slave_config, uint32_t slave_reg_addr,
					  uint32_t ctrlr_enable)
{
	if (espi_set_configuration(slave_reg_addr, slave_config) == -1)
		return -1;

	if (!(slave_config & ESPI_SLAVE_CHANNEL_ENABLE))
		return 0;

	if (espi_wait_channel_ready(slave_reg_addr) == -1)
		return -1;

	espi_enable_ctrlr_channel(ctrlr_enable);
	return 0;
}

static int espi_setup_vw_channel(const struct espi_config *mb_cfg, uint32_t slave_caps)
{
	uint32_t slave_vw_caps;
	uint32_t ctrlr_vw_caps;
	uint32_t slave_vw_count_supp;
	uint32_t ctrlr_vw_count_supp;
	uint32_t use_vw_count;
	uint32_t slave_config;

	if (!mb_cfg->vw_ch_en)
		return 0;

	if (!espi_slave_supports_vw_channel(slave_caps)) {
		printk(BIOS_ERR, "Error: eSPI slave doesn't support VW channel!\n");
		return -1;
	}

	if (espi_get_configuration(ESPI_SLAVE_VW_CFG, &slave_vw_caps) == -1)
		return -1;

	ctrlr_vw_caps = espi_read32(ESPI_MASTER_CAP);
	ctrlr_vw_count_supp = (ctrlr_vw_caps & ESPI_VW_MAX_SIZE_MASK) >> ESPI_VW_MAX_SIZE_SHIFT;

	slave_vw_count_supp = espi_slave_get_vw_count_supp(slave_vw_caps);
	use_vw_count = MIN(ctrlr_vw_count_supp, slave_vw_count_supp);

	slave_config = ESPI_SLAVE_CHANNEL_ENABLE | ESPI_SLAVE_VW_COUNT_SEL_VAL(use_vw_count);
	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_VW_CFG, ESPI_VW_CH_EN);
}

static int espi_setup_periph_channel(const struct espi_config *mb_cfg, uint32_t slave_caps)
{
	uint32_t slave_config;
	/* Peripheral channel requires BME bit to be set when enabling the channel. */
	const uint32_t slave_en_mask = ESPI_SLAVE_CHANNEL_READY |
					ESPI_SLAVE_PERIPH_BUS_MASTER_ENABLE;

	if (espi_get_configuration(ESPI_SLAVE_PERIPH_CFG, &slave_config) == -1)
		return -1;

	/*
	 * Peripheral channel is the only one which is enabled on reset. So, if the mainboard
	 * wants to disable it, set configuration to disable peripheral channel. It also
	 * requires that BME bit be cleared.
	 */
	if (mb_cfg->periph_ch_en) {
		if (!espi_slave_supports_periph_channel(slave_caps)) {
			printk(BIOS_ERR, "Error: eSPI slave doesn't support periph channel!\n");
			return -1;
		}
		slave_config |= slave_en_mask;
	} else {
		slave_config &= ~slave_en_mask;
	}

	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_PERIPH_CFG,
					      ESPI_PERIPH_CH_EN);
}

static int espi_setup_oob_channel(const struct espi_config *mb_cfg, uint32_t slave_caps)
{
	uint32_t slave_config;

	if (!mb_cfg->oob_ch_en)
		return 0;

	if (!espi_slave_supports_oob_channel(slave_caps)) {
		printk(BIOS_ERR, "Error: eSPI slave doesn't support OOB channel!\n");
		return -1;
	}

	if (espi_get_configuration(ESPI_SLAVE_OOB_CFG, &slave_config) == -1)
		return -1;

	slave_config |= ESPI_SLAVE_CHANNEL_ENABLE;

	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_OOB_CFG,
					      ESPI_OOB_CH_EN);
}

static int espi_setup_flash_channel(const struct espi_config *mb_cfg, uint32_t slave_caps)
{
	uint32_t slave_config;

	if (!mb_cfg->flash_ch_en)
		return 0;

	if (!espi_slave_supports_flash_channel(slave_caps)) {
		printk(BIOS_ERR, "Error: eSPI slave doesn't support flash channel!\n");
		return -1;
	}

	if (espi_get_configuration(ESPI_SLAVE_FLASH_CFG, &slave_config) == -1)
		return -1;

	slave_config |= ESPI_SLAVE_CHANNEL_ENABLE;

	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_FLASH_CFG,
					      ESPI_FLASH_CH_EN);
}

static void espi_set_initial_config(const struct espi_config *mb_cfg)
{
	uint32_t espi_initial_mode = ESPI_OP_FREQ_16_MHZ | ESPI_IO_MODE_SINGLE;

	if (mb_cfg->dedicated_alert_pin)
		espi_initial_mode |= ESPI_ALERT_MODE;

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

int espi_setup(void)
{
	uint32_t slave_caps;
	const struct espi_config *cfg = espi_get_config();

	espi_write32(ESPI_GLOBAL_CONTROL_0, ESPI_AL_STOP_EN);
	espi_write32(ESPI_GLOBAL_CONTROL_1, ESPI_RGCMD_INT(23) | ESPI_ERR_INT_SMI);
	espi_write32(ESPI_SLAVE0_INT_EN, 0);
	espi_clear_status();

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
	if (espi_send_reset() == -1) {
		printk(BIOS_ERR, "Error: In-band reset failed!\n");
		return -1;
	}
	espi_set_initial_config(cfg);

	/*
	 * Boot sequence: Step 3
	 * Get configuration of slave device.
	 */
	if (espi_get_general_configuration(&slave_caps) == -1) {
		printk(BIOS_ERR, "Error: Slave GET_CONFIGURATION failed!\n");
		return -1;
	}

	/*
	 * Boot sequence:
	 * Step 4: Write slave device general config
	 * Step 5: Set host slave config
	 */
	if (espi_set_general_configuration(cfg, slave_caps) == -1) {
		printk(BIOS_ERR, "Error: Slave SET_CONFIGURATION failed!\n");
		return -1;
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
	if (espi_setup_vw_channel(cfg, slave_caps) == -1) {
		printk(BIOS_ERR, "Error: Setup VW channel failed!\n");
		return -1;
	}

	/* De-assert PLTRST# if VW channel is enabled by mainboard. */
	if (espi_send_pltrst_deassert(cfg) == -1) {
		printk(BIOS_ERR, "Error: PLTRST deassertion failed!\n");
		return -1;
	}

	if (espi_setup_periph_channel(cfg, slave_caps) == -1) {
		printk(BIOS_ERR, "Error: Setup Periph channel failed!\n");
		return -1;
	}

	if (espi_setup_oob_channel(cfg, slave_caps) == -1) {
		printk(BIOS_ERR, "Error: Setup OOB channel failed!\n");
		return -1;
	}

	if (espi_setup_flash_channel(cfg, slave_caps) == -1) {
		printk(BIOS_ERR, "Error: Setup Flash channel failed!\n");
		return -1;
	}

	if (espi_configure_decodes(cfg) == -1) {
		printk(BIOS_ERR, "Error: Configuring decodes failed!\n");
		return -1;
	}

	/* Enable subtractive decode if configured */
	espi_setup_subtractive_decode(cfg);

	espi_write32(ESPI_GLOBAL_CONTROL_1,
		     espi_read32(ESPI_GLOBAL_CONTROL_1) | ESPI_BUS_MASTER_EN);

	return 0;
}
