/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * coreboot ACPI Table support
 */

/*
 * Each system port implementing ACPI has to provide two functions:
 *
 *   write_acpi_tables()
 *   acpi_dump_apics()
 *
 * See Kontron 986LCD-M port for a good example of an ACPI implementation
 * in coreboot.
 */

#include <acpi/acpi.h>
#include <acpi/acpi_apei.h>
#include <acpi/acpi_gnvs.h>
#include <acpi/acpi_iort.h>
#include <acpi/acpi_ivrs.h>
#include <acpi/acpigen.h>
#include <cbfs.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <drivers/crb/tpm.h>
#include <drivers/uart/pl011.h>
#include <security/tpm/tss.h>
#include <string.h>
#include <types.h>
#include <version.h>

static acpi_rsdp_t *valid_rsdp(acpi_rsdp_t *rsdp);

u8 acpi_checksum(u8 *table, u32 length)
{
	u8 ret = 0;
	while (length--) {
		ret += *table;
		table++;
	}
	return -ret;
}

/**
 * Add an ACPI table to the RSDT (and XSDT) structure, recalculate length
 * and checksum.
 */
void acpi_add_table(acpi_rsdp_t *rsdp, void *table)
{
	int i, entries_num;
	acpi_rsdt_t *rsdt;
	acpi_xsdt_t *xsdt;

	/* The 32bit RSDT may not be valid if tables live above 4GiB */
	rsdt = (acpi_rsdt_t *)(uintptr_t)rsdp->rsdt_address;
	xsdt = (acpi_xsdt_t *)(uintptr_t)rsdp->xsdt_address;

	/* This should always be MAX_ACPI_TABLES. */
	entries_num = ARRAY_SIZE(xsdt->entry);

	for (i = 0; i < entries_num; i++) {
		if (xsdt->entry[i] == 0)
			break;
	}

	if (i >= entries_num) {
		printk(BIOS_ERR, "ACPI: Error: Could not add ACPI table, "
			"too many tables.\n");
		return;
	}

	/* Add table to the XSDT. */
	xsdt->entry[i] = (u64)(uintptr_t)table;

	/* Fix XSDT length or the kernel will assume invalid entries. */
	xsdt->header.length = sizeof(acpi_header_t) + (sizeof(u64) * (i + 1));

	/* Re-calculate checksum. */
	xsdt->header.checksum = 0; /* Hope this won't get optimized away */
	xsdt->header.checksum = acpi_checksum((u8 *)xsdt, xsdt->header.length);

	/*
	 * And now the same thing for the RSDT. We use the same index as for
	 * now we want the XSDT and RSDT to always be in sync in coreboot.
	 */
	if (rsdt && (uintptr_t)table < UINT32_MAX) {
		/* Add table to the RSDT. */
		rsdt->entry[i] = (u32)(uintptr_t)table;

		/* Fix RSDT length. */
		rsdt->header.length = sizeof(acpi_header_t) + (sizeof(u32) * (i + 1));

		/* Re-calculate checksum. */
		rsdt->header.checksum = 0;
		rsdt->header.checksum = acpi_checksum((u8 *)rsdt, rsdt->header.length);
	}

	printk(BIOS_DEBUG, "ACPI: added table %d/%d, length now %d\n",
		i + 1, entries_num, xsdt->header.length);
}

static enum cb_err acpi_fill_header(acpi_header_t *header, const char name[4],
				    enum acpi_tables table, uint32_t size)
{
	if (!header)
		return CB_ERR;

	/* Fill out header fields. */
	memcpy(header->signature, name, 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->asl_compiler_revision = asl_revision;
	header->revision = get_acpi_table_revision(table);
	header->length = size;

	return CB_SUCCESS;
}

static int acpi_create_mcfg_mmconfig(acpi_mcfg_mmconfig_t *mmconfig, u64 base,
				u16 seg_nr, u8 start, u8 end)
{
	memset(mmconfig, 0, sizeof(*mmconfig));
	mmconfig->base_address = base;
	mmconfig->pci_segment_group_number = seg_nr;
	mmconfig->start_bus_number = start;
	mmconfig->end_bus_number = end;

	return sizeof(acpi_mcfg_mmconfig_t);
}

static void acpi_create_madt(acpi_header_t *header, void *unused)
{
	acpi_madt_t *madt = (acpi_madt_t *)header;
	unsigned long current = (unsigned long)madt + sizeof(acpi_madt_t);

	memset(madt, 0, sizeof(*madt));

	if (acpi_fill_header(header, "APIC", MADT, sizeof(acpi_madt_t)) != CB_SUCCESS)
		return;

	current = acpi_arch_fill_madt(madt, current);

	if (CONFIG(ACPI_CUSTOM_MADT))
		current = acpi_fill_madt(current);

	/* (Re)calculate length . */
	header->length = current - (unsigned long)madt;
}

static unsigned long acpi_fill_mcfg(unsigned long current)
{
	for (int i = 0; i < PCI_SEGMENT_GROUP_COUNT; i++) {
		current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current,
			CONFIG_ECAM_MMCONF_BASE_ADDRESS + i * PCI_PER_SEGMENT_GROUP_ECAM_SIZE,
			i,
			0,
			PCI_BUSES_PER_SEGMENT_GROUP - 1);
	}

	return current;
}

/* MCFG is defined in the PCI Firmware Specification 3.0. */
static void acpi_create_mcfg(acpi_header_t *header, void *unused)
{
	acpi_mcfg_t *mcfg = (acpi_mcfg_t *)header;
	unsigned long current = (unsigned long)mcfg + sizeof(acpi_mcfg_t);


	if (acpi_fill_header(header, "MCFG", MCFG, sizeof(acpi_mcfg_t)) != CB_SUCCESS)
		return;

	if (CONFIG(ECAM_MMCONF_SUPPORT))
		current = acpi_fill_mcfg(current);

	/* (Re)calculate length */
	header->length = current - (unsigned long)mcfg;
}

static void *get_tcpa_log(u32 *size)
{
	const struct cbmem_entry *ce;
	const u32 tcpa_default_log_len = 0x10000;
	void *lasa;
	ce = cbmem_entry_find(CBMEM_ID_TCPA_TCG_LOG);
	if (ce) {
		lasa = cbmem_entry_start(ce);
		*size = cbmem_entry_size(ce);
		printk(BIOS_DEBUG, "TCPA log found at %p\n", lasa);
		return lasa;
	}
	lasa = cbmem_add(CBMEM_ID_TCPA_TCG_LOG, tcpa_default_log_len);
	if (!lasa) {
		printk(BIOS_ERR, "TCPA log creation failed\n");
		return NULL;
	}

	printk(BIOS_DEBUG, "TCPA log created at %p\n", lasa);
	memset(lasa, 0, tcpa_default_log_len);

	*size = tcpa_default_log_len;
	return lasa;
}

static void acpi_create_tcpa(acpi_header_t *header, void *unused)
{
	if (tlcl_get_family() != TPM_1)
		return;

	acpi_tcpa_t *tcpa = (acpi_tcpa_t *)header;
	u32 tcpa_log_len;
	void *lasa;

	lasa = get_tcpa_log(&tcpa_log_len);
	if (!lasa)
		return;

	if (acpi_fill_header(header, "TCPA", TCPA, sizeof(acpi_tcpa_t)) != CB_SUCCESS)
		return;

	tcpa->platform_class = 0;
	tcpa->laml = tcpa_log_len;
	tcpa->lasa = (uintptr_t)lasa;
}

static void *get_tpm2_log(u32 *size)
{
	const struct cbmem_entry *ce;
	const u32 tpm2_default_log_len = 0x10000;
	void *lasa;
	ce = cbmem_entry_find(CBMEM_ID_TPM2_TCG_LOG);
	if (ce) {
		lasa = cbmem_entry_start(ce);
		*size = cbmem_entry_size(ce);
		printk(BIOS_DEBUG, "TPM2 log found at %p\n", lasa);
		return lasa;
	}
	lasa = cbmem_add(CBMEM_ID_TPM2_TCG_LOG, tpm2_default_log_len);
	if (!lasa) {
		printk(BIOS_ERR, "TPM2 log creation failed\n");
		return NULL;
	}

	printk(BIOS_DEBUG, "TPM2 log created at %p\n", lasa);
	memset(lasa, 0, tpm2_default_log_len);

	*size = tpm2_default_log_len;
	return lasa;
}

static void acpi_create_tpm2(acpi_header_t *header, void *unused)
{
	if (tlcl_get_family() != TPM_2)
		return;

	if (CONFIG(CRB_TPM) && !(CONFIG(SPI_TPM) || CONFIG(I2C_TPM) || CONFIG(MEMORY_MAPPED_TPM)))
		if (!crb_tpm_is_active())
			return;

	acpi_tpm2_t *tpm2 = (acpi_tpm2_t *)header;
	u32 tpm2_log_len;
	void *lasa;

	/*
	 * Some payloads like SeaBIOS depend on log area to use TPM2.
	 * Get the memory size and address of TPM2 log area or initialize it.
	 */
	lasa = get_tpm2_log(&tpm2_log_len);
	if (!lasa)
		tpm2_log_len = 0;

	if (acpi_fill_header(header, "TPM2", TPM2, sizeof(acpi_tpm2_t)) != CB_SUCCESS)
		return;

	/* Hard to detect for coreboot. Just set it to 0 */
	tpm2->platform_class = 0;
	if (CONFIG(CRB_TPM)) {
		/* Must be set to 7 for CRB Support */
		tpm2->control_area = crb_tpm_base_address() + 0x40;
		tpm2->start_method = 7;
	} else {
		/* Must be set to 0 for FIFO interface support */
		tpm2->control_area = 0;
		tpm2->start_method = 6;
	}
	memset(tpm2->msp, 0, sizeof(tpm2->msp));

	/* Fill the log area size and start address fields. */
	tpm2->laml = tpm2_log_len;
	tpm2->lasa = (uintptr_t)lasa;
}

static void acpi_ssdt_write_cbtable(void)
{
	const struct cbmem_entry *cbtable;
	uintptr_t base;
	uint32_t size;

	cbtable = cbmem_entry_find(CBMEM_ID_CBTABLE);
	if (!cbtable)
		return;
	base = (uintptr_t)cbmem_entry_start(cbtable);
	size = cbmem_entry_size(cbtable);

	acpigen_write_device("CTBL");
	acpigen_write_coreboot_hid(COREBOOT_ACPI_ID_CBTABLE);
	acpigen_write_name_integer("_UID", 0);
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		acpigen_write_STA(ACPI_STATUS_DEVICE_ALL_ON);
	else
		acpigen_write_STA(ACPI_STATUS_DEVICE_HIDDEN_ON);
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpigen_resource_consumer_mmio(base, base + size - 1,
				       MEM_RSRC_FLAG_MEM_READ_ONLY
				       | MEM_RSRC_FLAG_MEM_ATTR_CACHE);
	acpigen_write_resourcetemplate_footer();
	acpigen_pop_len();
}

static void acpi_create_ssdt_generator(acpi_header_t *ssdt, void *unused)
{
	unsigned long current = (unsigned long)ssdt + sizeof(acpi_header_t);

	if (acpi_fill_header(ssdt, "SSDT", SSDT, sizeof(acpi_header_t)) != CB_SUCCESS)
		return;

	acpigen_set_current((char *)current);

	/* Write object to declare coreboot tables */
	acpi_ssdt_write_cbtable();

	{
		struct device *dev;
		for (dev = all_devices; dev; dev = dev->next)
			if (dev->enabled && dev->ops && dev->ops->acpi_fill_ssdt)
				dev->ops->acpi_fill_ssdt(dev);
		current = (unsigned long)acpigen_get_current();
	}

	/* (Re)calculate length and checksum. */
	ssdt->length = current - (unsigned long)ssdt;
}

int acpi_create_srat_mem(acpi_srat_mem_t *mem, u8 node, u32 basek, u32 sizek,
				u32 flags)
{
	mem->type = 1; /* Memory affinity structure */
	mem->length = sizeof(acpi_srat_mem_t);
	mem->base_address_low = (basek << 10);
	mem->base_address_high = (basek >> (32 - 10));
	mem->length_low = (sizek << 10);
	mem->length_high = (sizek >> (32 - 10));
	mem->proximity_domain = node;
	mem->flags = flags;

	return mem->length;
}

int acpi_create_srat_gia_pci(acpi_srat_gia_t *gia, u32 proximity_domain,
			     struct device *dev, u32 flags)
{
	/* Only handle PCI devices. */
	if (dev->path.type != DEVICE_PATH_PCI)
		return 0;

	gia->type = ACPI_SRAT_STRUCTURE_GIA;
	gia->length = sizeof(acpi_srat_gia_t);
	gia->proximity_domain = proximity_domain;
	gia->dev_handle_type = ACPI_SRAT_GIA_DEV_HANDLE_PCI;
	/* First two bytes has segment number */
	gia->dev_handle[0] = dev->upstream->segment_group;
	gia->dev_handle[1] = 0;
	gia->dev_handle[2] = dev->upstream->secondary; /* Byte 2 has bus number */
	/* Byte 3 has bits 7:3 for dev, bits 2:0 for func */
	gia->dev_handle[3] = dev->path.pci.devfn;
	gia->flags = flags;

	return gia->length;
}

/* http://www.microsoft.com/whdc/system/sysinternals/sratdwn.mspx */
void acpi_create_srat(acpi_srat_t *srat,
		      unsigned long (*acpi_fill_srat_func)(unsigned long current))
{
	acpi_header_t *header = &(srat->header);
	unsigned long current = (unsigned long)srat + sizeof(acpi_srat_t);

	memset((void *)srat, 0, sizeof(acpi_srat_t));

	if (acpi_fill_header(header, "SRAT", SRAT, sizeof(acpi_srat_t)) != CB_SUCCESS)
		return;

	srat->resv = 1; /* Spec: Reserved to 1 for backwards compatibility. */

	current = acpi_fill_srat_func(current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)srat;
	header->checksum = acpi_checksum((void *)srat, header->length);
}

int acpi_create_cedt_chbs(acpi_cedt_chbs_t *chbs, u32 uid, u32 cxl_ver, u64 base)
{
	memset((void *)chbs, 0, sizeof(acpi_cedt_chbs_t));

	chbs->type = ACPI_CEDT_STRUCTURE_CHBS;
	chbs->length = sizeof(acpi_cedt_chbs_t);
	chbs->uid = uid;
	chbs->cxl_ver = cxl_ver;
	chbs->base = base;

	/*
	 * CXL spec 2.0 section 9.14.1.2 "CXL CHBS"
	 * CXL 1.1 spec compliant host bridge: 8KB
	 * CXL 2.0 spec compliant host bridge: 64KB
	 */
	if (cxl_ver == ACPI_CEDT_CHBS_CXL_VER_1_1)
		chbs->len = 8 * KiB;
	else if (cxl_ver == ACPI_CEDT_CHBS_CXL_VER_2_0)
		chbs->len = 64 * KiB;
	else
		printk(BIOS_ERR, "ACPI(%s:%s): Incorrect CXL version:%d\n", __FILE__, __func__,
		       cxl_ver);

	return chbs->length;
}

int acpi_create_cedt_cfmws(acpi_cedt_cfmws_t *cfmws, u64 base_hpa, u64 window_size, u8 eniw,
			   u32 hbig, u16 restriction, u16 qtg_id, const u32 *interleave_target)
{
	memset((void *)cfmws, 0, sizeof(acpi_cedt_cfmws_t));

	cfmws->type = ACPI_CEDT_STRUCTURE_CFMWS;

	u8 niw = 0;
	if (eniw >= 8)
		printk(BIOS_ERR, "ACPI(%s:%s): Incorrect eniw::%d\n", __FILE__, __func__, eniw);
	else
		/* NIW = 2 ** ENIW */
		niw = 0x1 << eniw;
	/* 36 + 4 * NIW */
	cfmws->length = sizeof(acpi_cedt_cfmws_t) + 4 * niw;

	cfmws->base_hpa = base_hpa;
	cfmws->window_size = window_size;
	cfmws->eniw = eniw;

	// 0: Standard Modulo Arithmetic. Other values reserved.
	cfmws->interleave_arithmetic = 0;

	cfmws->hbig = hbig;
	cfmws->restriction = restriction;
	cfmws->qtg_id = qtg_id;
	memcpy(&cfmws->interleave_target, interleave_target, 4 * niw);

	return cfmws->length;
}

void acpi_create_cedt(acpi_cedt_t *cedt, unsigned long (*acpi_fill_cedt_func)(unsigned long current))
{
	acpi_header_t *header = &(cedt->header);
	unsigned long current = (unsigned long)cedt + sizeof(acpi_cedt_t);

	memset((void *)cedt, 0, sizeof(acpi_cedt_t));

	if (acpi_fill_header(header, "CEDT", CEDT, sizeof(acpi_cedt_t)) != CB_SUCCESS)
		return;

	current = acpi_fill_cedt_func(current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)cedt;
	header->checksum = acpi_checksum((void *)cedt, header->length);
}

int acpi_create_hmat_mpda(acpi_hmat_mpda_t *mpda, u32 initiator, u32 memory)
{
	memset((void *)mpda, 0, sizeof(acpi_hmat_mpda_t));

	mpda->type = 0; /* Memory Proximity Domain Attributes structure */
	mpda->length = sizeof(acpi_hmat_mpda_t);
	/*
	 * Proximity Domain for Attached Initiator field is valid.
	 * Bit 1 and bit 2 are reserved since HMAT revision 2.
	 */
	mpda->flags = (1 << 0);
	mpda->proximity_domain_initiator = initiator;
	mpda->proximity_domain_memory = memory;

	return mpda->length;
}

void acpi_create_hmat(acpi_hmat_t *hmat,
		 unsigned long (*acpi_fill_hmat_func)(unsigned long current))
{
	acpi_header_t *header = &(hmat->header);
	unsigned long current = (unsigned long)hmat + sizeof(acpi_hmat_t);

	memset((void *)hmat, 0, sizeof(acpi_hmat_t));

	if (acpi_fill_header(header, "HMAT", HMAT, sizeof(acpi_hmat_t)) != CB_SUCCESS)
		return;

	current = acpi_fill_hmat_func(current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)hmat;
	header->checksum = acpi_checksum((void *)hmat, header->length);
}

/* http://h21007.www2.hp.com/portal/download/files/unprot/Itanium/slit.pdf */
void acpi_create_slit(acpi_slit_t *slit,
		      unsigned long (*acpi_fill_slit_func)(unsigned long current))
{
	acpi_header_t *header = &(slit->header);
	unsigned long current = (unsigned long)slit + sizeof(acpi_slit_t);

	memset((void *)slit, 0, sizeof(acpi_slit_t));

	if (acpi_fill_header(header, "SLIT", SLIT, sizeof(acpi_slit_t)) != CB_SUCCESS)
		return;

	current = acpi_fill_slit_func(current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)slit;
	header->checksum = acpi_checksum((void *)slit, header->length);
}

/*
 * This method adds the ACPI error injection capability. It fills the default information.
 * HW dependent code (caller) can modify the defaults upon return. If no changes are necessary
 * and the defaults are acceptable then caller can simply add the table (acpi_add_table).
 * INPUTS:
 * einj - ptr to the starting location of EINJ table
 * actions - number of actions to trigger an error (HW dependent)
 * addr - address of trigger action table. This should be ACPI reserved memory and it will be
 *        shared between OS and FW.
 */
void acpi_create_einj(acpi_einj_t *einj, uintptr_t addr, u8 actions)
{
	int i;
	acpi_header_t *header = &(einj->header);
	acpi_injection_header_t *inj_header = &(einj->inj_header);
	acpi_einj_smi_t *einj_smi = (acpi_einj_smi_t *)addr;
	acpi_einj_trigger_table_t *tat;
	if (!header)
		return;

	printk(BIOS_DEBUG, "%s einj_smi = %p\n", __func__, einj_smi);
	memset(einj_smi, 0, sizeof(acpi_einj_smi_t));
	tat = (acpi_einj_trigger_table_t *)((uint8_t *)einj_smi + sizeof(acpi_einj_smi_t));
	tat->header_size =  16;
	tat->revision =  0;
	tat->table_size =  sizeof(acpi_einj_trigger_table_t) +
		sizeof(acpi_einj_action_table_t) * actions - 1;
	tat->entry_count = actions;
	printk(BIOS_DEBUG, "%s trigger_action_table = %p\n", __func__, tat);

	for (i = 0; i < actions; i++) {
		tat->trigger_action[i].action = TRIGGER_ERROR;
		tat->trigger_action[i].instruction = NO_OP;
		tat->trigger_action[i].flags = FLAG_IGNORE;
		tat->trigger_action[i].reg.space_id = ACPI_ADDRESS_SPACE_MEMORY;
		tat->trigger_action[i].reg.bit_width = 64;
		tat->trigger_action[i].reg.bit_offset = 0;
		tat->trigger_action[i].reg.access_size = ACPI_ACCESS_SIZE_QWORD_ACCESS;
		tat->trigger_action[i].reg.addr = 0;
		tat->trigger_action[i].value = 0;
		tat->trigger_action[i].mask = 0xFFFFFFFF;
	}

	acpi_einj_action_table_t default_actions[ACTION_COUNT] = {
		[0] = {
			.action = BEGIN_INJECT_OP,
			.instruction = WRITE_REGISTER_VALUE,
			.flags = FLAG_PRESERVE,
			.reg = EINJ_REG_MEMORY((u64)(uintptr_t)&einj_smi->op_state),
			.value = 0,
			.mask = 0xFFFFFFFF
		},
		[1] = {
			.action = GET_TRIGGER_ACTION_TABLE,
			.instruction = READ_REGISTER,
			.flags = FLAG_IGNORE,
			.reg = EINJ_REG_MEMORY((u64)(uintptr_t)&einj_smi->trigger_action_table),
			.value = 0,
			.mask = 0xFFFFFFFFFFFFFFFF
		},
		[2] = {
			.action = SET_ERROR_TYPE,
			.instruction = WRITE_REGISTER,
			.flags = FLAG_PRESERVE,
			.reg = EINJ_REG_MEMORY((u64)(uintptr_t)&einj_smi->err_inject[0]),
			.value = 0,
			.mask = 0xFFFFFFFF
		},
		[3] = {
			.action = GET_ERROR_TYPE,
			.instruction = READ_REGISTER,
			.flags = FLAG_IGNORE,
			.reg = EINJ_REG_MEMORY((u64)(uintptr_t)&einj_smi->err_inj_cap),
			.value = 0,
			.mask = 0xFFFFFFFF
		},
		[4] = {
			.action = END_INJECT_OP,
			.instruction = WRITE_REGISTER_VALUE,
			.flags = FLAG_PRESERVE,
			.reg = EINJ_REG_MEMORY((u64)(uintptr_t)&einj_smi->op_state),
			.value = 0,
			.mask = 0xFFFFFFFF

		},
		[5] = {
			.action = EXECUTE_INJECT_OP,
			.instruction = WRITE_REGISTER_VALUE,
			.flags = FLAG_PRESERVE,
			.reg = EINJ_REG_IO(),
			.value = 0x9a,
			.mask = 0xFFFF,
		},
		[6] = {
			.action = CHECK_BUSY_STATUS,
			.instruction = READ_REGISTER_VALUE,
			.flags = FLAG_IGNORE,
			.reg = EINJ_REG_MEMORY((u64)(uintptr_t)&einj_smi->op_status),
			.value = 1,
			.mask = 1,
		},
		[7] = {
			.action = GET_CMD_STATUS,
			.instruction = READ_REGISTER,
			.flags = FLAG_PRESERVE,
			.reg = EINJ_REG_MEMORY((u64)(uintptr_t)&einj_smi->cmd_sts),
			.value = 0,
			.mask = 0x1fe,
		},
		[8] = {
			.action = SET_ERROR_TYPE_WITH_ADDRESS,
			.instruction = WRITE_REGISTER,
			.flags = FLAG_PRESERVE,
			.reg = EINJ_REG_MEMORY((u64)(uintptr_t)&einj_smi->setaddrtable),
			.value = 1,
			.mask = 0xffffffff
		}
	};

	einj_smi->err_inj_cap = ACPI_EINJ_DEFAULT_CAP;
	einj_smi->trigger_action_table = (u64)(uintptr_t)tat;

	for (i = 0; i < ACTION_COUNT; i++)
		printk(BIOS_DEBUG, "default_actions[%d].reg.addr is %llx\n", i,
			default_actions[i].reg.addr);

	memset((void *)einj, 0, sizeof(*einj));

	if (acpi_fill_header(header, "EINJ", EINJ, sizeof(acpi_einj_t)) != CB_SUCCESS)
		return;

	inj_header->einj_header_size = sizeof(acpi_injection_header_t);
	inj_header->entry_count = ACTION_COUNT;

	printk(BIOS_DEBUG, "%s einj->action_table = %p\n",
		 __func__, einj->action_table);
	memcpy((void *)einj->action_table, (void *)default_actions, sizeof(einj->action_table));
	header->checksum = acpi_checksum((void *)einj, sizeof(*einj));
}

void acpi_create_vfct(const struct device *device,
		      acpi_vfct_t *vfct,
		      unsigned long (*acpi_fill_vfct_func)(const struct device *device,
		      acpi_vfct_t *vfct_struct, unsigned long current))
{
	acpi_header_t *header = &(vfct->header);
	unsigned long current = (unsigned long)vfct + sizeof(acpi_vfct_t);

	memset((void *)vfct, 0, sizeof(acpi_vfct_t));

	if (acpi_fill_header(header, "VFCT", VFCT, sizeof(acpi_vfct_t)) != CB_SUCCESS)
		return;

	current = acpi_fill_vfct_func(device, vfct, current);

	/* If no BIOS image, return with header->length == 0. */
	if (!vfct->VBIOSImageOffset)
		return;

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)vfct;
	header->checksum = acpi_checksum((void *)vfct, header->length);
}

void acpi_create_ipmi(const struct device *device,
		      struct acpi_spmi *spmi,
		      const u16 ipmi_revision,
		      const acpi_addr_t *addr,
		      const enum acpi_ipmi_interface_type type,
		      const s8 gpe_interrupt,
		      const u32 apic_interrupt,
		      const u32 uid)
{
	acpi_header_t *header = &(spmi->header);
	memset((void *)spmi, 0, sizeof(struct acpi_spmi));

	if (acpi_fill_header(header, "SPMI", SPMI, sizeof(struct acpi_spmi)) != CB_SUCCESS)
		return;

	spmi->reserved = 1;

	if (device->path.type == DEVICE_PATH_PCI) {
		spmi->pci_device_flag = ACPI_IPMI_PCI_DEVICE_FLAG;
		spmi->pci_segment_group = device->upstream->segment_group;
		spmi->pci_bus = device->upstream->secondary;
		spmi->pci_device = device->path.pci.devfn >> 3;
		spmi->pci_function = device->path.pci.devfn & 0x7;
	} else if (type != IPMI_INTERFACE_SSIF) {
		memcpy(spmi->uid, &uid, sizeof(spmi->uid));
	}

	spmi->base_address = *addr;
	spmi->specification_revision = ipmi_revision;

	spmi->interface_type = type;

	if (gpe_interrupt >= 0 && gpe_interrupt < 32) {
		spmi->gpe = gpe_interrupt;
		spmi->interrupt_type |= ACPI_IPMI_INT_TYPE_SCI;
	}
	if (apic_interrupt > 0) {
		spmi->global_system_interrupt = apic_interrupt;
		spmi->interrupt_type |= ACPI_IPMI_INT_TYPE_APIC;
	}

	/* Calculate checksum. */
	header->checksum = acpi_checksum((void *)spmi, header->length);
}

void acpi_create_ivrs(acpi_ivrs_t *ivrs,
		      unsigned long (*acpi_fill_ivrs_func)(acpi_ivrs_t *ivrs_struct,
		      unsigned long current))
{
	acpi_header_t *header = &(ivrs->header);
	unsigned long current = (unsigned long)ivrs + sizeof(acpi_ivrs_t);

	memset((void *)ivrs, 0, sizeof(acpi_ivrs_t));

	if (acpi_fill_header(header, "IVRS", IVRS, sizeof(acpi_ivrs_t)) != CB_SUCCESS)
		return;

	current = acpi_fill_ivrs_func(ivrs, current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)ivrs;
	header->checksum = acpi_checksum((void *)ivrs, header->length);
}

void acpi_create_crat(struct acpi_crat_header *crat,
		      unsigned long (*acpi_fill_crat_func)(struct acpi_crat_header *crat_struct,
		      unsigned long current))
{
	acpi_header_t *header = &(crat->header);
	unsigned long current = (unsigned long)crat + sizeof(struct acpi_crat_header);

	memset((void *)crat, 0, sizeof(struct acpi_crat_header));

	if (acpi_fill_header(header, "CRAT", CRAT, sizeof(struct acpi_crat_header)) != CB_SUCCESS)
		return;

	current = acpi_fill_crat_func(crat, current);

	/* (Re)calculate length and checksum. */
	header->length = current - (unsigned long)crat;
	header->checksum = acpi_checksum((void *)crat, header->length);
}

static void acpi_create_dbg2(acpi_dbg2_header_t *dbg2,
		      int port_type, int port_subtype,
		      acpi_addr_t *address, uint32_t address_size,
		      const char *device_path)
{
	uintptr_t current;
	acpi_dbg2_device_t *device;
	uint32_t *dbg2_addr_size;
	acpi_header_t *header;
	size_t path_len;
	const char *path;
	char *namespace;

	/* Fill out header fields. */
	current = (uintptr_t)dbg2;
	memset(dbg2, 0, sizeof(acpi_dbg2_header_t));
	header = &(dbg2->header);

	if (acpi_fill_header(header, "DBG2", DBG2, sizeof(acpi_dbg2_header_t)) != CB_SUCCESS)
		return;

	/* One debug device defined */
	dbg2->devices_offset = sizeof(acpi_dbg2_header_t);
	dbg2->devices_count = 1;
	current += sizeof(acpi_dbg2_header_t);

	/* Device comes after the header */
	device = (acpi_dbg2_device_t *)current;
	memset(device, 0, sizeof(acpi_dbg2_device_t));
	current += sizeof(acpi_dbg2_device_t);

	device->revision = 0;
	device->address_count = 1;
	device->port_type = port_type;
	device->port_subtype = port_subtype;

	/* Base Address comes after device structure */
	memcpy((void *)current, address, sizeof(acpi_addr_t));
	device->base_address_offset = current - (uintptr_t)device;
	current += sizeof(acpi_addr_t);

	/* Address Size comes after address structure */
	dbg2_addr_size = (uint32_t *)current;
	device->address_size_offset = current - (uintptr_t)device;
	*dbg2_addr_size = address_size;
	current += sizeof(uint32_t);

	/* Namespace string comes last, use '.' if not provided */
	path = device_path ? : ".";
	/* Namespace string length includes NULL terminator */
	path_len = strlen(path) + 1;
	namespace = (char *)current;
	device->namespace_string_length = path_len;
	device->namespace_string_offset = current - (uintptr_t)device;
	strncpy(namespace, path, path_len);
	current += path_len;

	/* Update structure lengths and checksum */
	device->length = current - (uintptr_t)device;
	header->length = current - (uintptr_t)dbg2;
	header->checksum = acpi_checksum((uint8_t *)dbg2, header->length);
}

static unsigned long acpi_write_dbg2_uart(acpi_rsdp_t *rsdp, unsigned long current,
					  int space_id, uint64_t base, uint32_t size,
					  int access_size, const char *name)
{
	acpi_dbg2_header_t *dbg2 = (acpi_dbg2_header_t *)current;
	acpi_addr_t address;

	memset(&address, 0, sizeof(address));

	address.space_id = space_id;
	address.addrl = (uint32_t)base;
	address.addrh = (uint32_t)((base >> 32) & 0xffffffff);
	address.access_size = access_size;

	int subtype;
	/* 16550-compatible with parameters defined in Generic Address Structure */
	if (CONFIG(DRIVERS_UART_8250IO) || CONFIG(DRIVERS_UART_8250MEM))
		subtype = ACPI_DBG2_PORT_SERIAL_16550;
	else if (CONFIG(DRIVERS_UART_PL011))
		subtype = ACPI_DBG2_PORT_SERIAL_ARM_PL011;
	else
		return current;

	acpi_create_dbg2(dbg2,
			 ACPI_DBG2_PORT_SERIAL,
			 subtype,
			 &address, size,
			 name);

	if (dbg2->header.length) {
		current += dbg2->header.length;
		current = acpi_align_current(current);
		acpi_add_table(rsdp, dbg2);
	}

	return current;
}

unsigned long acpi_write_dbg2_pci_uart(acpi_rsdp_t *rsdp, unsigned long current,
				       const struct device *dev, uint8_t access_size)
{
	struct resource *res;

	if (!dev) {
		printk(BIOS_DEBUG, "%s: Device not found\n", __func__);
		return current;
	}
	if (!dev->enabled) {
		printk(BIOS_INFO, "%s: Device not enabled\n", __func__);
		return current;
	}
	res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res) {
		printk(BIOS_ERR, "%s: Unable to find resource for %s\n",
		       __func__, dev_path(dev));
		return current;
	}

	int space_id;
	if (res->flags & IORESOURCE_IO)
		space_id = ACPI_ADDRESS_SPACE_IO;
	else if (res->flags & IORESOURCE_MEM)
		space_id = ACPI_ADDRESS_SPACE_MEMORY;
	else {
		printk(BIOS_ERR, "%s: Unknown address space type\n", __func__);
		return current;
	}

	return acpi_write_dbg2_uart(rsdp, current, space_id, res->base, res->size, access_size, acpi_device_path(dev));
}

unsigned long acpi_pl011_write_dbg2_uart(acpi_rsdp_t *rsdp, unsigned long current,
					 uint64_t base, const char *name)
{
	return acpi_write_dbg2_uart(rsdp, current, ACPI_ADDRESS_SPACE_MEMORY, base,
				    sizeof(struct pl011_uart), ACPI_ACCESS_SIZE_DWORD_ACCESS,
				    name);
}

unsigned long acpi_16550_mmio32_write_dbg2_uart(acpi_rsdp_t *rsdp, unsigned long current,
					 uint64_t base, const char *name)
{
	return acpi_write_dbg2_uart(rsdp, current, ACPI_ADDRESS_SPACE_MEMORY, base,
				    0x100, ACPI_ACCESS_SIZE_DWORD_ACCESS,
				    name);
}

static void acpi_create_facs(void *header)
{
	acpi_facs_t *facs = header;

	memcpy(facs->signature, "FACS", 4);
	facs->length = sizeof(acpi_facs_t);
	facs->hardware_signature = 0;
	facs->firmware_waking_vector = 0;
	facs->global_lock = 0;
	facs->flags = 0;
	facs->x_firmware_waking_vector_l = 0;
	facs->x_firmware_waking_vector_h = 0;
	facs->version = get_acpi_table_revision(FACS);
}

static void acpi_write_rsdt(acpi_rsdt_t *rsdt, char *oem_id, char *oem_table_id)
{
	acpi_header_t *header = &(rsdt->header);

	if (acpi_fill_header(header, "RSDT", RSDT, sizeof(acpi_rsdt_t)) != CB_SUCCESS)
		return;

	/* Entries are filled in later, we come with an empty set. */

	/* Fix checksum. */
	header->checksum = acpi_checksum((void *)rsdt, sizeof(acpi_rsdt_t));
}

static void acpi_write_xsdt(acpi_xsdt_t *xsdt, char *oem_id, char *oem_table_id)
{
	acpi_header_t *header = &(xsdt->header);

	if (acpi_fill_header(header, "XSDT", XSDT, sizeof(acpi_xsdt_t)) != CB_SUCCESS)
		return;

	/* Entries are filled in later, we come with an empty set. */

	/* Fix checksum. */
	header->checksum = acpi_checksum((void *)xsdt, sizeof(acpi_xsdt_t));
}

static void acpi_write_rsdp(acpi_rsdp_t *rsdp, acpi_rsdt_t *rsdt,
			    acpi_xsdt_t *xsdt, char *oem_id)
{
	memset(rsdp, 0, sizeof(acpi_rsdp_t));

	memcpy(rsdp->signature, RSDP_SIG, 8);
	memcpy(rsdp->oem_id, oem_id, 6);

	rsdp->length = sizeof(acpi_rsdp_t);
	rsdp->rsdt_address = (uintptr_t)rsdt;

	/*
	 * Revision: ACPI 1.0: 0, ACPI 2.0/3.0/4.0: 2.
	 *
	 * Some OSes expect an XSDT to be present for RSD PTR revisions >= 2.
	 * If we don't have an ACPI XSDT, force ACPI 1.0 (and thus RSD PTR
	 * revision 0).
	 */
	if (xsdt == NULL) {
		rsdp->revision = 0;
	} else {
		rsdp->xsdt_address = (u64)(uintptr_t)xsdt;
		rsdp->revision = get_acpi_table_revision(RSDP);
	}

	/* Calculate checksums. */
	rsdp->checksum = acpi_checksum((void *)rsdp, 20);
	rsdp->ext_checksum = acpi_checksum((void *)rsdp, sizeof(acpi_rsdp_t));
}

unsigned long acpi_create_hest_error_source(acpi_hest_t *hest,
	acpi_hest_esd_t *esd, u16 type, void *data, u16 data_len)
{
	acpi_header_t *header = &(hest->header);
	acpi_hest_hen_t *hen;
	void *pos;
	u16 len;

	pos = esd;
	memset(pos, 0, sizeof(acpi_hest_esd_t));
	len = 0;
	esd->type = type;		/* MCE */
	esd->source_id = hest->error_source_count;
	esd->flags = 0;		/* FIRMWARE_FIRST */
	esd->enabled = 1;
	esd->prealloc_erecords = 1;
	esd->max_section_per_record = 0x1;

	len += sizeof(acpi_hest_esd_t);
	pos = esd + 1;

	switch (type) {
	case 0:			/* MCE */
		break;
	case 1:			/* CMC */
		hen = (acpi_hest_hen_t *)(pos);
		memset(pos, 0, sizeof(acpi_hest_hen_t));
		hen->type = 3;		/* SCI? */
		hen->length = sizeof(acpi_hest_hen_t);
		hen->conf_we = 0;	/* Configuration Write Enable. */
		hen->poll_interval = 0;
		hen->vector = 0;
		hen->sw2poll_threshold_val = 0;
		hen->sw2poll_threshold_win = 0;
		hen->error_threshold_val = 0;
		hen->error_threshold_win = 0;
		len += sizeof(acpi_hest_hen_t);
		pos = hen + 1;
		break;
	case 2:			/* NMI */
	case 6:			/* AER Root Port */
	case 7:			/* AER Endpoint */
	case 8:			/* AER Bridge */
	case 9:			/* Generic Hardware Error Source. */
		/* TODO: */
		break;
	default:
		printk(BIOS_DEBUG, "Invalid type of Error Source.");
		break;
	}
	hest->error_source_count++;

	memcpy(pos, data, data_len);
	len += data_len;
	if (header)
		header->length += len;

	return len;
}

/* ACPI 4.0 */
static void acpi_create_hest(acpi_header_t *header, void *unused)
{
	if (!CONFIG(ACPI_HEST))
		return;

	/* Reserve memory for Enhanced error logging */
	void *log_mem = cbmem_add(CBMEM_ID_ACPI_HEST, CONFIG_ACPI_HEST_ERROR_LOG_BUFFER_SIZE);
	if (!log_mem) {
		printk(BIOS_ERR, "Unable to allocate HEST memory\n");
		return;
	}
	printk(BIOS_DEBUG, "HEST elog_addr: %p, size:%d\n", log_mem,
		CONFIG_ACPI_HEST_ERROR_LOG_BUFFER_SIZE);

	acpi_hest_t *hest = (acpi_hest_t *)header;
	uintptr_t current = (uintptr_t)(hest + 1);

	memset(hest, 0, sizeof(acpi_hest_t));

	if (acpi_fill_header(header, "HEST", HEST, sizeof(acpi_hest_t)) != CB_SUCCESS)
		return;

	current = acpi_soc_fill_hest(hest, current, log_mem);

	/* (Re)calculate length. */
	header->length = current - (uintptr_t)hest;
}

/* ACPI 3.0b */
static void acpi_create_bert(acpi_header_t *header, void *unused)
{
	if (!CONFIG(ACPI_BERT))
		return;

	acpi_bert_t *bert = (acpi_bert_t *)header;

	void *region;
	size_t size;
	if (acpi_soc_get_bert_region(&region, &size) != CB_SUCCESS)
		return;

	if (acpi_fill_header(header, "BERT", BERT, sizeof(acpi_bert_t)) != CB_SUCCESS)
		return;

	bert->error_region = (uintptr_t)region;
	bert->region_length = (size_t)size;
}

__weak void arch_fill_fadt(acpi_fadt_t *fadt) { }
__weak void soc_fill_fadt(acpi_fadt_t *fadt) { }
__weak void mainboard_fill_fadt(acpi_fadt_t *fadt) { }

static acpi_header_t *dsdt;
static void acpi_create_fadt(acpi_header_t *header, void *arg1)
{
	acpi_fadt_t *fadt = (acpi_fadt_t *)header;
	acpi_facs_t *facs = (acpi_facs_t *)(*(acpi_facs_t **)arg1);

	if (acpi_fill_header(header, "FACP", FADT, sizeof(acpi_fadt_t)) != CB_SUCCESS)
		return;

	fadt->FADT_MinorVersion = get_acpi_fadt_minor_version();
	if ((uintptr_t)facs <= UINT32_MAX)
		fadt->firmware_ctrl = (uintptr_t)facs;
	else
		fadt->x_firmware_ctl_h = (uint32_t)((uint64_t)(uintptr_t)facs >> 32);
	fadt->x_firmware_ctl_l = (uint32_t)(uintptr_t)facs;

	if ((uintptr_t)dsdt <= UINT32_MAX)
		fadt->dsdt = (uintptr_t)dsdt;
	else
		fadt->x_dsdt_h = (uint32_t)((uint64_t)(uintptr_t)dsdt >> 32);
	fadt->x_dsdt_l = (uint32_t)(uintptr_t)dsdt;

	/* should be 0 ACPI 3.0 */
	fadt->reserved = 0;

	/* P_LVLx latencies are not used as CPU _CST will override them. */
	fadt->p_lvl2_lat = ACPI_FADT_C2_NOT_SUPPORTED;
	fadt->p_lvl3_lat = ACPI_FADT_C3_NOT_SUPPORTED;

	/* Use CPU _PTC instead to provide P_CNT details. */
	fadt->duty_offset = 0;
	fadt->duty_width = 0;

	fadt->preferred_pm_profile = acpi_get_preferred_pm_profile();

	arch_fill_fadt(fadt);

	acpi_fill_fadt(fadt);

	soc_fill_fadt(fadt);
	mainboard_fill_fadt(fadt);
}

static void acpi_create_lpit(acpi_header_t *header, void *unused)
{
	if (!CONFIG(ACPI_LPIT))
		return;

	acpi_lpit_t *lpit = (acpi_lpit_t *)header;
	unsigned long current = (unsigned long)lpit + sizeof(acpi_lpit_t);

	if (acpi_fill_header(header, "LPIT", LPIT, sizeof(acpi_lpit_t)) != CB_SUCCESS)
		return;

	current = acpi_fill_lpit(current);

	/* (Re)calculate length. */
	header->length = current - (unsigned long)lpit;
}

static void acpi_create_gtdt(acpi_header_t *header, void *unused)
{
	if (!CONFIG(ACPI_GTDT))
		return;

	acpi_gtdt_t *gtdt = (acpi_gtdt_t *)header;
	unsigned long current = (unsigned long)gtdt + sizeof(acpi_gtdt_t);

	if (acpi_fill_header(header, "GTDT", GTDT, sizeof(acpi_gtdt_t)) != CB_SUCCESS)
		return;

	/* Fill out header fields. */
	gtdt->platform_timer_offset = sizeof(acpi_gtdt_t);

	acpi_soc_fill_gtdt(gtdt);
	current = acpi_soc_gtdt_add_timers(&gtdt->platform_timer_count, current);

	/* (Re)calculate length. */
	header->length = current - (unsigned long)gtdt;
}

unsigned long acpi_gtdt_add_timer_block(unsigned long current, const uint64_t address,
					   struct acpi_gtdt_timer_entry *timers, size_t number)
{
	struct acpi_gtdt_timer_block *block = (struct acpi_gtdt_timer_block *)current;
	memset(block, 0, sizeof(struct acpi_gtdt_timer_block));

	assert(number < 8 && number != 0);
	const size_t entries_size = number * sizeof(struct acpi_gtdt_timer_entry);

	block->header.type = ACPI_GTDT_TYPE_TIMER_BLOCK;
	block->header.length = sizeof(struct acpi_gtdt_timer_block)
		+ entries_size;
	block->block_address = address;
	block->timer_count = number;
	block->timer_offset = sizeof(struct acpi_gtdt_timer_block);
	current += sizeof(struct acpi_gtdt_timer_block);
	memcpy((void *)current, timers, entries_size);
	current += entries_size;
	return current;
}

unsigned long acpi_gtdt_add_watchdog(unsigned long current, uint64_t refresh_frame,
				     uint64_t control_frame, uint32_t gsiv, uint32_t flags)
{
	struct acpi_gtdt_watchdog *wd = (struct acpi_gtdt_watchdog *)current;
	memset(wd, 0, sizeof(struct acpi_gtdt_watchdog));

	wd->header.type = ACPI_GTDT_TYPE_WATCHDOG;
	wd->header.length = sizeof(struct acpi_gtdt_watchdog);
	wd->refresh_frame_address = refresh_frame;
	wd->control_frame_address = control_frame;
	wd->timer_interrupt = gsiv;
	wd->timer_flags = flags;

	return current + sizeof(struct acpi_gtdt_watchdog);
}

static void acpi_create_iort(acpi_header_t *header, void *unused)
{
	if (!CONFIG(ACPI_IORT))
		return;

	acpi_iort_t *iort = (acpi_iort_t *)header;
	unsigned long current = (unsigned long)iort + sizeof(acpi_iort_t);

	if (acpi_fill_header(header, "IORT", IORT, sizeof(acpi_iort_t)) != CB_SUCCESS)
		return;

	iort->node_count = 0;
	iort->node_offset = current - (unsigned long)iort;

	current = acpi_soc_fill_iort(iort, current);

	/* (Re)calculate length */
	header->length = current - (unsigned long)iort;
}

static void acpi_create_wdat(acpi_header_t *header, void *unused)
{
	if (!CONFIG(ACPI_WDAT_WDT))
		return;

	acpi_wdat_t *wdat = (acpi_wdat_t *)header;
	unsigned long current = (unsigned long)wdat + sizeof(acpi_wdat_t);

	memset((void *)wdat, 0, sizeof(acpi_wdat_t));

	if (acpi_fill_header(header, "WDAT", WDAT, sizeof(acpi_wdat_t)) != CB_SUCCESS)
		return;

	current = acpi_soc_fill_wdat(wdat, current);

	/* (Re)calculate length. */
	header->length = current - (unsigned long)wdat;
}

unsigned long acpi_create_lpi_desc_ncst(acpi_lpi_desc_ncst_t *lpi_desc, uint16_t uid)
{
	memset(lpi_desc, 0, sizeof(acpi_lpi_desc_ncst_t));
	lpi_desc->header.length = sizeof(acpi_lpi_desc_ncst_t);
	lpi_desc->header.type = ACPI_LPI_DESC_TYPE_NATIVE_CSTATE;
	lpi_desc->header.uid = uid;

	return lpi_desc->header.length;
}

static void acpi_create_pptt(acpi_header_t *header, void *unused)
{
	if (!CONFIG(ACPI_PPTT))
		return;

	if (acpi_fill_header(header, "PPTT", PPTT, sizeof(acpi_pptt_t)) != CB_SUCCESS)
		return;

	acpi_pptt_t *pptt = (acpi_pptt_t *)header;
	acpi_create_pptt_body(pptt);
}

static uint8_t acpi_spcr_type(void)
{
	/* 16550-compatible with parameters defined in Generic Address Structure */
	if (CONFIG(DRIVERS_UART_8250IO) || CONFIG(DRIVERS_UART_8250MEM))
		return 0x12;
	if (CONFIG(DRIVERS_UART_PL011))
		return 0x3;

	printk(BIOS_ERR, "%s: unknown serial type\n", __func__);
	return 0xff;
}

static void acpi_create_spcr(acpi_header_t *header, void *unused)
{
	acpi_spcr_t *spcr = (acpi_spcr_t *)header;
	struct lb_serial serial;

	if (!CONFIG(CONSOLE_SERIAL))
		return;

	if (fill_lb_serial(&serial) != CB_SUCCESS)
		return;

	if (acpi_fill_header(header, "SPCR", SPCR, sizeof(acpi_spcr_t)) != CB_SUCCESS)
		return;

	spcr->interface_type = acpi_spcr_type();
	assert(serial.type == LB_SERIAL_TYPE_IO_MAPPED
	       || serial.type == LB_SERIAL_TYPE_MEMORY_MAPPED);
	spcr->base_address.space_id = serial.type == LB_SERIAL_TYPE_IO_MAPPED ?
		ACPI_ADDRESS_SPACE_IO : ACPI_ADDRESS_SPACE_MEMORY;
	spcr->base_address.bit_width = serial.regwidth * 8;
	spcr->base_address.bit_offset = 0;
	switch (serial.regwidth) {
	case 1:
		spcr->base_address.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
		break;
	case 2:
		spcr->base_address.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
		break;
	case 4:
		spcr->base_address.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
		break;
	default:
		printk(BIOS_ERR, "%s, Invalid serial regwidth\n", __func__);
	}

	spcr->base_address.addrl = serial.baseaddr;
	spcr->base_address.addrh = 0;
	spcr->interrupt_type = 0;
	spcr->irq = 0;
	spcr->configured_baudrate = 0; /* Have the OS use whatever is currently set */
	spcr->parity = 0;
	spcr->stop_bits = 1;
	spcr->flow_control = 0;
	spcr->terminal_type = 2; /* 2 = VT-UTF8 */
	spcr->language = 0;
	spcr->pci_did = 0xffff;
	spcr->pci_vid = 0xffff;

	header->checksum = acpi_checksum((void *)spcr, header->length);
}

unsigned long __weak fw_cfg_acpi_tables(unsigned long start)
{
	return 0;
}

static void acpi_create_dsdt(acpi_header_t *header, void *dsdt_file_arg)
{
	dsdt = header;
	acpi_header_t *dsdt_file = *(acpi_header_t **)dsdt_file_arg;
	unsigned long current = (unsigned long)header;

	dsdt = (acpi_header_t *)current;
	memcpy(dsdt, dsdt_file, sizeof(acpi_header_t));
	if (dsdt->length >= sizeof(acpi_header_t)) {
		current += sizeof(acpi_header_t);

		acpigen_set_current((char *)current);

		if (CONFIG(ACPI_SOC_NVS))
			acpi_fill_gnvs();
		if (CONFIG(CHROMEOS_NVS))
			acpi_fill_cnvs();

		current = (unsigned long)acpigen_get_current();
		memcpy((char *)current,
		       (char *)dsdt_file + sizeof(acpi_header_t),
		       dsdt->length - sizeof(acpi_header_t));
		current += dsdt->length - sizeof(acpi_header_t);

		/* (Re)calculate length. */
		dsdt->length = current - (unsigned long)dsdt;
	}
}

static void acpi_create_slic(acpi_header_t *header, void *slic_file_arg)
{
	acpi_header_t *slic_file = *(acpi_header_t **)slic_file_arg;
	acpi_header_t *slic = header;
	if (slic_file)
		memcpy(slic, slic_file, slic_file->length);
}

static uintptr_t coreboot_rsdp;

uintptr_t get_coreboot_rsdp(void)
{
	return coreboot_rsdp;
}

static void acpixtract_compatible_hexdump(const void *memory, size_t length)
{
	size_t i, j;
	uint8_t *line;
	size_t num_bytes;

	for (i = 0; i < length; i += 16) {
		num_bytes = MIN(length - i, 16);
		line = ((uint8_t *)memory) + i;

		printk(BIOS_SPEW, "    %04zX:", i);
		for (j = 0; j < num_bytes; j++)
			printk(BIOS_SPEW, " %02x", line[j]);
		for (; j < 16; j++)
			printk(BIOS_SPEW, "   ");
		printk(BIOS_SPEW, "  ");
		for (j = 0; j < num_bytes; j++)
			printk(BIOS_SPEW, "%c",
			       isprint(line[j]) ? line[j] : '.');
		printk(BIOS_SPEW, "\n");
	}
}

static void acpidump_print(void *table_ptr)
{
	if (table_ptr == NULL)
		return;
	const acpi_header_t *header = (acpi_header_t *)table_ptr;
	const size_t table_size = header->length;
	printk(BIOS_SPEW, "%.4s @ 0x0000000000000000\n", header->signature);
	acpixtract_compatible_hexdump(table_ptr, table_size);
	printk(BIOS_SPEW, "\n");
}

unsigned long write_acpi_tables(const unsigned long start)
{
	unsigned long current;
	acpi_rsdp_t *rsdp;
	acpi_rsdt_t *rsdt = NULL;
	acpi_xsdt_t *xsdt = NULL;
	acpi_facs_t *facs = NULL;
	acpi_header_t *slic_file;
	acpi_header_t *ssdt = NULL;
	acpi_header_t *dsdt_file;
	struct device *dev;
	unsigned long fw;
	size_t slic_size, dsdt_size;
	char oem_id[6], oem_table_id[8];

	const struct acpi_table_generator {
		void (*create_table)(acpi_header_t *table, void *arg);
		void *args;
		size_t min_size;
	} tables[] = {
		{ acpi_create_dsdt, &dsdt_file, sizeof(acpi_header_t) },
		{ acpi_create_fadt, &facs, sizeof(acpi_fadt_t) },
		{ acpi_create_slic, &slic_file, sizeof(acpi_header_t) },
		{ acpi_create_ssdt_generator, NULL, sizeof(acpi_header_t) },
		{ acpi_create_mcfg, NULL, sizeof(acpi_mcfg_t) },
		{ acpi_create_tcpa, NULL, sizeof(acpi_tcpa_t) },
		{ acpi_create_tpm2, NULL, sizeof(acpi_tpm2_t) },
		{ acpi_create_lpit, NULL, sizeof(acpi_lpit_t) },
		{ acpi_create_madt, NULL, sizeof(acpi_header_t) },
		{ acpi_create_hest, NULL, sizeof(acpi_hest_t) },
		{ acpi_create_bert, NULL, sizeof(acpi_bert_t) },
		{ acpi_create_spcr, NULL, sizeof(acpi_spcr_t) },
		{ acpi_create_gtdt, NULL, sizeof(acpi_gtdt_t) },
		{ acpi_create_pptt, NULL, sizeof(acpi_pptt_t) },
		{ acpi_create_iort, NULL, sizeof(acpi_iort_t) },
		{ acpi_create_wdat, NULL, sizeof(acpi_wdat_t) },
	};

	current = start;

	/* Align ACPI tables to 16byte */
	current = acpi_align_current(current);

	/* Special case for qemu */
	fw = fw_cfg_acpi_tables(current);
	if (fw) {
		rsdp = NULL;
		/* Find RSDP. */
		for (void *p = (void *)current; p < (void *)fw; p += 16) {
			if (valid_rsdp((acpi_rsdp_t *)p)) {
				rsdp = p;
				coreboot_rsdp = (uintptr_t)rsdp;
				break;
			}
		}
		if (!rsdp)
			return fw;

		current = fw;
		current = acpi_align_current(current);
		if (rsdp->xsdt_address == 0) {
			acpi_rsdt_t *existing_rsdt = (acpi_rsdt_t *)(uintptr_t)rsdp->rsdt_address;

			/*
			 * Qemu only provides a smaller ACPI 1.0 RSDP, thus
			 * allocate a bigger ACPI 2.0 RSDP structure.
			 */
			rsdp = (acpi_rsdp_t *)current;
			current += sizeof(acpi_rsdp_t);
			coreboot_rsdp = (uintptr_t)rsdp;

			xsdt = (acpi_xsdt_t *)current;
			current += sizeof(acpi_xsdt_t);
			current = acpi_align_current(current);

			/*
			 * Qemu only creates an RSDT.
			 * Add an XSDT based on the existing RSDT entries.
			 */
			acpi_write_rsdp(rsdp, existing_rsdt, xsdt, oem_id);
			acpi_write_xsdt(xsdt, oem_id, oem_table_id);
			/*
			 * Copy existing entries to the new XSDT. This will override existing
			 * RSDT entries with the same value.
			 */
			for (int i = 0; existing_rsdt->entry[i]; i++)
				acpi_add_table(rsdp, (void *)(uintptr_t)existing_rsdt->entry[i]);
		}

		/* Add BOOT0000 for Linux google firmware driver */
		printk(BIOS_DEBUG, "ACPI:     * SSDT\n");
		ssdt = (acpi_header_t *)current;
		current += sizeof(acpi_header_t);

		memset((void *)ssdt, 0, sizeof(acpi_header_t));

		memcpy(&ssdt->signature, "SSDT", 4);
		ssdt->revision = get_acpi_table_revision(SSDT);
		memcpy(&ssdt->oem_id, OEM_ID, 6);
		memcpy(&ssdt->oem_table_id, oem_table_id, 8);
		ssdt->oem_revision = 42;
		memcpy(&ssdt->asl_compiler_id, ASLC, 4);
		ssdt->asl_compiler_revision = asl_revision;
		ssdt->length = sizeof(acpi_header_t);

		acpigen_set_current((char *)current);

		/* Write object to declare coreboot tables */
		acpi_ssdt_write_cbtable();

		/* (Re)calculate length and checksum. */
		ssdt->length = current - (unsigned long)ssdt;
		ssdt->checksum = acpi_checksum((void *)ssdt, ssdt->length);

		acpi_create_ssdt_generator(ssdt, NULL);

		acpi_add_table(rsdp, ssdt);

		return current;
	}

	dsdt_file = cbfs_map(CONFIG_CBFS_PREFIX "/dsdt.aml", &dsdt_size);
	if (!dsdt_file) {
		printk(BIOS_ERR, "No DSDT file, skipping ACPI tables\n");
		return start;
	}

	if (dsdt_file->length > dsdt_size
	    || dsdt_file->length < sizeof(acpi_header_t)
	    || memcmp(dsdt_file->signature, "DSDT", 4) != 0) {
		printk(BIOS_ERR, "Invalid DSDT file, skipping ACPI tables\n");
		cbfs_unmap(dsdt_file);
		return start;
	}

	slic_file = cbfs_map(CONFIG_CBFS_PREFIX "/slic", &slic_size);
	if (slic_file
	    && (slic_file->length > slic_size
		|| slic_file->length < sizeof(acpi_header_t)
		|| (memcmp(slic_file->signature, "SLIC", 4) != 0
		    && memcmp(slic_file->signature, "MSDM", 4) != 0))) {
		cbfs_unmap(slic_file);
		slic_file = 0;
	}

	if (slic_file) {
		memcpy(oem_id, slic_file->oem_id, 6);
		memcpy(oem_table_id, slic_file->oem_table_id, 8);
	} else {
		memcpy(oem_id, OEM_ID, 6);
		memcpy(oem_table_id, ACPI_TABLE_CREATOR, 8);
	}

	printk(BIOS_INFO, "ACPI: Writing ACPI tables at %lx.\n", start);

	/* We need at least an RSDP, RSDT for ACPI 1.0 compat, otherwise XSDT */
	rsdp = (acpi_rsdp_t *)current;
	coreboot_rsdp = (uintptr_t)rsdp;
	current += sizeof(acpi_rsdp_t);
	current = acpi_align_current(current);
	if (current + sizeof(acpi_rsdt_t) - 1 <= UINT32_MAX) {
		rsdt = (acpi_rsdt_t *)current;
		current += sizeof(acpi_rsdt_t);
		current = acpi_align_current(current);
	} else {
		printk(BIOS_INFO, "Not adding RSDT because tables reside above 4G.");
	}

	xsdt = (acpi_xsdt_t *)current;
	current += sizeof(acpi_xsdt_t);
	current = acpi_align_current(current);

	/* clear all table memory */
	memset((void *)start, 0, current - start);

	acpi_write_rsdp(rsdp, rsdt, xsdt, oem_id);
	acpi_write_rsdt(rsdt, oem_id, oem_table_id);
	acpi_write_xsdt(xsdt, oem_id, oem_table_id);

	if (ENV_X86) {
		printk(BIOS_DEBUG, "ACPI:    * FACS\n");
		current = ALIGN_UP(current, 64);
		facs = (acpi_facs_t *)current;
		current += sizeof(acpi_facs_t);
		current = acpi_align_current(current);
		acpi_create_facs(facs);
	}

	for (size_t i = 0; i < ARRAY_SIZE(tables); i++) {
		acpi_header_t *header = (acpi_header_t *)current;
		memset(header, 0, tables[i].min_size);
		tables[i].create_table(header, tables[i].args);
		if (header->length < tables[i].min_size)
			continue;
		header->checksum = 0;
		header->checksum = acpi_checksum((void *)header, header->length);
		current += header->length;
		current = acpi_align_current(current);

		if (tables[i].create_table == acpi_create_dsdt)
			continue;

		printk(BIOS_DEBUG, "ACPI:    * %.4s\n", header->signature);
		acpi_add_table(rsdp, header);
	}

	/*
	 * cbfs_unmap() uses mem_pool_free() which works correctly only
	 * if freeing is done in reverse order than memory allocation.
	 * This is why unmapping of dsdt_file must be done after
	 * unmapping slic file.
	 */
	cbfs_unmap(slic_file);
	cbfs_unmap(dsdt_file);

	printk(BIOS_DEBUG, "current = %lx\n", current);

	for (dev = all_devices; dev; dev = dev->next) {
		if (dev->ops && dev->ops->write_acpi_tables) {
			current = dev->ops->write_acpi_tables(dev, current,
				rsdp);
			current = acpi_align_current(current);
		}
	}

	printk(BIOS_INFO, "ACPI: done.\n");

	if (CONFIG(DEBUG_ACPICA_COMPATIBLE)) {
		printk(BIOS_DEBUG, "Printing ACPI tables in ACPICA compatible format\n");
		if (facs)
			acpidump_print(facs);
		acpidump_print(dsdt);
		for (size_t i = 0; xsdt->entry[i] != 0; i++) {
			acpidump_print((void *)(uintptr_t)xsdt->entry[i]);
		}
		printk(BIOS_DEBUG, "Done printing ACPI tables in ACPICA compatible format\n");
	}

	return current;
}

static acpi_rsdp_t *valid_rsdp(acpi_rsdp_t *rsdp)
{
	if (strncmp((char *)rsdp, RSDP_SIG, sizeof(RSDP_SIG) - 1) != 0)
		return NULL;

	printk(BIOS_DEBUG, "Looking on %p for valid checksum\n", rsdp);

	if (acpi_checksum((void *)rsdp, 20) != 0)
		return NULL;
	printk(BIOS_DEBUG, "Checksum 1 passed\n");

	if ((rsdp->revision > 1) && (acpi_checksum((void *)rsdp,
						rsdp->length) != 0))
		return NULL;
	printk(BIOS_DEBUG, "Checksum 2 passed all OK\n");

	return rsdp;
}

void *acpi_find_wakeup_vector(void)
{
	char *p, *end;
	acpi_xsdt_t *xsdt;
	acpi_facs_t *facs;
	acpi_fadt_t *fadt = NULL;
	acpi_rsdp_t *rsdp = NULL;
	void *wake_vec;
	int i;

	if (!acpi_is_wakeup_s3())
		return NULL;

	printk(BIOS_DEBUG, "Trying to find the wakeup vector...\n");

	/* Find RSDP. */
	for (p = (char *)0xe0000; p < (char *)0xfffff; p += 16) {
		rsdp = valid_rsdp((acpi_rsdp_t *)p);
		if (rsdp)
			break;
	}

	if (rsdp == NULL) {
		printk(BIOS_ALERT,
		       "No RSDP found, wake up from S3 not possible.\n");
		return NULL;
	}

	printk(BIOS_DEBUG, "RSDP found at %p\n", rsdp);
	xsdt = (acpi_xsdt_t *)(uintptr_t)rsdp->xsdt_address;

	end = (char *)xsdt + xsdt->header.length;
	printk(BIOS_DEBUG, "XSDT found at %p ends at %p\n", xsdt, end);

	for (i = 0; ((char *)&xsdt->entry[i]) < end; i++) {
		fadt = (acpi_fadt_t *)(uintptr_t)xsdt->entry[i];
		if (strncmp((char *)fadt, "FACP", 4) == 0)
			break;
		fadt = NULL;
	}

	if (fadt == NULL) {
		printk(BIOS_ALERT,
		       "No FADT found, wake up from S3 not possible.\n");
		return NULL;
	}

	printk(BIOS_DEBUG, "FADT found at %p\n", fadt);
	facs = (acpi_facs_t *)(uintptr_t)((uint64_t)fadt->x_firmware_ctl_l
			       | (uint64_t)fadt->x_firmware_ctl_h << 32);

	if (facs == NULL) {
		printk(BIOS_ALERT,
		       "No FACS found, wake up from S3 not possible.\n");
		return NULL;
	}

	printk(BIOS_DEBUG, "FACS found at %p\n", facs);
	wake_vec = (void *)(uintptr_t)facs->firmware_waking_vector;
	printk(BIOS_DEBUG, "OS waking vector is %p\n", wake_vec);

	return wake_vec;
}

__weak int acpi_get_gpe(int gpe)
{
	return -1; /* implemented by SOC */
}

u8 get_acpi_fadt_minor_version(void)
{
	return ACPI_FADT_MINOR_VERSION_0;
}

int get_acpi_table_revision(enum acpi_tables table)
{
	switch (table) {
	case FADT:
		return ACPI_FADT_REV_ACPI_6;
	case MADT: /* ACPI 3.0: 2, ACPI 4.0/5.0: 3, ACPI 6.2b/6.3: 5 */
		return 3;
	case MCFG:
		return 1;
	case TCPA:
		return 2;
	case TPM2:
		return 4;
	case SSDT: /* ACPI 3.0 up to 6.3: 2 */
		return 2;
	case SRAT: /* ACPI 2.0: 1, ACPI 3.0: 2, ACPI 4.0 up to 6.4: 3 */
		return 3;
	case HMAT: /* ACPI 6.4: 2 */
		return 2;
	case DMAR:
		return 1;
	case SLIT: /* ACPI 2.0 up to 6.3: 1 */
		return 1;
	case SPMI: /* IMPI 2.0 */
		return 5;
	case HPET: /* Currently 1. Table added in ACPI 2.0. */
		return 1;
	case VFCT: /* ACPI 2.0/3.0/4.0: 1 */
		return 1;
	case IVRS:
		return IVRS_FORMAT_MIXED;
	case DBG2:
		return 0;
	case FACS: /* ACPI 2.0/3.0: 1, ACPI 4.0 up to 6.3: 2 */
		return 1;
	case RSDT: /* ACPI 1.0 up to 6.3: 1 */
		return 1;
	case XSDT: /* ACPI 2.0 up to 6.3: 1 */
		return 1;
	case RSDP: /* ACPI 2.0 up to 6.3: 2 */
		return 2;
	case EINJ:
		return 1;
	case HEST:
		return 1;
	case NHLT:
		return 5;
	case BERT:
		return 1;
	case CEDT: /* CXL 3.0 section 9.17.1 */
		return 1;
	case CRAT:
		return 1;
	case LPIT: /* ACPI 5.1 up to 6.3: 0 */
		return 0;
	case SPCR:
		return 4;
	case GTDT:
		return 3;
	case PPTT: /* ACPI 6.4 */
		return 3;
	case IORT: /* IO Remapping Table E.e */
		return 6;
	case WDAT:
		return 1;
	default:
		return -1;
	}
	return -1;
}
