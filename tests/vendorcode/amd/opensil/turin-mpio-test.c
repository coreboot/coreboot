/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci_def.h>
#include <drivers/amd/opensil/opensil.h>
#include <Mpio/Common/MpioLibLocal.h>
#include <Mpio/MpioClass-api.h>
#include <Nbio/NbioClass-api.h>
#include <Cxl/CxlClass-api.h>
#include <RcMgr/DfX/RcManager4-api.h>
#include <string.h>
#include <tests/test.h>
#include <vendorcode/amd/opensil/opensil.h>
#include <vendorcode/amd/opensil/turin_poc/mpio/chip.h>

#define DOMAIN_COUNT (CONFIG_MAX_SOCKET * PROJ_MAX_RBS_PER_SOCKET)
#define DEVICE_COUNT (MAX_PORTS_SUPPORTED + 4)
#define GUARD_VALUE 0xada55ada

/* Guards catch writes beyond openSIL's fixed-size input block. */
static struct {
	uint32_t before;
	MPIOCLASS_INPUT_BLK input;
	uint32_t after;
} mpio;
static DFX_RCMGR_INPUT_BLK rc;
static NBIOCLASS_DATA_BLOCK nbio;
static CXLCLASS_DATA_BLK cxl;
static bool missing_mpio;
static bool missing_rc;
static unsigned int errors;
static unsigned int warnings;

struct device dev_root;
static struct bus root_bus;
static struct device domains[DOMAIN_COUNT + 1];
static struct bus domain_buses[DOMAIN_COUNT + 1];
static struct device devices[DEVICE_COUNT];
static struct drivers_amd_opensil_mpio_config configs[DEVICE_COUNT];
static size_t device_count;

extern struct chip_operations drivers_amd_opensil_mpio_ops;

void *SilFindStructure(SIL_DATA_BLOCK_ID id, uint16_t instance)
{
	assert_int_equal(instance, 0);
	switch (id) {
	case SilId_MpioClass:
		return missing_mpio ? NULL : &mpio.input;
	case SilId_RcManager:
		return missing_rc ? NULL : &rc;
	case SilId_NbioClass:
		return &nbio;
	case SilId_CxlClass:
		return &cxl;
	default:
		fail_msg("Unexpected input-block lookup: %u", id);
		return NULL;
	}
}

int printk(int level, const char *fmt, ...)
{
	if (level <= BIOS_ERR)
		errors++;
	else if (level == BIOS_WARNING)
		warnings++;
	return 0;
}

static void clear_devices(void)
{
	memset(devices, 0, sizeof(devices));
	memset(configs, 0, sizeof(configs));
	device_count = 0;
	dev_root.next = NULL;
}

static int setup(void **state)
{
	memset(&mpio, 0, sizeof(mpio));
	memset(&rc, 0, sizeof(rc));
	memset(&nbio, 0, sizeof(nbio));
	memset(&cxl, 0, sizeof(cxl));
	memset(&dev_root, 0, sizeof(dev_root));
	memset(&root_bus, 0, sizeof(root_bus));
	memset(domains, 0, sizeof(domains));
	memset(domain_buses, 0, sizeof(domain_buses));
	mpio.before = GUARD_VALUE;
	mpio.after = GUARD_VALUE;
	missing_mpio = false;
	missing_rc = false;
	errors = 0;
	warnings = 0;
	dev_root.path.type = DEVICE_PATH_ROOT;
	dev_root.enabled = true;
	dev_root.upstream = &root_bus;
	dev_root.downstream = &root_bus;
	root_bus.dev = &dev_root;
	for (size_t i = 0; i < ARRAY_SIZE(domains); i++) {
		domains[i].path.type = DEVICE_PATH_DOMAIN;
		domains[i].path.domain.domain_id = i;
		domains[i].upstream = &root_bus;
		domains[i].downstream = &domain_buses[i];
		domain_buses[i].dev = &domains[i];
	}
	clear_devices();
	return 0;
}

static int teardown(void **state)
{
	assert_int_equal(mpio.before, GUARD_VALUE);
	assert_int_equal(mpio.after, GUARD_VALUE);
	return 0;
}

static struct device *add_port(unsigned int domain, uint8_t lane, uint8_t slot, bool bmc)
{
	assert_true(device_count < ARRAY_SIZE(devices));
	assert_true(domain < ARRAY_SIZE(domains));
	struct device *dev = &devices[device_count];
	struct drivers_amd_opensil_mpio_config *config = &configs[device_count];

	*config = (struct drivers_amd_opensil_mpio_config) {
		.type = IFTYPE_PCIE,
		.start_lane = lane,
		.end_lane = lane,
		.gpio_group = 1,
		.speed = Gen4,
		.aspm = L1,
		.bmc = bmc,
	};
	dev->path.type = DEVICE_PATH_PCI;
	dev->path.pci.devfn = PCI_DEVFN(3, device_count % 7 + 1);
	dev->chip_info = config;
	dev->chip_ops = &drivers_amd_opensil_mpio_ops;
	dev->upstream = &domain_buses[domain];
	dev->enabled = true;
	if (device_count)
		devices[device_count - 1].next = dev;
	else
		dev_root.next = dev;
	device_count++;
	return dev;
}

static MPIO_PORT_DESCRIPTOR *socket_list(unsigned int socket)
{
	MPIO_PORT_DESCRIPTOR *list = NULL;

	/* Use openSIL's socket lookup and its end-of-list markers. */
	assert_int_equal(MpioGetTopologyForSocket(socket,
			 mpio.input.PcieTopologyData.PlatformData, &list), SilPass);
	return list;
}

static void check_complexes(void)
{
	MPIO_COMPLEX_DESCRIPTOR *complex_desc = mpio.input.PcieTopologyData.PlatformData;

	for (size_t socket = 0; socket < CONFIG_MAX_SOCKET; socket++) {
		assert_non_null(complex_desc);
		assert_int_equal(complex_desc->SocketId, socket);
		assert_int_equal(complex_desc->Flags,
				 socket == CONFIG_MAX_SOCKET - 1 ? DESCRIPTOR_TERMINATE_LIST : 0);
		complex_desc = PcieConfigGetNextDataDescriptor(complex_desc);
	}
	assert_null(complex_desc);
}

static void check_list(unsigned int socket, const uint8_t *lanes, size_t count)
{
	MPIO_PORT_DESCRIPTOR *port = socket_list(socket);

	for (size_t i = 0; i < count; i++) {
		assert_non_null(port);
		assert_int_equal(port->EngineData.StartLane, lanes[i]);
		assert_int_equal(port->Flags, i == count - 1 ? DESCRIPTOR_TERMINATE_LIST : 0);
		port = PcieConfigGetNextDataDescriptor(port);
	}
	assert_null(port);
}

static void test_empty_socket_lists(void **state)
{
	configure_mpio();
	check_complexes();
	for (unsigned int socket = 0; socket < CONFIG_MAX_SOCKET; socket++)
		check_list(socket, NULL, 0);
	assert_int_equal(errors, 0);
}

static void test_single_socket_ports(void **state)
{
	add_port(0, 48, 7, false);
	struct device *disabled = add_port(7, 0, 0, false);
	disabled->enabled = false;
	add_port(5, 132, 9, false);
	configure_mpio();

	const uint8_t lanes[] = {48, 0, 132};
	check_complexes();
	check_list(0, lanes, ARRAY_SIZE(lanes));
	MPIO_PORT_DESCRIPTOR *ports = socket_list(0);
	assert_int_equal(ports[0].Port.PortPresent, MpioPortEnabled);
	assert_int_equal(ports[1].Port.PortPresent, MpioPortDisabled);
	assert_int_equal(ports[2].EngineData.EngineType, MpioPcieEngine);
	assert_int_equal(ports[0].Port.DeviceNumber, 3);
	assert_int_equal(ports[0].Port.FunctionNumber, 1);
	assert_int_equal(ports[0].Port.TargetLinkSpeed, MaxSupported);
	assert_int_equal(errors, 0);
}


#if CONFIG_MAX_SOCKET > 1
static void test_interleaved_sockets(void **state)
{
	/* Start on S1, then insert before it repeatedly, including after the last domain. */
	add_port(8, 0, 0, false);
	configs[0].end_lane = 7;
	configs[0].speed = Gen3;
	configs[0].gpio_group = 2;
	configs[0].hotplug = Basic;
	configs[0].aspm_l1_1 = true;
	configs[0].clock_pm = true;
	add_port(0, 48, 4, false);
	configs[1].end_lane = 63;
	configs[1].speed = Gen5;
	add_port(15, 48, 50, false);
	configs[2].aspm = aspm_disabled;
	add_port(7, 0, 0, false);
	add_port(13, 128, 7, false);
	configs[4].end_lane = 131;
	configure_mpio();

	const uint8_t s0_lanes[] = {48, 0};
	const uint8_t s1_lanes[] = {0, 48, 128};
	check_complexes();
	check_list(0, s0_lanes, ARRAY_SIZE(s0_lanes));
	check_list(1, s1_lanes, ARRAY_SIZE(s1_lanes));
	assert_ptr_equal(socket_list(0), &mpio.input.PcieTopologyData.PortList[0]);
	assert_ptr_equal(socket_list(1), &mpio.input.PcieTopologyData.PortList[2]);
	/* Check shifted descriptor contents as well as their socket and termination. */
	assert_int_equal(socket_list(1)[0].EngineData.EndLane, 7);
	assert_int_equal(socket_list(1)[0].EngineData.GpioGroupId, 2);
	assert_int_equal(socket_list(1)[0].EngineData.HotPluggable, 1);
	assert_int_equal(socket_list(1)[0].Port.FunctionNumber, 1);
	assert_int_equal(socket_list(1)[0].Port.LinkSpeedCapability, Gen3);
	assert_int_equal(socket_list(1)[0].Port.LinkHotplug, Basic);
	assert_int_equal(socket_list(1)[0].Port.LinkAspm, L1);
	assert_int_equal(socket_list(1)[0].Port.LinkAspmL1_1, 1);
	assert_int_equal(socket_list(1)[0].Port.MiscControls.ClkPmSupport, 1);
	assert_int_equal(socket_list(0)[0].EngineData.EndLane, 63);
	assert_int_equal(socket_list(0)[0].Port.LinkSpeedCapability, Gen5);
	assert_int_equal(socket_list(0)[0].Port.FunctionNumber, 2);
	assert_int_equal(socket_list(1)[1].Port.FunctionNumber, 3);
	assert_int_equal(socket_list(1)[1].Port.LinkAspm, aspm_disabled);
	assert_int_equal(socket_list(1)[2].EngineData.EndLane, 131);
	assert_int_equal(socket_list(1)[2].Port.FunctionNumber, 5);
	assert_int_equal(errors, 0);
}
#endif

static void test_sata_ignores_pcie_settings(void **state)
{
	add_port(0, 48, 0, false);
	add_port(5, 128, 0, false);
	configs[1].type = IFTYPE_SATA;
	configs[1].end_lane = 135;
	configs[1].hotplug = Basic;
	configs[1].aspm_l1_1 = true;
	configs[1].aspm_l1_2 = true;
	configs[1].clock_pm = true;
	add_port(7, 0, 0, false);
	configure_mpio();

	const uint8_t lanes[] = {48, 128, 0};
	check_complexes();
	check_list(0, lanes, ARRAY_SIZE(lanes));
	const MPIO_PORT_DESCRIPTOR *ports = socket_list(0);
	const MPIO_PORT_DATA sata_port = { .PortPresent = 1 };
	assert_memory_equal(&ports[1].Port, &sata_port, sizeof(sata_port));
	assert_int_equal(ports[1].EngineData.EngineType, MpioSATAEngine);
	assert_int_equal(ports[1].EngineData.EndLane, 135);
	assert_int_equal(ports[1].EngineData.GpioGroupId, 1);
	assert_int_equal(ports[1].EngineData.HotPluggable, 0);
	assert_int_equal(ports[0].Port.AlwaysExpose, 1);
	assert_int_equal(ports[2].Port.AlwaysExpose, 1);
	assert_int_equal(ports[0].Port.SlotNum, 1);
	assert_int_equal(ports[2].Port.SlotNum, 2);
	assert_int_equal(errors, 0);
}

static void test_bmc_socket_and_duplicate(void **state)
{
	unsigned int socket = CONFIG_MAX_SOCKET - 1;
	struct device *first = add_port(socket * PROJ_MAX_RBS_PER_SOCKET + 5, 135, 0, true);
	struct device *duplicate = add_port(5, 134, 0, true);
	opensil_mpio_global_config();
	opensil_mpio_per_device_config(first);
	assert_int_equal(mpio.input.EarlyBmcLinkSocket, socket);
	assert_int_equal(mpio.input.EarlyBmcLinkLaneNum, 135);
	assert_int_equal(rc.BmcSocket, socket);
	assert_int_equal(rc.EarlyBmcLinkLaneNum, 135);
	opensil_mpio_per_device_config(duplicate);
	assert_int_equal(errors, 1);
	assert_int_equal(mpio.input.EarlyBmcLinkSocket, socket);
	assert_int_equal(mpio.input.EarlyBmcLinkLaneNum, 135);
	assert_int_equal(rc.BmcSocket, socket);
	assert_int_equal(rc.EarlyBmcLinkLaneNum, 135);
	check_list(socket, NULL, 0);
}

static void test_bmc_missing_input_blocks(void **state)
{
	unsigned int socket = CONFIG_MAX_SOCKET - 1;
	struct device *bmc = add_port(socket * PROJ_MAX_RBS_PER_SOCKET + 5, 134, 0, true);
	opensil_mpio_global_config();
	missing_rc = true;
	opensil_mpio_per_device_config(bmc);
	assert_int_equal(errors, 1);
	assert_int_equal(mpio.input.EarlyBmcLinkLaneNum, CONFIG_OPENSIL_MPIO_EARLY_BMC_LINK_LANE);
	missing_rc = false;
	missing_mpio = true;
	opensil_mpio_per_device_config(bmc);
	assert_int_equal(errors, 2);
	assert_int_equal(rc.EarlyBmcLinkLaneNum, CONFIG_OPENSIL_MPIO_EARLY_BMC_LINK_LANE);
	missing_mpio = false;
	/* Failed attempts must not mark a BMC as already configured. */
	opensil_mpio_per_device_config(bmc);
	assert_int_equal(errors, 2);
	assert_int_equal(rc.BmcSocket, socket);
	assert_int_equal(rc.EarlyBmcLinkLaneNum, 134);
	assert_int_equal(mpio.input.EarlyBmcLinkSocket, socket);
	assert_int_equal(mpio.input.EarlyBmcLinkLaneNum, 134);
}

static void test_rejected_devices(void **state)
{
	struct device *outside = add_port(DOMAIN_COUNT, 135, 0, true);
	struct device *orphan = add_port(0, 48, 0, false);
	orphan->upstream = &root_bus;
	struct device *invalid = add_port(0, 32, 0, false);
	configs[2].type = 99;
	struct device *unused = add_port(0, 16, 0, false);
	configs[3].type = IFTYPE_UNUSED;
	struct device *valid = add_port(0, 128, 0, false);
	opensil_mpio_global_config();
	opensil_mpio_per_device_config(outside);
	opensil_mpio_per_device_config(orphan);
	opensil_mpio_per_device_config(invalid);
	opensil_mpio_per_device_config(unused);
	assert_false(unused->enabled);
	assert_int_equal(errors, 3);
	check_list(0, NULL, 0);
	assert_int_equal(mpio.input.EarlyBmcLinkSocket, 0);
	opensil_mpio_per_device_config(valid);
	const uint8_t lanes[] = {128};
	check_list(0, lanes, ARRAY_SIZE(lanes));
}


static void test_full_capacity_one_socket(void **state)
{
	unsigned int socket = CONFIG_MAX_SOCKET - 1;
	uint8_t lanes[MAX_PORTS_SUPPORTED];
	for (size_t i = 0; i < ARRAY_SIZE(lanes); i++) {
		lanes[i] = i;
		add_port(socket * PROJ_MAX_RBS_PER_SOCKET, lanes[i], 0, false);
	}
	configure_mpio();
	check_list(socket, lanes, ARRAY_SIZE(lanes));
	PCIe_PLATFORM_TOPOLOGY saved = mpio.input.PcieTopologyData;
	struct device *overflow = add_port(0, 128, 0, false);
	opensil_mpio_per_device_config(overflow);
	assert_int_equal(errors, 1);
	assert_memory_equal(&saved, &mpio.input.PcieTopologyData, sizeof(saved));
	check_complexes();
}

#if CONFIG_MAX_SOCKET > 1
static void test_full_capacity_interleaved(void **state)
{
	uint8_t even_lanes[MAX_PORTS_SUPPORTED / 2];
	uint8_t odd_lanes[MAX_PORTS_SUPPORTED / 2];
	for (size_t i = 0; i < MAX_PORTS_SUPPORTED; i++) {
		if (i % 2)
			odd_lanes[i / 2] = i;
		else
			even_lanes[i / 2] = i;
		add_port(i % 2 ? 0 : 8, i, 0, false);
	}
	configure_mpio();
	check_list(0, odd_lanes, ARRAY_SIZE(odd_lanes));
	check_list(1, even_lanes, ARRAY_SIZE(even_lanes));
	PCIe_PLATFORM_TOPOLOGY saved = mpio.input.PcieTopologyData;
	struct device *overflow = add_port(0, 128, 0, false);
	opensil_mpio_per_device_config(overflow);
	assert_int_equal(errors, 1);
	assert_memory_equal(&saved, &mpio.input.PcieTopologyData, sizeof(saved));
}
#endif

static void test_repeated_walk_clears_topology(void **state)
{
	unsigned int socket = CONFIG_MAX_SOCKET - 1;
	add_port(socket * PROJ_MAX_RBS_PER_SOCKET, 48, 50, false);
	add_port(socket * PROJ_MAX_RBS_PER_SOCKET + 5, 134, 0, true);
	configure_mpio();
	assert_int_equal(rc.BmcSocket, socket);
	assert_int_equal(rc.EarlyBmcLinkLaneNum, 134);
	clear_devices();
	configure_mpio();
	check_complexes();
	for (unsigned int i = 0; i < CONFIG_MAX_SOCKET; i++)
		check_list(i, NULL, 0);
	assert_int_equal(rc.BmcSocket, 0);
	assert_int_equal(rc.EarlyBmcLinkLaneNum, CONFIG_OPENSIL_MPIO_EARLY_BMC_LINK_LANE);
	assert_int_equal(mpio.input.EarlyBmcLinkSocket, 0);
	assert_int_equal(mpio.input.EarlyBmcLinkLaneNum, CONFIG_OPENSIL_MPIO_EARLY_BMC_LINK_LANE);
	add_port(0, 128, 0, false);
	configure_mpio();
	const uint8_t lanes[] = {128};
	check_list(0, lanes, ARRAY_SIZE(lanes));
	assert_int_equal(errors, 0);
}





int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(test_sata_ignores_pcie_settings, setup, teardown),
		cmocka_unit_test_setup_teardown(test_empty_socket_lists, setup, teardown),
		cmocka_unit_test_setup_teardown(test_single_socket_ports, setup, teardown),
#if CONFIG_MAX_SOCKET > 1
		cmocka_unit_test_setup_teardown(test_interleaved_sockets, setup, teardown),
#endif
		cmocka_unit_test_setup_teardown(test_bmc_socket_and_duplicate, setup, teardown),
		cmocka_unit_test_setup_teardown(test_bmc_missing_input_blocks, setup, teardown),
		cmocka_unit_test_setup_teardown(test_rejected_devices, setup, teardown),
		cmocka_unit_test_setup_teardown(test_full_capacity_one_socket, setup, teardown),
#if CONFIG_MAX_SOCKET > 1
		cmocka_unit_test_setup_teardown(test_full_capacity_interleaved, setup, teardown),
#endif
		cmocka_unit_test_setup_teardown(test_repeated_walk_clears_topology,
					     setup, teardown),
	};
	return cb_run_group_tests(tests, NULL, NULL);
}
