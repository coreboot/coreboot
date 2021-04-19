/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <assert.h>
#include <soc/pch.h>
#include <types.h>
#include <version.h>

static void acpi_write_serialio_psx_methods(const char *const name, const uint32_t bar1)
{
	const char *const spcs = "SPCS";
	const unsigned int spcs_bits = 32;
	const unsigned long offset = bar1 + 0x84;
	const uint8_t flags = FIELD_DWORDACC | FIELD_NOLOCK | FIELD_PRESERVE;
	const struct opregion op_reg = OPREGION("SPRT", SYSTEMMEMORY, offset, spcs_bits / 8);
	const struct fieldlist field = FIELDLIST_NAMESTR(spcs, spcs_bits);

	acpigen_write_scope(name);
	{
		acpigen_write_opregion(&op_reg);
		acpigen_write_field(op_reg.name, &field, 1, flags);

		acpigen_write_method_serialized("_PS0", 0);
		{
			/* SPCS &= 0xfffffffc */
			acpigen_emit_byte(AND_OP);
			acpigen_emit_namestring(spcs);
			acpigen_write_dword(0xfffffffc);
			acpigen_emit_namestring(spcs);

			/* Do a posting read after writing */
			acpigen_write_store();
			acpigen_emit_namestring(spcs);
			acpigen_emit_byte(LOCAL0_OP);
		}
		acpigen_pop_len();

		acpigen_write_method_serialized("_PS3", 0);
		{
			/* SPCS |= 3 */
			acpigen_emit_byte(OR_OP);
			acpigen_emit_namestring(spcs);
			acpigen_write_byte(3);
			acpigen_emit_namestring(spcs);

			/* Do a posting read after writing */
			acpigen_write_store();
			acpigen_emit_namestring(spcs);
			acpigen_emit_byte(LOCAL0_OP);
		}
		acpigen_pop_len();
	}
	acpigen_pop_len();
}

static struct pch_acpi_device_state device_state[NUM_PCH_ACPI_DEVICES] = { 0 };

struct pch_acpi_device_state *get_acpi_device_state(enum pch_acpi_device dev_index)
{
	assert(dev_index < ARRAY_SIZE(device_state));
	return &device_state[dev_index];
}

static void acpi_create_serialio_ssdt_entry(enum pch_acpi_device dev_index)
{
	const struct pch_acpi_device_state *state = get_acpi_device_state(dev_index);

	const char idx = '0' + dev_index;
	const char sxen[5] = { 'S', idx, 'E', 'N', '\0' };
	acpigen_write_name_byte(sxen, state->enable);

	const char sxb0[5] = { 'S', idx, 'B', '0', '\0' };
	acpigen_write_name_dword(sxb0, state->bar0);

	const char sxb1[5] = { 'S', idx, 'B', '1', '\0' };
	acpigen_write_name_dword(sxb1, state->bar1);
}

void acpi_create_serialio_ssdt(acpi_header_t *ssdt)
{
	unsigned long current = (unsigned long)ssdt + sizeof(acpi_header_t);

	/* Fill the SSDT header */
	memset((void *)ssdt, 0, sizeof(acpi_header_t));
	memcpy(&ssdt->signature, "SSDT", 4);
	ssdt->revision = get_acpi_table_revision(SSDT);
	memcpy(&ssdt->oem_id, OEM_ID, 6);
	memcpy(&ssdt->oem_table_id, "SERIALIO", 8);
	ssdt->oem_revision = 43;
	memcpy(&ssdt->asl_compiler_id, ASLC, 4);
	ssdt->asl_compiler_revision = asl_revision;
	ssdt->length = sizeof(acpi_header_t);
	acpigen_set_current((char *)current);

	/* Fill the SSDT with an entry for each SerialIO device */
	for (enum pch_acpi_device dev_index = 0; dev_index < NUM_PCH_ACPI_DEVICES; dev_index++)
		acpi_create_serialio_ssdt_entry(dev_index);

	acpigen_write_scope("\\_SB.PCI0");
	{
		acpi_write_serialio_psx_methods("I2C0", device_state[PCH_ACPI_I2C0].bar1);
		acpi_write_serialio_psx_methods("I2C1", device_state[PCH_ACPI_I2C1].bar1);
		acpi_write_serialio_psx_methods("SPI0", device_state[PCH_ACPI_GSPI0].bar1);
		acpi_write_serialio_psx_methods("SPI1", device_state[PCH_ACPI_GSPI1].bar1);
		acpi_write_serialio_psx_methods("UAR0", device_state[PCH_ACPI_UART0].bar1);
		acpi_write_serialio_psx_methods("UAR1", device_state[PCH_ACPI_UART1].bar1);
	}
	acpigen_pop_len();

	/* (Re)calculate length and checksum. */
	current = (unsigned long)acpigen_get_current();
	ssdt->length = current - (unsigned long)ssdt;
	ssdt->checksum = acpi_checksum((void *)ssdt, ssdt->length);
}
