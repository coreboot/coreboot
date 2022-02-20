/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <cf9_reset.h>
#include <option.h>
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <cpu/intel/model_206ax/model_206ax.h>
#include <southbridge/intel/common/gpio.h>
#include <superio/smsc/sch5545/sch5545.h>
#include <superio/smsc/sch5545/sch5545_emi.h>

#include <baseboard/sch5545_ec.h>
#include <variant/sch5545_ec_tables.h>

#define GPIO_CHASSIS_ID0			1
#define GPIO_CHASSIS_ID1			17
#define GPIO_CHASSIS_ID2			37
#define GPIO_FRONT_PANEL_CHASSIS_DET_L		70

static const struct ec_val_reg ec_hwm_init_seq[] = {
	{ 0xa0, 0x02fc },
	{ 0x32, 0x02fd },
	{ 0x77, 0x0005 },
	{ 0x0f, 0x0018 },
	{ 0x2f, 0x0019 },
	{ 0x2f, 0x001a },
	{ 0x33, 0x008a },
	{ 0x33, 0x008b },
	{ 0x33, 0x008c },
	{ 0x10, 0x00ba },
	{ 0xff, 0x00d1 },
	{ 0xff, 0x00d6 },
	{ 0xff, 0x00db },
	{ 0x00, 0x0048 },
	{ 0x00, 0x0049 },
	{ 0x00, 0x007a },
	{ 0x00, 0x007b },
	{ 0x00, 0x007c },
	{ 0x00, 0x0080 },
	{ 0x00, 0x0081 },
	{ 0x00, 0x0082 },
	{ 0xbb, 0x0083 },
	{ 0xb0, 0x0084 },
	{ 0x88, 0x01a1 },
	{ 0x80, 0x01a4 },
	{ 0x00, 0x0088 },
	{ 0x00, 0x0089 },
	{ 0x02, 0x00a0 },
	{ 0x02, 0x00a1 },
	{ 0x02, 0x00a2 },
	{ 0x04, 0x00a4 },
	{ 0x04, 0x00a5 },
	{ 0x04, 0x00a6 },
	{ 0x00, 0x00ab },
	{ 0x3f, 0x00ad },
	{ 0x07, 0x00b7 },
	{ 0x50, 0x0062 },
	{ 0x46, 0x0063 },
	{ 0x50, 0x0064 },
	{ 0x46, 0x0065 },
	{ 0x50, 0x0066 },
	{ 0x46, 0x0067 },
	{ 0x98, 0x0057 },
	{ 0x98, 0x0059 },
	{ 0x7c, 0x0061 },
	{ 0x00, 0x01bc },
	{ 0x00, 0x01bd },
	{ 0x00, 0x01bb },
	{ 0xdd, 0x0085 },
	{ 0xdd, 0x0086 },
	{ 0x07, 0x0087 },
	{ 0x5e, 0x0090 },
	{ 0x5e, 0x0091 },
	{ 0x5d, 0x0095 },
	{ 0x00, 0x0096 },
	{ 0x00, 0x0097 },
	{ 0x00, 0x009b },
	{ 0x86, 0x00ae },
	{ 0x86, 0x00af },
	{ 0x67, 0x00b3 },
	{ 0xff, 0x00c4 },
	{ 0xff, 0x00c5 },
	{ 0xff, 0x00c9 },
	{ 0x01, 0x0040 },
	{ 0x00, 0x02fc },
	{ 0x9a, 0x02b3 },
	{ 0x05, 0x02b4 },
	{ 0x01, 0x02cc },
	{ 0x4c, 0x02d0 },
	{ 0x01, 0x02d2 },
	{ 0x01, 0x006f },
	{ 0x02, 0x0070 },
	{ 0x03, 0x0071 },
};


static uint8_t send_mbox_msg_with_int(uint8_t mbox_message)
{
	uint8_t int_sts, int_cond;

	sch5545_emi_h2ec_mbox_write(mbox_message);

	do {
		int_sts = sch5545_emi_get_int_src_low();
		int_cond = int_sts & 0x71;
	} while (int_cond == 0);

	sch5545_emi_set_int_src_low(int_cond);

	if ((int_sts & 1) == 0)
		return 0;

	if (sch5545_emi_ec2h_mbox_read() == mbox_message)
		return 1;

	return 0;
}

static uint8_t send_mbox_msg_simple(uint8_t mbox_message)
{
	uint8_t int_sts;

	sch5545_emi_h2ec_mbox_write(mbox_message);

	do {
		int_sts = sch5545_emi_get_int_src_low();
		if ((int_sts & 70) != 0)
			return 0;
	} while ((int_sts & 1) == 0);

	if (sch5545_emi_ec2h_mbox_read() == mbox_message)
		return 1;

	return 0;
}

static void ec_check_mbox_and_int_status(uint8_t int_src, uint8_t mbox_msg)
{
	uint8_t val;

	val = sch5545_emi_ec2h_mbox_read();
	if (val != mbox_msg)
		printk(BIOS_SPEW, "EC2H mailbox should be %02x, is %02x\n", mbox_msg, val);

	val = sch5545_emi_get_int_src_low();
	if (val != int_src)
		printk(BIOS_SPEW, "EC INT SRC should be %02x, is %02x\n", int_src, val);

	sch5545_emi_set_int_src_low(val);
}

static uint8_t ec_read_write_reg(uint8_t ldn, uint16_t reg, uint8_t *value, uint8_t rw_bit)
{
	uint8_t int_mask_bckup, ret = 0;
	rw_bit &= 1;

	int_mask_bckup = sch5545_emi_get_int_mask_low();
	sch5545_emi_set_int_mask_low(0);

	sch5545_emi_ec_write16(0x8000, (ldn << 1) | 0x100 | rw_bit);
	if (rw_bit)
		sch5545_emi_ec_write32(0x8004, (reg << 16) | *value);
	else
		sch5545_emi_ec_write32(0x8004, reg << 16);

	ret = send_mbox_msg_with_int(1);
	if (ret && !rw_bit)
		*value = sch5545_emi_ec_read8(0x8004);
	else if (ret != 1 && rw_bit)
		printk(BIOS_WARNING, "EC mailbox returned unexpected value "
				     "when writing %02x to %04x\n", *value, reg);
	else if (ret != 1 && !rw_bit)
		printk(BIOS_WARNING, "EC mailbox returned unexpected value "
				     "when reading %04x\n", reg);

	sch5545_emi_set_int_mask_low(int_mask_bckup);

	return ret;
}

uint16_t sch5545_get_ec_fw_version(void)
{
	uint8_t val = 0;
	uint16_t ec_fw_version;

	/* Read the FW version currently loaded used by EC */
	ec_read_write_reg(EC_HWM_LDN, 0x2ad, &val, READ_OP);
	ec_fw_version = (val << 8);
	ec_read_write_reg(EC_HWM_LDN, 0x2ae, &val, READ_OP);
	ec_fw_version |= val;
	ec_read_write_reg(EC_HWM_LDN, 0x2ac, &val, READ_OP);
	ec_read_write_reg(EC_HWM_LDN, 0x2fd, &val, READ_OP);
	ec_read_write_reg(EC_HWM_LDN, 0x2b0, &val, READ_OP);

	return ec_fw_version;
}

void sch5545_update_ec_firmware(uint16_t ec_version)
{
	uint8_t status;
	uint16_t ec_fw_version;
	uint32_t *ec_fw_file;
	size_t ec_fw_file_size;

	ec_fw_file = cbfs_map("sch5545_ecfw.bin", &ec_fw_file_size);

	if (!ec_fw_file || ec_fw_file_size != 0x1750) {
		printk(BIOS_ERR, "EC firmware file not found in CBFS!\n");
		printk(BIOS_ERR, "The fans will keep running at maximum speed.\n");
		return;
	}

	ec_fw_version = ec_fw_file[3] & 0xffff;

	/*
	 * After power failure EC loses its configuration. The currently used firmware version
	 * by EC will be reported as 0x0000. In such case EC firmware needs to be uploaded.
	 */
	if (ec_version != ec_fw_version) {
		printk(BIOS_INFO, "SCH5545 EC is not functional, probably due to power "
				  "failure\n");
		printk(BIOS_INFO, "Uploading EC firmware (version %04x) to SCH5545\n",
				   ec_fw_version);

		if (!send_mbox_msg_simple(0x03)) {
			printk(BIOS_WARNING, "EC didn't accept FW upload start signal\n");
			printk(BIOS_WARNING, "EC firmware update failed!\n");
			return;
		}

		sch5545_emi_ec_write32_bulk(0x8100, ec_fw_file, ec_fw_file_size);

		status = send_mbox_msg_simple(0x04);
		status += send_mbox_msg_simple(0x06);

		if (status != 2)
			printk(BIOS_WARNING, "EC firmware update failed!\n");

		if (ec_fw_version != sch5545_get_ec_fw_version()) {
			printk(BIOS_ERR, "EC firmware update failed!\n");
			printk(BIOS_ERR, "The fans will keep running at maximum speed\n");
		} else {
			printk(BIOS_INFO, "EC firmware update success\n");
			/*
			 * The vendor BIOS does a full reset after EC firmware update. Most
			 * likely because the fans are adapting very slowly after automatic fan
			 * control is enabled. This makes huge noise. To avoid it, also do the
			 * full reset. On next boot, it will not be necessary.
			 */
			full_reset();
		}
	} else {
		printk(BIOS_INFO, "SCH5545 EC firmware up to date (version %04x)\n",
		       ec_version);
	}
}

void sch5545_ec_hwm_early_init(void)
{
	uint8_t val = 0;
	int i;

	printk(BIOS_DEBUG, "%s\n", __func__);

	ec_check_mbox_and_int_status(0x20, 0x01);

	ec_read_write_reg(2, 0xcb, &val, READ_OP);
	ec_read_write_reg(2, 0xb8, &val, READ_OP);

	for (i = 0; i < ARRAY_SIZE(ec_hwm_init_seq); i++) {
		val = ec_hwm_init_seq[i].val;
		ec_read_write_reg(EC_HWM_LDN, ec_hwm_init_seq[i].reg, &val,
				  WRITE_OP);
	}

	ec_check_mbox_and_int_status(0x01, 0x01);
}

static uint8_t get_sku_tdp_config(void)
{
	msr_t msr;
	uint32_t power_unit, tdp;
	/* Get units */
	msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = msr.lo & 0xf;

	/* Get power defaults for this SKU */
	msr = rdmsr(MSR_PKG_POWER_SKU);
	tdp = msr.lo & 0x7fff;

	/* These numbers will determine which settings to use to init EC */
	if ((tdp >> power_unit) < 66)
		return 16;
	else
		return 32;
}

static uint8_t get_chassis_type(void)
{
	uint8_t chassis_id;

	chassis_id  = get_gpio(GPIO_CHASSIS_ID0);
	chassis_id |= get_gpio(GPIO_CHASSIS_ID1) << 1;
	chassis_id |= get_gpio(GPIO_CHASSIS_ID2) << 2;
	chassis_id |= get_gpio(GPIO_FRONT_PANEL_CHASSIS_DET_L) << 3;

	/* This mapping will determine which EC init sequence to use */
	switch (chassis_id) {
	case 0x0:
	case 0x4:
		return 5; /* MT */
	case 0x8:
		return 4; /* DT */
	case 0x3:
	case 0xb:
		return 3; /* USFF */
	case 0x1:
	case 0x9:
	case 0x5:
	case 0xd:
		return 6; /* SFF */
	default:
		printk(BIOS_DEBUG, "Unknown chassis ID %x\n", chassis_id);
		break;
	}

	return 0xff;
}

static void ec_hwm_init_late(const ec_chassis_tdp_t *ec_hwm_sequence, size_t size)
{
	unsigned int i;
	uint8_t val;
	uint8_t tdp_config = get_sku_tdp_config();

	for (i = 0; i < size; i++) {
		if (ec_hwm_sequence[i].tdp == tdp_config ||
		    ec_hwm_sequence[i].tdp == TDP_COMMON) {
			val = ec_hwm_sequence[i].val;
			ec_read_write_reg(EC_HWM_LDN, ec_hwm_sequence[i].reg, &val, WRITE_OP);
		}
	}
}

static void prepare_for_hwm_ec_sequence(uint8_t write_only, uint8_t *value)
{
	uint16_t reg;
	uint8_t val;

	if (write_only == 1) {
		val = *value;
		reg = 0x02fc;
	} else {
		if (value != NULL)
			ec_read_write_reg(EC_HWM_LDN, 0x02fc, value, READ_OP);
		val = 0xa0;
		ec_read_write_reg(EC_HWM_LDN, 0x2fc, &val, WRITE_OP);
		val = 0x32;
		reg = 0x02fd;
	}

	ec_read_write_reg(1, reg, &val, WRITE_OP);
}

void sch5545_ec_hwm_init(void *unused)
{
	uint8_t val = 0, val_2fc, chassis_type;

	printk(BIOS_DEBUG, "%s\n", __func__);
	sch5545_emi_init(0x2e);

	chassis_type = get_chassis_type();

	ec_read_write_reg(EC_HWM_LDN, 0x0048, &val, READ_OP);
	ec_read_write_reg(EC_HWM_LDN, 0x0048, &val, WRITE_OP);
	ec_read_write_reg(EC_HWM_LDN, 0x0042, &val, READ_OP);
	ec_read_write_reg(EC_HWM_LDN, 0x0048, &val, READ_OP);
	val |= 0x02;
	ec_read_write_reg(EC_HWM_LDN, 0x0048, &val, WRITE_OP);

	ec_read_write_reg(EC_HWM_LDN, 0x0048, &val, READ_OP);
	ec_read_write_reg(EC_HWM_LDN, 0x0048, &val, WRITE_OP);
	ec_read_write_reg(EC_HWM_LDN, 0x0042, &val, READ_OP);
	ec_read_write_reg(EC_HWM_LDN, 0x0048, &val, READ_OP);
	val |= 0x04;
	ec_read_write_reg(EC_HWM_LDN, 0x0048, &val, WRITE_OP);

	ec_read_write_reg(EC_HWM_LDN, 0x0081, &val, READ_OP);
	ec_read_write_reg(EC_HWM_LDN, 0x0027, &val, READ_OP);

	ec_check_mbox_and_int_status(0x00, 0x01);

	prepare_for_hwm_ec_sequence(0, &val_2fc);

	if (chassis_type != 0xff) {
		printk(BIOS_DEBUG, "Performing HWM init for chassis %d\n", chassis_type);
		switch (chassis_type) {
		case 3:
			ec_hwm_init_late(ec_hwm_chassis3, ARRAY_SIZE(ec_hwm_chassis3));
			break;
		case 4:
			ec_hwm_init_late(ec_hwm_chassis4, ARRAY_SIZE(ec_hwm_chassis4));
			break;
		case 5:
			ec_hwm_init_late(ec_hwm_chassis5, ARRAY_SIZE(ec_hwm_chassis5));
			break;
		case 6:
			ec_hwm_init_late(ec_hwm_chassis6, ARRAY_SIZE(ec_hwm_chassis6));
			break;
		}
	}

	if (CONFIG_MAX_CPUS > 2) {
		val = 0x30;
		ec_read_write_reg(EC_HWM_LDN, 0x009e, &val, WRITE_OP);
		ec_read_write_reg(EC_HWM_LDN, 0x00ea, &val, READ_OP);
		ec_read_write_reg(EC_HWM_LDN, 0x00eb, &val, WRITE_OP);
	}

	ec_read_write_reg(EC_HWM_LDN, 0x02fc, &val_2fc, WRITE_OP);

	unsigned int fan_speed_full = get_uint_option("fan_full_speed", 0);
	if (fan_speed_full) {
		printk(BIOS_INFO, "Will set up fans to run at full speed\n");
		ec_read_write_reg(EC_HWM_LDN, 0x0080, &val, READ_OP);
		val |= 0x60;
		ec_read_write_reg(EC_HWM_LDN, 0x0080, &val, WRITE_OP);
		ec_read_write_reg(EC_HWM_LDN, 0x0081, &val, READ_OP);
		val |= 0x60;
		ec_read_write_reg(EC_HWM_LDN, 0x0081, &val, WRITE_OP);
	} else {
		printk(BIOS_INFO, "Will set up fans for automatic control\n");
	}

	ec_read_write_reg(EC_HWM_LDN, 0x00b8, &val, READ_OP);


	if (chassis_type == 4 || chassis_type == 5) {
		ec_read_write_reg(EC_HWM_LDN, 0x0027, &val, READ_OP);
		if (val == 0) {
			printk(BIOS_INFO, "Applying HWM fix-up for MT/DT chassis\n");
			ec_read_write_reg(EC_HWM_LDN, 0x00a0, &val, READ_OP);
			val &= 0xfb;
			ec_read_write_reg(EC_HWM_LDN, 0x00a0, &val, WRITE_OP);
			ec_read_write_reg(EC_HWM_LDN, 0x00a1, &val, READ_OP);
			val &= 0xfb;
			ec_read_write_reg(EC_HWM_LDN, 0x00a1, &val, WRITE_OP);
			ec_read_write_reg(EC_HWM_LDN, 0x00a2, &val, READ_OP);
			val &= 0xfb;
			ec_read_write_reg(EC_HWM_LDN, 0x00a2, &val, WRITE_OP);
			val = 0x99;
			ec_read_write_reg(EC_HWM_LDN, 0x008a, &val, WRITE_OP);
			val = 0x47;
			ec_read_write_reg(EC_HWM_LDN, 0x008b, &val, WRITE_OP);
			val = 0x91;
			ec_read_write_reg(EC_HWM_LDN, 0x008c, &val, WRITE_OP);
		}
	}

	ec_read_write_reg(EC_HWM_LDN, 0x0049, &val, READ_OP);
	val &= 0xf7;
	ec_read_write_reg(EC_HWM_LDN, 0x0049, &val, WRITE_OP);

	val = 0x6a;
	if (chassis_type != 3)
		ec_read_write_reg(EC_HWM_LDN, 0x0059, &val, WRITE_OP);
	else
		ec_read_write_reg(EC_HWM_LDN, 0x0057, &val, WRITE_OP);

	ec_read_write_reg(EC_HWM_LDN, 0x0041, &val, READ_OP);
	val |= 0x40;
	ec_read_write_reg(EC_HWM_LDN, 0x0041, &val, WRITE_OP);

	if (chassis_type == 3) {
		ec_read_write_reg(EC_HWM_LDN, 0x0049, &val, READ_OP);
		val |= 0x04;
	} else {
		ec_read_write_reg(EC_HWM_LDN, 0x0049, &val, READ_OP);
		val |= 0x08;
	}
	ec_read_write_reg(EC_HWM_LDN, 0x0049, &val, WRITE_OP);

	val = 0x0e;
	ec_read_write_reg(EC_HWM_LDN, 0x007b, &val, WRITE_OP);
	ec_read_write_reg(EC_HWM_LDN, 0x007c, &val, WRITE_OP);
	val = 0x01;
	ec_read_write_reg(EC_HWM_LDN, 0x007a, &val, WRITE_OP);
}
