/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/root_complex.h>
#include <amdblocks/data_fabric.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/fabric.h>
#include <string.h>
#include <tests/test.h>
#include <xPrfFabric.h>

#define BUSES_PER_ROOT (CONFIG_ECAM_MMCONF_BUS_NUMBER / (CONFIG_MAX_SOCKET * 8))

struct root_fixture {
	uint32_t fabric, base, limit, physical;
	bool fch, smu;
};

static struct {
	uint32_t sockets, dies, roots;
	uint32_t socket_dies[2], socket_roots[2], die_roots[2];
	struct root_fixture rb[2][8];
	XPRF_ROOT_BRIDGE_LOCATION fch, smu;
	unsigned int failed_api;
} fixture;
static const uint8_t physical_order[] = { 4, 5, 6, 7, 3, 2, 1, 0 };
static struct device domains[17];
static struct bus buses[17];
static struct bus upstream;
struct device dev_root;
static struct device *scanned;
static unsigned int errors;
static bool fallback_decode;

enum cb_err data_fabric_get_pci_bus_numbers(struct device *domain,
					  uint8_t *segment, uint8_t *bus, uint8_t *limit)
{
	assert_ptr_equal(domain, &domains[0]);
	*segment = 0;
	*bus = 0;
	*limit = BUSES_PER_ROOT - 1;
	return fallback_decode ? CB_SUCCESS : CB_ERR;
}

int printk(int level, const char *fmt, ...)
{
	if (level <= BIOS_ERR)
		errors++;
	return 0;
}

void pci_host_bridge_scan_bus(struct device *domain)
{
	assert_null(scanned);
	scanned = domain;
}

SIL_STATUS xPrfFabricGetSystemInfo(uint32_t *sockets, uint32_t *dies, uint32_t *roots,
				 XPRF_ROOT_BRIDGE_LOCATION *fch,
				 XPRF_ROOT_BRIDGE_LOCATION *smu)
{
	*sockets = fixture.sockets;
	*dies = fixture.dies;
	*roots = fixture.roots;
	*fch = fixture.fch;
	*smu = fixture.smu;
	return fixture.failed_api == 1 ? SilNotFound : SilPass;
}

SIL_STATUS xPrfFabricGetProcessorInfo(uint32_t socket, uint32_t *dies, uint32_t *roots)
{
	assert_true(socket < fixture.sockets && socket < 2);
	*dies = fixture.socket_dies[socket];
	*roots = fixture.socket_roots[socket];
	return fixture.failed_api == 2 ? SilNotFound : SilPass;
}

SIL_STATUS xPrfFabricGetDieInfo(uint32_t socket, uint32_t die, uint32_t *roots,
			      uint32_t *offset, const XPRF_FABRIC_DEVICE_MAP **map)
{
	assert_int_equal(die, 0);
	*roots = fixture.die_roots[socket];
	return fixture.failed_api == 3 ? SilNotFound : SilPass;
}

SIL_STATUS xPrfFabricGetRootBridgeInfo(uint32_t socket, uint32_t die, uint32_t index,
				    uint32_t *fabric, uint32_t *base, uint32_t *limit,
				    uint32_t *physical, bool *fch, bool *smu)
{
	assert_int_equal(die, 0);
	assert_true(index < fixture.die_roots[socket] && index < 8);
	/* A consumer must not observe a partially initialized cache. */
	assert_null(turin_get_root_bridge(0));
	assert_int_equal(turin_get_socket_count(), 0);
	size_t count = 99;
	assert_null(get_iohc_info(&count));
	assert_int_equal(count, 0);
	const struct root_fixture *rb = &fixture.rb[socket][index];
	*fabric = rb->fabric;
	*base = rb->base;
	*limit = rb->limit;
	*physical = rb->physical;
	*fch = rb->fch;
	*smu = rb->smu;
	return fixture.failed_api == 4 ? SilNotFound : SilPass;
}

static int setup(void **state)
{
	/* Deliberately different from both domain order and physical RB numbering. */
	static const uint8_t discovery_order[] = { 1, 4, 5, 0, 7, 2, 3, 6 };
	static const uint16_t fabric_order[] = { 0x24, 0x25, 0x26, 0x27, 0x23, 0x22, 0x21, 0x20 };
	memset(&fixture, 0, sizeof(fixture));
	memset(domains, 0, sizeof(domains));
	memset(buses, 0, sizeof(buses));
	fixture.sockets = CONFIG_MAX_SOCKET;
	fixture.dies = fixture.sockets;
	fixture.roots = fixture.sockets * 8;
	fixture.fch = (XPRF_ROOT_BRIDGE_LOCATION) { 0, 0, 1 };
	fixture.smu = fixture.fch;
	for (unsigned int socket = 0; socket < fixture.sockets; socket++) {
		fixture.socket_dies[socket] = 1;
		fixture.socket_roots[socket] = 8;
		fixture.die_roots[socket] = 8;
		for (unsigned int index = 0; index < 8; index++) {
			unsigned int local = 0;
			while (physical_order[local] != discovery_order[index])
				local++;
			fixture.rb[socket][index] = (struct root_fixture) {
				.physical = discovery_order[index],
				.fabric = socket * 0x100 + fabric_order[local],
				.base = (socket * 8 + local) * BUSES_PER_ROOT,
				.limit = (socket * 8 + local + 1) * BUSES_PER_ROOT - 1,
				.fch = local == 0,
				.smu = local == 0,
			};
		}
	}
	for (unsigned int i = 0; i < ARRAY_SIZE(domains); i++) {
		domains[i].path.type = DEVICE_PATH_DOMAIN;
		domains[i].path.domain.domain_id = i;
		domains[i].upstream = &upstream;
		domains[i].downstream = &buses[i];
		domains[i].enabled = 1;
	}
	errors = 0;
	fallback_decode = true;
	scanned = NULL;
	return 0;
}

static void assert_rejected(void)
{
	assert_int_equal(turin_fabric_init(), CB_ERR);
	assert_true(errors > 0);
	assert_int_equal(turin_get_socket_count(), 0);
	assert_non_null(turin_get_root_bridge(0));
	for (unsigned int i = 1; i < 17; i++)
		assert_null(turin_get_root_bridge(i));
	size_t count = 99;
	const struct domain_iohc_info *iohc = get_iohc_info(&count);
	assert_non_null(iohc);
	assert_int_equal(count, 1);
	assert_int_equal(iohc[0].fabric_id, 0x24);
	assert_int_equal(iohc[0].smn_dev, PCI_DEV(0, 0, 0));
}

static void test_fch_fallback_scan(void **state)
{
	fixture.failed_api = 1;
	assert_rejected();
	turin_pci_domain_scan_bus(&domains[0]);
	assert_ptr_equal(scanned, &domains[0]);
	assert_true(domains[0].enabled);
	assert_int_equal(buses[0].secondary, 0);
	assert_int_equal(buses[0].max_subordinate, BUSES_PER_ROOT - 1);
	scanned = NULL;
	fallback_decode = false;
	turin_pci_domain_scan_bus(&domains[0]);
	assert_null(scanned);
	assert_false(domains[0].enabled);
}

static void test_discovery_and_scan(void **state)
{
	assert_int_equal(turin_fabric_init(), CB_SUCCESS);
	assert_int_equal(errors, 0);
	assert_int_equal(turin_get_socket_count(), fixture.sockets);
	size_t count = 0;
	const struct domain_iohc_info *iohc = get_iohc_info(&count);
	assert_int_equal(count, fixture.sockets * 8);
	for (unsigned int i = 0; i < count; i++) {
		const struct turin_root_bridge *rb = turin_get_root_bridge(i);
		assert_non_null(rb);
		assert_int_equal(rb->socket, i / 8);
		assert_int_equal(rb->die, 0);
		assert_int_equal(rb->physical_rb, physical_order[i % 8]);
		assert_int_equal(rb->fabric_id, fixture.rb[i / 8][rb->index].fabric);
		assert_int_equal(rb->is_system_fch, i == 0);
		assert_int_equal(rb->is_system_smu, i == 0);
		assert_int_equal(rb->has_fch, i % 8 == 0);
		assert_int_equal(iohc[i].smn_dev, PCI_DEV(i * BUSES_PER_ROOT, 0, 0));
		assert_ptr_equal(get_domain_iohc_info(&domains[i]), &iohc[i]);
		assert_int_equal(get_iohc_fabric_id(&domains[i]), rb->fabric_id);
		turin_pci_domain_scan_bus(&domains[i]);
		assert_ptr_equal(scanned, &domains[i]);
		assert_int_equal(buses[i].secondary, (i * BUSES_PER_ROOT) & 0xff);
		assert_int_equal(buses[i].subordinate, (i * BUSES_PER_ROOT) & 0xff);
		assert_int_equal(buses[i].max_subordinate,
				 (i * BUSES_PER_ROOT + BUSES_PER_ROOT - 1) & 0xff);
		assert_int_equal(buses[i].segment_group, (i * BUSES_PER_ROOT) >> 8);
		scanned = NULL;
	}
	assert_null(turin_get_root_bridge(count));
	assert_int_equal(turin_get_root_bridge(0)->index, 1);
	assert_int_equal(turin_get_root_bridge(7)->index, 3);
	assert_int_equal(iohc[0].misc_smn_base, SMN_IOHC_MISC_BASE_13D1);
	assert_int_equal(iohc[7].misc_smn_base, SMN_IOHC_MISC_BASE_13B1);
}

static void use_sp5_topology(void)
{
	/* DF IOS instances are numbered before the NBIO RBIndex fixup.
	 * The FCH/SMU root is DF4 (NBIO2).
	 */
	static const uint8_t bus_slot[] = { 7, 6, 5, 4, 0, 1, 2, 3 };
	const unsigned int buses_per_root = 256 / (fixture.sockets * 8);
	fixture.fch = (XPRF_ROOT_BRIDGE_LOCATION) { 0, 0, 4 };
	fixture.smu = fixture.fch;
	for (unsigned int socket = 0; socket < fixture.sockets; socket++) {
		for (unsigned int index = 0; index < 8; index++) {
			const unsigned int base = (socket * 8 + bus_slot[index]) * buses_per_root;
			fixture.rb[socket][index] = (struct root_fixture) {
				.physical = index,
				.fabric = socket * 0x100 + 0x20 + index,
				.base = base,
				.limit = base + buses_per_root - 1,
				.fch = index == 4,
				.smu = index == 4,
			};
		}
	}
}

static void test_sp5_df_topology(void **state)
{
	static const struct {
		uint8_t domain;
		uint32_t smn_base;
	} expected_by_df[] = {
		{ 7, SMN_IOHC_MISC_BASE_13B1 },
		{ 6, SMN_IOHC_MISC_BASE_1D51 },
		{ 5, SMN_IOHC_MISC_BASE_13C1 },
		{ 4, SMN_IOHC_MISC_BASE_1D41 },
		{ 0, SMN_IOHC_MISC_BASE_13D1 },
		{ 1, SMN_IOHC_MISC_BASE_1D71 },
		{ 2, SMN_IOHC_MISC_BASE_13E1 },
		{ 3, SMN_IOHC_MISC_BASE_1D61 },
	};
	use_sp5_topology();
	assert_int_equal(turin_fabric_init(), CB_SUCCESS);
	assert_int_equal(errors, 0);
	for (unsigned int socket = 0; socket < fixture.sockets; socket++) {
		for (unsigned int df = 0; df < 8; df++) {
			const unsigned int domain = socket * 8 + expected_by_df[df].domain;
			const struct turin_root_bridge *root = turin_get_root_bridge(domain);
			const struct domain_iohc_info *iohc = get_domain_iohc_info(&domains[domain]);
			assert_non_null(root);
			assert_non_null(iohc);
			assert_int_equal(root->physical_rb, df);
			assert_int_equal(root->index, df);
			assert_int_equal(root->fabric_id, socket * 0x100 + 0x20 + df);
			assert_int_equal(root->socket, socket);
			assert_int_equal(root->segment_group, 0);
			assert_int_equal(root->bus_base, fixture.rb[socket][df].base);
			assert_int_equal(root->bus_limit, fixture.rb[socket][df].limit);
			assert_int_equal(root->has_fch, df == 4);
			assert_int_equal(root->has_smu, df == 4);
			assert_int_equal(root->is_system_fch, socket == 0 && df == 4);
			assert_int_equal(root->is_system_smu, socket == 0 && df == 4);
			assert_int_equal(iohc->misc_smn_base, expected_by_df[df].smn_base);
			assert_int_equal(iohc->smn_dev, PCI_DEV(root->bus_base, 0, 0));
		}
	}
	assert_int_equal(turin_get_root_bridge(0)->bus_base, 0);
#if CONFIG_MAX_SOCKET == 2
	assert_int_equal(turin_get_root_bridge(8)->bus_base, 0x80);
#endif
}

static void test_sp5_mismatched_roles_rejected(void **state)
{
	use_sp5_topology();
	fixture.fch.Index = 2; /* NBIO2 is not DF2. */
	assert_rejected();
	use_sp5_topology();
	fixture.smu.Index = 2;
	assert_rejected();
}

static void test_opensil_errors_and_root_count(void **state)
{
	for (unsigned int api = 1; api <= 4; api++) {
		setup(state);
		fixture.failed_api = api;
		assert_rejected();
	}
	setup(state);
	fixture.sockets = CONFIG_MAX_SOCKET + 1;
	assert_rejected();
	setup(state);
	fixture.sockets = 0;
	assert_rejected();
	setup(state);
	fixture.dies++;
	assert_rejected();
	setup(state);
	fixture.socket_dies[0] = 2;
	assert_rejected();
	setup(state);
	fixture.socket_roots[0] = 9;
	assert_rejected();
	setup(state);
	fixture.die_roots[0]--;
	assert_rejected();
	setup(state);
	fixture.roots--;
	assert_rejected();
}

static void test_invalid_and_duplicate_roots(void **state)
{
	fixture.rb[0][0].physical = 8;
	assert_rejected();
	setup(state);
	fixture.rb[0][0].physical = fixture.rb[0][1].physical;
	assert_rejected();
	setup(state);
	fixture.rb[0][0].fabric = 0x1000;
	assert_rejected();
	setup(state);
	fixture.rb[0][0].fabric = fixture.rb[0][1].fabric;
	assert_rejected();
	setup(state);
	fixture.rb[0][0].base = fixture.rb[0][1].limit;
	assert_rejected();
}

static void test_invalid_bus_ranges(void **state)
{
	static const uint32_t invalid[][2] = {
		{ 0x80, 0x7f }, { 0x80, 0x100 }, { 0, UINT32_MAX },
		{ 0x1000, 0x101f }, { CONFIG_ECAM_MMCONF_BUS_NUMBER, CONFIG_ECAM_MMCONF_BUS_NUMBER + 31 },
	};
	for (unsigned int i = 0; i < ARRAY_SIZE(invalid); i++) {
		setup(state);
		fixture.rb[0][0].base = invalid[i][0];
		fixture.rb[0][0].limit = invalid[i][1];
		assert_rejected();
	}
}

static void test_sparse_and_undecoded_roots(void **state)
{
	fixture.rb[0][3].base = UINT32_MAX; /* Physical RB0, domain 7. */
	fixture.rb[0][3].limit = 0xff; /* openSIL's no-map limit. */
	fixture.socket_roots[0]--;
	fixture.die_roots[0]--;
	fixture.roots--; /* Omit final openSIL entry: DF physical root 6, domain 2. */
	assert_int_equal(turin_fabric_init(), CB_SUCCESS);
	assert_null(turin_get_root_bridge(7));
	assert_null(turin_get_root_bridge(2));
	assert_non_null(turin_get_root_bridge(3));
	assert_null(get_domain_iohc_info(&domains[7]));
	turin_pci_domain_scan_bus(&domains[7]);
	assert_null(scanned);
	assert_false(domains[7].enabled);
	assert_int_equal(buses[7].secondary, 0);
	assert_int_equal(buses[7].max_subordinate, 0);
}

static void test_primary_roots_required(void **state)
{
	fixture.rb[0][1].base = UINT32_MAX;
	assert_rejected();
	setup(state);
	fixture.fch.Index = 3;
	assert_rejected();
	setup(state);
	fixture.smu.Index = 8;
	assert_rejected();
	setup(state);
	fixture.rb[0][1].fch = false;
	assert_rejected();
	setup(state);
	fixture.rb[0][1].smu = false;
	assert_rejected();
	setup(state);
	fixture.rb[0][1].base = 1;
	assert_rejected();
}

static void test_failed_rediscovery_unpublishes_cache(void **state)
{
	assert_int_equal(turin_fabric_init(), CB_SUCCESS);
	fixture.failed_api = 4;
	assert_rejected();
	turin_pci_domain_scan_bus(&domains[0]);
	assert_ptr_equal(scanned, &domains[0]);
	assert_true(domains[0].enabled);
	scanned = NULL;
	turin_pci_domain_scan_bus(&domains[1]);
	assert_null(scanned);
	assert_false(domains[1].enabled);
}

#if CONFIG_MAX_SOCKET == 2
static void test_socket_is_independent_of_segment(void **state)
{
	/* Both sockets fit in one segment; each root gets sixteen buses. */
	for (unsigned int socket = 0; socket < 2; socket++) {
		for (unsigned int index = 0; index < 8; index++) {
			struct root_fixture *rb = &fixture.rb[socket][index];
			rb->base = rb->base / BUSES_PER_ROOT * 16;
			rb->limit = rb->base + 15;
		}
	}
	assert_int_equal(turin_fabric_init(), CB_SUCCESS);
	assert_int_equal(turin_get_root_bridge(8)->segment_group, 0);
	assert_int_equal(turin_get_root_bridge(8)->bus_base, 0x80);
	assert_int_equal(turin_get_root_bridge(8)->socket, 1);
	assert_int_equal(get_domain_iohc_info(&domains[8])->smn_dev, PCI_DEV(0x80, 0, 0));
#if CONFIG_ECAM_MMCONF_BUS_NUMBER > 256
	/* Conversely, a socket-0 root may use a different segment from its FCH. */
	fixture.rb[0][0].base = 0x180;
	fixture.rb[0][0].limit = 0x18f;
	assert_int_equal(turin_fabric_init(), CB_SUCCESS);
	assert_int_equal(turin_get_root_bridge(6)->socket, 0);
	assert_int_equal(turin_get_root_bridge(6)->segment_group, 1);
#endif
}

static void test_unpopulated_second_socket(void **state)
{
	fixture.sockets = 1;
	fixture.dies = 1;
	fixture.roots = 8;
	assert_int_equal(turin_fabric_init(), CB_SUCCESS);
	assert_int_equal(turin_get_socket_count(), 1);
	assert_null(turin_get_root_bridge(8));
	turin_pci_domain_scan_bus(&domains[8]);
	assert_null(scanned);
	assert_false(domains[8].enabled);
}
#endif

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_fch_fallback_scan, setup),
		cmocka_unit_test_setup(test_discovery_and_scan, setup),
		cmocka_unit_test_setup(test_sp5_df_topology, setup),
		cmocka_unit_test_setup(test_sp5_mismatched_roles_rejected, setup),
		cmocka_unit_test_setup(test_opensil_errors_and_root_count, setup),
		cmocka_unit_test_setup(test_invalid_and_duplicate_roots, setup),
		cmocka_unit_test_setup(test_invalid_bus_ranges, setup),
		cmocka_unit_test_setup(test_sparse_and_undecoded_roots, setup),
		cmocka_unit_test_setup(test_primary_roots_required, setup),
		cmocka_unit_test_setup(test_failed_rediscovery_unpublishes_cache, setup),
#if CONFIG_MAX_SOCKET == 2
		cmocka_unit_test_setup(test_socket_is_independent_of_segment, setup),
		cmocka_unit_test_setup(test_unpopulated_second_socket, setup),
#endif
	};
	return cb_run_group_tests(tests, NULL, NULL);
}
