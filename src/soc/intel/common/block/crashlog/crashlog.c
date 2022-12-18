/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <intelblocks/crashlog.h>
#include <intelblocks/pmc_ipc.h>
#include <string.h>

int __weak cl_get_cpu_record_size(void)
{
	return 0;
}

int __weak cl_get_pmc_record_size(void)
{
	return 0;
}

u32 __weak cl_get_cpu_bar_addr(void)
{
	return 0;
}

u32 __weak cl_get_cpu_tmp_bar(void)
{
	return 0;
}

u32 __weak cl_get_cpu_mb_int_addr(void)
{
	return 0;
}

int __weak cl_get_total_data_size(void)
{
	return cl_get_cpu_record_size() + cl_get_pmc_record_size();
}

bool __weak cl_pmc_sram_has_mmio_access(void)
{
	return false;
}

bool __weak cpu_crashlog_support(void)
{
	return false;
}

bool __weak pmc_crashlog_support(void)
{
	return false;
}

bool __weak cl_cpu_data_present(void)
{
	return false;
}

bool __weak cl_pmc_data_present(void)
{
	return false;
}

__weak void reset_discovery_buffers(void) {}

__weak void update_new_pmc_crashlog_size(u32 *pmc_crash_size) {}

__weak void update_new_cpu_crashlog_size(u32 *cpu_crash_size) {}

pmc_ipc_discovery_buf_t __weak cl_get_pmc_discovery_buf(void)
{
	pmc_ipc_discovery_buf_t discov_buf;
	memset(&discov_buf, 0, sizeof(pmc_ipc_discovery_buf_t));
	return discov_buf;
}

pmc_crashlog_desc_table_t __weak cl_get_pmc_descriptor_table(void)
{
	pmc_crashlog_desc_table_t desc_tab;
	memset(&desc_tab, 0, sizeof(pmc_crashlog_desc_table_t));
	return desc_tab;
}

cpu_crashlog_discovery_table_t __weak cl_get_cpu_discovery_table(void)
{
	cpu_crashlog_discovery_table_t cpu_disc_tab;
	memset(&cpu_disc_tab, 0, sizeof(cpu_crashlog_discovery_table_t));
	return cpu_disc_tab;
}

int cpu_cl_poll_mailbox_ready(u32 cl_mailbox_addr)
{
	cpu_crashlog_mailbox_t cl_mailbox_interface;
	u16 stall_cnt = 0;

	do {
		cl_mailbox_interface.data = read32((u32 *)cl_mailbox_addr);
		udelay(CPU_CRASHLOG_MAILBOX_WAIT_STALL);
		stall_cnt++;
	} while ((cl_mailbox_interface.fields.busy == 1)
		 && stall_cnt < CPU_CRASHLOG_MAILBOX_WAIT_TIMEOUT);

	if ((cl_mailbox_interface.fields.busy == 1)
	    && (stall_cnt >= CPU_CRASHLOG_MAILBOX_WAIT_TIMEOUT)) {
		printk(BIOS_ERR, "CPU crashlog mailbox timed out.\n");
		return 0;
	}

	return 1;
}

int cpu_cl_mailbox_cmd(u8 cmd, u8 param)
{
	cpu_crashlog_mailbox_t cl_mailbox_intf;
	u32 cl_base_addr;

	memset(&cl_mailbox_intf, 0, sizeof(cpu_crashlog_mailbox_t));

	cl_base_addr = cl_get_cpu_bar_addr();

	cl_mailbox_intf.fields.command = cmd;
	cl_mailbox_intf.fields.param = param;
	cl_mailbox_intf.fields.busy = 1;

	write32((u32 *)(cl_base_addr + cl_get_cpu_mb_int_addr()),
		cl_mailbox_intf.data);

	cpu_cl_poll_mailbox_ready(cl_base_addr + cl_get_cpu_mb_int_addr());

	return 1;
}

int cpu_cl_clear_data(void)
{
	return cpu_cl_mailbox_cmd(CPU_CRASHLOG_CMD_CLEAR, 0);
}

int pmc_cl_gen_descriptor_table(u32 desc_table_addr,
				pmc_crashlog_desc_table_t *descriptor_table)
{
	int total_data_size = 0;
	descriptor_table->numb_regions = read32((u32 *)desc_table_addr);
	printk(BIOS_DEBUG, "CL PMC desc table: numb of regions is 0x%x at addr 0x%x\n",
	       descriptor_table->numb_regions, desc_table_addr);
	for (int i = 0; i < descriptor_table->numb_regions; i++) {
		desc_table_addr += 4;
		descriptor_table->regions[i].data = read32((u32 *)(desc_table_addr));
		total_data_size += descriptor_table->regions[i].bits.size * sizeof(u32);
		printk(BIOS_DEBUG, "CL PMC desc table: region 0x%x has size 0x%x at offset 0x%x\n",
			i, descriptor_table->regions[i].bits.size,
			descriptor_table->regions[i].bits.offset);
		if (i > 255) {
			printk(BIOS_ERR, "More than 255 regions in PMC crashLog descriptor table");
			break;
		}
	}
	return total_data_size;
}

bool __weak pmc_cl_discovery(void)
{
	return false;
}

bool __weak cpu_cl_discovery(void)
{
	return false;
}

int cl_pmc_re_arm_after_reset(void)
{

	const struct pmc_ipc_buffer req = { 0 };
	struct pmc_ipc_buffer res;
	uint32_t cmd_reg;
	int r;

	cmd_reg = pmc_make_ipc_cmd(PMC_IPC_CMD_CRASHLOG,
				PMC_IPC_CMD_ID_CRASHLOG_RE_ARM_ON_RESET,
				PMC_IPC_CMD_SIZE_SHIFT);

	r = pmc_send_ipc_cmd(cmd_reg, &req, &res);

	if (r < 0) {
		printk(BIOS_ERR, "pmc_send_ipc_cmd failed in %s\n", __func__);
		return 0;
	}

	return r;
}

/* Sends PMC IPC to clear CrashLog from PMC SSRAM area */
int cl_pmc_clear(void)
{
	const struct pmc_ipc_buffer req = { 0 };
	struct pmc_ipc_buffer res;
	uint32_t cmd_reg;
	int r;

	cmd_reg = pmc_make_ipc_cmd(PMC_IPC_CMD_CRASHLOG,
				PMC_IPC_CMD_ID_CRASHLOG_ERASE,
				PMC_IPC_CMD_SIZE_SHIFT);

	r = pmc_send_ipc_cmd(cmd_reg, &req, &res);

	if (r < 0) {
		printk(BIOS_ERR, "pmc_send_ipc_cmd failed in %s\n", __func__);
		return 0;
	}

	return r;
}

/*
 * Sends PMC IPC to populate CrashLog on all reboot.
 * The SSRAM area will be cleared on G3 by PMC automatically
 */
int cl_pmc_en_gen_on_all_reboot(void)
{
	const struct pmc_ipc_buffer req = { 0 };
	struct pmc_ipc_buffer res;
	uint32_t cmd_reg;
	int r;

	cmd_reg = pmc_make_ipc_cmd(PMC_IPC_CMD_CRASHLOG,
				PMC_IPC_CMD_ID_CRASHLOG_ON_RESET,
				PMC_IPC_CMD_SIZE_SHIFT);

	r = pmc_send_ipc_cmd(cmd_reg, &req, &res);

	if (r < 0) {
		printk(BIOS_ERR, "pmc_send_ipc_cmd failed in %s\n", __func__);
		return 0;
	}

	return r;
}

bool discover_crashlog(void)
{
	bool cpu_cl_discovered = false, pmc_cl_discovered = false;

	reset_discovery_buffers();

	/* PCH crashLog discovery */
	pmc_cl_discovered = pmc_cl_discovery();

	/* CPU crashLog discovery */
	cpu_cl_discovered = cpu_cl_discovery();

	return (cpu_cl_discovered || pmc_cl_discovered);
}

bool cl_copy_data_from_sram(u32 src_bar,
				u32 offset,
				u32 size,
				u32 *dest_addr,
				u32 buffer_index,
				bool pmc_sram)
{
	if (src_bar == 0) {
		printk(BIOS_ERR, "Invalid bar 0x%x and offset 0x%x for %s\n",
		       src_bar, offset, __func__);
		return false;
	}

	u32 src_addr = src_bar + offset;

	u32 data =  read32((u32 *)src_addr);
	/* PMC: copy if 1st DWORD in buffer is not zero and its 31st bit is not set */
	if (pmc_sram && !(data && !(data & BIT(31)))) {
		printk(BIOS_DEBUG, "Invalid data 0x%x at offset 0x%x from addr 0x%x"
			" of PMC SRAM.\n", data, offset, src_bar);
		return false;
	}
	/*CPU: don't copy if 1st DWORD in first buffer is zero */
	if (!pmc_sram && !data && (buffer_index == 0)) {
		printk(BIOS_DEBUG, "Invalid data 0x%x at offset 0x%x from addr 0x%x"
			" of telemetry SRAM.\n", data, offset, src_bar);
		return false;
	}

	u32 copied = 0;
	while (copied < size) {
		/* DW by DW copy: byte access to PMC SRAM not allowed */
		*dest_addr = read32((u32 *)src_addr);
		dest_addr++;
		src_addr += 4;
		copied++;
	}
	return true;
}

void cl_get_pmc_sram_data(void)
{
	u32 *dest = NULL;
	u32 tmp_bar_addr = cl_get_cpu_tmp_bar();
	u32 pmc_crashLog_size = cl_get_pmc_record_size();

	if (!cl_pmc_sram_has_mmio_access())
		return;

	pmc_ipc_discovery_buf_t discovery_buf = cl_get_pmc_discovery_buf();

	if (discovery_buf.bits.supported != 1) {
		printk(BIOS_DEBUG, "PCH crashlog feature not supported.\n");
		goto pmc_send_re_arm_after_reset;
	}

	/* Get the size of data to copy */
	if (discovery_buf.bits.discov_mechanism == 1) {
		if (discovery_buf.bits.base_offset & BIT(31)) {
			printk(BIOS_DEBUG, "PCH discovery to be used is disabled.\n");
			goto pmc_send_re_arm_after_reset;
		}
		printk(BIOS_DEBUG, "PMC crashLog size in discovery mode : 0x%X\n",
		       pmc_crashLog_size);
	} else {
		if (discovery_buf.bits.dis) {
			printk(BIOS_DEBUG, "PCH crashlog is disabled in legacy mode.\n");
			return;
		}
		pmc_crashLog_size = (discovery_buf.bits.size != 0) ?
					discovery_buf.bits.size : 0xC00;
		printk(BIOS_DEBUG, "PMC crashLog size in legacy mode : 0x%X\n",
		       pmc_crashLog_size);
	}

	/* allocate mem for the record to be copied */
	unsigned long pmc_cl_cbmem_addr;

	pmc_cl_cbmem_addr = (unsigned long)cbmem_add(CBMEM_ID_PMC_CRASHLOG,
						pmc_crashLog_size);
	if (!pmc_cl_cbmem_addr) {
		printk(BIOS_ERR, "Unable to allocate CBMEM PMC crashLog entry.\n");
		return;
	}

	memset((void *)pmc_cl_cbmem_addr, 0, pmc_crashLog_size);
	dest = (u32 *)(uintptr_t)pmc_cl_cbmem_addr;
	bool pmc_sram = true;
	pmc_crashlog_desc_table_t descriptor_table =  cl_get_pmc_descriptor_table();
	if (discovery_buf.bits.discov_mechanism == 1) {
		for (int i = 0; i < descriptor_table.numb_regions; i++) {
			if (cl_copy_data_from_sram(tmp_bar_addr,
						  descriptor_table.regions[i].bits.offset,
						  descriptor_table.regions[i].bits.size,
						  dest,
						  i,
						  pmc_sram)) {
				dest = (u32 *)((u32)dest +
						(descriptor_table.regions[i].bits.size
						* sizeof(u32)));
			} else {
				pmc_crashLog_size -= descriptor_table.regions[i].bits.size *
							sizeof(u32);
				printk(BIOS_DEBUG, "discover mode PMC crashlog size adjusted"
					" to: 0x%x\n", pmc_crashLog_size);
			}
		}
	} else {
		if (!cl_copy_data_from_sram(tmp_bar_addr,
					   discovery_buf.bits.base_offset,
					   discovery_buf.bits.size,
					   dest,
					   0,
					   pmc_sram)) {
			pmc_crashLog_size -= discovery_buf.bits.size * sizeof(u32);
			printk(BIOS_DEBUG, "legacy mode PMC crashlog size adjusted to: 0x%x\n",
			       pmc_crashLog_size);
		}
	}

	update_new_pmc_crashlog_size(&pmc_crashLog_size);

pmc_send_re_arm_after_reset:
	/* when bit 7 of discov cmd resp is set -> bit 2 of size field */
	if (discovery_buf.bits.size & BIT(2))
		cl_pmc_re_arm_after_reset();

	/* Clear the SSRAM region after copying the error log */
	cl_pmc_clear();

}

void cl_get_cpu_sram_data(void)
{
	u32 tmp_bar_addr = 0;
	u32 *dest = NULL;
	u32 m_cpu_crashLog_size = cl_get_cpu_record_size();
	cpu_crashlog_discovery_table_t cpu_cl_disc_tab = cl_get_cpu_discovery_table();

	if (m_cpu_crashLog_size < 1) {
		printk(BIOS_DEBUG, "%s: no data to collect.\n", __func__);
		return;
	}

	printk(BIOS_DEBUG, "CPU crash data size: 0x%X bytes in 0x%X region(s).\n",
	       m_cpu_crashLog_size, cpu_cl_disc_tab.header.fields.count);

	/* allocate memory buffers for CPU crashog data to be copied */
	unsigned long cpu_crashlog_cbmem_addr;
	cpu_crashlog_cbmem_addr = (unsigned long)cbmem_add(CBMEM_ID_CPU_CRASHLOG,
								m_cpu_crashLog_size);
	if (!cpu_crashlog_cbmem_addr) {
		printk(BIOS_ERR, "Failed to add CPU main crashLog entries to CBMEM.\n");
		return;
	}

	memset((void *)cpu_crashlog_cbmem_addr, 0, m_cpu_crashLog_size);
	tmp_bar_addr = cl_get_cpu_bar_addr();
	dest = (u32 *)(uintptr_t)cpu_crashlog_cbmem_addr;
	bool pmc_sram = false;

	for (int i = 0 ; i < cpu_cl_disc_tab.header.fields.count ; i++) {
		if (cl_copy_data_from_sram(tmp_bar_addr,
					   cpu_cl_disc_tab.buffers[i].fields.offset,
					   cpu_cl_disc_tab.buffers[i].fields.size,
					   dest,
					   i,
					   pmc_sram)) {
			dest = (u32 *)((u32)dest +
					(cpu_cl_disc_tab.buffers[i].fields.size * sizeof(u32)));
		} else {
			m_cpu_crashLog_size -= cpu_cl_disc_tab.buffers[i].fields.size
						* sizeof(u32);

			/* for CPU skip all buffers if the 1st one is not valid */
			if (i == 0) {
				m_cpu_crashLog_size = 0;
				break;
			}
		}
	}

	update_new_cpu_crashlog_size(&m_cpu_crashLog_size);

	/* clear telemetry SRAM region */
	cpu_cl_clear_data();
}

void collect_pmc_and_cpu_crashlog_from_srams(void)
{
	if (pmc_crashlog_support() && cl_pmc_data_present()
		&& (cl_get_pmc_record_size() > 0)) {
		if (CONFIG(SOC_INTEL_CRASHLOG_ON_RESET)) {
			cl_pmc_en_gen_on_all_reboot();
			printk(BIOS_DEBUG, "Crashlog collection enabled on every reboot.\n");
		}
		cl_get_pmc_sram_data();
	} else {
		printk(BIOS_DEBUG, "Skipping PMC crashLog collection. Data not present.\n");
	}

	printk(BIOS_DEBUG, "m_cpu_crashLog_size : 0x%X bytes\n", cl_get_cpu_record_size());

	if (cpu_crashlog_support() && cl_cpu_data_present()
		&& (cl_get_cpu_record_size() > 0)) {
		printk(BIOS_DEBUG, "CPU crashLog present.\n");
		cl_get_cpu_sram_data();
	} else {
		printk(BIOS_DEBUG, "Skipping CPU crashLog collection. Data not present.\n");
	}
}

bool cl_fill_cpu_records(void *cl_record)
{
	void *cl_src_addr = NULL;

	u32 m_cpu_crashLog_size = cl_get_cpu_record_size();

	if (!cl_cpu_data_present() || m_cpu_crashLog_size == 0) {
		printk(BIOS_DEBUG, "CPU crashLog not present, skipping.\n");
		return false;
	}

	printk(BIOS_DEBUG, "CPU crash data collection.\n");
	cl_src_addr = cbmem_find(CBMEM_ID_CPU_CRASHLOG);
	memcpy(cl_record, cl_src_addr, m_cpu_crashLog_size);

	return true;
}

bool cl_fill_pmc_records(void *cl_record)
{
	void *cl_src_addr = NULL;

	u32 m_pmc_crashLog_size = cl_get_pmc_record_size();

	if (!cl_pmc_data_present() || m_pmc_crashLog_size == 0) {
		printk(BIOS_DEBUG, "PMC crashLog not present, skipping.\n");
		return false;
	}

	printk(BIOS_DEBUG, "PMC crash data collection.\n");
	cl_src_addr = cbmem_find(CBMEM_ID_PMC_CRASHLOG);
	memcpy(cl_record, cl_src_addr, m_pmc_crashLog_size);

	return true;
}
