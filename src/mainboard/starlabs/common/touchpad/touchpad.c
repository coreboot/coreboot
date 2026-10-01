/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <option.h>
#include <string.h>

#include <common/touchpad.h>

static struct starlabs_touchpad_op_ctx starlabs_touchpad_cached_ctx;
static int starlabs_touchpad_cached_ctx_valid;

static void buf_set_le16(uint8_t *buf, size_t offset, uint16_t value)
{
	buf[offset] = value & 0xff;
	buf[offset + 1] = value >> 8;
}

static size_t starlabs_touchpad_encode_command(uint8_t *buf, uint8_t opcode,
					       uint8_t report_type,
					       uint8_t report_id)
{
	size_t length = 0;

	if (report_id < 0x0f) {
		buf[length++] = (report_type << 4) | report_id;
		buf[length++] = opcode;
	} else {
		buf[length++] = (report_type << 4) | 0x0f;
		buf[length++] = opcode;
		buf[length++] = report_id;
	}

	return length;
}

static int starlabs_touchpad_set_report(unsigned int bus, uint16_t cmd_reg,
					uint16_t data_reg, uint8_t report_id,
					const uint8_t *payload,
					size_t payload_len)
{
	uint8_t buf[32];
	size_t cmd_len = 0;
	size_t report_len = sizeof(uint16_t);

	if (report_id)
		report_len++;
	report_len += payload_len;

	buf_set_le16(buf, cmd_len, cmd_reg);
	cmd_len += sizeof(uint16_t);
	cmd_len += starlabs_touchpad_encode_command(buf + cmd_len,
						    I2C_HID_OPCODE_SET_REPORT,
						    I2C_HID_REPORT_TYPE_FEATURE,
						    report_id);
	buf_set_le16(buf, cmd_len, data_reg);
	cmd_len += sizeof(uint16_t);
	buf_set_le16(buf, cmd_len, report_len);
	cmd_len += sizeof(uint16_t);

	if (report_id)
		buf[cmd_len++] = report_id;

	if (payload_len > sizeof(buf) - cmd_len)
		return -1;

	memcpy(buf + cmd_len, payload, payload_len);
	cmd_len += payload_len;

	return i2c_write_raw(bus, STARLABS_TOUCHPAD_I2C_ADDR, buf, cmd_len);
}

static int starlabs_touchpad_get_report(unsigned int bus, uint16_t cmd_reg,
					uint16_t data_reg, uint8_t report_id,
					uint8_t *payload, size_t payload_len)
{
	uint8_t cmd_buf[8];
	uint8_t rx_buf[32];
	struct i2c_msg segs[2];
	size_t cmd_len = 0;
	size_t report_len;
	size_t response_len;
	int ret;

	buf_set_le16(cmd_buf, cmd_len, cmd_reg);
	cmd_len += sizeof(uint16_t);
	cmd_len += starlabs_touchpad_encode_command(cmd_buf + cmd_len,
						    I2C_HID_OPCODE_GET_REPORT,
						    I2C_HID_REPORT_TYPE_FEATURE,
						    report_id);
	buf_set_le16(cmd_buf, cmd_len, data_reg);
	cmd_len += sizeof(uint16_t);

	report_len = payload_len + (report_id ? 1 : 0);
	response_len = sizeof(uint16_t) + report_len;
	if (response_len > sizeof(rx_buf))
		return -1;

	segs[0].flags = 0;
	segs[0].slave = STARLABS_TOUCHPAD_I2C_ADDR;
	segs[0].buf = cmd_buf;
	segs[0].len = cmd_len;
	segs[1].flags = I2C_M_RD;
	segs[1].slave = STARLABS_TOUCHPAD_I2C_ADDR;
	segs[1].buf = rx_buf;
	segs[1].len = response_len;

	ret = i2c_transfer(bus, segs, ARRAY_SIZE(segs));
	if (ret != 0)
		return ret;

	response_len = rx_buf[0] | (rx_buf[1] << 8);
	if (response_len < sizeof(uint16_t) + report_len)
		return -1;

	if (report_id && rx_buf[2] != report_id)
		return -1;

	memcpy(payload, rx_buf + sizeof(uint16_t) + (report_id ? 1 : 0), payload_len);

	return 0;
}

static int starlabs_touchpad_reset(unsigned int bus, uint16_t cmd_reg)
{
	uint8_t buf[4];
	size_t cmd_len = 0;

	buf_set_le16(buf, cmd_len, cmd_reg);
	cmd_len += sizeof(uint16_t);
	cmd_len += starlabs_touchpad_encode_command(buf + cmd_len,
				    I2C_HID_OPCODE_RESET,
				    0, 0);

	return i2c_write_raw(bus, STARLABS_TOUCHPAD_I2C_ADDR, buf, cmd_len);
}

static int starlabs_touchpad_set_haptics(unsigned int bus, uint16_t cmd_reg,
				 uint16_t data_reg, uint8_t level)
{
#if CONFIG(STARLABS_TOUCHPAD_CST)
	uint8_t payload[STARLABS_TOUCHPAD_CST_REPORT_LEN] = { 0 };

	payload[0] = STARLABS_TOUCHPAD_CST_WRITE_HAPTICS;
	payload[1] = level;
	payload[2] = payload[1];

	return starlabs_touchpad_set_report(bus, cmd_reg, data_reg,
					    STARLABS_TOUCHPAD_CST_REPORT_ID,
					    payload, sizeof(payload));
#else
	return starlabs_touchpad_set_report(bus, cmd_reg, data_reg,
				    STARLABS_TOUCHPAD_HAPTICS_REPORT_ID,
				    &level, sizeof(level));
#endif
}

static int starlabs_touchpad_set_force(unsigned int bus, uint16_t cmd_reg,
				       uint16_t data_reg, uint16_t press,
				       uint16_t release)
{
#if CONFIG(STARLABS_TOUCHPAD_CST)
	uint8_t payload[STARLABS_TOUCHPAD_CST_REPORT_LEN] = { 0 };

	payload[0] = STARLABS_TOUCHPAD_CST_WRITE_FORCE;
	buf_set_le16(payload, 1, press);
	buf_set_le16(payload, 3, release);
	payload[5] = payload[1] ^ payload[2] ^ payload[3] ^ payload[4];

	return starlabs_touchpad_set_report(bus, cmd_reg, data_reg,
					    STARLABS_TOUCHPAD_CST_REPORT_ID,
					    payload, sizeof(payload));
#else
	uint8_t payload[4];

	buf_set_le16(payload, 0, press);
	buf_set_le16(payload, 2, release);

	return starlabs_touchpad_set_report(bus, cmd_reg, data_reg,
					    STARLABS_TOUCHPAD_FORCE_REPORT_ID,
					    payload, sizeof(payload));
#endif
}

#if CONFIG(STARLABS_TOUCHPAD_PIXART)
static int starlabs_touchpad_write_user_reg(unsigned int bus, uint16_t cmd_reg,
					    uint16_t data_reg, uint8_t bank,
					    uint8_t addr, uint8_t value)
{
	const uint8_t payload[] = { addr, bank, value };

	return starlabs_touchpad_set_report(bus, cmd_reg, data_reg,
					    STARLABS_TOUCHPAD_USER_REG_REPORT_ID,
					    payload, sizeof(payload));
}
#endif

static int starlabs_touchpad_get_haptics(unsigned int bus, uint16_t cmd_reg,
					 uint16_t data_reg, uint8_t *level)
{
#if CONFIG(STARLABS_TOUCHPAD_CST)
	uint8_t payload[STARLABS_TOUCHPAD_CST_REPORT_LEN] = { 0 };
	int ret;

	payload[0] = STARLABS_TOUCHPAD_CST_READ_HAPTICS;
	ret = starlabs_touchpad_set_report(bus, cmd_reg, data_reg,
					   STARLABS_TOUCHPAD_CST_REPORT_ID,
					   payload, sizeof(payload));
	if (ret != 0)
		return ret;

	ret = starlabs_touchpad_get_report(bus, cmd_reg, data_reg,
					   STARLABS_TOUCHPAD_CST_REPORT_ID,
					   payload, sizeof(payload));
	if (ret != 0)
		return ret;

	*level = payload[0];
	return 0;
#else
	return starlabs_touchpad_get_report(bus, cmd_reg, data_reg,
					    STARLABS_TOUCHPAD_HAPTICS_REPORT_ID,
					    level, sizeof(*level));
#endif
}

static int starlabs_touchpad_get_force(unsigned int bus, uint16_t cmd_reg,
				       uint16_t data_reg, uint16_t *press,
				       uint16_t *release)
{
#if CONFIG(STARLABS_TOUCHPAD_CST)
	uint8_t payload[STARLABS_TOUCHPAD_CST_REPORT_LEN] = { 0 };
	int ret;

	payload[0] = STARLABS_TOUCHPAD_CST_READ_FORCE;
	ret = starlabs_touchpad_set_report(bus, cmd_reg, data_reg,
					   STARLABS_TOUCHPAD_CST_REPORT_ID,
					   payload, sizeof(payload));
	if (ret != 0)
		return ret;

	ret = starlabs_touchpad_get_report(bus, cmd_reg, data_reg,
					   STARLABS_TOUCHPAD_CST_REPORT_ID,
					   payload, sizeof(payload));
	if (ret != 0)
		return ret;

	*press = payload[0] | (payload[1] << 8);
	*release = payload[2] | (payload[3] << 8);
	return 0;
#else
	uint8_t payload[4];
	int ret;

	ret = starlabs_touchpad_get_report(bus, cmd_reg, data_reg,
					   STARLABS_TOUCHPAD_FORCE_REPORT_ID,
					   payload, sizeof(payload));
	if (ret != 0)
		return ret;

	*press = payload[0] | (payload[1] << 8);
	*release = payload[2] | (payload[3] << 8);

	return 0;
#endif
}

#if CONFIG(STARLABS_TOUCHPAD_PIXART)
static int starlabs_touchpad_read_user_reg(unsigned int bus, uint16_t cmd_reg,
					   uint16_t data_reg, uint8_t *bank,
					   uint8_t *addr, uint8_t *value)
{
	uint8_t payload[3];
	int ret;

	ret = starlabs_touchpad_get_report(bus, cmd_reg, data_reg,
					   STARLABS_TOUCHPAD_USER_REG_REPORT_ID,
					   payload, sizeof(payload));
	if (ret != 0)
		return ret;

	*addr = payload[0];
	*bank = payload[1];
	*value = payload[2];

	return 0;
}
#endif

static int starlabs_touchpad_set_haptics_op(void *arg)
{
	const struct starlabs_touchpad_op_ctx *ctx = arg;

	return starlabs_touchpad_set_haptics(ctx->bus, ctx->cmd_reg,
				     ctx->data_reg, ctx->level);
}

static int starlabs_touchpad_set_force_op(void *arg)
{
	const struct starlabs_touchpad_op_ctx *ctx = arg;

	return starlabs_touchpad_set_force(ctx->bus, ctx->cmd_reg,
					   ctx->data_reg,
					   ctx->press, ctx->release);
}

#if CONFIG(STARLABS_TOUCHPAD_PIXART)
static int starlabs_touchpad_write_user_reg_op(void *arg)
{
	const struct starlabs_touchpad_op_ctx *ctx = arg;

	return starlabs_touchpad_write_user_reg(ctx->bus, ctx->cmd_reg,
						ctx->data_reg, ctx->bank,
						ctx->addr, ctx->value);
}
#endif

static int starlabs_touchpad_reset_op(void *arg)
{
	const struct starlabs_touchpad_op_ctx *ctx = arg;

	return starlabs_touchpad_reset(ctx->bus, ctx->cmd_reg);
}

static int starlabs_touchpad_verify_haptics(const struct starlabs_touchpad_op_ctx *ctx)
{
	uint8_t level = 0;
	int ret = -1;
	int attempt;

	for (attempt = 0; attempt < STARLABS_TOUCHPAD_VERIFY_RETRIES; attempt++) {
		ret = starlabs_touchpad_get_haptics(ctx->bus, ctx->cmd_reg,
						    ctx->data_reg, &level);
		if (ret == 0 && level == ctx->level)
			return 0;
		mdelay(STARLABS_TOUCHPAD_VERIFY_DELAY_MS);
	}

	return ret == 0 ? -1 : ret;
}

static int starlabs_touchpad_verify_force(const struct starlabs_touchpad_op_ctx *ctx)
{
	uint16_t press = 0;
	uint16_t release = 0;
	int ret = -1;
	int attempt;

	for (attempt = 0; attempt < STARLABS_TOUCHPAD_VERIFY_RETRIES; attempt++) {
		ret = starlabs_touchpad_get_force(ctx->bus, ctx->cmd_reg,
						  ctx->data_reg, &press, &release);
		if (ret == 0 && press == ctx->press && release == ctx->release)
			return 0;
		mdelay(STARLABS_TOUCHPAD_VERIFY_DELAY_MS);
	}

	return ret == 0 ? -1 : ret;
}

#if CONFIG(STARLABS_TOUCHPAD_PIXART)
static int starlabs_touchpad_verify_user_reg(const struct starlabs_touchpad_op_ctx *ctx)
{
	uint8_t bank = 0;
	uint8_t addr = 0;
	uint8_t value = 0;
	int ret = -1;
	int attempt;

	for (attempt = 0; attempt < STARLABS_TOUCHPAD_VERIFY_RETRIES; attempt++) {
		ret = starlabs_touchpad_read_user_reg(ctx->bus, ctx->cmd_reg,
						      ctx->data_reg, &bank,
						      &addr, &value);
		if (ret == 0 && bank == ctx->bank && addr == ctx->addr && value == ctx->value)
			return 0;
		mdelay(STARLABS_TOUCHPAD_VERIFY_DELAY_MS);
	}

	return ret == 0 ? -1 : ret;
}
#endif

static int starlabs_touchpad_wait_for_reset_ready(const struct starlabs_touchpad_op_ctx *ctx)
{
	uint8_t level;
	int ret = -1;
	int attempt;

	for (attempt = 0; attempt < STARLABS_TOUCHPAD_RESET_READY_RETRIES; attempt++) {
		ret = starlabs_touchpad_get_haptics(ctx->bus, ctx->cmd_reg,
						    ctx->data_reg, &level);
		if (ret == 0)
			return 0;
		mdelay(STARLABS_TOUCHPAD_RESET_READY_DELAY_MS);
	}

	return ret == 0 ? -1 : ret;
}

static void starlabs_touchpad_init_ctx(struct starlabs_touchpad_op_ctx *ctx)
{
	const struct option_constraints press = OPTION_ENUM(STARLABS_TOUCHPAD_PRESS_FORCE_MINIMAL,
		STARLABS_TOUCHPAD_PRESS_FORCE_LOW, STARLABS_TOUCHPAD_PRESS_FORCE_AVERAGE,
		STARLABS_TOUCHPAD_PRESS_FORCE_HIGH, STARLABS_TOUCHPAD_PRESS_FORCE_HULK);
	const struct option_constraints release = OPTION_ENUM(
		STARLABS_TOUCHPAD_RELEASE_FORCE_MINIMAL, STARLABS_TOUCHPAD_RELEASE_FORCE_LOW,
		STARLABS_TOUCHPAD_RELEASE_FORCE_AVERAGE, STARLABS_TOUCHPAD_RELEASE_FORCE_HIGH,
		STARLABS_TOUCHPAD_RELEASE_FORCE_HULK);

	*ctx = (struct starlabs_touchpad_op_ctx){
		.bus = STARLABS_TOUCHPAD_I2C_BUS,
		.cmd_reg = STARLABS_TOUCHPAD_FALLBACK_CMD_REG,
		.data_reg = STARLABS_TOUCHPAD_FALLBACK_DATA_REG,
		.level = get_uint_option_checked("touchpad_haptics",
						 STARLABS_TOUCHPAD_HAPTICS_DEFAULT,
						 STARLABS_TOUCHPAD_HAPTICS_RULE),
		.press = get_uint_option_checked("touchpad_force_press",
						 STARLABS_TOUCHPAD_PRESS_FORCE_DEFAULT, press),
		.release = get_uint_option_checked("touchpad_force_release",
						   STARLABS_TOUCHPAD_RELEASE_FORCE_DEFAULT,
						   release),
#if CONFIG(STARLABS_TOUCHPAD_PIXART)
		.bank = STARLABS_TOUCHPAD_USER_REG_BANK,
		.addr = STARLABS_TOUCHPAD_USER_REG_ADDR_RATE,
		.value = get_uint_option_checked("touchpad_report_rate",
						 STARLABS_TOUCHPAD_REPORT_RATE_DEFAULT,
						 OPTION_ENUM(STARLABS_TOUCHPAD_RATE_RELAXED,
							     STARLABS_TOUCHPAD_RATE_BALANCED,
							     STARLABS_TOUCHPAD_RATE_FAST,
							     STARLABS_TOUCHPAD_RATE_LUDICROUS,
							     STARLABS_TOUCHPAD_RATE_PLAID)),
#endif
	};
}

static int starlabs_touchpad_retry(int (*op)(void *arg), void *arg)
{
	int ret;
	int attempt;

	for (attempt = 0; attempt < STARLABS_TOUCHPAD_RETRIES; attempt++) {
		ret = op(arg);
		if (ret == 0)
			return 0;
		mdelay(STARLABS_TOUCHPAD_RETRY_DELAY_MS);
	}

	return ret;
}

static void starlabs_touchpad_cache_ctx(void)
{
	if (starlabs_touchpad_cached_ctx_valid)
		return;

	starlabs_touchpad_init_ctx(&starlabs_touchpad_cached_ctx);
	starlabs_touchpad_cached_ctx_valid = 1;
}

static int starlabs_touchpad_wake(const struct starlabs_touchpad_op_ctx *ctx)
{
	uint8_t wake_byte;

	(void)i2c_read_raw(ctx->bus, STARLABS_TOUCHPAD_I2C_ADDR, &wake_byte, 1);
	mdelay(STARLABS_TOUCHPAD_WAKE_DELAY_MS);

	return 0;
}

#if CONFIG(STARLABS_TOUCHPAD_PIXART)
static int starlabs_touchpad_ensure_user_reg(const struct starlabs_touchpad_op_ctx *ctx)
{
	int ret;

	ret = starlabs_touchpad_verify_user_reg(ctx);
	if (ret == 0)
		return 0;

	ret = starlabs_touchpad_retry(starlabs_touchpad_write_user_reg_op, (void *)ctx);
	if (ret != 0)
		return ret;

	return starlabs_touchpad_verify_user_reg(ctx);
}
#endif

static int starlabs_touchpad_ensure_force(const struct starlabs_touchpad_op_ctx *ctx)
{
	int ret;

	ret = starlabs_touchpad_verify_force(ctx);
	if (ret == 0)
		return 0;

	ret = starlabs_touchpad_retry(starlabs_touchpad_set_force_op, (void *)ctx);
	if (ret != 0)
		return ret;

	return starlabs_touchpad_verify_force(ctx);
}

static void starlabs_touchpad_prepare_settings(void *arg)
{
	int ret;

	(void)arg;

	starlabs_touchpad_cache_ctx();
	starlabs_touchpad_wake(&starlabs_touchpad_cached_ctx);

#if CONFIG(STARLABS_TOUCHPAD_PIXART)
	ret = starlabs_touchpad_retry(starlabs_touchpad_write_user_reg_op,
				      &starlabs_touchpad_cached_ctx);
	if (ret != 0) {
		printk(BIOS_WARNING,
		       "Touchpad settings: early report rate write %u failed: %d\n",
		       starlabs_touchpad_cached_ctx.value, ret);
	}
#endif

	ret = starlabs_touchpad_retry(starlabs_touchpad_set_force_op,
				      &starlabs_touchpad_cached_ctx);
	if (ret != 0) {
		printk(BIOS_WARNING,
		       "Touchpad settings: early force write %u/%u failed: %d\n",
		       starlabs_touchpad_cached_ctx.press,
		       starlabs_touchpad_cached_ctx.release, ret);
	}
}

static void starlabs_touchpad_apply_settings(void *arg)
{
	int ret;
	const struct starlabs_touchpad_op_ctx *ctx;

	(void)arg;

	starlabs_touchpad_cache_ctx();
	ctx = &starlabs_touchpad_cached_ctx;

	starlabs_touchpad_wake(ctx);

#if CONFIG(STARLABS_TOUCHPAD_PIXART)
	ret = starlabs_touchpad_ensure_user_reg(ctx);
	if (ret != 0) {
		printk(BIOS_ERR, "Touchpad settings: failed to verify report rate %u: %d\n",
		       ctx->value, ret);
		return;
	}
#endif

	ret = starlabs_touchpad_ensure_force(ctx);
	if (ret != 0) {
		printk(BIOS_ERR,
		       "Touchpad settings: failed to verify force thresholds %u/%u: %d\n",
		       ctx->press, ctx->release, ret);
		return;
	}

	ret = starlabs_touchpad_retry(starlabs_touchpad_set_haptics_op, (void *)ctx);
	if (ret == 0)
		ret = starlabs_touchpad_verify_haptics(ctx);
	if (ret != 0) {
		printk(BIOS_WARNING,
		       "Touchpad settings: direct haptics write failed, retrying after reset: %d\n",
		       ret);
		ret = starlabs_touchpad_retry(starlabs_touchpad_reset_op, (void *)ctx);
		if (ret != 0) {
			printk(BIOS_ERR,
			       "Touchpad settings: failed to reset device before haptics write: %d\n",
			       ret);
			return;
		}

		ret = starlabs_touchpad_wait_for_reset_ready(ctx);
		if (ret != 0) {
			printk(BIOS_ERR,
			       "Touchpad settings: touchpad did not become ready after reset: %d\n",
			       ret);
			return;
		}

		ret = starlabs_touchpad_retry(starlabs_touchpad_set_haptics_op, (void *)ctx);
		if (ret == 0)
			ret = starlabs_touchpad_verify_haptics(ctx);
		if (ret != 0) {
			printk(BIOS_ERR, "Touchpad settings: failed to set haptics level %u: %d\n",
			       ctx->level, ret);
			return;
		}
	}

#if CONFIG(STARLABS_TOUCHPAD_CST)
	printk(BIOS_INFO,
	       "Touchpad settings: applied via regs 0x%04x/0x%04x; "
	       "haptics=%u click=%u release=%u\n",
	       ctx->cmd_reg, ctx->data_reg, ctx->level,
	       ctx->press, ctx->release);
#else
	printk(BIOS_INFO,
	       "Touchpad settings: applied via regs 0x%04x/0x%04x; "
	       "haptics=%u click=%u release=%u rate=%u\n",
	       ctx->cmd_reg, ctx->data_reg, ctx->level,
	       ctx->press, ctx->release, ctx->value);
#endif
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_ENTRY, starlabs_touchpad_prepare_settings,
		      NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, starlabs_touchpad_apply_settings,
		      NULL);
