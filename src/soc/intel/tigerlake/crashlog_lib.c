/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <intelblocks/crashlog.h>
#include <string.h>
#include <soc/crashlog.h>
#include <arch/bert_storage.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>

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

u32 __weak cl_get_cpu_mb_int_addr(void)
{
	return CRASHLOG_MAILBOX_INTF_ADDRESS;
}

bool pmc_cl_discovery(void)
{
	u32 tmp_bar_addr = 0, desc_table_addr = 0;

	const struct pmc_ipc_buffer *req = { 0 };
	struct pmc_ipc_buffer *res = NULL;
	uint32_t cmd_reg;
	int r;

	cmd_reg = pmc_make_ipc_cmd(PMC_IPC_CMD_CRASHLOG,
				PMC_IPC_CMD_ID_CRASHLOG_DISCOVERY,
				PMC_IPC_CMD_SIZE_SHIFT);
	printk(BIOS_DEBUG, "cmd_reg from pmc_make_ipc_cmd %d\n", cmd_reg);

	r = pmc_send_ipc_cmd(cmd_reg, req, res);

	if (r < 0) {
		printk(BIOS_ERR, "pmc_send_ipc_cmd failed in %s\n", __func__);
		return false;
	}
	discovery_buf.val_64_bits = ((u64)res->buf[1] << 32) | res->buf[0];


	if (discovery_buf.bits.supported != 1) {
		printk(BIOS_DEBUG, "PCH crashlog feature not supported.\n");
		m_pmc_crashLog_support = false;
		return false;
	}
	m_pmc_crashLog_support = true;

	/* Program BAR 0 and enable command register memory space decoding */
	tmp_bar_addr = SPI_BASE_ADDRESS;
	pci_write_config32(PCH_DEV_SRAM, PCI_BASE_ADDRESS_0, tmp_bar_addr);
	pci_or_config16(PCH_DEV_SRAM, PCI_COMMAND, PCI_COMMAND_MEMORY);

	if (discovery_buf.bits.discov_mechanism == 1) {
		/* discovery mode */
		if (discovery_buf.bits.base_offset & BIT(31)) {
			printk(BIOS_DEBUG, "PCH discovery to be used is disabled.\n");
			m_pmc_crashLog_present = false;
			m_pmc_crashLog_size = 0;
			return false;
		}
		desc_table_addr = tmp_bar_addr + discovery_buf.bits.desc_tabl_offset;
		m_pmc_crashLog_size = pmc_cl_gen_descriptor_table(desc_table_addr,
								  &descriptor_table);
		printk(BIOS_DEBUG, "PMC crashLog size in discovery mode : 0x%X\n",
		       m_pmc_crashLog_size);
	} else {
		/* legacy mode */
		if (discovery_buf.bits.dis) {
			printk(BIOS_DEBUG, "PCH crashlog is disabled in legacy mode.\n");
			m_pmc_crashLog_present = false;
			return false;
		}
		m_pmc_crashLog_size = (discovery_buf.bits.size != 0) ?
					(discovery_buf.bits.size * sizeof(u32)) : 0xC00;
		printk(BIOS_DEBUG, "PMC crashlog size in legacy mode = 0x%x\n",
			m_pmc_crashLog_size);
	}
	m_pmc_crashLog_present = true;

	return true;
}

u32 cl_get_cpu_bar_addr(void)
{
	u32 base_addr = 0;
	if (cpu_cl_devsc_cap.discovery_data.fields.t_bir_q == TEL_DVSEC_TBIR_BAR0) {
		base_addr = pci_read_config32(SA_DEV_TMT, PCI_BASE_ADDRESS_0) &
				~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	} else if (cpu_cl_devsc_cap.discovery_data.fields.t_bir_q == TEL_DVSEC_TBIR_BAR1) {
		base_addr = pci_read_config32(SA_DEV_TMT, PCI_BASE_ADDRESS_1) &
				~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	} else {
		printk(BIOS_ERR, "Invalid TEL_CFG_BAR value %d:\n",
			cpu_cl_devsc_cap.discovery_data.fields.t_bir_q);
	}

	return base_addr;
}


u32 cl_get_cpu_tmp_bar(void)
{
	return SPI_BASE_ADDRESS;
}

bool  cl_pmc_sram_has_mmio_access(void)
{

	if (pci_read_config16(PCH_DEV_SRAM, PCI_VENDOR_ID) == 0xFFFF) {
		printk(BIOS_ERR, "PMC SSRAM PCI device is disabled.\n");
		return false;
	}

	return true;
}

static bool cpu_cl_get_capability(tel_crashlog_devsc_cap_t *cl_devsc_cap)
{
	cl_devsc_cap->cap_data.data = pci_read_config32(SA_DEV_TMT,
						TEL_DVSEC_OFFSET + TEL_DVSEC_PCIE_CAP_ID);
	if (cl_devsc_cap->cap_data.fields.pcie_cap_id != TELEMETRY_EXTENDED_CAP_ID) {
		printk(BIOS_DEBUG, "Read ID for Telemetry: 0x%x differs from expected: 0x%x\n",
		       cl_devsc_cap->cap_data.fields.pcie_cap_id, TELEMETRY_EXTENDED_CAP_ID);
		return false;
	}

	/* walk through the entries until crashLog entry */
	cl_devsc_cap->devsc_data.data_32[1] = pci_read_config32(SA_DEV_TMT, TEL_DVSEV_ID);
	int new_offset = 0;
	while (cl_devsc_cap->devsc_data.fields.devsc_id != CRASHLOG_DVSEC_ID) {
		if (cl_devsc_cap->cap_data.fields.next_cap_offset == 0
		     || cl_devsc_cap->cap_data.fields.next_cap_offset == 0xFFFF) {
			printk(BIOS_DEBUG, "Read invalid pcie_cap_id value: : 0x%x\n",
			       cl_devsc_cap->cap_data.fields.pcie_cap_id);
			return false;
		}
		new_offset = cl_devsc_cap->cap_data.fields.next_cap_offset;
		cl_devsc_cap->cap_data.data = pci_read_config32(SA_DEV_TMT,
						new_offset + TEL_DVSEC_PCIE_CAP_ID);
		cl_devsc_cap->devsc_data.data_32[1] = pci_read_config32(SA_DEV_TMT,
							new_offset + TEL_DVSEV_ID);
	}
	cpu_crash_version = cl_devsc_cap->devsc_data.fields.devsc_ver;

	cl_devsc_cap->discovery_data.data = pci_read_config32(SA_DEV_TMT, new_offset
						+ TEL_DVSEV_DISCOVERY_TABLE_OFFSET);

	return true;
}


static bool cpu_cl_gen_discovery_table(void)
{
	u32 bar_addr = 0, disc_tab_addr = 0;
	bar_addr = cl_get_cpu_bar_addr();
	disc_tab_addr = bar_addr +
			cpu_cl_devsc_cap.discovery_data.fields.discovery_table_offset;
	memset(&cpu_cl_disc_tab, 0, sizeof(cpu_crashlog_discovery_table_t));

	cpu_cl_disc_tab.header.data = ((u64)read32((u32 *)disc_tab_addr) +
				     ((u64)read32((u32 *)(disc_tab_addr + 4)) << 32));

	cpu_cl_disc_tab.cmd_mailbox.data = read32((u32 *)(disc_tab_addr + 8));
	cpu_cl_disc_tab.mailbox_data = read32((u32 *)(disc_tab_addr + 12));

	printk(BIOS_DEBUG, "cpu_crashlog_discovery_table buffer count: 0x%x\n",
		cpu_cl_disc_tab.header.fields.count);

	if (cpu_cl_disc_tab.header.fields.guid != CPU_CRASHLOG_DISC_TAB_GUID_VALID) {
		printk(BIOS_ERR, "Invalid CPU crashlog discovery table GUID, expected = 0x%X ,"
			"actual = 0x%X\n", CPU_CRASHLOG_DISC_TAB_GUID_VALID,
			cpu_cl_disc_tab.header.fields.guid);
		return false;
	}

	int cur_offset = 0;
	for (int i = 0; i <  cpu_cl_disc_tab.header.fields.count ; i++) {
		cur_offset = 16 + 8*i;
		cpu_cl_disc_tab.buffers[i].data = ((u64)read32((u32 *)(disc_tab_addr +
						cur_offset)) + ((u64)read32((u32 *)
						(disc_tab_addr + cur_offset + 4)) << 32));
		printk(BIOS_DEBUG, "cpu_crashlog_discovery_table buffer: 0x%x size:"
			"0x%x offset: 0x%x\n", i,  cpu_cl_disc_tab.buffers[i].fields.size,
			cpu_cl_disc_tab.buffers[i].fields.offset);
		m_cpu_crashLog_size += cpu_cl_disc_tab.buffers[i].fields.size * sizeof(u32);
	}

	m_cpu_crashLog_present = m_cpu_crashLog_size > 0;

	return true;
}

bool cpu_cl_discovery(void)
{
	memset(&cpu_cl_devsc_cap, 0, sizeof(tel_crashlog_devsc_cap_t));

	if (!cpu_cl_get_capability(&cpu_cl_devsc_cap)) {
		printk(BIOS_ERR, "CPU crashlog capability  not found.\n");
		m_cpu_crashLog_support = false;
		return false;
	}

	m_cpu_crashLog_support = true;

	/* Program BAR address and enable command register memory space decoding */
	u32 tmp_bar_addr = PCH_PWRM_BASE_ADDRESS;
	printk(BIOS_DEBUG, "tmp_bar_addr: 0x%X\n", tmp_bar_addr);

	if (cpu_cl_devsc_cap.discovery_data.fields.t_bir_q == TEL_DVSEC_TBIR_BAR0) {
		pci_write_config32(SA_DEV_TMT, PCI_BASE_ADDRESS_0, tmp_bar_addr);
	} else if (cpu_cl_devsc_cap.discovery_data.fields.t_bir_q == TEL_DVSEC_TBIR_BAR1) {
		pci_write_config32(SA_DEV_TMT, PCI_BASE_ADDRESS_1, tmp_bar_addr);
	} else {
		printk(BIOS_DEBUG, "invalid discovery data t_bir_q: 0x%x\n",
			cpu_cl_devsc_cap.discovery_data.fields.t_bir_q);
		return false;
	}
	pci_or_config16(SA_DEV_TMT, PCI_COMMAND, PCI_COMMAND_MEMORY);

	if (!cpu_cl_gen_discovery_table()) {
		printk(BIOS_ERR, "CPU crashlog discovery table not valid.\n");
		m_cpu_crashLog_present = false;
		return false;
	}
	m_cpu_crashLog_present = true;

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
	return m_pmc_crashLog_size + m_cpu_crashLog_size;
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
