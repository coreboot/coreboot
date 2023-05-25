/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bert_storage.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <intelblocks/crashlog.h>
#include <intelblocks/pmc_ipc.h>
#include <soc/crashlog.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <string.h>

/* global crashLog info */
static bool m_pmc_crashLog_support;
static bool m_pmc_crashLog_present;
static bool m_cpu_crashLog_support;
static bool m_cpu_crashLog_present;
static bool m_ioe_crashLog_support;
static bool m_ioe_crashLog_present;
static u32 m_pmc_crashLog_size;
static u32 m_ioe_crashLog_size;
static u32 m_cpu_crashLog_size;
static u32 cpu_crash_version;
static pmc_ipc_discovery_buf_t discovery_buf;
static pmc_crashlog_desc_table_t descriptor_table;
static tel_crashlog_devsc_cap_t cpu_cl_devsc_cap;
static cpu_crashlog_discovery_table_t cpu_cl_disc_tab;

u32 __weak cl_get_cpu_mb_int_addr(void)
{
	return CRASHLOG_MAILBOX_INTF_ADDRESS;
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

static void configure_sram(const struct device *sram_dev, u32 base_addr)
{
	pci_update_config16(sram_dev, PCI_COMMAND, ~(PCI_COMMAND_IO | PCI_COMMAND_MEMORY), 0);

	/* Program BAR 0 and enable command register memory space decoding */
	pci_write_config32(sram_dev, PCI_BASE_ADDRESS_0, base_addr);
	pci_or_config16(sram_dev, PCI_COMMAND, PCI_COMMAND_MEMORY);
}

void cl_get_pmc_sram_data(void)
{
	u32 *soc_pmc_dest = NULL, *ioe_pmc_dest = NULL;
	u32 pmc_sram_base = cl_get_cpu_tmp_bar();
	u32 ioe_sram_base = get_sram_bar(PCI_DEVFN_IOE_SRAM);
	u32 pmc_crashLog_size = cl_get_pmc_record_size();
	u32 ioe_crashLog_size = 0;

	if (!pmc_sram_base) {
		printk(BIOS_ERR, "PMC SRAM base not valid\n");
		return;
	}

	if (!pmc_crashLog_size) {
		printk(BIOS_ERR, "No PMC crashlog records\n");
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

	/* allocate memory for the PMC crash records to be copied */
	unsigned long pmc_cl_cbmem_addr;

	pmc_cl_cbmem_addr = (unsigned long) cbmem_add(CBMEM_ID_PMC_CRASHLOG,
			pmc_crashLog_size);
	if (!pmc_cl_cbmem_addr) {
		printk(BIOS_ERR, "Unable to allocate CBMEM PMC crashLog entry.\n");
		return;
	}

	memset((void *)pmc_cl_cbmem_addr, 0, pmc_crashLog_size);
	soc_pmc_dest = (u32 *)(uintptr_t) pmc_cl_cbmem_addr;

	bool pmc_sram = true;

	/* process crashlog records for SOC PMC SRAM */
	for (int i = 0; i < descriptor_table.numb_regions + 1; i++) {
		printk(BIOS_DEBUG, "Region[0x%x].Tag=0x%x offset=0x%x, size=0x%x\n",
				i,
				descriptor_table.regions[i].bits.assign_tag,
				descriptor_table.regions[i].bits.offset,
				descriptor_table.regions[i].bits.size);

		if (!descriptor_table.regions[i].bits.size)
			continue;

		if (descriptor_table.regions[i].bits.assign_tag ==
				CRASHLOG_DESCRIPTOR_TABLE_TAG_SOC) {

			if (cl_copy_data_from_sram(pmc_sram_base,
						descriptor_table.regions[i].bits.offset,
						descriptor_table.regions[i].bits.size,
						soc_pmc_dest,
						i,
						pmc_sram)) {
				soc_pmc_dest = (u32 *)((u32)soc_pmc_dest +
						(descriptor_table.regions[i].bits.size
						 * sizeof(u32)));
			} else {
				pmc_crashLog_size -= descriptor_table.regions[i].bits.size *
					sizeof(u32);
				printk(BIOS_DEBUG, "PMC crashlog size adjusted to: 0x%x\n",
							pmc_crashLog_size);
			}
		} else if (descriptor_table.regions[i].bits.assign_tag ==
				CRASHLOG_DESCRIPTOR_TABLE_TAG_IOE) {
			/*
			 * SOC PMC crashlog records contains information about IOE SRAM
			 * records as well. Calculate IOE records size while parsing SOC
			 * PME SRAM.
			 */
			ioe_crashLog_size += descriptor_table.regions[i].bits.size * sizeof(u32);
		}
	}

	pmc_crashLog_size -= ioe_crashLog_size;
	update_new_pmc_crashlog_size(&pmc_crashLog_size);

	if (ioe_crashLog_size)
		m_ioe_crashLog_present = true;
	else
		goto pmc_send_re_arm_after_reset;

	/* allocate memory for the IOE crashlog records to be copied */
	unsigned long ioe_cl_cbmem_addr;

	ioe_cl_cbmem_addr = (unsigned long) cbmem_add(CBMEM_ID_IOE_CRASHLOG,
							ioe_crashLog_size);
	if (!ioe_cl_cbmem_addr) {
		printk(BIOS_ERR, "Unable to allocate CBMEM IOE crashLog entry.\n");
		return;
	}

	memset((void *)ioe_cl_cbmem_addr, 0, ioe_crashLog_size);
	ioe_pmc_dest = (u32 *)(uintptr_t) ioe_cl_cbmem_addr;

	/* process crashlog records for IOE SRAM */
	for (int i = 0; i < descriptor_table.numb_regions + 1; i++) {
		printk(BIOS_DEBUG, "Region[0x%x].Tag=0x%x offset=0x%x, size=0x%x\n",
				i,
				descriptor_table.regions[i].bits.assign_tag,
				descriptor_table.regions[i].bits.offset,
				descriptor_table.regions[i].bits.size);

		if (!descriptor_table.regions[i].bits.size)
			continue;

		if (descriptor_table.regions[i].bits.assign_tag ==
				CRASHLOG_DESCRIPTOR_TABLE_TAG_IOE) {

			if (cl_copy_data_from_sram(ioe_sram_base,
						descriptor_table.regions[i].bits.offset,
						descriptor_table.regions[i].bits.size,
						ioe_pmc_dest,
						i,
						pmc_sram)) {
				ioe_pmc_dest = (u32 *)((u32)ioe_pmc_dest +
						(descriptor_table.regions[i].bits.size
						 * sizeof(u32)));
			} else {

				ioe_crashLog_size -= descriptor_table.regions[i].bits.size *
								sizeof(u32);
				printk(BIOS_DEBUG, "IOE crashlog size adjusted to: 0x%x\n",
								ioe_crashLog_size);
			}
		}
	}

	update_new_ioe_crashlog_size(&ioe_crashLog_size);

pmc_send_re_arm_after_reset:
	/* when bit 7 of discov cmd resp is set -> bit 2 of size field */
	cl_pmc_re_arm_after_reset();

	/* Clear the SSRAM region after copying the error log */
	cl_pmc_clear();
}

bool pmc_cl_discovery(void)
{
	u32 bar_addr = 0, desc_table_addr = 0;

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
		m_ioe_crashLog_support = false;
		m_pmc_crashLog_size = 0;
		m_ioe_crashLog_size = 0;
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

u32 cl_get_cpu_bar_addr(void)
{
	u32 base_addr = 0;
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

u32 cl_get_cpu_tmp_bar(void)
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

static bool cpu_cl_gen_discovery_table(void)
{
	u32 bar_addr = 0, disc_tab_addr = 0;
	bar_addr = cl_get_cpu_bar_addr();

	if (!bar_addr)
		return false;

	disc_tab_addr = bar_addr +
			cpu_cl_devsc_cap.discovery_data.fields.discovery_table_offset;
	memset(&cpu_cl_disc_tab, 0, sizeof(cpu_crashlog_discovery_table_t));

	printk(BIOS_DEBUG, "cpu discovery table offset: 0x%x\n",
		cpu_cl_devsc_cap.discovery_data.fields.discovery_table_offset);

	cpu_cl_disc_tab.header.data = ((u64)read32((u32 *)disc_tab_addr) +
				     ((u64)read32((u32 *)(disc_tab_addr + 4)) << 32));

	printk(BIOS_DEBUG, "cpu_crashlog_discovery_table buffer count: 0x%x\n",
		cpu_cl_disc_tab.header.fields.count);

	int cur_offset = 0;
	for (int i = 0; i < cpu_cl_disc_tab.header.fields.count; i++) {
		cur_offset = 8 + 24*i;
		cpu_cl_disc_tab.buffers[i].data = ((u64)read32((u32 *)(disc_tab_addr +
						cur_offset)) + ((u64)read32((u32 *)
						(disc_tab_addr + cur_offset + 4)) << 32));
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
	printk(BIOS_DEBUG, "crashlog size:pmc-0x%x, ioe-pmc-0x%x cpu-0x%x\n",
			m_pmc_crashLog_size, m_ioe_crashLog_size, m_cpu_crashLog_size);
	return m_pmc_crashLog_size + m_cpu_crashLog_size + m_ioe_crashLog_size;
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

int cl_get_ioe_record_size(void)
{
	return m_ioe_crashLog_size;
}

bool cl_cpu_data_present(void)
{
	return m_cpu_crashLog_present;
}

bool cl_pmc_data_present(void)
{
	return m_pmc_crashLog_present;
}

bool cl_ioe_data_present(void)
{
	return m_ioe_crashLog_present;
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

void update_new_ioe_crashlog_size(u32 *ioe_crash_size)
{
	m_ioe_crashLog_size = *ioe_crash_size;
}

cpu_crashlog_discovery_table_t cl_get_cpu_discovery_table(void)
{
	return cpu_cl_disc_tab;
}

void update_new_cpu_crashlog_size(u32 *cpu_crash_size)
{
	m_cpu_crashLog_size = *cpu_crash_size;
}
