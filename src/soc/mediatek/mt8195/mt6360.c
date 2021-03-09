/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/i2c_simple.h>
#include <soc/mt6360.h>
#include <stdbool.h>
#include <string.h>

enum {
	LDO_INDEX = 0,
	PMIC_INDEX,
};

struct mt6360_i2c_data {
	u8 bus;
	u8 addr;
};

static struct mt6360_i2c_data i2c_data[] = {
	[LDO_INDEX] = {
		.addr = MT6360_LDO_I2C_ADDR,
	},
	[PMIC_INDEX] = {
		.addr = MT6360_PMIC_I2C_ADDR,
	},
};

static const uint32_t mt6360_ldo1_vsel_table[0x10] = {
	[0x4] = 1800000,
	[0x5] = 2000000,
	[0x6] = 2100000,
	[0x7] = 2500000,
	[0x8] = 2700000,
	[0x9] = 2800000,
	[0xA] = 2900000,
	[0xB] = 3000000,
	[0xC] = 3100000,
	[0xD] = 3300000,
};

static const uint32_t mt6360_ldo3_vsel_table[0x10] = {
	[0x4] = 1800000,
	[0xA] = 2900000,
	[0xB] = 3000000,
	[0xD] = 3300000,
};

static const uint32_t mt6360_ldo5_vsel_table[0x10] = {
	[0x2] = 2900000,
	[0x3] = 3000000,
	[0x5] = 3300000,
};

struct mt6360_ldo_data {
	uint8_t enable_reg;
	uint8_t enable_mask;
	uint8_t vsel_reg;
	uint8_t vsel_mask;
	const uint32_t *vsel_table;
	uint32_t vsel_table_len;
};

#define MT6360_LDO_DATA(_enreg, _enmask, _vreg, _vmask, _table)	\
{								\
	.enable_reg = _enreg,					\
	.enable_mask = _enmask,					\
	.vsel_reg = _vreg,					\
	.vsel_mask = _vmask,					\
	.vsel_table = _table,					\
	.vsel_table_len = ARRAY_SIZE(_table),			\
}


static const struct mt6360_ldo_data ldo_data[MT6360_LDO_COUNT] = {
	[MT6360_LDO1] = MT6360_LDO_DATA(0x17, 0x40, 0x1B, 0xFF, mt6360_ldo1_vsel_table),
	[MT6360_LDO2] = MT6360_LDO_DATA(0x11, 0x40, 0x15, 0xFF, mt6360_ldo1_vsel_table),
	[MT6360_LDO3] = MT6360_LDO_DATA(0x05, 0x40, 0x09, 0xFF, mt6360_ldo3_vsel_table),
	[MT6360_LDO5] = MT6360_LDO_DATA(0x0B, 0x40, 0x0F, 0xFF, mt6360_ldo5_vsel_table),
};

#define CRC8_TABLE_SIZE 256
static u8 crc8_table[CRC8_TABLE_SIZE];

static u8 crc8(const u8 table[CRC8_TABLE_SIZE], u8 *pdata, size_t nbytes)
{
	u8 crc = 0;

	while (nbytes-- > 0)
		crc = table[(crc ^ *pdata++) & 0xff];

	return crc;
}

static int mt6360_i2c_write_byte(struct mt6360_i2c_data *i2c, u8 reg, u8 data)
{
	u8 chunk[5] = { 0 };

	if ((reg & 0xc0) != 0) {
		printk(BIOS_ERR, "%s: not support reg [%#x]\n", __func__, reg);
		return -1;
	}

	/*
	 * chunk[0], dev address
	 * chunk[1], reg address
	 * chunk[2], data to write
	 * chunk[3], crc of chunk[0 ~ 2]
	 * chunk[4], blank
	 */
	chunk[0] = (i2c->addr & 0x7f) << 1;
	chunk[1] = (reg & 0x3f);
	chunk[2] = data;
	chunk[3] = crc8(crc8_table, chunk, 3);

	return i2c_write_raw(i2c->bus, i2c->addr, &chunk[1], 4);
}

static int mt6360_i2c_read_byte(struct mt6360_i2c_data *i2c, u8 reg, u8 *data)
{
	u8 chunk[4] = { 0 };
	u8 buf[2];
	int ret;
	u8 crc;

	ret = i2c_read_bytes(i2c->bus, i2c->addr, reg & 0x3f, buf, 2);

	if (ret)
		return ret;
	/*
	 * chunk[0], dev address
	 * chunk[1], reg address
	 * chunk[2], received data
	 * chunk[3], received crc of chunk[0 ~ 2]
	 */
	chunk[0] = ((i2c->addr & 0x7f) << 1) + 1;
	chunk[1] = (reg & 0x3f);
	chunk[2] = buf[0];
	chunk[3] = buf[1];
	crc = crc8(crc8_table, chunk, 3);

	if (chunk[3] != crc) {
		printk(BIOS_ERR, "%s: incorrect CRC: expected %#x, got %#x",
		       __func__, crc, chunk[3]);
		return -1;
	}

	*data = chunk[2];

	return 0;
}

static int mt6360_read_interface(u8 index, u8 reg, u8 *data, u8 mask, u8 shift)
{
	int ret;
	u8 val = 0;

	ret = mt6360_i2c_read_byte(&i2c_data[index], reg, &val);
	if (ret < 0) {
		printk(BIOS_ERR, "%s: fail, reg = %#x, ret = %d\n",
		       __func__, reg, ret);
		return ret;
	}
	val &= (mask << shift);
	*data = (val >> shift);
	return 0;
}

static int mt6360_config_interface(u8 index, u8 reg, u8 data, u8 mask, u8 shift)
{
	int ret;
	u8 val = 0;

	ret = mt6360_i2c_read_byte(&i2c_data[index], reg, &val);
	if (ret < 0) {
		printk(BIOS_ERR, "%s: fail, reg = %#x, ret = %d\n",
		       __func__, reg, ret);
		return ret;
	}
	val &= ~(mask << shift);
	val |= (data << shift);

	return mt6360_i2c_write_byte(&i2c_data[index], reg, val);
}

static bool is_valid_ldo(enum mt6360_ldo_id id)
{
	if (id >= MT6360_LDO_COUNT) {
		printk(BIOS_ERR, "%s: LDO %d is not supported\n", __func__, id);
		return false;
	}

	return true;
}

void mt6360_ldo_enable(enum mt6360_ldo_id id, uint8_t enable)
{
	u8 val;
	const struct mt6360_ldo_data *data;

	if (!is_valid_ldo(id))
		return;

	data = &ldo_data[id];

	if (mt6360_read_interface(LDO_INDEX, data->enable_reg, &val, 0xFF, 0) < 0)
		return;

	if (enable)
		val |= data->enable_mask;
	else
		val &= ~(data->enable_mask);

	mt6360_config_interface(LDO_INDEX, data->enable_reg, val, 0xFF, 0);
}

uint8_t mt6360_ldo_is_enabled(enum mt6360_ldo_id id)
{
	u8 val;
	const struct mt6360_ldo_data *data;

	if (!is_valid_ldo(id))
		return 0;

	data = &ldo_data[id];

	if (mt6360_read_interface(LDO_INDEX, data->enable_reg, &val, 0xFF, 0) < 0)
		return 0;

	return (val & data->enable_mask) ? 1 : 0;
}

void mt6360_ldo_set_voltage(enum mt6360_ldo_id id, u32 voltage_uv)
{
	u8 val = 0;
	u32 voltage_uv_temp = 0;
	int i;

	const struct mt6360_ldo_data *data;

	if (!is_valid_ldo(id))
		return;

	data = &ldo_data[id];

	for (i = 0; i < data->vsel_table_len; i++) {
		u32 uv = data->vsel_table[i];

		if (uv == 0)
			continue;
		if (uv > voltage_uv)
			break;

		val = i << 4;
		voltage_uv_temp = voltage_uv - uv;
	}

	if (val == 0) {
		printk(BIOS_ERR, "%s: LDO %d, set %d uV not supported\n",
		       __func__, id, voltage_uv);
		return;
	}

	voltage_uv_temp /= 10000;
	voltage_uv_temp = MIN(voltage_uv_temp, 0xA);
	val |= (u8)voltage_uv_temp;

	mt6360_config_interface(LDO_INDEX, data->vsel_reg, val, 0xFF, 0);
}

u32 mt6360_ldo_get_voltage(enum mt6360_ldo_id id)
{
	u8 val;
	u32 voltage_uv;

	const struct mt6360_ldo_data *data;

	if (!is_valid_ldo(id))
		return 0;

	data = &ldo_data[id];

	if (mt6360_read_interface(LDO_INDEX, data->vsel_reg, &val, 0xFF, 0) < 0)
		return 0;

	voltage_uv = data->vsel_table[(val & 0xF0) >> 4];
	if (voltage_uv == 0) {
		printk(BIOS_ERR, "%s: LDO %d read fail, reg = %#x\n", __func__, id, val);
		return 0;
	}

	val = MIN(val & 0x0F, 0x0A);
	voltage_uv += val * 10000;

	return voltage_uv;
}

static void crc8_populate_msb(u8 table[CRC8_TABLE_SIZE], u8 polynomial)
{
	int i, j;
	const u8 msbit = 0x80;
	u8 t = msbit;

	table[0] = 0;

	for (i = 1; i < CRC8_TABLE_SIZE; i *= 2) {
		t = (t << 1) ^ (t & msbit ? polynomial : 0);
		for (j = 0; j < i; j++)
			table[i + j] = table[j] ^ t;
	}
}

void mt6360_init(uint8_t bus)
{
	u8 delay01, delay02, delay03, delay04;

	crc8_populate_msb(crc8_table, 0x7);

	i2c_data[LDO_INDEX].bus = bus;
	i2c_data[PMIC_INDEX].bus = bus;

	mt6360_config_interface(PMIC_INDEX, 0x07, 0x04, 0xFF, 0);
	mt6360_config_interface(PMIC_INDEX, 0x08, 0x00, 0xFF, 0);
	mt6360_config_interface(PMIC_INDEX, 0x09, 0x02, 0xFF, 0);
	mt6360_config_interface(PMIC_INDEX, 0x0A, 0x00, 0xFF, 0);

	mt6360_read_interface(PMIC_INDEX, 0x07, &delay01, 0xFF, 0);
	mt6360_read_interface(PMIC_INDEX, 0x08, &delay02, 0xFF, 0);
	mt6360_read_interface(PMIC_INDEX, 0x09, &delay03, 0xFF, 0);
	mt6360_read_interface(PMIC_INDEX, 0x0A, &delay04, 0xFF, 0);
	printk(BIOS_DEBUG,
	       "%s: power off sequence delay: %#x, %#x, %#x, %#x\n",
	       __func__, delay01, delay02, delay03, delay04);
}
