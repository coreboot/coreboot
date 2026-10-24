/* SPDX-License-Identifier: GPL-2.0-only */

#include <opensil_config.h>
#include <DF/DfClass-api.h>
#include <ProjSocConst.h>
#include <RcMgr/DfX/RcManager4-api.h>
#include <string.h>
#include <tests/test.h>
#include <vendorcode/amd/opensil/opensil.h>
#include <xSIM-api.h>
#include <xPrfFabric.h>

void __noreturn die(const char *msg, ...)
{
	mock_assert(false, msg, __FILE__, __LINE__);
	__builtin_unreachable();
}

static DFCLASS_INPUT_BLK df;
static DFX_RCMGR_INPUT_BLK rc;
static uint32_t reported_sockets;
static SIL_STATUS query_status;
static SIL_DATA_BLOCK_ID missing;
static unsigned int query_count;

void *SilFindStructure(SIL_DATA_BLOCK_ID id, uint16_t instance)
{
	assert_int_equal(instance, 0);
	if (id == missing)
		return NULL;
	if (id == SilId_DfClass)
		return &df;
	assert_int_equal(id, SilId_RcManager);
	return &rc;
}

SIL_STATUS xPrfFabricGetSystemInfo(uint32_t *sockets, uint32_t *dies,
		uint32_t *roots, XPRF_ROOT_BRIDGE_LOCATION *fch,
		XPRF_ROOT_BRIDGE_LOCATION *smu)
{
	/* The real PCI accessor adds this base before reading the fabric register. */
	assert_int_equal(df.AmdPciExpressBaseAddress, CONFIG_ECAM_MMCONF_BASE_ADDRESS);
	assert_null(dies);
	assert_null(roots);
	assert_null(fch);
	assert_null(smu);
	query_count++;
	*sockets = reported_sockets;
	return query_status;
}

unsigned int cpu_phys_address_size(void)
{
	return 52;
}

static int setup(void **state)
{
	memset(&df, 0, sizeof(df));
	memset(&rc, 0, sizeof(rc));
	/* openSIL's default differs from the ECAM base used by these boards. */
	df.AmdPciExpressBaseAddress = 0xe0000000;
	reported_sockets = CONFIG_MAX_SOCKET;
	query_status = SilPass;
	query_count = 0;
	missing = SilId_ListEnd;
	return 0;
}

static void check_resources(unsigned int expected)
{
	opensil_setup_resources();
	assert_int_equal(query_count, 1);
	assert_int_equal(df.AmdNumberOfPhysicalSocket, expected);
	assert_int_equal(rc.SocketNumber, expected);
	assert_int_equal(rc.RbsPerSocket, PROJ_MAX_RBS_PER_SOCKET);
	assert_int_equal(rc.PciExpressBaseAddress, CONFIG_ECAM_MMCONF_BASE_ADDRESS);
	assert_false(rc.SetRcBasedOnNv);
	assert_int_equal(rc.MmioAbove4GLimit, 1ULL << 48);
}

static void test_configure_ecam_before_discovery(void **state)
{
	check_resources(CONFIG_MAX_SOCKET);
}

static void test_single_populated_socket(void **state)
{
	reported_sockets = 1;
	check_resources(1);
}

static void test_invalid_count(void **state)
{
	const uint32_t invalid[] = {0, CONFIG_MAX_SOCKET + 1, UINT32_MAX};
	for (size_t i = 0; i < ARRAY_SIZE(invalid); i++) {
		reported_sockets = invalid[i];
		query_count = 0;
		check_resources(1);
	}
}

static void test_query_failure(void **state)
{
	query_status = SilNotFound;
	check_resources(1);
}

static void test_missing_input_blocks(void **state)
{
	const SIL_DATA_BLOCK_ID ids[] = {SilId_DfClass, SilId_RcManager};
	for (size_t i = 0; i < ARRAY_SIZE(ids); i++) {
		missing = ids[i];
		expect_assert_failure(opensil_setup_resources());
		assert_int_equal(query_count, 0);
		assert_int_equal(rc.SocketNumber, 0);
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_configure_ecam_before_discovery, setup),
		cmocka_unit_test_setup(test_single_populated_socket, setup),
		cmocka_unit_test_setup(test_invalid_count, setup),
		cmocka_unit_test_setup(test_query_failure, setup),
		cmocka_unit_test_setup(test_missing_input_blocks, setup),
	};
	return cb_run_group_tests(tests, NULL, NULL);
}
