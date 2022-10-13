/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <commonlib/helpers.h>
#include <device/mmio.h>
#include <types.h>
#include <soc/socinfo.h>
#include <soc/addressmap.h>

#define JTAG_OFFSET	0xB1014

static struct chipinfo chipinfolut[] = {
	/* CHIPINFO_ID_SM_KODIAK = 475 */
	{ .jtagid = CHIPINFO_PARTNUM_SM_KODIAK, .modem = 1, .pro = 0 },
	/* CHIPINFO_ID_SC_KODIAK_CHROME = 487 */
	{ .jtagid = CHIPINFO_PARTNUM_SC_KODIAK_CHROME, .modem = 1, .pro = 0 },
	/* CHIPINFO_ID_SC_KODIAK_WINDOWS = 488 */
	{ .jtagid = CHIPINFO_PARTNUM_SC_KODIAK_WINDOWS,	.modem = 1, .pro = 0 },
	/* CHIPINFO_ID_QCM_KODIAK = 497 */
	{ .jtagid = CHIPINFO_PARTNUM_QCM_KODIAK, .modem = 1, .pro = 0 },
	/* CHIPINFO_ID_QCS_KODIAK = 498 */
	{ .jtagid = CHIPINFO_PARTNUM_QCS_KODIAK, .modem = 0, .pro = 0 },
	/* CHIPINFO_ID_SMP_KODIAK = 499 */
	{ .jtagid = CHIPINFO_PARTNUM_SMP_KODIAK, .modem = 0, .pro = 0 },
	/* CHIPINFO_ID_SM_KODIAK_LTE_ONLY = 515 */
	{ .jtagid = CHIPINFO_PARTNUM_SM_KODIAK_LTE_ONLY, .modem = 1, .pro = 0 },
	/* CHIPINFO_ID_SCP_KODIAK = 546 */
	{ .jtagid = CHIPINFO_PARTNUM_SCP_KODIAK, .modem = 0, .pro = 0 },
	/* CHIPINFO_ID_SC_8CGEN3 = 553 */
	{ .jtagid = CHIPINFO_PARTNUM_SC_8CGEN3,	.modem = 1, .pro = 1 },
	/* CHIPINFO_ID_SCP_8CGEN3 = 563 */
	{ .jtagid = CHIPINFO_PARTNUM_SCP_8CGEN3, .modem = 0, .pro = 1 },
	/* CHIPINFO_ID_KODIAK_SCP_7CGEN3 = 567 */
	{ .jtagid = CHIPINFO_PARTNUM_KODIAK_SCP_7CGEN3,	.modem = 0, .pro = 0 },
	/* CHIPINFO_ID_QCS_KODIAK_LITE = 575 */
	{ .jtagid = CHIPINFO_PARTNUM_QCS_KODIAK_LITE, .modem = 0, .pro = 0 },
	/* CHIPINFO_ID_QCM_KODIAK_LITE = 576 */
	{ .jtagid = CHIPINFO_PARTNUM_QCM_KODIAK_LITE, .modem = 0, .pro = 0 },
};

static uint16_t read_jtagid(void)
{
	return (read32((void *)(TLMM_TILE_BASE + JTAG_OFFSET)) & DEVICE_ID);
}

static int match_jtagid(uint16_t jtagid)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(chipinfolut); i++)
		if (chipinfolut[i].jtagid == jtagid)
			return i;

	return -1;
}

uint16_t socinfo_modem_supported(void)
{
	uint16_t jtagid;
	int ret;

	jtagid = read_jtagid();

	ret = match_jtagid(jtagid);
	if (ret != -1)
		return chipinfolut[ret].modem;

	die("could not match jtagid\n");
}

uint16_t socinfo_pro_part(void)
{
	uint16_t jtagid;
	int ret;

	jtagid = read_jtagid();

	ret = match_jtagid(jtagid);
	if (ret != -1)
		return chipinfolut[ret].pro;

	die("could not match jtagid\n");

}
