/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bert_storage.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/cpu_ids.h>
#include <delay.h>
#include <device/pci_ops.h>
#include <intelblocks/crashlog.h>
#include <intelblocks/pmc_ipc.h>
#include <soc/crashlog.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <string.h>

#define CONTROL_INTERFACE_OFFSET	0x5
#define CRASHLOG_NODES_COUNT		0x2  /* Crashlog record of PUNIT & Uncore of SoC-N Die */
#define CRASHLOG_PUNIT_STORAGE_OFF_MASK	BIT(24)
#define CRASHLOG_RE_ARM_STATUS_MASK	BIT(25)
#define CRASHLOG_CONSUMED_MASK		BIT(31)

/* global crashLog info */
static bool m_pmc_crashLog_support;
static bool m_pmc_crashLog_present;
static bool m_cpu_crashLog_support;
static bool m_cpu_crashLog_present;
static u32 m_pmc_crashLog_size;
static u32 m_cpu_crashLog_size;
static u32 cpu_crash_version;
static pmc_ipc_discovery_buf_t discovery_buf;
static pmc_crashlog_desc_table_t descriptor_table;
static tel_crashlog_devsc_cap_t cpu_cl_devsc_cap;
static cpu_crashlog_discovery_table_t cpu_cl_disc_tab;
static uintptr_t disc_tab_addr;

static u64 get_disc_tab_header(void)
{
	return read64p(disc_tab_addr);
}

/* Get the SRAM BAR. */
static uintptr_t get_sram_bar(pci_devfn_t sram_devfn)
{
	uintptr_t sram_bar;
	const struct device *dev;
	struct resource *res;

	dev = pcidev_path_on_root(sram_devfn);
	if (!dev) {
		printk(BIOS_ERR, "device: 0x%x not found!\n", sram_devfn);
		return 0;
	}

	res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res) {
		printk(BIOS_ERR, "SOC SRAM device not found!\n");
		return 0;
	}

	/* Get the base address of the resource */
	sram_bar = res->base;

	return sram_bar;
}

static void configure_sram(const struct device *sram_dev, uintptr_t base_addr)
{
	pci_update_config16(sram_dev, PCI_COMMAND, ~(PCI_COMMAND_IO | PCI_COMMAND_MEMORY), 0);

	/* Program BAR 0 and enable command register memory space decoding */
	pci_write_config32(sram_dev, PCI_BASE_ADDRESS_0, base_addr);
	pci_or_config16(sram_dev, PCI_COMMAND, PCI_COMMAND_MEMORY);
}

void cl_get_pmc_sram_data(cl_node_t *head)
{
	uintptr_t pmc_sram_base = cl_get_cpu_tmp_bar();
	uintptr_t ioe_sram_base = get_sram_bar(PCI_DEVFN_IOE_SRAM);
	u32 pmc_crashLog_size = cl_get_pmc_record_size();
	cl_node_t *cl_cur = head;

	if (!pmc_crashLog_size) {
		printk(BIOS_ERR, "No PMC crashlog records\n");
		return;
	}

	if (!pmc_sram_base) {
		printk(BIOS_ERR, "PMC SRAM base not valid\n");
		return;
	}

	if (!ioe_sram_base) {
		printk(BIOS_ERR, "IOE SRAM base not valid\n");
		return;
	}

	configure_sram(PCI_DEV_IOE_SRAM, ioe_sram_base);

	if (!cl_pmc_sram_has_mmio_access())
		return;

	if (!cl_ioe_sram_has_mmio_access())
		return;

	printk(BIOS_DEBUG, "PMC crashLog size : 0x%x\n", pmc_crashLog_size);

	/* goto tail node */
	while (cl_cur && cl_cur->next) {
		cl_cur = cl_cur->next;
	}

	/* process crashlog records */
	for (int i = 0; i < descriptor_table.numb_regions + 1; i++) {
		uintptr_t sram_base = 0;
		bool pmc_sram = true;
		printk(BIOS_DEBUG, "Region[0x%x].Tag=0x%x offset=0x%x, size=0x%x\n",
				i,
				descriptor_table.regions[i].bits.assign_tag,
				descriptor_table.regions[i].bits.offset,
				descriptor_table.regions[i].bits.size);

		if (!descriptor_table.regions[i].bits.size)
			continue;

		/*
		 * Region with metadata TAG contains information about BDF entry for SOC PMC SRAM
		 * and IOE SRAM. We don't need to parse this as we already define BDFs in
		 * soc/pci_devs.h for these SRAMs. Also we need to skip this region as it does not
		 * contain any crashlog data.
		 */
		if (descriptor_table.regions[i].bits.assign_tag ==
				CRASHLOG_DESCRIPTOR_TABLE_TAG_META) {
			pmc_crashLog_size -= descriptor_table.regions[i].bits.size *
						sizeof(u32);
			printk(BIOS_DEBUG, "Found metadata tag. PMC crashlog size adjusted to: 0x%x\n",
					pmc_crashLog_size);
			continue;
		} else {
			if (descriptor_table.regions[i].bits.assign_tag ==
					CRASHLOG_DESCRIPTOR_TABLE_TAG_SOC)
				sram_base = pmc_sram_base;
			else if (descriptor_table.regions[i].bits.assign_tag ==
					CRASHLOG_DESCRIPTOR_TABLE_TAG_IOE)
				sram_base = ioe_sram_base;
			else
				continue;

			cl_node_t *cl_node = malloc_cl_node(descriptor_table.regions[i].bits.size);

			if (!cl_node) {
				printk(BIOS_DEBUG, "failed to allocate cl_node [region = %d]\n", i);
				goto pmc_send_re_arm_after_reset;
			}

			if (cl_copy_data_from_sram(sram_base,
						descriptor_table.regions[i].bits.offset,
						descriptor_table.regions[i].bits.size,
						cl_node->data,
						i,
						pmc_sram)) {
				cl_cur->next = cl_node;
				cl_cur = cl_cur->next;
			} else {
				/* coping data from sram failed */
				pmc_crashLog_size -= descriptor_table.regions[i].bits.size *
									sizeof(u32);
				printk(BIOS_DEBUG, "PMC crashlog size adjusted to: 0x%x\n",
							pmc_crashLog_size);
				/* free cl_node */
				free_cl_node(cl_node);
			}
		}
	}

	update_new_pmc_crashlog_size(&pmc_crashLog_size);

pmc_send_re_arm_after_reset:
	/* when bit 7 of discov cmd resp is set -> bit 2 of size field */
	cl_pmc_re_arm_after_reset();

	/* Clear the SSRAM region after copying the error log */
	cl_pmc_clear();
}

bool pmc_cl_discovery(void)
{
	uintptr_t bar_addr = 0, desc_table_addr = 0;

	const struct pmc_ipc_buffer req = { 0 };
	struct pmc_ipc_buffer res;
	uint32_t cmd_reg;
	int r;

	cmd_reg = pmc_make_ipc_cmd(PMC_IPC_CMD_CRASHLOG,
				PMC_IPC_CMD_ID_CRASHLOG_DISCOVERY,
				PMC_IPC_CMD_SIZE_SHIFT);
	printk(BIOS_DEBUG, "cmd_reg from pmc_make_ipc_cmd %d in %s\n", cmd_reg, __func__);

	r = pmc_send_ipc_cmd(cmd_reg, &req, &res);

	if (r < 0) {
		printk(BIOS_ERR, "pmc_send_ipc_cmd failed in %s\n", __func__);
		return false;
	}
	discovery_buf.conv_val_64_bits = ((u64)res.buf[1] << 32) | res.buf[0];

	if ((discovery_buf.conv_bits64.supported != 1) ||
		(discovery_buf.conv_bits64.discov_mechanism == 0) ||
			(discovery_buf.conv_bits64.crash_dis_sts == 1)) {
		printk(BIOS_INFO, "PCH crashlog feature not supported.\n");
		m_pmc_crashLog_support = false;
		m_pmc_crashLog_size = 0;
		printk(BIOS_DEBUG, "discovery_buf supported: %d, mechanism: %d, CrashDisSts: %d\n",
			discovery_buf.conv_bits64.supported,
			discovery_buf.conv_bits64.discov_mechanism,
			discovery_buf.conv_bits64.crash_dis_sts);
		return false;
	}

	printk(BIOS_INFO, "PMC crashlog feature is supported.\n");
	m_pmc_crashLog_support = true;

	/* Program BAR 0 and enable command register memory space decoding */
	bar_addr = get_sram_bar(PCI_DEVFN_SRAM);
	if (bar_addr == 0) {
		printk(BIOS_ERR, "PCH SRAM not available, crashlog feature can't be enabled.\n");
		return false;
	}

	configure_sram(PCI_DEV_SRAM, bar_addr);

	desc_table_addr = bar_addr + discovery_buf.conv_bits64.desc_tabl_offset;
	m_pmc_crashLog_size = pmc_cl_gen_descriptor_table(desc_table_addr,
								&descriptor_table);
	printk(BIOS_DEBUG, "PMC CrashLog size in discovery mode: 0x%X\n",
					m_pmc_crashLog_size);
	m_pmc_crashLog_present = m_pmc_crashLog_size > 0;

	return true;
}

uintptr_t cl_get_cpu_bar_addr(void)
{
	uintptr_t base_addr = 0;
	if (cpu_cl_devsc_cap.discovery_data.fields.t_bir_q == TEL_DVSEC_TBIR_BAR0) {
		base_addr = pci_read_config32(PCI_DEV_TELEMETRY, PCI_BASE_ADDRESS_0) &
				~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	} else if (cpu_cl_devsc_cap.discovery_data.fields.t_bir_q == TEL_DVSEC_TBIR_BAR1) {
		base_addr = pci_read_config32(PCI_DEV_TELEMETRY, PCI_BASE_ADDRESS_1) &
				~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	} else {
		printk(BIOS_ERR, "Invalid TEL_CFG_BAR value %d, discovery failure expected.\n",
			cpu_cl_devsc_cap.discovery_data.fields.t_bir_q);
	}

	return base_addr;
}

uintptr_t cl_get_cpu_tmp_bar(void)
{
	return get_sram_bar(PCI_DEVFN_SRAM);
}

bool  cl_pmc_sram_has_mmio_access(void)
{
	if (pci_read_config16(PCI_DEV_SRAM, PCI_VENDOR_ID) == 0xFFFF) {
		printk(BIOS_ERR, "PMC SSRAM PCI device disabled. Can be enabled in device tree.\n");
		return false;
	}

	return true;
}

bool  cl_ioe_sram_has_mmio_access(void)
{
	if (pci_read_config16(PCI_DEV_IOE_SRAM, PCI_VENDOR_ID) == 0xFFFF) {
		printk(BIOS_ERR, "IOE SSRAM PCI device disabled. Can be enabled in device tree.\n");
		return false;
	}
	return true;
}

static bool cpu_cl_get_capability(tel_crashlog_devsc_cap_t *cl_devsc_cap)
{
	cl_devsc_cap->cap_data.data = pci_read_config32(PCI_DEV_TELEMETRY,
						TEL_DVSEC_OFFSET + TEL_DVSEC_PCIE_CAP_ID);
	if (cl_devsc_cap->cap_data.fields.pcie_cap_id != TELEMETRY_EXTENDED_CAP_ID) {
		printk(BIOS_DEBUG, "Read ID for Telemetry: 0x%x differs from expected: 0x%x\n",
		       cl_devsc_cap->cap_data.fields.pcie_cap_id, TELEMETRY_EXTENDED_CAP_ID);
		return false;
	}

	/* walk through the entries until crashLog entry */
	cl_devsc_cap->devsc_data.data_32[1] = pci_read_config32(PCI_DEV_TELEMETRY, TEL_DVSEV_ID);
	int new_offset = 0;
	while (cl_devsc_cap->devsc_data.fields.devsc_id != CRASHLOG_DVSEC_ID) {
		if (cl_devsc_cap->cap_data.fields.next_cap_offset == 0
		     || cl_devsc_cap->cap_data.fields.next_cap_offset == 0xFFFF) {
			printk(BIOS_DEBUG, "Read invalid pcie_cap_id value: 0x%x\n",
			       cl_devsc_cap->cap_data.fields.pcie_cap_id);
			return false;
		}
		new_offset = cl_devsc_cap->cap_data.fields.next_cap_offset;
		cl_devsc_cap->cap_data.data = pci_read_config32(PCI_DEV_TELEMETRY,
						new_offset + TEL_DVSEC_PCIE_CAP_ID);
		cl_devsc_cap->devsc_data.data_32[1] = pci_read_config32(PCI_DEV_TELEMETRY,
							new_offset + TEL_DVSEV_ID);
	}
	cpu_crash_version = cl_devsc_cap->devsc_data.fields.devsc_ver;

	cl_devsc_cap->discovery_data.data = pci_read_config32(PCI_DEV_TELEMETRY, new_offset
						+ TEL_DVSEV_DISCOVERY_TABLE_OFFSET);

	return true;
}

static u32 get_disc_table_offset(void)
{
	u32 offset = cpu_cl_devsc_cap.discovery_data.fields.discovery_table_offset;
	if (cpu_get_cpuid() >= CPUID_METEORLAKE_B0) {
		offset <<= 3;
		printk(BIOS_DEBUG, "adjusted cpu discovery table offset: 0x%x\n", offset);
	}

	return offset;
}

static bool is_crashlog_data_valid(u32 dw0)
{
	return (dw0 != 0x0 && dw0 != INVALID_CRASHLOG_RECORD);
}

static bool cpu_cl_gen_discovery_table(void)
{
	uintptr_t bar_addr = cl_get_cpu_bar_addr();

	if (!bar_addr)
		return false;

	disc_tab_addr = bar_addr + get_disc_table_offset();
	memset(&cpu_cl_disc_tab, 0, sizeof(cpu_crashlog_discovery_table_t));
	cpu_cl_disc_tab.header.data = get_disc_tab_header();
	/* Check both 32 bit header data and status register for non-zero values */
	if ((!is_crashlog_data_valid(cpu_cl_disc_tab.header.data & 0xFFFFFFFF)) &&
		(!is_crashlog_data_valid((cpu_cl_disc_tab.header.data) >> 32)))
		return false;

	u32 cur_offset = 0;
	cpu_cl_disc_tab.header.fields.count = CRASHLOG_NODES_COUNT;
	printk(BIOS_DEBUG, "cpu_crashlog_discovery_table buffer count: 0x%x\n",
			cpu_cl_disc_tab.header.fields.count);
	for (int i = 0; i < cpu_cl_disc_tab.header.fields.count; i++) {
		cur_offset = 8 + 24 * i;
		u32 cl_buffer_size = read32p(disc_tab_addr + cur_offset + 4);
		/* Check for buffer size */
		if (!(is_crashlog_data_valid(cl_buffer_size)))
			continue;

		u32 dw0 = read32p(disc_tab_addr + cur_offset);
		if (dw0 & CRASHLOG_CONSUMED_MASK) {
			printk(BIOS_DEBUG, "cpu crashlog records already consumed."
						"id: 0x%x dw0: 0x%x\n", i, dw0);
			break;
		}

		cpu_cl_disc_tab.buffers[i].data = read64p(disc_tab_addr + cur_offset);
		printk(BIOS_DEBUG, "cpu_crashlog_discovery_table buffer: 0x%x size: "
			"0x%x offset: 0x%x\n", i,  cpu_cl_disc_tab.buffers[i].fields.size,
			cpu_cl_disc_tab.buffers[i].fields.offset);
		m_cpu_crashLog_size += cpu_cl_disc_tab.buffers[i].fields.size * sizeof(u32);
	}

	if (m_cpu_crashLog_size > 0)
		m_cpu_crashLog_present = true;
	else
		m_cpu_crashLog_present = false;

	return true;
}

bool cpu_cl_discovery(void)
{
	memset(&cpu_cl_devsc_cap, 0, sizeof(tel_crashlog_devsc_cap_t));

	if (!cpu_cl_get_capability(&cpu_cl_devsc_cap)) {
		printk(BIOS_ERR, "CPU crashlog capability not found.\n");
		m_cpu_crashLog_support = false;
		return false;
	}

	m_cpu_crashLog_support = true;

	if (!cpu_cl_gen_discovery_table()) {
		printk(BIOS_ERR, "CPU crashlog discovery table not valid.\n");
		m_cpu_crashLog_present = false;
		return false;
	}

	return true;
}

void reset_discovery_buffers(void)
{
	memset(&discovery_buf, 0, sizeof(pmc_ipc_discovery_buf_t));
	memset(&descriptor_table, 0, sizeof(pmc_crashlog_desc_table_t));
	memset(&cpu_cl_devsc_cap, 0, sizeof(tel_crashlog_devsc_cap_t));
}

int cl_get_total_data_size(void)
{
	printk(BIOS_DEBUG, "crashlog size:pmc-0x%x, cpu-0x%x\n",
			m_pmc_crashLog_size, m_cpu_crashLog_size);
	return m_pmc_crashLog_size + m_cpu_crashLog_size;
}

static uintptr_t get_control_status_interface(void)
{
	if (disc_tab_addr)
		return (disc_tab_addr + CONTROL_INTERFACE_OFFSET * sizeof(u32));
	return 0;
}

int cpu_cl_clear_data(void)
{
	return 0;
}

static bool wait_and_check(u32 bit_mask)
{
	u32 stall_cnt = 0;

	do {
		cpu_cl_disc_tab.header.data = get_disc_tab_header();
		udelay(CPU_CRASHLOG_WAIT_STALL);
		stall_cnt++;
	} while (((cpu_cl_disc_tab.header.data & bit_mask) == 0) &&
			((stall_cnt * CPU_CRASHLOG_WAIT_STALL) < CPU_CRASHLOG_WAIT_TIMEOUT));

	return (cpu_cl_disc_tab.header.data & bit_mask);
}

void cpu_cl_rearm(void)
{
	uintptr_t ctrl_sts_intfc_addr = get_control_status_interface();

	if (!ctrl_sts_intfc_addr) {
		printk(BIOS_ERR, "CPU crashlog control and status interface address not valid\n");
		return;
	}

	/* Rearm the CPU crashlog. Crashlog does not get collected if rearming fails */
	cl_punit_control_interface_t punit_ctrl_intfc;
	memset(&punit_ctrl_intfc, 0, sizeof(cl_punit_control_interface_t));
	punit_ctrl_intfc.fields.set_re_arm = 1;
	write32p(ctrl_sts_intfc_addr, punit_ctrl_intfc.data);

	if (!wait_and_check(CRASHLOG_RE_ARM_STATUS_MASK))
		printk(BIOS_ERR, "CPU crashlog re_arm not asserted\n");
	else
		printk(BIOS_DEBUG, "CPU crashlog re_arm asserted\n");
}

void cpu_cl_cleanup(void)
{
	/* Perform any SOC specific cleanup after reading the crashlog data from SRAM */
	uintptr_t ctrl_sts_intfc_addr = get_control_status_interface();

	if (!ctrl_sts_intfc_addr) {
		printk(BIOS_ERR, "CPU crashlog control and status interface address not valid\n");
		return;
	}

	/* If storage-off is supported, turn off the PUNIT SRAM
	 * stroage to save power. This clears crashlog records also.
	 */

	if (!cpu_cl_disc_tab.header.fields.storage_off_support) {
		printk(BIOS_INFO, "CPU crashlog storage_off not supported\n");
		return;
	}

	cl_punit_control_interface_t punit_ctrl_intfc;
	memset(&punit_ctrl_intfc, 0, sizeof(cl_punit_control_interface_t));
	punit_ctrl_intfc.fields.set_storage_off = 1;
	write32p(ctrl_sts_intfc_addr, punit_ctrl_intfc.data);

	if (!wait_and_check(CRASHLOG_PUNIT_STORAGE_OFF_MASK))
		printk(BIOS_ERR, "CPU crashlog storage_off not asserted\n");
	else
		printk(BIOS_DEBUG, "CPU crashlog storage_off asserted\n");
}

pmc_ipc_discovery_buf_t cl_get_pmc_discovery_buf(void)
{
	return discovery_buf;
}

pmc_crashlog_desc_table_t cl_get_pmc_descriptor_table(void)
{
	return descriptor_table;
}

int cl_get_pmc_record_size(void)
{
	return m_pmc_crashLog_size;
}

int cl_get_cpu_record_size(void)
{
	return m_cpu_crashLog_size;
}

bool cl_cpu_data_present(void)
{
	return m_cpu_crashLog_present;
}

bool cl_pmc_data_present(void)
{
	return m_pmc_crashLog_present;
}

bool cpu_crashlog_support(void)
{
	return m_cpu_crashLog_support;
}

bool pmc_crashlog_support(void)
{
	return m_pmc_crashLog_support;
}

void update_new_pmc_crashlog_size(u32 *pmc_crash_size)
{
	m_pmc_crashLog_size = *pmc_crash_size;
}

cpu_crashlog_discovery_table_t cl_get_cpu_discovery_table(void)
{
	return cpu_cl_disc_tab;
}

void update_new_cpu_crashlog_size(u32 *cpu_crash_size)
{
	m_cpu_crashLog_size = *cpu_crash_size;
}
