/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <arch/smp/mpspec.h>
#include <assert.h>
#include <cbmem.h>
#include <cpu/intel/turbo.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <intelblocks/acpi.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/pmclib.h>
#include <soc/acpi.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <hob_iiouds.h>

int soc_madt_sci_irq_polarity(int sci)
{
	if (sci >= 20)
		return MP_IRQ_POLARITY_LOW;
	else
		return MP_IRQ_POLARITY_HIGH;
}

uint32_t soc_read_sci_irq_select(void)
{
	/* PMC controller is hidden - hence PWRMBASE can't be accessbile using PCI cfg space */
	uintptr_t pmc_bar = PCH_PWRM_BASE_ADDRESS;
	return read32((void *)pmc_bar + PMC_ACPI_CNT);
}

void soc_fill_fadt(acpi_fadt_t *fadt)
{
	const uint16_t pmbase = ACPI_BASE_ADDRESS;

	fadt->FADT_MinorVersion = 1;
	fadt->pm_tmr_blk = pmbase + PM1_TMR;
	fadt->pm_tmr_len = 4;
	/* Clear flags set by common/block/acpi/acpi.c acpi_fill_fadt() */
	fadt->flags &= ~ACPI_FADT_SEALED_CASE;

	fadt->preferred_pm_profile = PM_ENTERPRISE_SERVER;
	fadt->pm2_cnt_blk = pmbase + PM2_CNT;
	fadt->pm2_cnt_len = 1;

	fadt->x_pm1b_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1b_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;

	/* PM2 Control Registers */
	fadt->x_pm2_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm2_cnt_blk.bit_width = fadt->pm2_cnt_len * 8;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_pm2_cnt_blk.addrl = fadt->pm2_cnt_blk;
	fadt->x_pm2_cnt_blk.addrh = 0x0;

	/* PM1 Timer Register */
	fadt->x_pm_tmr_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm_tmr_blk.bit_width = fadt->pm_tmr_len * 8;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm_tmr_blk.addrl = fadt->pm_tmr_blk;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	fadt->x_gpe0_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe0_blk.bit_width = fadt->gpe0_blk_len * 8;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_gpe0_blk.addrl = fadt->gpe0_blk;
	fadt->x_gpe0_blk.addrh = 0x0;

	fadt->x_gpe1_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe1_blk.bit_width = fadt->gpe1_blk_len * 8;
	fadt->x_gpe1_blk.bit_offset = 0;
	fadt->x_gpe1_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_gpe1_blk.addrl = fadt->gpe1_blk;
	fadt->x_gpe1_blk.addrh = 0x0;
}

static void create_dsdt_iou_pci_resource(uint8_t socket, uint8_t stack, const STACK_RES *ri,
					 bool stack_enabled)
{
	/*
	   Stacks 0 (TYPE_UBOX_IIO)
		Scope: PC<socket><stack>, ResourceTemplate: P0RS
	   Stacks 1 .. 5 (TYPE_UBOX_IIO)
		Scope: PC<socket><stack>, ResourceTemplate: RBRS
	*/

	/* Write ResourceTemplate resource name */
	char tres[16];
	snprintf(tres, sizeof(tres), "PT%d%X", socket, stack);
	acpigen_write_name(tres);

	printk(BIOS_DEBUG, "\tCreating ResourceTemplate %s for socket: %d, stack: %d\n", tres,
	       socket, stack);

	acpigen_write_resourcetemplate_header();

	/* Bus Resource */
	if (stack_enabled) {
		/* For stack with CXL device, the PCIe bus resource is BusBase only. */
		if (is_iio_cxl_stack_res(ri))
			acpigen_resource_word(2, 0xc, 0, 0, ri->BusBase, ri->BusBase, 0x0, 1);
		else
			acpigen_resource_word(2, 0xc, 0, 0, ri->BusBase, ri->BusLimit, 0x0,
					      (ri->BusLimit - ri->BusBase + 1));
	} else {
		acpigen_resource_word(2, 0, 0, 0, 0, 0, 0, 0);
	}

	/* Additional IO resources on socket 0 bus 0 */
	if (socket == 0 && stack == 0) {
		/* ACPI 6.4.2.5 I/O Port Descriptor */
		acpigen_write_io16(0xCF8, 0xCFF, 0x1, 0x8, 1);

		/* IO decode  CF8-CFF */
		acpigen_resource_word(1, 0xc, 0x3, 0, 0x0000, 0x03AF, 0, 0x03B0);
		acpigen_resource_word(1, 0xc, 0x3, 0, 0x03E0, 0x0CF7, 0, 0x0918);
		acpigen_resource_word(1, 0xc, 0x3, 0, 0x03B0, 0x03BB, 0, 0x000C);
		acpigen_resource_word(1, 0xc, 0x3, 0, 0x03C0, 0x03DF, 0, 0x0020);
	}

	/* IO resource */
	if (stack_enabled) {
		acpigen_resource_word(1, 0xc, 0x3, 0, ri->PciResourceIoBase,
				      ri->PciResourceIoLimit, 0x0,
				      (ri->PciResourceIoLimit - ri->PciResourceIoBase + 1));

		/* Additional Mem32 resources on socket 0 bus 0 */
		if (socket == 0 && stack == 0) {
			acpigen_resource_dword(0, 0xc, 3, 0, VGA_BASE_ADDRESS,
					       (VGA_BASE_ADDRESS + VGA_BASE_SIZE - 1), 0x0,
					       VGA_BASE_SIZE);
			acpigen_resource_dword(0, 0xc, 1, 0, SPI_BASE_ADDRESS,
					       (SPI_BASE_ADDRESS + SPI_BASE_SIZE - 1), 0x0,
					       SPI_BASE_SIZE);
		}

		/* Mem32 resource */
		acpigen_resource_dword(
			0, 0xc, 1, 0, ri->PciResourceMem32Base, ri->PciResourceMem32Limit, 0x0,
			(ri->PciResourceMem32Limit - ri->PciResourceMem32Base + 1));

		/* Mem64 resource */
		acpigen_resource_qword(
			0, 0xc, 1, 0, ri->PciResourceMem64Base, ri->PciResourceMem64Limit, 0x0,
			(ri->PciResourceMem64Limit - ri->PciResourceMem64Base + 1));
	} else {
		/* Zeroed IO resource */
		acpigen_resource_word(1, 0, 3, 0, 0, 0, 0, 0);

		/* Zeroed Mem32 resource */
		acpigen_resource_dword(0, 0, 1, 0, 0, 0, 0, 0);

		/* Zeroed Mem64 resource */
		acpigen_resource_dword(0, 0, 1, 0, 0, 0, 0, 0);
	}

	acpigen_write_resourcetemplate_footer();
}

static void create_dsdt_iou_cxl_resource(uint8_t socket, uint8_t stack, const STACK_RES *ri, bool stack_enabled)
{
	/*
	   Stacks 1 .. 5 (TYPE_UBOX_IIO)
		Scope: CX<socket><stack>, ResourceTemplate: RBRS
	*/
	/* write ResourceTemplate resource name */
	char tres[16];
	snprintf(tres, sizeof(tres), "CT%d%X", socket, stack);
	acpigen_write_name(tres);

	printk(BIOS_DEBUG, "\tCreating ResourceTemplate %s for socket: %d, stack: %d\n", tres,
	       socket, stack);

	acpigen_write_resourcetemplate_header();

	if (stack_enabled) {
		/* bus resource, from (BusBase + 1) to BusLimit */
		acpigen_resource_word(2, 0xc, 0, 0, ri->BusBase + 1, ri->BusLimit, 0x0,
				      (ri->BusLimit - ri->BusBase));

		/* IO resource */
		acpigen_resource_word(1, 0xc, 0x3, 0, ri->IoBase, ri->PciResourceIoBase - 1,
				      0x0, ri->PciResourceIoBase - ri->IoBase);

		/* Mem32 resource */
		acpigen_resource_dword(0, 0xc, 1, 0, ri->Mmio32Base,
				       ri->PciResourceMem32Base - 1, 0x0,
				       ri->PciResourceMem32Base - ri->Mmio32Base);

		/* Mem64 resource */
		acpigen_resource_qword(0, 0xc, 1, 0, ri->Mmio64Base,
				       ri->PciResourceMem64Base - 1, 0x0,
				       ri->PciResourceMem64Base - ri->Mmio64Base);
	} else {
		/* bus resource, from (BusBase + 1) to BusLimit */
		acpigen_resource_word(2, 0, 0, 0, 0, 0, 0, 0);

		/* IO resource */
		acpigen_resource_word(1, 0, 3, 0, 0, 0, 0, 0);

		/* Mem32 resource */
		acpigen_resource_dword(0, 0, 1, 0, 0, 0, 0, 0);

		/* Mem64 resource */
		acpigen_resource_qword(0, 0, 1, 0, 0, 0, 0, 0);
	}

	acpigen_write_resourcetemplate_footer();
}

static void create_dsdt_dino_resource(uint8_t socket, uint8_t stack, const STACK_RES *ri, bool stack_enabled)
{
	/*
	   Stacks 8 .. B (TYPE_DINO)
		Scope: DI<socket><stack> for DINO, ResourceTemplate: DT
		Scope: CP<socket><stack> for CPM (i.e., QAT), ResourceTemplate: MT
		Scope: HQ<socket><stack> for HQM (i.e., DLB), ResourceTemplate: HT
	*/

	enum {
		DSDT_DINO = 0,
		DSDT_CPM,
		DSDT_HQM,
		DSDT_CPM1,
		DSDT_HQM1
	};
	uint8_t rlist[] = {DSDT_DINO, DSDT_CPM, DSDT_HQM, DSDT_CPM1, DSDT_HQM1};

	for (int i = 0; i < ARRAY_SIZE(rlist); ++i) {
		uint8_t bus_base, bus_limit;
		uint64_t mem64_base, mem64_limit;
		char tres[16];

		/* Note, This allocates the resources in a different order than
		 * coreboot (DINO base is last). This causes the kernel to
		 * reallocate the DINO BARs.
		 * TODO: Use the resource settings from coreboot */
		if (rlist[i] == DSDT_DINO) {
			bus_base = ri->BusBase;
			bus_limit = ri->BusBase;
			mem64_base = ri->PciResourceMem64Base + CPM_MMIO_SIZE + HQM_MMIO_SIZE
				     + CPM_MMIO_SIZE + HQM_MMIO_SIZE;
			mem64_limit = ri->PciResourceMem64Limit;
			snprintf(tres, sizeof(tres), "DT%d%X", socket, stack);
		} else if (rlist[i] == DSDT_CPM) {
			bus_base = ri->BusBase + CPM_BUS_OFFSET;
			bus_limit = bus_base + CPM_RESERVED_BUS;
			mem64_base = ri->PciResourceMem64Base;
			mem64_limit = mem64_base + CPM_MMIO_SIZE - 1;
			snprintf(tres, sizeof(tres), "MT%d%X", socket, stack);
		} else if (rlist[i] == DSDT_HQM) {
			bus_base = ri->BusBase + HQM_BUS_OFFSET;
			bus_limit = bus_base + HQM_RESERVED_BUS;
			mem64_base = ri->PciResourceMem64Base + CPM_MMIO_SIZE;
			mem64_limit = mem64_base + HQM_MMIO_SIZE - 1;
			snprintf(tres, sizeof(tres), "HT%d%X", socket, stack);
		} else if (rlist[i] == DSDT_CPM1) {
			bus_base = ri->BusBase + CPM1_BUS_OFFSET;
			bus_limit = bus_base + CPM_RESERVED_BUS;
			mem64_base = ri->PciResourceMem64Base + CPM_MMIO_SIZE + HQM_MMIO_SIZE;
			mem64_limit = mem64_base + CPM_MMIO_SIZE - 1;
			snprintf(tres, sizeof(tres), "MU%d%X", socket, stack);
		} else { // DSDT_HQM1
			bus_base = ri->BusBase + HQM1_BUS_OFFSET;
			bus_limit = bus_base + HQM_RESERVED_BUS;
			mem64_base = ri->PciResourceMem64Base + CPM_MMIO_SIZE + HQM_MMIO_SIZE
				     + CPM_MMIO_SIZE;
			mem64_limit = mem64_base + HQM_MMIO_SIZE - 1;
			snprintf(tres, sizeof(tres), "HU%d%X", socket, stack);
		}

		printk(BIOS_DEBUG,
		       "\tCreating Dino ResourceTemplate %s for socket: %d, "
		       "stack: %d\n bus_base:0x%x, bus_limit:0x%x\n",
		       tres, socket, stack, bus_base, bus_limit);

		acpigen_write_name(tres);
		acpigen_write_resourcetemplate_header();

		if (stack_enabled) {
			acpigen_resource_word(2, 0xc, 0, 0, bus_base, bus_limit, 0x0,
					      (bus_limit - bus_base + 1));

			/* Mem32 resource */
			if (rlist[i] == DSDT_DINO)
				acpigen_resource_dword(0, 0xc, 1, 0, ri->PciResourceMem32Base,
						       ri->PciResourceMem32Limit, 0x0,
						       (ri->PciResourceMem32Limit
							- ri->PciResourceMem32Base + 1));

			/* Mem64 resource */
			acpigen_resource_qword(0, 0xc, 1, 0, mem64_base, mem64_limit, 0,
					       (mem64_limit - mem64_base + 1));
		} else {
			acpigen_resource_word(2, 0, 0, 0, 0, 0, 0, 0);

			/* Mem32 resource */
			if (rlist[i] == DSDT_DINO)
				acpigen_resource_dword(0, 0, 1, 0, 0, 0, 0, 0);

			/* Mem64 resource */
			acpigen_resource_qword(0, 0, 1, 0, 0, 0, 0, 0);
		}

		acpigen_write_resourcetemplate_footer();
	}
}

static void create_dsdt_ubox_resource(uint8_t socket, uint8_t stack, const STACK_RES *ri, bool stack_enabled)
{
	/*
	   Stacks D .. E (TYPE_UBOX)
		Scope: UC/UD<socket><0..1> for UBOX[1-2], ResourceTemplate: UT/UU
	*/

	for (int i = 0; i < 2; ++i) {
		char tres[16];
		/* write ResourceTemplate resource name */
		if (i == 0)
			snprintf(tres, sizeof(tres), "UT%d%X", socket, stack);
		else
			snprintf(tres, sizeof(tres), "UU%d%X", socket, stack);

		printk(BIOS_DEBUG, "\tCreating ResourceTemplate %s for socket: %d, stack: %d\n",
		       tres, socket, stack);

		acpigen_write_name(tres);
		acpigen_write_resourcetemplate_header();

		if (!stack_enabled)
			acpigen_resource_word(2, 0, 0, 0, 0, 0, 0, 0);
		else if (i == 0)
			acpigen_resource_word(2, 0xc, 0, 0, ri->BusBase, ri->BusBase, 0x0, 1);
		else
			acpigen_resource_word(2, 0xc, 0, 0, ri->BusBase + 1, ri->BusBase + 1,
					      0x0, 1);

		acpigen_write_resourcetemplate_footer();
	}
}


/*
 * Add a DSDT ACPI Name field for STACK enable setting.
 *  This is retrieved by the device _STA defined in iiostack.asl
 */
static void create_dsdt_stack_sta(uint8_t socket, uint8_t stack, const STACK_RES *ri, bool stack_enabled)
{
	char stack_sta[16];
	snprintf(stack_sta, sizeof(stack_sta), "ST%d%X", socket, stack);

	if (!stack_enabled)
		acpigen_write_name_integer(stack_sta, ACPI_STATUS_DEVICE_ALL_OFF);
	else
		acpigen_write_name_integer(stack_sta, ACPI_STATUS_DEVICE_ALL_ON);
}

void uncore_inject_dsdt(const struct device *device)
{
	bool stack_enabled;

	/* Only add RTxx entries once. */
	if (device->bus->secondary != 0)
		return;

	/*
	   Write stack scope - this needs to match RP ACPI scopes.
	   Stacks 0 (TYPE_UBOX_IIO)
		Scope: PC<socket><stack>, ResourceTemplate: P0RS
	   Stacks 1 .. 5 (TYPE_UBOX_IIO)
		Scope: PC<socket><stack> & CX<socket><stack>, ResourceTemplate: RBRS
	   Stacks 8 .. B (TYPE_DINO)
		Scope: DI<socket><stack> for DINO, ResourceTemplate: RBRS
		Scope: CP<socket><stack> for CPM (i.e., QAT), ResourceTemplate: RBRS
		Scope: HQ<socket><stack> for HQM (i.e., DLB), ResourceTemplate: RBRS
	   Stacks D .. E (TYPE_UBOX)
		Scope: UC<socket><0..1> for UBOX[1-2], ResourceTemplate: UNRS
	*/

	printk(BIOS_DEBUG, "%s device: %s\n", __func__, dev_path(device));

	acpigen_write_scope("\\_SB");

	/* The _CSR generation must match SPR iiostack.asl. */
	const IIO_UDS *hob = get_iio_uds();
	/* Iterate over CONFIG_MAX_SOCKET to keep ASL templates and DSDT injection in sync */
	for (uint8_t socket = 0; socket < CONFIG_MAX_SOCKET; ++socket) {
		for (int stack = 0; stack < MAX_LOGIC_IIO_STACK; ++stack) {
			const STACK_RES *ri =
				&hob->PlatformData.IIO_resource[socket].StackRes[stack];

			stack_enabled = hob->PlatformData.IIO_resource[socket].Valid &&
					ri->Personality < TYPE_RESERVED;

			printk(BIOS_DEBUG, "%s processing socket: %d, stack: %d, type: %d\n",
			       __func__, socket, stack, ri->Personality);

			if (stack <= IioStack5) { // TYPE_UBOX_IIO
				create_dsdt_iou_pci_resource(socket, stack, ri, stack_enabled);
				create_dsdt_iou_cxl_resource(socket, stack, ri, stack_enabled);
				create_dsdt_stack_sta(socket, stack, ri, stack_enabled);
			} else if (stack >= IioStack8 && stack <= IioStack11) { // TYPE_DINO
				create_dsdt_dino_resource(socket, stack, ri, stack_enabled);
				create_dsdt_stack_sta(socket, stack, ri, stack_enabled);
			} else if (stack == IioStack13) { // TYPE_UBOX
				create_dsdt_ubox_resource(socket, stack, ri, stack_enabled);
				create_dsdt_stack_sta(socket, stack, ri, stack_enabled);
			}
		}
	}

	acpigen_pop_len();
}

/* TODO: See if we can use the common generate_p_state_entries */
void soc_power_states_generation(int core, int cores_per_package)
{
	int ratio_min, ratio_max, ratio_turbo, ratio_step;
	int coord_type, power_max, power_unit, num_entries;
	int ratio, power, clock, clock_max;
	msr_t msr;

	/* Determine P-state coordination type from MISC_PWR_MGMT[0] */
	msr = rdmsr(MSR_MISC_PWR_MGMT);
	if (msr.lo & MISC_PWR_MGMT_EIST_HW_DIS)
		coord_type = SW_ANY;
	else
		coord_type = HW_ALL;

	/* Get bus ratio limits and calculate clock speeds */
	msr = rdmsr(MSR_PLATFORM_INFO);
	ratio_min = (msr.hi >> (40 - 32)) & 0xff; /* Max Efficiency Ratio */

	/* Determine if this CPU has configurable TDP */
	if (cpu_config_tdp_levels()) {
		/* Set max ratio to nominal TDP ratio */
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		ratio_max = msr.lo & 0xff;
	} else {
		/* Max Non-Turbo Ratio */
		ratio_max = (msr.lo >> 8) & 0xff;
	}
	clock_max = ratio_max * CONFIG_CPU_BCLK_MHZ;

	/* Calculate CPU TDP in mW */
	msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 2 << ((msr.lo & 0xf) - 1);
	msr = rdmsr(MSR_PKG_POWER_SKU);
	power_max = ((msr.lo & 0x7fff) / power_unit) * 1000;

	/* Write _PCT indicating use of FFixedHW */
	acpigen_write_empty_PCT();

	/* Write _PPC with no limit on supported P-state */
	acpigen_write_PPC_NVS();

	/* Write PSD indicating configured coordination type */
	acpigen_write_PSD_package(core, 1, coord_type);

	/* Add P-state entries in _PSS table */
	acpigen_write_name("_PSS");

	/* Determine ratio points */
	ratio_step = PSS_RATIO_STEP;
	num_entries = ((ratio_max - ratio_min) / ratio_step) + 1;
	if (num_entries > PSS_MAX_ENTRIES) {
		ratio_step += 1;
		num_entries = ((ratio_max - ratio_min) / ratio_step) + 1;
	}

	/* P[T] is Turbo state if enabled */
	if (get_turbo_state() == TURBO_ENABLED) {
		/* _PSS package count including Turbo */
		acpigen_write_package(num_entries + 2);

		msr = rdmsr(MSR_TURBO_RATIO_LIMIT);
		ratio_turbo = msr.lo & 0xff;

		/* Add entry for Turbo ratio */
		acpigen_write_PSS_package(clock_max + 1,	  /* MHz */
					  power_max,		  /* mW */
					  PSS_LATENCY_TRANSITION, /* lat1 */
					  PSS_LATENCY_BUSMASTER,  /* lat2 */
					  ratio_turbo << 8,	  /* control */
					  ratio_turbo << 8);	  /* status */
	} else {
		/* _PSS package count without Turbo */
		acpigen_write_package(num_entries + 1);
	}

	/* First regular entry is max non-turbo ratio */
	acpigen_write_PSS_package(clock_max,		  /* MHz */
				  power_max,		  /* mW */
				  PSS_LATENCY_TRANSITION, /* lat1 */
				  PSS_LATENCY_BUSMASTER,  /* lat2 */
				  ratio_max << 8,	  /* control */
				  ratio_max << 8);	  /* status */

	/* Generate the remaining entries */
	for (ratio = ratio_min + ((num_entries - 1) * ratio_step); ratio >= ratio_min;
	     ratio -= ratio_step) {

		/* Calculate power at this ratio */
		power = common_calculate_power_ratio(power_max, ratio_max, ratio);
		clock = ratio * CONFIG_CPU_BCLK_MHZ;
		// clock = 1;
		acpigen_write_PSS_package(clock,		  /* MHz */
					  power,		  /* mW */
					  PSS_LATENCY_TRANSITION, /* lat1 */
					  PSS_LATENCY_BUSMASTER,  /* lat2 */
					  ratio << 8,		  /* control */
					  ratio << 8);		  /* status */
	}

	/* Fix package length */
	acpigen_pop_len();
}

unsigned long xeonsp_acpi_create_madt_lapics(unsigned long current)
{
	struct device *cpu;
	uint8_t num_cpus = 0;

	for (cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC)
		    || (cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled)
			continue;
		current = acpi_create_madt_one_lapic(current, num_cpus, cpu->path.apic.apic_id);
		num_cpus++;
	}

	return current;
}

unsigned long acpi_fill_cedt(unsigned long current)
{
	const IIO_UDS *hob = get_iio_uds();
	union uid {
		uint32_t data;
		struct {
			uint8_t byte0;
			uint8_t byte1;
			uint8_t byte2;
			uint8_t byte3;
		};
	} cxl_uid;
	u32 cxl_ver;
	u64 base;

	cxl_uid.byte0 = 'C';
	cxl_uid.byte1 = 'X';
	/* Loop through all sockets and stacks, add CHBS for each CXL IIO stack */
	for (uint8_t s = 0; s < hob->PlatformData.numofIIO; ++s) {
		for (int x = 0; x < MAX_LOGIC_IIO_STACK; ++x) {
			const STACK_RES *ri = &hob->PlatformData.IIO_resource[s].StackRes[x];
			if (!is_iio_cxl_stack_res(ri))
				continue;
			/* uid needs to match with ACPI CXL device ID, eg. acpi/iiostack.asl */
			cxl_uid.byte2 = s + '0';
			cxl_uid.byte3 = x + '0';
			cxl_ver = ACPI_CEDT_CHBS_CXL_VER_1_1;
			base = ri->Mmio32Base; /* DP RCRB base */
			current += acpi_create_cedt_chbs((acpi_cedt_chbs_t *)current,
							 cxl_uid.data, cxl_ver, base);
		}
	}

	return current;
}
